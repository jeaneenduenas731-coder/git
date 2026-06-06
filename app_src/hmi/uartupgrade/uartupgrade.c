#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <nds32_intrinsic.h>
#include "uarts.h"
#include "dma.h"
#include "uarts_interface.h"
#include "timeout.h"
#include "debug.h"
#include "app_config.h"
#include "i2s.h"
#include "i2s_interface.h"
#include "clk.h"
#include "ctrlvars.h"
#include "audio_adc.h"
#include "dac.h"
#include "spdif.h"
#include "uarts_interface.h"
#include "audio_effect_library.h"
#include "rtos_api.h"
#include "roboeffect_api.h"
#include "communication.h"
#include "audio_core_api.h"
#include "roboeffect_prot.h"
#include "ctrlvars.h"
#include "main_task.h"
#include "user_effect_parameter.h"
#include "uartupgrade.h"


#ifdef CFG_UPGRADE_BY_UART

#define MAX_PACKET_SIZE 512

typedef struct {
    uint8_t buffer[MAX_PACKET_SIZE]; // 数据缓冲区
    uint8_t length;                 // 数据包长度
    uint8_t index;                  // 当前接收到的字节索引
    bool packet_received;           // 是否接收到完整数据包
} UART_Packet;
static UART_Packet packet = {0};

void UART_ProcessByte(UART_Packet *packet, uint8_t byte);
void UART_ProcessPacket(UART_Packet *packet); 
TIMER	GetRemoteCmdTime;
bool NeedBeganTime = FALSE;

// 包头定义
const uint8_t HEADER[] = {0xA5, 0x5A, 0x22};
const uint8_t HEADER_SIZE = 3;
const uint8_t PACKET_SIZE = 1;
const uint8_t CRC_SIZE = 1;
const uint8_t CMD_SIZE = 1;

#define ROBO_UART_BUF_MAX_LEN 512
uint8_t robo_uart_recv_buf[ROBO_UART_BUF_MAX_LEN] = {0};
MemHandle uart_recv_handle;

static uint8_t uart_tx_buf[512];
static uint8_t uart_rx_buf[512];
//static uint8_t s_rx_buf[512];
uint8_t recv_byte;
static uint8_t uart_buf[512];
void uartupgrade_data_init(void)
{

	if(GET_DEBUG_GPIO_PORT(CFG_FUNC_UPGRADE_RX_PORT) == DEBUG_GPIO_PORT_A)
		GPIO_PortAModeSet(GET_DEBUG_GPIO_PIN(CFG_FUNC_UPGRADE_RX_PORT), GET_DEBUG_GPIO_MODE(CFG_FUNC_UPGRADE_RX_PORT));
	else
		GPIO_PortBModeSet(GET_DEBUG_GPIO_PIN(CFG_FUNC_UPGRADE_RX_PORT), GET_DEBUG_GPIO_MODE(CFG_FUNC_UPGRADE_RX_PORT));

	if(GET_DEBUG_GPIO_PORT(CFG_FUNC_UPGRADE_TX_PORT) == DEBUG_GPIO_PORT_A)
		GPIO_PortAModeSet(GET_DEBUG_GPIO_PIN(CFG_FUNC_UPGRADE_TX_PORT), GET_DEBUG_GPIO_MODE(CFG_FUNC_UPGRADE_TX_PORT));
	else
		GPIO_PortBModeSet(GET_DEBUG_GPIO_PIN(CFG_FUNC_UPGRADE_TX_PORT), GET_DEBUG_GPIO_MODE(CFG_FUNC_UPGRADE_TX_PORT));

	UARTS_Init(GET_DEBUG_GPIO_UARTPORT(CFG_FUNC_UPGRADE_TX_PORT), CFG_FUNC_UPGRADE_UART_BAUDRATE, 8, 0, 1);

	UART_IOCtl(GET_DEBUG_GPIO_UARTPORT(CFG_FUNC_UPGRADE_TX_PORT), UART_IOCTL_DMA_TX_EN, 1);
	UART_IOCtl(GET_DEBUG_GPIO_UARTPORT(CFG_FUNC_UPGRADE_TX_PORT), UART_IOCTL_DMA_RX_EN, 1);

	DMA_CircularConfig(CFG_FUNC_UPGRADE_RX_DMA_PORT, sizeof(uart_tx_buf)/2, uart_rx_buf, sizeof(uart_tx_buf));
	DMA_CircularConfig(CFG_FUNC_UPGRADE_TX_DMA_PORT, sizeof(uart_tx_buf)/2, uart_tx_buf, sizeof(uart_tx_buf));
	DMA_ChannelEnable(CFG_FUNC_UPGRADE_RX_DMA_PORT);
	DMA_ChannelEnable(CFG_FUNC_UPGRADE_TX_DMA_PORT);

	mv_mopen(&uart_recv_handle, robo_uart_recv_buf, ROBO_UART_BUF_MAX_LEN - 1, NULL);
}

