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

#include "CAN_messages.h"

#define STRING_BUFF_LEN 32
#define LONG_STRING_BUFF_LEN 512


int main(void) {


	int s; // File descriptor for the socket
	//int nbytes;
	//struct can_frame frame;

	CAN_Init(&s); /* Run the initialization procedure, updating s */

	measurement_t measurements_table[CAN_MAX_TOTAL_MESSAGES];/* this lists all the meas. that are available on CANbus and that we'll include in our .csv file (in order). */
	uint32_t nb_measurements; /* Number of elements in the measurements_table */

	CAN_DetectMeasurements(s, &measurements_table, &nb_measurements);

	printf("Measurement table done\n");



	return 0;
}
