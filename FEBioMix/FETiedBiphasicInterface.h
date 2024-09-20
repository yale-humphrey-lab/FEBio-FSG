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



#pragma once
#include <FEBioMech/FEContactInterface.h>
#include "FEBiphasicContactSurface.h"

//-----------------------------------------------------------------------------
class FEBIOMIX_API FETiedBiphasicContactPoint: public FEBiphasicContactPoint
{
public:
    vec3d   m_Gap;  //!< initial gap in reference configuration
    vec3d   m_dg;   //!< gap function at integration points
    vec3d   m_nu;   //!< normal at integration points
    vec2d   m_rs;   //!< natural coordinates of projection of integration point
    vec3d   m_Lmd;  //!< lagrange multipliers for displacements
    vec3d   m_tr;   //!< contact traction
    double  m_epsn; //!< penalty factors
    double  m_epsp; //!< pressure penalty factors

    void Init() override
    {
        FEBiphasicContactPoint::Init();
        m_Gap = m_dg = m_nu = m_Lmd = m_tr = vec3d(0,0,0);
        m_rs = vec2d(0,0);
        m_epsn = 1.0;
        m_epsp = 1.0;
    }
    
    void Serialize(DumpStream& ar) override;
};

//-----------------------------------------------------------------------------
class FEBIOMIX_API FETiedBiphasicSurface : public FEBiphasicContactSurface
{
public:
	//! constructor
	FETiedBiphasicSurface(FEModel* pfem);
	
	//! initialization
	bool Init() override;
	
	//! create material point data
	FEMaterialPoint* CreateMaterialPoint() override;

	//! calculate the nodal normals
	void UpdateNodeNormals();
	
	void Serialize(DumpStream& ar) override;
	
	void SetPoroMode(bool bporo) { m_bporo = bporo; }
	
public:
    void GetVectorGap      (int nface, vec3d& pg) override;
    void GetContactTraction(int nface, vec3d& pt) override;
    
public:
	bool				m_bporo;	//!< set poro-mode
	
    vector<bool>			m_poro;	//!< surface element poro status
    vector<vec3d>			m_nn;	//!< node normals
};

//-----------------------------------------------------------------------------
class FEBIOMIX_API FETiedBiphasicInterface :	public FEContactInterface
{
public:
	//! constructor
	FETiedBiphasicInterface(FEModel* pfem);
	
	//! destructor
	~FETiedBiphasicInterface();
	
	//! initialization
	bool Init() override;
	
	//! interface activation
	void Activate() override;

	//! serialize data to archive
	void Serialize(DumpStream& ar) override;

	//! return the primary and secondary surface
	FESurface* GetPrimarySurface() override { return &m_ss; }
	FESurface* GetSecondarySurface() override { return &m_ms; }

	//! return integration rule class
	bool UseNodalIntegration() override { return false; }

	//! build the matrix profile for use in the stiffness matrix
	void BuildMatrixProfile(FEGlobalMatrix& K) override;

public:
	//! calculate contact forces
	void LoadVector(FEGlobalVector& R, const FETimeInfo& tp) override;

	//! calculate contact stiffness
	void StiffnessMatrix(FELinearSystem& LS, const FETimeInfo& tp) override;

	//! calculate Lagrangian augmentations
	bool Augment(int naug, const FETimeInfo& tp) override;

	//! update
	void Update() override;

protected:
	void InitialProjection(FETiedBiphasicSurface& ss, FETiedBiphasicSurface& ms);
	void ProjectSurface(FETiedBiphasicSurface& ss, FETiedBiphasicSurface& ms);
	
	//! calculate penalty factor
    void UpdateAutoPenalty();
    
	void CalcAutoPenalty(FETiedBiphasicSurface& s);
	
	void CalcAutoPressurePenalty(FETiedBiphasicSurface& s);
	double AutoPressurePenalty(FESurfaceElement& el, FETiedBiphasicSurface& s);
	
public:
	FETiedBiphasicSurface	m_ss;	//!< primary surface
	FETiedBiphasicSurface	m_ms;	//!< secondary surface
	
	int				m_knmult;		//!< higher order stiffness multiplier
	bool			m_btwo_pass;	//!< two-pass flag
	double			m_atol;			//!< augmentation tolerance
	double			m_gtol;			//!< gap tolerance
	double			m_ptol;			//!< pressure gap tolerance
	double			m_stol;			//!< search tolerance
	bool			m_bsymm;		//!< use symmetric stiffness components only
	double			m_srad;			//!< contact search radius
	int				m_naugmax;		//!< maximum nr of augmentations
	int				m_naugmin;		//!< minimum nr of augmentations
	
	double			m_epsn;			//!< normal penalty factor
	bool			m_bautopen;		//!< use autopenalty factor
    bool            m_bupdtpen;     //!< update penalty at each time step

	// biphasic contact parameters
	double			m_epsp;		//!< flow rate penalty

protected:
	int	m_dofP;
	
	DECLARE_FECORE_CLASS();
};
