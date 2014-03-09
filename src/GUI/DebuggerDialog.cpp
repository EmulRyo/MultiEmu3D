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

#include <wx/listctrl.h>
#include <wx/imaglist.h>
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
    
    m_font = new wxFont(12, wxTELETYPE, wxNORMAL, wxNORMAL);
    
    m_regsCtrl = new wxTextCtrl(this, -1, wxEmptyString, wxDefaultPosition, wxSize(80, 90), wxTE_MULTILINE | wxTE_READONLY);
    m_regsCtrl->SetFont(*m_font);
    
    // --- Dissassembler ---
    wxStaticText *disassemblerText = new wxStaticText(this, -1, wxT("Disassembler:"));
    
    m_disassemblerView = new wxListView(this, wxID_ANY, wxDefaultPosition, wxSize(294, 132), wxLC_REPORT);
    
    m_disassemblerView->InsertColumn (0, "");
    m_disassemblerView->SetColumnWidth (0, 20);
    m_disassemblerView->InsertColumn (1, "Address");
    m_disassemblerView->SetColumnWidth (1, 52);
    m_disassemblerView->InsertColumn (2, "Name");
    m_disassemblerView->SetColumnWidth (2, 120);
    m_disassemblerView->InsertColumn (3, "Data");
    m_disassemblerView->SetColumnWidth (3, 80);
    
    wxTextValidator *validator = new wxTextValidator(wxFILTER_INCLUDE_CHAR_LIST);
    validator->SetCharIncludes(wxT("0123456789ABCDEFabcdef"));
    
    wxStaticText *memText = new wxStaticText(this, -1, wxT("Memory:"));
    m_addressMemCtrl = new wxTextCtrl(this, ID_DEBUG_MEMADDRESS, wxEmptyString, wxDefaultPosition, wxSize(40, 20), 0, *validator);
    m_addressMemCtrl->SetFont(*m_font);
    m_addressMemCtrl->SetValue(wxT("0000"));
    m_addressMemCtrl->SetMaxLength(4);
    
    m_memCtrl = new wxTextCtrl(this, -1, wxEmptyString, wxDefaultPosition, wxSize(420, 90), wxTE_MULTILINE | wxTE_READONLY);
    m_memCtrl->SetFont(*m_font);
    
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
    disassemblerSizer->Add(m_disassemblerView);
    
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
    
    UpdateDissassembler();
}

void DebuggerDialog::UpdateDissassembler() {
    WORD currentAddress, nextAddress;
    string address, name, data;
    
    m_debugger->DisassembleNext(currentAddress, nextAddress, name, data);
    
    m_disassemblerView->DeleteAllItems();
    for (int i=0; i<6; i++) {
        address = m_debugger->ToHex(currentAddress, 4, '0');
        
        m_disassemblerView->InsertItem(i, "");
        //m_disassemblerView->SetItemColumnImage(i, 0, 0);
        m_disassemblerView->SetItem(i, 1, address);
        m_disassemblerView->SetItem(i, 2, name);
        m_disassemblerView->SetItem(i, 3, data);
        m_disassemblerView->SetItemFont(i, *m_font);
        
        if (nextAddress < currentAddress)
            break;
        else {
            currentAddress = nextAddress;
            m_debugger->DisassembleOne(currentAddress, nextAddress, name, data);
        }
    }
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
