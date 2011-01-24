#pragma once

#include "LinearSolver.h"
#include "CompactMatrix.h"
#include <vector>
using namespace std;

namespace FECore {
//-----------------------------------------------------------------------------
//! this class implements an iterative conjugate gradient solver 
class ConjGradIterSolver : public LinearSolver
{
public:
	ConjGradIterSolver();

	bool PreProcess();
	bool Factor();
	bool Solve(vector<double>& x, vector<double>& b);

	void Destroy();

	SparseMatrix* CreateSparseMatrix(Matrix_Type ntype) { return (m_pA = (ntype == SPARSE_SYMMETRIC? new CompactSymmMatrix() : 0)); }

public:
	double	m_tol;		// convergence tolerance
	int		m_kmax;		// max iterations
	int		m_nprint;	// printing level

	vector<double>	m_P;	// preconditioning vector
};

} // namespace FECore
