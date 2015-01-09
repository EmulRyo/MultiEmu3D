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

#include <sstream>
#include <fstream>
#include <iomanip>
#include "Registers.h"
#include "SMSException.h"

using namespace std;

Registers::Registers() {ResetRegs();}

void Registers::ResetRegs()
{
	SetAF(0x0000);
	SetBC(0x0000);
	SetDE(0x0000);
	SetHL(0x0000);
	SetPC(0x0000);
    SetIX(0x0000);
    SetIY(0x0000);
	SetSP(0xDFF0);
    SetR(0);
    SetAF2(0x0000);
    SetBC2(0x0000);
    SetDE2(0x0000);
    SetHL2(0x0000);
	SetHalt(false);
	SetIFF1(false);
    SetIFF2(false);
    SetIntMode(0);
    SetIncPC(true);
}

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
        case IX: return GetIX(); break;
        case IY: return GetIY(); break;
		default:
			stringstream out;
			out << "Get_Reg - Error, incorrect register: " << reg << endl;
			throw SMSException(out.str().data());
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
        case IX: SetIX(value); break;
        case IY: SetIY(value); break;
		default:
			stringstream out;
			out << "Set_Reg - Error, incorrect register: " << reg << endl;
			throw SMSException(out.str().data());
	}
}

u8 Registers::GetFlag(e_registers flag)
{
	switch (flag){
		case f_C: return GetFlagC();
		case f_H: return GetFlagH();
		case f_N: return GetFlagN();
		case f_Z: return GetFlagZ();
        case f_S: return GetFlagS();
        case f_PV: return GetFlagPV();
		default:
			stringstream out;
			out << "Error, incorrect flag (Get): " << flag << endl;
			throw SMSException(out.str().data());
	}
}

void Registers::SetFlag(e_registers flag, u8 value)
{
	switch (flag){
		case f_C: SetFlagC(value);
		case f_H: SetFlagH(value);
		case f_N: SetFlagN(value);
		case f_Z: SetFlagZ(value);
        case f_S: SetFlagS(value);
        case f_PV: SetFlagPV(value);
		default:
			stringstream out;
			out << "Error, incorrect flag (Set): " << flag << endl;
			throw SMSException(out.str().data());
	}
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
		<< ", H: " << GetHalt();
	
	return out.str();
}

void Registers::SaveRegs(ostream *stream)
{
	stream->write((char *)&m_af.d, sizeof(u16));
	stream->write((char *)&m_bc.d, sizeof(u16));
	stream->write((char *)&m_de.d, sizeof(u16));
	stream->write((char *)&m_hl.d, sizeof(u16));
    stream->write((char *)&m_ix.d, sizeof(u16));
    stream->write((char *)&m_iy.d, sizeof(u16));
    stream->write((char *)&m_af2.d, sizeof(u16));
    stream->write((char *)&m_bc2.d, sizeof(u16));
    stream->write((char *)&m_de2.d, sizeof(u16));
    stream->write((char *)&m_hl2.d, sizeof(u16));
    stream->write((char *)&m_r, sizeof(u8));
    stream->write((char *)&m_i, sizeof(u8));
	stream->write((char *)&m_pc, sizeof(u16));
	stream->write((char *)&m_sp, sizeof(u16));
	stream->write((char *)&m_iff1, sizeof(bool));
	stream->write((char *)&m_iff2, sizeof(bool));
    stream->write((char *)&m_intMode, sizeof(u8));
	stream->write((char *)&m_halt, sizeof(bool));
    stream->write((char *)&m_intPending, sizeof(bool));
}

void Registers::LoadRegs(istream *stream)
{
	stream->read((char *)&m_af.d, sizeof(u16));
	stream->read((char *)&m_bc.d, sizeof(u16));
	stream->read((char *)&m_de.d, sizeof(u16));
	stream->read((char *)&m_hl.d, sizeof(u16));
    stream->read((char *)&m_ix.d, sizeof(u16));
	stream->read((char *)&m_iy.d, sizeof(u16));
    stream->read((char *)&m_af2.d, sizeof(u16));
    stream->read((char *)&m_bc2.d, sizeof(u16));
    stream->read((char *)&m_de2.d, sizeof(u16));
    stream->read((char *)&m_hl2.d, sizeof(u16));
    stream->read((char *)&m_r, sizeof(u8));
    stream->read((char *)&m_i, sizeof(u8));
	stream->read((char *)&m_pc, sizeof(u16));
	stream->read((char *)&m_sp, sizeof(u16));
	stream->read((char *)&m_iff1, sizeof(bool));
	stream->read((char *)&m_iff2, sizeof(bool));
    stream->read((char *)&m_intMode, sizeof(u8));
	stream->read((char *)&m_halt, sizeof(bool));
    stream->read((char *)&m_intPending, sizeof(bool));
}
