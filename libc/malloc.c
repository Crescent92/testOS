#include<malloc.h>
#include<syscalls.h>
#include<printf.h>

static void *kmem_coalesce(void *bp);
static void *extend_heap(uint32 size);
static void *find_fit(uint32 sz);
static void place(void *bp, uint32 asz);
static char *kmem_sbrk();

#define	WSIZE	4
#define DSIZE	8
#define	CHUNK	(1 << 12)

#define MAX(x, y)	((x) > (y) ? (x) : (y))

#define PACK(sz, alloc)	((sz) | (alloc))

#define GET(p)	(*(unsigned int *)(p))
#define PUT(p, val)	(*(unsigned int *)(p) = (val))

#define GET_SIZE(p)	((GET(p)) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

#define HDRP(bp)	((char *)(bp) - WSIZE)
#define FTRP(bp)	((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

#define NEXT_BLKP(bp)	((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp)	((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))


static char *bottom_kheap;
static char *brk_kheap;
static char *cur_max_heap;
static char *heap_list;

static char *_sbrk(int sz) {
	if ((sz % 4096) != 0 ) {
		sz = sz + (4096 - sz % 4096);
	}

	brk_kheap = (char *)0xB0000000;
	cprintf("buf_kheap: %x",brk_kheap);
	return brk_kheap;

}

static void _kmem_init() {
	cprintf("_kmem_init\n");
	bottom_kheap = _sbrk(4096);
	cprintf("poke\n");
	cur_max_heap = (char *)(bottom_kheap + 4096);
	cprintf("_kmem_init_done\n");
}

static int kmem_init() {
	cprintf("kmem_init\n");
	_kmem_init();
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
 * sz should be multiple of 4k.
 */
static char *kmem_sbrk(int sz) {

	char * old_brk = brk_kheap;
	if ((sz < 0) || ((brk_kheap + sz) > cur_max_heap)) {
		//map 1 page
		brk_kheap = _sbrk(4096);
		cur_max_heap += sz;
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

char *kmem_alloc(uint32 sz) {
	uint32 asz;
	uint32 extend_sz;
	char *bp;

	if (heap_list == 0) {
		kmem_init();
	}

	if (sz == 0) {
		return 0;
	}

	if (sz <= DSIZE) {
		asz = 2 * DSIZE;
	} else {
		asz = DSIZE * ((sz + (DSIZE) + (DSIZE - 1)) / DSIZE);
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
		PUT(HDRP(bp), PACK(csz - asz, 0));
		PUT(FTRP(bp), PACK(csz - asz, 0));
	} else {
		PUT(HDRP(bp), PACK(csz, 1));
		PUT(FTRP(bp), PACK(csz, 1));
	}
}