#include <raylib.h>

int main(void) {
    const int screen_width = 1600;
    const int screen_height = 900;
    InitWindow(screen_width, screen_height, "gravitation");
    SetTargetFPS(60);
    const int vec_spacing = 15;
    while (!WindowShouldClose()) {

        BeginDrawing();
            ClearBackground(RAYWHITE);
            for(int x = 0; x < screen_width; x += vec_spacing){
                for(int y = 0; y < screen_height; y += vec_spacing){
                    DrawCircle(x, y, 1, GREEN);
                }
            }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
