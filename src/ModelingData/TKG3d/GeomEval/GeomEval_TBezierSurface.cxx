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

#include <GeomEval_TBezierSurface.hxx>

#include <NCollection_LocalArray.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Dump.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomEval_TBezierSurface, Geom_BoundedSurface)

namespace
{
  //! Validate that a dimension count is odd and >= 3.
  void validateDimension(int theSize, const char* theName)
  {
    if (theSize < 3 || (theSize % 2) == 0)
    {
      TCollection_AsciiString aMsg("GeomEval_TBezierSurface: ");
      aMsg += theName;
      aMsg += " pole count must be odd and >= 3";
      throw Standard_ConstructionError(aMsg.ToCString());
    }
  }

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
  void evalTrigAxisDerivs(const double theT,
                          const double theAlpha,
                          const int    theOrder,
                          double*      theDerivs)
  {
    const int aDim = 2 * theOrder + 1;
    for (int d = 0; d <= theMaxOrder; ++d)
    {
      theDerivs[d * aDim] = (d == 0) ? 1.0 : 0.0;
    }

    int aIdx = 1;
    for (int k = 1; k <= theOrder; ++k, aIdx += 2)
    {
      const double aFreq  = double(k) * theAlpha;
      const double anArg  = aFreq * theT;
      const double aSin   = std::sin(anArg);
      const double aCos   = std::cos(anArg);
      const double aFreq2 = aFreq * aFreq;
      const double aFreq3 = aFreq2 * aFreq;

      theDerivs[aIdx]     = aSin;
      theDerivs[aIdx + 1] = aCos;
      if constexpr (theMaxOrder >= 1)
      {
        theDerivs[aDim + aIdx]     = aFreq * aCos;
        theDerivs[aDim + aIdx + 1] = -aFreq * aSin;
      }
      if constexpr (theMaxOrder >= 2)
      {
        theDerivs[2 * aDim + aIdx]     = -aFreq2 * aSin;
        theDerivs[2 * aDim + aIdx + 1] = -aFreq2 * aCos;
      }
      if constexpr (theMaxOrder >= 3)
      {
        theDerivs[3 * aDim + aIdx]     = -aFreq3 * aCos;
        theDerivs[3 * aDim + aIdx + 1] = aFreq3 * aSin;
      }
    }
  }

  void evalTrigAxisNthDeriv(const double theT,
                            const double theAlpha,
                            const int    theOrder,
                            const int    theDerivOrder,
                            double*      theDerivs)
  {
    theDerivs[0] = (theDerivOrder == 0) ? 1.0 : 0.0;
    if (theDerivOrder == 0)
    {
      int aIdx = 1;
      for (int k = 1; k <= theOrder; ++k, aIdx += 2)
      {
        const double anArg = double(k) * theAlpha * theT;
        theDerivs[aIdx]     = std::sin(anArg);
        theDerivs[aIdx + 1] = std::cos(anArg);
      }
      return;
    }

    const int aOrderMod4 = theDerivOrder & 3;
    int       aIdx       = 1;
    for (int k = 1; k <= theOrder; ++k, aIdx += 2)
    {
      const double aFreq = double(k) * theAlpha;
      const double anArg = aFreq * theT;
      const double aSin  = std::sin(anArg);
      const double aCos  = std::cos(anArg);
      const double aPow  = powInt(aFreq, theDerivOrder);
      double       aSinDeriv = 0.0;
      double       aCosDeriv = 0.0;
      evalTrigPairNthDeriv(aSin, aCos, aOrderMod4, aSinDeriv, aCosDeriv);
      theDerivs[aIdx]     = aPow * aSinDeriv;
      theDerivs[aIdx + 1] = aPow * aCosDeriv;
    }
  }
}

//==================================================================================================

GeomEval_TBezierSurface::GeomEval_TBezierSurface(const NCollection_Array2<gp_Pnt>& thePoles,
                                                  double theAlphaU,
                                                  double theAlphaV)
    : myPoles(thePoles),
      myAlphaU(theAlphaU),
      myAlphaV(theAlphaV),
      myRational(false)
{
  validateDimension(thePoles.NbRows(), "U");
  validateDimension(thePoles.NbColumns(), "V");
  if (theAlphaU <= 0.0)
  {
    throw Standard_ConstructionError(
      "GeomEval_TBezierSurface: alphaU must be > 0");
  }
  if (theAlphaV <= 0.0)
  {
    throw Standard_ConstructionError(
      "GeomEval_TBezierSurface: alphaV must be > 0");
  }
}

//==================================================================================================

