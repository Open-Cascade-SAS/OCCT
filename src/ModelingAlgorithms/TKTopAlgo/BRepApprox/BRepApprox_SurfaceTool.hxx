// Created on: 1995-07-20
// Created by: Modelistation
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

#ifndef _BRepApprox_SurfaceTool_HeaderFile
#define _BRepApprox_SurfaceTool_HeaderFile

#include <BRepAdaptor_Surface.hxx>
#include <GeomAbs_Shape.hxx>
#include <NCollection_Array1.hxx>
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

class Geom_BezierSurface;
class Geom_BSplineSurface;

class BRepApprox_SurfaceTool
{
public:
  DEFINE_STANDARD_ALLOC

  static double FirstUParameter(const BRepAdaptor_Surface& S);

  static double FirstVParameter(const BRepAdaptor_Surface& S);

  static double LastUParameter(const BRepAdaptor_Surface& S);

  static double LastVParameter(const BRepAdaptor_Surface& S);

  static int NbUIntervals(const BRepAdaptor_Surface& S, const GeomAbs_Shape Sh);

  static int NbVIntervals(const BRepAdaptor_Surface& S, const GeomAbs_Shape Sh);

  static void UIntervals(const BRepAdaptor_Surface& S,
                         NCollection_Array1<double>&      T,
                         const GeomAbs_Shape        Sh);

  static void VIntervals(const BRepAdaptor_Surface& S,
                         NCollection_Array1<double>&      T,
                         const GeomAbs_Shape        Sh);

  //! If <First> >= <Last>
  static occ::handle<Adaptor3d_Surface> UTrim(const BRepAdaptor_Surface& S,
                                         const double        First,
                                         const double        Last,
                                         const double        Tol);

  //! If <First> >= <Last>
  static occ::handle<Adaptor3d_Surface> VTrim(const BRepAdaptor_Surface& S,
                                         const double        First,
                                         const double        Last,
                                         const double        Tol);

  static bool IsUClosed(const BRepAdaptor_Surface& S);

  static bool IsVClosed(const BRepAdaptor_Surface& S);

  static bool IsUPeriodic(const BRepAdaptor_Surface& S);

  static double UPeriod(const BRepAdaptor_Surface& S);

  static bool IsVPeriodic(const BRepAdaptor_Surface& S);

  static double VPeriod(const BRepAdaptor_Surface& S);

  static gp_Pnt Value(const BRepAdaptor_Surface& S, const double u, const double v);

  static void D0(const BRepAdaptor_Surface& S,
                 const double        u,
                 const double        v,
                 gp_Pnt&                    P);

  static void D1(const BRepAdaptor_Surface& S,
                 const double        u,
                 const double        v,
                 gp_Pnt&                    P,
                 gp_Vec&                    D1u,
                 gp_Vec&                    D1v);

  static void D2(const BRepAdaptor_Surface& S,
                 const double        u,
                 const double        v,
                 gp_Pnt&                    P,
                 gp_Vec&                    D1U,
                 gp_Vec&                    D1V,
                 gp_Vec&                    D2U,
                 gp_Vec&                    D2V,
                 gp_Vec&                    D2UV);

  static void D3(const BRepAdaptor_Surface& S,
                 const double        u,
                 const double        v,
                 gp_Pnt&                    P,
                 gp_Vec&                    D1U,
                 gp_Vec&                    D1V,
                 gp_Vec&                    D2U,
                 gp_Vec&                    D2V,
                 gp_Vec&                    D2UV,
                 gp_Vec&                    D3U,
                 gp_Vec&                    D3V,
                 gp_Vec&                    D3UUV,
                 gp_Vec&                    D3UVV);

  static gp_Vec DN(const BRepAdaptor_Surface& S,
                   const double        u,
                   const double        v,
                   const int     Nu,
                   const int     Nv);

  static double UResolution(const BRepAdaptor_Surface& S, const double R3d);

  static double VResolution(const BRepAdaptor_Surface& S, const double R3d);

  static GeomAbs_SurfaceType GetType(const BRepAdaptor_Surface& S);

  static gp_Pln Plane(const BRepAdaptor_Surface& S);

  static gp_Cylinder Cylinder(const BRepAdaptor_Surface& S);

  static gp_Cone Cone(const BRepAdaptor_Surface& S);

  static gp_Torus Torus(const BRepAdaptor_Surface& S);

  static gp_Sphere Sphere(const BRepAdaptor_Surface& S);

  static occ::handle<Geom_BezierSurface> Bezier(const BRepAdaptor_Surface& S);

  static occ::handle<Geom_BSplineSurface> BSpline(const BRepAdaptor_Surface& S);

  static gp_Ax1 AxeOfRevolution(const BRepAdaptor_Surface& S);

  static gp_Dir Direction(const BRepAdaptor_Surface& S);

  static occ::handle<Adaptor3d_Curve> BasisCurve(const BRepAdaptor_Surface& S);

  Standard_EXPORT static int NbSamplesU(const BRepAdaptor_Surface& S);

  Standard_EXPORT static int NbSamplesV(const BRepAdaptor_Surface& S);

  Standard_EXPORT static int NbSamplesU(const BRepAdaptor_Surface& S,
                                                     const double        u1,
                                                     const double        u2);

  Standard_EXPORT static int NbSamplesV(const BRepAdaptor_Surface& S,
                                                     const double        v1,
                                                     const double        v2);

};

#include <BRepApprox_SurfaceTool.lxx>

#endif // _BRepApprox_SurfaceTool_HeaderFile
