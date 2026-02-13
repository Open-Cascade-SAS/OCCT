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

#ifndef _Geom2dGridEval_OffsetCurve_HeaderFile
#define _Geom2dGridEval_OffsetCurve_HeaderFile

#include <Geom2d_Curve.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2dGridEval.hxx>
#include <NCollection_Array1.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Batch evaluator for 2D offset curve grid points.
//!
//! Evaluates the 2D offset curve formula:
//! P(u) = C(u) + Offset * N / ||N||
//! where N = (D1.Y, -D1.X) is the normal (tangent rotated 90 degrees).
//!
//! Uses Geom2dGridEval_Curve for batch evaluation of the basis curve,
//! then applies offset transformation.
//!
//! Usage:
//! @code
//!   Geom2dGridEval_OffsetCurve anEvaluator(myGeom2dOffset);
//!   NCollection_Array1<gp_Pnt2d> aGrid = anEvaluator.EvaluateGrid(myParams);
//! @endcode
class Geom2dGridEval_OffsetCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param theOffset the 2D offset curve geometry to evaluate
  Geom2dGridEval_OffsetCurve(const occ::handle<Geom2d_OffsetCurve>& theOffset)
      : myGeom(theOffset),
        myOffset(0.0)
  {
    if (!myGeom.IsNull())
    {
      myOffset = myGeom->Offset();
      myBasis  = myGeom->BasisCurve();
    }
  }

  //! Non-copyable and non-movable.
  Geom2dGridEval_OffsetCurve(const Geom2dGridEval_OffsetCurve&)            = delete;
  Geom2dGridEval_OffsetCurve& operator=(const Geom2dGridEval_OffsetCurve&) = delete;
  Geom2dGridEval_OffsetCurve(Geom2dGridEval_OffsetCurve&&)                 = delete;
  Geom2dGridEval_OffsetCurve& operator=(Geom2dGridEval_OffsetCurve&&)      = delete;

  //! Returns the geometry handle.
  const occ::handle<Geom2d_OffsetCurve>& Geometry() const { return myGeom; }

  //! Evaluate all grid points.
  //! @param theParams array of parameter values
  //! @return array of evaluated points (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  Standard_EXPORT NCollection_Array1<gp_Pnt2d> EvaluateGrid(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate all grid points with first derivative.
  //! @param theParams array of parameter values
  //! @return array of CurveD1 (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  Standard_EXPORT NCollection_Array1<Geom2dGridEval::CurveD1> EvaluateGridD1(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate all grid points with first and second derivatives.
  //! @param theParams array of parameter values
  //! @return array of CurveD2 (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  Standard_EXPORT NCollection_Array1<Geom2dGridEval::CurveD2> EvaluateGridD2(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate all grid points with derivatives up to third order.
  //! @param theParams array of parameter values
  //! @return array of CurveD3 (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  Standard_EXPORT NCollection_Array1<Geom2dGridEval::CurveD3> EvaluateGridD3(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate Nth derivative at all grid points.
  //! For orders 1-3, reuses EvaluateGridD1/D2/D3.
  //! For orders > 3, uses basis curve DN method.
  //! @param theParams array of parameter values
  //! @param theN derivative order (N >= 1)
  //! @return array of derivative vectors (1-based indexing)
  Standard_EXPORT NCollection_Array1<gp_Vec2d> EvaluateGridDN(
    const NCollection_Array1<double>& theParams,
    int                               theN) const;

private:
  occ::handle<Geom2d_OffsetCurve> myGeom;
  occ::handle<Geom2d_Curve>       myBasis;
  double                          myOffset;
};

#endif // _Geom2dGridEval_OffsetCurve_HeaderFile
