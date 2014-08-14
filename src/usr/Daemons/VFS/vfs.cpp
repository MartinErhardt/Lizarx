
#include"vfs.h"
#include<OO/ipc_helper.h>
#include<stdint.h>
#include<dbg.h>
#include"../../asm_inline.h"

extern "C" void uprintf(const char* fmt, ...);
extern "C" char * itoa(unsigned int n, unsigned int base);
static int strn_cmp_n(const char * s1, const char * s2);
#define MAX_PATH_LENGTH 256
struct VFS_msg
{
	unsigned long type;
	unsigned long message_queue_id;
	char path[MAX_PATH_LENGTH+1];
};
struct VFS_msg buf;
void VFS::loop()
{
	uprintf("[VFS] I init VFS ...");
	unsigned long id = IPC_Helper::msgget();
	uprintf("SUCCESS\n");
	while(1)
		if(IPC_Helper::msgrcv(id, &buf, sizeof(struct VFS_msg)))
			switch(buf.type)
			{
				case(VFS_OPEN):
					uprintf("VFS open: ");
					uprintf((char*)&(buf.path));
					uprintf("from pid ");
					uprintf(itoa(buf.message_queue_id,10));
					break;
				default: break;
			}
}
struct mount_point * VFS::resolve_path(const char * path)
{
	int mps_n = mount_points->alist_get_entry_n();
	int i;
	int most_matched_n = 0;
	int current_matched_n = 0;
	struct mount_point * most_matched = mount_points->alist_get_by_index(0);
	struct mount_point * current = most_matched;
	for(i=0;i<mps_n; i++)
	{
		current = mount_points->alist_get_by_index(i);
		if((current_matched_n = strn_cmp_n(path, current->mount_point))>most_matched_n)
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
	struct mount_point * new_mp =(struct mount_point *)malloc(sizeof( struct mount_point));
	new_mp->path	= (char*)target;
	new_mp->source	= (char*)source;
	new_mp->access	= 0;
	new_mp->type	= (char*) filesystemtype;
	mount_points->alist_add(new_mp);
	return 0;
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
