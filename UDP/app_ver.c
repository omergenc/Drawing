/**
  ******************************************************************************
  * @file    app_ver.c
  * @author  Litum Embedded Team
  * @brief   LT020101 Version info
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================


  @endverbatim
  ******************************************************************************
  * @attention
  *
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp_flash.h"

#include "libVers.h"

#include "app_ver.h"

/* Private define ------------------------------------------------------------*/
#define APP_VERSION_APPLICATION_BITS            e_LITUMTYPE_APPLICATION_TYPE_RTLS
#define APP_VERSION_BRANCH_BITS                 LITUMTYPE_VERSION_MASTER
#define APP_VERSION_HARDWARE_TYPE               e_LITUMTYPE_HARDWARE_TYPE_LT_02_01
#define APP_VERSION_RELEASE                     0x04
#define APP_VERSION_PATCH                       0x01

#define APP_VERSION_DEBUG_BYTE                  'D'
#define APP_VERSION_NON_DEBUG_BYTE              'R'

/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef struct
{
    U8                                          branch;
    U8                                          releaseH;
    U8                                          releaseL;
    U8                                          patch;
    U8                                          debug;
} App_Version_t;

/* Private variables ---------------------------------------------------------*/
#pragma location = BSP_FLASH_APP_VERSION_ADDR
__root const App_Version_t AppVer =
{
    .branch                                     = ((APP_VERSION_APPLICATION_BITS << 3) | APP_VERSION_BRANCH_BITS),
    .releaseH                                   = APP_VERSION_HARDWARE_TYPE,
    .releaseL                                   = APP_VERSION_RELEASE,
    .patch                                      = APP_VERSION_PATCH,
#ifdef DEBUG
    .debug                                      = APP_VERSION_DEBUG_BYTE,
#else
    .debug                                      = APP_VERSION_NON_DEBUG_BYTE,
#endif
};

/* Private function prototypes -----------------------------------------------*/
/* Private consts ------------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

//! @brief This function provides minimum delay (in milliseconds) based
//! @note   In the default implementation , SysTick timer is the source of time base.
//! @param  None
//! @retval None
void App_Ver_Get_App_Version(U8 a_appVer[4])
{
    a_appVer[0] = AppVer.branch;
    a_appVer[1] = AppVer.releaseH;
    a_appVer[2] = AppVer.releaseL;
    a_appVer[3] = AppVer.patch;
}


//! @brief This function provides minimum delay (in milliseconds) based
//! @note   In the default implementation , SysTick timer is the source of time base.
//! @param  None
//! @retval None
void App_Ver_Get_Boot_Version(U8 a_bootVer[4])
{
    MEM_CPY(&a_bootVer[0], (U8 *)(BSP_FLASH_BOOT_VERSION_ADDR + 0), 1);
    MEM_CPY(&a_bootVer[1], (U8 *)(BSP_FLASH_BOOT_VERSION_ADDR + 1), 1);
    MEM_CPY(&a_bootVer[2], (U8 *)(BSP_FLASH_BOOT_VERSION_ADDR + 2), 1);
    MEM_CPY(&a_bootVer[3], (U8 *)(BSP_FLASH_BOOT_VERSION_ADDR + 3), 1);
}

/*********************** (C) COPYRIGHT Litum Embedded Team *****END OF FILE****/
