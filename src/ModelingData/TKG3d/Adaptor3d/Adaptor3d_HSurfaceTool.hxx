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

#ifndef _Adaptor3d_HSurfaceTool_HeaderFile
#define _Adaptor3d_HSurfaceTool_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Cone.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_Vec.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <NCollection_Array1.hxx>

class Adaptor3d_HSurfaceTool
{
public:
  DEFINE_STANDARD_ALLOC

  static double FirstUParameter(const occ::handle<Adaptor3d_Surface>& theSurf)
  {
    return theSurf->FirstUParameter();
  }

  static double FirstVParameter(const occ::handle<Adaptor3d_Surface>& theSurf)
  {
    return theSurf->FirstVParameter();
  }

  static double LastUParameter(const occ::handle<Adaptor3d_Surface>& theSurf)
  {
    return theSurf->LastUParameter();
  }

  static double LastVParameter(const occ::handle<Adaptor3d_Surface>& theSurf)
  {
    return theSurf->LastVParameter();
  }

  static int NbUIntervals(const occ::handle<Adaptor3d_Surface>& theSurf, const GeomAbs_Shape theSh)
  {
    return theSurf->NbUIntervals(theSh);
  }

  static int NbVIntervals(const occ::handle<Adaptor3d_Surface>& theSurf, const GeomAbs_Shape theSh)
  {
    return theSurf->NbVIntervals(theSh);
  }

  static void UIntervals(const occ::handle<Adaptor3d_Surface>& theSurf,
                         NCollection_Array1<double>&           theTab,
                         const GeomAbs_Shape                   theSh)
  {
    theSurf->UIntervals(theTab, theSh);
  }

  static void VIntervals(const occ::handle<Adaptor3d_Surface>& theSurf,
                         NCollection_Array1<double>&           theTab,
                         const GeomAbs_Shape                   theSh)
  {
    theSurf->VIntervals(theTab, theSh);
  }

  //! If <First> >= <Last>
  static occ::handle<Adaptor3d_Surface> UTrim(const occ::handle<Adaptor3d_Surface>& theSurf,
                                              const double                          theFirst,
                                              const double                          theLast,
                                              const double                          theTol)
  {
    return theSurf->UTrim(theFirst, theLast, theTol);
  }

  //! If <First> >= <Last>
  static occ::handle<Adaptor3d_Surface> VTrim(const occ::handle<Adaptor3d_Surface>& theSurf,
                                              const double                          theFirst,
                                              const double                          theLast,
                                              const double                          theTol)
  {
    return theSurf->VTrim(theFirst, theLast, theTol);
  }

  static bool IsUClosed(const occ::handle<Adaptor3d_Surface>& theSurf)
  {
    return theSurf->IsUClosed();
  }

  static bool IsVClosed(const occ::handle<Adaptor3d_Surface>& theSurf)
  {
    return theSurf->IsVClosed();
  }

  static bool IsUPeriodic(const occ::handle<Adaptor3d_Surface>& theSurf)
  {
    return theSurf->IsUPeriodic();
  }

  static double UPeriod(const occ::handle<Adaptor3d_Surface>& theSurf)
  {
    return theSurf->UPeriod();
  }

  static bool IsVPeriodic(const occ::handle<Adaptor3d_Surface>& theSurf)
  {
    return theSurf->IsVPeriodic();
  }

  static double VPeriod(const occ::handle<Adaptor3d_Surface>& theSurf)
  {
    return theSurf->VPeriod();
  }

  static gp_Pnt Value(const occ::handle<Adaptor3d_Surface>& theSurf,
                      const double                          theU,
                      const double                          theV)
  {
    return theSurf->Value(theU, theV);
  }

  static void D0(const occ::handle<Adaptor3d_Surface>& theSurf,
                 const double                          theU,
                 const double                          theV,
                 gp_Pnt&                               thePnt)
  {
    theSurf->D0(theU, theV, thePnt);
  }

  static void D1(const occ::handle<Adaptor3d_Surface>& theSurf,
                 const double                          theU,
                 const double                          theV,
                 gp_Pnt&                               thePnt,
                 gp_Vec&                               theD1U,
                 gp_Vec&                               theD1V)
  {
    theSurf->D1(theU, theV, thePnt, theD1U, theD1V);
  }

  static void D2(const occ::handle<Adaptor3d_Surface>& theSurf,
                 const double                          theU,
                 const double                          theV,
                 gp_Pnt&                               thePnt,
                 gp_Vec&                               theD1U,
                 gp_Vec&                               theD1V,
                 gp_Vec&                               theD2U,
                 gp_Vec&                               theD2V,
                 gp_Vec&                               theD2UV)
  {
    theSurf->D2(theU, theV, thePnt, theD1U, theD1V, theD2U, theD2V, theD2UV);
  }

