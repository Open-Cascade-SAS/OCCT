// Created on: 1991-06-25
// Created by: JCV
// Copyright (c) 1991-1999 Matra Datavision
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

//  modified by Edward AGAPOV (eap) Jan 28 2002 --- DN(), occ143(BUC60654)

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include "Geom2d_EvalRepCurveDesc.hxx"
#include "Geom2d_EvalRepUtils.pxx"
#include <Geom2d_Geometry.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_OffsetCurveUtils.pxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_UndefinedDerivative.hxx>
#include <Geom2d_UndefinedValue.hxx>
#include <Standard_NullValue.hxx>
#include <gp.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_ProgramError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2d_OffsetCurve, Geom2d_Curve)

static const double MyAngularToleranceForG1 = Precision::Angular();

//==================================================================================================

void Geom2d_OffsetCurve::SetEvalRepresentation(
  const occ::handle<Geom2d_EvalRepCurveDesc::Base>& theDesc)
{
  Geom2d_EvalRepUtils::ValidateCurveDesc(theDesc, this);
  myEvalRep = theDesc;
}

//==================================================================================================

occ::handle<Geom2d_Geometry> Geom2d_OffsetCurve::Copy() const
{
  return new Geom2d_OffsetCurve(*this);
}

//==================================================================================================
// function : Geom2d_OffsetCurve
// purpose  : Basis curve cannot be an Offset curve or trimmed from
//            offset curve.
//==================================================================================================

Geom2d_OffsetCurve::Geom2d_OffsetCurve(const occ::handle<Geom2d_Curve>& theCurve,
                                       const double                     theOffset,
                                       const bool                       isTheNotCheckC0)
    : offsetValue(theOffset)
{
  SetBasisCurve(theCurve, isTheNotCheckC0);
}

//==================================================================================================

Geom2d_OffsetCurve::Geom2d_OffsetCurve(const Geom2d_OffsetCurve& theOther)
    : basisCurve(occ::down_cast<Geom2d_Curve>(theOther.basisCurve->Copy())),
      myEvalRep(Geom2d_EvalRepUtils::CloneCurveDesc(theOther.myEvalRep)),
      offsetValue(theOther.offsetValue),
      myBasisCurveContinuity(theOther.myBasisCurveContinuity)
{
  // Deep copy basis curve without validation
}

//==================================================================================================

void Geom2d_OffsetCurve::Reverse()
{
  ClearEvalRepresentation();
  basisCurve->Reverse();
  offsetValue = -offsetValue;
}

//==================================================================================================

double Geom2d_OffsetCurve::ReversedParameter(const double U) const
{
  return basisCurve->ReversedParameter(U);
}

//==================================================================================================

void Geom2d_OffsetCurve::SetBasisCurve(const occ::handle<Geom2d_Curve>& C, const bool isNotCheckC0)
{
  ClearEvalRepresentation();
  const double              aUf = C->FirstParameter(), aUl = C->LastParameter();
  occ::handle<Geom2d_Curve> aCheckingCurve = C;
  bool                      isTrimmed      = false;

  while (aCheckingCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve))
         || aCheckingCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve)))
  {
    if (aCheckingCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
    {
      occ::handle<Geom2d_TrimmedCurve> aTrimC = occ::down_cast<Geom2d_TrimmedCurve>(aCheckingCurve);
      aCheckingCurve                          = aTrimC->BasisCurve();
      isTrimmed                               = true;
    }

    if (aCheckingCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve)))
    {
      occ::handle<Geom2d_OffsetCurve> aOC = occ::down_cast<Geom2d_OffsetCurve>(aCheckingCurve);
      aCheckingCurve                      = aOC->BasisCurve();
      offsetValue += aOC->Offset();
    }
  }

  myBasisCurveContinuity = aCheckingCurve->Continuity();

  bool isC0 = !isNotCheckC0 && (myBasisCurveContinuity == GeomAbs_C0);

  // Basis curve must be at least C1
  if (isC0 && aCheckingCurve->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
  {
    occ::handle<Geom2d_BSplineCurve> aBC = occ::down_cast<Geom2d_BSplineCurve>(aCheckingCurve);
    if (aBC->IsG1(aUf, aUl, MyAngularToleranceForG1))
    {
      // Checking if basis curve has more smooth (C1, G2 and above) is not done.
      // It can be done in case of need.
      myBasisCurveContinuity = GeomAbs_G1;
      isC0                   = false;
    }

    // Raise exception if still C0
    if (isC0)
    {
      throw Standard_ConstructionError("Offset on C0 curve");
    }
  }
  //
  if (isTrimmed)
  {
    basisCurve = new Geom2d_TrimmedCurve(aCheckingCurve, aUf, aUl);
  }
  else
  {
    basisCurve = aCheckingCurve;
  }
}

