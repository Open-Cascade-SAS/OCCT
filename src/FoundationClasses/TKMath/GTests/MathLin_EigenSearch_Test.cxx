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

#include <MathLin_EigenSearch.hxx>
#include <math_EigenValuesSearcher.hxx>

#include <NCollection_Array1.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>

#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

namespace
{
constexpr double THE_EIGEN_TOL       = 1.0e-10;
constexpr double THE_RESIDUAL_TOL    = 1.0e-10;
constexpr double THE_ORTHOGONAL_TOL  = 1.0e-10;
constexpr double THE_NORMALIZED_TOL  = 1.0e-10;
constexpr int    THE_RANDOM_NB_CASES = 120;

math_Matrix BuildSymmetricTridiagonal(const math_Vector& theDiag, const math_Vector& theSubdiag)
{
  const int   aN = theDiag.Length();
  math_Matrix aM(1, aN, 1, aN, 0.0);
  for (int i = 1; i <= aN; ++i)
  {
    aM(i, i) = theDiag(theDiag.Lower() + i - 1);
  }
  for (int i = 2; i <= aN; ++i)
  {
    const double aE = theSubdiag(theSubdiag.Lower() + i - 1);
    aM(i, i - 1)    = aE;
    aM(i - 1, i)    = aE;
  }
  return aM;
}

void BuildLegacyArrays(const math_Vector&       theDiag,
                       const math_Vector&       theSubdiag,
                       NCollection_Array1<double>& theDiagLegacy,
                       NCollection_Array1<double>& theSubdiagLegacy)
{
  const int aN = theDiag.Length();
  for (int i = 1; i <= aN; ++i)
  {
    theDiagLegacy(i)    = theDiag(theDiag.Lower() + i - 1);
    theSubdiagLegacy(i) = theSubdiag(theSubdiag.Lower() + i - 1);
  }
}

std::vector<double> SortedEigenValuesFromLegacy(const math_EigenValuesSearcher& theLegacy)
{
  const int               aN = theLegacy.Dimension();
  std::vector<double> aVals;
  aVals.reserve(static_cast<size_t>(aN));
  for (int i = 1; i <= aN; ++i)
  {
    aVals.push_back(theLegacy.EigenValue(i));
  }
  std::sort(aVals.begin(), aVals.end());
  return aVals;
}

std::vector<double> SortedEigenValuesFromModern(const MathLin::EigenResult& theModern)
{
  std::vector<double> aVals;
  if (!theModern.EigenValues.has_value())
  {
    return aVals;
  }

  const math_Vector& aEig = *theModern.EigenValues;
  aVals.reserve(static_cast<size_t>(aEig.Length()));
  for (int i = aEig.Lower(); i <= aEig.Upper(); ++i)
  {
    aVals.push_back(aEig(i));
  }
  std::sort(aVals.begin(), aVals.end());
  return aVals;
}

double VectorNorm2(const math_Vector& theVec)
{
  double aNorm2 = 0.0;
  for (int i = theVec.Lower(); i <= theVec.Upper(); ++i)
  {
    aNorm2 += theVec(i) * theVec(i);
  }
  return std::sqrt(aNorm2);
}

double PairResidualInfinity(const math_Matrix& theMatrix, double theLambda, const math_Vector& theVector)
{
  const int aN = theMatrix.RowNumber();
  double    aMax = 0.0;
  for (int i = 1; i <= aN; ++i)
  {
    double aAx = 0.0;
    for (int j = 1; j <= aN; ++j)
    {
      aAx += theMatrix(i, j) * theVector(j);
    }
    const double aRes = std::abs(aAx - theLambda * theVector(i));
    if (aRes > aMax)
    {
      aMax = aRes;
    }
  }
  return aMax;
}

double DotProduct(const math_Vector& theV1, const math_Vector& theV2)
{
  const int aN = theV1.Length();
  double    aDot = 0.0;
  for (int i = 1; i <= aN; ++i)
  {
    aDot += theV1(i) * theV2(i);
  }
  return aDot;
}

} // namespace

TEST(MathLin_EigenSearch_Test, BasicParityWithLegacy_3x3)
{
  math_Vector aDiag(1, 3);
  math_Vector aSubdiag(1, 3);
  aDiag(1)    = 4.0;
  aDiag(2)    = 4.0;
  aDiag(3)    = 4.0;
  aSubdiag(1) = 0.0;
  aSubdiag(2) = 1.0;
  aSubdiag(3) = 1.0;

  NCollection_Array1<double> aDiagLegacy(1, 3);
  NCollection_Array1<double> aSubdiagLegacy(1, 3);
  BuildLegacyArrays(aDiag, aSubdiag, aDiagLegacy, aSubdiagLegacy);

  math_EigenValuesSearcher aLegacy(aDiagLegacy, aSubdiagLegacy);
  MathLin::EigenResult     aModern = MathLin::EigenTridiagonal(aDiag, aSubdiag);

  ASSERT_TRUE(aLegacy.IsDone());
  ASSERT_TRUE(aModern.IsDone());
  ASSERT_TRUE(aModern.EigenValues.has_value());
  ASSERT_TRUE(aModern.EigenVectors.has_value());

  const std::vector<double> aLegacySorted = SortedEigenValuesFromLegacy(aLegacy);
  const std::vector<double> aModernSorted = SortedEigenValuesFromModern(aModern);
  ASSERT_EQ(aLegacySorted.size(), aModernSorted.size());
  for (size_t i = 0; i < aLegacySorted.size(); ++i)
  {
    EXPECT_NEAR(aLegacySorted[i], aModernSorted[i], THE_EIGEN_TOL);
  }

  const math_Matrix aA = BuildSymmetricTridiagonal(aDiag, aSubdiag);
  const math_Vector& aEigVals = *aModern.EigenValues;
  for (int i = 1; i <= 3; ++i)
  {
    const math_Vector aVec = MathLin::GetEigenVector(aModern, i);
    EXPECT_NEAR(VectorNorm2(aVec), 1.0, THE_NORMALIZED_TOL);
    EXPECT_NEAR(PairResidualInfinity(aA, aEigVals(i), aVec), 0.0, THE_RESIDUAL_TOL);
  }
}

