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

#include "raygui.h"
#include "Settings.h"
#include "SettingsDialog.h"

int TabBar(Rectangle bounds, const char** text, int* icons, int count, int* active);
const char* GetKeyName(int key);

SettingsDialog::SettingsDialog()
{
	m_gbPaletteActive = Settings::GetGreenScale() == false ? 0 : 1;
	m_soundChecked = Settings::GetSoundEnabled();
	int sampleRates[] = { 22050, 32000, 44100, 48000 };
	m_soundSampleRateActive = 2;
	for (int i = 0; i < 4; i++)
	{
		if (Settings::GetSoundSampleRate() == sampleRates[i])
			m_soundSampleRateActive = i;
	}

    int* gbKeys = Settings::GetInput(DeviceType::GAMEBOY);
	const char* gbLabels[] = {"Up", "Down", "Left", "Right", "A", "B", "Select", "Start"};
	for (int i = 0; i < 8; i++) {
		strcpy_s((char *)(m_gbInput[i].label), INPUT_STR_MAX, (const char *)(gbLabels[i]));
        m_gbInput[i].key = gbKeys[i];
        strcpy_s((char*)(m_gbInput[i].name), INPUT_STR_MAX, GetKeyName(gbKeys[i]));
	}

    int* smsKeys = Settings::GetInput(DeviceType::MASTERSYSTEM);
    const char* smsLabels[] = { "Up", "Down", "Left", "Right", "1", "2", "Up", "Down", "Left", "Right", "1", "2", "Start/Pause"};
    for (int i = 0; i < 13; i++) {
        strcpy_s((char*)(m_smsInput[i].label), INPUT_STR_MAX, (const char*)(smsLabels[i]));
        m_smsInput[i].key = smsKeys[i];
        strcpy_s((char*)(m_smsInput[i].name), INPUT_STR_MAX, GetKeyName(smsKeys[i]));
    }

    int* nesKeys = Settings::GetInput(DeviceType::NES);
    const char* nesLabels[] = { "Up", "Down", "Left", "Right", "A", "B", "Select", "Start", "Up", "Down", "Left", "Right", "A", "B", "Select", "Start" };
    for (int i = 0; i < 16; i++) {
        strcpy_s((char*)(m_nesInput[i].label), INPUT_STR_MAX, (const char*)(nesLabels[i]));
        m_nesInput[i].key = nesKeys[i];
        strcpy_s((char*)(m_nesInput[i].name), INPUT_STR_MAX, GetKeyName(nesKeys[i]));
    }
}

SettingsDialog::~SettingsDialog() {

}

void SettingsDialog::Update() {

}

void SettingsDialog::Close() {
	Dialog::Close();

	Settings::SetGreenScale(m_gbPaletteActive == 1);
	Settings::SetSoundEnabled(m_soundChecked);
	int samples[] = { 22050, 32000, 44100, 4800 };
	Settings::SetSoundSampleRate(samples[m_soundSampleRateActive]);

    int gbKeys[8] = { 0 };
    for (int i = 0; i < 8; i++)
        gbKeys[i] = m_gbInput[i].key;
    Settings::SetInput(DeviceType::GAMEBOY, gbKeys);

    int smsKeys[13] = { 0 };
    for (int i = 0; i < 13; i++)
        smsKeys[i] = m_smsInput[i].key;
    Settings::SetInput(DeviceType::MASTERSYSTEM, smsKeys);

    int nesKeys[16] = { 0 };
    for (int i = 0; i < 16; i++)
        nesKeys[i] = m_nesInput[i].key;
    Settings::SetInput(DeviceType::NES, nesKeys);
	
	Settings::Save();
	if (m_onClosed)
		m_onClosed();
}

void SettingsDialog::OnClosed(std::function<void(void)> funCB) {
	m_onClosed = funCB;
}

