/*
 This file is part of MARS.
 
 MARS is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 MARS is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with MARS.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <string>
#include "../Common/VideoGameDevice.h"

class Settings
{
public:
	static int  GetRenderMethod();
	static bool GetGreenScale();
	static int  GetWindowZoom();
	static bool GetSoundEnabled();
	static int  GetSoundSampleRate();
	static std::string_view GetLanguage();
	static int* GetInput(DeviceType type);
	static std::string* GetRecentRoms();

	static void SetRenderMethod(int renderMethod);
	static void SetGreenScale(bool greenScale);
	static void SetWindowZoom(int windowZoom);
	static void SetSoundEnabled(bool enabled);
	static void SetSoundSampleRate(int sampleRate);
	static void SetLanguage(std::string_view language);
	static void SetInput(DeviceType type, const int* padKeys);
	static void SetRecentRoms(const std::string* recentRoms);

	static void SetFile(const std::string& fileName);
	static void Load(const std::string& fileName = "");
	static void Save(const std::string& fileName = "");

};

#endif
