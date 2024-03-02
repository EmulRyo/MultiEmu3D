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

#ifndef __BASERENDERER_H__
#define __BASERENDERER_H__

#include "../Common/IScreenDrawable.h"
#include "raylib.h"

/*******************************************************************************
 * RendererBase Class
 *******************************************************************************/

namespace Renderer {
    enum Icon { Play, Pause, Stop, RewindL, RewindR };
};

class RendererBase: public IScreenDrawable {
    
public:
	RendererBase();
	~RendererBase();
	
    int  GetMinimunWidth();
    int  GetMinimunHeight();
    void SetRewindValue(float value);
    void SetIcon(Renderer::Icon icon, int frames=120);
    void SetGBPalette(bool original);
	
	void OnClear();
	void OnRefreshEmulatedScreen();
    void OnRefreshRealScreen();
    void OnDrawPixel(int idColor, int x, int y);
    void OnDrawPixel(u8 r, u8 g, u8 b, int x, int y);
    u8*  GetBufferPtr();
    virtual void OnSizeChanged(int x, int y, int width, int height);
    virtual void OnChangeView() = 0;
    virtual void Draw(Rectangle dst) = 0;
    
protected:
	u8 *m_imgBuf1;
    u8 *m_imgBuf2;
    u8 *m_frontBuffer;
    u8 *m_backBuffer;
    int m_bufferWidth, m_bufferHeight; // Tamaño de los buffers. (Se crean al principio y no cambian)
    int m_x, m_y, m_width, m_height;   // Posicion x, y, ancho y alto que usa el emulador dentro del buffer ya creado
    float m_rewindValue;
    int m_iconFrames;
    Renderer::Icon m_icon;
    
private:
    int m_gbPalette;
    
	void CreateBuffers();
    void PageFlip();
};

#endif
