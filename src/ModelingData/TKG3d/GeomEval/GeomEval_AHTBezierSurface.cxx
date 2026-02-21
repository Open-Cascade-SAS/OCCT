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

#include <GeomEval_AHTBezierSurface.hxx>

#include <Geom_UndefinedDerivative.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomEval_AHTBezierSurface, Geom_BoundedSurface)

//==================================================================================================

int GeomEval_AHTBezierSurface::basisDimension(int    theAlgDegree,
                                               double theAlpha,
                                               double theBeta)
{
  return theAlgDegree + 1
       + (theAlpha > 0.0 ? 2 : 0)
       + (theBeta > 0.0 ? 2 : 0);
}

//==================================================================================================

void GeomEval_AHTBezierSurface::evalBasis(double                      theT,
                                           int                         theAlgDegree,
                                           double                      theAlpha,
                                           double                      theBeta,
                                           NCollection_Array1<double>& theBasis)
{
  const int aLower = theBasis.Lower();
  int       anIdx  = aLower;

  // Polynomial part: 1, t, t^2, ..., t^k
  double aPow = 1.0;
  for (int k = 0; k <= theAlgDegree; ++k)
  {
    theBasis.ChangeValue(anIdx++) = aPow;
    aPow *= theT;
  }

  // Hyperbolic part: sinh(alpha*t), cosh(alpha*t)
  if (theAlpha > 0.0)
  {
    const double aAlphaT = theAlpha * theT;
    theBasis.ChangeValue(anIdx++) = std::sinh(aAlphaT);
    theBasis.ChangeValue(anIdx++) = std::cosh(aAlphaT);
  }

  // Trigonometric part: sin(beta*t), cos(beta*t)
  if (theBeta > 0.0)
  {
    const double aBetaT = theBeta * theT;
    theBasis.ChangeValue(anIdx++) = std::sin(aBetaT);
    theBasis.ChangeValue(anIdx++) = std::cos(aBetaT);
  }
}

//==================================================================================================

void GeomEval_AHTBezierSurface::evalBasisDeriv(double                      theT,
                                                int                         theDerivOrder,
                                                int                         theAlgDegree,
                                                double                      theAlpha,
                                                double                      theBeta,
                                                NCollection_Array1<double>& theBasisDeriv)
{
  const int aLower = theBasisDeriv.Lower();
  int       anIdx  = aLower;

  // Polynomial part: d^d/dt^d [t^k] = k!/(k-d)! * t^(k-d) if d <= k, else 0
  for (int k = 0; k <= theAlgDegree; ++k)
  {
    if (theDerivOrder > k)
    {
      theBasisDeriv.ChangeValue(anIdx++) = 0.0;
    }
    else
    {
      double aCoeff = 1.0;
      for (int j = 0; j < theDerivOrder; ++j)
      {
        aCoeff *= (k - j);
      }
      theBasisDeriv.ChangeValue(anIdx++) = aCoeff * std::pow(theT, k - theDerivOrder);
    }
  }

  // Hyperbolic part
  if (theAlpha > 0.0)
  {
    const double aAlphaT     = theAlpha * theT;
    const double aAlphaPow   = std::pow(theAlpha, theDerivOrder);
    const double aSinh       = std::sinh(aAlphaT);
    const double aCosh       = std::cosh(aAlphaT);
    const bool   isEvenDeriv = (theDerivOrder % 2 == 0);

    theBasisDeriv.ChangeValue(anIdx++) = aAlphaPow * (isEvenDeriv ? aSinh : aCosh);
    theBasisDeriv.ChangeValue(anIdx++) = aAlphaPow * (isEvenDeriv ? aCosh : aSinh);
  }

  // Trigonometric part
  if (theBeta > 0.0)
  {
    const double aBetaT   = theBeta * theT;
    const double aBetaPow = std::pow(theBeta, theDerivOrder);
    const double aPhase   = theDerivOrder * M_PI / 2.0;

    theBasisDeriv.ChangeValue(anIdx++) = aBetaPow * std::sin(aBetaT + aPhase);
    theBasisDeriv.ChangeValue(anIdx++) = aBetaPow * std::cos(aBetaT + aPhase);
  }
}

//==================================================================================================

GeomEval_AHTBezierSurface::GeomEval_AHTBezierSurface(
  const NCollection_Array2<gp_Pnt>& thePoles,
  int                               theAlgDegreeU,
  int                               theAlgDegreeV,
  double                            theAlphaU,
  double                            theAlphaV,
  double                            theBetaU,
  double                            theBetaV)
    : myPoles(thePoles),
      myWeights(1, 1, 1, 1),
      myAlgDegreeU(theAlgDegreeU),
      myAlgDegreeV(theAlgDegreeV),
      myAlphaU(theAlphaU),
      myAlphaV(theAlphaV),
      myBetaU(theBetaU),
      myBetaV(theBetaV),
      myURational(false),
      myVRational(false)
{
  if (theAlgDegreeU < 0 || theAlgDegreeV < 0)
  {
    throw Standard_ConstructionError(
      "GeomEval_AHTBezierSurface: algebraic degree must be >= 0");
  }
  if (theAlphaU < 0.0 || theAlphaV < 0.0)
  {
    throw Standard_ConstructionError(
      "GeomEval_AHTBezierSurface: alpha must be >= 0");
  }
  if (theBetaU < 0.0 || theBetaV < 0.0)
  {
    throw Standard_ConstructionError(
      "GeomEval_AHTBezierSurface: beta must be >= 0");
  }
  const int aDimU = basisDimension(theAlgDegreeU, theAlphaU, theBetaU);
  const int aDimV = basisDimension(theAlgDegreeV, theAlphaV, theBetaV);
  if (thePoles.NbRows() != aDimU || thePoles.NbColumns() != aDimV)
  {
    throw Standard_ConstructionError(
      "GeomEval_AHTBezierSurface: poles array dimensions must match basis dimensions");
  }
}

