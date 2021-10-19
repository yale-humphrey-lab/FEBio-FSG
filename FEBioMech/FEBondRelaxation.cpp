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
#include "FEBondRelaxation.h"
#include "FEElasticMaterial.h"
#include <FECore/log.h>
#include <FECore/expint_Ei.h>

///////////////////////////////////////////////////////////////////////////////
//
// FEBondRelaxationExponential
//
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// define the material parameters
BEGIN_FECORE_CLASS(FEBondRelaxationExponential, FEBondRelaxation)
    ADD_PARAMETER(m_tau, FE_RANGE_GREATER(0.0), "tau");
END_FECORE_CLASS();

//-----------------------------------------------------------------------------
//! Constructor.
FEBondRelaxationExponential::FEBondRelaxationExponential(FEModel* pfem) : FEBondRelaxation(pfem)
{
    m_tau = 0;
}

//-----------------------------------------------------------------------------
//! Relaxation function
double FEBondRelaxationExponential::Relaxation(FEMaterialPoint& mp, const double t, const mat3ds D)
{
	// --- constant relaxation times ---
    double g = exp(-t/m_tau);
	
	return g;
}

///////////////////////////////////////////////////////////////////////////////
//
// FEBondRelaxationExpDistortion
//
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// define the material parameters
BEGIN_FECORE_CLASS(FEBondRelaxationExpDistortion, FEBondRelaxation)
    ADD_PARAMETER(m_tau0 , FE_RANGE_GREATER         (0.0), "tau0" );
    ADD_PARAMETER(m_tau1 , FE_RANGE_GREATER_OR_EQUAL(0.0), "tau1" );
    ADD_PARAMETER(m_alpha, FE_RANGE_GREATER_OR_EQUAL(0.0), "alpha");
END_FECORE_CLASS();

//-----------------------------------------------------------------------------
//! Constructor.
FEBondRelaxationExpDistortion::FEBondRelaxationExpDistortion(FEModel* pfem) : FEBondRelaxation(pfem)
{
    m_tau0 = 0;
    m_tau1 = 0;
    m_alpha = 1;
}

//-----------------------------------------------------------------------------
//! Relaxation function
double FEBondRelaxationExpDistortion::Relaxation(FEMaterialPoint& mp, const double t, const mat3ds D)
{
    // get the elastic material point data
    FEElasticMaterialPoint& pt = *mp.ExtractData<FEElasticMaterialPoint>();
    
    // evaluate spatial Hencky (logarithmic) strain
    mat3ds h = pt.LeftHencky();
    
    // evaluate distortion magnitude (always positive)
    double K2 = (h.dev()).norm();

    double K2a = pow(K2,m_alpha);
    double tau = m_tau0 + m_tau1*K2a;

    double g = exp(-t/tau);
    
    return g;
}

///////////////////////////////////////////////////////////////////////////////
//
// FEBondRelaxationExpDistUser
//
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// define the material parameters
BEGIN_FECORE_CLASS(FEBondRelaxationExpDistUser, FEBondRelaxation)
ADD_PROPERTY(m_tau  , "tau");
END_FECORE_CLASS();

//-----------------------------------------------------------------------------
//! Constructor.
FEBondRelaxationExpDistUser::FEBondRelaxationExpDistUser(FEModel* pfem) : FEBondRelaxation(pfem)
{
    m_tau = nullptr;
}

//-----------------------------------------------------------------------------
//! Relaxation function
double FEBondRelaxationExpDistUser::Relaxation(FEMaterialPoint& mp, const double t, const mat3ds D)
{
    // get the elastic material point data
    FEElasticMaterialPoint& pt = *mp.ExtractData<FEElasticMaterialPoint>();
    
    // evaluate spatial Hencky (logarithmic) strain
    mat3ds h = pt.LeftHencky();
    
    // evaluate distortion magnitude (always positive)
    double K2 = (h.dev()).norm();
    
    double tau = m_tau->value(K2);
    
    double g = exp(-t/tau);
    
    return g;
}

///////////////////////////////////////////////////////////////////////////////
//
// FEBondRelaxationFung
//
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// define the material parameters
BEGIN_FECORE_CLASS(FEBondRelaxationFung, FEBondRelaxation)
    ADD_PARAMETER(m_tau1, FE_RANGE_GREATER(0.0), "tau1");
    ADD_PARAMETER(m_tau2, FE_RANGE_GREATER(0.0), "tau2");
END_FECORE_CLASS();

//-----------------------------------------------------------------------------
//! Constructor.
FEBondRelaxationFung::FEBondRelaxationFung(FEModel* pfem) : FEBondRelaxation(pfem)
{
}

