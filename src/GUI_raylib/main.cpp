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

int main() {
    InitWindow(256*3, (int)(256*3*240.0f/256.0f), "MultiEmu3D");

    MainFrame mainFrame("");

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        mainFrame.Update(dt);

        BeginDrawing();
        ClearBackground(BLACK);
        mainFrame.Draw();
        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();

    return 1;
}