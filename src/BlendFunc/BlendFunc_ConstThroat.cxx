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

#include <BlendFunc_ConstThroat.hxx>
#include <ElCLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <math_Gauss.hxx>
#include <Standard_NotImplemented.hxx>

#define Eps 1.e-15


//=======================================================================
//function : BlendFunc_ConstThroat
//purpose  : 
//=======================================================================

BlendFunc_ConstThroat::BlendFunc_ConstThroat(const Handle(Adaptor3d_Surface)& S1,
                                             const Handle(Adaptor3d_Surface)& S2,
                                             const Handle(Adaptor3d_Curve)& C)
: BlendFunc_GenChamfer(S1,S2,C),
  istangent(Standard_False),
  param(0.0),
  Throat(0.0),
  normtg(0.0),
  theD(0.0)
{
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BlendFunc_ConstThroat::Set(const Standard_Real aThroat,
                                const Standard_Real,
                                const Standard_Integer Choix)
{
  Throat = aThroat;
  choix = Choix;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BlendFunc_ConstThroat::Set(const Standard_Real Param)
{
  param = Param;
  curv->D2(param,ptgui,d1gui,d2gui);
  normtg = d1gui.Magnitude();
  nplan  = d1gui.Normalized();
  theD = - (nplan.XYZ().Dot(ptgui.XYZ()));
}

//=======================================================================
//function : IsSolution
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ConstThroat::IsSolution(const math_Vector& Sol, const Standard_Real Tol)
{
  math_Vector secmember(1,4), valsol(1,4);
  math_Matrix gradsol(1,4,1,4);

  Value(Sol, valsol);
  Derivatives(Sol, gradsol);

  tol = Tol;
  
  gp_Vec dnplan, temp1, temp2, tempmid;

  if (Abs(valsol(1)) <= Tol &&
      Abs(valsol(2)) <= Tol &&
      Abs(valsol(3)) <= Tol*Tol &&
      Abs(valsol(4)) <= Tol*Tol)
  {
    dnplan.SetLinearForm(1./normtg,d2gui,
			 -1./normtg*(nplan.Dot(d2gui)),nplan); 
    
    temp1.SetXYZ(pts1.XYZ() - ptgui.XYZ());
    temp2.SetXYZ(pts2.XYZ() - ptgui.XYZ());
    tempmid.SetXYZ((pts1.XYZ() + pts2.XYZ())/2 - ptgui.XYZ());
    surf1->D1(Sol(1),Sol(2),pts1,d1u1,d1v1);
    surf2->D1(Sol(3),Sol(4),pts2,d1u2,d1v2);
    
    secmember(1) = nplan.Dot(d1gui) - dnplan.Dot(temp1);
    secmember(2) = nplan.Dot(d1gui) - dnplan.Dot(temp2);
    secmember(3) = 2.*d1gui.Dot(tempmid);
    secmember(4) = 2.*d1gui.Dot(temp2) - 2.*d1gui.Dot(temp1);

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

Standard_Boolean BlendFunc_ConstThroat::Value(const math_Vector& X, math_Vector& F)
{
  surf1->D0( X(1), X(2), pts1 );
  surf2->D0( X(3), X(4), pts2 );
  
  F(1) = nplan.XYZ().Dot(pts1.XYZ()) + theD;
  F(2) = nplan.XYZ().Dot(pts2.XYZ()) + theD;

  const gp_Pnt ptmid((pts1.XYZ() + pts2.XYZ())/2);
  const gp_Vec vmid(ptgui, ptmid);
  
  F(3) = vmid.SquareMagnitude() - Throat*Throat;

  const gp_Vec vref1(ptgui, pts1);
  const gp_Vec vref2(ptgui, pts2);

  F(4) = vref1.SquareMagnitude() - vref2.SquareMagnitude();
  
  return Standard_True;
}

//=======================================================================
//function : Derivatives
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ConstThroat::Derivatives(const math_Vector& X, math_Matrix& D)
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
  D(3,1) = gp_Vec((pts1.XYZ() + pts2.XYZ())/2 - ptgui.XYZ()).Dot(d1u1);
  D(3,2) = gp_Vec((pts1.XYZ() + pts2.XYZ())/2 - ptgui.XYZ()).Dot(d1v1);
  D(3,3) = gp_Vec((pts1.XYZ() + pts2.XYZ())/2 - ptgui.XYZ()).Dot(d1u2);
  D(3,4) = gp_Vec((pts1.XYZ() + pts2.XYZ())/2 - ptgui.XYZ()).Dot(d1v2);
  D(4,1) = 2.*gp_Vec(ptgui,pts1).Dot(d1u1);
  D(4,2) = 2.*gp_Vec(ptgui,pts1).Dot(d1v1);
  D(4,3) = -2.*gp_Vec(ptgui,pts2).Dot(d1u2);
  D(4,4) = -2.*gp_Vec(ptgui,pts2).Dot(d1v2);
  
  return Standard_True;
}

//=======================================================================
//function : PointOnS1
//purpose  : 
//=======================================================================

const gp_Pnt& BlendFunc_ConstThroat::PointOnS1 () const
{
  return pts1;
}


//=======================================================================
//function : PointOnS2
//purpose  : 
//=======================================================================

const gp_Pnt& BlendFunc_ConstThroat::PointOnS2 () const
{
  return pts2;
}


//=======================================================================
//function : IsTangencyPoint
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ConstThroat::IsTangencyPoint () const
{
  return istangent;
}


//=======================================================================
//function : TangentOnS1
//purpose  : 
//=======================================================================

const gp_Vec& BlendFunc_ConstThroat::TangentOnS1 () const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_ConstThroat::TangentOnS1");
  return tg1;
}


//=======================================================================
//function : TangentOnS2
//purpose  : 
//=======================================================================

const gp_Vec& BlendFunc_ConstThroat::TangentOnS2 () const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_ConstThroat::TangentOnS2");
  return tg2;
}


//=======================================================================
//function : Tangent2dOnS1
//purpose  : 
//=======================================================================

const gp_Vec2d& BlendFunc_ConstThroat::Tangent2dOnS1 () const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_ConstThroat::Tangent2dOnS1");
  return tg12d;
}