GeomEval_TBezierSurface::GeomEval_TBezierSurface(const NCollection_Array2<gp_Pnt>& thePoles,
                                                  const NCollection_Array2<double>& theWeights,
                                                  double theAlphaU,
                                                  double theAlphaV)
    : myPoles(thePoles),
      myWeights(theWeights),
      myAlphaU(theAlphaU),
      myAlphaV(theAlphaV),
      myRational(true)
{
  validateDimension(thePoles.NbRows(), "U");
  validateDimension(thePoles.NbColumns(), "V");
  if (theAlphaU <= 0.0)
  {
    throw Standard_ConstructionError(
      "GeomEval_TBezierSurface: alphaU must be > 0");
  }
  if (theAlphaV <= 0.0)
  {
    throw Standard_ConstructionError(
      "GeomEval_TBezierSurface: alphaV must be > 0");
  }
  if (theWeights.NbRows() != thePoles.NbRows()
      || theWeights.NbColumns() != thePoles.NbColumns())
  {
    throw Standard_ConstructionError(
      "GeomEval_TBezierSurface: weights dimensions must match poles dimensions");
  }
  for (int i = theWeights.LowerRow(); i <= theWeights.UpperRow(); ++i)
  {
    for (int j = theWeights.LowerCol(); j <= theWeights.UpperCol(); ++j)
    {
      if (theWeights.Value(i, j) <= 0.0)
      {
        throw Standard_ConstructionError(
          "GeomEval_TBezierSurface: all weights must be > 0");
      }
    }
  }
}

//==================================================================================================

const NCollection_Array2<gp_Pnt>& GeomEval_TBezierSurface::Poles() const
{
  return myPoles;
}

//==================================================================================================

const NCollection_Array2<double>& GeomEval_TBezierSurface::Weights() const
{
  return myWeights;
}

//==================================================================================================

double GeomEval_TBezierSurface::AlphaU() const
{
  return myAlphaU;
}

//==================================================================================================

double GeomEval_TBezierSurface::AlphaV() const
{
  return myAlphaV;
}

//==================================================================================================

int GeomEval_TBezierSurface::NbUPoles() const
{
  return myPoles.NbRows();
}

//==================================================================================================

int GeomEval_TBezierSurface::NbVPoles() const
{
  return myPoles.NbColumns();
}

//==================================================================================================

int GeomEval_TBezierSurface::OrderU() const
{
  return (myPoles.NbRows() - 1) / 2;
}

//==================================================================================================

int GeomEval_TBezierSurface::OrderV() const
{
  return (myPoles.NbColumns() - 1) / 2;
}

//==================================================================================================

bool GeomEval_TBezierSurface::IsRational() const
{
  return myRational;
}

//==================================================================================================

void GeomEval_TBezierSurface::UReverse()
{
  throw Standard_NotImplemented("GeomEval_TBezierSurface::UReverse");
}

//==================================================================================================

double GeomEval_TBezierSurface::UReversedParameter(const double U) const
{
  (void)U;
  throw Standard_NotImplemented("GeomEval_TBezierSurface::UReversedParameter");
}

//==================================================================================================

void GeomEval_TBezierSurface::VReverse()
{
  throw Standard_NotImplemented("GeomEval_TBezierSurface::VReverse");
}

//==================================================================================================

double GeomEval_TBezierSurface::VReversedParameter(const double V) const
{
  (void)V;
  throw Standard_NotImplemented("GeomEval_TBezierSurface::VReversedParameter");
}

//==================================================================================================

void GeomEval_TBezierSurface::Bounds(double& U1, double& U2, double& V1, double& V2) const
{
  U1 = 0.0;
  U2 = M_PI / myAlphaU;
  V1 = 0.0;
  V2 = M_PI / myAlphaV;
}

//==================================================================================================

bool GeomEval_TBezierSurface::IsUClosed() const
{
  // Check if the first and last U-rows of poles coincide.
  const int aLR = myPoles.LowerRow();
  const int aUR = myPoles.UpperRow();
  for (int j = myPoles.LowerCol(); j <= myPoles.UpperCol(); ++j)
  {
    if (myPoles.Value(aLR, j).Distance(myPoles.Value(aUR, j)) > Precision::Confusion())
    {
      return false;
    }
  }
  return true;
}

//==================================================================================================

bool GeomEval_TBezierSurface::IsVClosed() const
{
  // Check if the first and last V-columns of poles coincide.
  const int aLC = myPoles.LowerCol();
  const int aUC = myPoles.UpperCol();
  for (int i = myPoles.LowerRow(); i <= myPoles.UpperRow(); ++i)
  {
    if (myPoles.Value(i, aLC).Distance(myPoles.Value(i, aUC)) > Precision::Confusion())
    {
      return false;
    }
  }
  return true;
}

//==================================================================================================

bool GeomEval_TBezierSurface::IsUPeriodic() const
{
  return false;
}

//==================================================================================================

bool GeomEval_TBezierSurface::IsVPeriodic() const
{
  return false;
}

//==================================================================================================

GeomAbs_Shape GeomEval_TBezierSurface::Continuity() const
{
  return GeomAbs_CN;
}

//==================================================================================================

bool GeomEval_TBezierSurface::IsCNu(int /*N*/) const
{
  return true;
}

