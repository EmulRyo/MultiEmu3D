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

#define RAYGUI_CUSTOM_ICONS
#include "iconset.rgi.h"
#include "raylib.h"
#include "raygui.h"
#include "raymath.h"
#include "../NES/NES.h"
#include "../NES/Debugger.h"
#include "DebuggerNESDialog.h"

DebuggerNESDialog::DebuggerNESDialog(VideoGameDevice* device)
{
	m_disassemblerFirst = m_disassemblerLast = 0;
	m_dialogOffset = { 0 };
	m_dragging = false;
	Nes::NES* nes = (Nes::NES*)device;
	m_debugger = (Nes::Debugger*)nes->GetDebugger();
	m_monoFont = LoadFontEx("Fonts/RobotoMono-Regular.ttf", 20, NULL, 0);
	m_videoRegsScroll = { 0 };
	m_otherRegsScroll = { 0 };
	m_registersActive = 0;
	m_memoryActive = 0;

	Image img = GenImageColor(128, 256, WHITE);
	ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8);
	m_tilesTexture = LoadTextureFromImage(img);
	UnloadImage(img);
}

DebuggerNESDialog::~DebuggerNESDialog() {
	UnloadFont(m_monoFont);
	UnloadTexture(m_tilesTexture);
}

void DebuggerNESDialog::Update() {
	
}

void DebuggerNESDialog::Show() {
	DebuggerDialog::Show();
	UpdatePrevValues();
}

void DebuggerNESDialog::UpdatePad() {
	m_debugger->UpdatePad1(&m_inputs[0]);
}

void DebuggerNESDialog::Draw(Rectangle dst) {
	if (!m_enabled)
		return;

	float width = 768;
	float height = 792;
	Rectangle titleRec = { dst.x + 10 + m_dialogOffset.x, dst.y + 10 + m_dialogOffset.y, width - 20 - 16, 16 };
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), titleRec)) {
		m_dragging = true;
		SetMouseCursor(MOUSE_CURSOR_RESIZE_ALL);
	}
	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		m_dragging = false;
		SetMouseCursor(MOUSE_CURSOR_DEFAULT);
	}

	if (m_dragging) {
		Vector2 delta = GetMouseDelta();
		m_dialogOffset = Vector2Add(m_dialogOffset, delta);
		m_dialogOffset = Vector2Clamp(m_dialogOffset, { 0, 0 }, { dst.width - 36, dst.height - 36 });
	}

	int borderColorNormal = GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL);
	int lineColor = GuiGetStyle(DEFAULT, LINE_COLOR);
	int fontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
	int baseColorPressed = GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED);
	GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, 0x808080FF);
	GuiSetStyle(DEFAULT, LINE_COLOR, 0x808080FF);
	GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, 0x3D3D3DFF);
	
	Font font = GuiGetFont();

	GuiSetFont(m_monoFont);
	GuiSetStyle(DEFAULT, TEXT_SIZE, m_monoFont.baseSize);

	Rectangle winRec = { dst.x + 10 + m_dialogOffset.x, dst.y + 10 + m_dialogOffset.y, width - 20, height - 20 };
	if (GuiWindowBox(winRec, "Debugger"))
		Close();
	
	Rectangle registersRec = { winRec.x + 10, winRec.y + 80, 140, 300 };
	DrawRegisters(registersRec);
	
	Rectangle dissassemblerRec = { registersRec.x + registersRec.width + 10, winRec.y + 80, 310, 300 };
	DrawDisassembler(dissassemblerRec);

	GuiSpinner({ dst.x+ m_dialogOffset.x+width-72-20, winRec.y + 57, 72, 24 }, "", &m_registersActive, 0, 4, false);

	// Video registers
	Rectangle videoRegRec = { dissassemblerRec.x + dissassemblerRec.width + 10, dissassemblerRec.y, 258, dissassemblerRec.height };

	if (m_registersActive == 0)
		DrawVideoRegisters(videoRegRec);
	else if (m_registersActive == 1)
		DrawOtherRegisters(videoRegRec);
	else if (m_registersActive == 2)
		DrawBreakpoints(videoRegRec);
	else if(m_registersActive == 3)
		DrawTiles(videoRegRec, 0);
	else
		DrawTiles(videoRegRec, 1);

	Rectangle flagsRec = { winRec.x + 10, registersRec.y + registersRec.height + 20, 70, 360 };
	DrawFlags(flagsRec);

	// Input: U, D, L, R, A, B, SE, ST
	Rectangle inputRec = { flagsRec.x + flagsRec.width, flagsRec.y, flagsRec.width, flagsRec.height };
	DrawInput(inputRec);

	Rectangle memoryRec = { dissassemblerRec.x, flagsRec.y, dissassemblerRec.width + 10 + videoRegRec.width, flagsRec.height };
	DrawMemory(memoryRec);

	Rectangle buttonsRec = { winRec.x + 10, winRec.y + 32, 0, 32 };
	DrawButtons(buttonsRec);

	GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, borderColorNormal);
	GuiSetStyle(DEFAULT, LINE_COLOR, lineColor);
	GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, baseColorPressed);
	GuiSetFont(font);
	GuiSetStyle(DEFAULT, TEXT_SIZE, font.baseSize);
}

