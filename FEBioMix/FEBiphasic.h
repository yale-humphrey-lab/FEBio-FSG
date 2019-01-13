#pragma once
#include <FEBioMech/FEElasticMaterial.h>
#include "FEHydraulicPermeability.h"
#include "FESolventSupply.h"
#include "FEActiveMomentumSupply.h"
#include <FEBioMech/FEBodyForce.h>

//-----------------------------------------------------------------------------
//! Biphasic material point class.
//
class FEBIOMIX_API FEBiphasicMaterialPoint : public FEMaterialPoint
{
public:
	//! constructor
	FEBiphasicMaterialPoint(FEMaterialPoint* ppt);

	//! create a shallow copy
	FEMaterialPoint* Copy();

	//! data serialization
	void Serialize(DumpStream& ar);

	//! Data initialization
	void Init();

public:
	// poro-elastic material data
	// The actual fluid pressure is the same as the effective fluid pressure
	// in a poroelastic material without solute(s).  The actual fluid pressure
	// is included here so that models that include both poroelastic and
	// solute-poroelastic domains produce plotfiles with consistent fluid
	// pressure fields.
	double		m_p;		//!< fluid pressure
	vec3d		m_gradp;	//!< spatial gradient of p
    vec3d		m_gradpp;	//!< gradp at previous time
	vec3d		m_w;		//!< fluid flux
	double		m_pa;		//!< actual fluid pressure
	double		m_phi0;		//!< referential solid volume fraction at current time
	double		m_phi0p;	//!< referential solid volume fraction at previous time
	double		m_phi0hat;	//!< referential solid volume fraction supply at current time
    double      m_Jp;       //!< determinant of solid deformation gradient at previous time
};

//-----------------------------------------------------------------------------
//! Base class for biphasic materials.

class FEBIOMIX_API FEBiphasic : public FEMaterial
{
public:
	FEBiphasic(FEModel* pfem);
	
	// returns a pointer to a new material point object
	FEMaterialPoint* CreateMaterialPointData() override;

	// Get the elastic component (overridden from FEMaterial)
	FEElasticMaterial* GetElasticMaterial() { return m_pSolid; }
	
public:
	//! calculate stress at material point
	mat3ds Stress(FEMaterialPoint& pt);
	
	//! calculate tangent stiffness at material point
	tens4ds Tangent(FEMaterialPoint& pt);

	//! return the permeability tensor as a matrix
	void Permeability(double k[3][3], FEMaterialPoint& pt);

	//! return the permeability as a tensor
	mat3ds Permeability(FEMaterialPoint& pt);

	//! return the permeability property
	FEHydraulicPermeability* GetPermeability() { return m_pPerm; }
	
	//! calculate actual fluid pressure
	double Pressure(FEMaterialPoint& pt);

	//! porosity
	double Porosity(FEMaterialPoint& pt);
	
    //! solid density
    FEParamDouble& SolidDensity() { return m_pSolid->Density(); }
    
	//! fluid density
	double FluidDensity() { return m_rhoTw; }

	//! get the solvent supply
	double SolventSupply(FEMaterialPoint& mp) { return (m_pSupp? m_pSupp->Supply(mp) : 0); }

	//! get the solvent supply property
	FESolventSupply* GetSolventSupply() { return m_pSupp; }

	//! Get the active momentum supply
	FEActiveMomentumSupply* GetActiveMomentumSupply() { return m_pAmom; }

public: // material parameters
	double						m_rhoTw;	//!< true fluid density
	double						m_phi0;		//!< solid volume fraction in reference configuration
    double                      m_tau;      //!< characteristic time constant for stabilization
    vector<FEBodyForce*>        m_bf;       //!< body forces acting on this biphasic material

private: // material properties
	FEElasticMaterial*			m_pSolid;	//!< pointer to elastic solid material
	FEHydraulicPermeability*	m_pPerm;	//!< pointer to permeability material
	FESolventSupply*			m_pSupp;	//!< pointer to solvent supply
	FEActiveMomentumSupply*		m_pAmom;	//!< pointer to active momentum supply
	
	DECLARE_FECORE_CLASS();
};
