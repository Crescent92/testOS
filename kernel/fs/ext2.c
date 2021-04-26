#include "ext2.h"


// static void read_block(ext2_fs *fs, uint32 block, uint8 *buf) {
//     memcpy(buf, fs->device + block * 4096, 4096);
// }


// static void write_block(ext2_fs *fs, uint32 block, uint8 *buf) {
//     memcpy(fs->device + block * 4096, buf, 4096);
// }

// static void clear_block(ext2_fs *fs, uint32 block) {
//     memset(fs->device + block * 4096, 0, 4096);
// }

// static void write_superblock(ext2_fs *fs) {
//     write_block(fs, 1, (uint8 *)/*superblock*/);
// }

// static void write_group_descriptor() {
//     write_block(fs, 2, (uint8 *)&fs->/*group descriptors */);
// }

