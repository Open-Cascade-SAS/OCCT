// Created on: 1991-05-13
// Created by: Laurent PAINNOT
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _math_DirectPolynomialRoots_HeaderFile
#define _math_DirectPolynomialRoots_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_RangeError.hxx>
#include <StdFail_InfiniteSolutions.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_OStream.hxx>

//! This class implements the calculation of all the real roots of a real
//! polynomial of degree <= 4 using direct algebraic methods. The implementation
//! uses Ferrari's method for quartics, Cardano's formula for cubics, and
//! numerically stable algorithms for quadratics and linear equations.
//!
//! Key features:
//! - Robust numerical algorithms with coefficient scaling
//! - Newton-Raphson root refinement for improved accuracy
//! - Proper handling of degenerate and edge cases
//! - Multiple root detection and infinite solution handling
//! - Scientific reference ordering for deterministic results
//!
//! Once found, all roots are polished using the Newton-Raphson method
//! to achieve maximum numerical precision.
class math_DirectPolynomialRoots
{
public:
  DEFINE_STANDARD_ALLOC

  //! Computes all the real roots of the quartic polynomial
  //! Ax^4 + Bx^3 + Cx^2 + Dx + E = 0 using Ferrari's method.
  //!
  //! The algorithm:
  //! 1. Checks for degree reduction (A ~= 0)
  //! 2. Normalizes and scales coefficients for numerical stability
  //! 3. Solves Ferrari's resolvent cubic equation
  //! 4. Factors quartic into two quadratic equations
  //! 5. Solves both quadratics independently
  //! 6. Refines all roots using Newton-Raphson method
  //!
  //! @param theA coefficient of x^4 term
  //! @param theB coefficient of x^3 term
  //! @param theC coefficient of x^2 term
  //! @param theD coefficient of x term
  //! @param theE constant term
  Standard_EXPORT math_DirectPolynomialRoots(const double theA,
                                             const double theB,
                                             const double theC,
                                             const double theD,
                                             const double theE);

  //! Computes all the real roots of the cubic polynomial
  //! Ax^3 + Bx^2 + Cx + D = 0 using Cardano's method with Vieta substitution.
  //!
  //! The algorithm:
  //! 1. Transforms to depressed cubic t^3 + Pt + Q = 0
  //! 2. Computes discriminant Delta = -4P^3/27 - Q^2/4
  //! 3. Uses trigonometric method for Delta < 0 (three real roots)
  //! 4. Uses Cardano's formula for Delta > 0 (one real root)
  //! 5. Handles multiple roots when Delta = 0
  //! 6. Applies Newton-Raphson refinement
  //!
  //! @param theA coefficient of x^3 term
  //! @param theB coefficient of x^2 term
  //! @param theC coefficient of x term
  //! @param theD constant term
  Standard_EXPORT math_DirectPolynomialRoots(const double theA,
                                             const double theB,
                                             const double theC,
                                             const double theD);

  //! Computes all the real roots of the quadratic polynomial
  //! Ax^2 + Bx + C = 0 using numerically stable formulas.
  //!
  //! The algorithm avoids catastrophic cancellation by using:
  //! - Discriminant with error bounds: Delta = B^2 - 4AC
  //! - Stable root formulas based on sign of B
  //! - Newton-Raphson refinement for improved accuracy
  //!
  //! @param theA coefficient of x^2 term
  //! @param theB coefficient of x term
  //! @param theC constant term
  Standard_EXPORT math_DirectPolynomialRoots(const double theA,
                                             const double theB,
                                             const double theC);

  //! Computes the real root of the linear equation Ax + B = 0.
  //!
  //! Handles all cases:
  //! - A != 0: unique solution x = -B/A
  //! - A = 0, B != 0: no solution (inconsistent)
  //! - A = 0, B = 0: infinite solutions (identity)
  //!
  //! @param theA coefficient of x term
  //! @param theB constant term
  Standard_EXPORT math_DirectPolynomialRoots(const double theA, const double theB);

  //! Returns true if the computations are successful, otherwise returns false.
  //! Computations may fail due to numerical issues or overflow conditions.
  bool IsDone() const;

  //! Returns true if there is an infinity of roots, otherwise returns false.
  //! This occurs only for the degenerate linear case 0*x + 0 = 0.
  bool InfiniteRoots() const;

  //! Returns the number of distinct real roots found.
  //! An exception is raised if there are an infinity of roots.
  //! For multiple roots, this counts each root according to its multiplicity.
  int NbSolutions() const;