//==================================================================================================

GeomEval_AHTBezierSurface::GeomEval_AHTBezierSurface(
  const NCollection_Array2<gp_Pnt>&  thePoles,
  const NCollection_Array2<double>& theWeights,
  int                               theAlgDegreeU,
  int                               theAlgDegreeV,
  double                            theAlphaU,
  double                            theAlphaV,
  double                            theBetaU,
  double                            theBetaV)
    : myPoles(thePoles),
      myWeights(theWeights),
      myAlgDegreeU(theAlgDegreeU),
      myAlgDegreeV(theAlgDegreeV),
      myAlphaU(theAlphaU),
      myAlphaV(theAlphaV),
      myBetaU(theBetaU),
      myBetaV(theBetaV),
      myURational(true),
      myVRational(true)
{
  if (theAlgDegreeU < 0 || theAlgDegreeV < 0)
  {
    throw Standard_ConstructionError(
      "GeomEval_AHTBezierSurface: algebraic degree must be >= 0");
  }
  if (theAlphaU < 0.0 || theAlphaV < 0.0)
  {
    throw Standard_ConstructionError(
      "GeomEval_AHTBezierSurface: alpha must be >= 0");
  }
  if (theBetaU < 0.0 || theBetaV < 0.0)
  {
    throw Standard_ConstructionError(
      "GeomEval_AHTBezierSurface: beta must be >= 0");
  }
  const int aDimU = basisDimension(theAlgDegreeU, theAlphaU, theBetaU);
  const int aDimV = basisDimension(theAlgDegreeV, theAlphaV, theBetaV);
  if (thePoles.NbRows() != aDimU || thePoles.NbColumns() != aDimV)
  {
    throw Standard_ConstructionError(
      "GeomEval_AHTBezierSurface: poles array dimensions must match basis dimensions");
  }
  if (theWeights.NbRows() != thePoles.NbRows()
   || theWeights.NbColumns() != thePoles.NbColumns())
  {
    throw Standard_ConstructionError(
      "GeomEval_AHTBezierSurface: weights array dimensions must match poles");
  }
  for (int i = theWeights.LowerRow(); i <= theWeights.UpperRow(); ++i)
  {
    for (int j = theWeights.LowerCol(); j <= theWeights.UpperCol(); ++j)
    {
      if (theWeights.Value(i, j) <= 0.0)
      {
        throw Standard_ConstructionError(
          "GeomEval_AHTBezierSurface: all weights must be > 0");
      }
    }
  }
}

//==================================================================================================

const NCollection_Array2<gp_Pnt>& GeomEval_AHTBezierSurface::Poles() const
{
  return myPoles;
}

//==================================================================================================

const NCollection_Array2<double>& GeomEval_AHTBezierSurface::Weights() const
{
  return myWeights;
}

//==================================================================================================

int GeomEval_AHTBezierSurface::AlgDegreeU() const
{
  return myAlgDegreeU;
}

//==================================================================================================

int GeomEval_AHTBezierSurface::AlgDegreeV() const
{
  return myAlgDegreeV;
}

//==================================================================================================

double GeomEval_AHTBezierSurface::AlphaU() const
{
  return myAlphaU;
}

//==================================================================================================

double GeomEval_AHTBezierSurface::AlphaV() const
{
  return myAlphaV;
}

//==================================================================================================

double GeomEval_AHTBezierSurface::BetaU() const
{
  return myBetaU;
}

//==================================================================================================

double GeomEval_AHTBezierSurface::BetaV() const
{
  return myBetaV;
}

//==================================================================================================

int GeomEval_AHTBezierSurface::NbPolesU() const
{
  return myPoles.NbRows();
}

//==================================================================================================

int GeomEval_AHTBezierSurface::NbPolesV() const
{
  return myPoles.NbColumns();
}

//==================================================================================================

bool GeomEval_AHTBezierSurface::IsURational() const
{
  return myURational;
}

//==================================================================================================

bool GeomEval_AHTBezierSurface::IsVRational() const
{
  return myVRational;
}

//==================================================================================================

void GeomEval_AHTBezierSurface::UReverse()
{
  const int aLR = myPoles.LowerRow();
  const int aUR = myPoles.UpperRow();
  const int aLC = myPoles.LowerCol();
  const int aUC = myPoles.UpperCol();
  for (int j = aLC; j <= aUC; ++j)
  {
    for (int i = aLR, k = aUR; i < k; ++i, --k)
    {
      gp_Pnt aTmp = myPoles.Value(i, j);
      myPoles.ChangeValue(i, j) = myPoles.Value(k, j);
      myPoles.ChangeValue(k, j) = aTmp;
    }
  }
  if (myURational || myVRational)
  {
    const int aWLR = myWeights.LowerRow();
    const int aWUR = myWeights.UpperRow();
    const int aWLC = myWeights.LowerCol();
    const int aWUC = myWeights.UpperCol();
    for (int j = aWLC; j <= aWUC; ++j)
    {
      for (int i = aWLR, k = aWUR; i < k; ++i, --k)
      {
        double aTmp = myWeights.Value(i, j);
        myWeights.ChangeValue(i, j) = myWeights.Value(k, j);
        myWeights.ChangeValue(k, j) = aTmp;
      }
    }
  }
}

