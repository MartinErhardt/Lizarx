/*   <src-path>/src/kernel/lib/time.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include"time.h"
#include<drv/timer/timer.h>
#include<drv/hwtime/hwtime.h>
#include<mt/threads.h>
#define SECS_PER_MIN 60
#define SECS_PER_HOUR 3600
#define SECS_PER_DAY 86400
#define SECS_PER_YEAR 31556952

#define YEAR0_IN_UNIX_TIME 1970

clock_t clock (void)
{
	return -1;
}/*
double difftime (time_t end, time_t beginning)
{
	return (double)(end-beginning);
}*/
time_t mktime (struct tm * timeptr)
{
	return 	((((timeptr->tm_year-YEAR0_IN_UNIX_TIME)*SECS_PER_YEAR)+
		((timeptr->tm_year/4)*SECS_PER_DAY))+// leap year
		timeptr->tm_yday*SECS_PER_DAY+
		timeptr->tm_hour*SECS_PER_HOUR+
		timeptr->tm_min*SECS_PER_MIN);
}
time_t time (time_t* timer)
{
	time_t new_timer=mktime(get_time());
	if(timer!=NULL){
		*timer=new_timer;
	}
	return new_timer;
}