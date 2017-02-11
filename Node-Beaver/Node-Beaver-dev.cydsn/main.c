/* Node-Beaver
	 This node logs data via CAN and presents the data to the outside world.
 */

#include <project.h>

#include "data.h"
#include "time_manager.h"
#include "can_manager.h"
#include "usb_manager.h"
#include "sd_manager.h"
#include "radio_manager.h"

extern uint8 command_received;  //in xbee_Rx_int.c     CY_ISR(xbee_isr_Interrupt) fn

int main(void) {
	CYGlobalIntEnable;      //Uncomment this line to enable global interrupts
    
	DataPacket data_queue[DATA_QUEUE_LENGTH];
	uint16_t data_head, data_tail;
	data_head = data_tail = 0;
    uint8 i;
    
    LED_Write(1);
	//time_init();                //init everything
	can_init();
	//usb_init();
	//sd_init(time_get());
    radio_init_UART();          //xbee UART
    //radio_init_SPI();           //xbee SPI
    CyDelay(3000);
    LED_Write(0);
    CyDelay(1000);
    
	for(;;)	{
        //can_test_send();
        //usb_get();
       
        //time_announce(data_queue, &data_head, &data_tail);  //time of current message
        can_test_receive(data_queue, &data_tail, &data_head);
		can_get(data_queue, &data_head, &data_tail);        //gets received messages from the CAN network,
		
        
        //save messages to external devices
		usb_put(data_queue, data_head, data_tail);  		//send message over usb
		sd_push(data_queue, data_head, data_tail);          //inject message into sd card
        xbee_send(data_queue, data_head, data_tail);        //send message over xbee UART
        //radio_put(data_queue, data_head, data_tail);         //send message over xbee SPI
		data_head = data_tail = 0;                          //clear buffer       
        
        //ISR
        /*
		if(power_comp_GetCompare()) { // if powered down, 
			LED_Write(1);               //jumps to isr
         }
        */
        
		CyDelay(1000);                 //refresh intervaal
	} // main loop

	return 0;
} // main()
