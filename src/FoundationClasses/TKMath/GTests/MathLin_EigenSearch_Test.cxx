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
#include <MathLin_Jacobi.hxx>
#include <math_EigenValuesSearcher.hxx>

#include <NCollection_Array1.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>

#include <algorithm>
#include <cmath>
#include <limits>
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
  const size_t aN = theDiag.Size();
  math_Matrix  aM(aN, aN, 0.0);
  for (size_t i = 0; i < aN; ++i)
  {
    aM.ChangeAt(i, i) = theDiag.At(i);
  }
  for (size_t i = 1; i < aN; ++i)
  {
    const double aE       = theSubdiag.At(i);
    aM.ChangeAt(i, i - 1) = aE;
    aM.ChangeAt(i - 1, i) = aE;
  }
  return aM;
}

void BuildLegacyArrays(const math_Vector&          theDiag,
                       const math_Vector&          theSubdiag,
                       NCollection_Array1<double>& theDiagLegacy,
                       NCollection_Array1<double>& theSubdiagLegacy)
{
  const size_t aN = theDiag.Size();
  for (size_t i = 0; i < aN; ++i)
  {
    theDiagLegacy.ChangeValue(static_cast<int>(i) + 1)    = theDiag.At(i);
    theSubdiagLegacy.ChangeValue(static_cast<int>(i) + 1) = theSubdiag.At(i);
  }
}

std::vector<double> SortedEigenValuesFromLegacy(const math_EigenValuesSearcher& theLegacy)
{
  const int           aN = theLegacy.Dimension();
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
  aVals.reserve(aEig.Size());
  for (size_t i = 0; i < aEig.Size(); ++i)
  {
    aVals.push_back(aEig.At(i));
  }
  std::sort(aVals.begin(), aVals.end());
  return aVals;
}

double VectorNorm2(const math_Vector& theVec)
{
  double aNorm2 = 0.0;
  for (size_t i = 0; i < theVec.Size(); ++i)
  {
    aNorm2 += theVec.At(i) * theVec.At(i);
  }
  return std::sqrt(aNorm2);
}

double PairResidualInfinity(const math_Matrix& theMatrix,
                            double             theLambda,
                            const math_Vector& theVector)
{
  const size_t aN   = theMatrix.RowSize();
  double       aMax = 0.0;
  for (size_t i = 0; i < aN; ++i)
  {
    double aAx = 0.0;
    for (size_t j = 0; j < aN; ++j)
    {
      aAx += theMatrix.At(i, j) * theVector.At(j);
    }
    const double aRes = std::abs(aAx - theLambda * theVector.At(i));
    if (aRes > aMax)
    {
      aMax = aRes;
    }
  }
  return aMax;
}

double DotProduct(const math_Vector& theV1, const math_Vector& theV2)
{
  const size_t aN   = theV1.Size();
  double       aDot = 0.0;
  for (size_t i = 0; i < aN; ++i)
  {
    aDot += theV1.At(i) * theV2.At(i);
  }
  return aDot;
}

} // namespace

TEST(MathLin_EigenSearch_Test, CoexistsWithJacobiInDangerousIncludeOrder)
{
  math_Vector aDiag(size_t{2}, 0.0);
  math_Vector aSubdiag(size_t{2}, 0.0);
  aDiag[0]    = 1.0;
  aDiag[1]    = 2.0;
  aSubdiag[1] = 0.5;

  const MathLin::EigenResult aTridiagonal = MathLin::EigenTridiagonal(aDiag, aSubdiag);

  math_Matrix aSymmetric(2, 2, 0.0);
  aSymmetric.ChangeAt(0, 0)            = 1.0;
  aSymmetric.ChangeAt(0, 1)            = 0.5;
  aSymmetric.ChangeAt(1, 0)            = 0.5;
  aSymmetric.ChangeAt(1, 1)            = 2.0;
  const MathUtils::EigenResult aJacobi = MathLin::Jacobi(aSymmetric);

  EXPECT_TRUE(aTridiagonal.IsDone());
  EXPECT_TRUE(aJacobi.IsDone());
}

TEST(MathLin_EigenSearch_Test, AcceptsDocumentedSubdiagonalShape)
{
  math_Vector aDiag(size_t{3}, 2.0);
  math_Vector aSubdiag(size_t{2}, 1.0);
  const MathLin::EigenResult aResult = MathLin::EigenTridiagonal(aDiag, aSubdiag);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.Dimension, 3u);
  EXPECT_EQ(aResult.EigenValues->Size(), 3u);
  const std::vector<double> aValues = SortedEigenValuesFromModern(aResult);
  EXPECT_NEAR(aValues[0], 2.0 - std::sqrt(2.0), THE_EIGEN_TOL);
  EXPECT_NEAR(aValues[1], 2.0, THE_EIGEN_TOL);
  EXPECT_NEAR(aValues[2], 2.0 + std::sqrt(2.0), THE_EIGEN_TOL);
}

