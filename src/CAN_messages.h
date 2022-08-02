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

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>


#define CAN_INTERFACE "can0"	/* Name of the CANbus interface used for the program*/


/* arg s is the file descriptor of the socket */
int CAN_Init(int* s);

void CAN_SendSync(int s);

int CAN_ReceiveMessages(int s);


#endif /* CAN_MESSAGES_H_ */
