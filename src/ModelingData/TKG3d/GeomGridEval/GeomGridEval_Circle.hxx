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

#ifndef _GeomGridEval_Circle_HeaderFile
#define _GeomGridEval_Circle_HeaderFile

#include <Geom_Circle.hxx>
#include <GeomGridEval.hxx>
#include <NCollection_Array1.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TColStd_Array1OfReal.hxx>

//! @brief Efficient batch evaluator for circle grid points.
//!
//! Uses analytical formula: P(u) = Center + R * (cos(u) * XDir + sin(u) * YDir)
//!
//! Usage:
//! @code
//!   GeomGridEval_Circle anEvaluator(myGeomCircle);
//!   NCollection_Array1<gp_Pnt> aGrid = anEvaluator.EvaluateGrid(myParams);
//! @endcode
class GeomGridEval_Circle
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param theCircle the circle geometry to evaluate
  GeomGridEval_Circle(const Handle(Geom_Circle)& theCircle)
      : myGeom(theCircle)
  {
  }

  //! Non-copyable and non-movable.
  GeomGridEval_Circle(const GeomGridEval_Circle&)            = delete;
  GeomGridEval_Circle& operator=(const GeomGridEval_Circle&) = delete;
  GeomGridEval_Circle(GeomGridEval_Circle&&)                 = delete;
  GeomGridEval_Circle& operator=(GeomGridEval_Circle&&)      = delete;

  //! Returns the geometry handle.
  const Handle(Geom_Circle)& Geometry() const { return myGeom; }

  //! Evaluate all grid points.
  //! @param theParams array of parameter values (angles in radians)
  //! @return array of evaluated points (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  Standard_EXPORT NCollection_Array1<gp_Pnt> EvaluateGrid(
    const TColStd_Array1OfReal& theParams) const;

  //! Evaluate all grid points with first derivative.
  //! D1 = R * (-sin(u) * XDir + cos(u) * YDir)
  //! @param theParams array of parameter values
  //! @return array of CurveD1 (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  Standard_EXPORT NCollection_Array1<GeomGridEval::CurveD1> EvaluateGridD1(
    const TColStd_Array1OfReal& theParams) const;

  //! Evaluate all grid points with first and second derivatives.
  //! D2 = R * (-cos(u) * XDir - sin(u) * YDir) = -P (relative to center)
  //! @param theParams array of parameter values
  //! @return array of CurveD2 (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  Standard_EXPORT NCollection_Array1<GeomGridEval::CurveD2> EvaluateGridD2(
    const TColStd_Array1OfReal& theParams) const;

  //! Evaluate all grid points with first, second, and third derivatives.
  //! D3 = R * (sin(u) * XDir - cos(u) * YDir) = -D1
  //! @param theParams array of parameter values
  //! @return array of CurveD3 (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  Standard_EXPORT NCollection_Array1<GeomGridEval::CurveD3> EvaluateGridD3(
    const TColStd_Array1OfReal& theParams) const;

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
  Standard_EXPORT NCollection_Array1<gp_Vec> EvaluateGridDN(const TColStd_Array1OfReal& theParams,
                                                            int                         theN) const;

private:
  Handle(Geom_Circle) myGeom;
};

#endif // _GeomGridEval_Circle_HeaderFile
