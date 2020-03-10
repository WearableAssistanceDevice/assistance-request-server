#pragma once

#include "bsp.h"
#include "ble.h"
#include "nrf_ble_gq.h"
#include "ble_db_discovery.h"


/**@brief Function for initializing the BLE Assistance Request service.
 */
void ble_assist_service_init(nrf_ble_gq_t* p_gatt_queue);


/**@brief Function for handling Assistance Request related BSP events.
 *
 * @param[in] event  BSP event.
 */
void ble_assist_bsp_evt_handler(bsp_event_t event);


/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
void ble_assist_ble_evt_handler(const ble_evt_t* p_ble_evt, void* p_context);


void ble_assist_db_disc_handler(ble_db_discovery_evt_t* p_evt);