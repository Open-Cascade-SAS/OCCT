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

#ifndef _ExtremaPC2d_Curve_HeaderFile
#define _ExtremaPC2d_Curve_HeaderFile

#include <ExtremaPC2d.hxx>
#include <ExtremaPC2d_BezierCurve.hxx>
#include <ExtremaPC2d_BSplineCurve.hxx>
#include <ExtremaPC2d_Circle.hxx>
#include <ExtremaPC2d_Ellipse.hxx>
#include <ExtremaPC2d_Hyperbola.hxx>
#include <ExtremaPC2d_Line.hxx>
#include <ExtremaPC2d_OffsetCurve.hxx>
#include <ExtremaPC2d_OtherCurve.hxx>
#include <ExtremaPC2d_Parabola.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_DefineAlloc.hxx>

#include <variant>

//! @brief Main aggregator for Point-Curve extrema computation.
//!
//! Provides a unified interface for computing extrema between a 2D point
//! and any type of curve. Uses std::variant with std::visit for efficient
//! curve type dispatch at runtime.
//!
//! Supports all elementary curve types (analytical solutions):
//! - Line, Circle, Ellipse, Hyperbola, Parabola
//!
//! And numerical curves (derivative-aware multiple-root isolation):
//! - BSpline, Bezier, Offset, and general curves
//!
//! @note The parameter domain is fixed at construction time. Numerical evaluators refresh their
//!       parameter partition on Perform() so mutations of referenced geometry are observed.
//!
//! Usage example:
//! @code
//! ExtremaPC2d_Curve anExtPC(myAdaptorCurve);
//! const ExtremaPC2d::Result& aResult = anExtPC.Perform(myPoint, 1.0e-9);
//! if (aResult.IsDone())
//! {
//!   for (size_t i = 0; i < aResult.NbExt(); ++i)
//!   {
//!     double aDist = std::sqrt(aResult[i].SquareDistance);
//!     gp_Pnt2d aPtOnCurve = aResult[i].Point;
//!   }
//! }
//! @endcode
class ExtremaPC2d_Curve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Variant type holding all specialized evaluators.
  using EvaluatorVariant = std::variant<std::monostate,
                                        ExtremaPC2d_Line,
                                        ExtremaPC2d_Circle,
                                        ExtremaPC2d_Ellipse,
                                        ExtremaPC2d_Hyperbola,
                                        ExtremaPC2d_Parabola,
                                        ExtremaPC2d_BezierCurve,
                                        ExtremaPC2d_BSplineCurve,
                                        ExtremaPC2d_OffsetCurve,
                                        ExtremaPC2d_OtherCurve>;

  //! Constructor with curve adaptor.
  //! Uses the curve's natural parameter bounds as domain.
  //! @param[in] theCurve curve adaptor; must outlive this evaluator when dispatched as an offset
  //!                     or general curve
  Standard_EXPORT explicit ExtremaPC2d_Curve(const Adaptor2d_Curve2d& theCurve);

  //! Constructor with curve adaptor and parameter range.
  //! @param[in] theCurve curve adaptor; must outlive this evaluator when dispatched as an offset
  //!                     or general curve
  //! @param[in] theUMin lower parameter bound
  //! @param[in] theUMax upper parameter bound
  Standard_EXPORT ExtremaPC2d_Curve(const Adaptor2d_Curve2d& theCurve,
                                    double                   theUMin,
                                    double                   theUMax);

  //! Constructor with Geom2d_Curve.
  //! For non-trimmed curves, uses the natural domain. Numerical offset and general
  //! curves require this domain to be finite; otherwise Perform() returns InvalidInput.
  //! For trimmed curves, uses the trimmed bounds as domain.
  //! @param[in] theCurve geometric curve handle
  Standard_EXPORT explicit ExtremaPC2d_Curve(const occ::handle<Geom2d_Curve>& theCurve);

  //! Constructor with Geom2d_Curve and parameter range.
  //! For trimmed curves, intersects input bounds with trimmed bounds.
  //! @param[in] theCurve geometric curve handle
  //! @param[in] theUMin lower parameter bound
  //! @param[in] theUMax upper parameter bound
  Standard_EXPORT ExtremaPC2d_Curve(const occ::handle<Geom2d_Curve>& theCurve,
                                    double                           theUMin,
                                    double                           theUMax);

  ExtremaPC2d_Curve(const ExtremaPC2d_Curve&)            = delete;
  ExtremaPC2d_Curve& operator=(const ExtremaPC2d_Curve&) = delete;
  ExtremaPC2d_Curve(ExtremaPC2d_Curve&&)                 = delete;
  ExtremaPC2d_Curve& operator=(ExtremaPC2d_Curve&&)      = delete;

  //! Computes extrema between point P and the curve.
  //! Uses domain specified at construction time.
  //! @param[in] theP query point
  //! @param[in] theTol tolerance for extrema computation
  //! @param[in] theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing all found extrema
  [[nodiscard]] Standard_EXPORT const ExtremaPC2d::Result& Perform(
    const gp_Pnt2d&         theP,
    double                  theTol,
    ExtremaPC2d::SearchMode theMode = ExtremaPC2d::SearchMode::MinMax) const;

  //! Computes extrema between point P and the curve, including endpoints.
  //! Uses domain specified at construction time.
  //! @param[in] theP query point
  //! @param[in] theTol tolerance for extrema computation
  //! @param[in] theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing interior + endpoint extrema
  [[nodiscard]] Standard_EXPORT const ExtremaPC2d::Result& PerformWithEndpoints(
    const gp_Pnt2d&         theP,
    double                  theTol,
    ExtremaPC2d::SearchMode theMode = ExtremaPC2d::SearchMode::MinMax) const;

  //! Returns true if the evaluator is properly initialized.
  bool IsInitialized() const { return !std::holds_alternative<std::monostate>(myEvaluator); }

private:
  //! Helper to initialize evaluator from an adaptor curve with curve-type switch.
  //! @param[in] theCurve the adaptor curve
  //! @param[in] theDomain parameter domain
  void initFromAdaptor(const Adaptor2d_Curve2d& theCurve, const ExtremaPC2d::Domain1D& theDomain);

  //! Helper method to initialize evaluator from a Geom2d_Curve.
  //! Handles all curve type detection and evaluator creation.
  //! @param[in] theCurve the curve to initialize from (must not be null)
  //! @param[in] theDomain optional domain to use
  void initFromGeomCurve(const occ::handle<Geom2d_Curve>&            theCurve,
                         const std::optional<ExtremaPC2d::Domain1D>& theDomain);

  EvaluatorVariant               myEvaluator;            //!< Specialized evaluator
  const Adaptor2d_Curve2d*       myAdaptorRef = nullptr; //!< Non-owning numerical adaptor
  occ::handle<Adaptor2d_Curve2d> myAdaptorOwned;         //!< Adaptor for Geom2d_Curve input
};

#endif // _ExtremaPC2d_Curve_HeaderFile
