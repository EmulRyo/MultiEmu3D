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
using namespace Nes;

Registers::Registers() {ResetRegs();}

void Registers::ResetRegs()
{
	SetA(0x00);
	SetX(0x00);
	SetY(0x00);
	SetS(0xFD);
	SetP(0x34);
	SetPC(0x0000);
}

Registers::~Registers()
{
}

Registers *Registers::GetPtrRegisters() {return this;}

string Registers::ToString()
{
	stringstream out;
	
	out << "PC: " << setfill('0') << setw(4) << uppercase << hex << (int)GetPC()
		<< ", A: " << setfill('0') << setw(2) << uppercase << hex << (int)GetA()
		<< ", X: " << setfill('0') << setw(2) << uppercase << hex << (int)GetX()
		<< ", Y: " << setfill('0') << setw(2) << uppercase << hex << (int)GetY()
		<< ", S: " << setfill('0') << setw(4) << uppercase << hex << (int)GetS()
		<< ", P: " << setfill('0') << setw(4) << uppercase << hex << (int)GetP();
	
	return out.str();
}

void Registers::SaveRegs(ostream *stream)
{
	stream->write((char *)&m_A, sizeof(u8));
	stream->write((char *)&m_X, sizeof(u8));
	stream->write((char *)&m_Y, sizeof(u8));
	stream->write((char *)&m_S, sizeof(u8));
    stream->write((char *)&m_P, sizeof(u8));
    stream->write((char *)&m_PC, sizeof(u16));
}

void Registers::LoadRegs(istream *stream)
{
	stream->read((char *)&m_A, sizeof(u8));
	stream->read((char *)&m_X, sizeof(u8));
	stream->read((char *)&m_Y, sizeof(u8));
	stream->read((char *)&m_S, sizeof(u8));
    stream->read((char *)&m_P, sizeof(u8));
	stream->read((char *)&m_PC, sizeof(u16));
}
