#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include "catalog.h"
#include "astro_math.h"
#include "locate.h"
#include "params.h"



double dms_to_deg(char *dms_string) {
	char *colon1 = strchr(dms_string, ':');
	
	if (colon1 == NULL) return -1;

	char *colon2 = strchr(colon1+1, ':');

	if (colon2 == NULL) return -1;

	double degrees = atof(dms_string);
	double minutes = atof(colon1+1);
	double seconds = atof(colon2+1);

	double degrees_total = degrees + minutes/60 + seconds/(60*60);

	return degrees_total;
};

int get_STAR(char *starname, STAR *star_ptr) {
	int star_index = -1;

	for (int i = 0; i<STAR_NUMBER; i++) {
		if (strcmp(catalog[i].name, starname) == 0) star_index = i;
	}

	if (star_index == -1) return 1;

	*star_ptr = catalog[star_index];
	return 0;	
}

struct observations fileread(char *filename) {
	
	struct observations observe_struct = {0};
	
	FILE *fptr = fopen(filename,"r");

	char line[50];
	int line_number = 0;

	while (fgets(line, sizeof(line), fptr) != NULL && line_number < MAX_OBSERVATIONS) {
		line_number++;

		char *comma1 = strchr(line, ',');

		if (comma1 == NULL) {
			printf("Warning: Incorrect formatting on line %d\nSkipping...\n", line_number);

			continue;
		}

		char *comma2 = strchr(comma1+1, ',');

		if (comma2 == NULL) {
			printf("Warning: Incorrect formatting on line %d\nSkipping...\n", line_number);
			continue;
		}

		char *newline = strchr(comma2, '\n');

		*comma1 = '\0';
		*comma2 = '\0';
		if (newline) *newline = '\0';

		for (char * ptr = comma1+1; ptr<comma2+1; ptr++) {*ptr = tolower(*ptr);}

		char *timestring = line;
		char *starname = comma1 + 1;
		char *dmsstring = comma2 + 1;


		struct tm time_tm = {0};
		char* strp_return_ptr = strptime(timestring, "%Y-%m-%d-%H:%M:%S", &time_tm);
		time_tm.tm_isdst = 0;

		if (strp_return_ptr == NULL) {
			printf("Incorrect time formatting in line %d. Use YYYY-mm-dd-HH:MM:SS. Skipping...\n", line_number);
			continue;
		}

		time_t time = timegm(&time_tm);
		double altitude = dms_to_deg(dmsstring);

		STAR star;

		if (get_STAR(starname, &star)) {
			printf("Star \"%s\" not found. Skipping...\n", starname);
			continue;
		}

		observe_struct.obs[line_number].altitude = dms_to_deg(dmsstring);
		observe_struct.obs[line_number].gp = get_GP(&star, time);
	};

	observe_struct.n = line_number;

	return observe_struct;
};


int main(int argc, char **argv) {
	char filename[50];
	strcpy(filename, argv[1]);

	struct observations obs = fileread(filename);
	
};



	

