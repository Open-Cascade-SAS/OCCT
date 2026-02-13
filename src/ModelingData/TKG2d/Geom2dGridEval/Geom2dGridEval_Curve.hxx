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

#ifndef _Geom2dGridEval_Curve_HeaderFile
#define _Geom2dGridEval_Curve_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <GeomAbs_CurveType.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dGridEval_BSplineCurve.hxx>
#include <Geom2dGridEval_BezierCurve.hxx>
#include <Geom2dGridEval_Circle.hxx>
#include <Geom2dGridEval_Ellipse.hxx>
#include <Geom2dGridEval_Hyperbola.hxx>
#include <Geom2dGridEval_Line.hxx>
#include <Geom2dGridEval_OffsetCurve.hxx>
#include <Geom2dGridEval_OtherCurve.hxx>
#include <Geom2dGridEval_Parabola.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <variant>

//! @brief Unified grid evaluator for any 2D curve.
//!
//! Uses std::variant for compile-time type safety and zero heap allocation
//! for the evaluator itself. Automatically detects curve type from
//! Adaptor2d_Curve2d and dispatches to the appropriate specialized evaluator.
//!
//! Supported curve types with optimized evaluation:
//! - Line: Direct analytical formula
//! - Circle: Trigonometric formula
//! - Ellipse: Analytical formula
//! - Hyperbola: Analytical formula
//! - Parabola: Analytical formula
//! - BezierCurve: Optimized batch evaluation via BSplCLib
//! - BSplineCurve: Optimized batch evaluation via BSplCLib with span caching
//! - OffsetCurve: Composite evaluation using basis curve batch evaluator
//! - Other: Fallback using Adaptor2d_Curve2d::D0
//!
//! Usage:
//! @code
//!   Geom2dGridEval_Curve anEval;
//!   anEval.Initialize(myAdaptorCurve2d);
//!   // OR
//!   anEval.Initialize(myGeom2dCurve);
//!   NCollection_Array1<gp_Pnt2d> aGrid = anEval.EvaluateGrid(myParams);
//! @endcode
class Geom2dGridEval_Curve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Variant type holding all possible 2D curve evaluators.
  using EvaluatorVariant = std::variant<std::monostate,              // Uninitialized state
                                        Geom2dGridEval_Line,         // Line curve
                                        Geom2dGridEval_Circle,       // Circle curve
                                        Geom2dGridEval_Ellipse,      // Ellipse curve
                                        Geom2dGridEval_Hyperbola,    // Hyperbola curve
                                        Geom2dGridEval_Parabola,     // Parabola curve
                                        Geom2dGridEval_BezierCurve,  // Bezier curve
                                        Geom2dGridEval_BSplineCurve, // B-spline curve
                                        Geom2dGridEval_OffsetCurve,  // Offset curve
                                        Geom2dGridEval_OtherCurve>;  // Fallback for other types

  //! Default constructor - uninitialized state.
  Geom2dGridEval_Curve()
      : myEvaluator(std::monostate{}),
        myCurveType(GeomAbs_OtherCurve)
  {
  }

  //! Non-copyable and non-movable.
  Geom2dGridEval_Curve(const Geom2dGridEval_Curve&)            = delete;
  Geom2dGridEval_Curve& operator=(const Geom2dGridEval_Curve&) = delete;
  Geom2dGridEval_Curve(Geom2dGridEval_Curve&&)                 = delete;
  Geom2dGridEval_Curve& operator=(Geom2dGridEval_Curve&&)      = delete;

  //! Initialize from 2D adaptor reference (auto-detects curve type).
  //! For Geom2dAdaptor_Curve, extracts underlying Geom2d_Curve for optimized evaluation.
  //! For other adaptors, stores reference for fallback evaluation.
  //! @note The curve adaptor reference must remain valid during the lifetime
  //!       of this evaluator when using fallback evaluation.
  //! @param theCurve 2D curve adaptor reference to evaluate
  Standard_EXPORT void Initialize(const Adaptor2d_Curve2d& theCurve);

  //! Initialize from geometry handle (auto-detects curve type).
  //! @param theCurve 2D geometry to evaluate
  Standard_EXPORT void Initialize(const occ::handle<Geom2d_Curve>& theCurve);

  //! Returns true if properly initialized.
  Standard_EXPORT bool IsInitialized() const;

  //! Evaluate grid points at all parameters.
  //! @param theParams array of parameter values
  //! @return array of 2D points (1-based indexing)
  Standard_EXPORT NCollection_Array1<gp_Pnt2d> EvaluateGrid(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate grid points with first derivative.
  //! @param theParams array of parameter values
  //! @return array of CurveD1 (1-based indexing)
  Standard_EXPORT NCollection_Array1<Geom2dGridEval::CurveD1> EvaluateGridD1(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate grid points with first and second derivatives.
  //! @param theParams array of parameter values
  //! @return array of CurveD2 (1-based indexing)
  Standard_EXPORT NCollection_Array1<Geom2dGridEval::CurveD2> EvaluateGridD2(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate grid points with first, second, and third derivatives.
  //! @param theParams array of parameter values
  //! @return array of CurveD3 (1-based indexing)
  Standard_EXPORT NCollection_Array1<Geom2dGridEval::CurveD3> EvaluateGridD3(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate Nth derivative at all grid points.
  //! @param theParams array of parameter values
  //! @param theN derivative order (N >= 1)
  //! @return array of derivative vectors (1-based indexing)
  Standard_EXPORT NCollection_Array1<gp_Vec2d> EvaluateGridDN(
    const NCollection_Array1<double>& theParams,
    int                               theN) const;

  //! Returns the detected curve type.
  GeomAbs_CurveType GetType() const { return myCurveType; }

private:
  EvaluatorVariant  myEvaluator;
  GeomAbs_CurveType myCurveType;
};

#endif // _Geom2dGridEval_Curve_HeaderFile
