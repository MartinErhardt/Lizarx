/*
 * Copyright 2002-2010 Haiku Inc. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _DIRENT_H
#define _DIRENT_H


#include <sys/types.h>


typedef struct dirent {
	dev_t			d_dev;		/* device */
	dev_t			d_pdev;		/* parent device (only for queries) */
	ino_t			d_ino;		/* inode number */
	ino_t			d_pino;		/* parent inode (only for queries) */
	unsigned short	d_reclen;	/* length of this record, not the name */
	char			d_name[1];	/* name of the entry (null byte terminated) */
} dirent_t;

/*struct __DIR {
	        int                             fd;
        short                   next_entry;
        unsigned short  entries_left;
        long                    seek_position;
        long                    current_position;
        struct dirent   first_entry;
	int dd_fd;
	long dd_loc;
};


typedef struct __DIR DIR;
*/

typedef struct {
	int dd_fd;          /* directory file */
	int dd_loc;         /* position in buffer */
	int dd_seek;
	char *dd_buf;       /* buffer */
	int dd_len;         /* buffer length */
	int dd_size;        /* amount of data in buffer */
	int dd_lock;
} DIR;


#ifndef MAXNAMLEN
#	ifdef  NAME_MAX
#		define MAXNAMLEN NAME_MAX
#	else
#		define MAXNAMLEN 256
#	endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

DIR*			fdopendir(int fd);
DIR*			opendir(const char* dirName);
struct dirent*	readdir(DIR* dir);
int				readdir_r(DIR* dir, struct dirent* entry,
					struct dirent** _result);
int				closedir(DIR* dir);
void			rewinddir(DIR* dir);
void 			seekdir(DIR* dir, long int position);
long int		telldir(DIR* dir);
int				dirfd(DIR* dir);

int				alphasort(const struct dirent** entry1,
					const struct dirent** entry2);
int				scandir(const char* dir, struct dirent*** _entryArray,
					int (*selectFunc)(const struct dirent*),
					int (*compareFunc)(const struct dirent** entry1,
						const struct dirent** entry2));

#ifdef __cplusplus
}
#endif

#endif	/* _DIRENT_H */
