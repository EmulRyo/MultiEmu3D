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

#include <iomanip>
#include <sstream>
#include <wx/listctrl.h>
#include <wx/imaglist.h>
#include "IDControls.h"
#include "DebuggerNESDialog.h"
#include "BreakpointsDialog.h"
#include "../NES/NES.h"
#include "../NES/Debugger.h"

#include "Xpm/currentRow.xpm"
#include "Xpm/breakPoint.xpm"
#include "Xpm/currentBreak.xpm"

using namespace std;
using namespace Nes;

BEGIN_EVENT_TABLE(DebuggerNESDialog, wxDialog)
EVT_BUTTON(ID_DEBUG_RESET, DebuggerNESDialog::OnReset)
EVT_BUTTON(ID_DEBUG_STEPINTO, DebuggerNESDialog::OnStepInto)
EVT_BUTTON(ID_DEBUG_ONEFRAME, DebuggerNESDialog::OnOneFrame)
EVT_BUTTON(ID_DEBUG_ONESECOND, DebuggerNESDialog::OnOneSecond)
EVT_BUTTON(ID_DEBUG_BREAKPOINTS, DebuggerNESDialog::OnBreakpoints)
EVT_BUTTON(ID_DEBUG_SAVETILES, DebuggerNESDialog::OnSaveTiles)
EVT_TEXT(ID_DEBUG_MEMADDRESS, DebuggerNESDialog::OnMemAddressChange)
EVT_LIST_ITEM_ACTIVATED(ID_DEBUG_DISASSEMBLER, DebuggerNESDialog::OnActivated)
EVT_RADIOBOX(ID_DEBUG_MEMSELECT, DebuggerNESDialog::OnMemSelectChange)
END_EVENT_TABLE()

