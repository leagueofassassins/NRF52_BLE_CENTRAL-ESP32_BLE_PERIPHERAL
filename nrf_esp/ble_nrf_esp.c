/**
  ******************************************************************************
  * @file    ble_nrf_esp.c
  * @author  leagueofassaissins
  * @brief   nrf ble central device implementation as a GATT client.
  ******************************************************************************
  */

#include "sdk_common.h"
#include "ble_db_discovery.h"
#include "ble_types.h"
#include "ble_gattc.h"
#include "ble_nrf_esp.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define WRITE_MESSAGE_LENGTH   BLE_CCCD_VALUE_LEN

/**@brief Function for intercepting the errors of GATTC and the BLE GATT Queue.
 *
 * @param[in] nrf_error   Error code.
 * @param[in] p_ctx       Parameter from the event handler.
 * @param[in] conn_handle Connection handle.
 */
static void gatt_error_handler(uint32_t   nrf_error,
                               void     * p_ctx,
                               uint16_t   conn_handle)
{
    ble_nrf_esp_t * p_ble_nrf_esp = (ble_nrf_esp_t *)p_ctx;

    NRF_LOG_DEBUG("A GATT Client error has occurred on conn_handle: 0X%X", conn_handle);

    if (p_ble_nrf_esp->error_handler != NULL)
    {
        p_ble_nrf_esp->error_handler(nrf_error);
    }
}

/**@brief Function for handling Handle Value Notification received from the SoftDevice.
 *
 * @details This function uses the Handle Value Notification received from the SoftDevice
 *          and checks whether it is a notification of accelerometer value from the peer. If
 *          it is, this function decodes the value of accelerometer and sends it to the
 *          application.
 *
 * @param[in] p_ble_nrf_esp Pointer to the accelerometer Client structure.
 * @param[in] p_ble_evt     Pointer to the BLE event received.
 */
static void on_hvx(ble_nrf_esp_t * p_ble_nrf_esp, ble_evt_t const * p_ble_evt)
{
    // Check if the event is on the link for this instance.
    if (p_ble_nrf_esp->conn_handle != p_ble_evt->evt.gattc_evt.conn_handle)
    {
        return;
    }
    // Check if this is a aacelerometer notification.
    if (p_ble_evt->evt.gattc_evt.params.hvx.handle == p_ble_nrf_esp->peer_nrf_esp_db.acc_handle)
    {
            ble_nrf_esp_evt_t ble_nrf_esp_evt;

            ble_nrf_esp_evt.evt_type                   = BLE_NRF_ESP_EVT_ACC_NOTIFICATION;
            ble_nrf_esp_evt.conn_handle                = p_ble_nrf_esp->conn_handle;
            for(int i=0;i<p_ble_evt->evt.gattc_evt.params.hvx.len;i++){
                ble_nrf_esp_evt.params.acc.acc_value[i]   = (char)p_ble_evt->evt.gattc_evt.params.hvx.data[i];
            }
           
            p_ble_nrf_esp->evt_handler(p_ble_nrf_esp, &ble_nrf_esp_evt);
    }

    if (p_ble_evt->evt.gattc_evt.params.hvx.handle == p_ble_nrf_esp->peer_nrf_esp_db.acc1_handle)
    {
            ble_nrf_esp_evt_t ble_nrf_esp_evt;

            ble_nrf_esp_evt.evt_type                   = BLE_NRF_ESP_EVT_ACC1_NOTIFICATION;
            ble_nrf_esp_evt.conn_handle                = p_ble_nrf_esp->conn_handle;
            for(int i=0;i<p_ble_evt->evt.gattc_evt.params.hvx.len;i++){
                ble_nrf_esp_evt.params.acc.acc1_value[i]   = (char)p_ble_evt->evt.gattc_evt.params.hvx.data[i];
            }
            p_ble_nrf_esp->evt_handler(p_ble_nrf_esp, &ble_nrf_esp_evt);
    }

    if (p_ble_evt->evt.gattc_evt.params.hvx.handle == p_ble_nrf_esp->peer_nrf_esp_db.acc2_handle)
    {
            ble_nrf_esp_evt_t ble_nrf_esp_evt;

            ble_nrf_esp_evt.evt_type                   = BLE_NRF_ESP_EVT_ACC2_NOTIFICATION;
            ble_nrf_esp_evt.conn_handle                = p_ble_nrf_esp->conn_handle;
            for(int i=0;i<p_ble_evt->evt.gattc_evt.params.hvx.len;i++){
                ble_nrf_esp_evt.params.acc.acc2_value[i]   = (char)p_ble_evt->evt.gattc_evt.params.hvx.data[i];
            }
            p_ble_nrf_esp->evt_handler(p_ble_nrf_esp, &ble_nrf_esp_evt);
    }
    
}


