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

#ifndef _MathUtils_FunctorVector_HeaderFile
#define _MathUtils_FunctorVector_HeaderFile

#include <math_Vector.hxx>
#include <math_Matrix.hxx>

#include <cmath>
#include <utility>

//! @file MathUtils_FunctorVector.hxx
//! @brief Non-virtual functor classes for N-dimensional (vector) functions.
//!
//! Provides ready-to-use functor classes that work with the template-based
//! math API (MathOpt::Powell, MathOpt::BFGS, MathSys::Newton) without
//! virtual dispatch overhead.

//! Core utilities for modern math solvers.
namespace MathUtils
{

//! Lambda wrapper for N-D objective functions (value only).
//! Wraps a lambda/callable into a functor with Value() method.
//!
//! Usage:
//! @code
//! auto aFunc = VectorLambda([](const math_Vector& x, double& y) {
//!   y = x(1)*x(1) + x(2)*x(2);  // Sphere function
//!   return true;
//! });
//! auto aResult = MathOpt::Powell(aFunc, aStartPoint);
//! @endcode
//!
//! @tparam Lambda callable type with signature bool(const math_Vector&, double&)
template <typename Lambda>
class VectorLambda
{
public:
  //! Constructor from lambda/callable.
  //! @param theLambda callable with signature bool(const math_Vector&, double&)
  explicit VectorLambda(Lambda theLambda)
      : myLambda(std::move(theLambda))
  {
  }

  //! Evaluates the function at theX.
  //! @param[in] theX input vector
  //! @param[out] theY function value f(theX)
  //! @return true if evaluation succeeded
  bool Value(const math_Vector& theX, double& theY) const { return myLambda(theX, theY); }

private:
  Lambda myLambda;
};

//! Lambda wrapper for N-D objective functions with gradient.
//! Wraps a lambda/callable into a functor with Value() and Gradient() methods.
//!
//! Usage:
//! @code
//! auto aFunc = VectorLambdaWithGradient(
//!   [](const math_Vector& x, double& y) {
//!     y = x(1)*x(1) + x(2)*x(2);
//!     return true;
//!   },
//!   [](const math_Vector& x, math_Vector& g) {
//!     g(1) = 2.0 * x(1);
//!     g(2) = 2.0 * x(2);
//!     return true;
//!   });
//! auto aResult = MathOpt::BFGS(aFunc, aStartPoint);
//! @endcode
//!
//! @tparam ValueLambda callable for value: bool(const math_Vector&, double&)
//! @tparam GradLambda callable for gradient: bool(const math_Vector&, math_Vector&)
template <typename ValueLambda, typename GradLambda>
class VectorLambdaWithGradient
{
public:
  //! Constructor from value and gradient lambdas.
  //! @param theValueLambda callable for value evaluation
  //! @param theGradLambda callable for gradient evaluation
  VectorLambdaWithGradient(ValueLambda theValueLambda, GradLambda theGradLambda)
      : myValueLambda(std::move(theValueLambda)),
        myGradLambda(std::move(theGradLambda))
  {
  }

  //! Evaluates the function value at theX.
  //! @param[in] theX input vector
  //! @param[out] theY function value f(theX)
  //! @return true if evaluation succeeded
  bool Value(const math_Vector& theX, double& theY) const { return myValueLambda(theX, theY); }

  //! Evaluates the gradient at theX.
  //! @param[in] theX input vector
  //! @param[out] theG gradient vector
  //! @return true if evaluation succeeded
  bool Gradient(const math_Vector& theX, math_Vector& theG) const
  {
    return myGradLambda(theX, theG);
  }

