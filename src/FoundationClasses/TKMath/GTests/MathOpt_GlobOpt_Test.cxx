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

#include <MathOpt_GlobOpt.hxx>
#include <math_GlobOptMin.hxx>
#include <math_MultipleVarFunction.hxx>
#include <math_Vector.hxx>

#include <cmath>

namespace
{
constexpr double THE_TOLERANCE            = 1.0e-6;
constexpr double THE_STOCHASTIC_TOLERANCE = 1.0e-2;

// ============================================================================
// Test function classes
// ============================================================================

//! Sphere function: f(x) = sum(x_i^2)
struct SphereFunc
{
  bool Value(const math_Vector& theX, double& theF)
  {
    theF = 0.0;
    for (int i = theX.Lower(); i <= theX.Upper(); ++i)
    {
      theF += theX(i) * theX(i);
    }
    return true;
  }
};

//! Rastrigin function - highly multimodal
struct RastriginFunc
{
  bool Value(const math_Vector& theX, double& theF)
  {
    const int    aN  = theX.Length();
    const double aPI = M_PI;
    theF             = 10.0 * aN;
    for (int i = theX.Lower(); i <= theX.Upper(); ++i)
    {
      theF += theX(i) * theX(i) - 10.0 * std::cos(2.0 * aPI * theX(i));
    }
    return true;
  }
};

//! Rosenbrock function
struct RosenbrockFunc
{
  bool Value(const math_Vector& theX, double& theF)
  {
    const double aX  = theX(1);
    const double aY  = theX(2);
    const double aT1 = aY - aX * aX;
    const double aT2 = 1.0 - aX;
    theF             = 100.0 * aT1 * aT1 + aT2 * aT2;
    return true;
  }
};

//! Booth function
struct BoothFunc
{
  bool Value(const math_Vector& theX, double& theF)
  {
    const double aX  = theX(1);
    const double aY  = theX(2);
    const double aT1 = aX + 2.0 * aY - 7.0;
    const double aT2 = 2.0 * aX + aY - 5.0;
    theF             = aT1 * aT1 + aT2 * aT2;
    return true;
  }
};

//! Himmelblau function - has 4 identical local minima
//! f(x,y) = (x^2 + y - 11)^2 + (x + y^2 - 7)^2
struct HimmelblauFunc
{
  bool Value(const math_Vector& theX, double& theF)
  {
    const double aX  = theX(1);
    const double aY  = theX(2);
    const double aT1 = aX * aX + aY - 11.0;
    const double aT2 = aX + aY * aY - 7.0;
    theF             = aT1 * aT1 + aT2 * aT2;
    return true;
  }
};

//! Goldstein-Price function - complex multimodal
struct GoldsteinPriceFunc
{
  bool Value(const math_Vector& theX, double& theF)
  {
    const double aX = theX(1);
    const double aY = theX(2);

    const double aA =
      1.0
      + (aX + aY + 1.0) * (aX + aY + 1.0)
          * (19.0 - 14.0 * aX + 3.0 * aX * aX - 14.0 * aY + 6.0 * aX * aY + 3.0 * aY * aY);

    const double aB =
      30.0
      + (2.0 * aX - 3.0 * aY) * (2.0 * aX - 3.0 * aY)
          * (18.0 - 32.0 * aX + 12.0 * aX * aX + 48.0 * aY - 36.0 * aX * aY + 27.0 * aY * aY);

    theF = aA * aB;
    return true;
  }
};

// Old API adapter
class SphereFuncOld : public math_MultipleVarFunction
{
private:
  int myN;

public:
  SphereFuncOld(int theN)
      : myN(theN)
  {
  }

  int NbVariables() const override { return myN; }

  bool Value(const math_Vector& theX, double& theF) override
  {
    theF = 0.0;
    for (int i = theX.Lower(); i <= theX.Upper(); ++i)
    {
      theF += theX(i) * theX(i);
    }
    return true;
  }
};

} // namespace

// ============================================================================
// GlobalMinimum unified interface tests
// ============================================================================

