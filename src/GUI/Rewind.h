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

#ifndef __REWIND_H__
#define __REWIND_H__

#include <sstream>

#define MAX_REWINDS 600

class RendererBase;
class VideoGameDevice;

class Rewind {
public:
    Rewind(VideoGameDevice *device);
    ~Rewind();
    bool IsEnabled();
    void Disable();
    void SetRenderer(RendererBase *renderer);
    void AddFrame();
    void UpdateScreen();
    void UpdatePad(bool buttonsState[12], bool buttonRewind);
    
private:
    bool m_enabled;
    RendererBase *m_renderer;
    VideoGameDevice *m_device;
    std::stringstream *m_data[MAX_REWINDS];
    int m_tail, m_head, m_length;
    int m_visible;
    
    void SetPosition();
};

#endif
