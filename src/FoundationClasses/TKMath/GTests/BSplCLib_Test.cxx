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

#include <gtest/gtest.h>

#include <BSplCLib.hxx>
#include <math_Matrix.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_LocalArray.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_Real.hxx>

#include <algorithm>
#include <cmath>

TEST(BSplCLibTest, UnitWeights_SmallSize_ReturnsNonOwning)
{
  const int                        aNbElems = 10;
  const NCollection_Array1<double> aWeights = BSplCLib::UnitWeights(aNbElems);

  EXPECT_EQ(aWeights.Lower(), 1);
  EXPECT_EQ(aWeights.Upper(), aNbElems);
  EXPECT_EQ(aWeights.Length(), aNbElems);
  EXPECT_FALSE(aWeights.IsDeletable());

  for (int i = 1; i <= aNbElems; ++i)
  {
    EXPECT_DOUBLE_EQ(aWeights(i), 1.0);
  }
}

TEST(BSplCLibTest, UnitWeights_MaxSize_ReturnsNonOwning)
{
  const int                        aNbElems = BSplCLib::MaxUnitWeightsSize();
  const NCollection_Array1<double> aWeights = BSplCLib::UnitWeights(aNbElems);

  EXPECT_EQ(aWeights.Length(), aNbElems);
  EXPECT_FALSE(aWeights.IsDeletable());

  EXPECT_DOUBLE_EQ(aWeights(1), 1.0);
  EXPECT_DOUBLE_EQ(aWeights(aNbElems), 1.0);
}

TEST(BSplCLibTest, UnitWeights_OverMaxSize_ReturnsOwning)
{
  const int                        aNbElems = BSplCLib::MaxUnitWeightsSize() + 1;
  const NCollection_Array1<double> aWeights = BSplCLib::UnitWeights(aNbElems);

  EXPECT_EQ(aWeights.Lower(), 1);
  EXPECT_EQ(aWeights.Upper(), aNbElems);
  EXPECT_EQ(aWeights.Length(), aNbElems);
  EXPECT_TRUE(aWeights.IsDeletable());

  for (int i = 1; i <= aNbElems; ++i)
  {
    EXPECT_DOUBLE_EQ(aWeights(i), 1.0);
  }
}

TEST(BSplCLibTest, UnitWeights_SingleElement)
{
  const NCollection_Array1<double> aWeights = BSplCLib::UnitWeights(1);

  EXPECT_EQ(aWeights.Length(), 1);
  EXPECT_FALSE(aWeights.IsDeletable());
  EXPECT_DOUBLE_EQ(aWeights(1), 1.0);
}

TEST(BSplCLibTest, MaxUnitWeightsSize_IsPositive)
{
  EXPECT_GT(BSplCLib::MaxUnitWeightsSize(), 0);
  EXPECT_EQ(BSplCLib::MaxUnitWeightsSize(), 2049);
}

