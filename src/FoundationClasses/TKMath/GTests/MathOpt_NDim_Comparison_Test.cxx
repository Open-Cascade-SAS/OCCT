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

// New API
#include <MathOpt_Powell.hxx>
#include <MathOpt_BFGS.hxx>

// Old API
#include <math_Powell.hxx>
#include <math_BFGS.hxx>
#include <math_MultipleVarFunction.hxx>
#include <math_MultipleVarFunctionWithGradient.hxx>
#include <math_Matrix.hxx>

#include <cmath>

namespace
{
constexpr double THE_TOLERANCE = 1.0e-6;

// ============================================================================
// Adapter classes for old API (inherit from math_MultipleVarFunction)
// ============================================================================

//! Quadratic function: f(x,y) = (x-1)^2 + (y-2)^2
//! Minimum at (1, 2) with f = 0
class QuadraticFuncOld : public math_MultipleVarFunction
{
public:
  Standard_Integer NbVariables() const override { return 2; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    const double aDx = theX(1) - 1.0;
    const double aDy = theX(2) - 2.0;
    theF             = aDx * aDx + aDy * aDy;
    return Standard_True;
  }
};

//! Rosenbrock function: f(x,y) = 100*(y-x^2)^2 + (1-x)^2
//! Minimum at (1, 1) with f = 0
class RosenbrockFuncOld : public math_MultipleVarFunction
{
public:
  Standard_Integer NbVariables() const override { return 2; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    const double aX  = theX(1);
    const double aY  = theX(2);
    const double aT1 = aY - aX * aX;
    const double aT2 = 1.0 - aX;
    theF             = 100.0 * aT1 * aT1 + aT2 * aT2;
    return Standard_True;
  }
};

//! Booth function: f(x,y) = (x + 2y - 7)^2 + (2x + y - 5)^2
//! Minimum at (1, 3) with f = 0
class BoothFuncOld : public math_MultipleVarFunction
{
public:
  Standard_Integer NbVariables() const override { return 2; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    const double aX  = theX(1);
    const double aY  = theX(2);
    const double aT1 = aX + 2.0 * aY - 7.0;
    const double aT2 = 2.0 * aX + aY - 5.0;
    theF             = aT1 * aT1 + aT2 * aT2;
    return Standard_True;
  }
};

//! Sphere function in N dimensions: f(x) = sum(x_i^2)
//! Minimum at origin with f = 0
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

// ============================================================================
// Adapter classes for old BFGS API (inherit from math_MultipleVarFunctionWithGradient)
// ============================================================================

//! Quadratic function with gradient: f(x,y) = (x-1)^2 + (y-2)^2
class QuadraticFuncGradOld : public math_MultipleVarFunctionWithGradient
{
public:
  Standard_Integer NbVariables() const override { return 2; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    const double aDx = theX(1) - 1.0;
    const double aDy = theX(2) - 2.0;
    theF             = aDx * aDx + aDy * aDy;
    return Standard_True;
  }

  Standard_Boolean Gradient(const math_Vector& theX, math_Vector& theG) override
  {
    theG(1) = 2.0 * (theX(1) - 1.0);
    theG(2) = 2.0 * (theX(2) - 2.0);
    return Standard_True;
  }

  Standard_Boolean Values(const math_Vector& theX, Standard_Real& theF, math_Vector& theG) override
  {
    return Value(theX, theF) && Gradient(theX, theG);
  }
};

//! Rosenbrock function with gradient
class RosenbrockFuncGradOld : public math_MultipleVarFunctionWithGradient
{
public:
  Standard_Integer NbVariables() const override { return 2; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    const double aX  = theX(1);
    const double aY  = theX(2);
    const double aT1 = aY - aX * aX;
    const double aT2 = 1.0 - aX;
    theF             = 100.0 * aT1 * aT1 + aT2 * aT2;
    return Standard_True;
  }

