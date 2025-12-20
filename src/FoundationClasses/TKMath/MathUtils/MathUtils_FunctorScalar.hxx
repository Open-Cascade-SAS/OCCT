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

#ifndef _MathUtils_FunctorScalar_HeaderFile
#define _MathUtils_FunctorScalar_HeaderFile

#include <math_Vector.hxx>

#include <cmath>
#include <initializer_list>
#include <utility>

//! @file MathUtils_FunctorScalar.hxx
//! @brief Non-virtual functor classes for scalar (1D) functions.
//!
//! Provides ready-to-use functor classes that work with the template-based
//! math API (MathRoot, MathOpt, MathInteg) without virtual dispatch overhead.

//! Core utilities for modern math solvers.
namespace MathUtils
{

//! Lambda wrapper for scalar functions with value only.
//! Wraps a lambda/callable into a functor with Value() method.
//!
//! Usage:
//! @code
//! auto aFunc = ScalarLambda([](double x, double& y) {
//!   y = x * x - 2.0;
//!   return true;
//! });
//! auto aResult = MathRoot::Brent(aFunc, 0.0, 2.0);
//! @endcode
//!
//! @tparam Lambda callable type with signature bool(double, double&)
template <typename Lambda>
class ScalarLambda
{
public:
  //! Constructor from lambda/callable.
  //! @param theLambda callable with signature bool(double theX, double& theY)
  explicit ScalarLambda(Lambda theLambda)
    : myLambda(std::move(theLambda))
  {
  }

  //! Evaluates the function at theX.
  //! @param[in] theX input value
  //! @param[out] theY function value f(theX)
  //! @return true if evaluation succeeded
  bool Value(double theX, double& theY) const { return myLambda(theX, theY); }

private:
  Lambda myLambda;
};

//! Lambda wrapper for scalar functions with value and derivative.
//! Wraps a lambda/callable into a functor with Values() method.
//!
//! Usage:
//! @code
//! auto aFunc = ScalarLambdaWithDerivative([](double x, double& y, double& dy) {
//!   y = x * x - 2.0;
//!   dy = 2.0 * x;
//!   return true;
//! });
//! auto aResult = MathRoot::Newton(aFunc, 1.0);
//! @endcode
//!
//! @tparam Lambda callable type with signature bool(double, double&, double&)
template <typename Lambda>
class ScalarLambdaWithDerivative
{
public:
  //! Constructor from lambda/callable.
  //! @param theLambda callable with signature bool(double theX, double& theY, double& theDY)
  explicit ScalarLambdaWithDerivative(Lambda theLambda)
    : myLambda(std::move(theLambda))
  {
  }

  //! Evaluates function and derivative at theX.
  //! @param[in] theX input value
  //! @param[out] theY function value f(theX)
  //! @param[out] theDY derivative value f'(theX)
  //! @return true if evaluation succeeded
  bool Values(double theX, double& theY, double& theDY) const { return myLambda(theX, theY, theDY); }

  //! Evaluates only the function value (for algorithms that don't need derivative).
  //! @param[in] theX input value
  //! @param[out] theY function value f(theX)
  //! @return true if evaluation succeeded
  bool Value(double theX, double& theY) const
  {
    double aDummy = 0.0;
    return myLambda(theX, theY, aDummy);
  }

private:
  Lambda myLambda;
};

//! Helper function to create ScalarLambda with type deduction.
//! @tparam Lambda callable type (auto-deduced)
//! @param theLambda callable with signature bool(double, double&)
//! @return ScalarLambda wrapper
template <typename Lambda>
ScalarLambda<Lambda> MakeScalar(Lambda theLambda)
{
  return ScalarLambda<Lambda>(std::move(theLambda));
}

//! Helper function to create ScalarLambdaWithDerivative with type deduction.
//! @tparam Lambda callable type (auto-deduced)
//! @param theLambda callable with signature bool(double, double&, double&)
//! @return ScalarLambdaWithDerivative wrapper
template <typename Lambda>
ScalarLambdaWithDerivative<Lambda> MakeScalarWithDerivative(Lambda theLambda)
{
  return ScalarLambdaWithDerivative<Lambda>(std::move(theLambda));
}

//! Polynomial functor: f(x) = sum(a[i] * x^i).
//! Coefficients are stored in order: a[0] + a[1]*x + a[2]*x^2 + ...
//!
//! Usage:
//! @code
//! // x^2 - 2 (find sqrt(2))
//! Polynomial aPoly({-2.0, 0.0, 1.0});
//!
//! // x^3 - 6x^2 + 11x - 6 = (x-1)(x-2)(x-3)
//! Polynomial aCubic({-6.0, 11.0, -6.0, 1.0});
//! @endcode
class Polynomial
{
public:
  //! Constructor from initializer list.
  //! @param theCoeffs coefficients in ascending power order
  Polynomial(std::initializer_list<double> theCoeffs)
    : myCoeffs(0, static_cast<int>(theCoeffs.size()) - 1)
  {
    int anIdx = 0;
    for (double aCoeff : theCoeffs)
    {
      myCoeffs(anIdx++) = aCoeff;
    }
  }

