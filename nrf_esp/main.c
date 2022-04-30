/**
 * @brief BLE Accelerometer Service central and client application main file.
 *
 * This file contains the source code for a sample client application using the accelerometer service.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"
#include "nrf_pwr_mgmt.h"
#include "app_timer.h"
#include "boards.h"
#include "bsp.h"
#include "bsp_btn_ble.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_db_discovery.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_scan.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include <string.h>
#include "ble_nrf_esp.h"
#define CENTRAL_SCANNING_LED            BSP_BOARD_LED_0                     /**< Scanning LED will be on when the device is scanning. */
#define CENTRAL_CONNECTED_LED           BSP_BOARD_LED_2                     /**< Connected LED will be on when the device is connected. */
#define MIN_CONNECTION_INTERVAL         MSEC_TO_UNITS(7.5, UNIT_1_25_MS)    /**< Determines minimum connection interval in milliseconds. */
#define MAX_CONNECTION_INTERVAL         MSEC_TO_UNITS(30, UNIT_1_25_MS)     /**< Determines maximum connection interval in milliseconds. */
#define SLAVE_LATENCY                   0                                   /**< Determines slave latency in terms of connection events. */
#define SUPERVISION_TIMEOUT             MSEC_TO_UNITS(4000, UNIT_10_MS)     /**< Determines supervision time-out in units of 10 milliseconds. */
#define APP_BLE_CONN_CFG_TAG            1                                   /**< A tag identifying the SoftDevice BLE configuration. */
#define APP_BLE_OBSERVER_PRIO           3                                   /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define SCAN_DURATION_WITELIST  3000  

BLE_DB_DISCOVERY_DEF(m_db_disc);                                /**< DB discovery module instance. */
BLE_NRF_ESP_DEF(m_ble_nrf_esp);                                     
NRF_BLE_SCAN_DEF(m_scan);                                       /**< Scanning module instance. */
NRF_BLE_GATT_DEF(m_gatt);                                       /**< GATT module instance. */
NRF_BLE_GQ_DEF(m_ble_gatt_queue,                                /**< BLE GATT Queue instance. */
               NRF_SDH_BLE_CENTRAL_LINK_COUNT,
               NRF_BLE_GQ_QUEUE_SIZE);


volatile uint8_t battery_percent;

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
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}

static void nrf_esp_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

static ble_gap_scan_params_t const m_scan_param =
{
    .active        = 0x01,
    .interval      = NRF_BLE_SCAN_SCAN_INTERVAL,
    .window        = NRF_BLE_SCAN_SCAN_WINDOW,
    .filter_policy = BLE_GAP_SCAN_FP_ACCEPT_ALL, //BLE_GAP_SCAN_FP_WHITELIST,
    .timeout       = SCAN_DURATION_WITELIST,
    .scan_phys     = BLE_GAP_PHY_1MBPS,
};

void print_manufacturer_data(const ble_gap_evt_adv_report_t* p_adv_report) {

    uint16_t offset_uuid = 0;
    uint16_t offset_manuf = 0;
    char uuid_esp [] = "d2b0524fc47443f394f5b297f342e76f";
    char manuf_esp[] = "e502246f281742fc3b00";
    //char uuid_esp [] = "23d1bcea5f782315deef121223150000";
    uint8_t result_manuf = 0;
    uint8_t result_uuid = 0;

    ble_gap_scan_params_t const * p_scan_params = &m_scan.scan_params;
    ble_gap_conn_params_t const * p_conn_params = &m_scan.conn_params;
    uint8_t                       con_cfg_tag   = m_scan.conn_cfg_tag;
    ble_gap_addr_t const        * p_addr        = &p_adv_report->peer_addr;

    uint16_t length = ble_advdata_search(p_adv_report->data.p_data, p_adv_report->data.len, &offset_manuf, BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA);
    if (length != 0) {
        char manuf_string[1024] = { 0 };
        char* pos = manuf_string;
        for (int manuf_length = 0; manuf_length < length && manuf_length < 512; manuf_length++) {
            sprintf(pos, "%02x", p_adv_report->data.p_data[offset_manuf+manuf_length]);
            pos += 2;
        }
        result_manuf = strcmp(manuf_string,manuf_esp);
        if(result_manuf == 0){
            battery_percent = p_adv_report->data.p_data[offset_manuf+8];
        }
    }

    uint16_t length_uuid = ble_advdata_search(p_adv_report->data.p_data, p_adv_report->data.len, &offset_uuid, BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE);

    if (length_uuid != 0) {
        char uuid_string[1024] = { 0 };
        char* pos_uuid = uuid_string;

        for (int i = 0; i < length_uuid && i < 512; i++) {

            sprintf(pos_uuid, "%02x", p_adv_report->data.p_data[offset_uuid+i]);
            pos_uuid += 2;
        }

        //NRF_LOG_INFO("service uuid: %s", nrf_log_push(uuid_string));
        result_uuid = strcmp(uuid_string,uuid_esp);

        if(result_uuid == 0){
            NRF_LOG_INFO("esp device found");
            for (int peer_address_len = 0;peer_address_len < 6; peer_address_len++){
                NRF_LOG_INFO("address: %02x", p_adv_report->peer_addr.addr[peer_address_len]);
            }
            NRF_LOG_INFO("battery percent: %d",battery_percent);
            sd_ble_gap_connect (p_addr, p_scan_params, p_conn_params,con_cfg_tag);
        }

        else{
            NRF_LOG_INFO("esp device not found");   
        }    
    }

}
/**@brief Function for the LEDs initialization.
 *
 * @details Initializes all LEDs used by the application.
 */
