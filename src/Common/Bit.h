/*
 This file is part of MultiEmu3D.
 
 MultiEmu3D is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 MultiEmu3D is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with MultiEmu3D.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __BIT_H__
#define __BIT_H__

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
