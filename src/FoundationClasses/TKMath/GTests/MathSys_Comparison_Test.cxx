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

#include <MathSys_Newton.hxx>
#include <math_NewtonFunctionSetRoot.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <math_Vector.hxx>
#include <math_Matrix.hxx>

#include <cmath>
#include <limits>

namespace
{
constexpr double THE_TOLERANCE = 1.0e-8;

//! Simple 2x2 nonlinear system: F1 = x^2 + y^2 - 1, F2 = x - y
//! Solution: x = y = sqrt(0.5) ~ 0.7071
class SimpleCircleLineSystem : public math_FunctionSetWithDerivatives
{
public:
  int NbVariables() const override { return 2; }

  int NbEquations() const override { return 2; }

  bool Value(const math_Vector& X, math_Vector& F) override
  {
    F.ChangeAt(0) = X.At(0) * X.At(0) + X.At(1) * X.At(1) - 1.0; // x^2 + y^2 = 1
    F.ChangeAt(1) = X.At(0) - X.At(1);                           // x = y
    return true;
  }

  bool Derivatives(const math_Vector& X, math_Matrix& D) override
  {
    D.ChangeAt(0, 0) = 2.0 * X.At(0); // dF1/dx
    D.ChangeAt(0, 1) = 2.0 * X.At(1); // dF1/dy
    D.ChangeAt(1, 0) = 1.0;           // dF2/dx
    D.ChangeAt(1, 1) = -1.0;          // dF2/dy
    return true;
  }

  bool Values(const math_Vector& X, math_Vector& F, math_Matrix& D) override
  {
    return Value(X, F) && Derivatives(X, D);
  }
};

//! Rosenbrock-like 2D system: F1 = 10*(y - x^2), F2 = 1 - x
//! Solution: x = 1, y = 1
class RosenbrockSystem : public math_FunctionSetWithDerivatives
{
public:
  int NbVariables() const override { return 2; }

  int NbEquations() const override { return 2; }

  bool Value(const math_Vector& X, math_Vector& F) override
  {
    F.ChangeAt(0) = 10.0 * (X.At(1) - X.At(0) * X.At(0));
    F.ChangeAt(1) = 1.0 - X.At(0);
    return true;
  }

  bool Derivatives(const math_Vector& X, math_Matrix& D) override
  {
    D.ChangeAt(0, 0) = -20.0 * X.At(0);
    D.ChangeAt(0, 1) = 10.0;
    D.ChangeAt(1, 0) = -1.0;
    D.ChangeAt(1, 1) = 0.0;
    return true;
  }

  bool Values(const math_Vector& X, math_Vector& F, math_Matrix& D) override
  {
    return Value(X, F) && Derivatives(X, D);
  }
};

class DiagonalQuadraticSystem : public math_FunctionSetWithDerivatives
{
public:
  int NbVariables() const override { return 2; }

  int NbEquations() const override { return 2; }

  bool Value(const math_Vector& theX, math_Vector& theF) override
  {
    theF.ChangeAt(0) = theX.At(0) * theX.At(0) - 2.0;
    theF.ChangeAt(1) = theX.At(1);
    return true;
  }

  bool Derivatives(const math_Vector& theX, math_Matrix& theD) override
  {
    theD.ChangeAt(0, 0) = 2.0 * theX.At(0);
    theD.ChangeAt(0, 1) = 0.0;
    theD.ChangeAt(1, 0) = 0.0;
    theD.ChangeAt(1, 1) = 1.0;
    return true;
  }

  bool Values(const math_Vector& theX, math_Vector& theF, math_Matrix& theD) override
  {
    return Value(theX, theF) && Derivatives(theX, theD);
  }
};

class BoundaryTangentSystem : public math_FunctionSetWithDerivatives
{
public:
  int NbVariables() const override { return 2; }

  int NbEquations() const override { return 2; }

  bool Value(const math_Vector& theX, math_Vector& theF) override
  {
    const double aX  = theX.At(0);
    const double aY  = theX.At(1);
    theF.ChangeAt(0) = (1.0 + aX) * (aY - 1.0);
    theF.ChangeAt(1) = aY * (aY - 1.0);
    return true;
  }

  bool Derivatives(const math_Vector& theX, math_Matrix& theD) override
  {
    const double aX = theX.At(0);
    const double aY = theX.At(1);
    theD.ChangeAt(0, 0) = aY - 1.0;
    theD.ChangeAt(0, 1) = 1.0 + aX;
    theD.ChangeAt(1, 0) = 0.0;
    theD.ChangeAt(1, 1) = 2.0 * aY - 1.0;
    return true;
  }

  bool Values(const math_Vector& theX, math_Vector& theF, math_Matrix& theD) override
  {
    return Value(theX, theF) && Derivatives(theX, theD);
  }
};

//! 3x3 linear-like system: F1 = 2x + y - 2, F2 = x + 3y - 5, F3 = z - 1
//! Solution: x = 0.2, y = 1.6, z = 1
class Linear3System : public math_FunctionSetWithDerivatives
{
public:
  int NbVariables() const override { return 3; }

  int NbEquations() const override { return 3; }

  bool Value(const math_Vector& X, math_Vector& F) override
  {
    F.ChangeAt(0) = 2.0 * X.At(0) + X.At(1) - 2.0;
    F.ChangeAt(1) = X.At(0) + 3.0 * X.At(1) - 5.0;
    F.ChangeAt(2) = X.At(2) - 1.0;
    return true;
  }

  bool Derivatives(const math_Vector&, math_Matrix& D) override
  {
    D.ChangeAt(0, 0) = 2.0;
    D.ChangeAt(0, 1) = 1.0;
    D.ChangeAt(0, 2) = 0.0;
    D.ChangeAt(1, 0) = 1.0;
    D.ChangeAt(1, 1) = 3.0;
    D.ChangeAt(1, 2) = 0.0;
    D.ChangeAt(2, 0) = 0.0;
    D.ChangeAt(2, 1) = 0.0;
    D.ChangeAt(2, 2) = 1.0;
    return true;
  }

