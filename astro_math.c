#include <time.h>
#include <math.h>
#include "catalog.h"
#include "astro_math.h"
#include "types.h"

#define PI 3.14159265359

struct unitvec lonlat_to_unitvec(struct location loc) {
	// convert longditude and latitude to a vector of length magnitude 1 

	struct unitvec vector;

	vector.x = cos(2*PI/360*loc.lat) * cos(2*PI/360*loc.lon);
	vector.y = cos(2*PI/360*loc.lat) * sin(2*PI/360*loc.lon);
	vector.z = sin(2*PI/360*loc.lat);

	return vector;
};

struct location unitvec_to_lonlat(struct unitvec vec) {
	// Convert unit vector to longditude and latitude. 
	struct location loc;

	loc.lon = 360/(2*PI)*atan2(vec.y, vec.x);
	loc.lat = 360/(2*PI)*asin(vec.z);

	return loc;
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
    		.tm_year = 100, // years since 1900, so 2000
		.tm_isdst = 0
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


struct location get_GP(STAR *star, time_t time_UTC) {
	/*
	Function for obtaining GP at a given time

	Takes a pointer for star object and time_t passed directly
	
	returns struct location
	*/

	double GMST = UTC_to_GMST(time_UTC);

	struct location GP;

	GP.lon = fmod(star->RA - (GMST * 360.0 / 24.0), 360.0);
	if (GP.lon >  180.0) GP.lon -= 360.0;
	if (GP.lon < -180.0) GP.lon += 360.0;

	GP.lat = star->DEC;

	return GP;
};