TEST(BSplCLibTest, ScalarBuildEvalUsesSharedLayout)
{
  NCollection_Array1<double> aPoles(1, 3);
  aPoles(1) = 2.0;
  aPoles(2) = 3.0;
  aPoles(3) = 5.0;
  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = 2.0;
  aWeights(3) = 4.0;

  double aPolynomial[3];
  BSplCLib::BuildEval(2, 0, aPoles, BSplCLib::NoWeights(), aPolynomial[0]);
  EXPECT_DOUBLE_EQ(aPolynomial[0], 2.0);
  EXPECT_DOUBLE_EQ(aPolynomial[1], 3.0);
  EXPECT_DOUBLE_EQ(aPolynomial[2], 5.0);

  double aRational[6];
  BSplCLib::BuildEval(2, 0, aPoles, &aWeights, aRational[0]);
  EXPECT_DOUBLE_EQ(aRational[0], 2.0);
  EXPECT_DOUBLE_EQ(aRational[1], 1.0);
  EXPECT_DOUBLE_EQ(aRational[2], 6.0);
  EXPECT_DOUBLE_EQ(aRational[3], 2.0);
  EXPECT_DOUBLE_EQ(aRational[4], 20.0);
  EXPECT_DOUBLE_EQ(aRational[5], 4.0);

  aPoles(1) = -0.0;
  double aSignedZero;
  BSplCLib::BuildEval(0, 0, aPoles, BSplCLib::NoWeights(), aSignedZero);
  EXPECT_TRUE(std::signbit(aSignedZero));

  NCollection_Array1<double> aShiftedPoles(-2, 0);
  aShiftedPoles.ChangeAt(0) = 2.0;
  aShiftedPoles.ChangeAt(1) = 3.0;
  aShiftedPoles.ChangeAt(2) = 5.0;
  double aWrapped[3];
  BSplCLib::BuildEval(2, 2, aShiftedPoles, BSplCLib::NoWeights(), aWrapped[0]);
  EXPECT_DOUBLE_EQ(aWrapped[0], 5.0);
  EXPECT_DOUBLE_EQ(aWrapped[1], 2.0);
  EXPECT_DOUBLE_EQ(aWrapped[2], 3.0);

  NCollection_Array1<double> aShiftedWeights(-2, 0);
  aShiftedWeights.ChangeAt(0) = 1.0;
  aShiftedWeights.ChangeAt(1) = 2.0;
  aShiftedWeights.ChangeAt(2) = 4.0;
  double aWrappedRational[6];
  BSplCLib::BuildEval(2, 2, aShiftedPoles, &aShiftedWeights, aWrappedRational[0]);
  EXPECT_DOUBLE_EQ(aWrappedRational[0], 20.0);
  EXPECT_DOUBLE_EQ(aWrappedRational[1], 4.0);
  EXPECT_DOUBLE_EQ(aWrappedRational[2], 2.0);
  EXPECT_DOUBLE_EQ(aWrappedRational[3], 1.0);
  EXPECT_DOUBLE_EQ(aWrappedRational[4], 6.0);
  EXPECT_DOUBLE_EQ(aWrappedRational[5], 2.0);
}

TEST(BSplCLibTest, ScalarEvaluationMatches2dSharedImplementation)
{
  constexpr int aDegree = 3;

  NCollection_Array1<double>   aScalarPoles(1, 7);
  NCollection_Array1<gp_Pnt2d> aPoles2d(1, 7);
  NCollection_Array1<double>   aWeights(1, 7);
  for (size_t anIndex = 0; anIndex < aScalarPoles.Size(); ++anIndex)
  {
    const double aValue            = std::sin(static_cast<double>(anIndex + 1));
    aScalarPoles.ChangeAt(anIndex) = aValue;
    aPoles2d.ChangeAt(anIndex)     = gp_Pnt2d(aValue, 0.0);
    aWeights.ChangeAt(anIndex)     = 1.0 + 0.1 * static_cast<double>(anIndex + 1);
  }

  NCollection_Array1<double> aKnots(1, 4);
  aKnots(1) = 0.0;
  aKnots(2) = 0.3;
  aKnots(3) = 0.7;
  aKnots(4) = 1.0;
  NCollection_Array1<int> aMults(1, 4);
  aMults(1) = 4;
  aMults(2) = 1;
  aMults(3) = 2;
  aMults(4) = 4;

  for (const NCollection_Array1<double>* aCurveWeights : {BSplCLib::NoWeights(), &aWeights})
  {
    for (const double aParameter :
         {0.0, std::nextafter(0.3, 0.0), 0.3, 0.5, std::nextafter(0.7, 0.0), 0.7, 1.0})
    {
      double aValue, aD1, aD2, aD3;
      BSplCLib::D3(aParameter,
                   0,
                   aDegree,
                   false,
                   aScalarPoles,
                   aCurveWeights,
                   aKnots,
                   &aMults,
                   aValue,
                   aD1,
                   aD2,
                   aD3);

      gp_Pnt2d aPoint;
      gp_Vec2d aVec1, aVec2, aVec3;
      BSplCLib::D3(aParameter,
                   0,
                   aDegree,
                   false,
                   aPoles2d,
                   aCurveWeights,
                   aKnots,
                   &aMults,
                   aPoint,
                   aVec1,
                   aVec2,
                   aVec3);

      EXPECT_NEAR(aValue, aPoint.X(), 1.0e-12);
      EXPECT_NEAR(aD1, aVec1.X(), 1.0e-12);
      EXPECT_NEAR(aD2, aVec2.X(), 1.0e-12);
      EXPECT_NEAR(aD3, aVec3.X(), 1.0e-12);
      EXPECT_NEAR(aPoint.Y(), 0.0, 1.0e-12);
      EXPECT_NEAR(aVec1.Y(), 0.0, 1.0e-12);
      EXPECT_NEAR(aVec2.Y(), 0.0, 1.0e-12);
      EXPECT_NEAR(aVec3.Y(), 0.0, 1.0e-12);

      for (int aDerivative = 1; aDerivative <= aDegree + 1; ++aDerivative)
      {
        double   aScalarDN;
        gp_Vec2d aVecDN;
        BSplCLib::DN(aParameter,
                     aDerivative,
                     0,
                     aDegree,
                     false,
                     aScalarPoles,
                     aCurveWeights,
                     aKnots,
                     &aMults,
                     aScalarDN);
        BSplCLib::DN(aParameter,
                     aDerivative,
                     0,
                     aDegree,
                     false,
                     aPoles2d,
                     aCurveWeights,
                     aKnots,
                     &aMults,
                     aVecDN);
        EXPECT_NEAR(aScalarDN, aVecDN.X(), 1.0e-12);
        EXPECT_NEAR(aVecDN.Y(), 0.0, 1.0e-12);
      }
    }
  }
}