  //! Returns the value of the Nth root in default ordering.
  //! The default ordering may vary depending on the algorithm used.
  //! An exception is raised if there are an infinity of roots.
  //! Exception RangeError is raised if theIndex is < 1
  //! or theIndex > NbSolutions.
  //!
  //! @param theIndex root index (1-based)
  //! @return root value
  double Value(const int theIndex) const;

  //! Prints diagnostic information about the current state of the solver.
  //! Outputs computation status, number of roots, and individual root values.
  //! This method is used to redefine the operator << for debugging purposes.
  //!
  //! @param theStream output stream for diagnostic information
  Standard_EXPORT void Dump(Standard_OStream& theStream) const;

protected:
  //! Solves quartic equation Ax^4 + Bx^3 + Cx^2 + Dx + E = 0 using Ferrari's method
  //! with modern numerical enhancements for stability and accuracy.
  //!
  //! Implementation details:
  //! - Degree reduction check for nearly-zero leading coefficient
  //! - Coefficient normalization and scaling for numerical stability
  //! - Ferrari's resolvent cubic construction and solution
  //! - Quartic factorization into two quadratic equations
  //! - Newton-Raphson refinement of all roots
  //!
  //! @param theA coefficient of x^4 term
  //! @param theB coefficient of x^3 term
  //! @param theC coefficient of x^2 term
  //! @param theD coefficient of x term
  //! @param theE constant term
  Standard_EXPORT void Solve(const double theA,
                             const double theB,
                             const double theC,
                             const double theD,
                             const double theE);

  //! Solves cubic equation Ax^3 + Bx^2 + Cx + D = 0 using Cardano's method
  //! with Vieta substitution and trigonometric solution for stability.
  //!
  //! Implementation features:
  //! - Transformation to depressed cubic form
  //! - Discriminant-based method selection
  //! - Trigonometric solution for three real roots
  //! - Cardano's formula for one real root
  //! - Special handling for multiple roots
  //! - Coefficient scaling for numerical robustness
  //!
  //! @param theA coefficient of x^3 term
  //! @param theB coefficient of x^2 term
  //! @param theC coefficient of x term
  //! @param theD constant term
  Standard_EXPORT void Solve(const double theA,
                             const double theB,
                             const double theC,
                             const double theD);

  //! Solves quadratic equation Ax^2 + Bx + C = 0 using numerically stable
  //! formulas to avoid catastrophic cancellation.
  //!
  //! Features:
  //! - Discriminant computation with error bounds
  //! - Stable root extraction formulas
  //! - Proper handling of near-zero discriminant (double roots)
  //! - Newton-Raphson refinement
  //!
  //! @param theA coefficient of x^2 term
  //! @param theB coefficient of x term
  //! @param theC constant term
  Standard_EXPORT void Solve(const double theA,
                             const double theB,
                             const double theC);

  //! Solves linear equation Ax + B = 0 with proper handling of degenerate cases.
  //!
  //! Handles all cases:
  //! - Normal case (A != 0): unique solution
  //! - Inconsistent case (A = 0, B != 0): no solution
  //! - Identity case (A = 0, B = 0): infinite solutions
  //!
  //! @param theA coefficient of x term
  //! @param theB constant term
  Standard_EXPORT void Solve(const double theA, const double theB);

private:
  bool myDone;           //!< Computation status flag
  bool myInfiniteStatus; //!< Infinite solutions flag (0*x + 0 = 0)
  int myNbSol;          //!< Number of real roots found (0 to 4)
  double    myRoots[4];       //!< Array storing computed real roots
};

inline bool math_DirectPolynomialRoots::IsDone() const
{
  return myDone;
}

inline bool math_DirectPolynomialRoots::InfiniteRoots() const
{
  return myInfiniteStatus;
}

inline int math_DirectPolynomialRoots::NbSolutions() const
{
  StdFail_InfiniteSolutions_Raise_if(myInfiniteStatus, " ");
  return myNbSol;
}

inline double math_DirectPolynomialRoots::Value(const int theIndex) const
{
  StdFail_InfiniteSolutions_Raise_if(myInfiniteStatus, " ");
  Standard_RangeError_Raise_if((theIndex < 1) || (theIndex > myNbSol), " ");
  return myRoots[theIndex - 1];
}

inline Standard_OStream& operator<<(Standard_OStream&                 theStream,
                                    const math_DirectPolynomialRoots& theRoots)
{
  theRoots.Dump(theStream);
  return theStream;
}

#endif // _math_DirectPolynomialRoots_HeaderFile