  //! Constructor from math_Vector.
  //! @param theCoeffs coefficients in ascending power order
  explicit Polynomial(const math_Vector& theCoeffs)
    : myCoeffs(theCoeffs)
  {
  }

  //! Evaluates polynomial at theX using Horner's method.
  //! @param[in] theX input value
  //! @param[out] theY polynomial value p(theX)
  //! @return true (always succeeds for polynomials)
  bool Value(double theX, double& theY) const
  {
    if (myCoeffs.Length() <= 0)
    {
      theY = 0.0;
      return true;
    }

    // Horner's method: p(x) = a[0] + x*(a[1] + x*(a[2] + ...))
    const int aLast = myCoeffs.Upper();
    theY = myCoeffs(aLast);
    for (int i = aLast - 1; i >= myCoeffs.Lower(); --i)
    {
      theY = theY * theX + myCoeffs(i);
    }
    return true;
  }

  //! Evaluates polynomial and its derivative at theX.
  //! @param[in] theX input value
  //! @param[out] theY polynomial value p(theX)
  //! @param[out] theDY derivative value p'(theX)
  //! @return true (always succeeds for polynomials)
  bool Values(double theX, double& theY, double& theDY) const
  {
    if (myCoeffs.Length() <= 0)
    {
      theY  = 0.0;
      theDY = 0.0;
      return true;
    }

    const int n = myCoeffs.Length();
    if (n == 1)
    {
      theY  = myCoeffs(myCoeffs.Lower());
      theDY = 0.0;
      return true;
    }

    // Horner's method for value and derivative simultaneously
    const int aLower = myCoeffs.Lower();
    const int aLast  = myCoeffs.Upper();
    theY  = myCoeffs(aLast);
    theDY = 0.0;
    for (int i = aLast - 1; i >= aLower; --i)
    {
      theDY = theDY * theX + theY;
      theY  = theY * theX + myCoeffs(i);
    }
    return true;
  }

  //! Returns the degree of the polynomial.
  //! @return polynomial degree (number of coefficients - 1)
  int Degree() const { return myCoeffs.Length() <= 0 ? 0 : myCoeffs.Length() - 1; }

  //! Returns coefficient by index.
  //! @param theIndex coefficient index (0 = constant term)
  //! @return coefficient value
  double Coefficient(int theIndex) const
  {
    const int aIdx = myCoeffs.Lower() + theIndex;
    return (aIdx >= myCoeffs.Lower() && aIdx <= myCoeffs.Upper())
           ? myCoeffs(aIdx) : 0.0;
  }

private:
  math_Vector myCoeffs;
};

//! Rational function functor: f(x) = P(x) / Q(x).
//! Both numerator P and denominator Q are polynomials.
//!
//! Usage:
//! @code
//! // (x + 1) / (x^2 + 1)
//! Rational aRat({1.0, 1.0}, {1.0, 0.0, 1.0});
//! @endcode
class Rational
{
public:
  //! Constructor from math_Vector.
  //! @param theNum numerator coefficients (ascending power order)
  //! @param theDenom denominator coefficients (ascending power order)
  Rational(const math_Vector& theNum,
           const math_Vector& theDenom)
    : myNum(theNum)
    , myDenom(theDenom)
  {
  }

