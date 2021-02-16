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
    m_romSize = size;
    m_buffer = cartridgeBuffer;
    m_mem = cartridgeBuffer;
    m_name = GetShortName(fileName);
    if ((cartridgeBuffer != NULL) && (size > 0))
        m_isLoaded = true;
    else
        LoadFile(fileName, batteriesPath);
	   
    Reset();
}

Cartridge::~Cartridge(void)
{
	if (m_buffer)
		delete [] m_buffer;
    
    m_buffer = NULL;
	m_mem = NULL;
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
    m_ram.enabled = false;
    m_ram.numBank = 0;
    m_ram.page = &m_ram.mem[0x0000];
}

void Cartridge::LoadFile(string fileName, string batteriesPath) {
    ifstream::pos_type size;
	ifstream file (fileName.c_str(), ios::in|ios::binary|ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		m_romSize = (unsigned long)size;
		m_buffer = new u8 [size];
        m_mem = m_buffer;
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

u8 *Cartridge::GetData()
{
	return m_mem;
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

u8 Cartridge::Read(u16 address) {
    return m_mem[address];
};

void Cartridge::Write(u16 address, u8 value) {
    
};

u8 Cartridge::GetNumBank(u8 bank) {
    return m_numBanks[bank & 0x03];
}

bool Cartridge::GetRAMEnabled() {
    return m_ram.enabled;
}

u8 Cartridge::GetRAMNumBank() {
    return m_ram.numBank;
}