TEST(BSplCLibTest, FlatKnotsNearRepeatedKnotUseActualSpan)
{
  constexpr int aDegree = 3;

  NCollection_Array1<double> aPoles(1, 10);
  for (size_t anIndex = 0; anIndex < aPoles.Size(); ++anIndex)
  {
    aPoles.ChangeAt(anIndex) = std::sin(static_cast<double>(anIndex + 1));
  }

  NCollection_Array1<double> aKnots(1, 5);
  aKnots(1) = 0.0;
  aKnots(2) = 0.2;
  aKnots(3) = 0.5;
  aKnots(4) = 0.75;
  aKnots(5) = 1.0;
  NCollection_Array1<int> aMults(1, 5);
  aMults(1) = 4;
  aMults(2) = 1;
  aMults(3) = 2;
  aMults(4) = 3;
  aMults(5) = 4;

  NCollection_Array1<double> aFlatKnots(1, BSplCLib::KnotSequenceLength(aMults, aDegree, false));
  BSplCLib::KnotSequence(aKnots, aMults, aDegree, false, aFlatKnots);

  for (const double aParameter : {std::nextafter(0.5, 0.0), std::nextafter(0.75, 0.0)})
  {
    double aFlatParam = aParameter;
    int    aFlatSpan  = 0;
    BSplCLib::LocateParameter(aDegree,
                              aFlatKnots,
                              BSplCLib::NoMults(),
                              aParameter,
                              false,
                              aFlatSpan,
                              aFlatParam);
    EXPECT_LE(aFlatKnots.Value(aFlatSpan), aFlatParam);
    EXPECT_GT(aFlatKnots.Value(aFlatSpan + 1), aFlatParam);

    double aBoundedParam = aParameter;
    int    aBoundedSpan  = 0;
    BSplCLib::LocateParameter(aDegree,
                              aFlatKnots,
                              aParameter,
                              false,
                              aFlatKnots.Lower() + aDegree,
                              aFlatKnots.Upper() - aDegree,
                              aBoundedSpan,
                              aBoundedParam);
    EXPECT_EQ(aBoundedSpan, aFlatSpan);

    double aLegacyParam = aParameter;
    int    aLegacySpan  = 0;
    BSplCLib::LocateParameter(aDegree,
                              aFlatKnots,
                              aMults,
                              aParameter,
                              false,
                              aFlatKnots.Lower() + aDegree,
                              aFlatKnots.Upper() - aDegree,
                              aLegacySpan,
                              aLegacyParam);
    EXPECT_EQ(aLegacySpan, aFlatSpan);

    int    aRefSpan  = 0;
    double aRefParam = aParameter;
    BSplCLib::LocateParameter(aDegree, aKnots, &aMults, aParameter, false, aRefSpan, aRefParam);
    if (aRefParam < aKnots.Value(aRefSpan))
    {
      --aRefSpan;
    }

    double aRefValue, aRefD1, aRefD2, aRefD3;
    BSplCLib::D3(aRefParam,
                 aRefSpan,
                 aDegree,
                 false,
                 aPoles,
                 BSplCLib::NoWeights(),
                 aKnots,
                 &aMults,
                 aRefValue,
                 aRefD1,
                 aRefD2,
                 aRefD3);

    double aValue, aD1, aD2, aD3;
    BSplCLib::D3(aParameter,
                 0,
                 aDegree,
                 false,
                 aPoles,
                 BSplCLib::NoWeights(),
                 aFlatKnots,
                 BSplCLib::NoMults(),
                 aValue,
                 aD1,
                 aD2,
                 aD3);

    EXPECT_NEAR(aValue, aRefValue, 1.0e-12);
    EXPECT_NEAR(aD1, aRefD1, 1.0e-12);
    EXPECT_NEAR(aD2, aRefD2, 1.0e-12);
    EXPECT_NEAR(aD3, aRefD3, 1.0e-12);

    for (int aDerivative = 1; aDerivative <= aDegree; ++aDerivative)
    {
      double aRefDN, aDN;
      BSplCLib::DN(aRefParam,
                   aDerivative,
                   aRefSpan,
                   aDegree,
                   false,
                   aPoles,
                   BSplCLib::NoWeights(),
                   aKnots,
                   &aMults,
                   aRefDN);
      BSplCLib::DN(aParameter,
                   aDerivative,
                   0,
                   aDegree,
                   false,
                   aPoles,
                   BSplCLib::NoWeights(),
                   aFlatKnots,
                   BSplCLib::NoMults(),
                   aDN);
      EXPECT_NEAR(aDN, aRefDN, 1.0e-12);
    }
  }
}