/**@brief Function for handling the Disconnected event received from the SoftDevice.
 *
 * @details This function checks whether the disconnect event is happening on the link
 *          associated with the current instance of the module. If the event is happening, the function sets the instance's
 *          conn_handle to invalid.
 *
 * @param[in] p_ble_nrf_esp Pointer to the accelerometer Client structure.
 * @param[in] p_ble_evt     Pointer to the BLE event received.
 */
static void on_disconnected(ble_nrf_esp_t * p_ble_nrf_esp, ble_evt_t const * p_ble_evt)
{
    if (p_ble_nrf_esp->conn_handle == p_ble_evt->evt.gap_evt.conn_handle)
    {
        p_ble_nrf_esp->conn_handle                     = BLE_CONN_HANDLE_INVALID;
        p_ble_nrf_esp->peer_nrf_esp_db.acc_cccd_handle = BLE_GATT_HANDLE_INVALID;
        p_ble_nrf_esp->peer_nrf_esp_db.acc_handle      = BLE_GATT_HANDLE_INVALID;
        p_ble_nrf_esp->peer_nrf_esp_db.acc1_cccd_handle = BLE_GATT_HANDLE_INVALID;
        p_ble_nrf_esp->peer_nrf_esp_db.acc1_handle      = BLE_GATT_HANDLE_INVALID;
        p_ble_nrf_esp->peer_nrf_esp_db.acc2_cccd_handle = BLE_GATT_HANDLE_INVALID;
        p_ble_nrf_esp->peer_nrf_esp_db.acc2_handle      = BLE_GATT_HANDLE_INVALID;
    }
}

/**@brief Function is called when database discovery is complete.
 *
 * @details This fuction basically assigns characterstic handle and cccd handle to the 
 *          particular characteristic discovered in database
 *
 * @param[in] p_ble_nrf_esp              Pointer to the accelerometer Client structure.
 * @param[in] ble_db_discovery_evt_t     Pointer to the database discovery event received.
 */
void ble_nrf_esp_on_db_disc_evt(ble_nrf_esp_t * p_ble_nrf_esp, const ble_db_discovery_evt_t * p_evt)
{
    // Check if the nrf esp Service was discovered.
    NRF_LOG_INFO("serv uuid is 0x%04x",p_evt->params.discovered_db.srv_uuid.uuid)
    NRF_LOG_INFO("event type is  %d",p_evt->evt_type)
    NRF_LOG_INFO("uuid type is  %d",p_evt->params.discovered_db.srv_uuid.type)
    NRF_LOG_INFO("uuid type is  %d",p_ble_nrf_esp->uuid_type)

    if (p_evt->evt_type == BLE_DB_DISCOVERY_COMPLETE &&
        p_evt->params.discovered_db.srv_uuid.uuid == NRF_ESP_UUID_SERVICE &&
        p_evt->params.discovered_db.srv_uuid.type == p_ble_nrf_esp->uuid_type)
    {
        ble_nrf_esp_evt_t evt;

        evt.evt_type    = BLE_NRF_ESP_EVT_DISCOVERY_COMPLETE;
        evt.conn_handle = p_evt->conn_handle;

        for (uint32_t i = 0; i < p_evt->params.discovered_db.char_count; i++)
        {
            const ble_gatt_db_char_t * p_char = &(p_evt->params.discovered_db.charateristics[i]);
            NRF_LOG_INFO("char uuid is 0x%04x",p_char->characteristic.uuid.uuid)
            switch (p_char->characteristic.uuid.uuid)
            {
                case NRF_ESP_UUID_ACC_CHAR:
                    evt.params.peer_db.acc_handle      = p_char->characteristic.handle_value;
                    evt.params.peer_db.acc_cccd_handle = p_char->cccd_handle;
                    break;

                case NRF_ESP_UUID_ACC1_CHAR:
                    evt.params.peer_db.acc1_handle      = p_char->characteristic.handle_value;
                    evt.params.peer_db.acc1_cccd_handle = p_char->cccd_handle;
                    break;

                case NRF_ESP_UUID_ACC2_CHAR:
                    evt.params.peer_db.acc2_handle      = p_char->characteristic.handle_value;
                    evt.params.peer_db.acc2_cccd_handle = p_char->cccd_handle;
                    break;

                default:
                    break;
            }
        }

        NRF_LOG_DEBUG("nrf esp Service discovered at peer.");
        //If the instance was assigned prior to db_discovery, assign the db_handles
        if (p_ble_nrf_esp->conn_handle != BLE_CONN_HANDLE_INVALID)
        {
            if ((p_ble_nrf_esp->peer_nrf_esp_db.acc_handle         == BLE_GATT_HANDLE_INVALID)&&
                (p_ble_nrf_esp->peer_nrf_esp_db.acc_cccd_handle      == BLE_GATT_HANDLE_INVALID))
            {
                p_ble_nrf_esp->peer_nrf_esp_db = evt.params.peer_db;
            }
        }

        p_ble_nrf_esp->evt_handler(p_ble_nrf_esp, &evt);

    }
}

