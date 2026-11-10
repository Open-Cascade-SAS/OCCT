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

#include <MathSys_Newton.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <gtest/gtest.h>
#include <cmath>
#include <array>

namespace
{
//! Independent quadratic equations with roots 1, 2, ... .
struct MathSys_IndependentQuadratics
{
  int  Count; //!< Number of variables and equations.
  int  NbVariables() const;
  int  NbEquations() const;
  bool Value(const math_Vector& theX, math_Vector& theF) const;
  bool Values(const math_Vector& theX, math_Vector& theF, math_Matrix& theD) const;
};

//==================================================================================================

int MathSys_IndependentQuadratics::NbVariables() const
{
  return Count;
}

//==================================================================================================

int MathSys_IndependentQuadratics::NbEquations() const
{
  return Count;
}

//==================================================================================================

bool MathSys_IndependentQuadratics::Value(const math_Vector& theX, math_Vector& theF) const
{
  for (size_t anIndex = 0; anIndex < static_cast<size_t>(Count); ++anIndex)
  {
    const double aRoot     = static_cast<double>(anIndex + 1);
    theF.ChangeAt(anIndex) = theX.At(anIndex) * theX.At(anIndex) - aRoot * aRoot;
  }
  return true;
}

//==================================================================================================

bool MathSys_IndependentQuadratics::Values(const math_Vector& theX,
                                           math_Vector&       theF,
                                           math_Matrix&       theD) const
{
  theD.Init(0.0);
  for (size_t anIndex = 0; anIndex < static_cast<size_t>(Count); ++anIndex)
  {
    theD.ChangeAt(anIndex, anIndex) = 2.0 * theX.At(anIndex);
  }
  return Value(theX, theF);
}

//! A bounded exponential residual whose Newton direction needs deep backtracking.
class MathSys_ExponentialBacktrackingSystem
{
public:
  //! One unknown coordinate.
  int NbVariables() const;
  //! One residual equation.
  int NbEquations() const;
  //! Evaluate exp(x) - 1.
  bool Value(const math_Vector& theX, math_Vector& theF) const;
  //! Evaluate the residual and its derivative exp(x).
  bool Values(const math_Vector& theX, math_Vector& theF, math_Matrix& theD) const;
};

//==================================================================================================

int MathSys_ExponentialBacktrackingSystem::NbVariables() const
{
  return 1;
}

//==================================================================================================

int MathSys_ExponentialBacktrackingSystem::NbEquations() const
{
  return 1;
}

//==================================================================================================

bool MathSys_ExponentialBacktrackingSystem::Value(const math_Vector& theX, math_Vector& theF) const
{
  theF.ChangeAt(0) = std::expm1(theX.At(0));
  return true;
}

//==================================================================================================

bool MathSys_ExponentialBacktrackingSystem::Values(const math_Vector& theX,
                                                   math_Vector&       theF,
                                                   math_Matrix&       theD) const
{
  theD.ChangeAt(0, 0) = std::exp(theX.At(0));
  return Value(theX, theF);
}
} // namespace

//==================================================================================================

TEST(MathSys_ComparisonTest, NewtonBounded_MultipleIterationsWithFixedVariable)
{
  for (const int aCount : {4, 40})
  {
    SCOPED_TRACE(aCount);
    MathSys_IndependentQuadratics aFunction{aCount};
    const size_t                  aSize = static_cast<size_t>(aCount);
    math_Vector                   aStart(aSize);
    math_Vector                   aLower(aSize, 0.0);
    math_Vector                   anUpper(aSize, 100.0);
    for (size_t anIndex = 0; anIndex < aSize; ++anIndex)
    {
      aStart.ChangeAt(anIndex) = static_cast<double>(anIndex + 2);
    }
    aLower.ChangeAt(0) = anUpper.ChangeAt(0) = 1.0;
    const math_Vector           aTolerance(aSize, 1.e-10);
    const MathSys::SystemResult aResult =
      MathSys::NewtonBounded(aFunction, aStart, aLower, anUpper, aTolerance, 1.e-10);
    ASSERT_TRUE(aResult.IsDone());
    EXPECT_TRUE(aResult.IsRoot);
    ASSERT_TRUE(aResult.Solution.has_value());
    EXPECT_LE(aResult.ResidualNorm, 1.e-10);
    for (size_t anIndex = 0; anIndex < aSize; ++anIndex)
    {
      EXPECT_NEAR(aResult.Solution->At(anIndex), static_cast<double>(anIndex + 1), 1.e-10);
    }
  }
}

