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
class RendererBase;
class VideoGameDevice;
class EmulationThread;

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
    RendererBase* m_renderer;
    EmulationThread* m_emulation;
    Font m_font;
    int m_fontSize;
    std::string m_messageError;

    void ChangeFile(const std::string& fileName);
    void UpdateRecentMenu(const std::string& fileName);
    void DrawMenuBar(Rectangle dst);
    void DrawToolBar(Rectangle dst);
    void DrawStatusBar(Rectangle dst);
    void ShowErrorMessageBox(float winWidth, float winHeight);

    void OnOpenFileUI();
    void OnPlayUI();
    void OnPauseUI();
    void OnStopUI();
};

#endif
