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

#ifndef __BREAKPOINTSDIALOG_H__
#define __BREAKPOINTSDIALOG_H__

#include <wx/wx.h>

class wxListView;

class Debuggable;

/*******************************************************************************
 // BreakpointsDialog Class
 *******************************************************************************/

class BreakpointsDialog : public wxDialog {
public:
	
    BreakpointsDialog(wxWindow *parent, Debuggable *debugger);
	~BreakpointsDialog();
    
protected:
	DECLARE_EVENT_TABLE()
    
private:
    Debuggable *m_debugger;
    wxFont *m_font;
    wxListView *m_breakpointsView;
    wxTextCtrl *m_addressCtrl;
    
    void UpdateUI();
    
    void OnItemSelected(wxListEvent &);
    void OnAddBreakpoint(wxCommandEvent &);
    void OnDelBreakpoint(wxCommandEvent &);
};

#endif
