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


#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_HSurfaceTool.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <IntPatch_CSFunction.hxx>
#include <IntPatch_HCurve2dTool.hxx>
#include <math_Matrix.hxx>

#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#endif


#define SURFACE1 (*((Handle(Adaptor3d_HSurface) *)(surface1)))
#define SURFACE2 (*((Handle(Adaptor3d_HSurface) *)(surface2)))
#define CURVE    (*((Handle(Adaptor2d_HCurve2d) *)(curve)))

IntPatch_CSFunction::IntPatch_CSFunction(const Handle(Adaptor3d_HSurface)& S1,
					       const Handle(Adaptor2d_HCurve2d)& C,
					       const Handle(Adaptor3d_HSurface)& S2)
{
  surface1 = (Standard_Address)(&S1);
  surface2 = (Standard_Address)(&S2);
  curve    = (Standard_Address)(&C);
  f = 0.;
}

Standard_Integer IntPatch_CSFunction::NbVariables()const { return 3;}

Standard_Integer IntPatch_CSFunction::NbEquations()const { return 3;}

Standard_Boolean IntPatch_CSFunction::Value(const math_Vector& X,
			                    math_Vector& F){

  gp_Pnt Psurf(Adaptor3d_HSurfaceTool::Value(SURFACE1,X(1),X(2)));
  gp_Pnt2d p2d(IntPatch_HCurve2dTool::Value(CURVE,X(3)));
  gp_Pnt Pcurv(Adaptor3d_HSurfaceTool::Value(SURFACE2,p2d.X(),p2d.Y()));

  F(1) = Psurf.X()-Pcurv.X();
  F(2) = Psurf.Y()-Pcurv.Y();
  F(3) = Psurf.Z()-Pcurv.Z();
  f = F(1)*F(1)+ F(2)*F(2)+ F(3)*F(3);
  p = gp_Pnt((Psurf.XYZ()+Pcurv.XYZ())/2.);
  return Standard_True;
}

Standard_Boolean IntPatch_CSFunction::Derivatives ( const math_Vector& X,
						    math_Matrix& D) {
  gp_Pnt Psurf,Pcurv;
  gp_Vec D1u,D1v,D1w;
  gp_Pnt2d p2d;
  gp_Vec2d d2d;
  gp_Vec d1u,d1v;

  Adaptor3d_HSurfaceTool::D1(SURFACE1,X(1),X(2),Psurf,D1u,D1v);
  IntPatch_HCurve2dTool::D1(CURVE,X(3),p2d,d2d);
  Adaptor3d_HSurfaceTool::D1(SURFACE2,p2d.X(),p2d.Y(),Pcurv,d1u,d1v);
  D1w.SetLinearForm(d2d.X(),d1u,d2d.Y(),d1v);

  D(1,1) = D1u.X();
  D(1,2) = D1v.X();
  D(1,3) = -D1w.X();
  D(2,1) = D1u.Y();
  D(2,2) = D1v.Y();
  D(2,3) = -D1w.Y();
  D(3,1) = D1u.Z();
  D(3,2) = D1v.Z();
  D(3,3) = -D1w.Z();
  return Standard_True;
} 

Standard_Boolean IntPatch_CSFunction::Values( const math_Vector& X,
					      math_Vector& F,
					      math_Matrix& D) {
  gp_Pnt Psurf,Pcurv;
  gp_Vec D1u,D1v,D1w;

  gp_Pnt2d p2d;
  gp_Vec2d d2d;
  gp_Vec d1u,d1v;

  Adaptor3d_HSurfaceTool::D1(SURFACE1,X(1),X(2),Psurf,D1u,D1v);
  IntPatch_HCurve2dTool::D1(CURVE,X(3),p2d,d2d);
  Adaptor3d_HSurfaceTool::D1(SURFACE2,p2d.X(),p2d.Y(),Pcurv,d1u,d1v);
  D1w.SetLinearForm(d2d.X(),d1u,d2d.Y(),d1v);

  D(1,1) = D1u.X();
  D(1,2) = D1v.X();
  D(1,3) = -D1w.X();
  D(2,1) = D1u.Y();
  D(2,2) = D1v.Y();
  D(2,3) = -D1w.Y();
  D(3,1) = D1u.Z();
  D(3,2) = D1v.Z();
  D(3,3) = -D1w.Z();
  F(1) = Psurf.X()-Pcurv.X();
  F(2) = Psurf.Y()-Pcurv.Y();
  F(3) = Psurf.Z()-Pcurv.Z();
  f = F(1)*F(1)+ F(2)*F(2)+ F(3)*F(3);
  p = gp_Pnt((Psurf.XYZ()+Pcurv.XYZ())/2.);
  return Standard_True;
}

const gp_Pnt& IntPatch_CSFunction::Point() const { return p;}

Standard_Real IntPatch_CSFunction::Root() const { return f;}

const Handle(Adaptor3d_HSurface)& IntPatch_CSFunction::AuxillarSurface() const { 
  return SURFACE1;}

const Handle(Adaptor2d_HCurve2d)& IntPatch_CSFunction::AuxillarCurve() const { 
  return CURVE;}

#undef SURFACE1
#undef SURFACE2
#undef CURVE
