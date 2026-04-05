#define RAYGUI_IMPLEMENTATION
#include <raylib.h>
#include <raygui.h>
#include <raymath.h>
#include <vector>
#include <math.h>
typedef struct obj_s {
    float mass; 
    float rad;
    Vector2 pos_px; 
    Vector2 vel_px_s;
} obj_s;

void draw_arrowhead(float angle_rad, float tri_hi, float tri_b_h, Vector2 apex_pos, Color col){
    Vector2 d = (Vector2){apex_pos.x - tri_hi * cos(angle_rad), apex_pos.y - tri_hi * sin(angle_rad)};
    Vector2 v2 = (Vector2){d.x + tri_b_h * cos(angle_rad + PI/2), d.y + tri_b_h * sin(angle_rad + PI/2)};
    Vector2 v3 =  (Vector2){d.x + tri_b_h * cos(angle_rad - PI/2), d.y + tri_b_h * sin(angle_rad - PI/2)};
    DrawTriangle(v3, v2, apex_pos, col);
}

void draw_vec_dir(Vector2 pos, float len_px, float angle_rad, Color col){
    Vector2 end_pos = (Vector2){pos.x + len_px * cos(angle_rad), pos.y + len_px * sin(angle_rad)};
    DrawLineEx(pos, end_pos, 1.0f, col);
    const float tri_hi = 9.0f;
    const float tri_b_h = 2.0f;
    draw_arrowhead(angle_rad, tri_hi, tri_b_h, end_pos, col);
}

void draw_vec_end(Vector2 pos, Vector2 to_end, Color col, bool draw_stem){
    if(fabs(to_end.x) < 0.1 && fabs(to_end.y) < 0.1) {
        DrawCircle(pos.x, pos.y, 2.0f, col);
        return;
    }
    Vector2 end;
    if(draw_stem){
        end = (Vector2){pos.x + to_end.x, pos.y + to_end.y};
        DrawLineEx(pos, end, 1.0f, col);
    } else {
        end = pos;
    }
    float thet = atan2f(to_end.y, to_end.x);
    const float tri_hi = 9.0f;
    const float tri_b_h = 4.0f; 
    draw_arrowhead(thet, tri_hi, tri_b_h, end, col);
}

Vector2 calc_g_field_at_point_ignore_one_obj(Vector2 point, obj_s *obj_a, int obj_n, int idx_ignore){
    Vector2 ret = (Vector2){0, 0};
    for(int i = 0; i < obj_n; ++i) {
        if(idx_ignore == i) continue;
        float dx = obj_a[i].pos_px.x - point.x;
        float dy = obj_a[i].pos_px.y - point.y;
        float rsq = dx * dx + dy * dy;
        const int G = 10e2;
        float mag = G  *  obj_a[i].mass / rsq;
        Vector2 s = Vector2Normalize((Vector2){dx, dy});
        ret.x += mag * s.x;    
        ret.y += mag * s.y;
    }
    return ret;
}
Vector2 calc_g_field_at_point(Vector2 point, obj_s *obj_a, int obj_n){
    return calc_g_field_at_point_ignore_one_obj(point, obj_a, obj_n, -1);
}

void update_objs(obj_s *obj_a, int n, float dt) {
    for(int i = 0; i < n; ++i) {
        //calculate g-field at obj's point
        Vector2 g = calc_g_field_at_point_ignore_one_obj(obj_a[i].pos_px, obj_a, n, i); 
        obj_a[i].pos_px.x += obj_a[i].vel_px_s.x * dt;
        obj_a[i].pos_px.y += obj_a[i].vel_px_s.y * dt;
        obj_a[i].vel_px_s.x += g.x * dt;
        obj_a[i].vel_px_s.y += g.y * dt;
    }
}

