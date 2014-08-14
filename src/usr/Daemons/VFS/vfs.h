/*   <src-path>src/usr/Daemons/VFS/vfs.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
 * 
 *   Copyright (C) 2013  martin.erhardt98@googlemail.com
 *
 *  Lizarx is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Lizarx is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU LESSER General Public License
 *  along with Lizarx.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef VFS_H
#define VFS_H

#include<OO/AList.h>
#include<sys/types.h>
typedef enum {
	VFS_OPEN,
	VFS_CLOSE,
	VFS_WRITE
} syscalls;
class VFS
{
	AList<struct mount_point> * mount_points;
	AList<class FS> * loaded_fs_s;
	AList<struct open_file_info> * open_files;
	public:
		VFS()
		{
			mount_points = new AList<struct mount_point> (4);
			loaded_fs_s = new AList<class FS>(4);
			open_files = new AList<struct open_file_info>(50);
		};
		void loop();
		int mount(const char *source, const char *target,
          		const char *filesystemtype, unsigned long mountflags,
          		const void *data);
		void unmount();
		struct metablock * open(const char * path);
		void close(const char * path);
		struct mount_point * resolve_path(const char * path);
};
class FS
{
	void* mount_func;
	void* unmount_func;
	void* open_func;
	void* close_func;
	public:
		FS()
		{
			
		};
		void mount();
		void unmount();
		struct metablock * open(const char * path);
		void close(const char * path);
};
struct open_file_info
{
	char * path;
	struct mount_point* mp;
	struct metablock * block[12];
};
struct mount_point
{
	char * path;
	char * source;
	char * mount_point;
	mode_t access;
	char * type;
};
struct metablock
{
	void * block;
	uint8_t type;
};
#endif
