#include <raylib.h>
#include <math.h>

void draw_vec_dir(Vector2 pos, float len_px, float angle_rad, Color col){
    Vector2 end_pos = (Vector2){pos.x + len_px * cos(angle_rad), pos.y + len_px * sin(angle_rad)};
    DrawLineEx(pos, end_pos, 1.0f, col);
    const float tri_hi = 9.0f;
    const float tri_b_h = 4.0f;
    Vector2 v1 = (Vector2){end_pos.x + tri_hi * cos(angle_rad), end_pos.y + tri_hi * sin(angle_rad)};
    Vector2 v2 = (Vector2){end_pos.x + tri_b_h * cos(angle_rad + PI/2), end_pos.y + tri_b_h * sin(angle_rad + PI/2)};
    Vector2 v3 =  (Vector2){end_pos.x + tri_b_h * cos(angle_rad - PI/2), end_pos.y + tri_b_h * sin(angle_rad - PI/2)};
    DrawTriangle(v3, v2, v1, col);

}

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
                    draw_vec_dir((Vector2){x, y}, 10.0f, 0.0f, GREEN);
                }
            }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
