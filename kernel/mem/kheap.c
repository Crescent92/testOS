
#include "kheap.h"

#define _SIMP_ALLOC_ 

// #define _IMPL_ALLOC_

#ifdef _IMPL_ALLOC_
static void *kmem_coalesce(void *bp);
static void *extend_heap(uint32 size);
static void *find_fit(uint32 sz);

#define	WSIZE	4
#define DSIZE	8
#define	CHUNK	(1 << 12)

#define MAX(x, y)	((x) > (y) ? (x) : (y))

#define PACK(sz, alloc)	((sz) | (alloc))

#define GET(p)	(*(unsigned int *)(p))
#define PUT(p, val)	(*(unsigned int *)(p) = (val))

#define GET_SIZE(p)	(GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

#define HDRP(bp)	((char *)(bp) - WSIZE)
#define FTRP(bp)	((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

#define NEXT_BLKP	((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))
#define PREV_BLKP	((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))


static char *bottom_kheap;
static char *brk_kheap;
static char *cur_max_heap;
static char *heap_list;


void _kmem_init() {
	bottom_kheap = (char *)KHEAP_START;
	brk_kheap = bottom_kheap;
	cur_max_heap = (char *)(KHEAP_START + KHEAP_INITIAL_SIZE);
}

int kmem_init() {
	if ((heap_list = kmem_sbrk(4 * WSIZE)) == (char *)-1) {
		return -1;
	}

	PUT(heap_list, 0);
	PUT(heap_list + (1 * WSIZE), PACK(DSIZE, 1));
	PUT(heap_list + (2 * WSIZE), PACK(DSIZE, 1));
	PUT(heap_list + (3 * WSIZE), PACK(0, 1));

	heap_list += (2 * WSIZE);

	if (extend_heap(CHUNK / WSIZE) == 0) {
		return -1;
	} 

	return 0;
}
/*
 * Increase heap by sz amount.
 */
char *kmem_sbrk(int sz) {
	char * old_brk = brk_kheap;
	if ((sz < 0) || ((brk_kheap + sz) > cur_max_heap)) {
		//map 1 page
		vmm_map_page(current_kernel_directory, cur_max_heap, 1, 1);
		cur_max_heap += VMM_PAGE_SIZE;
	}
	brk_kheap += sz;
	return old_brk;
}

void kmem_free(void *bp) {
	uint32 sz = GET_SIZE(HDRP(bp));

	PUT(HDRP(bp), PACK(sz, 0));
	PUT(FTRP(bp), PACK(sz, 0));
	kmem_coalesce(bp);
}

void kmem_alloc(uint32 sz) {
	uint32 asz;
	uint32 extend_sz;
	char *bp;

	if (sz == 0) {
		return 0;
	}

	if (sz <= DSIZE) {
		asz = 2 * DSIZE;
	} else {
		asz = DSIZE * ((sz + (DSIZE) + (DSIZE - 1)) / DSIZE)
	}

	if ((bp = find_fit(asz)) != 0) {
		place(bp, asz);
		return bp;
	}

	extend_sz = MAX(asz, CHUNK);
	if ((bp = extend_heap(extend_sz / WSIZE)) == 0) {
		return 0;
	}

	place(bp, asz);
	return bp;
}

static void *extend_heap(uint32 words) {
	char *bp;
	uint32 sz; 

	sz = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
	if (((long)(bp = kmem_sbrk(sz))) == -1) {
		return 0;
	} 

	PUT(HDRP(bp), PACK(sz, 0));
	PUT(FTRP(bp), PACK(sz, 0));
	PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));
	return kmem_coalesce(bp);
}

static void *find_fit(uint32 sz) {
	char *bp = 0;
	for (bp = heap_list; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
		if (!GET_ALLOC(HDRP(bp)) && (sz <= GET_SIZE(HDRP(bp)))) {
			return bp;
		}
	}

	return 0;
}

static void *kmem_coalesce(void *bp) {
	uint32 prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
	uint32 next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
	uint32 sz = GET_SIZE(HDRP(bp));

	if (prev_alloc && next_alloc) {
		return bp;
	}	else if (prev_alloc && !next_alloc) {
		sz += GET_SIZE(HDRP(NEXT_BLKP(bp)));
		PUT(HDRP(bp), PACK(sz, 0));
		PUT(FTRP(bp), PACK(sz, 0));
	} else if (!prev_alloc && next_alloc) {
		sz += GET_SIZE(HDRP(PREV_BLKP(bp)));
		PUT(FTRP(bp), PACK(sz, 0));
		PUT(HDRP(PREV_BLKP(bp)), PACK(sz, 0));
		bp = PREV_BLKP(bp);
	} else {
		sz += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
		PUT(HDRP(PREV_BLKP(bp)), PACK(sz, 0));
		PUT(FTRP(NEXT_BLKP(bp)), PACK(sz, 0));
		bp = PREV_BLKP(bp);
	}

	return bp;
}

