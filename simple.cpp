#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <time.h>
#include <igl/AtA_cached.h>

//#include "mysparse.h"

using namespace Eigen;
using namespace igl;

typedef Eigen::Triplet<float> T;
typedef Eigen::SparseMatrix<float> SpMat;

const int nonzeros_per_col = 6;

bool verbose = false;

long long
timediff(struct timespec ts, struct timespec ts0)
{
        return (ts.tv_sec-ts0.tv_sec)*1000000+(ts.tv_nsec-ts0.tv_nsec)/1000;
}

void
buildDenseMatrix(MatrixXf &mat)
{
	for (int i = 0; i < mat.outerSize(); i++) {
		for (int j = 0; j < nonzeros_per_col; j++) {
			int ind = rand()%mat.innerSize();
			mat(i,ind) = (double) rand() / (RAND_MAX);
		}
	}
}

void
DenseMatMul(int n)
{
	std::cout << "matrix size: " << n << std::endl;

	MatrixXf m1 = MatrixXf::Zero(n, n);
	MatrixXf m2 = MatrixXf::Zero(n, n);
	buildDenseMatrix(m1);
	buildDenseMatrix(m2);
 
	struct timespec ts1, ts2;
        clock_gettime(CLOCK_REALTIME, &ts1);

	MatrixXf m = m1 * m2;

        clock_gettime(CLOCK_REALTIME, &ts2);

	std::cout << "Dense MatMul timing: " << timediff(ts2,ts1) << " us"  << " m[0,0]=" << m(0,0) << std::endl;

}

void buildSpMatrix(SpMat &mat)
{
	std::vector<T> coeff;
	for (int i = 0; i < mat.outerSize(); i++) {
		for (int j = 0; j < nonzeros_per_col; j++) {
			coeff.push_back(T(rand()%mat.innerSize(), i, (double) rand() / (RAND_MAX)));
		}
	}
	mat.setFromTriplets(coeff.begin(), coeff.end());
	std::cout << "built sparse matrix " << mat.outerSize() << " x " << mat.innerSize() << std::endl;
}

void
SparseMatMul(int n)
{
	SpMat A(n,n); 
	Eigen::SparseMatrix<float, RowMajor> At;

	SpMat AtA(n,n);
       	buildSpMatrix(A);
	struct timespec ts1, ts2;
        clock_gettime(CLOCK_REALTIME, &ts1);

	At = A.transpose();
	AtA = A * At;

	clock_gettime(CLOCK_REALTIME, &ts2);
	std::cout << "Sparse MatMul timing: " << timediff(ts2,ts1) << " us"   << " A nonzeros " << A.nonZeros() << " AtA nonzeros " << AtA.nonZeros() << std::endl;
	std::cout << "Sparse MatMul A " << A.outerSize() << " x " << A.innerSize() << std::endl;
	std::cout << "Sparse MatMul At " << At.outerSize() << " x " << At.innerSize() << std::endl;
	std::cout << "Sparse MatMul AtA " << AtA.outerSize() << " x " << AtA.innerSize() << std::endl;


}
/*
void
MyAtAMatMul(int n)
{
	SpMat A(n,n); 
	Eigen::SparseMatrix<float, RowMajor> At;

	SpMat AtA(n,n);
       	buildSpMatrix(A);
	struct timespec ts1, ts2;
	At = A.transpose();

        clock_gettime(CLOCK_REALTIME, &ts1);
	MySparseProduct(A, At, AtA);

	clock_gettime(CLOCK_REALTIME, &ts2);
	std::cout << "Sparse MatMul timing: " << timediff(ts2,ts1) << " us"   << " A nonzeros " << A.nonZeros() << " AtA nonzeros " << AtA.nonZeros() << std::endl;
	std::cout << "Sparse MatMul A " << A.outerSize() << " x " << A.innerSize() << std::endl;
	std::cout << "Sparse MatMul At " << At.outerSize() << " x " << At.innerSize() << std::endl;
	std::cout << "Sparse MatMul AtA " << AtA.outerSize() << " x " << AtA.innerSize() << std::endl;
}
*/
void
AtAMatMul(int n)
{
	SpMat A(n, n);
	SpMat AtA(n, n);
	std::cout << "AtAMatMul A size " << A.outerSize() << " x " << A.innerSize() << std::endl;
	std::cout << "AtAMatMul A cols " << A.cols() << " rows " << A.rows() << std::endl;
       	buildSpMatrix(A);
	if (verbose) 
		std::cout << "AtAMatMul A " << A << std::endl;

	A.makeCompressed();
	igl::AtA_cached_data cache;

	struct timespec ts1, ts2;
        clock_gettime(CLOCK_REALTIME, &ts1);
	igl::AtA_cached_precompute(A, cache, AtA);
	clock_gettime(CLOCK_REALTIME, &ts2);
	std::cout << "AtA (with pre-compute) timing: " << timediff(ts2,ts1) << " us"   << " A nonzeros " << A.nonZeros() << " AtA nonzeros " << AtA.nonZeros() << std::endl;

	assert(AtA.isCompressed());

        clock_gettime(CLOCK_REALTIME, &ts1);
	igl::AtA_cached(A, cache, AtA);
	clock_gettime(CLOCK_REALTIME, &ts2);
	std::cout << "AtA (w/o pre-compute) timing: " << timediff(ts2,ts1) << " us"   << " A nonzeros " << A.nonZeros() << " AtA nonzeros " << AtA.nonZeros() << std::endl;
}

int main(int argc, char **argv)
{
	int n = 1024;
	if (argc >2) {
		n = atoi(argv[2]);
	}
	if (argc < 2) {
		printf("usage: a|c|s <n>\n");
		exit(1);
	}
       	assert(n >= 2*nonzeros_per_col);

/*	DenseMatMul(n);	
	std::cout << "-----------" << std::endl;
	*/
	if (argv[1][0] == 's' || argv[1][0] == 'a') {
		SparseMatMul(n);
		std::cout << "-----------" << std::endl;
	} 
	if (argv[1][0] == 'c' || argv[1][0] == 'a' ) {
		AtAMatMul(n);
		std::cout << "-----------" << std::endl;
	} 
	//MyAtAMatMul(n);
}

