#ifndef PAGING_H
#define PAGING_H

#include "type.h"
#include "isr.h"
#include "monitor.h"
#include "common.h"
#include "fault.h"

#define VMM_PAGE_SIZE 0x1000
#define VMM_PAGE_FROM_VADDR(v) (v / VMM_PAGE_SIZE)
#define VMM_TABLE_FROM_PAGE(p) (p / 1024)

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

void vmm_switch_page_directory(page_directory_t * new);

page_t * get_page(uint32 address, int make, page_directory_t * dir);

void page_fault(registers_t * regs);

void vmm_switch_dir(page_directory_t* dir);

void vmm_map_page (page_directory_t *dir, uint32 vaddr, int kern, int wr);

page_directory_t *kernel_directory;
page_directory_t *current_kernel_directory;

#endif
