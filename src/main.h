/*
 * main.h
 *
 *  Created on: Aug. 3, 2022
 *      Author: hugo
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdbool.h>

#define STRING_BUFF_LEN 512


typedef enum {
    CHANNEL_ANALOG_0 = 0,
	CHANNEL_ANALOG_1 = 1,
	CHANNEL_ANALOG_2 = 2,
	CHANNEL_ANALOG_3 = 3,
	CHANNEL_ANALOG_4 = 4,
	CHANNEL_ANALOG_5 = 5,
	CHANNEL_ANALOG_6 = 6,
	CHANNEL_ANALOG_7 = 7,
	CHANNEL_ANALOG_8 = 8,
	CHANNEL_ANALOG_9 = 9,
	CHANNEL_ANALOG_10 = 10,
	CHANNEL_ANALOG_11 = 11,
	CHANNEL_ENV_TEMP = 12,
	CHANNEL_ENV_HUM = 13,
	CHANNEL_ENV_CO2 = 14,
	CHANNEL_UPTIME = 15,
	CHANNEL_ERROR = 16
}channel_t;

typedef struct {

    char channel_str[STRING_BUFF_LEN];
    channel_t channel;
    uint8_t id;
    uint32_t data; /* 32 bits of data. May actually be a float, an int, a uint, etc... */

}measurement_t;

#endif /* MAIN_H_ */
