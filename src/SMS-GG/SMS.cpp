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
#include "../Common/IScreenDrawable.h"

using namespace MasterSystem;

SMS::SMS() {
    SetType(MASTERSYSTEM);
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

void SMS::CartridgeLoad(const std::string &fileName, const std::string &batteriesPath,
                        u8 *cartridgeBuffer, unsigned long size) {
    if (m_cartridge)
        delete m_cartridge;
    
    m_cartridge = new Cartridge(fileName, batteriesPath, cartridgeBuffer, size);
    m_cpu->SetCartridge(m_cartridge);
    m_cpu->Reset();
    
    std::string fileNameLower = fileName;
    std::transform(fileNameLower.begin(), fileNameLower.end(), fileNameLower.begin(), ::tolower);
    if (EndsWith(fileNameLower, "gg"))
        SetType(GAMEGEAR);
    else
        SetType(MASTERSYSTEM);
    
    m_cpu->SetGGMode(GetType() == GAMEGEAR);
    
    delete m_debugger;
    m_debugger = new Debugger(m_sound, m_video, m_cpu, m_cartridge, m_pad);
}

void SMS::SetExtraData(const std::string &key, void *value) {
    if (key == "GGMode") {
        bool gg = *(bool *)value;
        SetType(gg ? GAMEGEAR : MASTERSYSTEM);
        m_cpu->SetGGMode(gg);
    }
}

void SMS::LoadState(const std::string &fileName, int id) {
    m_cpu->LoadState(fileName, id);
}

void SMS::SaveState(const std::string &fileName, int id) {
    m_cpu->SaveState(fileName, id);
}

void SMS::LoadStateFromRAM(std::istream *stream) {
    m_cpu->LoadStateFromRAM(stream);
}

void SMS::SaveStateToRAM(std::ostream *stream) {
    m_cpu->SaveStateToRAM(stream);
}

void SMS::PadSetButtons(bool *buttonsState) {
    m_pad->SetButtonsStatePad1(buttonsState);
    m_pad->SetButtonsStatePad2(&buttonsState[6]);
    m_pad->SetPauseState(buttonsState[12]);
}

int SMS::PadIdAcceptButton() {
    return B1;
}

int SMS::PadIdCancelButton() {
    return B2;
}

int SMS::PadIdLeftButton() {
    return LEFT;
}

int SMS::PadIdRightButton() {
    return RIGHT;
}

void SMS::SetScreen(IScreenDrawable *screen) {
    m_video->SetScreen(screen);
}

int SMS::GetWidth() {
    return SMS_SCREEN_W;
}

int SMS::GetHeight() {
    return SMS_SCREEN_H;
}

Debugger *SMS::GetDebugger() {
    return m_debugger;
}

bool SMS::IsValidExtension(const std::string &extension) {
    if ((extension == "sms") || (extension == "gg"))
        return true;
    else
        return false;
}

bool SMS::EndsWith(const std::string &fullString, const std::string &ending) {
    if (fullString.length() >= ending.length())
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    else
        return false;
}
