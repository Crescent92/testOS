#ifndef _ELF32_H
#define _ELF32_H

#include <type.h>
#include "process.h"

#define ELF_NIDENT	16
#define ELFMAG0     0x7F
#define ELFMAG1     'E'
#define ELFMAG2     'L'
#define ELFMAG3     'F'

#define ELFCLASSNONE    0
#define ELFCLASS32      1
#define ELFCLASS64      2

#define ELFDATANONE 0  // Invalid data encoding
#define ELFDATA2LSB 1  // Little Endian
#define ELFDATA2MSB 2  // Big Endian

#define ELFDATABO       1

#define EV_NONE     0
#define EV_CURRENT  1

// e_type
#define ET_NONE 0 // Unkown Type
#define ET_REL 1   // Relocatable File
#define ET_EXEC 2  // Executable File
#define ET_DYN 3   // Share object file
#define ET_CORE 4  // Core file

#define EM_NONE 0	// No machine
#define EM_M32 1	// AT&T WE 32100
#define EM_SPARC 2	// Sun Microsystems SPARC
#define EM_386 3	// Intel 80386
#define EM_68K 4	// Motorola 68000
#define EM_88K 5	// Motorola 88000
#define EM_486 6	/* Perhaps disused */
#define EM_860 7	// Intel 80860
#define EM_MIPS 8	// MIPS RS3000 (big-endian only)

#define SHN_UNDEF   0x00

#define ST_BINDING  ((INFO) >> 4)
#define ST_TYPE     ((INFO) & 0x0F)

#define PT_NULL     0
#define PT_LOAD     1

#define PF_X        0x1
#define PF_W        0x2
#define PF_R        0x4
#define PF_MASKPROC 0xf0000000

enum elf_ident {
    EI_MAG0,
    EI_MAG1,
    EI_MAG2,
    EI_MAG3,
    EI_CLASS,
    EI_DATA,
    EI_VERSION,
    EI_OSABI,
    EI_ABIVERS,
    EI_PAD
};

enum elf_sh_types {
    SH_NULL,
    SH_PROGBITS,
    SH_SYMTAB,
    SH_STRTAB,
    SH_RELA,
    SH_NOBITS,
    SH_REL
};

enum elf_sh_attr {
    SHF_WRITE,
    SHF_ALLOC
};

enum elf_st_bindings {
    STB_LOCAL,
    STB_GLOBAL,
    STB_WEAK
};

enum elf_st_types {
    STT_NOTYPE,
    STT_OBJECT,
    STT_FUNC
};

typedef struct elf32_hdr {
    uint8   _ident[ELF_NIDENT];
    uint16  _type;
    uint16  _machine;
    uint32  _version;
    uint32  _entry;
    uint32  _phoff;
    uint32  _shoff;
    uint32  _flags;
    uint16  _ehsize;
    uint16  _phentsz;
    uint16  _phnum;
    uint16  _shentsz;
    uint16  _shnum;
    uint16  _shstrndx;
} elf32_hdr_t;

typedef struct elf32_shdr {
    uint32  _sh_name;
    uint32  _sh_type;
    uint32  _sh_flags;
    uint32  _sh_addr;
    uint32  _sh_off;
    uint32  _sh_sz;
    uint32  _sh_link;
    uint32  _sh_info;
    uint32  _sh_addralgn;
    uint32  _sh_entry_sz;
}  elf32_shdr_t;

typedef struct elf32_sym {  // not needed?
    uint32  _st_name;
    uint32  _st_value;
    uint32  _st_sz;
    uint8   _st_info;
    uint8   _st_other;
    uint16  _st_shndx;
} elf32_sym_t;

typedef struct elf32_phdr { 
    uint32  _p_type;
    uint32  _p_offset;
    uint32  _p_vaddr;
    uint32  _p_paddr;
    uint32  _p_filesz;
    uint32  _p_memsz;
    uint32  _p_flags;
    uint32  _p_align;
}  elf32_phdr_t;


void elf32_load(process_t *proc, char *buf);

#endif