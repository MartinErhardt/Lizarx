/*   <src-path>src/usr/Daemons/VFS/ipc_manager.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef IPC_MANAGER_H
#define IPC_MANAGER_H

#include<sys/types.h>

class IPC_Helper
{
	public:
		static void sleep();
		static void wakeup(pid_t pid);
		static unsigned long msgget();
		static unsigned long msgsnd(unsigned long id, void *ptr, size_t size);
		static ssize_t msgrcv(unsigned long id, void *ptr, size_t size);
		static unsigned long shmget();
		static void * shmat(unsigned long id);
};
class IPC_Manager
{
	public:
		static void init();
		static void message_queue_loop();
		static void handle_open(pid_t proc, char *path);
		static void handle_close(pid_t proc, char *path);
};

#endif
