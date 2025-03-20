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
#include <TColStd_Array1OfReal.hxx>

class Adaptor3d_HSurfaceTool
{
public:
  DEFINE_STANDARD_ALLOC

  static Standard_Real FirstUParameter(const Handle(Adaptor3d_Surface)& theSurf)
  {
    return theSurf->FirstUParameter();
  }

  static Standard_Real FirstVParameter(const Handle(Adaptor3d_Surface)& theSurf)
  {
    return theSurf->FirstVParameter();
  }

  static Standard_Real LastUParameter(const Handle(Adaptor3d_Surface)& theSurf)
  {
    return theSurf->LastUParameter();
  }

  static Standard_Real LastVParameter(const Handle(Adaptor3d_Surface)& theSurf)
  {
    return theSurf->LastVParameter();
  }

  static Standard_Integer NbUIntervals(const Handle(Adaptor3d_Surface)& theSurf,
                                       const GeomAbs_Shape              theSh)
  {
    return theSurf->NbUIntervals(theSh);
  }

  static Standard_Integer NbVIntervals(const Handle(Adaptor3d_Surface)& theSurf,
                                       const GeomAbs_Shape              theSh)
  {
    return theSurf->NbVIntervals(theSh);
  }

  static void UIntervals(const Handle(Adaptor3d_Surface)& theSurf,
                         TColStd_Array1OfReal&            theTab,
                         const GeomAbs_Shape              theSh)
  {
    theSurf->UIntervals(theTab, theSh);
  }

  static void VIntervals(const Handle(Adaptor3d_Surface)& theSurf,
                         TColStd_Array1OfReal&            theTab,
                         const GeomAbs_Shape              theSh)
  {
    theSurf->VIntervals(theTab, theSh);
  }

  //! If <First> >= <Last>
  static Handle(Adaptor3d_Surface) UTrim(const Handle(Adaptor3d_Surface)& theSurf,
                                         const Standard_Real              theFirst,
                                         const Standard_Real              theLast,
                                         const Standard_Real              theTol)
  {
    return theSurf->UTrim(theFirst, theLast, theTol);
  }

  //! If <First> >= <Last>
  static Handle(Adaptor3d_Surface) VTrim(const Handle(Adaptor3d_Surface)& theSurf,
                                         const Standard_Real              theFirst,
                                         const Standard_Real              theLast,
                                         const Standard_Real              theTol)
  {
    return theSurf->VTrim(theFirst, theLast, theTol);
  }

  static Standard_Boolean IsUClosed(const Handle(Adaptor3d_Surface)& theSurf)
  {
    return theSurf->IsUClosed();
  }

  static Standard_Boolean IsVClosed(const Handle(Adaptor3d_Surface)& theSurf)
  {
    return theSurf->IsVClosed();
  }

  static Standard_Boolean IsUPeriodic(const Handle(Adaptor3d_Surface)& theSurf)
  {
    return theSurf->IsUPeriodic();
  }

  static Standard_Real UPeriod(const Handle(Adaptor3d_Surface)& theSurf)
  {
    return theSurf->UPeriod();
  }

  static Standard_Boolean IsVPeriodic(const Handle(Adaptor3d_Surface)& theSurf)
  {
    return theSurf->IsVPeriodic();
  }

  static Standard_Real VPeriod(const Handle(Adaptor3d_Surface)& theSurf)
  {
    return theSurf->VPeriod();
  }

  static gp_Pnt Value(const Handle(Adaptor3d_Surface)& theSurf,
                      const Standard_Real              theU,
                      const Standard_Real              theV)
  {
    return theSurf->Value(theU, theV);
  }

  static void D0(const Handle(Adaptor3d_Surface)& theSurf,
                 const Standard_Real              theU,
                 const Standard_Real              theV,
                 gp_Pnt&                          thePnt)
  {
    theSurf->D0(theU, theV, thePnt);
  }

  static void D1(const Handle(Adaptor3d_Surface)& theSurf,
                 const Standard_Real              theU,
                 const Standard_Real              theV,
                 gp_Pnt&                          thePnt,
                 gp_Vec&                          theD1U,
                 gp_Vec&                          theD1V)
  {
    theSurf->D1(theU, theV, thePnt, theD1U, theD1V);
  }

