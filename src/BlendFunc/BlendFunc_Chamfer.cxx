// Created on: 1996-06-05
// Created by: Stagiaire Xuan Trang PHAMPHU
// Copyright (c) 1996-1999 Matra Datavision
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

// Modified : 20/08/96 PMN Ajout des methodes (Nb)Intervals et IsRationnal
// Modified : 30/12/96 PMN Ajout GetMinimalWeight, GetSectionSize;

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
//function : BlendFunc_Chamfer
//purpose  : 
//=======================================================================
BlendFunc_Chamfer::BlendFunc_Chamfer(const Handle(Adaptor3d_HSurface)& S1,
                                     const Handle(Adaptor3d_HSurface)& S2,
                                     const Handle(Adaptor3d_HCurve)&   CG) :
    surf1(S1),surf2(S2),
	curv(CG),
	distmin(RealLast()),
	corde1(S1,CG),corde2(S2,CG)
{
}

//=======================================================================
//function : NbEquations
//purpose  : 
//=======================================================================

Standard_Integer BlendFunc_Chamfer::NbEquations () const
{
  return 4;
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BlendFunc_Chamfer::Set(const Standard_Real Dist1, const Standard_Real Dist2,
			    const Standard_Integer Choix)
{
  corde1.SetDist(Dist1);
  corde2.SetDist(Dist2);
  choix = Choix;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BlendFunc_Chamfer::Set(const Standard_Real Param)
{
  corde1.SetParam(Param);
  corde2.SetParam(Param);
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BlendFunc_Chamfer::Set(const Standard_Real, const Standard_Real)
{
}

//=======================================================================
//function : GetTolerance
//purpose  : 
//=======================================================================

void BlendFunc_Chamfer::GetTolerance(math_Vector& Tolerance, const Standard_Real Tol) const
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

void BlendFunc_Chamfer::GetBounds(math_Vector& InfBound, math_Vector& SupBound) const
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
//function : IsSolution
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_Chamfer::IsSolution(const math_Vector& Sol, const Standard_Real Tol)
{
  math_Vector Sol1(1,2), Sol2(1,2);

  Sol1(1) = Sol(1);
  Sol1(2) = Sol(2);
  Sol2(1) = Sol(3); 
  Sol2(2) = Sol(4); 
  
  Standard_Boolean issol = corde1.IsSolution(Sol1,Tol);
  issol = issol && corde2.IsSolution(Sol2,Tol);
  tol = Tol;
  if (issol) 
    distmin = Min (distmin, corde1.PointOnS().Distance(corde2.PointOnS()));

  return issol;
}

//=======================================================================
//function : GetMinimalDistance
//purpose  : 
//=======================================================================

Standard_Real BlendFunc_Chamfer::GetMinimalDistance() const
{
  return distmin;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_Chamfer::Value(const math_Vector& X, math_Vector& F)
{
  math_Vector x(1,2), f(1,2);

  x(1) = X(1); x(2) = X(2);
  corde1.Value(x,f);
  F(1) = f(1); F(2) = f(2);

  x(1) = X(3); x(2) = X(4);
  corde2.Value(x,f);
  F(3) = f(1); F(4) = f(2);

  return Standard_True;
}


//=======================================================================
//function : Derivatives
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_Chamfer::Derivatives(const math_Vector& X, math_Matrix& D)
{
  Standard_Integer i,j;
  math_Vector x(1,2);
  math_Matrix d(1,2,1,2);

  x(1) = X(1); x(2) = X(2);
  corde1.Derivatives(x,d);
  for( i=1; i<3; i++ ){
    for( j=1; j<3; j++ ){
      D(i,j) = d(i,j);
      D(i,j+2) = 0.;
    }
  }   

  x(1) = X(3); x(2) = X(4);
  corde2.Derivatives(x,d);
  for( i=1; i<3; i++ ){
    for( j=1; j<3; j++ ){
      D(i+2,j+2) = d(i,j);
      D(i+2,j) = 0.;
    }
  }   

  return Standard_True;
}

//=======================================================================
//function : Values
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_Chamfer::Values(const math_Vector& X, math_Vector& F, math_Matrix& D)
{
  Standard_Boolean val = Value(X,F);
  return (val && Derivatives(X,D));
}

//=======================================================================
//function : PointOnS1
//purpose  : 
//=======================================================================

const gp_Pnt& BlendFunc_Chamfer::PointOnS1 () const
{
  return corde1.PointOnS();
}
  
//=======================================================================
//function : PointOnS2
//purpose  : 
//=======================================================================

const gp_Pnt& BlendFunc_Chamfer::PointOnS2 () const
{
  return corde2.PointOnS();
}


//=======================================================================
//function : IsTangencyPoint
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_Chamfer::IsTangencyPoint () const
{
  return corde1.IsTangencyPoint() && corde2.IsTangencyPoint();
}

//=======================================================================
//function : TangentOnS1
//purpose  : 
//=======================================================================

const gp_Vec& BlendFunc_Chamfer::TangentOnS1 () const
{
  return corde1.TangentOnS();
}

//=======================================================================
//function : TangentOnS2
//purpose  : 
//=======================================================================

const gp_Vec& BlendFunc_Chamfer::TangentOnS2 () const
{
  return corde2.TangentOnS();
}

//=======================================================================
//function : Tangent2dOnS1
//purpose  : 
//=======================================================================

const gp_Vec2d& BlendFunc_Chamfer::Tangent2dOnS1 () const
{
  return corde1.Tangent2dOnS();
}

//=======================================================================
//function : Tangent2dOnS2
//purpose  : 
//=======================================================================

const gp_Vec2d& BlendFunc_Chamfer::Tangent2dOnS2 () const
{
  return corde2.Tangent2dOnS();
}

//=======================================================================
//function : Tangent
//purpose  : TgF,NmF et TgL,NmL les tangentes et normales respectives
//           aux surfaces S1 et S2 
//=======================================================================

void BlendFunc_Chamfer::Tangent(const Standard_Real U1,
				const Standard_Real V1,
				const Standard_Real U2,
				const Standard_Real V2,
				gp_Vec& TgF,
				gp_Vec& TgL,
				gp_Vec& NmF,
				gp_Vec& NmL) const
{
  gp_Pnt pt1,pt2,ptgui;
  gp_Vec d1u1,d1v1,d1u2,d1v2;
  gp_Vec nplan;
  Standard_Boolean revF = Standard_False;
  Standard_Boolean revL = Standard_False;

  ptgui = corde1.PointOnGuide();
  nplan = corde1.NPlan();
  surf1->D1(U1,V1,pt1,d1u1,d1v1);
  NmF = d1u1.Crossed(d1v1);

  surf2->D1(U2,V2,pt2,d1u2,d1v2);
  NmL = d1u2.Crossed(d1v2);

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
//function : Section
//purpose  : 
//=======================================================================

void BlendFunc_Chamfer::Section(const Standard_Real /*Param*/,
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

Standard_Boolean BlendFunc_Chamfer::IsRational() const
{
  return Standard_False;
}

//=======================================================================
//function : GetSectionSize
//purpose  : Non implementee (non necessaire car non rationel)
//=======================================================================
Standard_Real BlendFunc_Chamfer::GetSectionSize() const 
{
  throw Standard_NotImplemented("BlendFunc_Chamfer::GetSectionSize()");
}

//=======================================================================
//function : GetMinimalWeight
//purpose  : 
//=======================================================================
void BlendFunc_Chamfer::GetMinimalWeight(TColStd_Array1OfReal& Weights) const 
{
  Weights.Init(1);
}


//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================

Standard_Integer BlendFunc_Chamfer::NbIntervals (const GeomAbs_Shape S) const
{
  return  curv->NbIntervals(BlendFunc::NextShape(S));
}


//=======================================================================
//function : Intervals
//purpose  : 
//=======================================================================

void BlendFunc_Chamfer::Intervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const
{
  curv->Intervals(T, BlendFunc::NextShape(S)); 
}

//=======================================================================
//function : GetShape
//purpose  : 
//=======================================================================

void BlendFunc_Chamfer::GetShape (Standard_Integer& NbPoles,
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
void BlendFunc_Chamfer::GetTolerance(const Standard_Real BoundTol, 
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

void BlendFunc_Chamfer::Knots(TColStd_Array1OfReal& TKnots)
{
  TKnots(1) = 0.;
  TKnots(2) = 1.;
}


//=======================================================================
//function : Mults
//purpose  : 
//=======================================================================

void BlendFunc_Chamfer::Mults(TColStd_Array1OfInteger& TMults)
{
  TMults(1) = 2;
  TMults(2) = 2;
}

//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_Chamfer::Section
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

Standard_Boolean BlendFunc_Chamfer::Section
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

void BlendFunc_Chamfer::Section(const Blend_Point& P,
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

void BlendFunc_Chamfer::Resolution(const Standard_Integer IC2d, const Standard_Real Tol,
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
