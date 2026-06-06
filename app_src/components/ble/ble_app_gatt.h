
//BLE configuration generator for use with MV_LE_PROFILE_TOOL
//Copyright 2024 MVSilicon
// ble_app_gatt.h generated from input.gatt for MV_LE_PROFILE_TOOL
// att db format version 1

// binary attribute representation:
// - size in bytes (16), flags(16), handle (16), uuid (16/128), value(...)

// 

#ifndef __BLE_APP_GATT_H__
#define __BLE_APP_GATT_H__

#include "ble_app_func.h"
#include "ble_api.h"

#define DEFAULT_MTU_SIZE (250)  // 250
#define LE_VAL_MAX_LEN (DEFAULT_MTU_SIZE-3)

#define RD_P    (PROP(RD)  | SEC_LVL(RP, NOT_ENC))
/// default write without response perm
#define WC_P    (PROP(WC)  | SEC_LVL(WP, NOT_ENC))
/// default write perm
#define WR_P    (PROP(WR)  | SEC_LVL(WP, NOT_ENC))
/// default notify perm
#define NTF_P   (PROP(N)   | SEC_LVL(NIP, NOT_ENC))
/// ind perm
#define IND_P   (PROP(I)   | SEC_LVL(NIP, NOT_ENC))




#define UUID16_1800_SERVICE (0x1800)
#define ATT16_DB_1800_SIZE (sizeof(ATT16_DB_1800)/sizeof(ble_gatt_att16_desc_t))
static ble_gatt_att16_desc_t ATT16_DB_1800[] = {
    // ATT UUID
    //  | Permission			| EXT PERM | MAX ATT SIZE
    {GATT_DECL_PRIMARY_SERVICE, RD_P, 0},

    {GATT_DECL_CHARACTERISTIC, RD_P,0},
    {0x2a00, RD_P | WR_P, LE_VAL_MAX_LEN},

    {GATT_DECL_CHARACTERISTIC, RD_P,0},
    {0x2a01, RD_P, LE_VAL_MAX_LEN},

};

#define UUID16_1801_SERVICE (0x1801)
#define ATT16_DB_1801_SIZE (sizeof(ATT16_DB_1801)/sizeof(ble_gatt_att16_desc_t))
static ble_gatt_att16_desc_t ATT16_DB_1801[] = {
    // ATT UUID
    //  | Permission			| EXT PERM | MAX ATT SIZE
    {GATT_DECL_PRIMARY_SERVICE, RD_P, 0},

    {GATT_DECL_CHARACTERISTIC, RD_P,0},
    {0x2A2B, RD_P, LE_VAL_MAX_LEN},

};

#define UUID16_AB00_SERVICE (0xAB00)
#define ATT16_DB_AB00_SIZE (sizeof(ATT16_DB_AB00)/sizeof(ble_gatt_att16_desc_t))
static ble_gatt_att16_desc_t ATT16_DB_AB00[] = {
    // ATT UUID
    //  | Permission			| EXT PERM | MAX ATT SIZE
    {GATT_DECL_PRIMARY_SERVICE, RD_P, 0},

    {GATT_DECL_CHARACTERISTIC, RD_P,0},
    {0xAB01, RD_P | WR_P, LE_VAL_MAX_LEN},

    {GATT_DECL_CHARACTERISTIC, RD_P,0},
    {0xAB02, NTF_P, LE_VAL_MAX_LEN},
    {GATT_DESC_CLIENT_CHAR_CFG, RD_P | WR_P, OPT(NO_OFFSET)},//client characteristic configuration descriptor

    {GATT_DECL_CHARACTERISTIC, RD_P,0},
    {0xAB03, NTF_P, LE_VAL_MAX_LEN},
    {GATT_DESC_CLIENT_CHAR_CFG, RD_P | WR_P, OPT(NO_OFFSET)},//client characteristic configuration descriptor

};

typedef enum
{
    UUID_2A00_DEVICE_NAME_HANDLE = 0x3,
    UUID_2A01_APPEARANCE_HANDLE = 0x5,
    UUID_2A2B_HANDLE = 0x8,
    UUID_AB01_HANDLE = 0xb,
    UUID_AB02_HANDLE = 0xd,
    UUID_AB02_CFG_HANDLE = 0xe,
    UUID_AB03_HANDLE = 0x10,
    UUID_AB03_CFG_HANDLE = 0x11,
} profile_att_handle;


ble_profile_add_t mv_profile[] = {

{UUID_TYPE_16, ATT16_DB_1800_SIZE, UUID16_1800_SERVICE, (ble_gatt_att16_desc_t*)ATT16_DB_1800, NULL,NULL },
{UUID_TYPE_16, ATT16_DB_1801_SIZE, UUID16_1801_SERVICE, (ble_gatt_att16_desc_t*)ATT16_DB_1801, NULL,NULL },
{UUID_TYPE_16, ATT16_DB_AB00_SIZE, UUID16_AB00_SERVICE, (ble_gatt_att16_desc_t*)ATT16_DB_AB00, NULL,NULL },

};

#define MV_profile_db_size (sizeof(mv_profile)/sizeof(ble_profile_add_t))


#endif/*__BLE_APP_GATT_H__*/