  //! Constructor from initializer lists.
  //! @param theNum numerator coefficients
  //! @param theDenom denominator coefficients
  Rational(std::initializer_list<double> theNum, std::initializer_list<double> theDenom)
    : myNum(0, static_cast<int>(theNum.size()) - 1)
    , myDenom(0, static_cast<int>(theDenom.size()) - 1)
  {
    int anIdx = 0;
    for (double aCoeff : theNum)
    {
      myNum(anIdx++) = aCoeff;
    }
    anIdx = 0;
    for (double aCoeff : theDenom)
    {
      myDenom(anIdx++) = aCoeff;
    }
  }

  //! Evaluates rational function at theX.
  //! @param[in] theX input value
  //! @param[out] theY function value P(theX)/Q(theX)
  //! @return false if denominator is zero
  bool Value(double theX, double& theY) const
  {
    double aNum = 0.0;
    double aDenom = 0.0;

    // Evaluate numerator using Horner's method
    if (myNum.Length() > 0)
    {
      const int aLast = myNum.Upper();
      aNum = myNum(aLast);
      for (int i = aLast - 1; i >= myNum.Lower(); --i)
      {
        aNum = aNum * theX + myNum(i);
      }
    }

    // Evaluate denominator using Horner's method
    if (myDenom.Length() > 0)
    {
      const int aLast = myDenom.Upper();
      aDenom = myDenom(aLast);
      for (int i = aLast - 1; i >= myDenom.Lower(); --i)
      {
        aDenom = aDenom * theX + myDenom(i);
      }
    }

    // Check for division by zero
    if (std::abs(aDenom) < 1e-15)
    {
      return false;
    }

    theY = aNum / aDenom;
    return true;
  }

private:
  math_Vector myNum;
  math_Vector myDenom;
};

//! Composite functor: f(g(x)).
//! Evaluates the outer function at the result of the inner function.
//!
//! Usage:
//! @code
//! Polynomial aInner({0.0, 1.0});         // g(x) = x
//! Polynomial aOuter({-1.0, 0.0, 1.0});   // f(x) = x^2 - 1
//! Composite aComp(aOuter, aInner);       // f(g(x)) = x^2 - 1
//! @endcode
//!
//! @tparam Outer outer function type (must have Value method)
//! @tparam Inner inner function type (must have Value method)
template <typename Outer, typename Inner>
class Composite
{
public:
  //! Constructor from outer and inner functions.
  //! @param theOuter outer function f
  //! @param theInner inner function g
  Composite(Outer theOuter, Inner theInner)
    : myOuter(std::move(theOuter))
    , myInner(std::move(theInner))
  {
  }

  //! Evaluates composite function f(g(theX)).
  //! @param[in] theX input value
  //! @param[out] theY function value f(g(theX))
  //! @return false if either evaluation fails
  bool Value(double theX, double& theY) const
  {
    double aInner = 0.0;
    if (!myInner.Value(theX, aInner))
    {
      return false;
    }
    return myOuter.Value(aInner, theY);
  }

private:
  Outer myOuter;
  Inner myInner;
};

//! Helper function to create Composite with type deduction.
//! @tparam Outer outer function type
//! @tparam Inner inner function type
//! @param theOuter outer function f
//! @param theInner inner function g
//! @return Composite functor representing f(g(x))
template <typename Outer, typename Inner>
Composite<Outer, Inner> MakeComposite(Outer theOuter, Inner theInner)
{
  return Composite<Outer, Inner>(std::move(theOuter), std::move(theInner));
}

//! Sum of functions functor: f(x) + g(x).
//!
//! Usage:
//! @code
//! Polynomial aF({1.0, 2.0});   // f(x) = 1 + 2x
//! Polynomial aG({3.0, 0.0, 1.0}); // g(x) = 3 + x^2
//! Sum aSum(aF, aG);            // h(x) = 4 + 2x + x^2
//! @endcode
//!
//! @tparam F first function type
//! @tparam G second function type
template <typename F, typename G>
class Sum
{
public:
  //! Constructor from two functions.
  //! @param theF first function
  //! @param theG second function
  Sum(F theF, G theG)
    : myF(std::move(theF))
    , myG(std::move(theG))
  {
  }

