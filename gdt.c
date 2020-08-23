#include "type.h"
#include "common.h"
#include "gdt.h"
#include "tss.h"

extern void gdt_flush();
extern void tss_flush();
static void gdt_set_gate(int num, uint32  base, uint32 limit, uint8 access, uint8 flags);
// static void gdt_set_gate(uint32 num, uint32 base, uint32 limit, uint8 access, uint8 gran);

gdt_entry_t gdt_entries[6];
gdt_ptr_t   gdt_ptr;

tss_entry_t tss_entry_;

void init_descriptor_tables() {
        init_gdt();
}

static void write_tss (int32 num, int16 ss0, uint32 esp0) {
        uint32  base = (uint32)&tss_entry_;
        uint32  limit = base + sizeof(tss_entry_);

        gdt_set_gate(num, base, limit, 0xE9, 0x00);

        memset(&tss_entry_, 0, sizeof(tss_entry_));

        tss_entry_.ss0 = ss0;
        tss_entry_.esp0 = esp0;

        tss_entry_.cs = 0x0B;
        tss_entry_.ss = tss_entry_.ds = tss_entry_.es = tss_entry_.fs = tss_entry_.gs = 0x13;

}

void set_kern_stack (uint32 stack) {
        tss_entry_.esp0 = stack;
}


void init_gdt() {
        gdt_ptr.limit = (sizeof(gdt_entry_t) * 6) - 1;
        gdt_ptr.base  = (uint32)&gdt_entries;

        //NUll segment
        gdt_set_gate(0, 0, 0, 0, 0);
        //kernel code segment
        gdt_set_gate(1, 0, 0xffffffff, 0x9a, 0xcf);
        //kernel data segmenz
        gdt_set_gate(2, 0, 0xffffffff, 0x92, 0xcf);
        //user mode code segment
        gdt_set_gate(3, 0, 0xffffffff, 0xfa, 0xcf);
        //user mode data segment
        gdt_set_gate(4, 0, 0xffffffff, 0xf2, 0xcf);
        printf("Write Tss!\n");
        write_tss(5, 0x10, 0x0);
        printf("Tss write done!\n");
        printf("flushing gdt!\n");
        gdt_flush();
        printf("done gdt flushing!\n");
        printf("flushing tss!\n");
        tss_flush();
}


static void gdt_set_gate(int num, uint32  base, uint32 limit, uint8 access, uint8 flags) {
        gdt_entries[num].base_low = (base & 0xffff);
        gdt_entries[num].base_middle = ((base >> 16) & 0xff);
        gdt_entries[num].base_high = ((base >> 24) & 0xff);

        gdt_entries[num].limit_low = (limit & 0xffff);
        gdt_entries[num].limit_high = ((limit >> 16) & 0x0f);

//        access |= 0x10;

        gdt_entries[num].flags = flags;
        gdt_entries[num].access_byte = access;

}


// static void gdt_set_gate(uint32 num, uint32 base, uint32 limit, uint8 access, uint8 gran)
// {
// 	gdt_entries[num].base_low = (base & 0xFFFF);
// 	gdt_entries[num].base_middle = (base >> 16) & 0xFF;
// 	gdt_entries[num].base_high = (base >> 24) & 0xFF;

// 	gdt_entries[num].limit_low = (limit & 0xFFFF);
// 	gdt_entries[num].granularity = (limit >> 16) & 0x0F;		// segment len

// 	gdt_entries[num].granularity |= gran & 0xF0;
// 	gdt_entries[num].access	= access;
// }
