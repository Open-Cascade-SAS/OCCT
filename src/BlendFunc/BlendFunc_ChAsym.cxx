// Created on: 1998-06-02
// Created by: Philippe NOUAILLE
// Copyright (c) 1998-1999 Matra Datavision
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
#include <Adaptor3d_HSurface.hxx>
#include <Blend_Point.hxx>
#include <BlendFunc.hxx>
#include <BlendFunc_ChAsym.hxx>
#include <ElCLib.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <math_Gauss.hxx>
#include <math_Matrix.hxx>
#include <math_SVD.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>

//=======================================================================
//function : BlendFunc_ChAsym
//purpose  : 
//=======================================================================
BlendFunc_ChAsym::BlendFunc_ChAsym(const Handle(Adaptor3d_HSurface)& S1,
                                   const Handle(Adaptor3d_HSurface)& S2,
                                   const Handle(Adaptor3d_HCurve)& C) :
    surf1(S1),surf2(S2),
    curv(C), tcurv(C),
    FX(1, 4),
    DX(1, 4, 1, 4),
    istangent(Standard_True),
    distmin(RealLast())
{ 
}


//=======================================================================
//function : NbEquations
//purpose  : 
//=======================================================================

Standard_Integer BlendFunc_ChAsym::NbEquations () const
{
  return 4;
}



//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BlendFunc_ChAsym::Set(const Standard_Real Param)
{
  param = Param;
}

//=======================================================================
//function : Set
//purpose  : Segmente la courbe a sa partie utile.
//           La precision est prise arbitrairement petite !?
//=======================================================================

void BlendFunc_ChAsym::Set(const Standard_Real First, const Standard_Real Last)
{
  tcurv = curv->Trim(First, Last, 1.e-12);
}

//=======================================================================
//function : GetTolerance
//purpose  : 
//=======================================================================

void BlendFunc_ChAsym::GetTolerance(math_Vector& Tolerance, const Standard_Real Tol) const
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

void BlendFunc_ChAsym::GetBounds(math_Vector& InfBound, math_Vector& SupBound) const
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

Standard_Boolean BlendFunc_ChAsym::IsSolution(const math_Vector& Sol, const Standard_Real Tol)
{
  math_Vector valsol(1, 4), secmember(1, 4);
  math_Matrix gradsol(1, 4, 1, 4);

  gp_Pnt ptgui;
  gp_Vec np, dnp, d1gui, d2gui, Nsurf1, dwtsurf1;
  gp_Vec d1u1, d1v1, d1u2, d1v2;  
  Standard_Real Normg;

  tcurv->D2(param, ptgui, d1gui, d2gui);
  Normg  = d1gui.Magnitude(); 
  np     = d1gui.Normalized();
  dnp    = (d2gui - np.Dot(d2gui) * np) / Normg;

 if (choix%2 != 0) {
    np.Reverse();
    dnp.Reverse();
    Normg = -Normg;
  }

  surf1->D1(Sol(1), Sol(2), pt1, d1u1, d1v1);
  Nsurf1   = d1u1.Crossed(d1v1);
  tsurf1   = Nsurf1.Crossed(np);
  dwtsurf1 = Nsurf1.Crossed(dnp);

  surf2->D1(Sol(3), Sol(4), pt2, d1u2, d1v2);

  gp_Vec pguis1(ptgui, pt1), pguis2(ptgui, pt2);
  gp_Vec CrossVec, s1s2(pt1, pt2);
  Standard_Real PScaInv = 1. / tsurf1.Dot(s1s2), F4, temp;   
  Standard_Real maxpiv  = 1.e-9;
  Standard_Real Nordu1 = d1u1.Magnitude(),
                Nordv1 = d1v1.Magnitude();

  temp = 2. * (Nordu1 + Nordv1) * s1s2.Magnitude() + 2. * Nordu1 * Nordv1;

  Values(Sol, valsol, gradsol);

  if (Abs(valsol(1)) < Tol &&
      Abs(valsol(2)) < Tol &&
      Abs(valsol(3)) < 2. * dist1 * Tol  &&
      Abs(valsol(4)) < Tol * (1. + tgang) * Abs(PScaInv) * temp) {

    secmember(1) = Normg - dnp.Dot(pguis1);
    secmember(2) = Normg - dnp.Dot(pguis2);
    secmember(3) = - 2. * d1gui.Dot(pguis1); 

    CrossVec     = tsurf1.Crossed(s1s2);
    F4           = np.Dot(CrossVec) * PScaInv;
    temp         = dnp.Dot(CrossVec) + np.Dot(dwtsurf1.Crossed(s1s2));

    temp        -= F4 * dwtsurf1.Dot(s1s2);
    secmember(4) = PScaInv * temp;

    math_Gauss Resol(gradsol, maxpiv);

    if (Resol.IsDone()) {
      Resol.Solve(secmember);
      istangent = Standard_False;
    }
    else {
      math_SVD SingRS (gradsol);
      if (SingRS.IsDone()) {
        math_Vector DEDT(1,4);
        DEDT = secmember;
        SingRS.Solve(DEDT, secmember, 1.e-6);
        istangent = Standard_False;
      }
      else istangent = Standard_True;    
    }

    if (!istangent) {
      tg1.SetLinearForm(secmember(1), d1u1, secmember(2), d1v1);
      tg2.SetLinearForm(secmember(3), d1u2, secmember(4), d1v2);
      tg12d.SetCoord(secmember(1),secmember(2));
      tg22d.SetCoord(secmember(3),secmember(4));
    }

    distmin = Min( distmin, pt1.Distance(pt2));

    return Standard_True;
  }

  istangent = Standard_True;
  return Standard_False;  
}


