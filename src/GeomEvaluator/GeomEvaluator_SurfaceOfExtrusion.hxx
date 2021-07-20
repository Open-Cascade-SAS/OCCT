// Created on: 2015-09-21
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _GeomEvaluator_SurfaceOfExtrusion_HeaderFile
#define _GeomEvaluator_SurfaceOfExtrusion_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <GeomEvaluator_Surface.hxx>
#include <Geom_Curve.hxx>
#include <gp_Dir.hxx>

//! Allows to calculate values and derivatives for surfaces of linear extrusion
class GeomEvaluator_SurfaceOfExtrusion : public GeomEvaluator_Surface
{
public:
  //! Initialize evaluator by surface
  Standard_EXPORT GeomEvaluator_SurfaceOfExtrusion(const Handle(Geom_Curve)& theBase,
                                                   const gp_Dir& theExtrusionDir);
  //! Initialize evaluator by surface adaptor
  Standard_EXPORT GeomEvaluator_SurfaceOfExtrusion(const Handle(Adaptor3d_Curve)& theBase,
                                                   const gp_Dir& theExtrusionDir);

  ///! Changes the direction of extrusion
  void SetDirection(const gp_Dir& theDirection)
  { myDirection = theDirection; }

  //! Value of surface
  Standard_EXPORT void D0(const Standard_Real theU, const Standard_Real theV,
                          gp_Pnt& theValue) const Standard_OVERRIDE;
  //! Value and first derivatives of surface
  Standard_EXPORT void D1(const Standard_Real theU, const Standard_Real theV,
                          gp_Pnt& theValue, gp_Vec& theD1U, gp_Vec& theD1V) const Standard_OVERRIDE;
  //! Value, first and second derivatives of surface
  Standard_EXPORT void D2(const Standard_Real theU, const Standard_Real theV,
                          gp_Pnt& theValue, gp_Vec& theD1U, gp_Vec& theD1V,
                          gp_Vec& theD2U, gp_Vec& theD2V, gp_Vec& theD2UV) const Standard_OVERRIDE;
  //! Value, first, second and third derivatives of surface
  Standard_EXPORT void D3(const Standard_Real theU, const Standard_Real theV,
                          gp_Pnt& theValue, gp_Vec& theD1U, gp_Vec& theD1V,
                          gp_Vec& theD2U, gp_Vec& theD2V, gp_Vec& theD2UV,
                          gp_Vec& theD3U, gp_Vec& theD3V,
                          gp_Vec& theD3UUV, gp_Vec& theD3UVV) const Standard_OVERRIDE;
  //! Calculates N-th derivatives of surface, where N = theDerU + theDerV.
  //!
  //! Raises if N < 1 or theDerU < 0 or theDerV < 0
  Standard_EXPORT gp_Vec DN(const Standard_Real theU,
                            const Standard_Real theV,
                            const Standard_Integer theDerU,
                            const Standard_Integer theDerV) const Standard_OVERRIDE;

  Standard_EXPORT Handle(GeomEvaluator_Surface) ShallowCopy() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(GeomEvaluator_SurfaceOfExtrusion,GeomEvaluator_Surface)

private:
  //! Shift the point along direction to the given distance (theShift)
  void Shift(const Standard_Real theShift, gp_Pnt& thePoint) const
  {
    thePoint.ChangeCoord() += myDirection.XYZ() * theShift;
  }

private:
  Handle(Geom_Curve)       myBaseCurve;
  Handle(Adaptor3d_Curve) myBaseAdaptor;

  gp_Dir myDirection;
};

DEFINE_STANDARD_HANDLE(GeomEvaluator_SurfaceOfExtrusion, GeomEvaluator_Surface)


#endif // _GeomEvaluator_SurfaceOfExtrusion_HeaderFile
