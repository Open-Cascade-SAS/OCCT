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

#include <NCollection_LocalArray.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>

#include <algorithm>

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

namespace
{

struct PolySurfaceDerivData
{
  gp_XYZ S00;
  gp_XYZ S10;
  gp_XYZ S01;
  gp_XYZ S20;
  gp_XYZ S02;
  gp_XYZ S11;
  gp_XYZ S30;
  gp_XYZ S03;
  gp_XYZ S21;
  gp_XYZ S12;
};

gp_XYZ cubicVal(const gp_XYZ& theC0,
                const gp_XYZ& theC1,
                const gp_XYZ& theC2,
                const gp_XYZ& theC3,
                const double  theT)
{
  return ((theC3 * theT + theC2) * theT + theC1) * theT + theC0;
}

gp_XYZ cubicD1(const gp_XYZ& theC0,
               const gp_XYZ& theC1,
               const gp_XYZ& theC2,
               const gp_XYZ& theC3,
               const double  theT)
{
  (void)theC0;
  return (theC3 * (3.0 * theT) + theC2 * 2.0) * theT + theC1;
}

gp_XYZ cubicD2(const gp_XYZ& theC0,
               const gp_XYZ& theC1,
               const gp_XYZ& theC2,
               const gp_XYZ& theC3,
               const double  theT)
{
  (void)theC0;
  (void)theC1;
  return theC3 * (6.0 * theT) + theC2 * 2.0;
}

gp_XYZ cubicD3(const gp_XYZ& theC0,
               const gp_XYZ& theC1,
               const gp_XYZ& theC2,
               const gp_XYZ& theC3)
{
  (void)theC0;
  (void)theC1;
  (void)theC2;
  return theC3 * 6.0;
}

template <int theMaxOrder>
void evalPolynomialSurfaceCubicNonRational(const NCollection_Array2<gp_Pnt>& thePoles,
                                           const double                       theU,
                                           const double                       theV,
                                           PolySurfaceDerivData&              theData)
{
  theData.S00 = gp_XYZ(0.0, 0.0, 0.0);
  theData.S10 = gp_XYZ(0.0, 0.0, 0.0);
  theData.S01 = gp_XYZ(0.0, 0.0, 0.0);
  theData.S20 = gp_XYZ(0.0, 0.0, 0.0);
  theData.S02 = gp_XYZ(0.0, 0.0, 0.0);
  theData.S11 = gp_XYZ(0.0, 0.0, 0.0);
  theData.S30 = gp_XYZ(0.0, 0.0, 0.0);
  theData.S03 = gp_XYZ(0.0, 0.0, 0.0);
  theData.S21 = gp_XYZ(0.0, 0.0, 0.0);
  theData.S12 = gp_XYZ(0.0, 0.0, 0.0);

  const int aLR = thePoles.LowerRow();
  const int aLC = thePoles.LowerCol();

  const gp_XYZ aP00 = thePoles.Value(aLR + 0, aLC + 0).XYZ();
  const gp_XYZ aP01 = thePoles.Value(aLR + 0, aLC + 1).XYZ();
  const gp_XYZ aP02 = thePoles.Value(aLR + 0, aLC + 2).XYZ();
  const gp_XYZ aP03 = thePoles.Value(aLR + 0, aLC + 3).XYZ();
  const gp_XYZ aP10 = thePoles.Value(aLR + 1, aLC + 0).XYZ();
  const gp_XYZ aP11 = thePoles.Value(aLR + 1, aLC + 1).XYZ();
  const gp_XYZ aP12 = thePoles.Value(aLR + 1, aLC + 2).XYZ();
  const gp_XYZ aP13 = thePoles.Value(aLR + 1, aLC + 3).XYZ();
  const gp_XYZ aP20 = thePoles.Value(aLR + 2, aLC + 0).XYZ();
  const gp_XYZ aP21 = thePoles.Value(aLR + 2, aLC + 1).XYZ();
  const gp_XYZ aP22 = thePoles.Value(aLR + 2, aLC + 2).XYZ();
  const gp_XYZ aP23 = thePoles.Value(aLR + 2, aLC + 3).XYZ();
  const gp_XYZ aP30 = thePoles.Value(aLR + 3, aLC + 0).XYZ();
  const gp_XYZ aP31 = thePoles.Value(aLR + 3, aLC + 1).XYZ();
  const gp_XYZ aP32 = thePoles.Value(aLR + 3, aLC + 2).XYZ();
  const gp_XYZ aP33 = thePoles.Value(aLR + 3, aLC + 3).XYZ();

  const gp_XYZ aQ00 = cubicVal(aP00, aP10, aP20, aP30, theU);
  const gp_XYZ aQ01 = cubicVal(aP01, aP11, aP21, aP31, theU);
  const gp_XYZ aQ02 = cubicVal(aP02, aP12, aP22, aP32, theU);
  const gp_XYZ aQ03 = cubicVal(aP03, aP13, aP23, aP33, theU);

  theData.S00 = cubicVal(aQ00, aQ01, aQ02, aQ03, theV);
  if constexpr (theMaxOrder >= 1)
  {
    const gp_XYZ aQ10 = cubicD1(aP00, aP10, aP20, aP30, theU);
    const gp_XYZ aQ11 = cubicD1(aP01, aP11, aP21, aP31, theU);
    const gp_XYZ aQ12 = cubicD1(aP02, aP12, aP22, aP32, theU);
    const gp_XYZ aQ13 = cubicD1(aP03, aP13, aP23, aP33, theU);

    theData.S10 = cubicVal(aQ10, aQ11, aQ12, aQ13, theV);
    theData.S01 = cubicD1(aQ00, aQ01, aQ02, aQ03, theV);

    if constexpr (theMaxOrder >= 2)
    {
      const gp_XYZ aQ20 = cubicD2(aP00, aP10, aP20, aP30, theU);
      const gp_XYZ aQ21 = cubicD2(aP01, aP11, aP21, aP31, theU);
      const gp_XYZ aQ22 = cubicD2(aP02, aP12, aP22, aP32, theU);
      const gp_XYZ aQ23 = cubicD2(aP03, aP13, aP23, aP33, theU);

      theData.S20 = cubicVal(aQ20, aQ21, aQ22, aQ23, theV);
      theData.S11 = cubicD1(aQ10, aQ11, aQ12, aQ13, theV);
      theData.S02 = cubicD2(aQ00, aQ01, aQ02, aQ03, theV);

      if constexpr (theMaxOrder >= 3)
      {
        const gp_XYZ aQ30 = cubicD3(aP00, aP10, aP20, aP30);
        const gp_XYZ aQ31 = cubicD3(aP01, aP11, aP21, aP31);
        const gp_XYZ aQ32 = cubicD3(aP02, aP12, aP22, aP32);
        const gp_XYZ aQ33 = cubicD3(aP03, aP13, aP23, aP33);

        theData.S30 = cubicVal(aQ30, aQ31, aQ32, aQ33, theV);
        theData.S21 = cubicD1(aQ20, aQ21, aQ22, aQ23, theV);
        theData.S12 = cubicD2(aQ10, aQ11, aQ12, aQ13, theV);
        theData.S03 = cubicD3(aQ00, aQ01, aQ02, aQ03);
      }
    }
  }
}

template <int theMaxOrder>
void evalPolynomialSurfaceNonRational(const NCollection_Array2<gp_Pnt>& thePoles,
                                      const int                          theDegreeU,
                                      const int                          theDegreeV,
                                      const double                       theU,
                                      const double                       theV,
                                      PolySurfaceDerivData&              theData)
{
  theData.S00 = gp_XYZ(0.0, 0.0, 0.0);
  theData.S10 = gp_XYZ(0.0, 0.0, 0.0);
  theData.S01 = gp_XYZ(0.0, 0.0, 0.0);
  theData.S20 = gp_XYZ(0.0, 0.0, 0.0);
  theData.S02 = gp_XYZ(0.0, 0.0, 0.0);
  theData.S11 = gp_XYZ(0.0, 0.0, 0.0);
  theData.S30 = gp_XYZ(0.0, 0.0, 0.0);
  theData.S03 = gp_XYZ(0.0, 0.0, 0.0);
  theData.S21 = gp_XYZ(0.0, 0.0, 0.0);
  theData.S12 = gp_XYZ(0.0, 0.0, 0.0);

  const int aLR = thePoles.LowerRow();
  const int aLC = thePoles.LowerCol();

  gp_XYZ aR00(0.0, 0.0, 0.0);
  gp_XYZ aR01(0.0, 0.0, 0.0);
  gp_XYZ aR02(0.0, 0.0, 0.0);
  gp_XYZ aR03(0.0, 0.0, 0.0);
  gp_XYZ aR10(0.0, 0.0, 0.0);
  gp_XYZ aR11(0.0, 0.0, 0.0);
  gp_XYZ aR12(0.0, 0.0, 0.0);
  gp_XYZ aR20(0.0, 0.0, 0.0);
  gp_XYZ aR21(0.0, 0.0, 0.0);
  gp_XYZ aR30(0.0, 0.0, 0.0);

  for (int j = theDegreeV; j >= 0; --j)
  {
    gp_XYZ aD0 = thePoles.Value(aLR + theDegreeU, aLC + j).XYZ();
    gp_XYZ aD1(0.0, 0.0, 0.0);
    gp_XYZ aD2(0.0, 0.0, 0.0);
    gp_XYZ aD3(0.0, 0.0, 0.0);

    for (int i = theDegreeU - 1; i >= 0; --i)
    {
      if constexpr (theMaxOrder >= 3)
      {
        aD3 = aD3 * theU + aD2 * 3.0;
      }
      if constexpr (theMaxOrder >= 2)
      {
        aD2 = aD2 * theU + aD1 * 2.0;
      }
      if constexpr (theMaxOrder >= 1)
      {
        aD1 = aD1 * theU + aD0;
      }
      aD0 = aD0 * theU + thePoles.Value(aLR + i, aLC + j).XYZ();
    }

    if constexpr (theMaxOrder >= 3)
    {
      aR03 = aR03 * theV + aR02 * 3.0;
    }
    if constexpr (theMaxOrder >= 2)
    {
      aR02 = aR02 * theV + aR01 * 2.0;
    }
    if constexpr (theMaxOrder >= 1)
    {
      aR01 = aR01 * theV + aR00;
    }
    aR00 = aR00 * theV + aD0;

    if constexpr (theMaxOrder >= 1)
    {
      if constexpr (theMaxOrder >= 2)
      {
        aR12 = aR12 * theV + aR11 * 2.0;
      }
      if constexpr (theMaxOrder >= 1)
      {
        aR11 = aR11 * theV + aR10;
      }
      aR10 = aR10 * theV + aD1;
    }

    if constexpr (theMaxOrder >= 2)
    {
      if constexpr (theMaxOrder >= 3)
      {
        aR21 = aR21 * theV + aR20;
      }
      aR20 = aR20 * theV + aD2;
    }

    if constexpr (theMaxOrder >= 3)
    {
      aR30 = aR30 * theV + aD3;
    }
  }

  theData.S00 = aR00;
  if constexpr (theMaxOrder >= 1)
  {
    theData.S01 = aR01;
    theData.S10 = aR10;
  }
  if constexpr (theMaxOrder >= 2)
  {
    theData.S02 = aR02;
    theData.S11 = aR11;
    theData.S20 = aR20;
  }
  if constexpr (theMaxOrder >= 3)
  {
    theData.S03 = aR03;
    theData.S12 = aR12;
    theData.S21 = aR21;
    theData.S30 = aR30;
  }
}

double& axisVal(double* theData, const int theDim, const int theOrder, const int theIdx)
{
  return theData[theOrder * theDim + theIdx];
}

void evalAxisDerivs(const double theT,
                    const int    theMaxOrder,
                    const int    theAlgDegree,
                    const double theAlpha,
                    const double theBeta,
                    const int    theDim,
                    double*      theDerivs)
{
  int anIdx = 0;

  if (theT == 0.0)
  {
    double aFactK = 1.0;
    for (int k = 0; k <= theAlgDegree; ++k)
    {
      if (k > 0)
      {
        aFactK *= double(k);
      }
      for (int d = 0; d <= theMaxOrder; ++d)
      {
        axisVal(theDerivs, theDim, d, anIdx) = (d == k) ? aFactK : 0.0;
      }
      ++anIdx;
    }
  }
  else
  {
    double aPow = 1.0;
    for (int k = 0; k <= theAlgDegree; ++k)
    {
      const int aMaxD = std::min(k, theMaxOrder);
      double    aVal  = aPow;
      for (int d = 0; d <= aMaxD; ++d)
      {
        axisVal(theDerivs, theDim, d, anIdx) = aVal;
        if (d < aMaxD)
        {
          aVal *= double(k - d) / theT;
        }
      }
      for (int d = aMaxD + 1; d <= theMaxOrder; ++d)
      {
        axisVal(theDerivs, theDim, d, anIdx) = 0.0;
      }
      aPow *= theT;
      ++anIdx;
    }
  }

  if (theAlpha > 0.0)
  {
    const double aAlphaT = theAlpha * theT;
    const double aSinh   = std::sinh(aAlphaT);
    const double aCosh   = std::cosh(aAlphaT);
    double       aAlphaPow = 1.0;
    for (int d = 0; d <= theMaxOrder; ++d)
    {
      const bool isEven = (d % 2 == 0);
      axisVal(theDerivs, theDim, d, anIdx)     = aAlphaPow * (isEven ? aSinh : aCosh);
      axisVal(theDerivs, theDim, d, anIdx + 1) = aAlphaPow * (isEven ? aCosh : aSinh);
      aAlphaPow *= theAlpha;
    }
    anIdx += 2;
  }

  if (theBeta > 0.0)
  {
    const double aBetaT = theBeta * theT;
    const double aSin   = std::sin(aBetaT);
    const double aCos   = std::cos(aBetaT);
    double       aBetaPow = 1.0;
    for (int d = 0; d <= theMaxOrder; ++d)
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
      axisVal(theDerivs, theDim, d, anIdx)     = aBetaPow * aDSin;
      axisVal(theDerivs, theDim, d, anIdx + 1) = aBetaPow * aDCos;
      aBetaPow *= theBeta;
    }
  }
}