  bool Values(const math_Vector& X, math_Vector& F, math_Matrix& D) override
  {
    return Value(X, F) && Derivatives(X, D);
  }
};

//! Polynomial system: x^3 - y = 0, y^3 - x = 0
//! Solutions: (0,0), (1,1), (-1,-1)
class PolynomialSystem : public math_FunctionSetWithDerivatives
{
public:
  int NbVariables() const override { return 2; }

  int NbEquations() const override { return 2; }

  bool Value(const math_Vector& X, math_Vector& F) override
  {
    F.ChangeAt(0) = X.At(0) * X.At(0) * X.At(0) - X.At(1);
    F.ChangeAt(1) = X.At(1) * X.At(1) * X.At(1) - X.At(0);
    return true;
  }

  bool Derivatives(const math_Vector& X, math_Matrix& D) override
  {
    D.ChangeAt(0, 0) = 3.0 * X.At(0) * X.At(0);
    D.ChangeAt(0, 1) = -1.0;
    D.ChangeAt(1, 0) = -1.0;
    D.ChangeAt(1, 1) = 3.0 * X.At(1) * X.At(1);
    return true;
  }

  bool Values(const math_Vector& X, math_Vector& F, math_Matrix& D) override
  {
    return Value(X, F) && Derivatives(X, D);
  }
};

class RectangularSystem
{
public:
  RectangularSystem(int theNbVariables, int theNbEquations)
      : myNbVariables(theNbVariables),
        myNbEquations(theNbEquations)
  {
  }

  int NbVariables() const { return myNbVariables; }

  int NbEquations() const { return myNbEquations; }

  bool Value(const math_Vector& theX, math_Vector& theF)
  {
    if (myNbEquations == 1)
    {
      theF.ChangeAt(0) = theX.At(0) + theX.At(1) - 3.0;
      return true;
    }
    theF.ChangeAt(0) = theX.At(0) - 1.0;
    theF.ChangeAt(1) = theX.At(1) - 2.0;
    theF.ChangeAt(2) = theX.At(0) + theX.At(1) - 3.0;
    return true;
  }

  bool Derivatives(const math_Vector&, math_Matrix& theD)
  {
    theD.Init(0.0);
    if (myNbEquations == 1)
    {
      theD.ChangeAt(0, 0) = 1.0;
      theD.ChangeAt(0, 1) = 1.0;
      return true;
    }
    theD.ChangeAt(0, 0) = 1.0;
    theD.ChangeAt(1, 1) = 1.0;
    theD.ChangeAt(2, 0) = 1.0;
    theD.ChangeAt(2, 1) = 1.0;
    return true;
  }

  bool Values(const math_Vector& theX, math_Vector& theF, math_Matrix& theD)
  {
    return Value(theX, theF) && Derivatives(theX, theD);
  }

private:
  int myNbVariables;
  int myNbEquations;
};

class SingularSystem
{
public:
  int NbVariables() const { return 2; }

  int NbEquations() const { return 2; }

  bool Value(const math_Vector& theX, math_Vector& theF)
  {
    const double aValue = theX.At(0) + theX.At(1) - 2.0;
    theF.ChangeAt(0)    = aValue;
    theF.ChangeAt(1)    = 2.0 * aValue;
    return true;
  }

  bool Derivatives(const math_Vector&, math_Matrix& theD)
  {
    theD.ChangeAt(0, 0) = theD.ChangeAt(0, 1) = 1.0;
    theD.ChangeAt(1, 0) = theD.ChangeAt(1, 1) = 2.0;
    return true;
  }

  bool Values(const math_Vector& theX, math_Vector& theF, math_Matrix& theD)
  {
    return Value(theX, theF) && Derivatives(theX, theD);
  }
};

class FailingSystem : public SingularSystem
{
public:
  bool Values(const math_Vector&, math_Vector& theF, math_Matrix& theD)
  {
    theF.ChangeAt(0) = std::numeric_limits<double>::quiet_NaN();
    theF.ChangeAt(1) = 0.0;
    theD.Init(0.0);
    return true;
  }
};

class CallbackFailingSystem : public SingularSystem
{
public:
  bool Values(const math_Vector&, math_Vector&, math_Matrix&) { return false; }
};

class CoupledBoundedSystem
{
public:
  int NbVariables() const { return 2; }

  int NbEquations() const { return 2; }

  bool Value(const math_Vector& theX, math_Vector& theF)
  {
    constexpr double THE_INV_SQRT_TWO = 0.70710678118654752440;
    theF.ChangeAt(0) = std::sqrt(2.0) * theX.At(0) + 3.0 * THE_INV_SQRT_TWO * theX.At(1)
                       - THE_INV_SQRT_TWO;
    theF.ChangeAt(1) = THE_INV_SQRT_TWO * theX.At(1) - THE_INV_SQRT_TWO;
    return true;
  }

  bool Derivatives(const math_Vector&, math_Matrix& theD)
  {
    constexpr double THE_INV_SQRT_TWO = 0.70710678118654752440;
    theD.ChangeAt(0, 0) = std::sqrt(2.0);
    theD.ChangeAt(0, 1) = 3.0 * THE_INV_SQRT_TWO;
    theD.ChangeAt(1, 0) = 0.0;
    theD.ChangeAt(1, 1) = THE_INV_SQRT_TWO;
    return true;
  }

  bool Values(const math_Vector& theX, math_Vector& theF, math_Matrix& theD)
  {
    return Value(theX, theF) && Derivatives(theX, theD);
  }
};

class ExponentialCircleFunctionSet : public math_FunctionSetWithDerivatives
{
public:
  int NbVariables() const override { return 2; }

  int NbEquations() const override { return 2; }

  bool Value(const math_Vector& theX, math_Vector& theF) override
  {
    const double aX = theX.At(0);
    const double aY = theX.At(1);
    theF.ChangeAt(0) = std::exp(aX) + aY * aY - 2.0;
    theF.ChangeAt(1) = aX * aX + aY - 1.0;
    return true;
  }