//==================================================================================================

bool GeomEval_TBezierSurface::IsCNv(int /*N*/) const
{
  return true;
}

//==================================================================================================

occ::handle<Geom_Curve> GeomEval_TBezierSurface::UIso(const double /*U*/) const
{
  throw Standard_NotImplemented("GeomEval_TBezierSurface::UIso");
}

//==================================================================================================

occ::handle<Geom_Curve> GeomEval_TBezierSurface::VIso(const double /*V*/) const
{
  throw Standard_NotImplemented("GeomEval_TBezierSurface::VIso");
}

//==================================================================================================

void GeomEval_TBezierSurface::evalTrigBasis(double                      theT,
                                            double                      theAlpha,
                                            int                         theOrder,
                                            NCollection_Array1<double>& theBasis)
{
  // T_0(t) = 1
  theBasis.SetValue(theBasis.Lower(), 1.0);
  for (int k = 1; k <= theOrder; ++k)
  {
    const double anArg = k * theAlpha * theT;
    theBasis.SetValue(theBasis.Lower() + 2 * k - 1, std::sin(anArg));
    theBasis.SetValue(theBasis.Lower() + 2 * k, std::cos(anArg));
  }
}

//==================================================================================================

void GeomEval_TBezierSurface::evalTrigBasisDeriv(double                      theT,
                                                  double                      theAlpha,
                                                  int                         theOrder,
                                                  int                         theDerivOrder,
                                                  NCollection_Array1<double>& theBasisDeriv)
{
  const int aDim = 2 * theOrder + 1;
  NCollection_LocalArray<double, 16> aDerivs(aDim);
  evalTrigAxisNthDeriv(theT, theAlpha, theOrder, theDerivOrder, aDerivs);
  for (int i = 0; i < aDim; ++i)
  {
    theBasisDeriv.SetValue(theBasisDeriv.Lower() + i, aDerivs[i]);
  }
}

//==================================================================================================

void GeomEval_TBezierSurface::evalBasisU(double                      theU,
                                         NCollection_Array1<double>& theBasis) const
{
  evalTrigBasis(theU, myAlphaU, OrderU(), theBasis);
}

//==================================================================================================

void GeomEval_TBezierSurface::evalBasisV(double                      theV,
                                         NCollection_Array1<double>& theBasis) const
{
  evalTrigBasis(theV, myAlphaV, OrderV(), theBasis);
}

//==================================================================================================

void GeomEval_TBezierSurface::evalBasisDerivU(double                      theU,
                                              int                         theOrder,
                                              NCollection_Array1<double>& theBasisDeriv) const
{
  evalTrigBasisDeriv(theU, myAlphaU, OrderU(), theOrder, theBasisDeriv);
}

//==================================================================================================

void GeomEval_TBezierSurface::evalBasisDerivV(double                      theV,
                                              int                         theOrder,
                                              NCollection_Array1<double>& theBasisDeriv) const
{
  evalTrigBasisDeriv(theV, myAlphaV, OrderV(), theOrder, theBasisDeriv);
}

//==================================================================================================

gp_Pnt GeomEval_TBezierSurface::EvalD0(const double U, const double V) const
{
  const int aNbU = myPoles.NbRows();
  const int aNbV = myPoles.NbColumns();
  NCollection_LocalArray<double, 16> aBasisU(aNbU);
  NCollection_LocalArray<double, 16> aBasisV(aNbV);
  evalTrigAxisDerivs<0>(U, myAlphaU, (aNbU - 1) / 2, aBasisU);
  evalTrigAxisDerivs<0>(V, myAlphaV, (aNbV - 1) / 2, aBasisV);

  if (!myRational)
  {
    gp_XYZ aSum(0.0, 0.0, 0.0);
    for (int i = 0; i < aNbU; ++i)
    {
      const double aBu = aBasisU[i];
      for (int j = 0; j < aNbV; ++j)
      {
        aSum += (aBu * aBasisV[j])
                * myPoles.Value(myPoles.LowerRow() + i, myPoles.LowerCol() + j).XYZ();
      }
    }
    return gp_Pnt(aSum);
  }

  // Rational evaluation.
  gp_XYZ aNumer(0.0, 0.0, 0.0);
  double  aDenom = 0.0;
  for (int i = 0; i < aNbU; ++i)
  {
    const double aBu = aBasisU[i];
    for (int j = 0; j < aNbV; ++j)
    {
      const double aWBuBv = myWeights.Value(myWeights.LowerRow() + i, myWeights.LowerCol() + j)
                          * aBu * aBasisV[j];
      aNumer += aWBuBv * myPoles.Value(myPoles.LowerRow() + i, myPoles.LowerCol() + j).XYZ();
      aDenom += aWBuBv;
    }
  }
  return gp_Pnt(aNumer / aDenom);
}

//==================================================================================================

