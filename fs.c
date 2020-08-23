
#include "fs.h"

fs_node_t *fs_root = 0;

uint32 read_fs (fs_node_t *node, uint32 offset, uint32 size, uint8 *buffer) {
	if (node->read) {
		uint32 ret = node->read (node, offset, size, buffer);
		return ret;
	} else {
		return 0;
	}
}


uint32 write_fs (fs_node_t *node, uint32 offset, uint32 size, uint8 *buffer) {
	if (node->write) {
		uint32 ret = node->write (node, offset, size, buffer);
		return ret;
	} else {
		return 0;
	}
}

void open_fs (fs_node_t *node, uint8 read, uint8 write) {
	if (node->open) {
		node->open (node, read, write);
	}
}

void close_fs (fs_node_t *node) {
	if (node->close) {
		node->close (node);
	}
}


struct dirent *readdir_fs (fs_node_t *node, uint32 index) {
	if ((node->flags & FS_DIRECTORY) && (node->readdir != 0)) {
		struct dirent *ret = node->readdir (node, index);
		return ret;
	} else {
		return (struct dirent *)NULL;
	}
}

fs_node_t *finddir_fs (fs_node_t *node, char *name) {
	if ((node->flags & FS_DIRECTORY) && (node->readdir != 0)) {
		fs_node_t *ret = node->finddir (node, name);
		return ret;
	} else {
		return (fs_node_t *)NULL;
	}
}

