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

#include <wx/defs.h>
//#include <wx/language.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/fileconf.h>
#include "Settings.h"
#include "../SMS-GG/Def.h"

using namespace std;

Settings settings;

Settings::Settings()
{
    renderMethod = 1;
	greenScale	 = false;
	windowZoom	 = 1;
	soundEnabled	= true;
	soundSampleRate = 44100;
    language = wxLANGUAGE_DEFAULT;
	
	pad1Keys[0]	= WXK_UP;	// Up
	pad1Keys[1]	= WXK_DOWN; // Down
	pad1Keys[2]	= WXK_LEFT; // Left
	pad1Keys[3]	= WXK_RIGHT;// Right
	pad1Keys[4]	= 'A';		// A
	pad1Keys[5]	= 'S';		// B
    
    pad2Keys[0]	= 'I'; // Up
    pad2Keys[1]	= 'K'; // Down
    pad2Keys[2]	= 'J'; // Left
    pad2Keys[3]	= 'L'; // Right
    pad2Keys[4]	= 'G'; // A
    pad2Keys[5]	= 'H'; // B
    
	pauseStartKey = WXK_RETURN; // Start
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

int* SettingsGetInput1() {
	return &settings.pad1Keys[0];
}

int* SettingsGetInput2() {
    return &settings.pad2Keys[0];
}

int SettingsGetPauseStart() {
    return settings.pauseStartKey;
}

void SettingsSetInput1(const int* padKeys) {
	for (int i=0; i<6; i++)
		settings.pad1Keys[i] = padKeys[i];
}

void SettingsSetInput2(const int* padKeys) {
    for (int i=0; i<6; i++)
        settings.pad2Keys[i] = padKeys[i];
}

void SettingsSetPauseStart(int padKey) {
    settings.pauseStartKey = padKey;
}

string* SettingsGetRecentRoms() {
	return &settings.recentRoms[0];
}

void SettingsSetRecentRoms(const string* recentRoms) {
	for (int i=0; i<10; i++)
        settings.recentRoms[i] = recentRoms[i];
}

void SettingsSaveToFile() {
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
	
	fileConfig.Write(wxT("Input1/up"), settings.pad1Keys[0]);
	fileConfig.Write(wxT("Input1/down"), settings.pad1Keys[1]);
	fileConfig.Write(wxT("Input1/left"), settings.pad1Keys[2]);
	fileConfig.Write(wxT("Input1/right"), settings.pad1Keys[3]);
	fileConfig.Write(wxT("Input1/a"), settings.pad1Keys[4]);
	fileConfig.Write(wxT("Input1/b"), settings.pad1Keys[5]);
    fileConfig.Write(wxT("Input2/up"), settings.pad2Keys[0]);
    fileConfig.Write(wxT("Input2/down"), settings.pad2Keys[1]);
    fileConfig.Write(wxT("Input2/left"), settings.pad2Keys[2]);
    fileConfig.Write(wxT("Input2/right"), settings.pad2Keys[3]);
    fileConfig.Write(wxT("Input2/a"), settings.pad2Keys[4]);
    fileConfig.Write(wxT("Input2/b"), settings.pad2Keys[5]);
	fileConfig.Write(wxT("Input/pauseStart"), settings.pauseStartKey);
	
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
}

Settings SettingsLoadFromFile()
{
    Settings settings;
    
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
	
	fileConfig.Read(wxT("Input1/up"),    &settings.pad1Keys[0]);
	fileConfig.Read(wxT("Input1/down"),	 &settings.pad1Keys[1]);
	fileConfig.Read(wxT("Input1/left"),	 &settings.pad1Keys[2]);
	fileConfig.Read(wxT("Input1/right"), &settings.pad1Keys[3]);
	fileConfig.Read(wxT("Input1/a"),     &settings.pad1Keys[4]);
	fileConfig.Read(wxT("Input1/b"),     &settings.pad1Keys[5]);
    fileConfig.Read(wxT("Input2/up"),    &settings.pad2Keys[0]);
    fileConfig.Read(wxT("Input2/down"),	 &settings.pad2Keys[1]);
    fileConfig.Read(wxT("Input2/left"),	 &settings.pad2Keys[2]);
    fileConfig.Read(wxT("Input2/right"), &settings.pad2Keys[3]);
    fileConfig.Read(wxT("Input2/a"),     &settings.pad2Keys[4]);
    fileConfig.Read(wxT("Input2/b"),     &settings.pad2Keys[5]);
	fileConfig.Read(wxT("Input/pauseStart"),  &settings.pauseStartKey);
	
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
    
    return settings;
}