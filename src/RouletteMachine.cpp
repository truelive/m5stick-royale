#include "RouletteMachine.h"
#include <Arduino.h> // For radians()

RouletteMachine::RouletteMachine(const Parameters &params)
    : sub_canvas(params.sub_canvas),
      roulette_numbers(params.roulette_numbers),
      roulette_degrees(params.roulette_degrees),
      symbol_tile_width(params.sym_w),
      symbol_tile_height(params.sym_h),
      bg_color(params.bg_color)
{
}

RouletteMachine::~RouletteMachine()
{
}

void RouletteMachine::update(bool btnA_pressed, bool btnB_pressed, bool btnC_pressed) {
    rotate_wheel();
    cur_ball_radian = cur_ball_radian - BALL_INITIAL_RADIAN_PER_UPDATE;
    if (cur_ball_radian > 2*M_PI) {
        cur_ball_radian = cur_ball_radian - 2*M_PI;
    }
    
}
void RouletteMachine::draw(M5Canvas &canvas, int top_left_x, int top_left_y, uint16_t transparent_color)
{   
    int rot_center_x = top_left_x + symbol_tile_width/2;
    int rot_center_y = top_left_y + symbol_tile_height/2;
    int rot_radius = symbol_tile_width/2;

    // Draw the rotating wheel
    sub_canvas->pushRotateZoom(&canvas, rot_center_x, rot_center_y, cur_degree, 1.0f, 1.0f, TFT_PINK);

    // Calculate ball's position to rotate opposite to the wheel
    // Assume BALL_RADIUS is defined elsewhere (e.g., in RouletteMachine.h or as a global constant)
    // The ball will orbit at a radius slightly less than the wheel's radius.
    // Let's use a similar radial distance as implied by the original fixed position.
    float ball_orbit_r = rot_radius - BALL_RADIUS * 2.0f; 
    // Ensure the orbit radius is not too small, at least the ball's own radius
    if (ball_orbit_r < BALL_RADIUS) {
        ball_orbit_r = BALL_RADIUS;
    }
    int ball_x = rot_center_x + ball_orbit_r * cos(cur_ball_radian);
    int ball_y = rot_center_y + ball_orbit_r * sin(cur_ball_radian);
    canvas.fillCircle(ball_x, ball_y, BALL_RADIUS, TFT_WHITE);
}

void RouletteMachine::rotate_wheel() {
    cur_degree += ROTTATION_PER_UPDATE;
    if (cur_degree > 360.0f) {
        cur_degree = cur_degree - 360.0f;
    }
}
