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

#ifndef __CARTRIDGE_H__
#define __CARTRIDGE_H__

#include <string>
#include "Def.h"


class Cartridge
{
private:
	unsigned long m_romSize;
	std::string m_name;
	bool m_isLoaded;
	BYTE *m_memCartridge;

	BYTE (*ptrRead)(WORD);
	void (*ptrWrite)(WORD, BYTE);
	void CheckCartridge(std::string batteriesPath="");
	int  CheckRomSize(int numHeaderSize, int fileSize);
    
public:
	Cartridge(std::string fileName, std::string batteriesPath="");
	Cartridge(BYTE *cartridgeBuffer, unsigned long size, std::string batteriesPath="");
	~Cartridge();
	
	BYTE *GetData();
	unsigned int GetSize();
    std::string GetGoodName(const char *name);
	std::string GetName();
	bool IsLoaded();

	inline BYTE Read(WORD direction) { return ptrRead(direction); };
	inline void Write(WORD direction, BYTE value) { ptrWrite(direction, value); };
	
	void SaveStateMBC(std::ofstream *file);
	void LoadStateMBC(std::ifstream *file);
    
    void Extract();
};

#endif
