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

#ifndef __NES_REGISTERS_H__
#define __NES_REGISTERS_H__

#include "../Common/Types.h"
#include <iostream>

namespace Nes {

    class Registers
    {
    private:
        u8 m_A, m_X, m_Y;
        u8 m_S;   // Stack Pointer
        u8 m_P;   // Status Register
        u16 m_PC; // Program Counter

    public:
        Registers();
        ~Registers();

        Registers *GetPtrRegisters();

        inline u8 GetA()			{ return m_A; }
        inline void SetA(u8 value)	{ m_A = value; }
        inline u8 *PtrA()           { return &m_A; }
        inline u8 GetX()			{ return m_X; }
        inline void SetX(u8 value)	{ m_X = value; }
        inline u8 *PtrX()           { return &m_X; }
        inline u8 GetY()			{ return m_Y; }
        inline void SetY(u8 value)	{ m_Y = value; }
        inline u8 *PtrY()           { return &m_Y; }

        inline u8 GetS()			{ return m_S; }
        inline void SetS(u8 value)	{ m_S = value; }
        inline u8 *PtrS()           { return &m_S; }
        inline void AddS(int value) { m_S += value; };
        inline u8 GetP()			{ return m_P; }
        inline void SetP(u8 value)	{ m_P = value; }
        inline u8 *PtrP()           { return &m_P; }

        inline u16 GetPC()           { return m_PC; }
        inline void SetPC(u16 value) { m_PC = value; }
        inline u16 *PtrPC()          { return &m_PC; }
        inline void AddPC(int value) { m_PC += value; };

        void ResetRegs();
        void SaveRegs(std::ostream *stream);
        void LoadRegs(std::istream *stream);
        
        std::string ToString();
    };
}

#endif
