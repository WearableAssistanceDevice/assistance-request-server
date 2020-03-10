#include "ble_assist_service.h"
#include "ble_service/ble_ars_c/ble_ars_c.h"

#include "config.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"


BLE_ARS_C_DEF(m_ble_ars_c);  /**< Main structure used by the Assistance Request client module. */


void ble_assist_bsp_evt_handler(bsp_event_t event) {
    ret_code_t err_code;

    switch (event) {
        case ASSIST_REQ_ACK_BUTTON: {
          err_code = ble_ars_assist_req_send(&m_ble_ars_c, false);
          if (err_code != NRF_SUCCESS &&
              err_code != BLE_ERROR_INVALID_CONN_HANDLE &&
              err_code != NRF_ERROR_INVALID_STATE)
          {
              APP_ERROR_CHECK(err_code);
          }
          if (err_code == NRF_SUCCESS)
          {
              NRF_LOG_INFO("ARS acknowledge Assistance Request");
          }
        }
        break;

        default: break;
    }
}


/**@brief Function for handling the Assistance Request Service client errors.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void ars_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Handles events coming from the Assistance Request central module.
 */
static void ars_c_evt_handler(ble_ars_c_t* p_ars_c, ble_ars_c_evt_t* p_ars_c_evt)
{
    switch (p_ars_c_evt->evt_type)
    {
        case BLE_ARS_C_EVT_DISCOVERY_COMPLETE:
        {
            ret_code_t err_code;

            err_code = ble_ars_c_handles_assign(&m_ble_ars_c,
                                                p_ars_c_evt->conn_handle,
                                                &p_ars_c_evt->params.peer_db);
            NRF_LOG_INFO("Assistance Request service discovered on conn_handle 0x%x.", p_ars_c_evt->conn_handle);

            err_code = app_button_enable();
            APP_ERROR_CHECK(err_code);

            // Assistance Request service discovered. Enable notification of assistance request.
            err_code = ble_ars_c_assist_req_notif_enable(p_ars_c);
            APP_ERROR_CHECK(err_code);
        } break; // BLE_ARS_C_EVT_DISCOVERY_COMPLETE

        case BLE_ARS_C_EVT_BUTTON_NOTIFICATION:
        {
            NRF_LOG_INFO("Assistance Request state changed on peer to 0x%x.", p_ars_c_evt->params.request.req_state);
            if (p_ars_c_evt->params.request.req_state)
            {
                bsp_board_led_on(ASSIST_REQUESTED_LED);
            }
            else
            {
                bsp_board_led_off(ASSIST_REQUESTED_LED);
            }
        } break; // BLE_ARS_C_EVT_BUTTON_NOTIFICATION

        default:
            // No implementation needed.
            break;
    }
}

void ble_assist_ble_evt_handler(const ble_evt_t* p_ble_evt, void* p_context) {
    ret_code_t err_code;

    // For readability
    const ble_gap_evt_t* p_gap_evt = &p_ble_evt->evt.gap_evt;

    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED: {
            err_code = ble_ars_c_handles_assign(&m_ble_ars_c, p_gap_evt->conn_handle, NULL);
            APP_ERROR_CHECK(err_code);
        } break;

        default: break;
    }
}


void ble_assist_db_disc_handler(ble_db_discovery_evt_t* p_evt) {
    ble_ars_on_db_disc_evt(&m_ble_ars_c, p_evt);
}


/**@brief Assistance Request client initialization.
 */
static void ars_c_init(nrf_ble_gq_t* p_gatt_queue)
{
    ret_code_t       err_code;
    ble_ars_c_init_t ars_c_init_obj;

    ars_c_init_obj.evt_handler   = ars_c_evt_handler;
    ars_c_init_obj.p_gatt_queue  = &p_gatt_queue;
    ars_c_init_obj.error_handler = ars_error_handler;

    err_code = ble_ars_c_init(&m_ble_ars_c, &ars_c_init_obj);
    APP_ERROR_CHECK(err_code);
}


void ble_assist_service_init(nrf_ble_gq_t* p_gatt_queue) {
    ars_c_init(p_gatt_queue);
}