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

#include <iostream>
#include <string>
#include <wx/toolbar.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/menu.h>
#include <wx/filedlg.h>
#include <wx/button.h>
#include <wx/settings.h>
#include "MainApp.h"
#include "MainFrame.h"
#include "AboutDialog.h"
#include "SettingsDialog.h"
#include "DebuggerSMSDialog.h"
#include "DebuggerGBDialog.h"
#include "DebuggerNESDialog.h"
#include "IDControls.h"
#include "Settings.h"
#include "../Common/Exception.h"
#include "../Common/VideoGameDevice.h"
#include "Xpm/open.xpm"
#include "Xpm/play.xpm"
#include "Xpm/pause.xpm"
#include "Xpm/stop.xpm"
#include "Xpm/recent.xpm"
#include "Xpm/gb16.xpm"
#include "Xpm/gb32.xpm"
#include "Xpm/changeView.xpm"
#include "RendererOGL.h"
#include "RendererSW.h"
#include "EmulationThread.h"
#include "AppDefs.h"

using namespace std;

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(wxID_EXIT, MainFrame::OnFileExit)
EVT_MENU(wxID_OPEN, MainFrame::OnFileOpen)
EVT_MENU(ID_OPEN_RECENT, MainFrame::OnRecent)
EVT_MENU_RANGE(ID_RECENT0, ID_RECENT9, MainFrame::OnRecentItem)
EVT_MENU(ID_CLEAR_RECENT, MainFrame::OnClearRecent)
EVT_MENU_RANGE(ID_LOADSTATE0, ID_LOADSTATE9, MainFrame::OnLoadState)
EVT_MENU_RANGE(ID_SAVESTATE0, ID_SAVESTATE9, MainFrame::OnSaveState)
EVT_MENU(wxID_PREFERENCES, MainFrame::OnSettings)
EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
EVT_MENU(ID_START, MainFrame::OnPlay)
EVT_MENU(ID_PAUSE, MainFrame::OnPause)
EVT_MENU(ID_STOP, MainFrame::OnStop)
EVT_MENU(ID_DEBUG, MainFrame::OnDebug)
EVT_MENU(ID_CHANGEVIEW, MainFrame::OnChangeView)
EVT_MENU(ID_FULLSCREEN, MainFrame::OnFullScreen)
EVT_UPDATE_UI( ID_START, MainFrame::OnPlayUpdateUI )
EVT_UPDATE_UI( ID_PAUSE, MainFrame::OnPauseUpdateUI )
EVT_UPDATE_UI( ID_STOP, MainFrame::OnStopUpdateUI )
EVT_UPDATE_UI( ID_DEBUG, MainFrame::OnDebugUpdateUI )
EVT_UPDATE_UI_RANGE(ID_LOADSTATE0, ID_LOADSTATE9, MainFrame::OnLoadStateUpdateUI)
EVT_UPDATE_UI_RANGE(ID_SAVESTATE0, ID_SAVESTATE9, MainFrame::OnSaveStateUpdateUI)
EVT_TIMER(ID_TIMER, MainFrame::OnTimer)
EVT_CLOSE(MainFrame::OnClose)
EVT_SIZE(MainFrame::OnResize)
EVT_MAXIMIZE(MainFrame::OnMaximize)
EVT_MENU_RANGE(ID_LANG_SYSTEM, ID_LANG_SPANISH, MainFrame::OnChangeLanguage)
END_EVENT_TABLE()

