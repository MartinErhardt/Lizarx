/*  <src-path>/src/usr/hlib/hlib_vfs.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#ifndef HLIB_VFS_H_
#define HLIB_VFS_H_
#include<sys/types.h>
#include"OO/sem.h"
#include<sys/stat.h>
struct VFS_msg
{
	unsigned long type;
	unsigned long pid;//FIXME get queue owner with msgctl
	int message_queue_id;
	int read_sem;
	int write_sem;
	int sync_sem;
	int seq;
	size_t fur_size;
	off_t off;
	char path[256];
};
struct file_header
{
	struct stat stat;
	int read_sem;
	int write_sem;
	int read_count;
	int fur_size;
};
typedef enum {
	VFS_OPEN,
	VFS_CLOSE,
	VFS_WRITE,
	VFS_READ
} vfs_calls;
struct fs_idds
{
	Sem * write_sem;
	Sem * read_sem;
	off_t file_pointer;
	struct file_header * file_header;
};
struct sys_semop
{
        int id;
        struct sem_buf* sops;
        size_t nsops;
};
struct sem_buf
{
        unsigned short sem_num;
        short semop;
        short sem_flg;
};
void init_ipc();
unsigned long get_pid_hlib();
int hlib_open(const char* fmt, int oflag);
int hlib_read(int fd, void* buf, size_t nbyte);
int hlib_lseek(int fd, off_t offset, int whence);
int hlib_write(int fd, void* buf, size_t nbyte);
int hlib_stat(const char * path, struct stat * buf);
int hlib_close(int fd);

#endif
