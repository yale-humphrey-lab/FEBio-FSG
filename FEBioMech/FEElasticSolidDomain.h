#pragma once
#include "FECore/FESolidDomain.h"
#include "FEElasticDomain.h"
#include "FESolidMaterial.h"
#include <FECore/FETypes.h>

//-----------------------------------------------------------------------------
//! domain described by Lagrange-type 3D volumetric elements
//!
class FEElasticSolidDomain : public FESolidDomain, public FEElasticDomain
{
public:
	//! constructor
	FEElasticSolidDomain(FEModel* pfem);

	//! assignment operator
	FEElasticSolidDomain& operator = (FEElasticSolidDomain& d);

	//! initialize class
	bool Init() override;

	//! activate
	void Activate() override;

	//! initialize elements
	void PreSolveUpdate(const FETimeInfo& timeInfo) override;

	//! Unpack solid element data
	void UnpackLM(FEElement& el, vector<int>& lm) override;

public: // overrides from FEDomain

	//! get the material
	FEMaterial* GetMaterial() override { return m_pMat; }

	//! set the material
	void SetMaterial(FEMaterial* pm) override;

public: // overrides from FEElasticDomain

	// update stresses
	void Update(const FETimeInfo& tp) override;

	// update the element stress
	virtual void UpdateElementStress(int iel);

	//! intertial forces for dynamic problems
	void InertialForces(FEGlobalVector& R, vector<double>& F) override;

	//! internal stress forces
	void InternalForces(FEGlobalVector& R) override;

	//! body forces
	void BodyForce(FEGlobalVector& R, FEBodyForce& BF) override;

	//! calculates the global stiffness matrix for this domain
	void StiffnessMatrix(FESolver* psolver) override;

	//! calculates inertial stiffness
	void MassMatrix(FESolver* psolver, double scale) override;

	//! body force stiffness
	void BodyForceStiffness(FESolver* psolver, FEBodyForce& bf) override;

public:
	// --- S T I F F N E S S ---

	//! calculates the solid element stiffness matrix
	virtual void ElementStiffness(const FETimeInfo& tp, int iel, matrix& ke);

	//! geometrical stiffness (i.e. initial stress)
	virtual void ElementGeometricalStiffness(FESolidElement& el, matrix& ke);

	//! material stiffness component
	virtual void ElementMaterialStiffness(FESolidElement& el, matrix& ke);

	//! calculates the solid element mass matrix
	void ElementMassMatrix(FESolidElement& el, matrix& ke, double a);

	//! calculates the stiffness matrix due to body forces 
	void ElementBodyForceStiffness(FEBodyForce& bf, FESolidElement& el, matrix& ke);

	// --- R E S I D U A L ---

	//! Calculates the internal stress vector for solid elements
	void ElementInternalForce(FESolidElement& el, vector<double>& fe);

	//! Calculatess external body forces for solid elements
	void ElementBodyForce(FEBodyForce& BF, FESolidElement& elem, vector<double>& fe);

    //! Calculates the inertial force vector for solid elements
    void ElementInertialForce(FESolidElement& el, vector<double>& fe);
    
protected:
	FESolidMaterial*	m_pMat;
    double              m_alphaf;
    double              m_alpham;
    double              m_beta;
};
