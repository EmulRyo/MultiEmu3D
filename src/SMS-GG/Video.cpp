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

#include <iostream>
#include "Memory.h"
#include "Video.h"
#include "../Common/Bit.h"
#include "../Common/IScreenDrawable.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MEMR(address) (m_mem->memory[(address)])

#define NAMEBASE        2
#define SPRITEBASE      5
#define TILEBASE        6
#define BACKDROPCOLOR   7
#define XSCROLL         8
#define YSCROLL         9

using namespace std;
using namespace MasterSystem;

const t_VDPMODES SMS2Modes[16] = {
    GRAPHIC_1,
    TEXT,
    GRAPHIC_2,
    MODE_12,
    MULTICOLOR,
    MODE_13,
    MODE_23,
    MODE_123,
    MODE_4_192,
    INVALID_TEXT,
    MODE_4_192,
    MODE_4_224,
    MODE_4_192,
    INVALID_TEXT,
    MODE_4_240,
    MODE_4_192
};

Video::Video(IScreenDrawable *screen)
{
    m_GameGear = false;
    m_pixel = new VideoPixel();
    Reset();
	SetScreen(screen);
}

Video::~Video(void)
{
	delete m_pixel;
}

void Video::SetScreen(IScreenDrawable *screen)
{
    m_screen = screen;
}

void Video::RefreshScreen()
{
    m_screen->OnRefreshEmulatedScreen();
}

void Video::Reset() {
    m_numWrite = 0;
    m_line = 0;
    m_cycles = 0;
    m_cyclesLine = 0;
    m_address = 0;
    m_vramAddress = true;
    m_status = 0x1F;
    m_readBuffer = 0;
    m_regs[ 0] = 0x36;
    m_regs[ 1] = 0x80;
    m_regs[ 2] = 0xFF;
    m_regs[ 3] = 0xFF;
    m_regs[ 4] = 0xFF;
    m_regs[ 5] = 0xFF;
    m_regs[ 6] = 0xFB;
    m_regs[ 7] = 0x00;
    m_regs[ 8] = 0x00;
    m_regs[ 9] = 0x00;
    m_regs[10] = 0xFF;
    
    m_lineIrqCounter = 0xFF;
    
    m_latch = 0;
    
    memset(m_memory, 0, VDP_MEM);
    memset(m_palettes, 0, 32);
    memset(m_rgbPalettes, 0, 32*3);
}

void Video::ClearScreen()
{
    if (m_screen)
        m_screen->OnClear();
}

void Video::SetGGMode(bool value) {
    m_GameGear = value;
    if (value)
        m_screen->OnSizeChanged(48, 24, 160, 144);
    else
        m_screen->OnSizeChanged(0, 0, 256, 192);
}

void Video::SetControl(u8 value) {
    m_numWrite++;
    
    if (m_numWrite == 2) {
        u8 place = value >> 6;
        m_address = ((value & 0x3F) << 8) | m_partialAddress;
        
        if (place == 0) {
            m_readBuffer = m_memory[m_address];
            m_address++;
        }
        else if (place == 2) {
            u8 reg = value & 0x0F;
            m_regs[reg] = m_partialAddress;
            CheckReg(reg);
        }
        
        m_vramAddress = (place != 3);
        
        m_numWrite = 0;
    }
    else
        m_partialAddress = value;
}


void Video::SetData(u8 value) {
    m_numWrite = 0;
    
    if (m_vramAddress)
        m_memory[m_address] = value;
    else {
        if (m_GameGear) {
            u8 numPalette = m_address & 0x3F;
            if ((numPalette % 2) == 0)
                m_latch = value;
            else {
                m_palettes[numPalette] = value;
                m_palettes[numPalette-1] = m_latch;
                UpdatePaletteGG(numPalette);
            }
        }
        else {
            u8 numPalette = m_address & 0x1F;
            m_palettes[numPalette] = value;
            UpdatePalette(numPalette);
        }
    }
    
    m_address++;
    m_address &= 0x3FFF;
    
    m_readBuffer = value;
}

u8 Video::GetControl() {
    m_numWrite = 0;
    m_pendingLIRQ = false;
    m_pendingVIRQ = false;
    u8 status = m_status;
    m_status &= 0x1F;
    return status;
}

u8 Video::GetData() {
    m_numWrite = 0;
    
    u8 value = m_readBuffer;
    m_readBuffer = m_memory[m_address];
    m_address = (m_address + 1) & 0x3FFF;
    
    return value;
}

u8 Video::GetV() {
    switch (m_mode) {
        case MODE_4_192:
            if (m_line < 0xDB)
                return m_line;
            else
                return m_line - 6;
            break;
            
        default:
            return m_line;
    }
}

