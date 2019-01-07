//
//  DOFS.cpp
//  FECore
//
//  Created by Gerard Ateshian on 12/28/13.
//  Copyright (c) 2013 febio.org. All rights reserved.
//
#include "stdafx.h"
#include "DOFS.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "DumpStream.h"
using namespace std;

//-----------------------------------------------------------------------------
DOFS::DOF_ITEM::DOF_ITEM()
{
	sz[0] = 0;
	ndof = -1;
}

//-----------------------------------------------------------------------------
DOFS::DOF_ITEM::DOF_ITEM(const char* sz)
{
	SetName(sz);
	ndof = -1;
}

//-----------------------------------------------------------------------------
DOFS::DOF_ITEM::DOF_ITEM(const DOFS::DOF_ITEM& d)
{
	SetName(d.sz);
	ndof = d.ndof;
}

//-----------------------------------------------------------------------------
void DOFS::DOF_ITEM::operator = (const DOFS::DOF_ITEM& d)
{
	SetName(d.sz);
	ndof = d.ndof;
}

//-----------------------------------------------------------------------------
DOFS::~DOFS()
{
	
}

//-----------------------------------------------------------------------------
void DOFS::DOF_ITEM::SetName(const char* szdof)
{
	strcpy(sz, szdof);
}

//-----------------------------------------------------------------------------
// constructor for the DOFS class
DOFS::DOFS()
{
	m_maxdofs = 0;
}

//-----------------------------------------------------------------------------
DOFS::DOFS(const DOFS& dofs)
{
	m_var = dofs.m_var;
	m_maxdofs = dofs.m_maxdofs;
}

//-----------------------------------------------------------------------------
DOFS& DOFS::operator = (const DOFS& dofs)
{
	m_var = dofs.m_var;
	m_maxdofs = dofs.m_maxdofs;
	return *this;
}

//-----------------------------------------------------------------------------
// destructor for the DOFS class
//
DOFS::DOF_ITEM::~DOF_ITEM()
{
}

//-----------------------------------------------------------------------------
DOFS::Var::Var()
{
	szname[0] = 0;
}

//-----------------------------------------------------------------------------
DOFS::Var::Var(const DOFS::Var& v) 
{ 
	strcpy(szname, v.szname); 
	m_dof = v.m_dof; 
	ntype = v.ntype; 
}

//-----------------------------------------------------------------------------
void DOFS::Var::operator = (const DOFS::Var& v)
{ 
	strcpy(szname, v.szname); 
	m_dof = v.m_dof; 
	ntype = v.ntype; 
}

//-----------------------------------------------------------------------------
void DOFS::Reset()
{
	// clear the DOFS
	if (m_var.empty() == false) m_var.clear();
	m_maxdofs = 0;
}

//-----------------------------------------------------------------------------
//! Define a variable.
//! This creates an empty variable. Add DOFs to this variable using one of the 
//! DOFS::AddDOF functions.
int DOFS::AddVariable(const char* szvar, int ntype)
{
	// Make sure szvar is a valid symbol
	if (szvar    == 0) return -1;	// cannot be null
	if (szvar[0] == 0) return -1;	// must have non-zero length

	// Make sure the variable does not exist yet
	int nvar = GetVariableIndex(szvar);
	if (nvar >= 0) return -1;

	// Okay, add the variable
	Var var;
	strcpy(var.szname, szvar);
	var.ntype = ntype;

	// allocate degrees of freedom
	int ndof = 0;
	if      (ntype == VAR_SCALAR) ndof = 1;
	else if (ntype == VAR_VEC2  ) ndof = 2;
	else if (ntype == VAR_VEC3  ) ndof = 3;
	else if (ntype == VAR_ARRAY ) ndof = 0;		// for array we start with no dofs predefined (use AddDOF to a dofs to an array variable)
	else { assert(false); return -1; }

	if (ndof > 0) var.m_dof.resize(ndof);
	m_var.push_back(var);

	Update();

	// return the index to this variable
	return (int) m_var.size() - 1;
}

//-----------------------------------------------------------------------------
//! Get number of variables
int DOFS::Variables() const
{
	return (int) m_var.size();
}

//-----------------------------------------------------------------------------
DOFS::Var* DOFS::GetVariable(const char* szvar)
{
	if (m_var.empty()) return 0;
	int NVAR = (int) m_var.size();
	for (int i=0; i<NVAR; ++i)
	{
		Var& var = m_var[i];
		if (strcmp(szvar, var.szname) == 0) return &var;
	}
	return 0;
}

//-----------------------------------------------------------------------------
int DOFS::GetVariableIndex(const char* szvar)
{
	if (m_var.empty()) return -1;
	int NVAR = (int) m_var.size();
	for (int i=0; i<NVAR; ++i)
	{
		Var& var = m_var[i];
		if (strcmp(szvar, var.szname) == 0) return i;
	}
	return -1;
}

