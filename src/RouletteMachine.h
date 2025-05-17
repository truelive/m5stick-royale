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
    float cur_ball_radian = 0.0f;
    int bg_color;

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

    static constexpr float ROTTATION_PER_UPDATE = 1.6f; 
    static const int BALL_RADIUS = 4;
    static constexpr float BALL_INITIAL_RADIAN_PER_UPDATE = (6.0f / 360.0f) * M_PI;  // in radians
};

#endif // ROULETTE_MACHINE_H