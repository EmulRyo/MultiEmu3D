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
#include <filesystem>
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

#include "MenuBar.h"

#include "nfd.h"

MainFrame::MainFrame(const std::string& fileName)
{
	// m_renderer = NULL;
    NFD_Init();

    m_renderer = new RendererSW();

    // create the emulation
    m_emulation = new EmulationThread();

    //m_fullScreen = false;
    
    m_emulation->SetScreen(m_renderer);
    m_emulation->SetState(EmuState::Playing);

    m_fontSize = 20;
    m_font = LoadFontEx("C:\\Windows\\Fonts\\segoeui.ttf", m_fontSize, nullptr, 0);
    GuiSetFont(m_font);
    SetStyle();

    m_menuBar.SetFont(m_font);
    SubMenu& fileMenu        = m_menuBar.NewSubMenu("File");
    SubMenu& emulationMenu   = m_menuBar.NewSubMenu("Emulation");
    SubMenu& languageMenu    = m_menuBar.NewSubMenu("Language");
    SubMenu& helpMenu        = m_menuBar.NewSubMenu("Help");

    fileMenu.NewItem("Open", std::bind(&MainFrame::OnOpenFileUI, this));
    SubMenuItem& openRecent = fileMenu.NewItem("Open Recent   ");
    SubMenuItem& loadState  = fileMenu.NewItem("Load State");
    SubMenuItem& saveState  = fileMenu.NewItem("Save State");
    fileMenu.NewItem("Exit", std::bind(&MainFrame::OnExitUI, this));

    emulationMenu.NewItem("Settings",   std::bind(&MainFrame::OnSettingsUI, this));
    emulationMenu.NewItem("Play",       std::bind(&MainFrame::OnPlayUI, this));
    emulationMenu.NewItem("Pause",      std::bind(&MainFrame::OnPauseUI, this));
    emulationMenu.NewItem("Stop",       std::bind(&MainFrame::OnStopUI, this));
    emulationMenu.NewItem("Debug",      std::bind(&MainFrame::OnDebugUI, this));
    emulationMenu.NewItem("Fullscreen", std::bind(&MainFrame::OnFullscreenUI, this));

    SubMenu& loadStateMenu = loadState.GetSubMenu();
    SubMenu& saveStateMenu = saveState.GetSubMenu();
    for (int i = 0; i < 10; i++) {
        loadStateMenu.NewItem("Slot "+std::to_string(i+1), std::bind(&MainFrame::OnLoadStateUI, this, std::placeholders::_1));
        saveStateMenu.NewItem("Slot "+std::to_string(i+1), std::bind(&MainFrame::OnSaveStateUI, this, std::placeholders::_1));
    }

    helpMenu.NewItem("About");

    m_recentMenuOpened = false;
    m_numRecentFiles = 0;

	if (!fileName.empty())
		ChangeFile(fileName);
}

MainFrame::~MainFrame()
{
    m_emulation->Exit();
    delete m_emulation;
    delete m_renderer;

    NFD_Quit();
}

void MainFrame::Update(float deltaTime) {
    m_emulation->UpdatePad();
    if (IsFileDropped()) {
        FilePathList files = LoadDroppedFiles();
        ChangeFile(files.paths[0]);
        UnloadDroppedFiles(files);
    }
}

void MainFrame::Draw(Rectangle r) {
    if (!m_messageError.empty())
        GuiDisable();

    m_renderer->Draw(Rectangle{ 0, 48, r.width, r.height - 72 });
    if (m_menuBar.IsOpened())
        GuiLock();
    DrawToolBar(Rectangle{ 0, 24, r.width, 24 });
    if (m_menuBar.IsOpened())
        GuiUnlock();
    m_menuBar.Draw(Rectangle{ 0, 0, r.width, 24 });
    if (m_recentMenuOpened) {
        SubMenu& recentMenu = m_menuBar.GetSubMenu(0).GetItem(1).GetSubMenu();
        recentMenu.Draw(24, 48);
    }
    DrawStatusBar(Rectangle{ 0, r.height-24, r.width, 24 });

    if (!m_messageError.empty()) {
        ShowErrorMessageBox(r.width, r.height);
    }
}

void MainFrame::ChangeFile(const std::string &fileName)
{
    if (m_emulation->ChangeFile(fileName))
        UpdateRecentMenu(fileName);
    else {
        m_messageError = m_emulation->GetLastError();
        m_emulation->SetState(EmuState::Paused);
    }
}

