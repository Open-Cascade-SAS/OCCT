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

#ifndef _Geom2dGridEval_Circle_HeaderFile
#define _Geom2dGridEval_Circle_HeaderFile

#include <Geom2d_Circle.hxx>
#include <Geom2dGridEval.hxx>
#include <NCollection_Array1.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Efficient batch evaluator for 2D circle grid points.
//!
//! Uses analytical formula: P(u) = Center + R * (cos(u) * XDir + sin(u) * YDir)
//!
//! Usage:
//! @code
//!   Geom2dGridEval_Circle anEvaluator(myGeom2dCircle);
//!   NCollection_Array1<gp_Pnt2d> aGrid = anEvaluator.EvaluateGrid(myParams);
//! @endcode
class Geom2dGridEval_Circle
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param theCircle the 2D circle geometry to evaluate
  Geom2dGridEval_Circle(const occ::handle<Geom2d_Circle>& theCircle)
      : myGeom(theCircle)
  {
  }

  //! Non-copyable and non-movable.
  Geom2dGridEval_Circle(const Geom2dGridEval_Circle&)            = delete;
  Geom2dGridEval_Circle& operator=(const Geom2dGridEval_Circle&) = delete;
  Geom2dGridEval_Circle(Geom2dGridEval_Circle&&)                 = delete;
  Geom2dGridEval_Circle& operator=(Geom2dGridEval_Circle&&)      = delete;

  //! Returns the geometry handle.
  const occ::handle<Geom2d_Circle>& Geometry() const { return myGeom; }

  //! Evaluate all grid points.
  //! @param theParams array of parameter values (angles in radians)
  //! @return array of evaluated points (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  Standard_EXPORT NCollection_Array1<gp_Pnt2d> EvaluateGrid(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate all grid points with first derivative.
  //! D1 = R * (-sin(u) * XDir + cos(u) * YDir)
  //! @param theParams array of parameter values
  //! @return array of CurveD1 (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  Standard_EXPORT NCollection_Array1<Geom2dGridEval::CurveD1> EvaluateGridD1(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate all grid points with first and second derivatives.
  //! D2 = R * (-cos(u) * XDir - sin(u) * YDir) = -P (relative to center)
  //! @param theParams array of parameter values
  //! @return array of CurveD2 (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  Standard_EXPORT NCollection_Array1<Geom2dGridEval::CurveD2> EvaluateGridD2(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate all grid points with first, second, and third derivatives.
  //! D3 = R * (sin(u) * XDir - cos(u) * YDir) = -D1
  //! @param theParams array of parameter values
  //! @return array of CurveD3 (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  Standard_EXPORT NCollection_Array1<Geom2dGridEval::CurveD3> EvaluateGridD3(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate Nth derivative at all grid points.
  //! Circle has cyclic derivatives with period 4:
  //! D1 = R * (-sin(u) * X + cos(u) * Y)
  //! D2 = R * (-cos(u) * X - sin(u) * Y)
  //! D3 = R * (sin(u) * X - cos(u) * Y)
  //! D4 = R * (cos(u) * X + sin(u) * Y) = D0, then repeats
  //! @param theParams array of parameter values
  //! @param theN derivative order (N >= 1)
  //! @return array of derivative vectors (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  Standard_EXPORT NCollection_Array1<gp_Vec2d> EvaluateGridDN(
    const NCollection_Array1<double>& theParams,
    int                               theN) const;

private:
  occ::handle<Geom2d_Circle> myGeom;
};

#endif // _Geom2dGridEval_Circle_HeaderFile