  //! Evaluates sum f(theX) + g(theX).
  //! @param[in] theX input value
  //! @param[out] theY sum value
  //! @return false if either evaluation fails
  bool Value(double theX, double& theY) const
  {
    double aF = 0.0;
    double aG = 0.0;
    if (!myF.Value(theX, aF))
    {
      return false;
    }
    if (!myG.Value(theX, aG))
    {
      return false;
    }
    theY = aF + aG;
    return true;
  }

private:
  F myF;
  G myG;
};

//! Helper function to create Sum with type deduction.
template <typename F, typename G>
Sum<F, G> MakeSum(F theF, G theG)
{
  return Sum<F, G>(std::move(theF), std::move(theG));
}

//! Difference of functions functor: f(x) - g(x).
//!
//! @tparam F first function type
//! @tparam G second function type
template <typename F, typename G>
class Difference
{
public:
  //! Constructor from two functions.
  //! @param theF first function (minuend)
  //! @param theG second function (subtrahend)
  Difference(F theF, G theG)
    : myF(std::move(theF))
    , myG(std::move(theG))
  {
  }

  //! Evaluates difference f(theX) - g(theX).
  //! @param[in] theX input value
  //! @param[out] theY difference value
  //! @return false if either evaluation fails
  bool Value(double theX, double& theY) const
  {
    double aF = 0.0;
    double aG = 0.0;
    if (!myF.Value(theX, aF))
    {
      return false;
    }
    if (!myG.Value(theX, aG))
    {
      return false;
    }
    theY = aF - aG;
    return true;
  }

private:
  F myF;
  G myG;
};

//! Helper function to create Difference with type deduction.
template <typename F, typename G>
Difference<F, G> MakeDifference(F theF, G theG)
{
  return Difference<F, G>(std::move(theF), std::move(theG));
}

//! Product of functions functor: f(x) * g(x).
//!
//! @tparam F first function type
//! @tparam G second function type
template <typename F, typename G>
class Product
{
public:
  //! Constructor from two functions.
  //! @param theF first function (multiplicand)
  //! @param theG second function (multiplier)
  Product(F theF, G theG)
    : myF(std::move(theF))
    , myG(std::move(theG))
  {
  }

  //! Evaluates product f(theX) * g(theX).
  //! @param[in] theX input value
  //! @param[out] theY product value
  //! @return false if either evaluation fails
  bool Value(double theX, double& theY) const
  {
    double aF = 0.0;
    double aG = 0.0;
    if (!myF.Value(theX, aF))
    {
      return false;
    }
    if (!myG.Value(theX, aG))
    {
      return false;
    }
    theY = aF * aG;
    return true;
  }

private:
  F myF;
  G myG;
};

//! Helper function to create Product with type deduction.
template <typename F, typename G>
Product<F, G> MakeProduct(F theF, G theG)
{
  return Product<F, G>(std::move(theF), std::move(theG));
}

//! Quotient of functions functor: f(x) / g(x).
//!
//! @tparam F numerator function type
//! @tparam G denominator function type
template <typename F, typename G>
class Quotient
{
public:
  //! Constructor from two functions.
  //! @param theF numerator function
  //! @param theG denominator function
  Quotient(F theF, G theG)
    : myF(std::move(theF))
    , myG(std::move(theG))
  {
  }

