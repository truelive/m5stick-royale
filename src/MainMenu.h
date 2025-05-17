#ifndef MAIN_MENU_H
#define MAIN_MENU_H
#include <M5Unified.h> // Required for M5Canvas and random()
#include "SlotMachine.h"

#define NUM_MENU_ITEMS 3

enum Mode
{
    Menu,
    Slot,
    Dice,
    Roulette
};

class MainMenu
{
public:
    MainMenu();
    ~MainMenu();

    void setSlotMachine(SlotMachine *mySlotMachine);
    void draw_slot_status_bar(M5Canvas &status_bar, unsigned long balance, int last_payout);
    void draw_menu_status_bar(M5Canvas &status_bar);
    void draw(M5Canvas &status_bar, M5Canvas &canvas, int top_left_x, int top_left_y, uint16_t transparent_color);
    void update(bool btnA_pressed, bool btnB_pressed, bool btnC_pressed);

private:
    SlotMachine *slotMachine;
    Mode selected_mode;
    int selected_menu_item;
    static const char* menu_item_names[NUM_MENU_ITEMS];
};
#endif // MAIN_MENU_H