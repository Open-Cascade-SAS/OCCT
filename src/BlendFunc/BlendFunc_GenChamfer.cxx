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
#include <BlendFunc_Chamfer.hxx>
#include <ElCLib.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <math_Matrix.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>


//=======================================================================
//function : BlendFunc_GenChamfer
//purpose  : 
//=======================================================================
BlendFunc_GenChamfer::BlendFunc_GenChamfer(const Handle(Adaptor3d_HSurface)& S1,
                                           const Handle(Adaptor3d_HSurface)& S2,
                                           const Handle(Adaptor3d_HCurve)&   CG)
: surf1(S1),
  surf2(S2),
  curv(CG),
  choix(0),
  tol(0.0),
  distmin(RealLast())
{
}

//=======================================================================
//function : NbEquations
//purpose  : 
//=======================================================================

Standard_Integer BlendFunc_GenChamfer::NbEquations () const
{
  return 4;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BlendFunc_GenChamfer::Set(const Standard_Real, const Standard_Real)
{
}

//=======================================================================
//function : GetTolerance
//purpose  : 
//=======================================================================

void BlendFunc_GenChamfer::GetTolerance(math_Vector& Tolerance, const Standard_Real Tol) const
{
  Tolerance(1) = surf1->UResolution(Tol);
  Tolerance(2) = surf1->VResolution(Tol);
  Tolerance(3) = surf2->UResolution(Tol);
  Tolerance(4) = surf2->VResolution(Tol);
}

//=======================================================================
//function : GetBounds
//purpose  : 
//=======================================================================

void BlendFunc_GenChamfer::GetBounds(math_Vector& InfBound, math_Vector& SupBound) const
{
  InfBound(1) = surf1->FirstUParameter();
  InfBound(2) = surf1->FirstVParameter();
  InfBound(3) = surf2->FirstUParameter();
  InfBound(4) = surf2->FirstVParameter();
  SupBound(1) = surf1->LastUParameter();
  SupBound(2) = surf1->LastVParameter();
  SupBound(3) = surf2->LastUParameter();
  SupBound(4) = surf2->LastVParameter();

  for(Standard_Integer i = 1; i <= 4; i++){
    if(!Precision::IsInfinite(InfBound(i)) &&
       !Precision::IsInfinite(SupBound(i))) {
      const Standard_Real range = (SupBound(i) - InfBound(i));
      InfBound(i) -= range;
      SupBound(i) += range;
    }
  }
}

//=======================================================================
//function : GetMinimalDistance
//purpose  : 
//=======================================================================

Standard_Real BlendFunc_GenChamfer::GetMinimalDistance() const
{
  return distmin;
}

//=======================================================================
//function : Values
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_GenChamfer::Values(const math_Vector& X, math_Vector& F, math_Matrix& D)
{
  Standard_Boolean val = Value(X,F);
  return (val && Derivatives(X,D));
}

//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

void BlendFunc_GenChamfer::Section(const Standard_Real /*Param*/,
                                   const Standard_Real U1,
                                   const Standard_Real V1,
                                   const Standard_Real U2,
                                   const Standard_Real V2,
                                   Standard_Real& Pdeb,
                                   Standard_Real& Pfin,
                                   gp_Lin& C)
{
  const gp_Pnt pts1 = surf1->Value(U1,V1);
  const gp_Pnt pts2 = surf2->Value(U2,V2);
  const gp_Dir dir( gp_Vec(pts1,pts2) );

  C.SetLocation(pts1);
  C.SetDirection(dir);

  Pdeb = 0.;
  Pfin = ElCLib::Parameter(C,pts2);
}  

//=======================================================================
//function : IsRational
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_GenChamfer::IsRational() const
{
  return Standard_False;
}

//=======================================================================
//function : GetMinimalWeight
//purpose  : 
//=======================================================================
void BlendFunc_GenChamfer::GetMinimalWeight(TColStd_Array1OfReal& Weights) const 
{
  Weights.Init(1);
}


//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================

Standard_Integer BlendFunc_GenChamfer::NbIntervals (const GeomAbs_Shape S) const
{
  return  curv->NbIntervals(BlendFunc::NextShape(S));
}


//=======================================================================
//function : Intervals
//purpose  : 
//=======================================================================

void BlendFunc_GenChamfer::Intervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const
{
  curv->Intervals(T, BlendFunc::NextShape(S)); 
}

//=======================================================================
//function : GetShape
//purpose  : 
//=======================================================================

void BlendFunc_GenChamfer::GetShape (Standard_Integer& NbPoles,
				  Standard_Integer& NbKnots,
				  Standard_Integer& Degree,
				  Standard_Integer& NbPoles2d)
{
  NbPoles = 2;
  NbPoles2d = 2;
  NbKnots = 2;
  Degree = 1;  
}
  
//=======================================================================
//function : GetTolerance
//purpose  : Determine les Tolerance a utiliser dans les approximations.
//=======================================================================
void BlendFunc_GenChamfer::GetTolerance(const Standard_Real BoundTol, 
                                        const Standard_Real, 
                                        const Standard_Real, 
                                        math_Vector& Tol3d, 
                                        math_Vector&) const
{
  Tol3d.Init(BoundTol);
}

//=======================================================================
//function : Knots
//purpose  : 
//=======================================================================

void BlendFunc_GenChamfer::Knots(TColStd_Array1OfReal& TKnots)
{
  TKnots(1) = 0.;
  TKnots(2) = 1.;
}


//=======================================================================
//function : Mults
//purpose  : 
//=======================================================================

void BlendFunc_GenChamfer::Mults(TColStd_Array1OfInteger& TMults)
{
  TMults(1) = 2;
  TMults(2) = 2;
}

//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_GenChamfer::Section
  (const Blend_Point& /*P*/,
   TColgp_Array1OfPnt& /*Poles*/,
   TColgp_Array1OfVec& /*DPoles*/,
   TColgp_Array1OfVec& /*D2Poles*/,
   TColgp_Array1OfPnt2d& /*Poles2d*/,
   TColgp_Array1OfVec2d& /*DPoles2d*/,
   TColgp_Array1OfVec2d& /*D2Poles2d*/,
   TColStd_Array1OfReal& /*Weights*/,
   TColStd_Array1OfReal& /*DWeights*/,
   TColStd_Array1OfReal& /*D2Weights*/)
{
  return Standard_False;
}

//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_GenChamfer::Section
  (const Blend_Point& P,
   TColgp_Array1OfPnt& Poles,
   TColgp_Array1OfVec& DPoles,
   TColgp_Array1OfPnt2d& Poles2d,
   TColgp_Array1OfVec2d& DPoles2d,
   TColStd_Array1OfReal& Weights,
   TColStd_Array1OfReal& DWeights)
{
  math_Vector sol(1,4),valsol(1,4),secmember(1,4);
  math_Matrix gradsol(1,4,1,4);

  Standard_Real prm = P.Parameter();
  Standard_Integer low = Poles.Lower();
  Standard_Integer upp = Poles.Upper();
  Standard_Boolean istgt;

  P.ParametersOnS1(sol(1),sol(2));
  P.ParametersOnS2(sol(3),sol(4));

  Set(prm);
  
  Values(sol,valsol,gradsol);
  IsSolution(sol,tol); 

  istgt = IsTangencyPoint();

  Poles2d(Poles2d.Lower()).SetCoord(sol(1),sol(2));
  Poles2d(Poles2d.Upper()).SetCoord(sol(3),sol(4));
  if (!istgt) {
  DPoles2d(Poles2d.Lower()).SetCoord(Tangent2dOnS1().X(),
				     Tangent2dOnS1().Y());
  DPoles2d(Poles2d.Upper()).SetCoord(Tangent2dOnS2().X(),
				     Tangent2dOnS2().Y());
  }
  Poles(low) = PointOnS1();
  Poles(upp) = PointOnS2();
  Weights(low) = 1.0;
  Weights(upp) = 1.0;
  if (!istgt) {
    DPoles(low) = TangentOnS1();
    DPoles(upp) = TangentOnS2();
    DWeights(low) = 0.0;
    DWeights(upp) = 0.0;  
  }

  return (!istgt);
}

//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

void BlendFunc_GenChamfer::Section(const Blend_Point& P,
                                   TColgp_Array1OfPnt& Poles,
                                   TColgp_Array1OfPnt2d& Poles2d,
                                   TColStd_Array1OfReal& Weights)
{
  Standard_Real u1,v1,u2,v2,prm = P.Parameter();
  Standard_Integer low = Poles.Lower();
  Standard_Integer upp = Poles.Upper();
  math_Vector X(1,4), F(1,4);

  P.ParametersOnS1(u1,v1);
  P.ParametersOnS2(u2,v2);
  X(1)=u1;
  X(2)=v1;
  X(3)=u2;
  X(4)=v2;
  Poles2d(Poles2d.Lower()).SetCoord(u1,v1);
  Poles2d(Poles2d.Upper()).SetCoord(u2,v2);

  Set(prm);
  Value(X,F);
  Poles(low) = PointOnS1();
  Poles(upp) = PointOnS2();
  Weights(low) = 1.0;
  Weights(upp) = 1.0;
}

void BlendFunc_GenChamfer::Resolution(const Standard_Integer IC2d, const Standard_Real Tol,
                                      Standard_Real& TolU, Standard_Real& TolV) const
{
  if(IC2d == 1){
    TolU = surf1->UResolution(Tol);
    TolV = surf1->VResolution(Tol);
  }
  else {
    TolU = surf2->UResolution(Tol);
    TolV = surf2->VResolution(Tol);
  }
}
