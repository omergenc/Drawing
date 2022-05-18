/**
  ******************************************************************************
  * @file    app_ver.h
  * @author  Litum Embedded Team
  * @brief   LT020101 app memory header file.
  *          This file contains all the functions prototypes for the app memory
  *          module driver.
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_VER_H
#define __APP_VER_H

/* Includes ------------------------------------------------------------------*/
// must include all libs here
#include "bsp_stm32Hal.h"

#include "adam4060.h"
#include "businessLogic.h"
#include "checksum.h"
#include "crc16.h"
#include "devInfo.h"
#include "disco.h"
#include "dw1000.h"
#include "espWroom02.h"
#include "frame.h"
#include "spiFlash.h"
#include "spiFlash_s25fl.h"
#include "litumRand.h"
#include "litumType.h"
#include "msgingService.h"
#include "msgQue.h"
#include "network.h"
#include "networking.h"
#include "nmea.h"
#include "nrf52840.h"
#include "serverFrame.h"
#include "setGet.h"
#include "stMachine.h"
#include "swUpdate.h"
#include "twrLitum.h"
#include "pattern.h"
#include "rtls.h"
#include "cw.h"
#include "vehicleUser.h"
#include "w5500.h"
#include "w5500_tcpClient.h"
#include "xModem.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define APP_BSP_VERSION_BRANCH                  LITUMTYPE_VERSION_MASTER
#define APP_BSP_VERSION_RELEASE                 0x0001
#define APP_BSP_VERSION_PATCH                   0x04

#define APP_ADAM4060_VERSION_BRANCH             LITUMTYPE_VERSION_MASTER
#define APP_ADAM4060_VERSION_RELEASE            0x0001
#define APP_ADAM4060_VERSION_PATCH              0x02

#define APP_BUSINESSLOGIC_VERSION_BRANCH        LITUMTYPE_VERSION_MASTER
#define APP_BUSINESSLOGIC_VERSION_RELEASE       0x0001
#define APP_BUSINESSLOGIC_VERSION_PATCH         0x0E

#define APP_CHECKSUM_VERSION_BRANCH             LITUMTYPE_VERSION_MASTER
#define APP_CHECKSUM_VERSION_RELEASE            0x0001
#define APP_CHECKSUM_VERSION_PATCH              0x00

#define APP_CRC16_VERSION_BRANCH                LITUMTYPE_VERSION_MASTER
#define APP_CRC16_VERSION_RELEASE               0x0001
#define APP_CRC16_VERSION_PATCH                 0x00

#define APP_DEVINFO_VERSION_BRANCH              LITUMTYPE_VERSION_MASTER
#define APP_DEVINFO_VERSION_RELEASE             0x0001
#define APP_DEVINFO_VERSION_PATCH               0x03

#define APP_DISCO_VERSION_BRANCH                LITUMTYPE_VERSION_MASTER
#define APP_DISCO_VERSION_RELEASE               0x0001
#define APP_DISCO_VERSION_PATCH                 0x0B

#define APP_DW1000_VERSION_BRANCH               LITUMTYPE_VERSION_MASTER
#define APP_DW1000_VERSION_RELEASE              0x0001
#define APP_DW1000_VERSION_PATCH                0x09

#define APP_ESPWROOM02_VERSION_BRANCH           LITUMTYPE_VERSION_MASTER
#define APP_ESPWROOM02_VERSION_RELEASE          0x0001
#define APP_ESPWROOM02_VERSION_PATCH            0x03

#define APP_SPIFLASH_VERSION_BRANCH             LITUMTYPE_VERSION_MASTER
#define APP_SPIFLASH_VERSION_RELEASE            0x0001
#define APP_SPIFLASH_VERSION_PATCH              0x02

#define APP_FRAME_VERSION_BRANCH                LITUMTYPE_VERSION_MASTER
#define APP_FRAME_VERSION_RELEASE               0x0001
#define APP_FRAME_VERSION_PATCH                 0x09

#define APP_HTTPSERVER_VERSION_BRANCH           LITUMTYPE_VERSION_MASTER
#define APP_HTTPSERVER_VERSION_RELEASE          0x0001
#define APP_HTTPSERVER_VERSION_PATCH            0x01

#define APP_LITUMRAND_VERSION_BRANCH            LITUMTYPE_VERSION_MASTER
#define APP_LITUMRAND_VERSION_RELEASE           0x0001
#define APP_LITUMRAND_VERSION_PATCH             0x00

