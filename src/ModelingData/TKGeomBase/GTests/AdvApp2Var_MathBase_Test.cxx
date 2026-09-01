// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <AdvApp2Var_ApproxF2var.hxx>
#include <AdvApp2Var_MathBase.hxx>
#include <AdvApp2Var_SysBase.hxx>

#include <gtest/gtest.h>

#include <array>
#include <atomic>
#include <cmath>
#include <thread>
#include <vector>

namespace
{
void checkHermiteConversion(const int                  theOrder,
                            const std::vector<double>& theConstraints,
                            const std::vector<double>& theExpected)
{
  int                   aDimension        = 1;
  int                   aCurveCount       = 1;
  int                   aCoefficientCount = static_cast<int>(theExpected.size());
  int                   aCoefficientLimit = aCoefficientCount;
  int                   anError           = 0;
  std::array<double, 2> aRange            = {-1.0, 1.0};
  std::vector<double>   aResult(theExpected.size(), 0.0);
  std::vector<double>   aConstraints = theConstraints;

  int anOrder = theOrder;
  AdvApp2Var_MathBase::mmhjcan_(&aDimension,
                                &aCurveCount,
                                &aCoefficientCount,
                                &anOrder,
                                &aCoefficientLimit,
                                aConstraints.data(),
                                aRange.data(),
                                aResult.data(),
                                &anError);

  ASSERT_EQ(anError, 0);
  ASSERT_EQ(aResult.size(), theExpected.size());
  for (std::size_t anIndex = 0; anIndex < aResult.size(); ++anIndex)
  {
    EXPECT_NEAR(aResult[anIndex], theExpected[anIndex], 1.0e-12) << "index " << anIndex;
  }
}

double evaluateDerivative(const std::vector<double>& theCoefficients,
                          const double               theParameter,
                          const int                  theDerivative)
{
  double aValue = 0.0;
  for (std::size_t aPower = theDerivative; aPower < theCoefficients.size(); ++aPower)
  {
    double aFactor = 1.0;
    for (int aDerivative = 0; aDerivative < theDerivative; ++aDerivative)
    {
      aFactor *= static_cast<double>(aPower - aDerivative);
    }
    aValue += aFactor * theCoefficients[aPower]
              * std::pow(theParameter, static_cast<int>(aPower) - theDerivative);
  }
  return aValue;
}
} // namespace

TEST(AdvApp2Var_MathBaseTest, HermiteConversion_ReconstructsCanonicalPolynomials)
{
  checkHermiteConversion(0, {2.0, 4.0}, {3.0, 1.0});
  checkHermiteConversion(1, {-2.0, 8.0, 10.0, 20.0}, {1.0, 2.0, 3.0, 4.0});
  checkHermiteConversion(2, {-3.0, 18.0, -78.0, 21.0, 70.0, 210.0}, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0});
}

TEST(AdvApp2Var_MathBaseTest, HermiteConversion_IsSafeForConcurrentCalls)
{
  std::atomic<bool>        hasFailure{false};
  std::vector<std::thread> aThreads;
  for (int aThreadIndex = 0; aThreadIndex < 8; ++aThreadIndex)
  {
    aThreads.emplace_back([&hasFailure]() {
      for (int anIteration = 0; anIteration < 200 && !hasFailure.load(); ++anIteration)
      {
        int                   aDimension        = 1;
        int                   aCurveCount       = 1;
        int                   aCoefficientCount = 6;
        int                   anOrder           = 2;
        int                   aCoefficientLimit = 6;
        int                   anError           = 0;
        std::array<double, 2> aRange            = {-1.0, 1.0};
        std::array<double, 6> aConstraints      = {-3.0, 18.0, -78.0, 21.0, 70.0, 210.0};
        std::array<double, 6> aResult{};

        AdvApp2Var_MathBase::mmhjcan_(&aDimension,
                                      &aCurveCount,
                                      &aCoefficientCount,
                                      &anOrder,
                                      &aCoefficientLimit,
                                      aConstraints.data(),
                                      aRange.data(),
                                      aResult.data(),
                                      &anError);
        for (std::size_t anIndex = 0; anIndex < aResult.size(); ++anIndex)
        {
          if (anError != 0
              || std::abs(aResult[anIndex] - static_cast<double>(anIndex + 1)) > 1.0e-12)
          {
            hasFailure.store(true);
            break;
          }
        }
      }
    });
  }

  for (std::thread& aThread : aThreads)
  {
    aThread.join();
  }
  EXPECT_FALSE(hasFailure.load());
}

