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

#include <algorithm>
#include "CPU.h"
#include "Sound.h"
#include "Video.h"
#include "Pad.h"
#include "Cartridge.h"
#include "Debugger.h"
#include "NES.h"
#include "../Common/IScreenDrawable.h"

using namespace Nes;

NES::NES() {
    SetType(DeviceType::NES);
    m_sound = new Sound();
    m_video = new Video(NULL);
    m_pad = new Pad();
    m_cpu = new CPU(m_video, m_pad, m_sound);
    m_cartridge = NULL;
    m_debugger = new Debugger(m_sound, m_video, m_cpu, m_cartridge, m_pad);
}

NES::~NES() {
    delete m_cpu;
    delete m_video;
    delete m_sound;
    delete m_pad;
    delete m_debugger;
    if (m_cartridge)
        delete m_cartridge;
}

bool NES::SoundIsEnabled() {
    return m_sound->GetEnabled();
}

void NES::SoundEnable(bool value) {
    m_sound->SetEnabled(value);
}

void NES::SoundSetSampleRate(long sampleRate) {
    m_sound->ChangeSampleRate(sampleRate);
}

void NES::Reset() {
    m_cpu->Reset();
}

void NES::ExecuteOneFrame() {
    m_cpu->ExecuteOneFrame();
}

void NES::CartridgeExtract() {
    if (m_cartridge)
        m_cartridge->Extract();
}

void NES::CartridgeLoad(const std::string &fileName, const std::string &batteriesPath,
                        u8 *cartridgeBuffer, unsigned long size) {
    if (m_cartridge)
        delete m_cartridge;
    
    m_cartridge = new Cartridge(fileName, batteriesPath, cartridgeBuffer, size);
    m_cpu->SetCartridge(m_cartridge);
    m_cpu->Reset();

    m_video->SetCartridge(m_cartridge);
    
    std::string fileNameLower = fileName;
    std::transform(fileNameLower.begin(), fileNameLower.end(), fileNameLower.begin(), ::tolower);
    SetType(DeviceType::NES);
    
    delete m_debugger;
    m_debugger = new Debugger(m_sound, m_video, m_cpu, m_cartridge, m_pad);
}

void NES::SetExtraData(const std::string &key, void *value) {
    
}

void NES::LoadState(const std::string &fileName, int id) {
    m_cpu->LoadState(fileName, id);
}

void NES::SaveState(const std::string &fileName, int id) {
    m_cpu->SaveState(fileName, id);
}

void NES::LoadStateFromRAM(std::istream *stream) {
    m_cpu->LoadStateFromRAM(stream);
}

void NES::SaveStateToRAM(std::ostream *stream) {
    m_cpu->SaveStateToRAM(stream);
}

int NES::PadGetNumButtons() {
    return 13;
}

void NES::PadSetButtons(bool *buttonsState) {
    m_pad->SetButtonsStatePad1(buttonsState);
    m_pad->SetButtonsStatePad2(&buttonsState[6]);
    m_pad->SetPauseState(buttonsState[12]);
}

int NES::PadIdAcceptButton() {
    return B1;
}

int NES::PadIdCancelButton() {
    return B2;
}

int NES::PadIdLeftButton() {
    return LEFT;
}

int NES::PadIdRightButton() {
    return RIGHT;
}

void NES::SetScreen(IScreenDrawable *screen) {
    m_video->SetScreen(screen);
}

int NES::GetWidth() {
    return NES_SCREEN_W;
}

int NES::GetHeight() {
    return NES_SCREEN_H;
}

Debuggable *NES::GetDebugger() {
    return m_debugger;
}

bool NES::IsValidExtension(const std::string &extension) {
    return (extension == "nes");
}
