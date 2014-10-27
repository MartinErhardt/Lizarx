/*  <src-path>/src/usr/hlib/inc/sys/stat.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#ifndef STAT_H
#define STAT_H
#include<sys/types.h>
struct stat
{
	dev_dev_t st_dev; 
	ino_t st_ino;
	mode_t st_mode; 
	nlink_t st_nlink; 
	uid_t st_uid;
	gid_t st_gid;
	dev_t st_rdev; 

	off_t st_size            
	struct timespec st_atim Last data access timestamp. 
	struct timespec st_mtim Last data modification timestamp. 
	struct timespec st_ctim Last file status change timestamp. 
}
#endif
