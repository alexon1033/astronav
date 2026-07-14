#ifndef ASTRO_MATH_H
#define ASTRO_MATH_H

#include "types.h"

struct unitvec lonlat_to_unitvec(struct location loc);

struct location unitvec_to_lonlat(struct unitvec vec);

double UTC_to_GMST(time_t time_UTC);

struct location get_GP(STAR *star, time_t time_UTC);

#endif
