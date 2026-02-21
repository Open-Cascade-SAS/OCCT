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

#include <Geom2dEval_TBezierCurve.hxx>

#include <Geom2d_UndefinedDerivative.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Dump.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2dEval_TBezierCurve, Geom2d_BoundedCurve)

//==================================================================================================

Geom2dEval_TBezierCurve::Geom2dEval_TBezierCurve(const NCollection_Array1<gp_Pnt2d>& thePoles,
                                                   double theAlpha)
    : myPoles(thePoles),
      myAlpha(theAlpha),
      myRational(false)
{
  const int aNbPoles = thePoles.Size();
  if (aNbPoles < 3 || (aNbPoles % 2) == 0)
  {
    throw Standard_ConstructionError(
      "Geom2dEval_TBezierCurve: number of poles must be odd and >= 3");
  }
  if (theAlpha <= 0.0)
  {
    throw Standard_ConstructionError(
      "Geom2dEval_TBezierCurve: alpha must be > 0");
  }
}

//==================================================================================================

Geom2dEval_TBezierCurve::Geom2dEval_TBezierCurve(const NCollection_Array1<gp_Pnt2d>& thePoles,
                                                   const NCollection_Array1<double>&   theWeights,
                                                   double theAlpha)
    : myPoles(thePoles),
      myWeights(theWeights),
      myAlpha(theAlpha),
      myRational(true)
{
  const int aNbPoles = thePoles.Size();
  if (aNbPoles < 3 || (aNbPoles % 2) == 0)
  {
    throw Standard_ConstructionError(
      "Geom2dEval_TBezierCurve: number of poles must be odd and >= 3");
  }
  if (theAlpha <= 0.0)
  {
    throw Standard_ConstructionError(
      "Geom2dEval_TBezierCurve: alpha must be > 0");
  }
  if (theWeights.Size() != aNbPoles)
  {
    throw Standard_ConstructionError(
      "Geom2dEval_TBezierCurve: weights size must equal poles size");
  }
  for (int i = theWeights.Lower(); i <= theWeights.Upper(); ++i)
  {
    if (theWeights.Value(i) <= 0.0)
    {
      throw Standard_ConstructionError(
        "Geom2dEval_TBezierCurve: all weights must be > 0");
    }
  }
}

//==================================================================================================

const NCollection_Array1<gp_Pnt2d>& Geom2dEval_TBezierCurve::Poles() const
{
  return myPoles;
}

//==================================================================================================

const NCollection_Array1<double>& Geom2dEval_TBezierCurve::Weights() const
{
  return myWeights;
}

//==================================================================================================

double Geom2dEval_TBezierCurve::Alpha() const
{
  return myAlpha;
}

//==================================================================================================

int Geom2dEval_TBezierCurve::NbPoles() const
{
  return myPoles.Size();
}

//==================================================================================================

int Geom2dEval_TBezierCurve::Order() const
{
  return (myPoles.Size() - 1) / 2;
}

//==================================================================================================

bool Geom2dEval_TBezierCurve::IsRational() const
{
  return myRational;
}

//==================================================================================================

gp_Pnt2d Geom2dEval_TBezierCurve::StartPoint() const
{
  return EvalD0(0.0);
}

//==================================================================================================

gp_Pnt2d Geom2dEval_TBezierCurve::EndPoint() const
{
  return EvalD0(M_PI / myAlpha);
}

//==================================================================================================

void Geom2dEval_TBezierCurve::Reverse()
{
  const int aLower = myPoles.Lower();
  const int aUpper = myPoles.Upper();
  for (int i = aLower, j = aUpper; i < j; ++i, --j)
  {
    gp_Pnt2d aTmp = myPoles.Value(i);
    myPoles.SetValue(i, myPoles.Value(j));
    myPoles.SetValue(j, aTmp);
  }
  if (myRational)
  {
    const int aWLower = myWeights.Lower();
    const int aWUpper = myWeights.Upper();
    for (int i = aWLower, j = aWUpper; i < j; ++i, --j)
    {
      double aTmpW = myWeights.Value(i);
      myWeights.SetValue(i, myWeights.Value(j));
      myWeights.SetValue(j, aTmpW);
    }
  }
}

//==================================================================================================