#define APP_LITUMTYPE_VERSION_BRANCH            LITUMTYPE_VERSION_MASTER
#define APP_LITUMTYPE_VERSION_RELEASE           0x0001
#define APP_LITUMTYPE_VERSION_PATCH             0x06

#define APP_MSGINGSERVICE_VERSION_BRANCH        LITUMTYPE_VERSION_MASTER
#define APP_MSGINGSERVICE_VERSION_RELEASE       0x0001
#define APP_MSGINGSERVICE_VERSION_PATCH         0x04

#define APP_MSGQUE_VERSION_BRANCH               LITUMTYPE_VERSION_MASTER
#define APP_MSGQUE_VERSION_RELEASE              0x0001
#define APP_MSGQUE_VERSION_PATCH                0x02

#define APP_NETWORK_VERSION_BRANCH              LITUMTYPE_VERSION_MASTER
#define APP_NETWORK_VERSION_RELEASE             0x0001
#define APP_NETWORK_VERSION_PATCH               0x13

#define APP_NETWORKING_VERSION_BRANCH           LITUMTYPE_VERSION_MASTER
#define APP_NETWORKING_VERSION_RELEASE          0x0001
#define APP_NETWORKING_VERSION_PATCH            0x37

#define APP_NMEA_VERSION_BRANCH                 LITUMTYPE_VERSION_MASTER
#define APP_NMEA_VERSION_RELEASE                0x0001
#define APP_NMEA_VERSION_PATCH                  0x02

#define APP_NRF52840_VERSION_BRANCH             LITUMTYPE_VERSION_MASTER
#define APP_NRF52840_VERSION_RELEASE            0x0001
#define APP_NRF52840_VERSION_PATCH              0x05

#define APP_SERVERFRAME_VERSION_BRANCH          LITUMTYPE_VERSION_MASTER
#define APP_SERVERFRAME_VERSION_RELEASE         0x0001
#define APP_SERVERFRAME_VERSION_PATCH           0x04

#define APP_SETGET_VERSION_BRANCH               LITUMTYPE_VERSION_MASTER
#define APP_SETGET_VERSION_RELEASE              0x0001
#define APP_SETGET_VERSION_PATCH                0x32

#define APP_STMACHINE_VERSION_BRANCH            LITUMTYPE_VERSION_MASTER
#define APP_STMACHINE_VERSION_RELEASE           0x0001
#define APP_STMACHINE_VERSION_PATCH             0x01

#define APP_SWUPDATE_VERSION_BRANCH             LITUMTYPE_VERSION_MASTER
#define APP_SWUPDATE_VERSION_RELEASE            0x0001
#define APP_SWUPDATE_VERSION_PATCH              0x10

#define APP_TWRLITUM_VERSION_BRANCH             LITUMTYPE_VERSION_MASTER
#define APP_TWRLITUM_VERSION_RELEASE            0x0001
#define APP_TWRLITUM_VERSION_PATCH              0x08

#define APP_W5500_VERSION_BRANCH                LITUMTYPE_VERSION_MASTER
#define APP_W5500_VERSION_RELEASE               0x0001
#define APP_W5500_VERSION_PATCH                 0x03

#define APP_XMODEM_VERSION_BRANCH               LITUMTYPE_VERSION_MASTER
#define APP_XMODEM_VERSION_RELEASE              0x0001
#define APP_XMODEM_VERSION_PATCH                0x01

#define APP_PATTERN_VERSION_BRANCH              LITUMTYPE_VERSION_MASTER
#define APP_PATTERN_VERSION_RELEASE             0x0001
#define APP_PATTERN_VERSION_PATCH               0x04

#define APP_RTLS_VERSION_BRANCH                 LITUMTYPE_VERSION_MASTER
#define APP_RTLS_VERSION_RELEASE                0x0001
#define APP_RTLS_VERSION_PATCH                  0x05

#define APP_CW_VERSION_BRANCH                   LITUMTYPE_VERSION_MASTER
#define APP_CW_VERSION_RELEASE                  0x0001
#define APP_CW_VERSION_PATCH                    0x18

#define APP_VEHICLEUSER_VERSION_BRANCH          LITUMTYPE_VERSION_MASTER
#define APP_VEHICLEUSER_VERSION_RELEASE         0x0001
#define APP_VEHICLEUSER_VERSION_PATCH           0x06

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void App_Ver_Get_App_Version(U8 a_appVer[4]);
void App_Ver_Get_Boot_Version(U8 a_bootVer[4]);

#endif /* __APP_VER_H */

/*********************** (C) COPYRIGHT Litum Embedded Team *****END OF FILE****/
