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

#ifndef __MESSAGEBOX_DIALOG_H__
#define __MESSAGEBOX_DIALOG_H__

#include <string>
#include "Dialog.h"
#include "raylib.h"

class MessageBoxDialog: public Dialog {
public:
    MessageBoxDialog(const std::string &message);
    
    void Update();
    void Draw(Rectangle dst);
    
protected:
    std::string m_message;
};

#endif
