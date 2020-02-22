/**
 * Copyright (c) 2016 - 2019, Nordic Semiconductor ASA
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
/**@file
 *
 * @defgroup ble_ars_c Assistance Request Service Client
 * @{
 * @brief    The Assistance Request Service client can be used to connect to an assistance device and
 *           acknowledge a request for assistance.
 *
 * @details  This module contains the APIs and types exposed by the Assistance Request Service Client
 *           module. The application can use these APIs and types to perform the discovery of
 *           Assistance Request Service at the peer and to interact with it.
 *
 * @note    The application must register this module as the BLE event observer by using the
 *          NRF_SDH_BLE_OBSERVER macro. Example:
 *          @code
 *              ble_ars_c_t instance;
 *              NRF_SDH_BLE_OBSERVER(anything, BLE_ARS_C_BLE_OBSERVER_PRIO,
 *                                   ble_ars_c_on_ble_evt, &instance);
 *          @endcode
 */

#ifndef BLE_ARS_C_H__
#define BLE_ARS_C_H__

#include <stdint.h>
#include "ble.h"
#include "ble_db_discovery.h"
#include "ble_srv_common.h"
#include "nrf_ble_gq.h"
#include "nrf_sdh_ble.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_ARS_C_BLE_OBSERVER_PRIO 2

/**@brief   Macro for defining a ble_ars_c instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_ARS_C_DEF(_name)                         \
static ble_ars_c_t _name;                           \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                 \
                     BLE_ARS_C_BLE_OBSERVER_PRIO,   \
                     ble_ars_c_on_ble_evt, &_name)

/**@brief   Macro for defining multiple ble_ars_c instances.
 *
 * @param   _name   Name of the array of instances.
 * @param   _cnt    Number of instances to define.
 */
#define BLE_ARS_C_ARRAY_DEF(_name, _cnt)                    \
static ble_ars_c_t _name[_cnt];                            \
NRF_SDH_BLE_OBSERVERS(_name ## _obs,                       \
                      BLE_ARS_C_BLE_OBSERVER_PRIO,         \
                      ble_ars_c_on_ble_evt, &_name, _cnt)


#define ARS_UUID_BASE {0xD2, 0x5F, 0xC5, 0xB3, 0xD6, 0xBA, 0xCF, 0x84, \
                      0x1E, 0x45, 0x13, 0x14, 0x6A, 0x66, 0xD7, 0xBA}

#define ARS_UUID_SERVICE         0x1000
#define ARS_UUID_ASSIST_REQ_CHAR 0x1001


/**@brief ARS Client event type. */
typedef enum
{
    BLE_ARS_C_EVT_DISCOVERY_COMPLETE = 1,  /**< Event indicating that the Assistance Request Service was discovered at the peer. */
    BLE_ARS_C_EVT_BUTTON_NOTIFICATION      /**< Event indicating that a notification of the Assistance Request characteristic was received from the peer. */
} ble_ars_c_evt_type_t;

/**@brief Structure containing the Assistance Request state received from the peer. */
typedef struct
{
    uint8_t req_state;  /**< Assistance Request Value. */
} ble_assist_req_t;

/**@brief Structure containing the handles related to the Assistance Request Service found on the peer. */
typedef struct
{
    uint16_t assist_req_cccd_handle;  /**< Handle of the CCCD of the Assistance Request characteristic. */
    uint16_t assist_req_handle;       /**< Handle of the Assistance Request characteristic as provided by the SoftDevice. */
} ars_db_t;

/**@brief Assistance Request Event structure. */
typedef struct
{
    ble_ars_c_evt_type_t evt_type;     /**< Type of the event. */
    uint16_t             conn_handle;  /**< Connection handle on which the event occured.*/
    union
    {
        ble_assist_req_t request;      /**< Assistance Request value received. This is filled if the evt_type is @ref BLE_ARS_C_EVT_BUTTON_NOTIFICATION. */
        ars_db_t         peer_db;      /**< Handles related to the Assistance Request Service found on the peer device. This is filled if the evt_type is @ref BLE_ARS_C_EVT_DISCOVERY_COMPLETE.*/
    } params;
} ble_ars_c_evt_t;

// Forward declaration of the ble_ars_c_t type.
typedef struct ble_ars_c_s ble_ars_c_t;

/**@brief   Event handler type.
 *
 * @details This is the type of the event handler that is to be provided by the application
 *          of this module in order to receive events.
 */
typedef void (* ble_ars_c_evt_handler_t)(ble_ars_c_t* p_ble_ars_c, ble_ars_c_evt_t* p_evt);

/**@brief Assistance Request Client structure. */
struct ble_ars_c_s
{
    uint16_t                  conn_handle;   /**< Connection handle as provided by the SoftDevice. */
    ars_db_t                  peer_ars_db;   /**< Handles related to ARS on the peer. */
    ble_ars_c_evt_handler_t   evt_handler;   /**< Application event handler to be called when there is an event related to the Assistance Request service. */
    ble_srv_error_handler_t   error_handler; /**< Function to be called in case of an error. */
    uint8_t                   uuid_type;     /**< UUID type. */
    nrf_ble_gq_t*             p_gatt_queue;  /**< Pointer to the BLE GATT Queue instance. */
};