//-----------------------------------------------------------------------------
//! Add a degree of freedom to a variable.
// This only works on array variables
//! Returns -1 if the degree of freedom exists or if the symbol is invalid
//! \sa DOFS::GetDOF
int DOFS::AddDOF(const char* szvar, const char* sz)
{
	// Make sure sz is a valid symbol
	if (sz    == 0) return -1;	// cannot be null
	if (sz[0] == 0) return -1;	// must have non-zero length

	// Make sure the symbol does not exist yet
	int ndof = GetDOF(sz);
	if (ndof >= 0) return -1;

	// Make sure the variable is valid
	Var* pvar = GetVariable(szvar);
	if (pvar && (pvar->ntype == VAR_ARRAY))
	{
		Var& var = *pvar;

		DOF_ITEM it(sz);
		var.m_dof.push_back(it);

		// update all dofs
		Update();

		// return a nonnegative number
		return 0;
	}

	// if we get here, the variable does not exist
	return -1;
}

//-----------------------------------------------------------------------------
//! Add a degree of freedom to a variable.
//! Returns -1 if the degree of freedom exists or if the symbol is invalid
//! \sa DOFS::GetDOF
int DOFS::AddDOF(int nvar, const char* sz)
{
	// Make sure sz is a valid symbol
	if (sz    == 0) return -1;	// cannot be null
	if (sz[0] == 0) return -1;	// must have non-zero length

	// Make sure the symbol is not defined yet
	int ndof = GetDOF(sz);
	if (ndof >= 0) return -1;

	// Make sure the variable index is valid
	if (nvar < 0) return -1;
	if (nvar >= (int) m_var.size()) return -1; 

	Var& var = m_var[nvar];
	if (var.ntype == VAR_ARRAY)
	{
		// Add the DOF
		DOF_ITEM it(sz);
		m_var[nvar].m_dof.push_back(it);

		// update all dofs
		Update();

		// return a nonnegative number
		return 0;
	}

	return -1;
}

//-----------------------------------------------------------------------------
int DOFS::GetIndex(const char* varName, const char* szdof)
{
	Var* var = GetVariable(varName);
	if (var == 0) return -1;
	for (int i=0; i<(int)var->m_dof.size(); ++i)
	{
		if (strcmp(var->m_dof[i].sz, szdof) == 0) return i;
	}
	return -1;
}

//-----------------------------------------------------------------------------
//! Return the DOF index from a variable and an index into the variable's dof array.
//! This index is used in the FENode::get(), FENode::set() functions to set 
//! the values of nodal values. This index is also used in the FENode::m_ID and FENode::m_BC arrays.
int DOFS::GetDOF(const char* szvar, int n)
{
	Var* pvar = GetVariable(szvar);
	if (pvar)
	{
//		assert((n >= 0) && (n<(int)pvar->m_dof.size()));
		if ((n >= 0) && (n<(int)pvar->m_dof.size()))
		{
			DOF_ITEM& it = pvar->m_dof[n];
			return it.ndof;
		}
	}
	return -1;
}

//-----------------------------------------------------------------------------
//! Return the DOF index from a variable and an index into the variable's dof array.
//! This index is used in the FENode::get(), FENode::set() functions to set 
//! the values of nodal values. This index is also used in the FENode::m_ID and FENode::m_BC arrays.
int DOFS::GetDOF(int nvar, int n)
{
	assert((nvar>=0)&&(nvar<(int)m_var.size()));
	Var& var = m_var[nvar];
	assert((n>=0)&&(n<(int)var.m_dof.size()));
	DOF_ITEM& it = var.m_dof[n];
	return it.ndof;
}

//-----------------------------------------------------------------------------
//! Return the DOF index from a dof symbol.
//! This index is used in the FENode::get(), FENode::set() functions to set 
//! the values of nodal values. This index is also used in the FENode::m_ID and FENode::m_BC arrays.
int DOFS::GetDOF(const char* szdof)
{
	const int NVAR = (int)m_var.size();
	for (int i=0; i<NVAR; ++i)
	{
		Var& var = m_var[i];
		int ndof = (int)var.m_dof.size();
		for (int j=0; j<ndof; ++j)
		{
			DOF_ITEM& it = var.m_dof[j];
			if (strcmp(it.sz, szdof) == 0) return it.ndof;
		}
	}
	return -1;
}

//-----------------------------------------------------------------------------
//! Returns a list of DOF indices for a variable. 
//! The returned list will be empty if the variable is not known
void DOFS::GetDOFList(const char* varName, std::vector<int>& dofs)
{
	// make sure we start with an empty list
	dofs.clear();

	// get the variable
	Var* var = GetVariable(varName);
	if (var == 0) return;

	// fill the dof list
	int n = (int) var->m_dof.size();
	if (n==0) return;
	dofs.resize(n);
	for (int i=0; i<n; ++i) dofs[i] = var->m_dof[i].ndof;
}

