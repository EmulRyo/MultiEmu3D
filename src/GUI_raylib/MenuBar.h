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

#ifndef __MENUBAR_H__
#define __MENUBAR_H__

#include <string>
#include <functional>
#include "raylib.h"

/*******************************************************************************
 // MenuBar Class
 *******************************************************************************/

class SubMenu;

class SubMenuItem {
public:
    SubMenuItem(const std::string& text, std::function<void(int)> funCB = nullptr);
    SubMenuItem(const SubMenuItem& source);
    SubMenuItem(SubMenuItem&& source) noexcept;
    ~SubMenuItem();

    void AddCallback(std::function<void(int)> funCB);
    SubMenu& GetSubMenu();
    const std::string& GetText();
    void SetText(const std::string& text);
    bool Click(int id);
private:
    std::string m_text;
    std::function<void(int)> m_funCB;
    SubMenu* m_subMenu;
};

class SubMenu {
public:
    SubMenu();
    SubMenu(const std::string& text);
    SubMenu(const SubMenu& source);
    SubMenu(SubMenu&& source) noexcept;
    ~SubMenu();

    const std::string GetText() const;
    int GetNumItems() const;
    SubMenuItem& NewItem(const std::string& text, std::function<void(int)> funCB=nullptr);
    SubMenuItem& GetItem(int pos);
    void Deselect();
    void SetWidth(float width);
    void UpdateTexts();

    void Update(float deltaTime);
    bool Draw(float x, float y);

private:
    std::string m_text;
    std::vector<SubMenuItem> m_items;
    std::vector<char *> m_vectorChars;
    int m_focused;
    float m_width;
};

class MenuBar {
public:
    MenuBar();

    void SetFont(Font font);
    SubMenu& NewSubMenu(const std::string& text);
    SubMenu& GetSubMenu(int pos);

    void Update(float deltaTime);
    void Draw(Rectangle dst);
    void Close();
    bool IsOpened() const;
    
private:
    std::vector<SubMenu> m_subMenus;
    char m_menuTexts[20][64];
    Font m_font;
    int m_menuActive;
    float m_menuX;
    bool pendingToClose;
};

#endif
