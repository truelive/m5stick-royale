#include "SlotMachine.h"

SlotMachine::SlotMachine(const Parameters& params)
        : symbols_data_ptr(params.symbol_pixel_data),
          symbols_weight_ptr(params.symbol_weight_data),
          slot_payout_counts(params.slot_payout_counts),
          slot_payout(params.slot_payout),
          num_symbol_types_count(params.num_types),
          symbol_tile_width(params.sym_w),
          symbol_tile_height(params.sym_h) {
        initialize_grid_randomly();
        animation_win_highlight_update_counter = 0;
        animation_win_highlight_update_flip = false;
        for (int i = 0; i < GRID_WIDTH; ++i) {
            column_vertical_offset[i] = 0;
            column_is_actively_spinning[i] = false;
            column_is_stopping_animation[i] = false;
            column_stop_animation_progress[i] = 0;
            // column_target_symbols will be filled when stop_column_on is called
        }
    }
// Constructor
SlotMachine::SlotMachine(const uint16_t** symbol_pixel_data, const uint16_t* symbol_weight_data, const int* slot_payout_counts, const int (*slot_payout)[4], int num_types, int sym_w, int sym_h)
    : symbols_data_ptr(symbol_pixel_data),
      symbols_weight_ptr(symbol_weight_data),
      slot_payout_counts(slot_payout_counts),
      slot_payout(slot_payout),
      num_symbol_types_count(num_types),
      symbol_tile_width(sym_w),
      symbol_tile_height(sym_h) {
    initialize_grid_randomly();
    animation_win_highlight_update_counter = 0;
    animation_win_highlight_update_flip = false;
    for (int i = 0; i < GRID_WIDTH; ++i) {
        column_vertical_offset[i] = 0;
        column_is_actively_spinning[i] = false;
        column_is_stopping_animation[i] = false;
        column_stop_animation_progress[i] = 0;
        // column_target_symbols will be filled when stop_column_on is called
    }
}

// Destructor
SlotMachine::~SlotMachine() {
    // No dynamic memory allocated by this class itself, so nothing to do here.
    // symbols_data_ptr is managed externally.
}

void SlotMachine::initialize_grid_randomly() {
    for (int col = 0; col < GRID_WIDTH; ++col) {
        column_vertical_offset[col] = 0; // Start with no offset
        // Initialize all GRID_HEIGHT + 1 symbols for the column's reel buffer
        for (int buffer_idx = 0; buffer_idx <= GRID_HEIGHT; ++buffer_idx) {
            // current_reel_symbols[col][0] is above the grid, [1] is top row, etc.
            current_reel_symbols[col][buffer_idx].symbol_index = select_weighted_symbol();
            if (buffer_idx < GRID_HEIGHT) {
                column_win_color[col][buffer_idx] = 0;
            }
        }
    }
}

void SlotMachine::spin_all_columns() {
    is_payout_calculated_flag = false; // Reset the flag
    for (int col = 0; col < GRID_WIDTH; ++col) {
        for(int row = 0; row < GRID_HEIGHT; ++row) {
            column_win_color[col][row] = 0;
        }
    }
    for (int col = 0; col < GRID_WIDTH; ++col) {
        spin_column(col);
    }
}

void SlotMachine::spin_column(int col_index) {
    if (col_index < 0 || col_index >= GRID_WIDTH) return; // Basic bounds check

    column_is_actively_spinning[col_index] = true;
    column_is_stopping_animation[col_index] = false; // Cancel any pending stop for this column
    column_stop_animation_progress[col_index] = 0;
}

void SlotMachine::stop_column_on(int col_index, const int final_symbol_indices[GRID_HEIGHT]) {
    if (col_index < 0 || col_index >= GRID_WIDTH) return;

    // If the column isn't spinning at all, just set the symbols directly.
    if (!column_is_actively_spinning[col_index] && !column_is_stopping_animation[col_index]) {
        set_column_to_final_state(col_index, final_symbol_indices);
        return;
    }

    // If it is spinning, initiate the stopping animation.
    // The column continues to be "actively_spinning" during the stopping animation phase.
    column_is_stopping_animation[col_index] = true;
    column_is_actively_spinning[col_index] = true; // Ensure it keeps spinning
    column_stop_animation_progress[col_index] = 0; // Reset progress for this stop sequence
    for (int row = 0; row < GRID_HEIGHT; ++row) {
        column_target_symbols[col_index][row] = final_symbol_indices[row];
    }
}