DebuggerNESDialog::DebuggerNESDialog(wxWindow *parent, VideoGameDevice *device)
{
    this->Create(parent, ID_DEBUGGERDIALOG, wxT("Debugger"), wxDefaultPosition,
           wxDefaultSize, wxDEFAULT_DIALOG_STYLE);
    
    Nes::NES *nes = (Nes::NES *)device;
    m_debugger = (Nes::Debugger *)nes->GetDebugger();
    
    wxStaticText *regsText = new wxStaticText(this, -1, wxT("Registers:"));
    
#ifdef __WXMSW__
	m_font = new wxFont(8, wxTELETYPE, wxNORMAL, wxNORMAL);
    int height1 = 152;
    int height2 = 360;
#endif
#ifdef __WXGTK__
	m_font = new wxFont(8, wxTELETYPE, wxNORMAL, wxNORMAL);
    int height1 = 166;
    int height2 = 420;
#endif
#ifdef __WXMAC__
    m_font = new wxFont(12, wxTELETYPE, wxNORMAL, wxNORMAL);
    int height1 = 170;
    int height2 = 426;
#endif
    
    m_regsView = new wxListView(this, wxID_ANY, wxDefaultPosition, wxSize(82, height1), wxLC_REPORT);
    m_regsView->InsertColumn (0, "Name");
    m_regsView->SetColumnWidth (0, 41);
    m_regsView->InsertColumn (1, "Value");
    m_regsView->SetColumnWidth (1, 41);
    
    // --- Dissassembler ---
    wxStaticText *disassemblerText = new wxStaticText(this, -1, wxT("Disassembler:"));
    
    m_disassemblerView = new wxListView(this, ID_DEBUG_DISASSEMBLER, wxDefaultPosition, wxSize(298, height1), wxLC_REPORT);
    
    wxImageList *imageList = new wxImageList(16, 14);
    wxBitmap bmpCurrentRow(currentRow_xpm);
    imageList->Add(bmpCurrentRow);
    wxBitmap bmpBreakpoint(breakPoint_xpm);
    imageList->Add(bmpBreakpoint);
    wxBitmap bmpCurrentBreak(currentBreak_xpm);
    imageList->Add(bmpCurrentBreak);
    m_disassemblerView->SetImageList(imageList, wxIMAGE_LIST_SMALL);
    
    m_disassemblerView->InsertColumn (0, "");
    m_disassemblerView->SetColumnWidth (0, 26);
    m_disassemblerView->InsertColumn (1, "Address");
    m_disassemblerView->SetColumnWidth (1, 52);
    m_disassemblerView->InsertColumn (2, "Name");
    m_disassemblerView->SetColumnWidth (2, 130);
    m_disassemblerView->InsertColumn (3, "Data");
    m_disassemblerView->SetColumnWidth (3, 90);
    
    m_disassemblerFirst = 0;
    m_disassemblerLast  = 0;
    
    wxStaticText *videoText = new wxStaticText(this, -1, wxT("Video registers:"));
    m_videoView = new wxListView(this, wxID_ANY, wxDefaultPosition, wxSize(110, height2), wxLC_REPORT);
    m_videoView->InsertColumn (0, "Name");
    m_videoView->SetColumnWidth (0, 60);
    m_videoView->InsertColumn (1, "Value");
    m_videoView->SetColumnWidth (1, 38);
    
    wxStaticText *othersText = new wxStaticText(this, -1, wxT("Other registers:"));
    m_othersView = new wxListView(this, wxID_ANY, wxDefaultPosition, wxSize(110, height2), wxLC_REPORT);
    m_othersView->InsertColumn (0, "Name");
    m_othersView->SetColumnWidth (0, 72);
    m_othersView->InsertColumn (1, "Value");
    m_othersView->SetColumnWidth (1, 38);
    
    wxTextValidator *validator = new wxTextValidator(wxFILTER_INCLUDE_CHAR_LIST);
    validator->SetCharIncludes(wxT("0123456789ABCDEFabcdef"));
    
    wxStaticText *memText = new wxStaticText(this, -1, wxT("Memory:"));
    
    wxString choices[2];
    choices[0] = "Z80";
    choices[1] = "VRAM";
    m_memSelRBox = new wxRadioBox(this, ID_DEBUG_MEMSELECT, wxT(""), wxDefaultPosition, wxDefaultSize, 2, choices, 1, wxRA_SPECIFY_ROWS);
    
    m_addressMemCtrl = new wxTextCtrl(this, ID_DEBUG_MEMADDRESS, wxEmptyString, wxDefaultPosition, wxSize(40, 20), 0, *validator);
    m_addressMemCtrl->SetFont(*m_font);
    m_addressMemCtrl->SetValue(wxT("0000"));
    m_addressMemCtrl->SetMaxLength(4);
    
    m_memCtrl = new wxTextCtrl(this, -1, wxEmptyString, wxDefaultPosition, wxSize(411, 146), wxTE_MULTILINE | wxTE_READONLY);
    m_memCtrl->SetFont(*m_font);
    
    wxSizer *buttonsSizer = CreateButtons();
    
    wxSizer *regsSizer = new wxBoxSizer(wxVERTICAL);
    regsSizer->Add(regsText, 0, wxBOTTOM, 5);
    regsSizer->Add(m_regsView);
    
    wxSizer *disassemblerSizer = new wxBoxSizer(wxVERTICAL);
    disassemblerSizer->Add(disassemblerText, 0, wxBOTTOM, 5);
    disassemblerSizer->Add(m_disassemblerView);
    
    wxSizer *RegsPlusDisSizer = new wxBoxSizer(wxHORIZONTAL);
    RegsPlusDisSizer->Add(regsSizer);
    RegsPlusDisSizer->AddStretchSpacer();
    RegsPlusDisSizer->Add(disassemblerSizer);
    
    wxSizer *addressAndChoiceSizer = new wxBoxSizer(wxHORIZONTAL);
    addressAndChoiceSizer->Add(m_addressMemCtrl, 0, wxUP, 10);
    addressAndChoiceSizer->AddSpacer(80);
    addressAndChoiceSizer->Add(m_memSelRBox, 0, wxRIGHT, 5);
    
    wxSizer *memSizer = new wxBoxSizer(wxVERTICAL);
    memSizer->Add(memText);
    memSizer->Add(addressAndChoiceSizer);
    memSizer->Add(m_memCtrl);
    
    wxSizer *flagsAndInputSizer = CreateFlagsAndInputControls();
    
    wxSizer *leftSizer = new wxBoxSizer(wxVERTICAL);
    leftSizer->Add(RegsPlusDisSizer, 0, wxEXPAND);
    leftSizer->Add(flagsAndInputSizer, 0, wxEXPAND|wxTOP, 5);
    leftSizer->AddStretchSpacer();
    leftSizer->Add(memSizer, 0, wxTOP, 10);
    
    wxSizer *videoSizer = new wxBoxSizer(wxVERTICAL);
    videoSizer->Add(videoText, 0, wxBOTTOM, 5);
    videoSizer->Add(m_videoView);
    
    wxSizer *othersSizer = new wxBoxSizer(wxVERTICAL);
    othersSizer->Add(othersText, 0, wxBOTTOM, 5);
    othersSizer->Add(m_othersView);
    
    wxSizer *hSizer = new wxBoxSizer(wxHORIZONTAL);
    hSizer->Add(leftSizer, 0, wxEXPAND);
    hSizer->Add(videoSizer, 0, wxLEFT, 10);
    hSizer->Add(othersSizer, 0, wxLEFT, 10);
    
    wxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(buttonsSizer, 0, wxEXPAND|wxALL, 10);
    mainSizer->Add(hSizer, 0, wxALL, 10);
    
    SetSizerAndFit(mainSizer);
    
    UpdateUI();
}