/**@brief Function is called to initialse ble parameters.
 *
 * @details This function registers service uuid and event handler
 *
 * @param[in] p_ble_nrf_esp_t            Pointer to the accelerometer Client structure.
 * @param[in] ble_nrf_esp_init_t         Pointer to the BLE initialize parameters.
 */
uint32_t ble_nrf_esp_init(ble_nrf_esp_t * p_ble_nrf_esp, ble_nrf_esp_init_t * p_ble_nrf_esp_init)
{
    uint32_t      err_code;
    ble_uuid_t    nrf_esp_uuid;
    ble_uuid128_t nrf_esp_base_uuid = {NRF_ESP_UUID_BASE};

    VERIFY_PARAM_NOT_NULL(p_ble_nrf_esp_init);
    VERIFY_PARAM_NOT_NULL(p_ble_nrf_esp_init);
    VERIFY_PARAM_NOT_NULL(p_ble_nrf_esp_init->evt_handler);
    VERIFY_PARAM_NOT_NULL(p_ble_nrf_esp_init->p_gatt_queue);

    p_ble_nrf_esp->peer_nrf_esp_db.acc_cccd_handle  = BLE_GATT_HANDLE_INVALID;
    p_ble_nrf_esp->peer_nrf_esp_db.acc_handle       = BLE_GATT_HANDLE_INVALID;
    p_ble_nrf_esp->conn_handle                      = BLE_CONN_HANDLE_INVALID;
    p_ble_nrf_esp->evt_handler                      = p_ble_nrf_esp_init->evt_handler;
    p_ble_nrf_esp->p_gatt_queue                     = p_ble_nrf_esp_init->p_gatt_queue;
    p_ble_nrf_esp->error_handler                    = p_ble_nrf_esp_init->error_handler;

    err_code = sd_ble_uuid_vs_add(&nrf_esp_base_uuid, &p_ble_nrf_esp->uuid_type);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    VERIFY_SUCCESS(err_code);

    nrf_esp_uuid.type = p_ble_nrf_esp->uuid_type;
    nrf_esp_uuid.uuid = NRF_ESP_UUID_SERVICE;
    return ble_db_discovery_evt_register(&nrf_esp_uuid);
}

/**@brief Function for getting notified value and also handles disconnection events.
 *
 * @param[in] ble_nrf_esp_t Pointer to the accelerometer Client structure..
 *
 * @return NRF_SUCCESS if the CCCD configure was successfully sent to the peer.
 */
void ble_nrf_esp_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    if ((p_context == NULL) || (p_ble_evt == NULL))
    {
        return;
    }

    ble_nrf_esp_t * p_ble_nrf_esp = (ble_nrf_esp_t *)p_context;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GATTC_EVT_HVX:
            on_hvx(p_ble_nrf_esp, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnected(p_ble_nrf_esp, p_ble_evt);
            break;

        default:
            break;
    }
}