  bool Derivatives(const math_Vector& theX, math_Matrix& theD) override
  {
    const double aX = theX.At(0);
    const double aY = theX.At(1);
    theD.ChangeAt(0, 0) = std::exp(aX);
    theD.ChangeAt(0, 1) = 2.0 * aY;
    theD.ChangeAt(1, 0) = 2.0 * aX;
    theD.ChangeAt(1, 1) = 1.0;
    return true;
  }

  bool Values(const math_Vector& theX, math_Vector& theF, math_Matrix& theD) override
  {
    return Value(theX, theF) && Derivatives(theX, theD);
  }
};

class TrigonometricProductFunctionSet : public math_FunctionSetWithDerivatives
{
public:
  int NbVariables() const override { return 2; }

  int NbEquations() const override { return 2; }

  bool Value(const math_Vector& theX, math_Vector& theF) override
  {
    const double aX = theX.At(0);
    const double aY = theX.At(1);
    theF.ChangeAt(0) = std::sin(aX) + aY * aY - 1.0;
    theF.ChangeAt(1) = aX * aY + std::cos(aY) - 1.2;
    return true;
  }

  bool Derivatives(const math_Vector& theX, math_Matrix& theD) override
  {
    const double aX = theX.At(0);
    const double aY = theX.At(1);
    theD.ChangeAt(0, 0) = std::cos(aX);
    theD.ChangeAt(0, 1) = 2.0 * aY;
    theD.ChangeAt(1, 0) = aY;
    theD.ChangeAt(1, 1) = aX - std::sin(aY);
    return true;
  }

  bool Values(const math_Vector& theX, math_Vector& theF, math_Matrix& theD) override
  {
    return Value(theX, theF) && Derivatives(theX, theD);
  }
};

class MixedSpaceFunctionSet : public math_FunctionSetWithDerivatives
{
public:
  int NbVariables() const override { return 3; }

  int NbEquations() const override { return 3; }

  bool Value(const math_Vector& theX, math_Vector& theF) override
  {
    const double aX = theX.At(0);
    const double aY = theX.At(1);
    const double aZ = theX.At(2);
    theF.ChangeAt(0) = aX * aX + aY + aZ - 2.5;
    theF.ChangeAt(1) = std::sin(aX) + aY * aY - aZ - 1.0;
    theF.ChangeAt(2) = std::exp(aZ) + aX * aY - 2.8;
    return true;
  }

  bool Derivatives(const math_Vector& theX, math_Matrix& theD) override
  {
    const double aX = theX.At(0);
    const double aY = theX.At(1);
    const double aZ = theX.At(2);
    theD.ChangeAt(0, 0) = 2.0 * aX;
    theD.ChangeAt(0, 1) = 1.0;
    theD.ChangeAt(0, 2) = 1.0;
    theD.ChangeAt(1, 0) = std::cos(aX);
    theD.ChangeAt(1, 1) = 2.0 * aY;
    theD.ChangeAt(1, 2) = -1.0;
    theD.ChangeAt(2, 0) = aY;
    theD.ChangeAt(2, 1) = aX;
    theD.ChangeAt(2, 2) = std::exp(aZ);
    return true;
  }

  bool Values(const math_Vector& theX, math_Vector& theF, math_Matrix& theD) override
  {
    return Value(theX, theF) && Derivatives(theX, theD);
  }
};

class CubicSpaceFunctionSet : public math_FunctionSetWithDerivatives
{
public:
  int NbVariables() const override { return 3; }

  int NbEquations() const override { return 3; }

  bool Value(const math_Vector& theX, math_Vector& theF) override
  {
    const double aX = theX.At(0);
    const double aY = theX.At(1);
    const double aZ = theX.At(2);
    theF.ChangeAt(0) = aX * aY + aZ * aZ - 1.5;
    theF.ChangeAt(1) = std::cos(aX) + aY - aZ - 0.6;
    theF.ChangeAt(2) = aX * aX * aX + aY * aY + aZ - 2.2;
    return true;
  }

  bool Derivatives(const math_Vector& theX, math_Matrix& theD) override
  {
    const double aX = theX.At(0);
    const double aY = theX.At(1);
    const double aZ = theX.At(2);
    theD.ChangeAt(0, 0) = aY;
    theD.ChangeAt(0, 1) = aX;
    theD.ChangeAt(0, 2) = 2.0 * aZ;
    theD.ChangeAt(1, 0) = -std::sin(aX);
    theD.ChangeAt(1, 1) = 1.0;
    theD.ChangeAt(1, 2) = -1.0;
    theD.ChangeAt(2, 0) = 3.0 * aX * aX;
    theD.ChangeAt(2, 1) = 2.0 * aY;
    theD.ChangeAt(2, 2) = 1.0;
    return true;
  }

  bool Values(const math_Vector& theX, math_Vector& theF, math_Matrix& theD) override
  {
    return Value(theX, theF) && Derivatives(theX, theD);
  }
};

class OverdeterminedTranscendentalFunctionSet : public math_FunctionSetWithDerivatives
{
public:
  int NbVariables() const override { return 2; }

  int NbEquations() const override { return 3; }

  bool Value(const math_Vector& theX, math_Vector& theF) override
  {
    const double aX = theX.At(0);
    const double aY = theX.At(1);
    theF.ChangeAt(0) = std::exp(aX) + aY * aY - 2.4621188003905088;
    theF.ChangeAt(1) = std::sin(aY) + aX * aY - 1.1973560908995227;
    theF.ChangeAt(2) = aX * aX + std::cos(aY) - 1.0567067093471654;
    return true;
  }

