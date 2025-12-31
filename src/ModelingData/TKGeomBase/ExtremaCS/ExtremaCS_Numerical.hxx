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

#ifndef _ExtremaCS_Numerical_HeaderFile
#define _ExtremaCS_Numerical_HeaderFile

#include <ExtremaCS.hxx>
#include <ExtremaCS_GridEvaluator3D.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Numerical curve-surface extrema computation template.
//!
//! Generic template for computing extrema between any curve and surface
//! using grid-based numerical approach with Newton refinement.
//!
//! This template is used for all curve-surface pairs that don't have
//! specialized analytical solutions.
//!
//! @tparam CurveEval Curve evaluator type (e.g., ExtremaCS_Circle)
//! @tparam SurfaceEval Surface evaluator type (e.g., ExtremaCS_Cylinder)
template <typename CurveEval, typename SurfaceEval>
class ExtremaCS_Numerical
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with curve and surface evaluators.
  ExtremaCS_Numerical(const CurveEval& theCurveEval, const SurfaceEval& theSurfaceEval)
      : myCurveEval(theCurveEval),
        mySurfaceEval(theSurfaceEval),
        myDomain(std::nullopt)
  {
  }

  //! Constructor with curve and surface evaluators and domain.
  ExtremaCS_Numerical(const CurveEval&                           theCurveEval,
                      const SurfaceEval&                         theSurfaceEval,
                      const std::optional<ExtremaCS::Domain3D>&  theDomain)
      : myCurveEval(theCurveEval),
        mySurfaceEval(theSurfaceEval),
        myDomain(theDomain)
  {
  }

  //! @brief Perform extrema computation.
  //! @param theTol tolerance
  //! @param theMode search mode
  //! @return const reference to result
  [[nodiscard]] const ExtremaCS::Result& Perform(
    double                theTol,
    ExtremaCS::SearchMode theMode = ExtremaCS::SearchMode::MinMax) const
  {
    myResult.Clear();

    // Build domain from evaluators if not provided
    ExtremaCS::Domain3D aDomain;
    if (myDomain.has_value())
    {
      aDomain = *myDomain;
    }
    else
    {
      aDomain.Curve   = myCurveEval.Domain();
      aDomain.Surface = mySurfaceEval.Domain();
    }

    // Use grid-based numerical approach
    ExtremaCS_GridEvaluator3D<CurveEval, SurfaceEval> aGridEval(myCurveEval,
                                                                 mySurfaceEval,
                                                                 aDomain);
    aGridEval.Perform(myResult, theTol, theMode);

    return myResult;
  }

private:
  CurveEval                          myCurveEval;   //!< Curve evaluator
  SurfaceEval                        mySurfaceEval; //!< Surface evaluator
  std::optional<ExtremaCS::Domain3D> myDomain;      //!< Optional domain override
  mutable ExtremaCS::Result          myResult;      //!< Reusable result storage
};

#endif // _ExtremaCS_Numerical_HeaderFile