/**@brief Assistance Request Client initialization structure. */
typedef struct
{
    ble_ars_c_evt_handler_t   evt_handler;   /**< Event handler to be called by the Assistance Request Client module when there is an event related to the Assistance Request Service. */
    nrf_ble_gq_t*             p_gatt_queue;  /**< Pointer to the BLE GATT Queue instance. */
    ble_srv_error_handler_t   error_handler; /**< Function to be called in case of an error. */
} ble_ars_c_init_t;


/**@brief Function for initializing the Assistance Request client module.
 *
 * @details This function registers with the Database Discovery module for the
 *          Assistance Request Service. The module looks for the presence of a Assistance Request Service instance
 *          at the peer when a discovery is started.
 *
 * @param[in] p_ble_ars_c      Pointer to the Assistance Request client structure.
 * @param[in] p_ble_ars_c_init Pointer to the Assistance Request initialization structure that contains the
 *                             initialization information.
 *
 * @retval    NRF_SUCCESS On successful initialization. 
 * @retval    err_code    Otherwise, this function propagates the error code returned by the Database Discovery module API
 *                        @ref ble_db_discovery_evt_register.
 */
uint32_t ble_ars_c_init(ble_ars_c_t* p_ble_ars_c, ble_ars_c_init_t* p_ble_ars_c_init);


/**@brief Function for handling BLE events from the SoftDevice.
 *
 * @details This function handles the BLE events received from the SoftDevice. If a BLE event
 *          is relevant to the Assistance Request Client module, the function uses the event's data to update interval
 *          variables and, if necessary, send events to the application.
 *
 * @param[in] p_ble_evt     Pointer to the BLE event.
 * @param[in] p_context     Pointer to the assistance request client structure.
 */
void ble_ars_c_on_ble_evt(const ble_evt_t* p_ble_evt, void* p_context);


/**@brief Function for requesting the peer to start sending notification of the Button
 *        Characteristic.
 *
 * @details This function enables to notification of the Button at the peer
 *          by writing to the CCCD of the Button characteristic.
 *
 * @param[in] p_ble_ars_c Pointer to the Assistance Request Client structure.
 *
 * @retval  NRF_SUCCESS 			If the SoftDevice has been requested to write to the CCCD of the peer.
 * @retval  NRF_ERROR_INVALID_STATE If no connection handle has been assigned (@ref ble_ars_c_handles_assign).
 * @retval  NRF_ERROR_NULL 			If the given parameter is NULL.
 * @retval  err_code				Otherwise, this API propagates the error code returned by function
 *          						@ref nrf_ble_gq_item_add.
 */
uint32_t ble_ars_c_assist_req_notif_enable(ble_ars_c_t* p_ble_ars_c);


/**@brief Function for handling events from the Database Discovery module.
 *
 * @details Call this function when you get a callback event from the Database Discovery module. This
 *          function handles an event from the Database Discovery module, and determines whether it
 *          relates to the discovery of Assistance Request service at the peer. If it does, this function calls the
 *          application's event handler to indicate that the Assistance Request service was discovered
 *          at the peer. The function also populates the event with service-related information before
 *          providing it to the application.
 *
 * @param[in] p_ble_ars_c Pointer to the Assistance Request client structure.
 * @param[in] p_evt Pointer to the event received from the Database Discovery module.
 */
void ble_ars_on_db_disc_evt(ble_ars_c_t* p_ble_ars_c, const ble_db_discovery_evt_t* p_evt);


/**@brief     Function for assigning handles to this instance of ars_c.
 *
 * @details Call this function when a link has been established with a peer to associate the link
 *          to this instance of the module. This makes it possible to handle several links and
 *          associate each link to a particular instance of this module.
 *
 * @param[in] p_ble_ars_c    Pointer to the Assistance Request client structure instance for associating the link.
 * @param[in] conn_handle    Connection handle to associate with the given Assistance Request Client Instance.
 * @param[in] p_peer_handles Assistance Request Service handles found on the peer (from @ref BLE_ARS_C_EVT_DISCOVERY_COMPLETE event).
 *
 * @retval NRF_SUCCESS If the status was sent successfully.
 * @retval err_code    Otherwise, this API propagates the error code returned by function
 *                     @ref nrf_ble_gq_item_add.
 *
 */
uint32_t ble_ars_c_handles_assign(ble_ars_c_t*    p_ble_ars_c,
                                  uint16_t        conn_handle,
                                  const ars_db_t* p_peer_handles);


/**@brief Function for writing the Assistance Request status to the connected server.
 *
 * @param[in] p_ble_ars_c Pointer to the Assistance Request client structure.
 * @param[in] status      Assistance Request status to send.
 *
 * @retval NRF_SUCCESS If the status was sent successfully.
 * @retval err_code    Otherwise, this API propagates the error code returned by function
 *                     @ref nrf_ble_gq_conn_handle_register.
 */
uint32_t ble_ars_assist_req_send(ble_ars_c_t* p_ble_ars_c, uint8_t status);


#ifdef __cplusplus
}
#endif

#endif // BLE_ARS_C_H__

/** @} */
