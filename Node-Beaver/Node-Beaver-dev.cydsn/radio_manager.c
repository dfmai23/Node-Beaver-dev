/* Enables xbee transmission to another xbee wirelessly
*/

#include "radio_manager.h"

static uint8_t addr[8];

//isr if xbee receives commands from arduino xbee
CY_ISR(xbee_isr) { 
    xbee_UART_ReadRxStatus();   //clear interrupt associated with Rx
    uint8_t receivedchar = xbee_UART_GetChar();   //char received from other xbee
    
    if (receivedchar == 0x31) { //received a 1
        receivedchar = 0x00;
        LED_Write(1);
        CyDelay(500);
        LED_Write(0);
        CyDelay(500);
        LED_Write(1);
        CyDelay(500);
        LED_Write(0);
        CyDelay(500);
        LED_Write(1);
        CyDelay(500);
        LED_Write(0);
    }
}

void radio_init_UART(void) {
    xbee_UART_Start();
    xbee_UART_EnableRxInt();       //enable Rx interrupts 5bytes RX buffer size on Xbee_UART
    xbee_Rx_int_StartEx(xbee_isr);  //start custom Rx isr
    //xbee_Rx_int_Start();           //start Rx interrupt service
}

void radio_init_SPI(void) {
    xbee_spi_Start();
    _set_des_addr(BROADCAST_ADDR);
}

// UART -------------------------------------------------------------------------------------
//sends data packets over xbee
void xbee_send(const DataPacket* data_queue, uint16_t data_head, uint16_t data_tail) { 
    int i;
    uint8_t xbee_msg[16];   //16bytes per message
    uint8_t xbee_msg2[18];   //16bytes per message
    uint16_t pos;
    
	for(pos=data_head; pos!=data_tail; pos=(pos+1)%DATA_QUEUE_LENGTH)   {   //for all messages in data queue
        LED_Write(1);
        /*
        xbee_msg[0] = (data_queue[pos].id>>8) & 0xff;                //CAN ID 2bytes
        xbee_msg[1] = data_queue[pos].id & 0xff;
        
        xbee_msg[2] = data_queue[pos].millicounter>>24;     //timestamp 4bytes
        xbee_msg[3] = data_queue[pos].millicounter>>16;
        xbee_msg[4] = data_queue[pos].millicounter>>8;
        xbee_msg[5] = data_queue[pos].millicounter;
        
        xbee_msg[6]  = data_queue[pos].data[0];             //payload 8bytes
        xbee_msg[7]  = data_queue[pos].data[1];
        xbee_msg[8]  = data_queue[pos].data[2];
        xbee_msg[9]  = data_queue[pos].data[3];
        xbee_msg[10] = data_queue[pos].data[4];
        xbee_msg[11] = data_queue[pos].data[5];
        xbee_msg[12] = data_queue[pos].data[6];
        xbee_msg[13] = data_queue[pos].data[7];
        
        xbee_msg[14] = 0xFF;                                    //delimited 0xFF10 2bytes
        xbee_msg[15] = 0x0A;
        xbee_UART_PutArray(xbee_msg, 16);
        */
        
        xbee_msg2[0] = (data_queue[pos].id>>8) & 0xff;                //CAN ID 2bytes
        xbee_msg2[1] = data_queue[pos].id & 0xff;
        xbee_msg2[2] = 0x20;
        
        xbee_msg2[3] = data_queue[pos].millicounter>>24;     //timestamp 4bytes
        xbee_msg2[4] = data_queue[pos].millicounter>>16;
        xbee_msg2[5] = data_queue[pos].millicounter>>8;
        xbee_msg2[6] = data_queue[pos].millicounter;
        xbee_msg2[7] = 0x20;
        
        xbee_msg2[8]  = data_queue[pos].data[0];             //payload 8bytes
        xbee_msg2[9]  = data_queue[pos].data[1];
        xbee_msg2[10] = data_queue[pos].data[2];
        xbee_msg2[11] = data_queue[pos].data[3];
        xbee_msg2[12] = data_queue[pos].data[4];
        xbee_msg2[13] = data_queue[pos].data[5];
        xbee_msg2[14] = data_queue[pos].data[6];
        xbee_msg2[15] = data_queue[pos].data[7];
        
        xbee_msg2[16] = 0xFF;                                    //delimited 0xFF10 2bytes
        xbee_msg2[17] = 0x10;
        
        xbee_UART_PutArray(xbee_msg2, 18);
        
        //CyDelay(100);
        LED_Write(0);
        
	} //for
}

// SPI ---------------------------------------------------------------
uint8_t translator(uint8_t data){
    uint8_t Lside=data&0xf;
    uint8_t Hside=(data>>4)&0xf;
    return 0;
}