MainFrame::MainFrame(wxString fileName)
{
	m_renderer = NULL;
    m_toolBar = NULL;

    // Create the MainFrame
    this->Create(0, ID_MAINFRAME, wxT(APP_NAME), wxDefaultPosition,
           wxDefaultSize, wxDEFAULT_FRAME_STYLE);
    
	wxIconBundle * icons = new wxIconBundle(wxIcon(gb16_xpm));
	icons->AddIcon(wxIcon(gb32_xpm));
	SetIcons(*icons);

    m_mainSizer = new wxBoxSizer(wxVERTICAL);
    
    CreateMenuBar();
	CreateToolBar();
	
	m_numRecentFiles = 0;

	m_settingsDialog = new SettingsDialog(this);
	m_settingsDialog->CentreOnScreen();
	CreateRecentMenu(SettingsGetRecentRoms());

    // create the emulation
    m_emulation = new EmulationThread();
    
    wxThreadError err = m_emulation->Create();
    if (err != wxTHREAD_NO_ERROR)
        wxMessageBox(wxT("Couldn't create the thread!"));

    err = m_emulation->Run();
    if (err != wxTHREAD_NO_ERROR)
        wxMessageBox(wxT("Couldn't run the thread!"));

    m_fullScreen = false;
    ChangeRenderer();
    
	if (fileName != wxT(""))
		ChangeFile(fileName);
		
    m_timer = new wxTimer(this, ID_TIMER);
	m_timer->Start(16);
    
    SetSizerAndFit(m_mainSizer);
}

MainFrame::~MainFrame()
{
	// No hacer nada aqui mejor hacerlo todo
    // en el OnClose
}

void MainFrame::CreateMenuBar()
{
	// create the main menubar
    m_mb = new wxMenuBar();

    // create the file menu
    wxMenu *fileMenu = new wxMenu;
    wxString openName = _("&Open");
    openName += wxT("\tCtrl+O");
	fileMenu->Append(wxID_OPEN, openName);

	m_recentMenuFile = new wxMenu;
	m_recentMenuFile->AppendSeparator();
	m_recentMenuFile->Append(ID_CLEAR_RECENT, _("Clear recent roms"));
	fileMenu->AppendSubMenu(m_recentMenuFile, _("Open Recent"));
    
    // Se crea un wxMenu que se tratará exactamente igual que a recentMenuFile
	// para poder tener uno en el menuBar y otro como popUp
	m_recentMenuPopup = new wxMenu;
	m_recentMenuPopup->AppendSeparator();
	m_recentMenuPopup->Append(ID_CLEAR_RECENT, _("Clear recent roms"));
	
	wxMenu * loadMenuFile = new wxMenu;
	wxMenu * saveMenuFile = new wxMenu;
	wxString slotMenu;
	for (int i=1; i<11; i++)
	{
		int id = i % 10;
		slotMenu = wxT("");
		slotMenu << wxT("Slot ") << id << wxT("\tCtrl+Alt+") << id;
		loadMenuFile->Append(ID_LOADSTATE0+id, slotMenu);
		slotMenu = wxT("");
		slotMenu << wxT("Slot ") << id << wxT("\tCtrl+") << id;
		saveMenuFile->Append(ID_SAVESTATE0+id, slotMenu);
	}
	fileMenu->AppendSubMenu(loadMenuFile, _("Load State"));
	fileMenu->AppendSubMenu(saveMenuFile, _("Save State"));
	
	fileMenu->Append(wxID_EXIT, _("E&xit"));

    // add the file menu to the menu bar
    m_mb->Append(fileMenu, _("&File"));

    wxString preferencesName = _("&Settings");
    preferencesName += wxT("\tCtrl+E");
    wxString startName = _("&Start");
    startName += wxT("\tCtrl+S");
    wxString pauseName = _("&Pause");
    pauseName += wxT("\tCtrl+P");
    wxString stopName = _("S&top");
    stopName += wxT("\tCtrl+T");
    wxString debugName = _("&Debug");
    debugName += wxT("\tCtrl+D");
    wxString fullscreenName = _("&FullScreen");
    fullscreenName += wxT("\tCtrl+F");

	// create the emulation menu
    wxMenu *emulationMenu = new wxMenu;
	emulationMenu->Append(wxID_PREFERENCES, preferencesName);
    emulationMenu->Append(ID_START, startName);
	emulationMenu->Append(ID_PAUSE, pauseName);
	emulationMenu->Append(ID_STOP, stopName);
    emulationMenu->Append(ID_DEBUG, debugName);
    emulationMenu->Append(ID_FULLSCREEN, fullscreenName);

    // add the file menu to the menu bar
    m_mb->Append(emulationMenu, _("&Emulation"));
    
    wxMenu *languageMenu = new wxMenu;
    languageMenu->Append(ID_LANG_SYSTEM,  _("System"));
    languageMenu->Append(ID_LANG_ENGLISH, wxT("English"));
    languageMenu->Append(ID_LANG_FRENCH,  wxString::FromUTF8("Français"));
    languageMenu->Append(ID_LANG_GERMAN,  wxString::FromUTF8("Deutsch"));
    languageMenu->Append(ID_LANG_GREEK,   wxString::FromUTF8("Ελληνικά"));
    languageMenu->Append(ID_LANG_SPANISH, wxString::FromUTF8("Español"));
    m_mb->Append(languageMenu, _("&Language"));

    // create the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, _("&About"));

    // add the help menu to the menu bar
    m_mb->Append(helpMenu, _("&Help"));

    // add the menu bar to the MainFrame
    SetMenuBar(m_mb);
}

