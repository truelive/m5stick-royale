#ifndef DICE_MACHINE_H
#define DICE_MACHINE_H

#include <M5Unified.h> // Required for M5Canvas and random()

class DiceMachine
{
public:
    struct Parameters
    {
        const uint16_t **symbol_pixel_data;
        int num_types;
        int sym_w;
        int sym_h;
        int bg_color;
    };
    enum BetType
    {
        EVEN,
        UNEVEN
    };
    DiceMachine(const Parameters &params);
    ~DiceMachine();

    void update(bool btnA_pressed, bool btnB_pressed, bool btnC_pressed); // Call this every frame to update animation states

    // Draws the current state of the slot machine onto the provided canvas.
    // transparent_color is used for pushImage transparency (e.g., TFT_WHITE).
    void draw(M5Canvas &canvas, int top_left_x, int top_left_y, uint16_t transparent_color = 0xFFFF);

    unsigned long get_last_payout() const { return last_payout; }
    unsigned long get_balance() const { return balance; }

private:
    const uint16_t **symbols_data_ptr; // Pointer to the array of symbol image data
    int symbol_tile_width;
    int symbol_tile_height;
    unsigned long last_payout = 0;
    unsigned long balance = 100;
    int bg_color;
    bool is_rolling;
    int current_dice1_idx;
    int current_dice2_idx;
    BetType current_bet_type;
    bool last_sum_was_even;    // To store the result of the last roll
    bool show_last_sum_result; // Flag to indicate if we should display the last sum result

    static const int NUM_DICE_FACES = 6; // Standard dice have 6 faces
    static const int DICE_SPACING = 5;   // Spacing between the two dice when drawn
    static const char *bet_type_strings[2];
};

#endif // DICE_MACHINE_H