  bool Derivatives(const math_Vector& theX, math_Matrix& theD) override
  {
    const double aX = theX.At(0);
    const double aY = theX.At(1);
    theD.ChangeAt(0, 0) = std::exp(aX);
    theD.ChangeAt(0, 1) = 2.0 * aY;
    theD.ChangeAt(1, 0) = aY;
    theD.ChangeAt(1, 1) = std::cos(aY) + aX;
    theD.ChangeAt(2, 0) = 2.0 * aX;
    theD.ChangeAt(2, 1) = -std::sin(aY);
    return true;
  }

  bool Values(const math_Vector& theX, math_Vector& theF, math_Matrix& theD) override
  {
    return Value(theX, theF) && Derivatives(theX, theD);
  }
};

class BoundedLogarithmicFunctionSet : public math_FunctionSetWithDerivatives
{
public:
  int NbVariables() const override { return 2; }

  int NbEquations() const override { return 2; }

  bool Value(const math_Vector& theX, math_Vector& theF) override
  {
    const double aX = theX.At(0);
    const double aY = theX.At(1);
    theF.ChangeAt(0) = std::log(1.0 + aX) + aY * aY - 1.5;
    theF.ChangeAt(1) = aX * aY + std::sin(aY) - 1.0;
    return true;
  }

  bool Derivatives(const math_Vector& theX, math_Matrix& theD) override
  {
    const double aX = theX.At(0);
    const double aY = theX.At(1);
    theD.ChangeAt(0, 0) = 1.0 / (1.0 + aX);
    theD.ChangeAt(0, 1) = 2.0 * aY;
    theD.ChangeAt(1, 0) = aY;
    theD.ChangeAt(1, 1) = aX + std::cos(aY);
    return true;
  }

  bool Values(const math_Vector& theX, math_Vector& theF, math_Matrix& theD) override
  {
    return Value(theX, theF) && Derivatives(theX, theD);
  }
};
} // namespace

//=================================================================================================
// Test: Simple circle-line system - compare new API with legacy
//=================================================================================================

TEST(MathSys_ComparisonTest, Newton_CircleLineSystem)
{
  SimpleCircleLineSystem aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 0.5;
  aStart(2) = 0.5;

  math_Vector aTolX(1, 2, 1.0e-10);
  double      aTolF = 1.0e-10;

  // Solve using new API
  auto aNewResult = MathSys::Newton(aFunc, aStart, aTolX, aTolF);
  ASSERT_TRUE(aNewResult.IsDone());

  // Solve using legacy API
  math_NewtonFunctionSetRoot aOldSolver(aFunc, aTolX, aTolF);
  aOldSolver.Perform(aFunc, aStart);
  ASSERT_TRUE(aOldSolver.IsDone());

  const math_Vector& aOldRoot = aOldSolver.Root();

  // Expected solution: x = y = sqrt(0.5)
  const double aExpected = std::sqrt(0.5);

  // Compare solutions
  EXPECT_NEAR(aNewResult.Solution->At(0), aExpected, THE_TOLERANCE);
  EXPECT_NEAR(aNewResult.Solution->At(1), aExpected, THE_TOLERANCE);
  EXPECT_NEAR(aOldRoot(1), aExpected, THE_TOLERANCE);
  EXPECT_NEAR(aOldRoot(2), aExpected, THE_TOLERANCE);

  // Compare new and old results
  EXPECT_NEAR(aNewResult.Solution->At(0), aOldRoot.At(0), THE_TOLERANCE);
  EXPECT_NEAR(aNewResult.Solution->At(1), aOldRoot.At(1), THE_TOLERANCE);
}

TEST(MathSys_ComparisonTest, Newton_CircleLineFromUnequalStart)
{
  SimpleCircleLineSystem aFunc;
  math_Vector            aStart(2);
  aStart.ChangeAt(0)                  = 0.8;
  aStart.ChangeAt(1)                  = 0.6;
  const MathSys::SystemResult aResult = MathSys::Newton(aFunc, aStart, 1.0e-13, 1.0e-13);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(aResult.Solution->At(0), 0.70710678118654746, 1.0e-12);
  EXPECT_NEAR(aResult.Solution->At(1), 0.70710678118654746, 1.0e-12);
}

//=================================================================================================
// Test: Rosenbrock system
//=================================================================================================

TEST(MathSys_ComparisonTest, Newton_RosenbrockSystem)
{
  RosenbrockSystem aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 0.0;
  aStart(2) = 0.0;

  math_Vector aTolX(1, 2, 1.0e-10);
  double      aTolF = 1.0e-10;

  // Solve using new API
  auto aNewResult = MathSys::Newton(aFunc, aStart, aTolX, aTolF);
  ASSERT_TRUE(aNewResult.IsDone());

  // Solve using legacy API
  math_NewtonFunctionSetRoot aOldSolver(aFunc, aTolX, aTolF);
  aOldSolver.Perform(aFunc, aStart);
  ASSERT_TRUE(aOldSolver.IsDone());

  const math_Vector& aOldRoot = aOldSolver.Root();

  // Expected solution: x = 1, y = 1
  EXPECT_NEAR(aNewResult.Solution->At(0), 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aNewResult.Solution->At(1), 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aOldRoot(1), 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aOldRoot(2), 1.0, THE_TOLERANCE);

  // Compare new and old
  EXPECT_NEAR(aNewResult.Solution->At(0), aOldRoot.At(0), THE_TOLERANCE);
  EXPECT_NEAR(aNewResult.Solution->At(1), aOldRoot.At(1), THE_TOLERANCE);
}

//=================================================================================================
// Test: 3D linear system
//=================================================================================================

