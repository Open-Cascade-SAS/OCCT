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

#include <Geom2dEval_AHTBezierCurve.hxx>

#include <NCollection_LocalArray.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Dump.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2dEval_AHTBezierCurve, Geom2d_BoundedCurve)

namespace
{
struct PolyEvalData
{
  gp_XY  N0;
  gp_XY  N1;
  gp_XY  N2;
  gp_XY  N3;
  double W0 = 0.0;
  double W1 = 0.0;
  double W2 = 0.0;
  double W3 = 0.0;
};

double powInt(const double theBase, const int theExp)
{
  double aRes = 1.0;
  for (int i = 0; i < theExp; ++i)
  {
    aRes *= theBase;
  }
  return aRes;
}

double fallingFactorial(const int theN, const int theK)
{
  double aRes = 1.0;
  for (int i = 0; i < theK; ++i)
  {
    aRes *= double(theN - i);
  }
  return aRes;
}

template <int theMaxOrder>
void evalPolynomialNonRational(const NCollection_Array1<gp_Pnt2d>& thePoles,
                               const int                           theDegree,
                               const double                        theU,
                               PolyEvalData&                       theData)
{
  theData                = PolyEvalData();
  const gp_Pnt2d* aPoles = &thePoles.Value(thePoles.Lower());
  theData.N0             = aPoles[theDegree].XY();

  for (int k = theDegree - 1; k >= 0; --k)
  {
    if constexpr (theMaxOrder >= 3)
    {
      theData.N3 = theData.N3 * theU + theData.N2 * 3.0;
    }
    if constexpr (theMaxOrder >= 2)
    {
      theData.N2 = theData.N2 * theU + theData.N1 * 2.0;
    }
    if constexpr (theMaxOrder >= 1)
    {
      theData.N1 = theData.N1 * theU + theData.N0;
    }
    theData.N0 = theData.N0 * theU + aPoles[k].XY();
  }
}

template <int theMaxOrder>
void evalPolynomialRational(const NCollection_Array1<gp_Pnt2d>& thePoles,
                            const NCollection_Array1<double>&   theWeights,
                            const int                           theDegree,
                            const double                        theU,
                            PolyEvalData&                       theData)
{
  theData                  = PolyEvalData();
  const gp_Pnt2d* aPoles   = &thePoles.Value(thePoles.Lower());
  const double*   aWeights = &theWeights.Value(theWeights.Lower());

  const double aWLead = aWeights[theDegree];
  theData.N0          = aPoles[theDegree].XY() * aWLead;
  theData.W0          = aWLead;

  for (int k = theDegree - 1; k >= 0; --k)
  {
    if constexpr (theMaxOrder >= 3)
    {
      theData.N3 = theData.N3 * theU + theData.N2 * 3.0;
      theData.W3 = theData.W3 * theU + theData.W2 * 3.0;
    }
    if constexpr (theMaxOrder >= 2)
    {
      theData.N2 = theData.N2 * theU + theData.N1 * 2.0;
      theData.W2 = theData.W2 * theU + theData.W1 * 2.0;
    }
    if constexpr (theMaxOrder >= 1)
    {
      theData.N1 = theData.N1 * theU + theData.N0;
      theData.W1 = theData.W1 * theU + theData.W0;
    }

    const double aWk = aWeights[k];
    theData.N0       = theData.N0 * theU + aPoles[k].XY() * aWk;
    theData.W0       = theData.W0 * theU + aWk;
  }
}

template <int theMaxOrder, bool theIsRational>
void evalMixedAHT(const NCollection_Array1<gp_Pnt2d>& thePoles,
                  const NCollection_Array1<double>&   theWeights,
                  const int                           theAlgDegree,
                  const double                        theAlpha,
                  const double                        theBeta,
                  const double                        theU,
                  PolyEvalData&                       theData)
{
  theData                                 = PolyEvalData();
  const gp_Pnt2d*                aPoles   = &thePoles.Value(thePoles.Lower());
  [[maybe_unused]] const double* aWeights = nullptr;
  if constexpr (theIsRational)
  {
    aWeights = &theWeights.Value(theWeights.Lower());
  }
  int aPoleIdx   = 0;
  int aWeightIdx = 0;

  double aPow   = 1.0;
  double aPowM1 = 0.0;
  double aPowM2 = 0.0;
  double aPowM3 = 0.0;
  for (int k = 0; k <= theAlgDegree; ++k)
  {
    const gp_XY& aPole = aPoles[aPoleIdx++].XY();
    const double aB0   = aPow;
    double       aB1   = 0.0;
    double       aB2   = 0.0;
    double       aB3   = 0.0;
    if constexpr (theMaxOrder >= 1)
    {
      aB1 = (k >= 1) ? double(k) * aPowM1 : 0.0;
    }
    if constexpr (theMaxOrder >= 2)
    {
      aB2 = (k >= 2) ? double(k) * double(k - 1) * aPowM2 : 0.0;
    }
    if constexpr (theMaxOrder >= 3)
    {
      aB3 = (k >= 3) ? double(k) * double(k - 1) * double(k - 2) * aPowM3 : 0.0;
    }

    if constexpr (theIsRational)
    {
      const double aWi = aWeights[aWeightIdx++];
      theData.N0 += aPole * (aWi * aB0);
      theData.W0 += aWi * aB0;
      if constexpr (theMaxOrder >= 1)
      {
        theData.N1 += aPole * (aWi * aB1);
        theData.W1 += aWi * aB1;
      }
      if constexpr (theMaxOrder >= 2)
      {
        theData.N2 += aPole * (aWi * aB2);
        theData.W2 += aWi * aB2;
      }
      if constexpr (theMaxOrder >= 3)
      {
        theData.N3 += aPole * (aWi * aB3);
        theData.W3 += aWi * aB3;
      }
    }
    else
    {
      theData.N0 += aPole * aB0;
      if constexpr (theMaxOrder >= 1)
      {
        theData.N1 += aPole * aB1;
      }
      if constexpr (theMaxOrder >= 2)
      {
        theData.N2 += aPole * aB2;
      }
      if constexpr (theMaxOrder >= 3)
      {
        theData.N3 += aPole * aB3;
      }
    }

    aPowM3 = aPowM2;
    aPowM2 = aPowM1;
    aPowM1 = aPow;
    aPow *= theU;
  }

  if (theAlpha > 0.0)
  {
    const double aAlphaU = theAlpha * theU;
    const double aSinh   = std::sinh(aAlphaU);
    const double aCosh   = std::cosh(aAlphaU);
    const double aAlpha2 = theAlpha * theAlpha;
    const double aAlpha3 = aAlpha2 * theAlpha;

    const gp_XY& aPoleSinh = aPoles[aPoleIdx++].XY();
    const gp_XY& aPoleCosh = aPoles[aPoleIdx++].XY();
    double       aWiSinh   = 1.0;
    double       aWiCosh   = 1.0;
    if constexpr (theIsRational)
    {
      aWiSinh = aWeights[aWeightIdx++];
      aWiCosh = aWeights[aWeightIdx++];
    }

    const double aB0Sinh = aSinh;
    const double aB0Cosh = aCosh;
    double       aB1Sinh = 0.0;
    double       aB1Cosh = 0.0;
    double       aB2Sinh = 0.0;
    double       aB2Cosh = 0.0;
    double       aB3Sinh = 0.0;
    double       aB3Cosh = 0.0;
    if constexpr (theMaxOrder >= 1)
    {
      aB1Sinh = theAlpha * aCosh;
      aB1Cosh = theAlpha * aSinh;
    }
    if constexpr (theMaxOrder >= 2)
    {
      aB2Sinh = aAlpha2 * aSinh;
      aB2Cosh = aAlpha2 * aCosh;
    }
    if constexpr (theMaxOrder >= 3)
    {
      aB3Sinh = aAlpha3 * aCosh;
      aB3Cosh = aAlpha3 * aSinh;
    }

    theData.N0 += aPoleSinh * (aWiSinh * aB0Sinh) + aPoleCosh * (aWiCosh * aB0Cosh);
    if constexpr (theIsRational)
    {
      theData.W0 += aWiSinh * aB0Sinh + aWiCosh * aB0Cosh;
    }
    if constexpr (theMaxOrder >= 1)
    {
      theData.N1 += aPoleSinh * (aWiSinh * aB1Sinh) + aPoleCosh * (aWiCosh * aB1Cosh);
      if constexpr (theIsRational)
      {
        theData.W1 += aWiSinh * aB1Sinh + aWiCosh * aB1Cosh;
      }
    }
    if constexpr (theMaxOrder >= 2)
    {
      theData.N2 += aPoleSinh * (aWiSinh * aB2Sinh) + aPoleCosh * (aWiCosh * aB2Cosh);
      if constexpr (theIsRational)
      {
        theData.W2 += aWiSinh * aB2Sinh + aWiCosh * aB2Cosh;
      }
    }
    if constexpr (theMaxOrder >= 3)
    {
      theData.N3 += aPoleSinh * (aWiSinh * aB3Sinh) + aPoleCosh * (aWiCosh * aB3Cosh);
      if constexpr (theIsRational)
      {
        theData.W3 += aWiSinh * aB3Sinh + aWiCosh * aB3Cosh;
      }
    }
  }

  if (theBeta > 0.0)
  {
    const double aBetaU = theBeta * theU;
    const double aSin   = std::sin(aBetaU);
    const double aCos   = std::cos(aBetaU);
    const double aBeta2 = theBeta * theBeta;
    const double aBeta3 = aBeta2 * theBeta;

    const gp_XY& aPoleSin = aPoles[aPoleIdx++].XY();
    const gp_XY& aPoleCos = aPoles[aPoleIdx++].XY();
    double       aWiSin   = 1.0;
    double       aWiCos   = 1.0;
    if constexpr (theIsRational)
    {
      aWiSin = aWeights[aWeightIdx++];
      aWiCos = aWeights[aWeightIdx++];
    }

    const double aB0Sin = aSin;
    const double aB0Cos = aCos;
    double       aB1Sin = 0.0;
    double       aB1Cos = 0.0;
    double       aB2Sin = 0.0;
    double       aB2Cos = 0.0;
    double       aB3Sin = 0.0;
    double       aB3Cos = 0.0;
    if constexpr (theMaxOrder >= 1)
    {
      aB1Sin = theBeta * aCos;
      aB1Cos = -theBeta * aSin;
    }
    if constexpr (theMaxOrder >= 2)
    {
      aB2Sin = -aBeta2 * aSin;
      aB2Cos = -aBeta2 * aCos;
    }
    if constexpr (theMaxOrder >= 3)
    {
      aB3Sin = -aBeta3 * aCos;
      aB3Cos = aBeta3 * aSin;
    }

    theData.N0 += aPoleSin * (aWiSin * aB0Sin) + aPoleCos * (aWiCos * aB0Cos);
    if constexpr (theIsRational)
    {
      theData.W0 += aWiSin * aB0Sin + aWiCos * aB0Cos;
    }
    if constexpr (theMaxOrder >= 1)
    {
      theData.N1 += aPoleSin * (aWiSin * aB1Sin) + aPoleCos * (aWiCos * aB1Cos);
      if constexpr (theIsRational)
      {
        theData.W1 += aWiSin * aB1Sin + aWiCos * aB1Cos;
      }
    }
    if constexpr (theMaxOrder >= 2)
    {
      theData.N2 += aPoleSin * (aWiSin * aB2Sin) + aPoleCos * (aWiCos * aB2Cos);
      if constexpr (theIsRational)
      {
        theData.W2 += aWiSin * aB2Sin + aWiCos * aB2Cos;
      }
    }
    if constexpr (theMaxOrder >= 3)
    {
      theData.N3 += aPoleSin * (aWiSin * aB3Sin) + aPoleCos * (aWiCos * aB3Cos);
      if constexpr (theIsRational)
      {
        theData.W3 += aWiSin * aB3Sin + aWiCos * aB3Cos;
      }
    }
  }
}
} // namespace