double Geom2dEval_TBezierCurve::ReversedParameter(const double U) const
{
  return M_PI / myAlpha - U;
}

//==================================================================================================

double Geom2dEval_TBezierCurve::FirstParameter() const
{
  return 0.0;
}

//==================================================================================================

double Geom2dEval_TBezierCurve::LastParameter() const
{
  return M_PI / myAlpha;
}

//==================================================================================================

bool Geom2dEval_TBezierCurve::IsClosed() const
{
  return StartPoint().Distance(EndPoint()) <= Precision::Confusion();
}

//==================================================================================================

bool Geom2dEval_TBezierCurve::IsPeriodic() const
{
  return false;
}

//==================================================================================================

GeomAbs_Shape Geom2dEval_TBezierCurve::Continuity() const
{
  return GeomAbs_CN;
}

//==================================================================================================

bool Geom2dEval_TBezierCurve::IsCN(const int /*N*/) const
{
  return true;
}

//==================================================================================================

void Geom2dEval_TBezierCurve::evalBasis(double                      theT,
                                         NCollection_Array1<double>& theBasis) const
{
  // T_0(t) = 1
  theBasis.SetValue(theBasis.Lower(), 1.0);
  const int anOrder = Order();
  for (int k = 1; k <= anOrder; ++k)
  {
    const double anArg = k * myAlpha * theT;
    theBasis.SetValue(theBasis.Lower() + 2 * k - 1, std::sin(anArg));
    theBasis.SetValue(theBasis.Lower() + 2 * k, std::cos(anArg));
  }
}

//==================================================================================================

void Geom2dEval_TBezierCurve::evalBasisDeriv(double                      theT,
                                              int                         theDerivOrder,
                                              NCollection_Array1<double>& theBasisDeriv) const
{
  // Derivative of constant basis function T_0 = 1 is always 0.
  theBasisDeriv.SetValue(theBasisDeriv.Lower(), 0.0);

  const int anOrder = Order();
  for (int k = 1; k <= anOrder; ++k)
  {
    const double aFreq = k * myAlpha;
    const double anArg = aFreq * theT;
    // d^d/dt^d [sin(k*alpha*t)] = (k*alpha)^d * sin(k*alpha*t + d*Pi/2)
    // d^d/dt^d [cos(k*alpha*t)] = (k*alpha)^d * cos(k*alpha*t + d*Pi/2)
    const double aFreqPow  = std::pow(aFreq, theDerivOrder);
    const double aPhase    = theDerivOrder * M_PI / 2.0;

    theBasisDeriv.SetValue(theBasisDeriv.Lower() + 2 * k - 1,
                           aFreqPow * std::sin(anArg + aPhase));
    theBasisDeriv.SetValue(theBasisDeriv.Lower() + 2 * k,
                           aFreqPow * std::cos(anArg + aPhase));
  }
}

//==================================================================================================

gp_Pnt2d Geom2dEval_TBezierCurve::evalNonRationalPoint(
  const NCollection_Array1<double>& theBasis) const
{
  gp_XY aSum(0.0, 0.0);
  for (int i = 0; i < myPoles.Size(); ++i)
  {
    aSum += theBasis.Value(theBasis.Lower() + i) * myPoles.Value(myPoles.Lower() + i).XY();
  }
  return gp_Pnt2d(aSum);
}

//==================================================================================================

gp_Vec2d Geom2dEval_TBezierCurve::evalNonRationalDeriv(
  const NCollection_Array1<double>& theBasisDeriv) const
{
  gp_XY aSum(0.0, 0.0);
  for (int i = 0; i < myPoles.Size(); ++i)
  {
    aSum += theBasisDeriv.Value(theBasisDeriv.Lower() + i)
            * myPoles.Value(myPoles.Lower() + i).XY();
  }
  return gp_Vec2d(aSum);
}

//==================================================================================================

gp_Pnt2d Geom2dEval_TBezierCurve::EvalD0(const double U) const
{
  const int aNbPoles = myPoles.Size();
  NCollection_Array1<double> aBasis(1, aNbPoles);
  evalBasis(U, aBasis);

  if (!myRational)
  {
    return evalNonRationalPoint(aBasis);
  }

  // Rational: C(t) = sum(w_i * P_i * B_i) / sum(w_i * B_i)
  gp_XY  aNumer(0.0, 0.0);
  double aDenom = 0.0;
  for (int i = 0; i < aNbPoles; ++i)
  {
    const double aWB = myWeights.Value(myWeights.Lower() + i) * aBasis.Value(1 + i);
    aNumer += aWB * myPoles.Value(myPoles.Lower() + i).XY();
    aDenom += aWB;
  }
  return gp_Pnt2d(aNumer / aDenom);
}