/**@brief Function for configuring the CCCD.
 *
 * @param[in] ble_nrf_esp_t Pointer to the accelerometer Client structure..
 * @param[in] enable        Whether to enable or disable the CCCD.
 *
 * @return NRF_SUCCESS if the CCCD configure was successfully sent to the peer.
 */
static uint32_t cccd_configure(ble_nrf_esp_t * p_ble_nrf_esp, bool enable)
{
    NRF_LOG_DEBUG("Configuring CCCD. CCCD Handle = %d, Connection Handle = %d",
                  p_ble_nrf_esp->peer_nrf_esp_db.acc_cccd_handle,
                  p_ble_nrf_esp->conn_handle);

    nrf_ble_gq_req_t cccd_req;
    uint16_t         cccd_val = enable ? BLE_GATT_HVX_NOTIFICATION : 0;
    uint8_t          cccd[WRITE_MESSAGE_LENGTH];

    cccd[0] = LSB_16(cccd_val);
    cccd[1] = MSB_16(cccd_val);

    cccd_req.type                        = NRF_BLE_GQ_REQ_GATTC_WRITE;
    cccd_req.error_handler.cb            = gatt_error_handler;
    cccd_req.error_handler.p_ctx         = p_ble_nrf_esp;
    cccd_req.params.gattc_write.handle   = p_ble_nrf_esp->peer_nrf_esp_db.acc_cccd_handle;
    cccd_req.params.gattc_write.len      = WRITE_MESSAGE_LENGTH;
    cccd_req.params.gattc_write.offset   = 0;
    cccd_req.params.gattc_write.p_value  = cccd;
    cccd_req.params.gattc_write.write_op = BLE_GATT_OP_WRITE_REQ;

    return nrf_ble_gq_item_add(p_ble_nrf_esp->p_gatt_queue, &cccd_req, p_ble_nrf_esp->conn_handle);
}

/**@brief Function for configuring the CCCD1.
 *
 * @param[in] ble_nrf_esp_t Pointer to the accelerometer Client structure.
 * @param[in] enable        Whether to enable or disable the CCCD1.
 *
 * @return NRF_SUCCESS if the CCCD configure was successfully sent to the peer.
 */
static uint32_t cccd1_configure(ble_nrf_esp_t * p_ble_nrf_esp, bool enable)
{
    NRF_LOG_DEBUG("Configuring CCCD. CCCD Handle = %d, Connection Handle = %d",
                  p_ble_nrf_esp->peer_nrf_esp_db.acc1_cccd_handle,
                  p_ble_nrf_esp->conn_handle);

    nrf_ble_gq_req_t cccd_req;
    uint16_t         cccd_val = enable ? BLE_GATT_HVX_NOTIFICATION : 0;
    uint8_t          cccd[WRITE_MESSAGE_LENGTH];

    cccd[0] = LSB_16(cccd_val);
    cccd[1] = MSB_16(cccd_val);

    cccd_req.type                        = NRF_BLE_GQ_REQ_GATTC_WRITE;
    cccd_req.error_handler.cb            = gatt_error_handler;
    cccd_req.error_handler.p_ctx         = p_ble_nrf_esp;
    cccd_req.params.gattc_write.handle   = p_ble_nrf_esp->peer_nrf_esp_db.acc1_cccd_handle;
    cccd_req.params.gattc_write.len      = WRITE_MESSAGE_LENGTH;
    cccd_req.params.gattc_write.offset   = 0;
    cccd_req.params.gattc_write.p_value  = cccd;
    cccd_req.params.gattc_write.write_op = BLE_GATT_OP_WRITE_REQ;

    return nrf_ble_gq_item_add(p_ble_nrf_esp->p_gatt_queue, &cccd_req, p_ble_nrf_esp->conn_handle);
}

/**@brief Function for configuring the CCCD2.
 *
 * @param[in] ble_nrf_esp_t Pointer to the accelerometer Client structure..
 * @param[in] enable        Whether to enable or disable the CCCD2.
 *
 * @return NRF_SUCCESS if the CCCD configure was successfully sent to the peer.
 */
