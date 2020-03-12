#pragma once

#include <stdbool.h>

#include "bsp.h"
#include "ble.h"
#include "ble_advertising.h"
#include "ble_db_discovery.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_gq.h"
#include "nrf_ble_qwr.h"


#ifdef __cplusplus
extern "C" {
#endif


/**@brief BLE advertisement event handler type
 *
 * @param[in] ble_adv_evt  The advertising event type
 */
typedef void (*ble_adv_evt_handler_t)(ble_adv_evt_t ble_adv_evt);


/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event
 * @param[in]   p_context   Unused
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


/**@brief BLE GATT server service initialization function type */
typedef void (*ble_gatts_service_init_func_t)(void);

/**@brief BLE GATT client service initialization function type */
typedef void (*ble_gattc_service_init_func_t)(nrf_ble_gq_t* p_gatt_queue);


/**@brief BLE services init structure */
typedef struct {
    ble_advertising_t*              p_ble_advertising;      /**< Pointer to the advertising module */
    ble_db_discovery_t*             p_ble_db_discovery;     /**< Pointer to the database discovery module */
    nrf_ble_gatt_t*                 p_ble_gatt;             /**< Pointer to the GATT module */
    nrf_ble_gq_t*                   p_ble_qatt_queue;       /**< Pointer to the GATT queue module */
    nrf_ble_qwr_t*                  p_ble_qwr;              /**< Pointer to the queued write module */

    ble_adv_evt_handler_t           adv_evt_handler;        /**< User event handler for advertising events */
    ble_evt_handler_t               ble_evt_handler;        /**< User event handler for BLE events */
    db_discovery_evt_handler_t      db_disc_evt_handler;    /**< User event handler for database discovery events */

    ble_gatts_service_init_func_t*  gatts_init_funcs;       /**< GATT Server service init functions */
    unsigned int                    gatts_init_func_count;  /**< Number of GATT Server init functions */

    ble_gattc_service_init_func_t*  gattc_init_funcs;       /**< GATT Client service init functions */
    unsigned int                    gattc_init_func_count;  /**< Number of GATT Client init functions */
} ble_services_init_t;



/**@brief Function for initializing BLE services.
 *
 * @param[in] p_init  BLE service initialization config.
 */
void ble_services_init(const ble_services_init_t* p_init);


/**@brief Function for starting BLE advertising.
 *
 * @param[in] erase_bonds  True if existing bonds should be erased.
 */
void advertising_start(bool erase_bonds);


/**@brief Function for handling BLE-related BSP events.
 *
 * @param[in] event  BSP event.
 */
void ble_bsp_evt_handler(bsp_event_t event);


#ifdef __cplusplus
}
#endif