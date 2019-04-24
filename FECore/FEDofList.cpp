/*This file is part of the FEBio source code and is licensed under the MIT license
listed below.

See Copyright-FEBio.txt for details.

Copyright (c) 2019 University of Utah, Columbia University, and others.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/
#include "stdafx.h"
#include "FEDofList.h"
#include "FEModel.h"

FEDofList::FEDofList(FEModel* fem)
{
	m_fem = fem;
}

FEDofList::FEDofList(const FEDofList& dofs)
{
	m_fem = dofs.m_fem;
	m_dofList = dofs.m_dofList;
}

void FEDofList::operator = (const FEDofList& dofs)
{
	assert(m_fem == dofs.m_fem);
	m_dofList = dofs.m_dofList;
}

void FEDofList::Clear()
{
	m_dofList.clear();
}

bool FEDofList::AddDof(const char* szdof)
{
	int dof = m_fem->GetDOFIndex(szdof);
	if (dof == -1) return false;
	m_dofList.push_back(dof);
	return true;
}

bool FEDofList::AddDof(int ndof)
{
	m_dofList.push_back(ndof);
	return true;
}

bool FEDofList::AddVariable(const char* szvar)
{
	DOFS& Dofs = m_fem->GetDOFS();
	std::vector<int> dofList;
	Dofs.GetDOFList(szvar, dofList);
	if (dofList.empty()) return false;

	m_dofList.insert(m_dofList.end(), dofList.begin(), dofList.end());
	return true;
}

// Add all the dofs a variable
bool FEDofList::AddVariable(int nvar)
{
	DOFS& Dofs = m_fem->GetDOFS();
	std::vector<int> dofList;
	Dofs.GetDOFList(nvar, dofList);
	if (dofList.empty()) return false;

	m_dofList.insert(m_dofList.end(), dofList.begin(), dofList.end());
	return true;
}

// Add degrees of freedom
bool FEDofList::AddDofs(const FEDofList& dofs)
{
	for (int i = 0; i < dofs.Size(); ++i) {
		if (AddDof(dofs[i]) == false) return false;
	}
	return true;
}

bool FEDofList::IsEmpty() const
{
	return m_dofList.empty();
}

int FEDofList::Size() const
{
	return (int)m_dofList.size();
}

int FEDofList::operator [] (int n) const
{
	return m_dofList[n];
}

void FEDofList::Serialize(DumpStream& ar)
{
	ar & m_dofList;
}
