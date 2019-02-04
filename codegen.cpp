#include <iostream>
#include <Eigen/Sparse>
#include <igl/AtA_cached.h>

#include "util.h"

using namespace std;

using namespace Eigen;
using namespace igl;


void
codegen(Eigen::SparseMatrix<float> &A)
{
	igl::AtA_cached_data cache;
	Eigen::SparseMatrix<float> AtA(A.rows(), A.cols());

	igl::AtA_cached_precompute(A, cache, AtA);
	printf("void MyMatMul(float *A, float *out) \n"
	       "{                                   \n");

	for (unsigned i = 0; i < cache.I_outer.size() - 1; i++) {
		printf("    out[%d] = 0;\n", i);
		for (unsigned j = cache.I_outer[i]; j < cache.I_outer[i+1]; j++)
			printf("    out[%d]+=A[%d]*A[%d];\n", i, cache.I_row[j], cache.I_col[j]);
	}
	printf("}");
}

void
usage()
{
	printf("usage: g|r <n>\n");
	exit(1);
}

int
main(int argc, char **argv)
{
	srand(1);
	if (argc < 2) {
		usage();
	}

	int n = 1024;
	if (argc >2) {
		n = atoi(argv[2]);
	}

	Eigen::SparseMatrix<float> A(n, n);
	buildSpMatrix(A, 6);
	if (argv[1][0] == 'g') {
	       	codegen(A);
	} 
}