//=======================================================================
//function : GetMinimalDistance
//purpose  : 
//=======================================================================

Standard_Real BlendFunc_ChAsym::GetMinimalDistance() const
{
  return distmin;
}

//=======================================================================
//function : ComputeValues
//purpose  : 
//=======================================================================
Standard_Boolean BlendFunc_ChAsym::ComputeValues(const math_Vector& X,
                                                 const Standard_Integer DegF,
                                                 const Standard_Integer DegL)
{
  if (DegF > DegL) return Standard_False;

  gp_Vec np, d1gui, d1u1, d1v1, d2u1, d2v1, d2uv1, d1u2, d1v2, Nsurf1;
  gp_Pnt ptgui;  
  Standard_Real PScaInv, F4; 

  tcurv->D1(param, ptgui, d1gui);
  nplan  = d1gui.Normalized();
  np     = nplan;

  if (choix%2 != 0) np.Reverse();

  if ( (DegF == 0) && (DegL == 0) ) {
    surf1->D1(X(1), X(2), pt1, d1u1, d1v1);
    pt2   = surf2->Value(X(3), X(4));
  }
  else { 
    surf1->D2(X(1), X(2), pt1, d1u1, d1v1, d2u1, d2v1, d2uv1);
    surf2->D1(X(3), X(4), pt2, d1u2, d1v2);
  }

  Nsurf1 = d1u1.Crossed(d1v1);
  tsurf1 = Nsurf1.Crossed(np);

  gp_Vec nps1(ptgui, pt1), s1s2(pt1, pt2);//, tempVec;     
  PScaInv = 1. /  tsurf1.Dot(s1s2);
  F4      = np.Dot(tsurf1.Crossed(s1s2)) * PScaInv;

  if (DegF == 0) {
    Standard_Real Dist;
    Dist    = ptgui.XYZ().Dot(np.XYZ());

    FX(1) = pt1.XYZ().Dot(np.XYZ()) - Dist;
    FX(2) = pt2.XYZ().Dot(np.XYZ()) - Dist;
    FX(3) = dist1 * dist1 - nps1.SquareMagnitude(); 
    FX(4) = tgang - F4;
  }

  if (DegL == 1) { 
    Standard_Real temp;
    gp_Vec tempVec;
    gp_Vec d1utsurf1, d1vtsurf1; 
    d1utsurf1 = (d2u1.Crossed(d1v1) + d1u1.Crossed(d2uv1)).Crossed(np);
    d1vtsurf1 = (d2uv1.Crossed(d1v1) + d1u1.Crossed(d2v1)).Crossed(np);
    
    DX(1, 1) = np.Dot(d1u1);
    DX(1, 2) = np.Dot(d1v1);
    DX(1, 3) = 0.;
    DX(1, 4) = 0.;
    
    DX(2, 1) = 0.;
    DX(2, 2) = 0.;
    DX(2, 3) = np.Dot(d1u2);
    DX(2, 4) =  np.Dot(d1v2);
    
    tempVec  = -2. * nps1;
    DX(3, 1) = d1u1.Dot(tempVec);
    DX(3, 2) = d1v1.Dot(tempVec);
    DX(3, 3) = 0.;
    DX(3, 4) = 0.;
 
    temp     = F4 * (d1utsurf1.Dot(s1s2) - tsurf1.Dot(d1u1));
    temp    += np.Dot(tsurf1.Crossed(d1u1) - d1utsurf1.Crossed(s1s2));
    DX(4, 1) = temp * PScaInv;
    
    temp     = F4 * (d1vtsurf1.Dot(s1s2) - tsurf1.Dot(d1v1));
    temp    += np.Dot(tsurf1.Crossed(d1v1) - d1vtsurf1.Crossed(s1s2));
    DX(4, 2) = temp * PScaInv;
    
    temp     = F4 * tsurf1.Dot(d1u2) - np.Dot(tsurf1.Crossed(d1u2));
    DX(4, 3) = temp * PScaInv;

    temp     = F4 * tsurf1.Dot(d1v2) - np.Dot(tsurf1.Crossed(d1v2));
    DX(4, 4) = temp * PScaInv;  
  }

  return Standard_True;

}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ChAsym::Value(const math_Vector& X, math_Vector& F)
{
  const Standard_Boolean Error = ComputeValues(X, 0, 0);
  F = FX;
  return Error;
}


