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
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

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