Geom_Surface::ResD1 GeomEval_TBezierSurface::EvalD1(const double U, const double V) const
{
  const int aNbU = myPoles.NbRows();
  const int aNbV = myPoles.NbColumns();
  NCollection_LocalArray<double, 16> aBUDerivs((1 + 1) * aNbU);
  NCollection_LocalArray<double, 16> aBVDerivs((1 + 1) * aNbV);
  evalTrigAxisDerivs<1>(U, myAlphaU, (aNbU - 1) / 2, aBUDerivs);
  evalTrigAxisDerivs<1>(V, myAlphaV, (aNbV - 1) / 2, aBVDerivs);

  const double* aBasisU  = aBUDerivs;
  const double* aBasisDU = aBUDerivs + aNbU;
  const double* aBasisV  = aBVDerivs;
  const double* aBasisDV = aBVDerivs + aNbV;

  Geom_Surface::ResD1 aResult;

  if (!myRational)
  {
    gp_XYZ aS(0.0, 0.0, 0.0);
    gp_XYZ aDU(0.0, 0.0, 0.0);
    gp_XYZ aDV(0.0, 0.0, 0.0);
    for (int i = 0; i < aNbU; ++i)
    {
      const double aBu  = aBasisU[i];
      const double aBuD = aBasisDU[i];
      for (int j = 0; j < aNbV; ++j)
      {
        const double aBv  = aBasisV[j];
        const double aBvD = aBasisDV[j];
        const gp_XYZ& aPij = myPoles.Value(myPoles.LowerRow() + i,
                                            myPoles.LowerCol() + j).XYZ();
        aS  += (aBu * aBv)  * aPij;
        aDU += (aBuD * aBv) * aPij;
        aDV += (aBu * aBvD) * aPij;
      }
    }
    aResult.Point = gp_Pnt(aS);
    aResult.D1U   = gp_Vec(aDU);
    aResult.D1V   = gp_Vec(aDV);
    return aResult;
  }

  // Rational: S = Sw/w, dS/du = (Sw_u - w_u*S)/w, dS/dv = (Sw_v - w_v*S)/w
  gp_XYZ aSw(0.0, 0.0, 0.0);
  gp_XYZ aSwDU(0.0, 0.0, 0.0);
  gp_XYZ aSwDV(0.0, 0.0, 0.0);
  double  aW   = 0.0;
  double  aWDU = 0.0;
  double  aWDV = 0.0;
  for (int i = 0; i < aNbU; ++i)
  {
    const double aBu  = aBasisU[i];
    const double aBuD = aBasisDU[i];
    for (int j = 0; j < aNbV; ++j)
    {
      const double aBv  = aBasisV[j];
      const double aBvD = aBasisDV[j];
      const double aWij = myWeights.Value(myWeights.LowerRow() + i, myWeights.LowerCol() + j);
      const gp_XYZ& aPij = myPoles.Value(myPoles.LowerRow() + i,
                                          myPoles.LowerCol() + j).XYZ();
      const double aWBuBv   = aWij * aBu  * aBv;
      const double aWBuDBv  = aWij * aBuD * aBv;
      const double aWBuBvD  = aWij * aBu  * aBvD;
      aSw   += aWBuBv  * aPij;
      aSwDU += aWBuDBv * aPij;
      aSwDV += aWBuBvD * aPij;
      aW    += aWBuBv;
      aWDU  += aWBuDBv;
      aWDV  += aWBuBvD;
    }
  }

  const gp_XYZ aS = aSw / aW;
  aResult.Point = gp_Pnt(aS);
  aResult.D1U   = gp_Vec((aSwDU - aWDU * aS) / aW);
  aResult.D1V   = gp_Vec((aSwDV - aWDV * aS) / aW);
  return aResult;
}

//==================================================================================================

