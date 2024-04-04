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

#include <fstream>
#include <exception>
#include "Settings.h"
#include "AppDefs.h"
#include "raylib.h"
#include "json.hpp"

static int  _renderMethod    = 1;
static bool _greenScale      = false;
static int  _windowZoom      = 1;
static bool _soundEnabled    = true;
static int  _soundSampleRate = 44100;
static std::string _language = "en";
                               //    Up,     Down,     Left,     Right,     A,     B,          Select,     Start
static int  _gbKeys[8]       = { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_A, KEY_S, KEY_RIGHT_SHIFT, KEY_ENTER };
                               //    Up,     Down,     Left,     Right,     1,     2,    Up,  Down,  Left, Right,     1,     2, Start/Pause
static int  _smsKeys[13]     = { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_A, KEY_S, KEY_I, KEY_J, KEY_K, KEY_L, KEY_G, KEY_H, KEY_ENTER };
                               //    Up,     Down,     Left,     Right,     A,     B,          Select,     Start,    Up,  Down,  Left, Right,     A,     B,Select, Start
static int  _nesKeys[16]     = { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_A, KEY_S, KEY_RIGHT_SHIFT, KEY_ENTER, KEY_I, KEY_K, KEY_J, KEY_L, KEY_G, KEY_H, KEY_O, KEY_P };
static std::string _recentRoms[10];


int Settings::GetRenderMethod() {
	return _renderMethod;
}

void Settings::SetRenderMethod(int renderMethod) {
	_renderMethod = renderMethod;
}

bool Settings::GetGreenScale() {
	return _greenScale;
}

void Settings::SetGreenScale(bool greenScale) {
	_greenScale = greenScale;
}

int Settings::GetWindowZoom() {
	return _windowZoom;
}

void Settings::SetWindowZoom(int windowZoom) {
	_windowZoom = windowZoom;
}

bool Settings::GetSoundEnabled() {
	return _soundEnabled;
}

void Settings::SetSoundEnabled(bool enabled) {
	_soundEnabled = enabled;
}

int Settings::GetSoundSampleRate() {
	return _soundSampleRate;
}

void Settings::SetSoundSampleRate(int sampleRate) {
	_soundSampleRate = sampleRate;
}

std::string_view Settings::GetLanguage() {
	return _language;
}

void Settings::SetLanguage(std::string_view language) {
	_language = std::string(language);
}

int* Settings::GetInput(DeviceType type) {
    switch (type) {
        case DeviceType::MASTERSYSTEM:
        case DeviceType::GAMEGEAR:
            return &_smsKeys[0];
        case DeviceType::GAMEBOY:
        case DeviceType::GAMEBOYCOLOR:
            return &_gbKeys[0];
        case DeviceType::NES:
            return &_nesKeys[0];
        default:
            return NULL;
    }
}

void Settings::SetInput(DeviceType type, const int* padKeys) {
    switch (type) {
        case DeviceType::MASTERSYSTEM:
        case DeviceType::GAMEGEAR:
            for (int i=0; i<13; i++)
                _smsKeys[i] = padKeys[i];
            break;
        case DeviceType::GAMEBOY:
        case DeviceType::GAMEBOYCOLOR:
            for (int i=0; i<8; i++)
                _gbKeys[i] = padKeys[i];
            break;
        case DeviceType::NES:
            for (int i = 0; i < 16; i++)
                _nesKeys[i] = padKeys[i];
            break;
    }
}

std::string* Settings::GetRecentRoms() {
	return &_recentRoms[0];
}

void Settings::SetRecentRoms(const std::string* recentRoms) {
	for (int i=0; i<10; i++)
        _recentRoms[i] = recentRoms[i];
}

void Settings::Save(const std::string& fileName) {
    nlohmann::json data;
    
    data["general"]["renderMethod"] = _renderMethod;
    data["general"]["greenScale"]   = _greenScale;
    data["general"]["windowZoom"]   = _windowZoom;
    data["general"]["language"]     = _language;
    
    data["sound"]["enabled"]        = _soundEnabled;
    data["sound"]["sampleRate"]     = _soundSampleRate;

    data["input"]["gb"]             = _gbKeys;
    data["input"]["sms"]            = _smsKeys;
    data["input"]["nes"]            = _nesKeys;

    data["recentRoms"]              = _recentRoms;

    std::string s = data.dump(4, ' ', false);
    std::ofstream f(fileName);
    f << s;
}

void Settings::Load(const std::string& fileName)
{
    std::ifstream f(fileName);
    nlohmann::json data;
    try {
        data = nlohmann::json::parse(f);
    }
    catch (const nlohmann::json::exception& e) {
        printf("%s\n", e.what());
        return;
    }

    if (data.contains("general")) {
        _renderMethod   = data["general"].value<int>("renderMethod", 1);
        _greenScale     = data["general"].value<bool>("greenScale", true);
        _windowZoom     = data["general"].value<int>("windowZoom", 1);
        _language       = data["general"].value<std::string>("language", "en");
    }

    if (data.contains("sound")) {
        _soundEnabled   = data["sound"].value<bool>("enabled", true);
        _soundSampleRate = data["sound"].value<int>("sampleRate", 44100);
    }

    if (data.contains("input")) {
        nlohmann::json& input = data["input"];
        if (input.contains("gb")) {
            nlohmann::json& gb = input["gb"];
            int numElements = gb.size() > 8 ? 8 : gb.size();
            for (int i=0; i<numElements; i++)
                _gbKeys[i] = gb[i];
        }
        if (input.contains("sms")) {
            nlohmann::json& sms = input["sms"];
            int numElements = sms.size() > 13 ? 13 : sms.size();
            for (int i = 0; i < numElements; i++)
                _smsKeys[i] = sms[i];
        }
        if (input.contains("nes")) {
            nlohmann::json& nes = input["nes"];
            int numElements = nes.size() > 16 ? 16 : nes.size();
            for (int i = 0; i < numElements; i++)
                _nesKeys[i] = nes[i];
        }
    }

    if (data.contains("recentRoms")) {
        nlohmann::json& recentRoms = data["recentRoms"];
        int numElements = recentRoms.size() > 10 ? 10 : recentRoms.size();
        for (int i = 0; i < numElements; i++)
            _recentRoms[i] = recentRoms[i];
    }
}