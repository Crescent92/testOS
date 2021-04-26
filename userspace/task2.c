
#include <printf.h>
#include <syscalls.h>
#include <malloc.h>


// char * brk_kheap2 = 0;
// int * brk_int_ptr = 0;
// int brk_int = 0;
// static char *_sbrk2(int sz) {
//     char * brk_kheap3;
// 	if ((sz % 4096) != 0 ) {
// 		sz = sz + (4096 - sz % 4096);
// 	}
//     brk_int = 12;
//     cprintf("buf_kheap: %d\n",brk_int);
// 	brk_kheap3 = (char *)0xB0000000;
//     // brk_int_ptr = (int *)0xC00000;
// 	cprintf("buf_kheap: %x\n",brk_kheap3);
// 	return brk_kheap3;

// }

void main(int argc, char **argv) {
    // cprintf("Test Malloc!\n");
    cprintf("Test Malloc2!\n");
    char * ptr = kmem_alloc(2);
    cprintf("ptr: %x\n", ptr);
    char * ptr2 = kmem_alloc(1);
    cprintf("ptr2: %x\n", ptr2);
    // char *ptr2 = (char *)kmem_alloc(1);
    // cprintf("ptr2: %x\n", ptr2);

    // cprintf("argc: %d\n", argc);
    // int x = 0;
    // int child_pid = 0;
    // printf("%s ", "Pokeabcd!\n");
    // cprintf("Pokeabcd!\n");
    // child_pid = sys_fork();
    // cprintf("ret val: %d\n", child_pid);
    // if (child_pid > 0) {
    //         cprintf("Henlo, Poke!\n");
    // } else {
    //         cprintf("Hello child!\n");
    //         sys_exec();
    // }
    // cprintf("ret val: %d\n", child_pid);
    // while (1) {
        // cprintf("Pokeabcd!\n");
        // if (child_pid > 0) {
            // cprintf("Henlo, Poke!\n");
        // } else {
            // cprintf("Hello child!\n");
            // sys_exec();
        // }
        
    // }
}