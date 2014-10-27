/*  <src-path>/src/usr/hlib/OO/sem.cpp is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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

#include"sem.h"
#include"../hlib_ipc.h"
#include"../hlib_vfs.h"
void Sem::up()
{
	struct sem_buf op_up;
	op_up.sem_num=0;
	op_up.semop=1;
	op_up.sem_flg=0;
	hlib_semop(sem_id, &op_up, 1);
}
void Sem::down()
{	
	struct sem_buf op_down;
	op_down.sem_num=0;
	op_down.semop=-1;
	op_down.sem_flg=0;
	hlib_semop(sem_id, &op_down, 1);
}
void Sem::op(int n)
{
	struct sem_buf op;
	op.sem_num=0;
	op.semop=n;
	op.sem_flg=0;
	hlib_semop(sem_id, &op, 1);
}