u8 Video::GetH() {
    return m_cyclesLine / 228.0f * 0xFF;
}

u16 Video::GetAddress() {
    return m_address;
}

void Video::Update(u8 cycles) {
    m_cycles += cycles;
    
    if (m_cycles > SMS_FRAME_CYCLES) {
        m_cycles -= SMS_FRAME_CYCLES;
        m_cyclesLine = 0;
        m_line = 0;
        m_lineIrqCounter = m_regs[10];
    }
    
    m_cyclesLine += cycles;
    if (m_cyclesLine > 228) {
        m_cyclesLine -= 228;
        u8 h = (m_mode == MODE_4_240) ? 240 : (m_mode == MODE_4_224) ? 244 : 192;
        if (m_line < h) {
            if ((!m_GameGear) || ((m_GameGear) && (m_line >= 24) && (m_line < 168)))
                UpdateLine(m_line);
        }
        else if (m_line == (h+1)) {
            RefreshScreen();
            
            // Bit 7 status flag
            m_status |= 0x80;
            m_pendingVIRQ = true;
        }
        
        if (m_line <= h)
            m_lineIrqCounter--;
        
        if (m_lineIrqCounter < 0) {
            m_pendingLIRQ = true;
            m_lineIrqCounter = m_regs[10];
        }
        m_line++;
    }
}

void Video::UpdateLine(u8 line) {
    // Si display encendido
    if (BIT6(m_regs[1])) {
        UpdateBG(line);
        UpdateSprites(line);
        if (!m_GameGear && (BIT5(m_regs[0]))) {
            // Pintar columna de 8 pixeles de un color sólido
            u8 indexColor = (m_regs[BACKDROPCOLOR] & 0x0F) + 16;
            u8 r = m_rgbPalettes[indexColor][0];
            u8 g = m_rgbPalettes[indexColor][1];
            u8 b = m_rgbPalettes[indexColor][2];
            for (int x=0; x<8; x++)
                m_screen->OnDrawPixel(r, g, b, x, line);
        }
    }
    else {
        for (int x=0; x<SMS_SCREEN_W; x++)
            m_screen->OnDrawPixel(0, 0, 0, x, line);
    }
}

void Video::UpdateBG(u8 y) {
    int x, yScrolled, scrollX, yTile2, rowMap2;
    
    m_pixel->mapIni = (m_regs[NAMEBASE] & 0x0E) << 10;
    
    yScrolled = (y + m_regs[YSCROLL]) % 224;
	
    m_pixel->y = y;
	m_pixel->yTile = yScrolled % 8;
	m_pixel->rowMap = ((yScrolled/8) * 32);
    
    // Preparar valores para deshabilitar scroll vertical
    if (BIT7(m_regs[0])) {
        yTile2 = y % 8;
        rowMap2 = (y/8) * 32;
    }
    
    if ((y < 16) && (BIT6(m_regs[0])))
        scrollX = 0;
    else
        scrollX = 256-m_regs[XSCROLL];
    
    int xBeg = m_GameGear ? 48 : 0;
    int xEnd = m_GameGear ? 209 : 256;
	for (x=xBeg; x<xEnd; x++) {
        m_pixel->x = x;
        m_pixel->xScrolled = (x + scrollX);
        if (m_pixel->xScrolled > 255)
            m_pixel->xScrolled -= 256;
        
        // Si bit activado deshabilitar scroll vertical en las columnas de la derecha
        if (BIT7(m_regs[0]) && (x>191)) {
            m_pixel->yTile = yTile2;
            m_pixel->rowMap = rowMap2;
        }
        
        GetColor(m_pixel);
        
        m_screen->OnDrawPixel(m_pixel->r, m_pixel->g, m_pixel->b, x, y);
	}
}

