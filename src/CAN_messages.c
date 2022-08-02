/*
 * CAN_messages.c
 *
 *  Created on: Aug. 2, 2022
 *      Author: hugo
 */
#include "CAN_messages.h"




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

int CAN_ReceiveMessages(int s){


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



