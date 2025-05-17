#include <M5Unified.h> // Make the M5Unified library available to your program.
#include "assets/assets.h"
#include "SlotMachine.h"
#include "MainMenu.h"


// global variables (define variables to be used throughout the program)
#define MS_PER_FRAME 16
#define MS_TO_AUTO_STOP_SPIN 900
#define MS_TO_AUTO_SPIN 1800

uint32_t count;
unsigned long last_draw = 0;
auto font = &fonts::Font2;
M5Canvas status_bar(&M5.Display);
M5Canvas main_canvas(&M5.Display);
M5Canvas sub_canvas(&main_canvas);
auto bg_color = 0xfdca;

const int NUM_SYMBOL_TYPES = 8; // Or calculate from slot_symbols array size

SlotMachine *mySlotMachine;
MainMenu *myMainMenu;

// setup function is executed only once at startup.
// This function mainly describes the initialization process.
void draw_status_bar(unsigned long balance, int last_payout);

void setup()
{
  auto cfg = M5.config();

  M5.begin(cfg); // initialize M5 device
  M5.Display.setRotation(1);
  M5.Display.setFont(font); // change text size
  M5.Display.fillRect(0, 0, M5.Display.width(), M5.Display.height(), bg_color);
  status_bar.createSprite(M5.Display.width(), font->height);
  status_bar.setFont(font);

  main_canvas.createSprite(M5.Display.width(), M5.Display.height() - font->height);
  sub_canvas.createSprite(111,111);
  sub_canvas.fillSprite(TFT_PINK);
  sub_canvas.pushImage(0, 0, 111, 111, roulette, TFT_WHITE);

  // In your setup() function:
  SlotMachine::Parameters params = {
    slot_symbols,
    slot_weights,
    slot_payout_counts,
    slot_payout,
    NUM_SYMBOL_TYPES,
    SYM_WIDTH,
    SYM_HEIGHT
  };
  mySlotMachine = new SlotMachine(params);
  myMainMenu = new MainMenu();
  myMainMenu->setSlotMachine(mySlotMachine);
}

// loop function is executed repeatedly for as long as it is running.
// loop function acquires values from sensors, rewrites the screen, etc.
void loop()
{
  unsigned long tick = millis(); // Get the current heartbeat value
  if ((tick - last_draw) < MS_PER_FRAME)
  {
    return;
  }
  //count++;
  M5.update();
  last_draw = tick;
  myMainMenu->update(M5.BtnA.wasPressed(), M5.BtnB.wasPressed(), M5.BtnPWR.wasPressed()); // Update animation logic
  main_canvas.fillRect(0, 0, main_canvas.width(), main_canvas.height(), bg_color); // Example clear
  myMainMenu->draw(status_bar, main_canvas, 0, 0, TFT_WHITE);                             // Draw the slot machine at top-left of main_canvas
  //sub_canvas.pushRotateZoom(count*0.8f, 1.0f, 1.0f, TFT_PINK);


  M5.Display.startWrite();
  status_bar.pushSprite(0, 0);
  main_canvas.pushSprite(0, status_bar.height());
  M5.Display.endWrite();
}
