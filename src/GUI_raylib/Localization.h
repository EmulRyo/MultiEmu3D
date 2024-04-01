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

#ifndef __LOCALIZATION_H__
#define __LOCALIZATION_H__

#include <string>

#define _(str) Localization::Translate(str)

class Localization {
public:
    static bool IsAvailable(std::string_view language);
    static bool SetLanguage(std::string_view language);
    static std::string_view TranslateStrView(std::string_view text);
    static const char* Translate(std::string_view text);
};

#endif
