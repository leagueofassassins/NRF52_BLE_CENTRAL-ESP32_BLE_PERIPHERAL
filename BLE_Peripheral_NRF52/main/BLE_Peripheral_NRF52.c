/* @file  BLE_Peripheral_NRF52.c
   @brief this code involves ESP32 in BLE peripheral mode and as a GATT server.
          LIS2DW12 sensor is used to send X,Y,Z axes raw value to NRF52 Dongle
          central device using notification characteristic property.
   @author bheesma-10
*/

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "BLE_Peripheral.h"
#include "LIS2DW12.h"

static const char *TAG = "Bluedroid-Peripheral";

#define DEVICE_NAME  "ESP32"

#define PROFILE_NUM         1                              //Number of Application Profiles
#define PROFILE_A_APP_ID    0                              //Application Profile ID

#define SVC_INST_ID         0                              //Service Instance ID

#define ADV_CONFIG_FLAG                           (1 << 0)
#define SCAN_RSP_CONFIG_FLAG                      (1 << 1)

/*variables*/
#define GATTS_CHAR_VAL_LEN_MAX 100
#define GATTS_DESC_VAL_LEN_MAX 50
#define CHAR_DECLARATION_SIZE       (sizeof(uint8_t))


#define PREPARE_BUF_MAX_SIZE        1024
typedef struct {
    uint8_t                 *prepare_buf;
    int                     prepare_len;
} prepare_type_env_t;
 
static prepare_type_env_t prepare_write_env;

/*
***************************************************************************************************************
GAP Profile 
***************************************************************************************************************
*/
static uint8_t adv_config_done = 0;

static uint16_t ble_peripheral_nrf[IDX_NB];

uint8_t notify_data=0;
uint16_t cccd_flag[3]={0};
uint16_t conn_id=0;

typedef struct {

    uint8_t company_id[2];    //Espressif Semicoductors ID - 0x02E5
    uint8_t mac_address[6];
    uint8_t battery_percentage;

}manufacturer_data;

manufacturer_data device_data = {.company_id = {0xE5,0x02}, .mac_address = {0}, .battery_percentage = 59};

static uint8_t custom_manufacturer_data[10];


/*128 bits service uuid*/
static uint8_t sec_service_uuid[16] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    0xd2, 0xb0, 0x52, 0x4f, 0xc4, 0x74, 0x43, 0xf3, 0x94, 0xf5, 0xb2, 0x97, 0xf3, 0x42, 0xe7, 0x6f
};

/*config primary adv data*/
static esp_ble_adv_data_t esp32_ble_adv_config = {
    .set_scan_rsp = false,
    .include_name = false,
    .include_txpower = true,
    .min_interval = 0x0006, //slave connection min interval, Time = min_interval * 1.25 msec
    .max_interval = 0x0010, //slave connection max interval, Time = max_interval * 1.25 msec
    .appearance = 0x00,
    .manufacturer_len = sizeof(custom_manufacturer_data), 
    .p_manufacturer_data =  custom_manufacturer_data, 
    .service_data_len = 0,
    .p_service_data = NULL,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

/*config scan response data*/
static esp_ble_adv_data_t esp32_ble_scan_rsp_config = {
    .set_scan_rsp = true,
    .include_name = false,
    .service_uuid_len = sizeof(sec_service_uuid),
    .p_service_uuid = sec_service_uuid,
};

/*configure esp32 advertising connection parameters*/
static esp_ble_adv_params_t esp32_ble_adv_params = {
    .adv_int_min        = 0x100,
    .adv_int_max        = 0x100,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_RANDOM,   /*Resolvable Private Address (comes with a key shared between paired devices)*/
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};




/*
 ****************************************************************************************
 GATT Profile
 ****************************************************************************************
 */


/*characteristics id's*/
static const uint16_t X_AXES_UUID                  = 0xFF01;
static const uint16_t Y_AXES_UUID                  = 0xFF02;
static const uint16_t Z_AXES_UUID                  = 0xFF03;

/*primary uuid's of service ,characteristics and CCCD descriptor*/
static const uint16_t primary_service_uuid         = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid   = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;

/*properties of characteristics*/
static const uint8_t char_prop_read_write_notify   = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;

/*CCCD of the three characteristics*/
static const uint8_t ble_cccd[3][2]                = {{0x00, 0x00},{0x00,0x00},{0x00,0x00}};

static const uint8_t char_value[4]                 = {0x11, 0x22, 0x33, 0x44};

/*structure responsible for application profile attributes*/
struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};



