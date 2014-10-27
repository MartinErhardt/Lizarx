/*  <src-path>/src/usr/Daemons/VFS/vfs.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#ifndef VFS_H
#define VFS_H

#include<OO/AList.h>
#include<sys/types.h>
#include<multiboot_modules.h>
#include<hlib_vfs.h>
class VFS
{
	AList<struct mount_point> * mount_points;
	AList<class FS> * loaded_fs;
	AList<struct open_file_info> * open_files;
	FS * get_FS_by_type(const char * fs_type);
	struct mount_point * resolve_path(const char * path);
	struct bootmod bootmod_main;
	public:
		VFS()
		{
			mount_points = new AList<struct mount_point> (4);
			loaded_fs = new AList<class FS>(4);
			open_files = new AList<struct open_file_info>(50);
		};
		void init();
		void loop();
		int mount(const char *source, const char *target,
			const char *filesystemtype, unsigned long mountflags,
			const void *data);
		int umount(const char *target);
		//unsigned long create(const char * path);
		struct stat* open(const char * path);
		void write(const char * path, off_t off);
		void close(const char * path);
		ssize_t pwrite(const char * path , void * buf, size_t nbytes,off_t off);
		ssize_t pread(const char* path, void * buf, size_t nbytes,off_t off);
};

typedef int func_fs(...);
class FS
{
	public:
		char* fs_type;
		func_fs *  mount_;
		func_fs * unmount_;
		func_fs * open_;
		func_fs * pwrite_;
		func_fs * pread_;
		func_fs * close_;
		void **fs_args;
		struct bootmod fs_bootmod;
		FS(const char * fs_type_)
		{
			fs_type=(char*)fs_type_;
		};
		int mount(const char *source,const char *target, unsigned long mountflags, const void *data);
		int unmount(const char *target);
		struct stat* open(const char * path);
		ssize_t pwrite(const char * path , void * buf, size_t nbytes,off_t off);
		ssize_t pread(const char* path, void * buf, size_t nbytes,off_t off);
		void close(const char * path);
};

struct open_file_info
{
	char path[256];
	int shmid;
	Sem * read_sem;
	Sem * write_sem;
	int ref_counter;
	void*addr;
};
struct mount_point
{
	char * path;
	char * source;
	char * mount_path;
	mode_t access;
	FS * type;
};
#endif
