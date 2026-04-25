#ifndef STARCATALOG_H
#define STARCATALOG_H

typedef struct {
	char name[20];
	double RA;
	double DEC;
} STAR;

#define STAR_NUMBER 58

extern const STAR catalog[STAR_NUMBER];

#endif
