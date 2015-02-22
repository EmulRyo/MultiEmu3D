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
#include <wx/collpane.h>
#include <wx/config.h>
#include "SettingsDialog.h"
#include "IDControls.h"
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
	
    wxRadioBox* greenscaleCtrl = (wxRadioBox*)	  FindWindow(ID_GREENSCALE);
	wxCheckBox* soundEnabledCtrl = (wxCheckBox*)  FindWindow(ID_SOUND_ENABLED);
	wxChoice* soundSRCtrl =		 (wxChoice*)	  FindWindow(ID_SOUND_SR);

    greenscaleCtrl->SetSelection(m_settings.greenScale);
    
	soundEnabledCtrl->SetValue(m_settings.soundEnabled);
	int sampleRates[] = { 22050, 32000, 44100, 48000 };
	int idSampleRate = 2;
	for (int i=0; i<4; i++)
	{
		if (m_settings.soundSampleRate == sampleRates[i])
			idSampleRate = i;
	}
	soundSRCtrl->SetSelection(idSampleRate);

    for (int i=0; i<8; i++)
        gbCtrl[i]->OnChangeKey(m_settings.gbKeys[i]);
    for (int i=0; i<13; i++)
        smsCtrl[i]->OnChangeKey(m_settings.smsKeys[i]);
    
	return true;
}

/*! * Transfer data from the window */
bool SettingsDialog::TransferDataFromWindow() {
    wxRadioBox* greenscaleCtrl = (wxRadioBox*)	  FindWindow(ID_GREENSCALE);
    
	wxCheckBox* soundEnabledCtrl = (wxCheckBox*)  FindWindow(ID_SOUND_ENABLED);
	wxChoice* soundSRCtrl =		 (wxChoice*)	  FindWindow(ID_SOUND_SR);

    m_settings.greenScale = greenscaleCtrl->GetSelection();
    
	m_settings.soundEnabled = soundEnabledCtrl->GetValue();
	int sampleRates[] = { 22050, 32000, 44100, 48000 };
	int idSampleRate = soundSRCtrl->GetSelection();
	m_settings.soundSampleRate = sampleRates[idSampleRate];
    
    for (int i=0; i<8; i++)
        m_settings.gbKeys[i] = gbCtrl[i]->keyCode;
    for (int i=0; i<13; i++)
        m_settings.smsKeys[i] = smsCtrl[i]->keyCode;
    
    SettingsSetNewValues(m_settings);
	SettingsSaveToFile();

	return true;
}

wxPanel* SettingsDialog::CreateVideoSettingsPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);
    
    wxStaticText * grayGreenLabel = new wxStaticText(panel, wxID_ANY, _("DMG color palette:"));
    wxString grayGreenChoices[2];
    grayGreenChoices[0] = _("Grayscale");
    grayGreenChoices[1] = _("Greenscale");
    wxRadioBox* grayGreenRadioBox = new wxRadioBox(panel, ID_GREENSCALE, wxT(""),
                                                   wxDefaultPosition, wxDefaultSize, 2, grayGreenChoices, 1, wxRA_SPECIFY_COLS);
    
    wxFlexGridSizer *grid = new wxFlexGridSizer(2, 3, 5);
    grid->Add(grayGreenLabel, 0, wxUP, 7);
    grid->Add(grayGreenRadioBox);
    
    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    topSizer->Add(grid, 0, wxALL, 10);
    
    panel->SetSizerAndFit(topSizer);
    
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
    
    wxWindow *gbPane = CreateInputSMS(panel);
    wxWindow *smsPane = CreateInputGB(panel);
    
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(0, 10);
    topSizer->Add(gbPane,  0, wxGROW|wxRIGHT|wxLEFT, 30);
    topSizer->Add(0, 10);
    topSizer->Add(smsPane, 0, wxGROW|wxRIGHT|wxLEFT, 30);
	topSizer->Add(0, 10);
	
	panel->SetSizerAndFit(topSizer);
	
    return panel;
}