int tensorIdx(const int theDu, const int theDv, const int theMaxV)
{
  return theDu * (theMaxV + 1) + theDv;
}

void evalTensorDerivs(const NCollection_Array2<gp_Pnt>&   thePoles,
                      const NCollection_Array2<double>* theWeights,
                      const int                          theMaxU,
                      const int                          theMaxV,
                      const int                          theDimU,
                      const int                          theDimV,
                      const double*                      theBUDerivs,
                      const double*                      theBVDerivs,
                      gp_XYZ*                            theNDerivs,
                      double*                            theWDerivs)
{
  const int aTensorSize = (theMaxU + 1) * (theMaxV + 1);
  for (int idx = 0; idx < aTensorSize; ++idx)
  {
    theNDerivs[idx] = gp_XYZ(0.0, 0.0, 0.0);
    if (theWDerivs != nullptr)
    {
      theWDerivs[idx] = 0.0;
    }
  }

  const int aLR  = thePoles.LowerRow();
  const int aLC  = thePoles.LowerCol();
  const int aWLR = (theWeights != nullptr) ? theWeights->LowerRow() : 0;
  const int aWLC = (theWeights != nullptr) ? theWeights->LowerCol() : 0;

  for (int i = 0; i < theDimU; ++i)
  {
    for (int j = 0; j < theDimV; ++j)
    {
      const gp_XYZ& aPole = thePoles.Value(aLR + i, aLC + j).XYZ();
      const double  aWij  = (theWeights != nullptr) ? theWeights->Value(aWLR + i, aWLC + j) : 1.0;
      for (int du = 0; du <= theMaxU; ++du)
      {
        const double aBU = theBUDerivs[du * theDimU + i];
        for (int dv = 0; dv <= theMaxV; ++dv)
        {
          const int    aIdx = tensorIdx(du, dv, theMaxV);
          const double aB   = aBU * theBVDerivs[dv * theDimV + j];
          if (theWeights != nullptr)
          {
            const double aWB = aWij * aB;
            theNDerivs[aIdx] += aPole * aWB;
            theWDerivs[aIdx] += aWB;
          }
          else
          {
            theNDerivs[aIdx] += aPole * aB;
          }
        }
      }
    }
  }
}

