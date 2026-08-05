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

#ifndef __MAINFRAME_H__
#define __MAINFRAME_H__

#include <string>
#include "raylib.h"
#include "EmulationThread.h"
#include "MenuBar.h"

#define MAX_RECENT_FILES 10

class RendererBase;
class Renderer3D;
class VideoGameDevice;
class Dialog;
class MessageBoxDialog;
class DebuggerDialog;
class SettingsDialog;

struct RecentFile
{
    std::string shortName;
    std::string fullName;
};

/*******************************************************************************
 // MainFrame Class
 *******************************************************************************/

class MainFrame {
public:
	
    /**
     * Creates a new MainFrame.
     */
    MainFrame(const std::string &fileName);
	~MainFrame();

    void Update(float deltaTime);
    void Draw(Rectangle dst);
    
private:
    RendererBase*     m_renderer    = nullptr;
    EmulationThread*  m_emulation   = nullptr;
    MessageBoxDialog* m_msgBoxDlg   = nullptr;
    DebuggerDialog*   m_debuggerDlg = nullptr;
    SettingsDialog*   m_settingsDlg = nullptr;
    Font m_font;
    int m_fontSize;
    MenuBar m_menuBar;
    int m_windowWidth;
    int m_windowHeight;
    bool m_recentMenuOpened;
    RecentFile m_recentFiles[MAX_RECENT_FILES];
    int m_numRecentFiles;
    EmuState m_prevState;

    void CreateMenuBar();
    void ChangeFile(const std::string& fileName);
    void UpdateRecentMenu(const std::string* fileNames);
    void UpdateRecentMenu(const std::string& fileName);
    void DrawToolBar(Rectangle dst);
    void DrawStatusBar(Rectangle dst);
    template <typename T, typename... Targs>
    void NewDialog(T** dialog, Targs... args);
    void RecentRomsToSettings();
    void SetStyle();
    void LoadFont(std::string_view language);

    void OnOpenFileUI();
    void OnRecentUI();
    void OnOpenRecentUI(int id);
    void OnLoadStateUI(int id);
    void OnSaveStateUI(int id);
    void OnPlayUI();
    void OnPauseUI();
    void OnStopUI();
    void OnSettingsUI();
    void OnDebugUI();
    void OnFullscreenUI();
    void OnLanguageUI(int id);
    void OnExitUI();
    void OnSettingsClosed();
};

#endif