TEST(MathLin_EigenSearch_Test, HandlesNonOneLowerBounds)
{
  math_Vector aDiag(-2, 2);
  math_Vector aSubdiag(-2, 2);
  aDiag(-2)    = 1.0;
  aDiag(-1)    = 3.0;
  aDiag(0)     = -2.0;
  aDiag(1)     = 5.0;
  aDiag(2)     = 4.0;
  aSubdiag(-2) = 0.0;
  aSubdiag(-1) = 0.3;
  aSubdiag(0)  = -0.2;
  aSubdiag(1)  = 0.7;
  aSubdiag(2)  = -1.1;

  NCollection_Array1<double> aDiagLegacy(-2, 2);
  NCollection_Array1<double> aSubdiagLegacy(-2, 2);
  for (int i = -2; i <= 2; ++i)
  {
    aDiagLegacy(i)    = aDiag(i);
    aSubdiagLegacy(i) = aSubdiag(i);
  }

  math_EigenValuesSearcher aLegacy(aDiagLegacy, aSubdiagLegacy);
  MathLin::EigenResult     aModern = MathLin::EigenTridiagonal(aDiag, aSubdiag);

  ASSERT_EQ(aLegacy.IsDone(), aModern.IsDone());
  ASSERT_TRUE(aLegacy.IsDone());

  const std::vector<double> aLegacySorted = SortedEigenValuesFromLegacy(aLegacy);
  const std::vector<double> aModernSorted = SortedEigenValuesFromModern(aModern);
  ASSERT_EQ(aLegacySorted.size(), aModernSorted.size());
  for (size_t i = 0; i < aLegacySorted.size(); ++i)
  {
    EXPECT_NEAR(aLegacySorted[i], aModernSorted[i], THE_EIGEN_TOL);
  }
}

TEST(MathLin_EigenSearch_Test, RandomParityAndOrthogonality)
{
  std::mt19937                     aGen(123456u);
  std::uniform_int_distribution<int> aDimDist(2, 32);
  std::uniform_real_distribution<double> aValDist(-100.0, 100.0);

  for (int aCase = 0; aCase < THE_RANDOM_NB_CASES; ++aCase)
  {
    const int aN = aDimDist(aGen);

    math_Vector aDiag(1, aN);
    math_Vector aSubdiag(1, aN);
    for (int i = 1; i <= aN; ++i)
    {
      aDiag(i)    = aValDist(aGen);
      aSubdiag(i) = (i == 1) ? 0.0 : aValDist(aGen);
    }

    NCollection_Array1<double> aDiagLegacy(1, aN);
    NCollection_Array1<double> aSubdiagLegacy(1, aN);
    BuildLegacyArrays(aDiag, aSubdiag, aDiagLegacy, aSubdiagLegacy);

    math_EigenValuesSearcher aLegacy(aDiagLegacy, aSubdiagLegacy);
    MathLin::EigenResult     aModern = MathLin::EigenTridiagonal(aDiag, aSubdiag);

    ASSERT_EQ(aLegacy.IsDone(), aModern.IsDone()) << "case=" << aCase;
    if (!aLegacy.IsDone())
    {
      continue;
    }

    ASSERT_TRUE(aModern.EigenValues.has_value()) << "case=" << aCase;
    ASSERT_TRUE(aModern.EigenVectors.has_value()) << "case=" << aCase;

    const std::vector<double> aLegacySorted = SortedEigenValuesFromLegacy(aLegacy);
    const std::vector<double> aModernSorted = SortedEigenValuesFromModern(aModern);
    ASSERT_EQ(aLegacySorted.size(), aModernSorted.size()) << "case=" << aCase;
    for (size_t i = 0; i < aLegacySorted.size(); ++i)
    {
      EXPECT_NEAR(aLegacySorted[i], aModernSorted[i], THE_EIGEN_TOL) << "case=" << aCase;
    }

    const math_Matrix aA = BuildSymmetricTridiagonal(aDiag, aSubdiag);
    const math_Vector& aEigVals = *aModern.EigenValues;

    for (int i = 1; i <= aN; ++i)
    {
      const math_Vector aVec = MathLin::GetEigenVector(aModern, i);
      EXPECT_NEAR(VectorNorm2(aVec), 1.0, THE_NORMALIZED_TOL) << "case=" << aCase;
      EXPECT_NEAR(PairResidualInfinity(aA, aEigVals(i), aVec), 0.0, THE_RESIDUAL_TOL)
        << "case=" << aCase;
    }

    for (int i = 1; i <= aN; ++i)
    {
      const math_Vector aVecI = MathLin::GetEigenVector(aModern, i);
      for (int j = i + 1; j <= aN; ++j)
      {
        const math_Vector aVecJ = MathLin::GetEigenVector(aModern, j);
        EXPECT_NEAR(DotProduct(aVecI, aVecJ), 0.0, THE_ORTHOGONAL_TOL) << "case=" << aCase;
      }
    }
  }
}
