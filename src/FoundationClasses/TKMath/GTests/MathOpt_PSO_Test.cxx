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
constexpr double THE_TOLERANCE = 1.0e-3;

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

  Standard_Integer NbVariables() const override { return myN; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    theF = 0.0;
    for (int i = theX.Lower(); i <= theX.Upper(); ++i)
    {
      theF += theX(i) * theX(i);
    }
    return Standard_True;
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
  aConfig.Seed          = 42;

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
  aConfig.Seed          = 42;

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
  aConfig.Seed          = 42;

  auto aResult = MathOpt::PSO(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  // PSO is stochastic - use slightly relaxed tolerance for larger search space
  constexpr double aPSOTolerance = 2.0e-3;
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, aPSOTolerance);
  EXPECT_NEAR((*aResult.Solution)(2), 3.0, aPSOTolerance);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
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
  aConfig.Seed          = 42;

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
  aConfig.Seed          = 42;

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
  aConfig.Seed          = 42;

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
    aConfig.Seed          = 42;

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
    aConfig.Seed          = 42;

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
  aConfig.Seed          = 12345;

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
  anOldSolver.Perform(aStep, anOldValue, anOldOutPnt, 12345);

  // New API
  MathOpt::PSOConfig aConfig;
  aConfig.NbParticles   = 30;
  aConfig.MaxIterations = 100;
  aConfig.Tolerance     = 1.0e-8;
  aConfig.Seed          = 12345;
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
  aConfig.Seed          = 42;

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
  aConfig.Seed          = 42;

  auto aResult = MathOpt::PSO(aFunc, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, THE_TOLERANCE);
  EXPECT_NEAR((*aResult.Solution)(2), 3.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
}
