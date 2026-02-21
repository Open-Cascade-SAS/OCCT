// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <GeomEval_AHTBezierCurve.hxx>

#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomEval_AHTBezierCurve, Geom_BoundedCurve)

//==================================================================================================

int GeomEval_AHTBezierCurve::basisDimension(int    theAlgDegree,
                                             double theAlpha,
                                             double theBeta)
{
  return theAlgDegree + 1
       + (theAlpha > 0.0 ? 2 : 0)
       + (theBeta > 0.0 ? 2 : 0);
}

//==================================================================================================

GeomEval_AHTBezierCurve::GeomEval_AHTBezierCurve(const NCollection_Array1<gp_Pnt>& thePoles,
                                                   int                               theAlgDegree,
                                                   double                            theAlpha,
                                                   double                            theBeta)
    : myPoles(thePoles),
      myWeights(1, 1),
      myAlgDegree(theAlgDegree),
      myAlpha(theAlpha),
      myBeta(theBeta),
      myRational(false)
{
  if (theAlgDegree < 0)
  {
    throw Standard_ConstructionError(
      "GeomEval_AHTBezierCurve: algebraic degree must be >= 0");
  }
  if (theAlpha < 0.0)
  {
    throw Standard_ConstructionError(
      "GeomEval_AHTBezierCurve: alpha must be >= 0");
  }
  if (theBeta < 0.0)
  {
    throw Standard_ConstructionError(
      "GeomEval_AHTBezierCurve: beta must be >= 0");
  }
  const int aDim = basisDimension(theAlgDegree, theAlpha, theBeta);
  if (thePoles.Size() != aDim)
  {
    throw Standard_ConstructionError(
      "GeomEval_AHTBezierCurve: NbPoles must equal basisDimension");
  }
}

//==================================================================================================

GeomEval_AHTBezierCurve::GeomEval_AHTBezierCurve(const NCollection_Array1<gp_Pnt>&  thePoles,
                                                   const NCollection_Array1<double>& theWeights,
                                                   int                               theAlgDegree,
                                                   double                            theAlpha,
                                                   double                            theBeta)
    : myPoles(thePoles),
      myWeights(theWeights),
      myAlgDegree(theAlgDegree),
      myAlpha(theAlpha),
      myBeta(theBeta),
      myRational(true)
{
  if (theAlgDegree < 0)
  {
    throw Standard_ConstructionError(
      "GeomEval_AHTBezierCurve: algebraic degree must be >= 0");
  }
  if (theAlpha < 0.0)
  {
    throw Standard_ConstructionError(
      "GeomEval_AHTBezierCurve: alpha must be >= 0");
  }
  if (theBeta < 0.0)
  {
    throw Standard_ConstructionError(
      "GeomEval_AHTBezierCurve: beta must be >= 0");
  }
  const int aDim = basisDimension(theAlgDegree, theAlpha, theBeta);
  if (thePoles.Size() != aDim)
  {
    throw Standard_ConstructionError(
      "GeomEval_AHTBezierCurve: NbPoles must equal basisDimension");
  }
  if (theWeights.Size() != thePoles.Size())
  {
    throw Standard_ConstructionError(
      "GeomEval_AHTBezierCurve: weights array size must match poles array size");
  }
  for (int anIdx = theWeights.Lower(); anIdx <= theWeights.Upper(); ++anIdx)
  {
    if (theWeights.Value(anIdx) <= 0.0)
    {
      throw Standard_ConstructionError(
        "GeomEval_AHTBezierCurve: all weights must be > 0");
    }
  }
}

//==================================================================================================

const NCollection_Array1<gp_Pnt>& GeomEval_AHTBezierCurve::Poles() const
{
  return myPoles;
}

//==================================================================================================

const NCollection_Array1<double>& GeomEval_AHTBezierCurve::Weights() const
{
  return myWeights;
}

//==================================================================================================

int GeomEval_AHTBezierCurve::AlgDegree() const
{
  return myAlgDegree;
}

//==================================================================================================

double GeomEval_AHTBezierCurve::Alpha() const
{
  return myAlpha;
}

//==================================================================================================

double GeomEval_AHTBezierCurve::Beta() const
{
  return myBeta;
}

//==================================================================================================

int GeomEval_AHTBezierCurve::NbPoles() const
{
  return myPoles.Size();
}

