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

#include "../Common/Types.h"
#include <iostream>

namespace MasterSystem {
    
    enum e_registers {
        A    = 0x00, B, C, D, E, F, H, L,       //registros simples
        AF   = 0x10, BC, DE, HL, IX, IY,        //registros dobles
        f_Z  = 0x20, f_N, f_H, f_C, f_S, f_PV,	//flags
        PC   = 0x30, SP, $, c_$$,
        c_BC = 0x40, c_DE, c_HL,                //memoria apuntada por el registro d
    };

    union u_register{
        u16 d;      // double
        u8  s[2];   // simple
    };

    class Registers
    {
    private:
        u_register m_af, m_bc, m_de, m_hl, m_ix, m_iy;
        u_register m_af2, m_bc2, m_de2, m_hl2;
        u8 m_r, m_i;
        u16 m_pc; //Program Counter
        u16 m_sp; //Stack Pointer
        u8 m_intMode;
        bool m_intPending;
        bool m_iff1;
        bool m_iff2;
        bool m_halt;
        bool m_conditionalTaken;
        bool m_incPC;
    public:
        Registers();
        ~Registers();

        Registers *GetPtrRegisters();

        inline u8 GetA()			{ return m_af.s[1]; }
        inline void SetA(u8 value)	{ m_af.s[1] = value; }
        inline u8 *PtrA()           { return &m_af.s[1]; }
        inline u8 GetB()			{ return m_bc.s[1]; }
        inline void SetB(u8 value)	{ m_bc.s[1] = value; }
        inline u8 *PtrB()           { return &m_bc.s[1]; }
        inline u8 GetC()			{ return m_bc.s[0]; }
        inline void SetC(u8 value)	{ m_bc.s[0] = value; }
        inline u8 *PtrC()           { return &m_bc.s[0]; }
        inline u8 GetD()			{ return m_de.s[1]; }
        inline void SetD(u8 value)	{ m_de.s[1] = value; }
        inline u8 *PtrD()           { return &m_de.s[1]; }
        inline u8 GetE()			{ return m_de.s[0]; }
        inline void SetE(u8 value)	{ m_de.s[0] = value; }
        inline u8 *PtrE()           { return &m_de.s[0]; }
        inline u8 GetF()			{ return m_af.s[0]; }
        inline void SetF(u8 value)	{ m_af.s[0] = value; }
        inline u8 *PtrF()           { return &m_af.s[0]; }
        inline u8 GetH()			{ return m_hl.s[1]; }
        inline void SetH(u8 value)	{ m_hl.s[1] = value; }
        inline u8 *PtrH()           { return &m_hl.s[1]; }
        inline u8 GetL()			{ return m_hl.s[0]; }
        inline void SetL(u8 value)	{ m_hl.s[0] = value; }
        inline u8 *PtrL()           { return &m_hl.s[0]; }
        inline u8 GetI()			{ return m_i; }
        inline void SetI(u8 value)	{ m_i = value; }
        inline u8 *PtrI()           { return &m_i; }
        inline u8 GetR()			{ return m_r; }
        inline void SetR(u8 value)	{ m_r = value; }
        inline u8 *PtrR()           { return &m_r; }
        inline void AddR(u8 value)	{ m_r = (m_r & 0x80) | ((m_r+value) & 0x7F); }

        inline u16 GetAF()           { return m_af.d; }
        inline void SetAF(u16 value) { m_af.d = value; }
        inline u16 *PtrAF()          { return &m_af.d; }
        inline u16 GetBC()           { return m_bc.d; }
        inline void SetBC(u16 value) { m_bc.d = value; }
        inline u16 *PtrBC()          { return &m_bc.d; }
        inline u16 GetDE()           { return m_de.d; }
        inline void SetDE(u16 value) { m_de.d = value; }
        inline u16 *PtrDE()          { return &m_de.d; }
        inline u16 GetHL()           { return m_hl.d; }
        inline void SetHL(u16 value) { m_hl.d = value; }
        inline u16 *PtrHL()          { return &m_hl.d; }
        
        inline u16 GetAF2()           { return m_af2.d; }
        inline void SetAF2(u16 value) { m_af2.d = value; }
        inline u16 GetBC2()           { return m_bc2.d; }
        inline void SetBC2(u16 value) { m_bc2.d = value; }
        inline u16 GetDE2()           { return m_de2.d; }
        inline void SetDE2(u16 value) { m_de2.d = value; }
        inline u16 GetHL2()           { return m_hl2.d; }
        inline void SetHL2(u16 value) { m_hl2.d = value; }

