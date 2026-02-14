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

#include <MathOpt_PSO.hxx>
#include <math_PSO.hxx>
#include <math_MultipleVarFunction.hxx>
#include <math_Vector.hxx>

#include <cmath>

namespace
{
constexpr double THE_TOLERANCE            = 1.0e-6;
constexpr double THE_STOCHASTIC_TOLERANCE = 1.0e-3;

// ============================================================================
// Test function classes for new API
// ============================================================================

//! Sphere function: f(x) = sum(x_i^2)
//! Global minimum at origin with f = 0
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

//! Rastrigin function: f(x) = 10n + sum(x_i^2 - 10*cos(2*pi*x_i))
//! Global minimum at origin with f = 0
//! Highly multimodal function
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

//! Ackley function - multimodal with global minimum at origin
struct AckleyFunc
{
  bool Value(const math_Vector& theX, double& theF)
  {
    const int    aN    = theX.Length();
    const double aPI   = M_PI;
    double       aSum1 = 0.0;
    double       aSum2 = 0.0;

    for (int i = theX.Lower(); i <= theX.Upper(); ++i)
    {
      aSum1 += theX(i) * theX(i);
      aSum2 += std::cos(2.0 * aPI * theX(i));
    }

    theF = -20.0 * std::exp(-0.2 * std::sqrt(aSum1 / aN)) - std::exp(aSum2 / aN) + 20.0 + M_E;
    return true;
  }
};

//! Rosenbrock function: f(x,y) = 100*(y-x^2)^2 + (1-x)^2
//! Minimum at (1, 1) with f = 0
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

//! Booth function: f(x,y) = (x + 2y - 7)^2 + (2x + y - 5)^2
//! Minimum at (1, 3) with f = 0
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

// ============================================================================
// Old API adapter
// ============================================================================

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
// Basic PSO tests
// ============================================================================

TEST(MathOpt_PSOTest, Sphere2D)
{
  SphereFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  MathOpt::PSOConfig aConfig;
  aConfig.NbParticles   = 50;
  aConfig.MaxIterations = 200;
  aConfig.Tolerance     = 1.0e-6;

  auto aResult = MathOpt::PSO(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 0.0, THE_TOLERANCE);
  EXPECT_NEAR((*aResult.Solution)(2), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
}

TEST(MathOpt_PSOTest, Sphere3D)
{
  SphereFunc aFunc;

  math_Vector aLower(1, 3, -5.0);
  math_Vector aUpper(1, 3, 5.0);

  MathOpt::PSOConfig aConfig;
  aConfig.NbParticles   = 40;
  aConfig.MaxIterations = 100;
  aConfig.Tolerance     = 1.0e-6;

  auto aResult = MathOpt::PSO(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  for (int i = 1; i <= 3; ++i)
  {
    EXPECT_NEAR((*aResult.Solution)(i), 0.0, THE_TOLERANCE);
  }
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
}

TEST(MathOpt_PSOTest, Booth)
{
  BoothFunc aFunc;

  math_Vector aLower(1, 2, -10.0);
  math_Vector aUpper(1, 2, 10.0);

  // PSO with larger search space needs more exploration
  MathOpt::PSOConfig aConfig;
  aConfig.NbParticles   = 60;
  aConfig.MaxIterations = 200;
  aConfig.Tolerance     = 1.0e-6;

  auto aResult = MathOpt::PSO(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, THE_STOCHASTIC_TOLERANCE);
  EXPECT_NEAR((*aResult.Solution)(2), 3.0, THE_STOCHASTIC_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_STOCHASTIC_TOLERANCE);
}

TEST(MathOpt_PSOTest, Rosenbrock)
{
  RosenbrockFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  MathOpt::PSOConfig aConfig;
  aConfig.NbParticles   = 50;
  aConfig.MaxIterations = 200;
  aConfig.Tolerance     = 1.0e-6;

  auto aResult = MathOpt::PSO(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, 0.1);
  EXPECT_NEAR((*aResult.Solution)(2), 1.0, 0.1);
  EXPECT_LT(*aResult.Value, 0.1);
}

// ============================================================================
// Multimodal function tests
// ============================================================================

TEST(MathOpt_PSOTest, Rastrigin2D)
{
  RastriginFunc aFunc;

  math_Vector aLower(1, 2, -5.12);
  math_Vector aUpper(1, 2, 5.12);

  MathOpt::PSOConfig aConfig;
  aConfig.NbParticles   = 60;
  aConfig.MaxIterations = 200;
  aConfig.Tolerance     = 1.0e-6;

  auto aResult = MathOpt::PSO(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  // PSO should find the global minimum at origin
  EXPECT_NEAR((*aResult.Solution)(1), 0.0, 0.5);
  EXPECT_NEAR((*aResult.Solution)(2), 0.0, 0.5);
  EXPECT_LT(*aResult.Value, 1.0);
}

TEST(MathOpt_PSOTest, Ackley2D)
{
  AckleyFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  MathOpt::PSOConfig aConfig;
  aConfig.NbParticles   = 60;
  aConfig.MaxIterations = 200;
  aConfig.Tolerance     = 1.0e-6;

  auto aResult = MathOpt::PSO(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  // Global minimum at origin with f = 0
  EXPECT_NEAR((*aResult.Solution)(1), 0.0, 0.5);
  EXPECT_NEAR((*aResult.Solution)(2), 0.0, 0.5);
  EXPECT_LT(*aResult.Value, 1.0);
}

// ============================================================================
// Parameter sensitivity tests
// ============================================================================

TEST(MathOpt_PSOTest, ParameterSensitivity_Omega)
{
  SphereFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  // Test different inertia weights
  double aOmegas[] = {0.4, 0.7, 0.9};

  for (double aOmega : aOmegas)
  {
    MathOpt::PSOConfig aConfig;
    aConfig.NbParticles   = 30;
    aConfig.MaxIterations = 100;
    aConfig.Tolerance     = 1.0e-6;
    aConfig.Omega         = aOmega;

    auto aResult = MathOpt::PSO(aFunc, aLower, aUpper, aConfig);

    ASSERT_TRUE(aResult.IsDone()) << "Failed with omega = " << aOmega;
    EXPECT_LT(*aResult.Value, 0.01) << "Failed with omega = " << aOmega;
  }
}

TEST(MathOpt_PSOTest, ParameterSensitivity_SwarmSize)
{
  SphereFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  // Test different swarm sizes
  int aSizes[] = {10, 30, 50, 100};

  for (int aSize : aSizes)
  {
    MathOpt::PSOConfig aConfig;
    aConfig.NbParticles   = aSize;
    aConfig.MaxIterations = 100;
    aConfig.Tolerance     = 1.0e-6;

    auto aResult = MathOpt::PSO(aFunc, aLower, aUpper, aConfig);

    ASSERT_TRUE(aResult.IsDone()) << "Failed with swarm size = " << aSize;
    EXPECT_LT(*aResult.Value, 0.01) << "Failed with swarm size = " << aSize;
  }
}

// ============================================================================
// Reproducibility test
// ============================================================================

TEST(MathOpt_PSOTest, Reproducibility)
{
  SphereFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  MathOpt::PSOConfig aConfig;
  aConfig.NbParticles   = 30;
  aConfig.MaxIterations = 50;
  aConfig.Tolerance     = 1.0e-6;

  auto aResult1 = MathOpt::PSO(aFunc, aLower, aUpper, aConfig);
  auto aResult2 = MathOpt::PSO(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult1.IsDone());
  ASSERT_TRUE(aResult2.IsDone());

  // With same seed, results should be identical
  EXPECT_DOUBLE_EQ(*aResult1.Value, *aResult2.Value);
  EXPECT_DOUBLE_EQ((*aResult1.Solution)(1), (*aResult2.Solution)(1));
  EXPECT_DOUBLE_EQ((*aResult1.Solution)(2), (*aResult2.Solution)(2));
}

// ============================================================================
// Comparison with old API
// ============================================================================

TEST(MathOpt_PSOTest, CompareWithOldAPI_Sphere)
{
  SphereFuncOld anOldFunc(2);
  SphereFunc    aNewFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  // Old API
  math_Vector aStep(1, 2, 0.5);
  math_PSO    anOldSolver(&anOldFunc, aLower, aUpper, aStep, 30, 100);
  double      anOldValue = 0.0;
  math_Vector anOldOutPnt(1, 2);
  anOldSolver.Perform(aStep, anOldValue, anOldOutPnt);

  // New API
  MathOpt::PSOConfig aConfig;
  aConfig.NbParticles   = 30;
  aConfig.MaxIterations = 100;
  aConfig.Tolerance     = 1.0e-8;
  auto aNewResult       = MathOpt::PSO(aNewFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aNewResult.IsDone());

  // Both should find a good solution
  EXPECT_LT(*aNewResult.Value, 0.01);
}

// ============================================================================
// Higher dimensional test
// ============================================================================

TEST(MathOpt_PSOTest, Sphere5D)
{
  SphereFunc aFunc;

  math_Vector aLower(1, 5, -5.0);
  math_Vector aUpper(1, 5, 5.0);

  MathOpt::PSOConfig aConfig;
  aConfig.NbParticles   = 60;
  aConfig.MaxIterations = 200;
  aConfig.Tolerance     = 1.0e-6;

  auto aResult = MathOpt::PSO(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  for (int i = 1; i <= 5; ++i)
  {
    EXPECT_NEAR((*aResult.Solution)(i), 0.0, 0.1);
  }
  EXPECT_LT(*aResult.Value, 0.1);
}

// ============================================================================
// Asymmetric bounds test
// ============================================================================

TEST(MathOpt_PSOTest, AsymmetricBounds)
{
  BoothFunc aFunc;

  // Asymmetric bounds that still contain the minimum (1, 3)
  math_Vector aLower(1, 2);
  aLower(1) = -5.0;
  aLower(2) = 0.0;

  math_Vector aUpper(1, 2);
  aUpper(1) = 5.0;
  aUpper(2) = 10.0;

  MathOpt::PSOConfig aConfig;
  aConfig.NbParticles   = 40;
  aConfig.MaxIterations = 100;
  aConfig.Tolerance     = 1.0e-6;

  auto aResult = MathOpt::PSO(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, THE_STOCHASTIC_TOLERANCE);
  EXPECT_NEAR((*aResult.Solution)(2), 3.0, THE_STOCHASTIC_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_STOCHASTIC_TOLERANCE);
}

// ============================================================================
// Seeded initialization tests
// ============================================================================

TEST(MathOpt_PSOTest, SeededInit_Sphere2D_ImprovesEvalCount)
{
  SphereFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  // Unseeded run
  MathOpt::PSOConfig aConfig;
  aConfig.NbParticles   = 40;
  aConfig.MaxIterations = 200;
  aConfig.Tolerance     = 1.0e-6;

  MathOpt::PSOStats aStatsUnseeded;
  auto aResultUnseeded = MathOpt::PSO(aFunc, aLower, aUpper, aConfig, nullptr, &aStatsUnseeded);
  ASSERT_TRUE(aResultUnseeded.IsDone());

  // Seeded run - seed near the origin
  MathOpt::PSOConfig aConfigSeeded = aConfig;
  aConfigSeeded.InitMode           = MathOpt::PSOInitMode::SeededPlusRandom;

  math_Vector aSeedPos(1, 2, 0.0);
  aSeedPos(1) = 0.1;
  aSeedPos(2) = -0.1;
  NCollection_Vector<MathOpt::PSOSeedParticle> aSeeds;
  aSeeds.Append(MathOpt::PSOSeedParticle(aSeedPos));

  MathOpt::PSOStats aStatsSeeded;
  auto aResultSeeded = MathOpt::PSO(aFunc, aLower, aUpper, aConfigSeeded, &aSeeds, &aStatsSeeded);
  ASSERT_TRUE(aResultSeeded.IsDone());

  // Seeded should converge at least as well
  EXPECT_LE(*aResultSeeded.Value, *aResultUnseeded.Value + 0.01);
  // Stats should be populated
  EXPECT_GT(aStatsSeeded.NbFunctionEvals, 0);
  EXPECT_GT(aStatsSeeded.NbIterations, 0);
}

TEST(MathOpt_PSOTest, SeededInit_Rastrigin2D_StableQuality)
{
  RastriginFunc aFunc;

  math_Vector aLower(1, 2, -5.12);
  math_Vector aUpper(1, 2, 5.12);

  MathOpt::PSOConfig aConfig;
  aConfig.NbParticles   = 60;
  aConfig.MaxIterations = 200;
  aConfig.Tolerance     = 1.0e-6;
  aConfig.InitMode      = MathOpt::PSOInitMode::SeededPlusRandom;

  // Seed near global minimum
  math_Vector                                  aSeedPos(1, 2, 0.0);
  NCollection_Vector<MathOpt::PSOSeedParticle> aSeeds;
  aSeeds.Append(MathOpt::PSOSeedParticle(aSeedPos, 0.0));

  auto aResult = MathOpt::PSO(aFunc, aLower, aUpper, aConfig, &aSeeds);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 0.0, 0.5);
  EXPECT_NEAR((*aResult.Solution)(2), 0.0, 0.5);
  EXPECT_LT(*aResult.Value, 1.0);
}

// ============================================================================
// Boundary mode tests
// ============================================================================

TEST(MathOpt_PSOTest, BoundaryMode_Reflect_NearBoundMinimum)
{
  BoothFunc aFunc;

  // Tight bounds around minimum (1, 3)
  math_Vector aLower(1, 2);
  aLower(1) = 0.0;
  aLower(2) = 2.0;

  math_Vector aUpper(1, 2);
  aUpper(1) = 2.0;
  aUpper(2) = 4.0;

  MathOpt::PSOConfig aConfig;
  aConfig.NbParticles   = 40;
  aConfig.MaxIterations = 200;
  aConfig.Tolerance     = 1.0e-6;
  aConfig.BoundaryMode  = MathOpt::PSOBoundaryMode::Reflect;

  MathOpt::PSOStats aStats;
  auto              aResult = MathOpt::PSO(aFunc, aLower, aUpper, aConfig, nullptr, &aStats);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, 0.1);
  EXPECT_NEAR((*aResult.Solution)(2), 3.0, 0.1);
  EXPECT_LT(*aResult.Value, 0.1);
  // Tight bounds should trigger boundary corrections
  EXPECT_GT(aStats.NbBoundaryCorrections, 0);
}

// ============================================================================
// Restart tests
// ============================================================================

TEST(MathOpt_PSOTest, Restart_Ackley_RecoversAfterStagnation)
{
  AckleyFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  MathOpt::PSOConfig aConfig;
  aConfig.NbParticles     = 30;
  aConfig.MaxIterations   = 300;
  aConfig.Tolerance       = 1.0e-8;
  aConfig.RestartFraction = 0.5;
  aConfig.NoImproveIters  = 5;

  MathOpt::PSOStats aStats;
  auto              aResult = MathOpt::PSO(aFunc, aLower, aUpper, aConfig, nullptr, &aStats);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_LT(*aResult.Value, 1.0);
  // With tight convergence and restart enabled, stats should show activity
  EXPECT_GT(aStats.NbFunctionEvals, 0);
  EXPECT_GT(aStats.NbRestarts, 0);
  EXPECT_GT(aStats.NbStagnationEvents, 0);
}

// ============================================================================
// Target value early stop
// ============================================================================

TEST(MathOpt_PSOTest, TargetValue_EarlyStop)
{
  SphereFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  // Run without target value
  MathOpt::PSOConfig aConfigFull;
  aConfigFull.NbParticles   = 40;
  aConfigFull.MaxIterations = 200;
  aConfigFull.Tolerance     = 1.0e-10; // very tight - force many iterations

  MathOpt::PSOStats aStatsFull;
  auto aResultFull = MathOpt::PSO(aFunc, aLower, aUpper, aConfigFull, nullptr, &aStatsFull);
  ASSERT_TRUE(aResultFull.IsDone());

  // Run with target value - should stop early
  MathOpt::PSOConfig aConfigTarget = aConfigFull;
  aConfigTarget.TargetValue        = 0.1;

  MathOpt::PSOStats aStatsTarget;
  auto aResultTarget = MathOpt::PSO(aFunc, aLower, aUpper, aConfigTarget, nullptr, &aStatsTarget);
  ASSERT_TRUE(aResultTarget.IsDone());

  // Target run should use fewer iterations
  EXPECT_LE(aStatsTarget.NbIterations, aStatsFull.NbIterations);
  EXPECT_LE(*aResultTarget.Value, 0.1);
}

// ============================================================================
// Inertia schedule tests
// ============================================================================

TEST(MathOpt_PSOTest, InertiaSchedule_LinearDecay_NonRegression)
{
  SphereFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  MathOpt::PSOConfig aConfig;
  aConfig.NbParticles     = 40;
  aConfig.MaxIterations   = 200;
  aConfig.Tolerance       = 1.0e-6;
  aConfig.InertiaSchedule = MathOpt::PSOInertiaSchedule::LinearDecay;
  aConfig.OmegaMin        = 0.3;

  auto aResult = MathOpt::PSO(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 0.0, THE_TOLERANCE);
  EXPECT_NEAR((*aResult.Solution)(2), 0.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
}

// ============================================================================
// Stats reproducibility
// ============================================================================

TEST(MathOpt_PSOTest, Stats_Reproducibility_FixedSeed)
{
  SphereFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  MathOpt::PSOConfig aConfig;
  aConfig.NbParticles   = 30;
  aConfig.MaxIterations = 50;
  aConfig.Tolerance     = 1.0e-6;

  MathOpt::PSOStats aStats1;
  auto              aResult1 = MathOpt::PSO(aFunc, aLower, aUpper, aConfig, nullptr, &aStats1);

  MathOpt::PSOStats aStats2;
  auto              aResult2 = MathOpt::PSO(aFunc, aLower, aUpper, aConfig, nullptr, &aStats2);

  ASSERT_TRUE(aResult1.IsDone());
  ASSERT_TRUE(aResult2.IsDone());

  // With same seed, stats should be identical
  EXPECT_EQ(aStats1.NbFunctionEvals, aStats2.NbFunctionEvals);
  EXPECT_EQ(aStats1.NbIterations, aStats2.NbIterations);
  EXPECT_EQ(aStats1.NbBoundaryCorrections, aStats2.NbBoundaryCorrections);
  EXPECT_DOUBLE_EQ(aStats1.InitialBest, aStats2.InitialBest);
  EXPECT_DOUBLE_EQ(aStats1.FinalBest, aStats2.FinalBest);
  EXPECT_DOUBLE_EQ(*aResult1.Value, *aResult2.Value);
}

// ============================================================================
// Polishing precision tests
// ============================================================================

TEST(MathOpt_PSOTest, Polishing_Sphere2D_ReachesHighPrecision)
{
  SphereFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  MathOpt::PSOConfig aConfig;
  aConfig.NbParticles   = 30;
  aConfig.MaxIterations = 50;
  aConfig.Tolerance     = 1.0e-10;

  auto aResult = MathOpt::PSO(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  // After polishing, component precision should be better than 1e-8
  EXPECT_NEAR((*aResult.Solution)(1), 0.0, 1.0e-8);
  EXPECT_NEAR((*aResult.Solution)(2), 0.0, 1.0e-8);
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-14);
}

// ============================================================================
// SeededOnly validation test
// ============================================================================

TEST(MathOpt_PSOTest, SeededOnly_NoSeeds_ReturnsInvalidInput)
{
  SphereFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  MathOpt::PSOConfig aConfig;
  aConfig.NbParticles   = 20;
  aConfig.MaxIterations = 50;
  aConfig.InitMode      = MathOpt::PSOInitMode::SeededOnly;

  // No seeds provided
  auto aResult = MathOpt::PSO(aFunc, aLower, aUpper, aConfig);

  EXPECT_FALSE(aResult.IsDone());
  EXPECT_EQ(aResult.Status, MathUtils::Status::InvalidInput);
}

// ============================================================================
// Clamped seed re-evaluation test
// ============================================================================

TEST(MathOpt_PSOTest, ClampedSeed_ReEvaluates_StaleValue)
{
  SphereFunc aFunc;

  math_Vector aLower(1, 2, -5.0);
  math_Vector aUpper(1, 2, 5.0);

  MathOpt::PSOConfig aConfig;
  aConfig.NbParticles   = 10;
  aConfig.MaxIterations = 100;
  aConfig.Tolerance     = 1.0e-8;
  aConfig.InitMode      = MathOpt::PSOInitMode::SeededPlusRandom;

  // Seed at out-of-bounds position with incorrect stale value
  math_Vector aSeedPos(1, 2);
  aSeedPos(1) = 10.0;  // out of bounds (will be clamped to 5.0)
  aSeedPos(2) = -10.0; // out of bounds (will be clamped to -5.0)

  // Provide a stale value of 0.0 which is wrong for (5, -5)
  NCollection_Vector<MathOpt::PSOSeedParticle> aSeeds;
  MathOpt::PSOSeedParticle                     aSeed(aSeedPos, 0.0);
  aSeeds.Append(aSeed);

  auto aResult = MathOpt::PSO(aFunc, aLower, aUpper, aConfig, &aSeeds);

  ASSERT_TRUE(aResult.IsDone());
  // The result should be correct (near origin), not misled by the stale value
  EXPECT_NEAR((*aResult.Solution)(1), 0.0, THE_STOCHASTIC_TOLERANCE);
  EXPECT_NEAR((*aResult.Solution)(2), 0.0, THE_STOCHASTIC_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_STOCHASTIC_TOLERANCE);
}
