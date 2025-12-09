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

#ifndef _GeomGridEvaluator_Curve_HeaderFile
#define _GeomGridEvaluator_Curve_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomGridEvaluator_BSplineCurve.hxx>
#include <GeomGridEvaluator_Circle.hxx>
#include <GeomGridEvaluator_Line.hxx>
#include <GeomGridEvaluator_OtherCurve.hxx>
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
//! - BSplineCurve: Optimized batch evaluation via BSplCLib_GridEvaluator
//! - Other: Fallback using Adaptor3d_Curve::D0
//!
//! Usage:
//! @code
//!   GeomGridEvaluator_Curve anEval;
//!   anEval.Initialize(myAdaptorCurve);
//!   anEval.SetParams(myParams);
//!   NCollection_Array1<gp_Pnt> aGrid = anEval.EvaluateGrid();
//! @endcode
class GeomGridEvaluator_Curve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Variant type holding all possible curve evaluators.
  using EvaluatorVariant = std::variant<std::monostate,             // Uninitialized state
                                        GeomGridEvaluator_Line,     // Line curve
                                        GeomGridEvaluator_Circle,   // Circle curve
                                        GeomGridEvaluator_BSplineCurve, // B-spline curve
                                        GeomGridEvaluator_OtherCurve>;  // Fallback for other types

  //! Default constructor - uninitialized state.
  GeomGridEvaluator_Curve()
      : myEvaluator(std::monostate{}),
        myCurveType(GeomAbs_OtherCurve)
  {
  }

  //! Initialize from adaptor (auto-detects curve type).
  //! @param theCurve curve adaptor to evaluate
  Standard_EXPORT void Initialize(const Adaptor3d_Curve& theCurve);

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

  //! Returns the detected curve type.
  GeomAbs_CurveType GetType() const { return myCurveType; }

private:
  EvaluatorVariant  myEvaluator;
  GeomAbs_CurveType myCurveType;
};

#endif // _GeomGridEvaluator_Curve_HeaderFile