void SettingsDialog::Draw(Rectangle dst) {
	if (!m_enabled)
		return;

	int borderColorNormal = GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL);
	int lineColor = GuiGetStyle(DEFAULT, LINE_COLOR);
	int baseColorPressed = GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED);
	GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, 0x808080FF);
	GuiSetStyle(DEFAULT, LINE_COLOR, 0x808080FF);
	GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, 0x3D3D3DFF);
	
	float width = dst.width - 20;
	if (width > 550)
		width = 550;
	float height = dst.height - 20;
	if (height > 350)
		height = 350;
	float x = (dst.width - width) / 2.0f;
	float y = (dst.height - height) / 2.0f;

	Rectangle winRec = { x, y, width, height };
	if (GuiWindowBox(winRec, "Settings"))
		Close();

	static const char* tabs[] = {
		"Video",
		"Sound",
		"Input"
	};
    static int icons[] = {
        ICON_MONITOR,
        ICON_AUDIO,
        ICON_MAILBOX,
    };

	Rectangle tabBarRec = { winRec.x + 10, winRec.y + 24 + 10, winRec.width - 20, 30 };
    BeginScissorMode(tabBarRec.x, tabBarRec.y, tabBarRec.width, tabBarRec.height);
	TabBar(tabBarRec, tabs, icons, 3, &m_tabActive);
    EndScissorMode();

	Rectangle tabLines = { tabBarRec.x, tabBarRec.y + tabBarRec.height, tabBarRec.width, winRec.height - tabBarRec.height - 24 - 10 - 10 };
	Rectangle tabContent = { tabLines.x + 10, tabLines.y+10, tabLines.width-20, tabLines.height-20 };
	switch (m_tabActive) {
		case 0: DrawVideo(tabContent); break;
		case 1: DrawSound(tabContent); break;
		case 2: DrawInput(tabLines); break;
	}

	DrawRectangleLinesEx(tabLines, 1, GetColor(0x808080FF));

	GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, borderColorNormal);
	GuiSetStyle(DEFAULT, LINE_COLOR, lineColor);
	GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, baseColorPressed);
}

void SettingsDialog::DrawVideo(Rectangle dst) {
	GuiLabel({ dst.x, dst.y, 150, 30 }, "GameBoy palette:");
	if (GuiDropdownBox({ dst.x + 150, dst.y, 120, 30 }, "Grayscale;Greenscale", &m_gbPaletteActive, m_gbPaletteEditMode)) {
		m_gbPaletteEditMode = !m_gbPaletteEditMode;
	}
}

void SettingsDialog::DrawSound(Rectangle dst) {
	GuiCheckBox({ dst.x, dst.y, 16, 16 }, "Enabled", &m_soundChecked);

	GuiLabel({ dst.x, dst.y+30, 150, 30 }, "Sample Rate:");
	if (GuiDropdownBox({ dst.x + 150, dst.y+30, 120, 30 }, "22050 Hz;32000 Hz;44100 Hz;48000 Hz", &m_soundSampleRateActive, m_soundSampleRateEditMode)) {
		m_soundSampleRateEditMode = !m_soundSampleRateEditMode;
	}
}

void SettingsDialog::DrawInput(Rectangle dst) {
    Rectangle panelView = { 0 };
    GuiScrollPanel(dst, nullptr, { dst.x, dst.y, dst.width - 14, 500 }, &m_inputScroll, &panelView);
    dst.x += 10;
    dst.y += 10;
    dst.width -= (20 + 14);
    dst.height -= 20;

    BeginScissorMode((int)panelView.x, (int)panelView.y, (int)panelView.width, (int)panelView.height);

	float y = m_inputScroll.y + dst.y;

    DrawSection({ dst.x, y, dst.width, 30 }, "GameBoy / GameBoy Color", 0);
    if (m_inputSection == 0)
        DrawInputGB(dst, y);

    y += 40;

    DrawSection({ dst.x, y, dst.width, 30 }, "Master System / Game Gear", 1);
    if (m_inputSection == 1)
        DrawInputSMS(dst, y);

    y += 40;

    DrawSection({ dst.x, y, dst.width, 30 }, "NES", 2);
    if (m_inputSection == 2)
        DrawInputNES(dst, y);

    EndScissorMode();
}

void SettingsDialog::DrawSection(Rectangle dst, const char* text, int sectionID) {
    const float sz = 18.0f;
    const float margin = 6.0f;

    GuiStatusBar(dst, text);
    if (GuiButton({ dst.x + dst.width - sz - margin, dst.y + margin, sz, sz }, GuiIconText(m_inputSection == sectionID ? ICON_ARROW_UP_FILL : ICON_ARROW_DOWN_FILL, ""))) {
        m_inputSection = (m_inputSection == sectionID) ? -1 : sectionID;
        m_inputScroll.y = m_inputSection == -1 ? 0 : -(40 * sectionID);
        m_selectedInput = -1;
    }
}