void Video::UpdateSprites(u8 y) {
    u16 spriteBase = (m_regs[SPRITEBASE] & 0x7E) << 7;
    u8  mode16 = BIT1(m_regs[1]);
    u8  hSprite = mode16 ? 16 : 8;
    u8  tileData[4];
    u8  sprites[64];
    u8  paletteOffset = 16;
    
    u8  spritesInLine = GetSprites(y, spriteBase, hSprite, &sprites[0]);

    for (int i=spritesInLine-1; i>=0; i--) {
        u8 numSprite = sprites[i];
        
        s16 ySprite = m_memory[spriteBase + numSprite];
        if (ySprite > (0xFF - hSprite))
            ySprite -= 0x100;
        
        // La y se almacena como y+1 (para poner un sprite en y == 0, la y original deber valer 0xFF)
        ySprite++;

        u8 xSprite = m_memory[spriteBase + 128 + numSprite*2];
        u8 numTile = m_memory[spriteBase + 129 + numSprite*2];
        if (mode16)
            numTile &= 0xFE;
        u16 addressTile = numTile * 32 + ((m_regs[TILEBASE]&0x04) ? 0x2000 : 0x0000);
        
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
            
            s16 x1 = xSprite + countX;
            s16 y1 = ySprite + countY;
            
            if (BIT3(m_regs[0]))
                x1 -= 8;
            
            if ((indexColor != 0) && (m_pixelAux[x1][y1] >= 0) && (x1 >= 0)){
                indexColor += paletteOffset;
                
                u8 r = m_rgbPalettes[indexColor][0];
                u8 g = m_rgbPalettes[indexColor][1];
                u8 b = m_rgbPalettes[indexColor][2];
                
                m_screen->OnDrawPixel(r, g, b, x1, y1);
                
                m_pixelAux[x1][y1]++;
                if (m_pixelAux[x1][y1] > 1)
                    m_status |= 0x20;
            }
            
            countX++;
        }
    }
}

u8 Video::GetSprites(u8 y, u16 spriteBase, u8 hSprite, u8 *sprites) {
    const u8 maxSprites = 64;
    u8 spritesInLine = 0;
    
    for (u8 numSprite = 0; numSprite<64; numSprite++) {
        s16 ySprite = m_memory[spriteBase + numSprite];
        if ((ySprite == 0xD0) && (m_mode == MODE_4_192))
            break;
        else if (ySprite > (0xFF - hSprite))
            ySprite -= 0x100;
        
        // La y se almacena como y+1 (para poner un sprite en y == 0, la y original deber valer 0xFF)
        ySprite++;
        if ((ySprite > y-hSprite) && (ySprite <= y)) {
            if (spritesInLine < maxSprites)
                sprites[spritesInLine] = numSprite;
            
            spritesInLine++;
        }
    }
    
    if (spritesInLine > maxSprites) {
        m_status |= 0x40;
        spritesInLine = maxSprites;
    }
    
    return spritesInLine;
}

