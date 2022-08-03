/*
 ============================================================================
 Name        : CAN-Acquisition-Module_Server.c
 Author      : Hugo Boyce
 Version     :
 Copyright   : Copyright Hugo Boyce 2022
 Description : Prompts acquisition modules on CANbus and saves the received data
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>


#include <linux/can.h>
#include <linux/can/raw.h>

#include "main.h"
#include "CAN_messages.h"




int main(void) {




	int s; // File descriptor for the socket
	//int nbytes;
	//struct can_frame frame;

	CAN_Init(&s); /* Run the initialization procedure, updating s */

	CAN_SendSync(s);/* Send a CAN SYNC frame on the specified socket */

	sleep(1); /* Sleep for 2 seconds */

	CAN_ReceiveMessages(s);

	measurement_t measurements_table[CAN_MAX_TOTAL_MESSAGES];
	uint32_t nb_measurements;




	return 0;
}