//==================================================================================================

void GeomEval_AHTBezierSurface::VReverse()
{
  const int aLR = myPoles.LowerRow();
  const int aUR = myPoles.UpperRow();
  const int aLC = myPoles.LowerCol();
  const int aUC = myPoles.UpperCol();
  for (int i = aLR; i <= aUR; ++i)
  {
    for (int j = aLC, k = aUC; j < k; ++j, --k)
    {
      gp_Pnt aTmp = myPoles.Value(i, j);
      myPoles.ChangeValue(i, j) = myPoles.Value(i, k);
      myPoles.ChangeValue(i, k) = aTmp;
    }
  }
  if (myURational || myVRational)
  {
    const int aWLR = myWeights.LowerRow();
    const int aWUR = myWeights.UpperRow();
    const int aWLC = myWeights.LowerCol();
    const int aWUC = myWeights.UpperCol();
    for (int i = aWLR; i <= aWUR; ++i)
    {
      for (int j = aWLC, k = aWUC; j < k; ++j, --k)
      {
        double aTmp = myWeights.Value(i, j);
        myWeights.ChangeValue(i, j) = myWeights.Value(i, k);
        myWeights.ChangeValue(i, k) = aTmp;
      }
    }
  }
}

//==================================================================================================

double GeomEval_AHTBezierSurface::UReversedParameter(const double U) const
{
  return 1.0 - U;
}

//==================================================================================================

double GeomEval_AHTBezierSurface::VReversedParameter(const double V) const
{
  return 1.0 - V;
}

//==================================================================================================

void GeomEval_AHTBezierSurface::Bounds(double& U1, double& U2,
                                        double& V1, double& V2) const
{
  U1 = 0.0;
  U2 = 1.0;
  V1 = 0.0;
  V2 = 1.0;
}

//==================================================================================================

bool GeomEval_AHTBezierSurface::IsUClosed() const
{
  return false;
}

//==================================================================================================

bool GeomEval_AHTBezierSurface::IsVClosed() const
{
  return false;
}

//==================================================================================================

bool GeomEval_AHTBezierSurface::IsUPeriodic() const
{
  return false;
}

//==================================================================================================

bool GeomEval_AHTBezierSurface::IsVPeriodic() const
{
  return false;
}

//==================================================================================================

occ::handle<Geom_Curve> GeomEval_AHTBezierSurface::UIso(const double /*U*/) const
{
  return nullptr;
}

//==================================================================================================

occ::handle<Geom_Curve> GeomEval_AHTBezierSurface::VIso(const double /*V*/) const
{
  return nullptr;
}

//==================================================================================================

GeomAbs_Shape GeomEval_AHTBezierSurface::Continuity() const
{
  return GeomAbs_CN;
}

//==================================================================================================

bool GeomEval_AHTBezierSurface::IsCNu(const int /*N*/) const
{
  return true;
}

//==================================================================================================

bool GeomEval_AHTBezierSurface::IsCNv(const int /*N*/) const
{
  return true;
}

//==================================================================================================

gp_Pnt GeomEval_AHTBezierSurface::EvalD0(const double U, const double V) const
{
  const int aDimU = NbPolesU();
  const int aDimV = NbPolesV();

  NCollection_Array1<double> aBasisU(0, aDimU - 1);
  NCollection_Array1<double> aBasisV(0, aDimV - 1);
  evalBasis(U, myAlgDegreeU, myAlphaU, myBetaU, aBasisU);
  evalBasis(V, myAlgDegreeV, myAlphaV, myBetaV, aBasisV);

  const bool isRational = (myURational || myVRational);
  const int  aLR = myPoles.LowerRow();
  const int  aLC = myPoles.LowerCol();

  if (!isRational)
  {
    gp_XYZ aSum(0.0, 0.0, 0.0);
    for (int i = 0; i < aDimU; ++i)
    {
      for (int j = 0; j < aDimV; ++j)
      {
        aSum += myPoles.Value(aLR + i, aLC + j).XYZ()
                * (aBasisU.Value(i) * aBasisV.Value(j));
      }
    }
    return gp_Pnt(aSum);
  }

  gp_XYZ aNumer(0.0, 0.0, 0.0);
  double  aDenom = 0.0;
  const int aWLR = myWeights.LowerRow();
  const int aWLC = myWeights.LowerCol();
  for (int i = 0; i < aDimU; ++i)
  {
    for (int j = 0; j < aDimV; ++j)
    {
      const double aWB = myWeights.Value(aWLR + i, aWLC + j)
                        * aBasisU.Value(i) * aBasisV.Value(j);
      aNumer += myPoles.Value(aLR + i, aLC + j).XYZ() * aWB;
      aDenom += aWB;
    }
  }
  return gp_Pnt(aNumer / aDenom);
}