TEST(MathSys_ComparisonTest, Newton_Linear3System)
{
  Linear3System aFunc;

  math_Vector aStart(1, 3);
  aStart(1) = 0.0;
  aStart(2) = 0.0;
  aStart(3) = 0.0;

  math_Vector aTolX(1, 3, 1.0e-10);
  double      aTolF = 1.0e-10;

  // Solve using new API
  auto aNewResult = MathSys::Newton(aFunc, aStart, aTolX, aTolF);
  ASSERT_TRUE(aNewResult.IsDone());

  // Solve using legacy API
  math_NewtonFunctionSetRoot aOldSolver(aFunc, aTolX, aTolF);
  aOldSolver.Perform(aFunc, aStart);
  ASSERT_TRUE(aOldSolver.IsDone());

  const math_Vector& aOldRoot = aOldSolver.Root();

  // Expected solution: x = 0.2, y = 1.6, z = 1.0
  EXPECT_NEAR(aNewResult.Solution->At(0), 0.2, THE_TOLERANCE);
  EXPECT_NEAR(aNewResult.Solution->At(1), 1.6, THE_TOLERANCE);
  EXPECT_NEAR(aNewResult.Solution->At(2), 1.0, THE_TOLERANCE);

  // Compare new and old
  for (size_t i = 0; i < aNewResult.Solution->Size(); ++i)
  {
    EXPECT_NEAR(aNewResult.Solution->At(i), aOldRoot.At(i), THE_TOLERANCE);
  }
}

//=================================================================================================
// Test: Polynomial system finding (1,1) solution
//=================================================================================================

TEST(MathSys_ComparisonTest, Newton_PolynomialSystem)
{
  PolynomialSystem aFunc;

  // Start near (1,1)
  math_Vector aStart(1, 2);
  aStart(1) = 0.9;
  aStart(2) = 0.9;

  math_Vector aTolX(1, 2, 1.0e-10);
  double      aTolF = 1.0e-10;

  // Solve using new API
  auto aNewResult = MathSys::Newton(aFunc, aStart, aTolX, aTolF);
  ASSERT_TRUE(aNewResult.IsDone());

  // Solve using legacy API
  math_NewtonFunctionSetRoot aOldSolver(aFunc, aTolX, aTolF);
  aOldSolver.Perform(aFunc, aStart);
  ASSERT_TRUE(aOldSolver.IsDone());

  const math_Vector& aOldRoot = aOldSolver.Root();

  // Expected solution: x = 1, y = 1
  EXPECT_NEAR(aNewResult.Solution->At(0), 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aNewResult.Solution->At(1), 1.0, THE_TOLERANCE);

  // Compare new and old
  EXPECT_NEAR(aNewResult.Solution->At(0), aOldRoot.At(0), THE_TOLERANCE);
  EXPECT_NEAR(aNewResult.Solution->At(1), aOldRoot.At(1), THE_TOLERANCE);
}

//=================================================================================================
// Test: Bounded Newton with simple system
//=================================================================================================

TEST(MathSys_ComparisonTest, NewtonBounded_CircleLineSystem)
{
  SimpleCircleLineSystem aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 0.5;
  aStart(2) = 0.5;

  math_Vector aInfBound(1, 2);
  aInfBound(1) = 0.0;
  aInfBound(2) = 0.0;

  math_Vector aSupBound(1, 2);
  aSupBound(1) = 2.0;
  aSupBound(2) = 2.0;

  math_Vector aTolX(1, 2, 1.0e-10);
  double      aTolF = 1.0e-10;

  // Solve using new bounded API
  auto aNewResult = MathSys::NewtonBounded(aFunc, aStart, aInfBound, aSupBound, aTolX, aTolF);
  ASSERT_TRUE(aNewResult.IsDone());

  // Solve using legacy bounded API
  math_NewtonFunctionSetRoot aOldSolver(aFunc, aTolX, aTolF);
  aOldSolver.Perform(aFunc, aStart, aInfBound, aSupBound);
  ASSERT_TRUE(aOldSolver.IsDone());

  const math_Vector& aOldRoot = aOldSolver.Root();

  // Expected solution: x = y = sqrt(0.5)
  const double aExpected = std::sqrt(0.5);

  EXPECT_NEAR(aNewResult.Solution->At(0), aExpected, THE_TOLERANCE);
  EXPECT_NEAR(aNewResult.Solution->At(1), aExpected, THE_TOLERANCE);

  // Compare new and old
  EXPECT_NEAR(aNewResult.Solution->At(0), aOldRoot.At(0), THE_TOLERANCE);
  EXPECT_NEAR(aNewResult.Solution->At(1), aOldRoot.At(1), THE_TOLERANCE);
}

//=================================================================================================
// Test: Verify function values at solution are near zero
//=================================================================================================

TEST(MathSys_ComparisonTest, Newton_VerifyFunctionValues)
{
  SimpleCircleLineSystem aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 0.5;
  aStart(2) = 0.5;

  auto aResult = MathSys::Newton(aFunc, aStart, 1.0e-10, 1.0e-10);
  ASSERT_TRUE(aResult.IsDone());

  // Evaluate function at solution
  math_Vector aF(1, 2);
  ASSERT_TRUE(aFunc.Value(*aResult.Solution, aF));

  // Function values should be near zero
  EXPECT_NEAR(aF(1), 0.0, 1.0e-8);
  EXPECT_NEAR(aF(2), 0.0, 1.0e-8);
}

//=================================================================================================
// Test: Iteration count comparison
//=================================================================================================

TEST(MathSys_ComparisonTest, IterationCount_NewVsOld)
{
  RosenbrockSystem aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 0.0;
  aStart(2) = 0.0;

  math_Vector aTolX(1, 2, 1.0e-10);
  double      aTolF = 1.0e-10;

  // Solve using new API
  auto aNewResult = MathSys::Newton(aFunc, aStart, aTolX, aTolF);
  ASSERT_TRUE(aNewResult.IsDone());

  // Solve using legacy API
  math_NewtonFunctionSetRoot aOldSolver(aFunc, aTolX, aTolF);
  aOldSolver.Perform(aFunc, aStart);
  ASSERT_TRUE(aOldSolver.IsDone());

  // Both should converge in similar number of iterations
  // (exact count may differ slightly due to implementation details)
  EXPECT_GT(aNewResult.NbIterations, 0u);
  EXPECT_LE(aNewResult.NbIterations, 20u); // Reasonable upper bound

  // The iteration counts should be reasonably close
  int aOldIter = aOldSolver.NbIterations();
  EXPECT_NEAR(static_cast<int>(aNewResult.NbIterations), aOldIter, 5);
}

