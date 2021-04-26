
#include "paging.h"
#include <cpu/irq.h>
#include "kheap.h"


extern void enable_paging();

extern heap_t *kheap;
extern uint32 end;
uint32 placement_address = (uint32)&end;

extern void copy_page_physical(uint32, uint32);
extern void arch_flush_tlb();

page_t *vmm_get_page(page_directory_t *dir, uint32 vaddr) {
	uint32 page = VMM_PAGE_FROM_VADDR(vaddr);
	uint32 table = VMM_TABLE_FROM_PAGE(page);

	if (dir->tables[table]) {
		return &dir->tables[table]->pages[page % 1024];
	}

	return 0;
}

uint32 vmm_get_phy_addr(page_directory_t *dir, uint32 vaddr) {
	page_t *page = vmm_get_page(dir, vaddr);
	if (page && page->present) {
		return page->frame * VMM_PAGE_SIZE + (vaddr & 0x00000FFF);
	}
	return 0;
}

extern uint32 kstart;
extern uint32 kend;
extern uint32 __end;

void vmm_clone_kern_pd(page_directory_t *new_dir) {
	page_directory_t *temp_pd = current_kernel_directory;
	uint32 kern_start_addr = &kstart;	//already page aligned to 1MB/0x100000
	uint32 kern_end_addr = &kend;
	uint32 pde_i = 0;
	//Copy over pde pointers to new directory
	for(pde_i = 0; pde_i < (VMM_PD_INDX(PAGE_ALIGN_ADDR(kern_end_addr)) + 1); pde_i++) {
		if(current_kernel_directory->tablesPhysical[pde_i]) {
			// printf("copy kernel pde idx: %d\n", pde_i);
			new_dir->tables[pde_i] = current_kernel_directory->tables[pde_i];
			new_dir->tablesPhysical[pde_i] = current_kernel_directory->tablesPhysical[pde_i];	
		}
	}

	//copy buffer page entry
	pde_i = VMM_PD_INDX(PAGE_BUFFER_LOCATION);
	new_dir->tables[pde_i] = current_kernel_directory->tables[pde_i];
	new_dir->tablesPhysical[pde_i] = current_kernel_directory->tablesPhysical[pde_i];

	//copy KHEAP

	for (pde_i = VMM_PD_INDX(KHEAP_START); pde_i < (VMM_PD_INDX(KHEAP_START + KHEAP_INITIAL_SIZE)) + 1 ; pde_i++) {
		if(current_kernel_directory->tablesPhysical[pde_i]) {
			// printf("copy kernel pde idx: %d\n", pde_i);
			new_dir->tables[pde_i] = current_kernel_directory->tables[pde_i];
			new_dir->tablesPhysical[pde_i] = current_kernel_directory->tablesPhysical[pde_i];	
		}
	}
}


void vmm_clone_user_data_pd(page_directory_t *new_dir) {
	page_directory_t *temp_pd = current_kernel_directory;
	//allocate and copy data after kernel end address.
	uint32 pde_i = 0;
	uint32 pte_i = 0;
	uint32 phy = 0;
	for (pde_i = 3; pde_i < 960; pde_i++) {
		if (current_kernel_directory->tablesPhysical[pde_i]) {
			new_dir->tables[pde_i] = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &phy);
			memset(new_dir->tables[pde_i], 0, sizeof(page_table_t));
			new_dir->tablesPhysical[pde_i] = phy | 0x7;
			// printf("copy user pd idx: %d\n", pde_i);
			for(pte_i = 0; pte_i < 1024; pte_i++) {
				if (current_kernel_directory->tables[pde_i]->pages[pte_i].present) {
					// printf("copy user pte idx: %d\n", pte_i);
					copy_page((pde_i * 1024 + pte_i) * 0x1000, new_dir);
				}
			}
		}
	}
}

page_directory_t *vmm_clone_dir5(int user) {
	uint32 phy = 0;
	page_directory_t *new_pd = (page_directory_t *)kmalloc_ap(sizeof(page_directory_t), &phy);

	memset(new_pd, 0, sizeof(page_directory_t));

	new_pd->physicalAddr = phy + ((uint32)new_pd->tablesPhysical - (uint32)new_pd);
	vmm_clone_kern_pd(new_pd);
	if (user) {
		vmm_clone_user_data_pd(new_pd);
	}
	return new_pd;
}

void page_fault(registers_t * regs) {
	//stored in cr2 register
	uint32 faulting_address;

	asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

	//Error code tells why pgae_fault was raised
	int present = (regs->err_code & 0x1);	//page not present
	int rw = regs->err_code & 0x2;		//WRite operation
	int us = regs->err_code & 0x4;		//Processor in user mode
	int reserved = regs->err_code & 0x8;	//Overwrite CPU reserved bits of page entry
	int id = regs->err_code & 0x10;		//Caused by an instruction fetch


	printf("Page fault! ( ");
	if (present) {printf("present ");}
	if (rw) {printf("read-only ");}
	if (us) {printf("user-mode ");}
	if (reserved) {printf("reserved ");}

	printf(") at %x - EIP: %x\n", faulting_address, regs->eip);

	KPANIC("Page fault!");
	for (;;);
}

