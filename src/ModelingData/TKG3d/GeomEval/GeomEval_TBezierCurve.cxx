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

#include <GeomEval_TBezierCurve.hxx>

#include <Geom_UndefinedDerivative.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Dump.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomEval_TBezierCurve, Geom_BoundedCurve)

namespace
{
template <int theMaxOrder>
struct CurveBasisEval
{
  gp_XYZ N0;
  gp_XYZ N1;
  gp_XYZ N2;
  gp_XYZ N3;
  double W0 = 0.0;
  double W1 = 0.0;
  double W2 = 0.0;
  double W3 = 0.0;
};

inline double powInt(const double theValue, const int thePower)
{
  double aRes = 1.0;
  for (int i = 0; i < thePower; ++i)
  {
    aRes *= theValue;
  }
  return aRes;
}

inline void evalTrigPairNthDeriv(const double theSin,
                                 const double theCos,
                                 const int    theOrderMod4,
                                 double&      theSinDeriv,
                                 double&      theCosDeriv)
{
  switch (theOrderMod4)
  {
    case 0:
      theSinDeriv = theSin;
      theCosDeriv = theCos;
      break;
    case 1:
      theSinDeriv = theCos;
      theCosDeriv = -theSin;
      break;
    case 2:
      theSinDeriv = -theSin;
      theCosDeriv = -theCos;
      break;
    default:
      theSinDeriv = -theCos;
      theCosDeriv = theSin;
      break;
  }
}

template <int theMaxOrder>
void evalCurveBasis(const NCollection_Array1<gp_Pnt>& thePoles,
                    const NCollection_Array1<double>* theWeights,
                    const double                      theAlpha,
                    const double                      theU,
                    CurveBasisEval<theMaxOrder>&      theData)
{
  theData                = CurveBasisEval<theMaxOrder>();
  const int     aPoleLow = thePoles.Lower();
  const int     aNbPoles = thePoles.Size();
  const gp_Pnt* aPoles   = &thePoles.Value(aPoleLow);

  const int     aWeightLow = (theWeights != nullptr) ? theWeights->Lower() : 0;
  const double* aWeights   = (theWeights != nullptr) ? &theWeights->Value(aWeightLow) : nullptr;
  const gp_XYZ& aPole0     = aPoles[0].XYZ();
  if (theWeights == nullptr)
  {
    theData.N0 += aPole0;
  }
  else
  {
    const double aW0 = aWeights[0];
    theData.N0 += aPole0 * aW0;
    theData.W0 += aW0;
  }

  const int anOrder = (aNbPoles - 1) / 2;
  if (anOrder <= 0)
  {
    return;
  }

  const double aTheta = theAlpha * theU;
  const double aSin1  = std::sin(aTheta);
  const double aCos1  = std::cos(aTheta);
  double       aSinK  = aSin1;
  double       aCosK  = aCos1;
  double       aFreq  = theAlpha;
  int          aIdx   = 1;
  for (int k = 1; k <= anOrder; ++k, aIdx += 2, aFreq += theAlpha)
  {
    const double aSin   = aSinK;
    const double aCos   = aCosK;
    const double aFreq2 = aFreq * aFreq;
    const double aFreq3 = aFreq2 * aFreq;

    const gp_XYZ& aPoleSin = aPoles[aIdx].XYZ();
    const gp_XYZ& aPoleCos = aPoles[aIdx + 1].XYZ();

    const double aB0Sin = aSin;
    const double aB0Cos = aCos;

    double aB1Sin = 0.0;
    double aB1Cos = 0.0;
    double aB2Sin = 0.0;
    double aB2Cos = 0.0;
    double aB3Sin = 0.0;
    double aB3Cos = 0.0;
    if constexpr (theMaxOrder >= 1)
    {
      aB1Sin = aFreq * aCos;
      aB1Cos = -aFreq * aSin;
    }
    if constexpr (theMaxOrder >= 2)
    {
      aB2Sin = -aFreq2 * aSin;
      aB2Cos = -aFreq2 * aCos;
    }
    if constexpr (theMaxOrder >= 3)
    {
      aB3Sin = -aFreq3 * aCos;
      aB3Cos = aFreq3 * aSin;
    }

    if (theWeights == nullptr)
    {
      theData.N0 += aPoleSin * aB0Sin + aPoleCos * aB0Cos;
      if constexpr (theMaxOrder >= 1)
      {
        theData.N1 += aPoleSin * aB1Sin + aPoleCos * aB1Cos;
      }
      if constexpr (theMaxOrder >= 2)
      {
        theData.N2 += aPoleSin * aB2Sin + aPoleCos * aB2Cos;
      }
      if constexpr (theMaxOrder >= 3)
      {
        theData.N3 += aPoleSin * aB3Sin + aPoleCos * aB3Cos;
      }
    }
    else
    {
      const double aWSin = aWeights[aIdx];
      const double aWCos = aWeights[aIdx + 1];

      theData.N0 += aPoleSin * (aWSin * aB0Sin) + aPoleCos * (aWCos * aB0Cos);
      theData.W0 += aWSin * aB0Sin + aWCos * aB0Cos;
      if constexpr (theMaxOrder >= 1)
      {
        theData.N1 += aPoleSin * (aWSin * aB1Sin) + aPoleCos * (aWCos * aB1Cos);
        theData.W1 += aWSin * aB1Sin + aWCos * aB1Cos;
      }
      if constexpr (theMaxOrder >= 2)
      {
        theData.N2 += aPoleSin * (aWSin * aB2Sin) + aPoleCos * (aWCos * aB2Cos);
        theData.W2 += aWSin * aB2Sin + aWCos * aB2Cos;
      }
      if constexpr (theMaxOrder >= 3)
      {
        theData.N3 += aPoleSin * (aWSin * aB3Sin) + aPoleCos * (aWCos * aB3Cos);
        theData.W3 += aWSin * aB3Sin + aWCos * aB3Cos;
      }
    }

    if (k < anOrder)
    {
      const double aSinNext = aSinK * aCos1 + aCosK * aSin1;
      const double aCosNext = aCosK * aCos1 - aSinK * aSin1;
      aSinK                 = aSinNext;
      aCosK                 = aCosNext;
    }
  }
}
} // namespace