//==================================================================================================

bool GeomEval_AHTBezierCurve::IsRational() const
{
  return myRational;
}

//==================================================================================================

gp_Pnt GeomEval_AHTBezierCurve::StartPoint() const
{
  return EvalD0(0.0);
}

//==================================================================================================

gp_Pnt GeomEval_AHTBezierCurve::EndPoint() const
{
  return EvalD0(1.0);
}

//==================================================================================================

void GeomEval_AHTBezierCurve::Reverse()
{
  // Reverse the poles array.
  const int aLower = myPoles.Lower();
  const int aUpper = myPoles.Upper();
  for (int i = aLower, j = aUpper; i < j; ++i, --j)
  {
    gp_Pnt aTmp = myPoles.Value(i);
    myPoles.ChangeValue(i) = myPoles.Value(j);
    myPoles.ChangeValue(j) = aTmp;
  }
  if (myRational)
  {
    const int aWLower = myWeights.Lower();
    const int aWUpper = myWeights.Upper();
    for (int i = aWLower, j = aWUpper; i < j; ++i, --j)
    {
      double aTmp = myWeights.Value(i);
      myWeights.ChangeValue(i) = myWeights.Value(j);
      myWeights.ChangeValue(j) = aTmp;
    }
  }
}

//==================================================================================================

double GeomEval_AHTBezierCurve::ReversedParameter(const double U) const
{
  return 1.0 - U;
}

//==================================================================================================

double GeomEval_AHTBezierCurve::FirstParameter() const
{
  return 0.0;
}

//==================================================================================================

double GeomEval_AHTBezierCurve::LastParameter() const
{
  return 1.0;
}

//==================================================================================================

bool GeomEval_AHTBezierCurve::IsClosed() const
{
  return StartPoint().Distance(EndPoint()) <= Precision::Confusion();
}

//==================================================================================================

bool GeomEval_AHTBezierCurve::IsPeriodic() const
{
  return false;
}

//==================================================================================================

GeomAbs_Shape GeomEval_AHTBezierCurve::Continuity() const
{
  return GeomAbs_CN;
}

//==================================================================================================

bool GeomEval_AHTBezierCurve::IsCN(const int /*N*/) const
{
  return true;
}

//==================================================================================================

void GeomEval_AHTBezierCurve::evalBasis(double                      theT,
                                         NCollection_Array1<double>& theBasis) const
{
  const int aLower = theBasis.Lower();
  int       anIdx  = aLower;

  // Polynomial part: 1, t, t^2, ..., t^k
  double aPow = 1.0;
  for (int k = 0; k <= myAlgDegree; ++k)
  {
    theBasis.ChangeValue(anIdx++) = aPow;
    aPow *= theT;
  }

  // Hyperbolic part: sinh(alpha*t), cosh(alpha*t)
  if (myAlpha > 0.0)
  {
    const double aAlphaT = myAlpha * theT;
    theBasis.ChangeValue(anIdx++) = std::sinh(aAlphaT);
    theBasis.ChangeValue(anIdx++) = std::cosh(aAlphaT);
  }

  // Trigonometric part: sin(beta*t), cos(beta*t)
  if (myBeta > 0.0)
  {
    const double aBetaT = myBeta * theT;
    theBasis.ChangeValue(anIdx++) = std::sin(aBetaT);
    theBasis.ChangeValue(anIdx++) = std::cos(aBetaT);
  }
}

//==================================================================================================

