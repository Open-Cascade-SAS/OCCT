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

#ifndef _GeomGridEval_Curve_HeaderFile
#define _GeomGridEval_Curve_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <Geom_Curve.hxx>
#include <GeomGridEval_BSplineCurve.hxx>
#include <GeomGridEval_BezierCurve.hxx>
#include <GeomGridEval_Circle.hxx>
#include <GeomGridEval_CurveOnSurface.hxx>
#include <GeomGridEval_Ellipse.hxx>
#include <GeomGridEval_Hyperbola.hxx>
#include <GeomGridEval_Line.hxx>
#include <GeomGridEval_OffsetCurve.hxx>
#include <GeomGridEval_OtherCurve.hxx>
#include <GeomGridEval_Parabola.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <NCollection_Array1.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>
#include <variant>

//! @brief Unified grid evaluator for any 3D curve.
//!
//! Uses std::variant for compile-time type safety and zero heap allocation
//! for the evaluator itself. Automatically detects curve type from
//! Adaptor3d_Curve and dispatches to the appropriate specialized evaluator.
//!
//! Supported curve types with optimized evaluation:
//! - Line: Direct analytical formula
//! - Circle: Trigonometric formula
//! - Ellipse: Analytical formula
//! - Hyperbola: Analytical formula
//! - Parabola: Analytical formula
//! - BezierCurve: Optimized batch evaluation via BSplCLib
//! - BSplineCurve: Optimized batch evaluation via BSplCLib_GridEvaluator
//! - Other: Fallback using Adaptor3d_Curve::D0
//!
//! Usage:
//! @code
//!   GeomGridEval_Curve anEval;
//!   anEval.Initialize(myAdaptorCurve);
//!   // OR
//!   anEval.Initialize(myGeomCurve);
//!   NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid(myParams);
//! @endcode
class GeomGridEval_Curve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Variant type holding all possible curve evaluators.
  using EvaluatorVariant = std::variant<std::monostate,               // Uninitialized state
                                        GeomGridEval_Line,            // Line curve
                                        GeomGridEval_Circle,          // Circle curve
                                        GeomGridEval_Ellipse,         // Ellipse curve
                                        GeomGridEval_Hyperbola,       // Hyperbola curve
                                        GeomGridEval_Parabola,        // Parabola curve
                                        GeomGridEval_BezierCurve,     // Bezier curve
                                        GeomGridEval_BSplineCurve,    // B-spline curve
                                        GeomGridEval_OffsetCurve,     // Offset curve
                                        GeomGridEval_CurveOnSurface,  // Curve on surface
                                        GeomGridEval_OtherCurve>;     // Fallback for other types

  //! Default constructor - uninitialized state.
  GeomGridEval_Curve()
      : myEvaluator(std::monostate{}),
        myCurveType(GeomAbs_OtherCurve)
  {
  }

  //! Non-copyable and non-movable.
  GeomGridEval_Curve(const GeomGridEval_Curve&)            = delete;
  GeomGridEval_Curve& operator=(const GeomGridEval_Curve&) = delete;
  GeomGridEval_Curve(GeomGridEval_Curve&&)                 = delete;
  GeomGridEval_Curve& operator=(GeomGridEval_Curve&&)      = delete;

  //! Initialize from adaptor reference (auto-detects curve type).
  //! For GeomAdaptor_Curve, extracts underlying Geom_Curve for optimized evaluation.
  //! For other adaptors, stores reference for fallback evaluation.
  //! @note The curve adaptor reference must remain valid during the lifetime
  //!       of this evaluator when using fallback evaluation.
  //! @param theCurve curve adaptor reference to evaluate
  Standard_EXPORT void Initialize(const Adaptor3d_Curve& theCurve);

  //! Initialize from geometry handle (auto-detects curve type).
  //! @param theCurve geometry to evaluate
  Standard_EXPORT void Initialize(const occ::handle<Geom_Curve>& theCurve);

  //! Returns true if properly initialized.
  Standard_EXPORT bool IsInitialized() const;

  //! Evaluate grid points at all parameters.
  //! @param theParams array of parameter values
  //! @return array of 3D points (1-based indexing)
  Standard_EXPORT NCollection_Array1<gp_Pnt> EvaluateGrid(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate grid points with first derivative.
  //! @param theParams array of parameter values
  //! @return array of CurveD1 (1-based indexing)
  Standard_EXPORT NCollection_Array1<GeomGridEval::CurveD1> EvaluateGridD1(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate grid points with first and second derivatives.
  //! @param theParams array of parameter values
  //! @return array of CurveD2 (1-based indexing)
  Standard_EXPORT NCollection_Array1<GeomGridEval::CurveD2> EvaluateGridD2(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate grid points with first, second, and third derivatives.
  //! @param theParams array of parameter values
  //! @return array of CurveD3 (1-based indexing)
  Standard_EXPORT NCollection_Array1<GeomGridEval::CurveD3> EvaluateGridD3(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate Nth derivative at all grid points.
  //! @param theParams array of parameter values
  //! @param theN derivative order (N >= 1)
  //! @return array of derivative vectors (1-based indexing)
  Standard_EXPORT NCollection_Array1<gp_Vec> EvaluateGridDN(
    const NCollection_Array1<double>& theParams,
    int                               theN) const;

  //! Returns the detected curve type.
  GeomAbs_CurveType GetType() const { return myCurveType; }

  //! Returns true if a transformation is applied.
  bool HasTransformation() const { return myTrsf.has_value(); }

  //! Returns the transformation (empty if not set).
  const std::optional<gp_Trsf>& GetTransformation() const { return myTrsf; }

private:
  //! Apply transformation to grid of points.
  void applyTransformation(NCollection_Array1<gp_Pnt>& theGrid) const;

  //! Apply transformation to grid of D1 results.
  void applyTransformation(NCollection_Array1<GeomGridEval::CurveD1>& theGrid) const;

  //! Apply transformation to grid of D2 results.
  void applyTransformation(NCollection_Array1<GeomGridEval::CurveD2>& theGrid) const;

  //! Apply transformation to grid of D3 results.
  void applyTransformation(NCollection_Array1<GeomGridEval::CurveD3>& theGrid) const;

  //! Apply transformation to grid of vectors.
  void applyTransformation(NCollection_Array1<gp_Vec>& theGrid) const;

  EvaluatorVariant       myEvaluator;
  GeomAbs_CurveType      myCurveType;
  std::optional<gp_Trsf> myTrsf; //!< Optional transformation for TransformedCurve/BRepAdaptor
};

#endif // _GeomGridEval_Curve_HeaderFile
