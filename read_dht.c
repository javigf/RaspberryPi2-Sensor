/*
 *  dht.c:
 *	read temperature and humidity from DHT11 or DHT22 sensor
 *
 *  sudo gcc -o dht22 dht22.c -L/usr/local/lib -lwiringPi
 
 */
#define DHT_VER 0.2
#include "read_dht.h"

float temperature, humidity;
 
int read_dht_data (uint8_t DHT_PIN) {
	uint8_t laststate	= HIGH;
	uint8_t counter		= 0;
	uint8_t j			= 0, i;
	

	data[0] = data[1] = data[2] = data[3] = data[4] = 0;

	/* pull pin down for 18 milliseconds */
	pinMode( DHT_PIN, OUTPUT );
	digitalWrite( DHT_PIN, LOW );
	delay( 18 );

	/* prepare to read the pin */
	pinMode( DHT_PIN, INPUT );

	/* detect change and read data */
	for ( i = 0; i < MAX_TIMINGS; i++ ){
		counter = 0;
		while ( digitalRead( DHT_PIN ) == laststate ){
			counter++;
			delayMicroseconds( 1 );
			
			if ( counter == 255 ){
				break;
			}
		}
		laststate = digitalRead( DHT_PIN );

		if ( counter == 255 )
			break;

		/* ignore first 3 transitions */
		if ((i >= 4) && (i % 2 == 0)){
			/* shove each bit into the storage bytes */
			data[j / 8] <<= 1;
			if ( counter > 16 )
				data[j / 8] |= 1;
			j++;
		}
	}

	/*
	 * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
	 * print it out if data is good
	 */
	if ((j >= 40) && (data[4] == ( (data[0] + data[1] + data[2] + data[3]) & 0xFF))){
		humidity = (float)((data[0] << 8) + data[1]) / 10;
		
		
		if ( humidity > 100 ){
			humidity = data[0];	// for DHT11
			DHTsensorType = TRUE;
		}
		temperature = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10;
		if ( temperature > 125 ){
			temperature = data[2];	// for DHT11
		}
		if ( data[2] & 0x80 ){
			temperature = -temperature;
		}		
		float f = temperature * 1.8f + 32;

	}
	else{
		//printf( "DHT Data not good, skip\n" );
		return 1;
	}
	return 0;
}
