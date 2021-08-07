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

#ifndef __SOUND_H__
#define __SOUND_H__

#include "../Common/Types.h"

class Basic_Gb_Apu;
#ifdef _WINDOWS
class SoundSDL;
#else
class SoundPortaudio;
#endif

namespace GameBoy {
    
    class Sound {
    private:
        Basic_Gb_Apu * apu;
#ifdef _WINDOWS
        SoundSDL * sound;
#else
        SoundPortaudio * sound;
#endif
        bool m_initialized;
        bool m_enabled;
        long m_sampleRate;
        
        int HandleError( const char* str );
    public:
        Sound();
        ~Sound();
        
        int ChangeSampleRate(long newSampleRate);
        int Start();
        int Stop();
        bool GetEnabled();
        void SetEnabled(bool enabled);
        void WriteRegister(u16 address, u8 value);
        u8 ReadRegister(u16 address);
        void EndFrame();
    };

}
#endif
