#include "RouletteMachine.h"
#include <Arduino.h> // For radians()

// Define BALL_RADIUS if not defined elsewhere (e.g. 5 pixels)
const int BALL_RADIUS = 4;
// ROTTATION_PER_UPDATE and BALL_INITIAL_RADIAN_PER_UPDATE will be passed via Parameters

RouletteMachine::RouletteMachine(const Parameters &params)
    : sub_canvas(params.sub_canvas),
      roulette_numbers(params.roulette_numbers),
      roulette_degrees(params.roulette_degrees),
      num_symbols(params.num_symbols),
      symbol_tile_width(params.sym_w),
      symbol_tile_height(params.sym_h),
      bg_color(params.bg_color),
      initial_wheel_speed_dpu(params.initial_wheel_speed),
      initial_ball_angular_speed_rpu(params.initial_ball_speed)
{
    current_state = IDLE;
    cur_degree = 0.0f;
    cur_ball_radian = M_PI / 2.0f; // Start ball at top (0, -r) in math coordinates

    // Calculate initial and final ball orbit radii based on wheel size
    float wheel_radius = symbol_tile_width / 2.0f;
    initial_ball_orbit_radius_offset = BALL_RADIUS * 3.0f; // How far from wheel edge ball starts
    // Place final ball position on the number band, e.g., 85% of wheel radius
    final_ball_radius_on_numbers = wheel_radius * 0.58f;
    if (final_ball_radius_on_numbers < BALL_RADIUS)
        final_ball_radius_on_numbers = BALL_RADIUS;

    current_ball_orbit_radius = wheel_radius - initial_ball_orbit_radius_offset;
    if (current_ball_orbit_radius < BALL_RADIUS)
        current_ball_orbit_radius = BALL_RADIUS;
    cur_degree = random(0, 3600) / 10.0f;        // Start wheel at a random position
    cur_ball_radian = random(0, 6283) / 1000.0f; // Start ball at a random radian

    current_wheel_speed_dpu = initial_wheel_speed_dpu;
    current_ball_angular_speed_rpu = initial_ball_angular_speed_rpu;
    players_number = RED;
}

RouletteMachine::~RouletteMachine()
{
}

void RouletteMachine::update(bool btnA_pressed, bool btnB_pressed, bool btnC_pressed)
{
    unsigned long current_time_ms = millis();
    cur_degree += current_wheel_speed_dpu;
    if (cur_degree > 360.0f)
        cur_degree -= 360.0f;

    cur_ball_radian -= current_ball_angular_speed_rpu;
    cur_ball_radian = trim_radian(cur_ball_radian);

    switch (current_state)
    {
    case IDLE:
        if (btnA_pressed || num_spins > 0)
        {
            is_payout_calculated_flag = false;
            num_spins++;
            current_state = SPINNING;
            balance--; // Cost to play
            last_payout = 0;
            current_ball_angular_speed_rpu = initial_ball_angular_speed_rpu;
            current_ball_orbit_radius = symbol_tile_width / 2.0f - initial_ball_orbit_radius_offset;
            spinning_start_time_ms = current_time_ms;
        }
        if (btnB_pressed)
        {
            players_number = (PlayerChoise)((players_number + 1) % 8);
        }
        break;

    case SPINNING:
    {
        unsigned long spinning_duration_ms = current_time_ms - spinning_start_time_ms;
        if (btnA_pressed || spinning_duration_ms > MS_TO_AUTO_STOP_SPIN)
        {
            current_state = BALL_SETTLING;
            animation_start_time_ms = current_time_ms;
            if (num_symbols > 0)
            {
                target_number_index = random(num_symbols);
                selected_number_value = roulette_numbers[target_number_index];
            }
            else
            { // Fallback if no symbols
                target_number_index = 0;
                selected_number_value = 0;
            }
        }
    }
    break;

    case BALL_SETTLING:
        float elapsed_time_ms = current_time_ms - animation_start_time_ms;
        float animation_progress = 0.0f;
        if (ANIMATION_DURATION_MS > 0)
        {
            animation_progress = elapsed_time_ms / (float)ANIMATION_DURATION_MS;
        }

        if (animation_progress >= 1.0f)
        {
            animation_progress = 1.0f;
        }

        float number_angle_on_wheel_rad = radians(roulette_degrees[target_number_index] - 95.0f);
        float target_ball_radian = radians(cur_degree) + number_angle_on_wheel_rad;

        current_ball_orbit_radius = final_ball_radius_on_numbers - final_ball_radius_on_numbers / 2 * sin(current_time_ms / 100.0f) * (1.0f - animation_progress);

        // Slow down ball's independent angular speed
        current_ball_angular_speed_rpu = initial_ball_angular_speed_rpu * (1.0f - animation_progress);
        if (current_ball_angular_speed_rpu < 0)
            current_ball_angular_speed_rpu = 0; // Adjust ball's radian to move towards the target
        float delta_radian = trim_radian(target_ball_radian - cur_ball_radian);

        // Adjust ball's radian to move towards the target
        cur_ball_radian += delta_radian * animation_progress * 0.15f;
        cur_ball_radian = trim_radian(cur_ball_radian);

        if (animation_progress >= 1.0f)
        {
            if (btnA_pressed)
            {
                current_state = IDLE;
            }
            // The ball's final screen angle is the wheel's final angle + the number's angle on the wheel sprite
            cur_ball_radian = trim_radian(target_ball_radian);

            // Snap to final position precisely
            current_ball_orbit_radius = final_ball_radius_on_numbers;
            if (!is_payout_calculated_flag)
            {
                is_payout_calculated_flag = true;
                int amount = determine_payout();
                balance += amount;
                last_payout = amount;
            }
            if (btnB_pressed)
            {
                players_number = (PlayerChoise)((players_number + 1) % 8);
            }
        }
        break;
    }
}

