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

#ifndef __DIALOG_H__
#define __DIALOG_H__

#include "raylib.h"

class Dialog {
public:
    Dialog(Font font, float fontSize) : m_font(font), m_fontSize(fontSize) {}
    virtual void Update() = 0;
    virtual void Draw(Rectangle dst) = 0;

    virtual void Show() { m_enabled = true; }
    virtual void Close() { m_enabled = false; }
    virtual bool IsEnabled() const { return m_enabled; }

protected:
    Font m_font;
    float m_fontSize;
    bool m_enabled = false;
};

#endif