void uartupgrade_data_entry(void)
{
	uint32_t len;
	uint8_t rxdata;
	uint8_t Getdatastatus = 1;

	if(DMA_CircularDataLenGet(CFG_FUNC_UPGRADE_RX_DMA_PORT) > 0)
	{
		memset(uart_buf, 0x00, sizeof(uart_buf));
		DMA_CircularDataGet(CFG_FUNC_UPGRADE_RX_DMA_PORT, uart_buf, 1);
		rxdata = uart_buf[0];
		mv_mwrite(&rxdata, 1, 1, &uart_recv_handle);
	}

	len = mv_msize(&uart_recv_handle);
	if(len > 0)
	{
		mv_mread(&recv_byte, 1, len, &uart_recv_handle);
		UART_ProcessByte(&packet, recv_byte);
	}
	UART_ProcessPacket(&packet);

	if(IsTimeOut(&GetRemoteCmdTime) && NeedBeganTime)
	{
		NeedBeganTime = FALSE;
		UsbupOnlineReadWriteAck(UART_UP_WRITEDATE_ACK_ERR,&Getdatastatus,1);
		APP_DBG("CMD error get remote cmd time out!!!\n");
	}
}


static bool in_packet = FALSE;
static uint8_t header_index = 0;
static uint8_t expected_length = 0;
void UART_ProcessByte(UART_Packet *packet, uint8_t byte) {
    if (!in_packet) {
        // 等待包头
        if (byte == HEADER[header_index]) {
            header_index++;
            if (header_index == HEADER_SIZE) {
                // 包头匹配成功，开始接收数据包
                in_packet = TRUE;
                header_index = 0;
                packet->index = 0;
                packet->buffer[packet->index++] = HEADER[0];
                packet->buffer[packet->index++] = HEADER[1];
                packet->buffer[packet->index++] = HEADER[2];
            }
        } else {
            header_index = 0; // 包头匹配失败，重置
        }
    } else {
        // 检查是否超过最大包长度
        if (packet->index >= MAX_PACKET_SIZE) {
            // 缓冲区溢出，重置状态
            in_packet = FALSE;
            header_index = 0;
            packet->index = 0;
            APP_DBG("Packet buffer overflow!\n");
            return;
        }
        
        // 接收数据
        packet->buffer[packet->index++] = byte;

        // 如果已经接收到长度字段（位置3）
        if (packet->index == HEADER_SIZE + PACKET_SIZE) {
            expected_length = packet->buffer[HEADER_SIZE] + HEADER_SIZE + CRC_SIZE;

            // 检查长度是否有效
            if (expected_length > MAX_PACKET_SIZE || expected_length < (HEADER_SIZE + PACKET_SIZE + CRC_SIZE)) {
                // 长度无效，丢弃数据包
                in_packet = FALSE;
                header_index = 0;
                packet->index = 0;
                APP_DBG("Invalid packet length: %d\n", expected_length);
                return;
            }
        }

        // 如果接收到完整的数据包
        if (packet->index == expected_length && expected_length > 0) {
            // 验证CRC
            uint8_t crc = 0;
            for (int i = 0; i < packet->index - 1; i++) {
                crc += packet->buffer[i];
            }
            
            if (crc == packet->buffer[packet->index - 1]) {
                packet->packet_received = TRUE;
            } else {
                APP_DBG("CRC error: expected %02X, got %02X\n", 
                       packet->buffer[packet->index - 1], crc);
            }
            
            in_packet = FALSE;
            expected_length = 0;
        }
    }
}