static void gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                        esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);


 // One gatt-based profile one app_id and one gatts_if, this array will store the gatts_if returned by ESP_GATTS_REG_EVT 
static struct gatts_profile_inst profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gatts_cb = gatts_profile_event_handler,
        .gatts_if = ESP_GATT_IF_NONE,       /*means that the Application Profile is not linked to any client yet*/
    },

};


/* Full Database Description - Used to add attributes into the database */
static const esp_gatts_attr_db_t ble_gatt_db[IDX_NB] =
{
    // Service Declaration
    [IDX_SVC]        =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
      sizeof(uint16_t), sizeof(sec_service_uuid), (uint8_t *)&sec_service_uuid}},

    /* Characteristic Declaration (X-AXES)*/
    [IDX_CHAR_A]     =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write_notify}},

    /* Characteristic Value */
    [IDX_CHAR_VAL_A] =
    {
        {ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&X_AXES_UUID, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
        GATTS_CHAR_VAL_LEN_MAX, sizeof(char_value), (uint8_t *)char_value}
    },

    /* Client Characteristic Configuration Descriptor */
    [IDX_CHAR_CFG_A]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      sizeof(uint16_t), sizeof(ble_cccd[0]), (uint8_t *)ble_cccd[0]}},


    /* Characteristic Declaration (Y-AXES)*/
    [IDX_CHAR_B]  =  
    {
        {ESP_GATT_AUTO_RSP},{ESP_UUID_LEN_16,(uint8_t*)&character_declaration_uuid,ESP_GATT_PERM_READ,
            CHAR_DECLARATION_SIZE,CHAR_DECLARATION_SIZE,(uint8_t*)&char_prop_read_write_notify}
    },

    /* Characteristic Value*/
    [IDX_CHAR_VAL_B]  = 
    {
            {ESP_GATT_AUTO_RSP},{ESP_UUID_LEN_16,(uint8_t*)&Y_AXES_UUID,ESP_GATT_PERM_READ,
            GATTS_CHAR_VAL_LEN_MAX,sizeof(char_value),(uint8_t*)char_value}
    },

    /* Client Characteristic Configuration Descriptor */
    [IDX_CHAR_CFG_B]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      sizeof(uint16_t), sizeof(ble_cccd[1]), (uint8_t *)ble_cccd[1]}},

    /* Characteristics Declaration (Z-AXES)*/
    [IDX_CHAR_C] = 
    {
        {ESP_GATT_AUTO_RSP},{ESP_UUID_LEN_16,(uint8_t*)&character_declaration_uuid,ESP_GATT_PERM_READ,
            CHAR_DECLARATION_SIZE,CHAR_DECLARATION_SIZE,(uint8_t*)&char_prop_read_write_notify}
    },

    /* Characteristic Value*/
    [IDX_CHAR_VAL_C]  = 
    {
        {ESP_GATT_AUTO_RSP},{ESP_UUID_LEN_16,(uint8_t *)&Z_AXES_UUID,ESP_GATT_PERM_READ,
                    GATTS_CHAR_VAL_LEN_MAX,sizeof(char_value),(uint8_t*)char_value}
    },

    /* Client Characteristic Configuration Descriptor */
    [IDX_CHAR_CFG_C]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      sizeof(uint16_t), sizeof(ble_cccd[2]), (uint8_t *)ble_cccd[2]}},


};


static char *esp_key_type_to_str(esp_ble_key_type_t key_type)
{
   char *key_str = NULL;
   switch(key_type) {
    case ESP_LE_KEY_NONE:
        key_str = "ESP_LE_KEY_NONE";
        break;
    case ESP_LE_KEY_PENC:
        key_str = "ESP_LE_KEY_PENC";
        break;
    case ESP_LE_KEY_PID:
        key_str = "ESP_LE_KEY_PID";
        break;
    case ESP_LE_KEY_PCSRK:
        key_str = "ESP_LE_KEY_PCSRK";
        break;
    case ESP_LE_KEY_PLK:
        key_str = "ESP_LE_KEY_PLK";
        break;
    case ESP_LE_KEY_LLK:
        key_str = "ESP_LE_KEY_LLK";
        break;
    case ESP_LE_KEY_LENC:
        key_str = "ESP_LE_KEY_LENC";
        break;
    case ESP_LE_KEY_LID:
        key_str = "ESP_LE_KEY_LID";
        break;
    case ESP_LE_KEY_LCSRK:
        key_str = "ESP_LE_KEY_LCSRK";
        break;
    default:
        key_str = "INVALID BLE KEY TYPE";
        break;

   }

   return key_str;
}

