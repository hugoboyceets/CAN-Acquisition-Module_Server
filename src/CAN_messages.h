/*
 * CAN_messages.h
 *
 *  Created on: Aug. 2, 2022
 *      Author: hugo
 */

#ifndef CAN_MESSAGES_H_
#define CAN_MESSAGES_H_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>


#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>


#define CAN_INTERFACE "can0"	/* Name of the CANbus interface used for the program*/

#define CAN_MAX_TOTAL_MESSAGES 4096 /* 128 nodes * 32 messages */


/* arg s is the file descriptor of the socket */
int CAN_Init(int* s);

void CAN_SendSync(int s);

void CAN_ReceiveMessages(int s);

uint32_t CAN_DetectMeasurements(int s/*, measurement_t * measurements_table*/);


#endif /* CAN_MESSAGES_H_ */
