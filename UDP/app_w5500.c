/**
  ******************************************************************************
  * @file    app_w5500.c
  * @author  Litum Embedded Team
  * @brief   LT020101 app_w5500 C file.
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
#include "bsp_gpio.h"
#include "bsp_spi.h"
#include "bsp_swTimer.h"

#include "w5500.h"
#include "w5500_dhcp.h"
#include "w5500_socket.h"
#include "w5500_tcpClient.h"

#include "app.h"
#include "app_w5500.h"
#include "app_service.h"

/* Private define ------------------------------------------------------------*/
#define APP_W5500_MAX_DHCP_RETRY                5
#define APP_W5500_RECOVERY_CHECK_PERIOD_MS      5000

/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef struct
{
    U8          a_ip[4];    
    U8          a_data[1024];
    U16         port;
    U16         len;
} App_W5500_UdpRx_t;

typedef struct
{
    U8          a_ip[4];
    U8          a_data[1024];
    U16         len;   
} App_W5500_UdpTx_t;

typedef struct
{
    App_W5500_UdpRx_t   rx;
    App_W5500_UdpTx_t   tx;
} App_W5500_UdpConn_t;

/* Private variables ---------------------------------------------------------*/
static U8                                       g_app_w5500_dhcpRetryCnt = 0;
static BOOL                                     g_app_w5500_isStaticIpInit  = FALSE;
static Bsp_SwTimer_t                            g_app_w5500_recoveryTimer = {0};
static App_W5500_UdpConn_t                      g_app_udp_conn = {0};

/* Private function prototypes -----------------------------------------------*/
static void App_W5500_Cs_Enable(void);
static void App_W5500_Cs_Disable(void);
static U8 App_W5500_Read_Byte(void);
static void App_W5500_Write_Byte(U8 wb);
static void App_W5500_Read_Burst(U8 *p_buf, U16 len);
static void App_W5500_Write_Burst(U8 *p_buf, U16 len);
static void App_W5500_Set_Static_Default(void);
void App_W5500_Udp_Handler(void);
static BOOL App_W5500_Dhcp_Handler(void);

