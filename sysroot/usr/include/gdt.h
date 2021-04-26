#ifndef _GDT_H
#define _GDT_H

#include "type.h"

void init_gdt();

struct gdt_entry_struct {
        uint16 limit_low;
        uint16 base_low;
        uint8 base_middle;
        uint8 access_byte;
        unsigned limit_high : 4;
        unsigned flags : 4;
        uint8    base_high;
} __attribute__((packed));


// struct gdt_entry_struct {
// 	unsigned short limit_low;
// 	unsigned short base_low;
// 	unsigned char base_middle;
// 	unsigned char access;
// 	unsigned char granularity;
// 	unsigned char base_high;
// } __attribute__((packed));


typedef struct gdt_entry_struct gdt_entry_t;

struct gdt_ptr_struct {
        uint16 limit;
        uint32 base;
} __attribute__((packed));

typedef struct gdt_ptr_struct gdt_ptr_t;

void init_descriptor_tables();

void set_kern_stack (uint32 stack);

#endif

