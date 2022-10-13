#ifndef _USER_IO_H_
#define _USER_IO_H_

#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
//#include <struct_tm.h>

#include "main.h"
#include "CAN_messages.h"


//#define ABSOLUTE_PATH  "/home/hugo/Documents/AcqData" /*Absolute path!*/

//void print_measurements(measurement_t *, uint8_t);
//int32_t prompt_for_number(int32_t *);
void generate_full_filename(char *, char *, uint32_t);
void write_file_header(FILE *, measurement_t *, uint8_t);

#endif