  //! Evaluates both value and gradient at theX.
  //! @param[in] theX input vector
  //! @param[out] theY function value
  //! @param[out] theG gradient vector
  //! @return true if evaluation succeeded
  bool Values(const math_Vector& theX, double& theY, math_Vector& theG) const
  {
    return myValueLambda(theX, theY) && myGradLambda(theX, theG);
  }

private:
  ValueLambda myValueLambda;
  GradLambda  myGradLambda;
};

//! Helper function to create VectorLambda with type deduction.
//! @tparam Lambda callable type (auto-deduced)
//! @param theLambda callable with signature bool(const math_Vector&, double&)
//! @return VectorLambda wrapper
template <typename Lambda>
VectorLambda<Lambda> MakeVector(Lambda theLambda)
{
  return VectorLambda<Lambda>(std::move(theLambda));
}

//! Helper function to create VectorLambdaWithGradient with type deduction.
//! @tparam ValueLambda callable for value
//! @tparam GradLambda callable for gradient
//! @param theValueLambda callable for value evaluation
//! @param theGradLambda callable for gradient evaluation
//! @return VectorLambdaWithGradient wrapper
template <typename ValueLambda, typename GradLambda>
VectorLambdaWithGradient<ValueLambda, GradLambda> MakeVectorWithGradient(ValueLambda theValueLambda,
                                                                         GradLambda  theGradLambda)
{
  return VectorLambdaWithGradient<ValueLambda, GradLambda>(std::move(theValueLambda),
                                                           std::move(theGradLambda));
}

//! Quadratic form functor: f(x) = x^T A x + b^T x + c.
//! Commonly used for testing optimization algorithms.
//!
//! Usage:
//! @code
//! math_Matrix A(1, 2, 1, 2);
//! A(1,1) = 2.0; A(1,2) = 0.0;
//! A(2,1) = 0.0; A(2,2) = 2.0;
//! math_Vector b(1, 2);
//! b(1) = -4.0; b(2) = -4.0;
//! QuadraticForm aFunc(A, b, 8.0);  // f(x) = 2*x1^2 + 2*x2^2 - 4*x1 - 4*x2 + 8
//! // Minimum at (1, 1) with value 4
//! @endcode
class QuadraticForm
{
public:
  //! Constructor from matrix, vector, and constant.
  //! @param theA quadratic coefficient matrix (must be square)
  //! @param theB linear coefficient vector
  //! @param theC constant term
  QuadraticForm(const math_Matrix& theA, const math_Vector& theB, double theC)
      : myA(theA),
        myB(theB),
        myC(theC)
  {
  }

  //! Evaluates the quadratic form f(x) = x^T A x + b^T x + c.
  //! @param[in] theX input vector
  //! @param[out] theY function value
  //! @return true if evaluation succeeded
  bool Value(const math_Vector& theX, double& theY) const
  {
    theY = myC;
    // x^T A x
    for (int i = theX.Lower(); i <= theX.Upper(); ++i)
    {
      for (int j = theX.Lower(); j <= theX.Upper(); ++j)
      {
        theY += theX(i) * myA(i, j) * theX(j);
      }
    }
    // b^T x
    for (int i = theX.Lower(); i <= theX.Upper(); ++i)
    {
      theY += myB(i) * theX(i);
    }
    return true;
  }

  //! Evaluates the gradient: g = 2*A*x + b.
  //! @param[in] theX input vector
  //! @param[out] theG gradient vector
  //! @return true if evaluation succeeded
  bool Gradient(const math_Vector& theX, math_Vector& theG) const
  {
    // g = (A + A^T) * x + b = 2*A*x + b (for symmetric A)
    for (int i = theX.Lower(); i <= theX.Upper(); ++i)
    {
      theG(i) = myB(i);
      for (int j = theX.Lower(); j <= theX.Upper(); ++j)
      {
        theG(i) += (myA(i, j) + myA(j, i)) * theX(j);
      }
    }
    return true;
  }

  //! Evaluates both value and gradient.
  //! @param[in] theX input vector
  //! @param[out] theY function value
  //! @param[out] theG gradient vector
  //! @return true if evaluation succeeded
  bool Values(const math_Vector& theX, double& theY, math_Vector& theG) const
  {
    return Value(theX, theY) && Gradient(theX, theG);
  }

private:
  math_Matrix myA;
  math_Vector myB;
  double      myC;
};

//! Rosenbrock function functor (for testing optimization).
//! f(x,y) = (a - x)^2 + b*(y - x^2)^2
//! Default: a = 1, b = 100
//! Global minimum at (a, a^2) = (1, 1) with f = 0.
//!
//! Usage:
//! @code
//! Rosenbrock aRosen;  // Default a=1, b=100
//! math_Vector aStart(1, 2);
//! aStart(1) = -1.0; aStart(2) = 1.0;
//! auto aResult = MathOpt::BFGS(aRosen, aStart);
//! // Should converge to (1, 1)
//! @endcode
class Rosenbrock
{
public:
  //! Constructor with parameters.
  //! @param theA parameter a (default 1.0)
  //! @param theB parameter b (default 100.0)
  Rosenbrock(double theA = 1.0, double theB = 100.0)
      : myA(theA),
        myB(theB)
  {
  }

