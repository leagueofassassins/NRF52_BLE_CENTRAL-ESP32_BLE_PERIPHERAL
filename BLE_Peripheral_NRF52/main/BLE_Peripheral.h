/* @file  BLE_Peripheral.h
   @brief header file for BLE peripheral with GATT server  
   @author bheesma-10
*/
#ifndef _main_h_
#define _main_h_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//Attributes State Machine for server attribute table
enum
{
    IDX_SVC,

    IDX_CHAR_A,
    IDX_CHAR_VAL_A,
    IDX_CHAR_CFG_A,

    IDX_CHAR_B,
    IDX_CHAR_VAL_B,
    IDX_CHAR_CFG_B,

    IDX_CHAR_C,
    IDX_CHAR_VAL_C,
    IDX_CHAR_CFG_C,    

    IDX_NB,
};

#endif