//=======================================================================
//function : Derivatives
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ChAsym::Derivatives(const math_Vector& X, math_Matrix& D)
{
  const Standard_Boolean Error =  ComputeValues(X, 1, 1);
  D = DX;
  return Error;
}


//=======================================================================
//function : Values
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ChAsym::Values(const math_Vector& X, math_Vector& F, math_Matrix& D)
{
  const Standard_Boolean Error = ComputeValues(X, 0, 1);
  F = FX;
  D = DX;
  return Error;
}


//=======================================================================
//function : PointOnS1
//purpose  : 
//=======================================================================

const gp_Pnt& BlendFunc_ChAsym::PointOnS1 () const
{
  return pt1;
}


//=======================================================================
//function : PointOnS2
//purpose  : 
//=======================================================================

const gp_Pnt& BlendFunc_ChAsym::PointOnS2 () const
{
  return pt2;
}


//=======================================================================
//function : IsTangencyPoint
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ChAsym::IsTangencyPoint () const
{
  return istangent;
}


//=======================================================================
//function : TangentOnS1
//purpose  : 
//=======================================================================

const gp_Vec& BlendFunc_ChAsym::TangentOnS1 () const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_ChAsym::TangentOnS1");
  return tg1;
}


//=======================================================================
//function : Tangent2dOnS1
//purpose  : 
//=======================================================================

const gp_Vec2d& BlendFunc_ChAsym::Tangent2dOnS1 () const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_ChAsym::Tangent2dOnS1");
  return tg12d;
}

//=======================================================================
//function : TangentOnS2
//purpose  : 
//=======================================================================

const gp_Vec& BlendFunc_ChAsym::TangentOnS2 () const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_ChAsym::TangentOnS2");
  return tg2;
}


//=======================================================================
//function : Tangent2dOnS2
//purpose  : 
//=======================================================================

const gp_Vec2d& BlendFunc_ChAsym::Tangent2dOnS2 () const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_ChAsym::Tangent2dOnS2");
  return tg22d;
}


//=======================================================================
//function : TwistOnS1
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ChAsym::TwistOnS1() const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_ChAsym::TwistOnS1");
  return tg1.Dot(nplan) < 0.;
}

//=======================================================================
//function : TwistOnS2
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ChAsym::TwistOnS2() const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_ChAsym::TwistOnS2");
  return tg2.Dot(nplan) < 0.;
}

//=======================================================================
//function : Tangent
//purpose  : TgF,NmF et TgL,NmL les tangentes et normales respectives
//           aux surfaces S1 et S2 
//=======================================================================

void BlendFunc_ChAsym::Tangent(const Standard_Real U1,
			       const Standard_Real V1,
			       const Standard_Real U2,
			       const Standard_Real V2,
			       gp_Vec& TgF,
			       gp_Vec& TgL,
			       gp_Vec& NmF,
			       gp_Vec& NmL) const
{
  gp_Pnt Pt1,Pt2,ptgui;
  gp_Vec d1u1,d1v1,d1u2,d1v2;
  gp_Vec np, d1gui;
  Standard_Boolean revF = Standard_False;
  Standard_Boolean revL = Standard_False;

  tcurv->D1(param, ptgui, d1gui);
  np = d1gui.Normalized();

  surf1->D1(U1,V1,Pt1,d1u1,d1v1);
  NmF = d1u1.Crossed(d1v1);

  surf2->D1(U2,V2,Pt2,d1u2,d1v2);
  NmL = d1u2.Crossed(d1v2);

  TgF = (np.Crossed(NmF)).Normalized();
  TgL = (np.Crossed(NmL)).Normalized();

  if ( (choix == 2)||(choix == 5) ){
    revF = Standard_True;
    revL = Standard_True;
  }

  if ( (choix == 4)||(choix == 7) )
    revL = Standard_True;

  if ( (choix == 3)||(choix == 8) )
    revF = Standard_True;

  if ( revF )
    TgF.Reverse();
  if ( revL )
    TgL.Reverse();
}



