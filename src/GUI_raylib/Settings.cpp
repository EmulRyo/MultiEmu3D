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

#include "Settings.h"
#include "AppDefs.h"
#include "raylib.h"

Settings settings;

Settings::Settings()
{
    renderMethod = 1;
	greenScale	 = false;
	windowZoom	 = 1;
	soundEnabled	= true;
	soundSampleRate = 44100;
    language = 0;
	
	smsKeys[ 0]	= KEY_UP;	// Up
	smsKeys[ 1]	= KEY_DOWN; // Down
	smsKeys[ 2]	= KEY_LEFT; // Left
	smsKeys[ 3]	= KEY_RIGHT;// Right
	smsKeys[ 4]	= KEY_A;    // 1
	smsKeys[ 5]	= KEY_S;	// 2
    
    smsKeys[ 6]	= KEY_I;    // Up
    smsKeys[ 7]	= KEY_K;    // Down
    smsKeys[ 8]	= KEY_J;    // Left
    smsKeys[ 9]	= KEY_L;    // Right
    smsKeys[10]	= KEY_G;    // 1
    smsKeys[11]	= KEY_H;    // 2
    
	smsKeys[12] = KEY_ENTER; // Start / Pause
    
    gbKeys[0]	= KEY_UP;	    // Up
    gbKeys[1]	= KEY_DOWN;     // Down
    gbKeys[2]	= KEY_LEFT;     // Left
    gbKeys[3]	= KEY_RIGHT;    // Right
    gbKeys[4]	= KEY_A;        // A
    gbKeys[5]	= KEY_S;	    // B
    gbKeys[6]	= KEY_RIGHT_SHIFT;  // Select
    gbKeys[7]	= KEY_ENTER;    // Start

    nesKeys[ 0] = KEY_UP;	    // Up
    nesKeys[ 1] = KEY_DOWN;     // Down
    nesKeys[ 2] = KEY_LEFT;     // Left
    nesKeys[ 3] = KEY_RIGHT;    // Right
    nesKeys[ 4] = KEY_A;        // A
    nesKeys[ 5] = KEY_S;	    // B
    nesKeys[ 6] = KEY_RIGHT_SHIFT;  // Select
    nesKeys[ 7] = KEY_ENTER;    // Start
    
    nesKeys[ 8] = KEY_I; // Up
    nesKeys[ 9] = KEY_K; // Down
    nesKeys[10] = KEY_J; // Left
    nesKeys[11] = KEY_L; // Right
    nesKeys[12] = KEY_G; // A
    nesKeys[13] = KEY_H; // B
    nesKeys[14] = KEY_O; // Select
    nesKeys[15] = KEY_P; // Start
}

Settings SettingsGetCopy() {
	return settings;
}

void SettingsSetNewValues(Settings newSettings) {
	settings = newSettings;
}

int SettingsGetRenderMethod() {
	return settings.renderMethod;
}

void SettingsSetGreenScale(int renderMethod) {
	settings.renderMethod = renderMethod;
}

bool SettingsGetGreenScale() {
	return settings.greenScale;
}

void SettingsSetGreenScale(bool greenScale) {
	settings.greenScale = greenScale;
}

int SettingsGetWindowZoom() {
	return settings.windowZoom;
}

void SettingsSetWindowZoom(int windowZoom) {
	settings.windowZoom = windowZoom;
}

bool SettingsGetSoundEnabled() {
	return settings.soundEnabled;
}

void SettingsSetSoundEnabled(bool enabled) {
	settings.soundEnabled = enabled;
}

int SettingsGetSoundSampleRate() {
	return settings.soundSampleRate;
}

void SettingsSetSoundSampleRate(int sampleRate) {
	settings.soundSampleRate = sampleRate;
}

long SettingsGetLanguage() {
	return settings.language;
}

void SettingsSetLanguage(long language) {
	settings.language = language;
}

int* SettingsGetInput(DeviceType type) {
    switch (type) {
        case DeviceType::MASTERSYSTEM:
        case DeviceType::GAMEGEAR:
            return &settings.smsKeys[0];
        case DeviceType::GAMEBOY:
        case DeviceType::GAMEBOYCOLOR:
            return &settings.gbKeys[0];
        case DeviceType::NES:
            return &settings.nesKeys[0];
        default:
            return NULL;
    }
}

