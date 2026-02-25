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

#ifndef _GeomProp_Curve_HeaderFile
#define _GeomProp_Curve_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomProp.hxx>
#include <GeomProp_BezierCurve.hxx>
#include <GeomProp_BSplineCurve.hxx>
#include <GeomProp_Circle.hxx>
#include <GeomProp_Ellipse.hxx>
#include <GeomProp_Hyperbola.hxx>
#include <GeomProp_Line.hxx>
#include <GeomProp_OffsetCurve.hxx>
#include <GeomProp_OtherCurve.hxx>
#include <GeomProp_Parabola.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <variant>

//! @brief Unified local differential property evaluator for any 3D curve.
//!
//! Uses std::variant for compile-time type safety and zero heap allocation
//! for the evaluator itself. Automatically detects curve type from
//! Adaptor3d_Curve or Geom_Curve and dispatches to the appropriate
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
//! - Other: Fallback using Geom_Curve virtual D1/D2/D3
//!
//! Usage:
//! @code
//!   GeomProp_Curve aProp(myGeomCurve);
//!   GeomProp::CurvatureResult aCurv = aProp.Curvature(0.5, Precision::Confusion());
//!   if (aCurv.IsDefined)
//!   {
//!     double aValue = aCurv.Value;
//!   }
//! @endcode
class GeomProp_Curve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Variant type holding all possible 3D curve property evaluators.
  using EvaluatorVariant = std::variant<std::monostate,
                                        GeomProp_Line,
                                        GeomProp_Circle,
                                        GeomProp_Ellipse,
                                        GeomProp_Hyperbola,
                                        GeomProp_Parabola,
                                        GeomProp_BezierCurve,
                                        GeomProp_BSplineCurve,
                                        GeomProp_OffsetCurve,
                                        GeomProp_OtherCurve>;

  //! Construct from 3D adaptor reference (auto-detects curve type).
  //! For GeomAdaptor_Curve, extracts underlying Geom_Curve for optimized evaluation.
  //! @param[in] theCurve 3D curve adaptor reference
  Standard_EXPORT GeomProp_Curve(const Adaptor3d_Curve& theCurve);

  //! Construct from geometry handle (auto-detects curve type).
  //! @param[in] theCurve 3D geometry to evaluate
  Standard_EXPORT GeomProp_Curve(const occ::handle<Geom_Curve>& theCurve);

  //! Non-copyable and non-movable.
  GeomProp_Curve(const GeomProp_Curve&)            = delete;
  GeomProp_Curve& operator=(const GeomProp_Curve&) = delete;
  GeomProp_Curve(GeomProp_Curve&&)                 = delete;
  GeomProp_Curve& operator=(GeomProp_Curve&&)      = delete;

  //! Returns the detected curve type.
  GeomAbs_CurveType GetType() const { return myCurveType; }

  //! Compute tangent at given parameter.
  //! @param[in] theParam curve parameter
  //! @param[in] theTol linear tolerance
  //! @return tangent result with validity flag
  Standard_EXPORT GeomProp::TangentResult Tangent(double theParam, double theTol) const;

  //! Compute curvature at given parameter.
  //! @param[in] theParam curve parameter
  //! @param[in] theTol linear tolerance
  //! @return curvature result with validity and infinity flags
  Standard_EXPORT GeomProp::CurvatureResult Curvature(double theParam, double theTol) const;

  //! Compute normal at given parameter.
  //! @param[in] theParam curve parameter
  //! @param[in] theTol linear tolerance
  //! @return normal result with validity flag
  Standard_EXPORT GeomProp::NormalResult Normal(double theParam, double theTol) const;

  //! Compute centre of curvature at given parameter.
  //! @param[in] theParam curve parameter
  //! @param[in] theTol linear tolerance
  //! @return centre result with validity flag
  Standard_EXPORT GeomProp::CentreResult CentreOfCurvature(double theParam, double theTol) const;

  //! Find curvature extrema on the curve.
  //! @return analysis result with special points sorted by parameter
  Standard_EXPORT GeomProp::CurveAnalysis FindCurvatureExtrema() const;

  //! Find inflection points on the curve.
  //! @return analysis result with inflection points sorted by parameter
  Standard_EXPORT GeomProp::CurveAnalysis FindInflections() const;

protected:
  //! Initialize from 3D adaptor reference (auto-detects curve type).
  //! @param[in] theCurve 3D curve adaptor reference
  Standard_EXPORT void initialization(const Adaptor3d_Curve& theCurve);

  //! Initialize from geometry handle (auto-detects curve type).
  //! @param[in] theCurve 3D geometry to evaluate
  Standard_EXPORT void initialization(const occ::handle<Geom_Curve>& theCurve);

private:
  //! Initialize from stored adaptor (dispatches to per-geometry evaluator).
  //! Must be called after myAdaptor is set. Per-geometry evaluators receive
  //! a non-owning pointer to myAdaptor; their lifetime is managed by the variant.
  Standard_EXPORT void initFromAdaptor();

  occ::handle<GeomAdaptor_Curve> myAdaptor; //!< Owns the adaptor (ensures lifetime).
  EvaluatorVariant  myEvaluator; //!< Per-geometry evaluator (non-owning pointer to myAdaptor).
  GeomAbs_CurveType myCurveType;
};

#endif // _GeomProp_Curve_HeaderFile