//==================================================================================================

TEST(BSplCLibTest, DerivativesRetainClampedEndpointOnUnevenKnots)
{
  for (const int aDegree : {3, 8, 15, BSplCLib::MaxDegree()})
  {
    for (const int aDimension : {1, 2, 3, 4, 7})
    {
      SCOPED_TRACE(aDegree);
      SCOPED_TRACE(aDimension);
      NCollection_Array1<double> aKnots(0, 2 * aDegree - 1);
      for (int anIndex = 0; anIndex < aDegree; ++anIndex)
      {
        const double aRatio       = double(anIndex) / aDegree;
        aKnots(anIndex)           = aRatio * aRatio;
        aKnots(anIndex + aDegree) = 1.0;
      }
      aKnots(aDegree - 1) = 1.0 - 1.e-8;
      NCollection_Array1<double> aPoles(0, (aDegree + 1) * aDimension - 1);
      for (int aPole = 0; aPole <= aDegree; ++aPole)
      {
        for (int aCoordinate = 0; aCoordinate < aDimension; ++aCoordinate)
        {
          aPoles(aPole * aDimension + aCoordinate) =
            std::sin(double((aPole + 1) * (aCoordinate + 1)));
        }
      }
      for (const int anOrder : {0, 1, 2, 3, aDegree})
      {
        SCOPED_TRACE(anOrder);
        NCollection_Array1<double> aResult = aPoles;
        BSplCLib::Bohm(1.0, aDegree, anOrder, aKnots.ChangeAt(0), aDimension, aResult.ChangeAt(0));
        NCollection_LocalArray<long double, 104> aDerivativePoles(aPoles.Size());
        for (size_t anIndex = 0; anIndex < aPoles.Size(); ++anIndex)
        {
          aDerivativePoles[anIndex] = aPoles.At(anIndex);
        }
        for (int aDerivative = 0; aDerivative <= anOrder; ++aDerivative)
        {
          for (int aCoordinate = 0; aCoordinate < aDimension; ++aCoordinate)
          {
            const double anExpected =
              double(aDerivativePoles[(aDegree - aDerivative) * aDimension + aCoordinate]);
            const double aTolerance = aDerivative == 0 ? 1.e-14 : 2.e-13 * std::abs(anExpected);
            EXPECT_NEAR(aResult(aDerivative * aDimension + aCoordinate), anExpected, aTolerance);
          }
          for (int aPole = 0; aPole < aDegree - aDerivative; ++aPole)
          {
            const long double aScale =
              (aDegree - aDerivative)
              / (static_cast<long double>(aKnots(aPole + aDegree)) - aKnots(aPole + aDerivative));
            for (int aCoordinate = 0; aCoordinate < aDimension; ++aCoordinate)
            {
              const size_t anIndex = aPole * aDimension + aCoordinate;
              aDerivativePoles[anIndex] =
                aScale * (aDerivativePoles[anIndex + aDimension] - aDerivativePoles[anIndex]);
            }
          }
        }
      }
    }
  }
}

