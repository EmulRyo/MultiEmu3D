/*
 This file is part of MarkSystem.
 
 DMGBoy is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 DMGBoy is distributed in the hope that it will be useful,
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
#include "InstructionsDef.h"
#include "SMSException.h"

using namespace std;

u8 CPU::Get8BitsInmValue(u8 offset) {
    return (MemR(GetPC() + offset));
}

u16 CPU::Get16BitsInmValue(u8 offset) {
    return ((MemR(GetPC() + offset + 1)) << 8) | MemR(GetPC() + offset);
}

u16 CPU::GetIXPlusD(u8 offset) {
    return GetIX()+Get8BitsInmValue(offset);
}

u16 CPU::GetIYPlusD(u8 offset) {
    return GetIY()+Get8BitsInmValue(offset);
}

void CPU::ExecuteOpcode(u8 opcode, Instructions &inst) {
    bool executed = false;
    SetIncPC(true);
    AddR(1);
    
    switch(opcode)
    {
        case (0x00): inst.NOP(); break;
        case (0x01): inst.LD_n_nn(BC); break;
        case (0x02): inst.LD_n_A(c_BC); break;
        case (0x03): inst.INC_NoFlags(PtrBC()); break;
        case (0x04): inst.INC(PtrB()); break;
        case (0x05): inst.DEC(PtrB()); break;
        case (0x06): inst.LD_nn_n(B); break;
        case (0x07): inst.RLC_n(A); break;
        case (0x08): inst.EX_AF_AF2(); break;
        case (0x09): inst.ADD_HL_n(BC); break;
        case (0x0A): inst.LD_A_n(c_BC); break;
        case (0x0B): inst.DEC_NoFlags(PtrBC()); break;
        case (0x0C): inst.INC(PtrC()); break;
        case (0x0D): inst.DEC(PtrC()); break;
        case (0x0E): inst.LD_nn_n(C); break;
        case (0x0F): inst.RRC_n(A); break;

        case (0x10): inst.DJNZ(); break;
        case (0x11): inst.LD_n_nn(DE); break;
        case (0x12): inst.LD_n_A(c_DE); break;
        case (0x13): inst.INC_NoFlags(PtrDE()); break;
        case (0x14): inst.INC(PtrD()); break;
        case (0x15): inst.DEC(PtrD()); break;
        case (0x16): inst.LD_nn_n(D); break;
        case (0x17): inst.RL_n(A); break;
        case (0x18): inst.JR(); break;
        case (0x19): inst.ADD_HL_n(DE); break;
        case (0x1A): inst.LD_A_n(c_DE); break;
        case (0x1B): inst.DEC_NoFlags(PtrDE()); break;
        case (0x1C): inst.INC(PtrE()); break;
        case (0x1D): inst.DEC(PtrE()); break;
        case (0x1E): inst.LD_nn_n(E); break;
        case (0x1F): inst.RR_n(A); break;

        case (0x20): inst.JR_CC_n(f_Z, 0); break;
        case (0x21): inst.LD_n_nn(HL); break;
        case (0x22): inst.LD_Mem(Get16BitsInmValue(1), GetHL()); break;
        case (0x23): inst.INC_NoFlags(PtrHL()); break;
        case (0x24): inst.INC(PtrH()); break;
        case (0x25): inst.DEC(PtrH()); break;
        case (0x26): inst.LD_nn_n(H); break;
        case (0x27): inst.DAA(); break;
        case (0x28): inst.JR_CC_n(f_Z, 1); break;
        case (0x29): inst.ADD_HL_n(HL); break;
        case (0x2A): inst.LD_HL_cNN(); break;
        case (0x2B): inst.DEC_NoFlags(PtrHL()); break;
        case (0x2C): inst.INC(PtrL()); break;
        case (0x2D): inst.DEC(PtrL()); break;
        case (0x2E): inst.LD_nn_n(L); break;
        case (0x2F): inst.CPL(); break;

        case (0x30): inst.JR_CC_n(f_C, 0); break;
        case (0x31): inst.LD_n_nn(SP); break;
        case (0x32): inst.LD_cNN_n(A); break;
        case (0x33): inst.INC_NoFlags(PtrSP()); break;
        case (0x34): inst.INC_Mem(GetHL()); break;
        case (0x35): inst.DEC_Mem(GetHL()); break;
        case (0x36): inst.LD_r1_r2(c_HL, $); break;
        case (0x37): inst.SCF(); break;
        case (0x38): inst.JR_CC_n(f_C, 1); break;
        case (0x39): inst.ADD_HL_n(SP); break;
        case (0x3A): inst.LD_A_n(c_$$); break;
        case (0x3B): inst.DEC_NoFlags(PtrSP()); break;
        case (0x3C): inst.INC(PtrA()); break;
        case (0x3D): inst.DEC(PtrA()); break;
        case (0x3E): inst.LD_A_n($); break;
        case (0x3F): inst.CCF(); break;

        case (0x40): inst.LD_r1_r2(B, B); break;
        case (0x41): inst.LD_r1_r2(B, C); break;
        case (0x42): inst.LD_r1_r2(B, D); break;
        case (0x43): inst.LD_r1_r2(B, E); break;
        case (0x44): inst.LD_r1_r2(B, H); break;
        case (0x45): inst.LD_r1_r2(B, L); break;
        case (0x46): inst.LD_r1_r2(B, c_HL); break;
        case (0x47): inst.LD_n_A(B); break;
        case (0x48): inst.LD_r1_r2(C, B); break;
        case (0x49): inst.LD_r1_r2(C, C); break;
        case (0x4A): inst.LD_r1_r2(C, D); break;
        case (0x4B): inst.LD_r1_r2(C, E); break;
        case (0x4C): inst.LD_r1_r2(C, H); break;
        case (0x4D): inst.LD_r1_r2(C, L); break;
        case (0x4E): inst.LD_r1_r2(C, c_HL); break;
        case (0x4F): inst.LD_n_A(C); break;

        case (0x50): inst.LD_r1_r2(D, B); break;
        case (0x51): inst.LD_r1_r2(D, C); break;
        case (0x52): inst.LD_r1_r2(D, D); break;
        case (0x53): inst.LD_r1_r2(D, E); break;
        case (0x54): inst.LD_r1_r2(D, H); break;
        case (0x55): inst.LD_r1_r2(D, L); break;
        case (0x56): inst.LD_r1_r2(D, c_HL); break;
        case (0x57): inst.LD_n_A(D); break;
        case (0x58): inst.LD_r1_r2(E, B); break;
        case (0x59): inst.LD_r1_r2(E, C); break;
        case (0x5A): inst.LD_r1_r2(E, D); break;
        case (0x5B): inst.LD_r1_r2(E, E); break;
        case (0x5C): inst.LD_r1_r2(E, H); break;
        case (0x5D): inst.LD_r1_r2(E, L); break;
        case (0x5E): inst.LD_r1_r2(E, c_HL); break;
        case (0x5F): inst.LD_n_A(E); break;

        case (0x60): inst.LD_r1_r2(H, B); break;
        case (0x61): inst.LD_r1_r2(H, C); break;
        case (0x62): inst.LD_r1_r2(H, D); break;
        case (0x63): inst.LD_r1_r2(H, E); break;
        case (0x64): inst.LD_r1_r2(H, H); break;
        case (0x65): inst.LD_r1_r2(H, L); break;
        case (0x66): inst.LD_r1_r2(H, c_HL); break;
        case (0x67): inst.LD_n_A(H); break;
        case (0x68): inst.LD_r1_r2(L, B); break;
        case (0x69): inst.LD_r1_r2(L, C); break;
        case (0x6A): inst.LD_r1_r2(L, D); break;
        case (0x6B): inst.LD_r1_r2(L, E); break;
        case (0x6C): inst.LD_r1_r2(L, H); break;
        case (0x6D): inst.LD_r1_r2(L, L); break;
        case (0x6E): inst.LD_r1_r2(L, c_HL); break;
        case (0x6F): inst.LD_n_A(L); break;

        case (0x70): inst.LD_r1_r2(c_HL, B); break;
        case (0x71): inst.LD_r1_r2(c_HL, C); break;
        case (0x72): inst.LD_r1_r2(c_HL, D); break;
        case (0x73): inst.LD_r1_r2(c_HL, E); break;
        case (0x74): inst.LD_r1_r2(c_HL, H); break;
        case (0x75): inst.LD_r1_r2(c_HL, L); break;
        case (0x76): inst.HALT(); break;
        case (0x77): inst.LD_n_A(c_HL); break;
        case (0x78): inst.LD_A_n(B); break;
        case (0x79): inst.LD_A_n(C); break;
        case (0x7A): inst.LD_A_n(D); break;
        case (0x7B): inst.LD_A_n(E); break;
        case (0x7C): inst.LD_A_n(H); break;
        case (0x7D): inst.LD_A_n(L); break;
        case (0x7E): inst.LD_A_n(c_HL); break;
        case (0x7F): inst.LD_n_A(A); break;

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

        case (0x90): inst.SUB_n(B); break;
        case (0x91): inst.SUB_n(C); break;
        case (0x92): inst.SUB_n(D); break;
        case (0x93): inst.SUB_n(E); break;
        case (0x94): inst.SUB_n(H); break;
        case (0x95): inst.SUB_n(L); break;
        case (0x96): inst.SUB_n(c_HL); break;
        case (0x97): inst.SUB_n(A); break;
        case (0x98): inst.SBC_A(B); break;
        case (0x99): inst.SBC_A(C); break;
        case (0x9A): inst.SBC_A(D); break;
        case (0x9B): inst.SBC_A(E); break;
        case (0x9C): inst.SBC_A(H); break;
        case (0x9D): inst.SBC_A(L); break;
        case (0x9E): inst.SBC_A(c_HL); break;
        case (0x9F): inst.SBC_A(A); break;

        case (0xA0): inst.AND(B); break;
        case (0xA1): inst.AND(C); break;
        case (0xA2): inst.AND(D); break;
        case (0xA3): inst.AND(E); break;
        case (0xA4): inst.AND(H); break;
        case (0xA5): inst.AND(L); break;
        case (0xA6): inst.AND(c_HL); break;
        case (0xA7): inst.AND(A); break;
        case (0xA8): inst.XOR_n(B); break;
        case (0xA9): inst.XOR_n(C); break;
        case (0xAA): inst.XOR_n(D); break;
        case (0xAB): inst.XOR_n(E); break;
        case (0xAC): inst.XOR_n(H); break;
        case (0xAD): inst.XOR_n(L); break;
        case (0xAE): inst.XOR_n(c_HL); break;
        case (0xAF): inst.XOR_n(A); break;

        case (0xB0): inst.OR(GetB()); break;
        case (0xB1): inst.OR(GetC()); break;
        case (0xB2): inst.OR(GetD()); break;
        case (0xB3): inst.OR(GetE()); break;
        case (0xB4): inst.OR(GetH()); break;
        case (0xB5): inst.OR(GetL()); break;
        case (0xB6): inst.OR(MemR(GetHL())); break;
        case (0xB7): inst.OR(GetA()); break;
        case (0xB8): inst.CP_n(B); break;
        case (0xB9): inst.CP_n(C); break;
        case (0xBA): inst.CP_n(D); break;
        case (0xBB): inst.CP_n(E); break;
        case (0xBC): inst.CP_n(H); break;
        case (0xBD): inst.CP_n(L); break;
        case (0xBE): inst.CP_n(c_HL); break;
        case (0xBF): inst.CP_n(A); break;

        case (0xC0): inst.RET_cc(f_Z, 0); break;
        case (0xC1): inst.POP_nn(BC); break;
        case (0xC2): inst.JP_cc_nn(f_Z, 0); break;
        case (0xC3): inst.JP_nn(); break;
        case (0xC4): inst.CALL_cc_nn(f_Z, 0); break;
        case (0xC5): inst.PUSH_nn(BC); break;
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
        case (0xD1): inst.POP_nn(DE); break;
        case (0xD2): inst.JP_cc_nn(f_C, 0); break;
        case (0xD3): inst.OUT($, A); break;
        case (0xD4): inst.CALL_cc_nn(f_C, 0); break;
        case (0xD5): inst.PUSH_nn(DE); break;
        case (0xD6): inst.SUB_n($); break;
        case (0xD7): inst.RST_n(0x10); break;
        case (0xD8): inst.RET_cc(f_C, 1); break;
        case (0xD9): inst.EXX(); break;
        case (0xDA): inst.JP_cc_nn(f_C, 1); break;
        case (0xDB): inst.IN_A_n(); break;
        case (0xDC): inst.CALL_cc_nn(f_C, 1); break;
        case (0xDD): OpcodeDD(inst, executed); break;
        case (0xDE): inst.SBC_A($); break;
        case (0xDF): inst.RST_n(0x18); break;

        case (0xE0): inst.RET_cc(f_PV, 0); break;
        case (0xE1): inst.POP_nn(HL); break;
        case (0xE3): inst.EX_cSP_HL(); break;
        case (0xE5): inst.PUSH_nn(HL); break;
        case (0xE6): inst.AND($); break;
        case (0xE7): inst.RST_n(0x20); break;
        case (0xE8): inst.RET_cc(f_PV, 1); break;
        case (0xE9): inst.JP_HL(); break;
        case (0xEA): inst.JP_cc_nn(f_PV, 1); break;
        case (0xEB): inst.EX_DE_HL(); break;
        case (0xED): OpcodeED(inst, executed); break;; break;
        case (0xEE): inst.XOR_n($); break;
        case (0xEF): inst.RST_n(0x28); break;

        case (0xF0): inst.RET_cc(f_S, 0); break;
        case (0xF1): inst.POP_nn(AF); break;
        case (0xF2): inst.JP_cc_nn(f_S, 0); break;
        case (0xF3): inst.DI(); break;
        case (0xF5): inst.PUSH_nn(AF); break;
        case (0xF6): inst.OR(Get8BitsInmValue(1)); break;
        case (0xF7): inst.RST_n(0x30); break;
        case (0xF8): inst.RET_cc(f_S, 1); break;
        case (0xF9): inst.LD_SP_HL(); break;
        case (0xFA): inst.JP_cc_nn(f_S, 1); break;
        case (0xFB): inst.EI(); break;
        case (0xFD): OpcodeFD(inst, executed); break;
        case (0xFE): inst.CP_n($); break;
        case (0xFF): inst.RST_n(0x38); break;
        default:
            stringstream out;
            out << "Error, instruction not implemented: 0x";
            out << setfill('0') << setw(2) << uppercase << hex << (int)opcode << endl;
            cout << out.str();
            throw SMSException(out.str());
            
    } // end switch
    
    if (!executed) {
        if (GetIncPC())
            AddPC(GetInstructionLength(opcode));

        executed = true;
    }
}


void CPU::OpcodeCB(Instructions &inst, bool &executed)
{
    u8 opcode = MemR(GetPC() + 1);
    AddR(1);

    switch (opcode)
    {
		case (0x00): inst.RLC_n(B); break;
		case (0x01): inst.RLC_n(C); break;
		case (0x02): inst.RLC_n(D); break;
		case (0x03): inst.RLC_n(E); break;
		case (0x04): inst.RLC_n(H); break;
		case (0x05): inst.RLC_n(L); break;
		case (0x06): inst.RLC_n(c_HL); break;
		case (0x07): inst.RLC_n(A); break;
		case (0x08): inst.RRC_n(B); break;
		case (0x09): inst.RRC_n(C); break;
		case (0x0A): inst.RRC_n(D); break;
		case (0x0B): inst.RRC_n(E); break;
		case (0x0C): inst.RRC_n(H); break;
		case (0x0D): inst.RRC_n(L); break;
		case (0x0E): inst.RRC_n(c_HL); break;
		case (0x0F): inst.RRC_n(A); break;

		case (0x10): inst.RL_n(B); break;
		case (0x11): inst.RL_n(C); break;
		case (0x12): inst.RL_n(D); break;
		case (0x13): inst.RL_n(E); break;
		case (0x14): inst.RL_n(H); break;
		case (0x15): inst.RL_n(L); break;
		case (0x16): inst.RL_n(c_HL); break;
		case (0x17): inst.RL_n(A); break;
		case (0x18): inst.RR_n(B); break;
		case (0x19): inst.RR_n(C); break;
		case (0x1A): inst.RR_n(D); break;
		case (0x1B): inst.RR_n(E); break;
		case (0x1C): inst.RR_n(H); break;
		case (0x1D): inst.RR_n(L); break;
		case (0x1E): inst.RR_n(c_HL); break;
		case (0x1F): inst.RR_n(A); break;

        case (0x20): inst.SLA_n(B); break;
        case (0x21): inst.SLA_n(C); break;
        case (0x22): inst.SLA_n(D); break;
        case (0x23): inst.SLA_n(E); break;
        case (0x24): inst.SLA_n(H); break;
        case (0x25): inst.SLA_n(L); break;
        case (0x26): inst.SLA_n(c_HL); break;
        case (0x27): inst.SLA_n(A); break;
		case (0x28): inst.SRA_n(B); break;
		case (0x29): inst.SRA_n(C); break;
		case (0x2A): inst.SRA_n(D); break;
		case (0x2B): inst.SRA_n(E); break;
		case (0x2C): inst.SRA_n(H); break;
		case (0x2D): inst.SRA_n(L); break;
		case (0x2E): inst.SRA_n(c_HL); break;
		case (0x2F): inst.SRA_n(A); break;

        case (0x30): inst.SLL_n(B); break;
        case (0x31): inst.SLL_n(C); break;
        case (0x32): inst.SLL_n(D); break;
        case (0x33): inst.SLL_n(E); break;
        case (0x34): inst.SLL_n(H); break;
        case (0x35): inst.SLL_n(L); break;
        case (0x36): inst.SLL_n(c_HL); break;
        case (0x37): inst.SLL_n(A); break;
		case (0x38): inst.SRL_n(B); break;
        case (0x39): inst.SRL_n(C); break;
        case (0x3A): inst.SRL_n(D); break;
        case (0x3B): inst.SRL_n(E); break;
        case (0x3C): inst.SRL_n(H); break;
        case (0x3D): inst.SRL_n(L); break;
        case (0x3E): inst.SRL_n(c_HL); break;
        case (0x3F): inst.SRL_n(A); break;

		case (0x40): inst.BIT(0, GetB()); break;
		case (0x41): inst.BIT(0, GetC()); break;
		case (0x42): inst.BIT(0, GetD()); break;
		case (0x43): inst.BIT(0, GetE()); break;
		case (0x44): inst.BIT(0, GetH()); break;
		case (0x45): inst.BIT(0, GetL()); break;
		case (0x46): inst.BIT(0, MemR(GetHL())); break;
		case (0x47): inst.BIT(0, GetA()); break;
		case (0x48): inst.BIT(1, GetB()); break;
		case (0x49): inst.BIT(1, GetC()); break;
		case (0x4A): inst.BIT(1, GetD()); break;
		case (0x4B): inst.BIT(1, GetE()); break;
		case (0x4C): inst.BIT(1, GetH()); break;
		case (0x4D): inst.BIT(1, GetL()); break;
		case (0x4E): inst.BIT(1, MemR(GetHL())); break;
		case (0x4F): inst.BIT(1, GetA()); break;

		case (0x50): inst.BIT(2, GetB()); break;
		case (0x51): inst.BIT(2, GetC()); break;
		case (0x52): inst.BIT(2, GetD()); break;
		case (0x53): inst.BIT(2, GetE()); break;
		case (0x54): inst.BIT(2, GetH()); break;
		case (0x55): inst.BIT(2, GetL()); break;
		case (0x56): inst.BIT(2, MemR(GetHL())); break;
		case (0x57): inst.BIT(2, GetA()); break;
		case (0x58): inst.BIT(3, GetB()); break;
		case (0x59): inst.BIT(3, GetC()); break;
		case (0x5A): inst.BIT(3, GetD()); break;
		case (0x5B): inst.BIT(3, GetE()); break;
		case (0x5C): inst.BIT(3, GetH()); break;
		case (0x5D): inst.BIT(3, GetL()); break;
		case (0x5E): inst.BIT(3, MemR(GetHL())); break;
		case (0x5F): inst.BIT(3, GetA()); break;

		case (0x60): inst.BIT(4, GetB()); break;
		case (0x61): inst.BIT(4, GetC()); break;
		case (0x62): inst.BIT(4, GetD()); break;
		case (0x63): inst.BIT(4, GetE()); break;
		case (0x64): inst.BIT(4, GetH()); break;
		case (0x65): inst.BIT(4, GetL()); break;
		case (0x66): inst.BIT(4, MemR(GetHL())); break;
		case (0x67): inst.BIT(4, GetA()); break;
		case (0x68): inst.BIT(5, GetB()); break;
		case (0x69): inst.BIT(5, GetC()); break;
		case (0x6A): inst.BIT(5, GetD()); break;
		case (0x6B): inst.BIT(5, GetE()); break;
		case (0x6C): inst.BIT(5, GetH()); break;
		case (0x6D): inst.BIT(5, GetL()); break;
		case (0x6E): inst.BIT(5, MemR(GetHL())); break;
		case (0x6F): inst.BIT(5, GetA()); break;

		case (0x70): inst.BIT(6, GetB()); break;
		case (0x71): inst.BIT(6, GetC()); break;
		case (0x72): inst.BIT(6, GetD()); break;
		case (0x73): inst.BIT(6, GetE()); break;
		case (0x74): inst.BIT(6, GetH()); break;
		case (0x75): inst.BIT(6, GetL()); break;
		case (0x76): inst.BIT(6, MemR(GetHL())); break;
		case (0x77): inst.BIT(6, GetA()); break;
		case (0x78): inst.BIT(7, GetB()); break;
		case (0x79): inst.BIT(7, GetC()); break;
		case (0x7A): inst.BIT(7, GetD()); break;
		case (0x7B): inst.BIT(7, GetE()); break;
		case (0x7C): inst.BIT(7, GetH()); break;
		case (0x7D): inst.BIT(7, GetL()); break;
		case (0x7E): inst.BIT(7, MemR(GetHL())); break;
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
			throw SMSException(out.str().data());
    }
    
    if (!executed) {
        AddPC(GetInstructionCBLength(opcode));
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
        case 0x29: inst.ADD(PtrIX(), GetIX()); break;
        case 0x2A: inst.LD(PtrIX(), Get16BitsInmValue(2)); break;
        case 0x2C: inst.INC(PtrIXl()); break;
        case 0x2D: inst.DEC(PtrIXl()); break;
            
        case 0x34: inst.INC_Mem(GetIXPlusD(2)); break;
        case 0x35: inst.DEC_Mem(GetIXPlusD(2)); break;
        case 0x36: inst.LD_Mem(GetIXPlusD(2), Get8BitsInmValue(3)); break;
        case 0x39: inst.ADD(PtrIX(), GetSP()); break;
            
        case 0x46: inst.LD(PtrB(), MemR(GetIXPlusD(2))); break;
        case 0x4E: inst.LD(PtrC(), MemR(GetIXPlusD(2))); break;
            
        case 0x56: inst.LD(PtrD(), MemR(GetIXPlusD(2))); break;
        case 0x5E: inst.LD(PtrE(), MemR(GetIXPlusD(2))); break;
            
        case 0x60: inst.LD(PtrIXh(), GetB()); break;
        case 0x61: inst.LD(PtrIXh(), GetC()); break;
        case 0x62: inst.LD(PtrIXh(), GetD()); break;
        case 0x63: inst.LD(PtrIXh(), GetE()); break;
        case 0x64: inst.LD(PtrIXh(), GetIXh()); break;
        case 0x65: inst.LD(PtrIXh(), GetIXl()); break;
        case 0x66: inst.LD(PtrH(), MemR(GetIXPlusD(2))); break;
        case 0x67: inst.LD(PtrIXl(), GetA()); break;
        case 0x68: inst.LD(PtrIXl(), GetB()); break;
        case 0x69: inst.LD(PtrIXl(), GetC()); break;
        case 0x6A: inst.LD(PtrIXl(), GetD()); break;
        case 0x6B: inst.LD(PtrIXl(), GetE()); break;
        case 0x6C: inst.LD(PtrIXl(), GetIXh()); break;
        case 0x6D: inst.LD(PtrIXl(), GetIXl()); break;
        case 0x6E: inst.LD(PtrL(), MemR(GetIXPlusD(2))); break;
        
        case 0x70: inst.LD_Mem(GetIXPlusD(2), GetB()); break;
        case 0x71: inst.LD_Mem(GetIXPlusD(2), GetC()); break;
        case 0x72: inst.LD_Mem(GetIXPlusD(2), GetD()); break;
        case 0x73: inst.LD_Mem(GetIXPlusD(2), GetE()); break;
        case 0x74: inst.LD_Mem(GetIXPlusD(2), GetH()); break;
        case 0x75: inst.LD_Mem(GetIXPlusD(2), GetL()); break;
        case 0x77: inst.LD_Mem(GetIXPlusD(2), GetA()); break;
        case 0x7E: inst.LD(PtrA(), MemR(GetIXPlusD(2))); break;
        
        case 0x84: inst.ADD(GetIXh());  break;
        case 0x85: inst.ADD(GetIXl());  break;
        case 0x86: inst.ADD(MemR(GetIXPlusD(2)));  break;
        case 0x8C: inst.ADC(GetIXh());  break;
        case 0x8D: inst.ADC(GetIXl());  break;
        case 0x8E: inst.ADC(MemR(GetIXPlusD(2)));  break;
            
        case 0xB4: inst.OR(GetIXh());  break;
        case 0xB5: inst.OR(GetIXl());  break;
        case 0xB6: inst.OR(MemR(GetIXPlusD(2)));  break;
            
        case 0xCB: OpcodeDDCB(inst, executed);  break;
            
        case 0xE1: inst.POP(PtrIX());  break;
        case 0xE5: inst.PUSH(GetIX());  break;
            
        default:
            stringstream out;
			out << "Error, instruction not implemented: 0xDD";
			out << setfill('0') << setw(2) << uppercase << hex << (int)opcode << "\n";
            cout << out.str();
			throw SMSException(out.str().data());
    }
    
    if (!executed) {
        if (GetIncPC())
            AddPC(GetInstructionDDLength(opcode));
        
        executed = true;
    }
}

void CPU::OpcodeED(Instructions &inst, bool &executed)
{
    u8 opcode = MemR(GetPC() + 1);
    AddR(1);
    
    switch (opcode)
    {
		case (0x40): inst.IN(B); break;
		case (0x41): inst.OUT(C, A); break;
		case (0x42): inst.SBC_HL(BC); break;
		case (0x43): inst.LD_Mem(Get16BitsInmValue(2), GetBC()); break;
        case (0x44): inst.NEG(); break;
		case (0x46): inst.IM(0); break;
		case (0x48): inst.IN(C); break;
		case (0x49): inst.OUT(C, C); break;
        case (0x4A): inst.ADC(GetBC()); break;
        case (0x4B): inst.LD(PtrBC(), Get16BitsInmValue(2)); break;
        case (0x4C): inst.NEG(); break;
		case (0x4D): inst.RETI(); break;
		case (0x4F): inst.LD_R_A(); break;
            
		case (0x50): inst.IN(D); break;
		case (0x51): inst.OUT(C, D); break;
		case (0x52): inst.SBC_HL(DE); break;
		case (0x53): inst.LD_Mem(Get16BitsInmValue(2), GetDE()); break;
        case (0x54): inst.NEG(); break;
		case (0x56): inst.IM(1); break;
		case (0x58): inst.IN(E); break;
		case (0x59): inst.OUT(C, E); break;
        case (0x5A): inst.ADC(GetDE()); break;
        case (0x5B): inst.LD(PtrDE(), Get16BitsInmValue(2)); break;
        case (0x5C): inst.NEG(); break;
		case (0x5E): inst.IM(2); break;
		case (0x5F): inst.LD_A_R(); break;
            
		case (0x60): inst.IN(H); break;
		case (0x61): inst.OUT(C, H); break;
		case (0x62): inst.SBC_HL(HL); break;
		case (0x63): inst.LD_Mem(Get16BitsInmValue(2), GetHL()); break;
        case (0x64): inst.NEG(); break;
		case (0x68): inst.IN(L); break;
		case (0x69): inst.OUT(C, L); break;
        case (0x6A): inst.ADC(GetHL()); break;
        case (0x6B): inst.LD(PtrHL(), Get16BitsInmValue(2)); break;
        case (0x6C): inst.NEG(); break;
        
        case (0x71): inst.OUT(C, f_Z); break;
		case (0x72): inst.SBC_HL(SP); break;
		case (0x73): inst.LD_Mem(Get16BitsInmValue(2), GetSP()); break;
        case (0x74): inst.NEG(); break;
        case (0x78): inst.IN(A); break;
		case (0x79): inst.OUT(C, A); break;
        case (0x7A): inst.ADC(GetSP()); break;
        case (0x7B): inst.LD(PtrSP(), Get16BitsInmValue(2)); break;
        case (0x7C): inst.NEG(); break;
        
        case (0xA0): inst.LDI(); break;
        case (0xA1): inst.CPI(); break;
        case (0xA3): inst.OUTI(); break;
        case (0xA8): inst.LDD(); break;
            
        case (0xB0): inst.LDIR(); break;
        case (0xB3): inst.OTIR(); break;
        case (0xB8): inst.LDDR(); break;
            
        default:
			stringstream out;
			out << "Error, instruction not implemented: 0xED";
			out << setfill('0') << setw(2) << uppercase << hex << (int)opcode << "\n";
            cout << out.str();
			throw SMSException(out.str().data());
    }
    
    if (!executed) {
        if (GetIncPC())
            AddPC(GetInstructionEDLength(opcode));
        
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
        case 0x23: inst.INC_NoFlags(PtrIY()); break;
        case 0x25: inst.DEC(PtrIYh()); break;
        case 0x29: inst.ADD(PtrIY(), GetIY()); break;
            
        case 0x36: inst.LD_Mem(GetIYPlusD(2), Get8BitsInmValue(3)); break;
        case 0x39: inst.ADD(PtrIY(), GetSP()); break;
        
        case 0x46: inst.LD(PtrB(), MemR(GetIYPlusD(2))); break;
        case 0x4E: inst.LD(PtrC(), MemR(GetIYPlusD(2))); break;
            
        case 0x56: inst.LD(PtrD(), MemR(GetIYPlusD(2))); break;
        case 0x5E: inst.LD(PtrE(), MemR(GetIYPlusD(2))); break;
            
        case 0x60: inst.LD(PtrIYh(), GetB()); break;
        case 0x61: inst.LD(PtrIYh(), GetC()); break;
        case 0x62: inst.LD(PtrIYh(), GetD()); break;
        case 0x63: inst.LD(PtrIYh(), GetE()); break;
        case 0x64: inst.LD(PtrIYh(), GetIYh()); break;
        case 0x65: inst.LD(PtrIYh(), GetIYl()); break;
        case 0x66: inst.LD(PtrH(), MemR(GetIYPlusD(2))); break;
        case 0x67: inst.LD(PtrIYl(), GetA()); break;
        case 0x68: inst.LD(PtrIYl(), GetB()); break;
        case 0x69: inst.LD(PtrIYl(), GetC()); break;
        case 0x6A: inst.LD(PtrIYl(), GetD()); break;
        case 0x6B: inst.LD(PtrIYl(), GetE()); break;
        case 0x6C: inst.LD(PtrIYl(), GetIYh()); break;
        case 0x6D: inst.LD(PtrIYl(), GetIYl()); break;
        case 0x6E: inst.LD(PtrL(), MemR(GetIYPlusD(2))); break;
            
        case 0x70: inst.LD_Mem(GetIYPlusD(2), GetB()); break;
        case 0x71: inst.LD_Mem(GetIYPlusD(2), GetC()); break;
        case 0x72: inst.LD_Mem(GetIYPlusD(2), GetD()); break;
        case 0x73: inst.LD_Mem(GetIYPlusD(2), GetE()); break;
        case 0x74: inst.LD_Mem(GetIYPlusD(2), GetH()); break;
        case 0x75: inst.LD_Mem(GetIYPlusD(2), GetL()); break;
        case 0x77: inst.LD_Mem(GetIYPlusD(2), GetA()); break;
        case 0x7E: inst.LD(PtrA(), MemR(GetIYPlusD(2))); break;
          
        case 0x84: inst.ADD(GetIYh());  break;
        case 0x85: inst.ADD(GetIYl());  break;
        case 0x86: inst.ADD(MemR(GetIYPlusD(2)));  break;
        case 0x8C: inst.ADC(GetIYh());  break;
        case 0x8D: inst.ADC(GetIYl());  break;
        case 0x8E: inst.ADC(MemR(GetIYPlusD(2)));  break;
            
        case 0xCB: OpcodeFDCB(inst, executed);  break;
            
        case 0xE1: inst.POP(PtrIY());  break;
        case 0xE5: inst.PUSH(GetIY()); break;
            
        default:
			stringstream out;
			out << "Error, instruction not implemented: 0xFD";
			out << setfill('0') << setw(2) << uppercase << hex << (int)opcode << "\n";
            cout << out.str();
			throw SMSException(out.str().data());
    }
    
    if (!executed) {
        if (GetIncPC())
            AddPC(GetInstructionFDLength(opcode));
        
        executed = true;
    }
}

void CPU::OpcodeDDCB(Instructions &inst, bool &executed)
{
    u8 opcode = MemR(GetPC() + 3);
    AddR(1);
    
    switch (opcode)
    {
        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x46:
        case 0x47: inst.BIT(0, MemR(GetIXPlusD(2))); break;
        case 0x48:
        case 0x49:
        case 0x4A:
        case 0x4B:
        case 0x4C:
        case 0x4D:
        case 0x4E:
        case 0x4F: inst.BIT(1, MemR(GetIXPlusD(2))); break;
            
        case 0x50:
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0x56:
        case 0x57: inst.BIT(2, MemR(GetIXPlusD(2))); break;
        case 0x58:
        case 0x59:
        case 0x5A:
        case 0x5B:
        case 0x5C:
        case 0x5D:
        case 0x5E:
        case 0x5F: inst.BIT(3, MemR(GetIXPlusD(2))); break;
            
        case 0x60:
        case 0x61:
        case 0x62:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
        case 0x67: inst.BIT(4, MemR(GetIXPlusD(2))); break;
        case 0x68:
        case 0x69:
        case 0x6A:
        case 0x6B:
        case 0x6C:
        case 0x6D:
        case 0x6E:
        case 0x6F: inst.BIT(5, MemR(GetIXPlusD(2))); break;
            
        case 0x70:
        case 0x71:
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x76:
        case 0x77: inst.BIT(6, MemR(GetIXPlusD(2))); break;
        case 0x78:
        case 0x79:
        case 0x7A:
        case 0x7B:
        case 0x7C:
        case 0x7D:
        case 0x7E:
        case 0x7F: inst.BIT(7, MemR(GetIXPlusD(2))); break;
            
        case 0x80:
        case 0x81:
        case 0x82:
        case 0x83:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0x87: inst.RES_Mem(0, MemR(GetIXPlusD(2))); break;
        case 0x88:
        case 0x89:
        case 0x8A:
        case 0x8B:
        case 0x8C:
        case 0x8D:
        case 0x8E:
        case 0x8F: inst.RES_Mem(1, MemR(GetIXPlusD(2))); break;
            
        case 0x90:
        case 0x91:
        case 0x92:
        case 0x93:
        case 0x94:
        case 0x95:
        case 0x96:
        case 0x97: inst.RES_Mem(2, MemR(GetIXPlusD(2))); break;
        case 0x98:
        case 0x99:
        case 0x9A:
        case 0x9B:
        case 0x9C:
        case 0x9D:
        case 0x9E:
        case 0x9F: inst.RES_Mem(3, MemR(GetIXPlusD(2))); break;
            
        case 0xA0:
        case 0xA1:
        case 0xA2:
        case 0xA3:
        case 0xA4:
        case 0xA5:
        case 0xA6:
        case 0xA7: inst.RES_Mem(4, MemR(GetIXPlusD(2))); break;
        case 0xA8:
        case 0xA9:
        case 0xAA:
        case 0xAB:
        case 0xAC:
        case 0xAD:
        case 0xAE:
        case 0xAF: inst.RES_Mem(5, MemR(GetIXPlusD(2))); break;
            
        case 0xB0:
        case 0xB1:
        case 0xB2:
        case 0xB3:
        case 0xB4:
        case 0xB5:
        case 0xB6:
        case 0xB7: inst.RES_Mem(6, MemR(GetIXPlusD(2))); break;
        case 0xB8:
        case 0xB9:
        case 0xBA:
        case 0xBB:
        case 0xBC:
        case 0xBD:
        case 0xBE:
        case 0xBF: inst.RES_Mem(7, MemR(GetIXPlusD(2))); break;
            
        case 0xC0:
        case 0xC1:
        case 0xC2:
        case 0xC3:
        case 0xC4:
        case 0xC5:
        case 0xC6:
        case 0xC7: inst.SET_Mem(0, MemR(GetIXPlusD(2))); break;
        case 0xC8:
        case 0xC9:
        case 0xCA:
        case 0xCB:
        case 0xCC:
        case 0xCD:
        case 0xCE:
        case 0xCF: inst.SET_Mem(1, MemR(GetIXPlusD(2))); break;
            
        case 0xD0:
        case 0xD1:
        case 0xD2:
        case 0xD3:
        case 0xD4:
        case 0xD5:
        case 0xD6:
        case 0xD7: inst.SET_Mem(2, MemR(GetIXPlusD(2))); break;
        case 0xD8:
        case 0xD9:
        case 0xDA:
        case 0xDB:
        case 0xDC:
        case 0xDD:
        case 0xDE:
        case 0xDF: inst.SET_Mem(3, MemR(GetIXPlusD(2))); break;
            
        case 0xE0:
        case 0xE1:
        case 0xE2:
        case 0xE3:
        case 0xE4:
        case 0xE5:
        case 0xE6:
        case 0xE7: inst.SET_Mem(4, MemR(GetIXPlusD(2))); break;
        case 0xE8:
        case 0xE9:
        case 0xEA:
        case 0xEB:
        case 0xEC:
        case 0xED:
        case 0xEE:
        case 0xEF: inst.SET_Mem(5, MemR(GetIXPlusD(2))); break;
            
        case 0xF0:
        case 0xF1:
        case 0xF2:
        case 0xF3:
        case 0xF4:
        case 0xF5:
        case 0xF6:
        case 0xF7: inst.SET_Mem(6, MemR(GetIXPlusD(2))); break;
        case 0xF8:
        case 0xF9:
        case 0xFA:
        case 0xFB:
        case 0xFC:
        case 0xFD:
        case 0xFE:
        case 0xFF: inst.SET_Mem(7, MemR(GetIXPlusD(2))); break;
            
        default:
			stringstream out;
			out << "Error, instruction not implemented: 0xDDCB";
			out << setfill('0') << setw(2) << uppercase << hex << (int)opcode << "\n";
            cout << out.str();
			throw SMSException(out.str().data());
    }
    
    if (!executed) {
        AddPC(GetInstructionDDCBLength(opcode));
        executed = true;
    }
}

void CPU::OpcodeFDCB(Instructions &inst, bool &executed)
{
    u8 opcode = MemR(GetPC() + 3);
    AddR(1);
    
    switch (opcode)
    {
        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x46:
        case 0x47: inst.BIT(0, MemR(GetIYPlusD(2))); break;
        case 0x48:
        case 0x49:
        case 0x4A:
        case 0x4B:
        case 0x4C:
        case 0x4D:
        case 0x4E:
        case 0x4F: inst.BIT(1, MemR(GetIYPlusD(2))); break;
            
        case 0x50:
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0x56:
        case 0x57: inst.BIT(2, MemR(GetIYPlusD(2))); break;
        case 0x58:
        case 0x59:
        case 0x5A:
        case 0x5B:
        case 0x5C:
        case 0x5D:
        case 0x5E:
        case 0x5F: inst.BIT(3, MemR(GetIYPlusD(2))); break;
            
        case 0x60:
        case 0x61:
        case 0x62:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
        case 0x67: inst.BIT(4, MemR(GetIYPlusD(2))); break;
        case 0x68:
        case 0x69:
        case 0x6A:
        case 0x6B:
        case 0x6C:
        case 0x6D:
        case 0x6E:
        case 0x6F: inst.BIT(5, MemR(GetIYPlusD(2))); break;
            
        case 0x70:
        case 0x71:
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x76:
        case 0x77: inst.BIT(6, MemR(GetIYPlusD(2))); break;
        case 0x78:
        case 0x79:
        case 0x7A:
        case 0x7B:
        case 0x7C:
        case 0x7D:
        case 0x7E:
        case 0x7F: inst.BIT(7, MemR(GetIYPlusD(2))); break;
            
        case 0x80:
        case 0x81:
        case 0x82:
        case 0x83:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0x87: inst.RES_Mem(0, MemR(GetIYPlusD(2))); break;
        case 0x88:
        case 0x89:
        case 0x8A:
        case 0x8B:
        case 0x8C:
        case 0x8D:
        case 0x8E:
        case 0x8F: inst.RES_Mem(1, MemR(GetIYPlusD(2))); break;
            
        case 0x90:
        case 0x91:
        case 0x92:
        case 0x93:
        case 0x94:
        case 0x95:
        case 0x96:
        case 0x97: inst.RES_Mem(2, MemR(GetIYPlusD(2))); break;
        case 0x98:
        case 0x99:
        case 0x9A:
        case 0x9B:
        case 0x9C:
        case 0x9D:
        case 0x9E:
        case 0x9F: inst.RES_Mem(3, MemR(GetIYPlusD(2))); break;
            
        case 0xA0:
        case 0xA1:
        case 0xA2:
        case 0xA3:
        case 0xA4:
        case 0xA5:
        case 0xA6:
        case 0xA7: inst.RES_Mem(4, MemR(GetIYPlusD(2))); break;
        case 0xA8:
        case 0xA9:
        case 0xAA:
        case 0xAB:
        case 0xAC:
        case 0xAD:
        case 0xAE:
        case 0xAF: inst.RES_Mem(5, MemR(GetIYPlusD(2))); break;
            
        case 0xB0:
        case 0xB1:
        case 0xB2:
        case 0xB3:
        case 0xB4:
        case 0xB5:
        case 0xB6:
        case 0xB7: inst.RES_Mem(6, MemR(GetIYPlusD(2))); break;
        case 0xB8:
        case 0xB9:
        case 0xBA:
        case 0xBB:
        case 0xBC:
        case 0xBD:
        case 0xBE:
        case 0xBF: inst.RES_Mem(7, MemR(GetIYPlusD(2))); break;
            
        case 0xC0:
        case 0xC1:
        case 0xC2:
        case 0xC3:
        case 0xC4:
        case 0xC5:
        case 0xC6:
        case 0xC7: inst.SET_Mem(0, MemR(GetIYPlusD(2))); break;
        case 0xC8:
        case 0xC9:
        case 0xCA:
        case 0xCB:
        case 0xCC:
        case 0xCD:
        case 0xCE:
        case 0xCF: inst.SET_Mem(1, MemR(GetIYPlusD(2))); break;
            
        case 0xD0:
        case 0xD1:
        case 0xD2:
        case 0xD3:
        case 0xD4:
        case 0xD5:
        case 0xD6:
        case 0xD7: inst.SET_Mem(2, MemR(GetIYPlusD(2))); break;
        case 0xD8:
        case 0xD9:
        case 0xDA:
        case 0xDB:
        case 0xDC:
        case 0xDD:
        case 0xDE:
        case 0xDF: inst.SET_Mem(3, MemR(GetIYPlusD(2))); break;
            
        case 0xE0:
        case 0xE1:
        case 0xE2:
        case 0xE3:
        case 0xE4:
        case 0xE5:
        case 0xE6:
        case 0xE7: inst.SET_Mem(4, MemR(GetIYPlusD(2))); break;
        case 0xE8:
        case 0xE9:
        case 0xEA:
        case 0xEB:
        case 0xEC:
        case 0xED:
        case 0xEE:
        case 0xEF: inst.SET_Mem(5, MemR(GetIYPlusD(2))); break;
            
        case 0xF0:
        case 0xF1:
        case 0xF2:
        case 0xF3:
        case 0xF4:
        case 0xF5:
        case 0xF6:
        case 0xF7: inst.SET_Mem(6, MemR(GetIYPlusD(2))); break;
        case 0xF8:
        case 0xF9:
        case 0xFA:
        case 0xFB:
        case 0xFC:
        case 0xFD:
        case 0xFE:
        case 0xFF: inst.SET_Mem(7, MemR(GetIYPlusD(2))); break;
            
        default:
			stringstream out;
			out << "Error, instruction not implemented: 0xFDCB";
			out << setfill('0') << setw(2) << uppercase << hex << (int)opcode << "\n";
            cout << out.str();
			throw SMSException(out.str().data());
    }
    
    if (!executed) {
        AddPC(GetInstructionFDCBLength(opcode));
        executed = true;
    }
}