DebuggerNESDialog::~DebuggerNESDialog()
{
	
}

wxSizer * DebuggerNESDialog::CreateButtons() {
    wxButton *resetButton = new wxButton(this, ID_DEBUG_RESET, wxT("Reset"));
    wxButton *stepIntoButton = new wxButton(this, ID_DEBUG_STEPINTO, wxT("Step Into"));
    wxButton *oneFrameButton = new wxButton(this, ID_DEBUG_ONEFRAME, wxT("Run one frame"));
    wxButton *oneSecondButton = new wxButton(this, ID_DEBUG_ONESECOND, wxT("Run one second"));
    wxButton *breakpointsButton = new wxButton(this, ID_DEBUG_BREAKPOINTS, wxT("Breakpoints"));
    wxButton *saveTilesButton = new wxButton(this, ID_DEBUG_SAVETILES, wxT("Save tiles"));
    
    stepIntoButton->SetToolTip("Step Into (F7)");
    saveTilesButton->SetToolTip("Save tiles as tiles.bmp");
#ifdef __WXMAC__
    resetButton->SetToolTip("Reset (Cmd+R)");
    oneFrameButton->SetToolTip("Run one frame (Cmd+O)");
    oneSecondButton->SetToolTip("Run one second (F9)");
    breakpointsButton->SetToolTip("Breakpoints (Cmd+B)");
#else
    resetButton->SetToolTip("Reset (Ctrl+R)");
    oneFrameButton->SetToolTip("Run one frame (Ctrl+O)");
    oneSecondButton->SetToolTip("Run one second (F9)");
    breakpointsButton->SetToolTip("Breakpoints (Ctrl+B)");
#endif
    
    wxAcceleratorEntry entries[5];
    entries[0].Set(wxACCEL_CTRL, (int) 'R', ID_DEBUG_RESET);
    entries[1].Set(wxACCEL_NORMAL, WXK_F7, ID_DEBUG_STEPINTO);
    entries[2].Set(wxACCEL_CTRL, (int) 'O', ID_DEBUG_ONEFRAME);
    entries[3].Set(wxACCEL_NORMAL, WXK_F9, ID_DEBUG_ONESECOND);
    entries[4].Set(wxACCEL_CTRL, (int) 'B', ID_DEBUG_BREAKPOINTS);
    wxAcceleratorTable accel(5, entries);
    SetAcceleratorTable(accel);
    
    wxSizer *buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonsSizer->Add(resetButton);
    buttonsSizer->AddSpacer(5);
    buttonsSizer->Add(stepIntoButton, 0, wxLEFT, 2);
    buttonsSizer->Add(oneFrameButton, 0, wxLEFT, 2);
    buttonsSizer->Add(oneSecondButton, 0, wxLEFT, 2);
    buttonsSizer->AddSpacer(5);
    buttonsSizer->Add(saveTilesButton, 0, wxLEFT, 2);
    buttonsSizer->AddStretchSpacer();
    buttonsSizer->Add(breakpointsButton);
    
    return buttonsSizer;
}