//==================================================================================================

TEST(BSplCLibTest, InterpolationRetainsConstantCoordinates)
{
  for (const int aDegree : {1, 3, 8, BSplCLib::MaxDegree()})
  {
    for (const int aDimension : {1, 2, 3, 4, 7})
    {
      SCOPED_TRACE(aDegree);
      SCOPED_TRACE(aDimension);
      NCollection_Array1<double> aKnots(0, 2 * aDegree - 1);
      for (int anIndex = 0; anIndex < aDegree; ++anIndex)
      {
        aKnots(anIndex)           = 0;
        aKnots(anIndex + aDegree) = 1;
      }
      NCollection_Array1<double> aPoles(0, (aDegree + 1) * aDimension - 1);
      for (int aSample = -100; aSample <= 200; ++aSample)
      {
        const double aParameter = double(aSample) / 100;
        for (int aPole = 0; aPole <= aDegree; ++aPole)
        {
          for (int aCoordinate = 0; aCoordinate < aDimension; ++aCoordinate)
          {
            aPoles(aPole * aDimension + aCoordinate) = 123456.789 * double(aCoordinate + 1);
          }
        }
        NCollection_Array1<double> aValue = aPoles;
        BSplCLib::Eval(aParameter, aDegree, aKnots.ChangeAt(0), aDimension, aValue.ChangeAt(0));
        BSplCLib::Bohm(aParameter,
                       aDegree,
                       aDegree,
                       aKnots.ChangeAt(0),
                       aDimension,
                       aPoles.ChangeAt(0));
        for (int aCoordinate = 0; aCoordinate < aDimension; ++aCoordinate)
        {
          const double anExpected = 123456.789 * double(aCoordinate + 1);
          EXPECT_EQ(aValue(aCoordinate), anExpected);
          EXPECT_EQ(aPoles(aCoordinate), anExpected);
          for (int aDerivative = 1; aDerivative <= aDegree; ++aDerivative)
          {
            EXPECT_EQ(aPoles(aDerivative * aDimension + aCoordinate), 0);
          }
        }
      }
    }
  }
}

TEST(BSplCLibTest, InterpolationOppositeLargeCoordinates)
{
  double aKnots[] = {0.0, 1.0};
  for (const double aParameter : {0.0, 0.25, 0.5, 0.75, 1.0})
  {
    double aPoles[] = {-1.e308, 1.e308};
    BSplCLib::Eval(aParameter, 1, aKnots[0], 1, aPoles[0]);
    EXPECT_TRUE(std::isfinite(aPoles[0]));
    const double anExpected = (1.0 - aParameter) * -1.e308 + aParameter * 1.e308;
    EXPECT_DOUBLE_EQ(aPoles[0], anExpected);
    double aBohmPoles[] = {-1.e308, 1.e308};
    BSplCLib::Bohm(aParameter, 1, 0, aKnots[0], 1, aBohmPoles[0]);
    EXPECT_DOUBLE_EQ(aBohmPoles[0], anExpected);
  }
}