static char *esp_auth_req_to_str(esp_ble_auth_req_t auth_req)
{
   char *auth_str = NULL;
   switch(auth_req) {
    case ESP_LE_AUTH_NO_BOND:
        auth_str = "ESP_LE_AUTH_NO_BOND";
        break;
    case ESP_LE_AUTH_BOND:
        auth_str = "ESP_LE_AUTH_BOND";
        break;
    case ESP_LE_AUTH_REQ_MITM:
        auth_str = "ESP_LE_AUTH_REQ_MITM";
        break;
    case ESP_LE_AUTH_REQ_BOND_MITM:
        auth_str = "ESP_LE_AUTH_REQ_BOND_MITM";
        break;
    case ESP_LE_AUTH_REQ_SC_ONLY:
        auth_str = "ESP_LE_AUTH_REQ_SC_ONLY";
        break;
    case ESP_LE_AUTH_REQ_SC_BOND:
        auth_str = "ESP_LE_AUTH_REQ_SC_BOND";
        break;
    case ESP_LE_AUTH_REQ_SC_MITM:
        auth_str = "ESP_LE_AUTH_REQ_SC_MITM";
        break;
    case ESP_LE_AUTH_REQ_SC_MITM_BOND:
        auth_str = "ESP_LE_AUTH_REQ_SC_MITM_BOND";
        break;
    default:
        auth_str = "INVALID BLE AUTH REQ";
        break;
   }

   return auth_str;
}

/**
 * @brief Lists all bonded devices(devices with shared pairing keys..........)
 */

static void show_bonded_devices(void)
{
    int dev_num = esp_ble_get_bond_device_num();

    esp_ble_bond_dev_t *dev_list = (esp_ble_bond_dev_t *)malloc(sizeof(esp_ble_bond_dev_t) * dev_num);
    esp_ble_get_bond_device_list(&dev_num, dev_list);
    ESP_LOGI(TAG, "Bonded devices number : %d\n", dev_num);

    ESP_LOGI(TAG, "Bonded devices list : %d\n", dev_num);
    for (int i = 0; i < dev_num; i++) {
        esp_log_buffer_hex(TAG, (void *)dev_list[i].bd_addr, sizeof(esp_bd_addr_t));
    }

    free(dev_list);
}

/**
 * @brief removes all bonded devices(devices with shared pairing keys..........)
 */
static void __attribute__((unused)) remove_all_bonded_devices(void)
{
    int dev_num = esp_ble_get_bond_device_num();

    esp_ble_bond_dev_t *dev_list = (esp_ble_bond_dev_t *)malloc(sizeof(esp_ble_bond_dev_t) * dev_num);
    esp_ble_get_bond_device_list(&dev_num, dev_list);
    for (int i = 0; i < dev_num; i++) {
        esp_ble_remove_bond_device(dev_list[i].bd_addr);
    }

    free(dev_list);
}

