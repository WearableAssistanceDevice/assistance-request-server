#pragma once

#include "bsp.h"
#include "ble.h"
#include "nrf_ble_scan.h"
#include "ble_db_discovery.h"


/**@brief Function for handling Scaning events.
 *
 * @param[in]   p_scan_evt   Scanning event.
 */
typedef void (*ble_scan_evt_handler_t)(const scan_evt_t* scan_evt);


/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
typedef void (*ble_evt_handler_t)(const ble_evt_t* p_ble_evt, void* p_context);


/**@brief Function for handling database discovery events.
 *
 * @details This function is callback function to handle events from the database discovery module.
 *          Depending on the UUIDs that are discovered, this function should forward the events
 *          to their respective services.
 *
 * @param[in] p_event  Pointer to the database discovery event.
 */
typedef void (*db_discovery_evt_handler_t)(ble_db_discovery_evt_t* p_evt);


/**@brief BLE service initialization function type */
typedef void (*ble_service_init_func_t)(nrf_ble_gq_t* p_gatt_queue);


/**@brief BLE services init structure */
typedef struct {
    ble_scan_evt_handler_t     scan_evt_handler;
    ble_evt_handler_t          ble_evt_handler;
    db_discovery_evt_handler_t db_disc_evt_handler;
    ble_service_init_func_t*   service_init_funcs;
    unsigned int               service_init_func_count;
} ble_services_init_t;


/**@brief Function for initializing BLE services.
 *
 * @param[in] p_init  BLE service initialization config.
 */
void ble_services_init(const ble_services_init_t* p_init);

/**@brief Function to start scanning */
void scan_start(void);