TEST(BSplCLibTest, InterpolationRetainsSmallControlPolygonVariation)
{
  // The exact cubic is origin + 6*u. Rounding every intermediate point
  // at the origin's magnitude erases increments smaller than its ulp.
  double aKnots[] = {0.0, 0.0, 0.0, 1.0, 1.0, 1.0};
  for (double anOrigin : {1.e16, -1.e16})
  {
    SCOPED_TRACE(anOrigin);
    double aPoles[] = {anOrigin, anOrigin + 2.0, anOrigin + 4.0, anOrigin + 6.0};
    BSplCLib::Eval(0.25, 3, aKnots[0], 1, aPoles[0]);
    EXPECT_EQ(aPoles[0], anOrigin + 2.0);
    double aDerivatives[] = {anOrigin, anOrigin + 2.0, anOrigin + 4.0, anOrigin + 6.0};
    BSplCLib::Bohm(0.25, 3, 3, aKnots[0], 1, aDerivatives[0]);
    EXPECT_EQ(aDerivatives[0], anOrigin + 2.0);
    EXPECT_EQ(aDerivatives[1], 6.0);
    EXPECT_EQ(aDerivatives[2], 0.0);
    EXPECT_EQ(aDerivatives[3], 0.0);
  }
}

TEST(BSplCLibTest, InterpolationRetainsEndpointsAtDifferentMagnitudes)
{
  double aKnots[] = {0.0, 0.0, 0.0, 1.0, 1.0, 1.0};
  for (double aFirst : {1.e16, -1.e16})
  {
    double aPoles[] = {aFirst, aFirst, 1.0, 1.0};
    BSplCLib::Eval(1.0, 3, aKnots[0], 1, aPoles[0]);
    EXPECT_EQ(aPoles[0], 1.0);
  }
}

TEST(BSplCLibTest, InterpolationPreservesSmallEndpointWeights)
{
  // A small parameter offset can give a finite coordinate contribution when
  // the other pole is large. Computing the weight as 1 - the opposite weight
  // loses that offset before interpolation.
  for (const int aDegree : {1, 2, 3})
  {
    for (const int aDimension : {1, 2, 3, 4, 7})
    {
      SCOPED_TRACE(aDegree);
      SCOPED_TRACE(aDimension);
      NCollection_Array1<double> aKnots(0, 2 * aDegree - 1);
      for (size_t aKnot = 0; aKnot < static_cast<size_t>(aDegree); ++aKnot)
      {
        aKnots.ChangeAt(aKnot)           = 0.0;
        aKnots.ChangeAt(aKnot + aDegree) = 1.0;
      }
      const double               aParameter = 1.0e-16;
      const double               aLastPole  = std::pow(1.0e16, aDegree);
      NCollection_Array1<double> aPoles(0, (aDegree + 1) * aDimension - 1);
      aPoles.Init(0.0);
      for (size_t aCoordinate = 0; aCoordinate < static_cast<size_t>(aDimension); ++aCoordinate)
      {
        aPoles.ChangeAt(aDegree * aDimension + aCoordinate) = aLastPole;
      }
      NCollection_Array1<double> aValue = aPoles;
      BSplCLib::Eval(aParameter, aDegree, aKnots.ChangeAt(0), aDimension, aValue.ChangeAt(0));
      BSplCLib::Bohm(aParameter, aDegree, 1, aKnots.ChangeAt(0), aDimension, aPoles.ChangeAt(0));
      const double anExpected  = aLastPole * std::pow(aParameter, aDegree);
      const double aDerivative = aDegree * aLastPole * std::pow(aParameter, aDegree - 1);
      for (size_t aCoordinate = 0; aCoordinate < static_cast<size_t>(aDimension); ++aCoordinate)
      {
        EXPECT_NEAR(aValue.At(aCoordinate), anExpected, 1.e-14 * anExpected);
        EXPECT_NEAR(aPoles.At(aCoordinate), anExpected, 1.e-14 * anExpected);
        EXPECT_NEAR(aPoles.At(aDimension + aCoordinate), aDerivative, 1.e-14 * aDerivative);
      }
    }
  }
}

//==================================================================================================

