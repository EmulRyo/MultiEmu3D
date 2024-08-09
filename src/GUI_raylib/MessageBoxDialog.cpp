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

#include "raygui.h"
#include "Localization.h"
#include "MessageBoxDialog.h"


MessageBoxDialog::MessageBoxDialog(const std::string& message)
    : m_message(message)
{

}

void MessageBoxDialog::Update() {

}

void MessageBoxDialog::Draw(Rectangle dst) {
    if (!m_enabled)
        return;

    GuiEnable();
    DrawRectangleRec(Rectangle { dst.x, dst.y + 24 * 2, dst.width, dst.height - 24 * 3 }, ColorAlpha(BLACK, 0.7f));
    Font font = GuiGetFont();
    Vector2 fontSize = MeasureTextEx(font, m_message.c_str(), (float)font.baseSize, 1);
    float w = fontSize.x + 20;
    float h = fontSize.y + 24 * 3;
    int result = GuiMessageBox(Rectangle{ (dst.width - w) / 2.0f, (dst.height - h) / 2.0f, w, h }, _("Error"), m_message.c_str(), _("OK"));
    if (result >= 0) {
        Close();
    }
}