//==================================================================================================

GeomEval_TBezierCurve::GeomEval_TBezierCurve(const NCollection_Array1<gp_Pnt>& thePoles,
                                             double                            theAlpha)
    : myPoles(thePoles),
      myAlpha(theAlpha),
      myRational(false)
{
  const int aNbPoles = thePoles.Size();
  if (aNbPoles < 3 || (aNbPoles % 2) == 0)
  {
    throw Standard_ConstructionError("GeomEval_TBezierCurve: number of poles must be odd and >= 3");
  }
  if (theAlpha <= 0.0)
  {
    throw Standard_ConstructionError("GeomEval_TBezierCurve: alpha must be > 0");
  }
}

//==================================================================================================

GeomEval_TBezierCurve::GeomEval_TBezierCurve(const NCollection_Array1<gp_Pnt>& thePoles,
                                             const NCollection_Array1<double>& theWeights,
                                             double                            theAlpha)
    : myPoles(thePoles),
      myWeights(theWeights),
      myAlpha(theAlpha),
      myRational(true)
{
  const int aNbPoles = thePoles.Size();
  if (aNbPoles < 3 || (aNbPoles % 2) == 0)
  {
    throw Standard_ConstructionError("GeomEval_TBezierCurve: number of poles must be odd and >= 3");
  }
  if (theAlpha <= 0.0)
  {
    throw Standard_ConstructionError("GeomEval_TBezierCurve: alpha must be > 0");
  }
  if (theWeights.Size() != aNbPoles)
  {
    throw Standard_ConstructionError("GeomEval_TBezierCurve: weights size must equal poles size");
  }
  for (int i = theWeights.Lower(); i <= theWeights.Upper(); ++i)
  {
    if (theWeights.Value(i) <= 0.0)
    {
      throw Standard_ConstructionError("GeomEval_TBezierCurve: all weights must be > 0");
    }
  }
}

//==================================================================================================

const NCollection_Array1<gp_Pnt>& GeomEval_TBezierCurve::Poles() const
{
  return myPoles;
}

//==================================================================================================

const NCollection_Array1<double>& GeomEval_TBezierCurve::Weights() const
{
  return myWeights;
}

//==================================================================================================

double GeomEval_TBezierCurve::Alpha() const
{
  return myAlpha;
}

//==================================================================================================

int GeomEval_TBezierCurve::NbPoles() const
{
  return myPoles.Size();
}

//==================================================================================================

int GeomEval_TBezierCurve::Order() const
{
  return (myPoles.Size() - 1) / 2;
}

//==================================================================================================

bool GeomEval_TBezierCurve::IsRational() const
{
  return myRational;
}

//==================================================================================================

gp_Pnt GeomEval_TBezierCurve::StartPoint() const
{
  return EvalD0(0.0);
}

//==================================================================================================

gp_Pnt GeomEval_TBezierCurve::EndPoint() const
{
  return EvalD0(M_PI / myAlpha);
}

