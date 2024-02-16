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

#include <wx/dnd.h>
#include "../Common/IScreenDrawable.h"

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
	
    wxWindow * GetWinRenderer();
	void SetWinRenderer(wxWindow *parent, wxWindow *renderer);
	void CreateScreen();
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
    void OnSizeChanged(int x, int y, int width, int height);
    virtual void OnChangeView() = 0;
    
protected:
	u8 *m_imgBuf1;
    u8 *m_imgBuf2;
    u8 *m_frontBuffer;
    u8 *m_backBuffer;
    float m_rewindValue;
    int m_iconFrames;
    Renderer::Icon m_icon;
    int m_x, m_y, m_width, m_height;
    
private:
	wxWindow *m_winRenderer;
    wxWindow *m_parent;
    int       m_gbPalette;
    
    void PageFlip();
};

class DnDFile : public wxFileDropTarget {
public:
	DnDFile(wxWindow *parent);
	virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
private:
	wxWindow *m_parent;
};

#endif
