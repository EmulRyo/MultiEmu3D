/*
 This file is part of MarkSystem.
 
 MultiEmu3D is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 MultiEmu3D is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with MarkSystem.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iomanip>
#include <sstream>
#include "CPU.h"
#include "Instructions.h"
#include "InstructionsLength.h"
#include "InstructionsCycles.h"
#include "../Common/Exception.h"

using namespace std;
using namespace MasterSystem;

u8 CPU::Get8BitsInmValue(u8 offset) {
    return (MemR(GetPC() + offset));
}

u16 CPU::Get16BitsInmValue(u8 offset) {
    return ((MemR(GetPC() + offset + 1)) << 8) | MemR(GetPC() + offset);
}

u16 CPU::GetIXPlusD(u8 offset) {
    return GetIX()+(s8)Get8BitsInmValue(offset);
}

u16 CPU::GetIYPlusD(u8 offset) {
    return GetIY()+(s8)Get8BitsInmValue(offset);
}

void CPU::ExecuteOpcode(u8 opcode, Instructions &inst) {
    if (GetHalt()) {
        m_lastCycles += 4;
        return;
    }
    
    bool executed = false;
    SetIncPC(true);
    SetConditionalTaken(false);
    AddR(1);
    
    switch(opcode)
    {
        case (0x00): inst.NOP(); break;
        case (0x01): inst.LD(PtrBC(), Get16BitsInmValue(1)); break;
        case (0x02): inst.LD_Mem(GetBC(), GetA()); break;
        case (0x03): inst.INC_NoFlags(PtrBC()); break;
        case (0x04): inst.INC(PtrB()); break;
        case (0x05): inst.DEC(PtrB()); break;
        case (0x06): inst.LD(PtrB(), Get8BitsInmValue(1)); break;
        case (0x07): inst.RLCA(); break;
        case (0x08): inst.EX_AF_AF2(); break;
        case (0x09): inst.ADD(PtrHL(), GetBC()); break;
        case (0x0A): inst.LD(PtrA(), MemR(GetBC())); break;
        case (0x0B): inst.DEC_NoFlags(PtrBC()); break;
        case (0x0C): inst.INC(PtrC()); break;
        case (0x0D): inst.DEC(PtrC()); break;
        case (0x0E): inst.LD(PtrC(), Get8BitsInmValue(1)); break;
        case (0x0F): inst.RRCA(); break;

        case (0x10): inst.DJNZ(); break;
        case (0x11): inst.LD(PtrDE(), Get16BitsInmValue(1)); break;
        case (0x12): inst.LD_Mem(GetDE(), GetA()); break;
        case (0x13): inst.INC_NoFlags(PtrDE()); break;
        case (0x14): inst.INC(PtrD()); break;
        case (0x15): inst.DEC(PtrD()); break;
        case (0x16): inst.LD(PtrD(), Get8BitsInmValue(1)); break;
        case (0x17): inst.RLA(); break;
        case (0x18): inst.JR(); break;
        case (0x19): inst.ADD(PtrHL(), GetDE()); break;
        case (0x1A): inst.LD(PtrA(), MemR(GetDE())); break;
        case (0x1B): inst.DEC_NoFlags(PtrDE()); break;
        case (0x1C): inst.INC(PtrE()); break;
        case (0x1D): inst.DEC(PtrE()); break;
        case (0x1E): inst.LD(PtrE(), Get8BitsInmValue(1)); break;
        case (0x1F): inst.RRA(); break;

        case (0x20): inst.JR_CC_n(f_Z, 0); break;
        case (0x21): inst.LD(PtrHL(), Get16BitsInmValue(1)); break;
        case (0x22): inst.LD_Mem(Get16BitsInmValue(1), GetHL()); break;
        case (0x23): inst.INC_NoFlags(PtrHL()); break;
        case (0x24): inst.INC(PtrH()); break;
        case (0x25): inst.DEC(PtrH()); break;
        case (0x26): inst.LD(PtrH(), Get8BitsInmValue(1)); break;
        case (0x27): inst.DAA(); break;
        case (0x28): inst.JR_CC_n(f_Z, 1); break;
        case (0x29): inst.ADD(PtrHL(), GetHL()); break;
        case (0x2A): inst.LD_HL_cNN(); break;
        case (0x2B): inst.DEC_NoFlags(PtrHL()); break;
        case (0x2C): inst.INC(PtrL()); break;
        case (0x2D): inst.DEC(PtrL()); break;
        case (0x2E): inst.LD(PtrL(), Get8BitsInmValue(1)); break;
        case (0x2F): inst.CPL(); break;

        case (0x30): inst.JR_CC_n(f_C, 0); break;
        case (0x31): inst.LD(PtrSP(), Get16BitsInmValue(1)); break;
        case (0x32): inst.LD_Mem(Get16BitsInmValue(1), GetA()); break;
        case (0x33): inst.INC_NoFlags(PtrSP()); break;
        case (0x34): inst.INC_Mem(GetHL()); break;
        case (0x35): inst.DEC_Mem(GetHL()); break;
        case (0x36): inst.LD_Mem(GetHL(), Get8BitsInmValue(1)); break;
        case (0x37): inst.SCF(); break;
        case (0x38): inst.JR_CC_n(f_C, 1); break;
        case (0x39): inst.ADD(PtrHL(), GetSP()); break;
        case (0x3A): inst.LD(PtrA(), MemR(Get16BitsInmValue(1))); break;
        case (0x3B): inst.DEC_NoFlags(PtrSP()); break;
        case (0x3C): inst.INC(PtrA()); break;
        case (0x3D): inst.DEC(PtrA()); break;
        case (0x3E): inst.LD(PtrA(), Get8BitsInmValue(1)); break;
        case (0x3F): inst.CCF(); break;

        case (0x40): inst.LD(PtrB(), GetB()); break;
        case (0x41): inst.LD(PtrB(), GetC()); break;
        case (0x42): inst.LD(PtrB(), GetD()); break;
        case (0x43): inst.LD(PtrB(), GetE()); break;
        case (0x44): inst.LD(PtrB(), GetH()); break;
        case (0x45): inst.LD(PtrB(), GetL()); break;
        case (0x46): inst.LD(PtrB(), MemR(GetHL())); break;
        case (0x47): inst.LD(PtrB(), GetA()); break;
        case (0x48): inst.LD(PtrC(), GetB()); break;
        case (0x49): inst.LD(PtrC(), GetC()); break;
        case (0x4A): inst.LD(PtrC(), GetD()); break;
        case (0x4B): inst.LD(PtrC(), GetE()); break;
        case (0x4C): inst.LD(PtrC(), GetH()); break;
        case (0x4D): inst.LD(PtrC(), GetL()); break;
        case (0x4E): inst.LD(PtrC(), MemR(GetHL())); break;
        case (0x4F): inst.LD(PtrC(), GetA()); break;

        case (0x50): inst.LD(PtrD(), GetB()); break;
        case (0x51): inst.LD(PtrD(), GetC()); break;
        case (0x52): inst.LD(PtrD(), GetD()); break;
        case (0x53): inst.LD(PtrD(), GetE()); break;
        case (0x54): inst.LD(PtrD(), GetH()); break;
        case (0x55): inst.LD(PtrD(), GetL()); break;
        case (0x56): inst.LD(PtrD(), MemR(GetHL())); break;
        case (0x57): inst.LD(PtrD(), GetA()); break;
        case (0x58): inst.LD(PtrE(), GetB()); break;
        case (0x59): inst.LD(PtrE(), GetC()); break;
        case (0x5A): inst.LD(PtrE(), GetD()); break;
        case (0x5B): inst.LD(PtrE(), GetE()); break;
        case (0x5C): inst.LD(PtrE(), GetH()); break;
        case (0x5D): inst.LD(PtrE(), GetL()); break;
        case (0x5E): inst.LD(PtrE(), MemR(GetHL())); break;
        case (0x5F): inst.LD(PtrE(), GetA()); break;

        case (0x60): inst.LD(PtrH(), GetB()); break;
        case (0x61): inst.LD(PtrH(), GetC()); break;
        case (0x62): inst.LD(PtrH(), GetD()); break;
        case (0x63): inst.LD(PtrH(), GetE()); break;
        case (0x64): inst.LD(PtrH(), GetH()); break;
        case (0x65): inst.LD(PtrH(), GetL()); break;
        case (0x66): inst.LD(PtrH(), MemR(GetHL())); break;
        case (0x67): inst.LD(PtrH(), GetA()); break;
        case (0x68): inst.LD(PtrL(), GetB()); break;
        case (0x69): inst.LD(PtrL(), GetC()); break;
        case (0x6A): inst.LD(PtrL(), GetD()); break;
        case (0x6B): inst.LD(PtrL(), GetE()); break;
        case (0x6C): inst.LD(PtrL(), GetH()); break;
        case (0x6D): inst.LD(PtrL(), GetL()); break;
        case (0x6E): inst.LD(PtrL(), MemR(GetHL())); break;
        case (0x6F): inst.LD(PtrL(), GetA()); break;

        case (0x70): inst.LD_Mem(GetHL(), GetB()); break;
        case (0x71): inst.LD_Mem(GetHL(), GetC()); break;
        case (0x72): inst.LD_Mem(GetHL(), GetD()); break;
        case (0x73): inst.LD_Mem(GetHL(), GetE()); break;
        case (0x74): inst.LD_Mem(GetHL(), GetH()); break;
        case (0x75): inst.LD_Mem(GetHL(), GetL()); break;
        case (0x76): inst.HALT(); break;
        case (0x77): inst.LD_Mem(GetHL(), GetA()); break;
        case (0x78): inst.LD(PtrA(), GetB()); break;
        case (0x79): inst.LD(PtrA(), GetC()); break;
        case (0x7A): inst.LD(PtrA(), GetD()); break;
        case (0x7B): inst.LD(PtrA(), GetE()); break;
        case (0x7C): inst.LD(PtrA(), GetH()); break;
        case (0x7D): inst.LD(PtrA(), GetL()); break;
        case (0x7E): inst.LD(PtrA(), MemR(GetHL())); break;
        case (0x7F): inst.LD(PtrA(), GetA()); break;

        case (0x80): inst.ADD(GetB()); break;
        case (0x81): inst.ADD(GetC()); break;
        case (0x82): inst.ADD(GetD()); break;
        case (0x83): inst.ADD(GetE()); break;
        case (0x84): inst.ADD(GetH()); break;
        case (0x85): inst.ADD(GetL()); break;
        case (0x86): inst.ADD(MemR(GetHL())); break;
        case (0x87): inst.ADD(GetA()); break;
        case (0x88): inst.ADC(GetB()); break;
        case (0x89): inst.ADC(GetC()); break;
        case (0x8A): inst.ADC(GetD()); break;
        case (0x8B): inst.ADC(GetE()); break;
        case (0x8C): inst.ADC(GetH()); break;
        case (0x8D): inst.ADC(GetL()); break;
        case (0x8E): inst.ADC(MemR(GetHL())); break;
        case (0x8F): inst.ADC(GetA()); break;

        case (0x90): inst.SUB(GetB()); break;
        case (0x91): inst.SUB(GetC()); break;
        case (0x92): inst.SUB(GetD()); break;
        case (0x93): inst.SUB(GetE()); break;
        case (0x94): inst.SUB(GetH()); break;
        case (0x95): inst.SUB(GetL()); break;
        case (0x96): inst.SUB(MemR(GetHL())); break;
        case (0x97): inst.SUB(GetA()); break;
        case (0x98): inst.SBC(GetB()); break;
        case (0x99): inst.SBC(GetC()); break;
        case (0x9A): inst.SBC(GetD()); break;
        case (0x9B): inst.SBC(GetE()); break;
        case (0x9C): inst.SBC(GetH()); break;
        case (0x9D): inst.SBC(GetL()); break;
        case (0x9E): inst.SBC(MemR(GetHL())); break;
        case (0x9F): inst.SBC(GetA()); break;

        case (0xA0): inst.AND(GetB()); break;
        case (0xA1): inst.AND(GetC()); break;
        case (0xA2): inst.AND(GetD()); break;
        case (0xA3): inst.AND(GetE()); break;
        case (0xA4): inst.AND(GetH()); break;
        case (0xA5): inst.AND(GetL()); break;
        case (0xA6): inst.AND(MemR(GetHL())); break;
        case (0xA7): inst.AND(GetA()); break;
        case (0xA8): inst.XOR(GetB()); break;
        case (0xA9): inst.XOR(GetC()); break;
        case (0xAA): inst.XOR(GetD()); break;
        case (0xAB): inst.XOR(GetE()); break;
        case (0xAC): inst.XOR(GetH()); break;
        case (0xAD): inst.XOR(GetL()); break;
        case (0xAE): inst.XOR(MemR(GetHL())); break;
        case (0xAF): inst.XOR(GetA()); break;

        case (0xB0): inst.OR(GetB()); break;
        case (0xB1): inst.OR(GetC()); break;
        case (0xB2): inst.OR(GetD()); break;
        case (0xB3): inst.OR(GetE()); break;
        case (0xB4): inst.OR(GetH()); break;
        case (0xB5): inst.OR(GetL()); break;
        case (0xB6): inst.OR(MemR(GetHL())); break;
        case (0xB7): inst.OR(GetA()); break;
        case (0xB8): inst.CP(GetB()); break;
        case (0xB9): inst.CP(GetC()); break;
        case (0xBA): inst.CP(GetD()); break;
        case (0xBB): inst.CP(GetE()); break;
        case (0xBC): inst.CP(GetH()); break;
        case (0xBD): inst.CP(GetL()); break;
        case (0xBE): inst.CP(MemR(GetHL())); break;
        case (0xBF): inst.CP(GetA()); break;

        case (0xC0): inst.RET_cc(f_Z, 0); break;
        case (0xC1): inst.POP(PtrBC()); break;
        case (0xC2): inst.JP_cc_nn(f_Z, 0); break;
        case (0xC3): inst.JP(Get16BitsInmValue(1)); break;
        case (0xC4): inst.CALL_cc_nn(f_Z, 0); break;
        case (0xC5): inst.PUSH(GetBC()); break;
        case (0xC6): inst.ADD(Get8BitsInmValue(1)); break;
        case (0xC7): inst.RST_n(0x00); break;
        case (0xC8): inst.RET_cc(f_Z, 1); break;
        case (0xC9): inst.RET(); break;
        case (0xCA): inst.JP_cc_nn(f_Z, 1); break;
        case (0xCB): OpcodeCB(inst, executed); break;
        case (0xCC): inst.CALL_cc_nn(f_Z, 1); break;
        case (0xCD): inst.CALL_nn(); break;
        case (0xCE): inst.ADC(Get8BitsInmValue(1)); break;
        case (0xCF): inst.RST_n(0x08); break;

        case (0xD0): inst.RET_cc(f_C, 0); break;
        case (0xD1): inst.POP(PtrDE()); break;
        case (0xD2): inst.JP_cc_nn(f_C, 0); break;
        case (0xD3): inst.OUT(Get8BitsInmValue(1), GetA()); break;
        case (0xD4): inst.CALL_cc_nn(f_C, 0); break;
        case (0xD5): inst.PUSH(GetDE()); break;
        case (0xD6): inst.SUB(Get8BitsInmValue(1)); break;
        case (0xD7): inst.RST_n(0x10); break;
        case (0xD8): inst.RET_cc(f_C, 1); break;
        case (0xD9): inst.EXX(); break;
        case (0xDA): inst.JP_cc_nn(f_C, 1); break;
        case (0xDB): inst.IN_A_n(); break;
        case (0xDC): inst.CALL_cc_nn(f_C, 1); break;
        case (0xDD): OpcodeDD(inst, executed); break;
        case (0xDE): inst.SBC(Get8BitsInmValue(1)); break;
        case (0xDF): inst.RST_n(0x18); break;

        case (0xE0): inst.RET_cc(f_PV, 0); break;
        case (0xE1): inst.POP(PtrHL()); break;
        case (0xE2): inst.JP_cc_nn(f_PV, 0); break;
        case (0xE3): inst.EX_cSP(PtrHL()); break;
        case (0xE4): inst.CALL_cc_nn(f_PV, 0); break;
        case (0xE5): inst.PUSH(GetHL()); break;
        case (0xE6): inst.AND(Get8BitsInmValue(1)); break;
        case (0xE7): inst.RST_n(0x20); break;
        case (0xE8): inst.RET_cc(f_PV, 1); break;
        case (0xE9): inst.JP(GetHL()); break;
        case (0xEA): inst.JP_cc_nn(f_PV, 1); break;
        case (0xEB): inst.EX_DE_HL(); break;
        case (0xEC): inst.CALL_cc_nn(f_PV, 1); break;
        case (0xED): OpcodeED(inst, executed); break;; break;
        case (0xEE): inst.XOR(Get8BitsInmValue(1)); break;
        case (0xEF): inst.RST_n(0x28); break;

        case (0xF0): inst.RET_cc(f_S, 0); break;
        case (0xF1): inst.POP(PtrAF()); break;
        case (0xF2): inst.JP_cc_nn(f_S, 0); break;
        case (0xF3): inst.DI(); break;
        case (0xF4): inst.CALL_cc_nn(f_S, 0); break;
        case (0xF5): inst.PUSH(GetAF()); break;
        case (0xF6): inst.OR(Get8BitsInmValue(1)); break;
        case (0xF7): inst.RST_n(0x30); break;
        case (0xF8): inst.RET_cc(f_S, 1); break;
        case (0xF9): inst.LD(PtrSP(), GetHL()); break;
        case (0xFA): inst.JP_cc_nn(f_S, 1); break;
        case (0xFB): inst.EI(); break;
        case (0xFC): inst.CALL_cc_nn(f_S, 1); break;
        case (0xFD): OpcodeFD(inst, executed); break;
        case (0xFE): inst.CP(Get8BitsInmValue(1)); break;
        case (0xFF): inst.RST_n(0x38); break;
        default:
            stringstream out;
            out << "Error, instruction not implemented: 0x";
            out << setfill('0') << setw(2) << uppercase << hex << (int)opcode << endl;
            cout << out.str();
            throw Exception(out.str());
            
    } // end switch
    
    if (!executed) {
        if (GetIncPC())
            AddPC(GetInstructionLength(opcode));
        
        if (GetConditionalTaken())
            m_lastCycles += GetInstructionCondicionalCycles(opcode);
        else
            m_lastCycles += GetInstructionCycles(opcode);

        executed = true;
    }
}


void CPU::OpcodeCB(Instructions &inst, bool &executed)
{
    u8 opcode = MemR(GetPC() + 1);
    AddR(1);

    switch (opcode)
    {
		case (0x00): inst.RLC(PtrB()); break;
		case (0x01): inst.RLC(PtrC()); break;
		case (0x02): inst.RLC(PtrD()); break;
		case (0x03): inst.RLC(PtrE()); break;
		case (0x04): inst.RLC(PtrH()); break;
		case (0x05): inst.RLC(PtrL()); break;
		case (0x06): inst.RLC_Mem(GetHL()); break;
		case (0x07): inst.RLC(PtrA()); break;
		case (0x08): inst.RRC(PtrB()); break;
		case (0x09): inst.RRC(PtrC()); break;
		case (0x0A): inst.RRC(PtrD()); break;
		case (0x0B): inst.RRC(PtrE()); break;
		case (0x0C): inst.RRC(PtrH()); break;
		case (0x0D): inst.RRC(PtrL()); break;
		case (0x0E): inst.RRC_Mem(GetHL()); break;
		case (0x0F): inst.RRC(PtrA()); break;

		case (0x10): inst.RL(PtrB()); break;
		case (0x11): inst.RL(PtrC()); break;
		case (0x12): inst.RL(PtrD()); break;
		case (0x13): inst.RL(PtrE()); break;
		case (0x14): inst.RL(PtrH()); break;
		case (0x15): inst.RL(PtrL()); break;
		case (0x16): inst.RL_Mem(GetHL()); break;
		case (0x17): inst.RL(PtrA()); break;
		case (0x18): inst.RR(PtrB()); break;
		case (0x19): inst.RR(PtrC()); break;
		case (0x1A): inst.RR(PtrD()); break;
		case (0x1B): inst.RR(PtrE()); break;
		case (0x1C): inst.RR(PtrH()); break;
		case (0x1D): inst.RR(PtrL()); break;
		case (0x1E): inst.RR_Mem(GetHL()); break;
		case (0x1F): inst.RR(PtrA()); break;

        case (0x20): inst.SLA(PtrB()); break;
        case (0x21): inst.SLA(PtrC()); break;
        case (0x22): inst.SLA(PtrD()); break;
        case (0x23): inst.SLA(PtrE()); break;
        case (0x24): inst.SLA(PtrH()); break;
        case (0x25): inst.SLA(PtrL()); break;
        case (0x26): inst.SLA_Mem(GetHL()); break;
        case (0x27): inst.SLA(PtrA()); break;
		case (0x28): inst.SRA(PtrB()); break;
		case (0x29): inst.SRA(PtrC()); break;
		case (0x2A): inst.SRA(PtrD()); break;
		case (0x2B): inst.SRA(PtrE()); break;
		case (0x2C): inst.SRA(PtrH()); break;
		case (0x2D): inst.SRA(PtrL()); break;
		case (0x2E): inst.SRA_Mem(GetHL()); break;
		case (0x2F): inst.SRA(PtrA()); break;

        case (0x30): inst.SLL(PtrB()); break;
        case (0x31): inst.SLL(PtrC()); break;
        case (0x32): inst.SLL(PtrD()); break;
        case (0x33): inst.SLL(PtrE()); break;
        case (0x34): inst.SLL(PtrH()); break;
        case (0x35): inst.SLL(PtrL()); break;
        case (0x36): inst.SLL_Mem(GetHL()); break;
        case (0x37): inst.SLL(PtrA()); break;
		case (0x38): inst.SRL(PtrB()); break;
        case (0x39): inst.SRL(PtrC()); break;
        case (0x3A): inst.SRL(PtrD()); break;
        case (0x3B): inst.SRL(PtrE()); break;
        case (0x3C): inst.SRL(PtrH()); break;
        case (0x3D): inst.SRL(PtrL()); break;
        case (0x3E): inst.SRL_Mem(GetHL()); break;
        case (0x3F): inst.SRL(PtrA()); break;

		case (0x40): inst.BIT(0, GetB()); break;
		case (0x41): inst.BIT(0, GetC()); break;
		case (0x42): inst.BIT(0, GetD()); break;
		case (0x43): inst.BIT(0, GetE()); break;
		case (0x44): inst.BIT(0, GetH()); break;
		case (0x45): inst.BIT(0, GetL()); break;
		case (0x46): inst.BIT_HL(0); break;
		case (0x47): inst.BIT(0, GetA()); break;
		case (0x48): inst.BIT(1, GetB()); break;
		case (0x49): inst.BIT(1, GetC()); break;
		case (0x4A): inst.BIT(1, GetD()); break;
		case (0x4B): inst.BIT(1, GetE()); break;
		case (0x4C): inst.BIT(1, GetH()); break;
		case (0x4D): inst.BIT(1, GetL()); break;
		case (0x4E): inst.BIT_HL(1); break;
		case (0x4F): inst.BIT(1, GetA()); break;

		case (0x50): inst.BIT(2, GetB()); break;
		case (0x51): inst.BIT(2, GetC()); break;
		case (0x52): inst.BIT(2, GetD()); break;
		case (0x53): inst.BIT(2, GetE()); break;
		case (0x54): inst.BIT(2, GetH()); break;
		case (0x55): inst.BIT(2, GetL()); break;
		case (0x56): inst.BIT_HL(2); break;
		case (0x57): inst.BIT(2, GetA()); break;
		case (0x58): inst.BIT(3, GetB()); break;
		case (0x59): inst.BIT(3, GetC()); break;
		case (0x5A): inst.BIT(3, GetD()); break;
		case (0x5B): inst.BIT(3, GetE()); break;
		case (0x5C): inst.BIT(3, GetH()); break;
		case (0x5D): inst.BIT(3, GetL()); break;
		case (0x5E): inst.BIT_HL(3); break;
		case (0x5F): inst.BIT(3, GetA()); break;

		case (0x60): inst.BIT(4, GetB()); break;
		case (0x61): inst.BIT(4, GetC()); break;
		case (0x62): inst.BIT(4, GetD()); break;
		case (0x63): inst.BIT(4, GetE()); break;
		case (0x64): inst.BIT(4, GetH()); break;
		case (0x65): inst.BIT(4, GetL()); break;
		case (0x66): inst.BIT_HL(4); break;
		case (0x67): inst.BIT(4, GetA()); break;
		case (0x68): inst.BIT(5, GetB()); break;
		case (0x69): inst.BIT(5, GetC()); break;
		case (0x6A): inst.BIT(5, GetD()); break;
		case (0x6B): inst.BIT(5, GetE()); break;
		case (0x6C): inst.BIT(5, GetH()); break;
		case (0x6D): inst.BIT(5, GetL()); break;
		case (0x6E): inst.BIT_HL(5); break;
		case (0x6F): inst.BIT(5, GetA()); break;

		case (0x70): inst.BIT(6, GetB()); break;
		case (0x71): inst.BIT(6, GetC()); break;
		case (0x72): inst.BIT(6, GetD()); break;
		case (0x73): inst.BIT(6, GetE()); break;
		case (0x74): inst.BIT(6, GetH()); break;
		case (0x75): inst.BIT(6, GetL()); break;
		case (0x76): inst.BIT_HL(6); break;
		case (0x77): inst.BIT(6, GetA()); break;
		case (0x78): inst.BIT(7, GetB()); break;
		case (0x79): inst.BIT(7, GetC()); break;
		case (0x7A): inst.BIT(7, GetD()); break;
		case (0x7B): inst.BIT(7, GetE()); break;
		case (0x7C): inst.BIT(7, GetH()); break;
		case (0x7D): inst.BIT(7, GetL()); break;
		case (0x7E): inst.BIT_HL(7); break;
		case (0x7F): inst.BIT(7, GetA()); break;

        case (0x80): inst.RES_b_r(0, B); break;
        case (0x81): inst.RES_b_r(0, C); break;
        case (0x82): inst.RES_b_r(0, D); break;
        case (0x83): inst.RES_b_r(0, E); break;
        case (0x84): inst.RES_b_r(0, H); break;
        case (0x85): inst.RES_b_r(0, L); break;
        case (0x86): inst.RES_b_r(0, c_HL); break;
        case (0x87): inst.RES_b_r(0, A); break;
        case (0x88): inst.RES_b_r(1, B); break;
        case (0x89): inst.RES_b_r(1, C); break;
        case (0x8A): inst.RES_b_r(1, D); break;
        case (0x8B): inst.RES_b_r(1, E); break;
        case (0x8C): inst.RES_b_r(1, H); break;
        case (0x8D): inst.RES_b_r(1, L); break;
        case (0x8E): inst.RES_b_r(1, c_HL); break;
        case (0x8F): inst.RES_b_r(1, A); break;

        case (0x90): inst.RES_b_r(2, B); break;
        case (0x91): inst.RES_b_r(2, C); break;
        case (0x92): inst.RES_b_r(2, D); break;
        case (0x93): inst.RES_b_r(2, E); break;
        case (0x94): inst.RES_b_r(2, H); break;
        case (0x95): inst.RES_b_r(2, L); break;
        case (0x96): inst.RES_b_r(2, c_HL); break;
        case (0x97): inst.RES_b_r(2, A); break;
        case (0x98): inst.RES_b_r(3, B); break;
        case (0x99): inst.RES_b_r(3, C); break;
        case (0x9A): inst.RES_b_r(3, D); break;
        case (0x9B): inst.RES_b_r(3, E); break;
        case (0x9C): inst.RES_b_r(3, H); break;
        case (0x9D): inst.RES_b_r(3, L); break;
        case (0x9E): inst.RES_b_r(3, c_HL); break;
        case (0x9F): inst.RES_b_r(3, A); break;

        case (0xA0): inst.RES_b_r(4, B); break;
        case (0xA1): inst.RES_b_r(4, C); break;
        case (0xA2): inst.RES_b_r(4, D); break;
        case (0xA3): inst.RES_b_r(4, E); break;
        case (0xA4): inst.RES_b_r(4, H); break;
        case (0xA5): inst.RES_b_r(4, L); break;
        case (0xA6): inst.RES_b_r(4, c_HL); break;
        case (0xA7): inst.RES_b_r(4, A); break;
        case (0xA8): inst.RES_b_r(5, B); break;
        case (0xA9): inst.RES_b_r(5, C); break;
        case (0xAA): inst.RES_b_r(5, D); break;
        case (0xAB): inst.RES_b_r(5, E); break;
        case (0xAC): inst.RES_b_r(5, H); break;
        case (0xAD): inst.RES_b_r(5, L); break;
        case (0xAE): inst.RES_b_r(5, c_HL); break;
        case (0xAF): inst.RES_b_r(5, A); break;

        case (0xB0): inst.RES_b_r(6, B); break;
        case (0xB1): inst.RES_b_r(6, C); break;
        case (0xB2): inst.RES_b_r(6, D); break;
        case (0xB3): inst.RES_b_r(6, E); break;
        case (0xB4): inst.RES_b_r(6, H); break;
        case (0xB5): inst.RES_b_r(6, L); break;
        case (0xB6): inst.RES_b_r(6, c_HL); break;
        case (0xB7): inst.RES_b_r(6, A); break;
        case (0xB8): inst.RES_b_r(7, B); break;
        case (0xB9): inst.RES_b_r(7, C); break;
        case (0xBA): inst.RES_b_r(7, D); break;
        case (0xBB): inst.RES_b_r(7, E); break;
        case (0xBC): inst.RES_b_r(7, H); break;
        case (0xBD): inst.RES_b_r(7, L); break;
        case (0xBE): inst.RES_b_r(7, c_HL); break;
        case (0xBF): inst.RES_b_r(7, A); break;

        case (0xC0): inst.SET_b_r(0, B); break;
        case (0xC1): inst.SET_b_r(0, C); break;
        case (0xC2): inst.SET_b_r(0, D); break;
        case (0xC3): inst.SET_b_r(0, E); break;
        case (0xC4): inst.SET_b_r(0, H); break;
        case (0xC5): inst.SET_b_r(0, L); break;
        case (0xC6): inst.SET_b_r(0, c_HL); break;
        case (0xC7): inst.SET_b_r(0, A); break;
        case (0xC8): inst.SET_b_r(1, B); break;
        case (0xC9): inst.SET_b_r(1, C); break;
        case (0xCA): inst.SET_b_r(1, D); break;
        case (0xCB): inst.SET_b_r(1, E); break;
        case (0xCC): inst.SET_b_r(1, H); break;
        case (0xCD): inst.SET_b_r(1, L); break;
        case (0xCE): inst.SET_b_r(1, c_HL); break;
        case (0xCF): inst.SET_b_r(1, A); break;

        case (0xD0): inst.SET_b_r(2, B); break;
        case (0xD1): inst.SET_b_r(2, C); break;
        case (0xD2): inst.SET_b_r(2, D); break;
        case (0xD3): inst.SET_b_r(2, E); break;
        case (0xD4): inst.SET_b_r(2, H); break;
        case (0xD5): inst.SET_b_r(2, L); break;
        case (0xD6): inst.SET_b_r(2, c_HL); break;
        case (0xD7): inst.SET_b_r(2, A); break;
        case (0xD8): inst.SET_b_r(3, B); break;
        case (0xD9): inst.SET_b_r(3, C); break;
        case (0xDA): inst.SET_b_r(3, D); break;
        case (0xDB): inst.SET_b_r(3, E); break;
        case (0xDC): inst.SET_b_r(3, H); break;
        case (0xDD): inst.SET_b_r(3, L); break;
        case (0xDE): inst.SET_b_r(3, c_HL); break;
        case (0xDF): inst.SET_b_r(3, A); break;

        case (0xE0): inst.SET_b_r(4, B); break;
        case (0xE1): inst.SET_b_r(4, C); break;
        case (0xE2): inst.SET_b_r(4, D); break;
        case (0xE3): inst.SET_b_r(4, E); break;
        case (0xE4): inst.SET_b_r(4, H); break;
        case (0xE5): inst.SET_b_r(4, L); break;
        case (0xE6): inst.SET_b_r(4, c_HL); break;
        case (0xE7): inst.SET_b_r(4, A); break;
        case (0xE8): inst.SET_b_r(5, B); break;
        case (0xE9): inst.SET_b_r(5, C); break;
        case (0xEA): inst.SET_b_r(5, D); break;
        case (0xEB): inst.SET_b_r(5, E); break;
        case (0xEC): inst.SET_b_r(5, H); break;
        case (0xED): inst.SET_b_r(5, L); break;
        case (0xEE): inst.SET_b_r(5, c_HL); break;
        case (0xEF): inst.SET_b_r(5, A); break;

        case (0xF0): inst.SET_b_r(6, B); break;
        case (0xF1): inst.SET_b_r(6, C); break;
        case (0xF2): inst.SET_b_r(6, D); break;
        case (0xF3): inst.SET_b_r(6, E); break;
        case (0xF4): inst.SET_b_r(6, H); break;
        case (0xF5): inst.SET_b_r(6, L); break;
        case (0xF6): inst.SET_b_r(6, c_HL); break;
        case (0xF7): inst.SET_b_r(6, A); break;
        case (0xF8): inst.SET_b_r(7, B); break;
        case (0xF9): inst.SET_b_r(7, C); break;
        case (0xFA): inst.SET_b_r(7, D); break;
        case (0xFB): inst.SET_b_r(7, E); break;
        case (0xFC): inst.SET_b_r(7, H); break;
        case (0xFD): inst.SET_b_r(7, L); break;
        case (0xFE): inst.SET_b_r(7, c_HL); break;
        case (0xFF): inst.SET_b_r(7, A); break;

        default:
			stringstream out;
			out << "Error, instruction not implemented: 0xCB";
			out << setfill('0') << setw(2) << uppercase << hex << (int)opcode << "\n";
            cout << out.str();
			throw Exception(out.str().data());
    }
    
    if (!executed) {
        AddPC(GetInstructionCBLength(opcode));
        m_lastCycles += GetInstructionCyclesCB(opcode);
        executed = true;
    }
}

void CPU::OpcodeDD(Instructions &inst, bool &executed)
{
    u8 opcode = MemR(GetPC() + 1);
    AddR(1);
    
    switch (opcode)
    {
        case 0x09: inst.ADD(PtrIX(), GetBC()); break;
            
        case 0x19: inst.ADD(PtrIX(), GetDE()); break;
            
        case 0x21: inst.LD(PtrIX(), Get16BitsInmValue(2)); break;
        case 0x22: inst.LD_Mem(Get16BitsInmValue(2), GetIX()); break;
        case 0x23: inst.INC_NoFlags(PtrIX()); break;
        case 0x24: inst.INC(PtrIXh()); break;
        case 0x25: inst.DEC(PtrIXh()); break;
        case 0x26: inst.LD(PtrIXh(), Get8BitsInmValue(2)); break;
        case 0x29: inst.ADD(PtrIX(), GetIX()); break;
        case 0x2A: inst.LD_Content(PtrIX(), Get16BitsInmValue(2)); break;
        case 0x2B: inst.DEC_NoFlags(PtrIX()); break;
        case 0x2C: inst.INC(PtrIXl()); break;
        case 0x2D: inst.DEC(PtrIXl()); break;
        case 0x2E: inst.LD(PtrIXl(), Get8BitsInmValue(2)); break;
            
        case 0x34: inst.INC_Mem(GetIXPlusD(2)); break;
        case 0x35: inst.DEC_Mem(GetIXPlusD(2)); break;
        case 0x36: inst.LD_Mem(GetIXPlusD(2), Get8BitsInmValue(3)); break;
        case 0x39: inst.ADD(PtrIX(), GetSP()); break;
        
        case 0x40: inst.LD(PtrB(), GetB()); break;
        case 0x41: inst.LD(PtrB(), GetC()); break;
        case 0x42: inst.LD(PtrB(), GetD()); break;
        case 0x43: inst.LD(PtrB(), GetE()); break;
        case 0x44: inst.LD(PtrB(), GetIXh()); break;
        case 0x45: inst.LD(PtrB(), GetIXl()); break;
        case 0x46: inst.LD(PtrB(), MemR(GetIXPlusD(2))); break;
        case 0x47: inst.LD(PtrB(), GetA()); break;
        case 0x48: inst.LD(PtrC(), GetB()); break;
        case 0x49: inst.LD(PtrC(), GetC()); break;
        case 0x4A: inst.LD(PtrC(), GetD()); break;
        case 0x4B: inst.LD(PtrC(), GetE()); break;
        case 0x4C: inst.LD(PtrC(), GetIXh()); break;
        case 0x4D: inst.LD(PtrC(), GetIXl()); break;
        case 0x4E: inst.LD(PtrC(), MemR(GetIXPlusD(2))); break;
        case 0x4F: inst.LD(PtrC(), GetA()); break;
          
        case 0x50: inst.LD(PtrD(), GetB()); break;
        case 0x51: inst.LD(PtrD(), GetC()); break;
        case 0x52: inst.LD(PtrD(), GetD()); break;
        case 0x53: inst.LD(PtrD(), GetE()); break;
        case 0x54: inst.LD(PtrD(), GetIXh()); break;
        case 0x55: inst.LD(PtrD(), GetIXl()); break;
        case 0x56: inst.LD(PtrD(), MemR(GetIXPlusD(2))); break;
        case 0x57: inst.LD(PtrD(), GetA()); break;
        case 0x58: inst.LD(PtrE(), GetB()); break;
        case 0x59: inst.LD(PtrE(), GetC()); break;
        case 0x5A: inst.LD(PtrE(), GetD()); break;
        case 0x5B: inst.LD(PtrE(), GetE()); break;
        case 0x5C: inst.LD(PtrE(), GetIXh()); break;
        case 0x5D: inst.LD(PtrE(), GetIXl()); break;
        case 0x5E: inst.LD(PtrE(), MemR(GetIXPlusD(2))); break;
        case 0x5F: inst.LD(PtrE(), GetA()); break;
            
        case 0x60: inst.LD(PtrIXh(), GetB()); break;
        case 0x61: inst.LD(PtrIXh(), GetC()); break;
        case 0x62: inst.LD(PtrIXh(), GetD()); break;
        case 0x63: inst.LD(PtrIXh(), GetE()); break;
        case 0x64: inst.LD(PtrIXh(), GetIXh()); break;
        case 0x65: inst.LD(PtrIXh(), GetIXl()); break;
        case 0x66: inst.LD(PtrH(), MemR(GetIXPlusD(2))); break;
        case 0x67: inst.LD(PtrIXh(), GetA()); break;
        case 0x68: inst.LD(PtrIXl(), GetB()); break;
        case 0x69: inst.LD(PtrIXl(), GetC()); break;
        case 0x6A: inst.LD(PtrIXl(), GetD()); break;
        case 0x6B: inst.LD(PtrIXl(), GetE()); break;
        case 0x6C: inst.LD(PtrIXl(), GetIXh()); break;
        case 0x6D: inst.LD(PtrIXl(), GetIXl()); break;
        case 0x6E: inst.LD(PtrL(), MemR(GetIXPlusD(2))); break;
        case 0x6F: inst.LD(PtrIXl(), GetA()); break;
        
        case 0x70: inst.LD_Mem(GetIXPlusD(2), GetB()); break;
        case 0x71: inst.LD_Mem(GetIXPlusD(2), GetC()); break;
        case 0x72: inst.LD_Mem(GetIXPlusD(2), GetD()); break;
        case 0x73: inst.LD_Mem(GetIXPlusD(2), GetE()); break;
        case 0x74: inst.LD_Mem(GetIXPlusD(2), GetH()); break;
        case 0x75: inst.LD_Mem(GetIXPlusD(2), GetL()); break;
        case 0x77: inst.LD_Mem(GetIXPlusD(2), GetA()); break;
        case 0x78: inst.LD(PtrA(), GetB()); break;
        case 0x79: inst.LD(PtrA(), GetC()); break;
        case 0x7A: inst.LD(PtrA(), GetD()); break;
        case 0x7B: inst.LD(PtrA(), GetE()); break;
        case 0x7C: inst.LD(PtrA(), GetIXh()); break;
        case 0x7D: inst.LD(PtrA(), GetIXl()); break;
        case 0x7E: inst.LD(PtrA(), MemR(GetIXPlusD(2))); break;
        case 0x7F: inst.LD(PtrA(), GetA()); break;
        
        case 0x84: inst.ADD(GetIXh());  break;
        case 0x85: inst.ADD(GetIXl());  break;
        case 0x86: inst.ADD(MemR(GetIXPlusD(2)));  break;
        case 0x8C: inst.ADC(GetIXh());  break;
        case 0x8D: inst.ADC(GetIXl());  break;
        case 0x8E: inst.ADC(MemR(GetIXPlusD(2)));  break;
            
        case 0x94: inst.SUB(GetIXh()); break;
        case 0x95: inst.SUB(GetIXl()); break;
        case 0x96: inst.SUB(MemR(GetIXPlusD(2))); break;
        case 0x9C: inst.SBC(GetIXh()); break;
        case 0x9D: inst.SBC(GetIXl()); break;
        case 0x9E: inst.SBC(MemR(GetIXPlusD(2))); break;
            
        case 0xA4: inst.AND(GetIXh());  break;
        case 0xA5: inst.AND(GetIXl());  break;
        case 0xA6: inst.AND(MemR(GetIXPlusD(2)));  break;
        case 0xAC: inst.XOR(GetIXh());  break;
        case 0xAD: inst.XOR(GetIXl());  break;
        case 0xAE: inst.XOR(MemR(GetIXPlusD(2)));  break;
            
        case 0xB4: inst.OR(GetIXh());  break;
        case 0xB5: inst.OR(GetIXl());  break;
        case 0xB6: inst.OR(MemR(GetIXPlusD(2)));  break;
        case 0xBC: inst.CP(GetIXh());  break;
        case 0xBD: inst.CP(GetIXl());  break;
        case 0xBE: inst.CP(MemR(GetIXPlusD(2)));  break;
            
        case 0xCB: OpcodeDDCB(inst, executed);  break;
            
        case 0xE1: inst.POP(PtrIX());  break;
        case 0xE3: inst.EX_cSP(PtrIX());  break;
        case 0xE5: inst.PUSH(GetIX());  break;
        case 0xE9: inst.JP(GetIX()); break;
            
        case 0xF9: inst.LD(PtrSP(), GetIX()); break;
            
        default:
            stringstream out;
			out << "Error, instruction not implemented: 0xDD";
			out << setfill('0') << setw(2) << uppercase << hex << (int)opcode << "\n";
            cout << out.str();
			throw Exception(out.str().data());
    }
    
    if (!executed) {
        if (GetIncPC())
            AddPC(GetInstructionDDLength(opcode));
        
        m_lastCycles += GetInstructionCyclesXD(opcode);
        executed = true;
    }
}

void CPU::OpcodeED(Instructions &inst, bool &executed)
{
    u8 opcode = MemR(GetPC() + 1);
    AddR(1);
    
    switch (opcode)
    {
		case (0x40): inst.IN(PtrB()); break;
		case (0x41): inst.OUT(GetC(), GetB()); break;
		case (0x42): inst.SBC(GetBC()); break;
		case (0x43): inst.LD_Mem(Get16BitsInmValue(2), GetBC()); break;
        case (0x44): inst.NEG(); break;
        case (0x45): inst.RETN(); break;
		case (0x46): inst.IM(0); break;
        case (0x47): inst.LD(PtrI(), GetA()); break;
		case (0x48): inst.IN(PtrC()); break;
		case (0x49): inst.OUT(GetC(), GetC()); break;
        case (0x4A): inst.ADC(GetBC()); break;
        case (0x4B): inst.LD_Content(PtrBC(), Get16BitsInmValue(2)); break;
        case (0x4C): inst.NEG(); break;
		case (0x4D): inst.RETI(); break;
		case (0x4F): inst.LD(PtrR(), GetA()); break;
            
		case (0x50): inst.IN(PtrD()); break;
		case (0x51): inst.OUT(GetC(), GetD()); break;
		case (0x52): inst.SBC(GetDE()); break;
		case (0x53): inst.LD_Mem(Get16BitsInmValue(2), GetDE()); break;
        case (0x54): inst.NEG(); break;
        case (0x55): inst.RETN(); break;
		case (0x56): inst.IM(1); break;
        case (0x57): inst.LD_A_I(); break;
		case (0x58): inst.IN(PtrE()); break;
		case (0x59): inst.OUT(GetC(), GetE()); break;
        case (0x5A): inst.ADC(GetDE()); break;
        case (0x5B): inst.LD_Content(PtrDE(), Get16BitsInmValue(2)); break;
        case (0x5C): inst.NEG(); break;
        case (0x5D): inst.RETN(); break;
		case (0x5E): inst.IM(2); break;
		case (0x5F): inst.LD_A_R(); break;
            
		case (0x60): inst.IN(PtrH()); break;
		case (0x61): inst.OUT(GetC(), GetH()); break;
		case (0x62): inst.SBC(GetHL()); break;
		case (0x63): inst.LD_Mem(Get16BitsInmValue(2), GetHL()); break;
        case (0x64): inst.NEG(); break;
        case (0x65): inst.RETN(); break;
        case (0x66): inst.IM(0); break;
        case (0x67): inst.RRD(); break;
		case (0x68): inst.IN(PtrL()); break;
		case (0x69): inst.OUT(GetC(), GetL()); break;
        case (0x6A): inst.ADC(GetHL()); break;
        case (0x6B): inst.LD_Content(PtrHL(), Get16BitsInmValue(2)); break;
        case (0x6C): inst.NEG(); break;
        case (0x6D): inst.RETN(); break;
        case (0x6F): inst.RLD(); break;
        
        case (0x70): inst.IN_OnlyFlags();
        case (0x71): inst.OUT(GetC(), 0); break;
		case (0x72): inst.SBC(GetSP()); break;
		case (0x73): inst.LD_Mem(Get16BitsInmValue(2), GetSP()); break;
        case (0x74): inst.NEG(); break;
        case (0x75): inst.RETN(); break;
        case (0x76): inst.IM(1); break;
        case (0x78): inst.IN(PtrA()); break;
		case (0x79): inst.OUT(GetC(), GetA()); break;
        case (0x7A): inst.ADC(GetSP()); break;
        case (0x7B): inst.LD_Content(PtrSP(), Get16BitsInmValue(2)); break;
        case (0x7C): inst.NEG(); break;
        case (0x7D): inst.RETN(); break;
        
        case (0xA0): inst.LDI(); break;
        case (0xA1): inst.CPI(); break;
        case (0xA2): inst.INI(); break;
        case (0xA3): inst.OUTI(); break;
        case (0xA8): inst.LDD(); break;
        case (0xA9): inst.CPD(); break;
        case (0xAB): inst.OUTD(); break;
            
        case (0xB0): inst.LDIR(); break;
        case (0xB1): inst.CPIR(); break;
        case (0xB2): inst.INIR(); break;
        case (0xB3): inst.OTIR(); break;
        case (0xB8): inst.LDDR(); break;
        case (0xB9): inst.CPDR(); break;
        case (0xBB): inst.OTDR(); break;
            
        default:
			stringstream out;
			out << "Error, instruction not implemented: 0xED";
			out << setfill('0') << setw(2) << uppercase << hex << (int)opcode << "\n";
            cout << out.str();
			throw Exception(out.str().data());
    }
    
    if (!executed) {
        if (GetIncPC())
            AddPC(GetInstructionEDLength(opcode));
        
        if (GetConditionalTaken())
            m_lastCycles += GetInstructionCondicionalCyclesED(opcode);
        else
            m_lastCycles += GetInstructionCyclesED(opcode);
        
        executed = true;
    }
}

void CPU::OpcodeFD(Instructions &inst, bool &executed)
{
    u8 opcode = MemR(GetPC() + 1);
    AddR(1);
    
    switch (opcode)
    {
        case 0x09: inst.ADD(PtrIY(), GetBC()); break;
            
        case 0x19: inst.ADD(PtrIY(), GetDE()); break;
            
        case 0x21: inst.LD(PtrIY(), Get16BitsInmValue(2)); break;
        case 0x22: inst.LD_Mem(Get16BitsInmValue(2), GetIY()); break;
        case 0x23: inst.INC_NoFlags(PtrIY()); break;
        case 0x24: inst.INC(PtrIYh()); break;
        case 0x25: inst.DEC(PtrIYh()); break;
        case 0x26: inst.LD(PtrIYh(), Get8BitsInmValue(2)); break;
        case 0x29: inst.ADD(PtrIY(), GetIY()); break;
        case 0x2A: inst.LD_Content(PtrIY(), Get16BitsInmValue(2)); break;
        case 0x2B: inst.DEC_NoFlags(PtrIY()); break;
        case 0x2C: inst.INC(PtrIYl()); break;
        case 0x2D: inst.DEC(PtrIYl()); break;
        case 0x2E: inst.LD(PtrIYl(), Get8BitsInmValue(2)); break;
            
        case 0x34: inst.INC_Mem(GetIYPlusD(2)); break;
        case 0x35: inst.DEC_Mem(GetIYPlusD(2)); break;
        case 0x36: inst.LD_Mem(GetIYPlusD(2), Get8BitsInmValue(3)); break;
        case 0x39: inst.ADD(PtrIY(), GetSP()); break;
        
        case 0x40: inst.LD(PtrB(), GetB()); break;
        case 0x41: inst.LD(PtrB(), GetC()); break;
        case 0x42: inst.LD(PtrB(), GetD()); break;
        case 0x43: inst.LD(PtrB(), GetE()); break;
        case 0x44: inst.LD(PtrB(), GetIYh()); break;
        case 0x45: inst.LD(PtrB(), GetIYl()); break;
        case 0x46: inst.LD(PtrB(), MemR(GetIYPlusD(2))); break;
        case 0x47: inst.LD(PtrB(), GetA()); break;
        case 0x48: inst.LD(PtrC(), GetB()); break;
        case 0x49: inst.LD(PtrC(), GetC()); break;
        case 0x4A: inst.LD(PtrC(), GetD()); break;
        case 0x4B: inst.LD(PtrC(), GetE()); break;
        case 0x4C: inst.LD(PtrC(), GetIYh()); break;
        case 0x4D: inst.LD(PtrC(), GetIYl()); break;
        case 0x4E: inst.LD(PtrC(), MemR(GetIYPlusD(2))); break;
        case 0x4F: inst.LD(PtrC(), GetA()); break;
            
        case 0x50: inst.LD(PtrD(), GetB()); break;
        case 0x51: inst.LD(PtrD(), GetC()); break;
        case 0x52: inst.LD(PtrD(), GetD()); break;
        case 0x53: inst.LD(PtrD(), GetE()); break;
        case 0x54: inst.LD(PtrD(), GetIYh()); break;
        case 0x55: inst.LD(PtrD(), GetIYl()); break;
        case 0x56: inst.LD(PtrD(), MemR(GetIYPlusD(2))); break;
        case 0x57: inst.LD(PtrD(), GetA()); break;
        case 0x58: inst.LD(PtrE(), GetB()); break;
        case 0x59: inst.LD(PtrE(), GetC()); break;
        case 0x5A: inst.LD(PtrE(), GetD()); break;
        case 0x5B: inst.LD(PtrE(), GetE()); break;
        case 0x5C: inst.LD(PtrE(), GetIYh()); break;
        case 0x5D: inst.LD(PtrE(), GetIYl()); break;
        case 0x5E: inst.LD(PtrE(), MemR(GetIYPlusD(2))); break;
        case 0x5F: inst.LD(PtrE(), GetA()); break;
            
        case 0x60: inst.LD(PtrIYh(), GetB()); break;
        case 0x61: inst.LD(PtrIYh(), GetC()); break;
        case 0x62: inst.LD(PtrIYh(), GetD()); break;
        case 0x63: inst.LD(PtrIYh(), GetE()); break;
        case 0x64: inst.LD(PtrIYh(), GetIYh()); break;
        case 0x65: inst.LD(PtrIYh(), GetIYl()); break;
        case 0x66: inst.LD(PtrH(), MemR(GetIYPlusD(2))); break;
        case 0x67: inst.LD(PtrIYh(), GetA()); break;
        case 0x68: inst.LD(PtrIYl(), GetB()); break;
        case 0x69: inst.LD(PtrIYl(), GetC()); break;
        case 0x6A: inst.LD(PtrIYl(), GetD()); break;
        case 0x6B: inst.LD(PtrIYl(), GetE()); break;
        case 0x6C: inst.LD(PtrIYl(), GetIYh()); break;
        case 0x6D: inst.LD(PtrIYl(), GetIYl()); break;
        case 0x6E: inst.LD(PtrL(), MemR(GetIYPlusD(2))); break;
        case 0x6F: inst.LD(PtrIYl(), GetA()); break;
            
        case 0x70: inst.LD_Mem(GetIYPlusD(2), GetB()); break;
        case 0x71: inst.LD_Mem(GetIYPlusD(2), GetC()); break;
        case 0x72: inst.LD_Mem(GetIYPlusD(2), GetD()); break;
        case 0x73: inst.LD_Mem(GetIYPlusD(2), GetE()); break;
        case 0x74: inst.LD_Mem(GetIYPlusD(2), GetH()); break;
        case 0x75: inst.LD_Mem(GetIYPlusD(2), GetL()); break;
        case 0x77: inst.LD_Mem(GetIYPlusD(2), GetA()); break;
        case 0x78: inst.LD(PtrA(), GetB()); break;
        case 0x79: inst.LD(PtrA(), GetC()); break;
        case 0x7A: inst.LD(PtrA(), GetD()); break;
        case 0x7B: inst.LD(PtrA(), GetE()); break;
        case 0x7C: inst.LD(PtrA(), GetIYh()); break;
        case 0x7D: inst.LD(PtrA(), GetIYl()); break;
        case 0x7E: inst.LD(PtrA(), MemR(GetIYPlusD(2))); break;
        case 0x7F: inst.LD(PtrA(), GetA()); break;
          
        case 0x84: inst.ADD(GetIYh());  break;
        case 0x85: inst.ADD(GetIYl());  break;
        case 0x86: inst.ADD(MemR(GetIYPlusD(2)));  break;
        case 0x8C: inst.ADC(GetIYh());  break;
        case 0x8D: inst.ADC(GetIYl());  break;
        case 0x8E: inst.ADC(MemR(GetIYPlusD(2)));  break;
            
        case 0x94: inst.SUB(GetIYh()); break;
        case 0x95: inst.SUB(GetIYl()); break;
        case 0x96: inst.SUB(MemR(GetIYPlusD(2))); break;
        case 0x9C: inst.SBC(GetIYh()); break;
        case 0x9D: inst.SBC(GetIYl()); break;
        case 0x9E: inst.SBC(MemR(GetIYPlusD(2))); break;
          
        case 0xA4: inst.AND(GetIYh());  break;
        case 0xA5: inst.AND(GetIYl());  break;
        case 0xA6: inst.AND(MemR(GetIYPlusD(2)));  break;
        case 0xAC: inst.XOR(GetIYh());  break;
        case 0xAD: inst.XOR(GetIYl());  break;
        case 0xAE: inst.XOR(MemR(GetIYPlusD(2)));  break;
            
        case 0xB4: inst.OR(GetIYh());  break;
        case 0xB5: inst.OR(GetIYl());  break;
        case 0xB6: inst.OR(MemR(GetIYPlusD(2)));  break;
        case 0xBC: inst.CP(GetIYh());  break;
        case 0xBD: inst.CP(GetIYl());  break;
        case 0xBE: inst.CP(MemR(GetIYPlusD(2)));  break;
            
        case 0xCB: OpcodeFDCB(inst, executed);  break;
            
        case 0xE1: inst.POP(PtrIY());  break;
        case 0xE3: inst.EX_cSP(PtrIY());  break;
        case 0xE5: inst.PUSH(GetIY()); break;
        case 0xE9: inst.JP(GetIY()); break;
            
        case 0xF9: inst.LD(PtrSP(), GetIY()); break;
            
        default:
			stringstream out;
			out << "Error, instruction not implemented: 0xFD";
			out << setfill('0') << setw(2) << uppercase << hex << (int)opcode << "\n";
            cout << out.str();
			throw Exception(out.str().data());
    }
    
    if (!executed) {
        if (GetIncPC())
            AddPC(GetInstructionFDLength(opcode));
        
        m_lastCycles += GetInstructionCyclesXD(opcode);
        
        executed = true;
    }
}

void CPU::OpcodeDDCB(Instructions &inst, bool &executed)
{
    u8 opcode = MemR(GetPC() + 3);
    AddR(1);
    
    switch (opcode)
    {
        case 0x00: inst.RLC_Mem(PtrB(), GetIXPlusD(2)); break;
        case 0x01: inst.RLC_Mem(PtrC(), GetIXPlusD(2)); break;
        case 0x02: inst.RLC_Mem(PtrD(), GetIXPlusD(2)); break;
        case 0x03: inst.RLC_Mem(PtrE(), GetIXPlusD(2)); break;
        case 0x04: inst.RLC_Mem(PtrH(), GetIXPlusD(2)); break;
        case 0x05: inst.RLC_Mem(PtrL(), GetIXPlusD(2)); break;
        case 0x06: inst.RLC_Mem(GetIXPlusD(2)); break;
        case 0x07: inst.RLC_Mem(PtrA(), GetIXPlusD(2)); break;
        case 0x08: inst.RRC_Mem(PtrB(), GetIXPlusD(2)); break;
        case 0x09: inst.RRC_Mem(PtrC(), GetIXPlusD(2)); break;
        case 0x0A: inst.RRC_Mem(PtrD(), GetIXPlusD(2)); break;
        case 0x0B: inst.RRC_Mem(PtrE(), GetIXPlusD(2)); break;
        case 0x0C: inst.RRC_Mem(PtrH(), GetIXPlusD(2)); break;
        case 0x0D: inst.RRC_Mem(PtrL(), GetIXPlusD(2)); break;
        case 0x0E: inst.RRC_Mem(GetIXPlusD(2)); break;
        case 0x0F: inst.RRC_Mem(PtrA(), GetIXPlusD(2)); break;
            
        case 0x10: inst.RL_Mem(PtrB(), GetIXPlusD(2)); break;
        case 0x11: inst.RL_Mem(PtrC(), GetIXPlusD(2)); break;
        case 0x12: inst.RL_Mem(PtrD(), GetIXPlusD(2)); break;
        case 0x13: inst.RL_Mem(PtrE(), GetIXPlusD(2)); break;
        case 0x14: inst.RL_Mem(PtrH(), GetIXPlusD(2)); break;
        case 0x15: inst.RL_Mem(PtrL(), GetIXPlusD(2)); break;
        case 0x16: inst.RL_Mem(GetIXPlusD(2)); break;
        case 0x17: inst.RL_Mem(PtrA(), GetIXPlusD(2)); break;
        case 0x18: inst.RR_Mem(PtrB(), GetIXPlusD(2)); break;
        case 0x19: inst.RR_Mem(PtrC(), GetIXPlusD(2)); break;
        case 0x1A: inst.RR_Mem(PtrD(), GetIXPlusD(2)); break;
        case 0x1B: inst.RR_Mem(PtrE(), GetIXPlusD(2)); break;
        case 0x1C: inst.RR_Mem(PtrH(), GetIXPlusD(2)); break;
        case 0x1D: inst.RR_Mem(PtrL(), GetIXPlusD(2)); break;
        case 0x1E: inst.RR_Mem(GetIXPlusD(2)); break;
        case 0x1F: inst.RR_Mem(PtrA(), GetIXPlusD(2)); break;
            
        case 0x20: inst.SLA_Mem(PtrB(), GetIXPlusD(2)); break;
        case 0x21: inst.SLA_Mem(PtrC(), GetIXPlusD(2)); break;
        case 0x22: inst.SLA_Mem(PtrD(), GetIXPlusD(2)); break;
        case 0x23: inst.SLA_Mem(PtrE(), GetIXPlusD(2)); break;
        case 0x24: inst.SLA_Mem(PtrH(), GetIXPlusD(2)); break;
        case 0x25: inst.SLA_Mem(PtrL(), GetIXPlusD(2)); break;
        case 0x26: inst.SLA_Mem(GetIXPlusD(2)); break;
        case 0x27: inst.SLA_Mem(PtrA(), GetIXPlusD(2)); break;
        case 0x28: inst.SRA_Mem(PtrB(), GetIXPlusD(2)); break;
        case 0x29: inst.SRA_Mem(PtrC(), GetIXPlusD(2)); break;
        case 0x2A: inst.SRA_Mem(PtrD(), GetIXPlusD(2)); break;
        case 0x2B: inst.SRA_Mem(PtrE(), GetIXPlusD(2)); break;
        case 0x2C: inst.SRA_Mem(PtrH(), GetIXPlusD(2)); break;
        case 0x2D: inst.SRA_Mem(PtrL(), GetIXPlusD(2)); break;
        case 0x2E: inst.SRA_Mem(GetIXPlusD(2)); break;
        case 0x2F: inst.SRA_Mem(PtrA(), GetIXPlusD(2)); break;
            
        case 0x30: inst.SLL_Mem(PtrB(), GetIXPlusD(2)); break;
        case 0x31: inst.SLL_Mem(PtrC(), GetIXPlusD(2)); break;
        case 0x32: inst.SLL_Mem(PtrD(), GetIXPlusD(2)); break;
        case 0x33: inst.SLL_Mem(PtrE(), GetIXPlusD(2)); break;
        case 0x34: inst.SLL_Mem(PtrH(), GetIXPlusD(2)); break;
        case 0x35: inst.SLL_Mem(PtrL(), GetIXPlusD(2)); break;
        case 0x36: inst.SLL_Mem(GetIXPlusD(2)); break;
        case 0x37: inst.SLL_Mem(PtrA(), GetIXPlusD(2)); break;
        case 0x38: inst.SRL_Mem(PtrB(), GetIXPlusD(2)); break;
        case 0x39: inst.SRL_Mem(PtrC(), GetIXPlusD(2)); break;
        case 0x3A: inst.SRL_Mem(PtrD(), GetIXPlusD(2)); break;
        case 0x3B: inst.SRL_Mem(PtrE(), GetIXPlusD(2)); break;
        case 0x3C: inst.SRL_Mem(PtrH(), GetIXPlusD(2)); break;
        case 0x3D: inst.SRL_Mem(PtrL(), GetIXPlusD(2)); break;
        case 0x3E: inst.SRL_Mem(GetIXPlusD(2)); break;
        case 0x3F: inst.SRL_Mem(PtrA(), GetIXPlusD(2)); break;
            
        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x46:
        case 0x47: inst.BIT(0, GetIXPlusD(2)); break;
        case 0x48:
        case 0x49:
        case 0x4A:
        case 0x4B:
        case 0x4C:
        case 0x4D:
        case 0x4E:
        case 0x4F: inst.BIT(1, GetIXPlusD(2)); break;
            
        case 0x50:
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0x56:
        case 0x57: inst.BIT(2, GetIXPlusD(2)); break;
        case 0x58:
        case 0x59:
        case 0x5A:
        case 0x5B:
        case 0x5C:
        case 0x5D:
        case 0x5E:
        case 0x5F: inst.BIT(3, GetIXPlusD(2)); break;
            
        case 0x60:
        case 0x61:
        case 0x62:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
        case 0x67: inst.BIT(4, GetIXPlusD(2)); break;
        case 0x68:
        case 0x69:
        case 0x6A:
        case 0x6B:
        case 0x6C:
        case 0x6D:
        case 0x6E:
        case 0x6F: inst.BIT(5, GetIXPlusD(2)); break;
            
        case 0x70:
        case 0x71:
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x76:
        case 0x77: inst.BIT(6, GetIXPlusD(2)); break;
        case 0x78:
        case 0x79:
        case 0x7A:
        case 0x7B:
        case 0x7C:
        case 0x7D:
        case 0x7E:
        case 0x7F: inst.BIT(7, GetIXPlusD(2)); break;
            
        case 0x80:
        case 0x81:
        case 0x82:
        case 0x83:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0x87: inst.RES_Mem(0, GetIXPlusD(2)); break;
        case 0x88:
        case 0x89:
        case 0x8A:
        case 0x8B:
        case 0x8C:
        case 0x8D:
        case 0x8E:
        case 0x8F: inst.RES_Mem(1, GetIXPlusD(2)); break;
            
        case 0x90:
        case 0x91:
        case 0x92:
        case 0x93:
        case 0x94:
        case 0x95:
        case 0x96:
        case 0x97: inst.RES_Mem(2, GetIXPlusD(2)); break;
        case 0x98:
        case 0x99:
        case 0x9A:
        case 0x9B:
        case 0x9C:
        case 0x9D:
        case 0x9E:
        case 0x9F: inst.RES_Mem(3, GetIXPlusD(2)); break;
            
        case 0xA0:
        case 0xA1:
        case 0xA2:
        case 0xA3:
        case 0xA4:
        case 0xA5:
        case 0xA6:
        case 0xA7: inst.RES_Mem(4, GetIXPlusD(2)); break;
        case 0xA8:
        case 0xA9:
        case 0xAA:
        case 0xAB:
        case 0xAC:
        case 0xAD:
        case 0xAE:
        case 0xAF: inst.RES_Mem(5, GetIXPlusD(2)); break;
            
        case 0xB0:
        case 0xB1:
        case 0xB2:
        case 0xB3:
        case 0xB4:
        case 0xB5:
        case 0xB6:
        case 0xB7: inst.RES_Mem(6, GetIXPlusD(2)); break;
        case 0xB8:
        case 0xB9:
        case 0xBA:
        case 0xBB:
        case 0xBC:
        case 0xBD:
        case 0xBE:
        case 0xBF: inst.RES_Mem(7, GetIXPlusD(2)); break;
            
        case 0xC0:
        case 0xC1:
        case 0xC2:
        case 0xC3:
        case 0xC4:
        case 0xC5:
        case 0xC6:
        case 0xC7: inst.SET_Mem(0, GetIXPlusD(2)); break;
        case 0xC8:
        case 0xC9:
        case 0xCA:
        case 0xCB:
        case 0xCC:
        case 0xCD:
        case 0xCE:
        case 0xCF: inst.SET_Mem(1, GetIXPlusD(2)); break;
            
        case 0xD0:
        case 0xD1:
        case 0xD2:
        case 0xD3:
        case 0xD4:
        case 0xD5:
        case 0xD6:
        case 0xD7: inst.SET_Mem(2, GetIXPlusD(2)); break;
        case 0xD8:
        case 0xD9:
        case 0xDA:
        case 0xDB:
        case 0xDC:
        case 0xDD:
        case 0xDE:
        case 0xDF: inst.SET_Mem(3, GetIXPlusD(2)); break;
            
        case 0xE0:
        case 0xE1:
        case 0xE2:
        case 0xE3:
        case 0xE4:
        case 0xE5:
        case 0xE6:
        case 0xE7: inst.SET_Mem(4, GetIXPlusD(2)); break;
        case 0xE8:
        case 0xE9:
        case 0xEA:
        case 0xEB:
        case 0xEC:
        case 0xED:
        case 0xEE:
        case 0xEF: inst.SET_Mem(5, GetIXPlusD(2)); break;
            
        case 0xF0:
        case 0xF1:
        case 0xF2:
        case 0xF3:
        case 0xF4:
        case 0xF5:
        case 0xF6:
        case 0xF7: inst.SET_Mem(6, GetIXPlusD(2)); break;
        case 0xF8:
        case 0xF9:
        case 0xFA:
        case 0xFB:
        case 0xFC:
        case 0xFD:
        case 0xFE:
        case 0xFF: inst.SET_Mem(7, GetIXPlusD(2)); break;
            
        default:
			stringstream out;
			out << "Error, instruction not implemented: 0xDDCB";
			out << setfill('0') << setw(2) << uppercase << hex << (int)opcode << "\n";
            cout << out.str();
			throw Exception(out.str().data());
    }
    
    if (!executed) {
        AddPC(GetInstructionDDCBLength(opcode));
        m_lastCycles += GetInstructionCyclesXDCB(opcode);
        executed = true;
    }
}

void CPU::OpcodeFDCB(Instructions &inst, bool &executed)
{
    u8 opcode = MemR(GetPC() + 3);
    AddR(1);
    
    switch (opcode)
    {
        case 0x00: inst.RLC_Mem(PtrB(), GetIYPlusD(2)); break;
        case 0x01: inst.RLC_Mem(PtrC(), GetIYPlusD(2)); break;
        case 0x02: inst.RLC_Mem(PtrD(), GetIYPlusD(2)); break;
        case 0x03: inst.RLC_Mem(PtrE(), GetIYPlusD(2)); break;
        case 0x04: inst.RLC_Mem(PtrH(), GetIYPlusD(2)); break;
        case 0x05: inst.RLC_Mem(PtrL(), GetIYPlusD(2)); break;
        case 0x06: inst.RLC_Mem(GetIYPlusD(2)); break;
        case 0x07: inst.RLC_Mem(PtrA(), GetIYPlusD(2)); break;
        case 0x08: inst.RRC_Mem(PtrB(), GetIYPlusD(2)); break;
        case 0x09: inst.RRC_Mem(PtrC(), GetIYPlusD(2)); break;
        case 0x0A: inst.RRC_Mem(PtrD(), GetIYPlusD(2)); break;
        case 0x0B: inst.RRC_Mem(PtrE(), GetIYPlusD(2)); break;
        case 0x0C: inst.RRC_Mem(PtrH(), GetIYPlusD(2)); break;
        case 0x0D: inst.RRC_Mem(PtrL(), GetIYPlusD(2)); break;
        case 0x0E: inst.RRC_Mem(GetIYPlusD(2)); break;
        case 0x0F: inst.RRC_Mem(PtrA(), GetIYPlusD(2)); break;
            
        case 0x10: inst.RL_Mem(PtrB(), GetIYPlusD(2)); break;
        case 0x11: inst.RL_Mem(PtrC(), GetIYPlusD(2)); break;
        case 0x12: inst.RL_Mem(PtrD(), GetIYPlusD(2)); break;
        case 0x13: inst.RL_Mem(PtrE(), GetIYPlusD(2)); break;
        case 0x14: inst.RL_Mem(PtrH(), GetIYPlusD(2)); break;
        case 0x15: inst.RL_Mem(PtrL(), GetIYPlusD(2)); break;
        case 0x16: inst.RL_Mem(GetIYPlusD(2)); break;
        case 0x17: inst.RL_Mem(PtrA(), GetIYPlusD(2)); break;
        case 0x18: inst.RR_Mem(PtrB(), GetIYPlusD(2)); break;
        case 0x19: inst.RR_Mem(PtrC(), GetIYPlusD(2)); break;
        case 0x1A: inst.RR_Mem(PtrD(), GetIYPlusD(2)); break;
        case 0x1B: inst.RR_Mem(PtrE(), GetIYPlusD(2)); break;
        case 0x1C: inst.RR_Mem(PtrH(), GetIYPlusD(2)); break;
        case 0x1D: inst.RR_Mem(PtrL(), GetIYPlusD(2)); break;
        case 0x1E: inst.RR_Mem(GetIYPlusD(2)); break;
        case 0x1F: inst.RR_Mem(PtrA(), GetIYPlusD(2)); break;
            
        case 0x20: inst.SLA_Mem(PtrB(), GetIYPlusD(2)); break;
        case 0x21: inst.SLA_Mem(PtrC(), GetIYPlusD(2)); break;
        case 0x22: inst.SLA_Mem(PtrD(), GetIYPlusD(2)); break;
        case 0x23: inst.SLA_Mem(PtrE(), GetIYPlusD(2)); break;
        case 0x24: inst.SLA_Mem(PtrH(), GetIYPlusD(2)); break;
        case 0x25: inst.SLA_Mem(PtrL(), GetIYPlusD(2)); break;
        case 0x26: inst.SLA_Mem(GetIYPlusD(2)); break;
        case 0x27: inst.SLA_Mem(PtrA(), GetIYPlusD(2)); break;
        case 0x28: inst.SRA_Mem(PtrB(), GetIYPlusD(2)); break;
        case 0x29: inst.SRA_Mem(PtrC(), GetIYPlusD(2)); break;
        case 0x2A: inst.SRA_Mem(PtrD(), GetIYPlusD(2)); break;
        case 0x2B: inst.SRA_Mem(PtrE(), GetIYPlusD(2)); break;
        case 0x2C: inst.SRA_Mem(PtrH(), GetIYPlusD(2)); break;
        case 0x2D: inst.SRA_Mem(PtrL(), GetIYPlusD(2)); break;
        case 0x2E: inst.SRA_Mem(GetIYPlusD(2)); break;
        case 0x2F: inst.SRA_Mem(PtrA(), GetIYPlusD(2)); break;
            
        case 0x30: inst.SLL_Mem(PtrB(), GetIYPlusD(2)); break;
        case 0x31: inst.SLL_Mem(PtrC(), GetIYPlusD(2)); break;
        case 0x32: inst.SLL_Mem(PtrD(), GetIYPlusD(2)); break;
        case 0x33: inst.SLL_Mem(PtrE(), GetIYPlusD(2)); break;
        case 0x34: inst.SLL_Mem(PtrH(), GetIYPlusD(2)); break;
        case 0x35: inst.SLL_Mem(PtrL(), GetIYPlusD(2)); break;
        case 0x36: inst.SLL_Mem(GetIYPlusD(2)); break;
        case 0x37: inst.SLL_Mem(PtrA(), GetIYPlusD(2)); break;
        case 0x38: inst.SRL_Mem(PtrB(), GetIYPlusD(2)); break;
        case 0x39: inst.SRL_Mem(PtrC(), GetIYPlusD(2)); break;
        case 0x3A: inst.SRL_Mem(PtrD(), GetIYPlusD(2)); break;
        case 0x3B: inst.SRL_Mem(PtrE(), GetIYPlusD(2)); break;
        case 0x3C: inst.SRL_Mem(PtrH(), GetIYPlusD(2)); break;
        case 0x3D: inst.SRL_Mem(PtrL(), GetIXPlusD(2)); break;
        case 0x3E: inst.SRL_Mem(GetIYPlusD(2)); break;
        case 0x3F: inst.SRL_Mem(PtrA(), GetIYPlusD(2)); break;
            
        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x46:
        case 0x47: inst.BIT(0, GetIYPlusD(2)); break;
        case 0x48:
        case 0x49:
        case 0x4A:
        case 0x4B:
        case 0x4C:
        case 0x4D:
        case 0x4E:
        case 0x4F: inst.BIT(1, GetIYPlusD(2)); break;
            
        case 0x50:
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0x56:
        case 0x57: inst.BIT(2, GetIYPlusD(2)); break;
        case 0x58:
        case 0x59:
        case 0x5A:
        case 0x5B:
        case 0x5C:
        case 0x5D:
        case 0x5E:
        case 0x5F: inst.BIT(3, GetIYPlusD(2)); break;
            
        case 0x60:
        case 0x61:
        case 0x62:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
        case 0x67: inst.BIT(4, GetIYPlusD(2)); break;
        case 0x68:
        case 0x69:
        case 0x6A:
        case 0x6B:
        case 0x6C:
        case 0x6D:
        case 0x6E:
        case 0x6F: inst.BIT(5, GetIYPlusD(2)); break;
            
        case 0x70:
        case 0x71:
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x76:
        case 0x77: inst.BIT(6, GetIYPlusD(2)); break;
        case 0x78:
        case 0x79:
        case 0x7A:
        case 0x7B:
        case 0x7C:
        case 0x7D:
        case 0x7E:
        case 0x7F: inst.BIT(7, GetIYPlusD(2)); break;
            
        case 0x80:
        case 0x81:
        case 0x82:
        case 0x83:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0x87: inst.RES_Mem(0, GetIYPlusD(2)); break;
        case 0x88:
        case 0x89:
        case 0x8A:
        case 0x8B:
        case 0x8C:
        case 0x8D:
        case 0x8E:
        case 0x8F: inst.RES_Mem(1, GetIYPlusD(2)); break;
            
        case 0x90:
        case 0x91:
        case 0x92:
        case 0x93:
        case 0x94:
        case 0x95:
        case 0x96:
        case 0x97: inst.RES_Mem(2, GetIYPlusD(2)); break;
        case 0x98:
        case 0x99:
        case 0x9A:
        case 0x9B:
        case 0x9C:
        case 0x9D:
        case 0x9E:
        case 0x9F: inst.RES_Mem(3, GetIYPlusD(2)); break;
            
        case 0xA0:
        case 0xA1:
        case 0xA2:
        case 0xA3:
        case 0xA4:
        case 0xA5:
        case 0xA6:
        case 0xA7: inst.RES_Mem(4, GetIYPlusD(2)); break;
        case 0xA8:
        case 0xA9:
        case 0xAA:
        case 0xAB:
        case 0xAC:
        case 0xAD:
        case 0xAE:
        case 0xAF: inst.RES_Mem(5, GetIYPlusD(2)); break;
            
        case 0xB0:
        case 0xB1:
        case 0xB2:
        case 0xB3:
        case 0xB4:
        case 0xB5:
        case 0xB6:
        case 0xB7: inst.RES_Mem(6, GetIYPlusD(2)); break;
        case 0xB8:
        case 0xB9:
        case 0xBA:
        case 0xBB:
        case 0xBC:
        case 0xBD:
        case 0xBE:
        case 0xBF: inst.RES_Mem(7, GetIYPlusD(2)); break;
            
        case 0xC0:
        case 0xC1:
        case 0xC2:
        case 0xC3:
        case 0xC4:
        case 0xC5:
        case 0xC6:
        case 0xC7: inst.SET_Mem(0, GetIYPlusD(2)); break;
        case 0xC8:
        case 0xC9:
        case 0xCA:
        case 0xCB:
        case 0xCC:
        case 0xCD:
        case 0xCE:
        case 0xCF: inst.SET_Mem(1, GetIYPlusD(2)); break;
            
        case 0xD0:
        case 0xD1:
        case 0xD2:
        case 0xD3:
        case 0xD4:
        case 0xD5:
        case 0xD6:
        case 0xD7: inst.SET_Mem(2, GetIYPlusD(2)); break;
        case 0xD8:
        case 0xD9:
        case 0xDA:
        case 0xDB:
        case 0xDC:
        case 0xDD:
        case 0xDE:
        case 0xDF: inst.SET_Mem(3, GetIYPlusD(2)); break;
            
        case 0xE0:
        case 0xE1:
        case 0xE2:
        case 0xE3:
        case 0xE4:
        case 0xE5:
        case 0xE6:
        case 0xE7: inst.SET_Mem(4, GetIYPlusD(2)); break;
        case 0xE8:
        case 0xE9:
        case 0xEA:
        case 0xEB:
        case 0xEC:
        case 0xED:
        case 0xEE:
        case 0xEF: inst.SET_Mem(5, GetIYPlusD(2)); break;
            
        case 0xF0:
        case 0xF1:
        case 0xF2:
        case 0xF3:
        case 0xF4:
        case 0xF5:
        case 0xF6:
        case 0xF7: inst.SET_Mem(6, GetIYPlusD(2)); break;
        case 0xF8:
        case 0xF9:
        case 0xFA:
        case 0xFB:
        case 0xFC:
        case 0xFD:
        case 0xFE:
        case 0xFF: inst.SET_Mem(7, GetIYPlusD(2)); break;
            
        default:
			stringstream out;
			out << "Error, instruction not implemented: 0xFDCB";
			out << setfill('0') << setw(2) << uppercase << hex << (int)opcode << "\n";
            cout << out.str();
			throw Exception(out.str().data());
    }
    
    if (!executed) {
        AddPC(GetInstructionFDCBLength(opcode));
        m_lastCycles += GetInstructionCyclesXDCB(opcode);
        executed = true;
    }
}