void _set_des_addr(uint64_t newAddr) {
    int i=0;
    for (i=0;i<8;i++){
        addr[7-i]=(newAddr>>(8*i))&0xff;
    }
    return;
}

void _XBee_tx_req_(const DataPacket* msg){
    int i=0;
    uint8_t send_msg[32];
    
    send_msg[0]=STARTER_DELIM;  //starter   
    send_msg[1]=0x00;           //MSB L
    send_msg[2]=0x1c;           //LSB L
    
    send_msg[3]=0x10;           //Frame type
    send_msg[4]=0x01;           //frame id
    
    send_msg[5]=addr[0];        //addr 8+2
    send_msg[6]=addr[1];
    send_msg[7]=addr[2];
    send_msg[8]=addr[3];
    send_msg[9]=addr[4];
    send_msg[10]=addr[5];
    send_msg[11]=addr[6];
    send_msg[12]=addr[7];
    send_msg[13]=0xff;
    send_msg[14]=0xfe;
    
    send_msg[15]=0x00;          //Broadcast
    send_msg[16]=0x00;          //opions
    
    send_msg[19] = (msg -> millicounter) & 0xff;      //time
    send_msg[18] = ((msg -> millicounter) >> 8) & 0xff;
    send_msg[17] = ((msg -> millicounter) >> 16) & 0xff;

    
    send_msg[21]=msg->id&0xff;      //id
    send_msg[20]=(msg->id>>8)&0xff;
    
    send_msg[22]=msg->length;      //length
    
    
    send_msg[23]=msg->data[0];     //value
    send_msg[24]=msg->data[1];
    send_msg[25]=msg->data[2];  
    send_msg[26]=msg->data[3];  
    send_msg[27]=msg->data[4];  
    send_msg[28]=msg->data[5];  
    send_msg[29]=msg->data[6];  
    send_msg[30]=msg->data[7];  
    send_msg[31]=checksum(send_msg,31);

    xbee_spi_WriteTxData(STARTER_DELIM);
    for (i = 1; i < 32; i++) {
        switch (send_msg[i]) {
            case STARTER_DELIM:
            case ESCAPE_CTRL:
            case SOFT_FLOW_CTRL1:
            case SOFT_FLOW_CTRL2:
                
                xbee_spi_WriteTxData(ESCAPE_CTRL);
                xbee_spi_WriteTxData(ESCAPER ^ send_msg[i]);
                break;
            default:
                xbee_spi_WriteTxData(send_msg[i]);
                break;
        }
    }
    while (!(xbee_spi_ReadTxStatus() & xbee_spi_STS_TX_FIFO_EMPTY)) {}
    CyDelayUs(2);
    return;
}

uint8_t checksum(uint8_t* msg,int len){
    uint8_t cksum=0x00;
    uint8_t i=0;
    for (i=3;i<len;i++){
        cksum=cksum+msg[i];
    }
    cksum=0xff-cksum;
    return cksum;
}

void dummy_put(){
    int i=0;
    uint8_t send_msg[40] = {
        0x7e, 0x00, 0x24,
        0x10, 0x01,
        0x00, 0x13, 0xa2, 0x00, 0x40, 0xc8, 0x4f, 0xbf, 0xff, 0xfe,
        0x00, 0x00,
        0x73,0x65,0x6e,0x64,
        0x20,0x66,
        0x72,
        0x6f,0x6d,0x20,0x61,0x6c,0x69,0x65, 0x6e,
        0x20,0x70,0x6c,0x61,0x6e,0x65,0x74,0xdb};
    uint8_t mymsg[29]={0x7E, 0x00 , 0x19 , 0x10 , 0x01 , 0x00 , 0x13 , 0xA2 , 0x00 , 0x40 , 0xC8 , 0x4F , 0xBF , 0xFF , 0xFE , 0x00 , 0x00 , 0x48 , 0x65 , 0x6C , 0x6C , 0x6F , 0x20 , 0x57 , 0x6F , 0x72 , 0x6C , 0x64 , 0x0A};
    
    for (i=0;i<40;i++){
        xbee_spi_WriteTxData(send_msg[i]);
    }
    return;
}


void radio_put(const DataPacket* data_queue, uint16_t data_head, uint16_t data_tail) {
    uint16_t data_ptr;
    //_XBee_tx_req_(&(data_queue[0])); // test

    for(data_ptr=data_head; data_ptr!=data_tail; data_ptr=(data_ptr+1)%DATA_QUEUE_LENGTH){
       //_XBee_tx_req_(&(data_queue[data_ptr]));
       dummy_put();
    }
} // radio_put()
    

