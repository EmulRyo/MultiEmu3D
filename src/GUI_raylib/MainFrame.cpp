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

#include <iostream>
#include <string>
#include "MainFrame.h"
#include "../Common/Exception.h"
#include "../Common/VideoGameDevice.h"
#include "../NES/NES.h"
#include "../GB-GBC/GB.h"
#include "RendererSW.h"
#include "EmulationThread.h"
#include "AppDefs.h"
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
MainFrame::MainFrame(const std::string& fileName)
{
	// m_renderer = NULL;
    m_renderer = new RendererSW();

    // create the emulation
    m_emulation = new EmulationThread();

    //m_fullScreen = false;
    
    m_emulation->SetScreen(m_renderer);
    m_emulation->SetState(EmuState::Playing);
     
	if (!fileName.empty())
		ChangeFile(fileName);

    m_fontSize = 20;
    m_font = LoadFontEx("C:\\Windows\\Fonts\\segoeui.ttf", m_fontSize, nullptr, 0);
    GuiSetFont(m_font);
    GuiSetStyle(DEFAULT, TEXT_SIZE, m_fontSize);

    GuiEnableTooltip();
}

MainFrame::~MainFrame()
{
    m_emulation->Exit();
    delete m_emulation;
    delete m_renderer;
}

void MainFrame::Update(float deltaTime) {
    m_emulation->UpdatePad();
}

void MainFrame::Draw(Rectangle r) {
    m_renderer->Draw(Rectangle{ 0, 48, r.width, r.height - 72 });
    DrawToolBar(Rectangle {0, 24, r.width, 24});
    DrawMenuBar(Rectangle{ 0, 0, r.width, 24 });
    DrawStatusBar(Rectangle{ 0, r.height-24, r.width, 24 });
}

void MainFrame::ChangeFile(const std::string &fileName)
{
    if (m_emulation->ChangeFile(fileName))
        UpdateRecentMenu(fileName);
}

void MainFrame::UpdateRecentMenu(const std::string &fileName)
{

}

void MainFrame::DrawMenuBar(Rectangle dst) {
    GuiSetStyle(DEFAULT, BACKGROUND_COLOR, 0x1F1F1FFF);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0xFFFFFFFF);
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, 0x1F1F1FFF);
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, 0x3D3D3DFF);
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, 0xFFFFFFFF);
    GuiSetStyle(DEFAULT, BORDER_WIDTH, 1);
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, 0x1F1F1FFF);
    GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, 0x707070FF);

    GuiDrawRectangle(dst, 0, GetColor(GuiGetStyle(DEFAULT, LINE_COLOR)), GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    GuiSetStyle(LISTVIEW, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiSetStyle(LISTVIEW, LIST_ITEMS_HEIGHT, 24);
    GuiSetStyle(LISTVIEW, LIST_ITEMS_SPACING, 0);
    static int visible = -1;
    Vector2 textSize;
    float x, width;
    static float listViewX;
    
    int textSpacing = GuiGetStyle(DEFAULT, TEXT_SPACING);

    const char* menuTexts[] = {
            "File",
            "Emulation",
            "Language",
            "Help"
    };

    int arrayLen = (int)(sizeof(menuTexts) / sizeof(const char*));

    static int fileActive = -1;
    static int fileScrollIndex = 0;
    static int fileFocus = -1;
    x = dst.x;
    
    for (int i = 0; i < arrayLen; i++) {
        textSize = MeasureTextEx(m_font, menuTexts[i], m_fontSize, textSpacing);
        width = textSize.x + 24;
        GuiListViewEx(Rectangle{ x, dst.y, width, dst.height }, &menuTexts[i], 1, nullptr, nullptr, nullptr);
        x += width;
    }

    if (fileActive==0 || fileFocus==0) {
        GuiSetStyle(LISTVIEW, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        GuiSetStyle(LISTVIEW, TEXT_PADDING, 10);
        int listViewItemsHeight = GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT);
        int listViewItemSpacing = GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING);
        const char* texts[] = {
            "Open",
            "Open Recent",
            "Load State",
            "Save State",
            "Exit"
        };
        static int active = -1;
        static int scrollIndex = 0;
        static int focus = -1;
        GuiListViewEx(Rectangle{ listViewX, dst.y + 24, 120, (listViewItemsHeight + listViewItemSpacing) * 5.0f + 2.0f }, &texts[0], 5, &scrollIndex, &active, &focus);
    }
    
    /*
    const char* texts[] = {
            "Settings",
            "Start",
            "Pause",
            "Stop",
            "Debug",
            "Fullscren"
        };
    */

    //GuiMessageBox(Rectangle{ 100, 100, 200, 100 }, "Title", "Message", "Ok");
    
}

void MainFrame::DrawToolBar(Rectangle dst) {
    GuiDrawRectangle(dst, 0, GetColor(GuiGetStyle(DEFAULT, LINE_COLOR)), GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
    float width = 24;
    float x = dst.x;
    GuiSetTooltip("Open");
    if (GuiButton(Rectangle{ x, dst.y, width, dst.height }, GuiIconText(ICON_FOLDER_FILE_OPEN, ""))) {
        
    }

    x += width;
    GuiSetTooltip("Recent");
    GuiButton(Rectangle{ x, dst.y, width, dst.height }, GuiIconText(ICON_ARROW_DOWN, ""));

    x += width*2;
    GuiSetTooltip("Play");
    GuiButton(Rectangle{ x, dst.y, width, dst.height }, GuiIconText(ICON_PLAYER_PLAY, ""));

    x += width;
    GuiSetTooltip("Pause");
    GuiButton(Rectangle{ x, dst.y, width, dst.height }, GuiIconText(ICON_PLAYER_PAUSE, ""));

    x += width;
    GuiSetTooltip("Stop");
    GuiButton(Rectangle{ x, dst.y, width, dst.height }, GuiIconText(ICON_PLAYER_STOP, ""));

    x += width*2;
    GuiSetTooltip("Switch between 2D and 3D");
    GuiButton(Rectangle{ x, dst.y, width, dst.height }, GuiIconText(ICON_MODE_3D, ""));

}

void MainFrame::DrawStatusBar(Rectangle dst) {
    float x, y, w, h;
    const char* text;
    x = dst.x;
    w = 0;
    h = dst.height;
    y = dst.y;
    
    w = (float)GetScreenWidth() * 0.5f;
    text = TextFormat("Refresh: %d fps", GetFPS());
    GuiStatusBar(Rectangle{ x, y, w, h }, text);
    x += w;

    w = (float)GetScreenWidth() * 0.5f;
    text = TextFormat("Emulation: %.1f fps", m_emulation->GetFPS());
    GuiStatusBar(Rectangle{ x, y, w, h }, text);
}