//==================================================================================================

Geom_Surface::ResD1 GeomEval_AHTBezierSurface::EvalD1(const double U, const double V) const
{
  const int aDimU = NbPolesU();
  const int aDimV = NbPolesV();

  NCollection_Array1<double> aBasisU(0, aDimU - 1);
  NCollection_Array1<double> aBasisV(0, aDimV - 1);
  NCollection_Array1<double> aBasisDU(0, aDimU - 1);
  NCollection_Array1<double> aBasisDV(0, aDimV - 1);
  evalBasis(U, myAlgDegreeU, myAlphaU, myBetaU, aBasisU);
  evalBasis(V, myAlgDegreeV, myAlphaV, myBetaV, aBasisV);
  evalBasisDeriv(U, 1, myAlgDegreeU, myAlphaU, myBetaU, aBasisDU);
  evalBasisDeriv(V, 1, myAlgDegreeV, myAlphaV, myBetaV, aBasisDV);

  const bool isRational = (myURational || myVRational);
  const int  aLR = myPoles.LowerRow();
  const int  aLC = myPoles.LowerCol();

  Geom_Surface::ResD1 aResult;

  if (!isRational)
  {
    gp_XYZ aSum(0.0, 0.0, 0.0);
    gp_XYZ aSumDU(0.0, 0.0, 0.0);
    gp_XYZ aSumDV(0.0, 0.0, 0.0);
    for (int i = 0; i < aDimU; ++i)
    {
      for (int j = 0; j < aDimV; ++j)
      {
        const gp_XYZ& aPole = myPoles.Value(aLR + i, aLC + j).XYZ();
        const double  aBU   = aBasisU.Value(i);
        const double  aBV   = aBasisV.Value(j);
        aSum   += aPole * (aBU * aBV);
        aSumDU += aPole * (aBasisDU.Value(i) * aBV);
        aSumDV += aPole * (aBU * aBasisDV.Value(j));
      }
    }
    aResult.Point = gp_Pnt(aSum);
    aResult.D1U   = gp_Vec(aSumDU);
    aResult.D1V   = gp_Vec(aSumDV);
    return aResult;
  }

  // Rational tensor product with quotient rule.
  gp_XYZ aN(0.0, 0.0, 0.0);
  gp_XYZ aNDU(0.0, 0.0, 0.0);
  gp_XYZ aNDV(0.0, 0.0, 0.0);
  double  aW   = 0.0;
  double  aWDU = 0.0;
  double  aWDV = 0.0;
  const int aWLR = myWeights.LowerRow();
  const int aWLC = myWeights.LowerCol();
  for (int i = 0; i < aDimU; ++i)
  {
    for (int j = 0; j < aDimV; ++j)
    {
      const gp_XYZ& aPole = myPoles.Value(aLR + i, aLC + j).XYZ();
      const double  aWij  = myWeights.Value(aWLR + i, aWLC + j);
      const double  aBU   = aBasisU.Value(i);
      const double  aBV   = aBasisV.Value(j);
      const double  aBDU  = aBasisDU.Value(i);
      const double  aBDV  = aBasisDV.Value(j);

      const double  aWBUV = aWij * aBU * aBV;
      aN  += aPole * aWBUV;
      aW  += aWBUV;

      const double aWDUBV = aWij * aBDU * aBV;
      aNDU += aPole * aWDUBV;
      aWDU += aWDUBV;

      const double aWBUDV = aWij * aBU * aBDV;
      aNDV += aPole * aWBUDV;
      aWDV += aWBUDV;
    }
  }
  const double aInvW = 1.0 / aW;
  const gp_XYZ aC = aN * aInvW;

  aResult.Point = gp_Pnt(aC);
  aResult.D1U   = gp_Vec((aNDU - aC * aWDU) * aInvW);
  aResult.D1V   = gp_Vec((aNDV - aC * aWDV) * aInvW);
  return aResult;
}

//==================================================================================================

