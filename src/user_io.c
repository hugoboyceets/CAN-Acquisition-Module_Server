
#include "user_io.h"
#include "main.h"




//void print_measurements(measurement_t *measurements_table, uint8_t nb_of_measurements){
//
//    uint8_t count = 0;
//    char temp_string[SHORT_STRING_BUFF_LEN];
//
//    while(count < nb_of_measurements){
//
//    	CAN_GetMeasurementNameStr(measurements_table[count].channel, measurements_table[count].node_id, temp_string, SHORT_STRING_BUFF_LEN);
//        printf("%s\n", temp_string);
//        count++;
//
//
//    }
//
//}

//int32_t prompt_for_number(int32_t *number){
//
//
//    /*Copied from: http://sekrit.de/webdocs/c/beginners-guide-away-from-scanf.html */
//    /* Thank you Felix Palmen*/
//
//    char buf[SHORT_STRING_BUFF_LEN]; // use big buffer
//    int32_t success; // flag for successful conversion
//
//    do
//    {
//
//        if (!fgets(buf, SHORT_STRING_BUFF_LEN, stdin))
//        {
//            // reading input failed:
//            return 1;
//        }
//
//        // have some input, convert it to integer:
//        char *endptr;
//
//        errno = 0; // reset error number
//        *number = strtol(buf, &endptr, 10);
//        if (errno == ERANGE)
//        {
//            printf("Sorry, this number is too small or too large.\n");
//            success = 0;
//        }
//        else if (endptr == buf)
//        {
//            // no character was read
//            success = 0;
//        }
//        else if (*number < 0)
//        {
//            printf("Sorry, this number is negative.\n");
//            success = 0;
//        }
//        else if (*endptr && *endptr != '\n')
//        {
//            // *endptr is neither end of string nor newline,
//            // so we didn't convert the *whole* input
//            success = 0;
//        }
//        else
//        {
//            success = 1;
//        }
//    } while (!success); // repeat until we got a valid number
//
//    return 0;
//}




void generate_full_filename(char* full_filename, char* path, uint32_t len){

    time_t rawtime = time(NULL);

    if (rawtime == -1) {

        puts("The time() function failed");

    }

	struct tm *ptm = localtime(&rawtime);

    if (ptm == NULL) {

        puts("The localtime() function failed");
    }

    snprintf(full_filename, len, "%s/Data_Acquisition_%04d-%02d-%02d_%02dh%02dm%02ds.csv", path, ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

    //printf("%s\n", full_filename);

}

void write_file_header(FILE *fptr, measurement_t *measurements_table, uint8_t nb_of_measurements){

    char temp_string[LONG_STRING_BUFF_LEN];
    uint8_t i;
     /* Print a list of the channels */
    fprintf(fptr,"Unix time, Local time");
    for(i = 0; i < nb_of_measurements;i++){// For every measurement

    	CAN_GetMeasurementNameStr(measurements_table[i].channel, measurements_table[i].node_id, temp_string, LONG_STRING_BUFF_LEN);/* Get the channel name string */
    	fprintf(fptr, ",%s", temp_string);/* Print the channel name in the file */

    }
    fprintf(fptr,"\n");


    fclose(fptr);

}
