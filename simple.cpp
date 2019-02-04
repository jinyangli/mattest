#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <time.h>
#include <unistd.h>
#include <igl/AtA_cached.h>
#include <igl/Timer.h>
#include <sys/types.h>

#include "util.h"

//#include "mysparse.h"

using namespace Eigen;
using namespace igl;

const int nonzeros_per_col = 6;

bool verbose = false;

void MyMatMul(float *A, float *out);

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

	MatrixXf m1 = MatrixXf::Zero(n, n);
	MatrixXf m2 = MatrixXf::Zero(n, n);
	buildDenseMatrix(m1);
	buildDenseMatrix(m2);

       	igl::Timer t;	
	t.start();
	MatrixXf m = m1 * m2;
	t.stop();

	std::cout << "Dense MatMul timing: " << t.getElapsedTimeInMicroSec() << " us"  << " m[0,0]=" << m(0,0) << std::endl;

}

void
SparseMatMul(int n)
{
	Eigen::SparseMatrix<float> A(n,n); 
	Eigen::SparseMatrix<float, RowMajor> At;

	Eigen::SparseMatrix<float> AtA(n,n);
       	buildSpMatrix(A, nonzeros_per_col);
	igl::Timer t;

	t.start();
	At = A.transpose();
	AtA = A * At;
	t.stop();

	std::cout << "Sparse MatMul timing: " << t.getElapsedTimeInMicroSec() << " us"   << " A nonzeros " << A.nonZeros() << " AtA nonzeros " << AtA.nonZeros() << std::endl;
	std::cout << "Sparse MatMul A " << A.outerSize() << " x " << A.innerSize() << std::endl;
	std::cout << "Sparse MatMul At " << At.outerSize() << " x " << At.innerSize() << std::endl;
	std::cout << "Sparse MatMul AtA " << AtA.outerSize() << " x " << AtA.innerSize() << std::endl;


}

void
CodeGenMatMul(int n)
{
	Eigen::SparseMatrix<float> A(n,n); 
	Eigen::SparseMatrix<float> AtA(n,n); 
	igl::AtA_cached_data cache;
	buildSpMatrix(A, nonzeros_per_col);
	A.makeCompressed();
	//this is just used to pre-allocate the space needed for AtA
	igl::AtA_cached_precompute(A, cache, AtA);

	std::cout << "A.valuePtr() " << A.valuePtr() << " AtA.valuePtr() " << AtA.valuePtr() << std::endl;
	igl::Timer t;
	t.start();
	MyMatMul(A.valuePtr(), AtA.valuePtr());
	t.stop();

	std::cout << "CodeGenMatMul timing: " << t.getElapsedTimeInMicroSec() << " us"   << " A nonzeros " << A.nonZeros() << " AtA nonzeros " << AtA.nonZeros() << std::endl;
}

void
AtAMatMul(int n)
{
	Eigen::SparseMatrix<float> A(n, n);
	Eigen::SparseMatrix<float> AtA(n, n);
	std::cout << "AtAMatMul A size " << A.outerSize() << " x " << A.innerSize() << std::endl;
	std::cout << "AtAMatMul A cols " << A.cols() << " rows " << A.rows() << std::endl;
       	buildSpMatrix(A, nonzeros_per_col);
	if (verbose) 
		std::cout << "AtAMatMul A " << A << std::endl;

	A.makeCompressed();
	igl::AtA_cached_data cache;

	igl::Timer t;
	t.start();
	igl::AtA_cached_precompute(A, cache, AtA);
	t.stop();

	std::cout << "AtA pre-compute timing: " << t.getElapsedTimeInMicroSec() << " us"   << " A nonzeros " << A.nonZeros() << " AtA nonzeros " << AtA.nonZeros() << std::endl;

	assert(AtA.isCompressed());

	t.start();
	igl::AtA_cached(A, cache, AtA);
	t.stop();
	std::cout << "AtA cached timing: " << t.getElapsedTimeInMicroSec() << " us"   << " A nonzeros " << A.nonZeros() << " AtA nonzeros " << AtA.nonZeros() << std::endl;
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("usage: c|s|g <n>\n");
		exit(1);
	}
	srand(1);
	int n = 1024;
	if (argc >2) {
		n = atoi(argv[2]);
	}
	std::cout << "matrix size: " << n << std::endl;

       	assert(n >= 2*nonzeros_per_col);

/*	DenseMatMul(n);	
	std::cout << "-----------" << std::endl;
	*/
	if (argv[1][0] == 's') {
		std::cout << "-----------" << std::endl;
		SparseMatMul(n);
	} else if (argv[1][0] == 'c' ) {
		std::cout << "-----------" << std::endl;
		AtAMatMul(n);
	} else if  (argv[1][0] == 'g' ) {
		std::cout << "-----------" << std::endl;
		CodeGenMatMul(n);
	}
}

