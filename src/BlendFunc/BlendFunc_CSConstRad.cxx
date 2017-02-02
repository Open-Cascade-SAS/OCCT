// Created on: 1993-12-02
// Created by: Jacques GOUSSARD
// Copyright (c) 1993-1999 Matra Datavision
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

// Modified 10/09/1996 PMN Ajout de (Nb)Intervalles, IsRationnal 
//                       + Utilisation de GeomFill::GetCircle dans Section.

#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Blend_Point.hxx>
#include <BlendFunc.hxx>
#include <BlendFunc_CSConstRad.hxx>
#include <ElCLib.hxx>
#include <GeomFill.hxx>
#include <gp.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <math_Gauss.hxx>
#include <math_Matrix.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>

//=======================================================================
//function : BlendFunc_CSConstRad
//purpose  : 
//=======================================================================
BlendFunc_CSConstRad::BlendFunc_CSConstRad(const Handle(Adaptor3d_HSurface)& S,
                                           const Handle(Adaptor3d_HCurve)& C,
                                           const Handle(Adaptor3d_HCurve)& CG) :

       surf(S),curv(C),guide(CG),istangent(Standard_True),
       maxang(RealFirst()), minang(RealLast()),mySShape(BlendFunc_Rational)
{}


//=======================================================================
//function : NbEquations
//purpose  : 
//=======================================================================