/**
 * @brief GAP event handler
 * @param event : Event type
 * @param param : Point to callback parameter, currently is union type
 */

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    ESP_LOGV(TAG, "GAP_EVT, event %d\n", event);

    switch (event) {
    case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
        adv_config_done &= (~SCAN_RSP_CONFIG_FLAG);
        if (adv_config_done == 0){
            esp_ble_gap_start_advertising(&esp32_ble_adv_params);
        }
        break;
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        adv_config_done &= (~ADV_CONFIG_FLAG);
        if (adv_config_done == 0){
            esp_ble_gap_start_advertising(&esp32_ble_adv_params);
        }
        break;
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        //advertising start complete event to indicate advertising start successfully or failed
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "advertising start failed, error status = %x", param->adv_start_cmpl.status);
            break;
        }
        ESP_LOGI(TAG, "advertising start success");
        break;
    case ESP_GAP_BLE_PASSKEY_REQ_EVT:                           /* passkey request event */
        ESP_LOGI(TAG, "ESP_GAP_BLE_PASSKEY_REQ_EVT");
        /* Call the following function to input the passkey which is displayed on the remote device */
        //esp_ble_passkey_reply(heart_rate_profile_tab[HEART_PROFILE_APP_IDX].remote_bda, true, 0x00);
        break;
    case ESP_GAP_BLE_OOB_REQ_EVT: {
        ESP_LOGI(TAG, "ESP_GAP_BLE_OOB_REQ_EVT");
        uint8_t tk[16] = {1}; //If you paired with OOB, both devices need to use the same tk
        esp_ble_oob_req_reply(param->ble_security.ble_req.bd_addr, tk, sizeof(tk));
        break;
    }
    case ESP_GAP_BLE_LOCAL_IR_EVT:                               /* BLE local IR event */
        ESP_LOGI(TAG, "ESP_GAP_BLE_LOCAL_IR_EVT");
        break;
    case ESP_GAP_BLE_LOCAL_ER_EVT:                               /* BLE local ER event */
        ESP_LOGI(TAG, "ESP_GAP_BLE_LOCAL_ER_EVT");
        break;
    case ESP_GAP_BLE_NC_REQ_EVT:
        /* The app will receive this evt when the IO has DisplayYesNO capability and the peer device IO also has DisplayYesNo capability.
        show the passkey number to the user to confirm it with the number displayed by peer device. */
        esp_ble_confirm_reply(param->ble_security.ble_req.bd_addr, true);
        ESP_LOGI(TAG, "ESP_GAP_BLE_NC_REQ_EVT, the passkey Notify number:%d", param->ble_security.key_notif.passkey);
        break;
    case ESP_GAP_BLE_SEC_REQ_EVT:
        /* send the positive(true) security response to the peer device to accept the security request.
        If not accept the security request, should send the security response with negative(false) accept value*/
        esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
        break;
    case ESP_GAP_BLE_PASSKEY_NOTIF_EVT:  ///the app will receive this evt when the IO  has Output capability and the peer device IO has Input capability.
        ///show the passkey number to the user to input it in the peer device.
        ESP_LOGI(TAG, "The passkey Notify number:%06d", param->ble_security.key_notif.passkey);
        break;
    case ESP_GAP_BLE_KEY_EVT:
        //shows the ble key info share with peer device to the user.
        ESP_LOGI(TAG, "key type = %s", esp_key_type_to_str(param->ble_security.ble_key.key_type));
        break;
    case ESP_GAP_BLE_AUTH_CMPL_EVT: {
        esp_bd_addr_t bd_addr;
        memcpy(bd_addr, param->ble_security.auth_cmpl.bd_addr, sizeof(esp_bd_addr_t));
       
        ESP_LOGI(TAG, "remote BD_ADDR: %08x%04x",\
                (bd_addr[0] << 24) + (bd_addr[1] << 16) + (bd_addr[2] << 8) + bd_addr[3],
                (bd_addr[4] << 8) + bd_addr[5]);
    
        ESP_LOGI(TAG, "address type = %d", param->ble_security.auth_cmpl.addr_type);
        ESP_LOGI(TAG, "pair status = %s",param->ble_security.auth_cmpl.success ? "success" : "fail");
        if(!param->ble_security.auth_cmpl.success) {
            ESP_LOGI(TAG, "fail reason = 0x%x",param->ble_security.auth_cmpl.fail_reason);
        } else {
            ESP_LOGI(TAG, "auth mode = %s",esp_auth_req_to_str(param->ble_security.auth_cmpl.auth_mode));
        }
        show_bonded_devices();
        break;
    }
    case ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT: {
        ESP_LOGD(TAG, "ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT status = %d", param->remove_bond_dev_cmpl.status);
        ESP_LOGI(TAG, "ESP_GAP_BLE_REMOVE_BOND_DEV");
        ESP_LOGI(TAG, "-----ESP_GAP_BLE_REMOVE_BOND_DEV----");
        esp_log_buffer_hex(TAG, (void *)param->remove_bond_dev_cmpl.bd_addr, sizeof(esp_bd_addr_t));
        ESP_LOGI(TAG, "------------------------------------");
        break;
    }
    case ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT:
        if (param->local_privacy_cmpl.status != ESP_BT_STATUS_SUCCESS){
            ESP_LOGE(TAG, "config local privacy failed, error status = %x", param->local_privacy_cmpl.status);
            break;
        }

        /*******************************MAC Address*******************************/
        
        ESP_ERROR_CHECK(esp_efuse_mac_get_default(device_data.mac_address));
        ESP_LOGI(TAG,"Mac address");

        for(int mac_size=0;mac_size<sizeof(device_data.mac_address);mac_size++){
            ESP_LOGI(TAG,"%x:",(unsigned int )device_data.mac_address[mac_size]);
        }


        /****************************prepare Manufacturer data for ADV_PDU***********************************/
        int data_index=0;

                    /*company id*/
        custom_manufacturer_data[data_index]=device_data.company_id[0];
        data_index++;
        custom_manufacturer_data[data_index]=device_data.company_id[1];
        data_index++;

                   /*MAC Address*/
        for(int manuf_data=0;manuf_data<sizeof(custom_manufacturer_data);manuf_data++,data_index++){
            custom_manufacturer_data[data_index]=device_data.mac_address[manuf_data];
        }

                    /*battery percentage*/
        custom_manufacturer_data[data_index]=convert_dec_to_hex(device_data.battery_percentage);


        


        esp_err_t ret = esp_ble_gap_config_adv_data(&esp32_ble_adv_config);
        if (ret){
            ESP_LOGE(TAG, "config adv data failed, error code = %x", ret);
        }else{
            adv_config_done |= ADV_CONFIG_FLAG;
        }

        ret = esp_ble_gap_config_adv_data(&esp32_ble_scan_rsp_config);
        if (ret){
            ESP_LOGE(TAG, "config adv data failed, error code = %x", ret);
        }else{
            adv_config_done |= SCAN_RSP_CONFIG_FLAG;
        }

        break;
    default:
        break;
    }
}


