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

#ifndef _Geom2dGridEval_Ellipse_HeaderFile
#define _Geom2dGridEval_Ellipse_HeaderFile

#include <Geom2d_Ellipse.hxx>
#include <Geom2dGridEval.hxx>
#include <NCollection_Array1.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Efficient batch evaluator for 2D ellipse grid points.
//!
//! Uses analytical formula:
//! P(u) = Center + MajorRadius * cos(u) * XDir + MinorRadius * sin(u) * YDir
//!
//! Usage:
//! @code
//!   Geom2dGridEval_Ellipse anEvaluator(myGeom2dEllipse);
//!   NCollection_Array1<gp_Pnt2d> aGrid = anEvaluator.EvaluateGrid(myParams);
//! @endcode
class Geom2dGridEval_Ellipse
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param theEllipse the 2D ellipse geometry to evaluate
  Geom2dGridEval_Ellipse(const occ::handle<Geom2d_Ellipse>& theEllipse)
      : myGeom(theEllipse)
  {
  }

  //! Non-copyable and non-movable.
  Geom2dGridEval_Ellipse(const Geom2dGridEval_Ellipse&)            = delete;
  Geom2dGridEval_Ellipse& operator=(const Geom2dGridEval_Ellipse&) = delete;
  Geom2dGridEval_Ellipse(Geom2dGridEval_Ellipse&&)                 = delete;
  Geom2dGridEval_Ellipse& operator=(Geom2dGridEval_Ellipse&&)      = delete;

  //! Returns the geometry handle.
  const occ::handle<Geom2d_Ellipse>& Geometry() const { return myGeom; }

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

  //! Evaluate all grid points with first, second, and third derivatives.
  //! @param theParams array of parameter values
  //! @return array of CurveD3 (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  Standard_EXPORT NCollection_Array1<Geom2dGridEval::CurveD3> EvaluateGridD3(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate Nth derivative at all grid points.
  //! Ellipse has cyclic derivatives with period 4:
  //! D1 = -MajR * sin(u) * X + MinR * cos(u) * Y
  //! D2 = -MajR * cos(u) * X - MinR * sin(u) * Y
  //! D3 = MajR * sin(u) * X - MinR * cos(u) * Y
  //! D4 = MajR * cos(u) * X + MinR * sin(u) * Y = D0, then repeats
  //! @param theParams array of parameter values
  //! @param theN derivative order (N >= 1)
  //! @return array of derivative vectors (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  Standard_EXPORT NCollection_Array1<gp_Vec2d> EvaluateGridDN(
    const NCollection_Array1<double>& theParams,
    int                               theN) const;

private:
  occ::handle<Geom2d_Ellipse> myGeom;
};

#endif // _Geom2dGridEval_Ellipse_HeaderFile