  //! Evaluates quotient f(theX) / g(theX).
  //! @param[in] theX input value
  //! @param[out] theY quotient value
  //! @return false if evaluation fails or denominator is zero
  bool Value(double theX, double& theY) const
  {
    double aF = 0.0;
    double aG = 0.0;
    if (!myF.Value(theX, aF))
    {
      return false;
    }
    if (!myG.Value(theX, aG))
    {
      return false;
    }
    if (std::abs(aG) < 1e-15)
    {
      return false;
    }
    theY = aF / aG;
    return true;
  }

private:
  F myF;
  G myG;
};

//! Helper function to create Quotient with type deduction.
template <typename F, typename G>
Quotient<F, G> MakeQuotient(F theF, G theG)
{
  return Quotient<F, G>(std::move(theF), std::move(theG));
}

//! Scaled function functor: c * f(x).
//!
//! @tparam F function type
template <typename F>
class Scaled
{
public:
  //! Constructor from function and scale factor.
  //! @param theF function to scale
  //! @param theScale scale factor
  Scaled(F theF, double theScale)
    : myF(std::move(theF))
    , myScale(theScale)
  {
  }

  //! Evaluates scaled function c * f(theX).
  //! @param[in] theX input value
  //! @param[out] theY scaled value
  //! @return false if evaluation fails
  bool Value(double theX, double& theY) const
  {
    double aF = 0.0;
    if (!myF.Value(theX, aF))
    {
      return false;
    }
    theY = myScale * aF;
    return true;
  }

private:
  F      myF;
  double myScale;
};

//! Helper function to create Scaled with type deduction.
template <typename F>
Scaled<F> MakeScaled(F theF, double theScale)
{
  return Scaled<F>(std::move(theF), theScale);
}

//! Shifted function functor: f(x) + c.
//!
//! @tparam F function type
template <typename F>
class Shifted
{
public:
  //! Constructor from function and shift value.
  //! @param theF function to shift
  //! @param theShift shift value (added to output)
  Shifted(F theF, double theShift)
    : myF(std::move(theF))
    , myShift(theShift)
  {
  }

  //! Evaluates shifted function f(theX) + c.
  //! @param[in] theX input value
  //! @param[out] theY shifted value
  //! @return false if evaluation fails
  bool Value(double theX, double& theY) const
  {
    double aF = 0.0;
    if (!myF.Value(theX, aF))
    {
      return false;
    }
    theY = aF + myShift;
    return true;
  }

private:
  F      myF;
  double myShift;
};

//! Helper function to create Shifted with type deduction.
template <typename F>
Shifted<F> MakeShifted(F theF, double theShift)
{
  return Shifted<F>(std::move(theF), theShift);
}

//! Negated function functor: -f(x).
//!
//! @tparam F function type
template <typename F>
class Negated
{
public:
  //! Constructor from function.
  //! @param theF function to negate
  explicit Negated(F theF)
    : myF(std::move(theF))
  {
  }

  //! Evaluates negated function -f(theX).
  //! @param[in] theX input value
  //! @param[out] theY negated value
  //! @return false if evaluation fails
  bool Value(double theX, double& theY) const
  {
    double aF = 0.0;
    if (!myF.Value(theX, aF))
    {
      return false;
    }
    theY = -aF;
    return true;
  }

private:
  F myF;
};

//! Helper function to create Negated with type deduction.
template <typename F>
Negated<F> MakeNegated(F theF)
{
  return Negated<F>(std::move(theF));
}

//! Constant function functor: f(x) = c.
class Constant
{
public:
  //! Constructor from constant value.
  //! @param theValue constant value
  explicit Constant(double theValue)
    : myValue(theValue)
  {
  }

  //! Evaluates constant function.
  //! @param[in] theX input value (ignored)
  //! @param[out] theY constant value
  //! @return true (always succeeds)
  bool Value(double /*theX*/, double& theY) const
  {
    theY = myValue;
    return true;
  }

  //! Evaluates constant and derivative (derivative is always 0).
  //! @param[in] theX input value (ignored)
  //! @param[out] theY constant value
  //! @param[out] theDY derivative (always 0)
  //! @return true (always succeeds)
  bool Values(double /*theX*/, double& theY, double& theDY) const
  {
    theY  = myValue;
    theDY = 0.0;
    return true;
  }

private:
  double myValue;
};

//! Linear function functor: f(x) = a*x + b.
class Linear
{
public:
  //! Constructor from slope and intercept.
  //! @param theSlope coefficient a (slope)
  //! @param theIntercept coefficient b (y-intercept)
  Linear(double theSlope, double theIntercept)
    : mySlope(theSlope)
    , myIntercept(theIntercept)
  {
  }