  //! Evaluates the Rosenbrock function.
  //! @param[in] theX input vector (must have length 2)
  //! @param[out] theY function value
  //! @return true if evaluation succeeded
  bool Value(const math_Vector& theX, double& theY) const
  {
    const double x  = theX(theX.Lower());
    const double y  = theX(theX.Lower() + 1);
    const double t1 = myA - x;
    const double t2 = y - x * x;
    theY            = t1 * t1 + myB * t2 * t2;
    return true;
  }

  //! Evaluates the gradient of the Rosenbrock function.
  //! @param[in] theX input vector
  //! @param[out] theG gradient vector
  //! @return true if evaluation succeeded
  bool Gradient(const math_Vector& theX, math_Vector& theG) const
  {
    const double x         = theX(theX.Lower());
    const double y         = theX(theX.Lower() + 1);
    const double t2        = y - x * x;
    theG(theG.Lower())     = -2.0 * (myA - x) - 4.0 * myB * x * t2;
    theG(theG.Lower() + 1) = 2.0 * myB * t2;
    return true;
  }

  //! Evaluates both value and gradient.
  //! @param[in] theX input vector
  //! @param[out] theY function value
  //! @param[out] theG gradient vector
  //! @return true if evaluation succeeded
  bool Values(const math_Vector& theX, double& theY, math_Vector& theG) const
  {
    return Value(theX, theY) && Gradient(theX, theG);
  }

private:
  double myA;
  double myB;
};

//! Sphere function functor (for testing optimization).
//! f(x) = sum(x[i]^2) for all i.
//! Global minimum at origin with f = 0.
//!
//! Usage:
//! @code
//! Sphere aSphere;
//! math_Vector aStart(1, 3);
//! aStart.Init(1.0);  // Start at (1, 1, 1)
//! auto aResult = MathOpt::Powell(aSphere, aStart);
//! // Should converge to (0, 0, 0)
//! @endcode
class Sphere
{
public:
  //! Evaluates the sphere function.
  //! @param[in] theX input vector
  //! @param[out] theY function value
  //! @return true (always succeeds)
  bool Value(const math_Vector& theX, double& theY) const
  {
    theY = 0.0;
    for (int i = theX.Lower(); i <= theX.Upper(); ++i)
    {
      theY += theX(i) * theX(i);
    }
    return true;
  }

  //! Evaluates the gradient of the sphere function.
  //! @param[in] theX input vector
  //! @param[out] theG gradient vector
  //! @return true (always succeeds)
  bool Gradient(const math_Vector& theX, math_Vector& theG) const
  {
    for (int i = theX.Lower(); i <= theX.Upper(); ++i)
    {
      theG(i) = 2.0 * theX(i);
    }
    return true;
  }

  //! Evaluates both value and gradient.
  //! @param[in] theX input vector
  //! @param[out] theY function value
  //! @param[out] theG gradient vector
  //! @return true (always succeeds)
  bool Values(const math_Vector& theX, double& theY, math_Vector& theG) const
  {
    return Value(theX, theY) && Gradient(theX, theG);
  }
};

//! Booth function functor (for testing optimization).
//! f(x,y) = (x + 2y - 7)^2 + (2x + y - 5)^2
//! Global minimum at (1, 3) with f = 0.
//!
//! Usage:
//! @code
//! Booth aBooth;
//! math_Vector aStart(1, 2);
//! aStart(1) = 0.0; aStart(2) = 0.0;
//! auto aResult = MathOpt::BFGS(aBooth, aStart);
//! // Should converge to (1, 3)
//! @endcode
class Booth
{
public:
  //! Evaluates the Booth function.
  //! @param[in] theX input vector (must have length 2)
  //! @param[out] theY function value
  //! @return true (always succeeds)
  bool Value(const math_Vector& theX, double& theY) const
  {
    const double x  = theX(theX.Lower());
    const double y  = theX(theX.Lower() + 1);
    const double t1 = x + 2.0 * y - 7.0;
    const double t2 = 2.0 * x + y - 5.0;
    theY            = t1 * t1 + t2 * t2;
    return true;
  }

