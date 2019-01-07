#pragma once
#include "SparseMatrix.h"
#include "LinearSolver.h"
#include "CompactSymmMatrix.h"
#include "CompactUnSymmMatrix.h"

//-----------------------------------------------------------------------------
// This class implements a diagonally symmetric block-structured matrix. That is
// A matrix for which the diagonal blocks are symmetric, but the off-diagonal
// matrices can be unsymmetric.
class FECORE_API BlockMatrix : public SparseMatrix
{
public:
	struct BLOCK
	{
		int		nstart_row, nend_row;
		int		nstart_col, nend_col;
		CompactMatrix*	pA;

		int Rows() { return nend_row - nstart_row + 1; }
		int Cols() { return nend_col - nstart_col + 1; }

		bool vmult(vector<double>& x, vector<double>& y)
		{
			return pA->mult_vector(&x[0], &y[0]);
		}
	};

public:
	BlockMatrix();
	~BlockMatrix();

public:
	//! Partition the matrix into blocks
	void Partition(const vector<int>& part, Matrix_Type mtype, int offset = 1);

public:
	//! Create a sparse matrix from a sparse-matrix profile
	void Create(SparseMatrixProfile& MP) override;

	//! assemble a matrix into the sparse matrix
	void Assemble(matrix& ke, std::vector<int>& lm) override;

	//! assemble a matrix into the sparse matrix
	void Assemble(matrix& ke, std::vector<int>& lmi, std::vector<int>& lmj) override;

	//! check if a matrix entry was allocated
	bool check(int i, int j) override;

	//! set entry to value
	void set(int i, int j, double v) override;

	//! add value to entry
	void add(int i, int j, double v) override;

	//! retrieve value
	double get(int i, int j) override;

	//! get the diagonal value
	double diag(int i) override;

	//! release memory for storing data
	void Clear() override;

	//! zero matrix elements
	void Zero() override;

	//! multiply with vector
	bool mult_vector(double* x, double* r) override;

public:
	//! return number of blocks
	int Blocks() const { return (int) m_Block.size(); }

	//! get a block
	BLOCK& Block(int i, int j);

	//! find the partition index of an equation number i
	int find_partition(int i);

	//! return number of partitions
	int Partitions() const { return (int) m_part.size() - 1; }

	//! Start equation index of partition i
	int StartEquationIndex(int i) { return m_part[i]; }

	//! number of equations in partition i
	int PartitionEquations(int i) { return m_part[i+1]-m_part[i]; }

protected:
	vector<int>		m_part;		//!< partition list
	vector<BLOCK>	m_Block;	//!< block matrices
};
