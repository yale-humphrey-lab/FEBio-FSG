#include "stdafx.h"
#include "CompactMatrix.h"


//=============================================================================
// CompactMatrix
//=============================================================================

//-----------------------------------------------------------------------------
FECore::CompactMatrix::CompactMatrix(int offset)
{
	m_pindices = 0;
	m_ppointers = 0;
	m_offset = offset;
}

//-----------------------------------------------------------------------------
void FECore::CompactMatrix::Create(int N, int nz, double* pv, int* pi, int* pp)
{
	if (m_pd  ) delete [] m_pd; m_pd = pv;
	if (m_pindices ) delete [] m_pindices; m_pindices = pi;
	if (m_ppointers) delete [] m_ppointers; m_ppointers = pp;

	m_ndim  = N;
	m_nsize = nz;
}

//-----------------------------------------------------------------------------
bool FECore::print_hb(CompactMatrix& m, FILE* fp)
{
	int nsize = m.Size();
	int nnz = m.NonZeroes();

	fwrite(&nsize, sizeof(nsize), 1, fp);
	fwrite(&nnz, sizeof(nnz), 1, fp);
	fwrite(m.Pointers(), sizeof(int)   , nsize+1, fp);
	fwrite(m.Indices (), sizeof(int)   , nnz, fp);
	fwrite(m.Values  (), sizeof(double), nnz, fp);

	return true;
}

//=============================================================================
// CompactSymmMatrix
//=============================================================================

//-----------------------------------------------------------------------------
//! constructor
FECore::CompactSymmMatrix::CompactSymmMatrix(int offset) : CompactMatrix(offset) {}

//-----------------------------------------------------------------------------
void FECore::CompactSymmMatrix::mult_vector(const vector<double>& x, vector<double>& r)
{
	int j, i, n;
	int N = x.size();
	assert(N == Size());

	double* pv, rj;
	int* pi;

	::zero(r);

	// loop over all columns
	for (j=0; j<N; ++j)
	{
		pv = m_pd  + m_ppointers[j];
		pi = m_pindices + m_ppointers[j];
		n = m_ppointers[j+1] - m_ppointers[j];

		// add off-diagonal elements
		for (i=1; i<n-7; i+=8)
		{
			// add lower triangular element
			r[pi[i  ]] += pv[i  ]*x[j];
			r[pi[i+1]] += pv[i+1]*x[j];
			r[pi[i+2]] += pv[i+2]*x[j];
			r[pi[i+3]] += pv[i+3]*x[j];
			r[pi[i+4]] += pv[i+4]*x[j];
			r[pi[i+5]] += pv[i+5]*x[j];
			r[pi[i+6]] += pv[i+6]*x[j];
			r[pi[i+7]] += pv[i+7]*x[j];
		}
		for (i=0; i<(n-1)%8; ++i)
			r[pi[n-1-i]] += pv[n-1-i]*x[j];

		rj = pv[0]*x[j]; // add diagonal element
		for (i=1; i<n-7; i+=8)
		{
			// add upper triangular element
			rj += pv[i  ]*x[pi[i  ]];
			rj += pv[i+1]*x[pi[i+1]];
			rj += pv[i+2]*x[pi[i+2]];
			rj += pv[i+3]*x[pi[i+3]];
			rj += pv[i+4]*x[pi[i+4]];
			rj += pv[i+5]*x[pi[i+5]];
			rj += pv[i+6]*x[pi[i+6]];
			rj += pv[i+7]*x[pi[i+7]];
		}
		for (i=0; i<(n-1)%8; ++i)
			rj += pv[n-1-i]*x[pi[n-1-i]];
		r[j] += rj;
	}
}


//-----------------------------------------------------------------------------
void FECore::CompactSymmMatrix::Create(SparseMatrixProfile& mp)
{
	int i, j, k, n;

	int neq = mp.size();

	int* pointers = new int[neq + 1];
	for (i=0; i<=neq; ++i) pointers[i] = 0;

	int nsize = 0;
	for (i=0; i<neq; ++i)
	{
		vector<int>& a = mp.column(i);
		n = a.size();
		for (j=0; j<n; j += 2)
		{
			nsize += a[j+1] - a[j] + 1;
			for (k=a[j]; k<=a[j+1]; ++k) pointers[k]++;
		}
	}

	int* pindices = new int[nsize];
	int m = 0;
	for (i=0; i<=neq; ++i)
	{
		n = pointers[i];
		pointers[i] = m;
		m += n;
	}

	int* pval = new int[neq];
	for (i=0; i<neq; ++i) pval[i] = 0;

	for (i=0; i<neq; ++i)
	{
		vector<int>& a = mp.column(i);
		n = a.size();
		for (j=0; j<n; j += 2)
		{
			for (k=a[j]; k<=a[j+1]; ++k)
			{
				pindices[ pointers[k] + pval[k]] = i;
				++pval[k];
			}
		}
	}

	// cleanup
	delete [] pval;

	// offset the indicies for fortran arrays
	if(Offset())
	{
		for (i=0; i<=neq; ++i) pointers[i]++;
		for (i=0; i<nsize; ++i) pindices[i]++;
	}

	// create the values array
	double* pvalues = new double[nsize];
	if (pvalues == 0) throw MemException(sizeof(double)*nsize);

	// create the stiffness matrix
	CompactMatrix::Create(neq, nsize, pvalues, pindices, pointers);
}

//-----------------------------------------------------------------------------
// this sort function is defined in qsort.cpp
void qsort(int n, int* arr, int* indx);

