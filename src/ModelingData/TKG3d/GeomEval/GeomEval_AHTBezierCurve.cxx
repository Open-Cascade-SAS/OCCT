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

#include <NCollection_LocalArray.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomEval_AHTBezierCurve, Geom_BoundedCurve)

namespace
{

struct PolyEvalData
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
void evalPolynomialNonRational(const NCollection_Array1<gp_Pnt>& thePoles,
                               const int                          theDegree,
                               const double                       theU,
                               PolyEvalData&                      theData)
{
  theData = PolyEvalData();
  const int aPoleLow = thePoles.Lower();
  theData.N0 = thePoles.Value(aPoleLow + theDegree).XYZ();

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
    theData.N0 = theData.N0 * theU + thePoles.Value(aPoleLow + k).XYZ();
  }
}

template <int theMaxOrder>
void evalPolynomialRational(const NCollection_Array1<gp_Pnt>&   thePoles,
                            const NCollection_Array1<double>& theWeights,
                            const int                          theDegree,
                            const double                       theU,
                            PolyEvalData&                      theData)
{
  theData = PolyEvalData();
  const int aPoleLow   = thePoles.Lower();
  const int aWeightLow = theWeights.Lower();

  const double aWLead = theWeights.Value(aWeightLow + theDegree);
  theData.N0 = thePoles.Value(aPoleLow + theDegree).XYZ() * aWLead;
  theData.W0 = aWLead;

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

    const double aWk = theWeights.Value(aWeightLow + k);
    theData.N0 = theData.N0 * theU + thePoles.Value(aPoleLow + k).XYZ() * aWk;
    theData.W0 = theData.W0 * theU + aWk;
  }
}