Geom_Surface::ResD2 GeomEval_AHTBezierSurface::EvalD2(const double U, const double V) const
{
  const int aDimU = NbPolesU();
  const int aDimV = NbPolesV();

  NCollection_Array1<double> aBasisU(0, aDimU - 1);
  NCollection_Array1<double> aBasisV(0, aDimV - 1);
  NCollection_Array1<double> aBasisDU(0, aDimU - 1);
  NCollection_Array1<double> aBasisDV(0, aDimV - 1);
  NCollection_Array1<double> aBasisD2U(0, aDimU - 1);
  NCollection_Array1<double> aBasisD2V(0, aDimV - 1);
  evalBasis(U, myAlgDegreeU, myAlphaU, myBetaU, aBasisU);
  evalBasis(V, myAlgDegreeV, myAlphaV, myBetaV, aBasisV);
  evalBasisDeriv(U, 1, myAlgDegreeU, myAlphaU, myBetaU, aBasisDU);
  evalBasisDeriv(V, 1, myAlgDegreeV, myAlphaV, myBetaV, aBasisDV);
  evalBasisDeriv(U, 2, myAlgDegreeU, myAlphaU, myBetaU, aBasisD2U);
  evalBasisDeriv(V, 2, myAlgDegreeV, myAlphaV, myBetaV, aBasisD2V);

  const bool isRational = (myURational || myVRational);
  const int  aLR = myPoles.LowerRow();
  const int  aLC = myPoles.LowerCol();

  Geom_Surface::ResD2 aResult;

  if (!isRational)
  {
    gp_XYZ aSum(0.0, 0.0, 0.0);
    gp_XYZ aSumDU(0.0, 0.0, 0.0);
    gp_XYZ aSumDV(0.0, 0.0, 0.0);
    gp_XYZ aSumD2U(0.0, 0.0, 0.0);
    gp_XYZ aSumD2V(0.0, 0.0, 0.0);
    gp_XYZ aSumDUV(0.0, 0.0, 0.0);
    for (int i = 0; i < aDimU; ++i)
    {
      for (int j = 0; j < aDimV; ++j)
      {
        const gp_XYZ& aPole = myPoles.Value(aLR + i, aLC + j).XYZ();
        const double  aBU   = aBasisU.Value(i);
        const double  aBV   = aBasisV.Value(j);
        const double  aBDU  = aBasisDU.Value(i);
        const double  aBDV  = aBasisDV.Value(j);
        aSum    += aPole * (aBU * aBV);
        aSumDU  += aPole * (aBDU * aBV);
        aSumDV  += aPole * (aBU * aBDV);
        aSumD2U += aPole * (aBasisD2U.Value(i) * aBV);
        aSumD2V += aPole * (aBU * aBasisD2V.Value(j));
        aSumDUV += aPole * (aBDU * aBDV);
      }
    }
    aResult.Point = gp_Pnt(aSum);
    aResult.D1U   = gp_Vec(aSumDU);
    aResult.D1V   = gp_Vec(aSumDV);
    aResult.D2U   = gp_Vec(aSumD2U);
    aResult.D2V   = gp_Vec(aSumD2V);
    aResult.D2UV  = gp_Vec(aSumDUV);
    return aResult;
  }

  // Rational: accumulate weighted sums.
  gp_XYZ aN(0.0, 0.0, 0.0);
  gp_XYZ aNDU(0.0, 0.0, 0.0);
  gp_XYZ aNDV(0.0, 0.0, 0.0);
  gp_XYZ aND2U(0.0, 0.0, 0.0);
  gp_XYZ aND2V(0.0, 0.0, 0.0);
  gp_XYZ aNDUV(0.0, 0.0, 0.0);
  double  aW    = 0.0;
  double  aWDU  = 0.0;
  double  aWDV  = 0.0;
  double  aWD2U = 0.0;
  double  aWD2V = 0.0;
  double  aWDUV = 0.0;
  const int aWLR = myWeights.LowerRow();
  const int aWLC = myWeights.LowerCol();
  for (int i = 0; i < aDimU; ++i)
  {
    for (int j = 0; j < aDimV; ++j)
    {
      const gp_XYZ& aPole = myPoles.Value(aLR + i, aLC + j).XYZ();
      const double  aWij  = myWeights.Value(aWLR + i, aWLC + j);
      const double  aBU   = aBasisU.Value(i);
      const double  aBV   = aBasisV.Value(j);
      const double  aBDU  = aBasisDU.Value(i);
      const double  aBDV  = aBasisDV.Value(j);
      const double  aBD2U = aBasisD2U.Value(i);
      const double  aBD2V = aBasisD2V.Value(j);

      aN    += aPole * (aWij * aBU * aBV);
      aNDU  += aPole * (aWij * aBDU * aBV);
      aNDV  += aPole * (aWij * aBU * aBDV);
      aND2U += aPole * (aWij * aBD2U * aBV);
      aND2V += aPole * (aWij * aBU * aBD2V);
      aNDUV += aPole * (aWij * aBDU * aBDV);
      aW    += aWij * aBU * aBV;
      aWDU  += aWij * aBDU * aBV;
      aWDV  += aWij * aBU * aBDV;
      aWD2U += aWij * aBD2U * aBV;
      aWD2V += aWij * aBU * aBD2V;
      aWDUV += aWij * aBDU * aBDV;
    }
  }
  const double aInvW = 1.0 / aW;
  const gp_XYZ aC    = aN * aInvW;
  const gp_XYZ aCDU  = (aNDU - aC * aWDU) * aInvW;
  const gp_XYZ aCDV  = (aNDV - aC * aWDV) * aInvW;

  aResult.Point = gp_Pnt(aC);
  aResult.D1U   = gp_Vec(aCDU);
  aResult.D1V   = gp_Vec(aCDV);
  aResult.D2U   = gp_Vec((aND2U - aCDU * (2.0 * aWDU) - aC * aWD2U) * aInvW);
  aResult.D2V   = gp_Vec((aND2V - aCDV * (2.0 * aWDV) - aC * aWD2V) * aInvW);
  aResult.D2UV  = gp_Vec((aNDUV - aCDU * aWDV - aCDV * aWDU - aC * aWDUV) * aInvW);
  return aResult;
}

//==================================================================================================

