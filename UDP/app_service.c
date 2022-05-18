/**
  ******************************************************************************
  * @file    app_service.c
  * @author  Litum Embedded Team
  * @brief   app_service C file.
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
#include "app_service.h"
#include "devInfo.h"
#include "app.h"

/* Private define ------------------------------------------------------------*/
#define APP_SERVICE_LITUM_NMEA_SENTENCE             "$LITCSV"
#define APP_SERVICE_LITUM_NMEA_SENTENCE_SIZE        7

/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef enum
{
    e_APP_SERVICE_FRAME_STATE_NONE              = 0x00,
    e_APP_SERVICE_FRAME_STATE_SOF               = 0x01,
    e_APP_SERVICE_FRAME_STATE_LEN               = 0x02,
    e_APP_SERVICE_FRAME_STATE_DATA              = 0x03,
    e_APP_SERVICE_FRAME_STATE_CS                = 0x04,
    e_APP_SERVICE_FRAME_STATE_EOF               = 0x05,
} App_Service_Frame_State_t;

typedef enum
{
    e_APP_SERVICE_MSG_TYPE_NONE                 = 0x00,
    e_APP_SERVICE_MSG_TYPE_DISCOVERY            = 0x01,
    e_APP_SERVICE_TEST_TYPE_SET                 = 0x02,
    e_APP_SERVICE_TEST_TYPE_GET                 = 0x03,
} App_Service_Msg_Type_t;

typedef enum
{
    e_APP_SERVICE_SETGET_RESET                  = 0x1D,
    e_APP_SERVICE_SETGET_MAC_ADDRESS            = 0x20,
    e_APP_SERVICE_SETGET_IP_SERVER              = 0x21,
    e_APP_SERVICE_SETGET_SERVER_PORT            = 0x22,
    e_APP_SERVICE_SETGET_IP_STATIC              = 0x23,
    e_APP_SERVICE_SETGET_DHCP                   = 0x47,
    e_APP_SERVICE_SETGET_IP_CONFIG_SERVER       = 0x58,
    e_APP_SERVICE_SETGET_CONFIG_SERVER_PORT     = 0x59,
    e_APP_SERVICE_SETGET_SUBNET_MASK            = 0x5D,
    e_APP_SERVICE_SETGET_IP_GATEWAY             = 0x5E,
    e_APP_SERVICE_SETGET_IP_DNS                 = 0x5F,
} App_Service_setGet_t;

typedef struct
{
    U8          len;
    U8          ver;
    U8          msgType;
    U8          a_data[255];
} App_Service_DataField_t;

typedef struct
{
    U8          frameVer;
    U8          frameMsgNo;
    U16         sourceNodeId;
    U16         destNodeId;
} App_Service_ParseMsg_t;

/* Private variables ---------------------------------------------------------*/
static App_Service_ParseMsg_t   g_app_service_parseMsg = {0};
static App_Service_DataField_t  g_app_service_dataField = {0};

