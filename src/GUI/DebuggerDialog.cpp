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

#include "IDControls.h"
#include "Xpm/play.xpm"
#include "DebuggerDialog.h"
#include "../Debugger.h"

using namespace std;

BEGIN_EVENT_TABLE(DebuggerDialog, wxDialog)
EVT_MENU(ID_STEP, DebuggerDialog::OnStep)
EVT_MENU(ID_ONEFRAME, DebuggerDialog::OnOneFrame)
END_EVENT_TABLE()

DebuggerDialog::DebuggerDialog(wxWindow *parent, Debugger *debugger)
{
    this->Create(parent, ID_DEBUGGERDIALOG, wxT("Debugger"), wxDefaultPosition,
           wxDefaultSize, wxDEFAULT_DIALOG_STYLE);
    
    m_debugger = debugger;
    
    m_mainSizer = new wxBoxSizer(wxVERTICAL);
    
    CreateToolBar();
    m_regsCtrl = new wxTextCtrl(this, -1, wxEmptyString, wxDefaultPosition, wxSize(640, 480), wxTE_MULTILINE | wxTE_READONLY);
    wxFont* tmpFont = new wxFont(12, wxTELETYPE, wxNORMAL, wxNORMAL);
    m_regsCtrl->SetFont(*tmpFont);
    m_regsCtrl->SetValue(m_debugger->GetRegs());
    m_mainSizer->Add(m_regsCtrl, 0, wxEXPAND);
    
    SetSizerAndFit(m_mainSizer);
}

DebuggerDialog::~DebuggerDialog()
{
	
}

void DebuggerDialog::CreateToolBar()
{
    wxToolBar *toolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL | wxTB_TEXT | wxTB_NOICONS);
    
    wxBitmap bmpPlay(play_xpm);
	toolBar->AddTool(ID_STEP, _("Step"), bmpPlay);
    toolBar->AddTool(ID_ONEFRAME, _("One frame"), bmpPlay);
    
    toolBar->Realize();
    m_mainSizer->Add(toolBar, 0, wxEXPAND);
}

void DebuggerDialog::OnStep(wxCommandEvent &event) {
    m_debugger->Step();
    m_regsCtrl->SetValue(m_debugger->GetRegs());
}

void DebuggerDialog::OnOneFrame(wxCommandEvent &event) {
    m_debugger->ExecuteOneFrame();
    m_regsCtrl->SetValue(m_debugger->GetRegs());
}