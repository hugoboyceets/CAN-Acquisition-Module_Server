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
#include <errno.h>


#include <linux/can.h>
#include <linux/can/raw.h>

#include "CAN_messages.h"
#include "user_io.h"
#include "main.h"

#define NUMBER_OF_ARGS 4 /* There should be 3 args plus the program call (= 4) */

#define MIN_SAMPLE_INTERVAL 1 		/* Minimum sample interval in seconds */
#define MAX_SAMPLE_INTERVAL 31536000	/* Max sample interval in seconds (one year, arbitrary) */

bool newDay(time_t);

int main(int argc, char *argv[]) {
/*
 * First argument should be the can port (usually "can0")
 *
 * Second argument should be the desired sampling interval in seconds
 *  ex.: "1","15","60","3600", etc
 *
 * Third argument should be the path to the folder in which the program will place the output files
 *	ex.:"/home/hugo/Documents/AcqData"
 *
*/

	if(argc != NUMBER_OF_ARGS){
		perror("Wrong number of arguments (there should be 3)");
		return EXIT_FAILURE;
	}

	/********************************************************************Initiate connection  start *****************************************************/

	int s; // File descriptor for the socket



	int ret;
	ret = CAN_Init(&s, argv[1]); /* Run the initialization procedure, updating s (the connection handle) and passing
	the interface name argument*/
	if(ret != EXIT_SUCCESS){ //If exit code is not null
		return ret; // Exit the program and return the code returned by CAN_Init()
	}

	// Try to write some bytes over the connection
	ret = CAN_SendSync(s);
	if(ret <= 0){ // If the number of written bytes is negative or null
		perror("Could not write bytes over the CAN connection");
		puts("Is the CAN adapter plugged in?");
		return EXIT_FAILURE;
	}

	/********************************************************************Initiate connection end *****************************************************/


	/******************************************************************** Sample intervel start *****************************************************/
	int32_t sample_interval_sec = 0;
	/*Prompt user for sample interval, number of samples and measurement type*/
	//printf("Please enter the desired sample interval in seconds:\n");
	//prompt_for_number(&sample_interval_sec);


	if(argv[2] != NULL){
		sscanf(argv[2],"%i",&sample_interval_sec);

		if(sample_interval_sec < MIN_SAMPLE_INTERVAL || sample_interval_sec > MAX_SAMPLE_INTERVAL){
			perror("Invalid sample interval");
			return EXIT_FAILURE;
		}

		printf("Sample interval: %i seconds.\n", sample_interval_sec);

	}else{
		perror("No sample interval given.");
		return EXIT_FAILURE;
	}

	/******************************************************************** Sample interval end *****************************************************/

	/* File info */
	char path_and_filename[LONG_STRING_BUFF_LEN];/* 1024 characters limit on the file path, enforced by snprintf() */
	FILE *fptr;

	measurement_t measurements_table[CAN_MAX_TOTAL_MESSAGES];/* this lists all the meas. that are available on CANbus and that we'll include in our .csv file (in order). */
	uint32_t nb_of_measurements; /* Number of elements in the measurements_table */


	time_t time_current;
	time(&time_current);
	time_t time_last_sample = 0;

	struct tm *localtime_ptr;


	bool newFile = true;/* Flag to start a new file */

	uint32_t datapoints_acquired = 0;


	while(1){ //never returns


		while (!(time_current >= (time_last_sample + sample_interval_sec))) { /*While the time to get a new sample hasn't arrived yet*/
		   time(&time_current); /* Check the time continuously*/
		}

		if(newDay(time_current)){/* If a new day has started */
			newFile = true; /* Start a new file */
		}


		if(newFile){
			newFile = false;

			CAN_DetectMeasurements(s, measurements_table, &nb_of_measurements);/* Detect measurements available on the CANbus*/

			/******************************************************************** File initialization start *****************************************************/
			/* Generate the filename*/
			generate_full_filename(path_and_filename, argv[3], LONG_STRING_BUFF_LEN);


			/* Create the file and open it in write mode*/
			fptr = fopen(path_and_filename, "w");
			if (fptr == NULL) {/* If null pointer is returned by fopen because it could not open the file*/
				perror("Error! Could not open file");
				free(path_and_filename);
				return EXIT_FAILURE;
			}

			//printf("Writing file header...\n");
			write_file_header(fptr, measurements_table, nb_of_measurements);

			/******************************************************************** File initialization end *****************************************************/


			/******************************************************************** Acquisition start *****************************************************/
			printf("New file started at:\n");
			printf("%s\n", path_and_filename);

		}


		time(&time_current); /* update time */
		time_last_sample = time_current;

		localtime_ptr = localtime(&time_current); /* update the localtime (human readable) time structure */


		/*Once the time for a new sample has arrived...*/
		/*Append one new data point(time plus all channels) to the file*/
		fptr = fopen(path_and_filename, "a");
		/*Print the Unix time (time_current as %ld) and the human-readable local time */
		fprintf(fptr,"%ld, %04d-%02d-%02d_%02dh%02dm%02ds,", time_current, localtime_ptr->tm_year+1900, localtime_ptr->tm_mon+1, localtime_ptr->tm_mday, localtime_ptr->tm_hour, localtime_ptr->tm_min, localtime_ptr->tm_sec);


		CAN_SendSync(s);/* Send a CAN SYNC frame on the specified socket */
		sleep(CAN_RECEPTION_TIME); /* Sleep for a second */
		CAN_ReceiveMessages(s); /* Bring all received messages into the buffer */

		int32_t i;
		char temp_string[STRING_BUFF_LEN];
		for (i = 0; i < nb_of_measurements; i++) {

			 CAN_GetMeasurementValueStr(measurements_table[i].channel, measurements_table[i].node_id, temp_string, STRING_BUFF_LEN);
			 fprintf(fptr," %s,", temp_string);

		}

		fprintf(fptr,"\n");
		fclose(fptr);

		/* Print the progress status in the terminal*/
		datapoints_acquired++;
		printf("\r");
		printf("Acq. Number: %ld ", (long int)datapoints_acquired);
		fflush(stdout); /* Printf prints nothing if sleep is called right after. Use fflush to prevent that.*/
		sleep(sample_interval_sec-1); /*Sleep for a little less than the sample period*/

	}

	// The 2 lines below should never execute because of while(1)
	free(path_and_filename);
	return EXIT_SUCCESS;

}

bool newDay(time_t time_current){

	static time_t time_last = 0;
	struct tm tm_last;
	localtime_r(&time_last, &tm_last);


	struct tm tm_current;
	localtime_r(&time_current, &tm_current);
	//printf("Current time is: %u, last time is: %u\n", time_current, time_last);
	//printf("current hour is %u, last hour is %u\n", tm_current.tm_hour, tm_last.tm_hour);

	if(tm_current.tm_hour < tm_last.tm_hour){/* If the hours have gotten smaller (overflowed) since the last measurement */
		time_last = time_current;
		return true;/* Return true */

	}else{
		time_last = time_current;
		return false;
	}



}
