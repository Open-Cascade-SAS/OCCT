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

#ifndef _GeomGridEval_SurfaceOfRevolution_HeaderFile
#define _GeomGridEval_SurfaceOfRevolution_HeaderFile

#include <GeomGridEval.hxx>
#include <GeomGridEval_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TColStd_Array1OfReal.hxx>

//! @brief Optimized batch evaluator for revolution surface grid points.
//!
//! Evaluates the revolution surface formula:
//! S(u, v) = Rotation(u, Axis) * C(v)
//!
//! Where:
//! - C(v) is the meridian (basis curve)
//! - u is the rotation angle around the axis
//! - Axis is the axis of revolution
//!
//! Optimization: Uses GeomGridEval_Curve for batch evaluation of the basis curve,
//! then applies rotation transformations. Precomputes sin/cos values for each U parameter.
//!
//! Mathematical formulas for derivatives:
//! - D1U = Axis × (P - AxisLocation), then rotated
//! - D1V = C'(v), rotated
//! - D2U = (Axis · (P - AxisLocation)) * Axis - (P - AxisLocation), then rotated
//! - D2UV = Axis × C'(v), then rotated
//! - D2V = C''(v), rotated
//!
//! Usage:
//! @code
//!   GeomGridEval_SurfaceOfRevolution anEvaluator(myRevolutionSurface);
//!   anEvaluator.SetUVParams(myUParams, myVParams);
//!   NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid();
//! @endcode
class GeomGridEval_SurfaceOfRevolution
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param theRevolution the revolution surface geometry to evaluate
  Standard_EXPORT GeomGridEval_SurfaceOfRevolution(
    const Handle(Geom_SurfaceOfRevolution)& theRevolution);

  //! Non-copyable and non-movable.
  GeomGridEval_SurfaceOfRevolution(const GeomGridEval_SurfaceOfRevolution&)            = delete;
  GeomGridEval_SurfaceOfRevolution& operator=(const GeomGridEval_SurfaceOfRevolution&) = delete;
  GeomGridEval_SurfaceOfRevolution(GeomGridEval_SurfaceOfRevolution&&)                 = delete;
  GeomGridEval_SurfaceOfRevolution& operator=(GeomGridEval_SurfaceOfRevolution&&)      = delete;

  //! Set UV parameters from two 1D arrays.
  //! @param theUParams array of U parameter values (rotation angle)
  //! @param theVParams array of V parameter values (curve parameter)
  Standard_EXPORT void SetUVParams(const TColStd_Array1OfReal& theUParams,
                                   const TColStd_Array1OfReal& theVParams);

  //! Returns the geometry handle.
  const Handle(Geom_SurfaceOfRevolution)& Geometry() const { return myGeom; }

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

  //! Evaluate partial derivative ∂^(NU+NV)S/(∂U^NU ∂V^NV) at all grid points.
  //! @param theNU derivative order in U direction
  //! @param theNV derivative order in V direction
  //! @return 2D array of derivative vectors (1-based indexing)
  Standard_EXPORT NCollection_Array2<gp_Vec> EvaluateGridDN(int theNU, int theNV) const;

private:
  Handle(Geom_SurfaceOfRevolution) myGeom;
  Handle(Geom_Curve)               myBasisCurve;
  gp_Ax1                           myAxis;
  gp_Pnt                           myAxisLocation;
  gp_Dir                           myAxisDirection;
  NCollection_Array1<double>       myUParams;
  NCollection_Array1<double>       myVParams;
};

#endif // _GeomGridEval_SurfaceOfRevolution_HeaderFile
