/**@file
 *
 * @defgroup ble_nrf_esp_c Accelerometer Service Client
 * @{
 * @ingroup  ble_sdk_srv
 * @brief    The Accelerometer Service client can be used to read an accelerometer value on a
 *           accelerometer service server.
 *
 * @details  This module contains the APIs and types exposed by the accelerometer Service Client
 *           module. The application can use these APIs and types to perform the discovery of
 *           accelerometer Service at the peer and to interact with it.
 *
 * @note    The application must register this module as the BLE event observer by using the
 *          NRF_SDH_BLE_OBSERVER macro. Example
 */
#ifndef BLE_NRF_ESP_H__
#define BLE_NRF_ESP_H__

#include <stdint.h>
#include "ble.h"
#include "ble_db_discovery.h"
#include "ble_srv_common.h"
#include "nrf_ble_gq.h"
#include "nrf_sdh_ble.h"

/**@brief   Macro for defining a ble_nrf_esp_c instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_NRF_ESP_DEF(_name)                                                                        \
static ble_nrf_esp_t _name;                                                                           \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
                     2,                                                   \
                     ble_nrf_esp_on_ble_evt, &_name)

#define NRF_ESP_UUID_BASE         {0xD2, 0xB0, 0x52, 0x4F, 0xC4, 0x74, 0x43, 0xF3, 0x94, 0xF5, 0xB2, 0x97, 0x00, 0x00, 0xE7, 0x6F}
#define NRF_ESP_UUID_SERVICE      0x42F3
#define NRF_ESP_UUID_ACC_CHAR     0xFF01
#define NRF_ESP_UUID_ACC1_CHAR    0xFF02
#define NRF_ESP_UUID_ACC2_CHAR    0xFF03

//d2 b0 52 4f c4 74 43 f3 94 f5 b2 97 f3 42 e7 6f
//000015231212efde1523785feabcd123
//000015241212efde1523785feabcd123
//23d1bcea5f782315deef121223150000

/**@brief Client event type. */
typedef enum
{
    BLE_NRF_ESP_EVT_DISCOVERY_COMPLETE = 1,  
    BLE_NRF_ESP_EVT_ACC_NOTIFICATION,
    BLE_NRF_ESP_EVT_ACC1_NOTIFICATION,
    BLE_NRF_ESP_EVT_ACC2_NOTIFICATION
} ble_nrf_esp_evt_type_t;

/**@brief Structure containing the accelerometer value received from the peer. */
typedef struct
{
    char acc_value[10];  
    char acc1_value[10];
    char acc2_value[10];
} ble_acc_t;

/**@brief Structure containing the handles related to the accelerometer Service found on the peer. */
typedef struct
{
    uint16_t acc_cccd_handle;  
    uint16_t acc_handle; 
    uint16_t acc1_cccd_handle;
    uint16_t acc1_handle;
    uint16_t acc2_cccd_handle;
    uint16_t acc2_handle;      
} nrf_esp_db_t;

/**@brief Accelerometer Event structure. */
typedef struct
{
    ble_nrf_esp_evt_type_t evt_type;    /**< Type of the event. */
    uint16_t               conn_handle; /**< Connection handle on which the event occured.*/
    union
    {
        ble_acc_t        acc;          
        nrf_esp_db_t     peer_db;         
    } params;
} ble_nrf_esp_evt_t;
// Forward declaration of the ble_nrf_esp_t type.
typedef struct ble_nrf_esp_s ble_nrf_esp_t;

/**@brief   Event handler type.
 *
 * @details This is the type of the event handler that is to be provided by the application
 *          of this module in order to receive events.
 */
typedef void (* ble_nrf_esp_evt_handler_t) (ble_nrf_esp_t * p_ble_nrf_esp, ble_nrf_esp_evt_t * p_evt);


struct ble_nrf_esp_s
{
    uint16_t                    conn_handle;      /**< Connection handle as provided by the SoftDevice. */
    nrf_esp_db_t                peer_nrf_esp_db;  
    ble_nrf_esp_evt_handler_t   evt_handler;      /**< Application event handler to be called when there is an event related to the accelerometer service. */
    ble_srv_error_handler_t     error_handler;    /**< Function to be called in case of an error. */
    uint8_t                     uuid_type;        /**< UUID type. */
    nrf_ble_gq_t                * p_gatt_queue;   /**< Pointer to the BLE GATT Queue instance. */
};

/**@brief Accelerometer Client initialization structure. */
typedef struct
{
    ble_nrf_esp_evt_handler_t   evt_handler;     /**< Event handler to be called by the Client module when there is an event related to the accelerometer Service. */
    nrf_ble_gq_t                * p_gatt_queue;  /**< Pointer to the BLE GATT Queue instance. */
    ble_srv_error_handler_t     error_handler;   /**< Function to be called in case of an error. */
} ble_nrf_esp_init_t;

uint32_t ble_nrf_esp_init(ble_nrf_esp_t * p_ble_nrf_esp, ble_nrf_esp_init_t * p_ble_nrf_esp_init);

void ble_nrf_esp_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);

uint32_t ble_nrf_esp_acc_notif_enable(ble_nrf_esp_t * p_ble_nrf_esp);

uint32_t ble_nrf_esp_acc1_notif_enable(ble_nrf_esp_t * p_ble_nrf_esp);

uint32_t ble_nrf_esp_acc2_notif_enable(ble_nrf_esp_t * p_ble_nrf_esp);

void ble_nrf_esp_on_db_disc_evt(ble_nrf_esp_t * p_ble_nrf_esp, const ble_db_discovery_evt_t * p_evt);

uint32_t ble_nrf_esp_handles_assign(ble_nrf_esp_t    * p_ble_nrf_esp,
                                    uint16_t         conn_handle, 
                                    const nrf_esp_db_t   * p_peer_handles);

#endif // BLE_NRF_ESP_H__