/*  <src-path>/src/kernel/lib/time.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#include"time.h"
#include<drv/timer/timer.h>
#include<drv/hwtime/hwtime.h>
#include<mt/threads.h>
#include<mm/vheap.h>
#include<dbg/console.h>

#define SECS_PER_MIN 60
#define SECS_PER_HOUR 3600
#define SECS_PER_DAY 86400
#define SECS_PER_YEAR 31536000

#define YEAR0_IN_UNIX_TIME 70

const char * week_days[] = {
  "Sunday", "Monday", "Tuesday", "Wednesday",
  "Thursday", "Friday", "Saturday"
};
const char * week_days_abrv[] = {
  "Sun", "Mon", "Tue", "Wed", 
  "Thu", "Fri", "Sat"
};
const char * years_months[] = {
  "January", "February", "March",
  "April", "May", "June",
  "July", "August", "September",
  "October", "November", "December"
};
const char * years_months_abbrev[] = {
  "Jan", "Feb", "Mar",
  "Apr", "May", "Jun",
  "Jul", "Aug", "Sep",
  "Oct", "Nov", "Dec"
};
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
	return 	( timeptr->tm_sec + timeptr->tm_min*SECS_PER_MIN + timeptr->tm_hour*SECS_PER_HOUR + timeptr->tm_yday*SECS_PER_DAY +
		(timeptr->tm_year-YEAR0_IN_UNIX_TIME)*SECS_PER_YEAR + ((timeptr->tm_year-(YEAR0_IN_UNIX_TIME-1))/4)*SECS_PER_DAY -
		((timeptr->tm_year-1)/100)*SECS_PER_DAY + ((timeptr->tm_year+299)/400)*SECS_PER_DAY );
}
time_t time (time_t* timer)
{
	time_t new_timer = mktime(get_time());
	if(timer!=NULL)
		*timer=new_timer;
	return new_timer;
}
struct tm * gmtime (const time_t * timer)
{
	struct tm * new_tm = kmalloc(sizeof(struct tm));
	return gmtime_r(timer, new_tm);
}
void print_time(struct tm * time)
{
	kprintf("%s the %dth of %s %d %d:%d:%d\n", week_days_abrv[time->tm_wday],time->tm_mday,years_months[time->tm_mon],time->tm_year+1900,time->tm_hour+2, time->tm_min,time->tm_sec );// add 1 hour to time for timezone another for summer/winter time
}
// NOTE I took the following code with minor changes from here
// 
// http://www.jbox.dk/sanos/source/lib/time.c.html
//
// time.c
//
// Time routines
//
// Copyright (C) 2002 Michael Ringgaard. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 
// 1. Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.  
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.  
// 3. Neither the name of the project nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
// SUCH DAMAGE.
// 

#define LEAPYEAR(year)          (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year)          (LEAPYEAR(year) ? 366 : 365)

const int _ytab[2][12] = {
  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

struct tm *gmtime_r(const time_t *timer, struct tm *tmbuf) 
{
	time_t time	= *timer;
	unsigned long dayclock, dayno;
	int year	= 1970;
	dayclock	= (unsigned long) time % SECS_PER_DAY;
	dayno		= (unsigned long) time / SECS_PER_DAY;
	tmbuf->tm_sec	= dayclock % SECS_PER_MIN;
	tmbuf->tm_min	= (dayclock % SECS_PER_HOUR) / SECS_PER_MIN;
	tmbuf->tm_hour	= dayclock / SECS_PER_HOUR;
	tmbuf->tm_wday	= (dayno + 4) % 7; // Day 0 was a thursday
	while (dayno >= (unsigned long) YEARSIZE(year)) {
		dayno -= YEARSIZE(year);
		year++;
	}
	tmbuf->tm_year	= year - 1900;
	tmbuf->tm_yday	= dayno+1;
	tmbuf->tm_mon = 0;
	while (dayno >= (unsigned long) _ytab[LEAPYEAR(year)][tmbuf->tm_mon]) {
		dayno -= _ytab[LEAPYEAR(year)][tmbuf->tm_mon];
		tmbuf->tm_mon++;
	}
	tmbuf->tm_mday	= dayno + 1;
	return tmbuf;
}
