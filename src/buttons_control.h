/**
 * @file buttons_control.h
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-06-23
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#ifndef BUTTONS_CONTROL_H
#define BUTTONS_CONTROL_H

#include <EncButton.h>

void initButtons();
void tickButtons();

extern Button powerBtn;
extern Button mainLightsBtn;
extern Button centerSwingBtn;
extern Button beaconLightModeBtn;
extern Button opt2Btn;
extern Button aBtn;
extern Button bBtn;
extern Button cBtn;
extern Button scanBtn;

#endif // BUTTONS_CONTROL_H