void computeRationalDerivs(const gp_XYZ* theNDerivs,
                           const double* theWDerivs,
                           const int     theMaxU,
                           const int     theMaxV,
                           gp_XYZ*       theCDerivs)
{
  const double aInvW = 1.0 / theWDerivs[tensorIdx(0, 0, theMaxV)];
  for (int anOrd = 0; anOrd <= theMaxU + theMaxV; ++anOrd)
  {
    const int aDuUpper = std::min(anOrd, theMaxU);
    for (int du = 0; du <= aDuUpper; ++du)
    {
      const int dv = anOrd - du;
      if (dv > theMaxV)
      {
        continue;
      }

      gp_XYZ aSum = theNDerivs[tensorIdx(du, dv, theMaxV)];
      for (int p = 0; p <= du; ++p)
      {
        double aBinU = 1.0;
        for (int m = 0; m < p; ++m)
        {
          aBinU = aBinU * double(du - m) / double(m + 1);
        }
        for (int q = 0; q <= dv; ++q)
        {
          if (p == 0 && q == 0)
          {
            continue;
          }
          double aBinV = 1.0;
          for (int m = 0; m < q; ++m)
          {
            aBinV = aBinV * double(dv - m) / double(m + 1);
          }
          const double aCoeff = aBinU
                              * aBinV
                              * theWDerivs[tensorIdx(p, q, theMaxV)];
          aSum -= theCDerivs[tensorIdx(du - p, dv - q, theMaxV)] * aCoeff;
        }
      }
      theCDerivs[tensorIdx(du, dv, theMaxV)] = aSum * aInvW;
    }
  }
}

} // namespace

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
  throw Standard_NotImplemented("GeomEval_AHTBezierSurface::UReverse");
}

