#ifndef _EXT2_H
#define _EXT2_H

#include <type.h>

typedef struct {
    uint32 s_inodes_count;
    uint32 s_blocks_count;
    uint32 s_r_blocks_count;
    uint32 s_free_blocks_count;
    uint32 s_free_inodes_count;
    uint32 s_first_data_block;
    uint32 s_log_block_size;
    uint32 s_log_frag_size;
    uint32 s_blocks_per_group;
    uint32 s_frags_per_group;
    uint32 s_inodes_per_group;
    uint32 s_mtime;
    uint32 s_wtime;

    uint16 s_mnt_count;
    uint16 s_max_mnt_count;
    uint16 s_magic;
    uint16 s_state;
    uint16 s_errors;
    uint16 s_minor_rev_level;

    uint32 s_lastcheck;
    uint32 s_checkinternval;
    uint32 s_creator_os;
    uint32 s_rev_level;

    uint16 s_def_resuid;
    uint16 s_def_resgid;

    uint32 s_first_ino;

    uint16 s_inode_size;
    uint16 s_block_group_nr;
    
    uint32 s_feature_compat;
    uint32 s_feature_incompat;
    uint32 s_feature_ro_compat;
    
    uint8 s_uuid[16];
    
    char s_volume_name[16];
    char s_last_mounted[64];

    uint32 s_algo_bitmap;
    
    uint8 s_prealloc_blocks;
    uint8 s_prealloc_dir_blocks;
    
    uint16  s_padding; 

    uint8 s_journal_uuid[16];
    
    uint32 s_journal_inum;
    uint32 s_journal_dev;
    uint32 s_last_orphan;

    uint32 s_hash_seed[4];

    uint8 s_def_hash_version;
    uint8 s_reserved_char_pad;
    uint16 s_reserved_word_pad;
    uint32 s_default_mount_options;
    uint32 s_first_meta_bg;
    uint32 s_reserved[190];
} ext2_super_blk;

typedef struct {
    uint32 bg_block_bitmap;
    uint32 bg_inode_bitmap;
    uint32 bg_inode_table;
    uint16 bg_free_blocks_count;
    uint16 bg_free_inodes_count;
    uint16 bg_used_dirs_count;
    uint16 bg_pad;
    uint32 bg_reserved[3];
} ext2_group_desc;

typedef struct {
    uint16  i_mode;
    uint16  i_uid;
    uint32  i_size;
    uint32  i_atime;
    uint32  i_ctime;
    uint32  i_mtime;
    uint32  i_dtime;
    uint16  i_gid;
    uint16  i_links_count;
    uint32  i_blocks;
    uint32  i_flags;

    uint32  i_osd1;

    uint32  i_block[15];
    uint32  i_generation;
    uint32  i_file_acl;
    uint32  i_dir_acl;
    uint32  i_faddr;
    union {
        struct linux2{
            uint8   l_i_frag;
            uint8   l_i_fsize;
            uint16  reserved;
            uint16  l_i_uid_high;
            uint16  l_i_gid_high;
            uint32  reserved2;
        };

        struct hurd2 {
            uint8   h_i_frag;
            uint8   h_i_fsize;
            uint16  h_i_mode_high;
            uint16  h_i_uid_high;
            uint16  h_i_gid_high;
            uint32  h_i_author;
        };

        struct masix2 {
            uint8 m_i_frag;
            uint8 m_i_fsize;
            uint8 reserved[10];
        };

    } i_osd2;
} ext2_inode;

#define EXT2_NAME_LEN_MAX   255

typedef struct {
    uint32  inode;
    uint16  rec_len;
    uint8   name_len;
    uint8   file_type;
    char    name[];
}   ext2_dir_ent;


#endif