//==================================================================================================

Geom2d_Curve::ResD1 Geom2dEval_TBezierCurve::EvalD1(const double U) const
{
  const int aNbPoles = myPoles.Size();
  NCollection_Array1<double> aBasis(1, aNbPoles);
  NCollection_Array1<double> aBasisD1(1, aNbPoles);
  evalBasis(U, aBasis);
  evalBasisDeriv(U, 1, aBasisD1);

  Geom2d_Curve::ResD1 aResult;

  if (!myRational)
  {
    aResult.Point = evalNonRationalPoint(aBasis);
    aResult.D1    = evalNonRationalDeriv(aBasisD1);
    return aResult;
  }

  // Rational evaluation using homogeneous coordinates.
  // Cw(t) = sum(w_i * P_i * B_i(t)), w(t) = sum(w_i * B_i(t))
  // C(t) = Cw(t) / w(t)
  // C'(t) = (Cw'(t) - w'(t) * C(t)) / w(t)
  gp_XY  aCw(0.0, 0.0);
  gp_XY  aCwD1(0.0, 0.0);
  double aW   = 0.0;
  double aWD1 = 0.0;
  for (int i = 0; i < aNbPoles; ++i)
  {
    const double aWi  = myWeights.Value(myWeights.Lower() + i);
    const double aBi  = aBasis.Value(1 + i);
    const double aBiD = aBasisD1.Value(1 + i);
    const gp_XY& aPi  = myPoles.Value(myPoles.Lower() + i).XY();
    aCw   += (aWi * aBi)  * aPi;
    aCwD1 += (aWi * aBiD) * aPi;
    aW    += aWi * aBi;
    aWD1  += aWi * aBiD;
  }

  const gp_XY aC = aCw / aW;
  aResult.Point = gp_Pnt2d(aC);
  aResult.D1    = gp_Vec2d((aCwD1 - aWD1 * aC) / aW);
  return aResult;
}

//==================================================================================================

Geom2d_Curve::ResD2 Geom2dEval_TBezierCurve::EvalD2(const double U) const
{
  const int aNbPoles = myPoles.Size();
  NCollection_Array1<double> aBasis(1, aNbPoles);
  NCollection_Array1<double> aBasisD1(1, aNbPoles);
  NCollection_Array1<double> aBasisD2(1, aNbPoles);
  evalBasis(U, aBasis);
  evalBasisDeriv(U, 1, aBasisD1);
  evalBasisDeriv(U, 2, aBasisD2);

  Geom2d_Curve::ResD2 aResult;

  if (!myRational)
  {
    aResult.Point = evalNonRationalPoint(aBasis);
    aResult.D1    = evalNonRationalDeriv(aBasisD1);
    aResult.D2    = evalNonRationalDeriv(aBasisD2);
    return aResult;
  }

  // Rational: Cw, Cw', Cw'', w, w', w''
  gp_XY  aCw(0.0, 0.0);
  gp_XY  aCwD1(0.0, 0.0);
  gp_XY  aCwD2(0.0, 0.0);
  double aW   = 0.0;
  double aWD1 = 0.0;
  double aWD2 = 0.0;
  for (int i = 0; i < aNbPoles; ++i)
  {
    const double aWi   = myWeights.Value(myWeights.Lower() + i);
    const double aBi   = aBasis.Value(1 + i);
    const double aBiD1 = aBasisD1.Value(1 + i);
    const double aBiD2 = aBasisD2.Value(1 + i);
    const gp_XY& aPi   = myPoles.Value(myPoles.Lower() + i).XY();
    aCw   += (aWi * aBi)   * aPi;
    aCwD1 += (aWi * aBiD1) * aPi;
    aCwD2 += (aWi * aBiD2) * aPi;
    aW    += aWi * aBi;
    aWD1  += aWi * aBiD1;
    aWD2  += aWi * aBiD2;
  }

  // C(t) = Cw / w
  // C'(t) = (Cw' - w' * C) / w
  // C''(t) = (Cw'' - 2*w'*C' - w''*C) / w
  const gp_XY aC   = aCw / aW;
  const gp_XY aCD1 = (aCwD1 - aWD1 * aC) / aW;
  const gp_XY aCD2 = (aCwD2 - 2.0 * aWD1 * aCD1 - aWD2 * aC) / aW;

  aResult.Point = gp_Pnt2d(aC);
  aResult.D1    = gp_Vec2d(aCD1);
  aResult.D2    = gp_Vec2d(aCD2);
  return aResult;
}

