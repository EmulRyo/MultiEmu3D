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

#ifndef __DEBUGGER_NES_DIALOG_H__
#define __DEBUGGER_NES_DIALOG_H__

#include <map>
#include "../Common/Types.h"
#include "DebuggerDialog.h"

namespace Nes {
    class Debugger;
}

class DebuggerNESDialog: public DebuggerDialog {
public:
    DebuggerNESDialog(Font font, float fontSize, VideoGameDevice *device);
    ~DebuggerNESDialog();

    void Update() override;
    void Draw(Rectangle dst) override;
    void Show() override;
    
private:
    void Reset();
    void StepInto();
    void RunUntilNextLine();
    void RunOneFrame();
    void RunOneSecond();
    void SaveTiles();
    void UpdatePad();
    void InitDisassemblerVars(u16& currentAddress, u16& nextAddress, std::string& name, std::string& data, u16& pc);
    u16  HexTextTou16(char* text);

    void DrawButtons(Rectangle dst);
    void DrawRegisters(Rectangle dst);
    void DrawFlags(Rectangle dst);
    void DrawInput(Rectangle dst);
    void DrawDisassembler(Rectangle dst);
    void DrawDisassemblerIcon(Rectangle dst, u16 currentAddress, u16 pc);
    void DrawVideoRegisters(Rectangle dst);
    void DrawOtherRegisters(Rectangle dst);
    void DrawMemory(Rectangle dst);
    void DrawBreakpoints(Rectangle dst);
    void DrawKeyValue(float x, float& y, float w1, float w2, float h, const std::string& key, const std::string& value);
    void UpdatePrevValues();

    Font m_monoFont;
    Nes::Debugger* m_debugger;
    u16 m_disassemblerFirst;
    u16 m_disassemblerLast;
    bool m_dragging;
    Vector2 m_dialogOffset;
    Vector2 m_videoRegsScroll;
    Vector2 m_otherRegsScroll;
    bool m_inputs[8] = { 0 };
    char m_addressMem[5] = "0000";
    bool m_addressMemEditMode = false;
    int m_registersActive = 0;
    int m_memoryActive = 0;
    char m_breakpointsSelectedAddress[5] = "";
    bool m_brkptsSelAddEditMode = false;
    int m_breakpointsListViewActive = -1;
    int m_breakpointsListViewScrollIndex = -1;
    int m_breakpointsListViewFocus = -1;
    std::map<std::string, std::string> m_prevValues;
};

#endif
