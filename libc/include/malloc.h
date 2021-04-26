#ifndef _STDLIB_H
#define _STDLIB_H

#include<type.h>

void kmem_free(void *bp);

char *kmem_alloc(uint32 size);

#endif