void UART_ProcessPacket(UART_Packet *packet) {
    if (packet->packet_received) {
        // 处理完整的数据包
        APP_DBG("Received valid packet: ");
        for (int i = 0; i < packet->index; i++) {
            APP_DBG("%02X ", packet->buffer[i]);
        }
        APP_DBG("\n");
        
        // 根据命令类型处理
        if (packet->index > HEADER_SIZE + PACKET_SIZE) {
            uint8_t cmd = packet->buffer[HEADER_SIZE + PACKET_SIZE];

			switch (cmd)
			{
				case USB_UP_WRITEDATE_ACK:
				case USB_UPG1TX_HANDSHAKE_ACK:
				case USB_UPG1RX_HANDSHAKE_ACK:
				case USB_UPB5TX_HANDSHAKE_ACK:
				case USB_UPB5RX_HANDSHAKE_ACK:
				TimeOutSet(&GetRemoteCmdTime, 5000);
				break;

				case USB_UP_END_ACK:
					DMA_CircularDataPut(CFG_FUNC_UPGRADE_TX_DMA_PORT, packet->buffer, packet->index);
					break;
				
				default:
					break;
			}
        }
        
        Union_Effect_Send(packet->buffer, packet->index);
        
        // 重置状态
        packet->packet_received = FALSE;
        packet->index = 0;
    }
}


void UsbupOnlineReadWriteAck(uint8_t cmd,uint8_t *buf,uint8_t len)
{
	uint8_t send_buffer[10];
	uint8_t crc = 0;
	uint8_t i;
	uint8_t send_len;
	memcpy(&send_buffer[0],HEADER,HEADER_SIZE);
	send_buffer[HEADER_SIZE] = CMD_SIZE + len + PACKET_SIZE;
	send_buffer[HEADER_SIZE + PACKET_SIZE] = cmd;
	memcpy(&send_buffer[HEADER_SIZE+PACKET_SIZE+CMD_SIZE],buf,len);
	for(i=0;i<HEADER_SIZE + CMD_SIZE + PACKET_SIZE + len;i++)//{0xA5, 0x5A, 0x22, 0x05, 0x10, 0x02, 0x03,0x04,0x05};
	{
		crc+=send_buffer[i];
	}
	send_buffer[HEADER_SIZE + CMD_SIZE + PACKET_SIZE + len] = crc;
	send_len = send_buffer[HEADER_SIZE] + HEADER_SIZE + CRC_SIZE;
	APP_DBG("UartupOnlineReadWriteAck len: %d\n",send_len);
	Union_Effect_Send(send_buffer,HEADER_SIZE+CMD_SIZE+len);
}

void UsbToUartProc(uint8_t *buf)
{   
	uint8_t Getdatastatus = 1;
	uint8_t i = 0;
	uint8_t crc = 0;

	uint16_t cmd_len = 0;
	UartUpCmd GetUartcmd;
	memset(&GetUartcmd,0,sizeof(UartUpCmd)); 
	memcpy(&GetUartcmd,&buf[HEADER_SIZE],sizeof(UartUpCmd));

	cmd_len = GetUartcmd.len;

	for(i=0;i<cmd_len + HEADER_SIZE;i++)//{0xA5, 0x5A, 0x22, 0x05, 0x10, 0x02, 0x03,0x04,0x05};
	{
		crc+=buf[i];	
	}
	if(crc != buf[cmd_len + HEADER_SIZE])
	{
		APP_DBG("data crc erro: %x,%x!!!\n",crc,buf[cmd_len + HEADER_SIZE]);
		return;
	}

    switch(GetUartcmd.cmd)
    {
		case USB_UPB5_HANDSHAKE:
			NeedBeganTime = TRUE;
			TimeOutSet(&GetRemoteCmdTime, 5000);
			APP_DBG("this is b5 upgrade...\n");
			break;

		case USB_UPG1TX_HANDSHAKE:
			NeedBeganTime = TRUE;
			TimeOutSet(&GetRemoteCmdTime, 5000);
			APP_DBG("this is g1 tx upgrade...\n");
			break;

		case USB_UPG1RX_HANDSHAKE:
			NeedBeganTime = TRUE;
			TimeOutSet(&GetRemoteCmdTime, 5000);
			APP_DBG("this is g1 rx upgrade...\n");
			break;

		case USB_UPB5TX_HANDSHAKE:
			NeedBeganTime = TRUE;
			TimeOutSet(&GetRemoteCmdTime, 5000);
			APP_DBG("this is b5 tx upgrade...\n");
			break;

		case USB_UPB5RX_HANDSHAKE:
			NeedBeganTime = TRUE;
			TimeOutSet(&GetRemoteCmdTime, 5000);
			APP_DBG("this is b5 rx upgrade...\n");
			break;
        
		case USB_UP_WRITEDATE:
			APP_DBG("mva writedate...\n");
			break;

		case USB_UP_END:
			NeedBeganTime = FALSE;
			APP_DBG("mva end...\n");
			break;

        default:
            break;
    }
	DMA_CircularDataPut(CFG_FUNC_UPGRADE_TX_DMA_PORT, buf, 256);
}

#endif