  //! Evaluates the gradient of the Booth function.
  //! @param[in] theX input vector
  //! @param[out] theG gradient vector
  //! @return true (always succeeds)
  bool Gradient(const math_Vector& theX, math_Vector& theG) const
  {
    const double x         = theX(theX.Lower());
    const double y         = theX(theX.Lower() + 1);
    const double t1        = x + 2.0 * y - 7.0;
    const double t2        = 2.0 * x + y - 5.0;
    theG(theG.Lower())     = 2.0 * t1 + 4.0 * t2;
    theG(theG.Lower() + 1) = 4.0 * t1 + 2.0 * t2;
    return true;
  }

  //! Evaluates both value and gradient.
  //! @param[in] theX input vector
  //! @param[out] theY function value
  //! @param[out] theG gradient vector
  //! @return true (always succeeds)
  bool Values(const math_Vector& theX, double& theY, math_Vector& theG) const
  {
    return Value(theX, theY) && Gradient(theX, theG);
  }
};

//! Beale function functor (for testing optimization).
//! f(x,y) = (1.5 - x + xy)^2 + (2.25 - x + xy^2)^2 + (2.625 - x + xy^3)^2
//! Global minimum at (3, 0.5) with f = 0.
class Beale
{
public:
  //! Evaluates the Beale function.
  //! @param[in] theX input vector (must have length 2)
  //! @param[out] theY function value
  //! @return true (always succeeds)
  bool Value(const math_Vector& theX, double& theY) const
  {
    const double x  = theX(theX.Lower());
    const double y  = theX(theX.Lower() + 1);
    const double t1 = 1.5 - x + x * y;
    const double t2 = 2.25 - x + x * y * y;
    const double t3 = 2.625 - x + x * y * y * y;
    theY            = t1 * t1 + t2 * t2 + t3 * t3;
    return true;
  }

  //! Evaluates the gradient of the Beale function.
  //! @param[in] theX input vector
  //! @param[out] theG gradient vector
  //! @return true (always succeeds)
  bool Gradient(const math_Vector& theX, math_Vector& theG) const
  {
    const double x         = theX(theX.Lower());
    const double y         = theX(theX.Lower() + 1);
    const double y2        = y * y;
    const double y3        = y2 * y;
    const double t1        = 1.5 - x + x * y;
    const double t2        = 2.25 - x + x * y2;
    const double t3        = 2.625 - x + x * y3;
    theG(theG.Lower())     = 2.0 * ((y - 1.0) * t1 + (y2 - 1.0) * t2 + (y3 - 1.0) * t3);
    theG(theG.Lower() + 1) = 2.0 * x * (t1 + 2.0 * y * t2 + 3.0 * y2 * t3);
    return true;
  }

  //! Evaluates both value and gradient.
  //! @param[in] theX input vector
  //! @param[out] theY function value
  //! @param[out] theG gradient vector
  //! @return true (always succeeds)
  bool Values(const math_Vector& theX, double& theY, math_Vector& theG) const
  {
    return Value(theX, theY) && Gradient(theX, theG);
  }
};

//! Himmelblau function functor (for testing optimization).
//! f(x,y) = (x^2 + y - 11)^2 + (x + y^2 - 7)^2
//! Has four local minima, all with f = 0:
//! (3.0, 2.0), (-2.805118, 3.131312), (-3.779310, -3.283186), (3.584428, -1.848126)
class Himmelblau
{
public:
  //! Evaluates the Himmelblau function.
  //! @param[in] theX input vector (must have length 2)
  //! @param[out] theY function value
  //! @return true (always succeeds)
  bool Value(const math_Vector& theX, double& theY) const
  {
    const double x  = theX(theX.Lower());
    const double y  = theX(theX.Lower() + 1);
    const double t1 = x * x + y - 11.0;
    const double t2 = x + y * y - 7.0;
    theY            = t1 * t1 + t2 * t2;
    return true;
  }

  //! Evaluates the gradient of the Himmelblau function.
  //! @param[in] theX input vector
  //! @param[out] theG gradient vector
  //! @return true (always succeeds)
  bool Gradient(const math_Vector& theX, math_Vector& theG) const
  {
    const double x         = theX(theX.Lower());
    const double y         = theX(theX.Lower() + 1);
    const double t1        = x * x + y - 11.0;
    const double t2        = x + y * y - 7.0;
    theG(theG.Lower())     = 4.0 * x * t1 + 2.0 * t2;
    theG(theG.Lower() + 1) = 2.0 * t1 + 4.0 * y * t2;
    return true;
  }

