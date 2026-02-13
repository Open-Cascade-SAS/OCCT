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

// 24-Aug-95 : xab removed C1 and C2 test : appeller  D1 et D2
//             avec discernement !
// 19-09-97  : JPI correction derivee seconde

#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_OffsetCurveUtils.pxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <gp.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_OffsetCurve, Geom_Curve)

static const double MyAngularToleranceForG1 = Precision::Angular();

//==================================================================================================

occ::handle<Geom_Geometry> Geom_OffsetCurve::Copy() const
{
  return new Geom_OffsetCurve(*this);
}

//==================================================================================================
// function : Geom_OffsetCurve
// purpose  : Basis curve cannot be an Offset curve or trimmed from
//            offset curve.
//==================================================================================================

Geom_OffsetCurve::Geom_OffsetCurve(const Geom_OffsetCurve& theOther)
    : basisCurve(occ::down_cast<Geom_Curve>(theOther.basisCurve->Copy())),
      direction(theOther.direction),
      offsetValue(theOther.offsetValue),
      myBasisCurveContinuity(theOther.myBasisCurveContinuity)
{
  // Deep copy without validation - source curve is already validated
}

//==================================================================================================

Geom_OffsetCurve::Geom_OffsetCurve(const occ::handle<Geom_Curve>& theCurve,
                                   const double                   theOffset,
                                   const gp_Dir&                  theDir,
                                   const bool                     isTheNotCheckC0)
    : direction(theDir),
      offsetValue(theOffset)
{
  SetBasisCurve(theCurve, isTheNotCheckC0);
}

//==================================================================================================

void Geom_OffsetCurve::Reverse()
{
  basisCurve->Reverse();
  offsetValue = -offsetValue;
}

//==================================================================================================

double Geom_OffsetCurve::ReversedParameter(const double U) const
{
  return basisCurve->ReversedParameter(U);
}

//==================================================================================================

const gp_Dir& Geom_OffsetCurve::Direction() const
{
  return direction;
}

//==================================================================================================

void Geom_OffsetCurve::SetDirection(const gp_Dir& V)
{
  direction = V;
}

//==================================================================================================

void Geom_OffsetCurve::SetOffsetValue(const double D)
{
  offsetValue = D;
}

//==================================================================================================

bool Geom_OffsetCurve::IsPeriodic() const
{
  return basisCurve->IsPeriodic();
}

//==================================================================================================

double Geom_OffsetCurve::Period() const
{
  return basisCurve->Period();
}

//==================================================================================================

void Geom_OffsetCurve::SetBasisCurve(const occ::handle<Geom_Curve>& C, const bool isNotCheckC0)
{
  const double            aUf = C->FirstParameter(), aUl = C->LastParameter();
  occ::handle<Geom_Curve> aCheckingCurve = occ::down_cast<Geom_Curve>(C->Copy());
  bool                    isTrimmed      = false;

  while (aCheckingCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))
         || aCheckingCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve)))
  {
    if (aCheckingCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
    {
      occ::handle<Geom_TrimmedCurve> aTrimC = occ::down_cast<Geom_TrimmedCurve>(aCheckingCurve);
      aCheckingCurve                        = aTrimC->BasisCurve();
      isTrimmed                             = true;
    }

    if (aCheckingCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve)))
    {
      occ::handle<Geom_OffsetCurve> aOC = occ::down_cast<Geom_OffsetCurve>(aCheckingCurve);
      aCheckingCurve                    = aOC->BasisCurve();
      double PrevOff                    = aOC->Offset();
      gp_Vec V1(aOC->Direction());
      gp_Vec V2(direction);
      gp_Vec Vdir(PrevOff * V1 + offsetValue * V2);

      if (offsetValue >= 0.)
      {
        offsetValue = Vdir.Magnitude();
        direction.SetXYZ(Vdir.XYZ());
      }
      else
      {
        offsetValue = -Vdir.Magnitude();
        direction.SetXYZ((-Vdir).XYZ());
      }
    }
  }

  myBasisCurveContinuity = aCheckingCurve->Continuity();

  bool isC0 = !isNotCheckC0 && (myBasisCurveContinuity == GeomAbs_C0);

  // Basis curve must be at least C1
  if (isC0 && aCheckingCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
  {
    occ::handle<Geom_BSplineCurve> aBC = occ::down_cast<Geom_BSplineCurve>(aCheckingCurve);
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
    basisCurve = new Geom_TrimmedCurve(aCheckingCurve, aUf, aUl);
  }
  else
  {
    basisCurve = aCheckingCurve;
  }
}

//==================================================================================================

occ::handle<Geom_Curve> Geom_OffsetCurve::BasisCurve() const
{
  return basisCurve;
}

//==================================================================================================

GeomAbs_Shape Geom_OffsetCurve::Continuity() const
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

std::optional<gp_Pnt> Geom_OffsetCurve::EvalD0(const double theU) const
{
  std::optional<Geom_Curve::ResD1> aBasisD1 = basisCurve->EvalD1(theU);
  if (!aBasisD1)
    return std::nullopt;
  gp_Pnt aValue = aBasisD1->Point;
  if (!Geom_OffsetCurveUtils::CalculateD0(aValue, aBasisD1->D1, direction.XYZ(), offsetValue))
    return std::nullopt;
  return aValue;
}

//==================================================================================================

