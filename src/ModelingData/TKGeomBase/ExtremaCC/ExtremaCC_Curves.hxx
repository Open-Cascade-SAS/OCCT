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

#ifndef _ExtremaCC_Curves_HeaderFile
#define _ExtremaCC_Curves_HeaderFile

#include <ExtremaCC.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Standard_DefineAlloc.hxx>

#include <variant>

// Forward declarations for analytical pairs
class ExtremaCC_CircleCircle;
class ExtremaCC_CircleEllipse;
class ExtremaCC_CircleHyperbola;
class ExtremaCC_CircleLine;
class ExtremaCC_CircleParabola;
class ExtremaCC_EllipseEllipse;
class ExtremaCC_EllipseHyperbola;
class ExtremaCC_EllipseLine;
class ExtremaCC_EllipseParabola;
class ExtremaCC_HyperbolaHyperbola;
class ExtremaCC_HyperbolaLine;
class ExtremaCC_HyperbolaParabola;
class ExtremaCC_LineLine;
class ExtremaCC_LineParabola;
class ExtremaCC_ParabolaParabola;

//! @brief Main aggregator for curve-curve extrema computation.
//!
//! This class provides a uniform interface for computing extrema between
//! any two 3D curves, automatically selecting the optimal algorithm based
//! on the curve types.
//!
//! For elementary curve pairs (Line, Circle, Ellipse, Hyperbola, Parabola),
//! specialized analytical algorithms are used.
//!
//! For numerical curve pairs (involving Bezier, BSpline, Offset, or other curves),
//! grid-based numerical methods with Newton refinement are used.
//!
//! Usage:
//! @code
//!   GeomAdaptor_Curve aCurve1(aGeomCurve1);
//!   GeomAdaptor_Curve aCurve2(aGeomCurve2);
//!   ExtremaCC_Curves anExtrema(aCurve1, aCurve2);
//!   const auto& aResult = anExtrema.Perform(1e-7);
//!   if (aResult.IsDone())
//!   {
//!     for (int i = 0; i < aResult.NbExtrema(); ++i)
//!     {
//!       const auto& anExt = aResult.Extremum(i);
//!       // Use anExt.Point1, anExt.Point2, anExt.SquareDistance, etc.
//!     }
//!   }
//! @endcode
class ExtremaCC_Curves
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with two curves (uses full curve domains).
  //! @param[in] theCurve1 first curve
  //! @param[in] theCurve2 second curve
  Standard_EXPORT ExtremaCC_Curves(const GeomAdaptor_Curve& theCurve1,
                                   const GeomAdaptor_Curve& theCurve2);

  //! Constructor with two curves and parameter domains.
  //! @param[in] theCurve1 first curve
  //! @param[in] theCurve2 second curve
  //! @param[in] theDomain parameter domains for both curves
  Standard_EXPORT ExtremaCC_Curves(const GeomAdaptor_Curve&   theCurve1,
                                   const GeomAdaptor_Curve&   theCurve2,
                                   const ExtremaCC::Domain2D& theDomain);

  //! Destructor.
  Standard_EXPORT ~ExtremaCC_Curves();

  //! Copy constructor is deleted.
  ExtremaCC_Curves(const ExtremaCC_Curves&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaCC_Curves& operator=(const ExtremaCC_Curves&) = delete;

  //! Move constructor.
  Standard_EXPORT ExtremaCC_Curves(ExtremaCC_Curves&& theOther) noexcept;

  //! Move assignment operator.
  Standard_EXPORT ExtremaCC_Curves& operator=(ExtremaCC_Curves&& theOther) noexcept;

  //! Compute extrema between the two curves.
  //! @param[in] theTol tolerance for root finding
  //! @param[in] theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing the extrema
  [[nodiscard]] Standard_EXPORT const ExtremaCC::Result& Perform(
    double                theTol,
    ExtremaCC::SearchMode theMode = ExtremaCC::SearchMode::MinMax) const;

  //! Compute extrema including endpoint evaluations.
  //! @param[in] theTol tolerance for root finding
  //! @param[in] theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing interior + endpoint extrema
  [[nodiscard]] Standard_EXPORT const ExtremaCC::Result& PerformWithEndpoints(
    double                theTol,
    ExtremaCC::SearchMode theMode = ExtremaCC::SearchMode::MinMax) const;

  //! Returns the domain.
  const ExtremaCC::Domain2D& Domain() const { return myDomain; }

  //! Returns true if curves were swapped to achieve canonical ordering.
  bool IsSwapped() const { return mySwapped; }

private:
  //! Initializes the appropriate pair handler based on curve types.
  void initPair(const GeomAdaptor_Curve& theCurve1, const GeomAdaptor_Curve& theCurve2);

  //! Variant type for analytical pairs (use unique_ptr to avoid header dependencies).
  struct AnalyticalPair;

  //! Variant type for numerical pairs (uses grid evaluator).
  struct NumericalPair;

  ExtremaCC::Domain2D       myDomain;  //!< Parameter domains
  bool                      mySwapped; //!< True if curves were swapped for canonical order
  mutable ExtremaCC::Result myResult;  //!< Result storage

  //! Implementation pointer (PIMPL idiom to reduce header dependencies).
  struct Impl;
  Impl* myImpl;
};

#endif // _ExtremaCC_Curves_HeaderFile
