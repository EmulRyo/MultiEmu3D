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

#include "CPU.h"
#include "Instructions.h"
#include <sstream>
#include "Registers.h"
#include "Video.h"
#include "Pad.h"
#include "SMSException.h"

#ifdef MAKEGBLOG
#include "Log.h"
#endif

using namespace std;

CPU::CPU(Video *v, Pad *p, Sound *s): Memory(this, v, p, s)
{
	Init(v, p);
}

CPU::CPU(Video *v, Pad *p, Cartridge *c, Sound *s): Memory(this, v, p, s)
{
    SetCartridge(c);
	Init(v, p);
}

void CPU::Init(Video *v, Pad *p) {
    m_v = v;
    m_p = p;
    // 3,579545 Mhz NTSC (59.94Hz = 59718,802)
    // 3,546893 Mhz PAL
    m_cyclesFrame = 59719;
    m_GameGear = false;
	ResetGlobalVariables();
}


CPU::~CPU() {}

void CPU::ResetGlobalVariables() {
    m_numInstructions = 0;
}

void CPU::Reset() {
    ResetGlobalVariables();
	ResetRegs();
	ResetMem();
    m_v->Reset();
	m_v->ClearScreen();
	if (m_s && (m_s->GetEnabled()))
	{
		m_s->Stop();
		m_s->Start();
	}
}

int CPU::ExecuteOneFrame() {
    int cycles = Execute(m_cyclesFrame);
    m_s->EndFrame(cycles);
    return cycles;
}

int CPU::Execute(int cyclesToExecute)
{
	if (!m_c)
		return 0;
	
    m_cycles = 0;
	u8 opcode = 0, nextOpcode = 0, lastOpcode = 0;

	Instructions inst(GetPtrRegisters(), GetPtrMemory());

    while (m_cycles < cyclesToExecute)
    {
		m_numInstructions++;
		lastOpcode = opcode;
		opcode = MemR(GetPC());
        nextOpcode = MemR(GetPC() + 1);
		
		ExecuteOpcode(opcode, inst);
        
        int tmpCycles = m_lastCycles;
        
        m_v->Update(m_lastCycles);
        
        Interrupts(inst);
        
        m_cycles += m_lastCycles;
        m_lastCycles -= tmpCycles;
		
	}//end while
    
    return m_cycles;
}

void CPU::SetGGMode(bool value) {
    m_GameGear = value;
    m_video->SetGGMode(value);
    m_pad->SetGGMode(value);
}

int  CPU::GetElapsedCycles() {
    return m_cycles;
}

void CPU::Interrupts(Instructions &inst) {
    if (m_pad->PauseInterrupt()) {
        inst.PUSH(GetPC());
        SetPC(0x66);
        SetIFF2(GetIFF1());
        SetIFF1(false);
        SetHalt(false);
        SetIntPending(false);
    }
    
    if (GetIFF1()) {
        if (m_video->Interrupt()) {
            inst.PUSH(GetPC());
            SetPC(0x38);
            SetIFF1(false);
            SetIFF2(false);
            SetHalt(false);
        }
    }
    
    if (GetIntPending()) {
        SetIFF1(true);
        SetIFF2(true);
        SetIntPending(false);
    }
}

void CPU::LoadStateFromRAM(istream *stream) {
    LoadRegs(stream);
    LoadMemory(stream);
    m_v->LoadState(stream);
    m_c->LoadStateMBC(stream);
}

void CPU::SaveStateToRAM(ostream *stream) {
    SaveRegs(stream);
    SaveMemory(stream);
    m_v->SaveState(stream);
    m_c->SaveStateMBC(stream);
}

void CPU::SaveState(string saveDirectory, int numSlot)
{
	if (m_c == NULL)
	{
		throw SMSException("There is not rom loaded. The process can't continue.");
	}
	
	stringstream st;
	st << saveDirectory << m_c->GetName().c_str();
	st << "-" << numSlot << ".sav";
	string fileName = st.str();
	ofstream * file = new ofstream(fileName.c_str(), ios::out|ios::binary|ios::trunc);

	if (file && file->is_open())
	{
		int version = SAVE_STATE_VERSION;
		file->write((char *)&version, sizeof(int));
		
		SaveRegs(file);
		SaveMemory(file);
        m_v->SaveState(file);
		m_c->SaveStateMBC(file);
		
		file->close();
	}
	
	if (file)
		delete file;
}

void CPU::LoadState(string loadDirectory, int numSlot)
{
	if (!m_c)
	{
		throw SMSException("There is not rom loaded. The process can't continue.");
	}
	
	stringstream st;
	st << loadDirectory << m_c->GetName().c_str();
	st << "-" << numSlot << ".sav";
	string fileName = st.str();
	ifstream * file = new ifstream(fileName.c_str(), ios::in|ios::binary);
	
	if (file && file->is_open())
	{
		int version = 0;
		file->read((char *)&version, sizeof(int));
		if (version != SAVE_STATE_VERSION)
		{
			file->close();
			delete file;
			throw SMSException("This filesave is not compatible and can't be loaded.");
		}
		
		LoadRegs(file);
		LoadMemory(file);
        m_v->LoadState(file);
		m_c->LoadStateMBC(file);
		
		file->close();
		
		delete file;
	}
	else
	{
		if (file)
			delete file;
		
		throw SMSException("Unable to open the filesave.");
	}
}