Geom_Surface::ResD2 GeomEval_TBezierSurface::EvalD2(const double U, const double V) const
{
  const int aNbU = myPoles.NbRows();
  const int aNbV = myPoles.NbColumns();
  NCollection_LocalArray<double, 16> aBUDerivs((2 + 1) * aNbU);
  NCollection_LocalArray<double, 16> aBVDerivs((2 + 1) * aNbV);
  evalTrigAxisDerivs<2>(U, myAlphaU, (aNbU - 1) / 2, aBUDerivs);
  evalTrigAxisDerivs<2>(V, myAlphaV, (aNbV - 1) / 2, aBVDerivs);

  const double* aBasisU  = aBUDerivs;
  const double* aBasisDU = aBUDerivs + aNbU;
  const double* aBasisD2U = aBUDerivs + 2 * aNbU;
  const double* aBasisV  = aBVDerivs;
  const double* aBasisDV = aBVDerivs + aNbV;
  const double* aBasisD2V = aBVDerivs + 2 * aNbV;

  Geom_Surface::ResD2 aResult;

  if (!myRational)
  {
    gp_XYZ aS(0.0, 0.0, 0.0);
    gp_XYZ aDU(0.0, 0.0, 0.0);
    gp_XYZ aDV(0.0, 0.0, 0.0);
    gp_XYZ aD2U(0.0, 0.0, 0.0);
    gp_XYZ aD2V(0.0, 0.0, 0.0);
    gp_XYZ aD2UV(0.0, 0.0, 0.0);
    for (int i = 0; i < aNbU; ++i)
    {
      const double aBu   = aBasisU[i];
      const double aBuD  = aBasisDU[i];
      const double aBuD2 = aBasisD2U[i];
      for (int j = 0; j < aNbV; ++j)
      {
        const double aBv   = aBasisV[j];
        const double aBvD  = aBasisDV[j];
        const double aBvD2 = aBasisD2V[j];
        const gp_XYZ& aPij = myPoles.Value(myPoles.LowerRow() + i,
                                            myPoles.LowerCol() + j).XYZ();
        aS    += (aBu   * aBv)   * aPij;
        aDU   += (aBuD  * aBv)   * aPij;
        aDV   += (aBu   * aBvD)  * aPij;
        aD2U  += (aBuD2 * aBv)   * aPij;
        aD2V  += (aBu   * aBvD2) * aPij;
        aD2UV += (aBuD  * aBvD)  * aPij;
      }
    }
    aResult.Point = gp_Pnt(aS);
    aResult.D1U   = gp_Vec(aDU);
    aResult.D1V   = gp_Vec(aDV);
    aResult.D2U   = gp_Vec(aD2U);
    aResult.D2V   = gp_Vec(aD2V);
    aResult.D2UV  = gp_Vec(aD2UV);
    return aResult;
  }

  // Rational second derivatives using quotient rule.
  gp_XYZ aSw(0.0, 0.0, 0.0);
  gp_XYZ aSwDU(0.0, 0.0, 0.0);
  gp_XYZ aSwDV(0.0, 0.0, 0.0);
  gp_XYZ aSwD2U(0.0, 0.0, 0.0);
  gp_XYZ aSwD2V(0.0, 0.0, 0.0);
  gp_XYZ aSwDUV(0.0, 0.0, 0.0);
  double  aW     = 0.0;
  double  aWDU   = 0.0;
  double  aWDV   = 0.0;
  double  aWD2U  = 0.0;
  double  aWD2V  = 0.0;
  double  aWDUV  = 0.0;
  for (int i = 0; i < aNbU; ++i)
  {
    const double aBu   = aBasisU[i];
    const double aBuD  = aBasisDU[i];
    const double aBuD2 = aBasisD2U[i];
    for (int j = 0; j < aNbV; ++j)
    {
      const double aBv   = aBasisV[j];
      const double aBvD  = aBasisDV[j];
      const double aBvD2 = aBasisD2V[j];
      const double aWij  = myWeights.Value(myWeights.LowerRow() + i, myWeights.LowerCol() + j);
      const gp_XYZ& aPij = myPoles.Value(myPoles.LowerRow() + i,
                                          myPoles.LowerCol() + j).XYZ();

      aSw    += (aWij * aBu   * aBv)   * aPij;
      aSwDU  += (aWij * aBuD  * aBv)   * aPij;
      aSwDV  += (aWij * aBu   * aBvD)  * aPij;
      aSwD2U += (aWij * aBuD2 * aBv)   * aPij;
      aSwD2V += (aWij * aBu   * aBvD2) * aPij;
      aSwDUV += (aWij * aBuD  * aBvD)  * aPij;
      aW     += aWij * aBu   * aBv;
      aWDU   += aWij * aBuD  * aBv;
      aWDV   += aWij * aBu   * aBvD;
      aWD2U  += aWij * aBuD2 * aBv;
      aWD2V  += aWij * aBu   * aBvD2;
      aWDUV  += aWij * aBuD  * aBvD;
    }
  }

  // S = Sw / w
  const gp_XYZ aS = aSw / aW;
  // S_u = (Sw_u - w_u * S) / w
  const gp_XYZ aSD1U = (aSwDU - aWDU * aS) / aW;
  // S_v = (Sw_v - w_v * S) / w
  const gp_XYZ aSD1V = (aSwDV - aWDV * aS) / aW;
  // S_uu = (Sw_uu - 2*w_u*S_u - w_uu*S) / w
  const gp_XYZ aSD2U = (aSwD2U - 2.0 * aWDU * aSD1U - aWD2U * aS) / aW;
  // S_vv = (Sw_vv - 2*w_v*S_v - w_vv*S) / w
  const gp_XYZ aSD2V = (aSwD2V - 2.0 * aWDV * aSD1V - aWD2V * aS) / aW;
  // S_uv = (Sw_uv - w_u*S_v - w_v*S_u - w_uv*S) / w
  const gp_XYZ aSD2UV = (aSwDUV - aWDU * aSD1V - aWDV * aSD1U - aWDUV * aS) / aW;

  aResult.Point = gp_Pnt(aS);
  aResult.D1U   = gp_Vec(aSD1U);
  aResult.D1V   = gp_Vec(aSD1V);
  aResult.D2U   = gp_Vec(aSD2U);
  aResult.D2V   = gp_Vec(aSD2V);
  aResult.D2UV  = gp_Vec(aSD2UV);
  return aResult;
}

