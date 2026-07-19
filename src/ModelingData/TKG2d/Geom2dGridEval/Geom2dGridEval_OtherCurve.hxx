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

#ifndef _Geom2dGridEval_OtherCurve_HeaderFile
#define _Geom2dGridEval_OtherCurve_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <Geom2dGridEval.hxx>
#include <NCollection_Array1.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <functional>

//! @brief Fallback evaluator for unknown 2D curve types.
//!
//! Uses Adaptor2d_Curve2d::D0 for point-by-point evaluation.
//! This is the slowest evaluator but handles any 2D curve type.
//!
//! @note The curve adaptor reference must remain valid during the lifetime
//!       of this evaluator. The evaluator does not take ownership.
//!
//! Usage:
//! @code
//!   Geom2dGridEval_OtherCurve anEvaluator(myCurveAdaptor2d);
//!   NCollection_Array1<gp_Pnt2d> aGrid = anEvaluator.EvaluateGrid(myParams);
//! @endcode
class Geom2dGridEval_OtherCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with curve adaptor reference.
  //! @param theCurve reference to 2D curve adaptor (must remain valid)
  Geom2dGridEval_OtherCurve(const Adaptor2d_Curve2d& theCurve)
      : myCurve(theCurve)
  {
  }

  //! Non-copyable and non-movable.
  Geom2dGridEval_OtherCurve(const Geom2dGridEval_OtherCurve&)            = delete;
  Geom2dGridEval_OtherCurve& operator=(const Geom2dGridEval_OtherCurve&) = delete;
  Geom2dGridEval_OtherCurve(Geom2dGridEval_OtherCurve&&)                 = delete;
  Geom2dGridEval_OtherCurve& operator=(Geom2dGridEval_OtherCurve&&)      = delete;

  //! Returns the curve adaptor reference.
  const Adaptor2d_Curve2d& Curve() const { return myCurve.get(); }

  //! Evaluate all grid points.
  //! @param theParams array of parameter values
  //! @return array of evaluated points (1-based indexing),
  //!         or empty array if no parameters
  Standard_EXPORT NCollection_Array1<gp_Pnt2d> EvaluateGrid(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate all grid points with first derivative.
  //! @param theParams array of parameter values
  //! @return array of CurveD1 (1-based indexing),
  //!         or empty array if no parameters
  Standard_EXPORT NCollection_Array1<Geom2dGridEval::CurveD1> EvaluateGridD1(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate all grid points with first and second derivatives.
  //! @param theParams array of parameter values
  //! @return array of CurveD2 (1-based indexing),
  //!         or empty array if no parameters
  Standard_EXPORT NCollection_Array1<Geom2dGridEval::CurveD2> EvaluateGridD2(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate all grid points with first, second, and third derivatives.
  //! @param theParams array of parameter values
  //! @return array of CurveD3 (1-based indexing),
  //!         or empty array if no parameters
  Standard_EXPORT NCollection_Array1<Geom2dGridEval::CurveD3> EvaluateGridD3(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate Nth derivative at all grid points.
  //! For orders 1-3, reuses EvaluateGridD1/D2/D3.
  //! For orders > 3, uses adaptor DN method.
  //! @param theParams array of parameter values
  //! @param theN derivative order (N >= 1)
  //! @return array of derivative vectors (1-based indexing)
  Standard_EXPORT NCollection_Array1<gp_Vec2d> EvaluateGridDN(
    const NCollection_Array1<double>& theParams,
    int                               theN) const;

private:
  std::reference_wrapper<const Adaptor2d_Curve2d> myCurve;
};

#endif // _Geom2dGridEval_OtherCurve_HeaderFile