wxWindow* SettingsDialog::CreateInputGB(wxWindow *parent) {
    wxCollapsiblePane *collPane = new wxCollapsiblePane(parent, wxID_ANY, "GameBoy / GameBoy Color:");
    wxWindow *pane = collPane->GetPane();
    
    wxStaticText *upLabel    = new wxStaticText(pane, wxID_ANY, _("Up:"));
    wxStaticText *downLabel  = new wxStaticText(pane, wxID_ANY, _("Down:"));
    wxStaticText *leftLabel  = new wxStaticText(pane, wxID_ANY, _("Left:"));
    wxStaticText *rightLabel = new wxStaticText(pane, wxID_ANY, _("Right:"));
    wxStaticText *aLabel     = new wxStaticText(pane, wxID_ANY, wxT("A:"));
    wxStaticText *bLabel     = new wxStaticText(pane, wxID_ANY, wxT("B:"));
    wxStaticText *selectLabel = new wxStaticText(pane, wxID_ANY, wxT("Select:"));
    wxStaticText *startLabel  = new wxStaticText(pane, wxID_ANY, wxT("Start:"));

    for (int i=0; i<8; i++)
        gbCtrl[i] = new InputTextCtrl(pane, wxID_ANY);
    
    wxFlexGridSizer *grid = new wxFlexGridSizer(2, 3, 5);
    grid->Add(upLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
    grid->Add(gbCtrl[0]);
    grid->Add(downLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
    grid->Add(gbCtrl[1]);
    grid->Add(leftLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
    grid->Add(gbCtrl[2]);
    grid->Add(rightLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
    grid->Add(gbCtrl[3]);
    grid->Add(aLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
    grid->Add(gbCtrl[4]);
    grid->Add(bLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
    grid->Add(gbCtrl[5]);
    grid->Add(selectLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
    grid->Add(gbCtrl[6]);
    grid->Add(startLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
    grid->Add(gbCtrl[7]);
    
    pane->SetSizer(grid);
    
    return collPane;
}

wxWindow* SettingsDialog::CreateInputSMS(wxWindow *parent) {
    wxCollapsiblePane *collPane = new wxCollapsiblePane(parent, wxID_ANY, "Master System / GameGear:");
    wxWindow *pane = collPane->GetPane();
    
    wxStaticText *pad1Label  = new wxStaticText(pane, wxID_ANY, _("Pad 1"));
    wxStaticText *pad2Label  = new wxStaticText(pane, wxID_ANY, _("Pad 2"));
    
    wxStaticText *upLabel    = new wxStaticText(pane, wxID_ANY, _("Up:"));
    wxStaticText *downLabel  = new wxStaticText(pane, wxID_ANY, _("Down:"));
    wxStaticText *leftLabel  = new wxStaticText(pane, wxID_ANY, _("Left:"));
    wxStaticText *rightLabel = new wxStaticText(pane, wxID_ANY, _("Right:"));
    wxStaticText *aLabel     = new wxStaticText(pane, wxID_ANY, wxT("1:"));
    wxStaticText *bLabel     = new wxStaticText(pane, wxID_ANY, wxT("2:"));
    
    wxStaticText *startLabel  = new wxStaticText(pane, wxID_ANY, wxT("Pause/Start:"));

    for (int i=0; i<13; i++)
        smsCtrl[i] = new InputTextCtrl(pane, wxID_ANY);
     
    wxFlexGridSizer *grid = new wxFlexGridSizer(3, 3, 5);
    grid->AddSpacer(0);
    grid->Add(pad1Label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
    grid->Add(pad2Label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
    grid->Add(upLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
    grid->Add(smsCtrl[0]);
    grid->Add(smsCtrl[6]);
    grid->Add(downLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
    grid->Add(smsCtrl[1]);
    grid->Add(smsCtrl[7]);
    grid->Add(leftLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
    grid->Add(smsCtrl[2]);
    grid->Add(smsCtrl[8]);
    grid->Add(rightLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
    grid->Add(smsCtrl[3]);
    grid->Add(smsCtrl[9]);
    grid->Add(aLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
    grid->Add(smsCtrl[4]);
    grid->Add(smsCtrl[10]);
    grid->Add(bLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
    grid->Add(smsCtrl[5]);
    grid->Add(smsCtrl[11]);
    
    grid->Add(startLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
    grid->Add(smsCtrl[12]);
    
    pane->SetSizer(grid);
    
    return collPane;
}

void SettingsDialog::Reload() {
    m_settings = SettingsGetCopy();
}

void SettingsDialog::AcceptValues() {
    SettingsSetNewValues(m_settings);
}