wxSizer * DebuggerNESDialog::CreateFlagsAndInputControls() {
    wxStaticText *flagsText = new wxStaticText(this, -1, "Flags: ");
    m_flags[0] = new wxCheckBox(this, wxID_ANY, "C");
    m_flags[1] = new wxCheckBox(this, wxID_ANY, "Z");
    m_flags[2] = new wxCheckBox(this, wxID_ANY, "I");
    m_flags[3] = new wxCheckBox(this, wxID_ANY, "D");
    m_flags[4] = new wxCheckBox(this, wxID_ANY, "B");
    m_flags[5] = new wxCheckBox(this, wxID_ANY, "B");
    m_flags[6] = new wxCheckBox(this, wxID_ANY, "V");
    m_flags[7] = new wxCheckBox(this, wxID_ANY, "N");
    
    wxStaticText *inputText = new wxStaticText(this, -1, wxT("Input:"));
    m_input[0] = new wxCheckBox(this, wxID_ANY, "U");
    m_input[1] = new wxCheckBox(this, wxID_ANY, "D");
    m_input[2] = new wxCheckBox(this, wxID_ANY, "L");
    m_input[3] = new wxCheckBox(this, wxID_ANY, "R");
    m_input[4] = new wxCheckBox(this, wxID_ANY, "1");
    m_input[5] = new wxCheckBox(this, wxID_ANY, "2");
    
    wxFlexGridSizer *grid = new wxFlexGridSizer(9, 2, 12);
    grid->SetFlexibleDirection(wxHORIZONTAL);
    grid->Add(flagsText);
    for (int i=0; i<8; i++) {
        m_flags[i]->Disable();
        grid->Add(m_flags[i]);
    }
    grid->Add(inputText);
    for (int i=0; i<6; i++)
        grid->Add(m_input[i]);
    
    return grid;
}

std::string DebuggerNESDialog::IntToString(int value, int width)
{
    std::stringstream ss;
    ss << setfill('0') << setw(width) << value;
    return ss.str();
}


void DebuggerNESDialog::UpdateUI() {
    UpdateRegisters();
    UpdateMemory();
    UpdateDisassembler();
    UpdateVideoRegs();
    UpdateOtherRegs();
    UpdateFlags();
}

void DebuggerNESDialog::UpdateMemory() {
    int memSelect = m_memSelRBox->GetSelection();
    int maxMem = (memSelect == 0) ? 0x10000 : 0x4000;
    wxString address = m_addressMemCtrl->GetValue();
    long value;
    if(address.ToLong(&value, 16)) {
        value = value & 0xFFF0;
        int numLines = 9;
        u16 maxStart = maxMem - (0x10*numLines);
        if (value > maxStart)
            value = maxStart;
        string mem = "      00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n";
        mem += m_debugger->GetMem(value, (value + (0x10*numLines)-1));
        m_memCtrl->SetValue(mem);
    }
}

void DebuggerNESDialog::UpdateRegisters() {
    const char *names[] = { "A", "X", "Y", "P", "PC", "S" };
    
    m_regsView->DeleteAllItems();
    
    for (int i=0; i<6; i++) {
        m_regsView->InsertItem(i, "");
        m_regsView->SetItem(i, 0, names[i]);
        m_regsView->SetItemFont(i, *m_font);
    }
    
    m_regsView->SetItem(0, 1, m_debugger->GetRegA());
    m_regsView->SetItem(1, 1, m_debugger->GetRegX());
    m_regsView->SetItem(2, 1, m_debugger->GetRegY());
    m_regsView->SetItem(3, 1, m_debugger->GetRegP());
    m_regsView->SetItem(4, 1, m_debugger->GetRegPC());
    m_regsView->SetItem(5, 1, m_debugger->GetRegS());
}