void MainFrame::CreateToolBar()
{
    m_toolBar = new wxToolBar(this, wxID_ANY);
    m_toolBar->SetToolBitmapSize(wxSize(16, 16));
    m_toolBar->SetMinSize(wxSize(SMS_SCREEN_W, 25));
    
	wxBitmap bmpOpen(open_xpm);
	m_toolBar->AddTool(wxID_OPEN, _("Open"), bmpOpen, _("Open"));
	
	wxBitmap bmpRecent(recent_xpm);
	m_toolBar->AddTool(ID_OPEN_RECENT, _("Recent"), bmpRecent, _("Recent"));

    m_toolBar->AddStretchableSpace();

	wxBitmap bmpPlay(play_xpm);
	m_toolBar->AddTool(ID_START, _("Start"), bmpPlay, _("Start"));

	wxBitmap bmpPause(pause_xpm);
	m_toolBar->AddTool(ID_PAUSE, _("Pause"), bmpPause, _("Pause"));

	wxBitmap bmpStop(stop_xpm);
	m_toolBar->AddTool(ID_STOP, _("Stop"), bmpStop, _("Stop"));
	
	m_toolBar->EnableTool(ID_START, false);
	m_toolBar->EnableTool(ID_PAUSE, false);
	m_toolBar->EnableTool(ID_STOP, false);
    
    m_toolBar->AddStretchableSpace();
    
    wxBitmap bmpChangeView(changeView_xpm);
	m_toolBar->AddTool(ID_CHANGEVIEW, _("Change View"), bmpChangeView, _("Change View"));
	
    m_toolBar->Realize();
    m_mainSizer->Add(m_toolBar, 0, wxEXPAND);
}

void MainFrame::OnRecent(wxCommandEvent &event)
{
	this->PopupMenu(m_recentMenuPopup, 0, 0);
}