  //! Evaluates linear function a*x + b.
  //! @param[in] theX input value
  //! @param[out] theY function value
  //! @return true (always succeeds)
  bool Value(double theX, double& theY) const
  {
    theY = mySlope * theX + myIntercept;
    return true;
  }

  //! Evaluates linear function and derivative.
  //! @param[in] theX input value
  //! @param[out] theY function value
  //! @param[out] theDY derivative (= slope)
  //! @return true (always succeeds)
  bool Values(double theX, double& theY, double& theDY) const
  {
    theY  = mySlope * theX + myIntercept;
    theDY = mySlope;
    return true;
  }

private:
  double mySlope;
  double myIntercept;
};

//! Sine function functor: f(x) = a * sin(b*x + c) + d.
class Sine
{
public:
  //! Constructor with full parameters.
  //! @param theAmplitude amplitude a
  //! @param theFrequency angular frequency b
  //! @param thePhase phase shift c
  //! @param theOffset vertical offset d
  Sine(double theAmplitude = 1.0,
       double theFrequency = 1.0,
       double thePhase     = 0.0,
       double theOffset    = 0.0)
    : myAmplitude(theAmplitude)
    , myFrequency(theFrequency)
    , myPhase(thePhase)
    , myOffset(theOffset)
  {
  }

  //! Evaluates sine function.
  //! @param[in] theX input value
  //! @param[out] theY function value
  //! @return true (always succeeds)
  bool Value(double theX, double& theY) const
  {
    theY = myAmplitude * std::sin(myFrequency * theX + myPhase) + myOffset;
    return true;
  }

  //! Evaluates sine function and derivative.
  //! @param[in] theX input value
  //! @param[out] theY function value
  //! @param[out] theDY derivative value
  //! @return true (always succeeds)
  bool Values(double theX, double& theY, double& theDY) const
  {
    const double anArg = myFrequency * theX + myPhase;
    theY               = myAmplitude * std::sin(anArg) + myOffset;
    theDY              = myAmplitude * myFrequency * std::cos(anArg);
    return true;
  }

private:
  double myAmplitude;
  double myFrequency;
  double myPhase;
  double myOffset;
};

//! Cosine function functor: f(x) = a * cos(b*x + c) + d.
class Cosine
{
public:
  //! Constructor with full parameters.
  //! @param theAmplitude amplitude a
  //! @param theFrequency angular frequency b
  //! @param thePhase phase shift c
  //! @param theOffset vertical offset d
  Cosine(double theAmplitude = 1.0,
         double theFrequency = 1.0,
         double thePhase     = 0.0,
         double theOffset    = 0.0)
    : myAmplitude(theAmplitude)
    , myFrequency(theFrequency)
    , myPhase(thePhase)
    , myOffset(theOffset)
  {
  }

  //! Evaluates cosine function.
  //! @param[in] theX input value
  //! @param[out] theY function value
  //! @return true (always succeeds)
  bool Value(double theX, double& theY) const
  {
    theY = myAmplitude * std::cos(myFrequency * theX + myPhase) + myOffset;
    return true;
  }

  //! Evaluates cosine function and derivative.
  //! @param[in] theX input value
  //! @param[out] theY function value
  //! @param[out] theDY derivative value
  //! @return true (always succeeds)
  bool Values(double theX, double& theY, double& theDY) const
  {
    const double anArg = myFrequency * theX + myPhase;
    theY               = myAmplitude * std::cos(anArg) + myOffset;
    theDY              = -myAmplitude * myFrequency * std::sin(anArg);
    return true;
  }

private:
  double myAmplitude;
  double myFrequency;
  double myPhase;
  double myOffset;
};

//! Exponential function functor: f(x) = a * exp(b*x) + c.
class Exponential
{
public:
  //! Constructor with full parameters.
  //! @param theScale scale factor a
  //! @param theRate rate b
  //! @param theOffset vertical offset c
  Exponential(double theScale = 1.0, double theRate = 1.0, double theOffset = 0.0)
    : myScale(theScale)
    , myRate(theRate)
    , myOffset(theOffset)
  {
  }