//==================================================================================================

TEST(MathSys_ComparisonTest, NewtonBounded_BacktrackingContinuesWhileRepresentable)
{
  MathSys_ExponentialBacktrackingSystem aFunction;
  const math_Vector                     aLower(size_t{1}, -23.0);
  const math_Vector                     anUpper(size_t{1}, 40.0);
  const math_Vector                     aTolX(size_t{1}, 1.0e-8);
  for (const double aSeed : std::array<double, 3>{{-18.0, -20.0, -22.0}})
  {
    SCOPED_TRACE(aSeed);
    const math_Vector           aStart(size_t{1}, aSeed);
    const MathSys::SystemResult aResult =
      MathSys::NewtonBounded(aFunction, aStart, aLower, anUpper, aTolX, 1.0e-8);
    EXPECT_TRUE(aResult.IsDone()) << "status " << static_cast<int>(aResult.Status)
                                  << ", iterations " << aResult.NbIterations;
    EXPECT_TRUE(aResult.IsRoot);
    ASSERT_TRUE(aResult.Solution.has_value());
    EXPECT_NEAR(aResult.Solution->At(0), 0.0, 1.0e-8);
    EXPECT_LE(aResult.ResidualNorm, 1.0e-8);
  }
}

//==================================================================================================

TEST(MathSys_ComparisonTest, NewtonBounded_DeepBacktrackingPreservesIterationBudget)
{
  MathSys_ExponentialBacktrackingSystem aFunction;
  const math_Vector                     aStart(size_t{1}, -20.0);
  const math_Vector                     aLower(size_t{1}, -23.0);
  const math_Vector                     anUpper(size_t{1}, 40.0);
  const math_Vector                     aTolerance(size_t{1}, 1.e-8);
  const MathSys::SystemResult           aResult =
    MathSys::NewtonBounded(aFunction, aStart, aLower, anUpper, aTolerance, 1.e-8, 1);
  EXPECT_EQ(aResult.Status, MathUtils::Status::MaxIterations);
  EXPECT_EQ(aResult.NbIterations, 1u);
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_GT(aResult.Solution->At(0), aStart.At(0));
  EXPECT_FALSE(aResult.IsRoot);
  const MathSys::SystemResult aZeroBudget =
    MathSys::NewtonBounded(aFunction, aStart, aLower, anUpper, aTolerance, 1.e-8, 0);
  EXPECT_EQ(aZeroBudget.Status, MathUtils::Status::InvalidInput);
}

namespace
{
//! Linear residual with a root between adjacent representable coordinates.
class MathSys_RoundedStepSystem
{
public:
  //! One unknown coordinate.
  int NbVariables() const;
  //! One residual equation.
  int NbEquations() const;
  //! Evaluate (x - 1e16) + 1 without rounding the target coordinate.
  bool Value(const math_Vector& theX, math_Vector& theF) const;
  //! Evaluate the residual with unit derivative.
  bool Values(const math_Vector& theX, math_Vector& theF, math_Matrix& theD) const;
};

//==================================================================================================

int MathSys_RoundedStepSystem::NbVariables() const
{
  return 1;
}

//==================================================================================================

int MathSys_RoundedStepSystem::NbEquations() const
{
  return 1;
}

//==================================================================================================

bool MathSys_RoundedStepSystem::Value(const math_Vector& theX, math_Vector& theF) const
{
  theF.ChangeAt(0) = (theX.At(0) - 1.e16) + 1;
  return true;
}

//==================================================================================================

bool MathSys_RoundedStepSystem::Values(const math_Vector& theX,
                                       math_Vector&       theF,
                                       math_Matrix&       theD) const
{
  theD.ChangeAt(0, 0) = 1;
  return Value(theX, theF);
}
} // namespace

//==================================================================================================

TEST(MathSys_ComparisonTest, Newton_BacktrackingStopsWithoutRepresentableMovement)
{
  MathSys_RoundedStepSystem   aFunction;
  const math_Vector           aStart(size_t{1}, 1.e16);
  const MathSys::SystemResult aResult = MathSys::Newton(aFunction, aStart, 1.e-8, 1.e-8);
  EXPECT_EQ(aResult.Status, MathUtils::Status::NotConverged);
  EXPECT_EQ(aResult.NbIterations, 1u);
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_EQ(aResult.Solution->At(0), aStart.At(0));
  EXPECT_FALSE(aResult.IsRoot);
}
