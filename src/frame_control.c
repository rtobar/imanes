/*  ImaNES: I'm a NES. An intelligent NES emulator

    frame_control.c   -    Frame rate control for ImaNES

    Copyright (C) 2009   Rodrigo Tobar Carrizo

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <time.h>
#ifdef _MSC_VER
#include <Windows.h>
#endif

#include "frame_control.h"
#include "imaconfig.h"
#include "ppu.h"
#include "screen.h"

static int frames;
#ifndef _MSC_VER
static struct timespec startTime;
#else
static time_t secs;
static LARGE_INTEGER freq;
static LARGE_INTEGER startTime;
#endif

void start_timing() {

#ifndef _MSC_VER
	clock_gettime(CLOCK_REALTIME, &startTime);
#else
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&startTime);
	time(&secs);
#endif

	frames = 0;
}

void add_frame() {
	frames++;
}

void frame_sleep() {

#ifndef _MSC_VER
	static long tmp;
	static struct timespec endTime;
	struct timespec sleepTime = { 0, (long)2e7 };
#else
	static time_t tmp;
	static LARGE_INTEGER endTime;
	LARGE_INTEGER sleepTime;
#endif

	frames++;

	/* Calculate how much we should sleep for 50/60 FPS */
	/* For this, we calculate the next "start" time,    */
	/* and then we calculate the different between it   */
	/* the actual time                                  */

#ifndef _MSC_VER

	/* Check current time, see if we should display the fps */
	tmp = endTime.tv_sec;
	clock_gettime(CLOCK_REALTIME, &endTime);
	startTime.tv_nsec += (long)1.666666e7;

	if( startTime.tv_nsec > 1e9 ) {
		startTime.tv_sec++;
		startTime.tv_nsec -= (long)1e9;
	}

	if( endTime.tv_sec != tmp ) {
		show_fps(frames);
		frames = 0;
	}

	if( config.run_fast )
		return;

	/* Calculate the sleep time */
	sleepTime.tv_nsec = startTime.tv_nsec - endTime.tv_nsec;
	sleepTime.tv_sec  = startTime.tv_sec  - endTime.tv_sec;
	if( sleepTime.tv_nsec < 0 ) {
		sleepTime.tv_sec--;
		sleepTime.tv_nsec += (long)1e9;
	}

	/* We were on pause or in fast run */
	if( sleepTime.tv_sec > 0 || sleepTime.tv_nsec > 1.666666e7 ) {
		clock_gettime(CLOCK_REALTIME, &startTime);
		sleepTime.tv_sec = 0;
		sleepTime.tv_nsec = 0;
	}
	else
		nanosleep(&sleepTime, NULL);

#else
	tmp = secs;
	QueryPerformanceCounter(&endTime);
	time(&secs);
	startTime.QuadPart += (LONGLONG)(1.6666e-2 * freq.QuadPart);
	sleepTime.QuadPart = startTime.QuadPart - endTime.QuadPart;

	if( tmp != secs ) {
		show_fps(frames);
		frames = 0;
	}

	/* We were on pause or in fast run */
	if( sleepTime.QuadPart > (LONGLONG)(1.6666e-2 * freq.QuadPart) ) {
		QueryPerformanceCounter(&startTime);
		time(&secs);
		sleepTime.QuadPart = 0;
	}

	if( !config.run_fast && sleepTime.QuadPart > 0 )
		Sleep((DWORD)((double)sleepTime.QuadPart*1000/(double)freq.QuadPart));
#endif

}
