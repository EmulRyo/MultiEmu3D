/*
 This file is part of DMGBoy.
 
 DMGBoy is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 DMGBoy is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with DMGBoy.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __DEF_H__
#define __DEF_H__

//#define MAKEGBLOG

#define APP_NAME		"MarkSystem"
#define APP_VERSION		"0.1"
#define APP_MAINT		"Pablo Gascó"
#define APP_LICENCE		"GPL v3"
#define APP_COPYRIGTH	"(C) 2014 Pablo Gasco"
#define APP_WEBSITE		"http://code.google.com/p/marksystem/"

#define SAVE_STATE_VERSION	0x01

typedef unsigned char    u8;
typedef unsigned short  u16;
typedef unsigned int    u32;
typedef unsigned long   u64;
typedef char             s8;
typedef short           s16;
typedef int             s32;
typedef long            s64;

#define SMS_SCREEN_W 256
#define SMS_SCREEN_H 224

#define FRAME_CYCLES    59719

#define BIT0(value)	((value) & 0x01)
#define BIT1(value)	((value) & 0x02)
#define BIT2(value)	((value) & 0x04)
#define BIT3(value)	((value) & 0x08)
#define BIT4(value)	((value) & 0x10)
#define BIT5(value)	((value) & 0x20)
#define BIT6(value)	((value) & 0x40)
#define BIT7(value)	((value) & 0x80)

#define BITS01(value)	((value) & 0x03)
#define BITS23(value)	((value) & 0x0C)
#define BITS45(value)	((value) & 0x30)
#define BITS67(value)	((value) & 0xC0)

#endif