TEST(BSplCLibTest, AllDerivativesRetainSmallEndpointWeights)
{
  for (const int aDegree : {2, 3, 8})
  {
    SCOPED_TRACE(aDegree);
    NCollection_Array1<double> aKnots(2 * aDegree);
    for (size_t aKnot = 0; aKnot < static_cast<size_t>(aDegree); ++aKnot)
    {
      aKnots.ChangeAt(aKnot)           = 0.0;
      aKnots.ChangeAt(aKnot + aDegree) = 1.0;
    }
    const double               aParameter = std::nextafter(1.0, 0.0);
    const double               aDistance  = 1.0 - aParameter;
    const double               aFirstPole = std::pow(1.e16, aDegree);
    NCollection_Array1<double> aPoles(aDegree + 1);
    aPoles.Init(0.0);
    aPoles.ChangeAt(0) = aFirstPole;
    BSplCLib::Bohm(aParameter, aDegree, aDegree, aKnots.ChangeAt(0), 1, aPoles.ChangeAt(0));
    double aFactor = aFirstPole;
    for (size_t aDerivative = 0; aDerivative <= static_cast<size_t>(aDegree); ++aDerivative)
    {
      const double anExpected =
        aFactor * std::pow(aDistance, aDegree - static_cast<int>(aDerivative));
      EXPECT_NEAR(aPoles.At(aDerivative), anExpected, 2.e-14 * std::abs(anExpected));
      aFactor *= -static_cast<double>(aDegree - aDerivative);
    }
  }
}

//==================================================================================================

TEST(BSplCLibTest, AllDerivativesRetainAffineCoordinates)
{
  for (const int aDegree : {8, 15, BSplCLib::MaxDegree()})
  {
    for (const double anOrigin : {1.e16, -1.e16})
    {
      SCOPED_TRACE(aDegree);
      SCOPED_TRACE(anOrigin);
      NCollection_Array1<double> aKnots(2 * aDegree);
      for (size_t aKnot = 0; aKnot < static_cast<size_t>(aDegree); ++aKnot)
      {
        aKnots.ChangeAt(aKnot)           = 0.0;
        aKnots.ChangeAt(aKnot + aDegree) = 1.0;
      }
      NCollection_Array1<double> aPoles(aDegree + 1);
      for (size_t aPole = 0; aPole < aPoles.Size(); ++aPole)
      {
        aPoles.ChangeAt(aPole) = anOrigin + 2.0 * static_cast<double>(aPole);
      }
      BSplCLib::Bohm(0.25, aDegree, aDegree, aKnots.ChangeAt(0), 1, aPoles.ChangeAt(0));
      EXPECT_EQ(aPoles.At(0), anOrigin + 0.5 * aDegree);
      EXPECT_EQ(aPoles.At(1), 2.0 * aDegree);
      for (size_t aDerivative = 2; aDerivative < aPoles.Size(); ++aDerivative)
      {
        EXPECT_EQ(aPoles.At(aDerivative), 0.0);
      }
    }
  }
}

//==================================================================================================

TEST(BSplCLibTest, AllDerivativesOfHighDegreePolynomial)
{
  constexpr size_t           aDegree    = 8;
  constexpr size_t           aDimension = 27;
  NCollection_Array1<double> aKnots(2 * aDegree);
  for (size_t aKnot = 0; aKnot < aDegree; ++aKnot)
  {
    aKnots.ChangeAt(aKnot)           = 0.0;
    aKnots.ChangeAt(aKnot + aDegree) = 1.0;
  }
  NCollection_Array1<double> aPoles((aDegree + 1) * aDimension);
  for (size_t aSample = 0; aSample <= 1000; ++aSample)
  {
    const double aParameter = static_cast<double>(aSample) / 1000.0;
    aPoles.Init(0.0);
    for (size_t aCoordinate = 0; aCoordinate < aDimension; ++aCoordinate)
    {
      aPoles.ChangeAt(aDegree * aDimension + aCoordinate) = static_cast<double>(aCoordinate + 1);
    }
    BSplCLib::Bohm(aParameter,
                   aDegree,
                   aDegree,
                   aKnots.ChangeAt(0),
                   aDimension,
                   aPoles.ChangeAt(0));
    double aFactor = 1.0;
    for (size_t aDerivative = 0; aDerivative <= aDegree; ++aDerivative)
    {
      const double aPower = std::pow(aParameter, static_cast<int>(aDegree - aDerivative));
      for (size_t aCoordinate = 0; aCoordinate < aDimension; ++aCoordinate)
      {
        const double anExpected = static_cast<double>(aCoordinate + 1) * aFactor * aPower;
        EXPECT_NEAR(aPoles.At(aDerivative * aDimension + aCoordinate),
                    anExpected,
                    1.e-12 * std::max(1.0, std::abs(anExpected)));
      }
      aFactor *= static_cast<double>(aDegree - aDerivative);
    }
  }
}