void MainFrame::OnFileOpen(wxCommandEvent &) {

	EmuState copyState = m_emulation->GetState();
    m_emulation->SetState(EmuState::Paused);
	
	wxFileDialog* openDialog = new wxFileDialog(this, _("Choose a rom to open"), wxEmptyString, wxEmptyString,
												wxT("All roms (*.gb; *.gbc; *.nes; *.sms; *.gg; *.zip)|**.gb;*.gbc;*.nes;.sms;*.gg;*.zip"),
												wxFD_OPEN, wxDefaultPosition);

	
	// Creates a "open file" dialog
	if (openDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
		ChangeFile(openDialog->GetPath());
	else
		m_emulation->SetState(copyState);

	// Clean up after ourselves
	openDialog->Destroy();
}
 
void MainFrame::OnRecentItem(wxCommandEvent &event)
{
	int idAux = event.GetId();
	int id = idAux - ID_RECENT0;
	ChangeFile(m_recentFiles[id].fullName);
}

void MainFrame::OnLoadState(wxCommandEvent &event)
{
	int idAux = event.GetId();
	int id = idAux - ID_LOADSTATE0;
	
	wxString savesDir = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator()
	+ wxT("SaveStates") + wxFileName::GetPathSeparator();

	try
	{
		m_emulation->LoadState(string(savesDir.mb_str()), id);
	}
	catch(Exception e)
	{
		wxMessageBox(wxString(e.what(), wxConvUTF8), _("Error"), wxICON_WARNING);
	}
}

void MainFrame::OnSaveState(wxCommandEvent &event)
{
	int idAux = event.GetId();
	int id = idAux - ID_SAVESTATE0;
	
	wxString savesDir = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator()
	+ wxT("SaveStates");
	
	if (!wxFileName::DirExists(savesDir))
		wxFileName::Mkdir(savesDir, 0777, wxPATH_MKDIR_FULL);
	
	savesDir += wxFileName::GetPathSeparator();
	try
	{
		m_emulation->SaveState(string(savesDir.mb_str()), id);
	}
	catch(Exception e)
	{
		wxMessageBox(wxString(e.what(), wxConvUTF8), _("Error"), wxICON_WARNING);
	}
}

void MainFrame::OnClearRecent(wxCommandEvent &)
{
	for (int i=0; i<m_numRecentFiles; i++)
	{
		m_recentMenuFile->Delete(ID_RECENT0+i);
		m_recentMenuPopup->Delete(ID_RECENT0+i);
	}
	m_numRecentFiles = 0;
	
	RecentRomsToSettings();
    m_settingsDialog->Reload();
    SettingsSaveToFile();
}

void MainFrame::ChangeFile(const wxString fileName)
{
	if (m_emulation->ChangeFile(fileName))
        UpdateRecentMenu(fileName);
}

void MainFrame::CreateRecentMenu(std::string * roms)
{
	for (int i=0; i<MAX_RECENT_FILES; i++)
	{
		if (roms[i] == "")
			break;
		
		m_recentFiles[i].fullName = wxString(roms[i].c_str(), wxConvUTF8);
		m_recentFiles[i].shortName = m_recentFiles[i].fullName.substr(m_recentFiles[i].fullName.rfind(wxFileName::GetPathSeparator())+1);
		
		int id = ID_RECENT0 + m_numRecentFiles;
		m_recentMenuFile->Insert(m_numRecentFiles, id, m_recentFiles[i].shortName);
		m_recentMenuPopup->Insert(m_numRecentFiles, id, m_recentFiles[i].shortName);
		
		m_numRecentFiles++;
	}
	
}

void MainFrame::UpdateRecentMenu(wxString fileName)
{
	wxString shortName = fileName.substr(fileName.rfind(wxFileName::GetPathSeparator())+1);
	int previousIndex = -1;
	for (int i=0; i<m_numRecentFiles; i++)
	{
		if (m_recentFiles[i].fullName == fileName)
		{
			previousIndex = i;
			break;
		}
	}
	
	int startFrom;
	
	// Si ya existia de antes y es el primero
	if (previousIndex == 0)
	{
		return;
	}
	// Si ya existia de antes y no es el primero
	else if (previousIndex > 0)
	{
		startFrom = previousIndex-1;
	}
	// Si no existia pero no hemos llegado al limite
	else if (m_numRecentFiles < MAX_RECENT_FILES)
	{
		startFrom = m_numRecentFiles-1;
		int id = ID_RECENT0 + m_numRecentFiles;
		m_recentMenuFile->Insert(m_numRecentFiles, id, wxT(" "));
		m_recentMenuPopup->Insert(m_numRecentFiles, id, wxT(" "));
		m_numRecentFiles++;
	}
	// Si no existia pero hemos llegado al limite
	else
	{
		startFrom = MAX_RECENT_FILES-2;
	}
	
	for (int i=startFrom; i>=0; i--)
	{
		m_recentFiles[i+1].shortName = m_recentFiles[i].shortName;
		m_recentFiles[i+1].fullName = m_recentFiles[i].fullName;
	}
	m_recentFiles[0].shortName = shortName;
	m_recentFiles[0].fullName = fileName;
	
	for (int i=0; i<m_numRecentFiles; i++)
	{
		m_recentMenuFile->SetLabel(ID_RECENT0+i, m_recentFiles[i].shortName);
		m_recentMenuPopup->SetLabel(ID_RECENT0+i, m_recentFiles[i].shortName);
	}
	
	RecentRomsToSettings();
    m_settingsDialog->Reload();
    SettingsSaveToFile();
}


void MainFrame::RecentRomsToSettings()
{
	std::string recentRomsSettings[10];
	
	for (int i=0; i<m_numRecentFiles; i++)
	{
		recentRomsSettings[i] = m_recentFiles[i].fullName.mb_str();
	}
	
	for(int i=m_numRecentFiles; i<MAX_RECENT_FILES; i++)
	{
		recentRomsSettings[i] = "";
	}
	
	SettingsSetRecentRoms(recentRomsSettings);
}

void MainFrame::OnFileExit(wxCommandEvent &)
{
	this->Close();
}

void MainFrame::OnClose(wxCloseEvent&)
{
    m_timer->Stop();
    
	if (m_settingsDialog) {
		m_settingsDialog->Destroy();
        delete m_settingsDialog;
    }
    
    m_emulation->Delete();
    while (!m_emulation->Finished())
        wxThread::This()->Sleep(1);
    
    Destroy();
}

/*
 * Abre un dialogo de configuracion. Cuando se cierra se encarga de aplicar ciertos cambios a la emulacion
 */
void MainFrame::OnSettings(wxCommandEvent &)
{
	EmuState lastState = m_emulation->GetState();
	if (m_emulation->GetState() == EmuState::Playing)
		m_emulation->SetState(EmuState::Paused);


    if (m_settingsDialog->ShowModal() == wxID_OK)
	{
		m_settingsDialog->AcceptValues();
        m_emulation->ApplySettings();
        
        if (m_renderer)
            m_renderer->SetGBPalette(SettingsGetGreenScale());
	}

    m_emulation->SetState(lastState);
}

void MainFrame::ChangeRenderer()
{
    if (m_renderer)
    {
        wxWindow *window = m_renderer->GetWinRenderer();
        m_mainSizer->Detach(window);
        window->Destroy();
    }
    
    m_renderer = new RendererOGL(this);
    m_emulation->SetScreen(m_renderer);
    
    m_mainSizer->Add(m_renderer->GetWinRenderer(), 1, wxEXPAND);
    m_mainSizer->Layout();
}

void MainFrame::OnFullScreen(wxCommandEvent &)
{
    ToggleFullScreen();
}

void MainFrame::OnAbout(wxCommandEvent &)
{
	AboutDialog(this);
}

void MainFrame::OnPlay(wxCommandEvent &)
{
    m_emulation->SetState(EmuState::Playing);
}

void MainFrame::OnPause(wxCommandEvent &)
{
	if (m_emulation->GetState() == EmuState::Playing)
		m_emulation->SetState(EmuState::Paused);
	else if (m_emulation->GetState() == EmuState::Paused)
		m_emulation->SetState(EmuState::Playing);
}

void MainFrame::OnStop(wxCommandEvent &)
{
    m_emulation->SetState(EmuState::Stopped);
}

void MainFrame::OnPlayUpdateUI(wxUpdateUIEvent& event)
{
	if ((m_emulation->GetState() == EmuState::NotStartedYet) || (m_emulation->GetState() == EmuState::Playing))
		event.Enable(false);
	else
		event.Enable(true);
}

void MainFrame::OnPauseUpdateUI(wxUpdateUIEvent& event)
{
	if ((m_emulation->GetState() == EmuState::NotStartedYet) || (m_emulation->GetState() == EmuState::Stopped))
		event.Enable(false);
	else
		event.Enable(true);
}

void MainFrame::OnStopUpdateUI(wxUpdateUIEvent& event)
{
	if ((m_emulation->GetState() == EmuState::Stopped) || (m_emulation->GetState() == EmuState::NotStartedYet))
		event.Enable(false);
	else
		event.Enable(true);
}

void MainFrame::OnDebugUpdateUI(wxUpdateUIEvent& event) {
    bool enabled = (m_emulation->GetState() != EmuState::NotStartedYet);
    event.Enable(enabled);
}

void MainFrame::OnLoadStateUpdateUI(wxUpdateUIEvent& event)
{
	if ((m_emulation->GetState() == EmuState::Stopped) || (m_emulation->GetState() == EmuState::NotStartedYet))
		event.Enable(false);
	else
		event.Enable(true);
}

void MainFrame::OnSaveStateUpdateUI(wxUpdateUIEvent& event)
{
	if ((m_emulation->GetState() == EmuState::Stopped)||(m_emulation->GetState() == EmuState::NotStartedYet))
		event.Enable(false);
	else
		event.Enable(true);
}

void MainFrame::OnDoubleClick(wxMouseEvent &event)
{
    ToggleFullScreen();
    event.Skip();
}

void MainFrame::ToggleFullScreen()
{
    m_fullScreen = !m_fullScreen;
    ShowFullScreen(m_fullScreen, wxFULLSCREEN_ALL);
}

void MainFrame::OnTimer(wxTimerEvent &event)
{
    m_renderer->OnRefreshRealScreen();
    m_emulation->UpdatePad();
}

double round(double number)
{
    return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
}

void MainFrame::OnResize(wxSizeEvent &event)
{
    if (!m_toolBar || !m_renderer)
        return;
    
    wxSize toolBarSize = m_toolBar->GetSize();
    wxSize clientSize = GetClientSize();
    wxSize imageSize = clientSize;
    imageSize.y -= toolBarSize.y;
    int titleBarHeight = 22;
    int w = m_renderer->GetMinimunWidth();
    int h = m_renderer->GetMinimunHeight();
    
    float aspectRatio = (float)w / h;

    int magneticBorder = 20;
    int mod = imageSize.x % w;
    
    if (mod < magneticBorder)
        imageSize.x -= mod;
    else if (mod > (w-magneticBorder))
        imageSize.x += (w-mod);
    
    imageSize.y = round((float)imageSize.x / aspectRatio);
    
    int maxHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y)-(toolBarSize.y+titleBarHeight);
    
    if (imageSize.y > maxHeight)
        imageSize.y = maxHeight;
    
    if (!IsFullScreen())
        this->SetClientSize(wxSize(imageSize.x, imageSize.y+toolBarSize.y));
    
    this->Layout();
	if (m_renderer)
		m_renderer->OnRefreshRealScreen();
}

