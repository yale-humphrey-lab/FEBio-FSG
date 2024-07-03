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
#include "FEElasticMaterial.h"
#include <FECore/FEModelParam.h>
#include "adcm.h"

//! Implementation of a neo-Hookean hyperelastic material using automatic differentation
class FEBIOMECH_API FENeoHookeanAD : public FEElasticMaterial
{
public:
	FENeoHookeanAD(FEModel* pfem);

public:
	FEParamDouble		m_E;	//!< Young's modulus
	FEParamDouble		m_v;	//!< Poisson's ratio

public:
	//! calculate stress at material point
	mat3ds Stress(FEMaterialPoint& pt) override;

	//! calculate tangent stiffness at material point
	tens4ds Tangent(FEMaterialPoint& pt) override;

	//! calculate strain energy density at material point
	double StrainEnergyDensity(FEMaterialPoint& pt) override;

	//! calculate the 2nd Piola-Kirchhoff stress at material point
	mat3ds PK2Stress(FEMaterialPoint& pt, const mat3ds E) override;

	//! calculate material tangent stiffness at material point
	tens4dmm MaterialTangent(FEMaterialPoint& pt, const mat3ds E) override;

public:
	ad::number StrainEnergy_AD(FEMaterialPoint& mp, ad::mat3ds& C);
	ad::mat3ds PK2Stress_AD(FEMaterialPoint& mp, ad::mat3ds& C);
	ad2::number StrainEnergy_AD2(FEMaterialPoint& mp, ad2::mat3ds& C);

	// declare the parameter list
	DECLARE_FECORE_CLASS();
};
