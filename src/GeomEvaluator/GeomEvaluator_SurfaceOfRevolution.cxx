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

#include <Adaptor3d_HCurve.hxx>
#include <gp_Trsf.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomEvaluator_SurfaceOfRevolution,GeomEvaluator_Surface)

GeomEvaluator_SurfaceOfRevolution::GeomEvaluator_SurfaceOfRevolution(
        const Handle(Geom_Curve)& theBase,
        const gp_Dir& theRevolDir,
        const gp_Pnt& theRevolLoc)
  : GeomEvaluator_Surface(),
    myBaseCurve(theBase),
    myRotAxis(theRevolLoc, theRevolDir)
{
}

GeomEvaluator_SurfaceOfRevolution::GeomEvaluator_SurfaceOfRevolution(
        const Handle(Adaptor3d_HCurve)& theBase,
        const gp_Dir& theRevolDir,
        const gp_Pnt& theRevolLoc)
  : GeomEvaluator_Surface(),
    myBaseAdaptor(theBase),
    myRotAxis(theRevolLoc, theRevolDir)
{
}

void GeomEvaluator_SurfaceOfRevolution::D0(
    const Standard_Real theU, const Standard_Real theV,
    gp_Pnt& theValue) const
{
  if (!myBaseAdaptor.IsNull())
    myBaseAdaptor->D0(theV, theValue);
  else
    myBaseCurve->D0(theV, theValue);

  gp_Trsf aRotation;
  aRotation.SetRotation(myRotAxis, theU);
  theValue.Transform(aRotation);
}

void GeomEvaluator_SurfaceOfRevolution::D1(
    const Standard_Real theU, const Standard_Real theV,
    gp_Pnt& theValue, gp_Vec& theD1U, gp_Vec& theD1V) const
{
  if (!myBaseAdaptor.IsNull())
    myBaseAdaptor->D1(theV, theValue, theD1V);
  else
    myBaseCurve->D1(theV, theValue, theD1V);

  // vector from center of rotation to the point on rotated curve
  gp_XYZ aCQ = theValue.XYZ() - myRotAxis.Location().XYZ();
  theD1U = gp_Vec(myRotAxis.Direction().XYZ().Crossed(aCQ));
  // If the point is placed on the axis of revolution then derivatives on U are undefined.
  // Manually set them to zero.
  if (theD1U.SquareMagnitude() < Precision::SquareConfusion())
    theD1U.SetCoord(0.0, 0.0, 0.0);

  gp_Trsf aRotation;
  aRotation.SetRotation(myRotAxis, theU);
  theValue.Transform(aRotation);
  theD1U.Transform(aRotation);
  theD1V.Transform(aRotation);
}

void GeomEvaluator_SurfaceOfRevolution::D2(
    const Standard_Real theU, const Standard_Real theV,
    gp_Pnt& theValue, gp_Vec& theD1U, gp_Vec& theD1V,
    gp_Vec& theD2U, gp_Vec& theD2V, gp_Vec& theD2UV) const
{
  if (!myBaseAdaptor.IsNull())
    myBaseAdaptor->D2(theV, theValue, theD1V, theD2V);
  else
    myBaseCurve->D2(theV, theValue, theD1V, theD2V);

  // vector from center of rotation to the point on rotated curve
  gp_XYZ aCQ = theValue.XYZ() - myRotAxis.Location().XYZ();
  const gp_XYZ& aDir = myRotAxis.Direction().XYZ();
  theD1U = gp_Vec(aDir.Crossed(aCQ));
  // If the point is placed on the axis of revolution then derivatives on U are undefined.
  // Manually set them to zero.
  if (theD1U.SquareMagnitude() < Precision::SquareConfusion())
    theD1U.SetCoord(0.0, 0.0, 0.0);
  theD2U = gp_Vec(aDir.Dot(aCQ) * aDir - aCQ);
  theD2UV = gp_Vec(aDir.Crossed(theD1V.XYZ()));

  gp_Trsf aRotation;
  aRotation.SetRotation(myRotAxis, theU);
  theValue.Transform(aRotation);
  theD1U.Transform(aRotation);
  theD1V.Transform(aRotation);
  theD2U.Transform(aRotation);
  theD2V.Transform(aRotation);
  theD2UV.Transform(aRotation);
}