void prepare_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param)
{
    ESP_LOGI(TAG, "prepare write, handle = %d, value len = %d", param->write.handle, param->write.len);
    esp_gatt_status_t status = ESP_GATT_OK;
    if (prepare_write_env->prepare_buf == NULL) {
        prepare_write_env->prepare_buf = (uint8_t *)malloc(PREPARE_BUF_MAX_SIZE * sizeof(uint8_t));
        prepare_write_env->prepare_len = 0;
        if (prepare_write_env->prepare_buf == NULL) {
            ESP_LOGE(TAG, "%s, Gatt_server prep no mem", __func__);
            status = ESP_GATT_NO_RESOURCES;
        }
    } else {
        if(param->write.offset > PREPARE_BUF_MAX_SIZE) {
            status = ESP_GATT_INVALID_OFFSET;
        } else if ((param->write.offset + param->write.len) > PREPARE_BUF_MAX_SIZE) {
            status = ESP_GATT_INVALID_ATTR_LEN;
        }
    }
    /*send response when param->write.need_rsp is true */
    if (param->write.need_rsp){
        esp_gatt_rsp_t *gatt_rsp = (esp_gatt_rsp_t *)malloc(sizeof(esp_gatt_rsp_t));
        if (gatt_rsp != NULL){
            gatt_rsp->attr_value.len = param->write.len;
            gatt_rsp->attr_value.handle = param->write.handle;
            gatt_rsp->attr_value.offset = param->write.offset;
            gatt_rsp->attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
            memcpy(gatt_rsp->attr_value.value, param->write.value, param->write.len);
            esp_err_t response_err = esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, status, gatt_rsp);
            if (response_err != ESP_OK){
               ESP_LOGE(TAG, "Send response error");
            }
            free(gatt_rsp);
        }else{
            ESP_LOGE(TAG, "%s, malloc failed", __func__);
        }
    }
    if (status != ESP_GATT_OK){
        return;
    }
    memcpy(prepare_write_env->prepare_buf + param->write.offset,
           param->write.value,
           param->write.len);
    prepare_write_env->prepare_len += param->write.len;

}



/**
 * @brief GATT profile event handler(gets called for every individual profiles.....)
 * @param event : Event type
 * @param gatts_if : GATT server access interface, normally
 *                   different gatts_if correspond to different profile
 * @param param : Point to callback parameter, currently is union type
 */