  Standard_Boolean Gradient(const math_Vector& theX, math_Vector& theG) override
  {
    const double aX = theX(1);
    const double aY = theX(2);
    theG(1)         = -400.0 * aX * (aY - aX * aX) - 2.0 * (1.0 - aX);
    theG(2)         = 200.0 * (aY - aX * aX);
    return Standard_True;
  }

  Standard_Boolean Values(const math_Vector& theX, Standard_Real& theF, math_Vector& theG) override
  {
    return Value(theX, theF) && Gradient(theX, theG);
  }
};

//! Booth function with gradient
class BoothFuncGradOld : public math_MultipleVarFunctionWithGradient
{
public:
  Standard_Integer NbVariables() const override { return 2; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    const double aX  = theX(1);
    const double aY  = theX(2);
    const double aT1 = aX + 2.0 * aY - 7.0;
    const double aT2 = 2.0 * aX + aY - 5.0;
    theF             = aT1 * aT1 + aT2 * aT2;
    return Standard_True;
  }

  Standard_Boolean Gradient(const math_Vector& theX, math_Vector& theG) override
  {
    const double aX  = theX(1);
    const double aY  = theX(2);
    const double aT1 = aX + 2.0 * aY - 7.0;
    const double aT2 = 2.0 * aX + aY - 5.0;
    theG(1)          = 2.0 * aT1 + 4.0 * aT2;
    theG(2)          = 4.0 * aT1 + 2.0 * aT2;
    return Standard_True;
  }

  Standard_Boolean Values(const math_Vector& theX, Standard_Real& theF, math_Vector& theG) override
  {
    return Value(theX, theF) && Gradient(theX, theG);
  }
};

//! Sphere function with gradient
class SphereFuncGradOld : public math_MultipleVarFunctionWithGradient
{
private:
  int myN;

public:
  SphereFuncGradOld(int theN)
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

  Standard_Boolean Gradient(const math_Vector& theX, math_Vector& theG) override
  {
    for (int i = theX.Lower(); i <= theX.Upper(); ++i)
    {
      theG(i) = 2.0 * theX(i);
    }
    return Standard_True;
  }

  Standard_Boolean Values(const math_Vector& theX, Standard_Real& theF, math_Vector& theG) override
  {
    return Value(theX, theF) && Gradient(theX, theG);
  }
};

// ============================================================================
// Function classes for new API
// ============================================================================

struct QuadraticFuncNew
{
  bool Value(const math_Vector& theX, double& theF)
  {
    const double aDx = theX(1) - 1.0;
    const double aDy = theX(2) - 2.0;
    theF             = aDx * aDx + aDy * aDy;
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    theGrad(1) = 2.0 * (theX(1) - 1.0);
    theGrad(2) = 2.0 * (theX(2) - 2.0);
    return true;
  }
};

struct RosenbrockFuncNew
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

  bool Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    const double aX = theX(1);
    const double aY = theX(2);
    theGrad(1)      = -400.0 * aX * (aY - aX * aX) - 2.0 * (1.0 - aX);
    theGrad(2)      = 200.0 * (aY - aX * aX);
    return true;
  }
};

struct BoothFuncNew
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

  bool Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    const double aX  = theX(1);
    const double aY  = theX(2);
    const double aT1 = aX + 2.0 * aY - 7.0;
    const double aT2 = 2.0 * aX + aY - 5.0;
    theGrad(1)       = 2.0 * aT1 + 4.0 * aT2;
    theGrad(2)       = 4.0 * aT1 + 2.0 * aT2;
    return true;
  }
};

struct SphereFuncNew
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

  bool Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    for (int i = theX.Lower(); i <= theX.Upper(); ++i)
    {
      theGrad(i) = 2.0 * theX(i);
    }
    return true;
  }
};

//! Create identity matrix for Powell directions.
math_Matrix CreateIdentityMatrix(int theN)
{
  math_Matrix aMat(1, theN, 1, theN, 0.0);
  for (int i = 1; i <= theN; ++i)
  {
    aMat(i, i) = 1.0;
  }
  return aMat;
}

} // namespace