void vmm_init2(phy_mem_layout_t _phy_mem_l) {
	char *mem_start = (char *)_phy_mem_l.start;
	char *mem_end = (char *)_phy_mem_l.end;
	char *mem_sz = (char *)_phy_mem_l.len;
	printf("mem_end - VMM_PAGE_SIZE: %x\n",mem_end - VMM_PAGE_SIZE);
	nframes = (((uint32)(mem_end - VMM_PAGE_SIZE)) / VMM_PAGE_SIZE); 
	printf("nframes: %x\n", nframes);

	frames = (uint32 *)kmalloc_a(INDEX_FROM_BIT(nframes));
	memset(frames, 0, INDEX_FROM_BIT(nframes));

	kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
	memset(kernel_directory, 0, sizeof(page_directory_t));
	kernel_directory->physicalAddr = kernel_directory->tablesPhysical;
	current_kernel_directory = kernel_directory;

	uint32 i = 0;
	for (i = 0; i < (_phy_mem_l.heap_start + 50*VMM_PAGE_SIZE); i += VMM_PAGE_SIZE) {
		alloc_frame (get_page(i, 1, kernel_directory), 1, 1, 0);	
	}

	alloc_frame(get_page(PAGE_BUFFER_LOCATION, 1, kernel_directory), 1, 1, 0);

	irq_install_handler (14, page_fault);
	vmm_switch_dir(kernel_directory);
	enable_paging();
}

void vmm_initialize() {

	// asm volatile("cli");
	uint32 mem_end_page = 0x1000000;

	nframes = (mem_end_page / 0x1000);

	frames = (uint32*)kmalloc_a(INDEX_FROM_BIT(nframes));
	memset(frames, 0, INDEX_FROM_BIT(nframes));

	kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
	memset(kernel_directory, 0, sizeof(page_directory_t));
	kernel_directory->physicalAddr = kernel_directory->tablesPhysical;
	current_kernel_directory = kernel_directory;


	for (uint32 addr = KHEAP_START; addr < KHEAP_START + KHEAP_INITIAL_SIZE; addr += 0x1000) {
		get_page (addr, 1, kernel_directory);
	}
	uint32 i = 0;
	while (i < placement_address + 0x3000) {
		alloc_frame (get_page(i, 1, kernel_directory), 1, 1, 0);
		i += 0x1000;
	}


	for (uint32 addr = KHEAP_START; addr < KHEAP_START + KHEAP_INITIAL_SIZE; addr += 0x1000) {
		alloc_frame(get_page(addr, 1, kernel_directory), 1, 1, 0);
	}

	alloc_frame(get_page(PAGE_BUFFER_LOCATION, 1, kernel_directory), 1, 1, 0);

	irq_install_handler (14, page_fault);
	// current_kernel_directory = vmm_clone_dir2(kernel_directory);
	vmm_switch_dir(kernel_directory);
	enable_paging();


	kheap = heap_create(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, 0xCFFFF000, 0, 0);

	// asm volatile("sti");
}

page_t *get_page (uint32 address, int make, page_directory_t *dir) {
	
	address /= 0x1000;

	uint32 table_idx = address / 1024;

	if (dir->tables[table_idx]) {
		return &dir->tables[table_idx]->pages[address % 1024];
	} else if (make) {
		uint32 tmp;
		dir->tables[table_idx] = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &tmp);
		memset(dir->tables[table_idx], 0, sizeof(page_table_t));
		dir->tablesPhysical[table_idx] = tmp | 0x7;
		return &dir->tables[table_idx]->pages[address % 1024];
	} else {
		return 0;
	}
}

void vmm_map_page (page_directory_t *dir, uint32 vaddr, int kern, int wr) {
	alloc_frame(get_page(vaddr, 1, dir), kern, wr, 0);
}

void vmm_switch_dir(page_directory_t* dir) {
	current_kernel_directory = dir;
	asm volatile("mov %0, %%cr3":: "r"(dir->physicalAddr));
}

void vmm_flush_tlb_entry2(uint32 addr)
{
	__asm__ __volatile__("invlpg (%0)" ::"r"(addr)
						 : "memory");
}

void copy_page (uint32 addr, page_directory_t *newdir) {
	page_directory_t *curdir = (page_directory_t *)current_kernel_directory;

	page_t *copy_page = get_page(addr, 1, curdir);
	page_t *buffer_page = get_page(PAGE_BUFFER_LOCATION, 1, curdir);
	page_t *new_page  = get_page(addr, 1, newdir);

	int bufframe = buffer_page->frame;

	alloc_frame(new_page, copy_page->user ? 0 : 1, copy_page->rw, 1); 

	buffer_page->frame = new_page->frame;
	memcpy((void *)PAGE_BUFFER_LOCATION, (void *)addr, 0x1000);
	buffer_page->frame = bufframe;
	vmm_flush_tlb_entry2(addr);
	arch_flush_tlb();
}


