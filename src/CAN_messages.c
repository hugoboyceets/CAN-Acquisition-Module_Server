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

		printf("%s at index %d\n", ifname, ifr.ifr_ifindex);

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

	printf("Wrote %d bytes\n", nbytes);

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

uint32_t CAN_DetectMeasurements(int s/*, measurement_t * measurements_table*/){

	uint32_t nb_measurements = 0;


	CAN_SendSync(s);/* Send a CAN SYNC frame on the specified socket */
	sleep(1); /* Sleep for 2 seconds */
	CAN_ReceiveMessages(s);


	return nb_measurements;
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

