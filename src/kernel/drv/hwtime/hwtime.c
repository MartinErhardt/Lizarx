/*  <src-path>/src/kernel/drv/hwtime/hwtime.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#include<drv/hwtime/hwtime.h>
#include<time.h>
#include<stdlib.h>
#include<string.h>
#include<cmos.h>
#include<dbg/console.h>

uint8_t days_per_month[12]=
{
	31,// january
	28,// february
	31,// march
	30,// april
	31,// may
	30,// juni
	31,// july
	31,// august
	30,// september
	31,// october
	30,// november
	31 // december
};
struct tm* get_time()
{
	uint8_t i =0;
	uint8_t yday=0;
	struct tm* new_time 	= malloc(sizeof(struct tm));
	new_time->tm_sec	= BCD_DECODE(cmos_read(0x00));
	new_time->tm_min	= BCD_DECODE(cmos_read(0x02));
	new_time->tm_hour	= BCD_DECODE(cmos_read(0x04));
	new_time->tm_wday	= 0;
	new_time->tm_mday	= BCD_DECODE(cmos_read(0x07));
	new_time->tm_mon	= BCD_DECODE(cmos_read(0x08))-1;
	new_time->tm_year	= BCD_DECODE(cmos_read(0x09)) + ((BCD_DECODE(cmos_read(0x32))*100)-1900);
	for(i=0;i<new_time->tm_mon;i++)
	{
		yday+=days_per_month[i];
	}
	new_time->tm_yday	= yday+ new_time->tm_mday-1 ;
	new_time->tm_isdst	= 0x0;
	return new_time;
}