static void gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                        esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    ESP_LOGV(TAG, "event = %x\n",event);
    switch (event) {
        case ESP_GATTS_REG_EVT:
            esp_ble_gap_set_device_name(DEVICE_NAME);
            //generate a resolvable random address
            esp_ble_gap_config_local_privacy(true);
            esp_ble_gatts_create_attr_tab(ble_gatt_db, gatts_if,IDX_NB, SVC_INST_ID);
            break;
        case ESP_GATTS_READ_EVT:
            ESP_LOGI(TAG,"Read Event");
            break;
        case ESP_GATTS_WRITE_EVT:
            if (!param->write.is_prep){

                char notify_data[3][10];
                char* axes_name[] = {"x-axes","y-axes","z-axes"};
                        

                ESP_LOGI(TAG,"attribute handle:%d, len:%d",param->write.handle,param->write.len);

                if(param->write.handle == ble_peripheral_nrf[IDX_CHAR_CFG_A] && param->write.len==2){
                    esp_log_buffer_hex(TAG,param->write.value,2);
                    cccd_flag[0] = param->write.value[0] | (param->write.value[1] << 8) ;
                    if(cccd_flag[0] == 0x0001){
                        ESP_LOGI(TAG,"notify enabled, handle:%d",ble_peripheral_nrf[IDX_CHAR_CFG_A]);

                        float x_axes_value = get_x_axes_data();
                        sprintf(notify_data[0],"%f",x_axes_value);
                        

                        conn_id = param->write.conn_id;
                        
                        esp_err_t ack = esp_ble_gatts_send_indicate(gatts_if,conn_id,ble_peripheral_nrf[IDX_CHAR_VAL_A],sizeof(notify_data[0]),(uint8_t*)notify_data[0],false);
                        if(ack==0){
                           ESP_LOGI(TAG,"sent data:%s for %s",notify_data[0],axes_name[0]);
                           ESP_LOGI(TAG,"conn_id:%d",conn_id);
                        }
                   
                    }
                    else if(cccd_flag[0] == 0x0000){
                        ESP_LOGI(TAG,"notify disabled, handle:%d",ble_peripheral_nrf[IDX_CHAR_CFG_A]);
                    }
                    else{
                        ESP_LOGI(TAG,"unknown descriptor, handle:%d",ble_peripheral_nrf[IDX_CHAR_CFG_A]);
                    } 
                }


                else if(param->write.handle == ble_peripheral_nrf[IDX_CHAR_CFG_B] && param->write.len==2){
                    esp_log_buffer_hex(TAG,param->write.value,2);
                    cccd_flag[1] = param->write.value[0] | (param->write.value[1] << 8) ;
                    if(cccd_flag[1] == 0x0001){
                        ESP_LOGI(TAG,"notify enabled, handle:%d",ble_peripheral_nrf[IDX_CHAR_CFG_B]);

                        float y_axes_value = get_y_axes_data();
                        sprintf(notify_data[1],"%f",y_axes_value);
                        

                        conn_id = param->write.conn_id;
                        
                        esp_err_t ack = esp_ble_gatts_send_indicate(gatts_if,conn_id,ble_peripheral_nrf[IDX_CHAR_VAL_B],sizeof(notify_data[1]),(uint8_t*)notify_data[1],false);
                        if(ack==0){
                           ESP_LOGI(TAG,"sent data:%s for %s",notify_data[1],axes_name[1]);
                           ESP_LOGI(TAG,"conn_id:%d",conn_id);
                        }
                        
                    }
                    else if(cccd_flag[1] == 0x0000){
                        ESP_LOGI(TAG,"notify disabled, handle:%d",ble_peripheral_nrf[IDX_CHAR_CFG_A]);
                    }
                    else{
                        ESP_LOGI(TAG,"unknown descriptor, handle:%d",ble_peripheral_nrf[IDX_CHAR_CFG_A]);
                    } 

                }

                else if(param->write.handle == ble_peripheral_nrf[IDX_CHAR_CFG_C] && param->write.len==2){
                    esp_log_buffer_hex(TAG,param->write.value,2);
                    cccd_flag[2] = param->write.value[0] | (param->write.value[1] << 8) ;
                    if(cccd_flag[2] == 0x0001){
                        ESP_LOGI(TAG,"notify enabled, handle:%d",ble_peripheral_nrf[IDX_CHAR_CFG_C]);

                        float z_axes_value = get_z_axes_data();
                        sprintf(notify_data[2],"%f",z_axes_value);
                        

                        conn_id = param->write.conn_id;
                        
                        esp_err_t ack = esp_ble_gatts_send_indicate(gatts_if,conn_id,ble_peripheral_nrf[IDX_CHAR_VAL_C],sizeof(notify_data[2]),(uint8_t*)notify_data[2],false);
                        if(ack==0){
                           ESP_LOGI(TAG,"sent data:%s for %s",notify_data[2],axes_name[2]);
                           ESP_LOGI(TAG,"conn_id:%d",conn_id);
                        }
                        
                    }
                    else if(cccd_flag[2] == 0x0000){
                        ESP_LOGI(TAG,"notify disabled, handle:%d",ble_peripheral_nrf[IDX_CHAR_CFG_A]);
                    }
                    else{
                        ESP_LOGI(TAG,"unknown descriptor, handle:%d",ble_peripheral_nrf[IDX_CHAR_CFG_A]);
                    } 

                }
                
            }
            else{
                /* handle prepare write */
                prepare_write_event_env(gatts_if, &prepare_write_env, param);
            }

            break;
        case ESP_GATTS_EXEC_WRITE_EVT:
            break;
        case ESP_GATTS_MTU_EVT:
        /*
         *************************************************************************************************
         *********event triggered after client request for updating MTU(set max to 350 for now)***********
         *************************************************************************************************
        */
            ESP_LOGI(TAG,"for connection id:%u",param->mtu.conn_id);
            ESP_LOGI(TAG,"requested MTU size from client:%u",param->mtu.mtu);
            break;
        case ESP_GATTS_CONF_EVT:
            break;
        case ESP_GATTS_UNREG_EVT:
            break;
        case ESP_GATTS_DELETE_EVT:
            break;
        case ESP_GATTS_START_EVT:
            break;
        case ESP_GATTS_STOP_EVT:
            break;

        case ESP_GATTS_CONNECT_EVT:
            ESP_LOGI(TAG, "ESP_GATTS_CONNECT_EVT");
             //start security connect with peer device when receive the connect event sent by the master 
            // esp_ble_set_encryption(param->connect.remote_bda, ESP_BLE_SEC_ENCRYPT_MITM);
            break;
        case ESP_GATTS_DISCONNECT_EVT:
            ESP_LOGI(TAG, "ESP_GATTS_DISCONNECT_EVT, disconnect reason 0x%x", param->disconnect.reason);
            /* start advertising again when missing the connect */
            esp_ble_gap_start_advertising(&esp32_ble_adv_params);
            break;
        case ESP_GATTS_OPEN_EVT:
            break;
        case ESP_GATTS_CANCEL_OPEN_EVT:
            break;
        case ESP_GATTS_CLOSE_EVT:
            break;
        case ESP_GATTS_LISTEN_EVT:
            break;
        case ESP_GATTS_CONGEST_EVT:
            break;
        case ESP_GATTS_CREAT_ATTR_TAB_EVT: {
            ESP_LOGI(TAG, "The number handle = %x",param->add_attr_tab.num_handle);
            if (param->create.status == ESP_GATT_OK){
                if(param->add_attr_tab.num_handle == IDX_NB) {
                    memcpy(ble_peripheral_nrf, param->add_attr_tab.handles,
                    sizeof(ble_peripheral_nrf));
                    esp_ble_gatts_start_service(ble_peripheral_nrf[IDX_SVC]);

                    for(int idx=0;idx<IDX_NB;idx++){
                        ESP_LOGI(TAG,"attribute table:%d",ble_peripheral_nrf[idx]);
                    }
                    
                }else{
                    ESP_LOGE(TAG, "Create attribute table abnormally, num_handle (%d) doesn't equal to IDX_NB(%d)",
                         param->add_attr_tab.num_handle, IDX_NB);
                }
            }else{
                ESP_LOGE(TAG, " Create attribute table failed, error code = %x", param->create.status);
            }
        break;
    }

        default:
           break;
    }
}


