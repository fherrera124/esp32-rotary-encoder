/**
 ******************************************************************************
 * @file           : re_init.c
 * @brief          : Convenient initializer of a predefined rotary encoder
 *                   intance. With an optional button instance attached to it.
 * @author         : Francisco Herrera
 * @date           : Oct 2022
 ******************************************************************************
 * @copyright GNU General Public License
 * 
 * Copyright (c) 2022 Francisco Herrera
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "button.h"
#include "rotary_encoder.h"

/* Private macro -------------------------------------------------------------*/
#define ROT_ENC_A_GPIO   CONFIG_RE_CLK_GPIO
#define ROT_ENC_B_GPIO   CONFIG_RE_DT_GPIO
#define ROT_ENC_BTN_GPIO CONFIG_RE_BTN_GPIO

#define ENABLE_HALF_STEPS false  // Set to true to enable tracking of rotary encoder at half step resolution
#define RESET_AT          0      // Set to a positive non-zero number to reset the position if this value is exceeded
#define FLIP_DIRECTION    false  // Set to true to reverse the clockwise/counterclockwise sense

/* Private variables ---------------------------------------------------------*/
#ifdef CONFIG_RE_BTN_ENABLE
button_t re_button = {0}; // Analize if should be defined in main instead
#endif

/* Exported functions --------------------------------------------------------*/
esp_err_t rotary_encoder_default_init(rotary_encoder_info_t* info) {
    // Initialise the rotary encoder device with the GPIOs for A and B signals
    ESP_ERROR_CHECK(rotary_encoder_init(info, ROT_ENC_A_GPIO, ROT_ENC_B_GPIO));
    ESP_ERROR_CHECK(rotary_encoder_enable_half_steps(info, ENABLE_HALF_STEPS));
#ifdef FLIP_DIRECTION
    ESP_ERROR_CHECK(rotary_encoder_flip_direction(info));
#endif

    // Create a queue for events from the rotary encoder driver.
    // Tasks can read from this queue to receive up to date position information.
    QueueHandle_t event_queue = rotary_encoder_create_queue();
    ESP_ERROR_CHECK(rotary_encoder_set_queue(info, event_queue));

#ifdef CONFIG_RE_BTN_ENABLE
    /* Keeping both rotary encoder and the button components
     * decoupled, only having in common the shared queue */
    ESP_ERROR_CHECK(re_button_init(&re_button, ROT_ENC_BTN_GPIO, info->queue));
#endif

    return ESP_OK;
}

/***************************** END OF FILE ************************************/