//=================================================================================================
// Test: Simplified Newton interface with uniform tolerance
//=================================================================================================

TEST(MathSys_ComparisonTest, Newton_UniformTolerance)
{
  SimpleCircleLineSystem aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 0.5;
  aStart(2) = 0.5;

  // Use simplified interface with uniform tolerance
  auto aResult = MathSys::Newton(aFunc, aStart, 1.0e-10, 1.0e-10);
  ASSERT_TRUE(aResult.IsDone());

  // Expected solution
  const double aExpected = std::sqrt(0.5);
  EXPECT_NEAR(aResult.Solution->At(0), aExpected, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Solution->At(1), aExpected, THE_TOLERANCE);
}

TEST(MathSys_ComparisonTest, Newton_ComponentToleranceControlsConvergence)
{
  DiagonalQuadraticSystem aFunc;
  math_Vector             aStart(size_t{2}, 0.0);
  aStart.ChangeAt(0) = 1.4;

  math_Vector aLooseTolX(size_t{2}, 1.0e-6);
  aLooseTolX.ChangeAt(0)                   = 2.0e-2;
  const MathSys::SystemResult aLooseResult = MathSys::Newton(aFunc, aStart, aLooseTolX, 1.0e-3, 10);
  ASSERT_TRUE(aLooseResult.IsDone());

  math_Vector aTightTolX(size_t{2}, 1.0e-6);
  aTightTolX.ChangeAt(0)                   = 1.0e-5;
  const MathSys::SystemResult aTightResult = MathSys::Newton(aFunc, aStart, aTightTolX, 1.0e-3, 10);
  ASSERT_TRUE(aTightResult.IsDone());
  EXPECT_GT(aTightResult.NbIterations, aLooseResult.NbIterations);
  EXPECT_LT(aTightResult.ResidualNorm, aLooseResult.ResidualNorm);

  math_Vector aRoot(size_t{2}, 0.0);
  aRoot.ChangeAt(0) = std::sqrt(2.0);
  const MathSys::SystemResult anInitialRoot =
    MathSys::Newton(aFunc, aRoot, aTightTolX, 1.0e-12, 10);
  EXPECT_TRUE(anInitialRoot.IsDone());
  EXPECT_EQ(anInitialRoot.NbIterations, 0u);
}

TEST(MathSys_ComparisonTest, NewtonBoundedResolvesFreeVariablesAtActiveBound)
{
  BoundaryTangentSystem aFunc;
  math_Vector           aStart(size_t{2}, 0.0);
  math_Vector           aLower(size_t{2});
  math_Vector           anUpper(size_t{2}, 2.0);
  math_Vector           aTolX(size_t{2}, 1.0e-10);
  aLower.ChangeAt(0) = 0.0;
  aLower.ChangeAt(1) = -2.0;

  const MathSys::SystemResult aResult =
    MathSys::NewtonBounded(aFunc, aStart, aLower, anUpper, aTolX, 1.0e-10);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.Solution->At(0), 0.0, 1.0e-12);
  EXPECT_NEAR(aResult.Solution->At(1), 1.0, 1.0e-10);
}

//=================================================================================================
// Test: Different starting points
//=================================================================================================

TEST(MathSys_ComparisonTest, Newton_DifferentStartingPoints)
{
  SimpleCircleLineSystem aFunc;

  math_Vector aTolX(1, 2, 1.0e-10);
  double      aTolF = 1.0e-10;

  // Test several starting points
  double aStartPoints[][2] = {
    {0.1, 0.1},
    {0.9, 0.9},
    {0.3, 0.7},
    {1.0, 0.0},
  };

  for (const auto& aStartPt : aStartPoints)
  {
    math_Vector aStart(1, 2);
    aStart(1) = aStartPt[0];
    aStart(2) = aStartPt[1];

    auto aNewResult = MathSys::Newton(aFunc, aStart, aTolX, aTolF);
    ASSERT_TRUE(aNewResult.IsDone())
      << "Failed for start (" << aStartPt[0] << ", " << aStartPt[1]
      << "), status=" << static_cast<int>(aNewResult.Status)
      << ", residual=" << aNewResult.ResidualNorm << ", step=" << aNewResult.StepNorm
      << ", iterations=" << aNewResult.NbIterations;

    math_NewtonFunctionSetRoot aOldSolver(aFunc, aTolX, aTolF);
    aOldSolver.Perform(aFunc, aStart);
    ASSERT_TRUE(aOldSolver.IsDone());

    const math_Vector& aOldRoot = aOldSolver.Root();

    // Both should find the same solution
    EXPECT_NEAR(aNewResult.Solution->At(0), aOldRoot.At(0), THE_TOLERANCE);
    EXPECT_NEAR(aNewResult.Solution->At(1), aOldRoot.At(1), THE_TOLERANCE);
  }
}

TEST(MathSys_ComparisonTest, Newton_RectangularAndRankDeficientSystems)
{
  math_Vector                 aStart(1, 2, 0.0);
  RectangularSystem           anOverdetermined(2, 3);
  const MathSys::SystemResult anOverResult =
    MathSys::Newton(anOverdetermined, aStart, 1.0e-12, 1.0e-12);
  ASSERT_TRUE(anOverResult.IsDone());
  EXPECT_EQ(anOverResult.Rank, 2);
  EXPECT_NEAR(anOverResult.Solution->At(0), 1.0, 1.0e-10);
  EXPECT_NEAR(anOverResult.Solution->At(1), 2.0, 1.0e-10);

  RectangularSystem           anUnderdetermined(2, 1);
  const MathSys::SystemResult anUnderResult =
    MathSys::Newton(anUnderdetermined, aStart, 1.0e-12, 1.0e-12);
  ASSERT_TRUE(anUnderResult.IsDone());
  EXPECT_EQ(anUnderResult.Rank, 1);
  EXPECT_NEAR(anUnderResult.Solution->At(0), 1.5, 1.0e-10);
  EXPECT_NEAR(anUnderResult.Solution->At(1), 1.5, 1.0e-10);

  SingularSystem              aSingular;
  const MathSys::SystemResult aSingularResult =
    MathSys::Newton(aSingular, aStart, 1.0e-12, 1.0e-12);
  ASSERT_TRUE(aSingularResult.IsDone());
  EXPECT_EQ(aSingularResult.Rank, 1);
  EXPECT_TRUE(aSingularResult.IsRoot);
}