//-----------------------------------------------------------------------------
//! Initialization.
bool FEBondRelaxationFung::Validate()
{
    if (FEBondRelaxation::Validate() == false) return false;
	if (m_tau2 <= m_tau1) { feLogError("tau2 must be > tau1"); return false; }
	return true;
}

//-----------------------------------------------------------------------------
//! Relaxation function
double FEBondRelaxationFung::Relaxation(FEMaterialPoint& mp, const double t, const mat3ds D)
{
    double g = 0;
    
    if (t > 0) {
        g = (m_tau2*exp(-t/m_tau2) - m_tau1*exp(-t/m_tau1)
        + t*(expint_Ei(-t/m_tau2) - expint_Ei(-t/m_tau1)))
        /(m_tau2 - m_tau1);
    }
    else
        g = 1;
    
    if (g < 0) g = 0;
    
    return g;
}

///////////////////////////////////////////////////////////////////////////////
//
// FEBondRelaxationPark
//
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// define the material parameters
BEGIN_FECORE_CLASS(FEBondRelaxationPark, FEBondRelaxation)
    ADD_PARAMETER(m_tau , FE_RANGE_GREATER(0.0), "tau");
    ADD_PARAMETER(m_beta, FE_RANGE_GREATER(0.0), "beta");
END_FECORE_CLASS();

//-----------------------------------------------------------------------------
//! Constructor.
FEBondRelaxationPark::FEBondRelaxationPark(FEModel* pfem) : FEBondRelaxation(pfem)
{
}

//-----------------------------------------------------------------------------
//! Relaxation function
double FEBondRelaxationPark::Relaxation(FEMaterialPoint& mp, const double t, const mat3ds D)
{
    double g = 1./(1+pow(t/m_tau,m_beta));
    
    return g;
}

///////////////////////////////////////////////////////////////////////////////
//
// FEBondRelaxationParkDistortion
//
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// define the material parameters
BEGIN_FECORE_CLASS(FEBondRelaxationParkDistortion, FEBondRelaxation)
    ADD_PARAMETER(m_tau0 , FE_RANGE_GREATER         (0.0), "tau0" );
    ADD_PARAMETER(m_tau1 , FE_RANGE_GREATER_OR_EQUAL(0.0), "tau1" );
    ADD_PARAMETER(m_beta0, FE_RANGE_GREATER         (0.0), "beta0");
    ADD_PARAMETER(m_beta1, FE_RANGE_GREATER_OR_EQUAL(0.0), "beta1");
    ADD_PARAMETER(m_alpha, FE_RANGE_GREATER_OR_EQUAL(0.0), "alpha");
END_FECORE_CLASS();

//-----------------------------------------------------------------------------
//! Constructor.
FEBondRelaxationParkDistortion::FEBondRelaxationParkDistortion(FEModel* pfem) : FEBondRelaxation(pfem)
{
    m_tau0 = 0;
    m_beta0 = 0;
    m_tau1 = 0;
    m_beta1 = 0;
    m_alpha = 1;
}

//-----------------------------------------------------------------------------
//! Relaxation function
double FEBondRelaxationParkDistortion::Relaxation(FEMaterialPoint& mp, const double t, const mat3ds D)
{
    double g;
    
    // get the elastic material point data
    FEElasticMaterialPoint& pt = *mp.ExtractData<FEElasticMaterialPoint>();
    
    // evaluate spatial Hencky (logarithmic) strain
    mat3ds h = pt.LeftHencky();
    
    // evaluate distortion magnitude (always positive)
    double K2 = (h.dev()).norm();

    double K2a = pow(K2,m_alpha);
    double tau = m_tau0 + m_tau1*K2a;
    double beta = m_beta0 + m_beta1*K2a;
    g = 1./(1+pow(t/tau,beta));
    
    return g;
}

///////////////////////////////////////////////////////////////////////////////
//
// FEBondRelaxationParkDistUser
//
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// define the material parameters
BEGIN_FECORE_CLASS(FEBondRelaxationParkDistUser, FEBondRelaxation)
ADD_PROPERTY(m_tau  , "tau");
ADD_PROPERTY(m_beta , "beta");
END_FECORE_CLASS();

//-----------------------------------------------------------------------------
//! Constructor.
FEBondRelaxationParkDistUser::FEBondRelaxationParkDistUser(FEModel* pfem) : FEBondRelaxation(pfem)
{
    m_tau = nullptr;
    m_beta = nullptr;
}

