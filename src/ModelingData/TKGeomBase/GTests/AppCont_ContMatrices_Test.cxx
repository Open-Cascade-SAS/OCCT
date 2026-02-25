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

#include <AppCont_ContMatrices.pxx>
#include <math.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <Standard_DimensionError.hxx>

#include <gtest/gtest.h>

static constexpr double THE_FORMULA_TOL  = 1e-14;
static constexpr double THE_SYMMETRY_TOL = 1e-15;

// Helper: compute binomial coefficient C(n,k).
static double testBinomCoeff(int theN, int theK)
{
  if (theK < 0 || theK > theN)
  {
    return 0.0;
  }
  if (theK > theN - theK)
  {
    theK = theN - theK;
  }
  double aResult = 1.0;
  for (int i = 0; i < theK; ++i)
  {
    aResult = aResult * (theN - i) / (i + 1);
  }
  return aResult;
}

// Helper: compute Bernstein mass matrix entry M(i,j) for degree n (0-based).
static double testBernsteinMassEntry(int theN, int theI, int theJ)
{
  return testBinomCoeff(theN, theI) * testBinomCoeff(theN, theJ)
         / ((2 * theN + 1) * testBinomCoeff(2 * theN, theI + theJ));
}

// Verify MMatrix entries match the closed-form Bernstein mass matrix formula.
TEST(AppCont_ContMatricesTest, MMatrix_MatchesFormula)
{
  for (int aClasse = 2; aClasse <= 20; aClasse++)
  {
    math_Matrix aMat(1, aClasse, 1, aClasse);
    AppCont_ContMatrices::MMatrix(aClasse, aMat);

    const int n = aClasse - 1;
    for (int i = 1; i <= aClasse; i++)
    {
      for (int j = 1; j <= aClasse; j++)
      {
        const double anExpected = testBernsteinMassEntry(n, i - 1, j - 1);
        EXPECT_NEAR(aMat(i, j), anExpected, THE_FORMULA_TOL)
          << "classe=" << aClasse << " i=" << i << " j=" << j;
      }
    }
  }
}

// Verify MMatrix is symmetric.
TEST(AppCont_ContMatricesTest, MMatrix_Symmetric)
{
  for (int aClasse = 2; aClasse <= 20; aClasse++)
  {
    math_Matrix aMat(1, aClasse, 1, aClasse);
    AppCont_ContMatrices::MMatrix(aClasse, aMat);

    for (int i = 1; i <= aClasse; i++)
    {
      for (int j = i + 1; j <= aClasse; j++)
      {
        EXPECT_NEAR(aMat(i, j), aMat(j, i), THE_SYMMETRY_TOL)
          << "classe=" << aClasse << " i=" << i << " j=" << j;
      }
    }
  }
}

// Verify InvMMatrix * MMatrix = Identity for low-to-moderate degrees.
TEST(AppCont_ContMatricesTest, InvMMatrix_IsInverse)
{
  for (int aClasse = 2; aClasse <= 12; aClasse++)
  {
    math_Matrix aMat(1, aClasse, 1, aClasse);
    math_Matrix anInvMat(1, aClasse, 1, aClasse);
    AppCont_ContMatrices::MMatrix(aClasse, aMat);
    AppCont_ContMatrices::InvMMatrix(aClasse, anInvMat);

    math_Matrix aProduct = anInvMat * aMat;
    for (int i = 1; i <= aClasse; i++)
    {
      for (int j = 1; j <= aClasse; j++)
      {
        const double anExpected = (i == j) ? 1.0 : 0.0;
        EXPECT_NEAR(aProduct(i, j), anExpected, 1e-8)
          << "classe=" << aClasse << " i=" << i << " j=" << j;
      }
    }
  }
}

// Verify InvMMatrix is symmetric (inverse of symmetric matrix is symmetric).
TEST(AppCont_ContMatricesTest, InvMMatrix_Symmetric)
{
  for (int aClasse = 2; aClasse <= 24; aClasse++)
  {
    math_Matrix anInvMat(1, aClasse, 1, aClasse);
    AppCont_ContMatrices::InvMMatrix(aClasse, anInvMat);

    for (int i = 1; i <= aClasse; i++)
    {
      for (int j = i + 1; j <= aClasse; j++)
      {
        EXPECT_DOUBLE_EQ(anInvMat(i, j), anInvMat(j, i))
          << "classe=" << aClasse << " i=" << i << " j=" << j;
      }
    }
  }
}