void SettingsSetInput(DeviceType type, const int* padKeys) {
    switch (type) {
        case DeviceType::MASTERSYSTEM:
        case DeviceType::GAMEGEAR:
            for (int i=0; i<13; i++)
                settings.smsKeys[i] = padKeys[i];
            break;
        case DeviceType::GAMEBOY:
        case DeviceType::GAMEBOYCOLOR:
            for (int i=0; i<8; i++)
                settings.gbKeys[i] = padKeys[i];
            break;
    }
}

std::string* SettingsGetRecentRoms() {
	return &settings.recentRoms[0];
}

void SettingsSetRecentRoms(const std::string* recentRoms) {
	for (int i=0; i<10; i++)
        settings.recentRoms[i] = recentRoms[i];
}

void SettingsSaveToFile() {
    /*
    Settings settings = SettingsGetCopy();
    
	wxString configDir = wxStandardPaths::Get().GetUserDataDir();
    
	if (!wxFileName::DirExists(configDir))
		wxFileName::Mkdir(configDir, 0777, wxPATH_MKDIR_FULL);
    
	wxFileName configPath(configDir, wxT("config.ini"));
    
	// Guardar a disco
	wxFileConfig fileConfig(wxT(APP_NAME), wxT("pablogasco"), configPath.GetFullPath());
    
    fileConfig.Write(wxT("General/renderMethod"), settings.renderMethod);
	fileConfig.Write(wxT("General/greenScale"), settings.greenScale);
	fileConfig.Write(wxT("General/windowZoom"), settings.windowZoom);
    fileConfig.Write(wxT("General/language"), settings.language);
	
	fileConfig.Write(wxT("Sound/enabled"), settings.soundEnabled);
	fileConfig.Write(wxT("Sound/sampleRate"), settings.soundSampleRate);
	
    fileConfig.Write(wxT("Input/GB/up"), settings.gbKeys[0]);
    fileConfig.Write(wxT("Input/GB/down"), settings.gbKeys[1]);
    fileConfig.Write(wxT("Input/GB/left"), settings.gbKeys[2]);
    fileConfig.Write(wxT("Input/GB/right"), settings.gbKeys[3]);
    fileConfig.Write(wxT("Input/GB/a"), settings.gbKeys[4]);
    fileConfig.Write(wxT("Input/GB/b"), settings.gbKeys[5]);
    fileConfig.Write(wxT("Input/GB/select"), settings.gbKeys[6]);
    fileConfig.Write(wxT("Input/GB/start"), settings.gbKeys[7]);
    
	fileConfig.Write(wxT("Input/SMS/Pad1/up"), settings.smsKeys[0]);
	fileConfig.Write(wxT("Input/SMS/Pad1/down"), settings.smsKeys[1]);
	fileConfig.Write(wxT("Input/SMS/Pad1/left"), settings.smsKeys[2]);
	fileConfig.Write(wxT("Input/SMS/Pad1/right"), settings.smsKeys[3]);
	fileConfig.Write(wxT("Input/SMS/Pad1/1"), settings.smsKeys[4]);
	fileConfig.Write(wxT("Input/SMS/Pad1/2"), settings.smsKeys[5]);
    fileConfig.Write(wxT("Input/SMS/Pad2/up"), settings.smsKeys[6]);
    fileConfig.Write(wxT("Input/SMS/Pad2/down"), settings.smsKeys[7]);
    fileConfig.Write(wxT("Input/SMS/Pad2/left"), settings.smsKeys[8]);
    fileConfig.Write(wxT("Input/SMS/Pad2/right"), settings.smsKeys[9]);
    fileConfig.Write(wxT("Input/SMS/Pad2/1"), settings.smsKeys[10]);
    fileConfig.Write(wxT("Input/SMS/Pad2/2"), settings.smsKeys[11]);
	fileConfig.Write(wxT("Input/SMS/pauseStart"), settings.smsKeys[12]);
	
	wxString auxString[10];
	for (int i=0; i<10; i++)
	{
		auxString[i] = wxString(settings.recentRoms[i].c_str(), wxConvUTF8);
	}
	
	fileConfig.Write(wxT("RecentRoms/01"), auxString[0]);
	fileConfig.Write(wxT("RecentRoms/02"), auxString[1]);
	fileConfig.Write(wxT("RecentRoms/03"), auxString[2]);
	fileConfig.Write(wxT("RecentRoms/04"), auxString[3]);
	fileConfig.Write(wxT("RecentRoms/05"), auxString[4]);
	fileConfig.Write(wxT("RecentRoms/06"), auxString[5]);
	fileConfig.Write(wxT("RecentRoms/07"), auxString[6]);
	fileConfig.Write(wxT("RecentRoms/08"), auxString[7]);
	fileConfig.Write(wxT("RecentRoms/09"), auxString[8]);
	fileConfig.Write(wxT("RecentRoms/10"), auxString[9]);
    */
}

