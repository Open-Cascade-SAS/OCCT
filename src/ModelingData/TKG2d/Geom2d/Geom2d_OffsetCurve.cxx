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
#include <Geom2d_Geometry.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_OffsetUtils.pxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_UndefinedDerivative.hxx>
#include <gp.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2d_OffsetCurve, Geom2d_Curve)

static const double MyAngularToleranceForG1 = Precision::Angular();

//==================================================================================================

Handle(Geom2d_Geometry) Geom2d_OffsetCurve::Copy() const
{
  return new Geom2d_OffsetCurve(*this);
}

//==================================================================================================
// function : Geom2d_OffsetCurve
// purpose  : Basis curve cannot be an Offset curve or trimmed from
//            offset curve.
//==================================================================================================

Geom2d_OffsetCurve::Geom2d_OffsetCurve(const Handle(Geom2d_Curve)& theCurve,
                                       const Standard_Real         theOffset,
                                       const Standard_Boolean      isTheNotCheckC0)
    : offsetValue(theOffset)
{
  SetBasisCurve(theCurve, isTheNotCheckC0);
}

//==================================================================================================

Geom2d_OffsetCurve::Geom2d_OffsetCurve(const Geom2d_OffsetCurve& theOther)
    : offsetValue(theOther.offsetValue),
      myBasisCurveContinuity(theOther.myBasisCurveContinuity)
{
  // Deep copy basis curve without validation
  basisCurve = Handle(Geom2d_Curve)::DownCast(theOther.basisCurve->Copy());
}

//==================================================================================================

void Geom2d_OffsetCurve::Reverse()
{
  basisCurve->Reverse();
  offsetValue = -offsetValue;
}

//==================================================================================================

Standard_Real Geom2d_OffsetCurve::ReversedParameter(const Standard_Real U) const
{
  return basisCurve->ReversedParameter(U);
}

//==================================================================================================