void MainFrame::OnMaximize(wxMaximizeEvent &event) {
    static int width=160, height=190;
    static bool maximized = false;
    static int posX=0, posY=0;
    
    if (maximized) {
        SetSize(width, height);
        SetPosition(wxPoint(posX, posY));
    }
    else {
        GetSize(&width, &height);
        GetPosition(&posX, &posY);
        event.Skip(true);
    }
    
    maximized = !maximized;
}

void MainFrame::OnChangeLanguage(wxCommandEvent &event) {
    int id = event.GetId();
    
    switch (id) {
        case ID_LANG_SYSTEM:
            SettingsSetLanguage(wxLANGUAGE_DEFAULT);
            break;
            
        case ID_LANG_ENGLISH:
            SettingsSetLanguage(wxLANGUAGE_ENGLISH);
            break;
            
        case ID_LANG_SPANISH:
            SettingsSetLanguage(wxLANGUAGE_SPANISH);
            break;
            
        case ID_LANG_FRENCH:
            SettingsSetLanguage(wxLANGUAGE_FRENCH);
            break;
            
        case ID_LANG_GREEK:
            SettingsSetLanguage(wxLANGUAGE_GREEK);
            break;
        
        case ID_LANG_GERMAN:
            SettingsSetLanguage(wxLANGUAGE_GERMAN);
            break;
            
        default:
            SettingsSetLanguage(wxLANGUAGE_ENGLISH);
            break;
    }
    
    SettingsSaveToFile();
    wxMessageBox(_("The language will change the next time you restart the application"), _("Language"));
}

void MainFrame::OnChangeView(wxCommandEvent &event) {
    m_renderer->OnChangeView();
}

void MainFrame::OnDebug(wxCommandEvent &event) {
    m_emulation->SetState(EmuState::Paused);
    
    VideoGameDevice *device = m_emulation->GetVideoGameDevice();
    if ((device->GetType() == DeviceType::MASTERSYSTEM) || (device->GetType() == DeviceType::GAMEGEAR)) {
        DebuggerSMSDialog debugger(this, device);
        debugger.ShowModal();
    }
    else if ((device->GetType() == DeviceType::GAMEBOY) || (device->GetType() == DeviceType::GAMEBOYCOLOR)) {
        DebuggerGBDialog debugger(this, device);
        debugger.ShowModal();
    }
	else if (device->GetType() == DeviceType::NES) {
		DebuggerNESDialog debugger(this, device);
		debugger.ShowModal();
	}
}