  static void D3(const occ::handle<Adaptor3d_Surface>& theSurf,
                 const double                          theU,
                 const double                          theV,
                 gp_Pnt&                               thePnt,
                 gp_Vec&                               theD1U,
                 gp_Vec&                               theD1V,
                 gp_Vec&                               theD2U,
                 gp_Vec&                               theD2V,
                 gp_Vec&                               theD2UV,
                 gp_Vec&                               theD3U,
                 gp_Vec&                               theD3V,
                 gp_Vec&                               theD3UUV,
                 gp_Vec&                               theD3UVV)
  {
    theSurf->D3(theU,
                theV,
                thePnt,
                theD1U,
                theD1V,
                theD2U,
                theD2V,
                theD2UV,
                theD3U,
                theD3V,
                theD3UUV,
                theD3UVV);
  }

  static gp_Vec DN(const occ::handle<Adaptor3d_Surface>& theSurf,
                   const double                          theU,
                   const double                          theV,
                   const int                             theNU,
                   const int                             theNV)
  {
    return theSurf->DN(theU, theV, theNU, theNV);
  }

  static double UResolution(const occ::handle<Adaptor3d_Surface>& theSurf, const double theR3d)
  {
    return theSurf->UResolution(theR3d);
  }

  static double VResolution(const occ::handle<Adaptor3d_Surface>& theSurf, const double theR3d)
  {
    return theSurf->VResolution(theR3d);
  }

  static GeomAbs_SurfaceType GetType(const occ::handle<Adaptor3d_Surface>& theSurf)
  {
    return theSurf->GetType();
  }

  static gp_Pln Plane(const occ::handle<Adaptor3d_Surface>& theSurf) { return theSurf->Plane(); }

  static gp_Cylinder Cylinder(const occ::handle<Adaptor3d_Surface>& theSurf)
  {
    return theSurf->Cylinder();
  }

  static gp_Cone Cone(const occ::handle<Adaptor3d_Surface>& theSurf) { return theSurf->Cone(); }

  static gp_Torus Torus(const occ::handle<Adaptor3d_Surface>& theSurf) { return theSurf->Torus(); }

  static gp_Sphere Sphere(const occ::handle<Adaptor3d_Surface>& theSurf)
  {
    return theSurf->Sphere();
  }

  static occ::handle<Geom_BezierSurface> Bezier(const occ::handle<Adaptor3d_Surface>& theSurf)
  {
    return theSurf->Bezier();
  }

  static occ::handle<Geom_BSplineSurface> BSpline(const occ::handle<Adaptor3d_Surface>& theSurf)
  {
    return theSurf->BSpline();
  }

  static gp_Ax1 AxeOfRevolution(const occ::handle<Adaptor3d_Surface>& theSurf)
  {
    return theSurf->AxeOfRevolution();
  }

  static gp_Dir Direction(const occ::handle<Adaptor3d_Surface>& theSurf)
  {
    return theSurf->Direction();
  }

  static occ::handle<Adaptor3d_Curve> BasisCurve(const occ::handle<Adaptor3d_Surface>& theSurf)
  {
    return theSurf->BasisCurve();
  }

  static occ::handle<Adaptor3d_Surface> BasisSurface(const occ::handle<Adaptor3d_Surface>& theSurf)
  {
    return theSurf->BasisSurface();
  }

  static double OffsetValue(const occ::handle<Adaptor3d_Surface>& theSurf)
  {
    return theSurf->OffsetValue();
  }

  Standard_EXPORT static bool IsSurfG1(const occ::handle<Adaptor3d_Surface>& theSurf,
                                       const bool                            theAlongU,
                                       const double theAngTol = Precision::Angular());

  Standard_EXPORT static int NbSamplesU(const occ::handle<Adaptor3d_Surface>& S);

  Standard_EXPORT static int NbSamplesV(const occ::handle<Adaptor3d_Surface>& S);

  Standard_EXPORT static int NbSamplesU(const occ::handle<Adaptor3d_Surface>& S,
                                        const double                          u1,
                                        const double                          u2);

  Standard_EXPORT static int NbSamplesV(const occ::handle<Adaptor3d_Surface>&,
                                        const double v1,
                                        const double v2);
};

#endif // _Adaptor3d_HSurfaceTool_HeaderFile