void GeomEval_AHTBezierCurve::evalBasisDeriv(double                      theT,
                                              int                         theDerivOrder,
                                              NCollection_Array1<double>& theBasisDeriv) const
{
  const int aLower = theBasisDeriv.Lower();
  int       anIdx  = aLower;

  // Polynomial part: d^d/dt^d [t^k] = k!/(k-d)! * t^(k-d) if d <= k, else 0
  for (int k = 0; k <= myAlgDegree; ++k)
  {
    if (theDerivOrder > k)
    {
      theBasisDeriv.ChangeValue(anIdx++) = 0.0;
    }
    else
    {
      // Compute falling factorial: k * (k-1) * ... * (k-d+1)
      double aCoeff = 1.0;
      for (int j = 0; j < theDerivOrder; ++j)
      {
        aCoeff *= (k - j);
      }
      theBasisDeriv.ChangeValue(anIdx++) = aCoeff * std::pow(theT, k - theDerivOrder);
    }
  }

  // Hyperbolic part:
  // d^d/dt^d [sinh(alpha*t)] = alpha^d * (d%2==0 ? sinh(alpha*t) : cosh(alpha*t))
  // d^d/dt^d [cosh(alpha*t)] = alpha^d * (d%2==0 ? cosh(alpha*t) : sinh(alpha*t))
  if (myAlpha > 0.0)
  {
    const double aAlphaT    = myAlpha * theT;
    const double aAlphaPow  = std::pow(myAlpha, theDerivOrder);
    const double aSinh      = std::sinh(aAlphaT);
    const double aCosh      = std::cosh(aAlphaT);
    const bool   isEvenDeriv = (theDerivOrder % 2 == 0);

    theBasisDeriv.ChangeValue(anIdx++) = aAlphaPow * (isEvenDeriv ? aSinh : aCosh);
    theBasisDeriv.ChangeValue(anIdx++) = aAlphaPow * (isEvenDeriv ? aCosh : aSinh);
  }

  // Trigonometric part:
  // d^d/dt^d [sin(beta*t)] = beta^d * sin(beta*t + d*PI/2)
  // d^d/dt^d [cos(beta*t)] = beta^d * cos(beta*t + d*PI/2)
  if (myBeta > 0.0)
  {
    const double aBetaT   = myBeta * theT;
    const double aBetaPow = std::pow(myBeta, theDerivOrder);
    const double aPhase   = theDerivOrder * M_PI / 2.0;

    theBasisDeriv.ChangeValue(anIdx++) = aBetaPow * std::sin(aBetaT + aPhase);
    theBasisDeriv.ChangeValue(anIdx++) = aBetaPow * std::cos(aBetaT + aPhase);
  }
}

//==================================================================================================

gp_Pnt GeomEval_AHTBezierCurve::EvalD0(const double U) const
{
  const int aDim = NbPoles();
  NCollection_Array1<double> aBasis(0, aDim - 1);
  evalBasis(U, aBasis);

  if (!myRational)
  {
    // Non-rational: C(t) = sum_i P_i * B_i(t)
    gp_XYZ aSum(0.0, 0.0, 0.0);
    for (int i = 0; i < aDim; ++i)
    {
      aSum += myPoles.Value(myPoles.Lower() + i).XYZ() * aBasis.Value(i);
    }
    return gp_Pnt(aSum);
  }

  // Rational: C(t) = sum_i (w_i * P_i * B_i(t)) / sum_i (w_i * B_i(t))
  gp_XYZ aNumer(0.0, 0.0, 0.0);
  double  aDenom = 0.0;
  for (int i = 0; i < aDim; ++i)
  {
    const double aWB = myWeights.Value(myWeights.Lower() + i) * aBasis.Value(i);
    aNumer += myPoles.Value(myPoles.Lower() + i).XYZ() * aWB;
    aDenom += aWB;
  }
  return gp_Pnt(aNumer / aDenom);
}

//==================================================================================================

Geom_Curve::ResD1 GeomEval_AHTBezierCurve::EvalD1(const double U) const
{
  const int aDim = NbPoles();
  NCollection_Array1<double> aBasis(0, aDim - 1);
  NCollection_Array1<double> aBasisD1(0, aDim - 1);
  evalBasis(U, aBasis);
  evalBasisDeriv(U, 1, aBasisD1);

  Geom_Curve::ResD1 aResult;

  if (!myRational)
  {
    gp_XYZ aSum(0.0, 0.0, 0.0);
    gp_XYZ aSumD1(0.0, 0.0, 0.0);
    for (int i = 0; i < aDim; ++i)
    {
      const gp_XYZ& aPole = myPoles.Value(myPoles.Lower() + i).XYZ();
      aSum   += aPole * aBasis.Value(i);
      aSumD1 += aPole * aBasisD1.Value(i);
    }
    aResult.Point = gp_Pnt(aSum);
    aResult.D1    = gp_Vec(aSumD1);
    return aResult;
  }

  // Rational quotient rule: C' = (N' * W - N * W') / W^2
  gp_XYZ aN(0.0, 0.0, 0.0);
  gp_XYZ aND1(0.0, 0.0, 0.0);
  double  aW   = 0.0;
  double  aWD1 = 0.0;
  for (int i = 0; i < aDim; ++i)
  {
    const gp_XYZ& aPole = myPoles.Value(myPoles.Lower() + i).XYZ();
    const double  aWi   = myWeights.Value(myWeights.Lower() + i);
    aN   += aPole * (aWi * aBasis.Value(i));
    aND1 += aPole * (aWi * aBasisD1.Value(i));
    aW   += aWi * aBasis.Value(i);
    aWD1 += aWi * aBasisD1.Value(i);
  }
  const double aInvW = 1.0 / aW;
  aResult.Point = gp_Pnt(aN * aInvW);
  aResult.D1    = gp_Vec((aND1 * aW - aN * aWD1) * (aInvW * aInvW));
  return aResult;
}

