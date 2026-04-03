#include <raylib.h>
#include <stdio.h>
#include <math.h>
#include <raymath.h>
typedef struct obj_s {
    float mass; 
    Vector2 pos_px; 
} obj_s;


void draw_vec_dir(Vector2 pos, float len_px, float angle_rad, Color col){
    Vector2 end_pos = (Vector2){pos.x + len_px * cos(angle_rad), pos.y + len_px * sin(angle_rad)};
    DrawLineEx(pos, end_pos, 1.0f, col);
    const float tri_hi = 9.0f;
    const float tri_b_h = 2.0f;
    Vector2 v1 = (Vector2){end_pos.x + tri_hi * cos(angle_rad), end_pos.y + tri_hi * sin(angle_rad)};
    Vector2 v2 = (Vector2){end_pos.x + tri_b_h * cos(angle_rad + PI/2), end_pos.y + tri_b_h * sin(angle_rad + PI/2)};
    Vector2 v3 =  (Vector2){end_pos.x + tri_b_h * cos(angle_rad - PI/2), end_pos.y + tri_b_h * sin(angle_rad - PI/2)};
    DrawTriangle(v3, v2, v1, col);

}

void draw_vec_end(Vector2 pos, Vector2 to_end, Color col){
    if(fabs(to_end.x) < 0.1 && fabs(to_end.y) < 0.1) {
        DrawCircle(pos.x, pos.y, 2.0f, col);
        return;
    }
    Vector2 end = (Vector2){pos.x + to_end.x, pos.y + to_end.y};
    DrawLineEx(pos, end, 1.0f, col);
    float thet = atan2f(to_end.y, to_end.x);
    const float tri_hi = 9.0f;
    const float tri_b_h = 4.0f; 
    Vector2 v1 = (Vector2){end.x + tri_hi * cos(thet), end.y + tri_hi * sin(thet)};
    Vector2 v2 = (Vector2){end.x + tri_b_h * cos(thet + PI/2), end.y + tri_b_h * sin(thet + PI/2)};
    Vector2 v3 =  (Vector2){end.x + tri_b_h * cos(thet - PI/2), end.y + tri_b_h * sin(thet - PI/2)};
    DrawTriangle(v3, v2, v1, col);

}

Vector2 calc_g_field_at_point(Vector2 point, obj_s *obj_a, int obj_n){
    Vector2 ret = (Vector2){0, 0};
    for(int i = 0; i < obj_n; ++i) {
        float dx = obj_a[i].pos_px.x - point.x;
        float dy = obj_a[i].pos_px.y - point.y;
        float rsq = dx * dx + dy * dy;
        const int G = 1000;
        float mag = G  *  obj_a[i].mass / rsq;
        Vector2 s = Vector2Normalize((Vector2){dx, dy});
        ret.x += mag * s.x;    
        ret.y += mag * s.y;
    }
    return ret;
}

int main(void) {
    const float screen_width = 1600;
    const float screen_height = 900;
    InitWindow(screen_width, screen_height, "gravitation");
    SetTargetFPS(60);
    const int vec_spacing = 15;
    #define num_obj 2
    obj_s objects[num_obj] = {
        (obj_s){100.0f, (Vector2){screen_width/2, screen_height/2}},
        (obj_s){300.0f, (Vector2){screen_width/4.0f, screen_height/2}}
        };
    while (!WindowShouldClose()) {
        static bool f = false;
        BeginDrawing();
           ClearBackground(RAYWHITE);
            for(int x = 0; x < screen_width; x += vec_spacing){
                for(int y = 0; y < screen_height; y += vec_spacing){
                    Vector2 g = calc_g_field_at_point((Vector2){x, y}, objects, num_obj);
                    float mag = sqrt(g.x * g.x + g.y * g.y);
                    #define MAX_MAG 15 
                    if(mag < 0) mag = 0;
                    else if(mag > MAX_MAG) mag = MAX_MAG;
                    Vector2 disp = Vector2Normalize(g);
                    disp.x *= mag;
                    disp.y *= mag;
                    Color col = GREEN;
                    col.b = (mag / MAX_MAG) * 255;
                    col.g = ((MAX_MAG - mag) / MAX_MAG) * 255;
                    draw_vec_end((Vector2){x,y}, disp, col);
//                    DrawCircle(x, y, 2.0f, ORANGE);
                }
            }
            f = true;
            for(int i = 0; i < num_obj; ++i) {
                DrawCircleV(objects[i].pos_px,10.0f, RED);
            }
       EndDrawing();
    }

    CloseWindow();

    return 0;
}