void DebuggerNESDialog::Reset() {
	UpdatePrevValues();
	UpdatePad();
	m_debugger->Reset();
}

void DebuggerNESDialog::StepInto() {
	UpdatePrevValues();
	UpdatePad();
	m_debugger->StepInto();
}

void DebuggerNESDialog::RunUntilNextLine() {
	UpdatePrevValues();
	UpdatePad();
	m_debugger->ExecuteUntilNextLine();
}

void DebuggerNESDialog::RunOneFrame() {
	UpdatePrevValues();
	UpdatePad();
	m_debugger->ExecuteOneFrame();
}

void DebuggerNESDialog::RunOneSecond() {
	UpdatePrevValues();
	UpdatePad();
	for (int i = 0; i < 60; i++) {
		if (!m_debugger->ExecuteOneFrame())
			break;
	}
}

void DebuggerNESDialog::SaveTiles() {
	const int width = 128;
	const int height = 256;
	u8* buffer = new u8[width * height * 3];
	m_debugger->GetTiles(buffer, width, height);
	Image img;
	img.width = width;
	img.height = height;
	img.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
	img.data = buffer;

	if (ExportImage(img, "tiles.png"))
		printf("File succesfully saved as tiles.png\n");
	else
		printf("Error trying to save the tiles\n");

	delete[] buffer;
}

void DebuggerNESDialog::DrawButtons(Rectangle dst) {
	float x = dst.x;
	float y = dst.y;
	float w = dst.width;
	float h = dst.height;

	Font font = GuiGetFont();

	GuiEnableTooltip();

	GuiSetTooltip("Reset");
	w = 32;
	if (GuiButton(Rectangle{ x, y, w, h }, GuiIconText(ICON_RESTART, nullptr)))
		Reset();

	GuiSetTooltip("Step into");
	x += w + 10.0f;
	w = 32;
	if (GuiButton(Rectangle{ x, y, w, h }, GuiIconText(ICON_STEP_INTO, nullptr)))
		StepInto();

	GuiSetTooltip("Run until next line");
	x += w + 10.0f;
	w = 32;
	if (GuiButton(Rectangle{ x, y, w, h }, GuiIconText(ICON_RUN_UNTIL_NEXT_LINE, nullptr)))
		RunUntilNextLine();

	GuiSetTooltip("Run one frame");
	x += w + 10.0f;
	w = 32;
	if (GuiButton(Rectangle{ x, y, w, h }, GuiIconText(ICON_RUN_1_FRAME, nullptr)))
		RunOneFrame();

	GuiSetTooltip("Run 60 frames");
	x += w + 10.0f;
	w = 32;
	if (GuiButton(Rectangle{ x, y, w, h }, GuiIconText(ICON_RUN_60_FRAMES, nullptr)))
		RunOneSecond();

	GuiSetTooltip("Save tiles");
	x += w + 10.0f;
	w = 32;
	if (GuiButton(Rectangle{ x, y, w, h }, GuiIconText(ICON_FILETYPE_IMAGE, nullptr)))
		SaveTiles();

	GuiDisableTooltip();
}