int RouletteMachine::determine_payout()
{
    if (players_number == ZERO && selected_number_value == 0)
    {
        return 35;
    }
    if (players_number == RED && target_number_index > 0 && target_number_index % 2 == 1)
    {
        return 2;
    }

    if (players_number == BLACK && target_number_index > 0 && target_number_index % 2 == 0)
    {
        return 2;
    }

    if (players_number == EVEN && selected_number_value % 2 == 0)
    {
        return 2;
    }

    if (players_number == UNEVEN && selected_number_value % 2 == 1)
    {
        return 2;
    }
    if (players_number == SIX && selected_number_value > 0 && selected_number_value < 7)
    {
        return 4;
    }
    if (players_number == TWELVE && selected_number_value > 6 && selected_number_value < 13)
    {
        return 4;
    }
    if (players_number == EIGHTEEN && selected_number_value > 12 && selected_number_value < 19)
    {
        return 4;
    }
    return 0;
}

void RouletteMachine::draw(M5Canvas &canvas, int top_left_x, int top_left_y, uint16_t transparent_color)
{
    int wheel_center_x = top_left_x + symbol_tile_width / 2;
    int wheel_center_y = top_left_y + symbol_tile_height / 2;

    // Draw the rotating wheel
    sub_canvas->pushRotateZoom(&canvas, wheel_center_x, wheel_center_y, cur_degree, 1.0f, 1.0f, TFT_PINK); // Assuming TFT_PINK is transparent for wheel sprite

    // Draw the ball
    int ball_x = wheel_center_x + current_ball_orbit_radius * cos(cur_ball_radian);
    int ball_y = wheel_center_y + current_ball_orbit_radius * sin(cur_ball_radian);
    canvas.fillCircle(ball_x, ball_y, BALL_RADIUS, TFT_WHITE);

    // draw a bid selection
    const int num_choices = 8; // Corresponds to PlayerChoise enum: RED, BLACK, EVEN, UNEVEN, ZERO
    // Ensure these colors and names correspond to the PlayerChoise enum order
    uint16_t choice_colors[] = {TFT_RED, TFT_BLACK, TFT_DARKGREEN, TFT_DARKGRAY, TFT_GREEN, TFT_DARKCYAN, TFT_DARKCYAN, TFT_DARKCYAN};
    const char *choice_names[] = {"R", "B", "E", "U", "0", "1-6", "7-12", "13-18"}; // Short names for display

    // --- Cone dimensions and positioning ---
    int cone_base_width = 30;
    int cone_height = 15;
    int cone_spacing = 4;
    int text_size = 1;

    // --- Draw each cone ---
    for (int i = 0; i < num_choices; ++i)
    {
        PlayerChoise current_choice_enum_val = static_cast<PlayerChoise>(i); // Assumes enum values are 0,1,2,3,4
        int top_left_x = canvas.width() / 2 + cone_base_width * (i % 4);
        int top_left_y = 10 + cone_height * (i / 4);
        uint16_t color = choice_colors[i];
        canvas.fillRect(top_left_x, top_left_y, cone_base_width, cone_height, color);

        // Highlight the currently selected choice with a border
        if (players_number == current_choice_enum_val)
        {
            uint16_t highlight_color = TFT_YELLOW;
            canvas.drawRect(top_left_x, top_left_y, cone_base_width, cone_height, highlight_color);
        }

        // Draw text label inside the cone
        canvas.setTextColor(TFT_WHITE); // Assuming white text is visible on most cone colors
        canvas.setTextSize(text_size);
        canvas.setTextDatum(MC_DATUM);                                                                             // Middle-Center datum
        canvas.drawString(choice_names[i], top_left_x + cone_base_width / 2, top_left_y + cone_height / 2 + 1, 1); // Font 1 (small)
    }

    // Draw result
    if (is_payout_calculated_flag)
    {
        uint16_t color = TFT_WHITE;

        if (selected_number_value == 0)
        {
            color = TFT_GREEN;
        }
        if (target_number_index > 0 && target_number_index % 2 == 1)
        {
            color = TFT_RED;
        }
        if (target_number_index > 0 && target_number_index % 2 == 0)
        {
            color = TFT_BLACK;
        }
        canvas.setTextSize(5);
        int result_width = 100;
        canvas.fillRect(canvas.width() / 2, canvas.height() / 2, result_width, canvas.fontHeight(), color);

        canvas.drawString(String(selected_number_value), canvas.width() / 2 + result_width / 2, canvas.height() / 2 + canvas.fontHeight() / 2 + 2);
    }
}

float RouletteMachine::trim_radian(float radian)
{
    float result = radian;

    while (result < 0)
        result += 2 * M_PI;
    while (result >= 2 * M_PI)
        result -= 2 * M_PI;
    return result;
}