void SlotMachine::stop_all_columns_on(const int final_results[GRID_WIDTH][GRID_HEIGHT]) {
    for (int col = 0; col < GRID_WIDTH; ++col) {
        stop_column_on(col, final_results[col]);
    }
}

void SlotMachine::stop_all_columns_with_weighted_random_result() {
    int final_results[GRID_WIDTH][GRID_HEIGHT];
    for (int col = 0; col < GRID_WIDTH; ++col) {
        for (int row = 0; row < GRID_HEIGHT; ++row) {
            final_results[col][row] = select_weighted_symbol();
        }
    }
    this->stop_all_columns_on(final_results);
}

void SlotMachine::set_column_to_final_state(int col_index, const int final_symbols[GRID_HEIGHT]) {
    column_vertical_offset[col_index] = 0;
    // Set the symbol above the top visible row (randomly or could be a specific "pre-target" symbol)
    current_reel_symbols[col_index][0].symbol_index = random(0, num_symbol_types_count);

    // Set the symbols for the visible grid rows
    for (int r = 0; r < GRID_HEIGHT; ++r) {
        if (final_symbols[r] >= 0 && final_symbols[r] < num_symbol_types_count) {
            current_reel_symbols[col_index][r + 1].symbol_index = final_symbols[r];
        }
    }
}

void SlotMachine::update_controls(bool btnA_pressed, bool btnB_pressed, bool btnC_pressed) {
  unsigned long tick = millis(); // Get the current heartbeat value
  bool is_any_column_spinns = is_any_column_spinning();
  bool is_any_column_stopps = is_any_column_stopping();

  if (btnA_pressed && !is_auto_spin)
  {
    if (is_any_column_spinns)
    {
      // If it's spinning, let's stop it with random symbols
      last_spin_stop = tick;
      stop_all_columns_with_weighted_random_result();
    }
    else
    {
      // If it's not spinning, let's start spinning all columns
      balance--;
      last_spin = tick;
      spin_all_columns();
    }
  }
  if (!is_payout_calculated() && !is_any_column_spinns)
  {
    last_payout = calculate_payout();
    balance += last_payout;
  }

  if (btnB_pressed)
  {
    is_auto_spin = !is_auto_spin;
  }
  if (is_auto_spin && !is_any_column_spinns && !is_any_column_stopps && tick - last_spin_stop > MS_TO_AUTO_SPIN)
  {
    balance--;
    last_spin = tick;
    spin_all_columns();
  }
  if (tick - last_spin > MS_TO_AUTO_STOP_SPIN && is_any_column_spinns && !is_any_column_stopps)
  {
    last_spin_stop = tick;
    stop_all_columns_with_weighted_random_result();
  }
}

void SlotMachine::update(bool btnA_pressed, bool btnB_pressed, bool btnC_pressed) {
    update_controls(btnA_pressed, btnB_pressed, btnC_pressed);

    animation_win_highlight_update_counter = (animation_win_highlight_update_counter + 1) % UPDATES_PER_WIN_HIGLIGHT;
    if (animation_win_highlight_update_counter == 0) {
        animation_win_highlight_update_flip = !animation_win_highlight_update_flip;
    }
    for (int col = 0; col < GRID_WIDTH; ++col) {
        if (column_is_actively_spinning[col]) {
            column_vertical_offset[col] += SPIN_SPEED_PIXELS_PER_UPDATE;

            if (column_vertical_offset[col] >= symbol_tile_height) {
                column_vertical_offset[col] %= symbol_tile_height; // Wrap offset

                // Shift symbols down (new symbol appears at the top of the reel buffer)
                for (int r = GRID_HEIGHT; r > 0; --r) { // r from GRID_HEIGHT down to 1
                    current_reel_symbols[col][r].symbol_index = current_reel_symbols[col][r - 1].symbol_index;
                }
                // New symbol for the top-most position (current_reel_symbols[col][0])
                current_reel_symbols[col][0].symbol_index = select_weighted_symbol();
            }

            if (column_is_stopping_animation[col]) {
                column_stop_animation_progress[col]++;
                if (column_stop_animation_progress[col] >= STOPPING_ANIMATION_DURATION_UPDATES) {
                    // Stopping animation duration reached, set to target symbols
                    set_column_to_final_state(col, column_target_symbols[col]);
                    column_is_actively_spinning[col] = false;   // Fully stopped
                    column_is_stopping_animation[col] = false;  // No longer in stopping phase
                    column_stop_animation_progress[col] = 0;    // Reset for next use
                }
            }
        }
    }
}