TEST(AdvApp2Var_MathBaseTest, HermiteConversion_PreservesEveryEndpointConstraint)
{
  for (int anOrder = 0; anOrder <= 2; ++anOrder)
  {
    const int aCoefficientCount = 2 * (anOrder + 1);
    for (int aConstraintIndex = 0; aConstraintIndex < aCoefficientCount; ++aConstraintIndex)
    {
      int                   aDimension        = 1;
      int                   aCurveCount       = 1;
      int                   aCoefficientLimit = aCoefficientCount;
      int                   anError           = 0;
      std::array<double, 2> aRange            = {-1.0, 1.0};
      std::vector<double>   aConstraints(aCoefficientCount, 0.0);
      std::vector<double>   aResult(aCoefficientCount, 0.0);
      aConstraints[aConstraintIndex] = 1.0;

      int aMutableCoefficientCount = aCoefficientCount;
      int aMutableOrder            = anOrder;
      AdvApp2Var_MathBase::mmhjcan_(&aDimension,
                                    &aCurveCount,
                                    &aMutableCoefficientCount,
                                    &aMutableOrder,
                                    &aCoefficientLimit,
                                    aConstraints.data(),
                                    aRange.data(),
                                    aResult.data(),
                                    &anError);

      ASSERT_EQ(anError, 0);
      for (int anEndpoint = 0; anEndpoint < 2; ++anEndpoint)
      {
        for (int aDerivative = 0; aDerivative <= anOrder; ++aDerivative)
        {
          const int    anEvaluatedConstraint = anEndpoint * (anOrder + 1) + aDerivative;
          const double anExpected = anEvaluatedConstraint == aConstraintIndex ? 1.0 : 0.0;
          EXPECT_NEAR(evaluateDerivative(aResult, anEndpoint == 0 ? -1.0 : 1.0, aDerivative),
                      anExpected,
                      1.0e-12);
        }
      }
    }
  }
}

TEST(AdvApp2Var_MathBaseTest, JacobiMaxima_UsesCanonicalTables)
{
  std::array<double, 64> aMaxima{};

  int aDegree = 58;
  int anOrder = 0;
  AdvApp2Var_ApproxF2var::mma2jmx_(&aDegree, &anOrder, aMaxima.data());
  EXPECT_DOUBLE_EQ(aMaxima[0], .9682458365518542212948163499456);
  EXPECT_DOUBLE_EQ(aMaxima[56], 3.72184531357268220291630708234186);

  aDegree = 58;
  anOrder = 1;
  AdvApp2Var_ApproxF2var::mma2jmx_(&aDegree, &anOrder, aMaxima.data());
  EXPECT_DOUBLE_EQ(aMaxima[0], 1.1092649593311780079813740546678);
  EXPECT_DOUBLE_EQ(aMaxima[54], 3.05886060707437081434964933864149);

  aDegree = 58;
  anOrder = 2;
  AdvApp2Var_ApproxF2var::mma2jmx_(&aDegree, &anOrder, aMaxima.data());
  EXPECT_DOUBLE_EQ(aMaxima[0], 1.21091229812484768570102219548814);
  EXPECT_DOUBLE_EQ(aMaxima[52], 2.71238965987606292679677228666411);
}

TEST(AdvApp2Var_SysBaseTest, MemoryFillHandlesOverlapAndNonPositiveSizes)
{
  std::array<int, 4> aValues = {1, 2, 3, 4};
  int                aSize   = 3 * static_cast<int>(sizeof(int));
  AdvApp2Var_SysBase::mcrfill_(&aSize, aValues.data(), aValues.data() + 1);
  EXPECT_EQ(aValues, (std::array<int, 4>{1, 1, 2, 3}));

  const std::array<int, 4> anExpected = aValues;
  aSize                                = -1;
  AdvApp2Var_SysBase::mcrfill_(&aSize, aValues.data(), aValues.data() + 1);
  EXPECT_EQ(aValues, anExpected);

  aSize = 0;
  EXPECT_EQ(AdvApp2Var_SysBase::mcrfill_(&aSize, nullptr, nullptr), 0);
}

TEST(AdvApp2Var_MathBaseTest, HermiteBasisEvaluation_MatchesEndpointDerivatives)
{
  for (int anEndpoint = 0; anEndpoint < 2; ++anEndpoint)
  {
    double                 aParameter        = anEndpoint == 0 ? -1.0 : 1.0;
    int                    anOrder           = 2;
    int                    aCoefficientCount = 6;
    int                    aDerivativeCount  = 2;
    int                    anError           = 0;
    std::array<double, 18> aValues{};

    AdvApp2Var_MathBase::mmpobas_(&aParameter,
                                  &anOrder,
                                  &aCoefficientCount,
                                  &aDerivativeCount,
                                  aValues.data(),
                                  &anError);

    ASSERT_EQ(anError, 0);
    for (int aDerivative = 0; aDerivative <= aDerivativeCount; ++aDerivative)
    {
      for (int aBasis = 0; aBasis < aCoefficientCount; ++aBasis)
      {
        const int anExpectedBasis = anEndpoint * 3 + aDerivative;
        EXPECT_NEAR(aValues[aBasis + aDerivative * aCoefficientCount],
                    aBasis == anExpectedBasis ? 1.0 : 0.0,
                    1.0e-12);
      }
    }
  }
}
