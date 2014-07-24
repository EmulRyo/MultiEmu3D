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

#include <iostream>
#include "Memory.h"
#include "ISMSScreenDrawable.h"
#include "Video.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MEMR(address) (m_mem->memory[(address)])

using namespace std;

Video::Video(ISMSScreenDrawable *screen)
{
    m_pixel = new VideoPixel();
	SetScreen(screen);
}

Video::~Video(void)
{
	delete m_pixel;
}

void Video::SetScreen(ISMSScreenDrawable *screen)
{
    m_screen = screen;
}

void Video::RefreshScreen()
{
    m_screen->OnRefreshGBScreen();
}

void Video::Reset() {
    m_numWrite = 0;
    m_line = 0;
    m_cycles = 0;
    m_cyclesLine = 0;
    m_vramAddress = true;
    m_regs[2] = 0x0E;
    m_regs[5] = 0x7E;
    
    memset(m_memory, 0, VDP_MEM);
    memset(m_palettes, 0, 32);
    memset(m_rgbPalettes, 0, 32*3);
}

void Video::ClearScreen()
{
    if (m_screen)
        m_screen->OnClear();
}

void Video::SetControl(u8 value) {
    m_numWrite++;
    
    if (m_numWrite == 2) {
        u8 place = value >> 6;
        
        switch (place) {
            case 0x00:
            case 0x01:
            {
                m_address = ((value & 0x3F) << 8) | m_partialAddress;
                //printf("VRAM address: 0x%X\n", address);
                m_vramAddress = true;
                break;
            }
            case 0x02:
            {
                u8 reg = value & 0x0F;
                //printf("VDP reg: %d = 0x%X\n", reg, m_firstValueAddress);
                m_regs[reg] = m_partialAddress;
                break;
            }
            case 0x03:
            {
                m_address = m_partialAddress & 0x1F;
                //printf("VDP palette: %d\n", address);
                m_vramAddress = false;
                break;
            }
                
            default:
                break;
        }
        
        m_numWrite = 0;
    }
    
    m_partialAddress = value;
}


void Video::SetData(u8 value) {
    m_numWrite = 0;
    
    if (m_vramAddress) {
        m_memory[m_address] = value;
        m_address++;
        m_address &= 0x3FFF;
    }
    else {
        m_palettes[m_address] = value;
        UpdatePalette(m_address);
        m_address++;
        m_address &= 0x3F;
    }
}

u8 Video::GetControl() {
    m_numWrite = 0;
    u8 status = m_status;
    m_status = 0;
    return status;
}

u8 Video::GetData() {
    m_numWrite = 0;
    if (m_vramAddress) {
        u8 value = m_memory[m_address];
        if (m_address < 0x3FFF)
            m_address++;
        
        return value;
    }
    else {
        u8 value = m_palettes[m_address];
        if (m_address < 31)
            m_address++;
        
        return value;
    }
}

u8 Video::GetLine() {
    return m_line;
}

void Video::Update(u8 cycles) {
    // 486 lineas visibles de 525 en NTSC. => 92.5714 % son visibles
    // 0,925714 * 59659 = 55227 ciclos en lineas visibles
    // 59659 - 55227 = 4432 ciclos en VBlank
    // 55227 / 224 = 247 ciclos cada linea
    
    m_cycles += cycles;

    if (m_cycles > 59659) {
        m_cycles -= 59659;
        m_cyclesLine = 0;
        m_line = 0;
    }
    
    m_cyclesLine += cycles;
    if (m_cyclesLine > 247) {
        m_cyclesLine -= 247;
        if (m_line < SMS_SCREEN_H)
            UpdateLine(m_line);
        else if (m_line == SMS_SCREEN_H) {
            RefreshScreen();
            // Bit 7 status flag
            m_status |= 0x80;
        }
        m_line++;
    }
}

void Video::UpdateLine(u8 line) {
	UpdateBG(line);
	UpdateSprites(line);
}

