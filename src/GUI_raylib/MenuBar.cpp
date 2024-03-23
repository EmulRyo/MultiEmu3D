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

#include "raygui.h"
#include "MenuBar.h"

SubMenuItem::SubMenuItem(const std::string& text, std::function<void(int)> funCB) :
m_text(text), m_funCB(funCB)
{
    m_subMenu = new SubMenu();
}

// Copy constructor
SubMenuItem::SubMenuItem(const SubMenuItem& source)
    : m_text(source.m_text), m_funCB(source.m_funCB)
{
    m_subMenu = new SubMenu(*source.m_subMenu);
}

// Move constructor
SubMenuItem::SubMenuItem(SubMenuItem&& source) noexcept
    : m_text(source.m_text), m_funCB(source.m_funCB), m_subMenu {source.m_subMenu}
{
    source.m_subMenu = nullptr;
}

SubMenuItem::~SubMenuItem() {
    if (m_subMenu)
    {
        delete m_subMenu;
        m_subMenu = nullptr;
    }
}

void SubMenuItem::AddCallback(std::function<void(int)> funCB) {

}

bool SubMenuItem::Click(int id) {
    if (m_funCB) {
        m_funCB(id);
        return true;
    }
    else
        return false;
}

SubMenu& SubMenuItem::GetSubMenu() {
    return *m_subMenu;
}

const std::string& SubMenuItem::GetText() {
    return m_text;
}

void SubMenuItem::SetText(const std::string& text) {
    m_text = text;
}

SubMenu::SubMenu()
    : m_text(), m_focused(-1), m_width(30.0f)
{
    m_items.reserve(10);
}

SubMenu::SubMenu(const std::string& text) 
    : m_text(text), m_items(), m_focused(-1), m_width(30.0f)
{ 
    m_items.reserve(10);
}

SubMenu::SubMenu(const SubMenu& source)
    :m_text(source.m_text), m_items(source.m_items), m_focused(source.m_focused), m_width(source.m_width)
{
    for (int i = 0; i < source.m_vectorChars.size(); i++) {
        size_t len = strlen(source.m_vectorChars[i]);
        char* buffer = new char[len+1];
        strncpy_s(buffer, len+1, source.m_vectorChars[i], len+1);
        m_vectorChars.push_back(buffer);
    }
}

SubMenu::SubMenu(SubMenu&& source) noexcept
    :m_text(source.m_text), m_items(source.m_items), m_focused(source.m_focused), m_width(source.m_width)
{
    for (int i = 0; i < source.m_vectorChars.size(); i++) {
        m_vectorChars.push_back(source.m_vectorChars[i]);
    }
    source.m_vectorChars.clear();
}

SubMenu::~SubMenu() {
    for (int i = 0; i < m_vectorChars.size(); i++) {
        if (m_vectorChars[i] != nullptr) {
            delete[] m_vectorChars[i];
        }
    }
}

SubMenuItem& SubMenu::NewItem(const std::string& text, std::function<void(int)> funCB) {
    char* buffer = new char[64];
    snprintf(buffer, 64, "  %s", text.c_str());
    m_vectorChars.push_back(buffer);
    Font font = GuiGetFont();
    int textSpacing = GuiGetStyle(DEFAULT, TEXT_SPACING);
    Vector2 size = MeasureTextEx(font, buffer, font.baseSize, textSpacing);
    m_width = fmaxf(size.x + 12.0f, m_width);

    return m_items.emplace_back(text, funCB);;
}

SubMenuItem& SubMenu::GetItem(int pos) {
    return m_items[pos];
}

const std::string SubMenu::GetText() const {
    return m_text;
}

int SubMenu::GetNumItems() const {
    return (int)m_items.size();
}

void SubMenu::Deselect() {
    m_focused = -1;
    for (int i = 0; i < m_items.size(); i++) {
        m_items[i].GetSubMenu().Deselect();
    }
}

void SubMenu::UpdateTexts() {
    m_width = 0;
    Font font = GuiGetFont();
    int textSpacing = GuiGetStyle(DEFAULT, TEXT_SPACING);

    for (int i = 0; i < m_items.size(); i++) {
        char* buffer = m_vectorChars[i];
        snprintf(buffer, 64, "  %s", m_items[i].GetText().c_str());
        Vector2 size = MeasureTextEx(font, buffer, font.baseSize, textSpacing);
        m_width = fmaxf(size.x+12.0f, m_width);
    }
}

void SubMenu::SetWidth(float width) {
    m_width = width;
}

void SubMenu::Update(float deltaTime) {
    
}