inline void Video::GetColor(VideoPixel *p)
{
	int xTile, addressIdTile, yTile, idMapTile;
    u16 addressTile;
    u8 bgPriority, tileData[4];
	
    idMapTile = p->rowMap + p->xScrolled/8;
    
	addressIdTile = p->mapIni + idMapTile*2;
    u8 data1 = m_memory[addressIdTile + 0];
    u8 data2 = m_memory[addressIdTile + 1];
    
    u16 tileNumber = ((data2 & 0x01) << 8) | data1;
	
	addressTile = tileNumber * 32;
    
    xTile = p->xScrolled % 8;
    yTile = p->yTile;
    
    if (BIT1(data2))    // Flip x
        xTile = ABS(xTile - 7);
    if (BIT2(data2))    // Flip y
        yTile = ABS(yTile - 7);
    bgPriority = BIT4(data2);
    u8 paletteOffset = BIT3(data2) ? 16 : 0;
	
	u16 addressLineTile = addressTile + (yTile * 4); //yTile * 4 porque cada linea de 1 tile ocupa 4 bytes
	
	tileData[0] = m_memory[addressLineTile + 0];
	tileData[1] = m_memory[addressLineTile + 1];
    tileData[2] = m_memory[addressLineTile + 2];
    tileData[3] = m_memory[addressLineTile + 3];
    
	u8 pixX = ABS(xTile - 7);
    u8 mask = 0x01 << pixX;
    
	//Un pixel lo componen 4 bits. Seleccionar la posicion del bit en los cuatro bytes (tileData)
	//Esto devolvera un numero de color que junto a la paleta de color nos dara el color requerido
	p->indexColor  = (((tileData[3] & mask) >> pixX) << 3);
    p->indexColor |= (((tileData[2] & mask) >> pixX) << 2);
    p->indexColor |= (((tileData[1] & mask) >> pixX) << 1);
    p->indexColor |=  ((tileData[0] & mask) >> pixX);
    
    p->indexColor += paletteOffset;
    
    m_pixelAux[p->x][p->y] = (bgPriority  && p->indexColor) ? -1 : 0;
    
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

void Video::UpdatePaletteGG(u8 numPalette) {
    u8 data1 = m_palettes[numPalette-1];
    u8 data2 = m_palettes[numPalette];
    
    numPalette = numPalette / 2;
    m_rgbPalettes[numPalette][0] = data1 & 0x0F;
    m_rgbPalettes[numPalette][1] = data1 >> 4;
    m_rgbPalettes[numPalette][2] = data2 & 0x0F;
    
    // Como el valor va de 0 a 15, hay que convertirlo de 0 a 255
    m_rgbPalettes[numPalette][0] *= 17;
    m_rgbPalettes[numPalette][1] *= 17;
    m_rgbPalettes[numPalette][2] *= 17;
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
    if (m_pendingVIRQ && GetIE0()) {
        m_pendingVIRQ = false;
        return true;
    } else if (m_pendingLIRQ && GetIE1()) {
        m_pendingLIRQ = false;
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

void Video::CheckReg(u8 reg) {
    switch (reg) {
        case 0x0:
        case 0x1:
            u8 m1 = BIT4(m_regs[1]) >> 4;
            u8 m2 = BIT1(m_regs[0]);
            u8 m3 = BIT3(m_regs[1]) >> 1;
            u8 m4 = BIT2(m_regs[0]) << 1;
            
            u8 numMode = m4 | m3 | m2 | m1;
            m_mode = SMS2Modes[numMode];
            
            if ((m_mode != MODE_4_192) && (m_mode != MODE_4_224) && (m_mode != MODE_4_240))
                printf("VDP mode not emulated\n");
            
            if (m_mode == MODE_4_224)
                printf("MODE 4 (224-line display)\n");
            else if (m_mode == MODE_4_240)
                printf("MODE 4 (240-line display)\n");
            
            break;
    }
}

u16 Video::GetLine() {
    return m_line;
}

u8 Video::GetStatus() {
    return m_status;
}

u8 Video::GetCyclesLine() {
    return m_cyclesLine;
}

bool Video::GetIE0() {
    return (BIT5(m_regs[1]) != 0);
}

bool Video::GetIE1() {
    return (BIT4(m_regs[0]) != 0);
}

void Video::SaveState(ostream *stream) {
    stream->write((char *)&m_regs[0],             sizeof(u8)*16);
    stream->write((char *)&m_palettes[0],         sizeof(u8)*64);
    stream->write((char *)&m_rgbPalettes[0][0],   sizeof(u8)*32*3);
    stream->write((char *)&m_address,             sizeof(u16));
    stream->write((char *)&m_numWrite,            sizeof(u8));
    stream->write((char *)&m_partialAddress,      sizeof(u8));
    stream->write((char *)&m_line,                sizeof(u16));
    stream->write((char *)&m_cycles,              sizeof(u32));
    stream->write((char *)&m_cyclesLine,          sizeof(u16));
    stream->write((char *)&m_status,              sizeof(u8));
    stream->write((char *)&m_lineIrqCounter,      sizeof(s16));
    stream->write((char *)&m_vramAddress,         sizeof(bool));
    stream->write((char *)&m_readBuffer,          sizeof(u8));
    stream->write((char *)&m_pendingVIRQ,         sizeof(bool));
    stream->write((char *)&m_pendingLIRQ,         sizeof(bool));
    stream->write((char *)&m_mode,                sizeof(t_VDPMODES));
    stream->write((char *)&m_latch,               sizeof(u8));
    
	stream->write((char *)&m_memory[0x0000], VDP_MEM);
}

void Video::LoadState(istream *stream) {
    stream->read((char *)&m_regs[0],             sizeof(u8)*16);
    stream->read((char *)&m_palettes[0],         sizeof(u8)*64);
    stream->read((char *)&m_rgbPalettes[0][0],   sizeof(u8)*32*3);
    stream->read((char *)&m_address,             sizeof(u16));
    stream->read((char *)&m_numWrite,            sizeof(u8));
    stream->read((char *)&m_partialAddress,      sizeof(u8));
    stream->read((char *)&m_line,                sizeof(u16));
    stream->read((char *)&m_cycles,              sizeof(u32));
    stream->read((char *)&m_cyclesLine,          sizeof(u16));
    stream->read((char *)&m_status,              sizeof(u8));
    stream->read((char *)&m_lineIrqCounter,      sizeof(s16));
    stream->read((char *)&m_vramAddress,         sizeof(bool));
    stream->read((char *)&m_readBuffer,          sizeof(u8));
    stream->read((char *)&m_pendingVIRQ,         sizeof(bool));
    stream->read((char *)&m_pendingLIRQ,         sizeof(bool));
    stream->read((char *)&m_mode,                sizeof(t_VDPMODES));
    stream->read((char *)&m_latch,               sizeof(u8));
    
	stream->read((char *)&m_memory[0x0000], VDP_MEM);
}
