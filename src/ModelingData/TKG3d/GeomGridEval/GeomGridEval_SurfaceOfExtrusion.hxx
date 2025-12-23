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

#ifndef _GeomGridEval_SurfaceOfExtrusion_HeaderFile
#define _GeomGridEval_SurfaceOfExtrusion_HeaderFile

#include <GeomGridEval.hxx>
#include <GeomGridEval_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <gp_Dir.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TColStd_Array1OfReal.hxx>

//! @brief Optimized batch evaluator for linear extrusion surface grid points.
//!
//! Evaluates the extrusion surface formula:
//! S(u, v) = C(u) + v * Direction
//!
//! Where:
//! - C(u) is the basis curve
//! - v is the extrusion parameter
//! - Direction is the extrusion direction (unit vector)
//!
//! Optimization: Uses GeomGridEval_Curve for batch evaluation of the basis curve,
//! then applies the linear shift v*Direction for each grid point.
//! This is more efficient than point-by-point evaluation.
//!
//! Usage:
//! @code
//!   GeomGridEval_SurfaceOfExtrusion anEvaluator(myExtrusionSurface);
//!   anEvaluator.SetUVParams(myUParams, myVParams);
//!   NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid();
//! @endcode
class GeomGridEval_SurfaceOfExtrusion
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param theExtrusion the extrusion surface geometry to evaluate
  Standard_EXPORT GeomGridEval_SurfaceOfExtrusion(
    const Handle(Geom_SurfaceOfLinearExtrusion)& theExtrusion);

  //! Non-copyable and non-movable.
  GeomGridEval_SurfaceOfExtrusion(const GeomGridEval_SurfaceOfExtrusion&)            = delete;
  GeomGridEval_SurfaceOfExtrusion& operator=(const GeomGridEval_SurfaceOfExtrusion&) = delete;
  GeomGridEval_SurfaceOfExtrusion(GeomGridEval_SurfaceOfExtrusion&&)                 = delete;
  GeomGridEval_SurfaceOfExtrusion& operator=(GeomGridEval_SurfaceOfExtrusion&&)      = delete;

  //! Set UV parameters from two 1D arrays.
  //! @param theUParams array of U parameter values (curve parameter)
  //! @param theVParams array of V parameter values (extrusion distance)
  Standard_EXPORT void SetUVParams(const TColStd_Array1OfReal& theUParams,
                                   const TColStd_Array1OfReal& theVParams);

  //! Returns the geometry handle.
  const Handle(Geom_SurfaceOfLinearExtrusion)& Geometry() const { return myGeom; }

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

  //! Evaluate all grid points with derivatives up to third order.
  //! @return 2D array of SurfD3 (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD3> EvaluateGridD3() const;

  //! Evaluate partial derivative d^(NU+NV)S/(dU^NU dV^NV) at all grid points.
  //! @param theNU derivative order in U direction
  //! @param theNV derivative order in V direction
  //! @return 2D array of derivative vectors (1-based indexing)
  Standard_EXPORT NCollection_Array2<gp_Vec> EvaluateGridDN(int theNU, int theNV) const;

private:
  Handle(Geom_SurfaceOfLinearExtrusion) myGeom;
  Handle(Geom_Curve)                    myBasisCurve;
  gp_Dir                                myDirection;
  NCollection_Array1<double>            myUParams;
  NCollection_Array1<double>            myVParams;
};

#endif // _GeomGridEval_SurfaceOfExtrusion_HeaderFile
