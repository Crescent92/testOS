#ifndef _ELF32_H
#define _ELF32_H

#include "type.h"

#define ELF_NIDENT	16
#define ELFMAG0     0x7F
#define ELFMAG1     'E'
#define ELFMAG2     'L'
#define ELFMAG3     'F'

#define ELFCLASS32  1
#define ELFDATABO   1

#define EM_386      3
#define EV_CURRENT  1

#define SHN_UNDEF   0x00

#define ST_BINDING  ((INFO) >> 4)
#define ST_TYPE     ((INFO) & 0x0F)

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

enum elf_type {
    ET_NONE,
    ET_REL,
    ET_EXEC
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

typedef struct {
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
} elf32_hdr;

typedef struct {
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
} elf32_shdr;

typedef struct {
    uint32  _st_name;
    uint32  _st_value;
    uint32  _st_sz;
    uint8   _st_info;
    uint8   _st_other;
    uint16  _st_shndx;
} elf32_sym;

typedef struct {
    uint32  _p_type;
    uint32  _p_offset;
    uint32  _p_vaddr;
    uint32  _p_paddr;
    uint32  _p_filesz;
    uint32  _p_memsz;
    uint32  _p_flags;
    uint32  _p_align;
} elf32_phdr;



#endif