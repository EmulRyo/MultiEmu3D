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

#ifndef __DEBUGGERNESDIALOG_H__
#define __DEBUGGERNESDIALOG_H__

#include <wx/wx.h>
#include <wx/listctrl.h>
#include "../Common/Types.h"

class wxListView;
class VideoGameDevice;
namespace Nes {
    class Debugger;
}

/*******************************************************************************
 // DebuggerDialog Class
 *******************************************************************************/

class DebuggerNESDialog : public wxDialog {
public:
	
    /**
     * Creates a new DebuggerDialog.
     */
    DebuggerNESDialog(wxWindow *parent, VideoGameDevice *device);
	~DebuggerNESDialog();
    
protected:
	DECLARE_EVENT_TABLE()
    
private:
    Nes::Debugger *m_debugger;
    wxListView *m_regsView;
    wxTextCtrl *m_addressMemCtrl;
    wxTextCtrl *m_memCtrl;
    wxListView *m_videoView;
    wxListView *m_othersView;
    wxListView *m_disassemblerView;
    wxRadioBox* m_memSelRBox;
    wxCheckBox *m_input[8];
    wxCheckBox *m_flags[8];
    wxFont* m_font;
    u16 m_disassemblerFirst;
    u16 m_disassemblerLast;
    
    wxSizer *CreateButtons();
    wxSizer *CreateFlagsAndInputControls();
    void UpdateUI();
    void UpdateRegisters();
    void UpdateMemory();
    void UpdateDisassembler();
    void InitDisassemblerVars(u16 &currentAddress, u16 &nextAddress, std::string &name, std::string &data, u16 &pc);
    void UpdateDisassemblerIcon(int numItem, u16 currentAddress, u16 pc);
    void UpdateVideoRegs();
    void UpdateOtherRegs();
    void UpdatePad();
    void UpdateFlags();
    void OnReset(wxCommandEvent &);
    void OnStepInto(wxCommandEvent &);
    void OnUntilNextLine(wxCommandEvent&);
    void OnOneFrame(wxCommandEvent &);
    void OnOneSecond(wxCommandEvent &);
    void OnBreakpoints(wxCommandEvent &);
    void OnMemAddressChange(wxCommandEvent &);
    void OnMemSelectChange(wxCommandEvent &);
    void OnSaveTiles(wxCommandEvent &);
    void OnActivated(wxListEvent &);
    
    std::string IntToString(int value, int width);
};

#endif