void DebuggerNESDialog::DrawRegisters(Rectangle dst) {
	// Registers
	GuiGroupBox(dst, "CPU Registers");
	float x = dst.x + 10;
	float y = dst.y + 16;
	float w = 64;
	float h = 16;
	GuiLabel(Rectangle{ x + 0, y, w, h }, "Name");
	GuiLabel(Rectangle{ x + w, y, w, h }, "Value");
	y += 24;
	Color color = GetColor(GuiGetStyle(DEFAULT, LINE_COLOR));
	DrawRectangleRec(Rectangle{ dst.x, dst.y + 40, dst.width, 1 }, color);
	DrawRectangleRec(Rectangle{ dst.x + w, dst.y+10, 1, dst.height-10 }, color);
	y += 16;
	
	DrawKeyValue(x, y, w, w, h, "A",  m_debugger->GetRegA());
	DrawKeyValue(x, y, w, w, h, "X",  m_debugger->GetRegX());
	DrawKeyValue(x, y, w, w, h, "Y",  m_debugger->GetRegY());
	DrawKeyValue(x, y, w, w, h, "P",  m_debugger->GetRegP());
	DrawKeyValue(x, y, w, w, h, "PC", m_debugger->GetRegPC());
	DrawKeyValue(x, y, w, w, h, "S",  m_debugger->GetRegS());
}

void DebuggerNESDialog::DrawFlags(Rectangle dst) {
	// Flags: C, Z, I, D, B, B, V, N
	GuiGroupBox(dst, "Flags");

	float x = dst.x + 20;
	float y = dst.y + 24;
	float w = 60;
	float h = dst.height;

	w = 16;
	h = 16;
	bool flags[8] = { 0 };
	const char* flagName[] = { "C", "Z", "I", "D", "B", "B", "V", "N" };
	for (int i = 0; i < 8; i++) {
		flags[i] = m_debugger->GetFlag(i);
		GuiCheckBox({ x, y, w, h }, flagName[i], &flags[i]);
		y += 28;
	}
}

void DebuggerNESDialog::DrawInput(Rectangle dst) {
	GuiGroupBox(dst, "Input");
	float x = dst.x + 16;
	float y = dst.y + 24;
	float w = 60;
	float h = dst.height;

	w = 16;
	h = 16;
	const char* inputName[] = { "U", "D", "L", "R", "A", "B", "SE", "ST" };
	for (int i = 0; i < 8; i++) {
		GuiCheckBox({ x, y, w, h }, inputName[i], &m_inputs[i]);
		y += 28;
	}
}

void DebuggerNESDialog::InitDisassemblerVars(u16& currentAddress, u16& nextAddress, std::string& name, std::string& data, u16& pc) {
	m_debugger->DisassembleNext(currentAddress, nextAddress, name, data);
	pc = currentAddress;
	if ((currentAddress >= m_disassemblerFirst) && (currentAddress <= m_disassemblerLast)) {
		currentAddress = m_disassemblerFirst;
		m_debugger->DisassembleOne(currentAddress, nextAddress, name, data);
	}
	else
		m_disassemblerFirst = currentAddress;
}

void DebuggerNESDialog::DrawDisassembler(Rectangle dst) {
	GuiGroupBox(dst, "Dissassembler");
	float x = dst.x + 5;
	float y = dst.y + 16;
	float w1 = 21;
	float w2 = 84;
	float w3 = 100;
	float w4 = 72;
	float h = 16;

	float x1 = x;
	float x2 = x1 + w1;
	float x3 = x2 + w2;
	float x4 = x3 + w3;

	GuiLabel(Rectangle{ x2, y, w2, h }, "Address");
	GuiLabel(Rectangle{ x3, y, w3, h }, "Name");
	GuiLabel(Rectangle{ x4, y, w4, h }, "Data");
	y += 24;

	Color color = GetColor(GuiGetStyle(DEFAULT, LINE_COLOR));
	DrawRectangleRec(Rectangle{ dst.x, dst.y + 40, dst.width, 1 }, color);
	y += 16;

	u16 currentAddress, nextAddress, pc, second;
	currentAddress = nextAddress = pc = second = 0;
	std::string address, name, data;
	const int lines = 10;

	InitDisassemblerVars(currentAddress, nextAddress, name, data, pc);

	for (int i = 0; i < lines; i++) {
		address = m_debugger->HexToString(currentAddress, 4, '0');

		if (i == 1)
			second = currentAddress;

		if ((currentAddress == pc) && (i >= ((lines / 2) - 1)))
			m_disassemblerFirst = second;

		GuiLabel(Rectangle{ x1, y, w1, h }, "");
		GuiLabel(Rectangle{ x2, y, w2, h }, address.c_str());
		GuiLabel(Rectangle{ x3, y, w3, h }, name.c_str());
		GuiLabel(Rectangle{ x4, y, w4, h }, data.c_str());
		Rectangle iconRec = { x1, y, 16, 16 };
		DrawDisassemblerIcon(iconRec, currentAddress, pc);

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), iconRec)) {
			if (m_debugger->HasBreakpoint(currentAddress))
				m_debugger->DelBreakpoint(currentAddress);
			else
				m_debugger->AddBreakpoint(currentAddress);
		}

		if (nextAddress < currentAddress)
			break;
		else {
			m_disassemblerLast = currentAddress;
			currentAddress = nextAddress;
			m_debugger->DisassembleOne(currentAddress, nextAddress, name, data);
		}

		y += 24;
	}
}