static void leds_init(void)
{
    bsp_board_init(BSP_INIT_LEDS);
}


/**@brief Function to start scanning.
 */
static void scan_start(void)
{
    ret_code_t err_code;

    err_code = nrf_ble_scan_start(&m_scan);
    APP_ERROR_CHECK(err_code);

    bsp_board_led_off(CENTRAL_CONNECTED_LED);
    bsp_board_led_on(CENTRAL_SCANNING_LED);
}

static void nrf_esp_evt_handler(ble_nrf_esp_t * p_nrf_esp, ble_nrf_esp_evt_t * p_nrf_esp_evt)
{
    switch (p_nrf_esp_evt->evt_type)
    {
        case BLE_NRF_ESP_EVT_DISCOVERY_COMPLETE:
        {
            ret_code_t err_code;

            err_code = ble_nrf_esp_handles_assign(&m_ble_nrf_esp,
                                                  p_nrf_esp_evt->conn_handle,
                                                  &p_nrf_esp_evt->params.peer_db);
            NRF_LOG_INFO("nrf esp service discovered on conn_handle 0x%x.", p_nrf_esp_evt->conn_handle);

            // Accelerometer service discovered. Enable notification.
            err_code = ble_nrf_esp_acc_notif_enable(p_nrf_esp);
            APP_ERROR_CHECK(err_code);

            err_code = ble_nrf_esp_acc1_notif_enable(p_nrf_esp);
            APP_ERROR_CHECK(err_code);

            err_code = ble_nrf_esp_acc2_notif_enable(p_nrf_esp);
            APP_ERROR_CHECK(err_code);
        } break; 

        case BLE_NRF_ESP_EVT_ACC_NOTIFICATION:
        {
            NRF_LOG_INFO("Accelerometer X value is %s",NRF_LOG_PUSH(p_nrf_esp_evt->params.acc.acc_value));
        } break;

        case BLE_NRF_ESP_EVT_ACC1_NOTIFICATION:
        {
            NRF_LOG_INFO("Accelerometer Y value is %s",NRF_LOG_PUSH(p_nrf_esp_evt->params.acc.acc1_value));
        } break;

        case BLE_NRF_ESP_EVT_ACC2_NOTIFICATION:
        {
            NRF_LOG_INFO("Accelerometer Z value is %s", NRF_LOG_PUSH(p_nrf_esp_evt->params.acc.acc2_value));
        } break;

        default:
            // No implementation needed.
            break;
    }
}

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    ret_code_t err_code;

    // For readability.
    ble_gap_evt_t const * p_gap_evt = &p_ble_evt->evt.gap_evt;

    switch (p_ble_evt->header.evt_id)
    {
        // Upon connection, check which peripheral has connected (HR or RSC), initiate DB
        // discovery, update LEDs status and resume scanning if necessary. */
        case BLE_GAP_EVT_CONNECTED:
        {
            NRF_LOG_INFO("Connected.");
            // Update LEDs status, and check if we should be looking for more
            // peripherals to connect to.
            err_code = ble_nrf_esp_handles_assign(&m_ble_nrf_esp, p_gap_evt->conn_handle, NULL);
            APP_ERROR_CHECK(err_code);

            err_code = ble_db_discovery_start(&m_db_disc, p_gap_evt->conn_handle);
            APP_ERROR_CHECK(err_code);
            bsp_board_led_on(CENTRAL_CONNECTED_LED);
            bsp_board_led_off(CENTRAL_SCANNING_LED);
        } break;

        // Upon disconnection, reset the connection handle of the peer which disconnected, update
        // the LEDs status and start scanning again.
        case BLE_GAP_EVT_DISCONNECTED:
        {
            NRF_LOG_INFO("Disconnected.");
            scan_start();
        } break;

        case BLE_GAP_EVT_TIMEOUT:
        {
            // We have not specified a timeout for scanning, so only connection attemps can timeout.
            if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_CONN)
            {
                NRF_LOG_DEBUG("Connection request timed out.");
            }
        } break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
        {
            // Accept parameters requested by peer.
            err_code = sd_ble_gap_conn_param_update(p_gap_evt->conn_handle,
                                        &p_gap_evt->params.conn_param_update_request.conn_params);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GATTC_EVT_TIMEOUT:
        {
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GATTS_EVT_TIMEOUT:
        {
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
        } break;

        default:
            // No implementation needed.
            break;
    }
}

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupts.
 */
