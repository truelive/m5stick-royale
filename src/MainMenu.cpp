#include "MainMenu.h"

const char* MainMenu::menu_item_names[NUM_MENU_ITEMS] = {"Slot Machine", "Even/Uneven", "Roulette"};

MainMenu::MainMenu()
{
    selected_mode = Menu;
    selected_menu_item = 0;
}

MainMenu::~MainMenu()
{
}

void MainMenu::draw_slot_status_bar(M5Canvas &status_bar,unsigned long balance, int last_payout)
{
  char hello[100];
  snprintf(hello, sizeof(hello), "Balance: %d, Payout: %d", balance, last_payout);
  //status_bar.setTextColor(TFT_WHITE, TFT_BLACK);
  status_bar.fillRect(0, 0, status_bar.width(), status_bar.height(), TFT_BLACK);
  status_bar.drawString(hello, 0, 0);
}

void MainMenu::draw_menu_status_bar(M5Canvas &status_bar)
{
  //status_bar.setTextColor(TFT_WHITE, TFT_BLACK);
  status_bar.fillRect(0, 0, status_bar.width(), status_bar.height(), TFT_BLACK);
  status_bar.drawString("Select a game:", 0, 0);
}

void MainMenu::draw(M5Canvas &status_bar, M5Canvas &canvas, int top_left_x, int top_left_y, uint16_t transparent_color) {
    if (selected_mode == Slot) {
        draw_slot_status_bar(status_bar, slotMachine->get_balance(), slotMachine->get_last_payout());
        slotMachine->draw(canvas, top_left_x, top_left_y, transparent_color);
        return;
    }
    if (selected_mode == Roulette) {
        // draw a status bar
        //slotMachine->draw(canvas, top_left_x, top_left_y, transparent_color);
        return;
    }
    if (selected_mode == Dice) {
        // draw a status bar
        //slotMachine->draw(canvas, top_left_x, top_left_y, transparent_color);
        return;
    }
    if (selected_mode == Menu) {
        draw_menu_status_bar(status_bar);
        canvas.fillSprite(TFT_BLACK);
        canvas.setTextColor(TFT_WHITE, TFT_BLACK);
        canvas.setTextSize(2);
        canvas.setTextDatum(MC_DATUM);
        int font_height = canvas.fontHeight();

        for (int i = 0; i < NUM_MENU_ITEMS; i++) {
            if (i == selected_menu_item) {
                canvas.fillRect(top_left_x, top_left_y + i * font_height + canvas.height()/3 - font_height/2, canvas.width(), font_height, TFT_WHITE);
            }
            canvas.drawString(menu_item_names[i], top_left_x + canvas.width() / 2, top_left_y + i * font_height + canvas.height()/3);
        }
        return;
    }
}
void MainMenu::update(bool btnA_pressed, bool btnB_pressed, bool btnC_pressed)
{
    if (selected_mode != Menu && btnC_pressed) {
        selected_mode = Menu;
     }
    if (selected_mode == Slot) {
        slotMachine->update(btnA_pressed, btnB_pressed, btnC_pressed);
        return;
    }
    if (selected_mode == Roulette) {
        //slotMachine->draw(canvas, top_left_x, top_left_y, transparent_color);
        return;
    }
    if (selected_mode == Dice) {
        //slotMachine->draw(canvas, top_left_x, top_left_y, transparent_color);
        return;
    }
    if (selected_mode == Menu) {
        if (btnA_pressed) {
            // change the mode to selected and forward the input
            selected_mode = (Mode)((int)selected_mode + 1);

        }
        if (btnB_pressed) {
            selected_menu_item = (selected_menu_item + 1) % 3;
        }
    } 
}

void MainMenu::setSlotMachine(SlotMachine *mySlotMachine)
{
    slotMachine = mySlotMachine;
}