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

#include <wx/artprov.h>
#include <wx/bookctrl.h>
#include <wx/spinctrl.h>
#include <wx/radiobox.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/config.h>
#include "SettingsDialog.h"
#include "IDControls.h"
#include "InputTextCtrl.h"
#include "../SMS-GG/Pad.h"


BEGIN_EVENT_TABLE(SettingsDialog, wxPropertySheetDialog)
END_EVENT_TABLE()

SettingsDialog::SettingsDialog(wxWindow* win)
{
	m_settings = SettingsGetCopy();

	SetSheetStyle(wxPROPSHEET_SHRINKTOFIT);
	
	Create(win, wxID_ANY, _("Preferences"));
	
	wxBookCtrlBase* notebook = GetBookCtrl();

	CreateButtons(wxOK | wxCANCEL);

    wxPanel* videoSettings = CreateVideoSettingsPage(notebook);
	wxPanel* soundSettings = CreateSoundSettingsPage(notebook);
	wxPanel* inputSettings = CreateInputSettingsPage(notebook);

	notebook->AddPage(videoSettings, _("Video"), true);
	notebook->AddPage(soundSettings, _("Sound"), false);
    notebook->AddPage(inputSettings, _("Input"), false);

    LayoutDialog();
}

SettingsDialog::~SettingsDialog()
{
    
}

/*! * Transfer data to the window */
bool SettingsDialog::TransferDataToWindow() {
	m_settings = SettingsGetCopy();
	
	wxCheckBox* soundEnabledCtrl = (wxCheckBox*)  FindWindow(ID_SOUND_ENABLED);
	wxChoice* soundSRCtrl =		 (wxChoice*)	  FindWindow(ID_SOUND_SR);
    
	InputTextCtrl* up1Ctrl =     (InputTextCtrl*) FindWindow(ID_TEXTCTRL_UP1);
	InputTextCtrl* down1Ctrl =	 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_DOWN1);
	InputTextCtrl* left1Ctrl =	 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_LEFT1);
	InputTextCtrl* right1Ctrl =	 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_RIGHT1);
	InputTextCtrl* a1Ctrl =		 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_A1);
	InputTextCtrl* b1Ctrl =		 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_B1);
    
    InputTextCtrl* up2Ctrl =     (InputTextCtrl*) FindWindow(ID_TEXTCTRL_UP2);
    InputTextCtrl* down2Ctrl =	 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_DOWN2);
    InputTextCtrl* left2Ctrl =	 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_LEFT2);
    InputTextCtrl* right2Ctrl =	 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_RIGHT2);
    InputTextCtrl* a2Ctrl =		 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_A2);
    InputTextCtrl* b2Ctrl =		 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_B2);
    
	InputTextCtrl* startCtrl =	 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_START);
	
	soundEnabledCtrl->SetValue(m_settings.soundEnabled);
	int sampleRates[] = { 22050, 32000, 44100, 48000 };
	int idSampleRate = 2;
	for (int i=0; i<4; i++)
	{
		if (m_settings.soundSampleRate == sampleRates[i])
			idSampleRate = i;
	}
	soundSRCtrl->SetSelection(idSampleRate);
	
	up1Ctrl->OnChangeKey(	m_settings.smsKeys[0]);
	down1Ctrl->OnChangeKey(	m_settings.smsKeys[1]);
	left1Ctrl->OnChangeKey(	m_settings.smsKeys[2]);
	right1Ctrl->OnChangeKey(m_settings.smsKeys[3]);
	a1Ctrl->OnChangeKey(	m_settings.smsKeys[4]);
	b1Ctrl->OnChangeKey(	m_settings.smsKeys[5]);
    
    up2Ctrl->OnChangeKey(	m_settings.smsKeys[6]);
    down2Ctrl->OnChangeKey(	m_settings.smsKeys[7]);
    left2Ctrl->OnChangeKey(	m_settings.smsKeys[8]);
    right2Ctrl->OnChangeKey(m_settings.smsKeys[9]);
    a2Ctrl->OnChangeKey(	m_settings.smsKeys[10]);
    b2Ctrl->OnChangeKey(	m_settings.smsKeys[11]);
    
	startCtrl->OnChangeKey(	m_settings.smsKeys[12]);

	return true;
}