int main(void) {
    const float screen_width = 1600;
    const float screen_height = 900;
    const float play_area_width = screen_width * 3.0/4.0f;
    InitWindow(screen_width, screen_height, "gravitation");
    SetTargetFPS(60);
    const int vec_spacing = 15;
    std::vector<obj_s> objects = {};
    enum {
        DISP_FIELDS,
        DISP_VECTORS
    };
    while (!WindowShouldClose()) {
        int num_obj = objects.size();
        static int disp_mode = DISP_FIELDS; 
        BeginDrawing();
           ClearBackground(RAYWHITE);
            static bool show_arrow_stems = false;
            for(float x = 0; x < play_area_width; x += vec_spacing){
                for(float y = 0; y < screen_height; y += vec_spacing){
                    Vector2 g = calc_g_field_at_point((Vector2){x, y}, objects.data(), num_obj);
                    float mag = sqrt(g.x * g.x + g.y * g.y);
                    const int MAX_MAG = 30;
                    if(mag < 0){
                       mag = 0;
                    } else if(mag > MAX_MAG){
                       mag = MAX_MAG;
                    }
                    Vector2 disp = Vector2Normalize(g);
                    Color col = ColorLerp(GREEN, BLUE, mag / MAX_MAG);
                    const int MAX_DIS_LEN = 20;
                    disp = (Vector2){disp.x * mag / MAX_MAG * MAX_DIS_LEN, disp.y * mag / MAX_MAG * MAX_DIS_LEN};
                    draw_vec_end((Vector2){x,y}, disp, col, show_arrow_stems);
                }
            }
            update_objs(objects.data(), num_obj, GetFrameTime());
            for(int i = 0; i < num_obj; ++i) {
                DrawCircleV(objects[i].pos_px, objects[i].rad, RED);
            }

            Vector2 mp = GetMousePosition();
            static bool mouse_out_but_keep_running = false;
            if(CheckCollisionPointRec(mp, (Rectangle){0, 0, play_area_width, screen_height})|| mouse_out_but_keep_running) {
                static float m_sel = 500.0f;
                static float rad_sel = 3.0f;
                float w = GetMouseWheelMove();
                if(IsKeyDown(KEY_LEFT_SHIFT)){
                    int m = (w > 0 ? 1 : -1);
                    rad_sel += m*w*w*100.0f*GetFrameTime();
                } else {
                    m_sel += w*100.0f*GetFrameTime();
                }
                if(m_sel < 0) {
                    m_sel = 0;
                }
                static Vector2 remain;
                static Vector2 vel;
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                    mouse_out_but_keep_running = true;
                    remain = mp;
                } 
                static bool cancelled_til_next_release = false;
                if(mouse_out_but_keep_running) {
                    if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                        mouse_out_but_keep_running = false;
                        cancelled_til_next_release = true;
                    }
                    DrawCircleLinesV(remain, rad_sel, BLACK);
                    DrawText(TextFormat("M: %.1f", m_sel), remain.x - 10.0f, remain.y - 50.0f, 30.0f, BLACK);
                    DrawLineV(remain, mp, GRAY);
                    vel = Vector2Subtract(remain, mp);
                    draw_vec_end(remain, vel, RED, true);
                    float mag = Vector2Length(vel);
                    const char *v = TextFormat("V: %.1f px/s", mag);
                    Vector2 sz = MeasureTextEx(GetFontDefault(), v, 30.0f, 1.0f);
                    const int vmarg = 5;
                    DrawRectangle(mp.x - vmarg, mp.y - 50.0f, sz.x + vmarg*2, sz.y + vmarg*2, RAYWHITE);
                    DrawText(v, mp.x, mp.y - 50.0f, 30.0f, BLACK);
                } else {
                    DrawCircleLinesV(mp, rad_sel, BLACK);
                    DrawText(TextFormat("M: %.1f", m_sel), mp.x - 10.0f, mp.y - 50.0f, 30.0f, BLACK);
                }
                if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                    if(cancelled_til_next_release) {
                        cancelled_til_next_release = false;
                    } else {
                        objects.push_back((obj_s){m_sel, rad_sel, (mouse_out_but_keep_running ? remain : mp), (mouse_out_but_keep_running ? vel : (Vector2){0, 0})});
                        mouse_out_but_keep_running = false;
                    }
                }
            } 

            const float sett_pan_l = screen_width * 1/4.0f;
            enum {
                UI_LAYER_BASE,
                UI_LAYER_DROPDOWN
            };
            static int ui_layer_mode = UI_LAYER_BASE;
            Vector2 scroll = (Vector2){0, 0};
            Rectangle view = (Rectangle){0, 0, 0, 0};
            Rectangle panel = (Rectangle){screen_width - sett_pan_l, 0, sett_pan_l, screen_height};
            GuiScrollPanel(panel, "SETTINGS", 
                    (Rectangle){0,0,sett_pan_l,screen_height*2}, &scroll, &view );
            BeginScissorMode(view.x, view.y, view.width, view.height);
                const float toggle_btns_h = 50.0f;
                const float marg_x = 35.0f;
                const float marg_y = 10.0f;
                const int num_elem = 2;
                Rectangle r = (Rectangle){view.x + marg_x - scroll.x, view.y + marg_y * (num_elem) + toggle_btns_h * (num_elem-1) + scroll.y, view.width - marg_x*2, toggle_btns_h};

                static bool disp_db_open = false;
                bool TEMP_s_a_s = show_arrow_stems;
                GuiToggle(r, "TOGGLE ARROW STEMS", &TEMP_s_a_s);
                if(ui_layer_mode == UI_LAYER_BASE) show_arrow_stems = TEMP_s_a_s;
                r.y -= toggle_btns_h + marg_y;
                if(GuiDropdownBox(r, "FIELD;FORCE_VECTORS", &disp_mode, disp_db_open)) {
                    disp_db_open = !disp_db_open;
                }

                if(disp_db_open) {
                    ui_layer_mode = UI_LAYER_DROPDOWN;
                } else {
                    ui_layer_mode = UI_LAYER_BASE;
                }
            EndScissorMode();
            EndDrawing();
    }

    CloseWindow();

    return 0;
}
