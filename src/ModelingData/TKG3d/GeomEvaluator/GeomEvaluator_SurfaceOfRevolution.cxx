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

#include <GeomEvaluator_SurfaceOfRevolution.hxx>

#include <Adaptor3d_Curve.hxx>
#include <Geom_RevolutionUtils.pxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomEvaluator_SurfaceOfRevolution, GeomEvaluator_Surface)

GeomEvaluator_SurfaceOfRevolution::GeomEvaluator_SurfaceOfRevolution(
  const Handle(Geom_Curve)& theBase,
  const gp_Dir&             theRevolDir,
  const gp_Pnt&             theRevolLoc)
    : GeomEvaluator_Surface(),
      myBaseCurve(theBase),
      myRotAxis(theRevolLoc, theRevolDir)
{
}

GeomEvaluator_SurfaceOfRevolution::GeomEvaluator_SurfaceOfRevolution(
  const Handle(Adaptor3d_Curve)& theBase,
  const gp_Dir&                  theRevolDir,
  const gp_Pnt&                  theRevolLoc)
    : GeomEvaluator_Surface(),
      myBaseAdaptor(theBase),
      myRotAxis(theRevolLoc, theRevolDir)
{
}

//==================================================================================================

void GeomEvaluator_SurfaceOfRevolution::D0(const Standard_Real theU,
                                           const Standard_Real theV,
                                           gp_Pnt&             theValue) const
{
  const gp_XYZ& aLoc = myRotAxis.Location().XYZ();
  const gp_XYZ& aDir = myRotAxis.Direction().XYZ();

  if (!myBaseAdaptor.IsNull())
    Geom_RevolutionUtils::D0(theU, theV, *myBaseAdaptor, aLoc, aDir, theValue);
  else
    Geom_RevolutionUtils::D0(theU, theV, *myBaseCurve, aLoc, aDir, theValue);
}

//==================================================================================================

void GeomEvaluator_SurfaceOfRevolution::D1(const Standard_Real theU,
                                           const Standard_Real theV,
                                           gp_Pnt&             theValue,
                                           gp_Vec&             theD1U,
                                           gp_Vec&             theD1V) const
{
  const gp_XYZ& aLoc = myRotAxis.Location().XYZ();
  const gp_XYZ& aDir = myRotAxis.Direction().XYZ();

  if (!myBaseAdaptor.IsNull())
    Geom_RevolutionUtils::D1(theU, theV, *myBaseAdaptor, aLoc, aDir, theValue, theD1U, theD1V);
  else
    Geom_RevolutionUtils::D1(theU, theV, *myBaseCurve, aLoc, aDir, theValue, theD1U, theD1V);
}

//==================================================================================================

void GeomEvaluator_SurfaceOfRevolution::D2(const Standard_Real theU,
                                           const Standard_Real theV,
                                           gp_Pnt&             theValue,
                                           gp_Vec&             theD1U,
                                           gp_Vec&             theD1V,
                                           gp_Vec&             theD2U,
                                           gp_Vec&             theD2V,
                                           gp_Vec&             theD2UV) const
{
  const gp_XYZ& aLoc = myRotAxis.Location().XYZ();
  const gp_XYZ& aDir = myRotAxis.Direction().XYZ();

  if (!myBaseAdaptor.IsNull())
    Geom_RevolutionUtils::D2(theU,
                             theV,
                             *myBaseAdaptor,
                             aLoc,
                             aDir,
                             theValue,
                             theD1U,
                             theD1V,
                             theD2U,
                             theD2V,
                             theD2UV);
  else
    Geom_RevolutionUtils::D2(theU,
                             theV,
                             *myBaseCurve,
                             aLoc,
                             aDir,
                             theValue,
                             theD1U,
                             theD1V,
                             theD2U,
                             theD2V,
                             theD2UV);
}

//==================================================================================================

void GeomEvaluator_SurfaceOfRevolution::D3(const Standard_Real theU,
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
  const gp_XYZ& aLoc = myRotAxis.Location().XYZ();
  const gp_XYZ& aDir = myRotAxis.Direction().XYZ();

  if (!myBaseAdaptor.IsNull())
    Geom_RevolutionUtils::D3(theU,
                             theV,
                             *myBaseAdaptor,
                             aLoc,
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
    Geom_RevolutionUtils::D3(theU,
                             theV,
                             *myBaseCurve,
                             aLoc,
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

gp_Vec GeomEvaluator_SurfaceOfRevolution::DN(const Standard_Real    theU,
                                             const Standard_Real    theV,
                                             const Standard_Integer theDerU,
                                             const Standard_Integer theDerV) const
{
  Standard_RangeError_Raise_if(theDerU < 0, "GeomEvaluator_SurfaceOfRevolution::DN(): theDerU < 0");
  Standard_RangeError_Raise_if(theDerV < 0, "GeomEvaluator_SurfaceOfRevolution::DN(): theDerV < 0");
  Standard_RangeError_Raise_if(theDerU + theDerV < 1,
                               "GeomEvaluator_SurfaceOfRevolution::DN(): theDerU + theDerV < 1");

  const gp_XYZ& aLoc = myRotAxis.Location().XYZ();
  const gp_XYZ& aDir = myRotAxis.Direction().XYZ();

  if (!myBaseAdaptor.IsNull())
    return Geom_RevolutionUtils::DN(theU, theV, *myBaseAdaptor, aLoc, aDir, theDerU, theDerV);
  else
    return Geom_RevolutionUtils::DN(theU, theV, *myBaseCurve, aLoc, aDir, theDerU, theDerV);
}

//==================================================================================================

Handle(GeomEvaluator_Surface) GeomEvaluator_SurfaceOfRevolution::ShallowCopy() const
{
  Handle(GeomEvaluator_SurfaceOfRevolution) aCopy;
  if (!myBaseAdaptor.IsNull())
  {
    aCopy = new GeomEvaluator_SurfaceOfRevolution(myBaseAdaptor->ShallowCopy(),
                                                  myRotAxis.Direction(),
                                                  myRotAxis.Location());
  }
  else
  {
    aCopy = new GeomEvaluator_SurfaceOfRevolution(myBaseCurve,
                                                  myRotAxis.Direction(),
                                                  myRotAxis.Location());
  }

  return aCopy;
}
