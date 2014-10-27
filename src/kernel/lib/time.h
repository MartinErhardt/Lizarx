/*  <src-path>/src/kernel/lib/time.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#ifndef TIMER_H
#define TIMER_H

#include<stdint.h>

#define CLOCKS_PER_SEC 1024

struct tm
{
	unsigned int tm_sec;
	unsigned int tm_min;
	unsigned int tm_hour;
	unsigned int tm_mday;
	unsigned int tm_mon;
	unsigned int tm_year;
	unsigned int tm_wday;
	unsigned int tm_yday;
	unsigned int tm_isdst;
};

typedef unsigned int clock_t;
typedef unsigned int time_t;

clock_t clock (void);
//double difftime (time_t end, time_t beginning);
time_t time (time_t* timer);
time_t mktime (struct tm * timeptr);

char* ctime (const time_t * timer);
char* asctime (const struct tm * timeptr);
struct tm * gmtime (const time_t * timer);
struct tm *gmtime_r(const time_t *timer, struct tm *tmbuf);
struct tm * localtime (const time_t * timer);
size_t strftime (char* ptr, size_t maxsize, const char* format,const struct tm* timeptr );
void print_time(struct tm * time);

#endif
