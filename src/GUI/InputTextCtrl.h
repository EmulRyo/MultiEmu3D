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

#ifndef __INPUTTEXTCTRL_H__
#define __INPUTTEXTCTRL_H__

#define NUM_KEYNAMES 1024

#include <wx/textctrl.h>

class InputTextCtrl: public wxTextCtrl
{
public:
	int keyCode;
	InputTextCtrl(wxWindow* parent, wxWindowID id);
	void OnChangeKey(int keyCode);
    
protected:
	DECLARE_EVENT_TABLE()
    
private:
	static wxString keyNames[NUM_KEYNAMES];
	static bool keyNamesInitialized;
	static void InitializeKeyNames();
	
	void OnKeyDown(wxKeyEvent& event);
	
};
#endif