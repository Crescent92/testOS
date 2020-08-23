
#ifndef _MEM_H
#define _MEM_H

#include "type.h"
#include "paging.h"

uint32 kmalloc_int (uint32 size, int align, uint32 * phy);

//page aligned
uint32 kmalloc_a (uint32 size);

//return physical addr
uint32 kmalloc_p (uint32 size, uint32 * phy);

//page aligned and return phy addr
uint32 kmalloc_ap (uint32 size, uint32 * phy);

//vanilla(unaligned)
uint32 kmalloc (uint32 size);


// A bitset of frames - used or free.
uint32 *frames;
uint32 nframes;

extern uint32 placement_address;

// Macros used in the bitset algorithms.
#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

// Static function to set a bit in the frames bitset
void set_frame(uint32 frame_addr);

// Static function to clear a bit in the frames bitset
void clear_frame(uint32 frame_addr);

// Static function to test if a bit is set.
uint32 test_frame(uint32 frame_addr);

// Static function to find the first free frame.
uint32 first_frame();

// Function to allocate a frame.
void alloc_frame(page_t *page, int is_kernel, int is_writeable);

// Function to deallocate a frame.
void free_frame(page_t *page);

#endif