//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

void BlendFunc_ChAsym::Section(const Standard_Real /*Param*/,
			       const Standard_Real U1,
			       const Standard_Real V1,
			       const Standard_Real U2,
			       const Standard_Real V2,
			       Standard_Real& Pdeb,
			       Standard_Real& Pfin,
			       gp_Lin& C)
{
  const gp_Pnt Pt1 = surf1->Value(U1,V1);
  const gp_Pnt Pt2 = surf2->Value(U2,V2);
  const gp_Dir dir( gp_Vec(Pt1,Pt2) );

  C.SetLocation(Pt1);
  C.SetDirection(dir);

  Pdeb = 0.;
  Pfin = ElCLib::Parameter(C, Pt2);
}


//=======================================================================
//function : IsRational
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ChAsym::IsRational () const
{
  return Standard_False; 
}

//=======================================================================
//function : GetSectionSize
//purpose  : 
//=======================================================================
Standard_Real BlendFunc_ChAsym::GetSectionSize() const 
{
  throw Standard_NotImplemented("BlendFunc_ChAsym::GetSectionSize()");
}

//=======================================================================
//function : GetMinimalWeight
//purpose  : 
//=======================================================================
void BlendFunc_ChAsym::GetMinimalWeight(TColStd_Array1OfReal& Weights) const 
{
  Weights.Init(1);
}

//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================

Standard_Integer BlendFunc_ChAsym::NbIntervals (const GeomAbs_Shape S) const
{
  return curv->NbIntervals(BlendFunc::NextShape(S));
}


//=======================================================================
//function : Intervals
//purpose  : 
//=======================================================================

void BlendFunc_ChAsym::Intervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const
{
  curv->Intervals(T, BlendFunc::NextShape(S));
}

//=======================================================================
//function : GetShape
//purpose  : 
//=======================================================================

void BlendFunc_ChAsym::GetShape (Standard_Integer& NbPoles,
				 Standard_Integer& NbKnots,
				 Standard_Integer& Degree,
				 Standard_Integer& NbPoles2d)
{
  NbPoles   = 2;
  NbPoles2d = 2;
  NbKnots   = 2;
  Degree    = 1;
}