TEST(MathLin_EigenSearch_Test, LargeFiniteDiagonalDoesNotDeflateCoupling)
{
  const double aMagnitude = std::numeric_limits<double>::max() / 2.0;
  math_Vector aDiag(size_t{2}, aMagnitude);
  math_Vector aSubdiag(size_t{1}, aMagnitude / 4.0);

  const MathLin::EigenResult aResult = MathLin::EigenTridiagonal(aDiag, aSubdiag);
  ASSERT_TRUE(aResult.IsDone());
  std::vector<double> aValues = SortedEigenValuesFromModern(aResult);
  ASSERT_EQ(aValues.size(), 2u);
  EXPECT_NEAR(aValues[0] / aMagnitude, 0.75, 1.0e-12);
  EXPECT_NEAR(aValues[1] / aMagnitude, 1.25, 1.0e-12);
}

TEST(MathLin_EigenSearch_Test, InputAndIterationStatuses)
{
  math_Vector aOne(size_t{1}, 7.0);
  math_Vector anEmpty(size_t{0});
  const MathLin::EigenResult aSingle = MathLin::EigenTridiagonal(aOne, anEmpty);
  ASSERT_TRUE(aSingle.IsDone());
  EXPECT_DOUBLE_EQ(aSingle.EigenValues->At(0), 7.0);

  math_Vector aWrong(size_t{2}, 0.0);
  EXPECT_EQ(MathLin::EigenTridiagonal(aOne, aWrong).Status, MathUtils::Status::InvalidInput);
  EXPECT_EQ(MathLin::EigenTridiagonal(aOne, anEmpty, 0).Status,
            MathUtils::Status::InvalidInput);

  math_Vector aDiag(size_t{4}, 2.0);
  math_Vector aSubdiag(size_t{3}, 1.0);
  EXPECT_EQ(MathLin::EigenTridiagonal(aDiag, aSubdiag, 1).Status,
            MathUtils::Status::MaxIterations);
}

TEST(MathLin_EigenSearch_Test, BasicParityWithLegacy_3x3)
{
  math_Vector aDiag(3);
  math_Vector aSubdiag(3);
  aDiag.ChangeAt(0)    = 4.0;
  aDiag.ChangeAt(1)    = 4.0;
  aDiag.ChangeAt(2)    = 4.0;
  aSubdiag.ChangeAt(0) = 0.0;
  aSubdiag.ChangeAt(1) = 1.0;
  aSubdiag.ChangeAt(2) = 1.0;

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

  const math_Matrix  aA       = BuildSymmetricTridiagonal(aDiag, aSubdiag);
  const math_Vector& aEigVals = *aModern.EigenValues;
  for (size_t i = 0; i < aModern.Dimension; ++i)
  {
    const math_Vector aVec = MathLin::GetEigenVector(aModern, i);
    EXPECT_NEAR(VectorNorm2(aVec), 1.0, THE_NORMALIZED_TOL);
    EXPECT_NEAR(PairResidualInfinity(aA, aEigVals.At(i), aVec), 0.0, THE_RESIDUAL_TOL);
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
  std::mt19937                           aGen(123456u);
  std::uniform_int_distribution<int>     aDimDist(2, 32);
  std::uniform_real_distribution<double> aValDist(-100.0, 100.0);

  for (int aCase = 0; aCase < THE_RANDOM_NB_CASES; ++aCase)
  {
    const int aN = aDimDist(aGen);

    math_Vector aDiag(static_cast<size_t>(aN));
    math_Vector aSubdiag(static_cast<size_t>(aN));
    for (size_t i = 0; i < aDiag.Size(); ++i)
    {
      aDiag.ChangeAt(i)    = aValDist(aGen);
      aSubdiag.ChangeAt(i) = (i == 0) ? 0.0 : aValDist(aGen);
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

    const math_Matrix  aA       = BuildSymmetricTridiagonal(aDiag, aSubdiag);
    const math_Vector& aEigVals = *aModern.EigenValues;

    for (size_t i = 0; i < aModern.Dimension; ++i)
    {
      const math_Vector aVec = MathLin::GetEigenVector(aModern, i);
      EXPECT_NEAR(VectorNorm2(aVec), 1.0, THE_NORMALIZED_TOL) << "case=" << aCase;
      EXPECT_NEAR(PairResidualInfinity(aA, aEigVals.At(i), aVec), 0.0, THE_RESIDUAL_TOL)
        << "case=" << aCase;
    }

    for (size_t i = 0; i < aModern.Dimension; ++i)
    {
      const math_Vector aVecI = MathLin::GetEigenVector(aModern, i);
      for (size_t j = i + 1; j < aModern.Dimension; ++j)
      {
        const math_Vector aVecJ = MathLin::GetEigenVector(aModern, j);
        EXPECT_NEAR(DotProduct(aVecI, aVecJ), 0.0, THE_ORTHOGONAL_TOL) << "case=" << aCase;
      }
    }
  }
}