//==================================================================================================

int Geom2dEval_AHTBezierCurve::basisDimension(int theAlgDegree, double theAlpha, double theBeta)
{
  return theAlgDegree + 1 + (theAlpha > 0.0 ? 2 : 0) + (theBeta > 0.0 ? 2 : 0);
}

//==================================================================================================

Geom2dEval_AHTBezierCurve::Geom2dEval_AHTBezierCurve(const NCollection_Array1<gp_Pnt2d>& thePoles,
                                                     int    theAlgDegree,
                                                     double theAlpha,
                                                     double theBeta)
    : myPoles(thePoles),
      myWeights(1, 1),
      myAlgDegree(theAlgDegree),
      myAlpha(theAlpha),
      myBeta(theBeta),
      myRational(false)
{
  if (theAlgDegree < 0)
  {
    throw Standard_ConstructionError("Geom2dEval_AHTBezierCurve: algebraic degree must be >= 0");
  }
  if (theAlpha < 0.0)
  {
    throw Standard_ConstructionError("Geom2dEval_AHTBezierCurve: alpha must be >= 0");
  }
  if (theBeta < 0.0)
  {
    throw Standard_ConstructionError("Geom2dEval_AHTBezierCurve: beta must be >= 0");
  }
  const int aDim = basisDimension(theAlgDegree, theAlpha, theBeta);
  if (thePoles.Size() != aDim)
  {
    throw Standard_ConstructionError(
      "Geom2dEval_AHTBezierCurve: NbPoles must equal basisDimension");
  }
}

