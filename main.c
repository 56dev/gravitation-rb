#include <raylib.h>
#include <math.h>
typedef struct obj_s {
    float mass; 
    Vector2 pos_px; 
} obj_s;


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

void draw_vec_end(Vector2 pos, Vector2 to_end, Color col){
    DrawLineEx(pos, to_end, 1.0f, col);
    const float tri_hi = 9.0f;
    const float tri_b_h = 4.0f;
    float dx = to_end.x - pos.x;
    float dy = to_end.y - pos.y;
    float thet = atan(dy / dx);
    Vector2 v1 = (Vector2){to_end.x + tri_hi * cos(thet), to_end.y + tri_hi * sin(thet)};
    Vector2 v2 = (Vector2){to_end.x + tri_b_h * cos(thet + PI/2), to_end.y + tri_b_h * sin(thet + PI/2)};
    Vector2 v3 =  (Vector2){to_end.x + tri_b_h * cos(thet - PI/2), to_end.y + tri_b_h * sin(thet - PI/2)};
    DrawTriangle(v3, v2, v1, col);

}

Vector2 calc_g_field_at_point(Vector2 point, obj_s *obj_a, int obj_n){
    Vector2 ret = (Vector2){0, 0};
    for(int i = 0; i < obj_n; ++i) {
        float dx = point.x - obj_a[i].pos_px.x;
        float dy = point.y - obj_a[i].pos_px.y;
        float rsq = dx * dx + dy * dy;
        float mag = obj_a[i].mass / rsq; //we don't necessarily need the gravitational constant right now
        float thet = atan(dy / dx);
        ret.x += mag * cos(thet);    
        ret.y += mag * sin(thet);
    }
    return ret;
}

int main(void) {
    const int screen_width = 1600;
    const int screen_height = 900;
    InitWindow(screen_width, screen_height, "gravitation");
    SetTargetFPS(60);
    const int vec_spacing = 15;
    #define num_obj 1
    obj_s objects[num_obj] = {(obj_s){10.0f, (Vector2){screen_width/2, screen_height/2}}};
    while (!WindowShouldClose()) {

        BeginDrawing();
            ClearBackground(RAYWHITE);
            for(int x = 0; x < screen_width; x += vec_spacing){
                for(int y = 0; y < screen_height; y += vec_spacing){
                    Vector2 n;
                    for(int i = 0; i < num_obj; ++i){
                        n = calc_g_field_at_point((Vector2){x, y}, objects, num_obj);
                    }
                    //draw_vec_dir((Vector2){x, y}, 10.0f, 0.0f, GREEN);
                    draw_vec_end((Vector2){x, y}, n, GREEN);
                }
            }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
