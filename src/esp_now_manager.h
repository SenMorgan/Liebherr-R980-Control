/**
 * @file esp_now_manager.h
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-06-16
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#ifndef ESP_NOW_MANAGER_H
#define ESP_NOW_MANAGER_H

#include <esp_now.h>

#include "data_structures.h"

void initEspNow();
void registerDataRecvCallback(esp_now_recv_cb_t callback);
void sendDataToExcavator(const controller_data_struct &data);

#endif // ESP_NOW_MANAGER_H