//=======================================================================
//function : Tangent2dOnS2
//purpose  : 
//=======================================================================

const gp_Vec2d& BlendFunc_ConstThroat::Tangent2dOnS2 () const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_ConstThroat::Tangent2dOnS2");
  return tg22d;
}


//=======================================================================
//function : Tangent
//purpose  : 
//=======================================================================

void BlendFunc_ConstThroat::Tangent(const Standard_Real U1,
                                    const Standard_Real V1,
                                    const Standard_Real U2,
                                    const Standard_Real V2,
                                    gp_Vec& TgF,
                                    gp_Vec& TgL,
                                    gp_Vec& NmF,
                                    gp_Vec& NmL) const
{
  gp_Pnt pt;
  gp_Vec d1u,d1v;
  Standard_Boolean revF = Standard_False;
  Standard_Boolean revL = Standard_False;

  surf1->D1(U1,V1,pt,d1u,d1v);
  NmF = d1u.Crossed(d1v);

  surf2->D1(U2,V2,pt,d1u,d1v);
  NmL = d1u.Crossed(d1v);

  TgF = (nplan.Crossed(NmF)).Normalized();
  TgL = (nplan.Crossed(NmL)).Normalized();

  if( (choix == 2)||(choix == 5) ){
    revF = Standard_True;
    revL = Standard_True;
  }
  if( (choix == 4)||(choix == 7) )
    revL = Standard_True;
  if( (choix == 3)||(choix == 8) )
    revF = Standard_True;

  if( revF )
    TgF.Reverse();
  if( revL )
    TgL.Reverse();
}

//=======================================================================
//function : GetSectionSize
//purpose  : 
//=======================================================================
Standard_Real BlendFunc_ConstThroat::GetSectionSize() const 
{
  throw Standard_NotImplemented("BlendFunc_ConstThroat::GetSectionSize()");
}