void Video::UpdateBG(u8 y) {
    int x, yScrolled;
    
    m_pixel->mapIni = 0x3800;
    
	yScrolled = (y + 0);
	
    m_pixel->y = y;
	m_pixel->yTile = yScrolled % 8;
	m_pixel->rowMap = ((yScrolled/8) * 32);
	
	for (x=0; x<SMS_SCREEN_W; x++)
	{
		m_pixel->x = x;
		m_pixel->xScrolled = (x + 0);
		if (m_pixel->xScrolled > 255)
			m_pixel->xScrolled -= 256;
        
		GetColor(m_pixel);
        
        m_screen->OnDrawPixel(m_pixel->r, m_pixel->g, m_pixel->b, x, y);
	}
}

void Video::UpdateSprites(u8 y) {
    u16 spriteBase = 0x3F00;
    u8  hSprite = 8;
    u8  tileData[4];
    
    u8 paletteOffset = 16;
    
	for (u8 numSprite = 0; numSprite<64; numSprite++) {
        u8 ySprite = m_memory[spriteBase + numSprite];
        if (ySprite == 0xD0)
            break;
        
        // La y se almacena como y+1 (nunca se podrá poner un sprite en y == 0)
        ySprite++;
        if ((ySprite > y-hSprite) && (ySprite <= y)) {
            u8 xSprite = m_memory[spriteBase + 128 + numSprite*2];
            u8 numTile = m_memory[spriteBase + 129 + numSprite*2];
            u16 addressTile = numTile * 32;;
            
            u8 yTile = y - ySprite;
            
            u8 xBeg = xSprite;
            u8 xTile = 0, countX = 0;
            u8 countY = yTile;
            
            for (u16 x=xBeg; ((x<xSprite+8) && (x<SMS_SCREEN_W)); x++) {
                xTile = countX;
                
                int addressLineTile = addressTile + (yTile * 4); //yTile * 4 porque cada linea de 1 tile ocupa 4 bytes
                
                tileData[0] = m_memory[addressLineTile + 0];
                tileData[1] = m_memory[addressLineTile + 1];
                tileData[2] = m_memory[addressLineTile + 2];
                tileData[3] = m_memory[addressLineTile + 3];
                
                int pixX = ABS(xTile - 7);
                
                //Un pixel lo componen 4 bits. Seleccionar la posicion del bit en los cuatro bytes (tileData)
                //Esto devolvera un numero de color que junto a la paleta de color nos dara el color requerido
                u8 indexColor  = (((tileData[3] & (0x01 << pixX)) >> pixX) << 3);
                   indexColor |= (((tileData[2] & (0x01 << pixX)) >> pixX) << 2);
                   indexColor |= (((tileData[1] & (0x01 << pixX)) >> pixX) << 1);
                   indexColor |=  ((tileData[0] & (0x01 << pixX)) >> pixX);
                
                indexColor += paletteOffset;
                
                u8 r = m_rgbPalettes[indexColor][0];
                u8 g = m_rgbPalettes[indexColor][1];
                u8 b = m_rgbPalettes[indexColor][2];
                
                m_screen->OnDrawPixel(r, g, b, xSprite + countX, ySprite + countY);
                
                countX++;
            }
        }
    }
}

