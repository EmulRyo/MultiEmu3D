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

#include <stdio.h>
#include <raylib.h>
#include "MainFrame.h"

int main(int argc, char **argv) {
    
    std::string fileName = "";
    if (argc > 1)
        fileName = std::string(argv[1]);

    SetConfigFlags(
        //FLAG_WINDOW_HIGHDPI |
        FLAG_MSAA_4X_HINT |
        FLAG_WINDOW_RESIZABLE |
        //FLAG_WINDOW_UNDECORATED |
        FLAG_VSYNC_HINT |
        0
    );
    InitWindow(256*3, (int)(256*3*240.0f/256.0f), "MultiEmu3D");
    //SetWindowIcon()

    MainFrame mainFrame(fileName);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        mainFrame.Update(dt);

        BeginDrawing();
        ClearBackground(BLACK);
        mainFrame.Draw(Rectangle {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()});
        EndDrawing();
    }

    CloseWindow();

    return 1;
}