// ============================================================================
// math_Powell vs MathOpt::Powell comparison tests
// ============================================================================

TEST(MathOpt_NDim_ComparisonTest, Powell_Quadratic)
{
  QuadraticFuncOld anOldFunc;
  QuadraticFuncNew aNewFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 5.0;
  aStart(2) = 7.0;

  // Old API
  math_Powell anOldSolver(anOldFunc, 1.0e-8, 200);
  math_Matrix aDirections = CreateIdentityMatrix(2);
  anOldSolver.Perform(anOldFunc, aStart, aDirections);

  // New API
  MathOpt::Config aConfig;
  aConfig.MaxIterations = 200;
  aConfig.XTolerance    = 1.0e-8;
  aConfig.FTolerance    = 1.0e-10;
  auto aNewResult       = MathOpt::Powell(aNewFunc, aStart, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Both should find the same minimum
  EXPECT_NEAR(anOldSolver.Location()(1), (*aNewResult.Solution)(1), THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Location()(2), (*aNewResult.Solution)(2), THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Minimum(), *aNewResult.Value, THE_TOLERANCE);

  // Verify correctness
  EXPECT_NEAR((*aNewResult.Solution)(1), 1.0, THE_TOLERANCE);
  EXPECT_NEAR((*aNewResult.Solution)(2), 2.0, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Value, 0.0, THE_TOLERANCE);
}

TEST(MathOpt_NDim_ComparisonTest, Powell_Booth)
{
  BoothFuncOld anOldFunc;
  BoothFuncNew aNewFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 0.0;
  aStart(2) = 0.0;

  // Old API
  math_Powell anOldSolver(anOldFunc, 1.0e-8, 200);
  math_Matrix aDirections = CreateIdentityMatrix(2);
  anOldSolver.Perform(anOldFunc, aStart, aDirections);

  // New API
  MathOpt::Config aConfig;
  aConfig.MaxIterations = 200;
  aConfig.XTolerance    = 1.0e-8;
  aConfig.FTolerance    = 1.0e-10;
  auto aNewResult       = MathOpt::Powell(aNewFunc, aStart, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Both should find the same minimum
  EXPECT_NEAR(anOldSolver.Location()(1), (*aNewResult.Solution)(1), THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Location()(2), (*aNewResult.Solution)(2), THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Minimum(), *aNewResult.Value, THE_TOLERANCE);

  // Verify correctness
  EXPECT_NEAR((*aNewResult.Solution)(1), 1.0, THE_TOLERANCE);
  EXPECT_NEAR((*aNewResult.Solution)(2), 3.0, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Value, 0.0, THE_TOLERANCE);
}

TEST(MathOpt_NDim_ComparisonTest, Powell_Rosenbrock)
{
  RosenbrockFuncOld anOldFunc;
  RosenbrockFuncNew aNewFunc;

  math_Vector aStart(1, 2);
  aStart(1) = -1.0;
  aStart(2) = 1.0;

  // Old API - needs more iterations for Rosenbrock
  math_Powell anOldSolver(anOldFunc, 1.0e-6, 1000);
  math_Matrix aDirections = CreateIdentityMatrix(2);
  anOldSolver.Perform(anOldFunc, aStart, aDirections);

  // New API
  MathOpt::Config aConfig;
  aConfig.MaxIterations = 1000;
  aConfig.XTolerance    = 1.0e-6;
  aConfig.FTolerance    = 1.0e-8;
  auto aNewResult       = MathOpt::Powell(aNewFunc, aStart, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Both should find the minimum (relaxed tolerance for Rosenbrock)
  EXPECT_NEAR(anOldSolver.Location()(1), (*aNewResult.Solution)(1), 1.0e-3);
  EXPECT_NEAR(anOldSolver.Location()(2), (*aNewResult.Solution)(2), 1.0e-3);

  // Verify correctness (Rosenbrock minimum at (1, 1))
  EXPECT_NEAR((*aNewResult.Solution)(1), 1.0, 1.0e-3);
  EXPECT_NEAR((*aNewResult.Solution)(2), 1.0, 1.0e-3);
}

TEST(MathOpt_NDim_ComparisonTest, Powell_Sphere3D)
{
  SphereFuncOld anOldFunc(3);
  SphereFuncNew aNewFunc;

  math_Vector aStart(1, 3);
  aStart(1) = 3.0;
  aStart(2) = -2.0;
  aStart(3) = 4.0;

  // Old API
  math_Powell anOldSolver(anOldFunc, 1.0e-8, 200);
  math_Matrix aDirections = CreateIdentityMatrix(3);
  anOldSolver.Perform(anOldFunc, aStart, aDirections);

  // New API
  MathOpt::Config aConfig;
  aConfig.MaxIterations = 200;
  aConfig.XTolerance    = 1.0e-8;
  aConfig.FTolerance    = 1.0e-10;
  auto aNewResult       = MathOpt::Powell(aNewFunc, aStart, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Both should find the origin
  for (int i = 1; i <= 3; ++i)
  {
    EXPECT_NEAR(anOldSolver.Location()(i), (*aNewResult.Solution)(i), THE_TOLERANCE);
    EXPECT_NEAR((*aNewResult.Solution)(i), 0.0, THE_TOLERANCE);
  }
  EXPECT_NEAR(anOldSolver.Minimum(), *aNewResult.Value, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Value, 0.0, THE_TOLERANCE);
}

// ============================================================================
// math_BFGS vs MathOpt::BFGS comparison tests
// ============================================================================

TEST(MathOpt_NDim_ComparisonTest, BFGS_Quadratic)
{
  QuadraticFuncGradOld anOldFunc;
  QuadraticFuncNew     aNewFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 5.0;
  aStart(2) = 7.0;

  // Old API
  math_BFGS anOldSolver(2, 1.0e-8, 100);
  anOldSolver.Perform(anOldFunc, aStart);

  // New API
  MathOpt::Config aConfig;
  aConfig.MaxIterations = 100;
  aConfig.XTolerance    = 1.0e-8;
  aConfig.FTolerance    = 1.0e-10;
  auto aNewResult       = MathOpt::BFGS(aNewFunc, aStart, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Both should find the same minimum
  EXPECT_NEAR(anOldSolver.Location()(1), (*aNewResult.Solution)(1), THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Location()(2), (*aNewResult.Solution)(2), THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Minimum(), *aNewResult.Value, THE_TOLERANCE);

  // Verify correctness
  EXPECT_NEAR((*aNewResult.Solution)(1), 1.0, THE_TOLERANCE);
  EXPECT_NEAR((*aNewResult.Solution)(2), 2.0, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Value, 0.0, THE_TOLERANCE);
}

TEST(MathOpt_NDim_ComparisonTest, BFGS_Booth)
{
  BoothFuncGradOld anOldFunc;
  BoothFuncNew     aNewFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 0.0;
  aStart(2) = 0.0;

  // Old API
  math_BFGS anOldSolver(2, 1.0e-8, 100);
  anOldSolver.Perform(anOldFunc, aStart);

  // New API
  MathOpt::Config aConfig;
  aConfig.MaxIterations = 100;
  aConfig.XTolerance    = 1.0e-8;
  aConfig.FTolerance    = 1.0e-10;
  auto aNewResult       = MathOpt::BFGS(aNewFunc, aStart, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Both should find the same minimum
  EXPECT_NEAR(anOldSolver.Location()(1), (*aNewResult.Solution)(1), THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Location()(2), (*aNewResult.Solution)(2), THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Minimum(), *aNewResult.Value, THE_TOLERANCE);

  // Verify correctness
  EXPECT_NEAR((*aNewResult.Solution)(1), 1.0, THE_TOLERANCE);
  EXPECT_NEAR((*aNewResult.Solution)(2), 3.0, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Value, 0.0, THE_TOLERANCE);
}

TEST(MathOpt_NDim_ComparisonTest, BFGS_Rosenbrock)
{
  RosenbrockFuncGradOld anOldFunc;
  RosenbrockFuncNew     aNewFunc;

  math_Vector aStart(1, 2);
  aStart(1) = -1.0;
  aStart(2) = 1.0;

  // Old API - needs more iterations for Rosenbrock
  math_BFGS anOldSolver(2, 1.0e-6, 1000);
  anOldSolver.Perform(anOldFunc, aStart);

  // New API
  MathOpt::Config aConfig;
  aConfig.MaxIterations = 1000;
  aConfig.XTolerance    = 1.0e-6;
  aConfig.FTolerance    = 1.0e-8;
  auto aNewResult       = MathOpt::BFGS(aNewFunc, aStart, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Both should find the minimum (relaxed tolerance for Rosenbrock)
  EXPECT_NEAR(anOldSolver.Location()(1), (*aNewResult.Solution)(1), 1.0e-3);
  EXPECT_NEAR(anOldSolver.Location()(2), (*aNewResult.Solution)(2), 1.0e-3);

  // Verify correctness (Rosenbrock minimum at (1, 1))
  EXPECT_NEAR((*aNewResult.Solution)(1), 1.0, 1.0e-3);
  EXPECT_NEAR((*aNewResult.Solution)(2), 1.0, 1.0e-3);
}

TEST(MathOpt_NDim_ComparisonTest, BFGS_Sphere3D)
{
  SphereFuncGradOld anOldFunc(3);
  SphereFuncNew     aNewFunc;

  math_Vector aStart(1, 3);
  aStart(1) = 3.0;
  aStart(2) = -2.0;
  aStart(3) = 4.0;

  // Old API
  math_BFGS anOldSolver(3, 1.0e-8, 100);
  anOldSolver.Perform(anOldFunc, aStart);

  // New API
  MathOpt::Config aConfig;
  aConfig.MaxIterations = 100;
  aConfig.XTolerance    = 1.0e-8;
  aConfig.FTolerance    = 1.0e-10;
  auto aNewResult       = MathOpt::BFGS(aNewFunc, aStart, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Both should find the origin
  for (int i = 1; i <= 3; ++i)
  {
    EXPECT_NEAR(anOldSolver.Location()(i), (*aNewResult.Solution)(i), THE_TOLERANCE);
    EXPECT_NEAR((*aNewResult.Solution)(i), 0.0, THE_TOLERANCE);
  }
  EXPECT_NEAR(anOldSolver.Minimum(), *aNewResult.Value, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Value, 0.0, THE_TOLERANCE);
}

// ============================================================================
// Iteration count comparison tests
// ============================================================================

TEST(MathOpt_NDim_ComparisonTest, IterationCount_BFGSVsPowell)
{
  QuadraticFuncNew aNewFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 10.0;
  aStart(2) = 10.0;

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 500;
  aConfig.XTolerance    = 1.0e-8;
  aConfig.FTolerance    = 1.0e-10;

  auto aPowellResult = MathOpt::Powell(aNewFunc, aStart, aConfig);
  auto aBFGSResult   = MathOpt::BFGS(aNewFunc, aStart, aConfig);

  ASSERT_TRUE(aPowellResult.IsDone());
  ASSERT_TRUE(aBFGSResult.IsDone());

  // Both should find the same minimum
  EXPECT_NEAR(*aPowellResult.Value, *aBFGSResult.Value, THE_TOLERANCE);

  // BFGS should use fewer iterations on quadratic functions (due to superlinear convergence)
  EXPECT_LT(aBFGSResult.NbIterations, aPowellResult.NbIterations);
}

TEST(MathOpt_NDim_ComparisonTest, IterationCount_LBFGSVsBFGS)
{
  SphereFuncNew aNewFunc;

  math_Vector aStart(1, 5);
  for (int i = 1; i <= 5; ++i)
  {
    aStart(i) = static_cast<double>(i);
  }

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 100;
  aConfig.XTolerance    = 1.0e-8;
  aConfig.FTolerance    = 1.0e-10;

  auto aBFGSResult  = MathOpt::BFGS(aNewFunc, aStart, aConfig);
  auto aLBFGSResult = MathOpt::LBFGS(aNewFunc, aStart, 10, aConfig);

  ASSERT_TRUE(aBFGSResult.IsDone());
  ASSERT_TRUE(aLBFGSResult.IsDone());

  // Both should find the same minimum
  EXPECT_NEAR(*aBFGSResult.Value, *aLBFGSResult.Value, THE_TOLERANCE);

  // Both should find the origin
  for (int i = 1; i <= 5; ++i)
  {
    EXPECT_NEAR((*aBFGSResult.Solution)(i), 0.0, THE_TOLERANCE);
    EXPECT_NEAR((*aLBFGSResult.Solution)(i), 0.0, THE_TOLERANCE);
  }
}

// ============================================================================
// Accuracy comparison tests
// ============================================================================

TEST(MathOpt_NDim_ComparisonTest, Accuracy_BFGSVsOld)
{
  QuadraticFuncGradOld anOldFunc;
  QuadraticFuncNew     aNewFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 5.0;
  aStart(2) = 7.0;

  // Tighter tolerance
  const double aTightTol = 1.0e-12;

  // Old API
  math_BFGS anOldSolver(2, aTightTol, 200);
  anOldSolver.Perform(anOldFunc, aStart);

  // New API
  MathOpt::Config aConfig;
  aConfig.MaxIterations = 200;
  aConfig.XTolerance    = aTightTol;
  aConfig.FTolerance    = 1.0e-14;
  auto aNewResult       = MathOpt::BFGS(aNewFunc, aStart, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Both should achieve good accuracy
  EXPECT_NEAR(anOldSolver.Location()(1), 1.0, 1.0e-8);
  EXPECT_NEAR(anOldSolver.Location()(2), 2.0, 1.0e-8);
  EXPECT_NEAR((*aNewResult.Solution)(1), 1.0, 1.0e-8);
  EXPECT_NEAR((*aNewResult.Solution)(2), 2.0, 1.0e-8);
}

// ============================================================================
// Different starting points comparison tests
// ============================================================================

TEST(MathOpt_NDim_ComparisonTest, DifferentStartingPoints)
{
  QuadraticFuncGradOld anOldFunc;
  QuadraticFuncNew     aNewFunc;

  // Test multiple starting points
  std::vector<std::pair<double, double>> aStartPoints = {
    {0.0, 0.0}, {10.0, 10.0}, {-5.0, 5.0}, {100.0, -50.0}};

  for (const auto& aPoint : aStartPoints)
  {
    math_Vector aStart(1, 2);
    aStart(1) = aPoint.first;
    aStart(2) = aPoint.second;

    // Old API
    math_BFGS anOldSolver(2, 1.0e-8, 200);
    anOldSolver.Perform(anOldFunc, aStart);

    // New API
    MathOpt::Config aConfig;
    aConfig.MaxIterations = 200;
    aConfig.XTolerance    = 1.0e-8;
    aConfig.FTolerance    = 1.0e-10;
    auto aNewResult       = MathOpt::BFGS(aNewFunc, aStart, aConfig);

    ASSERT_TRUE(anOldSolver.IsDone())
      << "Old API failed for start point (" << aPoint.first << ", " << aPoint.second << ")";
    ASSERT_TRUE(aNewResult.IsDone())
      << "New API failed for start point (" << aPoint.first << ", " << aPoint.second << ")";

    // Both should find the same minimum
    EXPECT_NEAR(anOldSolver.Location()(1), (*aNewResult.Solution)(1), THE_TOLERANCE);
    EXPECT_NEAR(anOldSolver.Location()(2), (*aNewResult.Solution)(2), THE_TOLERANCE);
    EXPECT_NEAR((*aNewResult.Solution)(1), 1.0, THE_TOLERANCE);
    EXPECT_NEAR((*aNewResult.Solution)(2), 2.0, THE_TOLERANCE);
  }
}
