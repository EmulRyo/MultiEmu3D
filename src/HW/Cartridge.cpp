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

#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include <iomanip>
#include <assert.h>
#include "Cartridge.h"
#include "Def.h"
#include "SMSException.h"

using namespace std;

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

u32 RoundUpPowerOf2(u32 v) {
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
    if ((m_romSize % 32768) == 0x200) {
        m_mem = m_buffer + 0x200;
        m_maskPages = (RoundUpPowerOf2(m_romSize-0x200)-1) >> 14;
    }
    else
        m_maskPages = (RoundUpPowerOf2(m_romSize)-1) >> 14;;
    
    m_numBanks[0] = 0;
    m_numBanks[1] = 1;
    m_numBanks[2] = 2;
    m_pages[0] = &m_mem[0x0000];
    m_pages[1] = &m_mem[0x4000];
    m_pages[2] = &m_mem[0x8000];
    
    m_ram.enabled = false;
    m_ram.numBank = 0;
    m_ram.page = &m_ram.mem[0x0000];
    
    // Estas lineas permiten arrancar la BIOS de Alex Kidd
    //m_mem[0x17EDE] = 0x00;
    //m_mem[0x17EDF] = 0x00;
    //m_mem[0x17EE0] = 0x00;
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
    if (address < 0x400)
        return m_mem[address];
    else if (address < 0x4000)
        return m_pages[0][address];
    else if (address < 0x8000)
        return m_pages[1][address-0x4000];
    else {
        if (m_ram.enabled)
            return m_ram.page[address-0x8000];
        else
            return m_pages[2][address-0x8000];
    }
};

void Cartridge::Write(u16 address, u8 value) {
    switch (address) {
        case 0xFFFC:
            m_ram.enabled = BIT3(value) != 0;
            m_ram.numBank = BIT2(value) ? 1 : 0;
            m_ram.page = BIT2(value) ? &m_ram.mem[0x4000] : &m_ram.mem[0];
            break;
        case 0xFFFD:
            m_numBanks[0] = value&m_maskPages;
            m_pages[0] = &m_mem[m_numBanks[0]*0x4000];
            break;
        case 0xFFFE:
            m_numBanks[1] = value&m_maskPages;
            m_pages[1] = &m_mem[m_numBanks[1]*0x4000];
            break;
        case 0xFFFF:
            m_numBanks[2] = value&m_maskPages;
            m_pages[2] = &m_mem[m_numBanks[2]*0x4000];
            break;
            
        default:
            if (m_ram.enabled && (address >= 0x8000) && ((address < 0xC000)))
                    m_ram.page[address-0x8000] = value;
            break;
    }
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