  //! Evaluates both value and gradient.
  //! @param[in] theX input vector
  //! @param[out] theY function value
  //! @param[out] theG gradient vector
  //! @return true (always succeeds)
  bool Values(const math_Vector& theX, double& theY, math_Vector& theG) const
  {
    return Value(theX, theY) && Gradient(theX, theG);
  }
};

//! Rastrigin function functor (for testing global optimization).
//! f(x) = A*n + sum(x[i]^2 - A*cos(2*pi*x[i])) for all i
//! Default: A = 10
//! Global minimum at origin with f = 0.
//! Highly multimodal - challenging for local optimizers.
class Rastrigin
{
public:
  //! Constructor with parameter.
  //! @param theA parameter A (default 10.0)
  explicit Rastrigin(double theA = 10.0)
      : myA(theA)
  {
  }

  //! Evaluates the Rastrigin function.
  //! @param[in] theX input vector
  //! @param[out] theY function value
  //! @return true (always succeeds)
  bool Value(const math_Vector& theX, double& theY) const
  {
    constexpr double aTwoPi = 2.0 * 3.14159265358979323846;
    const int        n      = theX.Upper() - theX.Lower() + 1;
    theY                    = myA * static_cast<double>(n);
    for (int i = theX.Lower(); i <= theX.Upper(); ++i)
    {
      theY += theX(i) * theX(i) - myA * std::cos(aTwoPi * theX(i));
    }
    return true;
  }

  //! Evaluates the gradient of the Rastrigin function.
  //! @param[in] theX input vector
  //! @param[out] theG gradient vector
  //! @return true (always succeeds)
  bool Gradient(const math_Vector& theX, math_Vector& theG) const
  {
    constexpr double aTwoPi = 2.0 * 3.14159265358979323846;
    for (int i = theX.Lower(); i <= theX.Upper(); ++i)
    {
      theG(i) = 2.0 * theX(i) + myA * aTwoPi * std::sin(aTwoPi * theX(i));
    }
    return true;
  }

  //! Evaluates both value and gradient.
  //! @param[in] theX input vector
  //! @param[out] theY function value
  //! @param[out] theG gradient vector
  //! @return true (always succeeds)
  bool Values(const math_Vector& theX, double& theY, math_Vector& theG) const
  {
    return Value(theX, theY) && Gradient(theX, theG);
  }

private:
  double myA;
};

//! Ackley function functor (for testing global optimization).
//! f(x) = -a*exp(-b*sqrt(sum(x[i]^2)/n)) - exp(sum(cos(c*x[i]))/n) + a + e
//! Default: a = 20, b = 0.2, c = 2*pi
//! Global minimum at origin with f = 0.
class Ackley
{
public:
  //! Constructor with parameters.
  //! @param theA parameter a (default 20.0)
  //! @param theB parameter b (default 0.2)
  //! @param theC parameter c (default 2*pi)
  Ackley(double theA = 20.0, double theB = 0.2, double theC = 2.0 * 3.14159265358979323846)
      : myA(theA),
        myB(theB),
        myC(theC)
  {
  }

  //! Evaluates the Ackley function.
  //! @param[in] theX input vector
  //! @param[out] theY function value
  //! @return true (always succeeds)
  bool Value(const math_Vector& theX, double& theY) const
  {
    constexpr double aE      = 2.718281828459045;
    const int        n       = theX.Upper() - theX.Lower() + 1;
    double           aSumSq  = 0.0;
    double           aSumCos = 0.0;
    for (int i = theX.Lower(); i <= theX.Upper(); ++i)
    {
      aSumSq += theX(i) * theX(i);
      aSumCos += std::cos(myC * theX(i));
    }
    theY = -myA * std::exp(-myB * std::sqrt(aSumSq / n)) - std::exp(aSumCos / n) + myA + aE;
    return true;
  }

private:
  double myA;
  double myB;
  double myC;
};

//! Linear system residual functor: f(x) = ||Ax - b||^2.
//! Useful for solving overdetermined linear systems via optimization.
//!
//! Usage:
//! @code
//! math_Matrix A(1, 3, 1, 2);  // 3x2 overdetermined system
//! math_Vector b(1, 3);
//! // ... fill A and b ...
//! LinearResidual aRes(A, b);
//! math_Vector aStart(1, 2);
//! aStart.Init(0.0);
//! auto aResult = MathOpt::BFGS(aRes, aStart);
//! @endcode
class LinearResidual
{
public:
  //! Constructor from matrix and right-hand side.
  //! @param theA coefficient matrix (m x n)
  //! @param theB right-hand side vector (m)
  LinearResidual(const math_Matrix& theA, const math_Vector& theB)
      : myA(theA),
        myB(theB)
  {
  }

