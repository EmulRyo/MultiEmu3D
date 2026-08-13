/*
 This file is part of MARS.
 
 MARS is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 MARS is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with MARS.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __RENDERER2D_H__
#define __RENDERER2D_H__

#include "RendererBase.h"
#include "raylib.h"

/*******************************************************************************
 * Renderer2D Class
 *******************************************************************************/

class Renderer2D : public RendererBase {

public:
	Renderer2D();
    ~Renderer2D();

	void Draw(const Rectangle& dst) override;
    void OnChangeView() override {};
    
private:
    
};

#endif
