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

#ifndef _Geom2dGridEval_Parabola_HeaderFile
#define _Geom2dGridEval_Parabola_HeaderFile

#include <Geom2d_Parabola.hxx>
#include <Geom2dGridEval.hxx>
#include <NCollection_Array1.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Efficient batch evaluator for 2D parabola grid points.
//!
//! Uses analytical formula:
//! P(u) = Center + (u^2 / (4*Focal)) * XDir + u * YDir
//!
//! Usage:
//! @code
//!   Geom2dGridEval_Parabola anEvaluator(myGeom2dParabola);
//!   NCollection_Array1<gp_Pnt2d> aGrid = anEvaluator.EvaluateGrid(myParams);
//! @endcode
class Geom2dGridEval_Parabola
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param theParabola the 2D parabola geometry to evaluate
  Geom2dGridEval_Parabola(const occ::handle<Geom2d_Parabola>& theParabola)
      : myGeom(theParabola)
  {
  }

  //! Non-copyable and non-movable.
  Geom2dGridEval_Parabola(const Geom2dGridEval_Parabola&)            = delete;
  Geom2dGridEval_Parabola& operator=(const Geom2dGridEval_Parabola&) = delete;
  Geom2dGridEval_Parabola(Geom2dGridEval_Parabola&&)                 = delete;
  Geom2dGridEval_Parabola& operator=(Geom2dGridEval_Parabola&&)      = delete;

  //! Returns the geometry handle.
  const occ::handle<Geom2d_Parabola>& Geometry() const { return myGeom; }

  //! Evaluate all grid points.
  //! @param theParams array of parameter values
  //! @return array of evaluated points (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  Standard_EXPORT NCollection_Array1<gp_Pnt2d> EvaluateGrid(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate all grid points with first derivative.
  //! @param theParams array of parameter values
  //! @return array of CurveD1 (1-based indexing)
  Standard_EXPORT NCollection_Array1<Geom2dGridEval::CurveD1> EvaluateGridD1(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate all grid points with first and second derivatives.
  //! @param theParams array of parameter values
  //! @return array of CurveD2 (1-based indexing)
  Standard_EXPORT NCollection_Array1<Geom2dGridEval::CurveD2> EvaluateGridD2(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate all grid points with first, second, and third derivatives.
  //! @param theParams array of parameter values
  //! @return array of CurveD3 (1-based indexing)
  Standard_EXPORT NCollection_Array1<Geom2dGridEval::CurveD3> EvaluateGridD3(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate Nth derivative at all grid points.
  //! Parabola: P = Center + (u^2/4F) * X + u * Y
  //! D1 = (u/2F) * X + Y (depends on u)
  //! D2 = (1/2F) * X (constant)
  //! DN = 0 for N >= 3
  //! @param theParams array of parameter values
  //! @param theN derivative order (N >= 1)
  //! @return array of derivative vectors (1-based indexing)
  Standard_EXPORT NCollection_Array1<gp_Vec2d> EvaluateGridDN(
    const NCollection_Array1<double>& theParams,
    int                               theN) const;

private:
  occ::handle<Geom2d_Parabola> myGeom;
};

#endif // _Geom2dGridEval_Parabola_HeaderFile
