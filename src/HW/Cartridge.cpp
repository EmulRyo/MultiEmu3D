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
	m_memCartridge = NULL;
    m_offset = 0;
	ifstream::pos_type size;
	ifstream file (fileName.c_str(), ios::in|ios::binary|ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		m_romSize = (unsigned long)size;
		m_memCartridge = new u8 [size];
		file.seekg (0, ios::beg);
		file.read((char *)m_memCartridge, (streamsize)size);
		file.close();

		cout << fileName << ":\nFile loaded in memory correctly" << endl;
		
		CheckCartridge(batteriesPath);
		
		m_isLoaded = true;
        
	}
	else
	{
		cerr << fileName << ": Error trying to open the file" << endl;
		m_isLoaded = false;
	}
}

/*
 * Constructor que recibe un buffer y su tamaño y lo procesa
 */
Cartridge::Cartridge(u8 *cartridgeBuffer, unsigned long size, string batteriesPath)
{
    m_offset = 0;
	m_romSize = size;
	m_memCartridge = cartridgeBuffer;
	
	CheckCartridge(batteriesPath);
	
	m_isLoaded = true;
}

Cartridge::~Cartridge(void)
{
	if (m_memCartridge)
		delete [] m_memCartridge;
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
 * Comprueba el buffer de la rom, extrae el nombre, compara el tamaño e inicializa el MBC
 */
void Cartridge::CheckCartridge(string batteriesPath)
{
	m_name = string("");
	
	//CheckRomSize((int)m_memCartridge[CART_ROM_SIZE], m_romSize);
    if (m_romSize == 33280)
        m_offset = 0x200;
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
	return m_memCartridge;
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
    return m_memCartridge[address+m_offset];
};

void Cartridge::Write(u16 address, u8 value) {
    switch (address) {
        case 0xFFFC:
            printf("Cartridge RAM\n");
            break;
        case 0xFFFD:
            printf("Page 0\n");
            break;
        case 0xFFFE:
            printf("Page 1\n");
            break;
        case 0xFFFF:
            printf("Page 2\n");
            break;
            
        default:
            break;
    }
};