// Verify strict legacy class limits for matrix providers.
TEST(AppCont_ContMatricesTest, LegacyRangeLimits)
{
  {
    math_Matrix aMat24(1, 24, 1, 24);
    math_Matrix aMat25(1, 25, 1, 25);
    EXPECT_NO_THROW(AppCont_ContMatrices::MMatrix(24, aMat24));
    EXPECT_THROW(AppCont_ContMatrices::MMatrix(25, aMat25), Standard_DimensionError);
  }

  {
    math_Matrix anInv24(1, 24, 1, 24);
    math_Matrix anInv25(1, 25, 1, 25);
    EXPECT_NO_THROW(AppCont_ContMatrices::InvMMatrix(24, anInv24));
    EXPECT_THROW(AppCont_ContMatrices::InvMMatrix(25, anInv25), Standard_DimensionError);
  }

  {
    math_Matrix anIBP26(1, 24, 1, 24);
    math_Matrix anIBP27(1, 25, 1, 25);
    EXPECT_NO_THROW(AppCont_ContMatrices::IBPMatrix(26, anIBP26));
    EXPECT_THROW(AppCont_ContMatrices::IBPMatrix(27, anIBP27), Standard_DimensionError);
  }

  {
    math_Matrix anIBT26(1, 22, 1, 22);
    math_Matrix anIBT27(1, 23, 1, 23);
    EXPECT_NO_THROW(AppCont_ContMatrices::IBTMatrix(26, anIBT26));
    EXPECT_THROW(AppCont_ContMatrices::IBTMatrix(27, anIBT27), Standard_DimensionError);
  }

  {
    math_Matrix aVB26x24(1, 26, 1, 24);
    math_Matrix aVB27x24(1, 27, 1, 24);
    math_Matrix aVB26x25(1, 26, 1, 25);
    EXPECT_NO_THROW(AppCont_ContMatrices::VBernstein(26, 24, aVB26x24));
    EXPECT_THROW(AppCont_ContMatrices::VBernstein(27, 24, aVB27x24), Standard_DimensionError);
    EXPECT_THROW(AppCont_ContMatrices::VBernstein(26, 25, aVB26x25), Standard_DimensionError);
  }
}

// Verify IBPMatrix is symmetric for degrees where legacy data retains symmetry.
TEST(AppCont_ContMatricesTest, IBPMatrix_Symmetric)
{
  for (int aClasse = 3; aClasse <= 22; aClasse++)
  {
    const int   aDim = aClasse - 2;
    math_Matrix anIBP(1, aDim, 1, aDim);
    AppCont_ContMatrices::IBPMatrix(aClasse, anIBP);

    for (int i = 1; i <= aDim; i++)
    {
      for (int j = i + 1; j <= aDim; j++)
      {
        EXPECT_DOUBLE_EQ(anIBP(i, j), anIBP(j, i))
          << "classe=" << aClasse << " i=" << i << " j=" << j;
      }
    }
  }
}

// Verify IBTMatrix is symmetric and has correct dimensions for all cached classes.
TEST(AppCont_ContMatricesTest, IBTMatrix_Symmetric)
{
  for (int aClasse = 5; aClasse <= 26; aClasse++)
  {
    const int   aDim = aClasse - 4;
    math_Matrix anIBT(1, aDim, 1, aDim);
    AppCont_ContMatrices::IBTMatrix(aClasse, anIBT);

    for (int i = 1; i <= aDim; i++)
    {
      for (int j = i + 1; j <= aDim; j++)
      {
        EXPECT_DOUBLE_EQ(anIBT(i, j), anIBT(j, i))
          << "classe=" << aClasse << " i=" << i << " j=" << j;
      }
    }
  }
}

// Verify VBernstein values match direct Bernstein evaluation at Gauss points.
TEST(AppCont_ContMatricesTest, VBernstein_MatchesBernsteinEval)
{
  for (int aClasse = 2; aClasse <= 20; aClasse++)
  {
    for (int aNbPts : {4, 8, 12, 20, 24})
    {
      math_Matrix aVB(1, aClasse, 1, aNbPts);
      AppCont_ContMatrices::VBernstein(aClasse, aNbPts, aVB);

      // Get Gauss-Legendre points on [-1, 1] and remap to [0, 1].
      math_Vector aGaussP(1, aNbPts);
      math::GaussPoints(aNbPts, aGaussP);

      math_Vector aParams(1, aNbPts);
      const int   aHalf = (aNbPts + 1) / 2;
      for (int k = 1; k <= aNbPts; k++)
      {
        const double aT = 0.5 * (1.0 + aGaussP(k));
        if (k <= aHalf)
        {
          aParams(aNbPts - k + 1) = aT;
        }
        else
        {
          aParams(k - aHalf) = aT;
        }
      }

      const int n = aClasse - 1;
      for (int i = 1; i <= aClasse; i++)
      {
        const int    aI     = i - 1;
        const double aBinom = testBinomCoeff(n, aI);
        for (int j = 1; j <= aNbPts; j++)
        {
          const double aT         = aParams(j);
          const double anExpected = aBinom * std::pow(aT, aI) * std::pow(1.0 - aT, n - aI);
          EXPECT_NEAR(aVB(i, j), anExpected, THE_FORMULA_TOL)
            << "classe=" << aClasse << " nbpoints=" << aNbPts << " i=" << i << " j=" << j;
        }
      }
    }
  }
}

// Verify partition of unity: sum of Bernstein basis values at each point should be 1.
TEST(AppCont_ContMatricesTest, VBernstein_PartitionOfUnity)
{
  for (int aClasse : {3, 5, 10, 20})
  {
    const int   aNbPts = 12;
    math_Matrix aVB(1, aClasse, 1, aNbPts);
    AppCont_ContMatrices::VBernstein(aClasse, aNbPts, aVB);

    for (int j = 1; j <= aNbPts; j++)
    {
      double aSum = 0.0;
      for (int i = 1; i <= aClasse; i++)
      {
        aSum += aVB(i, j);
      }
      EXPECT_NEAR(aSum, 1.0, 1e-13) << "classe=" << aClasse << " point=" << j;
    }
  }
}