void DebuggerNESDialog::DrawDisassemblerIcon(Rectangle dst, u16 currentAddress, u16 pc) {
	if (m_debugger->HasBreakpoint(currentAddress))
		GuiDrawIcon(ICON_DISASSEMBLER_BREAKPOINT, (int)dst.x, (int)dst.y, 1, RED);

	if (currentAddress == pc)
		GuiDrawIcon(ICON_DISASSEMBLER_CURRENT, (int)dst.x, (int)dst.y, 1, GREEN);
}

void DebuggerNESDialog::DrawVideoRegisters(Rectangle dst) {
	Rectangle panelView = { 0 };
	GuiScrollPanel({dst.x, dst.y + 40, dst.width, dst.height-40}, nullptr, {dst.x, dst.y, dst.width - 14, 30*24+24}, &m_videoRegsScroll, &panelView);

	GuiGroupBox(dst, "Video registers");
	float x = dst.x + 10;
	float y = dst.y + 16;
	float w1 = dst.width - 76 - 12;
	float w2 = dst.width - w1;
	float h = 16;
	GuiLabel(Rectangle{ x +  0, y, w1, h }, "Name");
	GuiLabel(Rectangle{ x + w1, y, w2, h }, "Value");
	y += 24;
	Color color = GetColor(GuiGetStyle(DEFAULT, LINE_COLOR));
	DrawRectangleRec(Rectangle{ dst.x, dst.y + 40, dst.width, 1 }, color);
	DrawRectangleRec(Rectangle{ dst.x + w1, dst.y + 10, 1, dst.height - 10 }, color);
	
	BeginScissorMode((int)panelView.x, (int)panelView.y, (int)panelView.width, (int)panelView.height);
	
	x = panelView.x + 10;
	y = panelView.y + 16 + m_videoRegsScroll.y;

	DrawKeyValue(x, y, w1, w2, h, "Frames",				m_debugger->GetVideoNumFrames());
	DrawKeyValue(x, y, w1, w2, h, "Cycles",				m_debugger->GetVideoX());
	DrawKeyValue(x, y, w1, w2, h, "Scanline",			m_debugger->GetVideoY());
	DrawKeyValue(x, y, w1, w2, h, "ScrollX",			m_debugger->GetVideoScrollX());
	DrawKeyValue(x, y, w1, w2, h, "ScrollY",			m_debugger->GetVideoScrollY());
	DrawKeyValue(x, y, w1, w2, h, "CoarseX",			m_debugger->GetVideoCoarseX());
	DrawKeyValue(x, y, w1, w2, h, "CoarseY",			m_debugger->GetVideoCoarseY());
	DrawKeyValue(x, y, w1, w2, h, "Name table sel.",	m_debugger->GetVideoNameTable());
	DrawKeyValue(x, y, w1, w2, h, "Fine Y scroll",		m_debugger->GetVideoFineY());
	DrawKeyValue(x, y, w1, w2, h, "Curr. address",		m_debugger->GetVideoCurrentAddress());
	DrawKeyValue(x, y, w1, w2, h, "Temp. address",		m_debugger->GetVideoTempAddress());
	DrawKeyValue(x, y, w1, w2, h, "Fine X scroll",		m_debugger->GetVideoFineXScroll());
	DrawKeyValue(x, y, w1, w2, h, "Write toggle",		m_debugger->GetVideoWriteToggle());
	DrawKeyValue(x, y, w1, w2, h, "$2000 (PPUCTRL)",	m_debugger->GetVideoReg(0));
	DrawKeyValue(x, y, w1, w2, h, "BaseNameTable",		m_debugger->GetVideoBaseNameTableAddress());
	DrawKeyValue(x, y, w1, w2, h, "Address incr.",		m_debugger->GetVideoVRAMAddressIncrement());
	DrawKeyValue(x, y, w1, w2, h, "Sprite pattern",		m_debugger->GetVideoSpritePatternTableAddress());
	DrawKeyValue(x, y, w1, w2, h, "BG pattern",			m_debugger->GetVideoBGPatternTableAddress());
	DrawKeyValue(x, y, w1, w2, h, "Sprite size",		m_debugger->GetVideoSpriteSize());
	DrawKeyValue(x, y, w1, w2, h, "Generate NMI",		m_debugger->GetVideoGenerateNMI());
	DrawKeyValue(x, y, w1, w2, h, "$2001 (PPUMASK)",	m_debugger->GetVideoReg(1));
	DrawKeyValue(x, y, w1, w2, h, "Grayscale",			m_debugger->GetVideoGrayscale());
	DrawKeyValue(x, y, w1, w2, h, "Show BG left8",		m_debugger->GetVideoShowBGLeft8());
	DrawKeyValue(x, y, w1, w2, h, "Show spr. left8",	m_debugger->GetVideoShowSpritesLeft8());
	DrawKeyValue(x, y, w1, w2, h, "Show BG",			m_debugger->GetVideoShowBG());
	DrawKeyValue(x, y, w1, w2, h, "Show sprites",		m_debugger->GetVideoShowSprites());
	DrawKeyValue(x, y, w1, w2, h, "$2002 (PPUSTAT)",	m_debugger->GetVideoReg(2));
	DrawKeyValue(x, y, w1, w2, h, "Spr. overflow",		m_debugger->GetVideoSpriteOverflow());
	DrawKeyValue(x, y, w1, w2, h, "Spr. 0 hit",			m_debugger->GetVideoSprite0Hit());
	DrawKeyValue(x, y, w1, w2, h, "VBlank",				m_debugger->GetVideoVBlank());
	
	EndScissorMode();
}