void MainFrame::UpdateRecentMenu(const std::string &fileName)
{
    SubMenu& recentMenu = m_menuBar.GetSubMenu(0).GetItem(1).GetSubMenu();

    wchar_t separator = std::filesystem::path::preferred_separator;
    std::string shortName = fileName.substr(fileName.rfind(separator) + 1);
    int previousIndex = -1;
    for (int i = 0; i < m_numRecentFiles; i++)
    {
        if (m_recentFiles[i].fullName == fileName)
        {
            previousIndex = i;
            break;
        }
    }

    int startFrom;

    // Si ya existia de antes y es el primero
    if (previousIndex == 0)
    {
        return;
    }
    // Si ya existia de antes y no es el primero
    else if (previousIndex > 0)
    {
        startFrom = previousIndex - 1;
    }
    // Si no existia pero no hemos llegado al limite
    else if (m_numRecentFiles < MAX_RECENT_FILES)
    {
        startFrom = m_numRecentFiles - 1;
        recentMenu.NewItem(" ", std::bind(&MainFrame::OnOpenRecentUI, this, std::placeholders::_1));
        m_numRecentFiles++;
    }
    // Si no existia pero hemos llegado al limite
    else
    {
        startFrom = MAX_RECENT_FILES - 2;
    }

    for (int i = startFrom; i >= 0; i--)
    {
        m_recentFiles[i + 1].shortName = m_recentFiles[i].shortName;
        m_recentFiles[i + 1].fullName = m_recentFiles[i].fullName;
    }
    m_recentFiles[0].shortName = shortName;
    m_recentFiles[0].fullName = fileName;

    for (int i = 0; i < m_numRecentFiles; i++)
    {
        recentMenu.GetItem(i).SetText(m_recentFiles[i].shortName);
    }

    recentMenu.UpdateTexts();

    //RecentRomsToSettings();
    //m_settingsDialog->Reload();
    //SettingsSaveToFile();
}

void MainFrame::SetStyle() {
    GuiSetStyle(DEFAULT, TEXT_SIZE, m_fontSize);
    GuiSetStyle(DEFAULT, BACKGROUND_COLOR, 0x1F1F1FFF);
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, 0x1F1F1FFF);
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, 0x1F1F1FFF);
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, 0x3D3D3DFF);
    GuiSetStyle(DEFAULT, BASE_COLOR_DISABLED, 0x1F1F1FFF);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0xFFFFFFFF);
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, 0xFFFFFFFF);
    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, 0xFFFFFFFF);
    GuiSetStyle(DEFAULT, TEXT_COLOR_DISABLED, 0x5F5F5FFF);
    GuiSetStyle(DEFAULT, BORDER_WIDTH, 1);
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, 0x1F1F1FFF);
    GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, 0x707070FF);
    GuiSetStyle(DEFAULT, BORDER_COLOR_PRESSED, 0x707070FF);
    GuiSetStyle(DEFAULT, BORDER_COLOR_DISABLED, 0x1F1F1FFF);
    GuiSetStyle(DEFAULT, LINE_COLOR, 0x1F1F1FFF);        
}

