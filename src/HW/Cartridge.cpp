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

/*
 * Constructor que recibe un fichero, lo carga en memoria y lo procesa
 */
Cartridge::Cartridge(string fileName, string batteriesPath)
{
    m_buffer = NULL;
	m_mem = NULL;
	LoadFile(fileName, batteriesPath);
}

/*
 * Constructor que recibe un buffer y su tamaño y lo procesa
 */
Cartridge::Cartridge(u8 *cartridgeBuffer, unsigned long size, string batteriesPath)
{
	m_romSize = size;
    m_buffer = cartridgeBuffer;
	m_mem = cartridgeBuffer;
	
	Init(batteriesPath);
	
	m_isLoaded = true;
}

Cartridge::~Cartridge(void)
{
	if (m_buffer)
		delete [] m_buffer;
    
    m_buffer = NULL;
	m_mem = NULL;
}

void Cartridge::Init(string batteriesPath)
{
    m_name = string("");
	
	//CheckRomSize((int)m_memCartridge[CART_ROM_SIZE], m_romSize);
    if ((m_romSize % 32768) == 0x200) {
        m_mem += 0x200;
        m_maskPages = ((m_romSize-0x200)-1) >> 14;
    }
    else
        m_maskPages = (m_romSize-1) >> 14;
    
    m_pages[0] = &m_mem[0x0000];
    m_pages[1] = &m_mem[0x4000];
    m_pages[2] = &m_mem[0x8000];
    
    m_ram.enabled = false;
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
		
		Init(batteriesPath);
		
		m_isLoaded = true;
	}
	else
	{
		cerr << fileName << ": Error trying to open the file" << endl;
		m_isLoaded = false;
	}
}

string Trim(const string &str) {
    size_t endpos = str.find_last_not_of(" ");
    if( string::npos != endpos )
        return str.substr( 0, endpos+1 );
    else
        return str;
}

string Cartridge::GetGoodName(const char *name) {
    string allow = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz ._-'!";
    
    int numChars = 16;
    string tmp = string(name, numChars);
    
    size_t found = 0;
    while (found != string::npos) {
        found = tmp.find_first_not_of(allow);
        if (found != string::npos)
            tmp[found] = ' ';
    }
    
    return Trim(tmp);
}

/*
 * Compara el tamaño de la rom con el valor de la cabecera
 */
int Cartridge::CheckRomSize(int numHeaderSize, int fileSize)
{
	int headerSize = 32768 << (numHeaderSize & 0x0F);
	if (numHeaderSize & 0xF0)
		headerSize += (32768 << ((numHeaderSize & 0xF0) >> 0x04));
	assert(headerSize == fileSize);
	if (headerSize != fileSize)
	{
		cout << "The header does not match with the file size" << endl;
		return 0;
	}
	else
		return 1;
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

void Cartridge::SaveStateMBC(ofstream * file)
{
	
}

void Cartridge::LoadStateMBC(ifstream * file)
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
            m_ram.page = BIT2(value) ? &m_ram.mem[0x4000] : &m_ram.mem[0];
            break;
        case 0xFFFD: m_pages[0] = &m_mem[(value&m_maskPages)*0x4000]; break;
        case 0xFFFE: m_pages[1] = &m_mem[(value&m_maskPages)*0x4000]; break;
        case 0xFFFF: m_pages[2] = &m_mem[(value&m_maskPages)*0x4000]; break;
            
        default:
            if (m_ram.enabled && (address >= 0x8000) && ((address < 0xC000)))
                    m_ram.page[address-0x8000] = value;
            break;
    }
};