//==================================================================================================

Geom_Surface::ResD3 GeomEval_TBezierSurface::EvalD3(const double U, const double V) const
{
  const int aNbU = myPoles.NbRows();
  const int aNbV = myPoles.NbColumns();
  NCollection_LocalArray<double, 16> aBUDerivs((3 + 1) * aNbU);
  NCollection_LocalArray<double, 16> aBVDerivs((3 + 1) * aNbV);
  evalTrigAxisDerivs<3>(U, myAlphaU, (aNbU - 1) / 2, aBUDerivs);
  evalTrigAxisDerivs<3>(V, myAlphaV, (aNbV - 1) / 2, aBVDerivs);

  const double* aBasisU   = aBUDerivs;
  const double* aBasisDU  = aBUDerivs + aNbU;
  const double* aBasisD2U = aBUDerivs + 2 * aNbU;
  const double* aBasisD3U = aBUDerivs + 3 * aNbU;
  const double* aBasisV   = aBVDerivs;
  const double* aBasisDV  = aBVDerivs + aNbV;
  const double* aBasisD2V = aBVDerivs + 2 * aNbV;
  const double* aBasisD3V = aBVDerivs + 3 * aNbV;

  Geom_Surface::ResD3 aResult;

  if (!myRational)
  {
    gp_XYZ aS(0.0, 0.0, 0.0);
    gp_XYZ aDU(0.0, 0.0, 0.0);
    gp_XYZ aDV(0.0, 0.0, 0.0);
    gp_XYZ aD2U(0.0, 0.0, 0.0);
    gp_XYZ aD2V(0.0, 0.0, 0.0);
    gp_XYZ aD2UV(0.0, 0.0, 0.0);
    gp_XYZ aD3U(0.0, 0.0, 0.0);
    gp_XYZ aD3V(0.0, 0.0, 0.0);
    gp_XYZ aD3UUV(0.0, 0.0, 0.0);
    gp_XYZ aD3UVV(0.0, 0.0, 0.0);

    for (int i = 0; i < aNbU; ++i)
    {
      const double aBu   = aBasisU[i];
      const double aBuD  = aBasisDU[i];
      const double aBuD2 = aBasisD2U[i];
      const double aBuD3 = aBasisD3U[i];
      for (int j = 0; j < aNbV; ++j)
      {
        const double aBv   = aBasisV[j];
        const double aBvD  = aBasisDV[j];
        const double aBvD2 = aBasisD2V[j];
        const double aBvD3 = aBasisD3V[j];
        const gp_XYZ& aPij = myPoles.Value(myPoles.LowerRow() + i,
                                            myPoles.LowerCol() + j).XYZ();

        aS     += (aBu   * aBv)   * aPij;
        aDU    += (aBuD  * aBv)   * aPij;
        aDV    += (aBu   * aBvD)  * aPij;
        aD2U   += (aBuD2 * aBv)   * aPij;
        aD2V   += (aBu   * aBvD2) * aPij;
        aD2UV  += (aBuD  * aBvD)  * aPij;
        aD3U   += (aBuD3 * aBv)   * aPij;
        aD3V   += (aBu   * aBvD3) * aPij;
        aD3UUV += (aBuD2 * aBvD)  * aPij;
        aD3UVV += (aBuD  * aBvD2) * aPij;
      }
    }
    aResult.Point = gp_Pnt(aS);
    aResult.D1U   = gp_Vec(aDU);
    aResult.D1V   = gp_Vec(aDV);
    aResult.D2U   = gp_Vec(aD2U);
    aResult.D2V   = gp_Vec(aD2V);
    aResult.D2UV  = gp_Vec(aD2UV);
    aResult.D3U   = gp_Vec(aD3U);
    aResult.D3V   = gp_Vec(aD3V);
    aResult.D3UUV = gp_Vec(aD3UUV);
    aResult.D3UVV = gp_Vec(aD3UVV);
    return aResult;
  }

  // Rational third derivatives: accumulate all weighted sums.
  gp_XYZ aSw(0.0, 0.0, 0.0);
  gp_XYZ aSwDU(0.0, 0.0, 0.0);
  gp_XYZ aSwDV(0.0, 0.0, 0.0);
  gp_XYZ aSwD2U(0.0, 0.0, 0.0);
  gp_XYZ aSwD2V(0.0, 0.0, 0.0);
  gp_XYZ aSwDUV(0.0, 0.0, 0.0);
  gp_XYZ aSwD3U(0.0, 0.0, 0.0);
  gp_XYZ aSwD3V(0.0, 0.0, 0.0);
  gp_XYZ aSwD2UDV(0.0, 0.0, 0.0);
  gp_XYZ aSwDUD2V(0.0, 0.0, 0.0);
  double  aW      = 0.0;
  double  aWDU    = 0.0;
  double  aWDV    = 0.0;
  double  aWD2U   = 0.0;
  double  aWD2V   = 0.0;
  double  aWDUV   = 0.0;
  double  aWD3U   = 0.0;
  double  aWD3V   = 0.0;
  double  aWD2UDV = 0.0;
  double  aWDUD2V = 0.0;

  for (int i = 0; i < aNbU; ++i)
  {
    const double aBu   = aBasisU[i];
    const double aBuD  = aBasisDU[i];
    const double aBuD2 = aBasisD2U[i];
    const double aBuD3 = aBasisD3U[i];
    for (int j = 0; j < aNbV; ++j)
    {
      const double aBv   = aBasisV[j];
      const double aBvD  = aBasisDV[j];
      const double aBvD2 = aBasisD2V[j];
      const double aBvD3 = aBasisD3V[j];
      const double aWij  = myWeights.Value(myWeights.LowerRow() + i, myWeights.LowerCol() + j);
      const gp_XYZ& aPij = myPoles.Value(myPoles.LowerRow() + i,
                                          myPoles.LowerCol() + j).XYZ();

      aSw      += (aWij * aBu   * aBv)   * aPij;
      aSwDU    += (aWij * aBuD  * aBv)   * aPij;
      aSwDV    += (aWij * aBu   * aBvD)  * aPij;
      aSwD2U   += (aWij * aBuD2 * aBv)   * aPij;
      aSwD2V   += (aWij * aBu   * aBvD2) * aPij;
      aSwDUV   += (aWij * aBuD  * aBvD)  * aPij;
      aSwD3U   += (aWij * aBuD3 * aBv)   * aPij;
      aSwD3V   += (aWij * aBu   * aBvD3) * aPij;
      aSwD2UDV += (aWij * aBuD2 * aBvD)  * aPij;
      aSwDUD2V += (aWij * aBuD  * aBvD2) * aPij;
      aW       += aWij * aBu   * aBv;
      aWDU     += aWij * aBuD  * aBv;
      aWDV     += aWij * aBu   * aBvD;
      aWD2U    += aWij * aBuD2 * aBv;
      aWD2V    += aWij * aBu   * aBvD2;
      aWDUV    += aWij * aBuD  * aBvD;
      aWD3U    += aWij * aBuD3 * aBv;
      aWD3V    += aWij * aBu   * aBvD3;
      aWD2UDV  += aWij * aBuD2 * aBvD;
      aWDUD2V  += aWij * aBuD  * aBvD2;
    }
  }

  // S = Sw / w
  const gp_XYZ aS = aSw / aW;
  // First derivatives
  const gp_XYZ aSD1U = (aSwDU - aWDU * aS) / aW;
  const gp_XYZ aSD1V = (aSwDV - aWDV * aS) / aW;
  // Second derivatives
  const gp_XYZ aSD2U  = (aSwD2U - 2.0 * aWDU * aSD1U - aWD2U * aS) / aW;
  const gp_XYZ aSD2V  = (aSwD2V - 2.0 * aWDV * aSD1V - aWD2V * aS) / aW;
  const gp_XYZ aSD2UV = (aSwDUV - aWDU * aSD1V - aWDV * aSD1U - aWDUV * aS) / aW;
  // Third derivatives: S_uuu, S_vvv, S_uuv, S_uvv
  // S_uuu = (Sw_uuu - 3*w_u*S_uu - 3*w_uu*S_u - w_uuu*S) / w
  const gp_XYZ aSD3U =
    (aSwD3U - 3.0 * aWDU * aSD2U - 3.0 * aWD2U * aSD1U - aWD3U * aS) / aW;
  // S_vvv = (Sw_vvv - 3*w_v*S_vv - 3*w_vv*S_v - w_vvv*S) / w
  const gp_XYZ aSD3V =
    (aSwD3V - 3.0 * aWDV * aSD2V - 3.0 * aWD2V * aSD1V - aWD3V * aS) / aW;
  // S_uuv = (Sw_uuv - w_uuv*S - 2*w_u*S_uv - w_v*S_uu - 2*w_uv*S_u - w_uu*S_v) / w
  // Using the general formula for mixed partials:
  // d/dv[S_uu] = (Sw_uuv - w_v*S_uu - 2*w_uv*S_u - 2*w_u*S_uv - w_uu*S_v - w_uuv*S) / w
  // Wait, let's apply the product rule to S_uu carefully.
  // Actually the standard recurrence for d^(p+q)/du^p dv^q of a rational function is complex.
  // Use the direct formula:
  // S_uuv = (Sw_uuv - w_uuv*S - 2*w_u*S_uv - w_v*S_uu - w_uu*S_v - 2*w_uv*S_u) / w
  // This follows from differentiating w*S_uu = Sw_uu - 2*w_u*S_u - w_uu*S with respect to v.
  const gp_XYZ aSD3UUV =
    (aSwD2UDV - aWD2UDV * aS - 2.0 * aWDU * aSD2UV - aWDV * aSD2U
     - aWD2U * aSD1V - 2.0 * aWDUV * aSD1U)
    / aW;
  // S_uvv: differentiate w*S_vv = Sw_vv - 2*w_v*S_v - w_vv*S with respect to u.
  const gp_XYZ aSD3UVV =
    (aSwDUD2V - aWDUD2V * aS - 2.0 * aWDV * aSD2UV - aWDU * aSD2V
     - aWD2V * aSD1U - 2.0 * aWDUV * aSD1V)
    / aW;

  aResult.Point = gp_Pnt(aS);
  aResult.D1U   = gp_Vec(aSD1U);
  aResult.D1V   = gp_Vec(aSD1V);
  aResult.D2U   = gp_Vec(aSD2U);
  aResult.D2V   = gp_Vec(aSD2V);
  aResult.D2UV  = gp_Vec(aSD2UV);
  aResult.D3U   = gp_Vec(aSD3U);
  aResult.D3V   = gp_Vec(aSD3V);
  aResult.D3UUV = gp_Vec(aSD3UUV);
  aResult.D3UVV = gp_Vec(aSD3UVV);
  return aResult;
}

