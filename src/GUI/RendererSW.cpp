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

#include <wx/dcbuffer.h>
#include <wx/image.h>
#include "IDControls.h"
#include "../SMS-GG/Def.h"
#include "Settings.h"
#include "RendererSW.h"

inline void RendererSW::OnEraseBackground(wxEraseEvent &) { /* do nothing */ }

BEGIN_EVENT_TABLE(RendererSW, wxPanel)
EVT_PAINT(RendererSW::OnPaint)
EVT_ERASE_BACKGROUND(RendererSW::OnEraseBackground)
END_EVENT_TABLE()

RendererSW::RendererSW(wxWindow *parent):
	wxPanel(parent, ID_MAINPANEL)
{
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	SetWinRenderer(parent, this);
}

RendererSW::~RendererSW() {
    
}

void RendererSW::OnPaint(wxPaintEvent &) {
    
	wxImage img = wxImage(SMS_SCREEN_W, SMS_SCREEN_H, m_frontBuffer, true);
	
	int winZoom = SettingsGetWindowZoom();
	if (winZoom > 1)
		img.Rescale(SMS_SCREEN_W*winZoom, SMS_SCREEN_H*winZoom, wxIMAGE_QUALITY_NORMAL);
    wxBitmap bmp(img);
    
    // paint the screen
    wxAutoBufferedPaintDC dc(this);
	dc.DrawBitmap(bmp, 0, 0);
		
	// dc.DrawText(wxString("Pokemon"), 0, 0);
}