void DebuggerNESDialog::DrawKeyValue(float x, float& y, float w1, float w2, float h, const std::string& key, const std::string& value) {
	Font font = GuiGetFont();
	float offset = (float)(5 - value.length()) * (font.glyphs[0].advanceX + 1);
	GuiLabel(Rectangle{ x +  0, y, w1, h }, key.c_str());
	
	if (m_prevValues[key] != value) {
		int color = GuiGetStyle(LABEL, TEXT_COLOR_NORMAL);
		GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, 0xFF0000FF);
		GuiLabel(Rectangle{ x + w1 + offset, y, w2, h }, value.c_str());
		GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, color);
	}
	else
		GuiLabel(Rectangle{ x + w1 + offset, y, w2, h }, value.c_str());

	y += 24;
}

void DebuggerNESDialog::DrawOtherRegisters(Rectangle dst) {
	Rectangle panelView = { 0 };
	float height = 24 * (4 + m_debugger->GetCartridgePRGBanksVisible() + m_debugger->GetCartridgeCHRBanksVisible() + (m_debugger->HasCartridgeIRQ() ? 4 : 0)) + 24;
	GuiScrollPanel({ dst.x, dst.y + 40, dst.width, dst.height - 40 }, nullptr, { dst.x, dst.y, dst.width - 14, height}, &m_otherRegsScroll, &panelView);

	GuiGroupBox(dst, "Other registers");
	float x = dst.x + 10;
	float y = dst.y + 16;
	float w1 = dst.width - 76 - 12;
	float w2 = dst.width - w1;
	float h = 16;
	GuiLabel(Rectangle{ x + 0, y, w1, h }, "Name");
	GuiLabel(Rectangle{ x + w1, y, w2, h }, "Value");
	y += 24;
	Color color = GetColor(GuiGetStyle(DEFAULT, LINE_COLOR));
	DrawRectangleRec(Rectangle{ dst.x, dst.y + 40, dst.width, 1 }, color);
	DrawRectangleRec(Rectangle{ dst.x + w1, dst.y + 10, 1, dst.height - 10 }, color);

	BeginScissorMode((int)panelView.x, (int)panelView.y, (int)panelView.width, (int)panelView.height);

	x = panelView.x + 10;
	y = panelView.y + 16 + m_otherRegsScroll.y;

	DrawKeyValue(x, y, w1, w2, h, "Mapper ID",	m_debugger->GetMapperID());
	DrawKeyValue(x, y, w1, w2, h, "Map. Name",	m_debugger->GetMapperName());
	DrawKeyValue(x, y, w1, w2, h, "PRG Banks",	m_debugger->GetCartridgePRGBanks());
	for (int i = 0; i < m_debugger->GetCartridgePRGBanksVisible(); i++) {
		const char* text = TextFormat("PRG %i", i);
		DrawKeyValue(x, y, w1, w2, h, text,		m_debugger->GetCartridgePRGBank(i));
	}
	DrawKeyValue(x, y, w1, w2, h, "CHR Banks",	m_debugger->GetCartridgeCHRBanks());
	for (int i = 0; i < m_debugger->GetCartridgeCHRBanksVisible(); i++) {
		const char* text = TextFormat("CHR %i", i);
		DrawKeyValue(x, y, w1, w2, h, text,		m_debugger->GetCartridgeCHRBank(i));
	}
	if (m_debugger->HasCartridgeIRQ()) {
		DrawKeyValue(x, y, w1, w2, h, "IRQ Counter",     m_debugger->GetCartridgeIRQCounter());
		DrawKeyValue(x, y, w1, w2, h, "IRQ Reload Flag", m_debugger->GetCartridgeIRQReloadFlag());
		DrawKeyValue(x, y, w1, w2, h, "IRQ Reload",      m_debugger->GetCartridgeIRQReloadValue());
		DrawKeyValue(x, y, w1, w2, h, "IRQ Enabled",     m_debugger->GetCartridgeIRQEnabled());
	}

	EndScissorMode();
}

