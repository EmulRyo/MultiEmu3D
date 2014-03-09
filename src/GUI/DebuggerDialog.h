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

#ifndef __DEBUGGERDIALOG_H__
#define __DEBUGGERDIALOG_H__

#include <wx/wx.h>

class Debugger;
class wxListView;

/*******************************************************************************
 // DebuggerDialog Class
 *******************************************************************************/

class DebuggerDialog : public wxDialog {
public:
	
    /**
     * Creates a new DebuggerDialog.
     */
    DebuggerDialog(wxWindow *parent, Debugger *debugger);
	~DebuggerDialog();
    
protected:
	DECLARE_EVENT_TABLE()
    
private:
    Debugger *m_debugger;
    wxTextCtrl *m_regsCtrl;
    wxTextCtrl *m_addressMemCtrl;
    wxTextCtrl *m_memCtrl;
    wxListView *m_disassemblerView;
    wxFont* m_font;
    
    void CreateToolBar();
    void UpdateUI();
    void UpdateDissassembler();
    void OnReset(wxCommandEvent &);
    void OnStepInto(wxCommandEvent &);
    void OnOneFrame(wxCommandEvent &);
    void OnMemAddressChange(wxCommandEvent &);
};

#endif
