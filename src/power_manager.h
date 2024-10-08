/**
 * @file power_manager.h
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-06-18
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include <EncButton.h>

void setupPowerManager(Button &powerBtn);
void go_to_deep_sleep(void);
uint16_t readBatteryVoltage(bool reEnableWiFi = true);
void verifyBatteryLevel(void);
uint8_t calculateBatteryLevel(uint16_t voltage);

#endif // POWER_MANAGER_H