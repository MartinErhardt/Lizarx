/*  <src-path>/src/kernel/inc/ipc/ipc.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#ifndef IPC_H
#define IPC_H
#include<mt/user.h>
#include<stdint.h>
struct ipc_perm
{
	uid_t uid;
	gid_t gid;
	uid_t cuid;
	gid_t cgid;
	mode_t mode;
};
typedef uint_t key_t;
#define IPC_CREAT 1
#define IPC_EXCL 2
#define NO_WAIT 4

#define IPC_PRIVATE 0

#define IPC_RMID 1
#define IPC_SET 2
#define IPC_STAT 3

#endif
