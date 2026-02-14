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

#ifndef _ExtremaPC_Curve_HeaderFile
#define _ExtremaPC_Curve_HeaderFile

#include <ExtremaPC.hxx>
#include <ExtremaPC_BezierCurve.hxx>
#include <ExtremaPC_BSplineCurve.hxx>
#include <ExtremaPC_Circle.hxx>
#include <ExtremaPC_Ellipse.hxx>
#include <ExtremaPC_Hyperbola.hxx>
#include <ExtremaPC_Line.hxx>
#include <ExtremaPC_OffsetCurve.hxx>
#include <ExtremaPC_OtherCurve.hxx>
#include <ExtremaPC_Parabola.hxx>
#include <Geom_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <Standard_DefineAlloc.hxx>

#include <variant>

//! @brief Main aggregator for Point-Curve extrema computation.
//!
//! Provides a unified interface for computing extrema between a 3D point
//! and any type of curve. Uses std::variant with std::visit for efficient
//! curve type dispatch at runtime.
//!
//! Supports all elementary curve types (analytical solutions):
//! - Line, Circle, Ellipse, Hyperbola, Parabola
//!
//! And numerical curves (grid-based with Newton refinement):
//! - BSpline, Bezier, Offset, and general curves
//!
//! @note The parameter domain is fixed at construction time for optimal performance.
//!       The inner curve evaluators build their grids eagerly in the constructor,
//!       so multiple Perform() calls reuse the pre-built data without rebuilding.
//!
//! Usage example:
//! @code
//! ExtremaPC_Curve anExtPC(myAdaptorCurve);
//! const ExtremaPC::Result& aResult = anExtPC.Perform(myPoint, 1.0e-9);
//! if (aResult.IsDone())
//! {
//!   for (int i = 0; i < aResult.NbExt(); ++i)
//!   {
//!     double aDist = std::sqrt(aResult[i].SquareDistance);
//!     gp_Pnt aPtOnCurve = aResult[i].Point;
//!   }
//! }
//! @endcode
class ExtremaPC_Curve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Variant type holding all specialized evaluators.
  using EvaluatorVariant = std::variant<std::monostate,
                                        ExtremaPC_Line,
                                        ExtremaPC_Circle,
                                        ExtremaPC_Ellipse,
                                        ExtremaPC_Hyperbola,
                                        ExtremaPC_Parabola,
                                        ExtremaPC_BezierCurve,
                                        ExtremaPC_BSplineCurve,
                                        ExtremaPC_OffsetCurve,
                                        ExtremaPC_OtherCurve>;

  //! Constructor with curve adaptor.
  //! Uses the curve's natural parameter bounds as domain.
  //! @param[in] theCurve curve adaptor
  Standard_EXPORT explicit ExtremaPC_Curve(const GeomAdaptor_Curve& theCurve);

  //! Constructor with curve adaptor and parameter range.
  //! @param[in] theCurve curve adaptor
  //! @param[in] theUMin lower parameter bound
  //! @param[in] theUMax upper parameter bound
  Standard_EXPORT ExtremaPC_Curve(const GeomAdaptor_Curve& theCurve, double theUMin, double theUMax);

  //! Constructor with Geom_Curve.
  //! For non-trimmed curves, does NOT set domain (uses natural/unbounded behavior).
  //! For trimmed curves, uses the trimmed bounds as domain.
  //! @param[in] theCurve geometric curve handle
  Standard_EXPORT explicit ExtremaPC_Curve(const occ::handle<Geom_Curve>& theCurve);

  //! Constructor with Geom_Curve and parameter range.
  //! For trimmed curves, intersects input bounds with trimmed bounds.
  //! @param[in] theCurve geometric curve handle
  //! @param[in] theUMin lower parameter bound
  //! @param[in] theUMax upper parameter bound
  Standard_EXPORT ExtremaPC_Curve(const occ::handle<Geom_Curve>& theCurve,
                                  double                         theUMin,
                                  double                         theUMax);

  //! Copy constructor is deleted.
  ExtremaPC_Curve(const ExtremaPC_Curve&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaPC_Curve& operator=(const ExtremaPC_Curve&) = delete;

  //! Move constructor.
  ExtremaPC_Curve(ExtremaPC_Curve&&) = default;

  //! Move assignment operator.
  ExtremaPC_Curve& operator=(ExtremaPC_Curve&&) = default;

  //! Computes extrema between point P and the curve.
  //! Uses domain specified at construction time.
  //! @param[in] theP query point
  //! @param[in] theTol tolerance for extrema computation
  //! @param[in] theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing all found extrema
  [[nodiscard]] Standard_EXPORT const ExtremaPC::Result& Perform(
    const gp_Pnt&         theP,
    double                theTol,
    ExtremaPC::SearchMode theMode = ExtremaPC::SearchMode::MinMax) const;

  //! Computes extrema between point P and the curve, including endpoints.
  //! Uses domain specified at construction time.
  //! @param[in] theP query point
  //! @param[in] theTol tolerance for extrema computation
  //! @param[in] theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing interior + endpoint extrema
  [[nodiscard]] Standard_EXPORT const ExtremaPC::Result& PerformWithEndpoints(
    const gp_Pnt&         theP,
    double                theTol,
    ExtremaPC::SearchMode theMode = ExtremaPC::SearchMode::MinMax) const;

  //! Returns true if the evaluator is properly initialized.
  bool IsInitialized() const { return !std::holds_alternative<std::monostate>(myEvaluator); }

private:
  //! Helper method to initialize evaluator from a Geom_Curve.
  //! Handles all curve type detection and evaluator creation.
  //! @param[in] theCurve the curve to initialize from (must not be null)
  //! @param[in] theDomain optional domain to use
  void initFromGeomCurve(const occ::handle<Geom_Curve>&            theCurve,
                         const std::optional<ExtremaPC::Domain1D>& theDomain);

  EvaluatorVariant               myEvaluator; //!< Specialized evaluator
  occ::handle<GeomAdaptor_Curve> myAdaptor;   //!< Stored adaptor for Geom-based construction
};

#endif // _ExtremaPC_Curve_HeaderFile