void SlotMachine::draw(M5Canvas& canvas, int top_left_x, int top_left_y, uint16_t transparent_color) {
    for (int col = 0; col < GRID_WIDTH; ++col) {
        int base_screen_x = top_left_x + col * symbol_tile_width;
        for (int buffer_idx = 0; buffer_idx <= GRID_HEIGHT; ++buffer_idx) { // Draw GRID_HEIGHT + 1 symbols
            int symbol_idx = current_reel_symbols[col][buffer_idx].symbol_index;
            // Ensure symbol_idx is valid before trying to access symbols_data_ptr
            if (symbol_idx >= 0 && symbol_idx < num_symbol_types_count && symbols_data_ptr[symbol_idx] != nullptr) {
                if (buffer_idx != 0 && column_win_color[col][buffer_idx-1] != 0) {
                    auto hl_color = TFT_SKYBLUE;
                    if (animation_win_highlight_update_flip) {
                        hl_color = TFT_DARKCYAN;
                    }
                    canvas.fillRect(
                        base_screen_x,
                        top_left_y + (buffer_idx - 1) * symbol_tile_height + column_vertical_offset[col],
                        symbol_tile_width,
                        symbol_tile_height,
                        hl_color
                    );
                }
                canvas.pushImage(
                    base_screen_x,
                    // current_reel_symbols[col][0] is above the grid, [1] is top row, etc.
                    // y_pos = top_of_slot_machine_display + (which_row_in_buffer_relative_to_top_slot - 1)*symbol_height + offset
                    top_left_y + (buffer_idx - 1) * symbol_tile_height + column_vertical_offset[col],
                    symbol_tile_width,
                    symbol_tile_height,
                    symbols_data_ptr[symbol_idx],
                    transparent_color
                );
            }
        }
    }
    draw_weights_table(canvas);
}

void SlotMachine::draw_weights_table(M5Canvas& canvas)
{
  int x = canvas.width() / 2 - 7;
  int y = 0;
  int cell_width = 25;
  int cell_height = 15;
  canvas.setTextSize(1);
  canvas.setFont(&fonts::Font0);

  canvas.setTextColor(TFT_BLACK, 0xfdcf);
  canvas.setTextDatum(MC_DATUM);
  for (int i = -1; i < num_symbol_types_count; i++)
  {
    if (i == -1)
    {
      for (int j = 0; j < 4; j++)
      {
        canvas.drawString(String(slot_payout_counts[j]), x + cell_width / 2, y + cell_height / 2);
        x += cell_width;
      }
      x = canvas.width() / 2 - 7;
      y += cell_height;
      continue;
    }
    if (i == 1 || i == 3 || i == 4 || i == 5)
    {
      continue;
    }
    for (int j = 0; j < 4; j++)
    {
      canvas.drawRect(x, y, cell_width, cell_height, TFT_BLACK);
      canvas.drawString(String(slot_payout[i][j]), x + cell_width / 2, y + cell_height / 2);
      x += cell_width;
    }
    canvas.pushImage(x, y, symbol_tile_width, symbol_tile_height, symbols_data_ptr[i], TFT_WHITE);
    x = canvas.width() / 2 - 7;
    y += cell_height;
  }
  canvas.pushImage(x, y, symbol_tile_width, symbol_tile_height, symbols_data_ptr[1], TFT_WHITE);
  x += cell_width * 2;
  canvas.drawString(String(slot_payout_counts[0]) + ":" + String(slot_payout[1][0]) + " " + String(slot_payout_counts[1]) + ":" + String(slot_payout[1][1]),
                         x + cell_width / 2, y + cell_height / 2);
  y += cell_height;
  canvas.drawString(String(slot_payout_counts[2]) + ":" + String(slot_payout[1][2]) + " " + String(slot_payout_counts[3]) + ":" + String(slot_payout[1][3]),
                         x + cell_width / 2, y + cell_height / 2);
}

bool SlotMachine::is_payout_calculated() const {
    return is_payout_calculated_flag;
}

bool SlotMachine::is_any_column_stopping() const {
    for (int col = 0; col < GRID_WIDTH; ++col) {
        if (column_is_stopping_animation[col]) {
            return true;
        }
    }
    return false;
}

bool SlotMachine::is_any_column_spinning() const {
    for (int col = 0; col < GRID_WIDTH; ++col) {
        if (column_is_actively_spinning[col] || column_is_stopping_animation[col]) {
            return true;
        }
    }
    return false;
}

