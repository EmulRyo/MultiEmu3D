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

#include "CPU.h"
#include "Sound.h"
#include "Video.h"
#include "Pad.h"
#include "Cartridge.h"
#include "Debugger.h"
#include "GB.h"

using namespace GameBoy;

GB::GB() {
    m_sound = new Sound();
    m_video = new Video(NULL);
    m_pad = new Pad();
    m_cpu = new CPU(m_video, m_pad, m_sound);
    m_cartridge = NULL;
    m_debugger = new Debugger(m_sound, m_video, m_cpu, m_cartridge);
}

GB::~GB() {
    delete m_cpu;
    delete m_video;
    delete m_sound;
    delete m_pad;
    delete m_debugger;
    if (m_cartridge)
        delete m_cartridge;
}

bool GB::SoundIsEnabled() {
    return m_sound->GetEnabled();
}

void GB::SoundEnable(bool value) {
    m_sound->SetEnabled(value);
}

void GB::SoundSetSampleRate(long sampleRate) {
    m_sound->ChangeSampleRate(sampleRate);
}

void GB::Reset() {
    m_cpu->Reset();
}

void GB::ExecuteOneFrame() {
    m_cpu->ExecuteOneFrame();
}

void GB::CartridgeExtract() {
    if (m_cartridge)
        m_cartridge->Extract();
}

void GB::CartridgeLoad(std::string fileName, std::string batteriesPath, u8 *cartridgeBuffer, unsigned long size) {
    if (m_cartridge)
        delete m_cartridge;
    
    if ((cartridgeBuffer != NULL) && (size > 0))
        m_cartridge = new Cartridge(cartridgeBuffer, size, batteriesPath);
    else
        m_cartridge = new Cartridge(fileName, batteriesPath);
    
    m_cpu->LoadCartridge(m_cartridge);
    m_cpu->Reset();
    
    delete m_debugger;
    m_debugger = new Debugger(m_sound, m_video, m_cpu, m_cartridge);
}

void GB::LoadState(std::string fileName, int id) {
    m_cpu->LoadState(fileName, id);
}

void GB::SaveState(std::string fileName, int id) {
    m_cpu->SaveState(fileName, id);
}

void GB::PadSetButtons(bool *buttonsState, bool pause) {
    /*
    m_pad->SetButtonsStatePad1(buttonsState);
    m_pad->SetButtonsStatePad2(&buttonsState[6]);
    m_pad->SetPauseState(pause);
    */
}

void GB::SetScreen(IScreenDrawable *screen) {
    m_video->SetScreen(screen);
}

GameBoy::CPU *GB::GetCPU() {
    return m_cpu;
}

Debugger *GB::GetDebugger() {
    return m_debugger;
}
