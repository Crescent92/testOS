
#ifndef _INITRD_H
#define _INITRD_H


#include "type.h"
#include "fs.h"
#include "common.h"
#include "mem.h"

typedef struct {
	uint32 nfiles;
} initrd_header_t;


typedef struct {
	uint8 magic;
	char name[64];
	uint32 offset;
	uint32 length;
} initrd_file_header_t;


fs_node_t *initialize_initrd (uint32 location);


#endif