//==================================================================================================

Geom_Curve::ResD2 GeomEval_AHTBezierCurve::EvalD2(const double U) const
{
  const int aDim = NbPoles();
  NCollection_Array1<double> aBasis(0, aDim - 1);
  NCollection_Array1<double> aBasisD1(0, aDim - 1);
  NCollection_Array1<double> aBasisD2(0, aDim - 1);
  evalBasis(U, aBasis);
  evalBasisDeriv(U, 1, aBasisD1);
  evalBasisDeriv(U, 2, aBasisD2);

  Geom_Curve::ResD2 aResult;

  if (!myRational)
  {
    gp_XYZ aSum(0.0, 0.0, 0.0);
    gp_XYZ aSumD1(0.0, 0.0, 0.0);
    gp_XYZ aSumD2(0.0, 0.0, 0.0);
    for (int i = 0; i < aDim; ++i)
    {
      const gp_XYZ& aPole = myPoles.Value(myPoles.Lower() + i).XYZ();
      aSum   += aPole * aBasis.Value(i);
      aSumD1 += aPole * aBasisD1.Value(i);
      aSumD2 += aPole * aBasisD2.Value(i);
    }
    aResult.Point = gp_Pnt(aSum);
    aResult.D1    = gp_Vec(aSumD1);
    aResult.D2    = gp_Vec(aSumD2);
    return aResult;
  }

  // Rational: use quotient rule for 2nd derivative.
  // C = N/W, C' = (N'W - NW') / W^2
  // C'' = (N''W^2 - NW'' - 2W'(N'W - NW')) / W^3
  //      = (N'' - 2*W'*C' - W''*C) / W
  gp_XYZ aN(0.0, 0.0, 0.0);
  gp_XYZ aND1(0.0, 0.0, 0.0);
  gp_XYZ aND2(0.0, 0.0, 0.0);
  double  aW   = 0.0;
  double  aWD1 = 0.0;
  double  aWD2 = 0.0;
  for (int i = 0; i < aDim; ++i)
  {
    const gp_XYZ& aPole = myPoles.Value(myPoles.Lower() + i).XYZ();
    const double  aWi   = myWeights.Value(myWeights.Lower() + i);
    aN   += aPole * (aWi * aBasis.Value(i));
    aND1 += aPole * (aWi * aBasisD1.Value(i));
    aND2 += aPole * (aWi * aBasisD2.Value(i));
    aW   += aWi * aBasis.Value(i);
    aWD1 += aWi * aBasisD1.Value(i);
    aWD2 += aWi * aBasisD2.Value(i);
  }
  const double aInvW = 1.0 / aW;
  const gp_XYZ aC    = aN * aInvW;
  const gp_XYZ aCD1  = (aND1 * aW - aN * aWD1) * (aInvW * aInvW);

  aResult.Point = gp_Pnt(aC);
  aResult.D1    = gp_Vec(aCD1);
  aResult.D2    = gp_Vec((aND2 - aCD1 * (2.0 * aWD1) - aC * aWD2) * aInvW);
  return aResult;
}

//==================================================================================================

