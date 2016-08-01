//******************************************************************************
// Author: Teunis van Beelen
// Copyright (C) 2010, 2011, 2012 Teunis van Beelen
// teuniz@gmail.com
//!
//***************************************************************************
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//**************************************************************************
//
// This version of GPL is at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
//
//**************************************************************************
//! \file   hal\mcu\mingw\timer.c
//! \brief  System timer implementation for Windows and Linux platforms
//**************************************************************************
#include "timer.h"

void (*timer_func_handler_pntr)(void);

#ifdef __linux__

#include <sys/time.h>
#include <signal.h>

void timer_handler(int);

struct itimerval timervalue;

struct sigaction new_handler, old_handler;

void timer_sig_handler(int);



int timer_start(int mSec, void (*timer_func_handler)(void))
{
    timer_func_handler_pntr = timer_func_handler;

    timervalue.it_interval.tv_sec = mSec / 1000;
    timervalue.it_interval.tv_usec = (mSec % 1000) * 1000;
    timervalue.it_value.tv_sec = mSec / 1000;
    timervalue.it_value.tv_usec = (mSec % 1000) * 1000;
    if(setitimer(ITIMER_REAL, &timervalue, NULL))
    {
        printf("\nsetitimer() error\n");
        return(1);
    }

    new_handler.sa_handler = &timer_sig_handler;
    new_handler.sa_flags = SA_NOMASK;
    if(sigaction(SIGALRM, &new_handler, &old_handler))
    {
        printf("\nsigaction() error\n");
        return(1);
    }

    return(0);
}


void timer_sig_handler(int arg)
{
    timer_func_handler_pntr();
}


void timer_stop(void)
{
    timervalue.it_interval.tv_sec = 0;
    timervalue.it_interval.tv_usec = 0;
    timervalue.it_value.tv_sec = 0;
    timervalue.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &timervalue, NULL);

    sigaction(SIGALRM, &old_handler, NULL);
}

#else

/* this code only works on win2000, XP, Vista, 7 and up */
/* win95, win98 and ME are not supported                */
/* WINVER must have value 0x500 or higher               */
#ifndef WINVER
#define WINVER 0x500
#endif

#if WINVER < 0x500
#error "WINVER is < 0x500, cannot compile for old windows versions"
#endif

#include <windows.h>

HANDLE win_timer;

VOID CALLBACK timer_sig_handler(PVOID, BOOLEAN);


int timer_start(int mSec, void (*timer_func_handler)(void))
{
    timer_func_handler_pntr = timer_func_handler;

    if(CreateTimerQueueTimer(&win_timer, NULL, (WAITORTIMERCALLBACK)timer_sig_handler, NULL, mSec, mSec, WT_EXECUTEINTIMERTHREAD) == 0)
        return(1);

    return(0);
}


VOID CALLBACK timer_sig_handler(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
    (void) lpParameter;
    (void) TimerOrWaitFired;

    timer_func_handler_pntr();
}


void timer_stop(void)
{
    DeleteTimerQueueTimer(NULL, win_timer, NULL);
    CloseHandle(win_timer);
}

#endif
//******************************************************************************
// End of file
//******************************************************************************