//==================================================================================================

void GeomEval_TBezierCurve::Reverse()
{
  throw Standard_NotImplemented("GeomEval_TBezierCurve::Reverse");
}

//==================================================================================================

double GeomEval_TBezierCurve::ReversedParameter(const double U) const
{
  (void)U;
  throw Standard_NotImplemented("GeomEval_TBezierCurve::ReversedParameter");
}

//==================================================================================================

double GeomEval_TBezierCurve::FirstParameter() const
{
  return 0.0;
}

//==================================================================================================

double GeomEval_TBezierCurve::LastParameter() const
{
  return M_PI / myAlpha;
}

//==================================================================================================

bool GeomEval_TBezierCurve::IsClosed() const
{
  return StartPoint().Distance(EndPoint()) <= Precision::Confusion();
}

//==================================================================================================

bool GeomEval_TBezierCurve::IsPeriodic() const
{
  return false;
}

//==================================================================================================

GeomAbs_Shape GeomEval_TBezierCurve::Continuity() const
{
  return GeomAbs_CN;
}

//==================================================================================================

bool GeomEval_TBezierCurve::IsCN(const int /*N*/) const
{
  return true;
}

//==================================================================================================

void GeomEval_TBezierCurve::evalBasis(double theT, NCollection_Array1<double>& theBasis) const
{
  // T_0(t) = 1
  const int aBasisLow = theBasis.Lower();
  double*   aBasis    = &theBasis.ChangeValue(aBasisLow);
  aBasis[0]           = 1.0;
  const int anOrder   = Order();
  if (anOrder <= 0)
  {
    return;
  }

  const double aTheta = myAlpha * theT;
  const double aSin1  = std::sin(aTheta);
  const double aCos1  = std::cos(aTheta);
  double       aSinK  = aSin1;
  double       aCosK  = aCos1;
  for (int k = 1; k <= anOrder; ++k)
  {
    aBasis[2 * k - 1] = aSinK;
    aBasis[2 * k]     = aCosK;
    if (k < anOrder)
    {
      const double aSinNext = aSinK * aCos1 + aCosK * aSin1;
      const double aCosNext = aCosK * aCos1 - aSinK * aSin1;
      aSinK                 = aSinNext;
      aCosK                 = aCosNext;
    }
  }
}

//==================================================================================================

void GeomEval_TBezierCurve::evalBasisDeriv(double                      theT,
                                           int                         theDerivOrder,
                                           NCollection_Array1<double>& theBasisDeriv) const
{
  // Derivative of constant basis function T_0 = 1 is always 0.
  const int aBasisLow = theBasisDeriv.Lower();
  double*   aBasis    = &theBasisDeriv.ChangeValue(aBasisLow);
  aBasis[0]           = 0.0;

  const int anOrder = Order();
  if (anOrder <= 0)
  {
    return;
  }

  const int    anOrderMod4 = theDerivOrder & 3;
  const double aTheta      = myAlpha * theT;
  const double aSin1       = std::sin(aTheta);
  const double aCos1       = std::cos(aTheta);
  double       aSinK       = aSin1;
  double       aCosK       = aCos1;
  double       aFreq       = myAlpha;
  for (int k = 1; k <= anOrder; ++k)
  {
    const double aSin      = aSinK;
    const double aCos      = aCosK;
    const double aFreqPow  = powInt(aFreq, theDerivOrder);
    double       aSinDeriv = 0.0;
    double       aCosDeriv = 0.0;
    evalTrigPairNthDeriv(aSin, aCos, anOrderMod4, aSinDeriv, aCosDeriv);
    aBasis[2 * k - 1] = aFreqPow * aSinDeriv;
    aBasis[2 * k]     = aFreqPow * aCosDeriv;
    aFreq += myAlpha;
    if (k < anOrder)
    {
      const double aSinNext = aSinK * aCos1 + aCosK * aSin1;
      const double aCosNext = aCosK * aCos1 - aSinK * aSin1;
      aSinK                 = aSinNext;
      aCosK                 = aCosNext;
    }
  }
}

//==================================================================================================

gp_Pnt GeomEval_TBezierCurve::evalNonRationalPoint(const NCollection_Array1<double>& theBasis) const
{
  gp_XYZ        aSum(0.0, 0.0, 0.0);
  const int     aNbPoles   = myPoles.Size();
  const gp_Pnt* aPolesData = &myPoles.Value(myPoles.Lower());
  const double* aBasisData = &theBasis.Value(theBasis.Lower());
  for (int i = 0; i < aNbPoles; ++i)
  {
    aSum += aBasisData[i] * aPolesData[i].XYZ();
  }
  return gp_Pnt(aSum);
}

