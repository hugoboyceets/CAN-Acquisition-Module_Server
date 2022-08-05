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
#include <stdbool.h>


#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>

#include "main.h"


#define CAN_INTERFACE "can0"	/* Name of the CANbus interface used for the program*/

#define CAN_MAX_TOTAL_MESSAGES 4096 /* ~128 nodes * ~32 messages = 4096 */

#define CAN_RECEPTION_TIME 1 /* Number of seconds to receive all the messages (must be integer) */

#define CAN_MAX_NODES 128
#define CAN_NODE_ID_Msk 0x7F /* =127. The 7 least significant bits are used for the node IDs.  */


typedef enum { /* The columns of the .csv file will appear in the order they are defined here.
 	 They can be arranged in any random order. However, be sure to leave the last element as CHANNEL_NUMBER_OF_CHANNELS!*/
    CHANNEL_ANALOG_0,
	CHANNEL_ANALOG_1,
	CHANNEL_ANALOG_2,
	CHANNEL_ANALOG_3,
	CHANNEL_ANALOG_4,
	CHANNEL_ANALOG_5,
	CHANNEL_ANALOG_6,
	CHANNEL_ANALOG_7,
	CHANNEL_ANALOG_8,
	CHANNEL_ANALOG_9,
	CHANNEL_ANALOG_10,
	CHANNEL_ANALOG_11,
	CHANNEL_ENV_TEMP,
	CHANNEL_ENV_HUM,
	CHANNEL_ENV_CO2,
	CHANNEL_UPTIME,
	CHANNEL_ERROR,
	CHANNEL_NUMBER_OF_CHANNELS
}channel_t;

typedef struct {

    channel_t channel;
    uint8_t node_id;

}measurement_t;


/* arg s is the file descriptor of the socket */
int CAN_Init(int* s);

void CAN_SendSync(int s);

void CAN_ReceiveMessages(int s);

void CAN_DetectMeasurements(int s, measurement_t *measurements_table, uint32_t *nb_measurements);

bool CAN_SearchMessage(channel_t channel, uint8_t node_id, uint32_t * index);
uint32_t CAN_IDLookup(channel_t channel, uint8_t node_id);
void CAN_ChannelNameLookup(channel_t channel, char * str, size_t str_len);

void CAN_GetMeasurementNameStr(channel_t channel, uint8_t node_id, char * str, size_t str_len);

void CAN_GetMeasurementValueStr(channel_t channel, uint8_t node_id, char * str, size_t str_len);

int CAN_MessagePrintout(int s);
void CAN_NodePrintout(uint8_t, uint8_t *);

#endif /* CAN_MESSAGES_H_ */
