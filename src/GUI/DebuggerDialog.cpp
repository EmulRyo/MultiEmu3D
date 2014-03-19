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
#include "BreakpointsDialog.h"
#include "../Debugger.h"

#include "Xpm/currentRow.xpm"
#include "Xpm/breakPoint.xpm"
#include "Xpm/currentBreak.xpm"

using namespace std;

BEGIN_EVENT_TABLE(DebuggerDialog, wxDialog)
EVT_BUTTON(ID_DEBUG_RESET, DebuggerDialog::OnReset)
EVT_BUTTON(ID_DEBUG_STEPINTO, DebuggerDialog::OnStepInto)
EVT_BUTTON(ID_DEBUG_ONEFRAME, DebuggerDialog::OnOneFrame)
EVT_BUTTON(ID_DEBUG_BREAKPOINTS, DebuggerDialog::OnBreakpoints)
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
    wxButton *breakpointsButton = new wxButton(this, ID_DEBUG_BREAKPOINTS, wxT("Breakpoints"));
    
    stepIntoButton->SetToolTip("Step Into (F7)");
#ifdef __WXMAC__
    resetButton->SetToolTip("Reset (Cmd+R)");
    oneFrameButton->SetToolTip("One frame (Cmd+O)");
    breakpointsButton->SetToolTip("Breakpoints (Cmd+B)");
#else
    resetButton->SetToolTip("Reset (Ctrl+R)");
    oneFrameButton->SetToolTip("One frame (Ctrl+O)");
    breakpointsButton->SetToolTip("Breakpoints (Ctrl+B)");
#endif
    
    
    wxAcceleratorEntry entries[4];
    entries[0].Set(wxACCEL_CTRL, (int) 'R', ID_DEBUG_RESET);
    entries[1].Set(wxACCEL_NORMAL, WXK_F7, ID_DEBUG_STEPINTO);
    entries[2].Set(wxACCEL_CTRL, (int) 'O', ID_DEBUG_ONEFRAME);
    entries[3].Set(wxACCEL_CTRL, (int) 'B', ID_DEBUG_BREAKPOINTS);
    wxAcceleratorTable accel(4, entries);
    SetAcceleratorTable(accel);
    
    wxStaticText *regsText = new wxStaticText(this, -1, wxT("Registers:"));
    
    m_font = new wxFont(12, wxTELETYPE, wxNORMAL, wxNORMAL);
    
    m_regsView = new wxListView(this, wxID_ANY, wxDefaultPosition, wxSize(80, 132), wxLC_REPORT);
    m_regsView->InsertColumn (0, "Reg");
    m_regsView->SetColumnWidth (0, 30);
    m_regsView->InsertColumn (1, "Value");
    m_regsView->SetColumnWidth (1, 40);
    
    // --- Dissassembler ---
    wxStaticText *disassemblerText = new wxStaticText(this, -1, wxT("Disassembler:"));
    
    m_disassemblerView = new wxListView(this, wxID_ANY, wxDefaultPosition, wxSize(294, 132), wxLC_REPORT);
    
    wxImageList *imageList = new wxImageList(16, 14);
    wxBitmap bmpCurrentRow(currentRow_xpm);
    imageList->Add(bmpCurrentRow);
    wxBitmap bmpBreakpoint(breakPoint_xpm);
    imageList->Add(bmpBreakpoint);
    wxBitmap bmpCurrentBreak(currentBreak_xpm);
    imageList->Add(bmpCurrentBreak);
    m_disassemblerView->SetImageList(imageList, wxIMAGE_LIST_SMALL);
    
    m_disassemblerView->InsertColumn (0, "");
    m_disassemblerView->SetColumnWidth (0, 24);
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
    buttonsSizer->AddSpacer(5);
    buttonsSizer->Add(breakpointsButton, 0, wxLEFT, 2);
    
    wxSizer *regsSizer = new wxBoxSizer(wxVERTICAL);
    regsSizer->Add(regsText, 0, wxBOTTOM, 5);
    regsSizer->Add(m_regsView);
    
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
    UpdateRegisters();
    
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

void DebuggerDialog::UpdateRegisters() {
    m_regsView->DeleteAllItems();
    
    m_regsView->InsertItem(0, "");
    m_regsView->SetItem(0, 0, "AF");
    m_regsView->SetItem(0, 1, m_debugger->GetRegAF());
    m_regsView->SetItemFont(0, *m_font);
    
    m_regsView->InsertItem(1, "");
    m_regsView->SetItem(1, 0, "BC");
    m_regsView->SetItem(1, 1, m_debugger->GetRegBC());
    m_regsView->SetItemFont(1, *m_font);
    
    m_regsView->InsertItem(2, "");
    m_regsView->SetItem(2, 0, "DE");
    m_regsView->SetItem(2, 1, m_debugger->GetRegDE());
    m_regsView->SetItemFont(2, *m_font);
    
    m_regsView->InsertItem(3, "");
    m_regsView->SetItem(3, 0, "HL");
    m_regsView->SetItem(3, 1, m_debugger->GetRegHL());
    m_regsView->SetItemFont(3, *m_font);
    
    m_regsView->InsertItem(4, "");
    m_regsView->SetItem(4, 0, "PC");
    m_regsView->SetItem(4, 1, m_debugger->GetRegPC());
    m_regsView->SetItemFont(4, *m_font);
    
    m_regsView->InsertItem(5, "");
    m_regsView->SetItem(5, 0, "SP");
    m_regsView->SetItem(5, 1, m_debugger->GetRegSP());
    m_regsView->SetItemFont(5, *m_font);
}

void DebuggerDialog::UpdateDissassembler() {
    WORD currentAddress, nextAddress;
    string address, name, data;
    
    m_debugger->DisassembleNext(currentAddress, nextAddress, name, data);
    
    m_disassemblerView->DeleteAllItems();
    for (int i=0; i<6; i++) {
        address = m_debugger->ToHex(currentAddress, 4, '0');
        
        m_disassemblerView->InsertItem(i, "");
        if (i == 0) {
            if (m_debugger->HasBreakpoint(currentAddress))
                m_disassemblerView->SetItemColumnImage(i, 0, 2);
            else
                m_disassemblerView->SetItemColumnImage(i, 0, 0);
        }
        else if (m_debugger->HasBreakpoint(currentAddress))
            m_disassemblerView->SetItemColumnImage(i, 0, 1);
            
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

void DebuggerDialog::OnBreakpoints(wxCommandEvent &event) {
    BreakpointsDialog breakpoints(this, m_debugger);
    breakpoints.ShowModal();
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
