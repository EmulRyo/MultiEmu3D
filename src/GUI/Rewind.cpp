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

#include "RendererBase.h"
#include "../HW/Def.h"
#include "../HW/Pad.h"
#include "../HW/CPU.h"
#include "Rewind.h"

using namespace std;

Rewind::Rewind()
{
    m_tail = -1;
    m_head = 0;
    m_length = 0;
    m_enabled = false;
}

Rewind::~Rewind() {
    for (int i=0; i<m_length; i++)
        delete m_data[i];
}

void Rewind::Disable() {
    m_enabled = false;
}

bool Rewind::IsEnabled() {
    return m_enabled;
}

void Rewind::SetCPU(CPU *cpu) {
    m_cpu = cpu;
}

void Rewind::SetRenderer(RendererBase *renderer) {
    m_renderer = renderer;
}

void Rewind::AddFrame() {
    m_tail = (m_tail + 1) % MAX_REWINDS;
    
    if (m_length >= MAX_REWINDS) {
        delete m_data[m_tail];
        if (m_tail == m_head)
            m_head = (m_head + 1) % MAX_REWINDS;
    }
    else
        m_length++;
    
    m_data[m_tail] = new stringstream();
    u8 *data = m_renderer->GetBufferPtr();
    int size = SMS_SCREEN_W*SMS_SCREEN_H*3;
    m_data[m_tail]->write((char *)data, size);
    m_cpu->SaveStateToRAM(m_data[m_tail]);
}

void Rewind::UpdateScreen() {
    u8 *data = m_renderer->GetBufferPtr();
    int size = SMS_SCREEN_W*SMS_SCREEN_H*3;
    stringstream *stream = m_data[m_visible];
    stream->seekg(0, stream->beg);
    stream->read((char *)data, size);
}

void Rewind::SetPosition() {
    float value = -1.0f;
    
    if (m_enabled) {
        int tail = m_tail;
        if (m_head > m_tail)
            tail += MAX_REWINDS;
        int total = tail - m_head + 1;
        tail = m_tail;
        if (m_visible > m_tail)
            tail += MAX_REWINDS;
        value = 1.0f - ((float)(tail - m_visible) / total);
    }
    
    m_renderer->SetRewindValue(value);
}

void Rewind::UpdatePad(bool buttonsState[12], bool buttonRewind) {
    if (m_enabled) {
        if (buttonsState[B1]) {
            m_enabled = false;
            m_cpu->LoadStateFromRAM(m_data[m_visible]);
            m_tail = m_visible;
            m_renderer->SetIcon(Renderer::Play);
            SetPosition();
        }
        else if (buttonsState[B2]) {
            m_enabled = false;
            m_renderer->SetIcon(Renderer::Play);
            SetPosition();
        }
        else if (buttonsState[LEFT]) {
            if (m_visible != m_head) {
                m_visible = (m_visible + MAX_REWINDS-1) % MAX_REWINDS;
                m_renderer->SetIcon(Renderer::RewindL);
                SetPosition();
                UpdateScreen();
            }
        }
        else if (buttonsState[RIGHT]) {
            if (m_visible != m_tail) {
                m_visible = (m_visible + 1) % MAX_REWINDS;
                m_renderer->SetIcon(Renderer::RewindR);
                SetPosition();
                UpdateScreen();
            }
        }
    }
    else {
        if (buttonRewind) {
            m_enabled = true;
            m_renderer->SetRewindValue(1.0f);
            m_renderer->SetIcon(Renderer::RewindL);
            m_visible = m_tail;
        }
    }
}
