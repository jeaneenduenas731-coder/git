
#include "type.h"
#include <string.h>
#include "ble_api.h"
#include "ble_app_func.h"
#include "bt_manager.h"
#include "bt_config.h"
#include "ble_app_gatt.h"
extern BT_CONFIGURATION_PARAMS		*btStackConfigParams;

#include "debug.h"
#if (BLE_SUPPORT)

extern void user_set_ble_bd_addr(uint8_t* local_addr);

#define BLE_DFLT_DEVICE_NAME			(sys_parameter.ble_LocalDeviceName)	//BLE名称
#define BLE_DFLT_DEVICE_NAME_LEN		(strlen(BLE_DFLT_DEVICE_NAME))



uint8_t ble_app_adv_data[30] = {
	//length + type + data
    // Flags general discoverable, BR/EDR not supported
    2, 0x01, 0x06,
    // Name
    9, 0x09, 'B','P','1','5','_','B','L','E',
};

const uint8_t ble_app_rsp_adv_data[30] = {
    0x03, 0xff, 0xff,0xff,
};

static uint8_t adv_len = sizeof(ble_app_adv_data);
static uint8_t rsp_adv_len = sizeof(ble_app_rsp_adv_data);

/***********************************************************************************
 * BLE初始化参数配置
 **********************************************************************************/
uint8_t LeInitConfigParams(void)
{

    LeAppRegCB(AppEventCallBack); // 注册应用层事件回调函数
    user_set_ble_bd_addr(btStackConfigParams->ble_LocalDeviceAddr);
    // BLE广播数据内容填充
    le_user_config.ble_device_name_len = BLE_DFLT_DEVICE_NAME_LEN;
    memcpy(le_user_config.ble_device_name,BLE_DFLT_DEVICE_NAME,le_user_config.ble_device_name_len);
    ble_app_adv_data[3] = BLE_DFLT_DEVICE_NAME_LEN+1;
    memcpy(&ble_app_adv_data[5],le_user_config.ble_device_name,le_user_config.ble_device_name_len);

    le_user_config.adv_data.adv_data = (uint8_t *)ble_app_adv_data;
    le_user_config.adv_data.adv_len = adv_len;

    // 广播回复数据需要时填写,不需要时填NULL
    le_user_config.rsp_data.adv_rsp_data = (uint8_t *)ble_app_rsp_adv_data;
    le_user_config.rsp_data.adv_rsp_len = rsp_adv_len;

    //设置广播间隔及广播通道
    le_user_config.adv_interval_param.adv_intv_max = 0x0200;
    le_user_config.adv_interval_param.adv_intv_min = 0x0100;
    le_user_config.adv_interval_param.ch_map = 0x07; //37,38,39 ch map

    //初始化报文
    le_user_config.mv_profile_add = mv_profile;
    le_user_config.mv_profile_data_size = MV_profile_db_size;

    le_user_config.att_default_mtu = DEFAULT_MTU_SIZE;
    return 0;
}


/**********************************************************************************
 *BLE CLIENT CONFIG
 **********************************************************************************/
#if (BLE_DUAL_ROLE == ENABLE)
static void gatt_client_discover_cmp_cb(uint8_t conidx, uint8_t user_lid, uint16_t metainfo, uint16_t status)

{
    APP_DBG("Discover complete, status = %x,metainfo: %x\n", status,metainfo);
}

static void gatt_client_read_cmp_cb(uint8_t conidx, uint8_t user_lid, uint16_t metainfo, uint16_t status, uint16_t hdl, uint16_t offset,const char *p_info)
{
    // Inform application about read name
    APP_DBG("Read complete, hdl = %x, name = %s\n", hdl, p_info);
}

static void gatt_client_write_cmp_cb(uint8_t conidx, uint8_t user_lid, uint16_t metainfo, uint16_t status)
{
    APP_DBG("Write Name complete, metainfo = %x\n", metainfo);
}

static void gatt_client_info_recv_cb(uint8_t conidx,  uint8_t event_type, uint16_t hdl, uint8_t info_len, const uint8_t *p_info)
{
	  APP_DBG("gatt_client_info_recv_cb, hdl = %x\n", hdl);
}
gatt_client_appli_itf_t le_appli_itf_t = {
    .cb_discover_cmp = gatt_client_discover_cmp_cb,
    .cb_read_cmp = gatt_client_read_cmp_cb,
    .cb_write_cmp = gatt_client_write_cmp_cb,
    .cb_info_recv = gatt_client_info_recv_cb,
};

uint16_t gatt_client_config(void)
{
    return gatt_client_init(&le_appli_itf_t);
}
#endif
/***********************************************************************************
 * BLE application initialize
 **********************************************************************************/
void BleAppInit(void)
{
	extern void rwble_enable_init(void);
    LeInitConfigParams(); //le parameters config
    rwble_enable_init();
}
#endif

