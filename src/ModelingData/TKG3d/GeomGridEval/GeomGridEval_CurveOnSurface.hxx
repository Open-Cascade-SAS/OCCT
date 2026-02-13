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

#ifndef _GeomGridEval_CurveOnSurface_HeaderFile
#define _GeomGridEval_CurveOnSurface_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_Surface.hxx>
#include <GeomGridEval.hxx>
#include <NCollection_Array1.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

//! @brief Batch evaluator for curve-on-surface using chain rule composition.
//!
//! Evaluates P(t) = S(u(t), v(t)) where (u,v) = C2d(t) by composing
//! 2D curve evaluation with surface evaluation directly.
//! This bypasses the overhead of Adaptor3d_CurveOnSurface (per-point
//! special-case detection, endpoint trimmed surface handling, virtual dispatch).
//!
//! Chain rule formulas:
//! - D0: P = S(u, v)
//! - D1: V = Su*u' + Sv*v'
//! - D2: V2 = Suu*(u')^2 + Svv*(v')^2 + 2*Suv*u'*v' + Su*u'' + Sv*v''
//! - D3: Complex formula using SetLinearForm helper
//!
//! @note The 2D curve and surface adaptor handles are stored to keep
//!       the underlying adaptors alive during evaluation.
//!
//! Usage:
//! @code
//!   GeomGridEval_CurveOnSurface anEvaluator(aCurve2d, aSurface);
//!   NCollection_Array1<gp_Pnt> aGrid = anEvaluator.EvaluateGrid(myParams);
//! @endcode
class GeomGridEval_CurveOnSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with 2D curve and surface adaptor handles.
  //! @param theCurve2d handle to the 2D curve adaptor
  //! @param theSurface handle to the surface adaptor
  GeomGridEval_CurveOnSurface(const occ::handle<Adaptor2d_Curve2d>& theCurve2d,
                               const occ::handle<Adaptor3d_Surface>& theSurface)
      : myCurve2d(theCurve2d),
        mySurface(theSurface)
  {
  }

  //! Non-copyable and non-movable.
  GeomGridEval_CurveOnSurface(const GeomGridEval_CurveOnSurface&)            = delete;
  GeomGridEval_CurveOnSurface& operator=(const GeomGridEval_CurveOnSurface&) = delete;
  GeomGridEval_CurveOnSurface(GeomGridEval_CurveOnSurface&&)                 = delete;
  GeomGridEval_CurveOnSurface& operator=(GeomGridEval_CurveOnSurface&&)      = delete;

  //! Returns the 2D curve adaptor handle.
  const occ::handle<Adaptor2d_Curve2d>& GetCurve2d() const { return myCurve2d; }

  //! Returns the surface adaptor handle.
  const occ::handle<Adaptor3d_Surface>& GetSurface() const { return mySurface; }

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
  //! @param theParams array of parameter values
  //! @param theN derivative order (N >= 1)
  //! @return array of derivative vectors (1-based indexing)
  Standard_EXPORT NCollection_Array1<gp_Vec> EvaluateGridDN(
    const NCollection_Array1<double>& theParams,
    int                               theN) const;

private:
  occ::handle<Adaptor2d_Curve2d> myCurve2d;
  occ::handle<Adaptor3d_Surface> mySurface;
};

#endif // _GeomGridEval_CurveOnSurface_HeaderFile
