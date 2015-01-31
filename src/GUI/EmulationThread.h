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

#ifndef __EMULATIONTHREAD_H__
#define __EMULATIONTHREAD_H__

#include <wx/thread.h>
#include <wx/stopwatch.h>
#include "../Common/IScreenDrawable.h"

enum enumEmuStates { NotStartedYet, Stopped, Paused, Playing };
enum EnumSpeed { SpeedNormal, SpeedMax };

class Joystick;
class wxMutex;
class Rewind;
class VideoGameDevice;

class EmulationThread : public wxThread
{
public:
	EmulationThread();
    ~EmulationThread();
    
    virtual ExitCode Entry();
    
    bool ChangeFile(wxString fileName);
    void LoadState(std::string fileName, int id);
    void SaveState(std::string fileName, int id);
    void ApplySettings();
    void SetScreen(IScreenDrawable *screen);
    void UpdatePad();
    VideoGameDevice *GetVideoGameDevice();
    void SetSpeed(EnumSpeed speed);
    
    enumEmuStates GetState();
    void SetState(enumEmuStates state);
    bool Finished();
    
private:
    VideoGameDevice *m_device;
    Joystick *joystick;
    wxMutex *mutex;
    wxStopWatch swFrame;
    wxKeyCode keysUsed[13];
    bool m_finished;
    EnumSpeed m_speed;
    bool m_soundEnabled;
    IScreenDrawable *m_screen;
    Rewind *m_rewind;
    
	enumEmuStates emuState;
    
    void ApplySettingsNoMutex();
    void SetScreenNoMutex(IScreenDrawable *screen);
    void LoadZip(const wxString &zipPath, unsigned char **buffer, unsigned long *size, wxString &extension);
    void PadSetKeys(int* keys);
    void UpdateRewindScreen();
    void SetRewindPosition();
};

#endif
