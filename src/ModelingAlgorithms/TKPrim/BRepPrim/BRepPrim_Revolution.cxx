// Created on: 1992-11-06
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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

#include <BRepPrim_Revolution.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

//=================================================================================================

BRepPrim_Revolution::BRepPrim_Revolution(const gp_Ax2&               A,
                                         const double         VMin,
                                         const double         VMax,
                                         const occ::handle<Geom_Curve>&   M,
                                         const occ::handle<Geom2d_Curve>& PM)
    : BRepPrim_OneAxis(BRepPrim_Builder(), A, VMin, VMax),
      myMeridian(M),
      myPMeridian(PM)
{
}

//=================================================================================================

BRepPrim_Revolution::BRepPrim_Revolution(const gp_Ax2&       A,
                                         const double VMin,
                                         const double VMax)
    : BRepPrim_OneAxis(BRepPrim_Builder(), A, VMin, VMax)
{
}

//=================================================================================================

void BRepPrim_Revolution::Meridian(const occ::handle<Geom_Curve>& M, const occ::handle<Geom2d_Curve>& PM)
{
  myMeridian  = M;
  myPMeridian = PM;
}

//=================================================================================================

TopoDS_Face BRepPrim_Revolution::MakeEmptyLateralFace() const
{
  occ::handle<Geom_SurfaceOfRevolution> S = new Geom_SurfaceOfRevolution(myMeridian, Axes().Axis());

  TopoDS_Face F;
  myBuilder.Builder().MakeFace(F, S, Precision::Confusion());
  return F;
}

//=================================================================================================

TopoDS_Edge BRepPrim_Revolution::MakeEmptyMeridianEdge(const double Ang) const
{
  TopoDS_Edge        E;
  occ::handle<Geom_Curve> C = occ::down_cast<Geom_Curve>(myMeridian->Copy());
  gp_Trsf            T;
  T.SetRotation(Axes().Axis(), Ang);
  C->Transform(T);
  myBuilder.Builder().MakeEdge(E, C, Precision::Confusion());
  return E;
}

//=================================================================================================

gp_Pnt2d BRepPrim_Revolution::MeridianValue(const double V) const
{
  return myPMeridian->Value(V);
}

//=================================================================================================

void BRepPrim_Revolution::SetMeridianPCurve(TopoDS_Edge& E, const TopoDS_Face& F) const
{
  myBuilder.Builder().UpdateEdge(E, myPMeridian, F, Precision::Confusion());
}
