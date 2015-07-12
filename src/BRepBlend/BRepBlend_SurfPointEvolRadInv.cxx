// Created on: 1997-07-29
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


#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <BRepBlend_SurfPointEvolRadInv.hxx>
#include <gp_Pnt.hxx>
#include <Law_Function.hxx>
#include <math_Matrix.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BRepBlend_SurfPointEvolRadInv::BRepBlend_SurfPointEvolRadInv
(const Handle(Adaptor3d_HSurface)& S,
const Handle(Adaptor3d_HCurve)& C,
 const Handle(Law_Function)& Evol
) : surf(S), curv(C)
{ tevol=Evol;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 void BRepBlend_SurfPointEvolRadInv::Set(const Standard_Integer Choix) 
{  choix = Choix;
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
 Standard_Integer BRepBlend_SurfPointEvolRadInv::NbEquations() const
{
  return 3;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 Standard_Boolean BRepBlend_SurfPointEvolRadInv::Value(const math_Vector& X,math_Vector& F) 
{
  Standard_Real theD,norm,unsurnorm;
  gp_Pnt ptcur,pts;
  gp_Vec d1cur,d1u,d1v;
  gp_XYZ nplan(0.,0.,0.),ns(0.,0.,0.),ref(0.,0.,0.);
  curv->D1(X(1),ptcur,d1cur);
  ray = sg1*tevol->Value(X(1));
  nplan = d1cur.Normalized().XYZ();
  theD = -(nplan.Dot(ptcur.XYZ()));
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
//function : 
//purpose  : 
//=======================================================================
 Standard_Boolean BRepBlend_SurfPointEvolRadInv::Derivatives(const math_Vector& X,math_Matrix& D) 
{
  gp_Pnt ptcur,pts;
  gp_Vec d1cur,d2cur,nplan,dnplan,d1u,d1v,d2u,d2v,duv;
  Standard_Real dtheD, normd1cur, unsurnormd1cur,dray;

  curv->D2(X(1),ptcur,d1cur,d2cur);
  tevol->D1(X(1),ray,dray);
  ray=sg1*ray;
  dray=sg1*dray;
  normd1cur = d1cur.Magnitude();
  unsurnormd1cur = 1./normd1cur;
  nplan = unsurnormd1cur * d1cur;
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
  dwref.SetLinearForm(1.,dwref,dray*unsurnorm,temp);
  duref.SetLinearForm(raysurnorm,dutemp,-raysurnorm2*dunorm,temp,d1u);
  dvref.SetLinearForm(raysurnorm,dvtemp,-raysurnorm2*dvnorm,temp,d1v);
  
  ref.Add(ref);
  D(3,1) = ref.Dot(dwref) - 2.*dray*ray;
  D(3,2) = ref.Dot(duref);
  D(3,3) = ref.Dot(dvref);

  return Standard_True;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 Standard_Boolean BRepBlend_SurfPointEvolRadInv::Values(const math_Vector& X,math_Vector& F,math_Matrix& D) 
{
  gp_Pnt ptcur,pts;
  gp_Vec d1cur,d2cur,nplan,dnplan,d1u,d1v,d2u,d2v,duv;
  Standard_Real theD, dtheD, normd1cur, unsurnormd1cur,dray;

  curv->D2(X(1),ptcur,d1cur,d2cur);
  tevol->D1(X(1),ray,dray);
  ray=sg1*ray;
  dray=sg1*dray;
  surf->D2(X(2),X(3),pts,d1u,d1v,d2u,d2v,duv);
  normd1cur = d1cur.Magnitude();
  unsurnormd1cur = 1./normd1cur;
  nplan = unsurnormd1cur * d1cur;
  theD = -(nplan.XYZ().Dot(ptcur.XYZ()));
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
  dwref.SetLinearForm(1.,dwref,dray*unsurnorm,temp);
  duref.SetLinearForm(raysurnorm,dutemp,-raysurnorm2*dunorm,temp,d1u);
  dvref.SetLinearForm(raysurnorm,dvtemp,-raysurnorm2*dvnorm,temp,d1v);
  
  ref.Add(ref);
  D(3,1) = ref.Dot(dwref) - 2.*dray*ray;
  D(3,2) = ref.Dot(duref);
  D(3,3) = ref.Dot(dvref);

  return Standard_True;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 void BRepBlend_SurfPointEvolRadInv::Set(const gp_Pnt& P) 
{
  point = P;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 void BRepBlend_SurfPointEvolRadInv::GetTolerance(math_Vector& Tolerance,const Standard_Real Tol) const
{
  Tolerance(1) = curv->Resolution(Tol);
  Tolerance(2) = surf->UResolution(Tol);
  Tolerance(3) = surf->VResolution(Tol);
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 void BRepBlend_SurfPointEvolRadInv::GetBounds(math_Vector& InfBound,math_Vector& SupBound) const
{
  InfBound(1) = curv->FirstParameter();
  SupBound(1) = curv->LastParameter();
  InfBound(2) = surf->FirstUParameter();
  SupBound(2) = surf->LastUParameter();
  InfBound(3) = surf->FirstVParameter();
  SupBound(3) = surf->LastVParameter();
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
 Standard_Boolean BRepBlend_SurfPointEvolRadInv::IsSolution(const math_Vector& Sol,const Standard_Real Tol) 
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

