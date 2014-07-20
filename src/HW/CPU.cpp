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
#include "InstructionsDef.h"

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
    LoadCartridge(c);
	Init(v, p);
}

void CPU::Init(Video *v, Pad *p)
{
    m_v = v;
    m_p = p;
    // 3,579545 Mhz NTSC
    // 3,546893 Mhz PAL
    m_cyclesFrame = 59659;
	ResetGlobalVariables();
	
#ifdef MAKEGBLOG
	m_log = new QueueLog(1000000);
#endif
}


CPU::~CPU() {}

void CPU::ResetGlobalVariables()
{
    m_numInstructions = 0;
    
}

void CPU::Reset()
{
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
    return Execute(m_cyclesFrame);
}

int CPU::Execute(int cyclesToExecute)
{
	if (!m_c)
		return 0;
	
    int cycles = 0;
	u8 opcode = 0, nextOpcode = 0, lastOpcode = 0;

	Instructions inst(GetPtrRegisters(), GetPtrMemory());

    while (cycles < cyclesToExecute)
    {
		m_numInstructions++;
		lastOpcode = opcode;
		opcode = MemR(GetPC());
        nextOpcode = MemR(GetPC() + 1);
		
		if (!GetHalt())
			ExecuteOpcode(opcode, inst);
        
        m_lastCycles += GetInstructionCycles(opcode);
        if (m_lastCycles == 0)
            m_lastCycles = 2;
        
        int tmpCycles = m_lastCycles;
        
        m_v->Update(m_lastCycles);
        
        Interrupts(inst);
        
        cycles += m_lastCycles;
        m_lastCycles -= tmpCycles;
		
	}//end while
    
    return cycles;
}

void CPU::Interrupts(Instructions &inst)
{
    if (GetIFF1()) {
        if (m_video->Interrupt()) {
            inst.PUSH(GetPC());
            SetPC(0x38);
            SetIFF1(false);
            SetIFF2(false);
        }
    }
    
    if (GetIntPending()) {
        SetIFF1(true);
        SetIFF2(true);
        SetIntPending(false);
    }
}

void CPU::OnEndFrame()
{
	m_v->RefreshScreen();
	if (m_s)
		m_s->EndFrame();
}

#ifdef MAKEGBLOG

void CPU::SaveLog()
{
	log->Save("log.txt");
}

#endif

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
		file->write(m_c->GetName().c_str(), 16);
		
		SaveRegs(file);
		SaveMemory(file);
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
		
		char *buffer = new char[16];
		file->read(buffer, 16);
        
        string cartName = m_c->GetGoodName(buffer);
		delete[] buffer;
		if (cartName != m_c->GetName())
		{
			file->close();
			delete file;
			throw SMSException("This filesave does not belong to this rom and can't be loaded.");
		}
		
		LoadRegs(file);
		LoadMemory(file);
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