/*! * Transfer data from the window */
bool SettingsDialog::TransferDataFromWindow() {
	wxCheckBox* soundEnabledCtrl = (wxCheckBox*)  FindWindow(ID_SOUND_ENABLED);
	wxChoice* soundSRCtrl =		 (wxChoice*)	  FindWindow(ID_SOUND_SR);
    
	InputTextCtrl* up1Ctrl    =	(InputTextCtrl*) FindWindow(ID_TEXTCTRL_UP1);
	InputTextCtrl* down1Ctrl  =	(InputTextCtrl*) FindWindow(ID_TEXTCTRL_DOWN1);
	InputTextCtrl* left1Ctrl  =	(InputTextCtrl*) FindWindow(ID_TEXTCTRL_LEFT1);
	InputTextCtrl* right1Ctrl =	(InputTextCtrl*) FindWindow(ID_TEXTCTRL_RIGHT1);
	InputTextCtrl* a1Ctrl     =	(InputTextCtrl*) FindWindow(ID_TEXTCTRL_A1);
	InputTextCtrl* b1Ctrl     =	(InputTextCtrl*) FindWindow(ID_TEXTCTRL_B1);
    
    InputTextCtrl* up2Ctrl    =	(InputTextCtrl*) FindWindow(ID_TEXTCTRL_UP2);
    InputTextCtrl* down2Ctrl  =	(InputTextCtrl*) FindWindow(ID_TEXTCTRL_DOWN2);
    InputTextCtrl* left2Ctrl  =	(InputTextCtrl*) FindWindow(ID_TEXTCTRL_LEFT2);
    InputTextCtrl* right2Ctrl =	(InputTextCtrl*) FindWindow(ID_TEXTCTRL_RIGHT2);
    InputTextCtrl* a2Ctrl     =	(InputTextCtrl*) FindWindow(ID_TEXTCTRL_A2);
    InputTextCtrl* b2Ctrl     =	(InputTextCtrl*) FindWindow(ID_TEXTCTRL_B2);
    
	InputTextCtrl* startCtrl =	 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_START);

	m_settings.soundEnabled = soundEnabledCtrl->GetValue();
	int sampleRates[] = { 22050, 32000, 44100, 48000 };
	int idSampleRate = soundSRCtrl->GetSelection();
	m_settings.soundSampleRate = sampleRates[idSampleRate];

	m_settings.smsKeys[ 0] = up1Ctrl->keyCode;
	m_settings.smsKeys[ 1] = down1Ctrl->keyCode;
	m_settings.smsKeys[ 2] = left1Ctrl->keyCode;
	m_settings.smsKeys[ 3] = right1Ctrl->keyCode;
	m_settings.smsKeys[ 4] = a1Ctrl->keyCode;
	m_settings.smsKeys[ 5] = b1Ctrl->keyCode;
    
    m_settings.smsKeys[ 6] = up2Ctrl->keyCode;
    m_settings.smsKeys[ 7] = down2Ctrl->keyCode;
    m_settings.smsKeys[ 8] = left2Ctrl->keyCode;
    m_settings.smsKeys[ 9] = right2Ctrl->keyCode;
    m_settings.smsKeys[10] = a2Ctrl->keyCode;
    m_settings.smsKeys[11] = b2Ctrl->keyCode;
    
	m_settings.smsKeys[12] = startCtrl->keyCode;
	
    SettingsSetNewValues(m_settings);
	SettingsSaveToFile();

	return true;
}

wxPanel* SettingsDialog::CreateVideoSettingsPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);

    return panel;
}