Settings SettingsLoadFromFile()
{
    Settings settings;
    /*
    
	wxString configDir = wxStandardPaths::Get().GetUserDataDir();
	wxFileName configPath(configDir, wxT("config.ini"));
	// Cargar de disco
	wxFileConfig fileConfig(wxT(APP_NAME), wxT("pablogasco"), configPath.GetFullPath());
    
    fileConfig.Read(wxT("General/renderMethod"), &settings.renderMethod);
	fileConfig.Read(wxT("General/greenScale"), &settings.greenScale);
	fileConfig.Read(wxT("General/windowZoom"), &settings.windowZoom);
    fileConfig.Read(wxT("General/language"), &settings.language);
	
	fileConfig.Read(wxT("Sound/enabled"),	 &settings.soundEnabled);
	fileConfig.Read(wxT("Sound/sampleRate"), &settings.soundSampleRate);
    
    fileConfig.Read(wxT("Input/GB/up"), &settings.gbKeys[0]);
    fileConfig.Read(wxT("Input/GB/down"), &settings.gbKeys[1]);
    fileConfig.Read(wxT("Input/GB/left"), &settings.gbKeys[2]);
    fileConfig.Read(wxT("Input/GB/right"), &settings.gbKeys[3]);
    fileConfig.Read(wxT("Input/GB/a"), &settings.gbKeys[4]);
    fileConfig.Read(wxT("Input/GB/b"), &settings.gbKeys[5]);
    fileConfig.Read(wxT("Input/GB/select"), &settings.gbKeys[6]);
    fileConfig.Read(wxT("Input/GB/start"), &settings.gbKeys[7]);
    
    fileConfig.Read(wxT("Input/SMS/Pad1/up"), &settings.smsKeys[0]);
    fileConfig.Read(wxT("Input/SMS/Pad1/down"), &settings.smsKeys[1]);
    fileConfig.Read(wxT("Input/SMS/Pad1/left"), &settings.smsKeys[2]);
    fileConfig.Read(wxT("Input/SMS/Pad1/right"), &settings.smsKeys[3]);
    fileConfig.Read(wxT("Input/SMS/Pad1/1"), &settings.smsKeys[4]);
    fileConfig.Read(wxT("Input/SMS/Pad1/2"), &settings.smsKeys[5]);
    fileConfig.Read(wxT("Input/SMS/Pad2/up"), &settings.smsKeys[6]);
    fileConfig.Read(wxT("Input/SMS/Pad2/down"), &settings.smsKeys[7]);
    fileConfig.Read(wxT("Input/SMS/Pad2/left"), &settings.smsKeys[8]);
    fileConfig.Read(wxT("Input/SMS/Pad2/right"), &settings.smsKeys[9]);
    fileConfig.Read(wxT("Input/SMS/Pad2/1"), &settings.smsKeys[10]);
    fileConfig.Read(wxT("Input/SMS/Pad2/2"), &settings.smsKeys[11]);
    fileConfig.Read(wxT("Input/SMS/pauseStart"), &settings.smsKeys[12]);
	
	wxString auxString[10];
	fileConfig.Read(wxT("RecentRoms/01"), &auxString[0]);
	fileConfig.Read(wxT("RecentRoms/02"), &auxString[1]);
	fileConfig.Read(wxT("RecentRoms/03"), &auxString[2]);
	fileConfig.Read(wxT("RecentRoms/04"), &auxString[3]);
	fileConfig.Read(wxT("RecentRoms/05"), &auxString[4]);
	fileConfig.Read(wxT("RecentRoms/06"), &auxString[5]);
	fileConfig.Read(wxT("RecentRoms/07"), &auxString[6]);
	fileConfig.Read(wxT("RecentRoms/08"), &auxString[7]);
	fileConfig.Read(wxT("RecentRoms/09"), &auxString[8]);
	fileConfig.Read(wxT("RecentRoms/10"), &auxString[9]);
	
	for (int i=0; i<10; i++)
		settings.recentRoms[i] = auxString[i].mb_str();
    
    */
    return settings;
}