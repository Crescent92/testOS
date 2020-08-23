

#include "mem.h"
#include "kheap.h"

extern heap_t *kheap;

uint32 kmalloc_int (uint32 size, int align, uint32 * phy) {
 
	if (kheap != 0) {
		void *addr = heap_alloc (size, (uint8)align, kheap);
		if (phy != 0) {
		}
		return (uint32)addr;
	} else {
		//If address is not page aligned, align it
		if ((align == 1) && (placement_address & 0x00000FFF)) {
			placement_address &= 0xfffff000;
			placement_address += 0x1000;
		}

		if (phy) {
			*phy = placement_address;
		}

		uint32 tmp = placement_address;
		placement_address += size;

		return tmp;
	}
}

void kfree (void *p) {
	if ((uint32)p <= placement_address) {
		return;
	}
	heap_free (p, kheap);
}

uint32 kmalloc_a (uint32 size) {
	return kmalloc_int (size, 1, 0);
}

uint32 kmalloc_p (uint32 size, uint32 * phy) {
	return kmalloc_int (size, 0, phy);
}

uint32 kmalloc_ap (uint32 size, uint32 * phy) {
	return kmalloc_int (size, 1, phy);
}

uint32 kmalloc (uint32 size) {
	return kmalloc_int (size, 0, 0);
}

void set_frame(uint32 frame_addr) {
	uint32 frame = (frame_addr/0x1000);
	uint32 index  = INDEX_FROM_BIT(frame);
	uint32 offset = OFFSET_FROM_BIT(frame);
	frames[index] |= (0x1 << offset);
}

void clear_frame(uint32 frame_addr) {
	uint32 frame = (frame_addr/0x1000);
	uint32 index = INDEX_FROM_BIT(frame);
	uint32 offset = OFFSET_FROM_BIT(frame);
	frames[index] &= ~(0x1 << offset);
}

uint32 test_frame(uint32 frame_addr) {
	uint32 frame = (frame_addr/0x1000);
	uint32 index = INDEX_FROM_BIT(frame);
	uint32 offset = OFFSET_FROM_BIT(frame);
	return (frames[index] & (0x1 << offset));
}

uint32 first_frame() {
	uint32 i, j;
	for (i=0; i < INDEX_FROM_BIT(nframes); i++) {
		if (frames[i] != 0xffffffff) {
			for (j=0; j < 32; j++) {
				if (!(frames[i] & (0x1 << j))) {
					return (i*4*8 + j);
				}
			}
		}
	}
	return -1;
}

void alloc_frame(page_t *page, int is_kernel, int is_writeable) {
	if (page->frame != 0) {
		return;
	} else {
		uint32 idx = first_frame();
		if (idx == (uint32)-1) {
			KPANIC("No free frames!");
		}
		set_frame(idx * 0x1000);
		page->present = 1;
		// page->rw = (is_writeable) ? 1 : 0;
		page->rw = 1;
		// page->user = (is_kernel) ? 0 : 1;
		page->user = 1;
		page->frame = idx;
	}
}

void free_frame(page_t *page) {
	uint32 frame;

	if (!(frame = page->frame)) {
		return;
	} else {
		clear_frame(frame * 0x1000);
		page->frame = 0x0;
		page->present = 0;
	}
}
