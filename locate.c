#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multifit.h>
#include <math.h>
#include "astro_math.h"
#include "types.h"

struct location locate(struct observations list) {
	int n = list.n;
	double chisq;

	gsl_matrix * G = gsl_matrix_alloc(n,3);
	gsl_vector * C = gsl_vector_alloc(n);
	gsl_vector * X = gsl_vector_alloc(3);
	gsl_matrix * cov = gsl_matrix_alloc(3,3);
	gsl_multifit_linear_workspace * workspace = gsl_multifit_linear_alloc(n,3);
	

	// Populate matrix G (GP matrix) and vector C (coaltitude vector)
	for (int i=0; i<n; i++) {
		struct location gp = list.obs[i].gp;
		struct unitvec vec = lonlat_to_unitvec(gp);

		gsl_matrix_set(G, i, 0, vec.x);
		gsl_matrix_set(G, i, 1, vec.y);
		gsl_matrix_set(G, i, 2, vec.z);

		double coaltitude = 90 - list.obs[i].altitude;

		double cosine_coaltitude = cos(coaltitude*2*PI/360);

		gsl_vector_set(C, i, cosine_coaltitude);
	};

	gsl_multifit_linear(G, C, X, cov, &chisq, workspace);

	struct unitvec result_vec;

	{
		double x = gsl_vector_get(X, 0);
		double y = gsl_vector_get(X, 1);
		double z = gsl_vector_get(X, 2);

		double norm = sqrt(x*x + y*y + z*z);

		result_vec.x = x/norm;
		result_vec.y = y/norm;
		result_vec.z = z/norm;
	}
	
	struct location result = unitvec_to_lonlat(result_vec);


	gsl_matrix_free(G);
	gsl_vector_free(C);
	gsl_vector_free(X);
	gsl_matrix_free(cov);
	gsl_multifit_linear_free(workspace);

	return result;	
};