/**
 * @brief GATTS event handler for all application profiles
 * @param event : Event type
 * @param gatts_if : GATT server access interface, normally
 *                   different gatts_if correspond to different profile
 * @param param : Point to callback parameter, currently is union type
 */

static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                                esp_ble_gatts_cb_param_t *param)
{
    /* If event is register event, store the gatts_if for each profile */
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            profile_tab[PROFILE_A_APP_ID].gatts_if = gatts_if;
        } else {
            ESP_LOGI(TAG, "Reg app failed, app_id %04x, status %d\n",
                    param->reg.app_id,
                    param->reg.status);
            return;
        }
    }

    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            if (gatts_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                    gatts_if == profile_tab[idx].gatts_if) {
                if (profile_tab[idx].gatts_cb) {
                    profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);

}


/*
*******************************************************************************************************
@brief task for LIS2DW12 initialization, value extraction and handling BLE notification from server end
@note  sends accelerometer value at every 1s interval
*******************************************************************************************************
*/
void LIS2DW12_Data(void *arg){

    /*initialize LIS2DW12*/
    lis2dw12_init(TAG);

    char notify_data[3][10];
    char* axes_name[] = {"x-axes","y-axes","z-axes"};
    esp_err_t ack;

    while(1){

        /*X-axis notification handle*/
        if(cccd_flag[0] == 0x0001){
            ESP_LOGI(TAG,"notify enabled");

            /*get axes value(mg)*/
            float x_axes_value = get_x_axes_data();
            sprintf(notify_data[0],"%f",x_axes_value);
    
            ESP_LOGI(TAG,"x(mg):%f",x_axes_value);

            

            ack = esp_ble_gatts_send_indicate(profile_tab[PROFILE_A_APP_ID].gatts_if,conn_id,ble_peripheral_nrf[IDX_CHAR_VAL_A],sizeof(notify_data[0]),(uint8_t*)notify_data[0],false);
            if(ack==0){
               ESP_LOGI(TAG,"sent data:%s for %s",notify_data[0],axes_name[0]);        
               ESP_LOGI(TAG,"conn_id:%d",conn_id);
            }
        }
        
        /*Y-axis notification handle*/
        if(cccd_flag[1] == 0x0001){
         
            /*get axes value(mg)*/
            float y_axes_value = get_y_axes_data();
            sprintf(notify_data[1],"%f",y_axes_value);
    
            ESP_LOGI(TAG,"y(mg):%f",y_axes_value);   

            ack = esp_ble_gatts_send_indicate(profile_tab[PROFILE_A_APP_ID].gatts_if,conn_id,ble_peripheral_nrf[IDX_CHAR_VAL_B],sizeof(notify_data[1]),(uint8_t*)notify_data[1],false);
            if(ack==0){
               ESP_LOGI(TAG,"sent data:%s for %s",notify_data[1],axes_name[1]);        
               ESP_LOGI(TAG,"conn_id:%d",conn_id);
            }

        }

        /*Z-axis notification handle*/
        if(cccd_flag[2] == 0x0001){
         
            /*get axes value(mg)*/
            float z_axes_value = get_z_axes_data();
            sprintf(notify_data[2],"%f",z_axes_value);
    
            ESP_LOGI(TAG,"z(mg):%f",z_axes_value);   

            ack = esp_ble_gatts_send_indicate(profile_tab[PROFILE_A_APP_ID].gatts_if,conn_id,ble_peripheral_nrf[IDX_CHAR_VAL_C],sizeof(notify_data[2]),(uint8_t*)notify_data[2],false);
            if(ack==0){
               ESP_LOGI(TAG,"sent data:%s for %s",notify_data[2],axes_name[2]);        
               ESP_LOGI(TAG,"conn_id:%d",conn_id);
            }

        }


        vTaskDelay(1000/portTICK_PERIOD_MS);

    }
}