//-----------------------------------------------------------------------------
bool DOFS::ParseDOFString(const char* sz, std::vector<int>& dofs)
{
	const char* ch = sz;
	char szdof[8] = {0}, *c = szdof;
	do
	{
		if ((*ch==',')||(*ch==0))
		{
			*c = 0;
			int ndof = GetDOF(szdof);
			if (ndof != -1) dofs.push_back(ndof); else return false;
			c = szdof;
			if (*ch != 0) ch++; else ch = 0;
		}
		else *c++ = *ch++;
	}
	while (ch);
	return true;
}

//-----------------------------------------------------------------------------
//! get the size of the dof array of a variable
int DOFS::GetVariableSize(const char* szvar)
{
	Var* pvar = GetVariable(szvar);
	if (pvar) return (int)pvar->m_dof.size();
	return -1;
}

//-----------------------------------------------------------------------------
//! get the size of the dof array of a variable
int DOFS::GetVariableSize(int nvar)
{
	if ((nvar < 0) || (nvar >= (int) m_var.size())) return -1;
	return (int)m_var[nvar].m_dof.size();
}

//-----------------------------------------------------------------------------
//! get the size of the dof array of a variable
int DOFS::GetVariableType(int nvar)
{
	if ((nvar < 0) || (nvar >= (int) m_var.size())) return -1;
	return m_var[nvar].ntype;
}

//-----------------------------------------------------------------------------
//! return the total number of degrees of freedom
int DOFS::GetTotalDOFS() const { return m_maxdofs; }

//-----------------------------------------------------------------------------
// Updates the DOF indices. 
// This is called after a dof is added. 
void DOFS::Update()
{
	m_maxdofs = 0;
	int NVAR = (int) m_var.size();
	for (int i=0; i<NVAR; ++i)
	{
		Var& var = m_var[i];
		int NDOF = (int) var.m_dof.size();
		for (int j=0; j<NDOF; ++j)
		{
			DOF_ITEM& it = var.m_dof[j];
			it.ndof = m_maxdofs++;
		}
	}
}

//-----------------------------------------------------------------------------
DOFS::DOF_ITEM* DOFS::GetDOFPtr(const char* szdof)
{
	const int NVAR = (int)m_var.size();
	for (int i=0; i<NVAR; ++i)
	{
		Var& var = m_var[i];
		int ndof = (int)var.m_dof.size();
		for (int j=0; j<ndof; ++j)
		{
			DOF_ITEM& it = var.m_dof[j];
			if (strcmp(it.sz, szdof) == 0) return &it;
		}
	}
	return 0;
}

//-----------------------------------------------------------------------------
void DOFS::SetDOFName(const char* szvar, int n, const char* szname)
{
	Var* pvar = GetVariable(szvar);
	if (pvar)
	{
		int nsize = (int) pvar->m_dof.size();
		if ((n>=0)&&(n<nsize))
		{
			DOF_ITEM& it = pvar->m_dof[n];
			it.SetName(szname);
		}
	}
}

//-----------------------------------------------------------------------------
void DOFS::SetDOFName(int nvar, int n, const char* szname)
{
	if ((nvar>=0)&&(nvar<(int)m_var.size()))
	{
		Var& var = m_var[nvar];
		int nsize = (int) var.m_dof.size();
		if ((n>=0)&&(n<nsize))
		{
			DOF_ITEM& it = var.m_dof[n];
			it.SetName(szname);
		}
	}
}

//-----------------------------------------------------------------------------
const char* DOFS::GetDOFName(int nvar, int n)
{
	if ((nvar >= 0) && (nvar<(int)m_var.size()))
	{
		Var& var = m_var[nvar];
		int nsize = (int)var.m_dof.size();
		if ((n >= 0) && (n<nsize))
		{
			DOF_ITEM& it = var.m_dof[n];
			return it.sz;
		}
	}

	return 0;
}

//-----------------------------------------------------------------------------
void DOFS::Serialize(DumpStream& ar)
{
	if (ar.IsShallow()) return;

	if (ar.IsSaving())
	{
		ar << m_maxdofs;
		int nvar = (int)m_var.size();
		ar << nvar;
		for (int i=0; i<nvar; ++i)
		{
			Var& v = m_var[i];
			ar << v.ntype;
			ar << v.szname;

			int ndof = (int) v.m_dof.size();
			ar << ndof;
			for (int j=0; j<ndof; ++j)
			{
				DOF_ITEM& d = v.m_dof[j];
				ar << d.ndof;
				ar << d.sz;
			}
		}
	}
	else
	{
		m_var.clear();
		ar >> m_maxdofs;
		int nvar;
		ar >> nvar;
		for (int i=0; i<nvar; ++i)
		{
			Var v;
			ar >> v.ntype;
			ar >> v.szname;

			int ndof;
			ar >> ndof;
			for (int j=0; j<ndof; ++j)
			{
				DOF_ITEM d;
				ar >> d.ndof;
				ar >> d.sz;

				v.m_dof.push_back(d);
			}

			m_var.push_back(v);
		}
	}
}
