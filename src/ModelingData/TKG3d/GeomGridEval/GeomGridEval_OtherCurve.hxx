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

#ifndef _GeomGridEval_OtherCurve_HeaderFile
#define _GeomGridEval_OtherCurve_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <GeomGridEval.hxx>
#include <NCollection_Array1.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <functional>

//! @brief Fallback evaluator for unknown curve types.
//!
//! Uses Adaptor3d_Curve::D0 for point-by-point evaluation.
//! This is the slowest evaluator but handles any curve type.
//!
//! @note The curve adaptor reference must remain valid during the lifetime
//!       of this evaluator. The evaluator does not take ownership.
//!
//! Usage:
//! @code
//!   GeomGridEval_OtherCurve anEvaluator(myCurveAdaptor);
//!   NCollection_Array1<gp_Pnt> aGrid = anEvaluator.EvaluateGrid(myParams);
//! @endcode
class GeomGridEval_OtherCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with curve adaptor reference.
  //! @param theCurve reference to curve adaptor (must remain valid)
  GeomGridEval_OtherCurve(const Adaptor3d_Curve& theCurve)
      : myCurve(theCurve)
  {
  }

  //! Non-copyable and non-movable.
  GeomGridEval_OtherCurve(const GeomGridEval_OtherCurve&)            = delete;
  GeomGridEval_OtherCurve& operator=(const GeomGridEval_OtherCurve&) = delete;
  GeomGridEval_OtherCurve(GeomGridEval_OtherCurve&&)                 = delete;
  GeomGridEval_OtherCurve& operator=(GeomGridEval_OtherCurve&&)      = delete;

  //! Returns the curve adaptor reference.
  const Adaptor3d_Curve& Curve() const { return myCurve.get(); }

  //! Evaluate all grid points.
  //! @param theParams array of parameter values
  //! @return array of evaluated points (1-based indexing),
  //!         or empty array if no parameters
  Standard_EXPORT NCollection_Array1<gp_Pnt> EvaluateGrid(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate all grid points with first derivative.
  //! @param theParams array of parameter values
  //! @return array of CurveD1 (1-based indexing),
  //!         or empty array if no parameters
  Standard_EXPORT NCollection_Array1<GeomGridEval::CurveD1> EvaluateGridD1(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate all grid points with first and second derivatives.
  //! @param theParams array of parameter values
  //! @return array of CurveD2 (1-based indexing),
  //!         or empty array if no parameters
  Standard_EXPORT NCollection_Array1<GeomGridEval::CurveD2> EvaluateGridD2(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate all grid points with first, second, and third derivatives.
  //! @param theParams array of parameter values
  //! @return array of CurveD3 (1-based indexing),
  //!         or empty array if no parameters
  Standard_EXPORT NCollection_Array1<GeomGridEval::CurveD3> EvaluateGridD3(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate Nth derivative at all grid points.
  //! For orders 1-3, reuses EvaluateGridD1/D2/D3.
  //! For orders > 3, uses adaptor DN method.
  //! @param theParams array of parameter values
  //! @param theN derivative order (N >= 1)
  //! @return array of derivative vectors (1-based indexing)
  Standard_EXPORT NCollection_Array1<gp_Vec> EvaluateGridDN(
    const NCollection_Array1<double>& theParams,
    int                               theN) const;

private:
  std::reference_wrapper<const Adaptor3d_Curve> myCurve;
};

#endif // _GeomGridEval_OtherCurve_HeaderFile