//==================================================================================================

Geom2dEval_AHTBezierCurve::Geom2dEval_AHTBezierCurve(const NCollection_Array1<gp_Pnt2d>& thePoles,
                                                     const NCollection_Array1<double>&   theWeights,
                                                     int    theAlgDegree,
                                                     double theAlpha,
                                                     double theBeta)
    : myPoles(thePoles),
      myWeights(theWeights),
      myAlgDegree(theAlgDegree),
      myAlpha(theAlpha),
      myBeta(theBeta),
      myRational(true)
{
  if (theAlgDegree < 0)
  {
    throw Standard_ConstructionError("Geom2dEval_AHTBezierCurve: algebraic degree must be >= 0");
  }
  if (theAlpha < 0.0)
  {
    throw Standard_ConstructionError("Geom2dEval_AHTBezierCurve: alpha must be >= 0");
  }
  if (theBeta < 0.0)
  {
    throw Standard_ConstructionError("Geom2dEval_AHTBezierCurve: beta must be >= 0");
  }
  const int aDim = basisDimension(theAlgDegree, theAlpha, theBeta);
  if (thePoles.Size() != aDim)
  {
    throw Standard_ConstructionError(
      "Geom2dEval_AHTBezierCurve: NbPoles must equal basisDimension");
  }
  if (theWeights.Size() != thePoles.Size())
  {
    throw Standard_ConstructionError(
      "Geom2dEval_AHTBezierCurve: weights array size must match poles array size");
  }
  for (int anIdx = theWeights.Lower(); anIdx <= theWeights.Upper(); ++anIdx)
  {
    if (theWeights.Value(anIdx) <= 0.0)
    {
      throw Standard_ConstructionError("Geom2dEval_AHTBezierCurve: all weights must be > 0");
    }
  }
}

