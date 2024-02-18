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
//#include "EmulationThread.h"
#include "AppDefs.h"
#include "raylib.h"

MainFrame::MainFrame(const std::string& fileName)
{
	// m_renderer = NULL;

    // create the emulation
    //m_emulation = new EmulationThread();

    //m_fullScreen = false;
    m_renderer = new RendererSW();
    
	//if (!fileName.empty())
	//	ChangeFile(fileName);

    std::string fixedFileName = "C:\\Users\\Pablo\\Documents\\Emulation\\NES ROMS\\0-NROM\\Balloon Fight (U) [!].nes";
    //std::string fixedFileName = "C:\\Users\\Pablo\\Documents\\Emulation\\GB ROMS\\Pokemon - Edicion Azul (S) [S][!].gb";
    m_device = new Nes::NES();
    //m_device = new GameBoy::GB();
    m_device->SetScreen(m_renderer);
    m_device->CartridgeLoad(fixedFileName);
}

MainFrame::~MainFrame()
{
	// No hacer nada aqui mejor hacerlo todo
    // en el OnClose
}

void MainFrame::Update(float deltaTime) {
    m_device->ExecuteOneFrame();
}

void MainFrame::Draw() {
    m_renderer->Draw();
}