static void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}


/**@brief Function for handling Scaning events.
 *
 * @param[in]   p_scan_evt   Scanning event.
 */
static void scan_evt_handler(scan_evt_t const * p_scan_evt)
{
  //  ret_code_t err_code;

    /*switch(p_scan_evt->scan_evt_id)
    {
        case NRF_BLE_SCAN_EVT_CONNECTING_ERROR:
            err_code = p_scan_evt->params.connecting_err.err_code;
            APP_ERROR_CHECK(err_code);
            break;
        default:
          break;
    }*/
  print_manufacturer_data(p_scan_evt->params.filter_match.p_adv_report);

}

/**@brief Function for handling database discovery events.
 *
 * @details This function is callback function to handle events from the database discovery module.
 *          Depending on the UUIDs that are discovered, this function should forward the events
 *          to their respective services.
 *
 * @param[in] p_event  Pointer to the database discovery event.
 */

/**@brief Function for initializing the log.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**@brief Function for initializing the timer.
 */
static void timer_init(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Power manager. */
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}


static void scan_init(void)
{
    ret_code_t          err_code;
    nrf_ble_scan_init_t init_scan;

    memset(&init_scan, 0, sizeof(init_scan));
    init_scan.p_scan_param     = &m_scan_param;
    init_scan.connect_if_match = false;
    init_scan.conn_cfg_tag     = APP_BLE_CONN_CFG_TAG;

    err_code = nrf_ble_scan_init(&m_scan, &init_scan, scan_evt_handler);
    APP_ERROR_CHECK(err_code);

}


/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
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
static void db_disc_handler(ble_db_discovery_evt_t * p_evt)
{
    ble_nrf_esp_on_db_disc_evt(&m_ble_nrf_esp, p_evt);
}
static void db_discovery_init(void)
{
    ble_db_discovery_init_t db_init;

    memset(&db_init, 0, sizeof(db_init));

    db_init.evt_handler  = db_disc_handler;
    db_init.p_gatt_queue = &m_ble_gatt_queue;

    ret_code_t err_code = ble_db_discovery_init(&db_init);
    APP_ERROR_CHECK(err_code);
}

static void nrf_esp_init(void)
{
    ret_code_t         err_code;
    ble_nrf_esp_init_t nrf_esp_init_obj;

    nrf_esp_init_obj.evt_handler   = nrf_esp_evt_handler;
    nrf_esp_init_obj.p_gatt_queue  = &m_ble_gatt_queue;
    nrf_esp_init_obj.error_handler = nrf_esp_error_handler;

    err_code = ble_nrf_esp_init(&m_ble_nrf_esp, &nrf_esp_init_obj);
    APP_ERROR_CHECK(err_code);
}
int main(void)
{
    // Initialize.
    log_init();
    timer_init();
    leds_init();
    power_management_init();
    ble_stack_init();
    scan_init();
    gatt_init();
    db_discovery_init();
    nrf_esp_init();

    NRF_LOG_INFO("BLE SIMPLE CENTRAL example started.");
    NRF_LOG_FLUSH();
    __WFE();


    scan_start();
   

    // Enter main loop.
    for (;;)
    {
        idle_state_handle();
    }
}
