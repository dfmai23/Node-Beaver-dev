#ifndef RADIO_MANAGER_H
#define RADIO_MANAGER_H

#include <project.h>
#include <stdio.h>
#include "data.h"

#define RADIO_UART 1
#define DATA_LEN (15)  
#define ESCAPE (0x11)
#define STANDARD_LEN 
#define _RECEIVER_ADDR (0x0013A20040C84FBF)         //
#define BROADCAST_ADDR (0x0000000000000001)         //MY 16-bit addr of other xbee/destination addr

#define STARTER_DELIM   (0x7E)      // ~
#define ESCAPE_CTRL     (0x7D)      // }
#define SOFT_FLOW_CTRL1 (0x11)      // Device control 1
#define SOFT_FLOW_CTRL2 (0x13)      // Device control 3

#define ESCAPER (0x20)              // ' ' space

#define XBEE_SPI 1

void radio_init_UART(void);
void radio_init_SPI(void);
void dummy_put();
void radio_put(const DataPacket* data_queue, uint16_t data_head, uint16_t data_tail);
void _XBee_tx_req_(const DataPacket* msg);
uint8_t checksum(uint8_t* msg,int len);
void _set_des_addr(uint64_t newAddr);

CY_ISR_PROTO(xbee_isr);         //custom interrupt
void xbee_send(const DataPacket* data_queue, uint16_t data_head, uint16_t data_tail);

#endif
