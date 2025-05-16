#include <M5Unified.h> // Make the M5Unified library available to your program.
#include "assets/assets.h"
#include "SlotMachine.h"

// global variables (define variables to be used throughout the program)
#define MS_PER_FRAME 16
#define MS_TO_AUTO_STOP_SPIN 900
#define MS_TO_AUTO_SPIN 1800

uint32_t count;
unsigned long last_draw = 0;
auto font = &fonts::Font2;
M5Canvas status_bar(&M5.Display);
M5Canvas main_canvas(&M5.Display);
auto bg_color = 0xfdca;

const int NUM_SYMBOL_TYPES = 8; // Or calculate from slot_symbols array size

SlotMachine *mySlotMachine;

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
  M5.update();
  last_draw = tick;
  mySlotMachine->update(M5.BtnA.wasPressed(), M5.BtnB.wasPressed(), M5.BtnC.wasPressed()); // Update animation logic
  draw_status_bar(mySlotMachine->get_balance(), mySlotMachine->get_last_payout());
  main_canvas.fillRect(0, 0, main_canvas.width(), main_canvas.height(), bg_color); // Example clear
  mySlotMachine->draw(main_canvas, 0, 0, TFT_WHITE);                               // Draw the slot machine at top-left of main_canvas

  M5.Display.startWrite();
  status_bar.pushSprite(0, 0);
  main_canvas.pushSprite(3, status_bar.height() + 3);
  M5.Display.endWrite();
}

void draw_status_bar(unsigned long balance, int last_payout)
{
  char hello[100];
  snprintf(hello, sizeof(hello), "Balance: %d, Payout: %d", balance, last_payout);
  status_bar.fillRect(0, 0, status_bar.width(), status_bar.height(), bg_color);
  status_bar.drawString(hello, 0, 0);
}

void draw_weights_table()
{
  int x = main_canvas.width() / 2 - 7;
  int y = 0;
  int cell_width = 25;
  int cell_height = 15;
  main_canvas.setTextSize(1);
  main_canvas.setFont(&fonts::Font0);

  main_canvas.setTextColor(TFT_BLACK, 0xfdcf);
  main_canvas.setTextDatum(MC_DATUM);
  for (int i = -1; i < NUM_SYMBOL_TYPES; i++)
  {
    if (i == -1)
    {
      for (int j = 0; j < 4; j++)
      {
        main_canvas.drawString(String(slot_payout_counts[j]), x + cell_width / 2, y + cell_height / 2);
        x += cell_width;
      }
      x = main_canvas.width() / 2 - 7;
      y += cell_height;
      continue;
    }
    if (i == 1 || i == 3 || i == 4 || i == 5)
    {
      continue;
    }
    for (int j = 0; j < 4; j++)
    {
      main_canvas.drawRect(x, y, cell_width, cell_height, TFT_BLACK);
      main_canvas.drawString(String(slot_payout[i][j]), x + cell_width / 2, y + cell_height / 2);
      x += cell_width;
    }
    main_canvas.pushImage(x, y, SYM_WIDTH, SYM_HEIGHT, slot_symbols[i], TFT_WHITE);
    x = main_canvas.width() / 2 - 7;
    y += cell_height;
  }
  main_canvas.pushImage(x, y, SYM_WIDTH, SYM_HEIGHT, slot_symbols[1], TFT_WHITE);
  x += cell_width * 2;
  main_canvas.drawString(String(slot_payout_counts[0]) + ":" + String(slot_payout[1][0]) + " " + String(slot_payout_counts[1]) + ":" + String(slot_payout[1][1]),
                         x + cell_width / 2, y + cell_height / 2);
  y += cell_height;
  main_canvas.drawString(String(slot_payout_counts[2]) + ":" + String(slot_payout[1][2]) + " " + String(slot_payout_counts[3]) + ":" + String(slot_payout[1][3]),
                         x + cell_width / 2, y + cell_height / 2);
}
