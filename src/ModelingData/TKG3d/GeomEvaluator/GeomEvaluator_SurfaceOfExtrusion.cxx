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

#include <GeomEvaluator_SurfaceOfExtrusion.hxx>

#include <GeomAdaptor_Curve.hxx>
#include <Geom_ExtrusionUtils.pxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomEvaluator_SurfaceOfExtrusion, GeomEvaluator_Surface)

GeomEvaluator_SurfaceOfExtrusion::GeomEvaluator_SurfaceOfExtrusion(
  const Handle(Geom_Curve)& theBase,
  const gp_Dir&             theExtrusionDir)
    : GeomEvaluator_Surface(),
      myBaseCurve(theBase),
      myDirection(theExtrusionDir)
{
}

GeomEvaluator_SurfaceOfExtrusion::GeomEvaluator_SurfaceOfExtrusion(
  const Handle(Adaptor3d_Curve)& theBase,
  const gp_Dir&                  theExtrusionDir)
    : GeomEvaluator_Surface(),
      myBaseAdaptor(theBase),
      myDirection(theExtrusionDir)
{
}

//==================================================================================================

void GeomEvaluator_SurfaceOfExtrusion::D0(const Standard_Real theU,
                                          const Standard_Real theV,
                                          gp_Pnt&             theValue) const
{
  const gp_XYZ& aDir = myDirection.XYZ();

  if (!myBaseAdaptor.IsNull())
    Geom_ExtrusionUtils::D0(theU, theV, *myBaseAdaptor, aDir, theValue);
  else
    Geom_ExtrusionUtils::D0(theU, theV, *myBaseCurve, aDir, theValue);
}

//==================================================================================================

void GeomEvaluator_SurfaceOfExtrusion::D1(const Standard_Real theU,
                                          const Standard_Real theV,
                                          gp_Pnt&             theValue,
                                          gp_Vec&             theD1U,
                                          gp_Vec&             theD1V) const
{
  const gp_XYZ& aDir = myDirection.XYZ();

  if (!myBaseAdaptor.IsNull())
    Geom_ExtrusionUtils::D1(theU, theV, *myBaseAdaptor, aDir, theValue, theD1U, theD1V);
  else
    Geom_ExtrusionUtils::D1(theU, theV, *myBaseCurve, aDir, theValue, theD1U, theD1V);
}

//==================================================================================================

void GeomEvaluator_SurfaceOfExtrusion::D2(const Standard_Real theU,
                                          const Standard_Real theV,
                                          gp_Pnt&             theValue,
                                          gp_Vec&             theD1U,
                                          gp_Vec&             theD1V,
                                          gp_Vec&             theD2U,
                                          gp_Vec&             theD2V,
                                          gp_Vec&             theD2UV) const
{
  const gp_XYZ& aDir = myDirection.XYZ();

  if (!myBaseAdaptor.IsNull())
    Geom_ExtrusionUtils::D2(theU,
                            theV,
                            *myBaseAdaptor,
                            aDir,
                            theValue,
                            theD1U,
                            theD1V,
                            theD2U,
                            theD2V,
                            theD2UV);
  else
    Geom_ExtrusionUtils::D2(theU,
                            theV,
                            *myBaseCurve,
                            aDir,
                            theValue,
                            theD1U,
                            theD1V,
                            theD2U,
                            theD2V,
                            theD2UV);
}

//==================================================================================================

void GeomEvaluator_SurfaceOfExtrusion::D3(const Standard_Real theU,
                                          const Standard_Real theV,
                                          gp_Pnt&             theValue,
                                          gp_Vec&             theD1U,
                                          gp_Vec&             theD1V,
                                          gp_Vec&             theD2U,
                                          gp_Vec&             theD2V,
                                          gp_Vec&             theD2UV,
                                          gp_Vec&             theD3U,
                                          gp_Vec&             theD3V,
                                          gp_Vec&             theD3UUV,
                                          gp_Vec&             theD3UVV) const
{
  const gp_XYZ& aDir = myDirection.XYZ();

  if (!myBaseAdaptor.IsNull())
    Geom_ExtrusionUtils::D3(theU,
                            theV,
                            *myBaseAdaptor,
                            aDir,
                            theValue,
                            theD1U,
                            theD1V,
                            theD2U,
                            theD2V,
                            theD2UV,
                            theD3U,
                            theD3V,
                            theD3UUV,
                            theD3UVV);
  else
    Geom_ExtrusionUtils::D3(theU,
                            theV,
                            *myBaseCurve,
                            aDir,
                            theValue,
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

//==================================================================================================

gp_Vec GeomEvaluator_SurfaceOfExtrusion::DN(const Standard_Real theU,
                                            const Standard_Real,
                                            const Standard_Integer theDerU,
                                            const Standard_Integer theDerV) const
{
  Standard_RangeError_Raise_if(theDerU < 0, "GeomEvaluator_SurfaceOfExtrusion::DN(): theDerU < 0");
  Standard_RangeError_Raise_if(theDerV < 0, "GeomEvaluator_SurfaceOfExtrusion::DN(): theDerV < 0");
  Standard_RangeError_Raise_if(theDerU + theDerV < 1,
                               "GeomEvaluator_SurfaceOfExtrusion::DN(): theDerU + theDerV < 1");

  const gp_XYZ& aDir = myDirection.XYZ();

  if (!myBaseAdaptor.IsNull())
    return Geom_ExtrusionUtils::DN(theU, *myBaseAdaptor, aDir, theDerU, theDerV);
  else
    return Geom_ExtrusionUtils::DN(theU, *myBaseCurve, aDir, theDerU, theDerV);
}

//==================================================================================================

Handle(GeomEvaluator_Surface) GeomEvaluator_SurfaceOfExtrusion::ShallowCopy() const
{
  Handle(GeomEvaluator_SurfaceOfExtrusion) aCopy;
  if (!myBaseAdaptor.IsNull())
  {
    aCopy = new GeomEvaluator_SurfaceOfExtrusion(myBaseAdaptor->ShallowCopy(), myDirection);
  }
  else
  {
    aCopy = new GeomEvaluator_SurfaceOfExtrusion(myBaseCurve, myDirection);
  }

  return aCopy;
}