inline void Video::GetColor(VideoPixel *p)
{
	int xTile, tileData[4], addressIdTile, addressTile, yTile, idMapTile, bgPriority;
	
    idMapTile = p->rowMap + p->xScrolled/8;
    //printf("[%d, %d]: %d\n", p->xScrolled, p->y, idMapTile);
    
	addressIdTile = p->mapIni + idMapTile*2;
    u8 data1 = m_memory[addressIdTile + 0];
    u8 data2 = m_memory[addressIdTile + 1];
    
    u16 tileNumber = (data2 & 0x01) << 8 | data1;
    //printf("[0x%X]: %d\n", addressIdTile, tileNumber);
	
	addressTile = tileNumber * 32;
    
    bgPriority = 0;
    xTile = p->xScrolled % 8;
    yTile = p->yTile;
    
    if (BIT1(data2))    // Flip x
        xTile = ABS(xTile - 7);
    if (BIT2(data2))    // Flip y
        yTile = ABS(yTile - 7);
    bgPriority = BIT4(data2);
    u8 paletteOffset = BIT3(data2) ? 16 : 0;
    
	
	int addressLineTile = addressTile + (yTile * 4); //yTile * 4 porque cada linea de 1 tile ocupa 4 bytes
	
	tileData[0] = m_memory[addressLineTile + 0];
	tileData[1] = m_memory[addressLineTile + 1];
    tileData[2] = m_memory[addressLineTile + 2];
    tileData[3] = m_memory[addressLineTile + 3];
    
	int pixX = ABS(xTile - 7);
    
	//Un pixel lo componen 4 bits. Seleccionar la posicion del bit en los cuatro bytes (tileData)
	//Esto devolvera un numero de color que junto a la paleta de color nos dara el color requerido
	p->indexColor  = (((tileData[3] & (0x01 << pixX)) >> pixX) << 3);
    p->indexColor |= (((tileData[2] & (0x01 << pixX)) >> pixX) << 2);
    p->indexColor |= (((tileData[1] & (0x01 << pixX)) >> pixX) << 1);
    p->indexColor |=  ((tileData[0] & (0x01 << pixX)) >> pixX);
    
    p->indexColor += paletteOffset;
    
    p->r = m_rgbPalettes[p->indexColor][0];
    p->g = m_rgbPalettes[p->indexColor][1];
    p->b = m_rgbPalettes[p->indexColor][2];
}

void Video::UpdatePalette(u8 numPalette) {
        u8 data = m_palettes[numPalette];
        
        m_rgbPalettes[numPalette][0] = data & 0x03;
        m_rgbPalettes[numPalette][1] = (data >> 2) & 0x03;
        m_rgbPalettes[numPalette][2] = (data >> 4) & 0x03;
        
        // Como el valor va de 0 a 3, hay que convertirlo de 0 a 255
        m_rgbPalettes[numPalette][0] *= 85;
        m_rgbPalettes[numPalette][1] *= 85;
        m_rgbPalettes[numPalette][2] *= 85;
}

void Video::GetTile(u8 *buffer, int widthSize, int tile)
{
    u8 tileData[4];
    
    int addressTile = tile * 32;
    
    for (int y=0; y<8; y++)
    {
        for (int x=0; x<8; x++)
        {
            int addressLineTile = addressTile + (y * 4); //yTile * 4 porque cada linea de 1 tile ocupa 4 bytes
            
            tileData[0] = m_memory[addressLineTile + 0];
            tileData[1] = m_memory[addressLineTile + 1];
            tileData[2] = m_memory[addressLineTile + 2];
            tileData[3] = m_memory[addressLineTile + 3];
            
            int pixX = ABS(x - 7);
            //Un pixel lo componen 4 bits. Seleccionar la posicion del bit en los cuatro bytes (tileData)
            //Esto devolvera un numero de color que junto a la paleta de color nos dara el color requerido
            int indexColor  = (((tileData[3] & (0x01 << pixX)) >> pixX) << 3);
                indexColor |= (((tileData[2] & (0x01 << pixX)) >> pixX) << 2);
                indexColor |= (((tileData[1] & (0x01 << pixX)) >> pixX) << 1);
                indexColor |=  ((tileData[0] & (0x01 << pixX)) >> pixX);
            
            int offset = widthSize*y + x*3;
            
            buffer[offset + 0] = m_rgbPalettes[indexColor][0];
            buffer[offset + 1] = m_rgbPalettes[indexColor][1];
            buffer[offset + 2] = m_rgbPalettes[indexColor][2];
        }
    }
}

bool Video::Interrupt() {
    if (BIT7(m_status) && BIT5(m_regs[1])) {
        m_status &= 0x7F;
        return true;
    } else
        return false;
}

u8 Video::MemR(u16 address) {
    return m_memory[address];
}

u8 Video::RegR(u8 reg) {
    return m_regs[reg];
}

u8 Video::PalR(u8 pal) {
    return m_palettes[pal];
}
