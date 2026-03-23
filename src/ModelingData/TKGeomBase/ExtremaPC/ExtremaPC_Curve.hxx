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
#include <GeomAdaptor_TransformedCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
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
  Standard_EXPORT ExtremaPC_Curve(const GeomAdaptor_Curve& theCurve,
                                  double                   theUMin,
                                  double                   theUMax);

  //! Constructor with transformed curve adaptor.
  //! Uses the curve's natural parameter bounds as domain.
  //! For elementary curves, uses already-transformed primitives (analytical solver).
  //! For BSpline/Bezier, uses inverse-transform optimization with post-processing.
  //! For offset/other curves, delegates through Adaptor3d_Curve interface.
  //! @param[in] theCurve transformed curve adaptor
  Standard_EXPORT explicit ExtremaPC_Curve(const GeomAdaptor_TransformedCurve& theCurve);

  //! Constructor with transformed curve adaptor and parameter range.
  //! @param[in] theCurve transformed curve adaptor
  //! @param[in] theUMin lower parameter bound
  //! @param[in] theUMax upper parameter bound
  Standard_EXPORT ExtremaPC_Curve(const GeomAdaptor_TransformedCurve& theCurve,
                                  double                              theUMin,
                                  double                              theUMax);

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

  ExtremaPC_Curve(const ExtremaPC_Curve&)            = delete;
  ExtremaPC_Curve& operator=(const ExtremaPC_Curve&) = delete;
  ExtremaPC_Curve(ExtremaPC_Curve&&)                 = delete;
  ExtremaPC_Curve& operator=(ExtremaPC_Curve&&)      = delete;

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
  //! Helper to initialize evaluator from an adaptor curve with curve-type switch.
  //! @param[in] theCurve the adaptor curve
  //! @param[in] theDomain parameter domain
  void initFromAdaptor(const GeomAdaptor_Curve& theCurve, const ExtremaPC::Domain1D& theDomain);

  //! Helper method to initialize evaluator from a Geom_Curve.
  //! Handles all curve type detection and evaluator creation.
  //! @param[in] theCurve the curve to initialize from (must not be null)
  //! @param[in] theDomain optional domain to use
  void initFromGeomCurve(const occ::handle<Geom_Curve>&            theCurve,
                         const std::optional<ExtremaPC::Domain1D>& theDomain);

  //! Helper to initialize from a TransformedCurve.
  //! Dispatches based on curve type and transform properties.
  //! @param[in] theCurve the transformed curve adaptor
  //! @param[in] theDomain optional domain to use
  void initFromTransformedCurve(const GeomAdaptor_TransformedCurve&       theCurve,
                                const std::optional<ExtremaPC::Domain1D>& theDomain);

  //! Post-process result from untransformed-space computation.
  //! Forward-transforms points and scales distances.
  //! @param[in] theSrc result in untransformed space
  //! @return reference to myTransformResult with transformed data
  const ExtremaPC::Result& postProcessTransform(const ExtremaPC::Result& theSrc) const;

  EvaluatorVariant             myEvaluator;            //!< Specialized evaluator
  const Adaptor3d_Curve*       myAdaptorRef = nullptr; //!< Non-owning pointer to caller's adaptor
  occ::handle<Adaptor3d_Curve> myAdaptorOwned;         //!< Owned adaptor for lifetime management
  gp_Trsf                      myTrsf; //!< Forward transform (identity = no post-processing)
  mutable ExtremaPC::Result    myTransformResult; //!< Result buffer for transform post-processing
};

#endif // _ExtremaPC_Curve_HeaderFile
