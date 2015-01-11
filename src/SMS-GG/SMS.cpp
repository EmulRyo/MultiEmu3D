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
#include "SMS.h"

using namespace MasterSystem;

SMS::SMS() {
    m_sound = new Sound();
    m_video = new Video(NULL);
    m_pad = new Pad();
    m_cpu = new CPU(m_video, m_pad, m_sound);
    m_cartridge = NULL;
    m_debugger = new Debugger(m_sound, m_video, m_cpu, m_cartridge, m_pad);
}

SMS::~SMS() {
    delete m_cpu;
    delete m_video;
    delete m_sound;
    delete m_pad;
    delete m_debugger;
    if (m_cartridge)
        delete m_cartridge;
}

bool SMS::SoundIsEnabled() {
    return m_sound->GetEnabled();
}

void SMS::SoundEnable(bool value) {
    m_sound->SetEnabled(value);
}

void SMS::SoundSetSampleRate(long sampleRate) {
    m_sound->ChangeSampleRate(sampleRate);
}

void SMS::Reset() {
    m_cpu->Reset();
}

void SMS::ExecuteOneFrame() {
    m_cpu->ExecuteOneFrame();
}

void SMS::CartridgeExtract() {
    if (m_cartridge)
        m_cartridge->Extract();
}

void SMS::CartridgeLoad(std::string fileName, std::string batteriesPath, u8 *cartridgeBuffer, unsigned long size) {
    if (m_cartridge)
        delete m_cartridge;
    
    m_cartridge = new Cartridge(fileName, batteriesPath, cartridgeBuffer, size);
    
    m_cpu->SetCartridge(m_cartridge);
    
    m_cpu->Reset();
    
    delete m_debugger;
    m_debugger = new Debugger(m_sound, m_video, m_cpu, m_cartridge, m_pad);
}

void SMS::SetGGMode(bool value) {
    m_cpu->SetGGMode(value);
}

void SMS::LoadState(std::string fileName, int id) {
    m_cpu->LoadState(fileName, id);
}

void SMS::SaveState(std::string fileName, int id) {
    m_cpu->SaveState(fileName, id);
}

void SMS::PadSetButtons(bool *buttonsState, bool pause) {
    m_pad->SetButtonsStatePad1(buttonsState);
    m_pad->SetButtonsStatePad2(&buttonsState[6]);
    m_pad->SetPauseState(pause);
}

void SMS::SetScreen(ISMSScreenDrawable *screen) {
    m_video->SetScreen(screen);
}

CPU *SMS::GetCPU() {
    return m_cpu;
}

Debugger *SMS::GetDebugger() {
    return m_debugger;
}
