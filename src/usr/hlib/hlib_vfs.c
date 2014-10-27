/*  <src-path>/src/usr/hlib/hlib_vfs.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#include"hlib_ipc.h"
#include"hlib_vfs.h"
#include<string.h>
#include"dbg.h"
#include<sys/stat.h>
#include<stdlib.h>
#include"OO/sem.h"
#include"OO/AList.h"
#include<errno.h>
#include<unistd.h>
#include"multiboot_modules.h"
int my_queue;
Sem* my_sem;
#define FAIL_BADF {errno=EBADF; return -1;}
#define CHECK_FD if(ids->alist_get_entry_n()<=fd) FAIL_BADF \
	struct fs_idds * my_id;\
	if ((my_id=ids->alist_get_by_index(fd))==(struct fs_idds*)5) FAIL_BADF
AList<struct fs_idds>* ids;
void init_ipc()
{
	my_queue=hlib_msgget();
	my_sem= new Sem();
	ids=new AList<struct fs_idds>(10);
}
int hlib_open(const char*path ,int oflag)
{
	struct VFS_msg my_buf;
	int ret[2];
	struct fs_idds * new_id = (struct fs_idds*)malloc(sizeof(struct fs_idds));
	new_id->read_sem=new Sem();
	new_id->write_sem=new Sem();
	new_id->file_pointer=0;
	new_id->read_sem->up();// initialize semaphore with 1
	new_id->write_sem->up();// initialize semaphore with 1
	my_buf.type=VFS_OPEN;
	my_buf.message_queue_id=my_queue;
	my_buf.read_sem=new_id->read_sem->sem_id;
	my_buf.write_sem=new_id->write_sem->sem_id;
	my_buf.sync_sem=my_sem->sem_id;
	my_buf.pid=get_pid_hlib();
	strcpy((char*)&(my_buf.path),path);
	hlib_msgsnd(0,&my_buf, sizeof(struct VFS_msg));
	my_sem->down();
	hlib_msgrcv(my_queue, &ret,sizeof(int)*2);
	if(ret[1])
	{
		errno=ret[1];
		free(new_id);
		return -1;
	}
	new_id->file_header=(struct file_header*)hlib_shmat(ret[0]);
	ids->alist_add(new_id);
//	uprintf((char*)(new_id->file_header)+sizeof(struct file_header));
	return ids->alist_get_index(new_id);
}
int hlib_close(int fd)
{
	CHECK_FD
	ids->alist_set(fd, (struct fs_idds*)5);
	free(my_id);
	//hlib_msgsnd(0,my_id->stat.name, sizeof(struct VFS_msg));
	return 0;
}
int hlib_write(int fd, void* buf, size_t nbyte)
{
	CHECK_FD
	my_id->write_sem->down();
	if(my_id->file_header->stat.st_size-my_id->file_pointer-nbyte<0)
		nbyte = my_id->file_header->stat.st_size-my_id->file_pointer;//FIXME cannot resize
	memcpy((((char*)my_id->file_header)+sizeof(struct file_header)),buf, nbyte);
	my_id->write_sem->up();
	return nbyte;
}
int hlib_lseek(int fd, off_t offset, int whence)
{
	CHECK_FD
	int ret=0;
	my_id->read_sem->down();
	if(!my_id->file_header->read_count)
		my_id->write_sem->down();
	my_id->file_header->read_count++;
	my_id->read_sem->up();
	if(whence==SEEK_SET)
		my_id->file_pointer=offset;
	else if (whence==SEEK_CUR)
		my_id->file_pointer+=offset;
	else if(whence==SEEK_END)
		my_id->file_pointer=my_id->file_header->stat.st_size-offset;
	else
	{
		errno=EINVAL;
		ret=-1;
	}
	if(!ret)
		ret=my_id->file_pointer;
	my_id->read_sem->down();
	my_id->file_header->read_count--;
	if(!my_id->file_header->read_count)
		my_id->write_sem->up();
	my_id->read_sem->up();
	return offset;
}
int hlib_read(int fd, void* buf, size_t nbyte)
{
	CHECK_FD
	my_id->read_sem->down();
	if(!my_id->file_header->read_count)
		my_id->write_sem->down();
	my_id->file_header->read_count++;
	my_id->read_sem->up();
	if(my_id->file_header->stat.st_size-my_id->file_pointer-nbyte<0)
		nbyte = my_id->file_header->stat.st_size-my_id->file_pointer;
	memcpy(buf,(const void*)(((char*)my_id->file_header)+sizeof(struct file_header)+my_id->file_pointer),nbyte);
	my_id->file_pointer+=nbyte;
	my_id->read_sem->down();
	my_id->file_header->read_count--;
	if(!my_id->file_header->read_count)
		my_id->write_sem->up();
	my_id->read_sem->up();
	return nbyte;
}
int hlib_stat(const char * path, struct stat * buf)
{
	int fd;
	if(fd=hlib_open(path,0)<0)
		return -1;
	struct fs_idds * my_id=ids->alist_get_by_index(fd);
	memcpy(buf, &my_id->file_header->stat, sizeof(struct stat));
	hlib_close(fd);
	return 0;
}
