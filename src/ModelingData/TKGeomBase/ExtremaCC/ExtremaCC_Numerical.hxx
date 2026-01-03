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

#ifndef _ExtremaCC_Numerical_HeaderFile
#define _ExtremaCC_Numerical_HeaderFile

#include <ExtremaCC.hxx>
#include <ExtremaCC_GridEvaluator2D.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Template class for numerical curve-curve extrema computation.
//!
//! This template provides a uniform interface for computing extrema between
//! any two curve types using the grid-based approach. It handles all 30
//! numerical curve pairs:
//!
//! Mixed (Elementary x Numerical) - 20 pairs:
//!   - Line/Circle/Ellipse/Hyperbola/Parabola x BezierCurve (5)
//!   - Line/Circle/Ellipse/Hyperbola/Parabola x BSplineCurve (5)
//!   - Line/Circle/Ellipse/Hyperbola/Parabola x OffsetCurve (5)
//!   - Line/Circle/Ellipse/Hyperbola/Parabola x OtherCurve (5)
//!
//! Pure Numerical - 10 pairs:
//!   - BezierCurve x {BezierCurve, BSplineCurve, OffsetCurve, OtherCurve} (4)
//!   - BSplineCurve x {BSplineCurve, OffsetCurve, OtherCurve} (3)
//!   - OffsetCurve x {OffsetCurve, OtherCurve} (2)
//!   - OtherCurve x OtherCurve (1)
//!
//! The algorithm:
//! 1. Build grids for both curves based on their domains
//! 2. Scan 2D parameter space for gradient sign changes
//! 3. Newton refinement for each candidate
//!
//! @tparam Curve1Eval Evaluator type for first curve (must have Value, D1, D2, Domain, IsBounded)
//! @tparam Curve2Eval Evaluator type for second curve
template <typename Curve1Eval, typename Curve2Eval>
class ExtremaCC_Numerical
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with two curve evaluators (uses their domains).
  //! @param[in] theCurve1 first curve evaluator
  //! @param[in] theCurve2 second curve evaluator
  ExtremaCC_Numerical(const Curve1Eval& theCurve1, const Curve2Eval& theCurve2)
      : myCurve1(theCurve1),
        myCurve2(theCurve2)
  {
    myDomain.Curve1 = myCurve1.Domain();
    myDomain.Curve2 = myCurve2.Domain();
  }

  //! Constructor with two curve evaluators and explicit domain.
  //! @param[in] theCurve1 first curve evaluator
  //! @param[in] theCurve2 second curve evaluator
  //! @param[in] theDomain parameter domains for both curves
  ExtremaCC_Numerical(const Curve1Eval&          theCurve1,
                      const Curve2Eval&          theCurve2,
                      const ExtremaCC::Domain2D& theDomain)
      : myCurve1(theCurve1),
        myCurve2(theCurve2),
        myDomain(theDomain)
  {
  }

  //! Copy constructor is deleted.
  ExtremaCC_Numerical(const ExtremaCC_Numerical&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaCC_Numerical& operator=(const ExtremaCC_Numerical&) = delete;

  //! Move constructor.
  ExtremaCC_Numerical(ExtremaCC_Numerical&&) = default;

  //! Move assignment operator.
  ExtremaCC_Numerical& operator=(ExtremaCC_Numerical&&) = default;

  //! Compute extrema between the two curves.
  //! @param[in] theTol tolerance for comparison
  //! @param[in] theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing the extrema
  [[nodiscard]] const ExtremaCC::Result& Perform(
    double                theTol,
    ExtremaCC::SearchMode theMode = ExtremaCC::SearchMode::MinMax) const
  {
    myResult.Clear();

    ExtremaCC_GridEvaluator2D<Curve1Eval, Curve2Eval> aGridEval(myCurve1, myCurve2, myDomain);
    aGridEval.Perform(myResult, theTol, theMode);

    return myResult;
  }

  //! Compute extrema including endpoints.
  //! @param[in] theTol tolerance
  //! @param[in] theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing interior + endpoint extrema
  [[nodiscard]] const ExtremaCC::Result& PerformWithEndpoints(
    double                theTol,
    ExtremaCC::SearchMode theMode = ExtremaCC::SearchMode::MinMax) const
  {
    (void)Perform(theTol, theMode);

    // Add endpoint extrema if curves are bounded
    if (myResult.Status == ExtremaCC::Status::OK)
    {
      if (myCurve1.IsBounded() || myCurve2.IsBounded())
      {
        ExtremaCC::AddEndpointExtrema(myResult, myDomain, myCurve1, myCurve2, theTol, theMode);
      }
    }

    return myResult;
  }

  //! Returns the first curve evaluator.
  const Curve1Eval& Curve1() const { return myCurve1; }

  //! Returns the second curve evaluator.
  const Curve2Eval& Curve2() const { return myCurve2; }

  //! Returns the domain.
  const ExtremaCC::Domain2D& Domain() const { return myDomain; }

private:
  Curve1Eval                myCurve1; //!< First curve evaluator
  Curve2Eval                myCurve2; //!< Second curve evaluator
  ExtremaCC::Domain2D       myDomain; //!< Parameter domains
  mutable ExtremaCC::Result myResult; //!< Reusable result storage
};

#endif // _ExtremaCC_Numerical_HeaderFile
