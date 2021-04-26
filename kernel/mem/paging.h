#ifndef PAGING_H
#define PAGING_H

#include <type.h>
#include <cpu/isr.h>
#include <devices/monitor.h>
#include <common.h>
#include <mem/fault.h>
#include <multiboot.h>

#define VMM_PAGE_SIZE 0x1000

#define VMM_IS_PAGE_BOUNDRY(x) (x % VMM_PAGE_SIZE == 0)
#define VMM_PAGE_FROM_VADDR(v) (v / VMM_PAGE_SIZE)
#define VMM_TABLE_FROM_PAGE(p) (p / 1024)

#define VMM_PD_INDX(x)	(((x) >> 22) & 0x3ff)
#define VMM_PTE_INDX(x)	(((x) >> 12) &0x3ff)
#define PAGE_ALIGN_ADDR(x)	(x & ~0xfff)


#define PAGE_BUFFER_LOCATION 0xF0000000

#define USER_STACK_LOCATION 0xE0000000
#define USER_STACK_LOCATION2 0xD0000000
#define USER_HEAP_LOCATION	0xB0000000

typedef struct page {
	uint32 present	: 1;
	uint32 rw	: 1;
	uint32 user	: 1;
	uint32 accessed : 1;
	uint32 dirty	: 1;
	uint32 unused	: 7;
	uint32 frame	: 20;
} page_t;

typedef struct page_table {
	page_t pages[1024];
} page_table_t;

typedef struct page_directory {
	page_table_t *tables[1024];

	uint32 tablesPhysical[1024];

	uint32 physicalAddr;
} page_directory_t;

void vmm_initialize();

void vmm_init2(phy_mem_layout_t _phy_mem_l);

void vmm_switch_page_directory(page_directory_t * new);

page_t * get_page(uint32 address, int make, page_directory_t * dir);

void page_fault(registers_t * regs);

void copy_page (uint32 addr, page_directory_t *newdir);

void vmm_switch_dir(page_directory_t* dir);

void vmm_map_page (page_directory_t *dir, uint32 vaddr, int kern, int wr);

page_directory_t *vmm_clone_dir5(int user);

uint32 vmm_get_phy_addr(page_directory_t *dir, uint32 vaddr);

page_t *vmm_get_page(page_directory_t *dir, uint32 vaddr);

page_directory_t *kernel_directory;
page_directory_t *current_kernel_directory;

#endif
