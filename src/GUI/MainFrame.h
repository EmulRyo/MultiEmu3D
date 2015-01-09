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

#ifndef __MAINFRAME_H__
#define __MAINFRAME_H__

#define MAX_RECENT_FILES 10

#include <wx/frame.h>
#include <wx/timer.h>
#include <wx/sizer.h>
#include "../SMS-GG/Def.h"

class RendererBase;
class SettingsDialog;
class EmulationThread;

struct RecentFile
{
	wxString shortName;
	wxString fullName;
};

/*******************************************************************************
 // MainFrame Class
 *******************************************************************************/

class MainFrame : public wxFrame {
public:
	
    /**
     * Creates a new MainFrame.
     */
    MainFrame(wxString fileName);
	~MainFrame();
	
	void ChangeFile(const wxString fileName);
    void OnDoubleClick(wxMouseEvent &event);
    
protected:
	DECLARE_EVENT_TABLE()
    
private:
	wxMenuBar *m_mb;
	wxMenu *m_recentMenuFile;
	wxMenu *m_recentMenuPopup;
	wxToolBar *m_toolBar;
	wxTimer *m_timer;
    wxBoxSizer *m_mainSizer;
    bool m_fullScreen;
    RendererBase *m_renderer;
    int m_typeRenderer;
	SettingsDialog  *m_settingsDialog;
    EmulationThread *m_emulation;
    int m_lastWidth, m_lastHeight;
	
	RecentFile m_recentFiles[MAX_RECENT_FILES];
	int m_numRecentFiles;
    
    /**
     * Called when exit from the file menu is selected.
     */
    void OnFileExit(wxCommandEvent &);
	void OnFileOpen(wxCommandEvent &);
	void OnRecent(wxCommandEvent &event);
	void OnRecentItem(wxCommandEvent &event);
	void OnClearRecent(wxCommandEvent &event);
	void OnLoadState(wxCommandEvent &event);
	void OnSaveState(wxCommandEvent &event);
	void OnSettings(wxCommandEvent &);
    void OnFullScreen(wxCommandEvent &);
	void OnAbout(wxCommandEvent &);
	void OnPlay(wxCommandEvent &);
	void OnPause(wxCommandEvent &);
	void OnStop(wxCommandEvent &);
    void OnDebug(wxCommandEvent &);
    void OnChangeView(wxCommandEvent &event);
	void OnPlayUpdateUI(wxUpdateUIEvent& event);
	void OnPauseUpdateUI(wxUpdateUIEvent& event);
	void OnStopUpdateUI(wxUpdateUIEvent& event);
    void OnDebugUpdateUI(wxUpdateUIEvent& event);
	void OnLoadStateUpdateUI(wxUpdateUIEvent& event);
	void OnSaveStateUpdateUI(wxUpdateUIEvent& event);
    void OnFullScreenUpdateUI(wxUpdateUIEvent& event);
    void OnRefreshScreen(wxCommandEvent& evt);
    void OnTimer(wxTimerEvent &event);
	void CreateMenuBar();
	void CreateToolBar();
	void CreateRecentMenu(std::string * roms);
	void RecentRomsToSettings();
	void UpdateRecentMenu(wxString fileName);
    void ToggleFullScreen();
    void ChangeRenderer();
	void OnClose(wxCloseEvent&);
    void OnResize(wxSizeEvent &event);
    void OnMaximize(wxMaximizeEvent &event);
    void OnChangeLanguage(wxCommandEvent &event);
};

#endif