//==================================================================================================

gp_Vec GeomEval_TBezierCurve::evalNonRationalDeriv(
  const NCollection_Array1<double>& theBasisDeriv) const
{
  gp_XYZ        aSum(0.0, 0.0, 0.0);
  const int     aNbPoles   = myPoles.Size();
  const gp_Pnt* aPolesData = &myPoles.Value(myPoles.Lower());
  const double* aBasisData = &theBasisDeriv.Value(theBasisDeriv.Lower());
  for (int i = 0; i < aNbPoles; ++i)
  {
    aSum += aBasisData[i] * aPolesData[i].XYZ();
  }
  return gp_Vec(aSum);
}

//==================================================================================================

gp_Pnt GeomEval_TBezierCurve::EvalD0(const double U) const
{
  CurveBasisEval<0> aData;
  if (!myRational)
  {
    evalCurveBasis<0>(myPoles, nullptr, myAlpha, U, aData);
    return gp_Pnt(aData.N0);
  }
  evalCurveBasis<0>(myPoles, &myWeights, myAlpha, U, aData);
  return gp_Pnt(aData.N0 / aData.W0);
}

//==================================================================================================

Geom_Curve::ResD1 GeomEval_TBezierCurve::EvalD1(const double U) const
{
  Geom_Curve::ResD1 aResult;
  CurveBasisEval<1> aData;

  if (!myRational)
  {
    evalCurveBasis<1>(myPoles, nullptr, myAlpha, U, aData);
    aResult.Point = gp_Pnt(aData.N0);
    aResult.D1    = gp_Vec(aData.N1);
    return aResult;
  }

  evalCurveBasis<1>(myPoles, &myWeights, myAlpha, U, aData);
  const gp_XYZ aC = aData.N0 / aData.W0;
  aResult.Point   = gp_Pnt(aC);
  aResult.D1      = gp_Vec((aData.N1 - aData.W1 * aC) / aData.W0);
  return aResult;
}

//==================================================================================================

Geom_Curve::ResD2 GeomEval_TBezierCurve::EvalD2(const double U) const
{
  Geom_Curve::ResD2 aResult;
  CurveBasisEval<2> aData;

  if (!myRational)
  {
    evalCurveBasis<2>(myPoles, nullptr, myAlpha, U, aData);
    aResult.Point = gp_Pnt(aData.N0);
    aResult.D1    = gp_Vec(aData.N1);
    aResult.D2    = gp_Vec(aData.N2);
    return aResult;
  }

  evalCurveBasis<2>(myPoles, &myWeights, myAlpha, U, aData);
  const gp_XYZ aC   = aData.N0 / aData.W0;
  const gp_XYZ aCD1 = (aData.N1 - aData.W1 * aC) / aData.W0;
  const gp_XYZ aCD2 = (aData.N2 - 2.0 * aData.W1 * aCD1 - aData.W2 * aC) / aData.W0;

  aResult.Point = gp_Pnt(aC);
  aResult.D1    = gp_Vec(aCD1);
  aResult.D2    = gp_Vec(aCD2);
  return aResult;
}

//==================================================================================================

Geom_Curve::ResD3 GeomEval_TBezierCurve::EvalD3(const double U) const
{
  Geom_Curve::ResD3 aResult;
  CurveBasisEval<3> aData;

  if (!myRational)
  {
    evalCurveBasis<3>(myPoles, nullptr, myAlpha, U, aData);
    aResult.Point = gp_Pnt(aData.N0);
    aResult.D1    = gp_Vec(aData.N1);
    aResult.D2    = gp_Vec(aData.N2);
    aResult.D3    = gp_Vec(aData.N3);
    return aResult;
  }

  evalCurveBasis<3>(myPoles, &myWeights, myAlpha, U, aData);
  const gp_XYZ aC   = aData.N0 / aData.W0;
  const gp_XYZ aCD1 = (aData.N1 - aData.W1 * aC) / aData.W0;
  const gp_XYZ aCD2 = (aData.N2 - 2.0 * aData.W1 * aCD1 - aData.W2 * aC) / aData.W0;
  const gp_XYZ aCD3 =
    (aData.N3 - 3.0 * aData.W2 * aCD1 - 3.0 * aData.W1 * aCD2 - aData.W3 * aC) / aData.W0;

  aResult.Point = gp_Pnt(aC);
  aResult.D1    = gp_Vec(aCD1);
  aResult.D2    = gp_Vec(aCD2);
  aResult.D3    = gp_Vec(aCD3);
  return aResult;
}

