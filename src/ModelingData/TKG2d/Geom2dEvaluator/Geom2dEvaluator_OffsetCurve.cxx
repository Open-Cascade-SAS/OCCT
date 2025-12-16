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

#include <Geom2dEvaluator_OffsetCurve.hxx>

#include <Geom2d_OffsetCurveUtils.pxx>
#include <Geom2d_UndefinedValue.hxx>
#include <Geom2dAdaptor_Curve.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2dEvaluator_OffsetCurve, Geom2dEvaluator_Curve)

Geom2dEvaluator_OffsetCurve::Geom2dEvaluator_OffsetCurve(const Handle(Geom2d_Curve)& theBase,
                                                         const Standard_Real         theOffset)
    : Geom2dEvaluator_Curve(),
      myBaseCurve(theBase),
      myOffset(theOffset)
{
}

Geom2dEvaluator_OffsetCurve::Geom2dEvaluator_OffsetCurve(const Handle(Geom2dAdaptor_Curve)& theBase,
                                                         const Standard_Real theOffset)
    : Geom2dEvaluator_Curve(),
      myBaseAdaptor(theBase),
      myOffset(theOffset)
{
}

//==================================================================================================

void Geom2dEvaluator_OffsetCurve::D0(const Standard_Real theU, gp_Pnt2d& theValue) const
{
  bool isOK = false;
  if (!myBaseAdaptor.IsNull())
  {
    isOK = Geom2d_OffsetCurveUtils::EvaluateD0(theU, myBaseAdaptor, myOffset, theValue);
  }
  else
  {
    isOK = Geom2d_OffsetCurveUtils::EvaluateD0(theU, myBaseCurve, myOffset, theValue);
  }

  if (!isOK)
  {
    throw Geom2d_UndefinedValue("Geom2dEvaluator_OffsetCurve::D0(): Unable to calculate normal");
  }
}

//==================================================================================================

void Geom2dEvaluator_OffsetCurve::D1(const Standard_Real theU,
                                     gp_Pnt2d&           theValue,
                                     gp_Vec2d&           theD1) const
{
  bool isOK = false;
  if (!myBaseAdaptor.IsNull())
  {
    isOK = Geom2d_OffsetCurveUtils::EvaluateD1(theU, myBaseAdaptor, myOffset, theValue, theD1);
  }
  else
  {
    isOK = Geom2d_OffsetCurveUtils::EvaluateD1(theU, myBaseCurve, myOffset, theValue, theD1);
  }

  if (!isOK)
  {
    throw Geom2d_UndefinedValue("Geom2dEvaluator_OffsetCurve::D1(): Unable to calculate normal");
  }
}

//==================================================================================================

void Geom2dEvaluator_OffsetCurve::D2(const Standard_Real theU,
                                     gp_Pnt2d&           theValue,
                                     gp_Vec2d&           theD1,
                                     gp_Vec2d&           theD2) const
{
  bool isOK = false;
  if (!myBaseAdaptor.IsNull())
  {
    isOK =
      Geom2d_OffsetCurveUtils::EvaluateD2(theU, myBaseAdaptor, myOffset, theValue, theD1, theD2);
  }
  else
  {
    isOK = Geom2d_OffsetCurveUtils::EvaluateD2(theU, myBaseCurve, myOffset, theValue, theD1, theD2);
  }

  if (!isOK)
  {
    throw Geom2d_UndefinedValue("Geom2dEvaluator_OffsetCurve::D2(): Unable to calculate normal");
  }
}

//==================================================================================================

void Geom2dEvaluator_OffsetCurve::D3(const Standard_Real theU,
                                     gp_Pnt2d&           theValue,
                                     gp_Vec2d&           theD1,
                                     gp_Vec2d&           theD2,
                                     gp_Vec2d&           theD3) const
{
  bool isOK = false;
  if (!myBaseAdaptor.IsNull())
  {
    isOK = Geom2d_OffsetCurveUtils::EvaluateD3(theU,
                                               myBaseAdaptor,
                                               myOffset,
                                               theValue,
                                               theD1,
                                               theD2,
                                               theD3);
  }
  else
  {
    isOK = Geom2d_OffsetCurveUtils::EvaluateD3(theU,
                                               myBaseCurve,
                                               myOffset,
                                               theValue,
                                               theD1,
                                               theD2,
                                               theD3);
  }

  if (!isOK)
  {
    throw Geom2d_UndefinedValue("Geom2dEvaluator_OffsetCurve::D3(): Unable to calculate normal");
  }
}

//==================================================================================================

gp_Vec2d Geom2dEvaluator_OffsetCurve::DN(const Standard_Real    theU,
                                         const Standard_Integer theDeriv) const
{
  Standard_RangeError_Raise_if(theDeriv < 1, "Geom2dEvaluator_OffsetCurve::DN(): theDeriv < 1");

  gp_Vec2d aResult;
  bool     isOK = false;
  if (!myBaseAdaptor.IsNull())
  {
    isOK = Geom2d_OffsetCurveUtils::EvaluateDN(theU, myBaseAdaptor, myOffset, theDeriv, aResult);
  }
  else
  {
    isOK = Geom2d_OffsetCurveUtils::EvaluateDN(theU, myBaseCurve, myOffset, theDeriv, aResult);
  }

  if (!isOK)
  {
    throw Geom2d_UndefinedValue("Geom2dEvaluator_OffsetCurve::DN(): Unable to calculate normal");
  }

  return aResult;
}

//==================================================================================================

Handle(Geom2dEvaluator_Curve) Geom2dEvaluator_OffsetCurve::ShallowCopy() const
{
  Handle(Geom2dEvaluator_OffsetCurve) aCopy;
  if (!myBaseAdaptor.IsNull())
  {
    aCopy = new Geom2dEvaluator_OffsetCurve(
      Handle(Geom2dAdaptor_Curve)::DownCast(myBaseAdaptor->ShallowCopy()),
      myOffset);
  }
  else
  {
    aCopy = new Geom2dEvaluator_OffsetCurve(myBaseCurve, myOffset);
  }

  return aCopy;
}