wxPanel* SettingsDialog::CreateSoundSettingsPage(wxWindow* parent)
{
	wxPanel* panel = new wxPanel(parent, wxID_ANY);
	
	wxStaticText * enabledLabel = new wxStaticText(panel, wxID_ANY, _("Enabled:"));
	wxCheckBox * enabledCheckBox = new wxCheckBox(panel, ID_SOUND_ENABLED, wxT(""));
	enabledCheckBox->SetValue(true);
	
	wxStaticText * sampleRateLabel = new wxStaticText(panel, wxID_ANY, _("Sample Rate:"));
    wxChoice* sampleRateChoice = new wxChoice(panel, ID_SOUND_SR);
	sampleRateChoice->Append(wxT("22050 Hz"));
	sampleRateChoice->Append(wxT("32000 Hz"));
	sampleRateChoice->Append(wxT("44100 Hz"));
	sampleRateChoice->Append(wxT("48000 Hz"));
	sampleRateChoice->SetSelection(0);
	
	wxFlexGridSizer *grid = new wxFlexGridSizer(2, 3, 5);
	grid->Add(enabledLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
	grid->Add(enabledCheckBox);
	grid->Add(sampleRateLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
	grid->Add(sampleRateChoice);
	
	wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
	topSizer->Add(grid, 0, wxALL, 10);
	
    panel->SetSizerAndFit(topSizer);
	
    return panel;
}

wxPanel* SettingsDialog::CreateInputSettingsPage(wxWindow* parent)
{
	wxPanel* panel = new wxPanel(parent, wxID_ANY);
	
    wxStaticText *pad1Label   = new wxStaticText(panel, wxID_ANY, _("Pad 1"));
    wxStaticText *pad2Label   = new wxStaticText(panel, wxID_ANY, _("Pad 2"));
    
    wxStaticText *upLabel    = new wxStaticText(panel, wxID_ANY, _("Up:"));
    wxStaticText *downLabel  = new wxStaticText(panel, wxID_ANY, _("Down:"));
    wxStaticText *leftLabel  = new wxStaticText(panel, wxID_ANY, _("Left:"));
    wxStaticText *rightLabel = new wxStaticText(panel, wxID_ANY, _("Right:"));
    wxStaticText *aLabel     = new wxStaticText(panel, wxID_ANY, wxT("1:"));
    wxStaticText *bLabel     = new wxStaticText(panel, wxID_ANY, wxT("2:"));
    
    wxStaticText *startLabel  = new wxStaticText(panel, wxID_ANY, wxT("Pause/Start:"));
    
	InputTextCtrl *up1TextCtrl    = new InputTextCtrl(panel, ID_TEXTCTRL_UP1);
	InputTextCtrl *down1TextCtrl  = new InputTextCtrl(panel, ID_TEXTCTRL_DOWN1);
	InputTextCtrl *left1TextCtrl  = new InputTextCtrl(panel, ID_TEXTCTRL_LEFT1);
	InputTextCtrl *right1TextCtrl = new InputTextCtrl(panel, ID_TEXTCTRL_RIGHT1);
	InputTextCtrl *a1TextCtrl     = new InputTextCtrl(panel, ID_TEXTCTRL_A1);
	InputTextCtrl *b1TextCtrl     = new InputTextCtrl(panel, ID_TEXTCTRL_B1);
    
    InputTextCtrl *up2TextCtrl    = new InputTextCtrl(panel, ID_TEXTCTRL_UP2);
    InputTextCtrl *down2TextCtrl  = new InputTextCtrl(panel, ID_TEXTCTRL_DOWN2);
    InputTextCtrl *left2TextCtrl  = new InputTextCtrl(panel, ID_TEXTCTRL_LEFT2);
    InputTextCtrl *right2TextCtrl = new InputTextCtrl(panel, ID_TEXTCTRL_RIGHT2);
    InputTextCtrl *a2TextCtrl     = new InputTextCtrl(panel, ID_TEXTCTRL_A2);
    InputTextCtrl *b2TextCtrl     = new InputTextCtrl(panel, ID_TEXTCTRL_B2);
    
	InputTextCtrl *startTextCtrl = new InputTextCtrl(panel, ID_TEXTCTRL_START);
	
	wxFlexGridSizer *grid = new wxFlexGridSizer(3, 3, 5);
    grid->AddSpacer(0);
    grid->Add(pad1Label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
    grid->Add(pad2Label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
	grid->Add(upLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
	grid->Add(up1TextCtrl);
    grid->Add(up2TextCtrl);
	grid->Add(downLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
    grid->Add(down1TextCtrl);
    grid->Add(down2TextCtrl);
	grid->Add(leftLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
	grid->Add(left1TextCtrl);
    grid->Add(left2TextCtrl);
	grid->Add(rightLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
	grid->Add(right1TextCtrl);
    grid->Add(right2TextCtrl);
	grid->Add(aLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
	grid->Add(a1TextCtrl);
    grid->Add(a2TextCtrl);
	grid->Add(bLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
	grid->Add(b1TextCtrl);
    grid->Add(b2TextCtrl);
    
	grid->Add(startLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
	grid->Add(startTextCtrl);
	
	wxBoxSizer * topSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(0, 10);
	topSizer->Add(grid, 0, wxRIGHT|wxLEFT, 30);
	topSizer->Add(0, 10);
	
	panel->SetSizerAndFit(topSizer);
	
    return panel;
}

void SettingsDialog::Reload() {
    m_settings = SettingsGetCopy();
}

void SettingsDialog::AcceptValues() {
    SettingsSetNewValues(m_settings);
}
