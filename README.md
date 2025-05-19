# M5Stick-Royale casino-like machines
A collection of classic casino-style games designed for M5Stick devices (like M5StickC/C Plus), bringing interactive gaming to your palm. This project currently includes a Slot Machine, Roulette, and Dice even/uneven.

## Hardware Requirements

*   An M5Stick device (e.g., M5StickC, M5StickC Plus) compatible with the M5Unified library.

## Software Requirements & Dependencies

*   **PlatformIO IDE**: Recommended for building and uploading the project (typically used within Visual Studio Code).
    * PlatformIO setup guide for M5StickC: https://docs.m5stack.com/en/arduino/m5unified/intro_vscode
*   **M5Unified Library**: This should be automatically managed by PlatformIO if listed as a dependency in your `platformio.ini` file.

## Project Structure

A brief overview of the key files in this project:

*   `src/main.cpp`: Contains the main application logic, sets up the M5Stick, handles button inputs, manages game state, and draws the UI elements (status bar, paytable).
*   `src/SlotMachine.h` & `src/SlotMachine.cpp`: Defines and implements the `SlotMachine` class, which encapsulates all core slot machine mechanics. This includes managing the reels, symbol data, spinning animations, stopping logic, and calculating payouts.
*   `lib/assets/assets.h`: A crucial header file that defines:
    *   Pointers to the graphical data for each slot symbol (e.g., `bar`, `seven`). These likely point to `uint16_t` arrays defined in separate `.h` files like `bar.h`, `seven.h`, etc. (not fully provided but referenced).
    *   `SYM_WIDTH` and `SYM_HEIGHT`: Dimensions of the symbol graphics.
    *   `slot_symbols[]`: An array of pointers to the symbol image data.
    *   `slot_weights[]`: An array defining the relative frequency/weight of each symbol appearing on the reels.
    *   `slot_payout_counts[]`: An array defining the number of identical symbols required for different payout tiers (e.g., 5, 7, 9, 10 symbols).
    *   `slot_payout[][4]`: A 2D array defining the payout amounts for each symbol at each corresponding tier in `slot_payout_counts`.
*   `platformio.ini`: (Not provided in the context, but essential for PlatformIO projects) This file configures the project, board, framework, and library dependencies.

## How to Build and Upload

1.  **Clone the Repository:**
    If this project is in a Git repository, clone it:
    ```bash
    git clone git@github.com:truelive/m5stick-royale.git
    cd m5stick-royale 
    ```
    Otherwise, ensure you have all the project files in a single directory.

2.  **Open in PlatformIO:**
    *   Open Visual Studio Code.
    *   If you haven't already, install the PlatformIO IDE extension.
    *   Click on the PlatformIO icon in the activity bar (usually an alien head).
    *   Choose "Open Project" and navigate to the `m5stick-royale` project folder.

3.  **Configure `platformio.ini`:**
    Ensure your `platformio.ini` file is correctly set up for your M5Stick model and includes `M5Unified` as a library dependency. For example:
    ```ini
    [env:m5stickcplus]
    platform = espressif32
    board = m5stick-c-plus
    framework = arduino
    lib_deps =
        m5stack/M5Unified
    monitor_speed = 115200
    ```

4.  **Build and Upload:**
    *   Connect your M5Stick device to your computer via USB.
    *   In PlatformIO, click the "Upload" button (typically an arrow icon in the status bar at the bottom) or run the "PlatformIO: Upload" task from the Command Palette.

## Customization

The game's core parameters can be easily customized by modifying `lib/assets/assets.h`:

*   **Symbol Graphics:** To change the appearance of symbols, you would need to modify the pixel data arrays (e.g., `bar`, `seven`, etc.) which are included by `assets.h`. Ensure the `SYM_WIDTH` and `SYM_HEIGHT` macros match your new graphics.
*   **Symbol Weights (`slot_weights[]`):** Adjust these values to change the rarity and frequency of each symbol. Higher numbers mean more frequent appearances.
*   **Payout Tiers (`slot_payout_counts[]`):** Change the number of symbols required for a payout (e.g., instead of 5, 7, 9, 10, you could have 6, 8, 10, 12).
*   **Payout Amounts (`slot_payout[][4]`):** Modify this 2D array to change how much each symbol pays out for each tier defined in `slot_payout_counts`.

## Screenshots/GIF

TBD
---

## Copyright/Attribution Notice: 
- "Pixel art slot machine", by Vircon32 (Carra). Published at OpenGameArt under license CC-BY 4.0. 
    - https://opengameart.org/content/pixel-art-slot-machine
- "Pixel art dice faces", by Vircon32 (Carra). Published at OpenGameArt under license CC-BY 4.0.
    - https://opengameart.org/content/pixel-art-dice-faces

Enjoy your M5Stick-Royale!