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

#include <GeomEvaluator_OffsetCurve.hxx>

#include <GeomAdaptor_Curve.hxx>
#include <Geom_OffsetCurveUtils.pxx>
#include <Standard_NullValue.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomEvaluator_OffsetCurve, GeomEvaluator_Curve)

GeomEvaluator_OffsetCurve::GeomEvaluator_OffsetCurve(const Handle(Geom_Curve)& theBase,
                                                     const Standard_Real       theOffset,
                                                     const gp_Dir&             theDirection)
    : GeomEvaluator_Curve(),
      myBaseCurve(theBase),
      myOffset(theOffset),
      myOffsetDir(theDirection)
{
}

GeomEvaluator_OffsetCurve::GeomEvaluator_OffsetCurve(const Handle(GeomAdaptor_Curve)& theBase,
                                                     const Standard_Real              theOffset,
                                                     const gp_Dir&                    theDirection)
    : GeomEvaluator_Curve(),
      myBaseAdaptor(theBase),
      myOffset(theOffset),
      myOffsetDir(theDirection)
{
}

//==================================================================================================

void GeomEvaluator_OffsetCurve::D0(const Standard_Real theU, gp_Pnt& theValue) const
{
  bool isOK = false;
  if (!myBaseAdaptor.IsNull())
    isOK = Geom_OffsetCurveUtils::EvaluateD0(theU, myBaseAdaptor, myOffsetDir, myOffset, theValue);
  else
    isOK = Geom_OffsetCurveUtils::EvaluateD0(theU, myBaseCurve, myOffsetDir, myOffset, theValue);

  if (!isOK)
  {
    throw Standard_NullValue("GeomEvaluator_OffsetCurve: Undefined normal vector "
                             "because tangent vector has zero-magnitude!");
  }
}

//==================================================================================================

void GeomEvaluator_OffsetCurve::D1(const Standard_Real theU, gp_Pnt& theValue, gp_Vec& theD1) const
{
  bool isOK = false;
  if (!myBaseAdaptor.IsNull())
    isOK = Geom_OffsetCurveUtils::EvaluateD1(theU,
                                             myBaseAdaptor,
                                             myOffsetDir,
                                             myOffset,
                                             theValue,
                                             theD1);
  else
    isOK =
      Geom_OffsetCurveUtils::EvaluateD1(theU, myBaseCurve, myOffsetDir, myOffset, theValue, theD1);

  if (!isOK)
  {
    throw Standard_NullValue("GeomEvaluator_OffsetCurve: Null derivative");
  }
}

//==================================================================================================

void GeomEvaluator_OffsetCurve::D2(const Standard_Real theU,
                                   gp_Pnt&             theValue,
                                   gp_Vec&             theD1,
                                   gp_Vec&             theD2) const
{
  bool isOK = false;
  if (!myBaseAdaptor.IsNull())
    isOK = Geom_OffsetCurveUtils::EvaluateD2(theU,
                                             myBaseAdaptor,
                                             myOffsetDir,
                                             myOffset,
                                             theValue,
                                             theD1,
                                             theD2);
  else
    isOK = Geom_OffsetCurveUtils::EvaluateD2(theU,
                                             myBaseCurve,
                                             myOffsetDir,
                                             myOffset,
                                             theValue,
                                             theD1,
                                             theD2);

  if (!isOK)
  {
    throw Standard_NullValue("GeomEvaluator_OffsetCurve: Null derivative");
  }
}

//==================================================================================================

void GeomEvaluator_OffsetCurve::D3(const Standard_Real theU,
                                   gp_Pnt&             theValue,
                                   gp_Vec&             theD1,
                                   gp_Vec&             theD2,
                                   gp_Vec&             theD3) const
{
  bool isOK = false;
  if (!myBaseAdaptor.IsNull())
    isOK = Geom_OffsetCurveUtils::EvaluateD3(theU,
                                             myBaseAdaptor,
                                             myOffsetDir,
                                             myOffset,
                                             theValue,
                                             theD1,
                                             theD2,
                                             theD3);
  else
    isOK = Geom_OffsetCurveUtils::EvaluateD3(theU,
                                             myBaseCurve,
                                             myOffsetDir,
                                             myOffset,
                                             theValue,
                                             theD1,
                                             theD2,
                                             theD3);

  if (!isOK)
  {
    throw Standard_NullValue("GeomEvaluator_OffsetCurve: Null derivative");
  }
}

//==================================================================================================

gp_Vec GeomEvaluator_OffsetCurve::DN(const Standard_Real    theU,
                                     const Standard_Integer theDeriv) const
{
  Standard_RangeError_Raise_if(theDeriv < 1, "GeomEvaluator_OffsetCurve::DN(): theDeriv < 1");

  gp_Vec aDN;
  bool   isOK = false;

  if (theDeriv <= 3)
  {
    if (!myBaseAdaptor.IsNull())
      isOK = Geom_OffsetCurveUtils::EvaluateDN(theU,
                                               myBaseAdaptor,
                                               myOffsetDir,
                                               myOffset,
                                               theDeriv,
                                               aDN);
    else
      isOK =
        Geom_OffsetCurveUtils::EvaluateDN(theU, myBaseCurve, myOffsetDir, myOffset, theDeriv, aDN);

    if (!isOK)
    {
      throw Standard_NullValue("GeomEvaluator_OffsetCurve: Null derivative");
    }
  }
  else
  {
    // For derivatives > 3, return basis curve derivative (no offset contribution)
    if (!myBaseAdaptor.IsNull())
      aDN = myBaseAdaptor->DN(theU, theDeriv);
    else
      aDN = myBaseCurve->DN(theU, theDeriv);
  }

  return aDN;
}

//==================================================================================================

Handle(GeomEvaluator_Curve) GeomEvaluator_OffsetCurve::ShallowCopy() const
{
  Handle(GeomEvaluator_OffsetCurve) aCopy;
  if (!myBaseAdaptor.IsNull())
  {
    aCopy = new GeomEvaluator_OffsetCurve(
      Handle(GeomAdaptor_Curve)::DownCast(myBaseAdaptor->ShallowCopy()),
      myOffset,
      myOffsetDir);
  }
  else
  {
    aCopy = new GeomEvaluator_OffsetCurve(myBaseCurve, myOffset, myOffsetDir);
  }
  return aCopy;
}
