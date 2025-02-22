/*This file is part of the FEBio source code and is licensed under the MIT license
listed below.

See Copyright-FEBio.txt for details.

Copyright (c) 2021 University of Utah, The Trustees of Columbia University in
the City of New York, and others.

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
#include "FENodalTargetForce.h"
#include "FEBioMech.h"
#include <FECore/FENodeSet.h>
#include <FECore/FEMaterialPoint.h>
#include <FECore/FENode.h>

// NOTE: We pass FEModelLoad as the base since I don't want the relative flag
BEGIN_FECORE_CLASS(FENodalTargetForce, FEModelLoad)
	ADD_PARAMETER(m_w, "scale")->setUnits(UNIT_NONE)->SetFlags(FE_PARAM_ADDLC | FE_PARAM_VOLATILE);
	ADD_PARAMETER(m_f, "force")->setUnits(UNIT_FORCE);
	ADD_PARAMETER(m_shellBottom, "shell_bottom");
END_FECORE_CLASS();

FENodalTargetForce::FENodalTargetForce(FEModel* fem) : FENodalLoad(fem)
{
	m_w = 1;
	m_f = vec3d(0, 0, 0);
	m_shellBottom = false;
}

void FENodalTargetForce::Activate()
{
	// use a little hack to get the initial reaction forces
	m_brelative = true;
	FENodalLoad::Activate();
	m_brelative = false;
}

bool FENodalTargetForce::SetDofList(FEDofList& dofList)
{
	if (m_shellBottom)
		return dofList.AddVariable(FEBioMech::GetVariableName(FEBioMech::SHELL_DISPLACEMENT));
	else
		return dofList.AddVariable(FEBioMech::GetVariableName(FEBioMech::DISPLACEMENT));
}

void FENodalTargetForce::GetNodalValues(int inode, std::vector<double>& val)
{
	assert(val.size() == 3);
	const FENodeSet& nset = *GetNodeSet();
	const FENode& node = *nset.Node(inode);

	FEMaterialPoint mp;
	mp.m_r0 = node.m_r0;
	mp.m_index = inode;

	vec3d f = m_f(mp);

	val[0] = m_rval[inode][0] * (1.0 - m_w) + m_w * f.x;
	val[1] = m_rval[inode][1] * (1.0 - m_w) + m_w * f.y;
	val[2] = m_rval[inode][2] * (1.0 - m_w) + m_w * f.z;
}