/* Private consts ------------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

//! @brief  This function is used to enable CS pin of W5500
//! @note   This function is used to enable CS pin of W5500
//! @param  None
//! @retval None
static void App_W5500_Cs_Enable(void)
{
    Bsp_Gpio_Write_Pin(e_BSP_GPIO_PIN_NAME_W5500_CS, e_BSP_GPIO_PIN_RESET);
}


//! @brief  This function is used to disable CS pin of W5500
//! @note   This function is used to disable CS pin of W5500
//! @param  None
//! @retval None
static void App_W5500_Cs_Disable(void)
{
    Bsp_Gpio_Write_Pin(e_BSP_GPIO_PIN_NAME_W5500_CS, e_BSP_GPIO_PIN_SET);
}


//! @brief  This function is used to write one byte to W5500
//! @note   This function is used to read only one byte from w5500
//! @param  None
//! @retval Read byte
static U8 App_W5500_Read_Byte(void)
{
    U8 retVal = 0;
    
    Bsp_Spi_Read(e_BSP_SPI_SPI4, &retVal, 1);
    
    return retVal;
}


//! @brief  This function is used to write one byte to W5500
//! @note   This function is used to write one byte to W5500
//! @param  Byte
//! @retval None
static void App_W5500_Write_Byte(U8 wb)
{
    Bsp_Spi_Write(e_BSP_SPI_SPI4, &wb, 1);
}


//! @brief  This function is used to read in burst mode
//! @note   This function is used to read in burst mode
//! @param  Length, Output Buffer
//! @retval None
static void App_W5500_Read_Burst(U8 *p_buf, U16 len)
{
    for(U32 i = 0; i < len; i++)
    {
        p_buf[i] = App_W5500_Read_Byte();
    }
}


//! @brief  This function is used to write in burst mode
//! @note   This function is used to write in burst mode
//! @param  Length, Input Buffer
//! @retval None
static void App_W5500_Write_Burst(U8 *p_buf, U16 len)
{
    for(U32 i = 0; i < len; i++)
    {
        App_W5500_Write_Byte(p_buf[i]);
    }
}


//! @brief  If dhcp server does not give an ip address we use default settings
//! @note   App_W5500_Set_Static_Default
//! @param  None
//! @retval None
static void App_W5500_Set_Static_Default(void)
{
    W5500_NetInfo_t w5500NetInfo = {0};
    W5500_NetTimeout_t w5500NetTimeout = {0};
    
    W5500_Ctl_Network(e_W5500_CTL_NETWORK_CN_GET_NETINFO, &w5500NetInfo);
    
    App_Get_Default_Ip(w5500NetInfo.a_ip);
    App_Get_Subnet_Mask(w5500NetInfo.a_sn);
    App_Get_Gateway_Ip(w5500NetInfo.a_gw);
    App_Get_Dns(w5500NetInfo.a_dns);
    
    W5500_Ctl_Network(e_W5500_CTL_NETWORK_CN_SET_NETINFO, &w5500NetInfo);
    
    w5500NetTimeout.retryCnt = 8;
    w5500NetTimeout.time100Us = 2000;
    W5500_Ctl_Network(e_W5500_CTL_NETWORK_CN_SET_TIMEOUT, &w5500NetTimeout);
}


//! @brief  App_W5500_Init
//! @note   App_W5500_Init
//! @param  None
//! @retval None
BOOL App_W5500_Init(void)
{
    BOOL retVal = FALSE;
    U8 netMode = W5500_NET_MODE_FORCE_ARP;
    U8 a_macAddress[6] = {0};
    W5500_NetInfo_t w5500NetInfo = {0};
    App_ServerApn_t *p_w5500Apn = NULL;
    
    g_app_w5500_isStaticIpInit = FALSE;
    
    App_W5500_Power_Control(TRUE);
    
    Bsp_SwTimer_Delay_ms(500);
    
    W5500_Init_Func_Cs(&App_W5500_Cs_Enable, &App_W5500_Cs_Disable);
    W5500_Init_Func_Spi_Byte(&App_W5500_Read_Byte, &App_W5500_Write_Byte);
    W5500_Init_Func_Spi_Burst(&App_W5500_Read_Burst, &App_W5500_Write_Burst);
    
    Bsp_Gpio_Write_Pin(e_BSP_GPIO_PIN_NAME_W5500_RST, e_BSP_GPIO_PIN_RESET);
    Bsp_SwTimer_Delay_ms(2);
    Bsp_Gpio_Write_Pin(e_BSP_GPIO_PIN_NAME_W5500_RST, e_BSP_GPIO_PIN_SET);
    Bsp_SwTimer_Delay_ms(2);
    
    if(W5500_Get_Version() == W5500_CHIP_VERSION)
    {
        W5500_Ctl_Network(e_W5500_CTL_NETWORK_CN_SET_NETMODE, &netMode);
        W5500_Ctl_Network(e_W5500_CTL_NETWORK_CN_GET_NETINFO, &w5500NetInfo);
        
        App_Get_Mac_Adress(a_macAddress);
        //! bu mac adresi ideal de �retim testleri i�in kullanilan mac adresi
        w5500NetInfo.a_mac[0] = a_macAddress[0];//0x00;
        w5500NetInfo.a_mac[1] = a_macAddress[1];//0x1c;
        w5500NetInfo.a_mac[2] = a_macAddress[2];//0xa8;
        w5500NetInfo.a_mac[3] = a_macAddress[3];//0xf0;
        w5500NetInfo.a_mac[4] = a_macAddress[4];//0xe8;
        w5500NetInfo.a_mac[5] = a_macAddress[5];//0xf7;
        
        W5500_Ctl_Network(e_W5500_CTL_NETWORK_CN_SET_NETINFO, &w5500NetInfo);
        
        W5500_NetTimeout_t W5500_NetTimeout;
        W5500_NetTimeout.retryCnt = 8;
        W5500_NetTimeout.time100Us = 2000;
        W5500_Ctl_Network(e_W5500_CTL_NETWORK_CN_SET_TIMEOUT, &W5500_NetTimeout);
        
        p_w5500Apn = App_Get_Selected_Server_Apn();
        W5500_TcpClient_Init(APP_W5500_TCP_CLIENT_SOCKET_NUMBER, p_w5500Apn->serverPort, p_w5500Apn->a_serverIP);
        
        if(W5500_Dhcp_Init(APP_W5500_DHCP_SOCKET_NUMBER) == TRUE)
        {
            retVal = TRUE;
        }
    }
    
    return retVal;
}


//! @brief  This function is used to control power of W5500
//! @note   This function is used to control power of W5500
//! @param  Power On/Off
//! @retval None
void App_W5500_Power_Control(BOOL turnOn)
{
    if(turnOn == TRUE)
    {
        Bsp_Gpio_Write_Pin(e_BSP_GPIO_PIN_NAME_W5500_PWR, e_BSP_GPIO_PIN_SET);
    }
    else
    {
        Bsp_Gpio_Write_Pin(e_BSP_GPIO_PIN_NAME_W5500_PWR, e_BSP_GPIO_PIN_RESET);
    }
}


//! @brief  App_W5500_Handler
//! @note   App_W5500_Handler
//! @param  None
//! @retval None
static BOOL App_W5500_Dhcp_Handler(void)
{
    BOOL retVal = FALSE;
    
    switch(W5500_Dhcp_Handler())
    {
        case e_W5500_DHCP_STATUS_RUNNING:
            break;
            
        case e_W5500_DHCP_STATUS_STOPPED:
            break;
            
        case e_W5500_DHCP_STATUS_ASSIGN:
        case e_W5500_DHCP_STATUS_CHANGED:
            break;
            
        case e_W5500_DHCP_STATUS_LEASED:
            retVal = TRUE;
            break;
            
        case e_W5500_DHCP_STATUS_FAILED:
            g_app_w5500_dhcpRetryCnt += 1;
            break;
            
        default:
            W5500_Dhcp_Init(APP_W5500_DHCP_SOCKET_NUMBER);
            break;
    }
    
    return retVal;
}


//! @brief  App_W5500_Get_TcpClientStatus
//! @note   App_W5500_Get_TcpClientStatus
//! @param  None
//! @retval Tcp Client conn established or not
BOOL App_W5500_Get_TcpClientStatus(void)
{
    BOOL retVal = FALSE;
    
    if(W5500_TcpClient_Get_Status() == e_W5500_TCPCLIENT_STATE_ESTABLISHED)
    {
        retVal = TRUE;
    }
    else
    {
        retVal = FALSE;
    }
    
    return retVal;
}

//! @brief  App_W5500_Handler
//! @note   App_W5500_Handler
//! @param  None
//! @retval None
void App_W5500_Handler(void)
{
    U8 a_macAddress[6] = {0};
    W5500_NetInfo_t w5500NetInfo = {0};
    BOOL ipAddIsOk = FALSE;
    
    if(App_Get_Is_Dhcp_Enable() == TRUE)
    {
        ipAddIsOk = App_W5500_Dhcp_Handler();
    }
    else
    {
        if(g_app_w5500_isStaticIpInit == TRUE)
        {
            ipAddIsOk = TRUE;
        }
        else
        {
            g_app_w5500_isStaticIpInit = TRUE;
            App_W5500_Set_Static_Default();
        }
    }
    
    if(ipAddIsOk == TRUE)
    {
        W5500_TcpClient_Handler();
        App_W5500_Udp_Handler();
        App_Service_Handler();
    }
    
    if(Bsp_SwTimer_Enabled(&g_app_w5500_recoveryTimer) == TRUE)
    {
        if(Bsp_SwTimer_Expired(&g_app_w5500_recoveryTimer) == TRUE)
        {
            Bsp_SwTimer_Restart(&g_app_w5500_recoveryTimer);
            W5500_Ctl_Network(e_W5500_CTL_NETWORK_CN_GET_NETINFO, &w5500NetInfo);
            App_Get_Mac_Adress(a_macAddress);
            
            if(MEM_CMP(w5500NetInfo.a_mac, a_macAddress, sizeof(a_macAddress)) != 0)
            {
                App_W5500_Init();
            }
        }
    }
    else
    {
        Bsp_SwTimer_Set(&g_app_w5500_recoveryTimer, APP_W5500_RECOVERY_CHECK_PERIOD_MS);
    }
}

static BOOL udpConnOk = FALSE;
static BOOL udpConnStatus = FALSE;

//! @brief  App_W5500_Handler
//! @note   App_W5500_Handler
//! @param  None
//! @retval None
void App_W5500_Udp_Handler(void)
{
    W5500_SocketError_t W5500_SocketErrorType = e_W5500_SOCKET_ERROR_NO_ERROR;
    
    if(udpConnOk == FALSE)
    {
        if(W5500_Socket_Open_Handler(APP_W5500_SOCKET_NUMBER_UDP_SERVICE, e_W5500_SOCKET_PROTOCOL_MR_UDP, APP_W5500_UDP_PORT, &W5500_SocketErrorType) == TRUE)
        {
            udpConnOk = TRUE;
        }
    }

    if(W5500_Socket_Udp_Receive_Handler(APP_W5500_SOCKET_NUMBER_UDP_SERVICE, g_app_udp_conn.rx.a_data, sizeof(g_app_udp_conn.rx.a_data), &g_app_udp_conn.rx.len, g_app_udp_conn.rx.a_ip, &g_app_udp_conn.rx.port, &W5500_SocketErrorType) == TRUE)
    {
        App_Service_InsertMsg(g_app_udp_conn.rx.a_data, g_app_udp_conn.rx.len);
        /*
        g_app_udp_conn.tx.len = g_app_udp_conn.rx.len;
        MEM_CPY(g_app_udp_conn.tx.a_ip, g_app_udp_conn.rx.a_ip, 4);
        MEM_CPY(g_app_udp_conn.tx.a_data, g_app_udp_conn.rx.a_data, g_app_udp_conn.rx.len);*/
    }
    
    if(W5500_SocketErrorType == e_W5500_SOCKET_ERROR_NO_ERROR)
    {
        udpConnStatus = TRUE;
    }
    else
    {
        udpConnStatus = FALSE;
    }
    
    if(g_app_udp_conn.tx.len != 0)
    {
        if(W5500_Socket_Udp_Send_Handler(APP_W5500_SOCKET_NUMBER_UDP_SERVICE, g_app_udp_conn.tx.a_data, g_app_udp_conn.tx.len, g_app_udp_conn.tx.a_ip, APP_W5500_UDP_PORT, &W5500_SocketErrorType) == TRUE)
        {
            g_app_udp_conn.tx.len = 0;
        }
    }
}

//! @brief  App_Service_Send_Packet_Callback
//! @note   App_Service_Send_Packet_Callback
//! @param  U8 data
//! @retval None
BOOL App_Service_Send_Packet_Callback(U8 *p_txBuff, U16 len)
{
    BOOL retVal = FALSE;
    
    if(g_app_udp_conn.tx.len == 0)
    {
        g_app_udp_conn.tx.len = len;
        MEM_CPY(g_app_udp_conn.tx.a_ip, g_app_udp_conn.rx.a_ip, 4);
        MEM_CPY(g_app_udp_conn.tx.a_data, p_txBuff, len);
        retVal = TRUE;
    }
    
    return retVal;
}

/*********************** (C) COPYRIGHT Litum Embedded Team *****END OF FILE****/