//-----------------------------------------------------------------------------
//! This function assembles the local stiffness matrix
//! into the global stiffness matrix which is in compact column storage
//!
void FECore::CompactSymmMatrix::Assemble(matrix& ke, vector<int>& LM)
{
	// get the number of degrees of freedom
	const int N = ke.rows();

	// find the permutation array that sorts LM in ascending order
	// we can use this to speed up the row search (i.e. loop over n below)
	vector<int> P(N);
	qsort(N, &LM[0], &P[0]);

	// get the data pointers 
	int* indices = Indices();
	int* pointers = Pointers();
	double* pd = Values();
	int offset = Offset();

	// find the starting index
	int N0 = 0;
	while ((N0<N) && (LM[P[N0]]<0)) ++N0;

	// assemble element stiffness
	for (int m=N0; m<N; ++m)
	{
		int j = P[m];
		int J = LM[j];
		int n = 0;
		double* pm = pd+pointers[J]-offset;
		int* pi = indices + pointers[J] - offset;
		int l = pointers[J+1] - pointers[J];
		int M0 = m;
		while ((M0>N0) && (LM[P[M0-1]] == J)) M0--;
		for (int k=M0; k<N; ++k)
		{
			int i = P[k];
			int I = LM[i] + offset;
			for (;n<l; ++n) 
				if (pi[n] == I)
				{
					pm[n] += ke[i][j];
					break;
				}
		}
	}
}


//-----------------------------------------------------------------------------
void FECore::CompactSymmMatrix::Assemble(matrix& ke, vector<int>& LMi, vector<int>& LMj)
{
	int i, j, I, J;

	const int N = ke.rows();
	const int M = ke.columns();

	int* indices = Indices();
	int* pointers = Pointers();
	double* pd = Values();

	int *pi, l, n;

	for (i=0; i<N; ++i)
	{
		I = LMi[i];

		for (j=0; j<M; ++j)
		{
			J = LMj[j];

			// only add values to lower-diagonal part of stiffness matrix
			if ((I>=J) && (J>=0))
			{
				pi = indices + pointers[J];
				l = pointers[J+1] - pointers[J];
				for (n=0; n<l; ++n) if (pi[n] == I)
				{
					pd[pointers[J] + n] += ke[i][j];
					break;
				}
			}
		}
	}
}

//=============================================================================
// CompactUnSymmMatrix
//=============================================================================

//-----------------------------------------------------------------------------
//! Constructor for CompactUnSymmMatrix class 
FECore::CompactUnSymmMatrix::CompactUnSymmMatrix(int offset, bool row_based) : CompactMatrix(offset)
{
	m_brow_based = row_based;
}

//-----------------------------------------------------------------------------
void FECore::CompactUnSymmMatrix::Create(SparseMatrixProfile& mp)
{
	int i, j, k, n;

	int neq = mp.size();

	int* pointers = new int[neq + 1];
	for (i=0; i<=neq; ++i) pointers[i] = 0;

	int nsize = 0;
	for (i=0; i<neq; ++i)
	{
		vector<int>& a = mp.column(i);
		n = a.size();
		for (j=0; j<n; j += 2)
		{
			nsize += 2*(a[j+1] - a[j] + 1);
			pointers[i] += a[j+1] - a[j] + 1;
			for (k=a[j]; k<=a[j+1]; ++k) pointers[k]++;
		}
		--pointers[i]; // we double counted the diagonal
		--nsize;
	}

	int* pindices = new int[nsize];
	int m = 0;
	for (i=0; i<=neq; ++i)
	{
		n = pointers[i];
		pointers[i] = m;
		m += n;
	}
	assert(pointers[neq] == nsize);

	int* pval = new int[neq];
	for (i=0; i<neq; ++i) pval[i] = 0;

	for (i=0; i<neq; ++i)
	{
		vector<int>& a = mp.column(i);
		n = a.size();
		for (j=0; j<n; j += 2)
		{
			for (k=a[j]; k<=a[j+1]; ++k)
			{
				pindices[ pointers[i] + pval[i]] = k;
				++pval[i];
			}
		}
		for (j=0; j<n; j += 2)
		{
			for (k=a[j]; k<=a[j+1]; ++k)
			{
				if (k != i)
				{
					pindices[ pointers[k] + pval[k]] = i;
					++pval[k];
				}
			}
		}
	}

	// cleanup
	delete [] pval;

	// offset the indicies for fortran arrays
	if(Offset())
	{
		for (i=0; i<=neq; ++i) pointers[i]++;
		for (i=0; i<nsize; ++i) pindices[i]++;
	}

	// create the values array
	double* pvalues = new double[nsize];
	if (pvalues == 0) throw MemException(sizeof(double)*nsize);

	// create the stiffness matrix
	CompactMatrix::Create(neq, nsize, pvalues, pindices, pointers);
}

//-----------------------------------------------------------------------------
//! This function assembles the local stiffness matrix
//! into the global stiffness matrix which is in compact column storage and
//! the matrix is unsymmetric
//!
void FECore::CompactUnSymmMatrix::Assemble(matrix& ke, vector<int>& LM)
{
	int i, j, I, J;

	const int N = ke.rows();

	for (i=0; i<N; ++i)
	{
		if ((I = LM[i])>=0)
		{
			for (j=0; j<N; ++j)
			{
				if ((J = LM[j]) >= 0) add(I,J, ke[i][j]);
			}
		}
	}
}

//-----------------------------------------------------------------------------
void FECore::CompactUnSymmMatrix::Assemble(matrix& ke, vector<int>& LMi, vector<int>& LMj)
{
	int i, j, I, J;

	const int N = ke.rows();
	const int M = ke.columns();

	for (i=0; i<N; ++i)
	{
		if ((I = LMi[i])>=0)
		{
			for (j=0; j<M; ++j)
			{
				if ((J = LMj[j]) >= 0) add(I,J, ke[i][j]);
			}
		}
	}
}
