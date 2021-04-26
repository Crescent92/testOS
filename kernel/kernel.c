#include <type.h>


#include <devices/monitor.h>
#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <system/timer.h>
#include <cpu/irq.h>
#include <devices/keyboard.h>
#include <mem/paging.h>
#include <mem/mem.h>
#include "multiboot.h"
#include <fs/fs.h>
#include <fs/initrd.h>
#include <proc/process.h>
#include <proc/elf32.h>
#include "syscall.h"
#include <cpu/exception.h>
#include <malloc.h>
#include <devices/pci.h>

phy_mem_layout_t phy_mem_;

extern process_t *last_task_created;
extern void switch_usr_mode();
extern uint32 get_esp();

void task1() {
	// int count = 0;
	//process_t *next = get_proc_t_by_name("task2");
	printf("task1..\n");
	// switch_usr_mode();
	while(1) {
		printf("task1..\n");
		// lock_scheduler();
		// schedule();
		// unlock_scheduler();
	}
	
	
}

void task2() {
	
	// process_t *next = get_proc_t_by_name("task1");
	printf("task2..\n");
	while (1)
	{
		printf("task2..\n");
		// lock_scheduler();
		// schedule();
		// unlock_scheduler();
	}
	
}

void task_user () {
	for(;;) {
		// printf("Henlo!\n");
	}
}

void ls_dir_con() {
	int i = 0;
	struct dirent *node = 0;
	while((node = readdir_fs(fs_root, i)) != 0) {
		printf("Found file! %s ", node->name);
		fs_node_t *fsnode = finddir_fs(fs_root, node->name);

		if ((fsnode->flags&0x7) == FS_DIRECTORY) {
			printf("\n(directory)\n");
		} else {
			printf("Size: %d ", fsnode->length);
		}/*else {
			printf("\ncontents: \"");
			char buf[256];
			uint32 sz = read_fs(fsnode, 0, 256, buf);
			int j;

			// printf("%s", buf[j]);
			for (j = 0; j < sz; j++)
				printf("%c", buf[j]);
			printf("\"\n");
		}*/
		i++;
	}
	printf("\n");
}

fs_node_t *find_file_node (char *name) {
	
	printf("Find file node\n");
	int i = 0;
	struct dirent *node = 0;
	while((node = readdir_fs(fs_root, i)) != 0) {
		// printf("Found file2! %s ", node->name);
		fs_node_t *fsnode = finddir_fs(fs_root, node->name);

		if ((fsnode->flags & 0x7) == FS_FILE) {
			if (!strcmp(fsnode->name, name)) {
				printf("Name: %s Size: %d \n", fsnode->name,fsnode->length);
				return fsnode;
			}
		}
		i++;
	}
	printf("\n");

	return 0;
}
extern uint32 kstart;
extern uint32 kend;
extern uint32 __end;

void kernel_main (struct multiboot *mboot_ptr) {
	uint32 esp_t;
	uint32 total_mem = 0;
	monitor_init();
    printf("Initializing GDT!\n");
	init_gdt();

    printf("Initializing IDT!\n");
	init_idt();
	asm volatile("cli");

	printf("Remapping and Installing IRQs\n");
	irq_install();
	exception_init();
	init_timer(10);
	init_keyboard();
	multiboot_mem_info_t *mmap_info = mboot_ptr->mmap_addr;
	while (mmap_info < ((unsigned char *)mboot_ptr->mmap_addr + mboot_ptr->mmap_length)) 
	{
		printf("addr: %x ",mmap_info->addr);
		printf("size: %d ",mmap_info->size);
		printf("len: %x ",mmap_info->len);
		printf("type: %x\n",mmap_info->type);

		if (mmap_info->type == 1) {
			if (mmap_info->len > total_mem) {
				total_mem = mmap_info->len;
				phy_mem_.start = mmap_info->addr;
				phy_mem_.end = (uint32)((unsigned char *)mmap_info->addr + mmap_info->len); 
				phy_mem_.len = mmap_info->len;
			}
		}
		mmap_info =  (multiboot_mem_info_t *)((unsigned char *)mmap_info + mmap_info->size + sizeof (unsigned int));
	}
	printf("mem_start: %x mem_end: %x mem_len: %d/%x\n",phy_mem_.start,phy_mem_.end,phy_mem_.len,phy_mem_.len);
	printf("kstart: %x kend: %x __end: %x\n", &kstart, &kend, &__end);
	uint32 kern_sz = (uint32)&kend - (uint32)&kstart;
	printf("Kernel size in bytes: %d\n", kern_sz);

//	ASSERT(mboot_ptr->mods_count > 0);

	uint32 initrd_location = *((uint32 *)mboot_ptr->mods_addr);
	uint32 initrd_end = *(uint32 *)(mboot_ptr->mods_addr + 4);
	
	placement_address = initrd_end;
	printf("placement_addr: %x\n",placement_address);
	placement_address = (char *)(placement_address & (~0xfff)) + VMM_PAGE_SIZE;	//start from next page
	printf("K: %x\n",placement_address);
	phy_mem_.heap_start = placement_address;
	vmm_init2(phy_mem_);

	// uint8 *data = (uint8 *)kmalloc_a(initrd_end - initrd_location);
	// memcpy(data, initrd_location, initrd_end - initrd_location);
	// for (;;)
	// {
	// 	/* code */
	// }
	
	// vmm_initialize();
	
	fs_root = initialize_initrd(initrd_location);
	
	// vmm_initialize();
	printf("Listing contents of fs\n");
	ls_dir_con();
	pci_init();
	printf("Done Listing contents of fs\n");
	initialize_multitasking();
	printf("Done init multitasking\n");
	init_syscalls();
	printf("Done init_syscalls\n");
	// create_kern_task(task1, "task1");
	// create_kern_task(task2, "task2");
	// printf("Switching to task1..\n");

	
	// asm volatile("mov %%esp, %0" : "=r"(esp_t));
	// printf("HENLO REVAMPED!");
	// set_kern_stack(esp_t);
	

	// lock_scheduler();
	// schedule();
	// unlock_scheduler();
	
	// asm volatile("sti");
	// asm volatile("sti");
	// printf("Start new proc\n");
#ifdef _RUN_
	start_new_proc(find_file_node("test"));
	asm volatile("sti");
#endif
	// switch_usr_mode();
	// lock_scheduler();
	// schedule();
	// unlock_scheduler();
	// printf("Done start neww proc\n");
	// asm volatile("sti");
	// switch_usr_mode();
	// switch_usr_mode();
	// process_t * p_ = get_proc_t_by_name("task1");
	// create_kern_task(task1, "task1");
	// printf("Switching to task1..\n");
#ifdef _RUN_
	for(;;) {

	}
#endif

}

