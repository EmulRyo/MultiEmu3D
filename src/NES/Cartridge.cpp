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
#include "Mappers/Mapper.h"

using namespace std;
using namespace Nes;

Cartridge::Cartridge(string fileName, string batteriesPath, u8 *cartridgeBuffer, unsigned long size)
{
    m_mapper = nullptr;
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

    if (m_mapper)
        delete m_mapper;
    
    m_buffer = NULL;
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

void Cartridge::ReadHeader() {
    bool iNESFormat = false;
    if (m_buffer[0] == 'N' && m_buffer[1] == 'E' && m_buffer[2] == 'S' && m_buffer[3] == 0x1A)
        iNESFormat = true;

    bool nes20Format = false;
    if (iNESFormat == true && (m_buffer[7] & 0x0C) == 0x08)
        nes20Format = true;

    u16 mapperId = (m_buffer[7] & 0xF0) | ((m_buffer[6] & 0xF0) >> 4);
    if (nes20Format)
        mapperId = ((m_buffer[8] & 0x0F) << 8) | mapperId;

    m_mapper = Mapper::Create(mapperId, m_buffer);
    assert((m_mapper != nullptr) && "Mapper not implemented");
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

NametableMirroring Cartridge::GetNametableMirroring() {
    return m_mapper->GetNametableMirroring();
}

void Cartridge::SaveState(ostream *stream)
{
    m_mapper->SaveState(stream);
}

void Cartridge::LoadState(istream *stream)
{
    m_mapper->LoadState(stream);
}

void Cartridge::Extract() {
    m_mapper->Extract();
}

u8 Cartridge::GetMapperNum() {
    return m_mapper->GetMapperNum();
}

const char* Cartridge::GetMapperName() {
    return m_mapper->GetMapperName();
}

u8 Cartridge::ReadPRG(u16 address) {
    return m_mapper->ReadPRG(address);
};

void Cartridge::WritePRG(u16 address, u8 value) {
    m_mapper->WritePRG(address, value);
};

u8 Cartridge::ReadCHR(u16 address) {
    return m_mapper->ReadCHR(address);
};

void Cartridge::WriteCHR(u16 address, u8 value) {
    m_mapper->WriteCHR(address, value);
};

u8 Cartridge::GetPRGBanks() {
    return m_mapper->GetPRGBanks();
}

u8 Cartridge::GetPRGBank0() {
    return m_mapper->GetPRGBank0();
}

u8 Cartridge::GetPRGBank1() {
    return m_mapper->GetPRGBank1();
}

u8 Cartridge::GetCHRBanks() {
    return m_mapper->GetCHRBanks();
}

u8 Cartridge::GetCHRBank0() {
    return m_mapper->GetCHRBank0();
}

u8 Cartridge::GetCHRBank1() {
    return m_mapper->GetCHRBank1();
}