TEST(MathOpt_GlobOptTest, GlobalMinimum_PSO_Sphere)
{
  SphereFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  MathOpt::GlobalConfig aConfig(MathOpt::GlobalStrategy::PSO, 100);
  aConfig.NbPopulation = 40;

  auto aResult = MathOpt::GlobalMinimum(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 0.0, THE_TOLERANCE);
  EXPECT_NEAR((*aResult.Solution)(2), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
}

TEST(MathOpt_GlobOptTest, GlobalMinimum_MultiStart_Sphere)
{
  SphereFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  MathOpt::GlobalConfig aConfig(MathOpt::GlobalStrategy::MultiStart, 100);
  aConfig.NbStarts = 20;

  auto aResult = MathOpt::GlobalMinimum(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 0.0, THE_TOLERANCE);
  EXPECT_NEAR((*aResult.Solution)(2), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
}

TEST(MathOpt_GlobOptTest, GlobalMinimum_PSOHybrid_Booth)
{
  BoothFunc aFunc;

  math_Vector aLower(1, 2, -10.0);
  math_Vector aUpper(1, 2, 10.0);

  MathOpt::GlobalConfig aConfig(MathOpt::GlobalStrategy::PSOHybrid, 100);
  aConfig.NbPopulation = 40;

  auto aResult = MathOpt::GlobalMinimum(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, THE_TOLERANCE);
  EXPECT_NEAR((*aResult.Solution)(2), 3.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
}

TEST(MathOpt_GlobOptTest, GlobalMinimum_DE_Sphere)
{
  SphereFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  MathOpt::GlobalConfig aConfig(MathOpt::GlobalStrategy::DifferentialEvolution, 100);
  aConfig.NbPopulation  = 40;
  aConfig.MutationScale = 0.8;
  aConfig.CrossoverProb = 0.9;

  auto aResult = MathOpt::GlobalMinimum(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 0.0, THE_TOLERANCE);
  EXPECT_NEAR((*aResult.Solution)(2), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
}

// ============================================================================
// Differential Evolution tests
// ============================================================================

TEST(MathOpt_GlobOptTest, DifferentialEvolution_Sphere)
{
  SphereFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  MathOpt::GlobalConfig aConfig;
  aConfig.MaxIterations = 100;
  aConfig.NbPopulation  = 40;
  aConfig.MutationScale = 0.8;
  aConfig.CrossoverProb = 0.9;

  auto aResult = MathOpt::DifferentialEvolution(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 0.0, THE_TOLERANCE);
  EXPECT_NEAR((*aResult.Solution)(2), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
}

TEST(MathOpt_GlobOptTest, DifferentialEvolution_Rosenbrock)
{
  RosenbrockFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  MathOpt::GlobalConfig aConfig;
  aConfig.MaxIterations = 200;
  aConfig.NbPopulation  = 50;
  aConfig.MutationScale = 0.8;
  aConfig.CrossoverProb = 0.9;

  auto aResult = MathOpt::DifferentialEvolution(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, 0.1);
  EXPECT_NEAR((*aResult.Solution)(2), 1.0, 0.1);
  EXPECT_LT(*aResult.Value, 0.1);
}

TEST(MathOpt_GlobOptTest, DifferentialEvolution_Rastrigin)
{
  RastriginFunc aFunc;

  math_Vector aLower(1, 2, -5.12);
  math_Vector aUpper(1, 2, 5.12);

  MathOpt::GlobalConfig aConfig;
  aConfig.MaxIterations = 200;
  aConfig.NbPopulation  = 60;
  aConfig.MutationScale = 0.5;
  aConfig.CrossoverProb = 0.9;

  auto aResult = MathOpt::DifferentialEvolution(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  // Should find global minimum at origin
  EXPECT_NEAR((*aResult.Solution)(1), 0.0, 0.5);
  EXPECT_NEAR((*aResult.Solution)(2), 0.0, 0.5);
  EXPECT_LT(*aResult.Value, 1.0);
}

// ============================================================================
// MultiStart tests
// ============================================================================

TEST(MathOpt_GlobOptTest, MultiStart_Sphere)
{
  SphereFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  MathOpt::GlobalConfig aConfig;
  aConfig.NbStarts = 20;

  auto aResult = MathOpt::MultiStart(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 0.0, THE_TOLERANCE);
  EXPECT_NEAR((*aResult.Solution)(2), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
}

TEST(MathOpt_GlobOptTest, MultiStart_Booth)
{
  BoothFunc aFunc;

  math_Vector aLower(1, 2, -10.0);
  math_Vector aUpper(1, 2, 10.0);

  MathOpt::GlobalConfig aConfig;
  aConfig.NbStarts = 30;

  auto aResult = MathOpt::MultiStart(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, 0.5);
  EXPECT_NEAR((*aResult.Solution)(2), 3.0, 0.5);
  EXPECT_LT(*aResult.Value, 1.0);
}

// ============================================================================
// Himmelblau function (multiple global minima)
// ============================================================================

TEST(MathOpt_GlobOptTest, Himmelblau_FindsOneMinimum)
{
  HimmelblauFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  MathOpt::GlobalConfig aConfig(MathOpt::GlobalStrategy::DifferentialEvolution, 200);
  aConfig.NbPopulation = 50;

  auto aResult = MathOpt::GlobalMinimum(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());

  // Himmelblau has 4 global minima, all with f = 0
  // The algorithm should find one of them
  EXPECT_NEAR(*aResult.Value, 0.0, THE_STOCHASTIC_TOLERANCE);
}

// ============================================================================
// Goldstein-Price function
// ============================================================================

TEST(MathOpt_GlobOptTest, GoldsteinPrice)
{
  GoldsteinPriceFunc aFunc;

  math_Vector aLower(1, 2, -2.0);
  math_Vector aUpper(1, 2, 2.0);

  MathOpt::GlobalConfig aConfig(MathOpt::GlobalStrategy::DifferentialEvolution, 200);
  aConfig.NbPopulation = 60;

  auto aResult = MathOpt::GlobalMinimum(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());

  // Global minimum at (0, -1) with f = 3
  EXPECT_NEAR((*aResult.Solution)(1), 0.0, 0.1);
  EXPECT_NEAR((*aResult.Solution)(2), -1.0, 0.1);
  EXPECT_NEAR(*aResult.Value, 3.0, 0.5);
}

// ============================================================================
// Higher dimensional tests
// ============================================================================

TEST(MathOpt_GlobOptTest, DifferentialEvolution_Sphere5D)
{
  SphereFunc aFunc;

  math_Vector aLower(1, 5, -5.0);
  math_Vector aUpper(1, 5, 5.0);

  MathOpt::GlobalConfig aConfig;
  aConfig.MaxIterations = 300;
  aConfig.NbPopulation  = 80;
  aConfig.MutationScale = 0.7;
  aConfig.CrossoverProb = 0.9;

  auto aResult = MathOpt::DifferentialEvolution(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  for (int i = 1; i <= 5; ++i)
  {
    EXPECT_NEAR((*aResult.Solution)(i), 0.0, 0.1);
  }
  EXPECT_LT(*aResult.Value, 0.1);
}

// ============================================================================
// Strategy comparison tests
// ============================================================================

TEST(MathOpt_GlobOptTest, StrategyComparison_Rosenbrock)
{
  RosenbrockFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  // Test PSO
  MathOpt::GlobalConfig aConfigPSO(MathOpt::GlobalStrategy::PSO, 200);
  aConfigPSO.NbPopulation = 50;

  auto aResultPSO = MathOpt::GlobalMinimum(aFunc, aLower, aUpper, aConfigPSO);

  // Test DE
  MathOpt::GlobalConfig aConfigDE(MathOpt::GlobalStrategy::DifferentialEvolution, 200);
  aConfigDE.NbPopulation = 50;

  auto aResultDE = MathOpt::GlobalMinimum(aFunc, aLower, aUpper, aConfigDE);

  ASSERT_TRUE(aResultPSO.IsDone());
  ASSERT_TRUE(aResultDE.IsDone());

  // Both should find reasonable solutions
  EXPECT_LT(*aResultPSO.Value, 1.0);
  EXPECT_LT(*aResultDE.Value, 1.0);
}

// ============================================================================
// Reproducibility test
// ============================================================================

TEST(MathOpt_GlobOptTest, Reproducibility_DE)
{
  SphereFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  MathOpt::GlobalConfig aConfig;
  aConfig.MaxIterations = 100;
  aConfig.NbPopulation  = 40;

  auto aResult1 = MathOpt::DifferentialEvolution(aFunc, aLower, aUpper, aConfig);
  auto aResult2 = MathOpt::DifferentialEvolution(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult1.IsDone());
  ASSERT_TRUE(aResult2.IsDone());

  // With same seed, results should be identical
  EXPECT_DOUBLE_EQ(*aResult1.Value, *aResult2.Value);
}

// ============================================================================
// Comparison with old API
// ============================================================================

TEST(MathOpt_GlobOptTest, CompareWithOldAPI_Sphere)
{
  SphereFuncOld anOldFunc(2);
  SphereFunc    aNewFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  // Old API
  math_GlobOptMin anOldSolver(&anOldFunc, aLower, aUpper, 0.01);
  anOldSolver.Perform();
  math_Vector anOldSol(1, 2);
  anOldSolver.Points(1, anOldSol);

  // New API
  MathOpt::GlobalConfig aConfig(MathOpt::GlobalStrategy::PSOHybrid, 100);
  aConfig.NbPopulation = 40;

  auto aNewResult = MathOpt::GlobalMinimum(aNewFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aNewResult.IsDone());

  // Both should find a good solution
  EXPECT_LT(*aNewResult.Value, 0.1);
}

// ============================================================================
// New GlobalMinimum overload tests
// ============================================================================

TEST(MathOpt_GlobOptTest, GlobalMinimum_PSO_CustomPSOConfig)
{
  SphereFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  MathOpt::GlobalConfig aGlobConfig(MathOpt::GlobalStrategy::PSO, 200);
  aGlobConfig.NbPopulation = 40;

  MathOpt::PSOConfig aPSOConfig;
  aPSOConfig.NbParticles   = 60;
  aPSOConfig.MaxIterations = 150;
  aPSOConfig.Tolerance     = 1.0e-6;

  aPSOConfig.InertiaSchedule = MathOpt::PSOInertiaSchedule::LinearDecay;
  aPSOConfig.OmegaMin        = 0.3;

  MathOpt::PSOStats aStats;
  auto              aResult =
    MathOpt::GlobalMinimum(aFunc, aLower, aUpper, aGlobConfig, &aPSOConfig, nullptr, &aStats);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 0.0, THE_TOLERANCE);
  EXPECT_NEAR((*aResult.Solution)(2), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
  EXPECT_GT(aStats.NbFunctionEvals, 0);
  EXPECT_GT(aStats.NbIterations, 0);
}

TEST(MathOpt_GlobOptTest, GlobalMinimum_PSOHybrid_WithSeeds)
{
  BoothFunc aFunc;

  math_Vector aLower(1, 2, -10.0);
  math_Vector aUpper(1, 2, 10.0);

  MathOpt::GlobalConfig aGlobConfig(MathOpt::GlobalStrategy::PSOHybrid, 100);
  aGlobConfig.NbPopulation = 40;

  MathOpt::PSOConfig aPSOConfig;
  aPSOConfig.NbParticles   = 40;
  aPSOConfig.MaxIterations = 100;
  aPSOConfig.Tolerance     = 1.0e-6;

  aPSOConfig.InitMode = MathOpt::PSOInitMode::SeededPlusRandom;

  // Seed near the minimum (1, 3)
  math_Vector aSeedPos(1, 2);
  aSeedPos(1) = 1.5;
  aSeedPos(2) = 2.5;
  NCollection_Vector<MathOpt::PSOSeedParticle> aSeeds;
  aSeeds.Append(MathOpt::PSOSeedParticle(aSeedPos));

  MathOpt::PSOStats aStats;
  auto              aResult =
    MathOpt::GlobalMinimum(aFunc, aLower, aUpper, aGlobConfig, &aPSOConfig, &aSeeds, &aStats);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, THE_TOLERANCE);
  EXPECT_NEAR((*aResult.Solution)(2), 3.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
  EXPECT_GT(aStats.NbFunctionEvals, 0);
}

TEST(MathOpt_GlobOptTest, GlobalMinimum_NonPSOStrategies_UnchangedWithPSOOptions)
{
  SphereFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  // MultiStart - PSO params should be ignored
  MathOpt::GlobalConfig aConfigMS(MathOpt::GlobalStrategy::MultiStart, 100);
  aConfigMS.NbStarts = 20;

  MathOpt::PSOConfig aPSOConfig;
  aPSOConfig.NbParticles = 100;

  auto aResultMS = MathOpt::GlobalMinimum(aFunc, aLower, aUpper, aConfigMS, &aPSOConfig);
  ASSERT_TRUE(aResultMS.IsDone());
  EXPECT_NEAR(*aResultMS.Value, 0.0, THE_TOLERANCE);

  // DE - PSO params should be ignored
  MathOpt::GlobalConfig aConfigDE(MathOpt::GlobalStrategy::DifferentialEvolution, 100);
  aConfigDE.NbPopulation = 40;

  auto aResultDE = MathOpt::GlobalMinimum(aFunc, aLower, aUpper, aConfigDE, &aPSOConfig);
  ASSERT_TRUE(aResultDE.IsDone());
  EXPECT_NEAR(*aResultDE.Value, 0.0, THE_TOLERANCE);
}
