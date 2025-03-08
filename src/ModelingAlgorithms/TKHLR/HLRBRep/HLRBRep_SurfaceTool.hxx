// Created on: 1993-07-02
// Created by: Laurent BUCHARD
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

#ifndef _HLRBRep_SurfaceTool_HeaderFile
#define _HLRBRep_SurfaceTool_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <GeomAbs_Shape.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <gp_Pln.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Cone.hxx>
#include <gp_Torus.hxx>
#include <gp_Sphere.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>

class gp_Pnt;
class gp_Vec;
class Geom_BezierSurface;
class Geom_BSplineSurface;

class HLRBRep_SurfaceTool
{
public:
  DEFINE_STANDARD_ALLOC

  static Standard_Real FirstUParameter(const Standard_Address S);

  static Standard_Real FirstVParameter(const Standard_Address S);

  static Standard_Real LastUParameter(const Standard_Address S);

  static Standard_Real LastVParameter(const Standard_Address S);

  static Standard_Integer NbUIntervals(const Standard_Address S, const GeomAbs_Shape Sh);

  static Standard_Integer NbVIntervals(const Standard_Address S, const GeomAbs_Shape Sh);

  static void UIntervals(const Standard_Address S, TColStd_Array1OfReal& T, const GeomAbs_Shape Sh);

  static void VIntervals(const Standard_Address S, TColStd_Array1OfReal& T, const GeomAbs_Shape Sh);

  //! If <First> >= <Last>
  static Handle(Adaptor3d_Surface) UTrim(const Standard_Address S,
                                         const Standard_Real    First,
                                         const Standard_Real    Last,
                                         const Standard_Real    Tol);

  //! If <First> >= <Last>
  static Handle(Adaptor3d_Surface) VTrim(const Standard_Address S,
                                         const Standard_Real    First,
                                         const Standard_Real    Last,
                                         const Standard_Real    Tol);

  static Standard_Boolean IsUClosed(const Standard_Address S);

  static Standard_Boolean IsVClosed(const Standard_Address S);

  static Standard_Boolean IsUPeriodic(const Standard_Address S);

  static Standard_Real UPeriod(const Standard_Address S);

  static Standard_Boolean IsVPeriodic(const Standard_Address S);

  static Standard_Real VPeriod(const Standard_Address S);

  static gp_Pnt Value(const Standard_Address S, const Standard_Real u, const Standard_Real v);

  static void D0(const Standard_Address S, const Standard_Real u, const Standard_Real v, gp_Pnt& P);

  static void D1(const Standard_Address S,
                 const Standard_Real    u,
                 const Standard_Real    v,
                 gp_Pnt&                P,
                 gp_Vec&                D1u,
                 gp_Vec&                D1v);

  static void D2(const Standard_Address S,
                 const Standard_Real    u,
                 const Standard_Real    v,
                 gp_Pnt&                P,
                 gp_Vec&                D1U,
                 gp_Vec&                D1V,
                 gp_Vec&                D2U,
                 gp_Vec&                D2V,
                 gp_Vec&                D2UV);

  static void D3(const Standard_Address S,
                 const Standard_Real    u,
                 const Standard_Real    v,
                 gp_Pnt&                P,
                 gp_Vec&                D1U,
                 gp_Vec&                D1V,
                 gp_Vec&                D2U,
                 gp_Vec&                D2V,
                 gp_Vec&                D2UV,
                 gp_Vec&                D3U,
                 gp_Vec&                D3V,
                 gp_Vec&                D3UUV,
                 gp_Vec&                D3UVV);

  static gp_Vec DN(const Standard_Address S,
                   const Standard_Real    u,
                   const Standard_Real    v,
                   const Standard_Integer Nu,
                   const Standard_Integer Nv);

  static Standard_Real UResolution(const Standard_Address S, const Standard_Real R3d);

  static Standard_Real VResolution(const Standard_Address S, const Standard_Real R3d);

  static GeomAbs_SurfaceType GetType(const Standard_Address S);

  static gp_Pln Plane(const Standard_Address S);

  static gp_Cylinder Cylinder(const Standard_Address S);

  static gp_Cone Cone(const Standard_Address S);

  static gp_Torus Torus(const Standard_Address S);

  static gp_Sphere Sphere(const Standard_Address S);

  static Handle(Geom_BezierSurface) Bezier(const Standard_Address S);

  static Handle(Geom_BSplineSurface) BSpline(const Standard_Address S);

  static gp_Ax1 AxeOfRevolution(const Standard_Address S);

  static gp_Dir Direction(const Standard_Address S);

  static Handle(Adaptor3d_Curve) BasisCurve(const Standard_Address S);

  static Handle(Adaptor3d_Surface) BasisSurface(const Standard_Address S);

  static Standard_Real OffsetValue(const Standard_Address S);

  Standard_EXPORT static Standard_Integer NbSamplesU(const Standard_Address S);

  Standard_EXPORT static Standard_Integer NbSamplesV(const Standard_Address S);

  Standard_EXPORT static Standard_Integer NbSamplesU(const Standard_Address S,
                                                     const Standard_Real    u1,
                                                     const Standard_Real    u2);

  Standard_EXPORT static Standard_Integer NbSamplesV(const Standard_Address S,
                                                     const Standard_Real    v1,
                                                     const Standard_Real    v2);

protected:
private:
};

#include <HLRBRep_SurfaceTool.lxx>

#endif // _HLRBRep_SurfaceTool_HeaderFile