void Geom2d_OffsetCurve::SetBasisCurve(const Handle(Geom2d_Curve)& C,
                                       const Standard_Boolean      isNotCheckC0)
{
  const double         aUf = C->FirstParameter(), aUl = C->LastParameter();
  Handle(Geom2d_Curve) aCheckingCurve = C;
  bool                 isTrimmed      = false;

  while (aCheckingCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve))
         || aCheckingCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve)))
  {
    if (aCheckingCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
    {
      Handle(Geom2d_TrimmedCurve) aTrimC = Handle(Geom2d_TrimmedCurve)::DownCast(aCheckingCurve);
      aCheckingCurve                     = aTrimC->BasisCurve();
      isTrimmed                          = true;
    }

    if (aCheckingCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve)))
    {
      Handle(Geom2d_OffsetCurve) aOC = Handle(Geom2d_OffsetCurve)::DownCast(aCheckingCurve);
      aCheckingCurve                 = aOC->BasisCurve();
      offsetValue += aOC->Offset();
    }
  }

  myBasisCurveContinuity = aCheckingCurve->Continuity();

  bool isC0 = !isNotCheckC0 && (myBasisCurveContinuity == GeomAbs_C0);

  // Basis curve must be at least C1
  if (isC0 && aCheckingCurve->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
  {
    Handle(Geom2d_BSplineCurve) aBC = Handle(Geom2d_BSplineCurve)::DownCast(aCheckingCurve);
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

void Geom2d_OffsetCurve::SetOffsetValue(const Standard_Real D)
{
  offsetValue = D;
}

//==================================================================================================

Handle(Geom2d_Curve) Geom2d_OffsetCurve::BasisCurve() const
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

void Geom2d_OffsetCurve::D0(const Standard_Real theU, gp_Pnt2d& theP) const
{
  gp_Vec2d aD1;
  basisCurve->D1(theU, theP, aD1);
  Geom2d_OffsetUtils::CalculateD0(theP, aD1, offsetValue);
}

//==================================================================================================

void Geom2d_OffsetCurve::D1(const Standard_Real theU, gp_Pnt2d& theP, gp_Vec2d& theV1) const
{
  gp_Vec2d aD2;
  basisCurve->D2(theU, theP, theV1, aD2);
  Geom2d_OffsetUtils::CalculateD1(theP, theV1, aD2, offsetValue);
}

//==================================================================================================

void Geom2d_OffsetCurve::D2(const Standard_Real theU,
                            gp_Pnt2d&           theP,
                            gp_Vec2d&           theV1,
                            gp_Vec2d&           theV2) const
{
  gp_Vec2d aD3;
  basisCurve->D3(theU, theP, theV1, theV2, aD3);

  bool isDirectionChange = false;
  if (theV1.SquareMagnitude() <= gp::Resolution())
  {
    gp_Vec2d aDummyD4;
    isDirectionChange = adjustDerivative(3, theU, theV1, theV2, aD3, aDummyD4);
  }

  Geom2d_OffsetUtils::CalculateD2(theP, theV1, theV2, aD3, isDirectionChange, offsetValue);
}

//==================================================================================================

void Geom2d_OffsetCurve::D3(const Standard_Real theU,
                            gp_Pnt2d&           theP,
                            gp_Vec2d&           theV1,
                            gp_Vec2d&           theV2,
                            gp_Vec2d&           theV3) const
{
  basisCurve->D3(theU, theP, theV1, theV2, theV3);
  gp_Vec2d aD4 = basisCurve->DN(theU, 4);

  bool isDirectionChange = false;
  if (theV1.SquareMagnitude() <= gp::Resolution())
  {
    isDirectionChange = adjustDerivative(4, theU, theV1, theV2, theV3, aD4);
  }

  Geom2d_OffsetUtils::CalculateD3(theP, theV1, theV2, theV3, aD4, isDirectionChange, offsetValue);
}

//==================================================================================================

gp_Vec2d Geom2d_OffsetCurve::DN(const Standard_Real U, const Standard_Integer N) const
{
  Standard_RangeError_Raise_if(N < 1, "Exception: Geom2d_OffsetCurve::DN(). N<1.");

  gp_Vec2d VN, VBidon;
  gp_Pnt2d PBidon;
  switch (N)
  {
    case 1:
      D1(U, PBidon, VN);
      break;
    case 2:
      D2(U, PBidon, VBidon, VN);
      break;
    case 3:
      D3(U, PBidon, VBidon, VBidon, VN);
      break;
    default:
      throw Standard_NotImplemented("Exception: Derivative order is greater than 3. "
                                    "Cannot compute of derivative.");
  }

  return VN;
}

//==================================================================================================

Standard_Real Geom2d_OffsetCurve::FirstParameter() const
{
  return basisCurve->FirstParameter();
}

//==================================================================================================

Standard_Real Geom2d_OffsetCurve::LastParameter() const
{
  return basisCurve->LastParameter();
}

//==================================================================================================

Standard_Real Geom2d_OffsetCurve::Offset() const
{
  return offsetValue;
}

//==================================================================================================

Standard_Boolean Geom2d_OffsetCurve::IsClosed() const
{
  gp_Pnt2d PF, PL;
  D0(FirstParameter(), PF);
  D0(LastParameter(), PL);
  return (PF.Distance(PL) <= gp::Resolution());
}

//==================================================================================================

Standard_Boolean Geom2d_OffsetCurve::IsCN(const Standard_Integer N) const
{
  Standard_RangeError_Raise_if(N < 0, " ");
  return basisCurve->IsCN(N + 1);
}

//==================================================================================================

Standard_Boolean Geom2d_OffsetCurve::IsPeriodic() const
{
  return basisCurve->IsPeriodic();
}

//==================================================================================================

Standard_Real Geom2d_OffsetCurve::Period() const
{
  return basisCurve->Period();
}

//==================================================================================================

void Geom2d_OffsetCurve::Transform(const gp_Trsf2d& T)
{
  basisCurve->Transform(T);
  offsetValue *= std::abs(T.ScaleFactor());
}

//==================================================================================================

Standard_Real Geom2d_OffsetCurve::TransformedParameter(const Standard_Real U,
                                                       const gp_Trsf2d&    T) const
{
  return basisCurve->TransformedParameter(U, T);
}

//==================================================================================================

Standard_Real Geom2d_OffsetCurve::ParametricTransformation(const gp_Trsf2d& T) const
{
  return basisCurve->ParametricTransformation(T);
}

//==================================================================================================

GeomAbs_Shape Geom2d_OffsetCurve::GetBasisCurveContinuity() const
{
  return myBasisCurveContinuity;
}

//==================================================================================================

bool Geom2d_OffsetCurve::adjustDerivative(int       theMaxDerivative,
                                          double    theU,
                                          gp_Vec2d& theD1,
                                          gp_Vec2d& theD2,
                                          gp_Vec2d& theD3,
                                          gp_Vec2d& theD4) const
{
  static const double aTol           = gp::Resolution();
  static const double aMinStep       = 1e-7;
  static const int    aMaxDerivOrder = 3;

  bool         isDirectionChange = false;
  const double anUinfium         = basisCurve->FirstParameter();
  const double anUsupremum       = basisCurve->LastParameter();

  static const double DivisionFactor = 1.e-3;
  double              du;
  if ((anUsupremum >= RealLast()) || (anUinfium <= RealFirst()))
  {
    du = 0.0;
  }
  else
  {
    du = anUsupremum - anUinfium;
  }

  const double aDelta = std::max(du * DivisionFactor, aMinStep);

  // Derivative is approximated by Taylor-series
  int      anIndex = 1; // Derivative order
  gp_Vec2d V;

  do
  {
    V = basisCurve->DN(theU, ++anIndex);
  } while ((V.SquareMagnitude() <= aTol) && anIndex < aMaxDerivOrder);

  double u;

  if (theU - anUinfium < aDelta)
  {
    u = theU + aDelta;
  }
  else
  {
    u = theU - aDelta;
  }

  gp_Pnt2d P1, P2;
  basisCurve->D0(std::min(theU, u), P1);
  basisCurve->D0(std::max(theU, u), P2);

  gp_Vec2d      V1(P1, P2);
  isDirectionChange     = V.Dot(V1) < 0.0;
  const double  aSign   = isDirectionChange ? -1.0 : 1.0;

  theD1                 = V * aSign;
  gp_Vec2d* aDeriv[3]   = {&theD2, &theD3, &theD4};
  for (int i = 1; i < theMaxDerivative; i++)
  {
    *(aDeriv[i - 1]) = basisCurve->DN(theU, anIndex + i) * aSign;
  }

  return isDirectionChange;
}

//==================================================================================================

void Geom2d_OffsetCurve::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom2d_Curve)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, basisCurve.get())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, offsetValue)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myBasisCurveContinuity)
}
