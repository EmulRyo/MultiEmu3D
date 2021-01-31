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

#ifndef __MBC_H__
#define __MBC_H__

#include <string>
#include "../Common/Types.h"

void InitMBCNone(std::string nameROM, u8 * memCartridge, int romSize);
void InitMBC1(std::string, u8 * memCartridge, int romSize, int ramHeaderSize);
void InitMBC2(std::string, u8 * memCartridge, int romSize);
void InitMBC3(std::string, u8 * memCartridge, int romSize, int ramHeaderSize, bool hasRTC);
void InitMBC5(std::string, u8 * memCartridge, int romSize, int ramHeaderSize);
void DestroyMBC();

u8 NoneRead(u16 address);
void NoneWrite(u16 address, u8 value);

u8 MBC1Read(u16 address);
void MBC1Write(u16 address, u8 value);

u8 MBC2Read(u16 address);
void MBC2Write(u16 address, u8 value);

u8 MBC3Read(u16 address);
void MBC3Write(u16 address, u8 value);

u8 MBC5Read(u16 address);
void MBC5Write(u16 address, u8 value);

void MBCPathBatteries(std::string path);

void MBCExtract();

void MBCSaveState(std::ostream *stream);
void MBCLoadState(std::istream *stream);

#endif
