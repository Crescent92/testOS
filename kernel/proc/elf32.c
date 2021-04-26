#include "elf32.h"
#include <mem/paging.h>
#include "process.h"



static int elf_check_file (struct elf32_hdr *elf_header){
    // printf("elf check file\n");
    if (elf_header == 0) {
        return 0;
    }

    if ((elf_header->_ident[EI_MAG0] != ELFMAG0) &&
    (elf_header->_ident[EI_MAG1] != ELFMAG1) &&
    (elf_header->_ident[EI_MAG2] != ELFMAG2) &&
    (elf_header->_ident[EI_MAG3] != ELFMAG3)) {
        printf("Here1\n");
        return 0;
    } 

    if (elf_header->_ident[EI_CLASS] != ELFCLASS32) {
        printf("Here2\n");
        return 0;
    } 

    if (elf_header->_ident[EI_DATA] != ELFDATA2LSB) {
        printf("Here3\n");
        return 0;
    }

    if (elf_header->_ident[EI_VERSION] != EV_CURRENT) {
        printf("Here4\n");
        return 0;
    }

    if (elf_header->_machine != EM_386) {
        printf("Here5\n");
        return 0;
    }

    if ((elf_header->_type != 1) && (elf_header->_type != 2)) {
        return 0;
    }
    // printf("Through!\n");
    return 1;

}

static void load_elf_data (page_directory_t *pages, char *src, elf32_phdr_t *phdr) {
    // printf("load_elf_data\n");
    for (uint32 addr = phdr->_p_vaddr; addr < (phdr->_p_vaddr + phdr->_p_memsz); addr += VMM_PAGE_SIZE) {
        vmm_map_page(current_kernel_directory, addr, 0, 1);
    }
    // printf("load elf data memcpy\n");
    memset(phdr->_p_vaddr, 0, phdr->_p_memsz);
    memcpy(phdr->_p_vaddr, src + phdr->_p_offset, phdr->_p_filesz);
    // printf("load elf data memcpy done\n");
    if (phdr->_p_memsz > phdr->_p_filesz) {
        memset((phdr->_p_vaddr + phdr->_p_filesz), 0, phdr->_p_memsz - phdr->_p_filesz);
    }
}

void elf32_load(process_t *proc, char *buf) {
    elf32_hdr_t *elf_header = (elf32_hdr_t *)buf;
    // printf("elf32_load\n");
    if ((elf_check_file(elf_header) != 1) || (elf_header->_phoff == 0)) {
        return;
    }
    // printf(" Header validated!\n");
    elf32_phdr_t *phdr;
    for (int i = 0; i < elf_header->_phnum; i++) {
        phdr = (elf32_phdr_t *)(buf + elf_header->_phoff + (i * elf_header->_phentsz));
        // printf("PHeader type %x offset %x vaddr %x mem sz %x\n", phdr->_p_type, phdr->_p_offset, phdr->_p_vaddr, phdr->_p_memsz);
        //load elf data
        if (phdr->_p_flags & PF_X) {
            printf("pheader exec\n");
        }

        if (phdr->_p_flags & PF_W) {
            printf("pheader write\n");
        }
        
        load_elf_data(current_kernel_directory, buf, phdr);
    }

    proc->entry = elf_header->_entry;
}


// void elf_load(char *buf, process_t *proc) {
//     elf32_hdr_t *elf_header = (elf32_hdr_t *)buf;
//     if ((elf_check_file(elf_header) != 1) || (elf_header->_phoff == 0)) {
//         return;
//     }
//     proc->entry = elf_header->_entry;

//     for (elf32_phdr_t *ph = (elf32_phdr_t *)(buf + elf_header->_phoff); 
//     ph && ((char *)ph < (buf + elf_header->_phoff + elf_header->_phentsz * elf_header->_phnum));
//     ++ph) {
//         if (ph->_p_type != PT_LOAD) {
//             continue;
//         }

//         if (((ph->_p_flags & PF_X) != 0) && ((ph->_p_flags & PF_R) != 0)) {
//             vmm_map_page(kernel_directory, ph->_p_vaddr, 0, 1);
//         } 
//         else if (((ph->_p_flags & PF_W) != 0) && ((ph->_p_flags & PF_R) != 0)) {
//             vmm_map_page(kernel_directory, ph->_p_vaddr, 0, 1);
//             //mmap
//         } 
//         else {
//             vmm_map_page(kernel_directory, ph->_p_vaddr, 0, 1);
//         }

//         memset((char *)ph->_p_vaddr, 0, ph->_p_memsz);
//         memcpy((char *)ph->_p_vaddr, buf + ph->_p_offset, ph->_p_filesz);
//     }

// }