  //! Evaluates the residual ||Ax - b||^2.
  //! @param[in] theX solution vector (n)
  //! @param[out] theY squared residual norm
  //! @return true (always succeeds)
  bool Value(const math_Vector& theX, double& theY) const
  {
    theY = 0.0;
    for (int i = myA.LowerRow(); i <= myA.UpperRow(); ++i)
    {
      double aResidual = -myB(i);
      for (int j = myA.LowerCol(); j <= myA.UpperCol(); ++j)
      {
        aResidual += myA(i, j) * theX(j);
      }
      theY += aResidual * aResidual;
    }
    return true;
  }

  //! Evaluates the gradient: g = 2 * A^T * (Ax - b).
  //! @param[in] theX solution vector
  //! @param[out] theG gradient vector
  //! @return true (always succeeds)
  bool Gradient(const math_Vector& theX, math_Vector& theG) const
  {
    // First compute residual r = Ax - b
    const int   m = myA.UpperRow() - myA.LowerRow() + 1;
    math_Vector aResidual(1, m);
    for (int i = myA.LowerRow(); i <= myA.UpperRow(); ++i)
    {
      aResidual(i - myA.LowerRow() + 1) = -myB(i);
      for (int j = myA.LowerCol(); j <= myA.UpperCol(); ++j)
      {
        aResidual(i - myA.LowerRow() + 1) += myA(i, j) * theX(j);
      }
    }
    // Then compute g = 2 * A^T * r
    for (int j = myA.LowerCol(); j <= myA.UpperCol(); ++j)
    {
      theG(j) = 0.0;
      for (int i = myA.LowerRow(); i <= myA.UpperRow(); ++i)
      {
        theG(j) += 2.0 * myA(i, j) * aResidual(i - myA.LowerRow() + 1);
      }
    }
    return true;
  }

  //! Evaluates both value and gradient.
  //! @param[in] theX solution vector
  //! @param[out] theY squared residual norm
  //! @param[out] theG gradient vector
  //! @return true (always succeeds)
  bool Values(const math_Vector& theX, double& theY, math_Vector& theG) const
  {
    return Value(theX, theY) && Gradient(theX, theG);
  }

private:
  math_Matrix myA;
  math_Vector myB;
};

//! Nonlinear system functor: F(x) = [f1(x), f2(x), ..., fn(x)].
//! Lambda wrapper for systems of nonlinear equations.
//!
//! Usage:
//! @code
//! auto aSys = SystemLambda([](const math_Vector& x, math_Vector& f) {
//!   f(1) = x(1)*x(1) + x(2)*x(2) - 1.0;  // Circle
//!   f(2) = x(1) - x(2);                   // Line y=x
//!   return true;
//! });
//! auto aResult = MathSys::Newton(aSys, aStartPoint);
//! @endcode
//!
//! @tparam Lambda callable with signature bool(const math_Vector&, math_Vector&)
template <typename Lambda>
class SystemLambda
{
public:
  //! Constructor from lambda.
  //! @param theLambda callable for system evaluation
  //! @param theNbEquations number of equations in the system
  SystemLambda(Lambda theLambda, int theNbEquations)
      : myLambda(std::move(theLambda)),
        myNbEquations(theNbEquations)
  {
  }

  //! Returns the number of equations.
  //! @return number of equations
  int NbEquations() const { return myNbEquations; }

  //! Evaluates the system F(x).
  //! @param[in] theX input vector
  //! @param[out] theF system values
  //! @return true if evaluation succeeded
  bool Value(const math_Vector& theX, math_Vector& theF) const { return myLambda(theX, theF); }

private:
  Lambda myLambda;
  int    myNbEquations;
};

//! Helper function to create SystemLambda with type deduction.
//! @tparam Lambda callable type
//! @param theLambda callable for system evaluation
//! @param theNbEquations number of equations
//! @return SystemLambda wrapper
template <typename Lambda>
SystemLambda<Lambda> MakeSystem(Lambda theLambda, int theNbEquations)
{
  return SystemLambda<Lambda>(std::move(theLambda), theNbEquations);
}

} // namespace MathUtils

#endif // _MathUtils_FunctorVector_HeaderFile
