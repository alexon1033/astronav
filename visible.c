#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include "catalog.h"

#define PI 3.14159265359

typedef struct{
	double lon;
	double lat;
} LOCATION;

struct altaz {
	double alt;
	double az;
};

struct degminsec {
	int deg;
	int min;
	int sec;
};

double UTC_to_GMST(time_t time_UTC) {
	/*
	 Function for estimating GMST using a unix timestamp of time_t. Source {2}
	 */

	// Terrerstrial time from UTC. 
	double time_TAI = (double)time_UTC + 37;
	double time_TT = time_TAI + 32.184;

	// Set midNIGHT jan 1 2000
	struct tm time_2000_tm = {
    		.tm_sec  = 0,
    		.tm_min  = 0,
    		.tm_hour = 0,
    		.tm_mday = 1,
    		.tm_mon  = 0,    // January (0-indexed)
    		.tm_year = 100   // years since 1900, so 2000
    	// remaining fields default to 0
	};

	time_t time_2000_UTC = timegm(&time_2000_tm);

	// Compute julian date UT1 (assumed same as UTC, err ~1 sec) using year 2000 epoch
	double julian_2000 = 2451544.500000;
	double julian_UT1 =  (time_TT - (double)time_2000_UTC)/(60*60*24) + julian_2000;

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

struct altaz RA_DEC_to_Alt_Az(time_t time_UTC, STAR *star, LOCATION *location) {
	/*
	 Given a star object, time and location, will find the altitude and azimuth of the given star to the observer. Source {1}.
	*/

	// Extract vars from structs.
	double RA = star->RA*2*PI/360;
	double DEC = star->DEC*2*PI/360;
	double lon = location->lon*2*PI/360;
	double lat = location->lat*2*PI/360;

	double GMST = UTC_to_GMST(time_UTC);
	double LST = fmod((GMST*(2*PI)/24 + lon), 2*PI); // Local Sidereal time
	double HA = LST - RA; // Hour angle
	
	double altitude = asin(sin(lat)*sin(DEC) + cos(lat)*cos(DEC)*cos(HA));
	double azimuth = atan2(sin(HA), cos(HA)*sin(lat) - tan(DEC)*cos(lat));

	altitude = altitude*360/(2*PI);
	azimuth = azimuth*360/(2*PI);
	azimuth = azimuth + 180;

	struct altaz sky_pos = {altitude, azimuth};

	return sky_pos;
};

struct degminsec deg_to_dms(double angle) {
	// Function for turning float in degrees to struct of astronomical degrees-minutes-seconds notation.
	
	double remainder;

	//truncate using explicit cast. Get remainder with fmod.
	int degrees = (int)angle;
	remainder = 60*fmod(angle, 1);
	
	int minutes = (int)remainder;
	remainder = 60*fmod(remainder, 1);

	int seconds = (int)remainder;

	struct degminsec result = {degrees, minutes, seconds};
	return result;
}

int main(int argc, char *argv[]) {
	time_t current_time = 0;

	// Search for t flag. If set use time value given. If not use current system time.
	for (int i = 0; i<argc; i++) {
		if (strcmp(argv[i], "-t") == 0) {
			if (i+1 >= argc) {
				printf("Incorrect Usage: t flag requires value of form: YYYY-mm-dd-HH:MM(:SS)\n");
				return 1;
			};

			struct tm time_tm = {0}; 
			char *strp_return_ptr = strptime(argv[i+1], "%Y-%m-%d-%H:%M:%S", &time_tm);
			time_tm.tm_isdst = -1;

			if (strp_return_ptr == NULL) {
				char *strp_return_ptr = strptime(argv[i+1], "%Y-%m-%d-%H:%M:%S", &time_tm);
				time_tm.tm_isdst = -1;

				if (strp_return_ptr == NULL) {
					printf("Incorrect Usage: Use YYYY-mm-dd-HH:MM(:SS) time format\n");
					return 1;
				}
			}
			
			current_time = mktime(&time_tm);
		}
	};

	if (current_time == 0) {
		current_time = time(NULL);
		// printf("Using current system time\n");
	};


	LOCATION location = {0.972261, 51.16834};

	char starid[20]; 
	strcpy(starid, argv[argc-1]); //argv[argc-1] is last command line argument from OS shell
	int starno = -1;

	// make lowercase
	for (int i = 0; starid[i]; i++) {
		starid[i] = tolower(starid[i]);
	};

	// Check for name in catalog
	for (int i = 0; i<STAR_NUMBER; i++) {
		if (strcmp(catalog[i].name, starid) == 0) {
			starno = i;
		};
	};

	// If not name then try number.
	if (starno == -1) {
		// check for invalid string for number conversion. 
		if (atoi(starid) == 0 && strcmp(starid, "0") != 0) {
			printf("Star not found. Give common star name or catalog number from 0 to %d\n", STAR_NUMBER);
			return 1;
		};

		starno = atoi(starid);
	};


	// Failure mode
	if (starno >= STAR_NUMBER) {
		printf("Star number exceeds catalog size. Maximum value: %d\n", STAR_NUMBER); 
		return 1;
	};

	STAR star = catalog[starno];
	// Conversion to local coordinates
	struct altaz sky_pos = RA_DEC_to_Alt_Az(current_time, &star, &location);

	// conversion to dms for readability
	struct degminsec altitude = deg_to_dms(sky_pos.alt);
	struct degminsec azimuth = deg_to_dms(sky_pos.az);


	printf("Star: %s\n", catalog[starno].name);
	printf("Altitude: %d\u00B0 %d\u2032 %d\u2033\n", altitude.deg, altitude.min, altitude.sec);	
	printf("Azimuth: %d\u00B0 %d\u2032 %d\u2033\n", azimuth.deg, azimuth.min, azimuth.sec);

	
	if (sky_pos.alt > 15) {
		printf("Visible: Yes\n");
	} else if (sky_pos.alt < 0) {
		printf("Visible: No\n");
	} else {
		printf("Visiblity: Poor\n");
	};


	return 0;

};



	

