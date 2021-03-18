#ifndef MAIN_H
#define MAIN_H

#include "pico/mutex.h"

#define DEBUG 1

#define CORE_INIT_FLAG 69

#define LED_PIN 25
#define LOW_POWER_ALTITUDE 2000

//SPI Constants
#define SPI_PORT_0 spi0
#define MISO_0 16
#define SCLK_0 18
#define MOSI_0 19

#define CS_BMP   17

static mutex_t mtx;

typedef enum {fmIdle, fmLaunched, fmDescending, fmLanding, fmLanded} TFlightMode;
static struct STATE
{
    // Current state of the payload
    // READING AND WRITING OPERATIONS MUST BE MUTEX'd TO REMAIN THREADSAFE
    long Time;							// Time as read from GPS, as an integer but 12:13:14 is 121314
	long SecondsInDay;					// Time in seconds since midnight.  Used for APRS timing, and LoRa timing in TDM mode
	int Hours, Minutes, Seconds;
	float Longitude, Latitude;
	long Altitude, MinimumAltitude, MaximumAltitude, PreviousAltitude;
	unsigned int Satellites;
	int Speed;
	int Direction;
	float AscentRate;
	float BatteryVoltage;
	float InternalTemperature;
	signed long ExternalTemperature;
	unsigned long Pressure;
	TFlightMode FlightMode;
	float PredictedLongitude, PredictedLatitude;
	float CDA;
	int UseHostPosition;
	int TimeTillLanding;
	float PredictedLandingSpeed;
} state;



void core_entry();
void check_LED(struct STATE *s);
void check_BMP(struct STATE *s);

#endif