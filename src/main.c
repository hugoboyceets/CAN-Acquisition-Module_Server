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
#include <time.h>


#include <linux/can.h>
#include <linux/can/raw.h>

#include "CAN_messages.h"
#include "user_io.h"
#include "main.h"



int main(void) {

	/********************************************************************Initiate connection  start *****************************************************/

	int s; // File descriptor for the socket


	CAN_Init(&s); /* Run the initialization procedure, updating s */

	/********************************************************************Initiate connection end *****************************************************/


	    /******************************************************************** User prompt start *****************************************************/
	    /*Prompt user for sample interval, number of samples and measurement type*/
	    int32_t sample_interval_sec, datapoints_to_acquire;
	    printf("Please enter the desired number of samples:\n");
	    prompt_for_number(&datapoints_to_acquire);
	    printf("Please enter the desired sample interval in seconds:\n");
	    prompt_for_number(&sample_interval_sec);

	    measurement_t measurements_table[CAN_MAX_TOTAL_MESSAGES];/* this lists all the meas. that are available on CANbus and that we'll include in our .csv file (in order). */
	    uint32_t nb_of_measurements; /* Number of elements in the measurements_table */
	    CAN_DetectMeasurements(s, measurements_table, &nb_of_measurements);

	    printf("The following measurements were found on the CANbus:\n");
	    print_measurements(measurements_table, nb_of_measurements);

	    printf("Make sure all the desired channels are there, then press ENTER\n");
	    getchar();
	    /******************************************************************** User prompt end *****************************************************/


	    /******************************************************************** File initialization start *****************************************************/
	    /* Generate the filename*/
	    char filename[LONG_STRING_BUFF_LEN];
	    generate_filename(filename);


	    /* Create the file and open it in write mode*/
	    FILE *fptr;
	    fptr = fopen(filename, "w");
	    if (fptr == NULL) {/* If null pointer is returned by fopen because it could not open the file*/
	        printf("Error! Could not open file.");
	        exit(1);
	    }

	    printf("Writing file header...\n");
	    write_file_header(fptr, measurements_table, nb_of_measurements);

	    /******************************************************************** File initialization end *****************************************************/


	    /******************************************************************** Acquisition start *****************************************************/
	    printf("Starting acquisition...\n");

	     time_t time_current;
	     time(&time_current);
	     time_t time_last_sample = 0;

	     int32_t datapoints_acquired = 0;


	     while (datapoints_acquired < datapoints_to_acquire) { /* Main loop that acquires all the points*/
	         datapoints_acquired++;

	        while (!(time_current >= (time_last_sample + sample_interval_sec))) { /*While the time to get a new sample hasn't arrived yet*/
	           time(&time_current); /* Check the time continuously*/
	        }
	        time_last_sample = time_current;

	            /*Once the time for a new sample has arrived...*/
	            /*Append one new data point(time plus all channels) to the file*/
	        fptr = fopen(filename, "a");
	        fprintf(fptr,"%ld,", time_current); /*Print the time. %ld to print long decimal*/
	        //fprintf(fptr,",");

	        CAN_SendSync(s);/* Send a CAN SYNC frame on the specified socket */
	        sleep(CAN_RECEPTION_TIME); /* Sleep for a few seconds */
	        CAN_ReceiveMessages(s); /* Bring all received messages into the buffer */

	        int32_t i;
	        char temp_string[LONG_STRING_BUFF_LEN];
	        for (i = 0; i < nb_of_measurements; i++) {

	             CAN_GetMeasurementValueStr(measurements_table[i].channel, measurements_table[i].node_id, temp_string, LONG_STRING_BUFF_LEN);
	             fprintf(fptr," %s,", temp_string);


	        }
	        fprintf(fptr,"\n");
	        fclose(fptr);

	        /* Print the progress status in the terminal*/
	        printf("\r");
	        printf("%ld/%ld ", (long int)datapoints_acquired, (long int)datapoints_to_acquire);
	        fflush(stdout); /* Printf prints nothing if sleep is called right after. Use fflush to prevent that.*/


	         sleep(sample_interval_sec-1); /*Sleep for a little less than the sample period*/
	    }
	    printf("\n");

	    printf("Acquisition finished...\n");
		//con_close(&con);
		exit(EXIT_SUCCESS);
	    /******************************************************************** Acquisition end *****************************************************/



	return 0;
}
