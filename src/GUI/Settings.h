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

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <string>
#include "../Common/VideoGameDevice.h"

class Settings
{
public:
    int  renderMethod;
	bool greenScale;
	int  windowZoom;
	bool soundEnabled;
	int  soundSampleRate;
    long language;
	
	int  gbKeys[8];
    int  smsKeys[13];
	int  nesKeys[16];
	std::string recentRoms[10];
	
public:
	Settings();
};

Settings SettingsGetCopy();

int  SettingsGetRenderMethod();
bool SettingsGetGreenScale();
int  SettingsGetWindowZoom();
bool SettingsGetSoundEnabled();
int  SettingsGetSoundSampleRate();
long SettingsGetLanguage();
int* SettingsGetInput(DeviceType type);
std::string* SettingsGetRecentRoms();

void SettingsSetNewValues(Settings newSettings);

void SettingsSetGreenScale(int renderMethod);
void SettingsSetGreenScale(bool greenScale);
void SettingsSetWindowZoom(int windowZoom);
void SettingsSetSoundEnabled(bool enabled);
void SettingsSetSoundSampleRate(int sampleRate);
void SettingsSetLanguage(long language);
void SettingsSetInput(DeviceType type, const int* padKeys);
void SettingsSetRecentRoms(const std::string* recentRoms);

void SettingsSaveToFile();
Settings SettingsLoadFromFile();

#endif
