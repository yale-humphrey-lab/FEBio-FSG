#pragma once
#include "FEBiphasic.h"

//-----------------------------------------------------------------------------
// This class implements a poroelastic material that has a strain-dependent
// permeability which is orthotropic in the reference state, but exhibits
// further strain-induced anisotropy, according to the constitutive relation
// of Ateshian and Weiss (JBME 2010)

class FEBIOMIX_API FEPermRefOrtho :	public FEHydraulicPermeability
{
public:
	//! constructor
	FEPermRefOrtho(FEModel* pfem);
		
	//! permeability
	mat3ds Permeability(FEMaterialPoint& pt) override;
		
	//! Tangent of permeability
	tens4ds Tangent_Permeability_Strain(FEMaterialPoint& mp) override;
		
public:
	double	m_perm0;		//!< permeability for I term
	double	m_perm1[3];		//!< permeability for b term
	double	m_perm2[3];		//!< permeability for b^2 term
	double	m_phi0;			//!< solid volume fraction in reference state
	double	m_M0;			//!< nonlinear exponential coefficient
	double	m_alpha0;		//!< nonlinear power exponent
	double	m_M[3];			//!< nonlinear exponential coefficient
	double	m_alpha[3];		//!< nonlinear power exponent
		
	// declare parameter list
	DECLARE_FECORE_CLASS();
};
