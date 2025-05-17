#ifndef ROULETTE_MACHINE_H
#define ROULETTE_MACHINE_H

#include <M5Unified.h> // Required for M5Canvas and random()

class RouletteMachine
{
private:
    M5Canvas *sub_canvas; // Pointer to the array of symbol image data
    const uint16_t *roulette_numbers;
    const float *roulette_degrees;
    int num_symbols;
    int symbol_tile_width;
    int symbol_tile_height;
    unsigned long last_payout = 0;
    unsigned long balance = 100;
    float cur_degree = 0.0f;
    int bg_color;
    float cur_ball_radian = M_PI / 2.0f; // Start ball at top for example

    enum RouletteState {
        IDLE,           // Waiting for player to start
        SPINNING,       // Wheel and ball are spinning freely
        BALL_SETTLING,  // Player pressed button, ball is slowing down and spiraling in
        SHOWING_RESULT  // Ball has landed, showing the outcome
    };
    RouletteState current_state = IDLE;

    enum PlayerChoise {
        RED,
        BLACK,
        EVEN,
        UNEVEN,
        ZERO,
        SIX,TWELVE,EIGHTEEN
    };

    PlayerChoise players_number;

    float current_wheel_speed_dpu; // Degrees per update for wheel
    float current_ball_angular_speed_rpu; // Radians per update for ball's own rotation
    float current_ball_orbit_radius;

    unsigned long animation_start_time_ms;
    unsigned long spinning_start_time_ms;
    unsigned long count = 0;
    unsigned long num_spins = 0;
    static const unsigned long ANIMATION_DURATION_MS = 3500; // Duration of settling animation
    static const unsigned long MS_TO_AUTO_STOP_SPIN = 1200;


    int target_number_index;       // Index of the landed number
    int selected_number_value;     // Actual value of the landed number
    bool is_payout_calculated_flag = false;

    // Constants for initial state (can be adjusted)
    // These will be initialized from existing ROTTATION_PER_UPDATE and BALL_INITIAL_RADIAN_PER_UPDATE if they exist
    float initial_wheel_speed_dpu;
    float initial_ball_angular_speed_rpu;
    float initial_ball_orbit_radius_offset; // How far from wheel edge ball starts
    float final_ball_radius_on_numbers; // Radius where ball rests on numbers


public:
    struct Parameters
    {
        M5Canvas *sub_canvas;
        const uint16_t *roulette_numbers;
        const float *roulette_degrees;
        int num_symbols;
        int sym_w;
        int sym_h;
        int bg_color;
        float initial_wheel_speed; // ROTTATION_PER_UPDATE
        float initial_ball_speed;  // BALL_INITIAL_RADIAN_PER_UPDATE
    };
    RouletteMachine(const Parameters &params);
    ~RouletteMachine();

    void update(bool btnA_pressed, bool btnB_pressed, bool btnC_pressed); // Call this every frame to update animation states

    // Draws the current state of the slot machine onto the provided canvas.
    // transparent_color is used for pushImage transparency (e.g., TFT_WHITE).
    void draw(M5Canvas &canvas, int top_left_x, int top_left_y, uint16_t transparent_color = 0xFFFF);
    unsigned long get_last_payout() const { return last_payout; }
    unsigned long get_balance() const { return balance; }
    void rotate_wheel();
    float trim_radian(float radian);
    int determine_payout();

    //static constexpr float ROTTATION_PER_UPDATE = 1.6f; 
    static const int BALL_RADIUS = 4;
    //static constexpr float BALL_INITIAL_RADIAN_PER_UPDATE = (6.0f / 360.0f) * M_PI;  // in radians
};

#endif // ROULETTE_MACHINE_H