//==================================================================================================

const NCollection_Array1<gp_Pnt2d>& Geom2dEval_AHTBezierCurve::Poles() const
{
  return myPoles;
}

//==================================================================================================

const NCollection_Array1<double>& Geom2dEval_AHTBezierCurve::Weights() const
{
  return myWeights;
}

//==================================================================================================

int Geom2dEval_AHTBezierCurve::AlgDegree() const
{
  return myAlgDegree;
}

//==================================================================================================

double Geom2dEval_AHTBezierCurve::Alpha() const
{
  return myAlpha;
}

//==================================================================================================

double Geom2dEval_AHTBezierCurve::Beta() const
{
  return myBeta;
}

//==================================================================================================

int Geom2dEval_AHTBezierCurve::NbPoles() const
{
  return myPoles.Size();
}

//==================================================================================================

bool Geom2dEval_AHTBezierCurve::IsRational() const
{
  return myRational;
}

//==================================================================================================

gp_Pnt2d Geom2dEval_AHTBezierCurve::StartPoint() const
{
  return EvalD0(0.0);
}

//==================================================================================================

gp_Pnt2d Geom2dEval_AHTBezierCurve::EndPoint() const
{
  return EvalD0(1.0);
}

//==================================================================================================

void Geom2dEval_AHTBezierCurve::Reverse()
{
  throw Standard_NotImplemented("Geom2dEval_AHTBezierCurve::Reverse");
}

//==================================================================================================

double Geom2dEval_AHTBezierCurve::ReversedParameter(const double U) const
{
  (void)U;
  throw Standard_NotImplemented("Geom2dEval_AHTBezierCurve::ReversedParameter");
}

//==================================================================================================

double Geom2dEval_AHTBezierCurve::FirstParameter() const
{
  return 0.0;
}

//==================================================================================================

double Geom2dEval_AHTBezierCurve::LastParameter() const
{
  return 1.0;
}

//==================================================================================================

bool Geom2dEval_AHTBezierCurve::IsClosed() const
{
  return StartPoint().Distance(EndPoint()) <= Precision::Confusion();
}

//==================================================================================================

bool Geom2dEval_AHTBezierCurve::IsPeriodic() const
{
  return false;
}

//==================================================================================================

GeomAbs_Shape Geom2dEval_AHTBezierCurve::Continuity() const
{
  return GeomAbs_CN;
}

//==================================================================================================

bool Geom2dEval_AHTBezierCurve::IsCN(const int /*N*/) const
{
  return true;
}

//==================================================================================================

