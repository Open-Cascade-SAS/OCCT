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

#ifndef _Geom2dProp_Curve_HeaderFile
#define _Geom2dProp_Curve_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <Geom2dProp.hxx>
#include <Geom2dProp_BezierCurve.hxx>
#include <Geom2dProp_BSplineCurve.hxx>
#include <Geom2dProp_Circle.hxx>
#include <Geom2dProp_Ellipse.hxx>
#include <Geom2dProp_Hyperbola.hxx>
#include <Geom2dProp_Line.hxx>
#include <Geom2dProp_OffsetCurve.hxx>
#include <Geom2dProp_OtherCurve.hxx>
#include <Geom2dProp_Parabola.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <variant>

//! @brief Unified local differential property evaluator for any 2D curve.
//!
//! Uses std::variant for compile-time type safety and zero heap allocation
//! for the evaluator itself. Automatically detects curve type from
//! Adaptor2d_Curve2d or Geom2d_Curve and dispatches to the appropriate
//! specialized evaluator.
//!
//! Supported curve types with optimized evaluation:
//! - Line: Trivial (zero curvature, constant tangent)
//! - Circle: Constant curvature 1/R
//! - Ellipse: Analytical curvature extrema at vertices
//! - Hyperbola: Analytical curvature extremum at vertex
//! - Parabola: Analytical curvature extremum at vertex
//! - BezierCurve: Numeric curvature extrema/inflection finding
//! - BSplineCurve: Numeric with C3 interval subdivision
//! - OffsetCurve: Numeric approach
//! - Other: Fallback using Geom2d_Curve virtual D1/D2/D3
//!
//! Usage:
//! @code
//!   Geom2dProp_Curve aProp(myGeom2dCurve);
//!   Geom2dProp::CurvatureResult aCurv = aProp.Curvature(0.5, Precision::Confusion());
//!   if (aCurv.IsDefined)
//!   {
//!     double aValue = aCurv.Value;
//!   }
//! @endcode
class Geom2dProp_Curve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Variant type holding all possible 2D curve property evaluators.
  using EvaluatorVariant = std::variant<std::monostate,
                                        Geom2dProp_Line,
                                        Geom2dProp_Circle,
                                        Geom2dProp_Ellipse,
                                        Geom2dProp_Hyperbola,
                                        Geom2dProp_Parabola,
                                        Geom2dProp_BezierCurve,
                                        Geom2dProp_BSplineCurve,
                                        Geom2dProp_OffsetCurve,
                                        Geom2dProp_OtherCurve>;

  //! Construct from 2D adaptor reference (auto-detects curve type).
  //! For Geom2dAdaptor_Curve, extracts underlying Geom2d_Curve for optimized evaluation.
  //! @param[in] theCurve 2D curve adaptor reference
  Standard_EXPORT Geom2dProp_Curve(const Adaptor2d_Curve2d& theCurve);

  //! Construct from geometry handle (auto-detects curve type).
  //! @param[in] theCurve 2D geometry to evaluate
  Standard_EXPORT Geom2dProp_Curve(const occ::handle<Geom2d_Curve>& theCurve);

  //! Non-copyable and non-movable.
  Geom2dProp_Curve(const Geom2dProp_Curve&)            = delete;
  Geom2dProp_Curve& operator=(const Geom2dProp_Curve&) = delete;
  Geom2dProp_Curve(Geom2dProp_Curve&&)                 = delete;
  Geom2dProp_Curve& operator=(Geom2dProp_Curve&&)      = delete;

  //! Returns the detected curve type.
  GeomAbs_CurveType GetType() const { return myCurveType; }

  //! Returns the adaptor pointer from the active evaluator, or null if not initialized.
  Standard_EXPORT const Geom2dAdaptor_Curve* Adaptor() const;

  //! Compute tangent at given parameter.
  //! @param[in] theParam curve parameter
  //! @param[in] theTol linear tolerance
  //! @return tangent result with validity flag
  Standard_EXPORT Geom2dProp::TangentResult Tangent(double theParam, double theTol) const;

  //! Compute curvature at given parameter.
  //! @param[in] theParam curve parameter
  //! @param[in] theTol linear tolerance
  //! @return curvature result with validity and infinity flags
  Standard_EXPORT Geom2dProp::CurvatureResult Curvature(double theParam, double theTol) const;

  //! Compute normal at given parameter.
  //! @param[in] theParam curve parameter
  //! @param[in] theTol linear tolerance
  //! @return normal result with validity flag
  Standard_EXPORT Geom2dProp::NormalResult Normal(double theParam, double theTol) const;

  //! Compute centre of curvature at given parameter.
  //! @param[in] theParam curve parameter
  //! @param[in] theTol linear tolerance
  //! @return centre result with validity flag
  Standard_EXPORT Geom2dProp::CentreResult CentreOfCurvature(double theParam, double theTol) const;

  //! Find curvature extrema on the curve.
  //! @return analysis result with special points sorted by parameter
  Standard_EXPORT Geom2dProp::CurveAnalysis FindCurvatureExtrema() const;

  //! Find inflection points on the curve.
  //! @return analysis result with inflection points sorted by parameter
  Standard_EXPORT Geom2dProp::CurveAnalysis FindInflections() const;

protected:
  //! Initialize from 2D adaptor reference (auto-detects curve type).
  //! @param[in] theCurve 2D curve adaptor reference
  Standard_EXPORT void initialization(const Adaptor2d_Curve2d& theCurve);

  //! Initialize from geometry handle (auto-detects curve type).
  //! @param[in] theCurve 2D geometry to evaluate
  Standard_EXPORT void initialization(const occ::handle<Geom2d_Curve>& theCurve);

private:
  //! Initialize from stored adaptor (dispatches to per-geometry evaluator).
  //! Must be called after myAdaptor is set. Per-geometry evaluators receive
  //! a non-owning pointer to myAdaptor; their lifetime is managed by the variant.
  Standard_EXPORT void initFromAdaptor();

  occ::handle<Geom2dAdaptor_Curve> myAdaptor; //!< Owns the adaptor (ensures lifetime).
  EvaluatorVariant  myEvaluator; //!< Per-geometry evaluator (non-owning pointer to myAdaptor).
  GeomAbs_CurveType myCurveType;
};

#endif // _Geom2dProp_Curve_HeaderFile
