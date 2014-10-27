/*  <src-path>/src/kernel/inc/mt/user.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#ifndef USER_H
#define USER_H

#include<stdint.h>
typedef int32_t uid_t;
typedef int32_t gid_t;
typedef enum 
{
	S_IRUSR=0000400,
	S_IWUSR=0000200,
	S_IXUSR=0000100,
	S_IRWXG=0000070,
	S_IRGRP=0000040,
	S_IWGRP=0000020,
	S_IXGRP=0000010,
	S_IRWXO=0000007,
	S_IROTH=0000004,
	S_IWOTH=0000002,
	S_IXOTH=0000001,
	S_ISUID=0004000,
	S_ISGID=0002000,
	S_ISVTX=0001000
} mode_t;

struct user {
    uint32_t 	u_id;
};
#endif