// Devuelve si el menu debe seguir abierto
bool SubMenu::Draw(float x, float y) {
    if (m_items.size() == 0)
        return false;

    GuiSetStyle(LISTVIEW, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    GuiSetStyle(LISTVIEW, TEXT_PADDING, 10);
    int listViewItemsHeight = GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT);
    int listViewItemSpacing = GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING);
    int active = -1;
    int scrollIndex = 0;
    int focus = -1;

    const char** data = (const char **)m_vectorChars.data();
    float w = m_width;

    GuiListViewEx(Rectangle{ x, y, w, (listViewItemsHeight + listViewItemSpacing) * m_items.size() + 2.0f }, data, (int)m_items.size(), &scrollIndex, &active, &focus);
    for (int i = 0; i < m_items.size(); i++) {
        SubMenu& subMenu = m_items[i].GetSubMenu();
        if (subMenu.m_items.size() > 0)
            GuiDrawIcon(ICON_ARROW_RIGHT_FILL, x + w - 18, y + (listViewItemsHeight + listViewItemSpacing) * i + 4.0f, 1, WHITE);
    }

    if (focus >= 0) {
        m_focused = focus;
    }

    if (m_focused >= 0) {
        SubMenu& subMenu = m_items[m_focused].GetSubMenu();
        if (subMenu.m_items.size() > 0)
            subMenu.Draw(x + w, y + (listViewItemsHeight + listViewItemSpacing) * m_focused + 2.0f);
    }

    if (active >= 0) {
        return !m_items[active].Click(active);
    }

    return false;
}

MenuBar::MenuBar()
    : m_font(GetFontDefault()), m_menuTexts { { 0 } }, m_menuActive(-1), pendingToClose(false)
{
    m_subMenus.reserve(10);
}

void MenuBar::SetFont(Font font) {
    m_font = font;
}

SubMenu& MenuBar::NewSubMenu(const std::string& text) {
    strncpy_s(m_menuTexts[m_subMenus.size()], text.c_str(), 64);
    return m_subMenus.emplace_back(text);
}

SubMenu& MenuBar::GetSubMenu(int pos) {
    return m_subMenus[pos];
}

void MenuBar::Update(float deltaTime) {
    
}

void MenuBar::Draw(Rectangle bounds) {
    DrawRectangle((int)bounds.x, (int)bounds.y, (int)bounds.width, (int)bounds.height, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    GuiSetStyle(LISTVIEW, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiSetStyle(LISTVIEW, LIST_ITEMS_HEIGHT, 24);
    GuiSetStyle(LISTVIEW, LIST_ITEMS_SPACING, 0);
    static int visible = -1;
    Vector2 textSize;
    float x, width;
    static float listViewX;

    int textSpacing = GuiGetStyle(DEFAULT, TEXT_SPACING);

    x = bounds.x;

    int openedInThisFrame = -1;

    for (int i = 0; i < m_subMenus.size(); i++) {
        const char* text = m_menuTexts[i];
        textSize = MeasureTextEx(m_font, text, (float)m_font.baseSize, (float)textSpacing);
        width = textSize.x + 24;
        int active = -1;
        int focus = -1;
        GuiListViewEx(Rectangle{ x, bounds.y, width, bounds.height }, &text, 1, nullptr, &active, &focus);

        if (active >= 0) {
            if (m_menuActive >= 0) {
                m_menuActive = -1;
            }
            else {
                m_menuActive = openedInThisFrame = i;
                m_menuX = x;
            }
        }
        else if ((m_menuActive >= 0) && (focus >= 0)) {
            m_menuActive = openedInThisFrame = i;
            m_menuX = x;
        }

        x += width;
    }

    bool shouldStillBeOpen = false;
    if (m_menuActive >= 0) {
        // El submenú puede solicitar que no se cierre
        shouldStillBeOpen = m_subMenus[m_menuActive].Draw(m_menuX, bounds.y + 24);

        // Si se hace click, se cierra el menu
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            if ((openedInThisFrame < 0) && !shouldStillBeOpen) {
                m_menuActive = -1;
                for (int i = 0; i < m_subMenus.size(); i++)
                    m_subMenus[i].Deselect();
                pendingToClose = true;
            }
        }
    }
    // A cualquiera modulo externo que pregunte si el menú está abierto solo se le informará de manera negativa cuando
    // el usuario halla levantado el botón. Esto evita que el click se transmita a otros botones que esten debajo
    if (pendingToClose) {
        if (IsMouseButtonUp(MOUSE_BUTTON_LEFT) && IsMouseButtonUp(MOUSE_BUTTON_RIGHT)) {
            pendingToClose = false;
        }
    }
}

void MenuBar::Close() {
    m_menuActive = -1;
    for (int i = 0; i < m_subMenus.size(); i++)
        m_subMenus[i].Deselect();
    pendingToClose = false;
}

bool MenuBar::IsOpened() const {
    return m_menuActive >= 0 || pendingToClose;
}
