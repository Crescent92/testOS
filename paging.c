
#include "paging.h"
#include "irq.h"
#include "kheap.h"

#define VMM_PAGE_SIZE 0x1000
extern void enable_paging();

extern heap_t *kheap;
extern uint32 end;
uint32 placement_address = (uint32)&end;

void page_fault(registers_t * regs) {
	//stored in cr2 register
	uint32 faulting_address;

	asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

	//Error code tells why pgae_fault was raised
	int present = !(regs->err_code & 0x1);	//page not present
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
}

void vmm_initialize() {
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
		alloc_frame (get_page(i, 1, kernel_directory), 0, 0);
		i += 0x1000;
	}


	for (uint32 addr = KHEAP_START; addr < KHEAP_START + KHEAP_INITIAL_SIZE; addr += 0x1000) {
		alloc_frame(get_page(addr, 1, kernel_directory), 0, 0);
	}

	irq_install_handler (14, page_fault);

	//enable_paging();

	vmm_switch_dir(kernel_directory);
	enable_paging();


	kheap = heap_create (KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, 0xCFFFF000, 0, 0);

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

void vmm_switch_dir(page_directory_t* dir) {
	asm volatile("mov %0, %%cr3":: "r"(dir->physicalAddr));
}


