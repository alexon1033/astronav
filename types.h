#ifndef TYPES_H
#define TYPES_H

#include "params.h"

struct location{
	double lon;
	double lat;
};

struct unitvec{
	double x;
	double y;
	double z;
};

typedef struct {
	char name[20];
	double RA;
	double DEC;
} STAR;

struct observation{
	double altitude;
	struct location gp;
};

struct observations{
	int n;
	struct observation obs[MAX_OBSERVATIONS];
};



#endif

