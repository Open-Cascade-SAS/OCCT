// Created on: 1997-07-28
// Created by: Jerome LEMONIER
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


#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Blend_Point.hxx>
#include <BlendFunc.hxx>
#include <BRepBlend_SurfRstEvolRad.hxx>
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
#include <math_SVD.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>
#include <TColStd_SequenceOfReal.hxx>

#define Eps 1.e-15

static void t3dto2d(Standard_Real& a,
		    Standard_Real& b,
		    const gp_Vec& A,
		    const gp_Vec& B,
		    const gp_Vec& C)
{
  Standard_Real AB = A.Dot(B);
  Standard_Real AC = A.Dot(C);
  Standard_Real BC = B.Dot(C);
  Standard_Real BB = B.Dot(B);
  Standard_Real CC = C.Dot(C);
  Standard_Real deno = (BB*CC-BC*BC);
  a = (AB*CC-AC*BC)/deno;
  b = (AC*BB-AB*BC)/deno;
}

static void FusionneIntervalles(const TColStd_Array1OfReal& I1,
			 const TColStd_Array1OfReal& I2,
			 TColStd_SequenceOfReal& Seq)
{
  Standard_Integer ind1=1, ind2=1;
  Standard_Real    Epspar = Precision::PConfusion()*0.99;
  // it is supposed that positioning works with PConfusion()/2
  Standard_Real    v1, v2;
// Initialisation : IND1 and IND2 point at the first element
// of each of 2 tables to be processed. INDS points at the last
// element created by TABSOR


//--- TABSOR is filled by parsing TABLE1 and TABLE2 simultaneously ---
//------------------ and eliminating multiple occurrencies ------------

 while ((ind1<=I1.Upper()) && (ind2<=I2.Upper())) {
      v1 = I1(ind1);
      v2 = I2(ind2);
      if (Abs(v1-v2)<= Epspar) {
// Here the elements of I1 and I2 fit.
         Seq.Append((v1+v2)/2);
	 ind1++;
         ind2++;
       }
      else if (v1 < v2) {
	// Here the element of I1 fits.
         Seq.Append(v1);
         ind1++;
       }
      else {
// Here the element of TABLE2 fits.
	 Seq.Append(v2);
	 ind2++;
       }
    }

  if (ind1>I1.Upper()) { 
//----- Here I1 is exhausted, completed with the end of TABLE2 -------

    for (; ind2<=I2.Upper(); ind2++) {
      Seq.Append(I2(ind2));
    }
  }

  if (ind2>I2.Upper()) { 
//----- Here I2 is exhausted, completed with the end of I1 -------

    for (; ind1<=I1.Upper(); ind1++) {
      Seq.Append(I1(ind1));
    }
  }
}

//=======================================================================
//function : BRepBlend_SurfRstEvolRad
//purpose  : Contructor
//=======================================================================
BRepBlend_SurfRstEvolRad::BRepBlend_SurfRstEvolRad
(const Handle(Adaptor3d_HSurface)& Surf,
 const Handle(Adaptor3d_HSurface)& SurfRst,
 const Handle(Adaptor2d_HCurve2d)& Rst,
 const Handle(Adaptor3d_HCurve)& CGuide,
 const Handle(Law_Function)& Evol):
 surf(Surf), surfrst(SurfRst), 
 rst(Rst), cons(Rst,SurfRst), 
 guide(CGuide), tguide(CGuide),
 istangent(Standard_True), 
 maxang(RealFirst()), minang(RealLast()),
 distmin(RealLast()),
 mySShape(BlendFunc_Rational)

