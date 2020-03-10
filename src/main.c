/**
 * Copyright (c) 2014 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/**
 * @brief BLE Assistance Request Service central and client application main file.
 *
 * This file contains the source code for a sample client application using the Assistance Request service.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

#include "board_service/board_service.h"
#include "ble_service/ble_services.h"
#include "ble_service/ble_assist_service.h"

#include "nrf_pwr_mgmt.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"


/**@brief Function to handle asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num     Line number of the failing ASSERT call.
 * @param[in] p_file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t* p_file_name)
{
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}


/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(const ble_evt_t* p_ble_evt, void* p_context)
{
    ble_assist_ble_evt_handler(p_ble_evt, p_context);
}


/**@brief Function for handling events from the button handler module.
 *
 * @param[in] event  BSP event
 */
static void bsp_event_handler(bsp_event_t event)
{
    ble_assist_bsp_evt_handler(event);
}


/**@brief Function for handling Scaning events.
 *
 * @param[in]   p_scan_evt   Scanning event.
 */
static void scan_evt_handler(const scan_evt_t* p_scan_evt)
{
}


/**@brief Function for handling database discovery events.
*/
static void db_disc_handler(ble_db_discovery_evt_t* p_evt)
{
    ble_assist_db_disc_handler(p_evt);
}



/**@brief Function for handling the idle state (main loop).
 *
 * @details Handle any pending log operation(s), then sleep until the next event occurs.
 */
static void idle_state_handle(void)
{
    NRF_LOG_FLUSH();
    nrf_pwr_mgmt_run();
}


int main(void)
{
    board_services_init_t board_init = {0};
    board_init.bsp_evt_handler = bsp_event_handler;

    ble_services_init_t ble_init = {0};
    ble_init.ble_evt_handler = ble_evt_handler;
    ble_init.scan_evt_handler = scan_evt_handler;
    ble_init.db_disc_evt_handler = db_disc_handler;

    ble_service_init_func_t ble_init_funcs[] = {
        ble_assist_service_init
    };
    ble_init.service_init_funcs = ble_init_funcs;
    ble_init.service_init_func_count = sizeof(ble_init_funcs) / sizeof(ble_init_funcs[0]);

    board_services_init(&board_init);
    ble_services_init(&ble_init);

    // Start execution.
    NRF_LOG_INFO("Assitance Request Server started.");
    scan_start();

    // Turn on the LED to signal scanning.
    bsp_board_led_on(CENTRAL_SCANNING_LED);

    // Enter main loop.
    for (;;)
    {
        idle_state_handle();
    }
}