void DebuggerNESDialog::UpdateVideoRegs() {
    /*
    const char *names[] = { "Status", "IE0", "IE1", "IPeriod", "Line", "Address" };
    int pos = 0;
    
    m_videoView->DeleteAllItems();
    
    for (int i=0; i<6; i++) {
        m_videoView->InsertItem(pos, "");
        m_videoView->SetItem(pos, 0, names[pos]);
        m_videoView->SetItemFont(pos, *m_font);
        pos++;
    }
    m_videoView->SetItem(0, 1, m_debugger->ToHex(m_debugger->GetVDPStatus(), 2, '0'));
    m_videoView->SetItem(1, 1, m_debugger->GetIE0() ? "1" : "0");
    m_videoView->SetItem(2, 1, m_debugger->GetIE1() ? "1" : "0");
    m_videoView->SetItem(3, 1, IntToString(m_debugger->GetIPeriod(), 1));
    m_videoView->SetItem(4, 1, IntToString(m_debugger->GetLine(), 1));
    m_videoView->SetItem(5, 1, m_debugger->ToHex(m_debugger->GetVideoAddress(), 4, '0'));
    */
}

void DebuggerNESDialog::UpdateOtherRegs() {
    /*
    const char *names[] = { "IE", "Bank 0", "Bank 1", "Bank 2", "Ram Enab", "Ram Bank" };
    
    m_othersView->DeleteAllItems();
    
    for (int i=0; i<6; i++) {
        m_othersView->InsertItem(i, "");
        m_othersView->SetItem(i, 0, names[i]);
        m_othersView->SetItemFont(i, *m_font);
    }
    
    m_othersView->SetItem(0, 1, m_debugger->GetIE() ? "1" : "0");
    m_othersView->SetItem(1, 1, IntToString(m_debugger->GetNumBank(0), 1));
    m_othersView->SetItem(2, 1, IntToString(m_debugger->GetNumBank(1), 1));
    m_othersView->SetItem(3, 1, IntToString(m_debugger->GetNumBank(2), 1));
    m_othersView->SetItem(4, 1, m_debugger->GetRAMEnabled() ? "1" : "0");
    m_othersView->SetItem(5, 1, IntToString(m_debugger->GetRAMNumBank(), 1));
    */
}

void DebuggerNESDialog::UpdateDisassemblerIcon(int numItem, u16 currentAddress, u16 pc) {
    if (currentAddress == pc) {
        if (m_debugger->HasBreakpoint(currentAddress))
            m_disassemblerView->SetItemColumnImage(numItem, 0, 2);
        else
            m_disassemblerView->SetItemColumnImage(numItem, 0, 0);
    }
    else if (m_debugger->HasBreakpoint(currentAddress))
        m_disassemblerView->SetItemColumnImage(numItem, 0, 1);
    else
        m_disassemblerView->SetItemColumnImage(numItem, 0, -1);
}

void DebuggerNESDialog::InitDisassemblerVars(u16 &currentAddress, u16 &nextAddress, string &name, string &data, u16 &pc) {
    m_debugger->DisassembleNext(currentAddress, nextAddress, name, data);
    pc = currentAddress;
    if ((currentAddress >= m_disassemblerFirst) && (currentAddress <= m_disassemblerLast)) {
        currentAddress = m_disassemblerFirst;
        m_debugger->DisassembleOne(currentAddress, nextAddress, name, data);
    } else
        m_disassemblerFirst = currentAddress;
}