        inline u16  GetIX()           { return m_ix.d; }
        inline u8   GetIXh()          { return m_ix.s[1]; }
        inline u8   GetIXl()          { return m_ix.s[0]; }
        inline void SetIX(u16 value)  { m_ix.d = value; }
        inline u16 *PtrIX()           { return &m_ix.d; }
        inline u8  *PtrIXh()          { return &m_ix.s[1]; }
        inline u8  *PtrIXl()          { return &m_ix.s[0]; }
        inline u16  GetIY()           { return m_iy.d; }
        inline u8   GetIYh()          { return m_iy.s[1]; }
        inline u8   GetIYl()          { return m_iy.s[0]; }
        inline void SetIY(u16 value)  { m_iy.d = value; }
        inline u16 *PtrIY()           { return &m_iy.d; }
        inline u8  *PtrIYh()          { return &m_iy.s[1]; }
        inline u8  *PtrIYl()          { return &m_iy.s[0]; }

        inline u16 GetPC()           { return m_pc; }
        inline void SetPC(u16 value) { m_pc = value; }
        inline u16 *PtrPC()          { return &m_pc; }
        inline void AddPC(int value) { m_pc += value; };
        inline u16 GetSP()           { return m_sp; }
        inline void SetSP(u16 value) { m_sp = value; }
        inline u16 *PtrSP()          { return &m_sp; }
        inline void AddSP(int value) { m_sp += value; };

        inline bool GetIntPending()           { return m_intPending; }
        inline void SetIntPending(bool value) { m_intPending = value; }
        inline bool GetIFF1()                 { return m_iff1; }
        inline bool GetIFF2()                 { return m_iff2; }
        inline void SetIFF1(bool value)       { m_iff1 = value; }
        inline void SetIFF2(bool value)       { m_iff2 = value; }
        
        inline u8   GetIntMode()         { return m_intMode; }
        inline void SetIntMode(u8 value) { m_intMode = value; }

        inline bool GetHalt()			{ return m_halt; }
        inline void SetHalt(bool value)	{ m_halt = value; }
        
        inline bool GetIncPC()           { return m_incPC; }
        inline void SetIncPC(bool incPC) { m_incPC = incPC; }

        u16  GetReg(e_registers reg);
        void SetReg(e_registers reg, u16 value);

        inline u8 GetFlagS()              { return (m_af.s[0] >> 7);}
        inline void SetFlagS(u8 value)    { m_af.s[0] = (m_af.s[0] & 0x7F) | ((value ? 1 : 0) << 7); }
        inline u8 GetFlagZ()              { return ((m_af.s[0] & 0x40) >> 6); }
        inline void SetFlagZ(u8 value)    { m_af.s[0] = (m_af.s[0] & 0xBF) | ((value ? 1 : 0) << 6); }
        inline u8 GetFlagY()              { return ((m_af.s[0] & 0x20) >> 5); }
        inline void SetFlagY(u8 value)    { m_af.s[0] = (m_af.s[0] & 0xDF) | ((value ? 1 : 0) << 5); }
        inline u8 GetFlagH()              { return ((m_af.s[0] & 0x10) >> 4); }
        inline void SetFlagH(u8 value)    { m_af.s[0] = (m_af.s[0] & 0xEF) | ((value ? 1 : 0) << 4); }
        inline u8 GetFlagX()              { return ((m_af.s[0] & 0x08) >> 3); }
        inline void SetFlagX(u8 value)    { m_af.s[0] = (m_af.s[0] & 0xF7) | ((value ? 1 : 0) << 3); }
        inline u8 GetFlagPV()             { return ((m_af.s[0] & 0x04) >> 2); }
        inline void SetFlagPV(u8 value)   { m_af.s[0] = (m_af.s[0] & 0xFB) | ((value ? 1 : 0) << 2); }
        inline u8 GetFlagN()              { return ((m_af.s[0] & 0x02) >> 1); }
        inline void SetFlagN(u8 value)    { m_af.s[0] = (m_af.s[0] & 0xFD) | ((value ? 1 : 0) << 1); }
        inline u8 GetFlagC()              { return (m_af.s[0] & 0x01); }
        inline void SetFlagC(u8 value)    { m_af.s[0] = (m_af.s[0] & 0xFE) | (value ? 1 : 0); }
        
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