//==================================================================================================

void GeomEval_AHTBezierSurface::VReverse()
{
  throw Standard_NotImplemented("GeomEval_AHTBezierSurface::VReverse");
}

//==================================================================================================

double GeomEval_AHTBezierSurface::UReversedParameter(const double U) const
{
  (void)U;
  throw Standard_NotImplemented("GeomEval_AHTBezierSurface::UReversedParameter");
}

//==================================================================================================

double GeomEval_AHTBezierSurface::VReversedParameter(const double V) const
{
  (void)V;
  throw Standard_NotImplemented("GeomEval_AHTBezierSurface::VReversedParameter");
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
  throw Standard_NotImplemented("GeomEval_AHTBezierSurface::UIso");
}

//==================================================================================================

occ::handle<Geom_Curve> GeomEval_AHTBezierSurface::VIso(const double /*V*/) const
{
  throw Standard_NotImplemented("GeomEval_AHTBezierSurface::VIso");
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
  const int  aDimU = NbPolesU();
  const int  aDimV = NbPolesV();
  const bool isRational = (myURational || myVRational);
  const bool isPurePolynomial = !isRational
                             && myAlphaU <= 0.0 && myBetaU <= 0.0
                             && myAlphaV <= 0.0 && myBetaV <= 0.0;

  if (isPurePolynomial)
  {
    PolySurfaceDerivData aPolyData;
    if (myAlgDegreeU == 3 && myAlgDegreeV == 3)
    {
      evalPolynomialSurfaceCubicNonRational<0>(myPoles, U, V, aPolyData);
    }
    else
    {
      evalPolynomialSurfaceNonRational<0>(myPoles, myAlgDegreeU, myAlgDegreeV, U, V, aPolyData);
    }
    return gp_Pnt(aPolyData.S00);
  }

  NCollection_LocalArray<double, 16> aBUDerivs(aDimU);
  NCollection_LocalArray<double, 16> aBVDerivs(aDimV);
  evalAxisDerivs(U, 0, myAlgDegreeU, myAlphaU, myBetaU, aDimU, aBUDerivs);
  evalAxisDerivs(V, 0, myAlgDegreeV, myAlphaV, myBetaV, aDimV, aBVDerivs);

  gp_XYZ aN[1];
  double aW[1];
  evalTensorDerivs(myPoles,
                   isRational ? &myWeights : nullptr,
                   0,
                   0,
                   aDimU,
                   aDimV,
                   aBUDerivs,
                   aBVDerivs,
                   aN,
                   isRational ? aW : nullptr);

  if (!isRational)
  {
    return gp_Pnt(aN[0]);
  }
  return gp_Pnt(aN[0] / aW[0]);
}

