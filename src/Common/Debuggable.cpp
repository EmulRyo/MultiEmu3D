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

#include <string>
#include <sstream>
#include <iomanip>
#include "Debuggable.h"

using namespace std;

Debuggable::Debuggable() {
    m_firstBreakpoint = NULL;
    m_lastBreakpoint = NULL;
}

Debuggable::~Debuggable() {
    ClearBreakpoints();
}

std::string Debuggable::ToHex(int value, int width, char fill)
{
    stringstream ss;
    ss << setfill(fill) << setw(width) << uppercase << hex << value;
    return ss.str();
}

void Debuggable::AppendHex(stringstream &ss, int value, int width, char fill)
{
    ss << setfill(fill) << setw(width) << uppercase << hex << (int)value;
}

void Debuggable::AddBreakpoint(u16 address) {
    if (GetBreakpointNode(address))
        return;
    
    BreakpointNode *node = new BreakpointNode;
    node->value = address;
    node->prev = m_lastBreakpoint;
    node->next = NULL;
    if (m_lastBreakpoint)
        m_lastBreakpoint->next = node;
    if (m_firstBreakpoint == NULL)
        m_firstBreakpoint = node;
    m_lastBreakpoint = node;
}

void Debuggable::DelBreakpoint(u16 address) {
    BreakpointNode *node = GetBreakpointNode(address);
    if (node) {
        if (node->prev)
            node->prev->next = node->next;
        if (node->next)
            node->next->prev = node->prev;
        if (m_firstBreakpoint == node)
            m_firstBreakpoint = node->next;
        if (m_lastBreakpoint == node)
            m_lastBreakpoint = node->prev;
        
        delete node;
    }
}

int Debuggable::GetNumBreakpoints() {
    int count = 0;
    BreakpointNode *node = m_firstBreakpoint;
    
    while (node) {
        count++;
        node = node->next;
    }
    
    return count;
}

u16 Debuggable::GetBreakpoint(int i) {
    int count = 0;
    u16 value = 0;
    BreakpointNode *node = m_firstBreakpoint;
    
    while (node) {
        if (count == i)
            value = node->value;
        count++;
        node = node->next;
    }
    
    return value;
}

bool Debuggable::HasBreakpoint(u16 address) {
    return GetBreakpointNode(address) ? true : false;
}

BreakpointNode *Debuggable::GetBreakpointNode(u16 address) {
    BreakpointNode *node = m_firstBreakpoint;
    while (node) {
        if (node->value == address)
            return node;
        
        node = node->next;
    }
    
    return NULL;
}

void Debuggable::ClearBreakpoints() {
    BreakpointNode *node = m_firstBreakpoint;
    BreakpointNode *next;
    
    while (node) {
        next = node->next;
        delete node;
        node = next;
    }
    
    m_firstBreakpoint = NULL;
    m_lastBreakpoint  = NULL;
}
