#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "catalog.h"

typedef struct{
	double lon;
	double lat;
} LOCATION;

double UTC_to_GMST(time_t time_UTC) {
	/*
	 Function for estimating GMST using a unix timestamp of time_t. Source {2}
	 */

	// Terrerstrial time from UTC. 
	time_t time_TAI = time_UTC + 37;
	time_t time_TT = time_TAI + 32.184;

	// Set midNIGHT jan 1 2000
	struct tm time_2000_tm = {0,0,0,1,0,100};
	time_t time_2000_UTC = mktime(&time_2000_tm);


	// Compute julian date UT1 (assumed same as UTC, err ~1 sec) using year 2000 epoch
	double julian_2000 = 2451544.500000;
	double julian_UT1 =  difftime(time_UTC, time_2000_UTC)/(60*60*24) + julian_2000;

	// Same for Terrestrial Time
	double julian_TT = difftime(time_TT, time_2000_UTC)/(60*60*24) + julian_2000;


	// Previous midnight would be something like 1234.5. all values from this to 1235.5 shall be rounded down to 1234.5.
	// Subtract 0.5 from JD. day range shall be 1234.0 - 1235.0 . Apply floor function to compress all decimal values to 1234 and add back 0.5.
	// Here UTC is used as main timekeeping system
	double julian_previous_midnight = floor(julian_UT1 - 0.5) + 0.5;

	double hours_since_previous_midnight = (julian_UT1 - julian_previous_midnight)*24;
	
	// days from J2000: MIDDAY Jan 1 2000
	double days_from_J2000_UT1 = julian_UT1 - 2451545.0;
	double days_from_J2000_TT = julian_TT - 2451545.0;

	double centuries_from_J2000_TT = days_from_J2000_TT/(365.25*100.0);


	// Final calcuation
        double time_GMST = fmod(
			(6.697375
			+ (0.065707485828 * days_from_J2000_UT1)
			+ (1.0027379 * hours_since_previous_midnight)
			+ (0.0854103 * centuries_from_J2000_TT) 
			+ (0.0000258 * centuries_from_J2000_TT * centuries_from_J2000_TT)
			), 24.0);

	return time_GMST;
};

LOCATION trilaterate(LOCATION **GP_list) {
	
	LOCATION position = {0,0};

	return position;
};

LOCATION get_GP(STAR *star, time_t time_UTC) {
	/*
	Function for obtaining GP at a given time

	Takes a pointer for star object and time_t passed directly
	
	returns LOCATION type
	*/

	double GMST = UTC_to_GMST(time_UTC);

	LOCATION GP;

	GP.lon = star->RA - (GMST*360/24);
	GP.lat = star->DEC;

	return GP;
};

int main(int argc, char *argv[]) {
	char timestr[64] = "";
	time_t current_time = 0;

	// Search for t flag. If set use time value given. If not use current system time.
	for (int i = 0; i<argc; i++) {
		if (strcmp(argv[i], "-t") == 0) {
			if (i+1 >= argc) {
				printf("Incorrect Usage: t flag requires value\n");
				return 1;
			};

			struct tm time_tm = {0}; 
			char *strp_return_ptr = strptime(argv[i+1], "%Y-%m-%d-%H:%M", &time_tm);
			time_tm.tm_isdst = -1;

			if (strp_return_ptr == NULL) {
				printf("Incorrect Usage: Use YYYY-mm-dd-HH:MM time format\n");
				return 1;
			}
			else {
				current_time = mktime(&time_tm);
			};
		}
	};

	if (current_time == 0) {
		current_time = time(NULL);
		printf("Using current system time\n");
	};

	int starno = atoi(argv[argc-1]); //argv[argc-1] is last command line argument from OS shell
	
	if (starno >= STAR_NUMBER) {
		printf("Catalog number does not exceed %d\n", STAR_NUMBER);
		return 1;
	};
	
	char starname[20];
	strcpy(starname, catalog[starno].name); // only way to move strings around without SEGFAULT

	STAR star = catalog[starno]; // catalog is an array of stars assigned numerically
        
	LOCATION GP = get_GP(&star, current_time);
	
	double lon = GP.lon;
	double lat = GP.lat;
	
	printf("%s", ctime(&current_time));

	printf("Star %s at %f (+E) %f (+N)\n", starname, lon, lat);
	
	return 0;

};



	

