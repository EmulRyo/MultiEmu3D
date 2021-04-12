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

#include "CPU.h"
#include "Instructions.h"
#include <sstream>
#include "Registers.h"
#include "Video.h"
#include "Pad.h"
#include "../Common/Exception.h"

using namespace std;
using namespace Nes;

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
	ResetGlobalVariables();
}


CPU::~CPU() {}

void CPU::ResetGlobalVariables() {
    m_numInstructions = 0;
}

void CPU::Reset() {
    ResetGlobalVariables();
	ResetRegs();
	u8 pch = MemR(0xFFFD);
	u8 pcl = MemR(0xFFFC);
	SetPC(pch << 8 | pcl);
	ResetMem();
    m_v->Reset();
	m_v->ClearScreen();
	if (m_s && (m_s->GetEnabled()))
	{
		m_s->Stop();
		m_s->Start();
	}
	OAMDMAPending = false;
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
	u8 opcode = 0, lastOpcode = 0;

	Instructions inst(GetPtrRegisters(), GetPtrMemory());

    while (m_cycles < cyclesToExecute)
    {
		m_numInstructions++;
		lastOpcode = opcode;
		opcode = MemR(GetPC());
		
		u8 cycles = ExecuteOpcode(opcode, inst);
		
		if (OAMDMAPending)
			OAMDMA();

        bool NMI = m_v->Update(cycles);

        Interrupts(NMI, inst);
        
        m_cycles += cycles;
		
	}//end while
    
    return m_cycles;
}

int  CPU::GetElapsedCycles() {
    return m_cycles;
}

void CPU::OAMDMARequest(u8 value) {
	OAMDMAPending = true;
	OAMDMAAddress = value;
}

void CPU::OAMDMA() {
	u16 address = OAMDMAAddress << 8;
	for (int i = 0; i < 256; i++) {
		m_v->WriteReg(0x2004, MemR(address + i));
	}
	m_cycles += 514;
	OAMDMAPending = false;
}

void CPU::Interrupts(bool NMI, Instructions &inst) {
	if (NMI)
		inst.NMI();
}

void CPU::LoadStateFromRAM(istream *stream) {
    LoadRegs(stream);
    LoadMemory(stream);
    m_v->LoadState(stream);
    m_c->LoadState(stream);
}

void CPU::SaveStateToRAM(ostream *stream) {
    SaveRegs(stream);
    SaveMemory(stream);
    m_v->SaveState(stream);
    m_c->SaveState(stream);
}

void CPU::SaveState(string saveDirectory, int numSlot)
{
	if (m_c == NULL)
	{
		throw Exception("There is not rom loaded. The process can't continue.");
	}
	
	stringstream st;
	st << saveDirectory << m_c->GetName().c_str();
	st << "-" << numSlot << ".sav";
	string fileName = st.str();
	ofstream * file = new ofstream(fileName.c_str(), ios::out|ios::binary|ios::trunc);

	if (file && file->is_open())
	{
		int version = NES_SAVE_STATE_VERSION;
		file->write((char *)&version, sizeof(int));
		
		SaveRegs(file);
		SaveMemory(file);
        m_v->SaveState(file);
		m_c->SaveState(file);
		
		file->close();
	}
	
	if (file)
		delete file;
}

void CPU::LoadState(string loadDirectory, int numSlot)
{
	if (!m_c)
	{
		throw Exception("There is not rom loaded. The process can't continue.");
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
		if (version != NES_SAVE_STATE_VERSION)
		{
			file->close();
			delete file;
			throw Exception("This filesave is not compatible and can't be loaded.");
		}
		
		LoadRegs(file);
		LoadMemory(file);
        m_v->LoadState(file);
		m_c->LoadState(file);
		
		file->close();
		
		delete file;
	}
	else
	{
		if (file)
			delete file;
		
		throw Exception("Unable to open the filesave.");
	}
}
