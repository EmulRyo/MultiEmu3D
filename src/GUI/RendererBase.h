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

#ifndef __BASERENDERER_H__
#define __BASERENDERER_H__

#include <wx/dnd.h>
#include "../HW/ISMSScreenDrawable.h"

/*******************************************************************************
 * RendererBase Class
 *******************************************************************************/

class RendererBase: public ISMSScreenDrawable {
    
public:
	RendererBase();
	~RendererBase();
	
    wxWindow * GetWinRenderer();
	void SetWinRenderer(wxWindow *parent, wxWindow *renderer);
	void CreateScreen();
	void ChangePalette(bool original);
    int  GetMinimunWidth();
    int  GetMinimunHeight();
	
	void OnClear();
	void OnRefreshGBScreen();
    void OnRefreshRealScreen();
    void OnDrawPixel(u8 r, u8 g, u8 b, int x, int y);
    void OnSizeChanged(int x, int y, int width, int height);
    virtual void OnChangeView() = 0;
    
protected:
	u8 *m_imgBuf1;
    u8 *m_imgBuf2;
    u8 *m_frontBuffer;
    u8 *m_backBuffer;
    int m_x, m_y, m_width, m_height;
    
private:
	wxWindow *m_winRenderer;
    wxWindow *m_parent;
    
    void PageFlip();
};

// A drop target that adds filenames to a list box
class DnDFile : public wxFileDropTarget {
public:
	DnDFile(wxWindow *parent);
	virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
private:
	wxWindow *m_parent;
};

#endif
