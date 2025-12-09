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

#ifndef _GeomGridEval_BezierSurface_HeaderFile
#define _GeomGridEval_BezierSurface_HeaderFile

#include <BSplSLib_Cache.hxx>
#include <Geom_BezierSurface.hxx>
#include <GeomGridEval.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TColStd_Array1OfReal.hxx>

//! @brief Efficient batch evaluator for Bezier surface grid points.
//!
//! Uses BSplSLib_Cache for optimized polynomial evaluation.
//! Bezier surfaces are treated as single-span B-spline surfaces.
//!
//! Usage:
//! @code
//!   GeomGridEval_BezierSurface anEvaluator(myGeomBezier);
//!   anEvaluator.SetUVParams(myUParams, myVParams);
//!   NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid();
//! @endcode
class GeomGridEval_BezierSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param theBezier the bezier surface geometry to evaluate
  GeomGridEval_BezierSurface(const Handle(Geom_BezierSurface)& theBezier)
      : myGeom(theBezier)
  {
  }

  //! Set UV parameters from two 1D arrays.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  Standard_EXPORT void SetUVParams(const TColStd_Array1OfReal& theUParams,
                                   const TColStd_Array1OfReal& theVParams);

  //! Returns the geometry handle.
  const Handle(Geom_BezierSurface)& Geometry() const { return myGeom; }

  //! Returns number of U parameters.
  int NbUParams() const { return myUParams.Size(); }

  //! Returns number of V parameters.
  int NbVParams() const { return myVParams.Size(); }

  //! Evaluate all grid points.
  //! @return 2D array of evaluated points (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array2<gp_Pnt> EvaluateGrid() const;

  //! Evaluate all grid points with first partial derivatives.
  //! @return 2D array of SurfD1 (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD1> EvaluateGridD1() const;

  //! Evaluate all grid points with first and second partial derivatives.
  //! @return 2D array of SurfD2 (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD2> EvaluateGridD2() const;

private:
  //! Build cache for the Bezier surface (single span).
  void buildCache() const;

private:
  Handle(Geom_BezierSurface)       myGeom;
  NCollection_Array1<double>       myUParams;
  NCollection_Array1<double>       myVParams;
  mutable Handle(BSplSLib_Cache)   myCache;
};

#endif // _GeomGridEval_BezierSurface_HeaderFile
