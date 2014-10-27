/*  <src-path>/src/usr/Daemons/VFS/FS/tmpFS/tmpfs.cpp is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#include"tmpfs.h"
#include<dbg.h>
#include<asm_inline.h>
#include<vmm.h>
#include<sys/dirent.h>
#include<multiboot_modules.h>
#include<sys/stat.h>
#include<errno.h>
AList<MountPoint> * mount_points;
static int strn_cmp_n(const char * s1, const char * s2);
static size_t chopN(char *str, size_t n);
static int find(char* to_search, char* search_in);
static int find_last(char* to_search, char* search_in);
static ssize_t read_from_node(struct ramfs_node* file_res,void *buf, size_t nbyte,off_t offset);
static ssize_t write_to_node(struct ramfs_node* file_des,const void *buf, size_t nbyte,off_t offset);
static void create_hardlink(struct ramfs_node* dir, struct ramfs_node* to);
typedef enum
{
	NO_FLGS,
	FLG_INITIAL
}mount_flags;
int vfs_err_;
void ramFS::mount()
{
	char *source=(char *)fs_args[1];
	char *target=(char *)fs_args[2];
	unsigned long mountflags=(unsigned long)fs_args[3];
	struct bootmod * initrd_mod= (struct bootmod *) fs_args[4];
	uprintf("[ramFS] I: mount ");
	if(mountflags&FLG_INITIAL)
		uprintf("initial ");
	uprintf("ramfs on ");
	uprintf(target);
	uprintf(" ... ");
	MountPoint*my_mp= new MountPoint();
	if(!mount_points)
		mount_points = new AList<MountPoint> (5);
	my_mp->mount_path=target;
	mount_points->alist_add(my_mp);
	uprintf(" SUCCESS\n");
	if(mountflags&FLG_INITIAL)
		my_mp->initrd((struct tar_header*)initrd_mod->start,((uintptr_t)initrd_mod->start)+initrd_mod->size);
}
void ramFS::unmount()
{
	//TODO
}
void ramFS::open()
{
	// The ramfs don't need to map any files into memory as e.g ext2 or others
	char * path=(char*)fs_args[0];
	MountPoint * my_mp=NULL;
	int matched;
	struct ramfs_node* ret;
	if(my_mp = MountPoint::get_mp(path,&matched))
	{
		if(ret=my_mp->stat(path+matched))
			fs_args[0] = (void*) &(ret->my_stat);//NOTE: the return value is of type ssize_t, but all arguments are void* pointers, so we cast to compile it
		else fs_args[0]=NULL;
	}
	else
		fs_args[0]=NULL;
	if((!fs_args[0])&&(!vfs_err_))
		fs_args[1]=(void*)EAGAIN;
	else
		fs_args[1]=(void*)vfs_err_;
}
void ramFS::close()
{
	// The ramfs don't need to map any files into memory as e.g ext2 or others
}
void ramFS::pwrite()
{
	char * path=(char*)fs_args[0];
	MountPoint *  my_mp=NULL;
	int matched;
	if(my_mp = MountPoint::get_mp(path,&matched))
		fs_args[0] = (void*) my_mp->pwrite(path+matched,(const void*)fs_args[1],(size_t)fs_args[2],(off_t)fs_args[3]); //NOTE: the return value is of type ssize_t, but all arguments are void* pointers, so we cast to compile it
	else
		fs_args[0]=NULL;
}
void ramFS::pread()
{
	char * path=(char*)fs_args[0];
	MountPoint * my_mp=NULL;
	int vfs_err_=0;
	int matched;
	if(my_mp = MountPoint::get_mp(path,&matched))
		fs_args[0] = (void*) my_mp->pread(path+matched,fs_args[1],(size_t)fs_args[2],(off_t)fs_args[3]);//NOTE: the return value is of type ssize_t, but all arguments are void* pointers, so we cast to compile it
	else
		fs_args[0]=NULL;
}
MountPoint * MountPoint::get_mp(char * path, int * matched)
{
	int mps_n = mount_points->alist_get_entry_n();
	int i;
	int most_matched_n = 0;
	int current_matched_n = 0;
	MountPoint * most_matched = NULL;
	MountPoint * current = most_matched;
	for(i=0;i<mps_n; i++)
	{
		current = mount_points->alist_get_by_index(i);
		if((current_matched_n = strn_cmp_n(path, current->mount_path))>most_matched_n)
		{
			most_matched = current;
			most_matched_n = current_matched_n;
		}
	
	}
	*matched=most_matched_n;
	return most_matched;
}
void MountPoint::initrd(struct tar_header * data,uintptr_t bootmod_end)
{
	size_t cur_size=0;
	struct ramfs_node* cur_node=NULL;
	while(data->filename[0]!= '\0')
	{
		cur_size=strtol(data->size, NULL, 8);
		uprintf("file size: ");
		uprintf(itoa(cur_size,16));
		uprintf("\tfile name: ");
		uprintf(data->filename);
		uprintf("\n");
		if(!cur_size && (data->filename[2]!='\0'))
			mkdir(&data->filename[2]);
		else if(data->filename[2]!='\0')
		{
			cur_node=create(&data->filename[2]);
			write_to_node(cur_node,data+1,cur_size,0);
		}
		data=(struct tar_header*) (((uintptr_t)data)+((cur_size / 512) + 1) * 512);
		if(cur_size% 512)
			data++;
	}
}
ssize_t MountPoint::pwrite(char* path,const void* buf, size_t nbytes, off_t off)
{
	struct ramfs_node* my_node=stat(path);
	if(my_node)
		return write_to_node(my_node,buf,nbytes, off);
	else return 0;
}
ssize_t MountPoint::pread(char* path,void* buf, size_t nbytes, off_t off)
{
	struct ramfs_node* my_node=stat(path);
	if(my_node)
		return read_from_node(my_node, buf, nbytes,off);
	else return 0;
}
struct ramfs_node* MountPoint::mkdir(char * path)
{
	struct ramfs_node * new_dir;
	if(path[strlen(path)-1] == '/')
		path[strlen(path)-1] ='\0';
	if(new_dir=create(path))
		new_dir->flags=FS_DIRECTORY;
}
struct ramfs_node* MountPoint::create(char * path)
{
	struct ramfs_node* our_dir;
	char * final_name=NULL;
	if(!(our_dir= resolve_path(path,&final_name)))
	{
		uprintf("[ramFS] E: MountPoint::create says: \"couldn't resolve path to /");
		uprintf(path);
		uprintf(" \"\n");
		return NULL;
	}
	struct ramfs_node * new_ramfs_node=(struct ramfs_node*)malloc(sizeof(struct ramfs_node));
	memset(new_ramfs_node,0,sizeof(struct ramfs_node));
	memcpy(new_ramfs_node->name,final_name, strlen(final_name));
	my_nodes->alist_add(new_ramfs_node);
	new_ramfs_node->my_stat.st_ino = my_nodes->alist_get_index(new_ramfs_node);
	new_ramfs_node->flags = FS_FILE;
	create_hardlink(our_dir,new_ramfs_node);
	return new_ramfs_node;
}
struct ramfs_node* MountPoint::stat(char * path)
{
	struct ramfs_node* our_dir;
	char* final_name=NULL;
	if(!(our_dir= resolve_path(path,&final_name)))
	{
		uprintf("[ramFS] E: MountPoint::create says: \"couldn't resolve path to /");
		uprintf(path);
		uprintf(" \"\n");
		return NULL;
	}
	return get_fromdir(our_dir,final_name);
}
struct ramfs_node* MountPoint::resolve_path(char* path, char** final_name)
{
	struct ramfs_node* current_node = my_nodes->alist_get_by_index(0);
	int cur_name_off=0;
	int final_name_off=0;
	*final_name=path;
	while(1)
	{
		if((final_name_off=find((char*)"/", *final_name ))<0)
			return current_node;
		*final_name=*final_name+final_name_off+1;
		if(!(current_node=get_fromdir(current_node,path+cur_name_off)))
		{
			uprintf("[ramFS] E: MountPoint::resolve_path says: \"Directory: /");
			uprintf(path+cur_name_off);
			uprintf(" not found\"\n");
			vfs_err_=ENOTDIR;
			return NULL;
		}
		if(current_node->flags!=FS_DIRECTORY)
		{
			uprintf("[ramFS] E: MountPoint::resolve_path says: \" /");
			uprintf(path+cur_name_off);
			uprintf(" is no Directory\"\n");
			vfs_err_=ENOTDIR;
			return NULL;
		}
		cur_name_off+=(final_name_off+1);
	}
}
struct ramfs_node* MountPoint::get_fromdir(struct ramfs_node* cur_dir, char* name)
{
	unsigned int allready_read=0;
	dirent_t cur_dirent_buf;
	int slash_there;
	if(slash_there = find((char*)"/", name))
		name[slash_there]='\0';
	while(allready_read<cur_dir->my_stat.st_size)
	{
		read_from_node(cur_dir,&cur_dirent_buf,sizeof(dirent_t),allready_read);
		allready_read+=sizeof(dirent_t);
		if(!strcmp(cur_dirent_buf.d_name,name))
		{
			if(slash_there)
				name[slash_there] = '/';
			return my_nodes->alist_get_by_index(cur_dirent_buf.d_ino);
		}
	}
	uprintf("[ramFS] E: Mountpoint::get_fromdir says: \"File not found");
	uprintf(name);
	uprintf("\"\n");
	return NULL;
}
static ssize_t read_from_node(struct ramfs_node* file_res,void *buf, size_t nbyte,off_t offset)
{
	unsigned int bytes_read=0;
	unsigned int block_n=offset/PAGE_SIZE;
	unsigned int to_read=nbyte;
	unsigned int place_in_block;
	char* cur_block=NULL;
	void * read_from;
	if(offset+nbyte>file_res->my_stat.st_size)
		file_res->my_stat.st_size = offset+nbyte;
	while(bytes_read<nbyte)
	{
		block_n = offset/PAGE_SIZE;
		place_in_block = PAGE_SIZE-(offset%PAGE_SIZE);
		if(place_in_block<nbyte-bytes_read)
			to_read=place_in_block;
		else
			to_read=nbyte-bytes_read;
		if(block_n<12) 
			cur_block=file_res->direct_block_ptr[block_n];
		else 
		{
			uprintf("[ramFS] I: read_from_node says: \"indirect block pointers, not yet tested\"\n");
			if(block_n>PAGE_SIZE/sizeof(char*)+10) // we have to sub two one,because index <> size and another because of the '<' operator
				return bytes_read;
			if(!file_res->once_indirect_block)
				return bytes_read;
			if(!file_res->once_indirect_block[block_n-12])
				return bytes_read;
			else
				cur_block=file_res->once_indirect_block[block_n-12];
		}
		if(!cur_block)
			return bytes_read;
		read_from=(void*)(cur_block+(offset%PAGE_SIZE));
		memcpy(((void*)(char*)buf+bytes_read),(const void*)read_from,to_read);
	//	uprintf("copy from");
	//	uprintf(itoa((uintptr_t)read_from,16));
	//	uprintf("to");
	//	uprintf(itoa((uintptr_t)(char*)buf+bytes_read,16));
		bytes_read+=to_read;
		offset=((offset/PAGE_SIZE)+1)*PAGE_SIZE;
	}
	return bytes_read;
}
static ssize_t write_to_node(struct ramfs_node* file_des,const void *buf, size_t nbyte,off_t offset)
{
	unsigned int bytes_written=0;
	unsigned int block_n=offset/PAGE_SIZE;
	unsigned int to_write=nbyte;
	unsigned int place_in_block;
	char* cur_block=NULL;
	if(offset+nbyte>file_des->my_stat.st_size)
		file_des->my_stat.st_size = offset+nbyte;
	while(bytes_written<nbyte)
	{
		block_n = offset/PAGE_SIZE;
		place_in_block = PAGE_SIZE-(offset%PAGE_SIZE);
		if(place_in_block<nbyte-bytes_written)
			to_write=place_in_block;
		else
			to_write=nbyte-bytes_written;
		if(block_n<12)
			cur_block=file_des->direct_block_ptr[block_n];
		else 
		{
			uprintf("[ramFS] I: write_to_node says: \"indirect block pointers, not yet tested\"\n");
			if(block_n>PAGE_SIZE/sizeof(char*)+10) // we have to sub two one,because index <> size and another because of the '<' operator
			{
				uprintf("[ramFS] E: write_to_node says: \"no more direct and once indirect block pointers\"\n");
				return bytes_written;
			}
			if(!file_des->once_indirect_block)
				file_des->once_indirect_block=(char**)vmm_malloc(PAGE_SIZE);
			if(!file_des->once_indirect_block[block_n-12])
			{
				cur_block=(char*)vmm_malloc(PAGE_SIZE);
				file_des->once_indirect_block[block_n-12]=cur_block;
			}
			else
				cur_block=file_des->once_indirect_block[block_n-12];
		}
		if(!cur_block)
		{
			cur_block=(char*)vmm_malloc(PAGE_SIZE);
			file_des->direct_block_ptr[block_n]=cur_block;
		}
		memcpy(cur_block+(offset%PAGE_SIZE),buf+bytes_written,to_write);
		bytes_written+=to_write;
		offset=((offset/PAGE_SIZE)+1)*PAGE_SIZE;
	}
	return bytes_written;
}
static void create_hardlink(struct ramfs_node* dir, struct ramfs_node* to)
{
	dirent_t new_dirent_buf;
	new_dirent_buf.d_ino=to->my_stat.st_ino;
	memset(new_dirent_buf.d_name,0,_D_NAME_MAX+1);
	memcpy(new_dirent_buf.d_name,&to->name[0],_D_NAME_MAX);
	write_to_node(dir,&new_dirent_buf,sizeof(dirent_t),dir->my_stat.st_size);
}
static int find(char* to_search, char* search_in)
{
	int whole_length=strlen(search_in);
	int to_search_length=strlen(to_search);
	int i,j;
	for(i=0;i<whole_length-to_search_length+1;i++)
		for(j=0;j<to_search_length;j++)
			if(*(search_in+i+j)!=*(to_search+j))
				break;
			else if(j==to_search_length-1)
				return i;
	return -1;
}
static int find_last(char* to_search, char* search_in)
{
	int whole_length=strlen(search_in);
	int to_search_length=strlen(to_search);
	int i,j;
	int found_at=-1;
	for(i=0;i<whole_length-to_search_length+1;i++)
		for(j=0;j<to_search_length;j++)
			if(*(search_in+i+j)!=*(to_search+j))
				break;
			else if(j==to_search_length-1)
				found_at=i;
	return found_at;
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
