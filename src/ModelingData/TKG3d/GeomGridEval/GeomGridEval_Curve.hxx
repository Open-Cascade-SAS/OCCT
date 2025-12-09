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
#include <GeomGridEval_Circle.hxx>
#include <GeomGridEval_Ellipse.hxx>
#include <GeomGridEval_Hyperbola.hxx>
#include <GeomGridEval_Line.hxx>
#include <GeomGridEval_OtherCurve.hxx>
#include <GeomGridEval_Parabola.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TColStd_Array1OfReal.hxx>

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
//! - BSplineCurve: Optimized batch evaluation via BSplCLib_GridEvaluator
//! - Other: Fallback using Adaptor3d_Curve::D0
//!
//! Usage:
//! @code
//!   GeomGridEval_Curve anEval;
//!   anEval.Initialize(myAdaptorCurve);
//!   // OR
//!   anEval.Initialize(myGeomCurve);
//!   anEval.SetParams(myParams);
//!   NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid();
//! @endcode
class GeomGridEval_Curve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Variant type holding all possible curve evaluators.
  using EvaluatorVariant = std::variant<std::monostate,             // Uninitialized state
                                        GeomGridEval_Line,     // Line curve
                                        GeomGridEval_Circle,   // Circle curve
                                        GeomGridEval_Ellipse,  // Ellipse curve
                                        GeomGridEval_Hyperbola, // Hyperbola curve
                                        GeomGridEval_Parabola,  // Parabola curve
                                        GeomGridEval_BSplineCurve, // B-spline curve
                                        GeomGridEval_OtherCurve>;  // Fallback for other types

  //! Default constructor - uninitialized state.
  GeomGridEval_Curve()
      : myEvaluator(std::monostate{}),
        myCurveType(GeomAbs_OtherCurve)
  {
  }

  //! Initialize from adaptor (auto-detects curve type).
  //! @param theCurve curve adaptor to evaluate
  Standard_EXPORT void Initialize(const Handle(Adaptor3d_Curve)& theCurve);

  //! Initialize from geometry handle (auto-detects curve type).
  //! @param theCurve geometry to evaluate
  Standard_EXPORT void Initialize(const Handle(Geom_Curve)& theCurve);

  //! Set parameters for evaluation.
  //! @param theParams array of parameter values (1-based)
  Standard_EXPORT void SetParams(const TColStd_Array1OfReal& theParams);

  //! Returns true if properly initialized.
  Standard_EXPORT bool IsInitialized() const;

  //! Returns number of parameters set.
  Standard_EXPORT int NbParams() const;

  //! Evaluate grid points at all set parameters.
  //! @return array of 3D points (1-based indexing)
  Standard_EXPORT NCollection_Array1<gp_Pnt> EvaluateGrid() const;

  //! Evaluate grid points with first derivative.
  //! @return array of CurveD1 (1-based indexing)
  Standard_EXPORT NCollection_Array1<GeomGridEval::CurveD1> EvaluateGridD1() const;

  //! Evaluate grid points with first and second derivatives.
  //! @return array of CurveD2 (1-based indexing)
  Standard_EXPORT NCollection_Array1<GeomGridEval::CurveD2> EvaluateGridD2() const;

  //! Evaluate grid points with first, second, and third derivatives.
  //! @return array of CurveD3 (1-based indexing)
  Standard_EXPORT NCollection_Array1<GeomGridEval::CurveD3> EvaluateGridD3() const;

  //! Returns the detected curve type.
  GeomAbs_CurveType GetType() const { return myCurveType; }

private:
  EvaluatorVariant  myEvaluator;
  GeomAbs_CurveType myCurveType;
};

#endif // _GeomGridEval_Curve_HeaderFile
