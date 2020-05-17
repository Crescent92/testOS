#ifndef _GDT_H
#define _GDT_H

#include "type.h"

void init_gdt();


/*typedef struct {
        uint16 seg_limit_low;
        uint16 base_addr_low;
        uint8  base_addr_mid;
        uint8  type : 4;
        uint8  s    : 1;
        uint8  dpl  : 2;
        uint8  p    : 1;
        uint8  seg_limit_high : 4;
        uint8  avail : 1;
        uint8  l    : 1;
        uint8  d_b  : 1;
        uint8  g    : 1;
        uint8  base_addr_high;
} gdt_entry_t;

*/

struct gdt_entry_struct {
        uint16 limit_low;
        uint16 base_low;
        uint8 base_middle;
        uint8 access_byte;
        unsigned limit_high : 4;
        unsigned flags : 4;
        uint8    base_high;
} __attribute__((packed));

typedef struct gdt_entry_struct gdt_entry_t;




struct gdt_ptr_struct {
        uint16 limit;
        uint32 base;
} __attribute__((packed));

typedef struct gdt_ptr_struct gdt_ptr_t;

void init_descriptor_tables();

#endif

