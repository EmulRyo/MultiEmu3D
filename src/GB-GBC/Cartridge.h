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
#include "../Common/Types.h"

#define CART_NAME		0x0134
#define CART_COLOR		0x0143
#define CART_TYPE		0x0147
#define CART_ROM_SIZE	0x0148
#define CART_RAM_SIZE	0x0149

namespace GameBoy {
    
    class Cartridge
    {
    private:
        unsigned long m_romSize;
        std::string m_name;
        bool m_isLoaded;
        bool m_hasRTC;
        u8 *m_memCartridge;

        u8 (*ptrRead)(u16);
        void (*ptrWrite)(u16, u8);
        void CheckCartridge(std::string batteriesPath="");
        int  CheckRomSize(int numHeaderSize, int fileSize);
        
    public:
        Cartridge(std::string fileName, std::string batteriesPath="");
        Cartridge(u8 *cartridgeBuffer, unsigned long size, std::string batteriesPath="");
        ~Cartridge();
        
        u8 *GetData();
        unsigned int GetSize();
        std::string GetGoodName(const char *name);
        std::string GetName();
        bool IsLoaded();

        inline u8 Read(u16 direction) { return ptrRead(direction); };
        inline void Write(u16 direction, u8 value) { ptrWrite(direction, value); };
        
        void SaveStateMBC(std::ostream *stream);
        void LoadStateMBC(std::istream *stream);
        
        void Extract();
    };
}

#endif
