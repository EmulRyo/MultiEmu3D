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

#ifndef __EMULATIONTHREAD_H__
#define __EMULATIONTHREAD_H__

#include <mutex>
#include <chrono>
#include "../Common/IScreenDrawable.h"

enum class EmuState { NotStartedYet, Stopped, Paused, Playing };
enum class EmuSpeed { Normal, Max };

//class Joystick;
//class Rewind;
class VideoGameDevice;
class std::thread;

class EmulationThread
{
public:
	EmulationThread();
    ~EmulationThread();
    
    //virtual ExitCode Entry();
    
    bool ChangeFile(const std::string &fileName);
    void LoadState(const std::string& fileName, int id);
    void SaveState(const std::string& fileName, int id);
    void ApplySettings();
    void SetScreen(IScreenDrawable *screen);
    void UpdatePad();
    VideoGameDevice *GetVideoGameDevice();
    void SetSpeed(EmuSpeed speed);
    
    EmuState GetState();
    float GetFPS();
    void SetState(EmuState state);
    void Exit();
    bool Finished();
    
private:
    VideoGameDevice *m_device;
    //Joystick* joystick;
    std::thread* m_thread;
    std::mutex m_mutex;
    std::atomic<bool> m_threadShouldClose;
    int keysUsed[16];
    bool m_finished;
    EmuSpeed m_speed;
    bool m_soundEnabled;
    float m_fps;
    IScreenDrawable *m_screen;
    //Rewind *m_rewind;
    
	EmuState m_emuState;
    
    void Entry();
    void ApplySettingsNoMutex();
    void SetScreenNoMutex(IScreenDrawable *screen);
    void LoadZip(const std::string &zipPath, u8 **buffer, unsigned long *size, std::string &extension);
    void PadSetKeys(int* keys);
    void UpdateRewindScreen();
    void SetRewindPosition();

    long long ElapsedMicroSeconds(std::chrono::steady_clock::time_point begin, std::chrono::steady_clock::time_point end);
    long long ElapsedMilliSeconds(std::chrono::steady_clock::time_point begin, std::chrono::steady_clock::time_point end);
};

#endif
