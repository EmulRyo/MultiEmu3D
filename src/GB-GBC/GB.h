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

#ifndef __GB_H__
#define __GB_H__

class IScreenDrawable;

namespace GameBoy {
    class Cartridge;
    class Video;
    class Sound;
    class Pad;
    class CPU;
    class Debugger;

    class GB {
    public:
        GB();
        ~GB();
        
        void Reset();
        void ExecuteOneFrame();
        void CartridgeExtract();
        void CartridgeLoad(std::string fileName, std::string batteriesPath="", unsigned char *cartridgeBuffer=NULL, unsigned long size=0);
        bool SoundIsEnabled();
        void SoundEnable(bool value);
        void SoundSetSampleRate(long sampleRate);
        void LoadState(std::string fileName, int id);
        void SaveState(std::string fileName, int id);
        void PadSetButtons(bool *buttonsState, bool pause);
        void SetScreen(IScreenDrawable *screen);
        
        CPU *GetCPU();
        Debugger *GetDebugger();
        
    private:
        Video     *m_video;
        Sound     *m_sound;
        Pad       *m_pad;
        Cartridge *m_cartridge;
        CPU       *m_cpu;
        Debugger  *m_debugger;
    };
}

#endif
