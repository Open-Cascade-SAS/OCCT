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

#ifndef _GeomGridEval_Sphere_HeaderFile
#define _GeomGridEval_Sphere_HeaderFile

#include <Geom_SphericalSurface.hxx>
#include <GeomGridEval.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TColStd_Array1OfReal.hxx>

//! @brief Efficient batch evaluator for sphere grid points.
//!
//! Uses analytical formula:
//! P(u,v) = Center + R * (cos(v) * cos(u) * XDir + cos(v) * sin(u) * YDir + sin(v) * ZDir)
//!
//! Where U is longitude (0 to 2*PI) and V is latitude (-PI/2 to PI/2).
//!
//! Usage:
//! @code
//!   GeomGridEval_Sphere anEvaluator(myGeomSphere);
//!   anEvaluator.SetUVParams(myUParams, myVParams);
//!   NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid();
//! @endcode
class GeomGridEval_Sphere
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param theSphere the spherical surface geometry to evaluate
  GeomGridEval_Sphere(const Handle(Geom_SphericalSurface)& theSphere)
      : myGeom(theSphere)
  {
  }

  //! Non-copyable and non-movable.
  GeomGridEval_Sphere(const GeomGridEval_Sphere&)            = delete;
  GeomGridEval_Sphere& operator=(const GeomGridEval_Sphere&) = delete;
  GeomGridEval_Sphere(GeomGridEval_Sphere&&)                 = delete;
  GeomGridEval_Sphere& operator=(GeomGridEval_Sphere&&)      = delete;

  //! Set UV parameters from two 1D arrays.
  //! @param theUParams array of U parameter values (longitude)
  //! @param theVParams array of V parameter values (latitude)
  Standard_EXPORT void SetUVParams(const TColStd_Array1OfReal& theUParams,
                                   const TColStd_Array1OfReal& theVParams);

  //! Returns the geometry handle.
  const Handle(Geom_SphericalSurface)& Geometry() const { return myGeom; }

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
  //! For orders 1-3, reuses EvaluateGridD1/D2/D3.
  //! For orders > 3, uses geometry DN method.
  //! @param theNU derivative order in U direction
  //! @param theNV derivative order in V direction
  //! @return 2D array of derivative vectors (1-based indexing)
  Standard_EXPORT NCollection_Array2<gp_Vec> EvaluateGridDN(int theNU, int theNV) const;

private:
  Handle(Geom_SphericalSurface) myGeom;
  NCollection_Array1<double>    myUParams;
  NCollection_Array1<double>    myVParams;
};

#endif // _GeomGridEval_Sphere_HeaderFile
