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

#include <sstream>
#include <fstream>
#include <iomanip>
#include "Registers.h"
#include "../Common/Exception.h"

using namespace std;
using namespace GameBoy;

Registers::Registers() {ResetRegs();}

Registers::~Registers()
{
}

Registers *Registers::GetPtrRegisters() {return this;}

u16 Registers::GetReg(e_registers reg)
{
	switch (reg){
		case A: return GetA(); break;
		case B: return GetB(); break;
		case C: return GetC(); break;
		case D: return GetD(); break;
		case E: return GetE(); break;
		case F: return GetF(); break;
		case H: return GetH(); break;
		case L: return GetL(); break;
		case AF: return GetAF(); break;
		case BC: return GetBC(); break;
		case DE: return GetDE(); break;
		case HL: return GetHL(); break;
		case PC: return GetPC(); break;
		case SP: return GetSP(); break;
		default:
			stringstream out;
			out << "Get_Reg - Error, incorrect register: " << reg << endl;
			throw Exception(out.str().data());
	}
}

void Registers::SetReg(e_registers reg, u16 value)
{
	switch (reg){
		case A: SetA((u8)value); break;
		case B: SetB((u8)value); break;
		case C: SetC((u8)value); break;
		case D: SetD((u8)value); break;
		case E: SetE((u8)value); break;
		case F: SetF((u8)value); break;
		case H: SetH((u8)value); break;
		case L: SetL((u8)value); break;
		case AF: SetAF(value); break;
		case BC: SetBC(value); break;
		case DE: SetDE(value); break;
		case HL: SetHL(value); break;
		case PC: SetPC(value); break;
		case SP: SetSP(value); break;
		default:
			stringstream out;
			out << "Set_Reg - Error, incorrect register: " << reg << endl;
			throw Exception(out.str().data());
	}
}

u8 Registers::GetFlag(e_registers flag)
{
	switch (flag){
		case f_C: return GetFlagC();
		case f_H: return GetFlagH();
		case f_N: return GetFlagN();
		case f_Z: return GetFlagZ();
		default:
			stringstream out;
			out << "Error, incorrect flag (Get): " << flag << endl;
			throw Exception(out.str().data());
	}
}

void Registers::SetFlag(e_registers flag, u8 value)
{
	switch (flag){
		case f_C: SetFlagC(value);
		case f_H: SetFlagH(value);
		case f_N: SetFlagN(value);
		case f_Z: SetFlagZ(value);
		default:
			stringstream out;
			out << "Error, incorrect flag (Set): " << flag << endl;
			throw Exception(out.str().data());
	}
}

void Registers::ResetRegs()
{
	SetAF(0x11B0);
	SetBC(0x0013);
	SetDE(0x00D8);
	SetHL(0x014D);
	SetPC(0x0100);
	SetSP(0xFFFE);
	SetHalt(false);
	SetStop(false);
	SetIME(false);
}

string Registers::ToString()
{
	stringstream out;
	
	out << "PC: " << setfill('0') << setw(4) << uppercase << hex << (int)GetPC()
		<< ", AF: " << setfill('0') << setw(4) << uppercase << hex << (int)GetAF()
		<< ", BC: " << setfill('0') << setw(4) << uppercase << hex << (int)GetBC()
		<< ", DE: " << setfill('0') << setw(4) << uppercase << hex << (int)GetDE()
		<< ", HL: " << setfill('0') << setw(4) << uppercase << hex << (int)GetHL()
		<< ", SP: " << setfill('0') << setw(4) << uppercase << hex << (int)GetSP()
		<< ", H: " << GetHalt() << ", I: " << GetIME();
	
	return out.str();
}

void Registers::SaveRegs(ostream *stream)
{
	stream->write((char *)&m_af.doble, sizeof(u16));
	stream->write((char *)&m_bc.doble, sizeof(u16));
	stream->write((char *)&m_de.doble, sizeof(u16));
	stream->write((char *)&m_hl.doble, sizeof(u16));
	stream->write((char *)&m_pc, sizeof(u16));
	stream->write((char *)&m_sp, sizeof(u16));
	stream->write((char *)&m_IME, sizeof(bool));
	stream->write((char *)&m_pendingIME, sizeof(bool));
	stream->write((char *)&m_pendingIMEvalue, sizeof(bool));
	stream->write((char *)&m_halt, sizeof(bool));
	stream->write((char *)&m_stop, sizeof(bool));
}

void Registers::LoadRegs(istream *stream)
{
	stream->read((char *)&m_af.doble, sizeof(u16));
	stream->read((char *)&m_bc.doble, sizeof(u16));
	stream->read((char *)&m_de.doble, sizeof(u16));
	stream->read((char *)&m_hl.doble, sizeof(u16));
	stream->read((char *)&m_pc, sizeof(u16));
	stream->read((char *)&m_sp, sizeof(u16));
	stream->read((char *)&m_IME, sizeof(bool));
	stream->read((char *)&m_pendingIME, sizeof(bool));
	stream->read((char *)&m_pendingIMEvalue, sizeof(bool));
	stream->read((char *)&m_halt, sizeof(bool));
	stream->read((char *)&m_stop, sizeof(bool));
}