//=======================================================================
//function : GetTolerance
//purpose  : Determine les Tolerances a utiliser dans les approximations.
//=======================================================================
void BlendFunc_ChAsym::GetTolerance(const Standard_Real BoundTol, 
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

void BlendFunc_ChAsym::Knots(TColStd_Array1OfReal& TKnots)
{
  TKnots(1) = 0.;
  TKnots(2) = 1.;
}


//=======================================================================
//function : Mults
//purpose  : 
//=======================================================================

void BlendFunc_ChAsym::Mults(TColStd_Array1OfInteger& TMults)
{
  TMults(1) = 2;
  TMults(2) = 2;
}


//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

void BlendFunc_ChAsym::Section(const Blend_Point& P,
			       TColgp_Array1OfPnt& Poles,
			       TColgp_Array1OfPnt2d& Poles2d,
			       TColStd_Array1OfReal& Weights)
{
  Standard_Real u1, v1, u2, v2, prm = P.Parameter();
  Standard_Integer low = Poles.Lower();
  Standard_Integer upp = Poles.Upper();
  math_Vector X(1,4), F(1,4);

  P.ParametersOnS1(u1, v1);
  P.ParametersOnS2(u2, v2);
  X(1) = u1;
  X(2) = v1;
  X(3) = u2;
  X(4) = v2;
  Poles2d(Poles2d.Lower()).SetCoord(u1,v1);
  Poles2d(Poles2d.Upper()).SetCoord(u2,v2);

  Set(prm);
  Value(X,F);
  Poles(low)   = PointOnS1();
  Poles(upp)   = PointOnS2();
  Weights(low) = 1.0;
  Weights(upp) = 1.0;
}


//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ChAsym::Section
  (const Blend_Point& P,
   TColgp_Array1OfPnt& Poles,
   TColgp_Array1OfVec& DPoles,
   TColgp_Array1OfPnt2d& Poles2d,
   TColgp_Array1OfVec2d& DPoles2d,
   TColStd_Array1OfReal& Weights,
   TColStd_Array1OfReal& DWeights)
{
  math_Vector Sol(1, 4), valsol(1, 4), secmember(1, 4);
  math_Matrix gradsol(1, 4, 1, 4);
  Standard_Real    prm = P.Parameter();
  Standard_Integer low = Poles.Lower();
  Standard_Integer upp = Poles.Upper();

  P.ParametersOnS1(Sol(1),Sol(2));
  P.ParametersOnS2(Sol(3),Sol(4));
  Set(prm);
  
  Poles2d(Poles2d.Lower()).SetCoord(Sol(1),Sol(2));
  Poles2d(Poles2d.Upper()).SetCoord(Sol(3),Sol(4));
  Poles(low)   = PointOnS1();
  Poles(upp)   = PointOnS2();
  Weights(low) = 1.0;
  Weights(upp) = 1.0;

  gp_Pnt ptgui;
  gp_Vec np, dnp, d1gui, d2gui, Nsurf1, dwtsurf1;
  gp_Vec d1u1, d1v1, d1u2, d1v2;  
  Standard_Real Normg;

  tcurv->D2(param, ptgui, d1gui, d2gui);
  Normg  = d1gui.Magnitude(); 
  np     = d1gui.Normalized();
  dnp    = (d2gui - np.Dot(d2gui) * np) / Normg;

  if (choix%2 != 0) {
     np.Reverse();
     dnp.Reverse();
     Normg = -Normg;
  }

  surf1->D1(Sol(1), Sol(2), pt1, d1u1, d1v1);
  Nsurf1   = d1u1.Crossed(d1v1);
  tsurf1   = Nsurf1.Crossed(np);
  dwtsurf1 = Nsurf1.Crossed(dnp);

  surf2->D1(Sol(3), Sol(4), pt2, d1u2, d1v2);

  gp_Vec pguis1(ptgui, pt1), pguis2(ptgui, pt2);
  gp_Vec CrossVec, s1s2(pt1, pt2);
  Standard_Real PScaInv = 1. / tsurf1.Dot(s1s2), F4, temp;   
  Standard_Real maxpiv  = 1.e-9;
  Standard_Real Nordu1 = d1u1.Magnitude(),
                Nordv1 = d1v1.Magnitude();

  temp = 2. * (Nordu1 + Nordv1) * s1s2.Magnitude() + 2. * Nordu1 * Nordv1;

  Values(Sol, valsol, gradsol);

  secmember(1) = Normg - dnp.Dot(pguis1);
  secmember(2) = Normg - dnp.Dot(pguis2);
  secmember(3) = - 2. * d1gui.Dot(pguis1); 

  CrossVec     = tsurf1.Crossed(s1s2);
  F4           = np.Dot(CrossVec) * PScaInv;
  temp         = dnp.Dot(CrossVec) + np.Dot(dwtsurf1.Crossed(s1s2));
  temp        -= F4 * dwtsurf1.Dot(s1s2);
  secmember(4) = PScaInv * temp;

  math_Gauss Resol(gradsol, maxpiv);

  if (Resol.IsDone()) {
    Resol.Solve(secmember);
    istangent = Standard_False;
  }
  else {
    math_SVD SingRS (gradsol);
    if (SingRS.IsDone()) {
      math_Vector DEDT(1,4);
      DEDT = secmember;
      SingRS.Solve(DEDT, secmember, 1.e-6);
      istangent = Standard_False;
    }
    else istangent = Standard_True;    
  }

  if (!istangent) {
    tg1.SetLinearForm(secmember(1), d1u1, secmember(2), d1v1);
    tg2.SetLinearForm(secmember(3), d1u2, secmember(4), d1v2);
    tg12d.SetCoord(secmember(1),secmember(2));
    tg22d.SetCoord(secmember(3),secmember(4));
  }

  distmin = Min( distmin, pt1.Distance(pt2));

  if (!istangent) {
    DPoles2d(Poles2d.Lower()).SetCoord(Tangent2dOnS1().X(),
	                      	       Tangent2dOnS1().Y());
    DPoles2d(Poles2d.Upper()).SetCoord(Tangent2dOnS2().X(),
				       Tangent2dOnS2().Y());

    DPoles(low)   = TangentOnS1();
    DPoles(upp)   = TangentOnS2();
    DWeights(low) = 0.0;
    DWeights(upp) = 0.0;  
  }

  return (!istangent);

}


//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ChAsym::Section
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
//function : Resolution
//purpose  : 
//=======================================================================
void BlendFunc_ChAsym::Resolution(const Standard_Integer IC2d, const Standard_Real Tol,
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


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
void BlendFunc_ChAsym::Set(const Standard_Real Dist1,
			   const Standard_Real Angle,
			   const Standard_Integer Choix)
{
  dist1 = Abs(Dist1);
  angle = Angle;
  tgang = Tan(Angle);
  choix = Choix;
}
