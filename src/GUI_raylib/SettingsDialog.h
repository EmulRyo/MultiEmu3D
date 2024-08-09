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

#ifndef __SETTINGS_DIALOG_H__
#define __SETTINGS_DIALOG_H__

constexpr int INPUT_STR_MAX = 64;

#include <functional>
#include "raylib.h"
#include "Dialog.h"

class SettingsDialog: public Dialog
{
public:
    SettingsDialog();
    ~SettingsDialog();

    void Update() override;
    void Draw(Rectangle dst) override;
    void Close() override;

    void OnClosed(std::function<void(void)> funCB);

private:
    struct Input {
        char label[INPUT_STR_MAX];
        int  key;
        char name[INPUT_STR_MAX];
    };

    void DrawVideo(Rectangle dst);
    void DrawSound(Rectangle dst);
    void DrawInput(Rectangle dst);
    void DrawInputGB(Rectangle dst, float &y);
    void DrawInputSMS(Rectangle dst, float& y);
    void DrawInputNES(Rectangle dst, float& y);
    void DrawSection(Rectangle dst, const char* text, int sectionID);
    void DrawInputControl(float x, float y, float x2, const char* label, const char* keyName, int inputID);
    void ListenForKey(Input& input);

    int m_tabActive = 0;
    int m_gbPaletteActive = 0;
    bool m_gbPaletteEditMode = false;
    bool m_soundChecked = false;
    int m_soundSampleRateActive = 0;
    bool m_soundSampleRateEditMode = false;
    std::function<void(void)> m_onClosed = nullptr;

    Input m_gbInput[8];
    Input m_smsInput[13];
    Input m_nesInput[16];
    int m_inputSection = -1;
    int m_selectedInput = -1;
    Vector2 m_inputScroll = { 0 };
};

#endif