  static void D2(const Handle(Adaptor3d_Surface)& theSurf,
                 const Standard_Real              theU,
                 const Standard_Real              theV,
                 gp_Pnt&                          thePnt,
                 gp_Vec&                          theD1U,
                 gp_Vec&                          theD1V,
                 gp_Vec&                          theD2U,
                 gp_Vec&                          theD2V,
                 gp_Vec&                          theD2UV)
  {
    theSurf->D2(theU, theV, thePnt, theD1U, theD1V, theD2U, theD2V, theD2UV);
  }

  static void D3(const Handle(Adaptor3d_Surface)& theSurf,
                 const Standard_Real              theU,
                 const Standard_Real              theV,
                 gp_Pnt&                          thePnt,
                 gp_Vec&                          theD1U,
                 gp_Vec&                          theD1V,
                 gp_Vec&                          theD2U,
                 gp_Vec&                          theD2V,
                 gp_Vec&                          theD2UV,
                 gp_Vec&                          theD3U,
                 gp_Vec&                          theD3V,
                 gp_Vec&                          theD3UUV,
                 gp_Vec&                          theD3UVV)
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

  static gp_Vec DN(const Handle(Adaptor3d_Surface)& theSurf,
                   const Standard_Real              theU,
                   const Standard_Real              theV,
                   const Standard_Integer           theNU,
                   const Standard_Integer           theNV)
  {
    return theSurf->DN(theU, theV, theNU, theNV);
  }

  static Standard_Real UResolution(const Handle(Adaptor3d_Surface)& theSurf,
                                   const Standard_Real              theR3d)
  {
    return theSurf->UResolution(theR3d);
  }

  static Standard_Real VResolution(const Handle(Adaptor3d_Surface)& theSurf,
                                   const Standard_Real              theR3d)
  {
    return theSurf->VResolution(theR3d);
  }

  static GeomAbs_SurfaceType GetType(const Handle(Adaptor3d_Surface)& theSurf)
  {
    return theSurf->GetType();
  }

  static gp_Pln Plane(const Handle(Adaptor3d_Surface)& theSurf) { return theSurf->Plane(); }

  static gp_Cylinder Cylinder(const Handle(Adaptor3d_Surface)& theSurf)
  {
    return theSurf->Cylinder();
  }

  static gp_Cone Cone(const Handle(Adaptor3d_Surface)& theSurf) { return theSurf->Cone(); }

  static gp_Torus Torus(const Handle(Adaptor3d_Surface)& theSurf) { return theSurf->Torus(); }

  static gp_Sphere Sphere(const Handle(Adaptor3d_Surface)& theSurf) { return theSurf->Sphere(); }

  static Handle(Geom_BezierSurface) Bezier(const Handle(Adaptor3d_Surface)& theSurf)
  {
    return theSurf->Bezier();
  }

  static Handle(Geom_BSplineSurface) BSpline(const Handle(Adaptor3d_Surface)& theSurf)
  {
    return theSurf->BSpline();
  }

  static gp_Ax1 AxeOfRevolution(const Handle(Adaptor3d_Surface)& theSurf)
  {
    return theSurf->AxeOfRevolution();
  }

  static gp_Dir Direction(const Handle(Adaptor3d_Surface)& theSurf) { return theSurf->Direction(); }

  static Handle(Adaptor3d_Curve) BasisCurve(const Handle(Adaptor3d_Surface)& theSurf)
  {
    return theSurf->BasisCurve();
  }

  static Handle(Adaptor3d_Surface) BasisSurface(const Handle(Adaptor3d_Surface)& theSurf)
  {
    return theSurf->BasisSurface();
  }

  static Standard_Real OffsetValue(const Handle(Adaptor3d_Surface)& theSurf)
  {
    return theSurf->OffsetValue();
  }

  Standard_EXPORT static Standard_Boolean IsSurfG1(
    const Handle(Adaptor3d_Surface)& theSurf,
    const Standard_Boolean           theAlongU,
    const Standard_Real              theAngTol = Precision::Angular());

  Standard_EXPORT static Standard_Integer NbSamplesU(const Handle(Adaptor3d_Surface)& S);

  Standard_EXPORT static Standard_Integer NbSamplesV(const Handle(Adaptor3d_Surface)& S);

  Standard_EXPORT static Standard_Integer NbSamplesU(const Handle(Adaptor3d_Surface)& S,
                                                     const Standard_Real              u1,
                                                     const Standard_Real              u2);

  Standard_EXPORT static Standard_Integer NbSamplesV(const Handle(Adaptor3d_Surface)&,
                                                     const Standard_Real v1,
                                                     const Standard_Real v2);
};

#endif // _Adaptor3d_HSurfaceTool_HeaderFile