TEST(MathSys_ComparisonTest, Newton_ArbitraryBoundsIndicesAndBoundStagnation)
{
  RectangularSystem           aSystem(2, 3);
  math_Vector                 aStart(5, 6, 0.0);
  math_Vector                 aTolerance(5, 6, 1.0e-12);
  math_Vector                 aLower(-2, -1, -10.0);
  math_Vector                 anUpper(9, 10, 10.0);
  const MathSys::SystemResult aResult =
    MathSys::NewtonBounded(aSystem, aStart, aLower, anUpper, aTolerance, 1.0e-12);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.Jacobian->LowerCol(), 0);
  EXPECT_NEAR(aResult.Solution->At(0), 1.0, 1.0e-10);
  EXPECT_NEAR(aResult.Solution->At(1), 2.0, 1.0e-10);

  anUpper(9) = 0.5;
  const MathSys::SystemResult aBlocked =
    MathSys::NewtonBounded(aSystem, aStart, aLower, anUpper, aTolerance, 1.0e-12, 20);
  EXPECT_FALSE(aBlocked.IsDone());
  EXPECT_FALSE(aBlocked.IsRoot);
  EXPECT_GT(aBlocked.ResidualNorm, 0.1);
}

TEST(MathSys_ComparisonTest, Newton_NonFiniteCallbackIsNumericalError)
{
  FailingSystem               aSystem;
  math_Vector                 aStart(1, 2, 0.0);
  const MathSys::SystemResult aResult = MathSys::Newton(aSystem, aStart, 1.0e-12, 1.0e-12);
  EXPECT_EQ(aResult.Status, MathUtils::Status::NumericalError);
  EXPECT_FALSE(aResult.IsDone());
}

TEST(MathSys_ComparisonTest, Newton_CallbackFalseIsCallbackError)
{
  CallbackFailingSystem       aSystem;
  math_Vector                 aStart(1, 2, 0.0);
  const MathSys::SystemResult aResult = MathSys::Newton(aSystem, aStart, 1.0e-12, 1.0e-12);
  EXPECT_EQ(aResult.Status, MathUtils::Status::CallbackError);
}

TEST(MathSys_ComparisonTest, NewtonBounded_UsesFeasibleGradientForActiveSet)
{
  CoupledBoundedSystem aSystem;
  math_Vector          aStart(size_t{2}, 0.0);
  math_Vector          aLower(size_t{2});
  math_Vector          anUpper(size_t{2});
  aLower.ChangeAt(0) = 0.0;
  aLower.ChangeAt(1) = -2.0;
  anUpper.ChangeAt(0) = 2.0;
  anUpper.ChangeAt(1) = 0.0;
  math_Vector aTolerance(size_t{2}, 1.0e-12);

  const MathSys::SystemResult aResult =
    MathSys::NewtonBounded(aSystem, aStart, aLower, anUpper, aTolerance, 1.0e-12, 10);
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_GT(aResult.Solution->At(0), 0.1);
  EXPECT_LT(aResult.ResidualNorm, 1.0);
}

TEST(MathSys_ComparisonTest, Linearized_UnequalColumnsRetainMinimumNormSolution)
{
  math_Matrix aMatrix(1, 2, 0.0);
  aMatrix.ChangeAt(0, 0) = 1.0;
  aMatrix.ChangeAt(0, 1) = 100.0;
  math_Vector aRhs(1, 1, 1.0);

  const MathSys::Utils::LinearStep aStep = MathSys::Utils::SolveLinearized(aMatrix, aRhs);
  ASSERT_TRUE(aStep.Solution.has_value());
  EXPECT_EQ(aStep.Rank, 1u);
  EXPECT_NEAR(aStep.Solution->At(0), 1.0 / 10001.0, 1.0e-10);
  EXPECT_NEAR(aStep.Solution->At(1), 100.0 / 10001.0, 1.0e-10);
}

TEST(MathSys_ComparisonTest, Newton_ExponentialCircleSystem)
{
  ExponentialCircleFunctionSet aFunction;
  math_Vector                  aStart(size_t{2});
  aStart.ChangeAt(0) = 0.5;
  aStart.ChangeAt(1) = 0.5;

  const MathSys::SystemResult aResult = MathSys::Newton(aFunction, aStart, 1.0e-12, 1.0e-12);
  ASSERT_EQ(aResult.Status, MathUtils::Status::OK);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(aResult.Solution->At(0), 0.0, 1.0e-10);
  EXPECT_NEAR(aResult.Solution->At(1), 1.0, 1.0e-10);
  EXPECT_LE(aResult.ResidualNorm, 1.0e-12);
  math_Vector aResidual(size_t{2});
  ASSERT_TRUE(aFunction.Value(*aResult.Solution, aResidual));
  EXPECT_NEAR(aResidual.At(0), 0.0, 1.0e-12);
  EXPECT_NEAR(aResidual.At(1), 0.0, 1.0e-12);
}

