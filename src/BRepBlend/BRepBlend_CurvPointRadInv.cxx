// Created on: 1997-02-12
// Created by: Laurent BOURESCHE
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Adaptor3d_HCurve.hxx>
#include <BRepBlend_CurvPointRadInv.hxx>
#include <gp_Pnt.hxx>
#include <math_Matrix.hxx>

//=======================================================================
//function : BRepBlend_CurvPointRadInv
//purpose  : 
//=======================================================================
BRepBlend_CurvPointRadInv::BRepBlend_CurvPointRadInv
(const Handle(Adaptor3d_HCurve)& C1,
 const Handle(Adaptor3d_HCurve)& C2) : curv1(C1), curv2(C2)
{
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BRepBlend_CurvPointRadInv::Set(const Standard_Integer Choix) 
{
  choix = Choix;
}

//=======================================================================
//function : NbEquations
//purpose  : 
//=======================================================================

Standard_Integer BRepBlend_CurvPointRadInv::NbEquations() const
{
  return 2;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean BRepBlend_CurvPointRadInv::Value(const math_Vector& X,
						  math_Vector& F) 
{
  Standard_Real theD;
  gp_Pnt ptcur1, ptcur2;
  gp_Vec d1cur1, d1cur2;
  gp_XYZ nplan;//, ref;
  curv1->D1(X(1),ptcur1, d1cur1);
  nplan = d1cur1.Normalized().XYZ();
  theD = -(nplan.Dot(ptcur1.XYZ()));
  curv2->D1(X(2), ptcur2,  d1cur2);
  F(1) = nplan.Dot(point.XYZ()) + theD;
  F(2) = nplan.Dot(ptcur2.XYZ()) + theD;
  return Standard_True;
}

//=======================================================================
//function : Derivatives
//purpose  : 
//=======================================================================

Standard_Boolean BRepBlend_CurvPointRadInv::Derivatives(const math_Vector& X,
							math_Matrix& D) 
{
  gp_Pnt ptcur1, ptcur2;
  gp_Vec d1cur1,d2cur1, d1cur2, nplan, dnplan;
  Standard_Real dtheD, normd1cur1, unsurnormd1cur1;

  curv1->D2(X(1), ptcur1, d1cur1, d2cur1);

  normd1cur1      = d1cur1.Magnitude();
  unsurnormd1cur1 = 1. / normd1cur1;
  nplan           = unsurnormd1cur1 * d1cur1;
  dnplan.SetLinearForm(-nplan.Dot(d2cur1), nplan, d2cur1);
  dnplan.Multiply(unsurnormd1cur1);
  dtheD  = - nplan.XYZ().Dot(d1cur1.XYZ()) - dnplan.XYZ().Dot(ptcur1.XYZ());
  D(1,1) = dnplan.XYZ().Dot(point.XYZ()) + dtheD;
  D(1,2) = 0.;
  curv2->D1(X(2), ptcur2, d1cur2);
  D(2,1) = dnplan.XYZ().Dot(ptcur2.XYZ()) + dtheD;
  D(2,2) = nplan.Dot(d1cur2);

  return Standard_True;
}

//=======================================================================
//function : Values
//purpose  : 
//=======================================================================

Standard_Boolean BRepBlend_CurvPointRadInv::Values(const math_Vector& X,
						   math_Vector& F,
						   math_Matrix& D) 
{
  Value(X, F);
  Derivatives(X, D);

  return Standard_True;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BRepBlend_CurvPointRadInv::Set(const gp_Pnt& P) 
{
  point = P;
}

//=======================================================================
//function : GetTolerance
//purpose  : 
//=======================================================================

void BRepBlend_CurvPointRadInv::GetTolerance(math_Vector& Tolerance,
					     const Standard_Real Tol) const
{
  Tolerance(1) = curv1->Resolution(Tol);
  Tolerance(2) = curv2->Resolution(Tol);
}

//=======================================================================
//function : GetBounds
//purpose  : 
//=======================================================================

void BRepBlend_CurvPointRadInv::GetBounds(math_Vector& InfBound,
					  math_Vector& SupBound) const
{
  InfBound(1) = curv1->FirstParameter();
  SupBound(1) = curv1->LastParameter();
  InfBound(2) = curv2->FirstParameter();
  SupBound(2) = curv2->LastParameter();

}

//=======================================================================
//function : IsSolution
//purpose  : 
//=======================================================================

Standard_Boolean BRepBlend_CurvPointRadInv::IsSolution(const math_Vector&  Sol,
						       const Standard_Real Tol) 
{
  math_Vector valsol(1, 2);
  Value(Sol,valsol);
  if (Abs(valsol(1)) <= Tol && 
      Abs(valsol(2)) <= Tol  ) {
    return Standard_True;
  }
  return Standard_False;
}