//==================================================================================================

Geom2d_Curve::ResD3 Geom2dEval_TBezierCurve::EvalD3(const double U) const
{
  const int aNbPoles = myPoles.Size();
  NCollection_Array1<double> aBasis(1, aNbPoles);
  NCollection_Array1<double> aBasisD1(1, aNbPoles);
  NCollection_Array1<double> aBasisD2(1, aNbPoles);
  NCollection_Array1<double> aBasisD3(1, aNbPoles);
  evalBasis(U, aBasis);
  evalBasisDeriv(U, 1, aBasisD1);
  evalBasisDeriv(U, 2, aBasisD2);
  evalBasisDeriv(U, 3, aBasisD3);

  Geom2d_Curve::ResD3 aResult;

  if (!myRational)
  {
    aResult.Point = evalNonRationalPoint(aBasis);
    aResult.D1    = evalNonRationalDeriv(aBasisD1);
    aResult.D2    = evalNonRationalDeriv(aBasisD2);
    aResult.D3    = evalNonRationalDeriv(aBasisD3);
    return aResult;
  }

  // Rational: compute weighted sums for derivatives up to order 3.
  gp_XY  aCw(0.0, 0.0);
  gp_XY  aCwD1(0.0, 0.0);
  gp_XY  aCwD2(0.0, 0.0);
  gp_XY  aCwD3(0.0, 0.0);
  double aW   = 0.0;
  double aWD1 = 0.0;
  double aWD2 = 0.0;
  double aWD3 = 0.0;
  for (int i = 0; i < aNbPoles; ++i)
  {
    const double aWi   = myWeights.Value(myWeights.Lower() + i);
    const double aBi   = aBasis.Value(1 + i);
    const double aBiD1 = aBasisD1.Value(1 + i);
    const double aBiD2 = aBasisD2.Value(1 + i);
    const double aBiD3 = aBasisD3.Value(1 + i);
    const gp_XY& aPi   = myPoles.Value(myPoles.Lower() + i).XY();
    aCw   += (aWi * aBi)   * aPi;
    aCwD1 += (aWi * aBiD1) * aPi;
    aCwD2 += (aWi * aBiD2) * aPi;
    aCwD3 += (aWi * aBiD3) * aPi;
    aW    += aWi * aBi;
    aWD1  += aWi * aBiD1;
    aWD2  += aWi * aBiD2;
    aWD3  += aWi * aBiD3;
  }

  // C   = Cw / w
  // C'  = (Cw' - w'*C) / w
  // C'' = (Cw'' - 2*w'*C' - w''*C) / w
  // C'''= (Cw''' - 3*w''*C' - 3*w'*C'' - w'''*C) / w
  const gp_XY aC   = aCw / aW;
  const gp_XY aCD1 = (aCwD1 - aWD1 * aC) / aW;
  const gp_XY aCD2 = (aCwD2 - 2.0 * aWD1 * aCD1 - aWD2 * aC) / aW;
  const gp_XY aCD3 = (aCwD3 - 3.0 * aWD2 * aCD1 - 3.0 * aWD1 * aCD2 - aWD3 * aC) / aW;

  aResult.Point = gp_Pnt2d(aC);
  aResult.D1    = gp_Vec2d(aCD1);
  aResult.D2    = gp_Vec2d(aCD2);
  aResult.D3    = gp_Vec2d(aCD3);
  return aResult;
}

//==================================================================================================