  //! Evaluates exponential function.
  //! @param[in] theX input value
  //! @param[out] theY function value
  //! @return true (always succeeds)
  bool Value(double theX, double& theY) const
  {
    theY = myScale * std::exp(myRate * theX) + myOffset;
    return true;
  }

  //! Evaluates exponential function and derivative.
  //! @param[in] theX input value
  //! @param[out] theY function value
  //! @param[out] theDY derivative value
  //! @return true (always succeeds)
  bool Values(double theX, double& theY, double& theDY) const
  {
    const double anExp = std::exp(myRate * theX);
    theY               = myScale * anExp + myOffset;
    theDY              = myScale * myRate * anExp;
    return true;
  }

private:
  double myScale;
  double myRate;
  double myOffset;
};

//! Power function functor: f(x) = a * x^n + b.
class Power
{
public:
  //! Constructor with full parameters.
  //! @param theExponent power n
  //! @param theScale scale factor a
  //! @param theOffset vertical offset b
  Power(double theExponent, double theScale = 1.0, double theOffset = 0.0)
    : myExponent(theExponent)
    , myScale(theScale)
    , myOffset(theOffset)
  {
  }

  //! Evaluates power function.
  //! @param[in] theX input value
  //! @param[out] theY function value
  //! @return false if x < 0 and exponent is non-integer
  bool Value(double theX, double& theY) const
  {
    if (theX < 0.0 && myExponent != std::floor(myExponent))
    {
      return false;
    }
    theY = myScale * std::pow(theX, myExponent) + myOffset;
    return true;
  }

  //! Evaluates power function and derivative.
  //! @param[in] theX input value
  //! @param[out] theY function value
  //! @param[out] theDY derivative value
  //! @return false if x < 0 and exponent is non-integer
  bool Values(double theX, double& theY, double& theDY) const
  {
    if (theX < 0.0 && myExponent != std::floor(myExponent))
    {
      return false;
    }
    const double aPow = std::pow(theX, myExponent);
    theY              = myScale * aPow + myOffset;
    if (std::abs(theX) < 1e-15)
    {
      theDY = (myExponent == 1.0) ? myScale : 0.0;
    }
    else
    {
      theDY = myScale * myExponent * aPow / theX;
    }
    return true;
  }

private:
  double myExponent;
  double myScale;
  double myOffset;
};

//! Gaussian function functor: f(x) = a * exp(-((x-mu)^2)/(2*sigma^2)).
class Gaussian
{
public:
  //! Constructor with full parameters.
  //! @param theAmplitude amplitude a (peak height)
  //! @param theMean mean mu (center)
  //! @param theSigma standard deviation sigma (width)
  Gaussian(double theAmplitude = 1.0, double theMean = 0.0, double theSigma = 1.0)
    : myAmplitude(theAmplitude)
    , myMean(theMean)
    , mySigma(theSigma)
  {
  }

  //! Evaluates Gaussian function.
  //! @param[in] theX input value
  //! @param[out] theY function value
  //! @return false if sigma is zero
  bool Value(double theX, double& theY) const
  {
    if (std::abs(mySigma) < 1e-15)
    {
      return false;
    }
    const double aZ = (theX - myMean) / mySigma;
    theY            = myAmplitude * std::exp(-0.5 * aZ * aZ);
    return true;
  }

  //! Evaluates Gaussian function and derivative.
  //! @param[in] theX input value
  //! @param[out] theY function value
  //! @param[out] theDY derivative value
  //! @return false if sigma is zero
  bool Values(double theX, double& theY, double& theDY) const
  {
    if (std::abs(mySigma) < 1e-15)
    {
      return false;
    }
    const double aZ   = (theX - myMean) / mySigma;
    const double aExp = std::exp(-0.5 * aZ * aZ);
    theY              = myAmplitude * aExp;
    theDY             = -myAmplitude * aZ * aExp / mySigma;
    return true;
  }

private:
  double myAmplitude;
  double myMean;
  double mySigma;
};

} // namespace MathUtils

#endif // _MathUtils_FunctorScalar_HeaderFile