//==================================================================================================

Geom_Surface::ResD1 GeomEval_AHTBezierSurface::EvalD1(const double U, const double V) const
{
  const int  aDimU = NbPolesU();
  const int  aDimV = NbPolesV();
  const bool isRational = (myURational || myVRational);
  const bool isPurePolynomial = !isRational
                             && myAlphaU <= 0.0 && myBetaU <= 0.0
                             && myAlphaV <= 0.0 && myBetaV <= 0.0;
  Geom_Surface::ResD1 aResult;

  if (isPurePolynomial)
  {
    PolySurfaceDerivData aPolyData;
    if (myAlgDegreeU == 3 && myAlgDegreeV == 3)
    {
      evalPolynomialSurfaceCubicNonRational<1>(myPoles, U, V, aPolyData);
    }
    else
    {
      evalPolynomialSurfaceNonRational<1>(myPoles, myAlgDegreeU, myAlgDegreeV, U, V, aPolyData);
    }
    aResult.Point = gp_Pnt(aPolyData.S00);
    aResult.D1U   = gp_Vec(aPolyData.S10);
    aResult.D1V   = gp_Vec(aPolyData.S01);
    return aResult;
  }

  NCollection_LocalArray<double, 16> aBUDerivs((1 + 1) * aDimU);
  NCollection_LocalArray<double, 16> aBVDerivs((1 + 1) * aDimV);
  evalAxisDerivs(U, 1, myAlgDegreeU, myAlphaU, myBetaU, aDimU, aBUDerivs);
  evalAxisDerivs(V, 1, myAlgDegreeV, myAlphaV, myBetaV, aDimV, aBVDerivs);

  gp_XYZ aN[(1 + 1) * (1 + 1)];
  double aW[(1 + 1) * (1 + 1)];
  evalTensorDerivs(myPoles,
                   isRational ? &myWeights : nullptr,
                   1,
                   1,
                   aDimU,
                   aDimV,
                   aBUDerivs,
                   aBVDerivs,
                   aN,
                   isRational ? aW : nullptr);

  if (!isRational)
  {
    aResult.Point = gp_Pnt(aN[tensorIdx(0, 0, 1)]);
    aResult.D1U   = gp_Vec(aN[tensorIdx(1, 0, 1)]);
    aResult.D1V   = gp_Vec(aN[tensorIdx(0, 1, 1)]);
    return aResult;
  }

  const double aInvW00 = 1.0 / aW[tensorIdx(0, 0, 1)];
  const gp_XYZ aC00    = aN[tensorIdx(0, 0, 1)] * aInvW00;
  const gp_XYZ aC10    = (aN[tensorIdx(1, 0, 1)] - aC00 * aW[tensorIdx(1, 0, 1)]) * aInvW00;
  const gp_XYZ aC01    = (aN[tensorIdx(0, 1, 1)] - aC00 * aW[tensorIdx(0, 1, 1)]) * aInvW00;

  aResult.Point = gp_Pnt(aC00);
  aResult.D1U   = gp_Vec(aC10);
  aResult.D1V   = gp_Vec(aC01);
  return aResult;
}

//==================================================================================================