/* Private function prototypes -----------------------------------------------*/
/* Private consts ------------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

//! @brief  App_Service_Check_Command
//! @note   App_Service_Check_Command
//! @param  None
//! @retval None
static void App_Service_Check_Command(void)
{
    U8 *p_nodeId;
    U8 txLen = 0;
    U8 txData[255];
    U8 a_ip[4];
    U8 a_mac[6];
    U16 port = 0;
    U16 ownNodeId = 0;
    
    p_nodeId = DevInfo_Get_Node_Id();
    MEM_CPY(&ownNodeId, p_nodeId, 2);
    
    if((g_app_service_parseMsg.destNodeId & ownNodeId) == ownNodeId)
    {
        switch(g_app_service_dataField.msgType)
        {
            case e_APP_SERVICE_MSG_TYPE_NONE:
                break;
                
            case e_APP_SERVICE_MSG_TYPE_DISCOVERY:
                txLen += sprintf((char *)&txData[txLen], "DISCOVERY,NODEID,%04X*00\r\n", ownNodeId);
                break;
                
            case e_APP_SERVICE_TEST_TYPE_SET:
            
                txLen += sprintf((char *)&txData[txLen], "SET,");
                
                switch(g_app_service_dataField.a_data[0])
                {
                    case e_APP_SERVICE_SETGET_RESET:
                        if(g_app_service_dataField.a_data[1] == 'R')
                        {
                            App_Sys_Reset_After_One_Sec();
                            txLen += sprintf((char *)&txData[txLen], "RESET\r\n");
                        }
                        break;
                        
                    case e_APP_SERVICE_SETGET_MAC_ADDRESS:
                        txLen += sprintf((char *)&txData[txLen], "MAC ADDRESS\r\n");
                        MEM_CPY(a_mac, &g_app_service_dataField.a_data[1], 6);
                        App_Set_Mac_Adress(a_mac);
                        break;
                        
                    case e_APP_SERVICE_SETGET_IP_DNS:
                        if(g_app_service_dataField.len == 7)
                        {
                            txLen += sprintf((char *)&txData[txLen], "DNS\r\n");
                            MEM_CPY(a_ip, &g_app_service_dataField.a_data[1], 4);
                            App_Set_Dns(a_ip);
                        }
                        break;
                        
                    case e_APP_SERVICE_SETGET_IP_GATEWAY:
                        if(g_app_service_dataField.len == 7)
                        {
                            txLen += sprintf((char *)&txData[txLen], "GATEWAY IP\r\n");
                            MEM_CPY(a_ip, &g_app_service_dataField.a_data[1], 4);
                            App_Set_Gateway_Ip(a_ip);
                        }
                        break;
                        
                    case e_APP_SERVICE_SETGET_SUBNET_MASK:
                        if(g_app_service_dataField.len == 7)
                        {
                            txLen += sprintf((char *)&txData[txLen], "SUBNET MASK\r\n");
                            MEM_CPY(a_ip, &g_app_service_dataField.a_data[1], 4);
                            App_Set_Subnet_Mask(a_ip);
                        }
                        break;
                        
                    case e_APP_SERVICE_SETGET_IP_STATIC:
                        if(g_app_service_dataField.len == 7)
                        {
                            txLen += sprintf((char *)&txData[txLen], "STATIC IP\r\n");
                            MEM_CPY(a_ip, &g_app_service_dataField.a_data[1], 4);
                            App_Set_Default_Ip(a_ip);
                        }
                        break;
                        
                    case e_APP_SERVICE_SETGET_DHCP:
                        txLen += sprintf((char *)&txData[txLen], "DHCP\r\n");
                        App_Set_Is_Dhcp_Enable(g_app_service_dataField.a_data[1]);
                        break;
                        
                    case e_APP_SERVICE_SETGET_SERVER_PORT:
                        txLen += sprintf((char *)&txData[txLen], "SERVER PORT\r\n");
                        MEM_CPY(&port, &g_app_service_dataField.a_data[1], 2);
                        App_Set_Server_Port(port);
                        break;
                        
                    case e_APP_SERVICE_SETGET_IP_SERVER:
                        if(g_app_service_dataField.len == 7)
                        {
                            txLen += sprintf((char *)&txData[txLen], "SERVER IP\r\n");
                            MEM_CPY(a_ip, &g_app_service_dataField.a_data[1], 4);
                            App_Set_Server_Adress(a_ip);
                        }
                        break;
                        
                    case e_APP_SERVICE_SETGET_CONFIG_SERVER_PORT:
                        txLen += sprintf((char *)&txData[txLen], "CONFIG SERVER PORT\r\n");
                        MEM_CPY(&port, &g_app_service_dataField.a_data[1], 2);
                        App_Set_Config_Server_Port(port);
                        break;
                        
                    case e_APP_SERVICE_SETGET_IP_CONFIG_SERVER:
                        if(g_app_service_dataField.len == 7)
                        {
                            txLen += sprintf((char *)&txData[txLen], "CONFIG SERVER IP\r\n");
                            MEM_CPY(a_ip, &g_app_service_dataField.a_data[1], 4);
                            App_Set_Config_Server_Adress(a_ip);
                        }
                        break;
                }
                break;
                
            case e_APP_SERVICE_TEST_TYPE_GET:
                txLen += sprintf((char *)&txData[txLen], "GET,");
                
                switch(g_app_service_dataField.a_data[0])
                {
                    case e_APP_SERVICE_SETGET_RESET:
                    
                        break;
                        
                    case e_APP_SERVICE_SETGET_MAC_ADDRESS:
                    
                        App_Get_Mac_Adress(a_mac);
                        txLen += sprintf((char *)&txData[txLen], "MAC ADDRESS %02X:%02X:%02X:%02X:%02X:%02X\r\n", a_mac[0], a_mac[1], a_mac[2], a_mac[3], a_mac[4], a_mac[5]);
                        
                        break;
                        
                    case e_APP_SERVICE_SETGET_IP_DNS:
                    
                        App_Get_Dns(a_ip);
                        txLen += sprintf((char *)&txData[txLen], "DNS %d.%d.%d.%d\r\n", a_ip[0], a_ip[1], a_ip[2], a_ip[3]);
                        
                        break;
                        
                    case e_APP_SERVICE_SETGET_IP_GATEWAY:
                    
                        App_Get_Gateway_Ip(a_ip);
                        txLen += sprintf((char *)&txData[txLen], "GATEWAY IP %d.%d.%d.%d\r\n", a_ip[0], a_ip[1], a_ip[2], a_ip[3]);
                        
                        break;
                        
                    case e_APP_SERVICE_SETGET_SUBNET_MASK:
                    
                        App_Get_Subnet_Mask(a_ip);
                        txLen += sprintf((char *)&txData[txLen], "SUBNET MASK %d.%d.%d.%d\r\n", a_ip[0], a_ip[1], a_ip[2], a_ip[3]);
                        
                        break;
                        
                    case e_APP_SERVICE_SETGET_IP_STATIC:
                    
                        App_Get_Default_Ip(a_ip);
                        txLen += sprintf((char *)&txData[txLen], "STATIC IP %d.%d.%d.%d\r\n", a_ip[0], a_ip[1], a_ip[2], a_ip[3]);
                        
                        break;
                        
                    case e_APP_SERVICE_SETGET_DHCP:
                      
                        txLen += sprintf((char *)&txData[txLen], "DHCP %d\r\n", App_Get_Is_Dhcp_Enable());
                        
                        break;
                        
                    case e_APP_SERVICE_SETGET_SERVER_PORT:
                        
                        port = App_Get_Server_Port();
                        txLen += sprintf((char *)&txData[txLen], "SERVER PORT %d\r\n", port);
                        
                        break;
                        
                    case e_APP_SERVICE_SETGET_IP_SERVER:
                    
                        App_Get_Server_Adress(a_ip);
                        txLen += sprintf((char *)&txData[txLen], "SERVER IP %d.%d.%d.%d\r\n", a_ip[0], a_ip[1], a_ip[2], a_ip[3]);
                        
                        break;
                        
                    case e_APP_SERVICE_SETGET_CONFIG_SERVER_PORT:
                    
                        port = App_Get_Config_Server_Port();
                        txLen += sprintf((char *)&txData[txLen], "CONFIG SERVER PORT %d\r\n", port);
                        
                        break;
                        
                    case e_APP_SERVICE_SETGET_IP_CONFIG_SERVER:
                    
                        App_Get_Config_Server_Adress(a_ip);
                        txLen += sprintf((char *)&txData[txLen], "CONFIG SERVER IP %d.%d.%d.%d\r\n", a_ip[0], a_ip[1], a_ip[2], a_ip[3]);
                        
                        break;
                }
                break;
        }
    }
    
    if(txLen != 0)
    {
        App_Service_Send_Packet_Callback(txData, txLen);
    }
}

//void App_Set_Node_Id(U16 nodeId);
//void App_Set_Sudoku(U8 sudokuVal);
//void App_Set_Max_Sudoku(U8 maxSudokuVal);
//void App_Set_Ext_Sudoku(U8 extSudokuVal);



//! @brief  App_Service_Framer
//! @note   App_Service_Framer
//! @param  None
//! @retval None
static BOOL App_Service_Framer(U8 *p_msg, U16 msgLen)
{
    U16 i = 0;
    U8 data = 0;
    U8 frameLen = 0;
    U8 checksum = 0;
    BOOL frameOk = FALSE;
    BOOL frameError = FALSE;
    App_Service_Frame_State_t frameState = e_APP_SERVICE_FRAME_STATE_SOF;
    
    for(i = 0; i < msgLen; i++)
    {
        data = p_msg[i];
        
        switch(frameState)
        {
            case e_APP_SERVICE_FRAME_STATE_NONE:
                frameError = TRUE;
                break;
                
            case e_APP_SERVICE_FRAME_STATE_SOF:
                if(data == '[')
                {
                    frameState = e_APP_SERVICE_FRAME_STATE_LEN;
                }
                else
                {
                    frameState = e_APP_SERVICE_FRAME_STATE_NONE;
                }
                break;
                
            case e_APP_SERVICE_FRAME_STATE_LEN:
                if(data > 1)
                {
                    frameLen = data - 1;
                    checksum += data;
                    frameState = e_APP_SERVICE_FRAME_STATE_DATA;
                }
                else
                {
                    frameState = e_APP_SERVICE_FRAME_STATE_NONE;
                }
                break;
                
            case e_APP_SERVICE_FRAME_STATE_DATA:
                checksum += data;
                frameLen -= 1;
                if(frameLen == 0)
                {
                    frameState = e_APP_SERVICE_FRAME_STATE_CS;
                }
                break;
                
            case e_APP_SERVICE_FRAME_STATE_CS:
                if(checksum == data)
                {
                    frameState = e_APP_SERVICE_FRAME_STATE_EOF;
                }
                else
                {
                    frameState = e_APP_SERVICE_FRAME_STATE_NONE;
                }
                break;
                
            case e_APP_SERVICE_FRAME_STATE_EOF:
                if(data == ']')
                {
                    frameOk = TRUE;
                }
                else
                {
                    frameState = e_APP_SERVICE_FRAME_STATE_NONE;
                }
                break;
        }
        
        if((frameError == TRUE) || (frameOk == TRUE))
        {
            break;
        }
    }
    
    return frameOk;
}


//! @brief  App_Service_Parser
//! @note   App_Service_Parser
//! @param  None
//! @retval None
static void App_Service_Parser(U8 *p_msg, U16 msgLen)
{
    MEM_CPY(&g_app_service_parseMsg.frameVer, (p_msg + 2), sizeof(g_app_service_parseMsg));
    MEM_CPY(&g_app_service_dataField.len, (p_msg + 8), (*(p_msg + 8) + 1));
}


//! @brief  App_Service_InsertMsg
//! @note   App_Service_InsertMsg
//! @param  None
//! @retval None
BOOL App_Service_InsertMsg(U8 *p_msg, U16 msgLen)
{
    BOOL retVal = FALSE;
    
    if(App_Service_Framer(p_msg, msgLen) == TRUE)
    {
        App_Service_Parser(p_msg, msgLen);
        App_Service_Check_Command();
    }
    
    return retVal;
}


//! @brief  App_Service_Handler
//! @note   App_Service_Handler
//! @param  None
//! @retval None
void App_Service_Handler(void)
{

}


//! @brief  App_Service_Send_Packet_Callback
//! @note   App_Service_Send_Packet_Callback
//! @param  U8 data
//! @retval None
__weak BOOL App_Service_Send_Packet_Callback(U8 *p_txBuff, U16 len)
{
    //! @note   This function Should not be modified, when the callback is needed,
    //! @note   the App_Service_Send_Packet_Callback could be implemented in the user
    return FALSE;
}
/*********************** (C) COPYRIGHT Litum Embedded Team *****END OF FILE****/
