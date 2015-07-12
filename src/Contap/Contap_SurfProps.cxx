// Created on: 1995-02-24
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


#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_HSurfaceTool.hxx>
#include <Contap_SurfProps.hxx>
#include <ElSLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

//=======================================================================
//function : Normale
//purpose  : 
//=======================================================================
void Contap_SurfProps::Normale(const Handle(Adaptor3d_HSurface)& S, 
                               const Standard_Real U, 
                               const Standard_Real V,
                               gp_Pnt& P,
                               gp_Vec& Norm)
{

  GeomAbs_SurfaceType typS = Adaptor3d_HSurfaceTool::GetType(S);
  switch (typS) {
  case GeomAbs_Plane:
    {
      gp_Pln pl(Adaptor3d_HSurfaceTool::Plane(S));
      Norm = pl.Axis().Direction();
      P = ElSLib::Value(U,V,pl);
      if (!pl.Direct()) {
        Norm.Reverse();
      }
    }
    break;


  case GeomAbs_Sphere:
    {
      gp_Sphere sp(Adaptor3d_HSurfaceTool::Sphere(S));
      P = ElSLib::Value(U,V,sp);
      Norm = gp_Vec(sp.Location(),P);
      if (sp.Direct()) {
        Norm.Divide(sp.Radius());
      }
      else {
        Norm.Divide(-sp.Radius());
      }
    }
    break;

  case GeomAbs_Cylinder:
    {
      gp_Cylinder cy(Adaptor3d_HSurfaceTool::Cylinder(S));
      P = ElSLib::Value(U,V,cy);
      Norm.SetLinearForm(Cos(U),cy.XAxis().Direction(),
        Sin(U),cy.YAxis().Direction());
      if (!cy.Direct()) {
        Norm.Reverse();
      }
    }
    break;


  case GeomAbs_Cone:
    {
      gp_Cone co(Adaptor3d_HSurfaceTool::Cone(S));
      P = ElSLib::Value(U,V,co);
      Standard_Real Angle = co.SemiAngle();
      Standard_Real Sina = sin(Angle);
      Standard_Real Cosa = cos(Angle);
      Standard_Real Rad = co.RefRadius(); 

      Standard_Real Vcalc = V;
      if (Abs(V*Sina + Rad) <= 1e-12) { // on est a l`apex
        /*
        Standard_Real Vfi = Adaptor3d_HSurfaceTool::FirstVParameter(S);
        if (Vfi < -Rad/Sina) { // partie valide pour V < Vapex
        Vcalc = V - 1;
        }
        else {
        Vcalc = V + 1.;
        }
        */
        Norm.SetCoord(0,0,0);
        return;
      }

      if (Rad + Vcalc*Sina < 0.) {
        Norm.SetLinearForm(Sina,       co.Axis().Direction(),
          Cosa*cos(U),co.XAxis().Direction(),
          Cosa*sin(U),co.YAxis().Direction());
      }
      else {
        Norm.SetLinearForm(-Sina,       co.Axis().Direction(),
          Cosa*cos(U),co.XAxis().Direction(),
          Cosa*sin(U),co.YAxis().Direction());
      }
      if (!co.Direct()) {
        Norm.Reverse();
      }
    }
    break;
  default:
    {
      gp_Vec d1u,d1v;
      Adaptor3d_HSurfaceTool::D1(S,U,V,P,d1u,d1v);
      Norm = d1u.Crossed(d1v);
    }
    break;


  }
}


//=======================================================================
//function : DerivAndNorm
//purpose  : 
//=======================================================================

void Contap_SurfProps::DerivAndNorm(const Handle(Adaptor3d_HSurface)& S, 
                                    const Standard_Real U, 
                                    const Standard_Real V,
                                    gp_Pnt& P,
                                    gp_Vec& d1u,
                                    gp_Vec& d1v,
                                    gp_Vec& Norm)
{

  GeomAbs_SurfaceType typS = Adaptor3d_HSurfaceTool::GetType(S);
  switch (typS) {
  case GeomAbs_Plane:
    {
      gp_Pln pl(Adaptor3d_HSurfaceTool::Plane(S));
      Norm = pl.Axis().Direction();
      ElSLib::D1(U,V,pl,P,d1u,d1v);
      if (!pl.Direct()) {
        Norm.Reverse();
      }
    }
    break;


  case GeomAbs_Sphere:
    {
      gp_Sphere sp(Adaptor3d_HSurfaceTool::Sphere(S));
      ElSLib::D1(U,V,sp,P,d1u,d1v);
      Norm = gp_Vec(sp.Location(),P);
      if (sp.Direct()) {
        Norm.Divide(sp.Radius());
      }
      else {
        Norm.Divide(-sp.Radius());
      }
    }
    break;

  case GeomAbs_Cylinder:
    {
      gp_Cylinder cy(Adaptor3d_HSurfaceTool::Cylinder(S));
      ElSLib::D1(U,V,cy,P,d1u,d1v);
      Norm.SetLinearForm(Cos(U),cy.XAxis().Direction(),
        Sin(U),cy.YAxis().Direction());
      if (!cy.Direct()) {
        Norm.Reverse();
      }
    }
    break;


  case GeomAbs_Cone:
    {
      gp_Cone co(Adaptor3d_HSurfaceTool::Cone(S));
      ElSLib::D1(U,V,co,P,d1u,d1v);
      Standard_Real Angle = co.SemiAngle();
      Standard_Real Sina = Sin(Angle);
      Standard_Real Cosa = Cos(Angle);
      Standard_Real Rad = co.RefRadius(); 

      Standard_Real Vcalc = V;
      if (Abs(V*Sina + Rad) <= RealEpsilon()) { // on est a l`apex
        Standard_Real Vfi = Adaptor3d_HSurfaceTool::FirstVParameter(S);
        if (Vfi < -Rad/Sina) { // partie valide pour V < Vapex
          Vcalc = V - 1;
        }
        else {
          Vcalc = V + 1.;
        }
      }

      if (Rad + Vcalc*Sina < 0.) {
        Norm.SetLinearForm(Sina,       co.Axis().Direction(),
          Cosa*Cos(U),co.XAxis().Direction(),
          Cosa*Sin(U),co.YAxis().Direction());
      }
      else {
        Norm.SetLinearForm(-Sina,       co.Axis().Direction(),
          Cosa*Cos(U),co.XAxis().Direction(),
          Cosa*Sin(U),co.YAxis().Direction());
      }
      if (!co.Direct()) {
        Norm.Reverse();
      }
    }
    break;
  default:
    {
      Adaptor3d_HSurfaceTool::D1(S,U,V,P,d1u,d1v);
      Norm = d1u.Crossed(d1v);
    }
    break;
  }
}