bool SlotMachine::is_column_spinning(int col_index) const {
    if (col_index < 0 || col_index >= GRID_WIDTH) return false;
    return column_is_actively_spinning[col_index] || column_is_stopping_animation[col_index];
}

int SlotMachine::select_weighted_symbol() {
    int total_weight = 0;

    for (int i = 0; i < num_symbol_types_count; i++) {
        total_weight += symbols_weight_ptr[i];
    }

    if (total_weight <= 0) {
        // Avoid division by zero or issues if all weights are zero
        // Return a default or handle error appropriately
        fprintf(stderr, "Error: Total weight is zero or negative.\n");
        if (num_symbol_types_count > 0) {
            return 0; // Default to first symbol
        }
        return -1;
    }

    // Generate a random number between 0 and total_weight - 1
    int random_number = rand() % total_weight;

    // Find the symbol corresponding to the random number
    int cumulative_weight = 0;
    for (int i = 0; i < num_symbol_types_count; i++) {
        cumulative_weight += symbols_weight_ptr[i];
        if (random_number < cumulative_weight) {
            return i; // Return the name of the selected symbol
        }
    }

    // Should not be reached if total_weight > 0 and weights are non-negative
    // but as a fallback:
    fprintf(stderr, "Error: Failed to select a symbol (should not happen).\n");
    if (num_symbol_types_count > 0) {
        return num_symbol_types_count - 1; // Default to last symbol
    }
    return 0;
}

int SlotMachine::calculate_payout() {
    // Ensure reels are not spinning. If they are, no payout.
    // This also implies that column_vertical_offset should be 0 for all stopped columns.
    if (is_any_column_spinning()) {
        return 0;
    }

    int symbol_counts[num_symbol_types_count];
    for (int i = 0; i < num_symbol_types_count; ++i) {
        symbol_counts[i] = 0;
    }

    // Count symbols on the visible grid.
    // The visible grid symbols are in current_reel_symbols[col][1] through current_reel_symbols[col][GRID_HEIGHT].
    for (int col = 0; col < GRID_WIDTH; ++col) {
        // Double-check if a specific column might still have an offset even if not "actively_spinning"
        // For this payout logic, we assume a truly stopped grid means offsets are zero.
        if (column_vertical_offset[col] != 0) {
            // This state (stopped but with offset) shouldn't occur if is_any_column_spinning() is false
            // and set_column_to_final_state correctly zeroes the offset.
            // Consider returning an error or 0 if the grid isn't perfectly settled.
            return 0; // Or log an error: grid not settled for payout calculation.
        }
        for (int row_in_grid = 0; row_in_grid < GRID_HEIGHT; ++row_in_grid) {
            // The symbol in the visible grid at [col][row_in_grid] is current_reel_symbols[col][row_in_grid + 1]
            int symbol_index = current_reel_symbols[col][row_in_grid + 1].symbol_index;
            column_win_color[col][row_in_grid] = 0;
            if (symbol_index >= 0 && symbol_index < num_symbol_types_count) {
                symbol_counts[symbol_index]++;
            }
        }
    }

    int total_payout = 0;
    int total_wins = 0;
    int symbol_wins[num_symbol_types_count];
    for (int i = 0; i < num_symbol_types_count; ++i) {
        symbol_wins[i] = 0;
    }

    for (int symbol_idx = 0; symbol_idx < num_symbol_types_count; ++symbol_idx) {
        int count = symbol_counts[symbol_idx];
        if (count <= 4) continue; // No payout for 4 or fewer symbols

        for (int i = 3; i >= 0; --i) { // Check against PAYOUT_RELEVANT_COUNTS {5, 7, 9, 10}
            if (count >= slot_payout_counts[i] && slot_payout[symbol_idx][i] > 0) {
                total_wins++;
                symbol_wins[symbol_idx] = total_wins;
                total_payout += slot_payout[symbol_idx][i];
                break; // Found the payout for this symbol and count, move to next symbol
            }
        }
    }

    for (int col = 0; col < GRID_WIDTH; ++col) {
        for (int row_in_grid = 0; row_in_grid < GRID_HEIGHT; ++row_in_grid) {
            // The symbol in the visible grid at [col][row_in_grid] is current_reel_symbols[col][row_in_grid + 1]
            int symbol_index = current_reel_symbols[col][row_in_grid + 1].symbol_index;
            column_win_color[col][row_in_grid] = symbol_wins[symbol_index];
        }
    }

    is_payout_calculated_flag = true;
    return total_payout;
}