template <int theMaxOrder, bool theIsRational>
void evalMixedAHT(const NCollection_Array1<gp_Pnt>&   thePoles,
                  const NCollection_Array1<double>& theWeights,
                  const int                          theAlgDegree,
                  const double                       theAlpha,
                  const double                       theBeta,
                  const double                       theU,
                  PolyEvalData&                      theData)
{
  theData = PolyEvalData();

  int aPoleIdx   = thePoles.Lower();
  int aWeightIdx = theWeights.Lower();

  // Polynomial part: {1, t, ..., t^k}.
  double aPow = 1.0;
  double aPowM1 = 0.0;
  double aPowM2 = 0.0;
  double aPowM3 = 0.0;
  for (int k = 0; k <= theAlgDegree; ++k)
  {
    const gp_XYZ& aPole = thePoles.Value(aPoleIdx++).XYZ();
    const double  aB0   = aPow;
    double        aB1   = 0.0;
    double        aB2   = 0.0;
    double        aB3   = 0.0;
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
      const double aWi = theWeights.Value(aWeightIdx++);
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

    const gp_XYZ& aPoleSinh = thePoles.Value(aPoleIdx++).XYZ();
    const gp_XYZ& aPoleCosh = thePoles.Value(aPoleIdx++).XYZ();
    double        aWiSinh   = 1.0;
    double        aWiCosh   = 1.0;
    if constexpr (theIsRational)
    {
      aWiSinh = theWeights.Value(aWeightIdx++);
      aWiCosh = theWeights.Value(aWeightIdx++);
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

    const gp_XYZ& aPoleSin = thePoles.Value(aPoleIdx++).XYZ();
    const gp_XYZ& aPoleCos = thePoles.Value(aPoleIdx++).XYZ();
    double        aWiSin   = 1.0;
    double        aWiCos   = 1.0;
    if constexpr (theIsRational)
    {
      aWiSin = theWeights.Value(aWeightIdx++);
      aWiCos = theWeights.Value(aWeightIdx++);
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
  throw Standard_NotImplemented("GeomEval_AHTBezierCurve::Reverse");
}

//==================================================================================================

double GeomEval_AHTBezierCurve::ReversedParameter(const double U) const
{
  (void)U;
  throw Standard_NotImplemented("GeomEval_AHTBezierCurve::ReversedParameter");
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

gp_Pnt GeomEval_AHTBezierCurve::EvalD0(const double U) const
{
  if (myAlpha <= 0.0 && myBeta <= 0.0)
  {
    PolyEvalData aPoly;
    if (!myRational)
    {
      evalPolynomialNonRational<0>(myPoles, myAlgDegree, U, aPoly);
      return gp_Pnt(aPoly.N0);
    }

    evalPolynomialRational<0>(myPoles, myWeights, myAlgDegree, U, aPoly);
    return gp_Pnt(aPoly.N0 / aPoly.W0);
  }
  PolyEvalData aMixed;
  if (!myRational)
  {
    evalMixedAHT<0, false>(myPoles, myWeights, myAlgDegree, myAlpha, myBeta, U, aMixed);
    return gp_Pnt(aMixed.N0);
  }

  evalMixedAHT<0, true>(myPoles, myWeights, myAlgDegree, myAlpha, myBeta, U, aMixed);
  return gp_Pnt(aMixed.N0 / aMixed.W0);
}

//==================================================================================================

Geom_Curve::ResD1 GeomEval_AHTBezierCurve::EvalD1(const double U) const
{
  if (myAlpha <= 0.0 && myBeta <= 0.0)
  {
    PolyEvalData        aPoly;
    Geom_Curve::ResD1 aResult;
    if (!myRational)
    {
      evalPolynomialNonRational<1>(myPoles, myAlgDegree, U, aPoly);
      aResult.Point = gp_Pnt(aPoly.N0);
      aResult.D1    = gp_Vec(aPoly.N1);
      return aResult;
    }

    evalPolynomialRational<1>(myPoles, myWeights, myAlgDegree, U, aPoly);
    const double aInvW = 1.0 / aPoly.W0;
    aResult.Point = gp_Pnt(aPoly.N0 * aInvW);
    aResult.D1    = gp_Vec((aPoly.N1 * aPoly.W0 - aPoly.N0 * aPoly.W1) * (aInvW * aInvW));
    return aResult;
  }

  Geom_Curve::ResD1 aResult;
  PolyEvalData aMixed;
  if (!myRational)
  {
    evalMixedAHT<1, false>(myPoles, myWeights, myAlgDegree, myAlpha, myBeta, U, aMixed);
    aResult.Point = gp_Pnt(aMixed.N0);
    aResult.D1    = gp_Vec(aMixed.N1);
    return aResult;
  }

  evalMixedAHT<1, true>(myPoles, myWeights, myAlgDegree, myAlpha, myBeta, U, aMixed);
  const double aInvW = 1.0 / aMixed.W0;
  aResult.Point = gp_Pnt(aMixed.N0 * aInvW);
  aResult.D1    = gp_Vec((aMixed.N1 * aMixed.W0 - aMixed.N0 * aMixed.W1) * (aInvW * aInvW));
  return aResult;
}

//==================================================================================================

Geom_Curve::ResD2 GeomEval_AHTBezierCurve::EvalD2(const double U) const
{
  if (myAlpha <= 0.0 && myBeta <= 0.0)
  {
    PolyEvalData        aPoly;
    Geom_Curve::ResD2 aResult;
    if (!myRational)
    {
      evalPolynomialNonRational<2>(myPoles, myAlgDegree, U, aPoly);
      aResult.Point = gp_Pnt(aPoly.N0);
      aResult.D1    = gp_Vec(aPoly.N1);
      aResult.D2    = gp_Vec(aPoly.N2);
      return aResult;
    }

    evalPolynomialRational<2>(myPoles, myWeights, myAlgDegree, U, aPoly);
    const double aInvW = 1.0 / aPoly.W0;
    const gp_XYZ aC    = aPoly.N0 * aInvW;
    const gp_XYZ aCD1  = (aPoly.N1 * aPoly.W0 - aPoly.N0 * aPoly.W1) * (aInvW * aInvW);
    aResult.Point = gp_Pnt(aC);
    aResult.D1    = gp_Vec(aCD1);
    aResult.D2    = gp_Vec((aPoly.N2 - aCD1 * (2.0 * aPoly.W1) - aC * aPoly.W2) * aInvW);
    return aResult;
  }

  Geom_Curve::ResD2 aResult;
  PolyEvalData aMixed;
  if (!myRational)
  {
    evalMixedAHT<2, false>(myPoles, myWeights, myAlgDegree, myAlpha, myBeta, U, aMixed);
    aResult.Point = gp_Pnt(aMixed.N0);
    aResult.D1    = gp_Vec(aMixed.N1);
    aResult.D2    = gp_Vec(aMixed.N2);
    return aResult;
  }

  evalMixedAHT<2, true>(myPoles, myWeights, myAlgDegree, myAlpha, myBeta, U, aMixed);
  const double aInvW = 1.0 / aMixed.W0;
  const gp_XYZ aC    = aMixed.N0 * aInvW;
  const gp_XYZ aCD1  = (aMixed.N1 * aMixed.W0 - aMixed.N0 * aMixed.W1) * (aInvW * aInvW);

  aResult.Point = gp_Pnt(aC);
  aResult.D1    = gp_Vec(aCD1);
  aResult.D2    = gp_Vec((aMixed.N2 - aCD1 * (2.0 * aMixed.W1) - aC * aMixed.W2) * aInvW);
  return aResult;
}

//==================================================================================================

Geom_Curve::ResD3 GeomEval_AHTBezierCurve::EvalD3(const double U) const
{
  if (myAlpha <= 0.0 && myBeta <= 0.0)
  {
    PolyEvalData        aPoly;
    Geom_Curve::ResD3 aResult;
    if (!myRational)
    {
      evalPolynomialNonRational<3>(myPoles, myAlgDegree, U, aPoly);
      aResult.Point = gp_Pnt(aPoly.N0);
      aResult.D1    = gp_Vec(aPoly.N1);
      aResult.D2    = gp_Vec(aPoly.N2);
      aResult.D3    = gp_Vec(aPoly.N3);
      return aResult;
    }

    evalPolynomialRational<3>(myPoles, myWeights, myAlgDegree, U, aPoly);
    const double aInvW = 1.0 / aPoly.W0;
    const gp_XYZ aC    = aPoly.N0 * aInvW;
    const gp_XYZ aCD1  = (aPoly.N1 - aC * aPoly.W1) * aInvW;
    const gp_XYZ aCD2  = (aPoly.N2 - aCD1 * (2.0 * aPoly.W1) - aC * aPoly.W2) * aInvW;
    const gp_XYZ aCD3  = (aPoly.N3 - aCD2 * (3.0 * aPoly.W1)
                                  - aCD1 * (3.0 * aPoly.W2)
                                  - aC * aPoly.W3)
                         * aInvW;

    aResult.Point = gp_Pnt(aC);
    aResult.D1    = gp_Vec(aCD1);
    aResult.D2    = gp_Vec(aCD2);
    aResult.D3    = gp_Vec(aCD3);
    return aResult;
  }

  Geom_Curve::ResD3 aResult;
  PolyEvalData aMixed;
  if (!myRational)
  {
    evalMixedAHT<3, false>(myPoles, myWeights, myAlgDegree, myAlpha, myBeta, U, aMixed);
    aResult.Point = gp_Pnt(aMixed.N0);
    aResult.D1    = gp_Vec(aMixed.N1);
    aResult.D2    = gp_Vec(aMixed.N2);
    aResult.D3    = gp_Vec(aMixed.N3);
    return aResult;
  }

  evalMixedAHT<3, true>(myPoles, myWeights, myAlgDegree, myAlpha, myBeta, U, aMixed);
  const double aInvW = 1.0 / aMixed.W0;
  const gp_XYZ aC    = aMixed.N0 * aInvW;
  const gp_XYZ aCD1  = (aMixed.N1 - aC * aMixed.W1) * aInvW;
  const gp_XYZ aCD2  = (aMixed.N2 - aCD1 * (2.0 * aMixed.W1) - aC * aMixed.W2) * aInvW;
  const gp_XYZ aCD3  = (aMixed.N3 - aCD2 * (3.0 * aMixed.W1)
                                  - aCD1 * (3.0 * aMixed.W2)
                                  - aC * aMixed.W3)
                         * aInvW;

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

  const int aPoleLow = myPoles.Lower();

  if (!myRational)
  {
    int    aPoleIdx = aPoleLow;
    gp_XYZ aSum(0.0, 0.0, 0.0);

    // Polynomial part.
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
      aSum += myPoles.Value(aPoleIdx++).XYZ() * aBasisN;
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
      aSum += myPoles.Value(aPoleIdx++).XYZ() * aSinhN;
      aSum += myPoles.Value(aPoleIdx++).XYZ() * aCoshN;
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
        case 0: aSinN = aSin;  aCosN = aCos;  break;
        case 1: aSinN = aCos;  aCosN = -aSin; break;
        case 2: aSinN = -aSin; aCosN = -aCos; break;
        case 3: aSinN = -aCos; aCosN = aSin;  break;
      }
      aSum += myPoles.Value(aPoleIdx++).XYZ() * (aBetaPow * aSinN);
      aSum += myPoles.Value(aPoleIdx++).XYZ() * (aBetaPow * aCosN);
    }

    return gp_Vec(aSum);
  }

  // Rational case. For general N, keep only W^(d) and C^(d) arrays;
  // N^(d) is computed on demand per derivative order.
  NCollection_LocalArray<double, 16> aWDerivs(N + 1);
  NCollection_LocalArray<gp_XYZ, 16> aCDerivs(N + 1);

  const int aWeightLow = myWeights.Lower();
  const int aPolyCount = myAlgDegree + 1;
  const int aHyperIdx  = aPoleLow + aPolyCount;
  const int aTrigIdx   = aHyperIdx + (myAlpha > 0.0 ? 2 : 0);

  const gp_XYZ* aPoleSinh = nullptr;
  const gp_XYZ* aPoleCosh = nullptr;
  const gp_XYZ* aPoleSin  = nullptr;
  const gp_XYZ* aPoleCos  = nullptr;
  double        aWiSinh   = 0.0;
  double        aWiCosh   = 0.0;
  double        aWiSin    = 0.0;
  double        aWiCos    = 0.0;

  if (myAlpha > 0.0)
  {
    aPoleSinh = &myPoles.Value(aHyperIdx).XYZ();
    aPoleCosh = &myPoles.Value(aHyperIdx + 1).XYZ();
    aWiSinh   = myWeights.Value(aWeightLow + aPolyCount);
    aWiCosh   = myWeights.Value(aWeightLow + aPolyCount + 1);
  }

  if (myBeta > 0.0)
  {
    const int aTrigWIdx = aWeightLow + aPolyCount + (myAlpha > 0.0 ? 2 : 0);
    aPoleSin = &myPoles.Value(aTrigIdx).XYZ();
    aPoleCos = &myPoles.Value(aTrigIdx + 1).XYZ();
    aWiSin   = myWeights.Value(aTrigWIdx);
    aWiCos   = myWeights.Value(aTrigWIdx + 1);
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
    gp_XYZ aNd(0.0, 0.0, 0.0);
    double aWd = 0.0;

    // Polynomial part: d/dt^d [t^k].
    for (int k = 0; k <= myAlgDegree; ++k)
    {
      const gp_XYZ& aPole = myPoles.Value(aPoleLow + k).XYZ();
      const double  aWi   = myWeights.Value(aWeightLow + k);
      double        aBasisD = 0.0;
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
        case 0: aDSin = aSin;  aDCos = aCos;  break;
        case 1: aDSin = aCos;  aDCos = -aSin; break;
        case 2: aDSin = -aSin; aDCos = -aCos; break;
        case 3: aDSin = -aCos; aDCos = aSin;  break;
      }
      aNd += (*aPoleSin) * (aWiSin * aBetaPow * aDSin) + (*aPoleCos) * (aWiCos * aBetaPow * aDCos);
      aWd += aWiSin * aBetaPow * aDSin + aWiCos * aBetaPow * aDCos;
    }

    aWDerivs[d] = aWd;
    if (d == 0)
    {
      aInvW0 = 1.0 / aWd;
      aCDerivs[0] = aNd * aInvW0;
    }
    else
    {
      gp_XYZ aSum = aNd;
      double aBinom = 1.0;
      for (int j = 1; j <= d; ++j)
      {
        aBinom = aBinom * double(d - j + 1) / double(j);
        aSum -= aCDerivs[d - j] * (aBinom * aWDerivs[j]);
      }
      aCDerivs[d] = aSum * aInvW0;
    }

    aAlphaPow *= myAlpha;
    aBetaPow  *= myBeta;
  }
  return gp_Vec(aCDerivs[N]);
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
