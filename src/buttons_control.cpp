/**
 * @file buttons_control.cpp
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-06-23
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#include "buttons_control.h"
#include "constants.h"

// Define buttons
Button powerBtn(POWER_BUTTON, INPUT_PULLUP);
Button mainLightsBtn(MAIN_LIGHTS_BUTTON, INPUT_PULLUP);
Button centerSwingBtn(CENTER_SWING_BUTTON, INPUT_PULLUP);
Button scanBtn(SCAN_BUTTON, INPUT_PULLUP);
Button opt1Btn(OPT_1_BUTTON, INPUT_PULLUP);
Button opt2Btn(OPT_2_BUTTON, INPUT_PULLUP);
Button aBtn(A_BUTTON, INPUT_PULLUP);
Button bBtn(B_BUTTON, INPUT_PULLUP);
Button cBtn(C_BUTTON, INPUT_PULLUP);

void defaultButtonCallback(const char *buttonName, Button &button)
{
    // Log only click events
    if (button.action() == EB_CLICK)
    {
        Serial.printf("Button %s clicked\n", buttonName);
    }
}

/**
 * Initializes the buttons and attaches callback functions to handle button presses.
 * Callbacks could be overridden by attaching new functions to the buttons.
 */
void initButtons()
{
    powerBtn.attach([=]()
                    { defaultButtonCallback("Power", powerBtn); });
    mainLightsBtn.attach([=]()
                         { defaultButtonCallback("Main Lights", mainLightsBtn); });
    centerSwingBtn.attach([=]()
                          { defaultButtonCallback("Center Swing", centerSwingBtn); });
    scanBtn.attach([=]()
                   { defaultButtonCallback("Scan", scanBtn); });
    opt1Btn.attach([=]()
                   { defaultButtonCallback("Option 1", opt1Btn); });
    opt2Btn.attach([=]()
                   { defaultButtonCallback("Option 2", opt2Btn); });
    aBtn.attach([=]()
                { defaultButtonCallback("A", aBtn); });
    bBtn.attach([=]()
                { defaultButtonCallback("B", bBtn); });
    cBtn.attach([=]()
                { defaultButtonCallback("C", cBtn); });
}

void tickButtons()
{
    powerBtn.tick();
    mainLightsBtn.tick();
    centerSwingBtn.tick();
    scanBtn.tick();
    opt1Btn.tick();
    opt2Btn.tick();
    aBtn.tick();
    bBtn.tick();
    cBtn.tick();
}