//-----------------------------------------------------------------------------
//! Relaxation function
double FEBondRelaxationParkDistUser::Relaxation(FEMaterialPoint& mp, const double t, const mat3ds D)
{
    double g;
    
    // get the elastic material point data
    FEElasticMaterialPoint& pt = *mp.ExtractData<FEElasticMaterialPoint>();
    
    // evaluate spatial Hencky (logarithmic) strain
    mat3ds h = pt.LeftHencky();
    
    // evaluate distortion magnitude (always positive)
    double K2 = (h.dev()).norm();
    
    double tau = m_tau->value(K2);
    double beta = m_beta->value(K2);
    g = 1./(1+pow(t/tau,beta));
    
    return g;
}

///////////////////////////////////////////////////////////////////////////////
//
// FEBondRelaxationPower
//
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// define the material parameters
BEGIN_FECORE_CLASS(FEBondRelaxationPower, FEBondRelaxation)
    ADD_PARAMETER(m_tau , FE_RANGE_GREATER(0.0), "tau");
    ADD_PARAMETER(m_beta, FE_RANGE_GREATER(0.0), "beta");
END_FECORE_CLASS();

//-----------------------------------------------------------------------------
//! Constructor.
FEBondRelaxationPower::FEBondRelaxationPower(FEModel* pfem) : FEBondRelaxation(pfem)
{
}

//-----------------------------------------------------------------------------
//! Relaxation function
double FEBondRelaxationPower::Relaxation(FEMaterialPoint& mp, const double t, const mat3ds D)
{
    double g = pow(1+t/m_tau,-m_beta);
    
    return g;
}

///////////////////////////////////////////////////////////////////////////////
//
// FEBondRelaxationPowerDistortion
//
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// define the material parameters
BEGIN_FECORE_CLASS(FEBondRelaxationPowerDistortion, FEBondRelaxation)
    ADD_PARAMETER(m_tau0 , FE_RANGE_GREATER         (0.0), "tau0");
    ADD_PARAMETER(m_tau1 , FE_RANGE_GREATER_OR_EQUAL(0.0), "tau1");
    ADD_PARAMETER(m_beta0, FE_RANGE_GREATER         (0.0), "beta0");
    ADD_PARAMETER(m_beta1, FE_RANGE_GREATER_OR_EQUAL(0.0), "beta1");
    ADD_PARAMETER(m_alpha, FE_RANGE_GREATER_OR_EQUAL(0.0), "alpha");
END_FECORE_CLASS();

//-----------------------------------------------------------------------------
//! Constructor.
FEBondRelaxationPowerDistortion::FEBondRelaxationPowerDistortion(FEModel* pfem) : FEBondRelaxation(pfem)
{
    m_tau1 = 0;
    m_alpha = 1;
}

//-----------------------------------------------------------------------------
//! Relaxation function
double FEBondRelaxationPowerDistortion::Relaxation(FEMaterialPoint& mp, const double t, const mat3ds D)
{
    double g;
    
    // get the elastic material point data
    FEElasticMaterialPoint& pt = *mp.ExtractData<FEElasticMaterialPoint>();
    
    // evaluate spatial Hencky (logarithmic) strain
    mat3ds h = pt.LeftHencky();
    
    // evaluate distortion magnitude (always positive)
    double K2 = (h.dev()).norm();
    
    double K2a = pow(K2,m_alpha);
    double tau = m_tau0 + m_tau1*K2a;
    double beta = m_beta0 + m_beta1*K2a;

    g = pow(1+t/tau,-beta);
    
    return g;
}

///////////////////////////////////////////////////////////////////////////////
//
// FEBondRelaxationPowerDistUser
//
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// define the material parameters
BEGIN_FECORE_CLASS(FEBondRelaxationPowerDistUser, FEBondRelaxation)
ADD_PROPERTY(m_tau  , "tau");
ADD_PROPERTY(m_beta , "beta");
END_FECORE_CLASS();

//-----------------------------------------------------------------------------
//! Constructor.
FEBondRelaxationPowerDistUser::FEBondRelaxationPowerDistUser(FEModel* pfem) : FEBondRelaxation(pfem)
{
    m_tau = nullptr;
    m_beta = nullptr;
}

//-----------------------------------------------------------------------------
//! Relaxation function
double FEBondRelaxationPowerDistUser::Relaxation(FEMaterialPoint& mp, const double t, const mat3ds D)
{
    double g;
    
    // get the elastic material point data
    FEElasticMaterialPoint& pt = *mp.ExtractData<FEElasticMaterialPoint>();
    
    // evaluate spatial Hencky (logarithmic) strain
    mat3ds h = pt.LeftHencky();
    
    // evaluate distortion magnitude (always positive)
    double K2 = (h.dev()).norm();
    
    double tau = m_tau->value(K2);
    double beta = m_beta->value(K2);
    
    g = pow(1+t/tau,-beta);
    
    return g;
}