Geom_Surface::ResD3 GeomEval_AHTBezierSurface::EvalD3(const double U, const double V) const
{
  const int aDimU = NbPolesU();
  const int aDimV = NbPolesV();

  // Evaluate basis and derivatives up to order 3 in each direction.
  NCollection_Array1<double> aBU0(0, aDimU - 1);
  NCollection_Array1<double> aBU1(0, aDimU - 1);
  NCollection_Array1<double> aBU2(0, aDimU - 1);
  NCollection_Array1<double> aBU3(0, aDimU - 1);
  NCollection_Array1<double> aBV0(0, aDimV - 1);
  NCollection_Array1<double> aBV1(0, aDimV - 1);
  NCollection_Array1<double> aBV2(0, aDimV - 1);
  NCollection_Array1<double> aBV3(0, aDimV - 1);
  evalBasis(U, myAlgDegreeU, myAlphaU, myBetaU, aBU0);
  evalBasis(V, myAlgDegreeV, myAlphaV, myBetaV, aBV0);
  evalBasisDeriv(U, 1, myAlgDegreeU, myAlphaU, myBetaU, aBU1);
  evalBasisDeriv(V, 1, myAlgDegreeV, myAlphaV, myBetaV, aBV1);
  evalBasisDeriv(U, 2, myAlgDegreeU, myAlphaU, myBetaU, aBU2);
  evalBasisDeriv(V, 2, myAlgDegreeV, myAlphaV, myBetaV, aBV2);
  evalBasisDeriv(U, 3, myAlgDegreeU, myAlphaU, myBetaU, aBU3);
  evalBasisDeriv(V, 3, myAlgDegreeV, myAlphaV, myBetaV, aBV3);

  const bool isRational = (myURational || myVRational);
  const int  aLR = myPoles.LowerRow();
  const int  aLC = myPoles.LowerCol();

  Geom_Surface::ResD3 aResult;

  if (!isRational)
  {
    gp_XYZ aS00(0.0, 0.0, 0.0); // S(u,v)
    gp_XYZ aS10(0.0, 0.0, 0.0); // dS/du
    gp_XYZ aS01(0.0, 0.0, 0.0); // dS/dv
    gp_XYZ aS20(0.0, 0.0, 0.0); // d2S/du2
    gp_XYZ aS02(0.0, 0.0, 0.0); // d2S/dv2
    gp_XYZ aS11(0.0, 0.0, 0.0); // d2S/dudv
    gp_XYZ aS30(0.0, 0.0, 0.0); // d3S/du3
    gp_XYZ aS03(0.0, 0.0, 0.0); // d3S/dv3
    gp_XYZ aS21(0.0, 0.0, 0.0); // d3S/du2dv
    gp_XYZ aS12(0.0, 0.0, 0.0); // d3S/dudv2
    for (int i = 0; i < aDimU; ++i)
    {
      for (int j = 0; j < aDimV; ++j)
      {
        const gp_XYZ& aPole = myPoles.Value(aLR + i, aLC + j).XYZ();
        const double  bu0 = aBU0.Value(i);
        const double  bu1 = aBU1.Value(i);
        const double  bu2 = aBU2.Value(i);
        const double  bu3 = aBU3.Value(i);
        const double  bv0 = aBV0.Value(j);
        const double  bv1 = aBV1.Value(j);
        const double  bv2 = aBV2.Value(j);
        const double  bv3 = aBV3.Value(j);
        aS00 += aPole * (bu0 * bv0);
        aS10 += aPole * (bu1 * bv0);
        aS01 += aPole * (bu0 * bv1);
        aS20 += aPole * (bu2 * bv0);
        aS02 += aPole * (bu0 * bv2);
        aS11 += aPole * (bu1 * bv1);
        aS30 += aPole * (bu3 * bv0);
        aS03 += aPole * (bu0 * bv3);
        aS21 += aPole * (bu2 * bv1);
        aS12 += aPole * (bu1 * bv2);
      }
    }
    aResult.Point = gp_Pnt(aS00);
    aResult.D1U   = gp_Vec(aS10);
    aResult.D1V   = gp_Vec(aS01);
    aResult.D2U   = gp_Vec(aS20);
    aResult.D2V   = gp_Vec(aS02);
    aResult.D2UV  = gp_Vec(aS11);
    aResult.D3U   = gp_Vec(aS30);
    aResult.D3V   = gp_Vec(aS03);
    aResult.D3UUV = gp_Vec(aS21);
    aResult.D3UVV = gp_Vec(aS12);
    return aResult;
  }

  // Rational: accumulate all weighted sums N^(ij) and W^(ij).
  // We need derivatives N and W for (du,dv) with du+dv <= 3.
  // Index encoding: [du][dv] for du=0..3, dv=0..3, du+dv<=3
  gp_XYZ aN[4][4];
  double  aWArr[4][4];
  for (int du = 0; du <= 3; ++du)
  {
    for (int dv = 0; dv <= 3 - du; ++dv)
    {
      aN[du][dv] = gp_XYZ(0.0, 0.0, 0.0);
      aWArr[du][dv] = 0.0;
    }
  }

  const int aWLR = myWeights.LowerRow();
  const int aWLC = myWeights.LowerCol();
  // U basis arrays indexed by derivative order
  const NCollection_Array1<double>* aBUArr[4] = {&aBU0, &aBU1, &aBU2, &aBU3};
  const NCollection_Array1<double>* aBVArr[4] = {&aBV0, &aBV1, &aBV2, &aBV3};

  for (int i = 0; i < aDimU; ++i)
  {
    for (int j = 0; j < aDimV; ++j)
    {
      const gp_XYZ& aPole = myPoles.Value(aLR + i, aLC + j).XYZ();
      const double  aWij  = myWeights.Value(aWLR + i, aWLC + j);
      for (int du = 0; du <= 3; ++du)
      {
        for (int dv = 0; dv <= 3 - du; ++dv)
        {
          const double aVal = aWij * aBUArr[du]->Value(i) * aBVArr[dv]->Value(j);
          aN[du][dv] += aPole * aVal;
          aWArr[du][dv] += aVal;
        }
      }
    }
  }

  // Now compute C^(du,dv) using the recursive formula for tensor-product rationals:
  // C^(du,dv) = (N^(du,dv) - sum_{(p,q)!=(0,0), p<=du, q<=dv} C(du,p)*C(dv,q)*W^(p,q)*C^(du-p,dv-q)) / W^(0,0)
  const double aInvW = 1.0 / aWArr[0][0];
  gp_XYZ aC[4][4];

  for (int aOrd = 0; aOrd <= 3; ++aOrd)
  {
    for (int du = 0; du <= aOrd; ++du)
    {
      const int dv = aOrd - du;
      if (dv > 3 || du > 3)
      {
        continue;
      }
      gp_XYZ aSum = aN[du][dv];
      for (int p = 0; p <= du; ++p)
      {
        for (int q = 0; q <= dv; ++q)
        {
          if (p == 0 && q == 0)
          {
            continue;
          }
          // Binomial coefficients C(du,p) * C(dv,q)
          double aBinomU = 1.0;
          for (int m = 0; m < p; ++m)
          {
            aBinomU = aBinomU * (du - m) / (m + 1);
          }
          double aBinomV = 1.0;
          for (int m = 0; m < q; ++m)
          {
            aBinomV = aBinomV * (dv - m) / (m + 1);
          }
          aSum -= aC[du - p][dv - q] * (aBinomU * aBinomV * aWArr[p][q]);
        }
      }
      aC[du][dv] = aSum * aInvW;
    }
  }

  aResult.Point = gp_Pnt(aC[0][0]);
  aResult.D1U   = gp_Vec(aC[1][0]);
  aResult.D1V   = gp_Vec(aC[0][1]);
  aResult.D2U   = gp_Vec(aC[2][0]);
  aResult.D2V   = gp_Vec(aC[0][2]);
  aResult.D2UV  = gp_Vec(aC[1][1]);
  aResult.D3U   = gp_Vec(aC[3][0]);
  aResult.D3V   = gp_Vec(aC[0][3]);
  aResult.D3UUV = gp_Vec(aC[2][1]);
  aResult.D3UVV = gp_Vec(aC[1][2]);
  return aResult;
}

