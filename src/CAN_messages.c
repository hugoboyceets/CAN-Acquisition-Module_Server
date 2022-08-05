/*
 * CAN_messages.c
 *
 *  Created on: Aug. 2, 2022
 *      Author: hugo
 */
#include "CAN_messages.h"

struct can_frame can_buff[CAN_MAX_TOTAL_MESSAGES]; /* Global CAN message buffer */
uint32_t can_msg_cnt;

int CAN_Init(int* s){

	struct sockaddr_can addr;
	struct ifreq ifr;

	const char *ifname = CAN_INTERFACE;

		if ((*s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) == -1) {
			perror("Error while opening socket");
			return -1;
		}

		strcpy(ifr.ifr_name, ifname);
		ioctl(*s, SIOCGIFINDEX, &ifr);

		fcntl(*s, F_SETFL, O_NONBLOCK); /* This will set the socket *globally* as non-blocking. That means if any other program uses the socket, it will
		be non-blocking too. recvfrom() with the MSG_DONTWAIT flag can be used to read in a non-blocking way on a per-call basis. From "man 2 recv", manual page for recv(); :

		       MSG_DONTWAIT (since Linux 2.2)
              Enables nonblocking operation; if the operation would block, the call  fails  with  the
              error  EAGAIN or EWOULDBLOCK.  This provides similar behavior to setting the O_NONBLOCK
              flag (via the fcntl(2) F_SETFL operation), but differs in that MSG_DONTWAIT is  a  per-
              call  option,  whereas  O_NONBLOCK  is  a  setting  on  the  open file description (see
              open(2)), which will affect all threads in the calling process and  as  well  as  other
              processes that hold file descriptors referring to the same open file description.


		*/

		addr.can_family  = AF_CAN;
		addr.can_ifindex = ifr.ifr_ifindex;

		printf("Using %s at index %d.\n", ifname, ifr.ifr_ifindex);

		if (bind(*s, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
			perror("Error in socket bind");
			return -2;
		}

		return 0;

}

void CAN_SendSync(int s){

	int nbytes;
	struct can_frame frame;

	frame.can_id  = 0x80 | CAN_EFF_FLAG; /* set address while forcing Extended Frame Format */
	frame.can_dlc = 2;
	frame.data[0] = 0x11;
	frame.data[1] = 0x22;

	nbytes = write(s, &frame, sizeof(struct can_frame));

	//printf("Wrote %d bytes\n", nbytes);

}

void CAN_ReceiveMessages(int s){


	int nbytes = 0; /* Initialize to zero so the fct runs the first time */
	can_msg_cnt = 0; /* Reset the message count cause we're about to receive a new batch of messages */

	do{

		nbytes = read(s, &can_buff[can_msg_cnt], sizeof(struct can_frame));/* Lis les messages CAN en les mettant dans le buffer */
		if(nbytes > 0){ /* Si un message significatif a été reçu */
			can_msg_cnt++;
		}

	}while(!(nbytes < 0));/* nbytes sera négatif lorsqu'il n'y aura plus de messages à recevoir */


}

void CAN_DetectMeasurements(int s, measurement_t * measurements_table, uint32_t * nb_measurements){


	uint32_t i, j; /* General purpose counters */

	CAN_SendSync(s);/* Send a CAN SYNC frame on the specified socket */
	sleep(CAN_RECEPTION_TIME); /* Sleep for a few seconds */
	CAN_ReceiveMessages(s); /* Bring all received messages into the buffer */

	/****************************************** Detect active nodes **********************************************************/
	bool nodeActive[CAN_MAX_NODES] = {false}; /* table of every possible node ID, we assume none are active to begin with. */
	for(i = 0; i < can_msg_cnt; i++){/* For every received message */
		nodeActive[can_buff[i].can_id & CAN_NODE_ID_Msk] = true; /* Set value as true at index (Node ID) in the table of all possible node IDs. */
	}

	uint8_t listOfActiveNodes[CAN_MAX_NODES]; /* Make a neat consecutive list of active nodes in ascending order */
	uint8_t totalNumberOfActiveNodes = 0;
	for(i = 0; i < CAN_MAX_NODES; i++){ /* For every possible node ID */
		if(nodeActive[i]){ /* If the node we're looking at is active */
			listOfActiveNodes[totalNumberOfActiveNodes] = i; /* Add it to the list */
			totalNumberOfActiveNodes++; /* Increment the count of active nodes */
		}
	}
	CAN_NodePrintout(totalNumberOfActiveNodes, listOfActiveNodes); /* Printout the list of nodes found */
	/****************************************** End of node detection **********************************************************/


	/****************************************** Build the measurements table ***************************************************/
	*nb_measurements = 0; /* reset the number of measurements since we're going to detect them all now. */

	for(i = 0; i < totalNumberOfActiveNodes; i++){/* For every active node */

		for(j = 0; j < CHANNEL_NUMBER_OF_CHANNELS; j++ ){ /* For every possible channel */

			if(CAN_SearchMessage(j, listOfActiveNodes[i], NULL)){ /* If the channel and node we're trying is available*/
				measurements_table[*nb_measurements].channel = j;/* Save it to the measurements table */
				measurements_table[*nb_measurements].node_id = listOfActiveNodes[i];
				(*nb_measurements)++;/* Increment the number of measurements */
			}

		}

	}
	/****************************************** Measurement table built ***************************************************/

	printf("Measurement table done\n");

}

bool CAN_SearchMessage(channel_t channel, uint8_t node_id, uint32_t * index){

	/*
	 * Returns true if the specified message is in the buffer, else returns false
	 *
	 * If the message is found, it will put the index of the message in the variable pointed by index. (unless the pointer is null)
	 *
	 * */
	uint32_t can_id;

	can_id = CAN_IDLookup(channel, node_id);

	uint32_t i;
	for(i = 0; i < can_msg_cnt; i++){/* For every CAN message in buffer */

		if(can_buff[i].can_id == can_id){ /* If the id of the message matches the id we're trying to find */
			if(index != NULL){
				*index = i;
			}
			return true;
		}

	}

	return false;
}

uint32_t CAN_IDLookup(channel_t channel, uint8_t node_id){ /* Get the CAN ID of a defined channel and node ID */

	/*
	 * See message definitions in the CAN_message.h file of the acquisition module firmware
	 *
	 * */

	node_id &= CAN_NODE_ID_Msk; /* Sanitize... */

	switch(channel){

		case CHANNEL_ANALOG_0:
			return 0x00001080 | node_id | CAN_EFF_FLAG;
			break;


		case CHANNEL_ANALOG_1:
			return 0x00001180 | node_id | CAN_EFF_FLAG;
			break;

		case CHANNEL_ANALOG_2:
			return 0x00001280 | node_id | CAN_EFF_FLAG;
			break;

		case CHANNEL_ANALOG_3:
			return 0x00001380 | node_id | CAN_EFF_FLAG;
			break;

		case CHANNEL_ANALOG_4:
			return 0x00001480 | node_id | CAN_EFF_FLAG;
			break;

		case CHANNEL_ANALOG_5:
			return 0x00001580 | node_id | CAN_EFF_FLAG;
			break;

		case CHANNEL_ANALOG_6:
			return 0x00001680 | node_id | CAN_EFF_FLAG;
			break;

		case CHANNEL_ANALOG_7:
			return 0x00001780 | node_id | CAN_EFF_FLAG;
			break;

		case CHANNEL_ANALOG_8:
			return 0x00001880 | node_id | CAN_EFF_FLAG;
			break;

		case CHANNEL_ANALOG_9:
			return 0x00001980 | node_id | CAN_EFF_FLAG;
			break;

		case CHANNEL_ANALOG_10:
			return 0x00001A80 | node_id | CAN_EFF_FLAG;
			break;

		case CHANNEL_ANALOG_11:
			return 0x00001B80 | node_id | CAN_EFF_FLAG;
			break;

		case CHANNEL_ENV_TEMP:
			return 0x00000180 | node_id | CAN_EFF_FLAG;
			break;

		case CHANNEL_ENV_HUM:
			return 0x00000280 | node_id | CAN_EFF_FLAG;
			break;

		case CHANNEL_ENV_CO2:
			return 0x00000380 | node_id | CAN_EFF_FLAG;
			break;

		case CHANNEL_UPTIME:
			return 0x00002080 | node_id | CAN_EFF_FLAG;
			break;

		case CHANNEL_ERROR:
			return 0x00002080 | node_id | CAN_EFF_FLAG;
			break;

		default:
			return 0;
	}

}

void CAN_ChannelNameLookup(channel_t channel, char * str, size_t str_len){ /* Get the name string of a channel */


	switch(channel){

		case CHANNEL_ANALOG_0:
			snprintf(str, str_len, "Analog_channel_0");
			break;


		case CHANNEL_ANALOG_1:
			snprintf(str, str_len, "Analog_channel_1");
			break;

		case CHANNEL_ANALOG_2:
			snprintf(str, str_len, "Analog_channel_2");
			break;

		case CHANNEL_ANALOG_3:
			snprintf(str, str_len, "Analog_channel_3");
			break;

		case CHANNEL_ANALOG_4:
			snprintf(str, str_len, "Analog_channel_4");
			break;

		case CHANNEL_ANALOG_5:
			snprintf(str, str_len, "Analog_channel_5");
			break;

		case CHANNEL_ANALOG_6:
			snprintf(str, str_len, "Analog_channel_6");
			break;

		case CHANNEL_ANALOG_7:
			snprintf(str, str_len, "Analog_channel_7");
			break;

		case CHANNEL_ANALOG_8:
			snprintf(str, str_len, "Analog_channel_8");
			break;

		case CHANNEL_ANALOG_9:
			snprintf(str, str_len, "Analog_channel_9");
			break;

		case CHANNEL_ANALOG_10:
			snprintf(str, str_len, "Analog_channel_10");
			break;

		case CHANNEL_ANALOG_11:
			snprintf(str, str_len, "Analog_channel_11");
			break;

		case CHANNEL_ENV_TEMP:
			snprintf(str, str_len, "Environmental_sensor_temperature");
			break;

		case CHANNEL_ENV_HUM:
			snprintf(str, str_len, "Environmental_sensor_humidity");
			break;

		case CHANNEL_ENV_CO2:
			snprintf(str, str_len, "Environmental_sensor_CO2");
			break;

		case CHANNEL_UPTIME:
			snprintf(str, str_len, "Module_uptime");
			break;

		case CHANNEL_ERROR:
			snprintf(str, str_len, "Module_error_code");
			break;

	}

}


void CAN_GetMeasurementNameStr(channel_t channel, uint8_t node_id, char * str, size_t str_len){


	char temp_str_A[STRING_BUFF_LEN];
	snprintf(temp_str_A, STRING_BUFF_LEN, "Node_%u.", node_id);

	char temp_str_B[STRING_BUFF_LEN];
	CAN_ChannelNameLookup(channel, temp_str_B, STRING_BUFF_LEN);

	snprintf(str, str_len, "%s%s", temp_str_A, temp_str_B);

}

void CAN_GetMeasurementValueStr(channel_t channel, uint8_t node_id, char * str, size_t str_len){

	uint32_t index;
	int32_t i;
	uint32_t temp_uint = 0;
	float temp_float;

	if(CAN_SearchMessage(channel, node_id, &index)){

		if(channel == CHANNEL_ERROR){/* If the channel is error... */

			for(i = 0; i < sizeof(temp_uint) ; i++){/* the data is in the lowest 4 bytes */

				temp_uint <<= 8; /* left shift by one byte */
				temp_uint |= can_buff[index].data[i];

			}

		}else{/* Else the data is in the upper 4 bytes */

			for(i = 4 ; i  < 4 + sizeof(temp_uint); i++){

				temp_uint <<= 8; /* left shift by one byte */
				temp_uint |= can_buff[index].data[i];

			}
		}

		if(channel == CHANNEL_ERROR || channel == CHANNEL_UPTIME){/* If the channel is error or uptime */

			/* No conversion is necessary, the data format is uint */
			snprintf(str, str_len, "%u", temp_uint);

		}else{/* else the data type is float */

			memcpy(&temp_float, &temp_uint, sizeof(float));/* duplicate data into float */
			snprintf(str, str_len, "%f", temp_float);

		}


	}else{
		snprintf(str, str_len,"NotFound");
	}


}

int CAN_MessagePrintout(int s){


	int nbytes;
	struct can_frame frame;
	uint8_t count = 0;
	for(count = 0; count < 32; count++){

		nbytes = read(s, &frame, sizeof(struct can_frame));

 		if (nbytes < 0) {/* If unsuccessful read */

 			if((errno == EAGAIN) | (errno == EWOULDBLOCK)){ /* If the error is because the operation would block */
 				printf("No more messages in buffer.\r\n");
 			}else{
 				perror("Read");
 				return 1;
 			}


		}else{

			printf("0x%03X [%d] ",frame.can_id, frame.can_dlc);

			int i;
			for (i = 0; i < frame.can_dlc; i++) printf("%02X ",frame.data[i]);

			printf("\r\n");
		}

	}

	return 0;

}

void CAN_NodePrintout(uint8_t nb_nodes, uint8_t * list_nodes){

	uint32_t i;
	printf("%u node(s) found: ",nb_nodes);
	for(i = 0; i < nb_nodes; i++){
		printf("%u", list_nodes[i]);
		if(i == nb_nodes - 2){/* If second to last node */
			printf(" and ");/* print " and " */
		}else if(i == nb_nodes - 1){/* if last node */
			//print nothing
		}else{/* Else */
			printf(", "); /* print comma */
		}
	}
	printf("\n");

}

