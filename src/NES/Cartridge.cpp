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

#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include <iomanip>
#include <assert.h>
#include "Cartridge.h"
#include "../Common/Bit.h"

using namespace std;
using namespace Nes;

Cartridge::Cartridge(string fileName, string batteriesPath, u8 *cartridgeBuffer, unsigned long size)
{
    m_prgBanks = 0;
    m_romSize = size;
    m_buffer = cartridgeBuffer;
    m_name = GetShortName(fileName);
    if ((cartridgeBuffer != NULL) && (size > 0))
        m_isLoaded = true;
    else
        LoadFile(fileName, batteriesPath);
	
    ReadHeader();
    Reset();
}

Cartridge::~Cartridge(void)
{
	if (m_buffer)
		delete [] m_buffer;
    
    m_buffer = NULL;
	m_prgData = NULL;
    m_chrData = NULL;
}

u32 Cartridge::RoundUpPowerOf2(u32 v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    return v++;
}

string Cartridge::GetShortName(string fileName) {
    size_t begin = fileName.find_last_of('/');
    if (begin == string::npos)
        begin = fileName.find_last_of('\\');
    
    size_t end = fileName.find_last_of('.');
    
    if ((begin != string::npos) && (end != string::npos))
        return fileName.substr(begin+1, end-begin-1);
    else
        return fileName;
}

void Cartridge::Reset()
{
    
}

void Cartridge::LoadFile(string fileName, string batteriesPath) {
    ifstream::pos_type size;
	ifstream file (fileName.c_str(), ios::in|ios::binary|ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		m_romSize = (unsigned long)size;
		m_buffer = new u8 [size];
		file.seekg (0, ios::beg);
		file.read((char *)m_buffer, (streamsize)size);
		file.close();
        
		cout << fileName << ":\nFile loaded in memory correctly" << endl;
		m_isLoaded = true;
	}
	else
	{
		cerr << fileName << ": Error trying to open the file" << endl;
		m_isLoaded = false;
	}
}

#define SIZE_PRG_ROM 4
#define SIZE_CHR_ROM 5

void Cartridge::ReadHeader() {
    bool iNESFormat = false;
    if (m_buffer[0] == 'N' && m_buffer[1] == 'E' && m_buffer[2] == 'S' && m_buffer[3] == 0x1A)
        iNESFormat = true;

    bool nes20Format = false;
    if (iNESFormat == true && (m_buffer[7] & 0x0C) == 0x08)
        nes20Format = true;

    m_prgBanks = m_buffer[SIZE_PRG_ROM];
    m_chrBanks = m_buffer[SIZE_CHR_ROM];
    size_t sizePrgRom = m_prgBanks * 16384ULL;
    size_t sizeChrRom = m_chrBanks * 8192ULL;

    u8   mirroring = m_buffer[6] & 0x01;
    bool battery = ((m_buffer[6] & 0x02) > 0) ? true : false;
    u8   trainer = ((m_buffer[6] & 0x04) > 0) ? 1 : 0;
    bool fourScreenMode = ((m_buffer[6] & 0x08) > 0) ? true : false;

    bool VSUnisystem = ((m_buffer[7] & 0x01) > 0) ? true : false;
    bool playChoice10 = ((m_buffer[7] & 0x02) > 0) ? true : false;
    u16 mapper = (m_buffer[7] & 0xF0) | ((m_buffer[6] & 0xF0) >> 4);
    if (nes20Format)
        mapper = ((m_buffer[8] & 0x0F) << 8) | mapper;

    size_t headerSize = 16;
    size_t trainerSize = 512;
    m_prgData = m_buffer + headerSize + trainer * trainerSize;
    m_chrData = m_prgData + sizePrgRom;
}

u8 *Cartridge::GetData()
{
	return nullptr;
}

unsigned int Cartridge::GetSize()
{
	return m_romSize;
}

string Cartridge::GetName()
{
	return m_name;
}

bool Cartridge::IsLoaded()
{
	return m_isLoaded;
}

void Cartridge::SaveStateMBC(ostream *stream)
{
	
}

void Cartridge::LoadStateMBC(istream *stream)
{
	
}

void Cartridge::Extract() {
    
}

u8 Cartridge::ReadPRG(u16 address) {
    if (address < 0x8000)
        return m_prgData[address-0x8000];
    else if (address < 0xC000)
        return m_prgData[address - 0x8000];
    else {
        if (m_prgBanks == 2)
            return m_prgData[address - 0x8000];
        else
            return m_prgData[address - 0xC000];
    }
};

void Cartridge::WritePRG(u16 address, u8 value) {
    
};

u8 Cartridge::ReadCHR(u16 address) {
    return m_chrData[address];
};

void Cartridge::WriteCHR(u16 address, u8 value) {

};