///////////////////////////////////////////////////////////////////////////////
//
// FEBondRelaxationCarreau
//
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// define the material parameters
BEGIN_FECORE_CLASS(FEBondRelaxationCarreau, FEBondRelaxation)
    ADD_PARAMETER(m_tau0 , FE_RANGE_GREATER         (0.0), "tau0");
    ADD_PARAMETER(m_lam  , FE_RANGE_GREATER_OR_EQUAL(0.0), "lambda");
    ADD_PARAMETER(m_n    , FE_RANGE_GREATER         (0.0), "n");
END_FECORE_CLASS();

//-----------------------------------------------------------------------------
//! Constructor.
FEBondRelaxationCarreau::FEBondRelaxationCarreau(FEModel* pfem) : FEBondRelaxation(pfem)
{
    m_tau0 = 0;
    m_lam = 0;
    m_n = 1;
}

//-----------------------------------------------------------------------------
//! Relaxation function
double FEBondRelaxationCarreau::Relaxation(FEMaterialPoint& mp, const double t, const mat3ds D)
{
    double g;
    
    // evaluate the engineering shear rate
    double gdot = sqrt(2.)*D.norm();
    
    // evaluate the relaxation time
    double tau = m_tau0*pow(1+pow(m_lam*gdot,2),(m_n-1)/2.);
    
    g = exp(-t/tau);
    
    return g;
}

///////////////////////////////////////////////////////////////////////////////
//
// FEBondRelaxationProny
//
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// define the material parameters
BEGIN_FECORE_CLASS(FEBondRelaxationProny, FEBondRelaxation)
    // material parameters
    ADD_PARAMETER(m_t[0], FE_RANGE_GREATER_OR_EQUAL(0.0), "t1");
    ADD_PARAMETER(m_t[1], FE_RANGE_GREATER_OR_EQUAL(0.0), "t2");
    ADD_PARAMETER(m_t[2], FE_RANGE_GREATER_OR_EQUAL(0.0), "t3");
    ADD_PARAMETER(m_t[3], FE_RANGE_GREATER_OR_EQUAL(0.0), "t4");
    ADD_PARAMETER(m_t[4], FE_RANGE_GREATER_OR_EQUAL(0.0), "t5");
    ADD_PARAMETER(m_t[5], FE_RANGE_GREATER_OR_EQUAL(0.0), "t6");
    ADD_PARAMETER(m_g[0], FE_RANGE_CLOSED(0.0, 1.0)     , "g1");
    ADD_PARAMETER(m_g[1], FE_RANGE_CLOSED(0.0, 1.0)     , "g2");
    ADD_PARAMETER(m_g[2], FE_RANGE_CLOSED(0.0, 1.0)     , "g3");
    ADD_PARAMETER(m_g[3], FE_RANGE_CLOSED(0.0, 1.0)     , "g4");
    ADD_PARAMETER(m_g[4], FE_RANGE_CLOSED(0.0, 1.0)     , "g5");
    ADD_PARAMETER(m_g[5], FE_RANGE_CLOSED(0.0, 1.0)     , "g6");
END_FECORE_CLASS();

//-----------------------------------------------------------------------------
//! Constructor.
FEBondRelaxationProny::FEBondRelaxationProny(FEModel* pfem) : FEBondRelaxation(pfem)
{
    for (int i=0; i<MAX_TERMS; ++i)
    {
        m_t[i] = 1;
        m_g[i] = 0;
    }
    m_sg = 0.0;
}

//-----------------------------------------------------------------------------
//! Initialization.
bool FEBondRelaxationProny::Validate()
{
    if (FEBondRelaxation::Validate() == false) return false;
    m_sg = 0;
    for (int i=0; i<MAX_TERMS; ++i) m_sg += m_g[i];
    if (m_sg <= 0) return false;
    return true;
}

//-----------------------------------------------------------------------------
//! Relaxation function
double FEBondRelaxationProny::Relaxation(FEMaterialPoint& mp, const double t, const mat3ds D)
{
    // --- Prony series ---
    double g = 0;
    for (int i=0; i<MAX_TERMS; ++i) g += m_g[i]*exp(-t/m_t[i]);
    g /= m_sg;
    
    return g;
}