void DebuggerNESDialog::UpdateDisassembler() {
    u16 currentAddress, nextAddress, pc, second;
    string address, name, data;
    const int lines = 8;
    
    InitDisassemblerVars(currentAddress, nextAddress, name, data, pc);
    
    m_disassemblerView->DeleteAllItems();
    for (int i=0; i<lines; i++) {
        address = m_debugger->ToHex(currentAddress, 4, '0');
        
        if (i == 1)
            second = currentAddress;
        
        if ((currentAddress == pc) && (i >= ((lines/2)-1)))
            m_disassemblerFirst = second;
        
        m_disassemblerView->InsertItem(i, "");
        m_disassemblerView->SetItem(i, 1, address);
        m_disassemblerView->SetItem(i, 2, name);
        m_disassemblerView->SetItem(i, 3, data);
        m_disassemblerView->SetItemFont(i, *m_font);
        UpdateDisassemblerIcon(i, currentAddress, pc);
        
        if (nextAddress < currentAddress)
            break;
        else {
            m_disassemblerLast = currentAddress;
            currentAddress = nextAddress;
            m_debugger->DisassembleOne(currentAddress, nextAddress, name, data);
        }
    }
}

void DebuggerNESDialog::UpdateFlags() {
    for (int i=0; i<8; i++) {
        bool value = m_debugger->GetFlag(i);
        m_flags[7-i]->SetValue(value);
    }
}

void DebuggerNESDialog::UpdatePad() {
    bool buttonsState[6];
    for (int i=0; i<6; i++)
        buttonsState[i] = m_input[i]->IsChecked();
    m_debugger->UpdatePad1(buttonsState);
}

void DebuggerNESDialog::OnReset(wxCommandEvent &event) {
    UpdatePad();
    m_debugger->Reset();
    UpdateUI();
}

void DebuggerNESDialog::OnStepInto(wxCommandEvent &event) {
    UpdatePad();
    m_debugger->StepInto();
    UpdateUI();
}

void DebuggerNESDialog::OnOneFrame(wxCommandEvent &event) {
    UpdatePad();
    m_debugger->ExecuteOneFrame();
    UpdateUI();
}

void DebuggerNESDialog::OnOneSecond(wxCommandEvent &event) {
    UpdatePad();
    for (int i=0; i<60; i++) {
        if (!m_debugger->ExecuteOneFrame())
            break;
    }
    
    UpdateUI();
}

void DebuggerNESDialog::OnBreakpoints(wxCommandEvent &event) {
    BreakpointsDialog breakpoints(this, m_debugger);
    breakpoints.ShowModal();
    UpdateUI();
}

void DebuggerNESDialog::OnMemAddressChange(wxCommandEvent &event) {
    if(!m_addressMemCtrl->IsModified())
        return;
    
    long insertionPoint = m_addressMemCtrl->GetInsertionPoint();
    wxString address = m_addressMemCtrl->GetValue().Upper();
    m_addressMemCtrl->ChangeValue(address);
    m_addressMemCtrl->SetInsertionPoint(insertionPoint);
    UpdateMemory();
}

void DebuggerNESDialog::OnActivated(wxListEvent &event) {
    long index = event.GetIndex();
    wxString address = m_disassemblerView->GetItemText(index, 1);
    long value;
    if(address.ToLong(&value, 16)) {
        value = value & 0xFFFF;
        if (m_debugger->HasBreakpoint(value))
            m_debugger->DelBreakpoint(value);
        else
            m_debugger->AddBreakpoint(value);
        UpdateDisassembler();
    }
}

void DebuggerNESDialog::OnMemSelectChange(wxCommandEvent &event) {
    UpdateMemory();
}

void DebuggerNESDialog::OnSaveTiles(wxCommandEvent &event) {
    /*
    const int width = 16*8;
    const int height = 28*8;
    u8 buffer[width*height*3];
    m_debugger->GetTiles(buffer, width, height);
    wxImage *img = new wxImage(width, height, buffer);
    
    if (img->SaveFile("tiles.bmp"))
        wxMessageBox("File succesfully saved as tiles.bmp");
    else
        wxMessageBox("Error trying to save the tiles");
    */
}
