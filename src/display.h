/**
 * @file display.h
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-08-25
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#ifndef DISPLAY_H
#define DISPLAY_H

enum DisplayState
{
    DISPLAY_OFF,
    DISPLAY_DEFAULT,
    DISPLAY_LOW_POWER,
    DISPLAY_OTA_UPDATE
};

void displayTaskInit(void);
void setDisplayState(DisplayState state);
void setOTAProgress(uint16_t percentage);
void disableDisplay(bool blocking = true);

#endif // DISPLAY_H