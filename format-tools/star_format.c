#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	if (argc == 9) {
		double RA = (atof(argv[3]) + atof(argv[4])/60 + atof(argv[5])/(60*60))*360/24;
		double DEC = atof(argv[6]) + atof(argv[7])/60 + atof(argv[8])/(60*60);

		printf("\t{\"%s\", %f, %f},\n", argv[2], RA, DEC);
	}
	else {
		printf("Inccorect format for conversion\n");
	};

	return 0;
};
