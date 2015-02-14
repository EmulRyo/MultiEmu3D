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

#ifndef __DEBUGGABLE_H__
#define __DEBUGGABLE_H__

#include "../Common/Types.h"

struct BreakpointNode {
    u16 value;
    BreakpointNode *prev;
    BreakpointNode *next;
};

class Debuggable {
public:
    Debuggable();
    virtual ~Debuggable();
    
    virtual std::string GetRegPC() = 0;
    virtual std::string GetRegs()  = 0;
    
    virtual std::string GetMem(u16 address) = 0;
    virtual std::string GetMem(u16 start, u16 end) = 0;
    virtual std::string Disassemble(u16 start, int numInstructions) = 0;
    virtual std::string Disassemble(int numInstructions) = 0;
    virtual void DisassembleNext(u16 &currentAddress, u16 &nextAddress, std::string &name, std::string &data) = 0;
    virtual void DisassembleOne(u16 address, u16 &nextAddress, std::string &name, std::string &data) = 0;
    
    virtual void Reset() = 0;
    virtual void StepInto() = 0;
    virtual bool ExecuteOneFrame() = 0;
    
    void AddBreakpoint(u16 address);
    void DelBreakpoint(u16 address);
    bool HasBreakpoint(u16 address);
    int  GetNumBreakpoints();
    u16  GetBreakpoint(int i);
    
    std::string ToHex(int value, int width, char fill);
    void AppendHex(std::stringstream &ss, int value, int width, char fill);
    
protected:
    BreakpointNode *m_firstBreakpoint;
    BreakpointNode *m_lastBreakpoint;
    
    BreakpointNode *GetBreakpointNode(u16 address);
    void ClearBreakpoints();
};

#endif