TEST(MathSys_ComparisonTest, NewtonBounded_TrigonometricProductSystem)
{
  TrigonometricProductFunctionSet aFunction;
  math_Vector                     aStart(size_t{2});
  math_Vector                     aLower(size_t{2}, 0.3);
  math_Vector                     anUpper(size_t{2}, 0.9);
  math_Vector                     aTolerance(size_t{2}, 1.0e-12);
  aStart.ChangeAt(0) = 0.7;
  aStart.ChangeAt(1) = 0.6;

  const MathSys::SystemResult aResult =
    MathSys::NewtonBounded(aFunction, aStart, aLower, anUpper, aTolerance, 1.0e-12);
  ASSERT_EQ(aResult.Status, MathUtils::Status::OK);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(aResult.Solution->At(0), 0.62151009999442219, 1.0e-10);
  EXPECT_NEAR(aResult.Solution->At(1), 0.64632535504472388, 1.0e-10);
  EXPECT_LE(aResult.ResidualNorm, 1.0e-12);
  math_Vector aResidual(size_t{2});
  ASSERT_TRUE(aFunction.Value(*aResult.Solution, aResidual));
  EXPECT_NEAR(aResidual.At(0), 0.0, 1.0e-12);
  EXPECT_NEAR(aResidual.At(1), 0.0, 1.0e-12);
}

TEST(MathSys_ComparisonTest, Newton_MixedSpaceSystem)
{
  MixedSpaceFunctionSet aFunction;
  math_Vector            aStart(size_t{3});
  aStart.ChangeAt(0) = 0.8;
  aStart.ChangeAt(1) = 1.0;
  aStart.ChangeAt(2) = 0.7;

  const MathSys::SystemResult aResult = MathSys::Newton(aFunction, aStart, 1.0e-12, 1.0e-12);
  ASSERT_EQ(aResult.Status, MathUtils::Status::OK);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(aResult.Solution->At(0), 0.96889156551226008, 1.0e-10);
  EXPECT_NEAR(aResult.Solution->At(1), 0.90960651426100725, 1.0e-10);
  EXPECT_NEAR(aResult.Solution->At(2), 0.65164262001819440, 1.0e-10);
  EXPECT_LE(aResult.ResidualNorm, 1.0e-12);
  math_Vector aResidual(size_t{3});
  ASSERT_TRUE(aFunction.Value(*aResult.Solution, aResidual));
  for (size_t anIndex = 0; anIndex < aResidual.Size(); ++anIndex)
  {
    EXPECT_NEAR(aResidual.At(anIndex), 0.0, 1.0e-12);
  }
}

TEST(MathSys_ComparisonTest, Newton_CubicSpaceSystem)
{
  CubicSpaceFunctionSet aFunction;
  math_Vector            aStart(size_t{3}, 0.8);

  const MathSys::SystemResult aResult = MathSys::Newton(aFunction, aStart, 1.0e-12, 1.0e-12);
  ASSERT_EQ(aResult.Status, MathUtils::Status::OK);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(aResult.Solution->At(0), 0.85234294426009105, 1.0e-10);
  EXPECT_NEAR(aResult.Solution->At(1), 0.83137580328938510, 1.0e-10);
  EXPECT_NEAR(aResult.Solution->At(2), 0.88959693122094752, 1.0e-10);
  EXPECT_LE(aResult.ResidualNorm, 1.0e-12);
  math_Vector aResidual(size_t{3});
  ASSERT_TRUE(aFunction.Value(*aResult.Solution, aResidual));
  for (size_t anIndex = 0; anIndex < aResidual.Size(); ++anIndex)
  {
    EXPECT_NEAR(aResidual.At(anIndex), 0.0, 1.0e-12);
  }
}

TEST(MathSys_ComparisonTest, Newton_OverdeterminedTranscendentalSystem)
{
  OverdeterminedTranscendentalFunctionSet aFunction;
  math_Vector                             aStart(size_t{2});
  aStart.ChangeAt(0) = 0.4;
  aStart.ChangeAt(1) = 0.9;

  const MathSys::SystemResult aResult = MathSys::Newton(aFunction, aStart, 1.0e-12, 1.0e-12);
  ASSERT_EQ(aResult.Status, MathUtils::Status::OK);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_EQ(aResult.Rank, 2u);
  EXPECT_NEAR(aResult.Solution->At(0), 0.6, 1.0e-10);
  EXPECT_NEAR(aResult.Solution->At(1), 0.8, 1.0e-10);
  EXPECT_LE(aResult.ResidualNorm, 1.0e-12);
  math_Vector aResidual(size_t{3});
  ASSERT_TRUE(aFunction.Value(*aResult.Solution, aResidual));
  for (size_t anIndex = 0; anIndex < aResidual.Size(); ++anIndex)
  {
    EXPECT_NEAR(aResidual.At(anIndex), 0.0, 1.0e-12);
  }
}

TEST(MathSys_ComparisonTest, NewtonBounded_LogarithmicSystem)
{
  BoundedLogarithmicFunctionSet aFunction;
  math_Vector                   aStart(size_t{2});
  math_Vector                   aLower(size_t{2});
  math_Vector                   anUpper(size_t{2});
  math_Vector                   aTolerance(size_t{2}, 1.0e-12);
  aStart.ChangeAt(0) = 0.4;
  aStart.ChangeAt(1) = 1.2;
  aLower.ChangeAt(0) = 0.01;
  aLower.ChangeAt(1) = 0.8;
  anUpper.ChangeAt(0) = 0.8;
  anUpper.ChangeAt(1) = 1.4;

  const MathSys::SystemResult aResult =
    MathSys::NewtonBounded(aFunction, aStart, aLower, anUpper, aTolerance, 1.0e-12);
  ASSERT_EQ(aResult.Status, MathUtils::Status::OK);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(aResult.Solution->At(0), 0.055810767307769064, 1.0e-10);
  EXPECT_NEAR(aResult.Solution->At(1), 1.2023689235833408, 1.0e-10);
  EXPECT_LE(aResult.ResidualNorm, 1.0e-12);
  math_Vector aResidual(size_t{2});
  ASSERT_TRUE(aFunction.Value(*aResult.Solution, aResidual));
  EXPECT_NEAR(aResidual.At(0), 0.0, 1.0e-12);
  EXPECT_NEAR(aResidual.At(1), 0.0, 1.0e-12);
}