//==================================================================================================

void Geom2d_OffsetCurve::SetOffsetValue(const double D)
{
  ClearEvalRepresentation();
  offsetValue = D;
}

//==================================================================================================

occ::handle<Geom2d_Curve> Geom2d_OffsetCurve::BasisCurve() const
{
  return basisCurve;
}

//==================================================================================================

GeomAbs_Shape Geom2d_OffsetCurve::Continuity() const
{
  GeomAbs_Shape OffsetShape = GeomAbs_C0;
  switch (myBasisCurveContinuity)
  {
    case GeomAbs_C0:
      OffsetShape = GeomAbs_C0;
      break;
    case GeomAbs_C1:
      OffsetShape = GeomAbs_C0;
      break;
    case GeomAbs_C2:
      OffsetShape = GeomAbs_C1;
      break;
    case GeomAbs_C3:
      OffsetShape = GeomAbs_C2;
      break;
    case GeomAbs_CN:
      OffsetShape = GeomAbs_CN;
      break;
    case GeomAbs_G1:
      OffsetShape = GeomAbs_G1;
      break;
    case GeomAbs_G2:
      OffsetShape = GeomAbs_G2;
      break;
  }

  return OffsetShape;
}

//==================================================================================================

gp_Pnt2d Geom2d_OffsetCurve::EvalD0(const double theU) const
{
  gp_Pnt2d aEvalRepResult;
  if (Geom2d_EvalRepUtils::TryEvalCurveD0(myEvalRep, theU, aEvalRepResult))
  {
    return aEvalRepResult;
  }

  const Geom2d_Curve::ResD1 aBasisD1 = basisCurve->EvalD1(theU);
  gp_Pnt2d aValue = aBasisD1.Point;
  if (!Geom2d_OffsetCurveUtils::CalculateD0(aValue, aBasisD1.D1, offsetValue))
    throw Geom2d_UndefinedValue("Geom2d_OffsetCurve::EvalD0");
  return aValue;
}

//==================================================================================================

Geom2d_Curve::ResD1 Geom2d_OffsetCurve::EvalD1(const double theU) const
{
  Geom2d_Curve::ResD1 aEvalRepResult;
  if (Geom2d_EvalRepUtils::TryEvalCurveD1(myEvalRep, theU, aEvalRepResult))
  {
    return aEvalRepResult;
  }

  const Geom2d_Curve::ResD2 aBasisD2 = basisCurve->EvalD2(theU);
  gp_Pnt2d aValue = aBasisD2.Point;
  gp_Vec2d aD1    = aBasisD2.D1;
  if (!Geom2d_OffsetCurveUtils::CalculateD1(aValue, aD1, aBasisD2.D2, offsetValue))
    throw Geom2d_UndefinedDerivative("Geom2d_OffsetCurve::EvalD1");
  return Geom2d_Curve::ResD1{aValue, aD1};
}

//==================================================================================================

Geom2d_Curve::ResD2 Geom2d_OffsetCurve::EvalD2(const double theU) const
{
  Geom2d_Curve::ResD2 aEvalRepResult;
  if (Geom2d_EvalRepUtils::TryEvalCurveD2(myEvalRep, theU, aEvalRepResult))
  {
    return aEvalRepResult;
  }

  const Geom2d_Curve::ResD3 aBasisD3 = basisCurve->EvalD3(theU);
  gp_Pnt2d aValue = aBasisD3.Point;
  gp_Vec2d aD1 = aBasisD3.D1, aD2 = aBasisD3.D2, aD3 = aBasisD3.D3;
  bool     isDirectionChange = false;
  if (aD1.SquareMagnitude() <= gp::Resolution())
  {
    gp_Vec2d aDummyD4;
    if (!Geom2d_OffsetCurveUtils::AdjustDerivative(*basisCurve,
                                                   3,
                                                   theU,
                                                   aD1,
                                                   aD2,
                                                   aD3,
                                                   aDummyD4,
                                                   isDirectionChange))
      throw Geom2d_UndefinedDerivative("Geom2d_OffsetCurve::EvalD2");
  }
  if (!Geom2d_OffsetCurveUtils::CalculateD2(aValue, aD1, aD2, aD3, isDirectionChange, offsetValue))
    throw Geom2d_UndefinedDerivative("Geom2d_OffsetCurve::EvalD2");
  return Geom2d_Curve::ResD2{aValue, aD1, aD2};
}