void SettingsDialog::DrawInputGB(Rectangle dst, float &y) {
    float x1 = dst.x + 3;
    float x2 = dst.x + 70;
    y += 40;
    for (int i = 0; i < 8; i++) {
        DrawInputControl(x1, y, x2, m_gbInput[i].label, m_gbInput[i].name, i);
        y += 40;
    }

    if (m_selectedInput >= 0)
        ListenForKey(m_gbInput[m_selectedInput]);
}

void SettingsDialog::DrawInputSMS(Rectangle dst, float& y) {
    float x1 = dst.x + 3;
    float x2 = dst.x + 70;
    float x3 = dst.x + 275;
    float x4 = x3 + 70;

    y += 40;
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiLabel({ x2, y, 150, 30 }, "Gamepad 1");
    GuiLabel({ x4, y, 150, 30 }, "Gamepad 2");
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    y += 40;

    for (int i = 0; i < 6; i++) {
        DrawInputControl(x1, y, x2, m_smsInput[i].label, m_smsInput[i].name, i);
        DrawInputControl(x3, y, x4, m_smsInput[6+i].label, m_smsInput[6+i].name, 6+i);
        y += 40;
    }

    DrawInputControl(x1, y, x2, m_smsInput[12].label, m_smsInput[12].name, 12);
    y += 40;

    if (m_selectedInput >= 0)
        ListenForKey(m_smsInput[m_selectedInput]);
}

void SettingsDialog::DrawInputNES(Rectangle dst, float& y) {
    float x1 = dst.x + 3;
    float x2 = dst.x + 70;
    float x3 = dst.x + 275;
    float x4 = x3 + 70;

    y += 40;
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiLabel({ x2, y, 150, 30 }, "Gamepad 1");
    GuiLabel({ x4, y, 150, 30 }, "Gamepad 2");
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    y += 40;

    for (int i = 0; i < 8; i++) {
        DrawInputControl(x1, y, x2, m_nesInput[i].label, m_nesInput[i].name, i);
        DrawInputControl(x3, y, x4, m_nesInput[8+i].label, m_nesInput[8+i].name, 8+i);
        y += 40;
    }

    if (m_selectedInput >= 0)
        ListenForKey(m_nesInput[m_selectedInput]);
}

void SettingsDialog::DrawInputControl(float x, float y, float x2, const char* label, const char* keyName, int inputID) {
    GuiLabel({ x, y, 55, 30 }, label);

    if (m_selectedInput == inputID) {
        GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, 0x0060DCFF);
        GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, 0x0060DCFF);
    }

    if (GuiButton({ x2, y, 150, 30 }, keyName)) {
        m_selectedInput = inputID;
    }
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, 0x808080FF);
    GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, 0x808080FF);
}

void SettingsDialog::ListenForKey(Input& input) {
    int key = GetKeyPressed();
    if (key != 0) {
        input.key = key;
        const char* name = GetKeyName(key);
        TextCopy((&input.name)[0], name);
    }
}

