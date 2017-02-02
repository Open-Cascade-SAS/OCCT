// Created on: 1995-01-04
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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
//                        + Utilisation de GeomFill::GetCircle dans Section.
// Modified 23/06/1997 PMN : Pb de division par 0.

#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Blend_Point.hxx>
#include <BlendFunc.hxx>
#include <BlendFunc_CSCircular.hxx>
#include <ElCLib.hxx>
#include <GeomFill.hxx>
#include <gp.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <Law_Function.hxx>
#include <math_Gauss.hxx>
#include <math_Matrix.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>

#define Eps 1.e-15

//=======================================================================
//function : BlendFunc_CSCircular
//purpose  : 
//=======================================================================

BlendFunc_CSCircular::BlendFunc_CSCircular(const Handle(Adaptor3d_HSurface)& S,
                                           const Handle(Adaptor3d_HCurve)& C,
                                           const Handle(Adaptor3d_HCurve)& CGuide,
                                           const Handle(Law_Function)& L) :
       surf(S),curv(C),guide(CGuide),law(L),istangent(Standard_True),
       //prmc, dprmc, istangent, ray, choix, normtg,
       maxang(RealFirst()),minang(RealLast()),mySShape(BlendFunc_Rational)
       //myTConv
{
  law = L;
}


//=======================================================================
//function : NbVariables
//purpose  : 
//=======================================================================

Standard_Integer BlendFunc_CSCircular::NbVariables () const
{
  return 2; 
}


//=======================================================================
//function : NbEquations
//purpose  : 
//=======================================================================