Geom_Surface::ResD2 GeomEval_AHTBezierSurface::EvalD2(const double U, const double V) const
{
  const int  aDimU = NbPolesU();
  const int  aDimV = NbPolesV();
  const bool isRational = (myURational || myVRational);
  const bool isPurePolynomial = !isRational
                             && myAlphaU <= 0.0 && myBetaU <= 0.0
                             && myAlphaV <= 0.0 && myBetaV <= 0.0;
  Geom_Surface::ResD2 aResult;

  if (isPurePolynomial)
  {
    PolySurfaceDerivData aPolyData;
    if (myAlgDegreeU == 3 && myAlgDegreeV == 3)
    {
      evalPolynomialSurfaceCubicNonRational<2>(myPoles, U, V, aPolyData);
    }
    else
    {
      evalPolynomialSurfaceNonRational<2>(myPoles, myAlgDegreeU, myAlgDegreeV, U, V, aPolyData);
    }
    aResult.Point = gp_Pnt(aPolyData.S00);
    aResult.D1U   = gp_Vec(aPolyData.S10);
    aResult.D1V   = gp_Vec(aPolyData.S01);
    aResult.D2U   = gp_Vec(aPolyData.S20);
    aResult.D2V   = gp_Vec(aPolyData.S02);
    aResult.D2UV  = gp_Vec(aPolyData.S11);
    return aResult;
  }

  NCollection_LocalArray<double, 16> aBUDerivs((2 + 1) * aDimU);
  NCollection_LocalArray<double, 16> aBVDerivs((2 + 1) * aDimV);
  evalAxisDerivs(U, 2, myAlgDegreeU, myAlphaU, myBetaU, aDimU, aBUDerivs);
  evalAxisDerivs(V, 2, myAlgDegreeV, myAlphaV, myBetaV, aDimV, aBVDerivs);

  gp_XYZ aN[(2 + 1) * (2 + 1)];
  double aW[(2 + 1) * (2 + 1)];
  evalTensorDerivs(myPoles,
                   isRational ? &myWeights : nullptr,
                   2,
                   2,
                   aDimU,
                   aDimV,
                   aBUDerivs,
                   aBVDerivs,
                   aN,
                   isRational ? aW : nullptr);

  if (!isRational)
  {
    aResult.Point = gp_Pnt(aN[tensorIdx(0, 0, 2)]);
    aResult.D1U   = gp_Vec(aN[tensorIdx(1, 0, 2)]);
    aResult.D1V   = gp_Vec(aN[tensorIdx(0, 1, 2)]);
    aResult.D2U   = gp_Vec(aN[tensorIdx(2, 0, 2)]);
    aResult.D2V   = gp_Vec(aN[tensorIdx(0, 2, 2)]);
    aResult.D2UV  = gp_Vec(aN[tensorIdx(1, 1, 2)]);
    return aResult;
  }

  const double aInvW00 = 1.0 / aW[tensorIdx(0, 0, 2)];
  const gp_XYZ aC00    = aN[tensorIdx(0, 0, 2)] * aInvW00;
  const gp_XYZ aC10    = (aN[tensorIdx(1, 0, 2)] - aC00 * aW[tensorIdx(1, 0, 2)]) * aInvW00;
  const gp_XYZ aC01    = (aN[tensorIdx(0, 1, 2)] - aC00 * aW[tensorIdx(0, 1, 2)]) * aInvW00;
  const gp_XYZ aC20    = (aN[tensorIdx(2, 0, 2)]
                        - aC10 * (2.0 * aW[tensorIdx(1, 0, 2)])
                        - aC00 * aW[tensorIdx(2, 0, 2)])
                       * aInvW00;
  const gp_XYZ aC02    = (aN[tensorIdx(0, 2, 2)]
                        - aC01 * (2.0 * aW[tensorIdx(0, 1, 2)])
                        - aC00 * aW[tensorIdx(0, 2, 2)])
                       * aInvW00;
  const gp_XYZ aC11    = (aN[tensorIdx(1, 1, 2)]
                        - aC10 * aW[tensorIdx(0, 1, 2)]
                        - aC01 * aW[tensorIdx(1, 0, 2)]
                        - aC00 * aW[tensorIdx(1, 1, 2)])
                       * aInvW00;

  aResult.Point = gp_Pnt(aC00);
  aResult.D1U   = gp_Vec(aC10);
  aResult.D1V   = gp_Vec(aC01);
  aResult.D2U   = gp_Vec(aC20);
  aResult.D2V   = gp_Vec(aC02);
  aResult.D2UV  = gp_Vec(aC11);
  return aResult;
}

//==================================================================================================

