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

#include <iostream>
// Definir la siguiente linea para que en Visual Studio no haya conflicto
// entre SDL y Sms_Snd_Emu al definir tipos basicos
#define BLARGG_COMPILER_HAS_NAMESPACE 1
#include "Sms_Apu.h"
#ifdef __WXMSW__
#include "SoundSDL.h"
#else
#include "SoundPortaudio.h"
#endif
#include "Sound.h"

using namespace std;

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

#ifdef __WXMSW__
	m_sound = new SoundSDL();
#else
    m_sound = new SoundPortaudio();
#endif
    
    m_apu = new Sms_Apu();
    m_buf = new Blip_Buffer();
	
	if (ChangeSampleRate(m_sampleRate) == ERROR)
	{
		m_initialized = false;
		return;
	}
    
    m_buf->clock_rate(3579545);
    m_apu->output(m_buf);
	
	if (Start() == ERROR)
	{
		m_initialized = false;
		return;
	}
}

Sound::~Sound()
{
    delete m_sound;
    delete m_apu;
    delete m_buf;
}

int Sound::ChangeSampleRate(long newSampleRate)
{
	if (!m_initialized)
		return NO_ERROR;
	
	m_sampleRate = newSampleRate;
	bool wasEnabled = m_enabled;
	
	if (wasEnabled)
		Stop();
	
	// Set sample rate and check for out of memory error
	if (!m_buf->sample_rate(m_sampleRate))
		return ERROR;
	
	if (wasEnabled)
	{
		if (Start() == ERROR)
			return ERROR;
	}
	
	return NO_ERROR;
}

int Sound::Start()
{
	if (!m_initialized)
		return NO_ERROR;
	
	if (!m_enabled)
	{
		// Generate a few seconds of sound and play using SDL
		if (m_sound->Start(m_sampleRate, 1) == false)
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
	
	m_apu->end_frame(cyclesElapsed);
    m_buf->end_frame(cyclesElapsed);
	
	int const bufSize = m_buf->samples_avail();
	blip_sample_t *buf = new blip_sample_t[bufSize];
	
    // Play whatever samples are available
    long count = m_buf->read_samples(buf, bufSize);
    
    m_sound->Write(buf, count);

	delete[] buf;
}

void Sound::WritePort(u8 port, u8 value, u32 cyclesElapsed) {
    if (m_enabled) {
        if (port == 0x06)
            m_apu->write_ggstereo(cyclesElapsed, value);
        else if ((port & 0xC0) == 0x40)
            m_apu->write_data(cyclesElapsed, value);
    }
}