//==================================================================================================

gp_Vec GeomEval_TBezierSurface::EvalDN(const double U,
                                       const double V,
                                       const int    Nu,
                                       const int    Nv) const
{
  if (Nu + Nv < 1 || Nu < 0 || Nv < 0)
  {
    throw Geom_UndefinedDerivative(
      "GeomEval_TBezierSurface::EvalDN: invalid derivative order");
  }

  // For non-rational surfaces, compute directly from basis derivatives.
  if (!myRational)
  {
    const int aNbU = myPoles.NbRows();
    const int aNbV = myPoles.NbColumns();
    NCollection_LocalArray<double, 16> aBasisU(aNbU);
    NCollection_LocalArray<double, 16> aBasisV(aNbV);
    evalTrigAxisNthDeriv(U, myAlphaU, (aNbU - 1) / 2, Nu, aBasisU);
    evalTrigAxisNthDeriv(V, myAlphaV, (aNbV - 1) / 2, Nv, aBasisV);

    gp_XYZ aSum(0.0, 0.0, 0.0);
    for (int i = 0; i < aNbU; ++i)
    {
      const double aBu = aBasisU[i];
      for (int j = 0; j < aNbV; ++j)
      {
        aSum += (aBu * aBasisV[j])
                * myPoles.Value(myPoles.LowerRow() + i, myPoles.LowerCol() + j).XYZ();
      }
    }
    return gp_Vec(aSum);
  }

  // For rational surfaces, delegate to the specialized methods for low orders.
  if (Nu + Nv <= 3)
  {
    if (Nu == 1 && Nv == 0) { return EvalD1(U, V).D1U; }
    if (Nu == 0 && Nv == 1) { return EvalD1(U, V).D1V; }
    if (Nu == 2 && Nv == 0) { return EvalD2(U, V).D2U; }
    if (Nu == 0 && Nv == 2) { return EvalD2(U, V).D2V; }
    if (Nu == 1 && Nv == 1) { return EvalD2(U, V).D2UV; }
    if (Nu == 3 && Nv == 0) { return EvalD3(U, V).D3U; }
    if (Nu == 0 && Nv == 3) { return EvalD3(U, V).D3V; }
    if (Nu == 2 && Nv == 1) { return EvalD3(U, V).D3UUV; }
    if (Nu == 1 && Nv == 2) { return EvalD3(U, V).D3UVV; }
  }

  // For higher-order rational derivatives, throw.
  throw Geom_UndefinedDerivative(
    "GeomEval_TBezierSurface::EvalDN: rational derivatives of order > 3 not implemented");
}

//==================================================================================================

void GeomEval_TBezierSurface::Transform(const gp_Trsf& T)
{
  (void)T;
  throw Standard_NotImplemented("GeomEval_TBezierSurface::Transform");
}

//==================================================================================================

occ::handle<Geom_Geometry> GeomEval_TBezierSurface::Copy() const
{
  if (myRational)
  {
    return new GeomEval_TBezierSurface(myPoles, myWeights, myAlphaU, myAlphaV);
  }
  return new GeomEval_TBezierSurface(myPoles, myAlphaU, myAlphaV);
}

//==================================================================================================

void GeomEval_TBezierSurface::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_BoundedSurface)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myAlphaU)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myAlphaV)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myRational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPoles.NbRows())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPoles.NbColumns())
}
