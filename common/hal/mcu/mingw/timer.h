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
//! \addtogroup mingwhal
//! @{
//**************************************************************************
//! \file   hal\mcu\mingw\timer.h
//! \brief  System timer implementation for Windows and Linux platforms
//**************************************************************************
#ifndef TIMER_INCLUDED_H
#define TIMER_INCLUDED_H

#ifdef __cplusplus
extern "C" {
#endif

int timer_start(int, void (*)(void));

void timer_stop(void);

#ifdef __cplusplus
}
#endif

#endif // TIMER_INCLUDED_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************
