/** @file
 *
 * @defgroup ble_assistance_server main.c
 * @{
 * @ingroup ble_assistance_server
 * @brief Assistance Server main file.
 *
 * DESCRIPTION HERE
 */


#include "config.h"

#include "nrf.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_gq.h"
#include "nrf_ble_qwr.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "ble.h"
#include "ble_advertising.h"
#include "ble_db_discovery.h"

#include "bsp.h"
#include "bsp_btn_ble.h"

#include "board_service/board_services.h"
#include "ble_service/ble_services.h"
#include "ble_service/ble_ars_c/ble_ars_c.h"


NRF_BLE_GATT_DEF(m_gatt);              /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising);    /**< Advertising module instance. */
BLE_DB_DISCOVERY_DEF(m_db_disc);       /**< DB discovery module instance. */
NRF_BLE_GQ_DEF(m_gatt_queue,           /**< BLE GATT Queue instance. */
               NRF_SDH_BLE_PERIPHERAL_LINK_COUNT,
               NRF_BLE_GQ_QUEUE_SIZE);

BLE_ARS_C_DEF(m_ble_ars_c);  /**< Main structure used by the Assistance Request client module. */


/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}


/**@brief Function for handling the Assistance Request Service client errors.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void ars_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
    ret_code_t err_code;

    err_code = bsp_indication_set(BSP_INDICATE_IDLE);
    APP_ERROR_CHECK(err_code);

    // Prepare wakeup buttons.
    err_code = bsp_btn_ble_sleep_mode_prepare();
    APP_ERROR_CHECK(err_code);

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}


/**@brief Handles events coming from the Assistance Request client module.
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
            NRF_LOG_INFO("Assistance request service discovered on conn_handle 0x%x.", p_ars_c_evt->conn_handle);

            // Assistance Request service discovered. Enable notification of assistance request.
            //err_code = ble_ars_c_assist_req_notif_enable(p_ars_c);
            //APP_ERROR_CHECK(err_code);

            NRF_LOG_INFO("Reading assistance request state...");
            err_code = ble_ars_c_assist_req_get(&m_ble_ars_c);
            APP_ERROR_CHECK(err_code);
        } break; // BLE_ARS_C_EVT_DISCOVERY_COMPLETE

        case BLE_ARS_C_EVT_BUTTON_NOTIFICATION:
        {
            NRF_LOG_INFO("Assistance Request state changed on peer to 0x%x.", p_ars_c_evt->params.request.req_state);
            if (p_ars_c_evt->params.request.req_state)
            {
                bsp_board_led_on(ASSISTANCE_REQUEST_LED);
            }
            else
            {
                bsp_board_led_off(ASSISTANCE_REQUEST_LED);
            }
        } break; // BLE_ARS_C_EVT_BUTTON_NOTIFICATION

        default:
            // No implementation needed.
            break;
    }
}


/**@brief User function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated when button is pressed.
 */
void bsp_event_handler(bsp_event_t event) {
    ret_code_t err_code;

    switch (event) {
        case BSP_EVENT_SLEEP:
            sleep_mode_enter();
            break;

        case ASSISTANCE_REQUEST_ACK_BUTTON: {
            bsp_board_led_off(ASSISTANCE_REQUEST_LED);
        } break;

        default: break;
    }

    ble_bsp_evt_handler(event);
}


/**@brief User function for handling BLE advertising events.
 *
 * @param[in]   ble_adv_evt   Advertising event.
 */
void ble_adv_evt_handler(ble_adv_evt_t ble_adv_evt) {
    switch (ble_adv_evt) {
        case BLE_ADV_EVT_IDLE:
            sleep_mode_enter();
            break;

        default: break;
    }
}


/**@brief User function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
void ble_evt_handler(const ble_evt_t* p_ble_evt, void* p_context) {
    ret_code_t err_code = NRF_SUCCESS;
    uint8_t assist_requested;

    const ble_gap_evt_t* p_gap_evt = &p_ble_evt->evt.gap_evt;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED: {
            bsp_board_led_on(CONNECTED_LED);
            err_code = ble_ars_c_handles_assign(&m_ble_ars_c, p_gap_evt->conn_handle, NULL);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GAP_EVT_DISCONNECTED: {
            bsp_board_led_off(CONNECTED_LED);
        } break;

        case BLE_GATTC_EVT_READ_RSP: {
            assist_requested = p_ble_evt->evt.gattc_evt.params.read_rsp.data[0];

            if (assist_requested) {
                NRF_LOG_INFO("Assistance request received");
                bsp_board_led_on(ASSISTANCE_REQUEST_LED);

                NRF_LOG_INFO("Acknowledging assistance request...");
                err_code = ble_ars_c_assist_req_send(&m_ble_ars_c, false);
                if (err_code != NRF_SUCCESS &&
                    err_code != BLE_ERROR_INVALID_CONN_HANDLE &&
                    err_code != NRF_ERROR_INVALID_STATE) {
                    APP_ERROR_CHECK(err_code);
                    NRF_LOG_INFO("Assistance request acknowledgement sent");
                }
                if (err_code == NRF_SUCCESS) {
                    NRF_LOG_INFO("Failed to send assistance request acknowledgement");
                }
            }
            else {
                bsp_board_led_off(ASSISTANCE_REQUEST_LED);
            }
        } break;

        default: break;
    }
}


/**@brief Function for handling database discovery events.
 *
 * @param[in] p_evt  The database discover event
 */
static void db_disc_handler(ble_db_discovery_evt_t* p_evt)
{
    ble_ars_on_db_disc_evt(&m_ble_ars_c, p_evt);
}

/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
static void idle_state_handle(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}


/**@brief Assistance Request client initialization.
 */
static void ars_c_init(nrf_ble_gq_t* p_gatt_queue)
{
    ret_code_t       err_code;
    ble_ars_c_init_t ars_c_init_obj;

    ars_c_init_obj.evt_handler   = ars_c_evt_handler;
    ars_c_init_obj.p_gatt_queue  = p_gatt_queue;
    ars_c_init_obj.error_handler = ars_error_handler;

    err_code = ble_ars_c_init(&m_ble_ars_c, &ars_c_init_obj);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for application main entry.
 */
int main(void)
{
    bool erase_bonds;

    // Board services config
    board_services_init_t board_init = {0};
    board_init.bsp_evt_handler = bsp_event_handler;
    board_init.erase_bonds     = &erase_bonds;

    // BLE services config
    ble_services_init_t ble_init = {0};
    ble_gattc_service_init_func_t init_funcs[] = {
        ars_c_init
    };

    ble_init.p_ble_advertising       = &m_advertising;
    ble_init.p_ble_db_discovery      = &m_db_disc;
    ble_init.p_ble_gatt              = &m_gatt;
    ble_init.p_ble_qatt_queue        = &m_gatt_queue;
    ble_init.p_ble_qwr               = &m_qwr;
    ble_init.adv_evt_handler         = ble_adv_evt_handler;
    ble_init.ble_evt_handler         = ble_evt_handler;
    ble_init.db_disc_evt_handler     = db_disc_handler;
    ble_init.gattc_init_funcs        = init_funcs;
    ble_init.gattc_init_func_count   = sizeof(init_funcs) / sizeof(init_funcs[0]);

    // Initialize
    board_services_init(&board_init);
    ble_services_init(&ble_init);

    // Start execution
    NRF_LOG_INFO("Assistance server started");

    advertising_start(erase_bonds);

    // Enter main loop
    for (;;) {
        idle_state_handle();
    }
}


/**
 * @}
 */