//==================================================================================================

gp_Vec GeomEval_AHTBezierSurface::EvalDN(const double U,
                                          const double V,
                                          const int    Nu,
                                          const int    Nv) const
{
  if (Nu + Nv < 1 || Nu < 0 || Nv < 0)
  {
    throw Geom_UndefinedDerivative(
      "GeomEval_AHTBezierSurface::EvalDN: invalid derivative order");
  }

  const int aMaxOrd = Nu + Nv;
  const int aDimU   = NbPolesU();
  const int aDimV   = NbPolesV();

  // Evaluate basis derivatives in both directions for all needed orders.
  NCollection_Array1<NCollection_Array1<double>> aBUDerivs(0, Nu);
  NCollection_Array1<NCollection_Array1<double>> aBVDerivs(0, Nv);
  for (int d = 0; d <= Nu; ++d)
  {
    aBUDerivs.ChangeValue(d).Resize(0, aDimU - 1, false);
    if (d == 0)
    {
      evalBasis(U, myAlgDegreeU, myAlphaU, myBetaU, aBUDerivs.ChangeValue(0));
    }
    else
    {
      evalBasisDeriv(U, d, myAlgDegreeU, myAlphaU, myBetaU, aBUDerivs.ChangeValue(d));
    }
  }
  for (int d = 0; d <= Nv; ++d)
  {
    aBVDerivs.ChangeValue(d).Resize(0, aDimV - 1, false);
    if (d == 0)
    {
      evalBasis(V, myAlgDegreeV, myAlphaV, myBetaV, aBVDerivs.ChangeValue(0));
    }
    else
    {
      evalBasisDeriv(V, d, myAlgDegreeV, myAlphaV, myBetaV, aBVDerivs.ChangeValue(d));
    }
  }

  const bool isRational = (myURational || myVRational);
  const int  aLR = myPoles.LowerRow();
  const int  aLC = myPoles.LowerCol();

  if (!isRational)
  {
    gp_XYZ aSum(0.0, 0.0, 0.0);
    for (int i = 0; i < aDimU; ++i)
    {
      for (int j = 0; j < aDimV; ++j)
      {
        aSum += myPoles.Value(aLR + i, aLC + j).XYZ()
                * (aBUDerivs.Value(Nu).Value(i) * aBVDerivs.Value(Nv).Value(j));
      }
    }
    return gp_Vec(aSum);
  }

  // Rational: general recursive formula.
  // Need N^(du,dv) and W^(du,dv) for all du<=Nu, dv<=Nv.
  const int aWLR = myWeights.LowerRow();
  const int aWLC = myWeights.LowerCol();

  // Allocate N and W derivative arrays.
  NCollection_Array1<NCollection_Array1<gp_XYZ>> aNArr(0, Nu);
  NCollection_Array1<NCollection_Array1<double>> aWDArr(0, Nu);
  for (int du = 0; du <= Nu; ++du)
  {
    aNArr.ChangeValue(du).Resize(0, Nv, false);
    aWDArr.ChangeValue(du).Resize(0, Nv, false);
    for (int dv = 0; dv <= Nv; ++dv)
    {
      aNArr.ChangeValue(du).ChangeValue(dv) = gp_XYZ(0.0, 0.0, 0.0);
      aWDArr.ChangeValue(du).ChangeValue(dv) = 0.0;
    }
  }

  for (int i = 0; i < aDimU; ++i)
  {
    for (int j = 0; j < aDimV; ++j)
    {
      const gp_XYZ& aPole = myPoles.Value(aLR + i, aLC + j).XYZ();
      const double  aWij  = myWeights.Value(aWLR + i, aWLC + j);
      for (int du = 0; du <= Nu; ++du)
      {
        for (int dv = 0; dv <= Nv; ++dv)
        {
          const double aVal = aWij * aBUDerivs.Value(du).Value(i)
                                   * aBVDerivs.Value(dv).Value(j);
          aNArr.ChangeValue(du).ChangeValue(dv) += aPole * aVal;
          aWDArr.ChangeValue(du).ChangeValue(dv) += aVal;
        }
      }
    }
  }

  // Compute C^(du,dv) recursively.
  const double aInvW = 1.0 / aWDArr.Value(0).Value(0);
  NCollection_Array1<NCollection_Array1<gp_XYZ>> aCArr(0, Nu);
  for (int du = 0; du <= Nu; ++du)
  {
    aCArr.ChangeValue(du).Resize(0, Nv, false);
  }

  for (int aOrd = 0; aOrd <= aMaxOrd; ++aOrd)
  {
    for (int du = 0; du <= std::min(aOrd, Nu); ++du)
    {
      const int dv = aOrd - du;
      if (dv > Nv)
      {
        continue;
      }
      gp_XYZ aSum = aNArr.Value(du).Value(dv);
      for (int p = 0; p <= du; ++p)
      {
        for (int q = 0; q <= dv; ++q)
        {
          if (p == 0 && q == 0)
          {
            continue;
          }
          double aBinomU = 1.0;
          for (int m = 0; m < p; ++m)
          {
            aBinomU = aBinomU * (du - m) / (m + 1);
          }
          double aBinomV = 1.0;
          for (int m = 0; m < q; ++m)
          {
            aBinomV = aBinomV * (dv - m) / (m + 1);
          }
          aSum -= aCArr.Value(du - p).Value(dv - q)
                  * (aBinomU * aBinomV * aWDArr.Value(p).Value(q));
        }
      }
      aCArr.ChangeValue(du).ChangeValue(dv) = aSum * aInvW;
    }
  }
  return gp_Vec(aCArr.Value(Nu).Value(Nv));
}

