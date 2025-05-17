#include "DiceMachine.h"
#include <Arduino.h> // For random()

// Define the static member for bet type strings
const char* DiceMachine::bet_type_strings[2] = {"Even", "Uneven"};

DiceMachine::DiceMachine(const Parameters &params)
    : symbols_data_ptr(params.symbol_pixel_data),
      symbol_tile_width(params.sym_w),
      symbol_tile_height(params.sym_h),
      is_rolling(false),
      current_bet_type(BetType::EVEN), // Default bet
      last_sum_was_even(false),
      show_last_sum_result(false),
      bg_color(params.bg_color)
{
  // Initialize dice to random faces
  // randomSeed(analogRead(0)); // Optional: seed random number generator if not done elsewhere
  current_dice1_idx = random(NUM_DICE_FACES); // random(6) gives 0-5
  current_dice2_idx = random(NUM_DICE_FACES);
  // Ensure random is seeded, M5.begin() usually does this.
  // If not, uncomment randomSeed(analogRead(A0)) or similar in your main setup.
}

DiceMachine::~DiceMachine()
{
}

void DiceMachine::update(bool btnA_pressed, bool btnB_pressed, bool btnC_pressed)
{
  int payout = 0;

  // Detect a press (rising edge) of button A
  if (btnA_pressed)
  {
    is_rolling = !is_rolling; // Toggle rolling state
    if (!is_rolling) { // Just stopped rolling
      // Calculate sum (dice faces are 1-6, indices are 0-5)
      int dice1_value = current_dice1_idx + 1;
      int dice2_value = current_dice2_idx + 1;
      int sum = dice1_value + dice2_value;
      last_sum_was_even = (sum % 2 == 0);
      show_last_sum_result = true;

      // Check for win
      if ((last_sum_was_even && current_bet_type == BetType::EVEN) ||
          (!last_sum_was_even && current_bet_type == BetType::UNEVEN))
      {
        balance += 2;
        last_payout = 2;
      }
    } else { // Just started rolling
        show_last_sum_result = false; // Hide previous result when starting a new roll
        balance--;
        last_payout = 0;
    }
  }

  // Detect a press (rising edge) of button B to change bet type
  // Allow changing bet only when not rolling
  if (!is_rolling && btnB_pressed)
  {
    current_bet_type = (current_bet_type == BetType::EVEN) ? BetType::UNEVEN : BetType::EVEN;
    show_last_sum_result = false; // Hide result if bet changes
  }
}

void DiceMachine::draw(M5Canvas &canvas, int top_left_x, int top_left_y, uint16_t transparent_color)
{
  // Ensure indices are valid (though random should keep them in range)
  if (current_dice1_idx < 0 || current_dice1_idx >= NUM_DICE_FACES ||
      current_dice2_idx < 0 || current_dice2_idx >= NUM_DICE_FACES)
  {
    // Handle error or default to a known good state if necessary
    // For now, we assume symbols_data_ptr has at least NUM_DICE_FACES elements
    return;
  }

  // Clear the canvas area or relevant part if needed, or rely on background redraw
  // canvas.fillRect(top_left_x, top_left_y, symbol_tile_width * 2 + DICE_SPACING + 50, symbol_tile_height + 20, TFT_BLACK); // Example clear

  if (is_rolling)
  {
    // If rolling, select new random faces for the dice
    current_dice1_idx = random(NUM_DICE_FACES);
    current_dice2_idx = random(NUM_DICE_FACES);
    show_last_sum_result = false; // Don't show old results while rolling
  }

  // Draw the first die
  canvas.pushImage(top_left_x, top_left_y, symbol_tile_width, symbol_tile_height, symbols_data_ptr[current_dice1_idx], transparent_color);

  // Draw the second die next to the first one, with spacing
  canvas.pushImage(top_left_x + symbol_tile_width + DICE_SPACING, top_left_y, symbol_tile_width, symbol_tile_height, symbols_data_ptr[current_dice2_idx], transparent_color);

  // Display the current bet type
  canvas.setTextSize(2); // Adjust size as needed
  canvas.setTextColor(TFT_BLACK, bg_color); // White text, black background for clarity
  int text_y_pos = top_left_y + symbol_tile_height + 10; // Position below the dice
  canvas.setCursor(top_left_x, text_y_pos);
  canvas.print("Bet: ");
  canvas.print(bet_type_strings[current_bet_type]);

  // Display the result of the last roll if applicable
  if (show_last_sum_result && !is_rolling) {
      canvas.setCursor(top_left_x, text_y_pos + 20); // Below the bet type
      canvas.print("Sum was: ");
      canvas.print(last_sum_was_even ? "Even" : "Uneven");
  }
}