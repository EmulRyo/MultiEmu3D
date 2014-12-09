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
#include "Rewind.h"

enum enumEmuStates { NotStartedYet, Stopped, Paused, Playing };
enum EnumSpeed { SpeedNormal, SpeedMax };

class Cartridge;
class Video;
class Sound;
class Pad;
class CPU;
class Debugger;
class Joystick;
class ISMSScreenDrawable;
class wxMutex;

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
    void SetScreen(ISMSScreenDrawable *screen);
    void UpdatePad();
    Debugger *GetDebugger();
    void SetSpeed(EnumSpeed speed);
    
    enumEmuStates GetState();
    void SetState(enumEmuStates state);
    bool Finished();
    
private:
	Video *video;
	Sound *sound;
    Pad *pad;
	Cartridge *cartridge;
    CPU *cpu;
    Debugger *debugger;
    Joystick *joystick;
    wxMutex *mutex;
    wxStopWatch swFrame;
    wxKeyCode keysUsed[13];
    bool m_finished;
    EnumSpeed m_speed;
    bool m_soundEnabled;
    ISMSScreenDrawable *m_screen;
    
    Rewind m_rewind;
    
	enumEmuStates emuState;
    
    void LoadZip(const wxString zipPath, u8 **buffer, unsigned long *size, bool *gg);
    void PadSetKeys(int* keys1, int* keys2);
    void UpdateRewindScreen();
    void SetRewindPosition();
};

#endif
