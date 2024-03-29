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

#ifndef __NES_SOUND_H__
#define __NES_SOUND_H__

#include "../Common/Types.h"

#ifdef _WINDOWS
class SoundSDL;
#else
class SoundPortaudio;
#endif

namespace Nes {
    class Sound
    {
    private:
    #ifdef _WINDOWS
        SoundSDL *m_sound;
    #else
        SoundPortaudio *m_sound;
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
        u8   MemR(u16 address);
        void MemW(u16 address, u8 value, u32 cyclesElapsed);
        void EndFrame(u32 cyclesElapsed);
    };
}

#endif