//==================================================================================================

Geom2d_Curve::ResD3 Geom2d_OffsetCurve::EvalD3(const double theU) const
{
  Geom2d_Curve::ResD3 aEvalRepResult;
  if (Geom2d_EvalRepUtils::TryEvalCurveD3(myEvalRep, theU, aEvalRepResult))
  {
    return aEvalRepResult;
  }

  const Geom2d_Curve::ResD3 aBasisD3 = basisCurve->EvalD3(theU);
  const gp_Vec2d            aD4Basis = basisCurve->EvalDN(theU, 4);
  gp_Pnt2d aValue = aBasisD3.Point;
  gp_Vec2d aD1 = aBasisD3.D1, aD2 = aBasisD3.D2, aD3 = aBasisD3.D3;
  gp_Vec2d aD4               = aD4Basis;
  bool     isDirectionChange = false;
  if (aD1.SquareMagnitude() <= gp::Resolution())
  {
    if (!Geom2d_OffsetCurveUtils::AdjustDerivative(*basisCurve,
                                                   4,
                                                   theU,
                                                   aD1,
                                                   aD2,
                                                   aD3,
                                                   aD4,
                                                   isDirectionChange))
      throw Geom2d_UndefinedDerivative("Geom2d_OffsetCurve::EvalD3");
  }
  if (!Geom2d_OffsetCurveUtils::CalculateD3(aValue,
                                            aD1,
                                            aD2,
                                            aD3,
                                            aD4,
                                            isDirectionChange,
                                            offsetValue))
    throw Geom2d_UndefinedDerivative("Geom2d_OffsetCurve::EvalD3");
  return Geom2d_Curve::ResD3{aValue, aD1, aD2, aD3};
}

//==================================================================================================

gp_Vec2d Geom2d_OffsetCurve::EvalDN(const double U, const int N) const
{
  if (N < 1)
    throw Geom2d_UndefinedDerivative("Geom2d_OffsetCurve::EvalDN");
  gp_Vec2d aEvalRepResult;
  if (Geom2d_EvalRepUtils::TryEvalCurveDN(myEvalRep, U, N, aEvalRepResult))
  {
    return aEvalRepResult;
  }
  switch (N)
  {
    case 1: {
      return EvalD1(U).D1;
    }
    case 2: {
      return EvalD2(U).D2;
    }
    case 3: {
      return EvalD3(U).D3;
    }
    default:
      return basisCurve->EvalDN(U, N);
  }
}

//==================================================================================================

double Geom2d_OffsetCurve::FirstParameter() const
{
  return basisCurve->FirstParameter();
}

//==================================================================================================

double Geom2d_OffsetCurve::LastParameter() const
{
  return basisCurve->LastParameter();
}

//==================================================================================================

double Geom2d_OffsetCurve::Offset() const
{
  return offsetValue;
}

//==================================================================================================

bool Geom2d_OffsetCurve::IsClosed() const
{
  gp_Pnt2d PF, PL;
  Geom2d_Curve::D0(FirstParameter(), PF);
  Geom2d_Curve::D0(LastParameter(), PL);
  return (PF.Distance(PL) <= gp::Resolution());
}

//==================================================================================================

bool Geom2d_OffsetCurve::IsCN(const int N) const
{
  Standard_RangeError_Raise_if(N < 0, " ");
  return basisCurve->IsCN(N + 1);
}

//==================================================================================================

bool Geom2d_OffsetCurve::IsPeriodic() const
{
  return basisCurve->IsPeriodic();
}

//==================================================================================================

double Geom2d_OffsetCurve::Period() const
{
  return basisCurve->Period();
}

//==================================================================================================

void Geom2d_OffsetCurve::Transform(const gp_Trsf2d& T)
{
  ClearEvalRepresentation();
  basisCurve->Transform(T);
  offsetValue *= std::abs(T.ScaleFactor());
}

//==================================================================================================

double Geom2d_OffsetCurve::TransformedParameter(const double U, const gp_Trsf2d& T) const
{
  return basisCurve->TransformedParameter(U, T);
}

//==================================================================================================

double Geom2d_OffsetCurve::ParametricTransformation(const gp_Trsf2d& T) const
{
  return basisCurve->ParametricTransformation(T);
}

//==================================================================================================

GeomAbs_Shape Geom2d_OffsetCurve::GetBasisCurveContinuity() const
{
  return myBasisCurveContinuity;
}

//==================================================================================================

void Geom2d_OffsetCurve::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom2d_Curve)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, basisCurve.get())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, offsetValue)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myBasisCurveContinuity)
}