Standard_Integer BlendFunc_CSCircular::NbEquations () const
{
  return 2; 
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BlendFunc_CSCircular::Set(const Standard_Real Radius, const Standard_Integer Choix)
{
  choix = Choix;
  switch (Choix)
  {
    case 3 :
    case 4 :
      ray = Abs(Radius);
      break;
    default :
      ray = -Abs(Radius);
      break;
  }
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BlendFunc_CSCircular::Set(const BlendFunc_SectionShape TypeSection)
{
  mySShape = TypeSection;
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BlendFunc_CSCircular::Set(const Standard_Real Param)
{
  gp_Pnt ptgui;
  guide->D2(Param,ptgui,d1gui,d2gui);
  law->D1(Param,prmc,dprmc);

  normtg = d1gui.Magnitude();
  nplan  = d1gui.Normalized();
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BlendFunc_CSCircular::Set(const Standard_Real, const Standard_Real)
{
  throw Standard_NotImplemented("BlendFunc_CSCircular::Set");
}

//=======================================================================
//function : GetTolerance
//purpose  : 
//=======================================================================

void BlendFunc_CSCircular::GetTolerance(math_Vector& Tolerance, const Standard_Real Tol) const
{
  Tolerance(1) = surf->UResolution(Tol);
  Tolerance(2) = surf->VResolution(Tol);
}


//=======================================================================
//function : GetBounds
//purpose  : 
//=======================================================================

void BlendFunc_CSCircular::GetBounds(math_Vector& InfBound, math_Vector& SupBound) const
{
  InfBound(1) = surf->FirstUParameter();
  InfBound(2) = surf->FirstVParameter();
  SupBound(1) = surf->LastUParameter();
  SupBound(2) = surf->LastVParameter();

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

Standard_Boolean BlendFunc_CSCircular::IsSolution(const math_Vector& Sol, const Standard_Real Tol)
{
  math_Vector valsol(1,2),secmember(1,2);
  math_Matrix gradsol(1,2,1,2);

  gp_Vec dnplan,d1u1,d1v1,d1c,d2c,temp,ns,ncrossns,resul,nc;
  Standard_Real norm,ndotns,grosterme;
  Standard_Real Cosa,Sina,Angle;

  Values(Sol,valsol,gradsol);

  if (Abs(valsol(1)) <= Tol &&
      Abs(valsol(2)) <= Tol*Tol) {

    // Calcul des tangentes

    pt2d = gp_Pnt2d(Sol(1),Sol(2));

    surf->D1(Sol(1),Sol(2),pts,d1u1,d1v1);
    curv->D2(prmc,ptc,d1c,d2c);

    dnplan.SetLinearForm(1./normtg,d2gui,
			 -1./normtg*(nplan.Dot(d2gui)),nplan);

    ns = d1u1.Crossed(d1v1);
    ncrossns = nplan.Crossed(ns);
    ndotns = nplan.Dot(ns);
    norm = ncrossns.Magnitude();
    if (norm < Eps) {
      norm = 1.;
    }

    temp.SetXYZ(pts.XYZ() - ptc.XYZ());
    secmember(1) = dprmc*(nplan.Dot(d1c)) - dnplan.Dot(temp);    

    grosterme = ncrossns.Dot(dnplan.Crossed(ns))/norm/norm;
    temp.SetLinearForm(ray/norm*(dnplan.Dot(ns)-grosterme*ndotns),nplan,
		       ray*ndotns/norm,dnplan,
		       ray*grosterme/norm,ns);
    temp -= dprmc*d1c;

    ns.SetLinearForm(ndotns/norm,nplan, -1./norm,ns);
    resul.SetLinearForm(ray,ns,gp_Vec(ptc,pts));

    secmember(2) = -2.*(resul.Dot(temp));

    math_Gauss Resol(gradsol);
    if (Resol.IsDone()) {

      Resol.Solve(secmember);

      tgs.SetLinearForm(secmember(1),d1u1,secmember(2),d1v1);
      tgc = dprmc*d1c;
      tg2d.SetCoord(secmember(1),secmember(2));
      istangent = Standard_False;
    }
    else {
      istangent = Standard_True;
    }
    // mise a jour de maxang

    if(ray > 0.) ns.Reverse();
    nc = -resul.Normalized();

    Cosa = ns.Dot(nc);
    Sina = nplan.Dot(ns.Crossed(nc));
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

Standard_Boolean BlendFunc_CSCircular::Value(const math_Vector& X, math_Vector& F)
{
  gp_Vec d1u1,d1v1,d1c;

  surf->D1(X(1),X(2),pts,d1u1,d1v1);
  curv->D1(prmc,ptc,d1c);

  F(1) = nplan.XYZ().Dot(pts.XYZ()-ptc.XYZ());

  gp_Vec ns = d1u1.Crossed(d1v1);
  Standard_Real norm = nplan.Crossed(ns).Magnitude();
  if (norm < Eps) {
    norm = 1.;
  }

  ns.SetLinearForm(nplan.Dot(ns)/norm,nplan, -1./norm,ns);
  gp_Vec vref;
  vref.SetLinearForm(ray,ns,gp_Vec(ptc,pts));

  F(2) = vref.SquareMagnitude() - ray*ray;

  pt2d = gp_Pnt2d(X(1),X(2));
  return Standard_True;
}


//=======================================================================
//function : Derivatives
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_CSCircular::Derivatives(const math_Vector& X, math_Matrix& D)
{
  gp_Vec d1u1,d1v1,d2u1,d2v1,d2uv1,d1c;
  gp_Vec ns,ncrossns,resul,temp,nsov,vref;

  Standard_Real norm,ndotns,grosterme;

  surf->D2(X(1),X(2),pts,d1u1,d1v1,d2u1,d2v1,d2uv1);
  curv->D1(prmc,ptc,d1c);

  D(1,1) = nplan.Dot(d1u1);
  D(1,2) = nplan.Dot(d1v1);

  ns = d1u1.Crossed(d1v1);
  ncrossns = nplan.Crossed(ns);
  norm = ncrossns.Magnitude();
  if (norm < Eps) {
    norm = 1.;
  }

  ndotns = nplan.Dot(ns);

  nsov.SetLinearForm(nplan.Dot(ns)/norm,nplan, -1./norm,ns);
  vref.SetLinearForm(ray,nsov,gp_Vec(ptc,pts));

  // Derivee par rapport a u de Ps + ray*ns
  temp = d2u1.Crossed(d1v1).Added(d1u1.Crossed(d2uv1));
  grosterme = ncrossns.Dot(nplan.Crossed(temp))/norm/norm;
  resul.SetLinearForm(-ray/norm*(grosterme*ndotns-nplan.Dot(temp)),nplan,
		      ray*grosterme/norm,ns,
		      -ray/norm,temp,
		      d1u1);

  D(2,1) = 2.*(resul.Dot(vref));

  // Derivee par rapport a v
  temp = d2uv1.Crossed(d1v1).Added(d1u1.Crossed(d2v1));
  grosterme = ncrossns.Dot(nplan.Crossed(temp))/norm/norm;
  resul.SetLinearForm(-ray/norm*(grosterme*ndotns-nplan.Dot(temp)),nplan,
		      ray*grosterme/norm,ns,
		      -ray/norm,temp,
		      d1v1);

  D(2,2) = 2.*(resul.Dot(vref));

  pt2d = gp_Pnt2d(X(1),X(2));
  return Standard_True;
}


//=======================================================================
//function : Values
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_CSCircular::Values(const math_Vector& X, math_Vector& F, math_Matrix& D)
{
  gp_Vec d1u1,d1v1,d1c;
  gp_Vec d2u1,d2v1,d2uv1;
  gp_Vec ns,ncrossns,resul,temp,vref,nsov;

  Standard_Real norm,ndotns,grosterme;

  surf->D2(X(1),X(2),pts,d1u1,d1v1,d2u1,d2v1,d2uv1);
  curv->D1(prmc,ptc,d1c);

  ns = d1u1.Crossed(d1v1);
  ncrossns = nplan.Crossed(ns);
  norm = ncrossns.Magnitude();
  if (norm < Eps) {
    norm = 1.;
  }

  ndotns = nplan.Dot(ns);
  nsov.SetLinearForm(ndotns/norm,nplan,-1./norm,ns);
  vref.SetLinearForm(ray,nsov,gp_Vec(ptc,pts));

  F(1) = nplan.XYZ().Dot(pts.XYZ()-ptc.XYZ());
  F(2) = vref.SquareMagnitude() - ray*ray;

  D(1,1) = nplan.Dot(d1u1);
  D(1,2) = nplan.Dot(d1v1);

   // Derivee par rapport a u
  temp = d2u1.Crossed(d1v1).Added(d1u1.Crossed(d2uv1));
  grosterme = ncrossns.Dot(nplan.Crossed(temp))/norm/norm;
  resul.SetLinearForm(-ray/norm*(grosterme*ndotns-nplan.Dot(temp)),nplan,
		      ray*grosterme/norm,ns,
		      -ray/norm,temp,
		      d1u1);

  D(2,1) = 2.*(resul.Dot(vref));

  // Derivee par rapport a v
  temp = d2uv1.Crossed(d1v1).Added(d1u1.Crossed(d2v1));
  grosterme = ncrossns.Dot(nplan.Crossed(temp))/norm/norm;
  resul.SetLinearForm(-ray/norm*(grosterme*ndotns-nplan.Dot(temp)),nplan,
		      ray*grosterme/norm,ns,
		      -ray/norm,temp,
		      d1v1);

  D(2,2) = 2.*(resul.Dot(vref));

  pt2d = gp_Pnt2d(X(1),X(2));
  return Standard_True;
}


//=======================================================================
//function : PointOnS
//purpose  : 
//=======================================================================

const gp_Pnt& BlendFunc_CSCircular::PointOnS () const
{
  return pts;
}

//=======================================================================
//function : PointOnC
//purpose  : 
//=======================================================================

const gp_Pnt& BlendFunc_CSCircular::PointOnC () const
{
  return ptc;
}

//=======================================================================
//function : Pnt2d
//purpose  : 
//=======================================================================

const gp_Pnt2d& BlendFunc_CSCircular::Pnt2d () const
{
  return pt2d;
}

//=======================================================================
//function : ParameterOnC
//purpose  : 
//=======================================================================

Standard_Real BlendFunc_CSCircular::ParameterOnC () const
{
  return prmc;
}


//=======================================================================
//function : IsTangencyPoint
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_CSCircular::IsTangencyPoint () const
{
  return istangent;
}

//=======================================================================
//function : TangentOnS
//purpose  : 
//=======================================================================

const gp_Vec& BlendFunc_CSCircular::TangentOnS () const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_CSCircular::TangentOnS");
  return tgs;
}

//=======================================================================
//function : TangentOnC
//purpose  : 
//=======================================================================

const gp_Vec& BlendFunc_CSCircular::TangentOnC () const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_CSCircular::TangentOnC");
  return tgc;
}

//=======================================================================
//function : Tangent2d
//purpose  : 
//=======================================================================

const gp_Vec2d& BlendFunc_CSCircular::Tangent2d () const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_CSCircular::Tangent2d");
  return tg2d;
}


//=======================================================================
//function : Tangent
//purpose  : 
//=======================================================================

void BlendFunc_CSCircular::Tangent(const Standard_Real U, const Standard_Real V,
                                   gp_Vec& TgS, gp_Vec& NmS) const
{
  gp_Pnt bid;
  gp_Vec d1u,d1v,ns;
  surf->D1(U,V,bid,d1u,d1v);
  NmS = ns = d1u.Crossed(d1v);
  
  const Standard_Real norm = nplan.Crossed(ns).Magnitude();
  ns.SetLinearForm(nplan.Dot(ns)/norm,nplan, -1./norm,ns);
  if(ray > 0.) ns.Reverse();
  TgS = nplan.Crossed(ns);
  if (choix%2 == 1)
    TgS.Reverse();
}


//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

void BlendFunc_CSCircular::Section(const Standard_Real Param,
				   const Standard_Real U,
				   const Standard_Real V,
				   const Standard_Real W,
				   Standard_Real& Pdeb,
				   Standard_Real& Pfin,
				   gp_Circ& C)
{
  gp_Vec d1u1,d1v1;
  gp_Vec ns;//,temp;
  Standard_Real norm;
  gp_Pnt Center;
  gp_Pnt ptgui;

  guide->D1(Param,ptgui,d1gui);
  nplan  = d1gui.Normalized();

  surf->D1(U,V,pts,d1u1,d1v1);
  ptc = curv->Value(W);

  ns = d1u1.Crossed(d1v1);
  norm = nplan.Crossed(ns).Magnitude();
  ns.SetLinearForm(nplan.Dot(ns)/norm,nplan, -1./norm,ns);

  Center.SetXYZ(pts.XYZ()+ray*ns.XYZ());
  C.SetRadius(Abs(ray));

  if(ray > 0.) ns.Reverse();

  if (choix%2 == 0) {
    C.SetPosition(gp_Ax2(Center,nplan,ns));
  }
  else {
    C.SetPosition(gp_Ax2(Center,nplan.Reversed(),ns));
  }
  Pdeb = 0.;
  Pfin = ElCLib::Parameter(C,ptc);
}


Standard_Boolean BlendFunc_CSCircular::Section(const Blend_Point& P, 
					   TColgp_Array1OfPnt& Poles, 
					   TColgp_Array1OfVec& DPoles, 
					   TColgp_Array1OfVec& D2Poles, 
					   TColgp_Array1OfPnt2d& Poles2d, 
					   TColgp_Array1OfVec2d& DPoles2d, 
					   TColgp_Array1OfVec2d& D2Poles2d, 
					   TColStd_Array1OfReal& Weigths, 
					   TColStd_Array1OfReal& DWeigths, 
					   TColStd_Array1OfReal& D2Weigths)
{
  return Blend_CSFunction::Section(P,Poles,DPoles,D2Poles,Poles2d,DPoles2d,D2Poles2d,Weigths,DWeigths,D2Weigths);
}

//=======================================================================
//function : GetSection
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_CSCircular::GetSection(const Standard_Real Param,
						  const Standard_Real U,
						  const Standard_Real V,
						  const Standard_Real /*W*/,
						  TColgp_Array1OfPnt& tabP,
						  TColgp_Array1OfVec& tabV)
{
  Standard_Integer NbPoint=tabP.Length();
  if (NbPoint != tabV.Length() || NbPoint < 2) {throw Standard_RangeError();}

  Standard_Integer i, lowp = tabP.Lower(), lowv = tabV.Lower();

  gp_Vec d1u1,d1v1,d2u1,d2v1,d2uv1,d1c,d2c; //,d1u2,d1v2;
  gp_Vec ns,dnplan,dnw,dn2w,ncrn,dncrn,ns2;
  gp_Vec ncrossns,resul;
  gp_Vec resulu,resulv,temp;

  Standard_Real norm,ndotns,grosterme;
  Standard_Real lambda,Cosa,Sina;
  Standard_Real Angle = 0.,Dangle = 0.;
  math_Vector sol(1,2),valsol(1,2),secmember(1,2);
  math_Matrix gradsol(1,2,1,2);

  Set(Param);
  dnplan.SetLinearForm(1./normtg,d2gui,
		       -1./normtg*(nplan.Dot(d2gui)),nplan);

  curv->D2(prmc,ptc,d1c,d2c);
  surf->D2(U,V,pts,d1u1,d1v1,d2u1,d2v1,d2uv1);

  ns = d1u1.Crossed(d1v1);
  ncrossns = nplan.Crossed(ns);
  ndotns = nplan.Dot(ns);
  norm = ncrossns.Magnitude();

  temp.SetXYZ(pts.XYZ() - ptc.XYZ());
  secmember(1) = dprmc*(nplan.Dot(d1c)) - dnplan.Dot(temp);    

  ns2.SetLinearForm(ndotns/norm,nplan, -1./norm,ns);

  // Derivee de n1 par rapport a w (param sur ligne guide)

  grosterme = ncrossns.Dot(dnplan.Crossed(ns))/norm/norm;
  dnw.SetLinearForm((dnplan.Dot(ns)-grosterme*ndotns)/norm,nplan,
		     ndotns/norm,dnplan,
		     grosterme/norm,ns);

  temp.SetLinearForm(ray,dnw,-dprmc,d1c);
  resul.SetLinearForm(ray,ns2,gp_Vec(ptc,pts));

  secmember(2) = -2.*(resul.Dot(temp));

  sol(1) = U; sol(2) = V;
  Values(sol,valsol,gradsol);
  math_Gauss Resol(gradsol);

  if (Resol.IsDone()) {
    
    Resol.Solve(secmember);
    
    tgs.SetLinearForm(secmember(1),d1u1,secmember(2),d1v1);
    tgc = dprmc*d1c;

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
    ns = ns2;
    dn2w.SetLinearForm(ray, dnw, -1., tgc, tgs);
    norm = resul.Magnitude();
    dn2w.Divide(norm);
    ns2 = -resul.Normalized();
    dn2w.SetLinearForm(ns2.Dot(dn2w),ns2,-1.,dn2w);

    if(ray > 0.) {
      ns.Reverse();
      dnw.Reverse();
    }

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
      tabP(lowp+i-1).SetXYZ(pts.XYZ() 
		     +Abs(ray)*((Cosa-1)*ns.XYZ() + Sina*ncrn.XYZ()));

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

Standard_Boolean BlendFunc_CSCircular::IsRational() const
{
  return (mySShape==BlendFunc_Rational || mySShape==BlendFunc_QuasiAngular);
}

//=======================================================================
//function : GetSectionSize
//purpose  : 
//=======================================================================

Standard_Real BlendFunc_CSCircular::GetSectionSize() const 
{
  return maxang*Abs(ray);
}

//=======================================================================
//function : GetMinimalWeight
//purpose  : 
//=======================================================================
void BlendFunc_CSCircular::GetMinimalWeight(TColStd_Array1OfReal& Weigths) const 
{
  BlendFunc::GetMinimalWeights(mySShape, myTConv,minang,maxang,Weigths);
  // On suppose que cela ne depend pas du Rayon! 
}

//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================

Standard_Integer BlendFunc_CSCircular::NbIntervals (const GeomAbs_Shape S) const
{
  return curv->NbIntervals(BlendFunc::NextShape(S));
}


//=======================================================================
//function : Intervals
//purpose  : 
//=======================================================================

void BlendFunc_CSCircular::Intervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const
{
  curv->Intervals(T, BlendFunc::NextShape(S));
}

//=======================================================================
//function : GetShape
//purpose  : 
//=======================================================================

void BlendFunc_CSCircular::GetShape (Standard_Integer& NbPoles,
				     Standard_Integer& NbKnots,
				     Standard_Integer& Degree,
				     Standard_Integer& NbPoles2d)
{
  NbPoles2d = 1;
  BlendFunc::GetShape(mySShape,maxang,NbPoles,NbKnots,Degree,myTConv);
}


//=======================================================================
//function : GetTolerance
//purpose  : Determine les Tolerances a utiliser dans les approximations.
//=======================================================================
void BlendFunc_CSCircular::GetTolerance(const Standard_Real BoundTol, 
				      const Standard_Real SurfTol, 
				      const Standard_Real AngleTol, 
				      math_Vector& Tol3d, 
				      math_Vector& Tol1d) const
{
  const Standard_Integer low = Tol3d.Lower();
  const Standard_Integer up = Tol3d.Upper();
  const Standard_Real Tol = GeomFill::GetTolerance(myTConv, minang, ray, AngleTol, SurfTol);
  Tol1d.Init(SurfTol);
  Tol3d.Init(SurfTol);
  Tol3d(low+1) = Tol3d(up-1) = Min( Tol, SurfTol);
  Tol3d(low) = Tol3d(up) = Min( Tol, BoundTol);
}


//=======================================================================
//function : Knots
//purpose  : 
//=======================================================================

void BlendFunc_CSCircular::Knots(TColStd_Array1OfReal& TKnots)
{
  GeomFill::Knots(myTConv,TKnots);
}

//=======================================================================
//function : Mults
//purpose  : 
//=======================================================================

void BlendFunc_CSCircular::Mults(TColStd_Array1OfInteger& TMults)
{
  GeomFill::Mults(myTConv,TMults);
}


//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

void BlendFunc_CSCircular::Section(const Blend_Point& P,
				   TColgp_Array1OfPnt& Poles,
				   TColgp_Array1OfPnt2d& Poles2d,
				   TColStd_Array1OfReal& Weights)
{
  gp_Vec d1u1,d1v1;//,d1;
  gp_Vec ns,ns2;//,temp,np2;
  gp_Pnt Center;

  Standard_Real norm,u1,v1;

  Standard_Real prm = P.Parameter();
  Standard_Integer low = Poles.Lower();
  Standard_Integer upp = Poles.Upper();

  Set(prm);

  P.ParametersOnS(u1,v1);
  surf->D1(u1,v1,pts,d1u1,d1v1);
  ptc = curv->Value(prmc);

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
  if(ray > 0.) ns.Reverse();

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

Standard_Boolean BlendFunc_CSCircular::Section
  (const Blend_Point& P,
   TColgp_Array1OfPnt& Poles,
   TColgp_Array1OfVec& DPoles,
   TColgp_Array1OfPnt2d& Poles2d,
   TColgp_Array1OfVec2d& DPoles2d,
   TColStd_Array1OfReal& Weights,
   TColStd_Array1OfReal& DWeights)
{
  gp_Vec d1u1,d1v1,d2u1,d2v1,d2uv1,d1,d2;
  gp_Vec ns,ns2,dnplan,dnw,dn2w; //,np2,dnp2;
  gp_Vec ncrossns;;
  gp_Vec resulu,resulv,temp,tgct,resul;

  gp_Pnt Center;

  Standard_Real norm,ndotns,grosterme;;

  math_Vector sol(1,2),valsol(1,2),secmember(1,2);
  math_Matrix gradsol(1,2,1,2);

  Standard_Real prm = P.Parameter();
  Standard_Integer low = Poles.Lower();
  Standard_Integer upp = Poles.Upper();
  Standard_Boolean istgt;

  Set(prm);
  dnplan.SetLinearForm(1./normtg,d2gui,
		       -1./normtg*(nplan.Dot(d2gui)),nplan);

  curv->D2(prmc,ptc,d1,d2);
  P.ParametersOnS(sol(1),sol(2));
  surf->D2(sol(1),sol(2),pts,d1u1,d1v1,d2u1,d2v1,d2uv1);

  ns = d1u1.Crossed(d1v1);
  ncrossns = nplan.Crossed(ns);
  ndotns = nplan.Dot(ns);
  norm = ncrossns.Magnitude();

  ns2.SetLinearForm(ndotns/norm,nplan, -1./norm,ns);
  temp.SetXYZ(pts.XYZ() - ptc.XYZ());

  secmember(1) = dprmc*(nplan.Dot(d1)) - dnplan.Dot(temp);    
  
  // Derivee de n1 par rapport a w

  grosterme = ncrossns.Dot(dnplan.Crossed(ns))/norm/norm;
  dnw.SetLinearForm((dnplan.Dot(ns)-grosterme*ndotns)/norm,nplan,
		     ndotns/norm,dnplan,
		     grosterme/norm,ns);

  temp.SetLinearForm(ray,dnw,-dprmc,d1);
  resul.SetLinearForm(ray,ns2,gp_Vec(ptc,pts));

  secmember(2) = -2.*(resul.Dot(temp));

  Values(sol,valsol,gradsol);
  math_Gauss Resol(gradsol);

  if (Resol.IsDone()) {
    
    Resol.Solve(secmember);
    
    tgs.SetLinearForm(secmember(1),d1u1,secmember(2),d1v1);
    tgc = dprmc*d1;

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
    ns = ns2;
    dn2w.SetLinearForm(ray, dnw,-1., tgc, tgs);
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

  if(ray > 0.) {
    ns.Reverse();
    if(!istgt) { dnw.Reverse(); }
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

void BlendFunc_CSCircular::Resolution(const Standard_Integer , const Standard_Real Tol,
                                      Standard_Real& TolU, Standard_Real& TolV) const
{
  TolU = surf->UResolution(Tol);
  TolV = surf->VResolution(Tol);
}