static int TabBar(Rectangle bounds, const char** text, int* icons, int count, int* active)
{
#define RAYGUI_TABBAR_ITEM_WIDTH    160

    int result = -1;
    //GuiState state = guiState;

    Rectangle tabBounds = { bounds.x, bounds.y, RAYGUI_TABBAR_ITEM_WIDTH, bounds.height };

    if (*active < 0) *active = 0;
    else if (*active > count - 1) *active = count - 1;

    int offsetX = 0;    // Required in case tabs go out of screen
    offsetX = (*active + 2) * RAYGUI_TABBAR_ITEM_WIDTH - GetScreenWidth();
    if (offsetX < 0) offsetX = 0;

    bool toggle = false;    // Required for individual toggles

    // Draw control
    //--------------------------------------------------------------------
    for (int i = 0; i < count; i++)
    {
        tabBounds.x = bounds.x + (RAYGUI_TABBAR_ITEM_WIDTH + 4) * i - offsetX;

        if (tabBounds.x < GetScreenWidth())
        {
            // Draw tabs as toggle controls
            int textAlignment = GuiGetStyle(TOGGLE, TEXT_ALIGNMENT);
            int textPadding = GuiGetStyle(TOGGLE, TEXT_PADDING);
            GuiSetStyle(TOGGLE, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
            GuiSetStyle(TOGGLE, TEXT_PADDING, 8);

            if (i == (*active))
            {
                toggle = true;
                GuiToggle(tabBounds, GuiIconText(icons[i], text[i]), &toggle);
            }
            else
            {
                toggle = false;
                GuiToggle(tabBounds, GuiIconText(icons[i], text[i]), &toggle);
                if (toggle) *active = i;
            }

            GuiSetStyle(TOGGLE, TEXT_PADDING, textPadding);
            GuiSetStyle(TOGGLE, TEXT_ALIGNMENT, textAlignment);
        }
    }

    // Draw tab-bar bottom line
    DrawRectangleRec({ bounds.x, bounds.y + bounds.height - 1, bounds.width, 1 }, GetColor(GuiGetStyle(TOGGLE, BORDER_COLOR_NORMAL)));
    //--------------------------------------------------------------------

    return result;     // Return as result the current TAB closing requested
}

static const char* GetKeyName(int key) {
    switch (key) {
    case KEY_NULL:          return "NULL";			// Key: NULL, used for no key pressed

    case KEY_APOSTROPHE:    return "APOSTROPHE";    // Key: '
    case KEY_COMMA:         return "COMMA";			// Key: ,
    case KEY_MINUS:         return "MINUS";			// Key: -
    case KEY_PERIOD:        return "PERIOD";		// Key: .
    case KEY_SLASH:         return "SLASH";			// Key: /
    case KEY_ZERO:          return "ZERO";			// Key: 0
    case KEY_ONE:           return "ONE";			// Key: 1
    case KEY_TWO:           return "TWO";			// Key: 2
    case KEY_THREE:         return "THREE";			// Key: 3
    case KEY_FOUR:          return "FOUR";			// Key: 4
    case KEY_FIVE:          return "FIVE";			// Key: 5
    case KEY_SIX:           return "SIX";			// Key: 6
    case KEY_SEVEN:         return "SEVEN";			// Key: 7
    case KEY_EIGHT:         return "EIGHT";			// Key: 8
    case KEY_NINE:          return "NINE";			// Key: 9
    case KEY_SEMICOLON:     return "SEMICOLON";     // Key: ;
    case KEY_EQUAL:         return "EQUAL";			// Key: =
    case KEY_A:             return "A";				// Key: A | a
    case KEY_B:             return "B";				// Key: B | b
    case KEY_C:             return "C";				// Key: C | c
    case KEY_D:             return "D";				// Key: D | d
    case KEY_E:             return "E";				// Key: E | e
    case KEY_F:             return "F";				// Key: F | f
    case KEY_G:             return "G";				// Key: G | g
    case KEY_H:             return "H";				// Key: H | h
    case KEY_I:             return "I";				// Key: I | i
    case KEY_J:             return "J";				// Key: J | j
    case KEY_K:             return "K";				// Key: K | k
    case KEY_L:             return "L";				// Key: L | l
    case KEY_M:             return "M";				// Key: M | m
    case KEY_N:             return "N";				// Key: N | n
    case KEY_O:             return "O";				// Key: O | o
    case KEY_P:             return "P";				// Key: P | p
    case KEY_Q:             return "Q";				// Key: Q | q
    case KEY_R:             return "R";				// Key: R | r
    case KEY_S:             return "S";				// Key: S | s
    case KEY_T:             return "T";				// Key: T | t
    case KEY_U:             return "U";				// Key: U | u
    case KEY_V:             return "V";				// Key: V | v
    case KEY_W:             return "W";				// Key: W | w
    case KEY_X:             return "X";				// Key: X | x
    case KEY_Y:             return "Y";				// Key: Y | y
    case KEY_Z:             return "Z";				// Key: Z | z
    case KEY_LEFT_BRACKET:  return "LEFT BRACKET";  // Key: [
    case KEY_BACKSLASH:     return "BACKSLASH";     // Key: '\'
    case KEY_RIGHT_BRACKET: return "RIGHT BRACKET"; // Key: ]
    case KEY_GRAVE:         return "GRAVE";			// Key: `

    case KEY_SPACE:         return "SPACE";			// Key: Space
    case KEY_ESCAPE:        return "ESCAPE";		// Key: Esc
    case KEY_ENTER:         return "ENTER";			// Key: Enter
    case KEY_TAB:           return "TAB";			// Key: Tab
    case KEY_BACKSPACE:     return "BACKSPACE";     // Key: Backspace
    case KEY_INSERT:        return "INSERT";		// Key: Ins
    case KEY_DELETE:        return "DELETE";		// Key: Del
    case KEY_RIGHT:         return "RIGHT";			// Key: Cursor right
    case KEY_LEFT:          return "LEFT";			// Key: Cursor left
    case KEY_DOWN:          return "DOWN";			// Key: Cursor down
    case KEY_UP:            return "UP";			// Key: Cursor up
    case KEY_PAGE_UP:       return "PAGE UP";		// Key: Page up
    case KEY_PAGE_DOWN:     return "PAGE DOWN";     // Key: Page down
    case KEY_HOME:          return "HOME";			// Key: Home
    case KEY_END:           return "END";			// Key: End
    case KEY_CAPS_LOCK:     return "CAPS LOCK";     // Key: Caps lock
    case KEY_SCROLL_LOCK:   return "SCROLL LOCK";   // Key: Scroll down
    case KEY_NUM_LOCK:      return "NUM LOCK";      // Key: Num lock
    case KEY_PRINT_SCREEN:  return "PRINT SCREEN";  // Key: Print screen
    case KEY_PAUSE:         return "PAUSE";			// Key: Pause
    case KEY_F1:            return "F1";			// Key: F1
    case KEY_F2:            return "F2";			// Key: F2
    case KEY_F3:            return "F3";			// Key: F3
    case KEY_F4:            return "F4";			// Key: F4
    case KEY_F5:            return "F5";			// Key: F5
    case KEY_F6:            return "F6";			// Key: F6
    case KEY_F7:            return "F7";			// Key: F7
    case KEY_F8:            return "F8";			// Key: F8
    case KEY_F9:            return "F9";			// Key: F9
    case KEY_F10:           return "F10";			// Key: F10
    case KEY_F11:           return "F11";			// Key: F11
    case KEY_F12:           return "F12";			// Key: F12
    case KEY_LEFT_SHIFT:    return "LEFT SHIFT";    // Key: Shift left
    case KEY_LEFT_CONTROL:  return "LEFT CONTROL";  // Key: Control left
    case KEY_LEFT_ALT:      return "LEFT ALT";      // Key: Alt left
    case KEY_LEFT_SUPER:    return "LEFT SUPER";    // Key: Super left
    case KEY_RIGHT_SHIFT:   return "RIGHT SHIFT";   // Key: Shift right
    case KEY_RIGHT_CONTROL: return "RIGHT CONTROL"; // Key: Control right
    case KEY_RIGHT_ALT:     return "RIGHT ALT";     // Key: Alt right
    case KEY_RIGHT_SUPER:   return "RIGHT SUPER";   // Key: Super right
    case KEY_KB_MENU:       return "KB MENU";		// Key: KB menu

    case KEY_KP_0:          return "KP 0";			// Key: Keypad 0
    case KEY_KP_1:          return "KP 1";			// Key: Keypad 1
    case KEY_KP_2:          return "KP 2";			// Key: Keypad 2
    case KEY_KP_3:          return "KP 3";			// Key: Keypad 3
    case KEY_KP_4:          return "KP 4";			// Key: Keypad 4
    case KEY_KP_5:          return "KP 5";			// Key: Keypad 5
    case KEY_KP_6:          return "KP 6";			// Key: Keypad 6
    case KEY_KP_7:          return "KP 7";			// Key: Keypad 7
    case KEY_KP_8:          return "KP 8";			// Key: Keypad 8
    case KEY_KP_9:          return "KP 9";			// Key: Keypad 9
    case KEY_KP_DECIMAL:    return "KP DECIMAL";    // Key: Keypad .
    case KEY_KP_DIVIDE:     return "KP DIVIDE";     // Key: Keypad /
    case KEY_KP_MULTIPLY:   return "KP MULTIPLY";   // Key: Keypad *
    case KEY_KP_SUBTRACT:   return "KP SUBTRACT";   // Key: Keypad -
    case KEY_KP_ADD:        return "KP ADD";		// Key: Keypad +
    case KEY_KP_ENTER:      return "KP ENTER";      // Key: Keypad Enter
    case KEY_KP_EQUAL:      return "KP EQUAL";      // Key: Keypad =

    case KEY_BACK:          return "BACK";			// Key: Android back button
    case KEY_MENU:          return "MENU";			// Key: Android menu button
    case KEY_VOLUME_UP:     return "VOLUME UP";     // Key: Android volume up button
    case KEY_VOLUME_DOWN:   return "VOLUME DOWN";   // Key: Android volume down button

    default:				return "UNKOWN";
    }
}