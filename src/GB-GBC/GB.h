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

#include "../Common/VideoGameDevice.h"

class IScreenDrawable;

namespace GameBoy{
    class Cartridge;
    class Video;
    class Sound;
    class Pad;
    class CPU;
    class Debugger;

    class GB: public VideoGameDevice {
    public:
        GB();
        ~GB();
        
        void Reset();
        void ExecuteOneFrame();
        void CartridgeExtract();
        void CartridgeLoad(const std::string &fileName, const std::string &batteriesPath="",
                           unsigned char *cartridgeBuffer=NULL, unsigned long size=0);
        bool SoundIsEnabled();
        void SoundEnable(bool value);
        void SoundSetSampleRate(long sampleRate);
        void LoadState(const std::string &fileName, int id);
        void SaveState(const std::string &fileName, int id);
        void LoadStateFromRAM(std::istream *stream);
        void SaveStateToRAM(std::ostream *stream);
        int  PadGetNumButtons();
        void PadSetButtons(bool *buttonsState);
        int  PadIdAcceptButton();
        int  PadIdCancelButton();
        int  PadIdLeftButton();
        int  PadIdRightButton();
        void SetScreen(IScreenDrawable *screen);
        int  GetWidth();
        int  GetHeight();
        void SetExtraData(const std::string &key, void *value);
        
        Debugger *GetDebugger();
        
        static bool IsValidExtension(const std::string &extension);
        
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