void GeomEvaluator_SurfaceOfRevolution::D3(
    const Standard_Real theU, const Standard_Real theV,
    gp_Pnt& theValue, gp_Vec& theD1U, gp_Vec& theD1V,
    gp_Vec& theD2U, gp_Vec& theD2V, gp_Vec& theD2UV,
    gp_Vec& theD3U, gp_Vec& theD3V, gp_Vec& theD3UUV, gp_Vec& theD3UVV) const
{
  if (!myBaseAdaptor.IsNull())
    myBaseAdaptor->D3(theV, theValue, theD1V, theD2V, theD3V);
  else
    myBaseCurve->D3(theV, theValue, theD1V, theD2V, theD3V);

  // vector from center of rotation to the point on rotated curve
  gp_XYZ aCQ = theValue.XYZ() - myRotAxis.Location().XYZ();
  const gp_XYZ& aDir = myRotAxis.Direction().XYZ();
  theD1U = gp_Vec(aDir.Crossed(aCQ));
  // If the point is placed on the axis of revolution then derivatives on U are undefined.
  // Manually set them to zero.
  if (theD1U.SquareMagnitude() < Precision::SquareConfusion())
    theD1U.SetCoord(0.0, 0.0, 0.0);
  theD2U = gp_Vec(aDir.Dot(aCQ) * aDir - aCQ);
  theD2UV = gp_Vec(aDir.Crossed(theD1V.XYZ()));
  theD3U = -theD1U;
  theD3UUV = gp_Vec(aDir.Dot(theD1V.XYZ()) * aDir - theD1V.XYZ());
  theD3UVV = gp_Vec(aDir.Crossed(theD2V.XYZ()));

  gp_Trsf aRotation;
  aRotation.SetRotation(myRotAxis, theU);
  theValue.Transform(aRotation);
  theD1U.Transform(aRotation);
  theD1V.Transform(aRotation);
  theD2U.Transform(aRotation);
  theD2V.Transform(aRotation);
  theD2UV.Transform(aRotation);
  theD3U.Transform(aRotation);
  theD3V.Transform(aRotation);
  theD3UUV.Transform(aRotation);
  theD3UVV.Transform(aRotation);
}

gp_Vec GeomEvaluator_SurfaceOfRevolution::DN(
    const Standard_Real theU, const Standard_Real theV,
    const Standard_Integer theDerU, const Standard_Integer theDerV) const
{
  Standard_RangeError_Raise_if(theDerU < 0, "GeomEvaluator_SurfaceOfRevolution::DN(): theDerU < 0");
  Standard_RangeError_Raise_if(theDerV < 0, "GeomEvaluator_SurfaceOfRevolution::DN(): theDerV < 0");
  Standard_RangeError_Raise_if(theDerU + theDerV < 1,
      "GeomEvaluator_SurfaceOfRevolution::DN(): theDerU + theDerV < 1");

  gp_Trsf aRotation;
  aRotation.SetRotation(myRotAxis, theU);

  gp_Pnt aP;
  gp_Vec aDV;
  gp_Vec aResult;
  if (theDerU == 0)
  {
    if (!myBaseAdaptor.IsNull())
      aResult = myBaseAdaptor->DN(theV, theDerV);
    else
      aResult = myBaseCurve->DN(theV, theDerV);
  }
  else
  {
    if (theDerV == 0)
    {
      if (!myBaseAdaptor.IsNull())
        myBaseAdaptor->D0(theV, aP);
      else
        myBaseCurve->D0(theV, aP);
      aDV = gp_Vec(aP.XYZ() - myRotAxis.Location().XYZ());
    }
    else
    {
      if (!myBaseAdaptor.IsNull())
        aDV = myBaseAdaptor->DN(theV, theDerV);
      else
        aDV = myBaseCurve->DN(theV, theDerV);
    }

    const gp_XYZ& aDir = myRotAxis.Direction().XYZ();
    if (theDerU % 4 == 1)
      aResult = gp_Vec(aDir.Crossed(aDV.XYZ()));
    else if (theDerU % 4 == 2)
      aResult = gp_Vec(aDir.Dot(aDV.XYZ()) * aDir - aDV.XYZ());
    else if (theDerU % 4 == 3)
      aResult = gp_Vec(aDir.Crossed(aDV.XYZ())) * (-1.0);
    else
      aResult = gp_Vec(aDV.XYZ() - aDir.Dot(aDV.XYZ()) * aDir);
  }

  aResult.Transform(aRotation);
  return aResult;
}

