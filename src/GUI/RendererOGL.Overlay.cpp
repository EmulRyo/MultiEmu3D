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

#ifdef __WXMAC__
#include <OpenGL/glu.h>
#else
#include <GL/glew.h>
#endif

#include "RendererOGL.h"

using namespace Renderer;

void RendererOGL::RenderOverlay() {
    if ((m_rewindValue < 0) && (m_iconFrames < 0))
        return;
    
    int winW, winH;
    GetClientSize(&winW, &winH);
    
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, winW, winH, 0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    
    if (m_iconFrames >= 0)
        DrawIcon(winW, winH);
    
    if (m_rewindValue >= 0)
        DrawRewind(winW, winH);
    
    glPopAttrib();
}

void RendererOGL::DrawIcon(int winW, int winH) {
    int x = 12;
    int y = 12;
    
    switch (m_icon) {
        case RewindL: DrawIconRewindL(x, y); break;
        case RewindR: DrawIconRewindR(x, y); break;
        case Play:    DrawIconPlay(x, y);    break;
        case Pause:   DrawIconPause(x, y);   break;
        case Stop:    DrawIconStop(x, y);    break;
            
        default: break;
    }
    
    m_iconFrames--;
}

void RendererOGL::DrawIconRewindL(int x, int y) {
    const int width = 14;
    const int height = 20;
    const int offset = 14;
    float x0 = x + 0.5f;
    float y0 = y + height/2 + 0.5f;
    float x1 = x + width + 0.5f;
    float y1 = y + height + 0.5f;
    float x2 = x1;
    float y2 = y + 0.5f;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLES);
    DrawTriangle(m_white, x0, y0, x1, y1, x2, y2);
    DrawTriangle(m_white, x0+offset, y0, x1+offset, y1, x2+offset, y2);
    glEnd();

    glLineWidth(1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_TRIANGLES);
    DrawTriangle(m_black, x0, y0, x1, y1, x2, y2);
    DrawTriangle(m_black, x0+offset, y0, x1+offset, y1, x2+offset, y2);
    glEnd();
}

void RendererOGL::DrawIconRewindR(int x, int y) {
    const int width = 14;
    const int height = 20;
    const int offset = 14;
    float x0 = x + width + 0.5f;
    float y0 = y + height/2 + 0.5f;
    float x1 = x + 0.5f;
    float y1 = y + 0.5f;
    float x2 = x1;
    float y2 = y + height + 0.5f;
    
    //GLubyte black[] = {   0,   0,   0, 255 };
    //GLubyte white[] = { 255, 255, 255, 180 };
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLES);
    DrawTriangle(m_white, x0, y0, x1, y1, x2, y2);
    DrawTriangle(m_white, x0+offset, y0, x1+offset, y1, x2+offset, y2);
    glEnd();
    
    glLineWidth(1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_TRIANGLES);
    DrawTriangle(m_black, x0, y0, x1, y1, x2, y2);
    DrawTriangle(m_black, x0+offset, y0, x1+offset, y1, x2+offset, y2);
    glEnd();
}

void RendererOGL::DrawIconPlay(int x, int y) {
    const int width = 14;
    const int height = 20;
    float x0 = x + width + 0.5f;
    float y0 = y + height/2 + 0.5f;
    float x1 = x + 0.5f;
    float y1 = y + 0.5f;
    float x2 = x1;
    float y2 = y + height + 0.5f;
    
    //GLubyte black[] = {   0,   0,   0, 255 };
    //GLubyte white[] = { 255, 255, 255, 180 };
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLES);
    DrawTriangle(m_white, x0, y0, x1, y1, x2, y2);
    glEnd();
    
    glLineWidth(1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_TRIANGLES);
    DrawTriangle(m_black, x0, y0, x1, y1, x2, y2);
    glEnd();
}

void RendererOGL::DrawIconPause(int x, int y) {
    const int width = 8;
    const int height = 20;
    const int offset = 12;
    float x0 = x + 0.5f;
    float y0 = y + 0.5f;
    float x1 = x + width + 0.5f;
    float y1 = y + height + 0.5f;
    
    //GLubyte black[] = {   0,   0,   0, 255 };
    //GLubyte white[] = { 255, 255, 255, 180 };
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_QUADS);
    DrawRectangle(m_white, x0, x1, y0, y1);
    DrawRectangle(m_white, x0+offset, x1+offset, y0, y1);
    glEnd();
    
    glLineWidth(1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_QUADS);
    DrawRectangle(m_black, x0, x1, y0, y1);
    DrawRectangle(m_black, x0+offset, x1+offset, y0, y1);
    glEnd();
}

void RendererOGL::DrawIconStop(int x, int y) {
    const int width = 20;
    const int height = 20;
    float x0 = x + 0.5f;
    float y0 = y + 0.5f;
    float x1 = x + width + 0.5f;
    float y1 = y + height + 0.5f;
    
    //GLubyte black[] = {   0,   0,   0, 255 };
    //GLubyte white[] = { 255, 255, 255, 180 };
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_QUADS);
    DrawRectangle(m_white, x0, x1, y0, y1);
    glEnd();
    
    glLineWidth(1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_QUADS);
    DrawRectangle(m_black, x0, x1, y0, y1);
    glEnd();
}

void RendererOGL::DrawRewind(int winW, int winH) {
    int gap = 12;
    int barHeight = 10;
    int cursorWidth = 6;
    int cursorHeightAdd = 3;
    
    float x0 = gap+0.5f;
    float x1 = winW-gap+0.5f;
    float y0 = winH-gap-barHeight+0.5f;
    float y1 = winH-gap+0.5f;
    float x2 = (int)(x0 - 0.5f + (x1-x0) * m_rewindValue) + 0.5f;
    float x3 = (int)(x2 - 0.5f - cursorWidth/2.0f) + 0.5f;
    float x4 = (int)(x2 - 0.5f + cursorWidth/2.0f) + 0.5f;
    float y3 = (int)(y0 - 0.5f - cursorHeightAdd) + 0.5f;
    float y4 = (int)(y1 - 0.5f + cursorHeightAdd) + 0.5f;
    
    glLineWidth(1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_QUADS);
    DrawRectangle(m_black, x0+1, x1-1, y0+1, y1-1);
    DrawRectangle(m_whiteOpaque, x0, x1, y0, y1);
    glEnd();
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_QUADS);
    DrawRectangle(m_blue, x0+1, x2, y0+1, y1-1);
    DrawRectangle(m_whiteOpaque, x3, x4, y3, y4);
    glEnd();
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_QUADS);
    DrawRectangle(m_black, x3, x4, y3, y4);
    glEnd();
}

void RendererOGL::DrawRectangle(GLubyte *rgba, float x0, float x1, float y0, float y1) {
    glColor4ubv(rgba);
    glVertex2f(x0, y0);
    glVertex2f(x0, y1);
    glVertex2f(x1, y1);
    glVertex2f(x1, y0);
}

void RendererOGL::DrawTriangle(GLubyte *rgba, float x0, float y0, float x1, float y1, float x2, float y2) {
    glColor4ubv(rgba);
    glVertex2f(x0, y0);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
}
