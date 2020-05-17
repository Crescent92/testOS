#include "type.h"
#include "common.h"
#include "gdt.h"

extern void gdt_flush();
static void gdt_set_gate(int num, uint32  base, uint32 limit, uint8 access, uint8 flags);

gdt_entry_t gdt_entries[5];
gdt_ptr_t   gdt_ptr;

void init_descriptor_tables() {
        init_gdt();
}



void init_gdt() {
        gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
        gdt_ptr.base  = (uint32)&gdt_entries;

        //NUll segment
        gdt_set_gate(0, 0, 0, 0, 0);
        //kernel code segment
        gdt_set_gate(1, 0, 0xffffffff, 0x9a, 0xcf);
        //kernel data segment
        gdt_set_gate(2, 0, 0xffffffff, 0x92, 0xcf);
        //user mode code segment
        gdt_set_gate(3, 0, 0xffffffff, 0xfa, 0xcf);
        //user mode data segment
        gdt_set_gate(4, 0, 0xffffffff, 0xf2, 0xcf);

        gdt_flush();
}

static void gdt_set_gate(int num, uint32  base, uint32 limit, uint8 access, uint8 flags) {
/*      uint32 * entry = (uint32 *)&gdt_entries[num];
        *entry = (limit & 0xffff) | ((base & 0xffff) << 16);
        *(entry + 1) = ((base >> 16) & 0xff) | ((access & 0xff) << 8) |
                (((limit >> 16) & 0x0f) << 16) | ((gran & 0xf0) << 16) |
                (((base >> 24) & 0xff) << 24);
                */
        gdt_entries[num].base_low = (base & 0xffff);
        gdt_entries[num].base_middle = ((base >> 16) & 0xff);
        gdt_entries[num].base_high = ((base >> 24) & 0xff);

        gdt_entries[num].limit_low = (limit & 0xffff);
        gdt_entries[num].limit_high = ((limit >> 16) & 0x0f);

        access |= 0x10;

        gdt_entries[num].flags = flags;
        gdt_entries[num].access_byte = access;

}