//==================================================================================================

void GeomEval_AHTBezierSurface::Transform(const gp_Trsf& T)
{
  // The tensor-product AHT basis does not form a partition of unity.
  // Only the (LowerRow, LowerCol) pole corresponds to the constant basis 1*1 = 1.
  // That pole gets the full affine transform; all others get only the linear part.
  gp_Pnt anOrigin(0.0, 0.0, 0.0);
  anOrigin.Transform(T);
  const gp_XYZ aTransVec = anOrigin.XYZ();

  for (int i = myPoles.LowerRow(); i <= myPoles.UpperRow(); ++i)
  {
    for (int j = myPoles.LowerCol(); j <= myPoles.UpperCol(); ++j)
    {
      myPoles.ChangeValue(i, j).Transform(T);
    }
  }
  for (int i = myPoles.LowerRow(); i <= myPoles.UpperRow(); ++i)
  {
    for (int j = myPoles.LowerCol(); j <= myPoles.UpperCol(); ++j)
    {
      if (i == myPoles.LowerRow() && j == myPoles.LowerCol())
      {
        continue;
      }
      myPoles.ChangeValue(i, j).SetXYZ(myPoles.Value(i, j).XYZ() - aTransVec);
    }
  }
}

//==================================================================================================

occ::handle<Geom_Geometry> GeomEval_AHTBezierSurface::Copy() const
{
  if (myURational || myVRational)
  {
    return new GeomEval_AHTBezierSurface(myPoles, myWeights,
                                          myAlgDegreeU, myAlgDegreeV,
                                          myAlphaU, myAlphaV,
                                          myBetaU, myBetaV);
  }
  return new GeomEval_AHTBezierSurface(myPoles,
                                        myAlgDegreeU, myAlgDegreeV,
                                        myAlphaU, myAlphaV,
                                        myBetaU, myBetaV);
}

//==================================================================================================

void GeomEval_AHTBezierSurface::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_BoundedSurface)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myAlgDegreeU)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myAlgDegreeV)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myAlphaU)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myAlphaV)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myBetaU)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myBetaV)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myURational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myVRational)

  for (int i = myPoles.LowerRow(); i <= myPoles.UpperRow(); ++i)
  {
    for (int j = myPoles.LowerCol(); j <= myPoles.UpperCol(); ++j)
    {
      gp_Pnt aPole = myPoles.Value(i, j);
      OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &aPole)
    }
  }
}