void DebuggerNESDialog::DrawMemory(Rectangle dst) {
	GuiGroupBox(dst, "Memory");
	float x = dst.x + 20;
	float y = dst.y + 16;
	GuiLabel(Rectangle{ x, y, 72, 24 }, "Address:");
	if (GuiTextBox({ x + 72 + 10, y, 51, 24 }, m_addressMem, 5, m_addressMemEditMode)) {
		m_addressMemEditMode = !m_addressMemEditMode;
	}
	GuiToggleGroup({ x + 240, y, 72, 24 }, "6502;PPU;OAM", &m_memoryActive);
	y = dst.y + 24 + 20;

	u16 addressValue = HexTextTou16(m_addressMem);

	u16 maxMem = 0;
    switch (m_memoryActive) {
        case 0: maxMem = 0xFFFF; break;
        case 1: maxMem = 0x3FFF; break;
        case 2: maxMem = 0xFF; break;
        default: maxMem = 0xFFFF;
    }

	addressValue = addressValue & 0xFFF0;
	int numLines = 12;
	u16 maxStart = (u16)(((int)maxMem+1) - (0x10 * numLines));
	if (addressValue > maxStart)
		addressValue = maxStart;

	GuiLabel(Rectangle{ x, y, 800, 24 }, "      00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
	y += 24;
	for (int i = 0; i < 12; i++) {
		if (m_memoryActive == 0)
			GuiLabel(Rectangle{ x, y, 800, 24 }, m_debugger->GetMem(addressValue, (addressValue + 0x0F)).c_str());
		else if (m_memoryActive == 1)
			GuiLabel(Rectangle{ x, y, 800, 24 }, m_debugger->GetVMem(addressValue, (addressValue + 0x0F)).c_str());
		else if (m_memoryActive == 2)
			GuiLabel(Rectangle{ x, y, 800, 24 }, m_debugger->GetOAMData(addressValue, (addressValue + 0x0F)).c_str());
		addressValue += 0x10;
		y += 24;
	}
}

u16 DebuggerNESDialog::HexTextTou16(char* text) {
	char* endPtr = text;
	u16 addressValue = (u16)strtol(text, &endPtr, 16);
	if (*endPtr != '\0') {
		size_t offset = endPtr - text;
		for (size_t i = offset; i < 4; i++) {
			text[i] = text[i + 1];
		}
	}

	return addressValue;
}

void DebuggerNESDialog::DrawBreakpoints(Rectangle dst) {
	GuiGroupBox(dst, "Breakpoints");

	static char brkPts[512];
	brkPts[0] = '\0';
	size_t offset = 0;
	for (int i = 0; i < m_debugger->GetNumBreakpoints(); i++) {
		std::string brkPtString = m_debugger->HexToString(m_debugger->GetBreakpoint(i), 4, '0');
		offset += snprintf(brkPts+offset, 512-offset, "%s%s", brkPtString.c_str(), i == m_debugger->GetNumBreakpoints() ? "" : ",");
	}
	int count = 0;
	char** text = TextSplit(brkPts, ',', &count);
	Rectangle listViewRec = { dst.x + 10, dst.y + 24, dst.width - 20, 190 };
	GuiListViewEx(listViewRec, (const char**)text, m_debugger->GetNumBreakpoints(), &m_breakpointsListViewScrollIndex, &m_breakpointsListViewActive, &m_breakpointsListViewFocus);

	if (m_breakpointsListViewActive >= 0) {
		for (int i=0; i<4; i++)
			m_breakpointsSelectedAddress[i] = text[m_breakpointsListViewActive][i+1];
		m_breakpointsSelectedAddress[4] = '\0';
	}

	Rectangle labelRec = { dst.x + dst.width - 80 - 60 -10, listViewRec.y + listViewRec.height + 10, 80, 24 };
	GuiLabel(labelRec, "Address:");

	Rectangle tbRec = { dst.x + dst.width - 50 - 10, listViewRec.y + listViewRec.height + 10, 51, 24 };
	if (GuiTextBox(tbRec, m_breakpointsSelectedAddress, 5, m_brkptsSelAddEditMode)) {
		m_brkptsSelAddEditMode = !m_brkptsSelAddEditMode;
		m_breakpointsListViewActive = -1;
	}

	u16 value = HexTextTou16(m_breakpointsSelectedAddress);

	Rectangle addRec = { dst.x + 10, tbRec.y + tbRec.height + 10, dst.width / 2.0f - 10 - 5, 24 };
	if (GuiButton(addRec, "Add")) {
		if (TextLength(m_breakpointsSelectedAddress) > 0) {
			m_debugger->AddBreakpoint(value);
			m_breakpointsSelectedAddress[0] = '\0';
		}
	}

	Rectangle delRec = { addRec.x + addRec.width + 10, addRec.y, dst.width / 2.0f - 10 - 5, addRec.height };
	if (GuiButton(delRec, "Delete")) {
		if (TextLength(m_breakpointsSelectedAddress) > 0) {
			m_debugger->DelBreakpoint(value);
			m_breakpointsSelectedAddress[0] = '\0';
		}
	}
}

void DebuggerNESDialog::UpdatePrevValues() {
	m_prevValues["A"] = m_debugger->GetRegA();
	m_prevValues["X"] = m_debugger->GetRegX();
	m_prevValues["Y"] = m_debugger->GetRegY();
	m_prevValues["P"] = m_debugger->GetRegP();
	m_prevValues["PC"] = m_debugger->GetRegPC();
	m_prevValues["S"] = m_debugger->GetRegS();

	m_prevValues["Frames"]			= m_debugger->GetVideoNumFrames();
	m_prevValues["Cycles"]			= m_debugger->GetVideoX();
	m_prevValues["Scanline"]		= m_debugger->GetVideoY();
	m_prevValues["ScrollX"]			= m_debugger->GetVideoScrollX();
	m_prevValues["ScrollY"]			= m_debugger->GetVideoScrollY();
	m_prevValues["Curr. address"]	= m_debugger->GetVideoCurrentAddress();
	m_prevValues["CoarseX"]			= m_debugger->GetVideoCoarseX();
	m_prevValues["CoarseY"]			= m_debugger->GetVideoCoarseY();
	m_prevValues["Name table sel."] = m_debugger->GetVideoNameTable();
	m_prevValues["Fine Y scroll"]	= m_debugger->GetVideoFineY();
	m_prevValues["Temp. address"]	= m_debugger->GetVideoTempAddress();
	m_prevValues["Fine X scroll"]	= m_debugger->GetVideoFineXScroll();
	m_prevValues["Write toggle"]	= m_debugger->GetVideoWriteToggle();
	m_prevValues["$2000 (PPUCTRL)"] = m_debugger->GetVideoReg(0);
	m_prevValues["BaseNameTable"]	= m_debugger->GetVideoBaseNameTableAddress();
	m_prevValues["Address incr."]	= m_debugger->GetVideoVRAMAddressIncrement();
	m_prevValues["Sprite pattern"]	= m_debugger->GetVideoSpritePatternTableAddress();
	m_prevValues["BG pattern"]		= m_debugger->GetVideoBGPatternTableAddress();
	m_prevValues["Sprite size"]		= m_debugger->GetVideoSpriteSize();
	m_prevValues["Generate NMI"]	= m_debugger->GetVideoGenerateNMI();
	m_prevValues["$2001 (PPUMASK)"] = m_debugger->GetVideoReg(1);
	m_prevValues["Grayscale"]		= m_debugger->GetVideoGrayscale();
	m_prevValues["Show BG left8"]	= m_debugger->GetVideoShowBGLeft8();
	m_prevValues["Show spr. left8"] = m_debugger->GetVideoShowSpritesLeft8();
	m_prevValues["Show BG"]			= m_debugger->GetVideoShowBG();
	m_prevValues["Show sprites"]	= m_debugger->GetVideoShowSprites();
	m_prevValues["$2002 (PPUSTAT)"] = m_debugger->GetVideoReg(2);
	m_prevValues["Spr. overflow"]	= m_debugger->GetVideoSpriteOverflow();
	m_prevValues["Spr. 0 hit"]		= m_debugger->GetVideoSprite0Hit();
	m_prevValues["VBlank"]			= m_debugger->GetVideoVBlank();

	m_prevValues["Mapper ID"]	= m_debugger->GetMapperID();
	m_prevValues["Map. Name"]	= m_debugger->GetMapperName();
	m_prevValues["PRG Banks"]	= m_debugger->GetCartridgePRGBanks();
	for (int i = 0; i < m_debugger->GetCartridgePRGBanksVisible(); i++) {
		const char* text = TextFormat("PRG %i", i);
		m_prevValues[text]		= m_debugger->GetCartridgePRGBank(i);
	}
	m_prevValues["CHR Banks"]	= m_debugger->GetCartridgeCHRBanks();
	for (int i = 0; i < m_debugger->GetCartridgeCHRBanksVisible(); i++) {
		const char* text = TextFormat("CHR %i", i);
		m_prevValues[text] = m_debugger->GetCartridgeCHRBank(i);
	}
	if (m_debugger->HasCartridgeIRQ()) {
		m_prevValues["IRQ Counter"]     = m_debugger->GetCartridgeIRQCounter();
		m_prevValues["IRQ Reload Flag"] = m_debugger->GetCartridgeIRQReloadFlag();
		m_prevValues["IRQ Reload"]      = m_debugger->GetCartridgeIRQReloadValue();
		m_prevValues["IRQ Enabled"]     = m_debugger->GetCartridgeIRQEnabled();
	}
}

void DebuggerNESDialog::DrawTiles(Rectangle dst, int slot) {
	const int width = 128;
	const int height = 256;
	static u8 buffer[width * height * 3];

	GuiGroupBox(dst, slot == 0 ? "Tiles 1" : "Tiles 2");

	m_debugger->GetTiles(buffer, width, height);

	UpdateTexture(m_tilesTexture, buffer);

	DrawTexturePro(m_tilesTexture, { 0.0f, slot*128.0f, 128.0f, 128.0f }, { dst.x+1, dst.y+24, 256, 256 }, { 0, 0 }, 0, WHITE);
}
