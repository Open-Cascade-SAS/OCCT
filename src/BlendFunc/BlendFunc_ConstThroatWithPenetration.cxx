// Created by: Julia GERASIMOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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
#include <Adaptor3d_HSurface.hxx>
#include <Blend_Point.hxx>
#include <BlendFunc.hxx>
#include <BlendFunc_ConstThroatWithPenetration.hxx>
#include <ElCLib.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <math_Matrix.hxx>
#include <math_Gauss.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>

#define Eps 1.e-15


//=======================================================================
//function : BlendFunc_ConstThroatWithPenetration
//purpose  : 
//=======================================================================

BlendFunc_ConstThroatWithPenetration::
BlendFunc_ConstThroatWithPenetration(const Handle(Adaptor3d_HSurface)& S1,
                                     const Handle(Adaptor3d_HSurface)& S2,
                                     const Handle(Adaptor3d_HCurve)& C)
  : BlendFunc_ConstThroat(S1,S2,C)
{
}

//=======================================================================
//function : IsSolution
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ConstThroatWithPenetration::
IsSolution(const math_Vector& Sol, const Standard_Real Tol)
{
  math_Vector secmember(1,4), valsol(1,4);
  math_Matrix gradsol(1,4,1,4);

  Value(Sol, valsol);
  Derivatives(Sol, gradsol);

  tol = Tol;
  
  gp_Vec dnplan, temp1, temp2, temp3;

  if (Abs(valsol(1)) <= Tol &&
      Abs(valsol(2)) <= Tol &&
      Abs(valsol(3)) <= Tol*Tol &&
      Abs(valsol(4)) <= Tol)
  {
    dnplan.SetLinearForm(1./normtg,d2gui,
			 -1./normtg*(nplan.Dot(d2gui)),nplan); 
    
    temp1.SetXYZ(pts1.XYZ() - ptgui.XYZ());
    temp2.SetXYZ(pts2.XYZ() - ptgui.XYZ());
    temp3.SetXYZ(pts2.XYZ() - pts1.XYZ());
    surf1->D1(Sol(1),Sol(2),pts1,d1u1,d1v1);
    surf2->D1(Sol(3),Sol(4),pts2,d1u2,d1v2);
    
    secmember(1) = nplan.Dot(d1gui) - dnplan.Dot(temp1);
    secmember(2) = nplan.Dot(d1gui) - dnplan.Dot(temp2);
    secmember(3) = 2.*d1gui.Dot(temp1);
    secmember(4) = d1gui.Dot(temp3);

    math_Gauss Resol(gradsol);
    if (Resol.IsDone()) {
      Resol.Solve(secmember);
      tg1.SetLinearForm(secmember(1),d1u1,secmember(2),d1v1);
      tg2.SetLinearForm(secmember(3),d1u2,secmember(4),d1v2);
      tg12d.SetCoord(secmember(1),secmember(2));
      tg22d.SetCoord(secmember(3),secmember(4));
      istangent = Standard_False;
    }
    else {
      istangent = Standard_True;
    }

    distmin = Min(distmin, pts1.Distance(pts2));
    
    return Standard_True;
  }
  
  return Standard_False;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ConstThroatWithPenetration::Value(const math_Vector& X,
                                                             math_Vector& F)
{
  surf1->D0( X(1), X(2), pts1 );
  surf2->D0( X(3), X(4), pts2 );
  
  F(1) = nplan.XYZ().Dot(pts1.XYZ()) + theD;
  F(2) = nplan.XYZ().Dot(pts2.XYZ()) + theD;

  const gp_Vec vref(ptgui, pts1);
  
  F(3) = vref.SquareMagnitude() - Throat*Throat;

  const gp_Vec vec12(pts1, pts2);

  F(4) = vref.Dot(vec12);
  
  return Standard_True;
}

//=======================================================================
//function : Derivatives
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ConstThroatWithPenetration::Derivatives(const math_Vector& X,
                                                                   math_Matrix& D)
{
  surf1->D1( X(1), X(2), pts1, d1u1, d1v1);  
  surf2->D1( X(3), X(4), pts2, d1u2, d1v2);

  D(1,1) = nplan.Dot(d1u1);
  D(1,2) = nplan.Dot(d1v1);
  D(1,3) = 0.;
  D(1,4) = 0.;
  D(2,1) = 0.;
  D(2,2) = 0.;
  D(2,3) = nplan.Dot(d1u2);
  D(2,4) = nplan.Dot(d1v2);
  D(3,1) = 2.*gp_Vec(ptgui,pts1).Dot(d1u1);
  D(3,2) = 2.*gp_Vec(ptgui,pts1).Dot(d1v1);
  D(3,3) = 0.;
  D(3,4) = 0.;
  D(4,1) = d1u1.Dot(gp_Vec(pts1,pts2)) - gp_Vec(ptgui,pts1).Dot(d1u1);
  D(4,2) = d1v1.Dot(gp_Vec(pts1,pts2)) - gp_Vec(ptgui,pts1).Dot(d1v1);
  D(4,3) = gp_Vec(ptgui,pts1).Dot(d1u2);
  D(4,4) = gp_Vec(ptgui,pts1).Dot(d1v2);
  
  return Standard_True;
}

//=======================================================================
//function : TangentOnS1
//purpose  : 
//=======================================================================

const gp_Vec& BlendFunc_ConstThroatWithPenetration::TangentOnS1 () const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_ConstThroatWithPenetration::TangentOnS1");
  return tg1;
}


//=======================================================================
//function : TangentOnS2
//purpose  : 
//=======================================================================

const gp_Vec& BlendFunc_ConstThroatWithPenetration::TangentOnS2 () const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_ConstThroatWithPenetration::TangentOnS2");
  return tg2;
}


//=======================================================================
//function : Tangent2dOnS1
//purpose  : 
//=======================================================================

const gp_Vec2d& BlendFunc_ConstThroatWithPenetration::Tangent2dOnS1 () const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_ConstThroatWithPenetration::Tangent2dOnS1");
  return tg12d;
}

//=======================================================================
//function : Tangent2dOnS2
//purpose  : 
//=======================================================================

const gp_Vec2d& BlendFunc_ConstThroatWithPenetration::Tangent2dOnS2 () const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_ConstThroatWithPenetration::Tangent2dOnS2");
  return tg22d;
}

//=======================================================================
//function : GetSectionSize
//purpose  : 
//=======================================================================
Standard_Real BlendFunc_ConstThroatWithPenetration::GetSectionSize() const 
{
  throw Standard_NotImplemented("BlendFunc_ConstThroatWithPenetration::GetSectionSize()");
}
