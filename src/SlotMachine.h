#ifndef SLOT_MACHINE_H
#define SLOT_MACHINE_H

#include <M5Unified.h> // Required for M5Canvas and random()

// Defines a single tile in the slot machine grid
struct Tile {
    int symbol_index; // Index into the provided array of symbol image data
};

class SlotMachine {
public:
    static const int GRID_WIDTH = 5;
    static const int GRID_HEIGHT = 5;

    // Constructor:
    // tile_symbols: Array of pointers to image data for each symbol type (e.g., your slot_symbols).
    // num_types: Number of different symbols (e.g., 8).
    // sym_w: Width of a single symbol image (e.g., SYM_WIDTH).
    // sym_h: Height of a single symbol image (e.g., SYM_HEIGHT).
    SlotMachine(const uint16_t** symbol_pixel_data, const uint16_t* symbol_weight_data, const int* slot_payout_counts, const int (*slot_payout)[4],  int num_types, int sym_w, int sym_h);
    ~SlotMachine();

    void spin_all_columns(); // Starts spinning all columns
    void spin_column(int col_index); // Starts spinning a specific column

    // Stops a specific column, making it land on the symbols provided in final_symbol_indices.
    // final_symbol_indices should be an array of GRID_HEIGHT integers (symbol indices).
    void stop_column_on(int col_index, const int final_symbol_indices[GRID_HEIGHT]);

    // Stops all columns. final_results should be a GRID_WIDTH x GRID_HEIGHT array of symbol indices.
    void stop_all_columns_on(const int final_results[GRID_WIDTH][GRID_HEIGHT]);

    // Generates a weighted random result for all columns and then initiates stopping.
    void stop_all_columns_with_weighted_random_result();

    void update(); // Call this every frame to update animation states

    // Draws the current state of the slot machine onto the provided canvas.
    // transparent_color is used for pushImage transparency (e.g., TFT_WHITE).
    void draw(M5Canvas& canvas, int top_left_x, int top_left_y, uint16_t transparent_color = 0xFFFF);

    // Calculates the payout based on the current grid state and a defined paytable.
    // Should be called when reels are stopped.
    int calculate_payout();

    bool is_payout_calculated() const;
    bool is_any_column_stopping() const; // Checks if any column is in stopping animation
    bool is_any_column_spinning() const; // Checks if any column is currently spinning or in stopping animation
    bool is_column_spinning(int col_index) const; // Checks if a specific column is spinning or stopping

private:
    // Stores the symbols currently in or entering the viewport for each column.
    // The +1 is for the symbol scrolling in from the top.
    // [col][0] is the symbol just above the visible grid, scrolling in.
    // [col][1] is the symbol for the first visible row.
    // [col][GRID_HEIGHT] is the symbol for the last visible row.
    Tile current_reel_symbols[GRID_WIDTH][GRID_HEIGHT + 1];
    const uint16_t** symbols_data_ptr; // Pointer to the array of symbol image data
    const uint16_t* symbols_weight_ptr; // Pointer to the array of symbol weight data
    const int* slot_payout_counts;
    const int (*slot_payout)[4];
    int num_symbol_types_count;
    int symbol_tile_width;
    int symbol_tile_height;

    int column_vertical_offset[GRID_WIDTH]; // Vertical pixel offset for each column (0 to symbol_tile_height-1)
    // Animation state per column
    bool column_is_actively_spinning[GRID_WIDTH];    // True if column is in fast spin mode
    bool column_is_stopping_animation[GRID_WIDTH];   // True if column is in the "slowing down" phase of stopping
    int column_spin_update_counter[GRID_WIDTH];      // Counter to control spin speed
    int column_target_symbols[GRID_WIDTH][GRID_HEIGHT]; // Symbols to land on for each column
    int column_win_color[GRID_WIDTH][GRID_HEIGHT]; // Background color for winning combinations
    int column_stop_animation_progress[GRID_WIDTH];  // Counter for the duration of the stopping animation
    bool is_payout_calculated_flag;
    int animation_win_highlight_update_counter;
    bool animation_win_highlight_update_flip;


    // Animation parameters
    static const int SPIN_SPEED_PIXELS_PER_UPDATE = 14; // Pixels to scroll per update() call
    static const int STOPPING_ANIMATION_DURATION_UPDATES = 45; // How many update() calls the "stopping" phase lasts before snapping
    static const int UPDATES_PER_WIN_HIGLIGHT = 30;

    void initialize_grid_randomly(); // Sets initial random symbols on the grid
    void set_column_to_final_state(int col_index, const int final_symbols[GRID_HEIGHT]); // Helper to set a column to its target symbols
    int select_weighted_symbol();
};

#endif // SLOT_MACHINE_H