{ tevol=Evol;
  fevol=Evol;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
Standard_Integer BRepBlend_SurfRstEvolRad::NbVariables() const
{  
  return 3;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
Standard_Integer BRepBlend_SurfRstEvolRad::NbEquations() const
{
  return 3;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
Standard_Boolean BRepBlend_SurfRstEvolRad::Value
(const math_Vector& X,
 math_Vector& F) 
{
  gp_Vec d1u1,d1v1,ns,vref;
  Standard_Real norm;

  surf->D1(X(1),X(2),pts,d1u1,d1v1);
  ptrst = cons.Value(X(3));
  
  F(1) = nplan.XYZ().Dot(pts.XYZ()) + theD;
  
  F(2) = nplan.XYZ().Dot(ptrst.XYZ()) + theD;
  
  ns = d1u1.Crossed(d1v1);
  norm = nplan.Crossed(ns).Magnitude();
  ns.SetLinearForm(nplan.Dot(ns)/norm,nplan, -1./norm,ns);
  vref.SetLinearForm(ray,ns,gp_Vec(ptrst,pts));
  F(3) = vref.SquareMagnitude() - ray*ray;
  return Standard_True;
}

//=======================================================================
//function : Derivatives
//purpose  : 
//=======================================================================
Standard_Boolean BRepBlend_SurfRstEvolRad::Derivatives
(const math_Vector& X,
 math_Matrix& D) 
{  gp_Vec d1u1,d1v1,d2u1,d2v1,d2uv1,d1;
  gp_Vec ns,ncrossns,resul,temp, vref;
  
  Standard_Real norm,ndotns,grosterme;
  
  surf->D2(X(1),X(2),pts,d1u1,d1v1,d2u1,d2v1,d2uv1);
  cons.D1(X(3),ptrst,d1);
  
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
  vref.SetLinearForm(ray,vref,gp_Vec(ptrst,pts));
  
  // Derivative corresponding to u1
  temp = d2u1.Crossed(d1v1).Added(d1u1.Crossed(d2uv1));
  grosterme = ncrossns.Dot(nplan.Crossed(temp))/norm/norm;
  resul.SetLinearForm(-ray/norm*(grosterme*ndotns-nplan.Dot(temp)),nplan,
		      ray*grosterme/norm,ns,
		      -ray/norm,temp,
		      d1u1);
  
  D(3,1) = 2.*(resul.Dot(vref));
  
  
  // Derivative corresponding to v1
  temp = d2uv1.Crossed(d1v1).Added(d1u1.Crossed(d2v1));
  grosterme = ncrossns.Dot(nplan.Crossed(temp))/norm/norm;
  resul.SetLinearForm(-ray/norm*(grosterme*ndotns-nplan.Dot(temp)),nplan,
		      ray*grosterme/norm,ns,
		      -ray/norm,temp,
		      d1v1);
  
  D(3,2) = 2.*(resul.Dot(vref));
  
  D(3,3) = -2.*(d1.Dot(vref));
  
  return Standard_True;

}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
Standard_Boolean BRepBlend_SurfRstEvolRad::Values
(const math_Vector& X,
 math_Vector& F,
 math_Matrix& D) 
{
  gp_Vec d1u1,d1v1,d1;
  gp_Vec d2u1,d2v1,d2uv1;
  gp_Vec ns,ncrossns,resul,temp,vref;
  
  Standard_Real norm,ndotns,grosterme;

  surf->D2(X(1),X(2),pts,d1u1,d1v1,d2u1,d2v1,d2uv1);
  cons.D1(X(3),ptrst,d1);
  
  F(1) = nplan.XYZ().Dot(pts.XYZ()) + theD;
  F(2) = nplan.XYZ().Dot(ptrst.XYZ()) + theD;
  
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
  vref.SetLinearForm(ray,vref,gp_Vec(ptrst,pts));
  
  F(3) = vref.SquareMagnitude() - ray*ray;
  
  
  // Derivative corresponding to u1
  temp = d2u1.Crossed(d1v1).Added(d1u1.Crossed(d2uv1));
  grosterme = ncrossns.Dot(nplan.Crossed(temp))/norm/norm;
  resul.SetLinearForm(-ray/norm*(grosterme*ndotns-nplan.Dot(temp)),nplan,
		      ray*grosterme/norm,ns,
		      -ray/norm,temp,
		      d1u1);
  
  D(3,1) = 2.*(resul.Dot(vref));
  
  
  // Derivative corresponding to v1
  temp = d2uv1.Crossed(d1v1).Added(d1u1.Crossed(d2v1));
  grosterme = ncrossns.Dot(nplan.Crossed(temp))/norm/norm;
  resul.SetLinearForm(-ray/norm*(grosterme*ndotns-nplan.Dot(temp)),nplan,
		      ray*grosterme/norm,ns,
		      -ray/norm,temp,
		      d1v1);
  
  D(3,2) = 2.*(resul.Dot(vref));
  
  D(3,3) = -2.*(d1.Dot(vref));
  
  return Standard_True;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
void BRepBlend_SurfRstEvolRad::Set
(const Handle(Adaptor3d_HSurface)& SurfRef,
const Handle(Adaptor2d_HCurve2d)& RstRef) 
{
  surfref = SurfRef;
  rstref = RstRef;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
void BRepBlend_SurfRstEvolRad::Set(const Standard_Real Param) 
{
  d1gui = gp_Vec(0.,0.,0.);
  nplan = gp_Vec(0.,0.,0.);
  tguide->D2(Param,ptgui,d1gui,d2gui);
  normtg = d1gui.Magnitude();
  nplan.SetXYZ(d1gui.Normalized().XYZ());
  gp_XYZ nplanXYZ(nplan.XYZ());
  gp_XYZ ptguiXYZ(ptgui.XYZ());
  theD =  nplanXYZ.Dot(ptguiXYZ)  ;
  theD = theD  * (-1.) ;
  tevol->D1(Param,ray,dray);
  ray=sg1*ray;
  dray=sg1*dray;
}

//=======================================================================
//function : 
//purpose  : Segments the curve in its useful part.
//           Precision is taken arbitrary small !?
//=======================================================================
 void BRepBlend_SurfRstEvolRad::Set
(const Standard_Real First,
const Standard_Real Last) 
{ 
  tguide = guide->Trim(First,Last,1.e-12);
  tevol  = fevol->Trim(First,Last,1.e-12);
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 void BRepBlend_SurfRstEvolRad::GetTolerance
(math_Vector& Tolerance,
const Standard_Real Tol) const
{
  Tolerance(1) = surf->UResolution(Tol);
  Tolerance(2) = surf->VResolution(Tol);
  Tolerance(3) = cons.Resolution(Tol);
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 void BRepBlend_SurfRstEvolRad::GetBounds
(math_Vector& InfBound,
math_Vector& SupBound) const
{
  InfBound(1) = surf->FirstUParameter();
  InfBound(2) = surf->FirstVParameter();
  InfBound(3) = cons.FirstParameter();
  SupBound(1) = surf->LastUParameter();
  SupBound(2) = surf->LastVParameter();
  SupBound(3) = cons.LastParameter();
  
  if(!Precision::IsInfinite(InfBound(1)) &&
     !Precision::IsInfinite(SupBound(1))) {
    Standard_Real range = (SupBound(1) - InfBound(1));
    InfBound(1) -= range;
    SupBound(1) += range;
  }
  if(!Precision::IsInfinite(InfBound(2)) &&
     !Precision::IsInfinite(SupBound(2))) {
    Standard_Real range = (SupBound(2) - InfBound(2));
    InfBound(2) -= range;
    SupBound(2) += range;
  }
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 Standard_Boolean BRepBlend_SurfRstEvolRad::IsSolution
(const math_Vector& Sol,
const Standard_Real Tol) 
{
  math_Vector valsol(1,3),secmember(1,3);
  math_Matrix gradsol(1,3,1,3);
  
  gp_Vec dnplan,d1u1,d1v1,d1urst,d1vrst,d1,temp,ns,ns2,ncrossns,resul;
  gp_Pnt bid;
  Standard_Real norm,ndotns,grosterme;
  Standard_Real Cosa,Sina,Angle;
  
  Values(Sol,valsol,gradsol);
  if (Abs(valsol(1)) <= Tol &&
      Abs(valsol(2)) <= Tol &&
      Abs(valsol(3)) <= 2*Tol*Abs(ray) ) {
    
    // Calculation of tangents
    
    pt2ds  = gp_Pnt2d(Sol(1),Sol(2));
    prmrst = Sol(3);
    pt2drst = rst->Value(prmrst);
    surf->D1(Sol(1),Sol(2),pts,d1u1,d1v1);
    cons.D1(Sol(3),ptrst,d1);
    dnplan.SetLinearForm(1./normtg,d2gui,
			 -1./normtg*(nplan.Dot(d2gui)),nplan);
    
    temp.SetXYZ(pts.XYZ() - ptgui.XYZ());
    secmember(1) = normtg - dnplan.Dot(temp);
    
    temp.SetXYZ(ptrst.XYZ() - ptgui.XYZ());
    secmember(2) = normtg - dnplan.Dot(temp);
    
    ns = d1u1.Crossed(d1v1);
    ncrossns = nplan.Crossed(ns);
    ndotns = nplan.Dot(ns);
    norm = ncrossns.Magnitude();
    
    grosterme = ncrossns.Dot(dnplan.Crossed(ns))/norm/norm;
    gp_Vec dnw;
    dnw.SetLinearForm((dnplan.Dot(ns)-grosterme*ndotns)/norm,nplan,
		      ndotns/norm,dnplan,
		      grosterme/norm,ns);
    
    ns.SetLinearForm(ndotns/norm,nplan, -1./norm,ns);
    resul.SetLinearForm(ray, ns, gp_Vec(ptrst,pts));
    
    secmember(3) = -2.*ray*(dnw.Dot(resul)) - 2.*dray*(ns.Dot(resul)) + 2.*ray*dray;
    math_Gauss Resol(gradsol);
    if (Resol.IsDone()) {
      Resol.Solve(secmember);
      istangent = Standard_False;
    }
    else {
      math_SVD SingRS (gradsol);
      if (SingRS.IsDone()) {
	math_Vector DEDT(1,3);
	DEDT = secmember;
	SingRS.Solve(DEDT, secmember, 1.e-6);
	istangent = Standard_False;
      }
      else istangent = Standard_True;
    }      
    
    if (!istangent) {  
      tgs.SetLinearForm(secmember(1),d1u1,secmember(2),d1v1);
      tgrst = secmember(3)*d1;
      tg2ds.SetCoord(secmember(1),secmember(2));
      surfrst->D1(pt2drst.X(),pt2drst.Y(),bid,d1urst,d1vrst);
      Standard_Real a, b;
      t3dto2d(a,b,tgrst,d1urst,d1vrst);
      tg2drst.SetCoord(a,b);
      istangent = Standard_False;
    }
    else {
      istangent = Standard_True;
    }
    // update of maxang
    if(ray>0.) ns.Reverse();
    ns2 = -resul.Normalized();
    
    Cosa = ns.Dot(ns2);
    Sina = nplan.Dot(ns.Crossed(ns2));
    if (choix%2 != 0) {
      Sina = -Sina;  //nplan is changed into -nplan
    }
    
    Angle = ACos(Cosa);
    if (Sina <0.) {
      Angle = 2.*M_PI - Angle;
    }
    
    if (Angle>maxang) {maxang = Angle;}
    if (Angle<minang) {minang = Angle;}
    distmin = Min( distmin, pts.Distance(ptrst));
    
    return Standard_True;
  }
  istangent = Standard_True;
  return Standard_False;
}


//=======================================================================
//function : GetMinimalDistance
//purpose  : 
//=======================================================================

Standard_Real BRepBlend_SurfRstEvolRad::GetMinimalDistance() const
{
  return distmin;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
const gp_Pnt& BRepBlend_SurfRstEvolRad::PointOnS() const
{
  return pts;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
const gp_Pnt& BRepBlend_SurfRstEvolRad::PointOnRst() const
{
  return ptrst ;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
const gp_Pnt2d& BRepBlend_SurfRstEvolRad::Pnt2dOnS() const
{
  return pt2ds;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
const gp_Pnt2d& BRepBlend_SurfRstEvolRad::Pnt2dOnRst() const
{
  return pt2drst;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 Standard_Real BRepBlend_SurfRstEvolRad::ParameterOnRst() const
{
  return prmrst;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 Standard_Boolean BRepBlend_SurfRstEvolRad::IsTangencyPoint() const
{
  return istangent;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
const gp_Vec& BRepBlend_SurfRstEvolRad::TangentOnS() const
{
  if (istangent) {throw Standard_DomainError();}
  return tgs;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
const gp_Vec2d& BRepBlend_SurfRstEvolRad::Tangent2dOnS() const
{
  if (istangent) {throw Standard_DomainError();}
  return tg2ds;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
const gp_Vec& BRepBlend_SurfRstEvolRad::TangentOnRst() const
{
  if (istangent) {throw Standard_DomainError();}
  return tgrst;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
const gp_Vec2d& BRepBlend_SurfRstEvolRad::Tangent2dOnRst() const
{
  if (istangent) {throw Standard_DomainError();}
  return tg2drst;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 Standard_Boolean BRepBlend_SurfRstEvolRad::Decroch
(const math_Vector& Sol,
gp_Vec& NS,
gp_Vec& TgS) const
{
  gp_Vec TgRst, NRst, NRstInPlane, NSInPlane;
  gp_Pnt bid,Center;
  gp_Vec d1u,d1v;
  Standard_Real norm,unsurnorm;
  
  surf->D1(Sol(1),Sol(2),bid,d1u,d1v);
  NS = NSInPlane = d1u.Crossed(d1v);
  
  norm = nplan.Crossed(NS).Magnitude();
  unsurnorm = 1./norm;
  NSInPlane.SetLinearForm(nplan.Dot(NS)*unsurnorm,nplan,-unsurnorm,NS);
  
  Center.SetXYZ(bid.XYZ()+ray*NSInPlane.XYZ());
  if(choix>2) NSInPlane.Reverse();
  TgS = nplan.Crossed(gp_Vec(Center,bid));
  if (choix%2 == 1) {
    TgS.Reverse();
  }
  Standard_Real u,v;
  rstref->Value(Sol(3)).Coord(u,v);
  surfref->D1(u,v,bid,d1u,d1v);
  NRst = d1u.Crossed(d1v);
  norm = nplan.Crossed(NRst).Magnitude();
  unsurnorm = 1./norm;
  NRstInPlane.SetLinearForm(nplan.Dot(NRst)*unsurnorm,nplan,-unsurnorm,NRst);
  gp_Vec centptrst(Center,bid);
  if(centptrst.Dot(NRstInPlane) < 0.) NRstInPlane.Reverse();
  TgRst = nplan.Crossed(centptrst);
  if (choix%2 == 1) {
    TgRst.Reverse();
  }

  Standard_Real dot, NT = NRstInPlane.Magnitude();
  NT *= TgRst.Magnitude();
  if (Abs(NT) < 1.e-7) {
    return Standard_False; // Singularity or Incoherence.
  }
  dot = NRstInPlane.Dot(TgRst);
  dot /= NT;
	 
  return (dot < 1.e-10);
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 void BRepBlend_SurfRstEvolRad::Set (const Standard_Integer Choix) 
{
  choix = Choix;
  switch (choix) {
  case 1 :
  case 2 :
    sg1 = -1;
    break;
  case 3 :
  case 4 :
    sg1 = 1;
    break;
  default :
    sg1 = -1;
    break;
  }
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 void BRepBlend_SurfRstEvolRad::Set(const BlendFunc_SectionShape TypeSection) 
{
  mySShape = TypeSection;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 void BRepBlend_SurfRstEvolRad::Section
(const Standard_Real Param,
const Standard_Real U,
const Standard_Real V,
const Standard_Real W,
Standard_Real& Pdeb,
Standard_Real& Pfin,
gp_Circ& C) 
{
  gp_Vec d1u1,d1v1;
  gp_Vec ns, np;
  Standard_Real norm;
  gp_Pnt Center;
  
  tguide->D1(Param,ptgui,d1gui);
  np  = d1gui.Normalized();
  ray = sg1*tevol->Value(Param);
  
  surf->D1(U,V,pts,d1u1,d1v1);
  ptrst = cons.Value(W);
  
  ns = d1u1.Crossed(d1v1);
  
  norm = nplan.Crossed(ns).Magnitude();
  ns.SetLinearForm(nplan.Dot(ns)/norm,nplan, -1./norm,ns);
  Center.SetXYZ(pts.XYZ()+ray*ns.XYZ());
  C.SetRadius(Abs(ray));

  if (ray > 0) {
    ns.Reverse();
  }    
  if (choix%2 != 0) {
    np.Reverse();
  }
  C.SetPosition(gp_Ax2(Center,np,ns));

  Pdeb = 0.; //ElCLib::Parameter(C,pts);
  Pfin = ElCLib::Parameter(C,ptrst);

  // Test negative and almost null angles : Single Case
  if (Pfin>1.5*M_PI) {
    np.Reverse();
    C.SetPosition(gp_Ax2(Center,np,ns));
    Pfin = ElCLib::Parameter(C,ptrst);
  }
  if (Pfin < Precision::PConfusion()) Pfin += Precision::PConfusion();  
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 Standard_Boolean BRepBlend_SurfRstEvolRad::IsRational() const
{
  return  (mySShape==BlendFunc_Rational || mySShape==BlendFunc_QuasiAngular);
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 Standard_Real BRepBlend_SurfRstEvolRad::GetSectionSize() const
{
  return maxang*Abs(ray);
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 void BRepBlend_SurfRstEvolRad::GetMinimalWeight(TColStd_Array1OfReal& Weigths) const
{
  BlendFunc::GetMinimalWeights(mySShape, myTConv, minang, maxang, Weigths );
  // It is supposed that it does not depend on the Radius! 
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 Standard_Integer BRepBlend_SurfRstEvolRad::NbIntervals(const GeomAbs_Shape S) const
{
  Standard_Integer Nb_Int_Courbe, Nb_Int_Loi;
  Nb_Int_Courbe =  guide->NbIntervals(BlendFunc::NextShape(S));
  Nb_Int_Loi    =  fevol->NbIntervals(S);

  if  (Nb_Int_Loi==1) {
    return Nb_Int_Courbe;
  }

  TColStd_Array1OfReal IntC(1, Nb_Int_Courbe+1);
  TColStd_Array1OfReal IntL(1, Nb_Int_Loi+1);
  TColStd_SequenceOfReal    Inter;
  guide->Intervals(IntC, BlendFunc::NextShape(S));
  fevol->Intervals(IntL, S);

  FusionneIntervalles( IntC, IntL, Inter);
  return Inter.Length()-1;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 void BRepBlend_SurfRstEvolRad::Intervals(TColStd_Array1OfReal& T,
					  const GeomAbs_Shape S) const
{
  Standard_Integer Nb_Int_Courbe, Nb_Int_Loi;  
  Nb_Int_Courbe =  guide->NbIntervals(BlendFunc::NextShape(S));
  Nb_Int_Loi    =  fevol->NbIntervals(S);

  if  (Nb_Int_Loi==1) {
    guide->Intervals(T, BlendFunc::NextShape(S));
  }
  else {
    TColStd_Array1OfReal IntC(1, Nb_Int_Courbe+1);
    TColStd_Array1OfReal IntL(1, Nb_Int_Loi+1);
    TColStd_SequenceOfReal    Inter;
    guide->Intervals(IntC, BlendFunc::NextShape(S));
    fevol->Intervals(IntL, S);

    FusionneIntervalles( IntC, IntL, Inter);
    for (Standard_Integer ii=1; ii<=Inter.Length(); ii++) {
      T(ii) = Inter(ii);
    }
  } 
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 void BRepBlend_SurfRstEvolRad::GetShape
(Standard_Integer& NbPoles,
Standard_Integer& NbKnots,
Standard_Integer& Degree,
Standard_Integer& NbPoles2d) 
{
  NbPoles2d = 2;
  BlendFunc::GetShape(mySShape,maxang,NbPoles,NbKnots,Degree,myTConv);
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 void BRepBlend_SurfRstEvolRad::GetTolerance
(const Standard_Real BoundTol,
const Standard_Real SurfTol,
const Standard_Real AngleTol,
math_Vector& Tol3d,
math_Vector& Tol1d) const
{
  Standard_Integer low = Tol3d.Lower() , up=Tol3d.Upper();
  Standard_Real Tol;
  Tol= GeomFill::GetTolerance(myTConv, minang, Abs(ray), 
			       AngleTol, SurfTol);
  Tol1d.Init(SurfTol);
  Tol3d.Init(SurfTol);
  Tol3d(low+1) = Tol3d(up-1) = Min( Tol, SurfTol);
  Tol3d(low) = Tol3d(up) = Min( Tol, BoundTol);
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 void BRepBlend_SurfRstEvolRad::Knots(TColStd_Array1OfReal& TKnots) 
{
  GeomFill::Knots(myTConv,TKnots);
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 void BRepBlend_SurfRstEvolRad::Mults(TColStd_Array1OfInteger& TMults) 
{
  GeomFill::Mults(myTConv,TMults);
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 Standard_Boolean BRepBlend_SurfRstEvolRad::Section
(const Blend_Point& P,
TColgp_Array1OfPnt& Poles,
TColgp_Array1OfVec& DPoles,
TColgp_Array1OfPnt2d& Poles2d,
TColgp_Array1OfVec2d& DPoles2d,
TColStd_Array1OfReal& Weigths,
TColStd_Array1OfReal& DWeigths) 
{
  
  gp_Vec d1u1,d1v1,d2u1,d2v1,d2uv1,d1;
  gp_Vec ns,ns2,dnplan,dnw,dn2w;//,np2,dnp2;
  gp_Vec ncrossns;;
  gp_Vec resulu,resulv,temp,tgct,resul;
  gp_Vec d1urst,d1vrst;
  gp_Pnt Center,bid;
  
  Standard_Real norm,ndotns,grosterme,aDray;
  
  math_Vector sol(1,3),valsol(1,3),secmember(1,3);
  math_Matrix gradsol(1,3,1,3);
  
  Standard_Real prm = P.Parameter(),rayprim;
  Standard_Integer low = Poles.Lower();
  Standard_Integer upp = Poles.Upper();
  Standard_Boolean istgt;
  
  tguide->D2(prm,ptgui,d1gui,d2gui);

  tevol->D1(prm,ray,aDray);
  ray=sg1*ray;
  aDray=sg1*aDray;
  normtg = d1gui.Magnitude();
  nplan  = d1gui.Normalized();
  dnplan.SetLinearForm(1./normtg,d2gui,
		       -1./normtg*(nplan.Dot(d2gui)),nplan);
  
  P.ParametersOnS(sol(1),sol(2));
  sol(3) = prmrst = P.ParameterOnC();
  pt2drst = rst->Value(prmrst);
  
  Values(sol,valsol,gradsol);
  
  surf->D2(sol(1),sol(2),pts,d1u1,d1v1,d2u1,d2v1,d2uv1);
  cons.D1(sol(3),ptrst,d1);
  
  temp.SetXYZ(pts.XYZ()- ptgui.XYZ());
  secmember(1) = normtg - dnplan.Dot(temp);
  
  temp.SetXYZ(ptrst.XYZ()- ptgui.XYZ());
  secmember(2) = normtg - dnplan.Dot(temp);
  
  ns = d1u1.Crossed(d1v1);
  ncrossns = nplan.Crossed(ns);
  ndotns = nplan.Dot(ns);
  norm = ncrossns.Magnitude();
  if (norm < Eps)  {
    norm = 1; // Not enough, but it is not necessary to stop
#ifdef OCCT_DEBUG
    cout << " SurfRstEvolRad : Surface single " << endl;
#endif
  }
  
  // Derivative of n1 corresponding to w
  
  grosterme = ncrossns.Dot(dnplan.Crossed(ns))/norm/norm;
  dnw.SetLinearForm((dnplan.Dot(ns)-grosterme*ndotns)/norm,nplan,
		    ndotns/norm,dnplan,
		    grosterme/norm,ns);
  
  temp.SetLinearForm(ndotns/norm,nplan, -1./norm,ns);
  resul.SetLinearForm(ray,temp,gp_Vec(ptrst,pts));

  //secmember(3) = -2.*ray*(dnw.Dot(resul)); // jag 950105 il manquait ray
  secmember(3) = -2.*ray*(dnw.Dot(resul)) - 2.*aDray*(temp.Dot(resul)) + 2.*ray*aDray;
  math_Gauss Resol(gradsol);
  
  if (Resol.IsDone()) {
    Resol.Solve(secmember);
    istgt = Standard_False;
  }
  else {    
    math_SVD SingRS (gradsol);
    if (SingRS.IsDone()) {
      math_Vector DEDT(1,3);
      DEDT = secmember;
      SingRS.Solve(DEDT, secmember, 1.e-6);
      istgt = Standard_False;
    }
    else istgt = Standard_True;
  }

  if (!istgt) {

    tgs.SetLinearForm(secmember(1),d1u1,secmember(2),d1v1);
    tgrst = secmember(3)*d1;
    // Derivative of n1 corresponding to u1
    temp = d2u1.Crossed(d1v1).Added(d1u1.Crossed(d2uv1));
    grosterme = ncrossns.Dot(nplan.Crossed(temp))/norm/norm;
    resulu.SetLinearForm(-(grosterme*ndotns-nplan.Dot(temp))/norm,nplan,
			 grosterme/norm,ns,
			 -1./norm,temp);
    
    // Derivative of n1 corresponding to v1
    temp = d2uv1.Crossed(d1v1).Added(d1u1.Crossed(d2v1));
    grosterme = ncrossns.Dot(nplan.Crossed(temp))/norm/norm;
    resulv.SetLinearForm(-(grosterme*ndotns-nplan.Dot(temp))/norm,nplan,
			 grosterme/norm,ns,
			 -1./norm,temp);
    
    
    dnw.SetLinearForm(secmember(1),resulu,secmember(2),resulv,dnw);
    ns.SetLinearForm(ndotns/norm,nplan, -1./norm,ns);
    
    dn2w.SetLinearForm(ray, dnw, -1., tgrst, tgs);
    dn2w.SetLinearForm(aDray,ns,dn2w);
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
  
  // Tops 2D
  
  Poles2d(Poles2d.Lower()).SetCoord(sol(1),sol(2));
  Poles2d(Poles2d.Upper()).SetCoord(pt2drst.X(),pt2drst.Y());
  if (!istgt) {
    DPoles2d(Poles2d.Lower()).SetCoord(secmember(1),secmember(2));
    surfrst->D1(pt2drst.X(),pt2drst.Y(),bid,d1urst,d1vrst);
    Standard_Real a, b;
    t3dto2d(a,b,tgrst,d1urst,d1vrst);
    DPoles2d(Poles2d.Upper()).SetCoord(a,b);
  }
  
  // Linear Case
  if (mySShape == BlendFunc_Linear) {
    Poles(low) = pts;
    Poles(upp) = ptrst;
    Weigths(low) = 1.0;
    Weigths(upp) = 1.0;
    if (!istgt) {
      DPoles(low) = tgs;
      DPoles(upp) = tgrst;
      DWeigths(low) = 0.0;
      DWeigths(upp) = 0.0;
    }
    return (!istgt);
  }
  
  // Case of the circle
  Center.SetXYZ(pts.XYZ()+ray*ns.XYZ());
  if (!istgt) {
    tgct.SetLinearForm(ray,dnw,aDray,ns,tgs);
  }
  
  if (ray > 0.) {
    ns.Reverse();
    if (!istgt) {
      dnw.Reverse();
    }
  }
  if (choix%2 != 0) {
    nplan.Reverse();
    dnplan.Reverse();
  }
  if (!istgt) {
    if (ray < 0.) { // to avoid Abs(dray) some lines below
      rayprim = -aDray;
    }
    else rayprim = aDray;

    return GeomFill::GetCircle(myTConv, 
			       ns, ns2, 
			       dnw, dn2w, 
			       nplan, dnplan, 
			       pts, ptrst, 
			       tgs, tgrst, 
			       Abs(ray),rayprim , 
			       Center, tgct, 
			       Poles, 
			       DPoles,
			       Weigths, 
			       DWeigths); 
  }
  else {
    GeomFill::GetCircle(myTConv,
			ns, ns2, 
			nplan, pts, ptrst,
			Abs(ray), Center, 
			Poles, Weigths);
    return Standard_False;
  }
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 Standard_Boolean BRepBlend_SurfRstEvolRad::Section (const Blend_Point& /*P*/,
                                                     TColgp_Array1OfPnt& /*Poles*/,
                                                     TColgp_Array1OfVec& /*DPoles*/,
                                                     TColgp_Array1OfVec& /*D2Poles*/,
                                                     TColgp_Array1OfPnt2d& /*Poles2d*/,
                                                     TColgp_Array1OfVec2d& /*DPoles2d*/,
                                                     TColgp_Array1OfVec2d& /*D2Poles2d*/,
                                                     TColStd_Array1OfReal& /*Weigths*/,
                                                     TColStd_Array1OfReal& /*DWeigths*/,
                                                     TColStd_Array1OfReal& /*D2Weigths*/) 
{
    return Standard_False;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 void BRepBlend_SurfRstEvolRad::Section
(const Blend_Point& P,
TColgp_Array1OfPnt& Poles,
TColgp_Array1OfPnt2d& Poles2d,
TColStd_Array1OfReal& Weigths) 
{
  gp_Vec d1u1,d1v1;//,d1;
  gp_Vec ns,ns2;//,temp,np2;
  gp_Pnt Center;
  
  Standard_Real norm,u1,v1,w;
  
  Standard_Real prm = P.Parameter();
  Standard_Integer low = Poles.Lower();
  Standard_Integer upp = Poles.Upper();
  
  tguide->D1(prm,ptgui,d1gui);
  ray = tevol->Value(prm);
  ray=sg1*ray;
  nplan  = d1gui.Normalized();
  
  P.ParametersOnS(u1,v1);
  w = P.ParameterOnC(); //jlr : point on curve not on surface
  gp_Pnt2d  pt2d = rst->Value(w);

  surf->D1(u1,v1,pts,d1u1,d1v1);
  ptrst = cons.Value(w);

  distmin = Min (distmin, pts.Distance(ptrst));
  
  Poles2d(Poles2d.Lower()).SetCoord(u1,v1);
  Poles2d(Poles2d.Upper()).SetCoord(pt2d.X(),pt2d.Y());
  
  // Linear case
  if (mySShape == BlendFunc_Linear) {
    Poles(low) = pts;
    Poles(upp) = ptrst;
    Weigths(low) = 1.0;
    Weigths(upp) = 1.0;
    return;
  }
  
  ns = d1u1.Crossed(d1v1);
  norm = nplan.Crossed(ns).Magnitude();
  
  ns.SetLinearForm(nplan.Dot(ns)/norm,nplan, -1./norm,ns);
  
  Center.SetXYZ(pts.XYZ()+ray*ns.XYZ());
  
  ns2 = gp_Vec(Center,ptrst).Normalized();
  if(ray>0) ns.Reverse();
  if (choix%2 != 0) {
    nplan.Reverse();
  }
  
  GeomFill::GetCircle(myTConv,
		      ns, ns2, 
		      nplan, pts, ptrst,
		      Abs(ray), Center, 
		      Poles, Weigths);
}

void BRepBlend_SurfRstEvolRad::Resolution(const Standard_Integer IC2d,
					  const Standard_Real Tol,
					  Standard_Real& TolU,
					  Standard_Real& TolV) const
{
  if(IC2d == 1){
    TolU = surf->UResolution(Tol);
    TolV = surf->VResolution(Tol);
  }
  else {
    TolU = surfrst->UResolution(Tol);
    TolV = surfrst->VResolution(Tol);
  }
}


