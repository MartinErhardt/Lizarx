/*  <src-path>/src/usr/Daemons/VFS/FS/tmpFS/tmpfs.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
 *  Copyright (C) 2013, 2014  martin.erhardt98@googlemail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef TMPFS_H
#define TMPFS_H
#include<stdint.h>
#include<OO/AList.h>
#include<sys/types.h>
#include<sys/stat.h>
#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_CHARDEVICE  0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE        0x05
#define FS_SYMLINK     0x06

void*fs_args[5];
struct ramfs_node
{
	char name[256];     // The filename.
	/*mode_t mask;        // The permissions mask.
	uid_t uid;         // The owning user.
	gid_t gid;         // The owning group.
	uint32_t flags;       // Includes the node type. See #defines above.
	ino_t inode;       // This is device-specific - provides a way for a filesystem to identify files.
	size_t length;      // Size of the file, in bytes.*/
	uint32_t flags;
	struct stat my_stat;
	int hardlink_ref;
	char * direct_block_ptr[12];
	char **once_indirect_block;
};
struct tar_header
{
    char filename[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag[1];
}__attribute__((aligned(512)));
class MountPoint
{
	AList<struct ramfs_node> * my_nodes;
	struct ramfs_node* resolve_path(char* path,char** final_name);
	struct ramfs_node*get_fromdir(struct ramfs_node* cur_dir, char* name);
	public:
		const char * mount_path;
		MountPoint()
		{
			my_nodes = new AList<struct ramfs_node> (100);
			struct ramfs_node * root=(struct ramfs_node*)malloc(sizeof(struct ramfs_node));
			memset(root,0,sizeof(struct ramfs_node));
			root->flags=FS_DIRECTORY;
			my_nodes->alist_add(root);
		};
		static MountPoint * get_mp(char * path, int * matched);
		int mount();
		int unmount(const char *target);
		unsigned long open(const char * path);
		void write(const char * path, off_t off);
		void close(const char * path);
		struct ramfs_node* create(char * path);
		struct ramfs_node* mkdir(char * path);
		void initrd(struct tar_header * data,uintptr_t bootmod_end);
		struct ramfs_node* stat(char * path);
		ssize_t pwrite(char* path,const void* buf, size_t nbytes, off_t off);
		ssize_t pread(char* path,void* buf, size_t nbytes, off_t off);
};

class ramFS
{
	//extern "C" void uprintf(const char* bla_bla);
	public:
		void mount();
		void unmount();
		void open();
		void close();
		void create();
		void pwrite();
		void pread();
};

#endif