static void place(void *bp, uint32 asz) {
	uint32 csz = GET_SIZE(HDRP(bp));

	if ((csz - asz) >= (2 * DSIZE)) {
		PUT(HDRP(bp), PACK(asz, 1));
		PUT(FTRP(bp), PACK(asz, 1));
		bp = NEXT_BLKP(bp);
		PUT(HDRP(bp), PACK(csz - asz), 0);
		PUT(FTRP(bp), PACK(csz - asz), 0);
	} else {
		PUT(HDRP(bp), PACK(csz, 1));
		PUT(FTRP(bp), PACK(csz, 1));
	}
}

#endif

#ifdef _SIMP_ALLOC_
heap_t *kheap=0;
static uint8 header_t_less_than (void *a, void *b) {
	return (((((header_t*)a)->size) < (((header_t*)b)->size)) ? 1 : 0);
}

static int32 find_smallest_hole (uint32 size, uint8 page_align, heap_t *heap) {
	uint32 iterator = 0;

	while (iterator < heap->index.size) {
		header_t * header = (header_t *)lookup_ordered_array(iterator, &heap->index);

		if (page_align > 0) {
			uint32 location = (uint32)header;
			int32 offset = 0;
			if ((location + sizeof(header_t) & 0xFFFFF000) != 0) {
				offset = 0x1000 - (location + sizeof(header_t)) % 0x1000;
			}

			int32 hole_size = (int32)header->size - offset;
			if (hole_size >= (int32)size) {
				break;
			}
		} else if (header->size >= size) {
			break;
		}
		iterator++;
	}

	if (iterator == heap->index.size) {
		return -1;
	} else {
		return iterator;
	}
}

heap_t *heap_create (uint32 start, uint32 end_addr, uint32 max, uint8 supervisor, uint8 readonly) {
	heap_t *heap = (heap_t*)kmalloc(sizeof(heap_t));

	ASSERT(start % 0x1000 == 0);
	ASSERT(end_addr % 0x1000 == 0);

	heap->index = place_ordered_array ((void*)start, HEAP_INDEX_SIZE, &header_t_less_than);

	start += sizeof(type_t) * HEAP_INDEX_SIZE;

	if ((start & 0xFFFFF000) != 0) {
		start &= 0xFFFFF000;
		start += 0x1000;
	}

	heap->start_address = start;
	heap->end_address = end_addr;
	heap->max_address = max;
	heap->supervisor = supervisor;
	heap->readonly = readonly;

	header_t *hole = (header_t *)start;
	hole->size = end_addr - start;
	hole->magic = HEAP_MAGIC;
	hole->is_hole = 1;
	insert_ordered_array ((void*)hole, &heap->index);

	return heap;
}

static void expand (uint32 new_size, heap_t *heap) {
	ASSERT (new_size > (heap->end_address - heap->start_address));

	if ((new_size & 0xFFFFF000) != 0) {
		new_size &= 0xFFFFF000;
		new_size += 0x1000;
	}

	ASSERT(heap->start_address + new_size <= heap->max_address);

	uint32 old_size = heap->end_address - heap->start_address;

	uint32 i = old_size;  	

	while (i < new_size) {
		alloc_frame(get_page(heap->start_address+i, 1, (current_kernel_directory == 0) ? kernel_directory : current_kernel_directory), 
				(heap->supervisor) ? 1 : 0, (heap->readonly) ? 0 : 1, 0);
		i += 0x1000;
	}
	heap->end_address = heap->start_address + new_size;
}

static uint32 contract (uint32 new_size, heap_t *heap) {
	ASSERT (new_size < heap->end_address - heap->start_address);

	if ((new_size & 0x1000) != 0) {
		new_size &= 0x1000;
		new_size += 0x1000;
	}

	if (new_size < HEAP_MIN_SIZE) {
		new_size = HEAP_MIN_SIZE;
	}

	uint32 old_size = heap->end_address - heap->start_address;
	uint32 i = old_size - 0x1000;

	while (new_size < i) {
		free_frame(get_page(heap->start_address + i, 0, (current_kernel_directory == 0) ? kernel_directory : current_kernel_directory));
		i -= 0x1000;
	}

	heap->end_address = heap->start_address + new_size;
	return new_size;
}

