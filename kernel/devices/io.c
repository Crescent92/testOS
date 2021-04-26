#include "io.h"

void outb(uint16 port, uint32 value)
{
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

uint8 inb(uint16 port)
{
   uint8 ret;
   asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
}


void outl(uint16 port, uint32 value)
{
    asm volatile ("outl %1, %0" : : "dN" (port), "a" (value));
}

uint32 inl(uint16 port)
{
   uint32 ret;
   asm volatile("inl %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
}

uint32 inportw(uint16 portid) {
    uint32 ret;
    asm volatile("inw %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

void outportw(uint16 port, uint32 value) {
    asm volatile ("outw %1, %0" : : "dN" (port), "a" (value));
}