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

#include "raylib.h"
#include "raygui.h"
#include "DebuggerNESDialog.h"

DebuggerNESDialog::DebuggerNESDialog(Font font, float fontSize, VideoGameDevice* device)
	: DebuggerDialog(font, fontSize, device)
{

}

void DebuggerNESDialog::Update() {

}

void DebuggerNESDialog::Draw(Rectangle dst) {
	if (!m_enabled)
		return;

	DrawRectangleRec(dst, ColorAlpha(BLACK, 0.90f));
	if (GuiButton(Rectangle{ 100, 100, 100, 48 }, "Close"))
		Close();
}