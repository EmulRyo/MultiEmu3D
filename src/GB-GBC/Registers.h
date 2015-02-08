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

#ifndef __REGISTERS_H__
#define __REGISTERS_H__

#include <iostream>
#include "../Common/Types.h"

namespace GameBoy {
    
    enum e_registers {
        A = 0x00, B, C, D, E, F, H, L,	//registros simples
        AF = 0x10, BC, DE, HL,			//registros dobles
        f_Z = 0x20, f_N, f_H, f_C,		//flags
        PC = 0x30, SP, $, c_$$,
        c_BC = 0x40, c_DE, c_HL,		//memoria apuntada por el registro doble
    };

    union u_register{
        u16 doble;
        u8 simple[2];
    };

    class Registers
    {
    private:
        u_register m_af, m_bc, m_de, m_hl;
        u16 m_pc; //Program Counter
        u16 m_sp; //Stack Pointer
        bool m_IME;
        bool m_pendingIME;
        bool m_pendingIMEvalue;
        bool m_halt;
        bool m_stop;
        bool m_conditionalTaken;
    public:
        Registers();
        ~Registers();

        Registers *GetPtrRegisters();

        inline u8 GetA()				{ return m_af.simple[1]; }
        inline void SetA(u8 value)	{ m_af.simple[1] = value; }
        inline u8 GetB()				{ return m_bc.simple[1]; }
        inline void SetB(u8 value)	{ m_bc.simple[1] = value; }
        inline u8 GetC()				{ return m_bc.simple[0]; }
        inline void SetC(u8 value)	{ m_bc.simple[0] = value; }
        inline u8 GetD()				{ return m_de.simple[1]; }
        inline void SetD(u8 value)	{ m_de.simple[1] = value; }
        inline u8 GetE()				{ return m_de.simple[0]; }
        inline void SetE(u8 value)	{ m_de.simple[0] = value; }
        inline u8 GetF()				{ return m_af.simple[0]; }
        inline void SetF(u8 value)	{ m_af.simple[0] = value & 0xF0; }
        inline u8 GetH()				{ return m_hl.simple[1]; }
        inline void SetH(u8 value)	{ m_hl.simple[1] = value; }
        inline u8 GetL()				{ return m_hl.simple[0]; }
        inline void SetL(u8 value)	{ m_hl.simple[0] = value; }

        inline u16 GetAF()             { return m_af.doble; }
        inline void SetAF(u16 value)	{ m_af.doble = value & 0xFFF0; }
        inline u16 GetBC()             { return m_bc.doble; }
        inline void SetBC(u16 value)	{ m_bc.doble = value; }
        inline u16 GetDE()             { return m_de.doble; }
        inline void SetDE(u16 value)	{ m_de.doble = value; }
        inline u16 GetHL()             { return m_hl.doble; }
        inline void SetHL(u16 value)	{ m_hl.doble = value; }

        inline u16 GetPC()             { return m_pc; }
        inline void SetPC(u16 value)   { m_pc = value; }
        inline void AddPC(int value)    { m_pc += value; };
        inline u16 GetSP()             { return m_sp; }
        inline void SetSP(u16 value)   { m_sp = value; }
        inline void AddSP(int value)    { m_sp += value; };

        inline bool GetIME()			{return m_IME;}
        inline void SetIME(bool value, bool immediately=true)
        {
            if (immediately)
            {
                m_IME = value;
                m_pendingIME = false;
            }
            else
            {
                m_pendingIME = true;
                m_pendingIMEvalue = value;
            }
        }
        
        inline void Set_PendingIME()
        {
            if (m_pendingIME)
            {
                m_IME = m_pendingIMEvalue;
                m_pendingIME = false;
            }
        }

        inline bool GetHalt()				{ return m_halt; }
        inline void SetHalt(bool value)	{ m_halt = value; }

        inline bool GetStop()				{ return m_stop; }
        inline void SetStop(bool value)	{ m_stop = value; }

        u16 GetReg(e_registers reg);
        void SetReg(e_registers reg, u16 value);

        inline u8 GetFlagZ()              { return (m_af.simple[0] >> 7);}
        inline void SetFlagZ(u8 value)    { m_af.simple[0] = (m_af.simple[0] & 0x7F) | (value << 7); }
        inline u8 GetFlagN()              { return ((m_af.simple[0] & 0x40) >> 6); }
        inline void SetFlagN(u8 value)    { m_af.simple[0] = (m_af.simple[0] & 0xBF) | (value << 6); }
        inline u8 GetFlagH()              { return ((m_af.simple[0] & 0x20) >> 5); }
        inline void SetFlagH(u8 value)    { m_af.simple[0] = (m_af.simple[0] & 0xDF) | (value << 5); }
        inline u8 GetFlagC()              { return ((m_af.simple[0] & 0x10) >> 4); }
        inline void SetFlagC(u8 value)    { m_af.simple[0] = (m_af.simple[0] & 0xEF) | (value << 4); }
        
        u8 GetFlag(e_registers flag);
        void SetFlag(e_registers flag, u8 value);
        
        void SetConditionalTaken(bool value) { m_conditionalTaken = value; }
        bool GetConditionalTaken() { return m_conditionalTaken; }

        void ResetRegs();
        void SaveRegs(std::ostream *stream);
        void LoadRegs(std::istream *stream);
        
        std::string ToString();
    };
}

#endif
