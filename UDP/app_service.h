/**
  ******************************************************************************
  * @file    app_service.h
  * @author  Litum Embedded Team
  * @brief   app_service header file.
  *          This file contains all the functions prototypes for the app_dw1000
  *          module driver.
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_SERVICE_H
#define __APP_SERVICE_H

/* Includes ------------------------------------------------------------------*/
#include "litumType.h"
#include "nmea.h"

/* Exported defines ----------------------------------------------------------*/
#define APP_SERVICE_RX_MSG_SIZE         NMEA_MAX_FRMLEN

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
BOOL App_Service_InsertMsg(U8 *p_msg, U16 msgLen);
void App_Service_Handler(void);
__weak BOOL App_Service_Send_Packet_Callback(U8 *p_txBuff, U16 len);

#endif /*__APP_SERVICE_H*/

/*********************** (C) COPYRIGHT Litum Embedded Team *****END OF FILE****/