Standard_Integer BlendFunc_CSConstRad::NbEquations () const
{
  return 3;
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BlendFunc_CSConstRad::Set(const Standard_Real Radius, const Standard_Integer Choix)
{
  choix = Choix;
  ray   = -Abs(Radius);
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BlendFunc_CSConstRad::Set(const BlendFunc_SectionShape TypeSection)
{
  mySShape = TypeSection;
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BlendFunc_CSConstRad::Set(const Standard_Real Param)
{
  guide->D2(Param,ptgui,d1gui,d2gui);
  normtg = d1gui.Magnitude();
  nplan  = d1gui.Normalized();
  theD = - (nplan.XYZ().Dot(ptgui.XYZ()));
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BlendFunc_CSConstRad::Set(const Standard_Real, const Standard_Real)
{
  throw Standard_NotImplemented("BlendFunc_CSConstRad::Set");
}


//=======================================================================
//function : GetTolerance
//purpose  : 
//=======================================================================

void BlendFunc_CSConstRad::GetTolerance(math_Vector& Tolerance, const Standard_Real Tol) const
{
  Tolerance(1) = surf->UResolution(Tol);
  Tolerance(2) = surf->VResolution(Tol);
  Tolerance(3) = curv->Resolution(Tol);
}


//=======================================================================
//function : GetBounds
//purpose  : 
//=======================================================================

void BlendFunc_CSConstRad::GetBounds(math_Vector& InfBound, math_Vector& SupBound) const
{
  InfBound(1) = surf->FirstUParameter();
  InfBound(2) = surf->FirstVParameter();
  InfBound(3) = curv->FirstParameter();
  SupBound(1) = surf->LastUParameter();
  SupBound(2) = surf->LastVParameter();
  SupBound(3) = curv->LastParameter();

  if(!Precision::IsInfinite(InfBound(1)) &&
     !Precision::IsInfinite(SupBound(1))) {
    const Standard_Real range = (SupBound(1) - InfBound(1));
    InfBound(1) -= range;
    SupBound(1) += range;
  }
  if(!Precision::IsInfinite(InfBound(2)) &&
     !Precision::IsInfinite(SupBound(2))) {
    const Standard_Real range = (SupBound(2) - InfBound(2));
    InfBound(2) -= range;
    SupBound(2) += range;
  }
}


//=======================================================================
//function : IsSolution
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_CSConstRad::IsSolution(const math_Vector& Sol, const Standard_Real Tol)
{
  math_Vector valsol(1,3),secmember(1,3);
  math_Matrix gradsol(1,3,1,3);

  gp_Vec dnplan,d1u1,d1v1,d1,temp,ns,ns2,ncrossns,resul;
  Standard_Real norm,ndotns,grosterme;
  Standard_Real Cosa,Sina,Angle;

  Values(Sol,valsol,gradsol);
  if (Abs(valsol(1)) <= Tol &&
      Abs(valsol(2)) <= Tol &&
      Abs(valsol(3)) <= Tol*Tol) {

    // Calcul des tangentes

    pt2d = gp_Pnt2d(Sol(1),Sol(2));
    prmc = Sol(3);
    surf->D1(Sol(1),Sol(2),pts,d1u1,d1v1);
    curv->D1(Sol(3),ptc,d1);
    dnplan.SetLinearForm(1./normtg,d2gui,
			 -1./normtg*(nplan.Dot(d2gui)),nplan);

    temp.SetXYZ(pts.XYZ() - ptgui.XYZ());
    secmember(1) = normtg - dnplan.Dot(temp);

    temp.SetXYZ(ptc.XYZ() - ptgui.XYZ());
    secmember(2) = normtg - dnplan.Dot(temp);

    ns = d1u1.Crossed(d1v1);
    ncrossns = nplan.Crossed(ns);
    ndotns = nplan.Dot(ns);
    norm = ncrossns.Magnitude();

    grosterme = ncrossns.Dot(dnplan.Crossed(ns))/norm/norm;
    temp.SetLinearForm(ray/norm*(dnplan.Dot(ns)-grosterme*ndotns),nplan,
		       ray*ndotns/norm,dnplan,
		       ray*grosterme/norm,ns);

    ns.SetLinearForm(ndotns/norm,nplan, -1./norm,ns);
    resul.SetLinearForm(ray,ns,gp_Vec(ptc,pts));
    secmember(3) = -2.*(temp.Dot(resul));

    math_Gauss Resol(gradsol);
    if (Resol.IsDone()) {

      Resol.Solve(secmember);

      tgs.SetLinearForm(secmember(1),d1u1,secmember(2),d1v1);
      tgc = secmember(3)*d1;
      tg2d.SetCoord(secmember(1),secmember(2));
      istangent = Standard_False;
    }
    else {
      istangent = Standard_True;
    }
    // mise a jour de maxang

    ns2 = -resul.Normalized();

    Cosa = ns.Dot(ns2);
    Sina = nplan.Dot(ns.Crossed(ns2));
    if (choix%2 != 0) {
      Sina = -Sina;  //nplan est change en -nplan
    }

    Angle = ACos(Cosa);
    if (Sina <0.) {
      Angle = 2.*M_PI - Angle;
    }

   if (Angle>maxang) {maxang = Angle;}
   if (Angle<minang) {minang = Angle;}

    return Standard_True;
  }
  istangent = Standard_True;
  return Standard_False;
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_CSConstRad::Value(const math_Vector& X, math_Vector& F)
{
  gp_Vec d1u1,d1v1;
  surf->D1(X(1),X(2),pts,d1u1,d1v1);
  ptc = curv->Value(X(3));

  F(1) = nplan.XYZ().Dot(pts.XYZ()) + theD;
  F(2) = nplan.XYZ().Dot(ptc.XYZ()) + theD;

  gp_Vec vref, ns = d1u1.Crossed(d1v1);
  const Standard_Real norm = nplan.Crossed(ns).Magnitude();
  ns.SetLinearForm(nplan.Dot(ns)/norm,nplan, -1./norm,ns);
  vref.SetLinearForm(ray,ns,gp_Vec(ptc,pts));

  F(3) = vref.SquareMagnitude() - ray*ray;

  pt2d = gp_Pnt2d(X(1),X(2));
  return Standard_True;
}


//=======================================================================
//function : Derivatives
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_CSConstRad::Derivatives(const math_Vector& X, math_Matrix& D)
{
  gp_Vec d1u1,d1v1,d2u1,d2v1,d2uv1,d1;
  gp_Vec ns,ncrossns,resul,temp, vref;

  Standard_Real norm,ndotns,grosterme;

  surf->D2(X(1),X(2),pts,d1u1,d1v1,d2u1,d2v1,d2uv1);
  curv->D1(X(3),ptc,d1);

  D(1,1) = nplan.Dot(d1u1);
  D(1,2) = nplan.Dot(d1v1);
  D(1,3) = 0.;

  D(2,1) = 0.;
  D(2,2) = 0.;
  D(2,3) = nplan.Dot(d1);


  ns = d1u1.Crossed(d1v1);
  ncrossns = nplan.Crossed(ns);
  norm = ncrossns.Magnitude();
  ndotns = nplan.Dot(ns);

  vref.SetLinearForm(ndotns,nplan,-1.,ns);
  vref.Divide(norm);
  vref.SetLinearForm(ray,vref,gp_Vec(ptc,pts));

   // Derivee par rapport a u1
  temp = d2u1.Crossed(d1v1).Added(d1u1.Crossed(d2uv1));
  grosterme = ncrossns.Dot(nplan.Crossed(temp))/norm/norm;
  resul.SetLinearForm(-ray/norm*(grosterme*ndotns-nplan.Dot(temp)),nplan,
		      ray*grosterme/norm,ns,
		      -ray/norm,temp,
		      d1u1);

  D(3,1) = 2.*(resul.Dot(vref));


  // Derivee par rapport a v1
  temp = d2uv1.Crossed(d1v1).Added(d1u1.Crossed(d2v1));
  grosterme = ncrossns.Dot(nplan.Crossed(temp))/norm/norm;
  resul.SetLinearForm(-ray/norm*(grosterme*ndotns-nplan.Dot(temp)),nplan,
		      ray*grosterme/norm,ns,
		      -ray/norm,temp,
		      d1v1);

  D(3,2) = 2.*(resul.Dot(vref));

  D(3,3) = -2.*(d1.Dot(vref));

  pt2d = gp_Pnt2d(X(1),X(2));
  return Standard_True;
}


//=======================================================================
//function : Values
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_CSConstRad::Values(const math_Vector& X, math_Vector& F, math_Matrix& D)
{
  gp_Vec d1u1,d1v1,d1;
  gp_Vec d2u1,d2v1,d2uv1;
  gp_Vec ns,ncrossns,resul,temp,vref;

  Standard_Real norm,ndotns,grosterme;

  surf->D2(X(1),X(2),pts,d1u1,d1v1,d2u1,d2v1,d2uv1);
  curv->D1(X(3),ptc,d1);

  F(1) = nplan.XYZ().Dot(pts.XYZ()) + theD;
  F(2) = nplan.XYZ().Dot(ptc.XYZ()) + theD;

  D(1,1) = nplan.Dot(d1u1);
  D(1,2) = nplan.Dot(d1v1);
  D(1,3) = 0.;

  D(2,1) = 0.;
  D(2,2) = 0.;
  D(2,3) = nplan.Dot(d1);


  ns = d1u1.Crossed(d1v1);
  ncrossns = nplan.Crossed(ns);
  norm = ncrossns.Magnitude();
  ndotns = nplan.Dot(ns);

  vref.SetLinearForm(ndotns,nplan,-1.,ns);
  vref.Divide(norm);
  vref.SetLinearForm(ray,vref,gp_Vec(ptc,pts));

  F(3) = vref.SquareMagnitude() - ray*ray;


   // Derivee par rapport a u1
  temp = d2u1.Crossed(d1v1).Added(d1u1.Crossed(d2uv1));
  grosterme = ncrossns.Dot(nplan.Crossed(temp))/norm/norm;
  resul.SetLinearForm(-ray/norm*(grosterme*ndotns-nplan.Dot(temp)),nplan,
		      ray*grosterme/norm,ns,
		      -ray/norm,temp,
		      d1u1);

  D(3,1) = 2.*(resul.Dot(vref));


  // Derivee par rapport a v1
  temp = d2uv1.Crossed(d1v1).Added(d1u1.Crossed(d2v1));
  grosterme = ncrossns.Dot(nplan.Crossed(temp))/norm/norm;
  resul.SetLinearForm(-ray/norm*(grosterme*ndotns-nplan.Dot(temp)),nplan,
		      ray*grosterme/norm,ns,
		      -ray/norm,temp,
		      d1v1);

  D(3,2) = 2.*(resul.Dot(vref));

  D(3,3) = -2.*(d1.Dot(vref));

  pt2d = gp_Pnt2d(X(1),X(2));
  return Standard_True;
}



//=======================================================================
//function : PointOnS
//purpose  : 
//=======================================================================

const gp_Pnt& BlendFunc_CSConstRad::PointOnS () const
{
  return pts;
}

//=======================================================================
//function : PointOnC
//purpose  : 
//=======================================================================

const gp_Pnt& BlendFunc_CSConstRad::PointOnC () const
{
  return ptc;
}

//=======================================================================
//function : Pnt2d
//purpose  : 
//=======================================================================

const gp_Pnt2d& BlendFunc_CSConstRad::Pnt2d () const
{
  return pt2d;
}

//=======================================================================
//function : ParameterOnC
//purpose  : 
//=======================================================================

Standard_Real BlendFunc_CSConstRad::ParameterOnC () const
{
  return prmc;
}


//=======================================================================
//function : IsTangencyPoint
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_CSConstRad::IsTangencyPoint () const
{
  return istangent;
}

//=======================================================================
//function : TangentOnS
//purpose  : 
//=======================================================================

const gp_Vec& BlendFunc_CSConstRad::TangentOnS () const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_CSConstRad::TangentOnS");
  return tgs;
}

//=======================================================================
//function : TangentOnC
//purpose  : 
//=======================================================================

const gp_Vec& BlendFunc_CSConstRad::TangentOnC () const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_CSConstRad::TangentOnC");
  return tgc;
}

//=======================================================================
//function : Tangent2d
//purpose  : 
//=======================================================================

const gp_Vec2d& BlendFunc_CSConstRad::Tangent2d () const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_CSConstRad::Tangent2d");
  return tg2d;
}


//=======================================================================
//function : Tangent
//purpose  : 
//=======================================================================

void BlendFunc_CSConstRad::Tangent(const Standard_Real U, const Standard_Real V,
                                   gp_Vec& TgS, gp_Vec& NmS) const
{
  gp_Pnt bid;
  gp_Vec d1u,d1v;
  surf->D1(U,V,bid,d1u,d1v);

  gp_Vec ns;
  NmS = ns = d1u.Crossed(d1v);
  
  const Standard_Real norm = nplan.Crossed(ns).Magnitude();
  ns.SetLinearForm(nplan.Dot(ns)/norm,nplan, -1./norm,ns);

  gp_Pnt Center(bid.XYZ()+ray*ns.XYZ());
  TgS = nplan.Crossed(gp_Vec(Center,bid));
  if (choix%2 == 1)
    TgS.Reverse();
}


//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

void BlendFunc_CSConstRad::Section(const Standard_Real Param,
				   const Standard_Real U,
				   const Standard_Real V,
				   const Standard_Real W,
				   Standard_Real& Pdeb,
				   Standard_Real& Pfin,
				   gp_Circ& C)
{
  gp_Vec d1u1,d1v1;
  gp_Vec ns;
  Standard_Real norm;
  gp_Pnt Center;

  guide->D1(Param,ptgui,d1gui);
  nplan  = d1gui.Normalized();

  surf->D1(U,V,pts,d1u1,d1v1);
  ptc = curv->Value(W);

  ns = d1u1.Crossed(d1v1);
  
  norm = nplan.Crossed(ns).Magnitude();
  ns.SetLinearForm(nplan.Dot(ns)/norm,nplan, -1./norm,ns);
  Center.SetXYZ(pts.XYZ()+ray*ns.XYZ());
  C.SetRadius(Abs(ray));

  if (choix%2 == 0) {
    C.SetPosition(gp_Ax2(Center,nplan,ns));
  }
  else {
    C.SetPosition(gp_Ax2(Center,nplan.Reversed(),ns));
  }
  Pdeb = 0.;
  Pfin = ElCLib::Parameter(C,ptc);
}

Standard_Boolean BlendFunc_CSConstRad::Section(const Blend_Point&, TColgp_Array1OfPnt&, TColgp_Array1OfVec&, TColgp_Array1OfVec&, TColgp_Array1OfPnt2d&, TColgp_Array1OfVec2d&, TColgp_Array1OfVec2d&, TColStd_Array1OfReal&, TColStd_Array1OfReal&, TColStd_Array1OfReal&)
{
 throw Standard_DomainError("BlendFunc_CSConstRad::Section : Not implemented");
}

//=======================================================================
//function : GetSection
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_CSConstRad::GetSection(const Standard_Real Param,
						  const Standard_Real U,
						  const Standard_Real V,
						  const Standard_Real W,
						  TColgp_Array1OfPnt& tabP,
						  TColgp_Array1OfVec& tabV)
{
  Standard_Integer NbPoint=tabP.Length();
  if (NbPoint != tabV.Length() || NbPoint < 2) {throw Standard_RangeError();}

  Standard_Integer i, lowp = tabP.Lower(), lowv = tabV.Lower();

  gp_Vec d1u1,d1v1,d2u1,d2v1,d2uv1,d1; //,d1u2,d1v2;
  gp_Vec ns,dnplan,dnw,dn2w,ncrn,dncrn,ns2;
  gp_Vec ncrossns,resul;
  gp_Vec resulu,resulv,temp;

  Standard_Real norm,ndotns,grosterme;
  Standard_Real lambda,Cosa,Sina;
  Standard_Real Angle = 0.,Dangle = 0.;
  math_Vector sol(1,3),valsol(1,3),secmember(1,3);
  math_Matrix gradsol(1,3,1,3);

  guide->D2(Param,ptgui,d1gui,d2gui);
  normtg = d1gui.Magnitude();
  nplan  = d1gui.Normalized();
  dnplan.SetLinearForm(1./normtg,d2gui,
		       -1./normtg*(nplan.Dot(d2gui)),nplan);

  sol(1) = U; sol(2) = V; sol(3) = W;

  Values(sol,valsol,gradsol);

  surf->D2(U,V,pts,d1u1,d1v1,d2u1,d2v1,d2uv1);
  curv->D1(W,ptc,d1);

  temp.SetXYZ(pts.XYZ()- ptgui.XYZ());
  secmember(1) = normtg - dnplan.Dot(temp);

  temp.SetXYZ(ptc.XYZ()- ptgui.XYZ());
  secmember(2) = normtg - dnplan.Dot(temp);

  ns = d1u1.Crossed(d1v1);
  ncrossns = nplan.Crossed(ns);
  ndotns = nplan.Dot(ns);
  norm = ncrossns.Magnitude();

  // Derivee de n1 par rapport a w

  grosterme = ncrossns.Dot(dnplan.Crossed(ns))/norm/norm;
  dnw.SetLinearForm((dnplan.Dot(ns)-grosterme*ndotns)/norm,nplan,
		     ndotns/norm,dnplan,
		     grosterme/norm,ns);

  temp.SetLinearForm(ndotns/norm,nplan, -1./norm,ns);
  resul.SetLinearForm(ray,temp,gp_Vec(ptc,pts));
  secmember(3) = -2.*ray*(dnw.Dot(resul)); // jag 950105 il manquait ray

  math_Gauss Resol(gradsol);
  if (Resol.IsDone()) {
    
    Resol.Solve(secmember);
    
    tgs.SetLinearForm(secmember(1),d1u1,secmember(2),d1v1);
    tgc = secmember(3)*d1;

    // Derivee de n1 par rapport a u1
    temp = d2u1.Crossed(d1v1).Added(d1u1.Crossed(d2uv1));
    grosterme = ncrossns.Dot(nplan.Crossed(temp))/norm/norm;
    resulu.SetLinearForm(-(grosterme*ndotns-nplan.Dot(temp))/norm,nplan,
			 grosterme/norm,ns,
			 -1./norm,temp);

    // Derivee de n1 par rapport a v1
    temp = d2uv1.Crossed(d1v1).Added(d1u1.Crossed(d2v1));
    grosterme = ncrossns.Dot(nplan.Crossed(temp))/norm/norm;
    resulv.SetLinearForm(-(grosterme*ndotns-nplan.Dot(temp))/norm,nplan,
			 grosterme/norm,ns,
			 -1./norm,temp);


    dnw.SetLinearForm(secmember(1),resulu,secmember(2),resulv,dnw);
    ns.SetLinearForm(ndotns/norm,nplan, -1./norm,ns);

    dn2w.SetLinearForm(ray, dnw, -1., tgc, tgs);
    norm = resul.Magnitude();
    dn2w.Divide(norm);
    ns2 = -resul.Normalized();
    dn2w.SetLinearForm(ns2.Dot(dn2w),ns2,-1.,dn2w);

    if (choix%2 != 0) {
      nplan.Reverse();
      dnplan.Reverse();
    }


    tabP(lowp) = pts;
    tabP(lowp+NbPoint-1)  = ptc;

    tabV(lowv) = tgs;
    tabV(lowv+NbPoint-1)  = tgc;

    if (NbPoint >2) {

      Cosa = ns.Dot(ns2);
      Sina = nplan.Dot(ns.Crossed(ns2));
      Angle = ACos(Cosa);
      if (Sina <0.) {
	Angle = 2.*M_PI - Angle;
      }
      Dangle = -(dnw.Dot(ns2) + ns.Dot(dn2w))/Sina;
      ncrn = nplan.Crossed(ns);
      dncrn = dnplan.Crossed(ns).Added(nplan.Crossed(dnw));
    }

    for (i=2; i <= NbPoint-1; i++) {
      lambda = (Standard_Real)(i-1)/(Standard_Real)(NbPoint-1);
      Cosa = Cos(lambda*Angle);
      Sina = Sin(lambda*Angle);
      tabP(lowp+i-1).SetXYZ(pts.XYZ() + 
		     Abs(ray)*((Cosa-1)*ns.XYZ() + Sina*ncrn.XYZ()));

      temp.SetLinearForm(-Sina,ns,Cosa,ncrn);
      temp.Multiply(lambda*Dangle);
      temp.Add(((Cosa-1)*dnw).Added(Sina*dncrn));
      temp.Multiply(Abs(ray));
      temp.Add(tgs);
      tabV(lowv+i-1)= temp;
    } 
    return Standard_True;
  }
  return Standard_False;
}


//=======================================================================
//function : IsRational
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_CSConstRad::IsRational () const
{
  return (mySShape==BlendFunc_Rational || mySShape==BlendFunc_QuasiAngular);
}

//=======================================================================
//function : GetSectionSize
//purpose  :
//=======================================================================
Standard_Real BlendFunc_CSConstRad::GetSectionSize() const 
{
  return maxang*Abs(ray);
}

//=======================================================================
//function : GetMinimalWeight
//purpose  : 
//=======================================================================
void BlendFunc_CSConstRad::GetMinimalWeight(TColStd_Array1OfReal& Weights) const 
{
  BlendFunc::GetMinimalWeights(mySShape, myTConv, minang, maxang, Weights );
  // On suppose que cela ne depend pas du Rayon! 
}

//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================

Standard_Integer BlendFunc_CSConstRad::NbIntervals (const GeomAbs_Shape S) const
{
 return curv->NbIntervals(BlendFunc::NextShape(S));
}


//=======================================================================
//function : Intervals
//purpose  : 
//=======================================================================

void BlendFunc_CSConstRad::Intervals (TColStd_Array1OfReal& T,
				    const GeomAbs_Shape S) const
{
  curv->Intervals(T, BlendFunc::NextShape(S));
}

//=======================================================================
//function : GetShape
//purpose  : 
//=======================================================================

void BlendFunc_CSConstRad::GetShape (Standard_Integer& NbPoles,
				     Standard_Integer& NbKnots,
				     Standard_Integer& Degree,
				     Standard_Integer& NbPoles2d)
{
  NbPoles2d = 1;
  BlendFunc::GetShape(mySShape,maxang,NbPoles,NbKnots,Degree,myTConv);
}

//=======================================================================
//function : GetTolerance
//purpose  : Determine les Tolerance a utiliser dans les approximations.
//=======================================================================
void BlendFunc_CSConstRad::GetTolerance(const Standard_Real BoundTol, 
					const Standard_Real SurfTol, 
					const Standard_Real AngleTol, 
					math_Vector& Tol3d, 
					math_Vector& Tol1d) const
{
 const Standard_Integer low = Tol3d.Lower();
 const Standard_Integer up = Tol3d.Upper();
 const Standard_Real Tol = GeomFill::GetTolerance(myTConv, minang, Abs(ray), AngleTol, SurfTol);
 Tol1d.Init(SurfTol);
 Tol3d.Init(SurfTol);
 Tol3d(low+1) = Tol3d(up-1) = Min( Tol, SurfTol);
 Tol3d(low) = Tol3d(up) = Min( Tol, BoundTol);
}

//=======================================================================
//function : Knots
//purpose  : 
//=======================================================================

void BlendFunc_CSConstRad::Knots(TColStd_Array1OfReal& TKnots)
{
  GeomFill::Knots(myTConv,TKnots);
}


//=======================================================================
//function : Mults
//purpose  : 
//=======================================================================

void BlendFunc_CSConstRad::Mults(TColStd_Array1OfInteger& TMults)
{
  GeomFill::Mults(myTConv,TMults);
}


//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

void BlendFunc_CSConstRad::Section(const Blend_Point& P,
				   TColgp_Array1OfPnt& Poles,
				   TColgp_Array1OfPnt2d& Poles2d,
				   TColStd_Array1OfReal& Weights)
{
  gp_Vec d1u1,d1v1;//,d1;
  gp_Vec ns,ns2;//,temp,np2;
  gp_Pnt Center;

  Standard_Real norm,u1,v1,w;

  Standard_Real prm = P.Parameter();
  Standard_Integer low = Poles.Lower();
  Standard_Integer upp = Poles.Upper();

  guide->D1(prm,ptgui,d1gui);
  nplan  = d1gui.Normalized();

  P.ParametersOnS(u1,v1);
  w = P.ParameterOnC();

  surf->D1(u1,v1,pts,d1u1,d1v1);
  ptc = curv->Value(w);

  Poles2d(Poles2d.Lower()).SetCoord(u1,v1);

  // Cas Linear
  if (mySShape == BlendFunc_Linear) {
    Poles(low) = pts;
    Poles(upp) = ptc;
    Weights(low) = 1.0;
    Weights(upp) = 1.0;
    return;
  }

  ns = d1u1.Crossed(d1v1);
  norm = nplan.Crossed(ns).Magnitude();

  ns.SetLinearForm(nplan.Dot(ns)/norm,nplan, -1./norm,ns);

  Center.SetXYZ(pts.XYZ()+ray*ns.XYZ());

  ns2 = gp_Vec(Center,ptc).Normalized();

  if (choix%2 != 0) {
    nplan.Reverse();
  }

  GeomFill::GetCircle(myTConv,
		      ns, ns2, 
		      nplan, pts, ptc,
		      Abs(ray), Center, 
		      Poles, Weights);
}
 

//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_CSConstRad::Section
  (const Blend_Point& P,
   TColgp_Array1OfPnt& Poles,
   TColgp_Array1OfVec& DPoles,
   TColgp_Array1OfPnt2d& Poles2d,
   TColgp_Array1OfVec2d& DPoles2d,
   TColStd_Array1OfReal& Weights,
   TColStd_Array1OfReal& DWeights)
{

  gp_Vec d1u1,d1v1,d2u1,d2v1,d2uv1,d1;
  gp_Vec ns,ns2,dnplan,dnw,dn2w; //,np2,dnp2;
  gp_Vec ncrossns;;
  gp_Vec resulu,resulv,temp,tgct,resul;

  gp_Pnt Center;

  Standard_Real norm,ndotns,grosterme;

  math_Vector sol(1,3),valsol(1,3),secmember(1,3);
  math_Matrix gradsol(1,3,1,3);

  Standard_Real prm = P.Parameter();
  Standard_Integer low = Poles.Lower();
  Standard_Integer upp = Poles.Upper();
  Standard_Boolean istgt;

  guide->D2(prm,ptgui,d1gui,d2gui);
  normtg = d1gui.Magnitude();
  nplan  = d1gui.Normalized();
  dnplan.SetLinearForm(1./normtg,d2gui,
		       -1./normtg*(nplan.Dot(d2gui)),nplan);

  P.ParametersOnS(sol(1),sol(2));
  sol(3) = P.ParameterOnC();

  Values(sol,valsol,gradsol);

  surf->D2(sol(1),sol(2),pts,d1u1,d1v1,d2u1,d2v1,d2uv1);
  curv->D1(sol(3),ptc,d1);

  temp.SetXYZ(pts.XYZ()- ptgui.XYZ());
  secmember(1) = normtg - dnplan.Dot(temp);

  temp.SetXYZ(ptc.XYZ()- ptgui.XYZ());
  secmember(2) = normtg - dnplan.Dot(temp);

  ns = d1u1.Crossed(d1v1);
  ncrossns = nplan.Crossed(ns);
  ndotns = nplan.Dot(ns);
  norm = ncrossns.Magnitude();

  // Derivee de n1 par rapport a w

  grosterme = ncrossns.Dot(dnplan.Crossed(ns))/norm/norm;
  dnw.SetLinearForm((dnplan.Dot(ns)-grosterme*ndotns)/norm,nplan,
		     ndotns/norm,dnplan,
		     grosterme/norm,ns);

  temp.SetLinearForm(ndotns/norm,nplan, -1./norm,ns);
  resul.SetLinearForm(ray,temp,gp_Vec(ptc,pts));
  secmember(3) = -2.*ray*(dnw.Dot(resul)); // jag 950105 il manquait ray

  math_Gauss Resol(gradsol);

  if (Resol.IsDone()) {
    
    Resol.Solve(secmember);
    
    tgs.SetLinearForm(secmember(1),d1u1,secmember(2),d1v1);
    tgc = secmember(3)*d1;

    // Derivee de n1 par rapport a u1
    temp = d2u1.Crossed(d1v1).Added(d1u1.Crossed(d2uv1));
    grosterme = ncrossns.Dot(nplan.Crossed(temp))/norm/norm;
    resulu.SetLinearForm(-(grosterme*ndotns-nplan.Dot(temp))/norm,nplan,
			 grosterme/norm,ns,
			 -1./norm,temp);

    // Derivee de n1 par rapport a v1
    temp = d2uv1.Crossed(d1v1).Added(d1u1.Crossed(d2v1));
    grosterme = ncrossns.Dot(nplan.Crossed(temp))/norm/norm;
    resulv.SetLinearForm(-(grosterme*ndotns-nplan.Dot(temp))/norm,nplan,
			 grosterme/norm,ns,
			 -1./norm,temp);


    dnw.SetLinearForm(secmember(1),resulu,secmember(2),resulv,dnw);
    ns.SetLinearForm(ndotns/norm,nplan, -1./norm,ns);

    dn2w.SetLinearForm(ray, dnw, -1., tgc, tgs);
    norm = resul.Magnitude();
    dn2w.Divide(norm);
    ns2 = -resul.Normalized();
    dn2w.SetLinearForm(ns2.Dot(dn2w),ns2,-1.,dn2w);

    istgt = Standard_False;
  }
  else {
    ns.SetLinearForm(ndotns/norm,nplan, -1./norm,ns);
    ns2 = -resul.Normalized();
    istgt = Standard_True;
  }
  
  // Les poles 2d
  
  Poles2d(Poles2d.Lower()).SetCoord(sol(1),sol(2));
  if (!istgt) {
    DPoles2d(Poles2d.Lower()).SetCoord(secmember(1),secmember(2));
  }

  // Cas Linear
  if (mySShape == BlendFunc_Linear) {
    Poles(low) = pts;
    Poles(upp) = ptc;
    Weights(low) = 1.0;
    Weights(upp) = 1.0;
    if (!istgt) {
      DPoles(low) = tgs;
      DPoles(upp) = tgc;
      DWeights(low) = 0.0;
      DWeights(upp) = 0.0;
    }
    return (!istgt);
  }

  // Cas du cercle
  Center.SetXYZ(pts.XYZ()+ray*ns.XYZ());
  if (!istgt) {
    tgct = tgs.Added(ray*dnw);
  }

  if (choix%2 != 0) {
    nplan.Reverse();
    dnplan.Reverse();
  }
  if (!istgt) {
    return GeomFill::GetCircle(myTConv, 
			       ns, ns2, 
			       dnw, dn2w, 
			       nplan, dnplan, 
			       pts, ptc, 
			       tgs, tgc, 
			       Abs(ray), 0, 
			       Center, tgct, 
			       Poles, 
			       DPoles,
			       Weights, 
			       DWeights); 
  }
  else {
    GeomFill::GetCircle(myTConv,
			ns, ns2, 
			nplan, pts, ptc,
			Abs(ray), Center, 
			Poles, Weights);
    return Standard_False;
  }

}


void BlendFunc_CSConstRad::Resolution(const Standard_Integer , const Standard_Real Tol,
                                      Standard_Real& TolU, Standard_Real& TolV) const
{
  TolU = surf->UResolution(Tol);
  TolV = surf->VResolution(Tol);
}