//==================================================================================================

TEST(BSplCLibTest, BasisInitializesAllDerivativeRows)
{
  double                           aKnotData[] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0};
  const NCollection_Array1<double> aKnots(aKnotData[0], 1, 8);
  math_Matrix                      aBasis(1, 4, 1, 4, std::nan(""));
  int                              aFirst = 0;
  ASSERT_EQ(BSplCLib::EvalBsplineBasis(3, 4, aKnots, 0.25, aFirst, aBasis), 0);
  for (size_t aDerivative = 0; aDerivative < 4; ++aDerivative)
  {
    double aSum = 0.0;
    for (size_t aPole = 0; aPole < 4; ++aPole)
    {
      const double aValue = aBasis(static_cast<int>(aDerivative) + 1, static_cast<int>(aPole) + 1);
      ASSERT_TRUE(std::isfinite(aValue));
      aSum += aValue;
    }
    EXPECT_NEAR(aSum, aDerivative == 0 ? 1.0 : 0.0, 1.e-12);
  }
}

//==================================================================================================

TEST(BSplCLibTest, InterpolationRemainsFiniteNearMaximumCoordinates)
{
  for (const int aDegree : {1, 3, 8, BSplCLib::MaxDegree()})
  {
    NCollection_Array1<double> aKnots(2 * aDegree);
    for (size_t aKnot = 0; aKnot < static_cast<size_t>(aDegree); ++aKnot)
    {
      aKnots.ChangeAt(aKnot)           = 0.0;
      aKnots.ChangeAt(aKnot + aDegree) = 1.0;
    }
    for (size_t aSample = 0; aSample <= 1002; ++aSample)
    {
      const double               aParameter = aSample == 1001   ? 1.e-16
                                              : aSample == 1002 ? std::nextafter(1.0, 0.0)
                                                                : static_cast<double>(aSample) / 1000.0;
      NCollection_Array1<double> aPoles(aDegree + 1);
      aPoles.Init(RealLast());
      aPoles.ChangeAt(0) = 0.0;
      BSplCLib::Eval(aParameter, aDegree, aKnots.ChangeAt(0), 1, aPoles.ChangeAt(0));
      EXPECT_TRUE(std::isfinite(aPoles.At(0))) << aDegree << ": " << aParameter;
    }
  }
}

//==================================================================================================

TEST(BSplCLibTest, AllDerivativesRetainContributionsBelowBasisExponentRange)
{
  constexpr size_t           aDegree    = BSplCLib::MaxDegree();
  constexpr double           aParameter = 1.e-16;
  constexpr double           aLastPole  = 1.e280;
  NCollection_Array1<double> aKnots(2 * aDegree);
  for (size_t aKnot = 0; aKnot < aDegree; ++aKnot)
  {
    aKnots.ChangeAt(aKnot)           = 0.0;
    aKnots.ChangeAt(aKnot + aDegree) = 1.0;
  }
  NCollection_Array1<double> aPoles(aDegree + 1);
  aPoles.Init(0.0);
  aPoles.ChangeAt(aDegree)          = aLastPole;
  NCollection_Array1<double> aValue = aPoles;
  BSplCLib::Eval(aParameter, aDegree, aKnots.ChangeAt(0), 1, aValue.ChangeAt(0));
  BSplCLib::Bohm(aParameter, aDegree, aDegree, aKnots.ChangeAt(0), 1, aPoles.ChangeAt(0));
  // The basis weight underflows in double, but the coordinate and derivatives are finite.
  long double aFactor = static_cast<long double>(aLastPole);
  for (size_t aDerivative = 0; aDerivative <= aDegree; ++aDerivative)
  {
    const double anExpected = static_cast<double>(
      aFactor
      * std::pow(static_cast<long double>(aParameter), static_cast<int>(aDegree - aDerivative)));
    EXPECT_NEAR(aPoles.At(aDerivative), anExpected, 1.e-13 * std::abs(anExpected));
    if (aDerivative == 0)
    {
      EXPECT_NEAR(aValue.At(0), anExpected, 1.e-13 * std::abs(anExpected));
    }
    aFactor *= static_cast<long double>(aDegree - aDerivative);
  }
}
