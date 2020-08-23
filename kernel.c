#include "type.h"
#include "monitor.h"
#include "gdt.h"
#include "idt.h"
#include "timer.h"
#include "irq.h"
#include "keyboard.h"
#include "paging.h"
#include "mem.h"
#include "multiboot.h"
#include "fs.h"
#include "initrd.h"
#include "process.h"

extern process_t *last_task_created;
extern void switch_usr_mode();
extern uint32 get_esp();

void task1() {
	int count = 0;
	//process_t *next = get_proc_t_by_name("task2");
	for (;;)
	{
		printf("task1..\n");
    	
		// if (next != NULL) {
		// 	task_switch(next);
		// }
		lock_scheduler();
		schedule();
		unlock_scheduler();

		count++;
		if( count == 2) {
			block_task(PROC_BLOCKED);
		}
		
	}
	
}

void task2() {
	
	// process_t *next = get_proc_t_by_name("task1");
	int count = 0;

	while (1)
	{
		printf("task2..\n");
		/* code */
	// 	if (next != NULL) {
	// 	task_switch(next);
	// }
		lock_scheduler();
		schedule();
		unlock_scheduler();

		count++;

		if(count == 10) {
			unblock_task(get_proc_t_by_name_from_wait_list("task1"));
		}
	}
	
}

void task_user () {
	for(;;) {
		// printf("Henlo!\n");
	}
}

void kernel_main (struct multiboot *mboot_ptr) {
	uint32 esp_t;
	monitor_init();
    printf("Initializing GDT!\n");
	init_gdt();

    printf("Initializing IDT!\n");
	init_idt();


	printf("Remapping and Installing IRQs\n");
	irq_install();

	init_timer(1000);
	init_keyboard();

//	ASSERT(mboot_ptr->mods_count > 0);


//	printf("mem_lower: %x mem_upper: %x \n", mboot_ptr->mem_lower, mboot_ptr->mem_upper);

/*	uint32 initrd_location = *((uint32 *)mboot_ptr->mods_addr);
	uint32 initrd_end = *(uint32 *)(mboot_ptr->mods_addr + 4);

	placement_address = initrd_end;

	vmm_initialize();

	fs_root = initialize_initrd(initrd_location);*/
	vmm_initialize();
	initialize_multitasking();
	asm volatile("mov %%esp, %0" : "=r"(esp_t));
	set_kern_stack(esp_t);
	// create_kern_task(task1, "task1");
	// create_kern_task(task2, "task2");
	//create_kern_task(task2);
	// printf("Switching to task1..\n");

	// lock_scheduler();
	// schedule();
	// unlock_scheduler();
	switch_usr_mode();
	// printf("Back to kernel_main..\n");
	for(;;) {
		printf("Hello user mode\n");
	}
}

