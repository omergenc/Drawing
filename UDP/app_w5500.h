/**
  ******************************************************************************
  * @file    app_dw1000.h
  * @author  Litum Embedded Team
  * @brief   LT020101 app_dw1000 header file.
  *          This file contains all the functions prototypes for the app_dw1000
  *          module driver.
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_W5500_H
#define __APP_W5500_H

/* Includes ------------------------------------------------------------------*/
#include "litumType.h"

/* Exported defines ----------------------------------------------------------*/
#define APP_W5500_DHCP_SOCKET_NUMBER                    0
#define APP_W5500_TCP_CLIENT_SOCKET_NUMBER              1
#define APP_W5500_SOCKET_NUMBER_UDP_SERVICE             2
#define APP_W5500_UDP_PORT                              8000

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
BOOL App_W5500_Init(void);
void App_W5500_Power_Control(BOOL turnOn);
BOOL App_W5500_Get_TcpClientStatus(void);
void App_W5500_Handler(void);

#endif /*__APP_W5500_H*/

/*********************** (C) COPYRIGHT Litum Embedded Team *****END OF FILE****/
