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
#include <Adaptor3d_HSurface.hxx>
#include <BRepBlend_SurfPointConstRadInv.hxx>
#include <gp_Pnt.hxx>
#include <math_Matrix.hxx>

//=======================================================================
//function : BRepBlend_SurfPointConstRadInv
//purpose  : 
//=======================================================================
BRepBlend_SurfPointConstRadInv::BRepBlend_SurfPointConstRadInv
(const Handle(Adaptor3d_HSurface)& S,
 const Handle(Adaptor3d_HCurve)&   C) : surf(S), curv(C)
{
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BRepBlend_SurfPointConstRadInv::Set(const Standard_Real R,
					 const Standard_Integer Choix) 
{
  choix = Choix;
  switch (choix) {
  case 1:
  case 2:
    {
      ray = -Abs(R);
    }
    break;
  case 3:
  case 4:
    {
      ray = Abs(R);
    }
    break;
  default:
    {
      ray = -Abs(R);
    }
  }
}

//=======================================================================
//function : NbEquations
//purpose  : 
//=======================================================================

Standard_Integer BRepBlend_SurfPointConstRadInv::NbEquations() const
{
  return 3;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean BRepBlend_SurfPointConstRadInv::Value(const math_Vector& X,
						       math_Vector& F) 
{
  Standard_Real theD,norm,unsurnorm;
  gp_Pnt ptcur,pts;
  gp_Vec d1cur(0.,0.,0.),d1u(0.,0.,0.),d1v(0.,0.,0.);
  gp_XYZ nplan(0.,0.,0.),ns(0.,0.,0.),ref(0.,0.,0.);
  curv->D1(X(1),ptcur,d1cur);
  nplan = d1cur.Normalized().XYZ();
//  theD = -(nplan.Dot(ptcur.XYZ()));
  gp_XYZ ptcurXYZ(ptcur.XYZ());
  theD =  nplan.Dot(ptcurXYZ)  ;
  theD = theD  * (-1.) ;

  surf->D1(X(2),X(3),pts,d1u,d1v);
  F(1) = nplan.Dot(point.XYZ()) + theD;
  F(2) = nplan.Dot(pts.XYZ()) + theD;
  ns = d1u.Crossed(d1v).XYZ();
  norm = nplan.Crossed(ns).Modulus();
  unsurnorm = 1./norm;
  ns.SetLinearForm(nplan.Dot(ns),nplan, -1.,ns);
  ns.Multiply(unsurnorm);
  ref = pts.XYZ() - point.XYZ();
  ref.SetLinearForm(ray,ns,ref);
  F(3) = ref.SquareModulus() - ray*ray;
  return Standard_True;
}

//=======================================================================
//function : Derivatives
//purpose  : 
//=======================================================================

Standard_Boolean BRepBlend_SurfPointConstRadInv::Derivatives(const math_Vector& X,
							     math_Matrix& D) 
{
  gp_Pnt ptcur,pts;
  gp_Vec d1cur,d2cur,nplan,dnplan,d1u,d1v,d2u,d2v,duv;
  Standard_Real theD, dtheD, normd1cur, unsurnormd1cur;

  curv->D2(X(1),ptcur,d1cur,d2cur);
  normd1cur = d1cur.Magnitude();
  unsurnormd1cur = 1./normd1cur;
  nplan = unsurnormd1cur * d1cur;
//  theD = -(nplan.XYZ().Dot(ptcur.XYZ()));
  gp_XYZ nplanXYZ(nplan.XYZ());
  gp_XYZ ptcurXYZ(ptcur.XYZ());
  theD =  nplanXYZ.Dot(ptcurXYZ)  ;
  theD = theD  * (-1.) ;

  dnplan.SetLinearForm(-nplan.Dot(d2cur),nplan,d2cur);
  dnplan.Multiply(unsurnormd1cur);
  dtheD = - nplan.XYZ().Dot(d1cur.XYZ()) - dnplan.XYZ().Dot(ptcur.XYZ());
  D(1,1) = dnplan.XYZ().Dot(point.XYZ()) + dtheD;
  D(1,2) = D(1,3) = 0.;
  surf->D2(X(2),X(3),pts,d1u,d1v,d2u,d2v,duv);
  D(2,1) = dnplan.XYZ().Dot(pts.XYZ()) + dtheD;
  D(2,2) = nplan.Dot(d1u);
  D(2,3) = nplan.Dot(d1v);
  
  gp_Vec nsurf = d1u.Crossed(d1v);
  gp_Vec dunsurf = d2u.Crossed(d1v).Added(d1u.Crossed(duv));
  gp_Vec dvnsurf = d1u.Crossed(d2v).Added(duv.Crossed(d1v));

  gp_Vec nplancrosnsurf = nplan.Crossed(nsurf);
  gp_Vec dwnplancrosnsurf = dnplan.Crossed(nsurf);
  gp_Vec dunplancrosnsurf = nplan.Crossed(dunsurf);
  gp_Vec dvnplancrosnsurf = nplan.Crossed(dvnsurf);

  Standard_Real norm2      = nplancrosnsurf.SquareMagnitude();
  Standard_Real norm       = sqrt(norm2);
  Standard_Real unsurnorm  = 1./norm;
  Standard_Real raysurnorm = ray*unsurnorm;
  Standard_Real unsurnorm2 = unsurnorm * unsurnorm;
  Standard_Real raysurnorm2 = ray*unsurnorm2;
  Standard_Real dwnorm = unsurnorm*nplancrosnsurf.Dot(dwnplancrosnsurf);
  Standard_Real dunorm = unsurnorm*nplancrosnsurf.Dot(dunplancrosnsurf);
  Standard_Real dvnorm = unsurnorm*nplancrosnsurf.Dot(dvnplancrosnsurf);

  Standard_Real nplandotnsurf   = nplan.Dot(nsurf);
  Standard_Real dwnplandotnsurf = dnplan.Dot(nsurf);
  Standard_Real dunplandotnsurf = nplan.Dot(dunsurf);
  Standard_Real dvnplandotnsurf = nplan.Dot(dvnsurf);
  
  gp_Vec temp,dwtemp,dutemp,dvtemp;
  temp.SetLinearForm(nplandotnsurf,nplan,-1.,nsurf);
  dwtemp.SetLinearForm(nplandotnsurf,dnplan,dwnplandotnsurf,nplan);
  dutemp.SetLinearForm(dunplandotnsurf,nplan,-1.,dunsurf);
  dvtemp.SetLinearForm(dvnplandotnsurf,nplan,-1.,dvnsurf);

  gp_Vec ref,dwref,duref,dvref,corde(point,pts);
  ref.SetLinearForm(raysurnorm,temp,corde);
  dwref.SetLinearForm(raysurnorm,dwtemp,-raysurnorm2*dwnorm,temp);
  duref.SetLinearForm(raysurnorm,dutemp,-raysurnorm2*dunorm,temp,d1u);
  dvref.SetLinearForm(raysurnorm,dvtemp,-raysurnorm2*dvnorm,temp,d1v);
  
  ref.Add(ref);
  D(3,1) = ref.Dot(dwref);
  D(3,2) = ref.Dot(duref);
  D(3,3) = ref.Dot(dvref);

  return Standard_True;
}

//=======================================================================
//function : Values
//purpose  : 
//=======================================================================

Standard_Boolean BRepBlend_SurfPointConstRadInv::Values(const math_Vector& X,
							math_Vector& F,
							math_Matrix& D) 
{
  gp_Pnt ptcur,pts;
  gp_Vec d1cur,d2cur,nplan,dnplan,d1u,d1v,d2u,d2v,duv;
  Standard_Real theD, dtheD, normd1cur, unsurnormd1cur;

  curv->D2(X(1),ptcur,d1cur,d2cur);
  surf->D2(X(2),X(3),pts,d1u,d1v,d2u,d2v,duv);
  normd1cur = d1cur.Magnitude();
  unsurnormd1cur = 1./normd1cur;
  nplan = unsurnormd1cur * d1cur;
//  theD = -(nplan.XYZ().Dot(ptcur.XYZ()));
  gp_XYZ nplanXYZ(nplan.XYZ());
  gp_XYZ ptcurXYZ(ptcur.XYZ());
  theD =  nplanXYZ.Dot(ptcurXYZ)  ;
  theD = theD  * (-1.) ;

  F(1) = nplan.XYZ().Dot(point.XYZ()) + theD;
  F(2) = nplan.XYZ().Dot(pts.XYZ()) + theD;

  dnplan.SetLinearForm(-nplan.Dot(d2cur),nplan,d2cur);
  dnplan.Multiply(unsurnormd1cur);
  dtheD = - nplan.XYZ().Dot(d1cur.XYZ()) - dnplan.XYZ().Dot(ptcur.XYZ());
  D(1,1) = dnplan.XYZ().Dot(point.XYZ()) + dtheD;
  D(1,2) = D(1,3) = 0.;
  D(2,1) = dnplan.XYZ().Dot(pts.XYZ()) + dtheD;
  D(2,2) = nplan.Dot(d1u);
  D(2,3) = nplan.Dot(d1v);
  
  gp_Vec nsurf = d1u.Crossed(d1v);
  gp_Vec dunsurf = d2u.Crossed(d1v).Added(d1u.Crossed(duv));
  gp_Vec dvnsurf = d1u.Crossed(d2v).Added(duv.Crossed(d1v));

  gp_Vec nplancrosnsurf = nplan.Crossed(nsurf);
  gp_Vec dwnplancrosnsurf = dnplan.Crossed(nsurf);
  gp_Vec dunplancrosnsurf = nplan.Crossed(dunsurf);
  gp_Vec dvnplancrosnsurf = nplan.Crossed(dvnsurf);

  Standard_Real norm2      = nplancrosnsurf.SquareMagnitude();
  Standard_Real norm       = sqrt(norm2);
  Standard_Real unsurnorm  = 1./norm;
  Standard_Real raysurnorm = ray*unsurnorm;
  Standard_Real unsurnorm2 = unsurnorm * unsurnorm;
  Standard_Real raysurnorm2 = ray*unsurnorm2;
  Standard_Real dwnorm = unsurnorm*nplancrosnsurf.Dot(dwnplancrosnsurf);
  Standard_Real dunorm = unsurnorm*nplancrosnsurf.Dot(dunplancrosnsurf);
  Standard_Real dvnorm = unsurnorm*nplancrosnsurf.Dot(dvnplancrosnsurf);

  Standard_Real nplandotnsurf   = nplan.Dot(nsurf);
  Standard_Real dwnplandotnsurf = dnplan.Dot(nsurf);
  Standard_Real dunplandotnsurf = nplan.Dot(dunsurf);
  Standard_Real dvnplandotnsurf = nplan.Dot(dvnsurf);
  
  gp_Vec temp,dwtemp,dutemp,dvtemp;
  temp.SetLinearForm(nplandotnsurf,nplan,-1.,nsurf);
  dwtemp.SetLinearForm(nplandotnsurf,dnplan,dwnplandotnsurf,nplan);
  dutemp.SetLinearForm(dunplandotnsurf,nplan,-1.,dunsurf);
  dvtemp.SetLinearForm(dvnplandotnsurf,nplan,-1.,dvnsurf);

  gp_Vec ref,dwref,duref,dvref,corde(point,pts);
  ref.SetLinearForm(raysurnorm,temp,corde);
  F(3) = ref.SquareMagnitude() - ray*ray;
  dwref.SetLinearForm(raysurnorm,dwtemp,-raysurnorm2*dwnorm,temp);
  duref.SetLinearForm(raysurnorm,dutemp,-raysurnorm2*dunorm,temp,d1u);
  dvref.SetLinearForm(raysurnorm,dvtemp,-raysurnorm2*dvnorm,temp,d1v);
  
  ref.Add(ref);
  D(3,1) = ref.Dot(dwref);
  D(3,2) = ref.Dot(duref);
  D(3,3) = ref.Dot(dvref);

  return Standard_True;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BRepBlend_SurfPointConstRadInv::Set(const gp_Pnt& P) 
{
  point = P;
}

//=======================================================================
//function : GetTolerance
//purpose  : 
//=======================================================================

void BRepBlend_SurfPointConstRadInv::GetTolerance(math_Vector& Tolerance,
						  const Standard_Real Tol) const
{
  Tolerance(1) = curv->Resolution(Tol);
  Tolerance(2) = surf->UResolution(Tol);
  Tolerance(3) = surf->VResolution(Tol);
}

//=======================================================================
//function : GetBounds
//purpose  : 
//=======================================================================

void BRepBlend_SurfPointConstRadInv::GetBounds(math_Vector& InfBound,
					       math_Vector& SupBound) const
{
  InfBound(1) = curv->FirstParameter();
  SupBound(1) = curv->LastParameter();
  InfBound(2) = surf->FirstUParameter();
  SupBound(2) = surf->LastUParameter();
  InfBound(3) = surf->FirstVParameter();
  SupBound(3) = surf->LastVParameter();
}

//=======================================================================
//function : IsSolution
//purpose  : 
//=======================================================================

Standard_Boolean BRepBlend_SurfPointConstRadInv::IsSolution(const math_Vector&  Sol,
							    const Standard_Real Tol) 
{
  math_Vector valsol(1,3);
  Value(Sol,valsol);
  if (Abs(valsol(1)) <= Tol && 
      Abs(valsol(2)) <= Tol &&
      Abs(valsol(3)) <= 2*Tol*Abs(ray) ) {
    return Standard_True;
  }
  return Standard_False;
}


