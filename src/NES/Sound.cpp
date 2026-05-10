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

#include <iostream>
#ifdef _WINDOWS
#include "../Common/SoundSDL.h"
#else
#include "../Common/SoundPortaudio.h"
#endif
#include "Sound.h"

using namespace std;
using namespace Nes;

enum SoundError { ERROR, NO_ERROR };

int Sound::HandleError( const char* str )
{
	if ( str )
	{
		cerr << "Error: " << str << endl;
		return ERROR;
	}
	else
		return NO_ERROR;
}

Sound::Sound()
{
	m_enabled = false;
	m_initialized = true;
	m_sampleRate = 44100;
	m_frameIRQFlag = false;
	m_frameIRQInhibit = false;
	m_frameFiveStepMode = false;
	m_lastCyclesElapsed = 0;
	m_totalCycles = 0;
	m_frameCounterResetCycle = 0;

#ifdef _WINDOWS
	m_sound = new SoundSDL();
#else
    m_sound = new SoundPortaudio();
#endif
	
	if (ChangeSampleRate(m_sampleRate) == ERROR)
	{
		m_initialized = false;
		return;
	}
	
	if (Start() == ERROR)
	{
		m_initialized = false;
		return;
	}
}

Sound::~Sound()
{
    delete m_sound;
}

int Sound::ChangeSampleRate(long newSampleRate)
{
	if (!m_initialized)
		return NO_ERROR;
	
	m_sampleRate = newSampleRate;
	bool wasEnabled = m_enabled;
	
	if (wasEnabled)
		Stop();
	
	if (wasEnabled)
	{
		if (Start() == ERROR)
			return ERROR;
	}
	
	return NO_ERROR;
}

int Sound::Start()
{
	return NO_ERROR;

	if (!m_initialized)
		return NO_ERROR;
	
	if (!m_enabled)
	{
		// Generate a few seconds of sound and play using SDL
		if (m_sound->Start(m_sampleRate, 2) == false)
			return ERROR;
	}
	m_enabled = true;
	
	return NO_ERROR;
}

int Sound::Stop()
{
	if (!m_initialized)
		return NO_ERROR;
	
	if (m_enabled)
		m_sound->Stop();
	
	m_enabled = false;
	
	return NO_ERROR;
}

bool Sound::GetEnabled()
{
	return m_enabled;
}

void Sound::SetEnabled(bool enabled)
{
	if (enabled)
		Start();
	else
		Stop();
}

void Sound::EndFrame(u32 cyclesElapsed) {
    
	if ((!m_initialized) || (!m_enabled))
		return;
}

u8 Sound::MemR(u16 address) {
	if (address == 0x4015) {
		u8 value = m_frameIRQFlag ? 0x40 : 0x00;
		m_frameIRQFlag = false;
		return value;
	}

	return 0;
}

void Sound::MemW(u16 address, u8 value, u32 cyclesElapsed) {
	Sync(cyclesElapsed);

	if (address == 0x4017) {
		m_frameFiveStepMode = (value & 0x80) != 0;
		m_frameIRQInhibit = (value & 0x40) != 0;
		m_frameCounterResetCycle = m_totalCycles;
		if (m_frameIRQInhibit)
			m_frameIRQFlag = false;
	}
}

bool Sound::IRQ(u32 cyclesElapsed) {
	Sync(cyclesElapsed);

	if (!m_frameFiveStepMode && !m_frameIRQInhibit && (m_totalCycles - m_frameCounterResetCycle) >= 29828)
		m_frameIRQFlag = true;

	return m_frameIRQFlag && !m_frameIRQInhibit;
}

void Sound::Sync(u32 cyclesElapsed) {
	if (cyclesElapsed >= m_lastCyclesElapsed)
		m_totalCycles += cyclesElapsed - m_lastCyclesElapsed;
	else
		m_totalCycles += cyclesElapsed;

	m_lastCyclesElapsed = cyclesElapsed;
}
