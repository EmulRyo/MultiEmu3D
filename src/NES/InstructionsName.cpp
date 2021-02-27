/*
 This file is part of MarkSystem.
 
 MarkSystem is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 MarkSystem is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with MarkSystem.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "InstructionsName.h"

const char* NESinstructionsName[] = {
    "BRK",
    "ORA (d,x)",
    "STP",
    "SLO (d,x)",
    "NOP d",
    "ORA d",
    "ASL d",
    "SLO d",
    "PHP",
    "ORA #i",
    "ASL",
    "ANC #i",
    "NOP a",
    "ORA a",
    "ASL a",
    "SLO a",

    //0x10
    "BPL *+d",
    "ORA (d),y",
    "STP",
    "SLO (d),y",
    "NOP d,x",
    "ORA d,x",
    "ASL d,x",
    "SLO d,x",
    "CLC",
    "ORA a,y",
    "NOP",
    "SLO a,y",
    "NOP a,x",
    "ORA a,x",
    "ASL a,x",
    "SLO a,x",

    //0x20
    "JSR a",
    "AND (d,x)",
    "STP",
    "RLA (d,x)",
    "BIT d",
    "AND d",
    "ROL d",
    "RLA d",
    "PLP",
    "AND #i",
    "ROL",
    "ANC #i",
    "BIT a",
    "AND a",
    "ROL a",
    "RLA a",

    //0x30
    "BMI *+d",
    "AND (d),y",
    "STP",
    "RLA (d),y",
    "NOP d,x",
    "AND d,x",
    "ROL d,x",
    "RLA d,x",
    "SEC",
    "AND a,y",
    "NOP",
    "RLA a,y",
    "NOP a,x",
    "AND a,x",
    "ROL a,x",
    "RLA a,x",

    //0x40
    "RTI",
    "EOR (d,x)",
    "STP",
    "SRE (d,x)",
    "NOP d",
    "EOR d",
    "LSR d",
    "SRE d",
    "PHA",
    "EOR #i",
    "LSR",
    "ALR #i",
    "JMP a",
    "EOR a",
    "LSR a",
    "SRE a",

    //0x50
    "BVC *+d",
    "EOR (d),y",
    "STP",
    "SRE (d),y",
    "NOP d,x",
    "EOR d,x",
    "LSR d,x",
    "SRE d,x",
    "CLI",
    "EOR a,y",
    "NOP",
    "SRE a,y",
    "NOP a,x",
    "EOR a,x",
    "LSR a,x",
    "SRE a,x",

    //0x60
    "RTS",
    "ADC (d,x)",
    "STP",
    "RRA (d,x)",
    "NOP d",
    "ADC d",
    "ROR d",
    "RRA d",
    "PLA",
    "ADC #i",
    "ROR",
    "ARR #i",
    "JMP (a)",
    "ADC a",
    "ROR a",
    "RRA a",

    //0x70
    "BVS *+d",
    "ADC (d),y",
    "STP",
    "RRA (d),y",
    "NOP d,x",
    "ADC d,x",
    "ROR d,x",
    "RRA d,x",
    "SEI",
    "ADC a,y",
    "NOP",
    "RRA a,y",
    "NOP a,x",
    "ADC a,x",
    "ROR a,x",
    "RRA a,x",

    //0x80
    "NOP #i",
    "STA (d,x)",
    "NOP #i",
    "SAX (d,x)",
    "STY d",
    "STA d",
    "STX d",
    "SAX d",
    "DEY",
    "NOP #i",
    "TXA",
    "XAA #i",
    "STY a",
    "STA a",
    "STX a",
    "SAX a",

    //0x90
    "BCC *+d",
    "STA (d),y",
    "STP",
    "AHX (d),y",
    "STY d,x",
    "STA d,x",
    "STX d,y",
    "SAX d,y",
    "TYA",
    "STA a,y",
    "TXS",
    "TAS a,y",
    "SHY a,x",
    "STA a,x",
    "SHX a,y",
    "AHX a,y",

    //0xA0
    "LDY #i",
    "LDA (d,x)",
    "LDX #i",
    "LAX (d,x)",
    "LDY d",
    "LDA d",
    "LDX d",
    "LAX d",
    "TAY",
    "LDA #i",
    "TAX",
    "LAX #i",
    "LDY a",
    "LDA a",
    "LDX a",
    "LAX a",

    //0xB0
    "BCS *+d",
    "LDA (d),y",
    "STP",
    "LAX (d),y",
    "LDY d,x",
    "LDA d,x",
    "LDX d,y",
    "LAX d,y",
    "CLV",
    "LDA a,y",
    "TSX",
    "LAS a,y",
    "LDY a,x",
    "LDA a,x",
    "LDX a,y",
    "LAX a,y",

    //0xC0
    "CPY #i",
    "CMP (d,x)",
    "NOP #i",
    "DCP (d,x)",
    "CPY d",
    "CMP d",
    "DEC d",
    "DCP d",
    "INY",
    "CMP #i",
    "DEX",
    "AXS #i",
    "CPY a",
    "CMP a",
    "DEC a",
    "DCP a",

    //0xD0
    "BNE *+d",
    "CMP (d),y",
    "STP",
    "DCP (d),y",
    "NOP d,x",
    "CMP d,x",
    "DEC d,x",
    "DCP d,x",
    "CLD",
    "CMP a,y",
    "NOP",
    "DCP a,y",
    "NOP a,x",
    "CMP a,x",
    "DEC a,x",
    "DCP a,x",

    //0xE0
    "CPX #i",
    "SBC (d,x)",
    "NOP #i",
    "ISC (d,x)",
    "CPX d",
    "SBC d",
    "INC d",
    "ISC d",
    "INX",
    "SBC #i",
    "NOP",
    "SBC #i",
    "CPX a",
    "SBC a",
    "INC a",
    "ISC a",

    //0xF0
    "BEQ *+d",
    "SBC (d),y",
    "STP",
    "ISC (d),y",
    "NOP d,x",
    "SBC d,x",
    "INC d,x",
    "ISC d,x",
    "SED",
    "SBC a,y",
    "NOP",
    "ISC a,y",
    "NOP a,x",
    "SBC a,x",
    "INC a,x",
    "ISC a,x",
};

const char* Nes::GetInstructionName(u8 opcode) {
    return NESinstructionsName[opcode];
}