void * heap_alloc (uint32 size, uint8 page_align, heap_t *heap) {
	uint32 new_size = size + sizeof(header_t) + sizeof(footer_t);

	int32 iterator = find_smallest_hole (new_size, page_align, heap);

	if (iterator == -1) {
		uint32 old_length = heap->end_address - heap->start_address;
		uint32 old_end_address = heap->end_address;

		expand(old_length + new_size, heap);

		uint32 new_length = heap->end_address - heap->start_address;

		iterator = 0;

		int32 idx = -1;
		uint32 value = 0x0;

		while (iterator < heap->index.size) {
			uint32 tmp = (uint32)lookup_ordered_array (iterator, &heap->index);
			if (tmp > value) {
				value = tmp;
				idx = iterator;
			}
			iterator++;
		}

		if (idx == -1) {
			header_t *header = (header_t *)old_end_address;
			header->magic = HEAP_MAGIC;
			header->size = new_length - old_length;
			header->is_hole = 1;
			footer_t *footer = (footer_t *)(old_end_address + header->size - sizeof(footer_t));
			footer->magic = HEAP_MAGIC;
			footer->header = header;
			insert_ordered_array ((void*)header, &heap->index);
		} else {
			header_t *header = lookup_ordered_array (idx, &heap->index);
			header->size += new_length - old_length;
			footer_t *footer = (footer_t *)((uint32)header + header->size - sizeof(footer_t));
			footer->header = header;
			footer->magic = HEAP_MAGIC;
		}	
		return heap_alloc (size, page_align, heap);
	}

	header_t *orig_hole_header = (header_t*)lookup_ordered_array(iterator, &heap->index);
	uint32 orig_hole_pos = (uint32)orig_hole_header;
	uint32 orig_hole_size = orig_hole_header->size;

	if ((orig_hole_size - new_size) < (sizeof(header_t) + sizeof(footer_t))) {
		size += orig_hole_size - new_size; 
		new_size = orig_hole_size;
	}

	if (page_align && ((orig_hole_pos + (sizeof(header_t))) & 0xFFFFF000)) {
		uint32 new_location = orig_hole_pos + 0x1000 - (orig_hole_pos  & 0xFFF) - sizeof(header_t);
		header_t* hole_header = (header_t*)orig_hole_pos;
		hole_header->size = 0x1000 - (orig_hole_pos & 0xfff) - sizeof(header_t);
		hole_header->magic = HEAP_MAGIC;
		hole_header->is_hole = 1;
		footer_t *hole_footer = (footer_t*)((uint32)new_location - sizeof(footer_t));
		hole_footer->magic = HEAP_MAGIC;
		hole_footer->header = hole_header;
		orig_hole_pos = new_location;
		orig_hole_size = orig_hole_size - hole_header->size; 

	} else {
		remove_ordered_array (iterator, &heap->index);
	}

	header_t *block_header = (header_t*)orig_hole_pos;
	block_header->magic = HEAP_MAGIC;
	block_header->is_hole = 0;
	block_header->size = new_size;

	footer_t *block_footer = (footer_t*)(orig_hole_pos + sizeof(header_t) + size);
	block_footer->magic = HEAP_MAGIC;
	block_footer->header = block_header;

	if ((orig_hole_size - new_size) > 0) {
		header_t *hole_header = (header_t*)(orig_hole_pos + sizeof(header_t) + size + sizeof(footer_t));
		hole_header->magic = HEAP_MAGIC;
		hole_header->is_hole = 1;
		hole_header->size = orig_hole_size - new_size;
		footer_t *hole_footer = (footer_t*)((uint32)hole_header + orig_hole_size - new_size - sizeof(footer_t));
		if((uint32)hole_footer < heap->end_address) {
			hole_footer->magic = HEAP_MAGIC;
			hole_footer->header = hole_header;
		}

		insert_ordered_array((void*)hole_header, &heap->index);
	}

	return (void *)((uint32)block_header + sizeof(header_t));
}

void heap_free (void *p, heap_t *heap) {

	if (p == 0) {
		return;
	}

	header_t *header = (header_t*)((uint32) p - sizeof(header_t));
	footer_t *footer = (footer_t*)((uint32)header + header->size -sizeof(footer_t));

	ASSERT (header->magic == HEAP_MAGIC);
	ASSERT (footer->magic == HEAP_MAGIC);

	header->is_hole = 1;

	char do_add = 1;

	footer_t *test_footer = (footer_t*)((uint32)header - sizeof(footer_t));
	if ((test_footer->magic == HEAP_MAGIC) &&
			(test_footer->header->is_hole == 1)) {
		uint32 cache_size = header->size;
		header = test_footer->header;
		footer->header = header;
		header->size += cache_size;
		do_add = 0;
	}

	header_t *test_header = (header_t *)((uint32)footer + sizeof(footer_t));
	if ((test_header->magic == HEAP_MAGIC) &&
			test_header->is_hole) {
		header->size += test_header->size;
		test_footer = (footer_t *)((uint32)test_header + test_header->size - sizeof(header_t));
		footer = test_footer;

		uint32 iterator = 0;

		while((iterator < heap->index.size) &&
				(lookup_ordered_array(iterator, &heap->index) != (void*)test_header)) {
			iterator++;
		}

		ASSERT (iterator < heap->index.size);
		remove_ordered_array (iterator, &heap->index);
	}

	if ((uint32)footer + sizeof(footer_t) == heap->end_address) {
		uint32 old_length = heap->end_address - heap->start_address;
		uint32 new_length = contract ((uint32)header - heap->start_address, heap);

		if ((header->size - (old_length - new_length)) > 0) {
			header->size -= old_length - new_length;
	   		footer = (footer_t *)((uint32)header + header->size -sizeof(footer_t));
	 		footer->magic = HEAP_MAGIC;
			footer->header = header;
		} else {
			uint32 iterator = 0;
			while ((iterator < heap->index.size) && 
					(lookup_ordered_array (iterator, &heap->index) != (void*)test_header)) {
				iterator++;
			}

			if (iterator < heap->index.size) {
				remove_ordered_array (iterator, &heap->index);
			}
		}		
	}

	if (do_add == 1) {
		insert_ordered_array ((void *)header, &heap->index);
	}

}

#endif