//==================================================================================================

gp_Vec GeomEval_TBezierCurve::EvalDN(const double U, const int N) const
{
  if (N < 1)
  {
    throw Geom_UndefinedDerivative("GeomEval_TBezierCurve::EvalDN: derivative order must be >= 1");
  }

  // For non-rational curves, compute directly.
  if (!myRational)
  {
    gp_XYZ    aSum(0.0, 0.0, 0.0);
    const int anOrder    = Order();
    const int aOrderMod4 = N & 3;
    if (anOrder <= 0)
    {
      return gp_Vec(aSum);
    }

    const gp_Pnt* aPolesData = &myPoles.Value(myPoles.Lower());
    const double  aTheta     = myAlpha * U;
    const double  aSin1      = std::sin(aTheta);
    const double  aCos1      = std::cos(aTheta);
    double        aSinK      = aSin1;
    double        aCosK      = aCos1;
    double        aFreq      = myAlpha;
    int           aIdx       = 1;

    for (int k = 1; k <= anOrder; ++k, aIdx += 2, aFreq += myAlpha)
    {
      const double aFreqPow = powInt(aFreq, N);
      const double aSin     = aSinK;
      const double aCos     = aCosK;

      double aSinDeriv = 0.0;
      double aCosDeriv = 0.0;
      evalTrigPairNthDeriv(aSin, aCos, aOrderMod4, aSinDeriv, aCosDeriv);

      aSum += aPolesData[aIdx].XYZ() * (aFreqPow * aSinDeriv);
      aSum += aPolesData[aIdx + 1].XYZ() * (aFreqPow * aCosDeriv);

      if (k < anOrder)
      {
        const double aSinNext = aSinK * aCos1 + aCosK * aSin1;
        const double aCosNext = aCosK * aCos1 - aSinK * aSin1;
        aSinK                 = aSinNext;
        aCosK                 = aCosNext;
      }
    }
    return gp_Vec(aSum);
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
  NCollection_Array1<gp_XYZ> aCwDerivs(0, N);
  const gp_Pnt*              aPolesData   = &myPoles.Value(myPoles.Lower());
  const double*              aWeightsData = &myWeights.Value(myWeights.Lower());
  for (int d = 0; d <= N; ++d)
  {
    const NCollection_Array1<double>& aBasisD    = aBasisDerivs.Value(d);
    const double*                     aBasisData = &aBasisD.Value(1);
    double                            aWd(0.0);
    gp_XYZ                            aCwd(0.0, 0.0, 0.0);
    for (int i = 0; i < aNbPoles; ++i)
    {
      const double  aWi = aWeightsData[i];
      const double  aBd = aBasisData[i];
      const gp_XYZ& aPi = aPolesData[i].XYZ();
      aCwd += (aWi * aBd) * aPi;
      aWd += aWi * aBd;
    }
    aWDerivs.SetValue(d, aWd);
    aCwDerivs.SetValue(d, aCwd);
  }

  // Compute C^(d) for d = 0..N using the recurrence.
  NCollection_Array1<gp_XYZ> aCDerivs(0, N);
  aCDerivs.SetValue(0, aCwDerivs.Value(0) / aWDerivs.Value(0));

  for (int d = 1; d <= N; ++d)
  {
    gp_XYZ aSum(0.0, 0.0, 0.0);
    // Binomial coefficients via incremental computation.
    double aBinom = 1.0;
    for (int k = 1; k <= d; ++k)
    {
      aBinom *= static_cast<double>(d - k + 1) / static_cast<double>(k);
      aSum += aBinom * aWDerivs.Value(k) * aCDerivs.Value(d - k);
    }
    aCDerivs.SetValue(d, (aCwDerivs.Value(d) - aSum) / aWDerivs.Value(0));
  }

  return gp_Vec(aCDerivs.Value(N));
}

//==================================================================================================

void GeomEval_TBezierCurve::Transform(const gp_Trsf& T)
{
  (void)T;
  throw Standard_NotImplemented("GeomEval_TBezierCurve::Transform");
}

//==================================================================================================

occ::handle<Geom_Geometry> GeomEval_TBezierCurve::Copy() const
{
  if (myRational)
  {
    return new GeomEval_TBezierCurve(myPoles, myWeights, myAlpha);
  }
  return new GeomEval_TBezierCurve(myPoles, myAlpha);
}

//==================================================================================================

void GeomEval_TBezierCurve::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_BoundedCurve)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myAlpha)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myRational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPoles.Size())
}