std::optional<Geom_Curve::ResD1> Geom_OffsetCurve::EvalD1(const double theU) const
{
  std::optional<Geom_Curve::ResD2> aBasisD2 = basisCurve->EvalD2(theU);
  if (!aBasisD2)
    return std::nullopt;
  gp_Pnt aValue = aBasisD2->Point;
  gp_Vec aD1    = aBasisD2->D1;
  if (!Geom_OffsetCurveUtils::CalculateD1(aValue, aD1, aBasisD2->D2, direction.XYZ(), offsetValue))
    return std::nullopt;
  return Geom_Curve::ResD1{aValue, aD1};
}

//==================================================================================================

std::optional<Geom_Curve::ResD2> Geom_OffsetCurve::EvalD2(const double theU) const
{
  std::optional<Geom_Curve::ResD3> aBasisD3 = basisCurve->EvalD3(theU);
  if (!aBasisD3)
    return std::nullopt;
  gp_Pnt aValue = aBasisD3->Point;
  gp_Vec aD1 = aBasisD3->D1, aD2 = aBasisD3->D2, aD3 = aBasisD3->D3;
  bool   isDirectionChange = false;
  if (aD1.SquareMagnitude() <= gp::Resolution())
  {
    gp_Vec aDummyD4;
    if (!Geom_OffsetCurveUtils::AdjustDerivative(*basisCurve,
                                                 3,
                                                 theU,
                                                 aD1,
                                                 aD2,
                                                 aD3,
                                                 aDummyD4,
                                                 isDirectionChange))
      return std::nullopt;
  }
  if (!Geom_OffsetCurveUtils::CalculateD2(aValue,
                                          aD1,
                                          aD2,
                                          aD3,
                                          direction.XYZ(),
                                          offsetValue,
                                          isDirectionChange))
    return std::nullopt;
  return Geom_Curve::ResD2{aValue, aD1, aD2};
}

//==================================================================================================

std::optional<Geom_Curve::ResD3> Geom_OffsetCurve::EvalD3(const double theU) const
{
  std::optional<Geom_Curve::ResD3> aBasisD3 = basisCurve->EvalD3(theU);
  if (!aBasisD3)
    return std::nullopt;
  std::optional<gp_Vec> aD4Opt = basisCurve->EvalDN(theU, 4);
  if (!aD4Opt)
    return std::nullopt;
  gp_Pnt aValue = aBasisD3->Point;
  gp_Vec aD1 = aBasisD3->D1, aD2 = aBasisD3->D2, aD3 = aBasisD3->D3;
  gp_Vec aD4               = *aD4Opt;
  bool   isDirectionChange = false;
  if (aD1.SquareMagnitude() <= gp::Resolution())
  {
    if (!Geom_OffsetCurveUtils::AdjustDerivative(*basisCurve,
                                                 4,
                                                 theU,
                                                 aD1,
                                                 aD2,
                                                 aD3,
                                                 aD4,
                                                 isDirectionChange))
      return std::nullopt;
  }
  if (!Geom_OffsetCurveUtils::CalculateD3(aValue,
                                          aD1,
                                          aD2,
                                          aD3,
                                          aD4,
                                          direction.XYZ(),
                                          offsetValue,
                                          isDirectionChange))
    return std::nullopt;
  return Geom_Curve::ResD3{aValue, aD1, aD2, aD3};
}

//==================================================================================================

std::optional<gp_Vec> Geom_OffsetCurve::EvalDN(const double U, const int N) const
{
  if (N < 1)
    return std::nullopt;
  switch (N)
  {
    case 1: {
      std::optional<Geom_Curve::ResD1> aR = EvalD1(U);
      return aR ? std::optional<gp_Vec>(aR->D1) : std::nullopt;
    }
    case 2: {
      std::optional<Geom_Curve::ResD2> aR = EvalD2(U);
      return aR ? std::optional<gp_Vec>(aR->D2) : std::nullopt;
    }
    case 3: {
      std::optional<Geom_Curve::ResD3> aR = EvalD3(U);
      return aR ? std::optional<gp_Vec>(aR->D3) : std::nullopt;
    }
    default:
      return basisCurve->EvalDN(U, N);
  }
}

//==================================================================================================

double Geom_OffsetCurve::FirstParameter() const
{
  return basisCurve->FirstParameter();
}

//==================================================================================================

double Geom_OffsetCurve::LastParameter() const
{
  return basisCurve->LastParameter();
}

//==================================================================================================

double Geom_OffsetCurve::Offset() const
{
  return offsetValue;
}

//==================================================================================================

bool Geom_OffsetCurve::IsClosed() const
{
  gp_Pnt PF, PL;
  Geom_Curve::D0(FirstParameter(), PF);
  Geom_Curve::D0(LastParameter(), PL);
  return (PF.Distance(PL) <= gp::Resolution());
}

//==================================================================================================

bool Geom_OffsetCurve::IsCN(const int N) const
{
  Standard_RangeError_Raise_if(N < 0, " ");
  return basisCurve->IsCN(N + 1);
}

//==================================================================================================

void Geom_OffsetCurve::Transform(const gp_Trsf& T)
{
  basisCurve->Transform(T);
  direction.Transform(T);
  offsetValue *= T.ScaleFactor();
}

//==================================================================================================

double Geom_OffsetCurve::TransformedParameter(const double U, const gp_Trsf& T) const
{
  return basisCurve->TransformedParameter(U, T);
}

//==================================================================================================

double Geom_OffsetCurve::ParametricTransformation(const gp_Trsf& T) const
{
  return basisCurve->ParametricTransformation(T);
}

//==================================================================================================

GeomAbs_Shape Geom_OffsetCurve::GetBasisCurveContinuity() const
{
  return myBasisCurveContinuity;
}

//==================================================================================================

void Geom_OffsetCurve::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_Curve)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, basisCurve.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &direction)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, offsetValue)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myBasisCurveContinuity)
}
