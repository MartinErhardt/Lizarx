/*  <src-path>/src/usr/Daemons/VFS/vfs.cpp is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#include"vfs.h"
#include<hlib_ipc.h>
#include<stdint.h>
#include<dbg.h>
#include<string.h>
#include"../../asm_inline.h"
#include<multiboot_modules.h>
#include<../../ld_a/ld.h>
#include<asm_inline.h>
#include<vmm.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<hlib_vfs.h>
#include<ld.h>
FS* where_is_code_=NULL; //TODO make this thread specific when we are more multithreading
static int strn_cmp_n(const char * s1, const char * s2);
#define MAX_PATH_LENGTH 255
struct VFS_msg buf;
struct bootmod bootmod_initrd;
int vfs_err;
void VFS::init()
{
	FS * initial_ramfs = new FS("ramFS");
	loaded_fs->alist_add(initial_ramfs);
	struct elf_lib lib_;
	struct elf_lib main_;
	initial_ramfs->fs_bootmod.start	= NULL;
	initial_ramfs->fs_bootmod.size	= 0;
	bootmod_main.start	= NULL;
	bootmod_main.size	= 0;
	get_bootmod(1,&(initial_ramfs->fs_bootmod));
	get_bootmod(2,&bootmod_main);
	get_bootmod(4,&bootmod_initrd);
	uintptr_t lib_load_addr	=(uintptr_t)init_shared_lib(initial_ramfs->fs_bootmod.start);
	
	lib_.header		= (struct elf_header *)initial_ramfs->fs_bootmod.start;
	
	lib_.runtime_addr	= lib_load_addr;
	main_.header		= (struct elf_header *)bootmod_main.start;
	
	main_.runtime_addr	= 0;
	
	link_lib_against(&lib_,&main_);
	initial_ramfs->mount_ = (func_fs*)get_func(&lib_, "_ZN5ramFS5mountEv");
	initial_ramfs->unmount_ =(func_fs*) get_func(&lib_, "_ZN5ramFS7unmountEv");
	initial_ramfs->open_ =(func_fs*) get_func(&lib_, "_ZN5ramFS4openEv");
	initial_ramfs->pwrite_=(func_fs*) get_func(&lib_, "_ZN5ramFS6pwriteEv");
	initial_ramfs->close_=(func_fs*) get_func(&lib_, "_ZN5ramFS5closeEv");
	initial_ramfs->pread_=(func_fs*) get_func(&lib_, "_ZN5ramFS5preadEv");
	initial_ramfs->fs_args=(void**) get_func(&lib_, "fs_args");
	mount(NULL,"/","ramFS",1,&bootmod_initrd);

//	char * license_txt=(char*)malloc(0x2000);
//	pread("/doc/README.md",(void*)license_txt,0x2000,0);
//	uprintf("\n");
//	uprintf(license_txt);
}
void VFS::loop()
{
	uprintf("[VFS] I: init VFS ...");
	int id = hlib_msgget();
	int i;
	uprintf("SUCCESS\n");
	struct open_file_info* new_open=NULL;
	struct stat* to_open;
	struct file_header my_header;
	int reply[2];
	hlib_wakeup(2);
	unsigned long*io_buf=NULL;
	while(1)
		if(hlib_msgrcv(id, &buf, sizeof(struct VFS_msg)))
			switch(buf.type)
			{
				case(VFS_OPEN):
					uprintf("VFS OPEN!");
					vfs_err=0;
					for(i=0;i<open_files->alist_get_entry_n();i++)
						if(!strcmp(open_files->alist_get_by_index(i)->path, buf.path))
							new_open=open_files->alist_get_by_index(i);
					if(!new_open)// in any other cases the file has been opened and is allready in open_files
					{
						to_open = open((const char*)buf.path);
						if(!to_open)
							goto fin;
						new_open=(struct open_file_info*)malloc(to_open->st_size);
						new_open->shmid=hlib_shmget(to_open->st_size);
						new_open->addr=hlib_shmat(new_open->shmid);
						new_open->read_sem=new Sem(buf.read_sem);
						new_open->write_sem=new Sem(buf.write_sem);
						new_open->ref_counter=1;
						open_files->alist_add(new_open);
						my_header.stat=*to_open;
						my_header.read_sem=buf.read_sem;
						my_header.write_sem=buf.write_sem;
						*((struct file_header*)((unsigned long)new_open->addr))=my_header;
						pread(buf.path, (char*)(((unsigned long)new_open->addr)+sizeof(struct file_header)),to_open->st_size, 0);
						reply[0] = new_open->shmid;
						reply[1]= 0;
					}
					else new_open->ref_counter ++;
fin:
					if(vfs_err)
					{
						reply[0]=0;
						reply[1]=vfs_err;
					}
					else
					{
						reply[0] = new_open->shmid;
						reply[1] =0;
					}
					hlib_msgsnd(buf.message_queue_id, &reply,sizeof(int)*2);
					new_open=NULL;
					hlib_semup(buf.sync_sem);
					break;
				case(VFS_WRITE):
					io_buf=(unsigned long*)malloc(buf.fur_size);
					hlib_msgrcv(buf.message_queue_id, io_buf,buf.fur_size);
					pwrite(buf.path,(char*)io_buf,buf.fur_size,buf.off);//FIXME Buffer overflow
					hlib_wakeup(buf.pid);
					free(io_buf);
					break;
				case(VFS_READ):
					uprintf("VFS read size: 0x");
					uprintf(itoa(buf.fur_size,16));
					io_buf=(unsigned long*)malloc(buf.fur_size);
					pread(buf.path, (char*)io_buf,buf.fur_size, buf.off);// FIXME Buffer overflow	
					hlib_msgsnd(buf.message_queue_id,io_buf,buf.fur_size);
					hlib_wakeup(buf.pid);
					free(io_buf);
				default: break;
			}
}
FS * VFS::get_FS_by_type(const char * type)
{
	unsigned int i;
	for(i=0; i<loaded_fs->alist_get_entry_n(); i++)
		if(!strcmp(loaded_fs->alist_get_by_index(i)->fs_type,type))
			return loaded_fs->alist_get_by_index(i);
	return NULL;
}
struct mount_point * VFS::resolve_path(const char * path)
{
	int mps_n = mount_points->alist_get_entry_n();
	int i;
	int most_matched_n = 0;
	int current_matched_n = 0;
	struct mount_point * most_matched = NULL;
	struct mount_point * current = most_matched;
	for(i=0;i<mps_n; i++)
	{
		current = mount_points->alist_get_by_index(i);
		if((current_matched_n = strn_cmp_n(path, current->mount_path))>most_matched_n)
		{
			most_matched = current;
			most_matched_n = current_matched_n;
		}
	}
	return most_matched;
}
int VFS::mount(const char *source, const char *target,
          const char *filesystemtype, unsigned long mountflags,
          const void *data)
{
	FS* my_FS= get_FS_by_type(filesystemtype);
	struct mount_point * new_mp =(struct mount_point *)malloc(sizeof( struct mount_point));
	new_mp->mount_path	= (char*)target;
	new_mp->source	= (char*)source;
	new_mp->access	= 0;
	new_mp->type	= my_FS;
	mount_points->alist_add(new_mp);
	my_FS->mount(source,target,mountflags,data);
	return 0;
}
struct stat* VFS::open(const char* path)
{
	struct mount_point*my_mp=resolve_path(path);
	if(my_mp)
		return my_mp->type->open(path);
	return NULL;
}
void VFS::close(const char* path)
{
}
ssize_t VFS::pwrite(const char*path,void*buf,size_t nbytes, off_t off)
{
	struct mount_point*my_mp=resolve_path(path);
	if(my_mp)
		return my_mp->type->pwrite(path,buf,nbytes,off);
	return 0;
}
ssize_t VFS::pread(const char*path,void*buf,size_t nbytes, off_t off)
{	
	struct mount_point*my_mp=resolve_path(path);
	if(my_mp)
		return my_mp->type->pread(path,buf,nbytes, off);
	return 0;
}
int FS::mount(const char *source, const char *target, unsigned long mountflags, const void *data)
{
	where_is_code_=this;
	fs_args[1]=(void*)source;
	fs_args[2]=(void*)target;
	fs_args[3]=(void*)mountflags;
	fs_args[4]=(void*)data;
	mount_(source,target,mountflags,data);
	where_is_code_=NULL;
	return 0;
}
int FS::unmount(const char *target)
{
	where_is_code_=this;
	unmount_();
	where_is_code_=NULL;
	return 0;
}
struct stat * FS::open(const char * path)
{
	where_is_code_=this;
	fs_args[0]=(void*)path;
	open_();
	vfs_err=(int)((unsigned long)fs_args[1]);
	where_is_code_=NULL;
	return (struct stat*)fs_args[0];
}
ssize_t FS::pwrite(const char * path , void * buf, size_t nbytes,off_t off)
{
	where_is_code_=this;
	fs_args[0] =(void*)path;
	fs_args[1] =buf;
	fs_args[2]=(void*)nbytes;
	fs_args[3]=(void*)off;
	pwrite_();
	where_is_code_=NULL;
	return (ssize_t)fs_args[0];
}
ssize_t FS::pread(const char* path, void * buf, size_t nbytes,off_t off)
{
	where_is_code_=this;
	fs_args[0] =(void*)path;
	fs_args[1] =buf;
	fs_args[2]=(void*)nbytes;
	fs_args[3]=(void*)off;
	pread_();
	where_is_code_=NULL;
	return (ssize_t)fs_args[0];
}
void FS::close(const char * path)
{
	where_is_code_=this;
	close_();
	where_is_code_=NULL;
}
static int strn_cmp_n(const char * s1, const char * s2)
{
	char * fmt = (char*)s1;
	char * fmt2 = (char*)s2;
	int sim = 0;
	while(*fmt && *fmt2)
		if(*fmt != *fmt2)
			break;
		else
			sim++,fmt++;
	return sim;
}
