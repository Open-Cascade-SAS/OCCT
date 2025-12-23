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
class HLRBRep_Surface;

class HLRBRep_SurfaceTool
{
public:
  DEFINE_STANDARD_ALLOC

  static double FirstUParameter(const HLRBRep_Surface* theSurf);

  static double FirstVParameter(const HLRBRep_Surface* theSurf);

  static double LastUParameter(const HLRBRep_Surface* theSurf);

  static double LastVParameter(const HLRBRep_Surface* theSurf);

  static int NbUIntervals(const HLRBRep_Surface* theSurf, const GeomAbs_Shape theSh);

  static int NbVIntervals(const HLRBRep_Surface* theSurf, const GeomAbs_Shape theSh);

  static void UIntervals(const HLRBRep_Surface* theSurf,
                         TColStd_Array1OfReal&  theT,
                         const GeomAbs_Shape    theSh);

  static void VIntervals(const HLRBRep_Surface* theSurf,
                         TColStd_Array1OfReal&  theT,
                         const GeomAbs_Shape    theSh);

  //! If <theFirst> >= <theLast>
  static Handle(Adaptor3d_Surface) UTrim(const HLRBRep_Surface* theSurf,
                                         const double           theFirst,
                                         const double           theLast,
                                         const double           theTol);

  //! If <theFirst> >= <theLast>
  static Handle(Adaptor3d_Surface) VTrim(const HLRBRep_Surface* theSurf,
                                         const double           theFirst,
                                         const double           theLast,
                                         const double           theTol);

  static bool IsUClosed(const HLRBRep_Surface* theSurf);

  static bool IsVClosed(const HLRBRep_Surface* theSurf);

  static bool IsUPeriodic(const HLRBRep_Surface* theSurf);

  static double UPeriod(const HLRBRep_Surface* theSurf);

  static bool IsVPeriodic(const HLRBRep_Surface* theSurf);

  static double VPeriod(const HLRBRep_Surface* theSurf);

  static gp_Pnt Value(const HLRBRep_Surface* theSurf, const double theU, const double theV);

  static void D0(const HLRBRep_Surface* theSurf, const double theU, const double theV, gp_Pnt& theP);

  static void D1(const HLRBRep_Surface* theSurf,
                 const double           theU,
                 const double           theV,
                 gp_Pnt&                theP,
                 gp_Vec&                theD1U,
                 gp_Vec&                theD1V);

  static void D2(const HLRBRep_Surface* theSurf,
                 const double           theU,
                 const double           theV,
                 gp_Pnt&                theP,
                 gp_Vec&                theD1U,
                 gp_Vec&                theD1V,
                 gp_Vec&                theD2U,
                 gp_Vec&                theD2V,
                 gp_Vec&                theD2UV);

  static void D3(const HLRBRep_Surface* theSurf,
                 const double           theU,
                 const double           theV,
                 gp_Pnt&                theP,
                 gp_Vec&                theD1U,
                 gp_Vec&                theD1V,
                 gp_Vec&                theD2U,
                 gp_Vec&                theD2V,
                 gp_Vec&                theD2UV,
                 gp_Vec&                theD3U,
                 gp_Vec&                theD3V,
                 gp_Vec&                theD3UUV,
                 gp_Vec&                theD3UVV);

  static gp_Vec DN(const HLRBRep_Surface* theSurf,
                   const double           theU,
                   const double           theV,
                   const int              theNu,
                   const int              theNv);

  static double UResolution(const HLRBRep_Surface* theSurf, const double theR3d);

  static double VResolution(const HLRBRep_Surface* theSurf, const double theR3d);

  static GeomAbs_SurfaceType GetType(const HLRBRep_Surface* theSurf);

  static gp_Pln Plane(const HLRBRep_Surface* theSurf);

  static gp_Cylinder Cylinder(const HLRBRep_Surface* theSurf);

  static gp_Cone Cone(const HLRBRep_Surface* theSurf);

  static gp_Torus Torus(const HLRBRep_Surface* theSurf);

  static gp_Sphere Sphere(const HLRBRep_Surface* theSurf);

  static Handle(Geom_BezierSurface) Bezier(const HLRBRep_Surface* theSurf);

  static Handle(Geom_BSplineSurface) BSpline(const HLRBRep_Surface* theSurf);

  static gp_Ax1 AxeOfRevolution(const HLRBRep_Surface* theSurf);

  static gp_Dir Direction(const HLRBRep_Surface* theSurf);

  static Handle(Adaptor3d_Curve) BasisCurve(const HLRBRep_Surface* theSurf);

  static Handle(Adaptor3d_Surface) BasisSurface(const HLRBRep_Surface* theSurf);

  static double OffsetValue(const HLRBRep_Surface* theSurf);

  Standard_EXPORT static int NbSamplesU(const HLRBRep_Surface* theSurf);

  Standard_EXPORT static int NbSamplesV(const HLRBRep_Surface* theSurf);

  Standard_EXPORT static int NbSamplesU(const HLRBRep_Surface* theSurf,
                                        const double           theU1,
                                        const double           theU2);

  Standard_EXPORT static int NbSamplesV(const HLRBRep_Surface* theSurf,
                                        const double           theV1,
                                        const double           theV2);

protected:
private:
};

#include <HLRBRep_SurfaceTool.lxx>

#endif // _HLRBRep_SurfaceTool_HeaderFile
