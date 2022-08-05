#ifndef _USER_IO_H_
#define _USER_IO_H_

#include <time.h>

#include "main.h"
#include "CAN_messages.h"


#define ABSOLUTE_PATH  "/home/hugo/eclipse-workspace/CAN-Acquisition-Module_Server/Data" /*Absolute path!*/

void print_measurements(measurement_t *, uint8_t);
int32_t prompt_for_number(int32_t *);
void generate_filename(char *);
void write_file_header(FILE *, measurement_t *, uint8_t);

#endif