void Geom2dEval_AHTBezierCurve::evalBasis(double theT, NCollection_Array1<double>& theBasis) const
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
    const double aAlphaT          = myAlpha * theT;
    theBasis.ChangeValue(anIdx++) = std::sinh(aAlphaT);
    theBasis.ChangeValue(anIdx++) = std::cosh(aAlphaT);
  }

  // Trigonometric part: sin(beta*t), cos(beta*t)
  if (myBeta > 0.0)
  {
    const double aBetaT           = myBeta * theT;
    theBasis.ChangeValue(anIdx++) = std::sin(aBetaT);
    theBasis.ChangeValue(anIdx++) = std::cos(aBetaT);
  }
}

//==================================================================================================

void Geom2dEval_AHTBezierCurve::evalBasisDeriv(double                      theT,
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
    const double aAlphaT     = myAlpha * theT;
    const double aAlphaPow   = std::pow(myAlpha, theDerivOrder);
    const double aSinh       = std::sinh(aAlphaT);
    const double aCosh       = std::cosh(aAlphaT);
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

gp_Pnt2d Geom2dEval_AHTBezierCurve::EvalD0(const double U) const
{
  if (myAlpha <= 0.0 && myBeta <= 0.0)
  {
    PolyEvalData aPoly;
    if (!myRational)
    {
      evalPolynomialNonRational<0>(myPoles, myAlgDegree, U, aPoly);
      return gp_Pnt2d(aPoly.N0);
    }

    evalPolynomialRational<0>(myPoles, myWeights, myAlgDegree, U, aPoly);
    return gp_Pnt2d(aPoly.N0 / aPoly.W0);
  }

  PolyEvalData aMixed;
  if (!myRational)
  {
    evalMixedAHT<0, false>(myPoles, myWeights, myAlgDegree, myAlpha, myBeta, U, aMixed);
    return gp_Pnt2d(aMixed.N0);
  }

  evalMixedAHT<0, true>(myPoles, myWeights, myAlgDegree, myAlpha, myBeta, U, aMixed);
  return gp_Pnt2d(aMixed.N0 / aMixed.W0);
}

//==================================================================================================

Geom2d_Curve::ResD1 Geom2dEval_AHTBezierCurve::EvalD1(const double U) const
{
  if (myAlpha <= 0.0 && myBeta <= 0.0)
  {
    PolyEvalData        aPoly;
    Geom2d_Curve::ResD1 aResult;
    if (!myRational)
    {
      evalPolynomialNonRational<1>(myPoles, myAlgDegree, U, aPoly);
      aResult.Point = gp_Pnt2d(aPoly.N0);
      aResult.D1    = gp_Vec2d(aPoly.N1);
      return aResult;
    }

    evalPolynomialRational<1>(myPoles, myWeights, myAlgDegree, U, aPoly);
    const double aInvW = 1.0 / aPoly.W0;
    aResult.Point      = gp_Pnt2d(aPoly.N0 * aInvW);
    aResult.D1         = gp_Vec2d((aPoly.N1 * aPoly.W0 - aPoly.N0 * aPoly.W1) * (aInvW * aInvW));
    return aResult;
  }

  Geom2d_Curve::ResD1 aResult;
  PolyEvalData        aMixed;
  if (!myRational)
  {
    evalMixedAHT<1, false>(myPoles, myWeights, myAlgDegree, myAlpha, myBeta, U, aMixed);
    aResult.Point = gp_Pnt2d(aMixed.N0);
    aResult.D1    = gp_Vec2d(aMixed.N1);
    return aResult;
  }

  evalMixedAHT<1, true>(myPoles, myWeights, myAlgDegree, myAlpha, myBeta, U, aMixed);
  const double aInvW = 1.0 / aMixed.W0;
  aResult.Point      = gp_Pnt2d(aMixed.N0 * aInvW);
  aResult.D1         = gp_Vec2d((aMixed.N1 * aMixed.W0 - aMixed.N0 * aMixed.W1) * (aInvW * aInvW));
  return aResult;
}

//==================================================================================================

Geom2d_Curve::ResD2 Geom2dEval_AHTBezierCurve::EvalD2(const double U) const
{
  if (myAlpha <= 0.0 && myBeta <= 0.0)
  {
    PolyEvalData        aPoly;
    Geom2d_Curve::ResD2 aResult;
    if (!myRational)
    {
      evalPolynomialNonRational<2>(myPoles, myAlgDegree, U, aPoly);
      aResult.Point = gp_Pnt2d(aPoly.N0);
      aResult.D1    = gp_Vec2d(aPoly.N1);
      aResult.D2    = gp_Vec2d(aPoly.N2);
      return aResult;
    }

    evalPolynomialRational<2>(myPoles, myWeights, myAlgDegree, U, aPoly);
    const double aInvW = 1.0 / aPoly.W0;
    const gp_XY  aC    = aPoly.N0 * aInvW;
    const gp_XY  aCD1  = (aPoly.N1 * aPoly.W0 - aPoly.N0 * aPoly.W1) * (aInvW * aInvW);
    aResult.Point      = gp_Pnt2d(aC);
    aResult.D1         = gp_Vec2d(aCD1);
    aResult.D2         = gp_Vec2d((aPoly.N2 - aCD1 * (2.0 * aPoly.W1) - aC * aPoly.W2) * aInvW);
    return aResult;
  }

  Geom2d_Curve::ResD2 aResult;
  PolyEvalData        aMixed;
  if (!myRational)
  {
    evalMixedAHT<2, false>(myPoles, myWeights, myAlgDegree, myAlpha, myBeta, U, aMixed);
    aResult.Point = gp_Pnt2d(aMixed.N0);
    aResult.D1    = gp_Vec2d(aMixed.N1);
    aResult.D2    = gp_Vec2d(aMixed.N2);
    return aResult;
  }

  evalMixedAHT<2, true>(myPoles, myWeights, myAlgDegree, myAlpha, myBeta, U, aMixed);
  const double aInvW = 1.0 / aMixed.W0;
  const gp_XY  aC    = aMixed.N0 * aInvW;
  const gp_XY  aCD1  = (aMixed.N1 * aMixed.W0 - aMixed.N0 * aMixed.W1) * (aInvW * aInvW);

  aResult.Point = gp_Pnt2d(aC);
  aResult.D1    = gp_Vec2d(aCD1);
  aResult.D2    = gp_Vec2d((aMixed.N2 - aCD1 * (2.0 * aMixed.W1) - aC * aMixed.W2) * aInvW);
  return aResult;
}

//==================================================================================================

Geom2d_Curve::ResD3 Geom2dEval_AHTBezierCurve::EvalD3(const double U) const
{
  if (myAlpha <= 0.0 && myBeta <= 0.0)
  {
    PolyEvalData        aPoly;
    Geom2d_Curve::ResD3 aResult;
    if (!myRational)
    {
      evalPolynomialNonRational<3>(myPoles, myAlgDegree, U, aPoly);
      aResult.Point = gp_Pnt2d(aPoly.N0);
      aResult.D1    = gp_Vec2d(aPoly.N1);
      aResult.D2    = gp_Vec2d(aPoly.N2);
      aResult.D3    = gp_Vec2d(aPoly.N3);
      return aResult;
    }

    evalPolynomialRational<3>(myPoles, myWeights, myAlgDegree, U, aPoly);
    const double aInvW = 1.0 / aPoly.W0;
    const gp_XY  aC    = aPoly.N0 * aInvW;
    const gp_XY  aCD1  = (aPoly.N1 - aC * aPoly.W1) * aInvW;
    const gp_XY  aCD2  = (aPoly.N2 - aCD1 * (2.0 * aPoly.W1) - aC * aPoly.W2) * aInvW;
    const gp_XY  aCD3 =
      (aPoly.N3 - aCD2 * (3.0 * aPoly.W1) - aCD1 * (3.0 * aPoly.W2) - aC * aPoly.W3) * aInvW;

    aResult.Point = gp_Pnt2d(aC);
    aResult.D1    = gp_Vec2d(aCD1);
    aResult.D2    = gp_Vec2d(aCD2);
    aResult.D3    = gp_Vec2d(aCD3);
    return aResult;
  }

  Geom2d_Curve::ResD3 aResult;
  PolyEvalData        aMixed;
  if (!myRational)
  {
    evalMixedAHT<3, false>(myPoles, myWeights, myAlgDegree, myAlpha, myBeta, U, aMixed);
    aResult.Point = gp_Pnt2d(aMixed.N0);
    aResult.D1    = gp_Vec2d(aMixed.N1);
    aResult.D2    = gp_Vec2d(aMixed.N2);
    aResult.D3    = gp_Vec2d(aMixed.N3);
    return aResult;
  }

  evalMixedAHT<3, true>(myPoles, myWeights, myAlgDegree, myAlpha, myBeta, U, aMixed);
  const double aInvW = 1.0 / aMixed.W0;
  const gp_XY  aC    = aMixed.N0 * aInvW;
  const gp_XY  aCD1  = (aMixed.N1 - aC * aMixed.W1) * aInvW;
  const gp_XY  aCD2  = (aMixed.N2 - aCD1 * (2.0 * aMixed.W1) - aC * aMixed.W2) * aInvW;
  const gp_XY  aCD3 =
    (aMixed.N3 - aCD2 * (3.0 * aMixed.W1) - aCD1 * (3.0 * aMixed.W2) - aC * aMixed.W3) * aInvW;

  aResult.Point = gp_Pnt2d(aC);
  aResult.D1    = gp_Vec2d(aCD1);
  aResult.D2    = gp_Vec2d(aCD2);
  aResult.D3    = gp_Vec2d(aCD3);
  return aResult;
}

//==================================================================================================

gp_Vec2d Geom2dEval_AHTBezierCurve::EvalDN(const double U, const int N) const
{
  if (N < 1)
  {
    throw Standard_RangeError("Geom2dEval_AHTBezierCurve::EvalDN: N must be >= 1");
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

  const int aPoleLow = myPoles.Lower();

  if (!myRational)
  {
    int   aPoleIdx = aPoleLow;
    gp_XY aSum(0.0, 0.0);

    for (int k = 0; k <= myAlgDegree; ++k)
    {
      double aBasisN = 0.0;
      if (N <= k)
      {
        if (U == 0.0)
        {
          aBasisN = (N == k) ? fallingFactorial(k, N) : 0.0;
        }
        else
        {
          aBasisN = fallingFactorial(k, N) * powInt(U, k - N);
        }
      }
      aSum += myPoles.Value(aPoleIdx++).XY() * aBasisN;
    }

    if (myAlpha > 0.0)
    {
      const double aAlphaU   = myAlpha * U;
      const double aAlphaPow = powInt(myAlpha, N);
      const double aSinh     = std::sinh(aAlphaU);
      const double aCosh     = std::cosh(aAlphaU);
      const bool   isEvenN   = (N % 2 == 0);
      const double aSinhN    = aAlphaPow * (isEvenN ? aSinh : aCosh);
      const double aCoshN    = aAlphaPow * (isEvenN ? aCosh : aSinh);
      aSum += myPoles.Value(aPoleIdx++).XY() * aSinhN;
      aSum += myPoles.Value(aPoleIdx++).XY() * aCoshN;
    }

    if (myBeta > 0.0)
    {
      const double aBetaU   = myBeta * U;
      const double aBetaPow = powInt(myBeta, N);
      const double aSin     = std::sin(aBetaU);
      const double aCos     = std::cos(aBetaU);
      double       aSinN    = 0.0;
      double       aCosN    = 0.0;
      switch (N & 3)
      {
        case 0:
          aSinN = aSin;
          aCosN = aCos;
          break;
        case 1:
          aSinN = aCos;
          aCosN = -aSin;
          break;
        case 2:
          aSinN = -aSin;
          aCosN = -aCos;
          break;
        case 3:
          aSinN = -aCos;
          aCosN = aSin;
          break;
      }
      aSum += myPoles.Value(aPoleIdx++).XY() * (aBetaPow * aSinN);
      aSum += myPoles.Value(aPoleIdx++).XY() * (aBetaPow * aCosN);
    }

    return gp_Vec2d(aSum);
  }

  NCollection_LocalArray<double, 16> aWDerivs(N + 1);
  NCollection_LocalArray<gp_XY, 16>  aCDerivs(N + 1);

  const int aWeightLow = myWeights.Lower();
  const int aPolyCount = myAlgDegree + 1;
  const int aHyperIdx  = aPoleLow + aPolyCount;
  const int aTrigIdx   = aHyperIdx + (myAlpha > 0.0 ? 2 : 0);

  const gp_XY* aPoleSinh = nullptr;
  const gp_XY* aPoleCosh = nullptr;
  const gp_XY* aPoleSin  = nullptr;
  const gp_XY* aPoleCos  = nullptr;
  double       aWiSinh   = 0.0;
  double       aWiCosh   = 0.0;
  double       aWiSin    = 0.0;
  double       aWiCos    = 0.0;

  if (myAlpha > 0.0)
  {
    aPoleSinh = &myPoles.Value(aHyperIdx).XY();
    aPoleCosh = &myPoles.Value(aHyperIdx + 1).XY();
    aWiSinh   = myWeights.Value(aWeightLow + aPolyCount);
    aWiCosh   = myWeights.Value(aWeightLow + aPolyCount + 1);
  }

  if (myBeta > 0.0)
  {
    const int aTrigWIdx = aWeightLow + aPolyCount + (myAlpha > 0.0 ? 2 : 0);
    aPoleSin            = &myPoles.Value(aTrigIdx).XY();
    aPoleCos            = &myPoles.Value(aTrigIdx + 1).XY();
    aWiSin              = myWeights.Value(aTrigWIdx);
    aWiCos              = myWeights.Value(aTrigWIdx + 1);
  }

  const double aAlphaU = myAlpha * U;
  const double aSinh   = (myAlpha > 0.0) ? std::sinh(aAlphaU) : 0.0;
  const double aCosh   = (myAlpha > 0.0) ? std::cosh(aAlphaU) : 0.0;
  const double aBetaU  = myBeta * U;
  const double aSin    = (myBeta > 0.0) ? std::sin(aBetaU) : 0.0;
  const double aCos    = (myBeta > 0.0) ? std::cos(aBetaU) : 0.0;

  double aAlphaPow = 1.0;
  double aBetaPow  = 1.0;
  double aInvW0    = 0.0;
  for (int d = 0; d <= N; ++d)
  {
    gp_XY  aNd(0.0, 0.0);
    double aWd = 0.0;

    for (int k = 0; k <= myAlgDegree; ++k)
    {
      const gp_XY& aPole   = myPoles.Value(aPoleLow + k).XY();
      const double aWi     = myWeights.Value(aWeightLow + k);
      double       aBasisD = 0.0;
      if (d <= k)
      {
        if (U == 0.0)
        {
          aBasisD = (d == k) ? fallingFactorial(k, d) : 0.0;
        }
        else
        {
          aBasisD = fallingFactorial(k, d) * powInt(U, k - d);
        }
      }

      aNd += aPole * (aWi * aBasisD);
      aWd += aWi * aBasisD;
    }

    if (myAlpha > 0.0)
    {
      const bool   isEvenD = (d % 2 == 0);
      const double aDSinh  = aAlphaPow * (isEvenD ? aSinh : aCosh);
      const double aDCosh  = aAlphaPow * (isEvenD ? aCosh : aSinh);
      aNd += (*aPoleSinh) * (aWiSinh * aDSinh) + (*aPoleCosh) * (aWiCosh * aDCosh);
      aWd += aWiSinh * aDSinh + aWiCosh * aDCosh;
    }

    if (myBeta > 0.0)
    {
      double aDSin = 0.0;
      double aDCos = 0.0;
      switch (d & 3)
      {
        case 0:
          aDSin = aSin;
          aDCos = aCos;
          break;
        case 1:
          aDSin = aCos;
          aDCos = -aSin;
          break;
        case 2:
          aDSin = -aSin;
          aDCos = -aCos;
          break;
        case 3:
          aDSin = -aCos;
          aDCos = aSin;
          break;
      }
      aNd += (*aPoleSin) * (aWiSin * aBetaPow * aDSin) + (*aPoleCos) * (aWiCos * aBetaPow * aDCos);
      aWd += aWiSin * aBetaPow * aDSin + aWiCos * aBetaPow * aDCos;
    }

    aWDerivs[d] = aWd;
    if (d == 0)
    {
      aInvW0      = 1.0 / aWd;
      aCDerivs[0] = aNd * aInvW0;
    }
    else
    {
      gp_XY  aSum   = aNd;
      double aBinom = 1.0;
      for (int j = 1; j <= d; ++j)
      {
        aBinom = aBinom * double(d - j + 1) / double(j);
        aSum -= aCDerivs[d - j] * (aBinom * aWDerivs[j]);
      }
      aCDerivs[d] = aSum * aInvW0;
    }

    aAlphaPow *= myAlpha;
    aBetaPow *= myBeta;
  }
  return gp_Vec2d(aCDerivs[N]);
}

//==================================================================================================

void Geom2dEval_AHTBezierCurve::Transform(const gp_Trsf2d& T)
{
  (void)T;
  throw Standard_NotImplemented("Geom2dEval_AHTBezierCurve::Transform");
}

//==================================================================================================

occ::handle<Geom2d_Geometry> Geom2dEval_AHTBezierCurve::Copy() const
{
  if (myRational)
  {
    return new Geom2dEval_AHTBezierCurve(myPoles, myWeights, myAlgDegree, myAlpha, myBeta);
  }
  return new Geom2dEval_AHTBezierCurve(myPoles, myAlgDegree, myAlpha, myBeta);
}

//==================================================================================================

void Geom2dEval_AHTBezierCurve::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom2d_BoundedCurve)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myAlgDegree)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myAlpha)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myBeta)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myRational)

  for (int i = myPoles.Lower(); i <= myPoles.Upper(); ++i)
  {
    gp_Pnt2d aPole = myPoles.Value(i);
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &aPole)
  }
}