static uint32_t cccd2_configure(ble_nrf_esp_t * p_ble_nrf_esp, bool enable)
{
    NRF_LOG_DEBUG("Configuring CCCD. CCCD Handle = %d, Connection Handle = %d",
                  p_ble_nrf_esp->peer_nrf_esp_db.acc2_cccd_handle,
                  p_ble_nrf_esp->conn_handle);

    nrf_ble_gq_req_t cccd_req;
    uint16_t         cccd_val = enable ? BLE_GATT_HVX_NOTIFICATION : 0;
    uint8_t          cccd[WRITE_MESSAGE_LENGTH];

    cccd[0] = LSB_16(cccd_val);
    cccd[1] = MSB_16(cccd_val);

    cccd_req.type                        = NRF_BLE_GQ_REQ_GATTC_WRITE;
    cccd_req.error_handler.cb            = gatt_error_handler;
    cccd_req.error_handler.p_ctx         = p_ble_nrf_esp;
    cccd_req.params.gattc_write.handle   = p_ble_nrf_esp->peer_nrf_esp_db.acc2_cccd_handle;
    cccd_req.params.gattc_write.len      = WRITE_MESSAGE_LENGTH;
    cccd_req.params.gattc_write.offset   = 0;
    cccd_req.params.gattc_write.p_value  = cccd;
    cccd_req.params.gattc_write.write_op = BLE_GATT_OP_WRITE_REQ;

    return nrf_ble_gq_item_add(p_ble_nrf_esp->p_gatt_queue, &cccd_req, p_ble_nrf_esp->conn_handle);
}

/**@brief Function for subscribing to get notified value.
 *
 * @param[in] ble_nrf_esp_t Pointer to the accelerometer Client structure.
 */
uint32_t ble_nrf_esp_acc_notif_enable(ble_nrf_esp_t * p_ble_nrf_esp)
{
    VERIFY_PARAM_NOT_NULL(p_ble_nrf_esp);

    if (p_ble_nrf_esp->conn_handle == BLE_CONN_HANDLE_INVALID)
    {
        return NRF_ERROR_INVALID_STATE;
    }

    return cccd_configure(p_ble_nrf_esp,
                          true);
}

/**@brief Function for subscribing to get notified value.
 *
 * @param[in] ble_nrf_esp_t Pointer to the accelerometer Client structure.
 */
uint32_t ble_nrf_esp_acc1_notif_enable(ble_nrf_esp_t * p_ble_nrf_esp)
{
    VERIFY_PARAM_NOT_NULL(p_ble_nrf_esp);

    if (p_ble_nrf_esp->conn_handle == BLE_CONN_HANDLE_INVALID)
    {
        return NRF_ERROR_INVALID_STATE;
    }

    return cccd1_configure(p_ble_nrf_esp,
                          true);
}

/**@brief Function for subscribing to get notified value.
 *
 * @param[in] ble_nrf_esp_t Pointer to the accelerometer Client structure.
 */
uint32_t ble_nrf_esp_acc2_notif_enable(ble_nrf_esp_t * p_ble_nrf_esp)
{
    VERIFY_PARAM_NOT_NULL(p_ble_nrf_esp);

    if (p_ble_nrf_esp->conn_handle == BLE_CONN_HANDLE_INVALID)
    {
        return NRF_ERROR_INVALID_STATE;
    }

    return cccd2_configure(p_ble_nrf_esp,
                          true);
}
/**@brief Function for registering connection handle.
 *
 * @param[in] ble_nrf_esp_t Pointer to the accelerometer Client structure.
 * @param[in] conn_handle   variable to connection handle 
 */
uint32_t ble_nrf_esp_handles_assign(ble_nrf_esp_t    * p_ble_nrf_esp,
                                  uint16_t         conn_handle,
                                  const nrf_esp_db_t * p_peer_handles)
{
    VERIFY_PARAM_NOT_NULL(p_ble_nrf_esp);

    p_ble_nrf_esp->conn_handle = conn_handle;
    if (p_peer_handles != NULL)
    {
        p_ble_nrf_esp->peer_nrf_esp_db = *p_peer_handles;
    }
    return nrf_ble_gq_conn_handle_register(p_ble_nrf_esp->p_gatt_queue, conn_handle);
}