Geom_Curve::ResD3 GeomEval_AHTBezierCurve::EvalD3(const double U) const
{
  const int aDim = NbPoles();
  NCollection_Array1<double> aBasis(0, aDim - 1);
  NCollection_Array1<double> aBasisD1(0, aDim - 1);
  NCollection_Array1<double> aBasisD2(0, aDim - 1);
  NCollection_Array1<double> aBasisD3(0, aDim - 1);
  evalBasis(U, aBasis);
  evalBasisDeriv(U, 1, aBasisD1);
  evalBasisDeriv(U, 2, aBasisD2);
  evalBasisDeriv(U, 3, aBasisD3);

  Geom_Curve::ResD3 aResult;

  if (!myRational)
  {
    gp_XYZ aSum(0.0, 0.0, 0.0);
    gp_XYZ aSumD1(0.0, 0.0, 0.0);
    gp_XYZ aSumD2(0.0, 0.0, 0.0);
    gp_XYZ aSumD3(0.0, 0.0, 0.0);
    for (int i = 0; i < aDim; ++i)
    {
      const gp_XYZ& aPole = myPoles.Value(myPoles.Lower() + i).XYZ();
      aSum   += aPole * aBasis.Value(i);
      aSumD1 += aPole * aBasisD1.Value(i);
      aSumD2 += aPole * aBasisD2.Value(i);
      aSumD3 += aPole * aBasisD3.Value(i);
    }
    aResult.Point = gp_Pnt(aSum);
    aResult.D1    = gp_Vec(aSumD1);
    aResult.D2    = gp_Vec(aSumD2);
    aResult.D3    = gp_Vec(aSumD3);
    return aResult;
  }

  // Rational: C = N/W
  // Use recursive formula: C^(k) = (N^(k) - sum_{j=1}^{k} C(k,j)*W^(j)*C^(k-j)) / W
  gp_XYZ aN(0.0, 0.0, 0.0);
  gp_XYZ aND1(0.0, 0.0, 0.0);
  gp_XYZ aND2(0.0, 0.0, 0.0);
  gp_XYZ aND3(0.0, 0.0, 0.0);
  double  aW   = 0.0;
  double  aWD1 = 0.0;
  double  aWD2 = 0.0;
  double  aWD3 = 0.0;
  for (int i = 0; i < aDim; ++i)
  {
    const gp_XYZ& aPole = myPoles.Value(myPoles.Lower() + i).XYZ();
    const double  aWi   = myWeights.Value(myWeights.Lower() + i);
    aN   += aPole * (aWi * aBasis.Value(i));
    aND1 += aPole * (aWi * aBasisD1.Value(i));
    aND2 += aPole * (aWi * aBasisD2.Value(i));
    aND3 += aPole * (aWi * aBasisD3.Value(i));
    aW   += aWi * aBasis.Value(i);
    aWD1 += aWi * aBasisD1.Value(i);
    aWD2 += aWi * aBasisD2.Value(i);
    aWD3 += aWi * aBasisD3.Value(i);
  }
  const double aInvW = 1.0 / aW;

  // C = N / W
  const gp_XYZ aC = aN * aInvW;

  // C' = (N' - W'*C) / W
  const gp_XYZ aCD1 = (aND1 - aC * aWD1) * aInvW;

  // C'' = (N'' - 2*W'*C' - W''*C) / W
  const gp_XYZ aCD2 = (aND2 - aCD1 * (2.0 * aWD1) - aC * aWD2) * aInvW;

  // C''' = (N''' - 3*W'*C'' - 3*W''*C' - W'''*C) / W
  const gp_XYZ aCD3 = (aND3 - aCD2 * (3.0 * aWD1)
                              - aCD1 * (3.0 * aWD2)
                              - aC * aWD3) * aInvW;

  aResult.Point = gp_Pnt(aC);
  aResult.D1    = gp_Vec(aCD1);
  aResult.D2    = gp_Vec(aCD2);
  aResult.D3    = gp_Vec(aCD3);
  return aResult;
}

//==================================================================================================

