#ifndef MAIN_MENU_H
#define MAIN_MENU_H
#include <M5Unified.h> // Required for M5Canvas and random()
#include "SlotMachine.h"

class MainMenu
{
public:
    MainMenu();
    ~MainMenu();

    void setSlotMachine(SlotMachine *mySlotMachine);
    void draw(M5Canvas& canvas, int top_left_x, int top_left_y, uint16_t transparent_color);
    void update(bool btnA_pressed, bool btnB_pressed, bool btnC_pressed);

private:
    SlotMachine *slotMachine;
};
#endif // MAIN_MENU_H