Geom_Surface::ResD3 GeomEval_AHTBezierSurface::EvalD3(const double U, const double V) const
{
  const int  aDimU = NbPolesU();
  const int  aDimV = NbPolesV();
  const bool isRational = (myURational || myVRational);
  const bool isPurePolynomial = !isRational
                             && myAlphaU <= 0.0 && myBetaU <= 0.0
                             && myAlphaV <= 0.0 && myBetaV <= 0.0;
  Geom_Surface::ResD3 aResult;

  if (isPurePolynomial)
  {
    PolySurfaceDerivData aPolyData;
    if (myAlgDegreeU == 3 && myAlgDegreeV == 3)
    {
      evalPolynomialSurfaceCubicNonRational<3>(myPoles, U, V, aPolyData);
    }
    else
    {
      evalPolynomialSurfaceNonRational<3>(myPoles, myAlgDegreeU, myAlgDegreeV, U, V, aPolyData);
    }
    aResult.Point = gp_Pnt(aPolyData.S00);
    aResult.D1U   = gp_Vec(aPolyData.S10);
    aResult.D1V   = gp_Vec(aPolyData.S01);
    aResult.D2U   = gp_Vec(aPolyData.S20);
    aResult.D2V   = gp_Vec(aPolyData.S02);
    aResult.D2UV  = gp_Vec(aPolyData.S11);
    aResult.D3U   = gp_Vec(aPolyData.S30);
    aResult.D3V   = gp_Vec(aPolyData.S03);
    aResult.D3UUV = gp_Vec(aPolyData.S21);
    aResult.D3UVV = gp_Vec(aPolyData.S12);
    return aResult;
  }

  NCollection_LocalArray<double, 16> aBUDerivs((3 + 1) * aDimU);
  NCollection_LocalArray<double, 16> aBVDerivs((3 + 1) * aDimV);
  evalAxisDerivs(U, 3, myAlgDegreeU, myAlphaU, myBetaU, aDimU, aBUDerivs);
  evalAxisDerivs(V, 3, myAlgDegreeV, myAlphaV, myBetaV, aDimV, aBVDerivs);

  gp_XYZ aN[(3 + 1) * (3 + 1)];
  double aW[(3 + 1) * (3 + 1)];
  evalTensorDerivs(myPoles,
                   isRational ? &myWeights : nullptr,
                   3,
                   3,
                   aDimU,
                   aDimV,
                   aBUDerivs,
                   aBVDerivs,
                   aN,
                   isRational ? aW : nullptr);

  if (!isRational)
  {
    aResult.Point = gp_Pnt(aN[tensorIdx(0, 0, 3)]);
    aResult.D1U   = gp_Vec(aN[tensorIdx(1, 0, 3)]);
    aResult.D1V   = gp_Vec(aN[tensorIdx(0, 1, 3)]);
    aResult.D2U   = gp_Vec(aN[tensorIdx(2, 0, 3)]);
    aResult.D2V   = gp_Vec(aN[tensorIdx(0, 2, 3)]);
    aResult.D2UV  = gp_Vec(aN[tensorIdx(1, 1, 3)]);
    aResult.D3U   = gp_Vec(aN[tensorIdx(3, 0, 3)]);
    aResult.D3V   = gp_Vec(aN[tensorIdx(0, 3, 3)]);
    aResult.D3UUV = gp_Vec(aN[tensorIdx(2, 1, 3)]);
    aResult.D3UVV = gp_Vec(aN[tensorIdx(1, 2, 3)]);
    return aResult;
  }

  const double aInvW00 = 1.0 / aW[tensorIdx(0, 0, 3)];
  const gp_XYZ aC00    = aN[tensorIdx(0, 0, 3)] * aInvW00;
  const gp_XYZ aC10    = (aN[tensorIdx(1, 0, 3)] - aC00 * aW[tensorIdx(1, 0, 3)]) * aInvW00;
  const gp_XYZ aC01    = (aN[tensorIdx(0, 1, 3)] - aC00 * aW[tensorIdx(0, 1, 3)]) * aInvW00;
  const gp_XYZ aC20    = (aN[tensorIdx(2, 0, 3)]
                        - aC10 * (2.0 * aW[tensorIdx(1, 0, 3)])
                        - aC00 * aW[tensorIdx(2, 0, 3)])
                       * aInvW00;
  const gp_XYZ aC02    = (aN[tensorIdx(0, 2, 3)]
                        - aC01 * (2.0 * aW[tensorIdx(0, 1, 3)])
                        - aC00 * aW[tensorIdx(0, 2, 3)])
                       * aInvW00;
  const gp_XYZ aC11    = (aN[tensorIdx(1, 1, 3)]
                        - aC10 * aW[tensorIdx(0, 1, 3)]
                        - aC01 * aW[tensorIdx(1, 0, 3)]
                        - aC00 * aW[tensorIdx(1, 1, 3)])
                       * aInvW00;
  const gp_XYZ aC30    = (aN[tensorIdx(3, 0, 3)]
                        - aC20 * (3.0 * aW[tensorIdx(1, 0, 3)])
                        - aC10 * (3.0 * aW[tensorIdx(2, 0, 3)])
                        - aC00 * aW[tensorIdx(3, 0, 3)])
                       * aInvW00;
  const gp_XYZ aC03    = (aN[tensorIdx(0, 3, 3)]
                        - aC02 * (3.0 * aW[tensorIdx(0, 1, 3)])
                        - aC01 * (3.0 * aW[tensorIdx(0, 2, 3)])
                        - aC00 * aW[tensorIdx(0, 3, 3)])
                       * aInvW00;
  const gp_XYZ aC21    = (aN[tensorIdx(2, 1, 3)]
                        - aC11 * (2.0 * aW[tensorIdx(1, 0, 3)])
                        - aC20 * aW[tensorIdx(0, 1, 3)]
                        - aC01 * aW[tensorIdx(2, 0, 3)]
                        - aC10 * (2.0 * aW[tensorIdx(1, 1, 3)])
                        - aC00 * aW[tensorIdx(2, 1, 3)])
                       * aInvW00;
  const gp_XYZ aC12    = (aN[tensorIdx(1, 2, 3)]
                        - aC11 * (2.0 * aW[tensorIdx(0, 1, 3)])
                        - aC02 * aW[tensorIdx(1, 0, 3)]
                        - aC10 * aW[tensorIdx(0, 2, 3)]
                        - aC01 * (2.0 * aW[tensorIdx(1, 1, 3)])
                        - aC00 * aW[tensorIdx(1, 2, 3)])
                       * aInvW00;

  aResult.Point = gp_Pnt(aC00);
  aResult.D1U   = gp_Vec(aC10);
  aResult.D1V   = gp_Vec(aC01);
  aResult.D2U   = gp_Vec(aC20);
  aResult.D2V   = gp_Vec(aC02);
  aResult.D2UV  = gp_Vec(aC11);
  aResult.D3U   = gp_Vec(aC30);
  aResult.D3V   = gp_Vec(aC03);
  aResult.D3UUV = gp_Vec(aC21);
  aResult.D3UVV = gp_Vec(aC12);
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

  const int aTotOrd = Nu + Nv;
  if (aTotOrd <= 3)
  {
    if (aTotOrd == 1)
    {
      const Geom_Surface::ResD1 aD1 = EvalD1(U, V);
      return (Nu == 1) ? aD1.D1U : aD1.D1V;
    }
    if (aTotOrd == 2)
    {
      const Geom_Surface::ResD2 aD2 = EvalD2(U, V);
      if (Nu == 2)
      {
        return aD2.D2U;
      }
      if (Nv == 2)
      {
        return aD2.D2V;
      }
      return aD2.D2UV;
    }

    const Geom_Surface::ResD3 aD3 = EvalD3(U, V);
    if (Nu == 3)
    {
      return aD3.D3U;
    }
    if (Nv == 3)
    {
      return aD3.D3V;
    }
    return (Nu == 2) ? aD3.D3UUV : aD3.D3UVV;
  }

  const int aDimU = NbPolesU();
  const int aDimV = NbPolesV();

  NCollection_LocalArray<double, 16> aBUDerivs((Nu + 1) * aDimU);
  NCollection_LocalArray<double, 16> aBVDerivs((Nv + 1) * aDimV);
  evalAxisDerivs(U, Nu, myAlgDegreeU, myAlphaU, myBetaU, aDimU, aBUDerivs);
  evalAxisDerivs(V, Nv, myAlgDegreeV, myAlphaV, myBetaV, aDimV, aBVDerivs);

  const bool isRational = (myURational || myVRational);

  if (!isRational)
  {
    gp_XYZ aSum(0.0, 0.0, 0.0);
    const int aLR = myPoles.LowerRow();
    const int aLC = myPoles.LowerCol();
    for (int i = 0; i < aDimU; ++i)
    {
      for (int j = 0; j < aDimV; ++j)
      {
        aSum += myPoles.Value(aLR + i, aLC + j).XYZ()
                * (aBUDerivs[Nu * aDimU + i] * aBVDerivs[Nv * aDimV + j]);
      }
    }
    return gp_Vec(aSum);
  }

  NCollection_LocalArray<gp_XYZ, 16> aNDerivs((Nu + 1) * (Nv + 1));
  NCollection_LocalArray<double, 16> aWDerivs((Nu + 1) * (Nv + 1));
  NCollection_LocalArray<gp_XYZ, 16> aCDerivs((Nu + 1) * (Nv + 1));
  evalTensorDerivs(myPoles,
                   &myWeights,
                   Nu,
                   Nv,
                   aDimU,
                   aDimV,
                   aBUDerivs,
                   aBVDerivs,
                   aNDerivs,
                   aWDerivs);
  computeRationalDerivs(aNDerivs, aWDerivs, Nu, Nv, aCDerivs);
  return gp_Vec(aCDerivs[tensorIdx(Nu, Nv, Nv)]);
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