gp_Vec GeomEval_AHTBezierCurve::EvalDN(const double U, const int N) const
{
  if (N < 1)
  {
    throw Standard_RangeError("GeomEval_AHTBezierCurve::EvalDN: N must be >= 1");
  }

  // For low orders, delegate to explicit implementations.
  if (N == 1)
  {
    return EvalD1(U).D1;
  }
  if (N == 2)
  {
    return EvalD2(U).D2;
  }
  if (N == 3)
  {
    return EvalD3(U).D3;
  }

  // General case: compute all derivative orders up to N.
  const int aDim = NbPoles();

  // Evaluate basis derivatives for orders 0..N.
  NCollection_Array1<NCollection_Array1<double>> aBasisDerivs(0, N);
  for (int d = 0; d <= N; ++d)
  {
    aBasisDerivs.ChangeValue(d).Resize(0, aDim - 1, false);
    if (d == 0)
    {
      evalBasis(U, aBasisDerivs.ChangeValue(0));
    }
    else
    {
      evalBasisDeriv(U, d, aBasisDerivs.ChangeValue(d));
    }
  }

  if (!myRational)
  {
    gp_XYZ aSum(0.0, 0.0, 0.0);
    for (int i = 0; i < aDim; ++i)
    {
      aSum += myPoles.Value(myPoles.Lower() + i).XYZ() * aBasisDerivs.Value(N).Value(i);
    }
    return gp_Vec(aSum);
  }

  // Rational: use recursive formula.
  // First compute N^(d) and W^(d) for d = 0..N.
  NCollection_Array1<gp_XYZ> aNDerivs(0, N);
  NCollection_Array1<double> aWDerivs(0, N);
  for (int d = 0; d <= N; ++d)
  {
    aNDerivs.ChangeValue(d) = gp_XYZ(0.0, 0.0, 0.0);
    aWDerivs.ChangeValue(d) = 0.0;
    for (int i = 0; i < aDim; ++i)
    {
      const double aWi = myWeights.Value(myWeights.Lower() + i);
      aNDerivs.ChangeValue(d) += myPoles.Value(myPoles.Lower() + i).XYZ()
                                 * (aWi * aBasisDerivs.Value(d).Value(i));
      aWDerivs.ChangeValue(d) += aWi * aBasisDerivs.Value(d).Value(i);
    }
  }

  // Now compute C^(d) for d = 0..N using the recursive formula:
  // C^(d) = (N^(d) - sum_{j=1}^{d} C(d,j)*W^(j)*C^(d-j)) / W
  const double aInvW = 1.0 / aWDerivs.Value(0);
  NCollection_Array1<gp_XYZ> aCDerivs(0, N);
  aCDerivs.ChangeValue(0) = aNDerivs.Value(0) * aInvW;
  for (int d = 1; d <= N; ++d)
  {
    gp_XYZ aSum = aNDerivs.Value(d);
    // Binomial coefficients C(d,j) for j=1..d
    double aBinom = 1.0;
    for (int j = 1; j <= d; ++j)
    {
      aBinom = aBinom * (d - j + 1) / j;
      aSum -= aCDerivs.Value(d - j) * (aBinom * aWDerivs.Value(j));
    }
    aCDerivs.ChangeValue(d) = aSum * aInvW;
  }
  return gp_Vec(aCDerivs.Value(N));
}

//==================================================================================================

void GeomEval_AHTBezierCurve::Transform(const gp_Trsf& T)
{
  // The AHT basis {1, t, ..., t^k, sinh, cosh, sin, cos} does not form
  // a partition of unity. Only the first basis function is constant (= 1).
  // Only the first pole gets the full affine transform; the rest get
  // only the linear part (rotation/scaling, no translation).
  gp_Pnt anOrigin(0.0, 0.0, 0.0);
  anOrigin.Transform(T);
  const gp_XYZ aTransVec = anOrigin.XYZ();

  for (int i = myPoles.Lower(); i <= myPoles.Upper(); ++i)
  {
    myPoles.ChangeValue(i).Transform(T);
  }
  for (int i = myPoles.Lower() + 1; i <= myPoles.Upper(); ++i)
  {
    myPoles.ChangeValue(i).SetXYZ(myPoles.Value(i).XYZ() - aTransVec);
  }
}

//==================================================================================================

occ::handle<Geom_Geometry> GeomEval_AHTBezierCurve::Copy() const
{
  if (myRational)
  {
    return new GeomEval_AHTBezierCurve(myPoles, myWeights, myAlgDegree, myAlpha, myBeta);
  }
  return new GeomEval_AHTBezierCurve(myPoles, myAlgDegree, myAlpha, myBeta);
}

//==================================================================================================

void GeomEval_AHTBezierCurve::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_BoundedCurve)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myAlgDegree)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myAlpha)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myBeta)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myRational)

  for (int i = myPoles.Lower(); i <= myPoles.Upper(); ++i)
  {
    gp_Pnt aPole = myPoles.Value(i);
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &aPole)
  }
}