void MainFrame::DrawToolBar(Rectangle dst) {
    GuiEnableTooltip();

    DrawRectangle((int)dst.x, (int)dst.y, (int)dst.width, (int)dst.height, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
    float width = 24;
    float x = dst.x;
    GuiSetTooltip("Open");
    if (GuiButton(Rectangle{ x, dst.y, width, dst.height }, GuiIconText(ICON_FOLDER_FILE_OPEN, ""))) {
        OnOpenFileUI();
    }

    if (m_recentMenuOpened)
        GuiDisableTooltip();

    x += width;
    GuiSetTooltip("Recent");
    if (GuiButton(Rectangle{ x, dst.y, width, dst.height }, GuiIconText(ICON_ARROW_DOWN, ""))) {
        OnRecentUI();
    }

    // Cerrar menu de roms recientemente abiertas si se pulsa fuera del botón
    if (m_recentMenuOpened &&
        (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) || IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) &&
        !CheckCollisionPointRec(GetMousePosition(), Rectangle{ x, dst.y, width, dst.height })) {
        m_recentMenuOpened = false;
    }

    GuiEnableTooltip();

    x += width*2;
    GuiSetTooltip("Play");
    if (GuiButton(Rectangle{ x, dst.y, width, dst.height }, GuiIconText(ICON_PLAYER_PLAY, ""))) {
        OnPlayUI();
    }

    x += width;
    GuiSetTooltip("Pause");
    if (GuiButton(Rectangle{ x, dst.y, width, dst.height }, GuiIconText(ICON_PLAYER_PAUSE, ""))) {
        OnPauseUI();
    }

    x += width;
    GuiSetTooltip("Stop");
    if (GuiButton(Rectangle{ x, dst.y, width, dst.height }, GuiIconText(ICON_PLAYER_STOP, ""))) {
        OnStopUI();
    }

    x += width*2;
    GuiSetTooltip("Switch between 2D and 3D");
    if (GuiButton(Rectangle{ x, dst.y, width, dst.height }, GuiIconText(ICON_MODE_3D, ""))) {

    }
    GuiDisableTooltip();
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

void MainFrame::OnOpenFileUI() {
    EmuState copyState = m_emulation->GetState();
    m_emulation->SetState(EmuState::Paused);

    nfdchar_t* outPath;
    nfdfilteritem_t filterItem[] = {
        { "All roms(*.gb; *.gbc; *.nes; *.sms; *.gg; *.zip)", "gb,gbc,nes,sms,gg,zip,7z" },
        { "GameBoy(*.gb)",          "gb"    },
        { "GameBoy Color(*.gbc)",   "gbc"   },
        { "Game Gear(*.gg)",        "gg"    },
        { "Master System(*.sms)",   "sms"   },
        { "NES(*.nes)",             "nes"   },
    };
    nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 6, NULL, GetWindowHandle());
    if (result == NFD_OKAY)
    {
        ChangeFile(std::string(outPath));
        NFD_FreePath(outPath);
    }
    else
        m_emulation->SetState(copyState);

    m_menuBar.Close();
}

void MainFrame::OnRecentUI() {
    m_recentMenuOpened = !m_recentMenuOpened;
}

void MainFrame::OnOpenRecentUI(int id) {
    ChangeFile(m_recentFiles[id].fullName);
}

void MainFrame::OnLoadStateUI(int id) {
    printf("Load state %d\n", id);

    std::filesystem::path savesDir = std::filesystem::absolute(std::filesystem::current_path());
    savesDir += std::filesystem::path::preferred_separator;
    savesDir = savesDir.append("SaveStates");
    savesDir += std::filesystem::path::preferred_separator;

    try
    {
        m_emulation->LoadState(savesDir.generic_u8string(), id);
    }
    catch (Exception e)
    {
        m_messageError = e.what();
    }
}

void MainFrame::OnSaveStateUI(int id) {
    std::filesystem::path savesDir = std::filesystem::absolute(std::filesystem::current_path());
    savesDir += std::filesystem::path::preferred_separator;
    savesDir = savesDir.append("SaveStates");
    savesDir += std::filesystem::path::preferred_separator;

    if (!std::filesystem::exists(savesDir))
        std::filesystem::create_directory(savesDir);

    try
    {
        m_emulation->SaveState(savesDir.generic_u8string(), id);
    }
    catch (Exception e)
    {
        m_messageError = e.what();
    }
}

void MainFrame::OnPlayUI() {
    m_emulation->SetState(EmuState::Playing);
}

void MainFrame::OnPauseUI() {
    if (m_emulation->GetState() == EmuState::Playing)
        m_emulation->SetState(EmuState::Paused);
    else if (m_emulation->GetState() == EmuState::Paused)
        m_emulation->SetState(EmuState::Playing);
}

void MainFrame::OnStopUI() {
    m_emulation->SetState(EmuState::Stopped);
}

void MainFrame::OnSettingsUI() {

}

void MainFrame::OnDebugUI() {

}

void MainFrame::OnFullscreenUI() {
    ToggleBorderlessWindowed();
}

void MainFrame::OnExitUI() {
    exit(0);
}

void MainFrame::ShowErrorMessageBox(float winWidth, float winHeight) {
    GuiEnable();
    DrawRectangle(0, 24 * 2, (int)winWidth, (int)(winHeight - 24 * 3), ColorAlpha(BLACK, 0.7f));
    Vector2 fontSize = MeasureTextEx(m_font, m_messageError.c_str(), (float)m_fontSize, 1);
    float w = fontSize.x + 20;
    float h = fontSize.y + 24 * 3;
    int result = GuiMessageBox(Rectangle{ (winWidth - w) / 2.0f, (winHeight - h) / 2.0f, w, h }, "Error", m_messageError.c_str(), "OK");
    if (result >= 0) {
        m_messageError = "";
    }
}