/*
**********************************************************************************************
@brief main application code
**********************************************************************************************
*/
void app_main(void)
{

esp_err_t ret;

/*
***********************************************************************************************
**************************BLE initialization and security setting******************************
***********************************************************************************************
*/

    // Initialize NVS.
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "%s init controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    /* Bluetooth stack should be up and running after below commands*/
    ESP_LOGI(TAG, "%s init bluetooth", __func__);
    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    /*The functions gatts_event_handler() and gap_event_handler() handle all the events that are 
    pushed to the application from the BLE stack.*/
    ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret){
        ESP_LOGE(TAG, "gatts register error, error code = %x", ret);
        return;
    }
    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret){
        ESP_LOGE(TAG, "gap register error, error code = %x", ret);
        return;
    }

    // esp_ble_gap_set_device_name(DEVICE_NAME);
    esp_ble_gap_config_local_privacy(true);

    ret = esp_ble_gatts_app_register(PROFILE_A_APP_ID);
    if (ret){
        ESP_LOGE(TAG, "gatts app register error, error code = %x", ret);
        return;
    }

    /* set the security iocap & auth_req & key size & init key response key parameters to the stack*/
    esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_MITM_BOND;     //bonding with peer device after authentication
    esp_ble_io_cap_t iocap = ESP_IO_CAP_NONE;           //set the IO capability to No output No input
    uint8_t key_size = 16;      //the key size should be 7~16 bytes
    uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    //set static passkey
    uint32_t passkey = 123456;
    uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;
    uint8_t oob_support = ESP_BLE_OOB_DISABLE;
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &auth_option, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_OOB_SUPPORT, &oob_support, sizeof(uint8_t));
    /* If your BLE device acts as a Slave, the init_key means you hope which types of key of the master should distribute to you,
    and the response key means which key you can distribute to the master;
    If your BLE device acts as a master, the response key means you hope which types of key of the slave should distribute to you,
    and the init key means which key you can distribute to the slave. */
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));

     // Just show how to clear all the bonded devices
     // * Delay 30s, clear all the bonded devices
     // *
     // vTaskDelay(30000 / portTICK_PERIOD_MS);
     // remove_all_bonded_devices();



    /*create task for accelerometer*/
    xTaskCreate(&LIS2DW12_Data,"Accelerometer value",2000,NULL,0,NULL);

    
}