gp_Vec2d Geom2dEval_TBezierCurve::EvalDN(const double U, const int N) const
{
  if (N < 1)
  {
    throw Geom2d_UndefinedDerivative(
      "Geom2dEval_TBezierCurve::EvalDN: derivative order must be >= 1");
  }

  // For non-rational curves, compute directly.
  if (!myRational)
  {
    const int aNbPoles = myPoles.Size();
    NCollection_Array1<double> aBasisDN(1, aNbPoles);
    evalBasisDeriv(U, N, aBasisDN);
    return evalNonRationalDeriv(aBasisDN);
  }

  // For rational curves with N <= 3, delegate to the full evaluation.
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

  // For N > 3 on rational curves, use the general recurrence formula:
  // C^(n)(t) = (Cw^(n)(t) - sum_{k=1..n} C(n,k)*w^(k)(t)*C^(n-k)(t)) / w(t)
  // We compute iteratively from lower-order derivatives.
  const int aNbPoles = myPoles.Size();

  // Precompute all basis derivative arrays up to order N.
  NCollection_Array1<NCollection_Array1<double>> aBasisDerivs(0, N);
  for (int d = 0; d <= N; ++d)
  {
    aBasisDerivs.ChangeValue(d).Resize(1, aNbPoles, false);
    if (d == 0)
    {
      evalBasis(U, aBasisDerivs.ChangeValue(0));
    }
    else
    {
      evalBasisDeriv(U, d, aBasisDerivs.ChangeValue(d));
    }
  }

  // Compute w^(d) and Cw^(d) for d = 0..N.
  NCollection_Array1<double> aWDerivs(0, N);
  NCollection_Array1<gp_XY>  aCwDerivs(0, N);
  for (int d = 0; d <= N; ++d)
  {
    double aWd(0.0);
    gp_XY  aCwd(0.0, 0.0);
    for (int i = 0; i < aNbPoles; ++i)
    {
      const double aWi  = myWeights.Value(myWeights.Lower() + i);
      const double aBd  = aBasisDerivs.Value(d).Value(1 + i);
      const gp_XY& aPi  = myPoles.Value(myPoles.Lower() + i).XY();
      aCwd += (aWi * aBd) * aPi;
      aWd  += aWi * aBd;
    }
    aWDerivs.SetValue(d, aWd);
    aCwDerivs.SetValue(d, aCwd);
  }

  // Compute C^(d) for d = 0..N using the recurrence.
  NCollection_Array1<gp_XY> aCDerivs(0, N);
  aCDerivs.SetValue(0, aCwDerivs.Value(0) / aWDerivs.Value(0));

  for (int d = 1; d <= N; ++d)
  {
    gp_XY aSum(0.0, 0.0);
    // Binomial coefficients via incremental computation.
    double aBinom = 1.0;
    for (int k = 1; k <= d; ++k)
    {
      aBinom *= static_cast<double>(d - k + 1) / static_cast<double>(k);
      aSum += aBinom * aWDerivs.Value(k) * aCDerivs.Value(d - k);
    }
    aCDerivs.SetValue(d, (aCwDerivs.Value(d) - aSum) / aWDerivs.Value(0));
  }

  return gp_Vec2d(aCDerivs.Value(N));
}

//==================================================================================================

void Geom2dEval_TBezierCurve::Transform(const gp_Trsf2d& T)
{
  // The trigonometric basis does not form a partition of unity.
  // Only the first pole (constant basis = 1) gets the full affine transform;
  // the rest get only the linear part (rotation/scaling, no translation).
  gp_Pnt2d anOrigin(0.0, 0.0);
  anOrigin.Transform(T);
  const gp_XY aTransVec = anOrigin.Coord();

  for (int i = myPoles.Lower(); i <= myPoles.Upper(); ++i)
  {
    myPoles.ChangeValue(i).Transform(T);
  }
  for (int i = myPoles.Lower() + 1; i <= myPoles.Upper(); ++i)
  {
    myPoles.ChangeValue(i).SetXY(myPoles.Value(i).XY() - aTransVec);
  }
}

//==================================================================================================

occ::handle<Geom2d_Geometry> Geom2dEval_TBezierCurve::Copy() const
{
  if (myRational)
  {
    return new Geom2dEval_TBezierCurve(myPoles, myWeights, myAlpha);
  }
  return new Geom2dEval_TBezierCurve(myPoles, myAlpha);
}

//==================================================================================================

void Geom2dEval_TBezierCurve::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom2d_BoundedCurve)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myAlpha)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myRational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPoles.Size())
}
