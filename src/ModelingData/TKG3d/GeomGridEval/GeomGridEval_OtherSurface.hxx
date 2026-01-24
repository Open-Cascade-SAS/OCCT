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

#ifndef _GeomGridEval_OtherSurface_HeaderFile
#define _GeomGridEval_OtherSurface_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <Geom_Surface.hxx>
#include <GeomGridEval.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <variant>

//! @brief Fallback evaluator for any surface type.
//!
//! Uses D0/D1/D2/D3/DN methods for point-by-point evaluation. Supports both
//! Adaptor3d_Surface (by pointer) and occ::handle<Geom_Surface> as input.
//! This is the slowest evaluator but handles any surface type.
//!
//! @note When using adaptor pointer, the adaptor must remain valid
//!       during the lifetime of this evaluator.
//!
//! Usage:
//! @code
//!   GeomGridEval_OtherSurface anEvaluator(&mySurfaceAdaptor);
//!   NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid(myUParams, myVParams);
//! @endcode
class GeomGridEval_OtherSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with surface adaptor pointer.
  //! @param theSurface pointer to surface adaptor (must remain valid)
  GeomGridEval_OtherSurface(const Adaptor3d_Surface* theSurface)
      : mySurface(theSurface)
  {
  }

  //! Constructor with geometry handle.
  //! @param theSurface handle to Geom_Surface
  GeomGridEval_OtherSurface(const occ::handle<Geom_Surface>& theSurface)
      : mySurface(theSurface)
  {
  }

  //! Non-copyable and non-movable.
  GeomGridEval_OtherSurface(const GeomGridEval_OtherSurface&)            = delete;
  GeomGridEval_OtherSurface& operator=(const GeomGridEval_OtherSurface&) = delete;
  GeomGridEval_OtherSurface(GeomGridEval_OtherSurface&&)                 = delete;
  GeomGridEval_OtherSurface& operator=(GeomGridEval_OtherSurface&&)      = delete;

  //! Evaluate grid points at Cartesian product of U and V parameters.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  //! @return 2D array of evaluated points (1-based indexing)
  Standard_EXPORT NCollection_Array2<gp_Pnt> EvaluateGrid(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const;

  //! Evaluate grid points with first partial derivatives.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  //! @return 2D array of SurfD1 (1-based indexing)
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD1> EvaluateGridD1(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const;

  //! Evaluate grid points with first and second partial derivatives.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  //! @return 2D array of SurfD2 (1-based indexing)
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD2> EvaluateGridD2(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const;

  //! Evaluate grid points with derivatives up to third order.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  //! @return 2D array of SurfD3 (1-based indexing)
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD3> EvaluateGridD3(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const;

  //! Evaluate partial derivative at all grid points.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  //! @param theNU derivative order in U direction
  //! @param theNV derivative order in V direction
  //! @return 2D array of derivative vectors (1-based indexing)
  Standard_EXPORT NCollection_Array2<gp_Vec> EvaluateGridDN(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams,
    int                               theNU,
    int                               theNV) const;

private:
  //! Evaluate D0 at given UV parameters.
  void evaluateD0(double theU, double theV, gp_Pnt& thePoint) const;

  //! Evaluate D1 at given UV parameters.
  void evaluateD1(double theU, double theV, gp_Pnt& thePoint, gp_Vec& theD1U, gp_Vec& theD1V) const;

  //! Evaluate D2 at given UV parameters.
  void evaluateD2(double  theU,
                  double  theV,
                  gp_Pnt& thePoint,
                  gp_Vec& theD1U,
                  gp_Vec& theD1V,
                  gp_Vec& theD2U,
                  gp_Vec& theD2V,
                  gp_Vec& theD2UV) const;

  //! Evaluate D3 at given UV parameters.
  void evaluateD3(double  theU,
                  double  theV,
                  gp_Pnt& thePoint,
                  gp_Vec& theD1U,
                  gp_Vec& theD1V,
                  gp_Vec& theD2U,
                  gp_Vec& theD2V,
                  gp_Vec& theD2UV,
                  gp_Vec& theD3U,
                  gp_Vec& theD3V,
                  gp_Vec& theD3UUV,
                  gp_Vec& theD3UVV) const;

  //! Evaluate DN at given UV parameters.
  gp_Vec evaluateDN(double theU, double theV, int theNU, int theNV) const;

private:
  //! Surface source: either adaptor pointer or geometry handle.
  std::variant<const Adaptor3d_Surface*, occ::handle<Geom_Surface>> mySurface;
};

#endif // _GeomGridEval_OtherSurface_HeaderFile