//=======================================================================
//function : NormAndDn
//purpose  : 
//=======================================================================

void Contap_SurfProps::NormAndDn(const Handle(Adaptor3d_HSurface)& S, 
                                 const Standard_Real U, 
                                 const Standard_Real V,
                                 gp_Pnt& P,
                                 gp_Vec& Norm,
                                 gp_Vec& Dnu,
                                 gp_Vec& Dnv)
{

  GeomAbs_SurfaceType typS = Adaptor3d_HSurfaceTool::GetType(S);
  switch (typS) {
  case GeomAbs_Plane:
    {
      gp_Pln pl(Adaptor3d_HSurfaceTool::Plane(S));
      P = ElSLib::Value(U,V,pl);
      Norm = pl.Axis().Direction();
      if (!pl.Direct()) {
        Norm.Reverse();
      }
      Dnu = Dnv = gp_Vec(0.,0.,0.);
    }
    break;

  case GeomAbs_Sphere:
    {
      gp_Sphere sp(Adaptor3d_HSurfaceTool::Sphere(S));
      ElSLib::D1(U,V,sp,P,Dnu,Dnv);
      Norm = gp_Vec(sp.Location(),P);
      Standard_Real Rad = sp.Radius();
      if (!sp.Direct()) {
        Rad = -Rad;
      }
      Norm.Divide(Rad);
      Dnu.Divide(Rad);
      Dnv.Divide(Rad);
    }
    break;

  case GeomAbs_Cylinder:
    {
      gp_Cylinder cy(Adaptor3d_HSurfaceTool::Cylinder(S));
      P = ElSLib::Value(U,V,cy);
      Norm.SetLinearForm(Cos(U),cy.XAxis().Direction(),
        Sin(U),cy.YAxis().Direction());
      Dnu.SetLinearForm(-Sin(U),cy.XAxis().Direction(),
        Cos(U),cy.YAxis().Direction());
      if (!cy.Direct()) {
        Norm.Reverse();
        Dnu.Reverse();
      }
      Dnv = gp_Vec(0.,0.,0.);
    }
    break;

  case GeomAbs_Cone:
    {

      gp_Cone co(Adaptor3d_HSurfaceTool::Cone(S));
      P = ElSLib::Value(U,V,co);
      Standard_Real Angle = co.SemiAngle();
      Standard_Real Sina = Sin(Angle);
      Standard_Real Cosa = Cos(Angle);
      Standard_Real Rad = co.RefRadius(); 
      Standard_Real Vcalc = V;
      if (Abs(V*Sina + Rad) <= RealEpsilon()) { // on est a l`apex
        Standard_Real Vfi = Adaptor3d_HSurfaceTool::FirstVParameter(S);
        if (Vfi < -Rad/Sina) { // partie valide pour V < Vapex
          Vcalc = V - 1;
        }
        else {
          Vcalc = V + 1.;
        }
      }

      if (Rad + Vcalc*Sina < 0.) {
        Norm.SetLinearForm(Sina,       co.Axis().Direction(),
          Cosa*Cos(U),co.XAxis().Direction(),
          Cosa*Sin(U),co.YAxis().Direction());
      }
      else {
        Norm.SetLinearForm(-Sina,       co.Axis().Direction(),
          Cosa*Cos(U),co.XAxis().Direction(),
          Cosa*Sin(U),co.YAxis().Direction());
      }
      Dnu.SetLinearForm(-Cosa*Sin(U),co.XAxis().Direction(),
        Cosa*Cos(U),co.YAxis().Direction());
      if (!co.Direct()) {
        Norm.Reverse();
        Dnu.Reverse();
      }
      Dnv = gp_Vec(0.,0.,0.);
    }
    break;

  default: 
    {
      gp_Vec d1u,d1v,d2u,d2v,d2uv;
      Adaptor3d_HSurfaceTool::D2(S,U,V,P,d1u,d1v,d2u,d2v,d2uv);
      Norm = d1u.Crossed(d1v);
      Dnu = d2u.Crossed(d1v) + d1u.Crossed(d2uv);
      Dnv = d2uv.Crossed(d1v) + d1u.Crossed(d2v);
    }
    break;
  }
}
