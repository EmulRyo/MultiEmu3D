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

#ifndef __ISCREENDRAWABLE_H__
#define __ISCREENDRAWABLE_H__

#include "Types.h"
    
class IScreenDrawable {
public:
    virtual u8*  GetBufferPtr() = 0;
    virtual void OnDrawPixel(int idColor, int x, int y) = 0;
    virtual void OnDrawPixel(u8 r, u8 g, u8 b, int x, int y) = 0;
    virtual void OnSizeChanged(int x, int y, int width, int height) = 0;
    virtual void OnRefreshEmulatedScreen() = 0;
    virtual void OnClear() = 0;
};

#endif