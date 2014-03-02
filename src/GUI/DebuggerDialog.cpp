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
#include "DebuggerDialog.h"
#include "../Debugger.h"

using namespace std;

BEGIN_EVENT_TABLE(DebuggerDialog, wxDialog)
EVT_BUTTON(ID_DEBUG_RESET, DebuggerDialog::OnReset)
EVT_BUTTON(ID_DEBUG_STEPINTO, DebuggerDialog::OnStepInto)
EVT_BUTTON(ID_DEBUG_ONEFRAME, DebuggerDialog::OnOneFrame)
EVT_TEXT(ID_DEBUG_MEMADDRESS, DebuggerDialog::OnMemAddressChange)
END_EVENT_TABLE()

DebuggerDialog::DebuggerDialog(wxWindow *parent, Debugger *debugger)
{
    this->Create(parent, ID_DEBUGGERDIALOG, wxT("Debugger"), wxDefaultPosition,
           wxDefaultSize, wxDEFAULT_DIALOG_STYLE);
    
    m_debugger = debugger;
    
    wxButton *resetButton = new wxButton(this, ID_DEBUG_RESET, wxT("Reset"));
    wxButton *stepIntoButton = new wxButton(this, ID_DEBUG_STEPINTO, wxT("Step Into"));
    wxButton *oneFrameButton = new wxButton(this, ID_DEBUG_ONEFRAME, wxT("One frame"));
    resetButton->SetToolTip("Reset (Cmd/Ctrl+R)");
    stepIntoButton->SetToolTip("Step Into (F7)");
    oneFrameButton->SetToolTip("One frame (Cmd/Ctrl+O)");
    
    wxAcceleratorEntry entries[3];
    entries[0].Set(wxACCEL_CTRL, (int) 'R', ID_DEBUG_RESET);
    entries[1].Set(wxACCEL_NORMAL, WXK_F7, ID_DEBUG_STEPINTO);
    entries[2].Set(wxACCEL_CTRL, (int) 'O', ID_DEBUG_ONEFRAME);
    wxAcceleratorTable accel(3, entries);
    SetAcceleratorTable(accel);
    
    wxStaticText *regsText = new wxStaticText(this, -1, wxT("Registers:"));
    
    wxFont* tmpFont = new wxFont(12, wxTELETYPE, wxNORMAL, wxNORMAL);
    
    m_regsCtrl = new wxTextCtrl(this, -1, wxEmptyString, wxDefaultPosition, wxSize(80, 90), wxTE_MULTILINE | wxTE_READONLY);
    m_regsCtrl->SetFont(*tmpFont);
    
    wxStaticText *disassemblerText = new wxStaticText(this, -1, wxT("Disassembler:"));
    
    m_disassemblerCtrl = new wxTextCtrl(this, -1, wxEmptyString, wxDefaultPosition, wxSize(200, 90), wxTE_MULTILINE | wxTE_READONLY);
    m_disassemblerCtrl->SetFont(*tmpFont);
    
    wxTextValidator *validator = new wxTextValidator(wxFILTER_INCLUDE_CHAR_LIST);
    validator->SetCharIncludes(wxT("0123456789ABCDEFabcdef"));
    
    wxStaticText *memText = new wxStaticText(this, -1, wxT("Memory:"));
    m_addressMemCtrl = new wxTextCtrl(this, ID_DEBUG_MEMADDRESS, wxEmptyString, wxDefaultPosition, wxSize(40, 20), 0, *validator);
    m_addressMemCtrl->SetFont(*tmpFont);
    m_addressMemCtrl->SetValue(wxT("0000"));
    m_addressMemCtrl->SetMaxLength(4);
    
    m_memCtrl = new wxTextCtrl(this, -1, wxEmptyString, wxDefaultPosition, wxSize(420, 90), wxTE_MULTILINE | wxTE_READONLY);
    m_memCtrl->SetFont(*tmpFont);
    
    wxSizer *buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonsSizer->Add(resetButton);
    buttonsSizer->AddSpacer(5);
    buttonsSizer->Add(stepIntoButton, 0, wxLEFT, 2);
    buttonsSizer->Add(oneFrameButton, 0, wxLEFT, 2);
    
    wxSizer *regsSizer = new wxBoxSizer(wxVERTICAL);
    regsSizer->Add(regsText, 0, wxBOTTOM, 5);
    regsSizer->Add(m_regsCtrl);
    
    wxSizer *disassemblerSizer = new wxBoxSizer(wxVERTICAL);
    disassemblerSizer->Add(disassemblerText, 0, wxBOTTOM, 5);
    disassemblerSizer->Add(m_disassemblerCtrl);
    
    wxSizer *horizontalSizer = new wxBoxSizer(wxHORIZONTAL);
    horizontalSizer->Add(regsSizer);
    horizontalSizer->Add(disassemblerSizer, 0, wxLEFT, 5);
    
    wxSizer *memSizer = new wxBoxSizer(wxVERTICAL);
    memSizer->Add(memText, 0, wxBOTTOM, 5);
    memSizer->Add(m_addressMemCtrl, 0, wxBOTTOM, 5);
    memSizer->Add(m_memCtrl);
    
    wxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(buttonsSizer, 0, wxALL, 5);
    mainSizer->Add(horizontalSizer, 0, wxALL, 5);
    mainSizer->Add(memSizer, 0, wxALL, 5);
    
    SetSizerAndFit(mainSizer);
    
    UpdateUI();
}

DebuggerDialog::~DebuggerDialog()
{
	
}

void DebuggerDialog::UpdateUI() {
    m_regsCtrl->SetValue(m_debugger->GetRegs());
    
    wxString address = m_addressMemCtrl->GetValue();
    long value;
    if(address.ToLong(&value, 16)) {
        value = value & 0xFFF0;
        if (value > 0xFFA0)
            value = 0xFFA0;
        m_memCtrl->SetValue(m_debugger->GetMem(value, (value + 0x5F)));
    }
    
    m_disassemblerCtrl->SetValue(m_debugger->Disassemble(6));
}

void DebuggerDialog::OnReset(wxCommandEvent &event) {
    m_debugger->Reset();
    UpdateUI();
}

void DebuggerDialog::OnStepInto(wxCommandEvent &event) {
    m_debugger->StepInto();
    UpdateUI();
}

void DebuggerDialog::OnOneFrame(wxCommandEvent &event) {
    m_debugger->ExecuteOneFrame();
    UpdateUI();
}

void DebuggerDialog::OnMemAddressChange(wxCommandEvent &event) {
    if(!m_addressMemCtrl->IsModified())
        return;
    
    long insertionPoint = m_addressMemCtrl->GetInsertionPoint();
    wxString address = m_addressMemCtrl->GetValue().Upper();
    m_addressMemCtrl->ChangeValue(address);
    m_addressMemCtrl->SetInsertionPoint(insertionPoint);
    UpdateUI();
}
