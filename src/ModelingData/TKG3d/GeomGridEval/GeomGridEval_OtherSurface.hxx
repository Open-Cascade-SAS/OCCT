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
#include <GeomGridEval.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TColStd_Array1OfReal.hxx>

#include <functional>

//! @brief Fallback evaluator for unknown surface types.
//!
//! Uses Adaptor3d_Surface::D0 for point-by-point evaluation.
//! This is the slowest evaluator but handles any surface type.
//!
//! @note The surface adaptor reference must remain valid during the lifetime
//!       of this evaluator. The evaluator does not take ownership.
//!
//! Usage:
//! @code
//!   GeomGridEval_OtherSurface anEvaluator(mySurfaceAdaptor);
//!   anEvaluator.SetUVParams(myUParams, myVParams);
//!   NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid();
//! @endcode
class GeomGridEval_OtherSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with surface adaptor reference.
  //! @param theSurface reference to surface adaptor (must remain valid)
  GeomGridEval_OtherSurface(const Adaptor3d_Surface& theSurface)
      : mySurface(theSurface)
  {
  }

  //! Non-copyable and non-movable.
  GeomGridEval_OtherSurface(const GeomGridEval_OtherSurface&)            = delete;
  GeomGridEval_OtherSurface& operator=(const GeomGridEval_OtherSurface&) = delete;
  GeomGridEval_OtherSurface(GeomGridEval_OtherSurface&&)                 = delete;
  GeomGridEval_OtherSurface& operator=(GeomGridEval_OtherSurface&&)      = delete;

  //! Set UV parameters from two 1D arrays.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  Standard_EXPORT void SetUVParams(const TColStd_Array1OfReal& theUParams,
                                   const TColStd_Array1OfReal& theVParams);

  //! Returns the surface adaptor reference.
  const Adaptor3d_Surface& Surface() const { return mySurface.get(); }

  //! Returns number of U parameters.
  int NbUParams() const { return myUParams.Size(); }

  //! Returns number of V parameters.
  int NbVParams() const { return myVParams.Size(); }

  //! Evaluate all grid points.
  //! @return 2D array of evaluated points (1-based indexing),
  //!         or empty array if surface is null or no parameters set
  Standard_EXPORT NCollection_Array2<gp_Pnt> EvaluateGrid() const;

  //! Evaluate all grid points with first partial derivatives.
  //! @return 2D array of SurfD1 (1-based indexing),
  //!         or empty array if surface is null or no parameters set
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD1> EvaluateGridD1() const;

  //! Evaluate all grid points with first and second partial derivatives.
  //! @return 2D array of SurfD2 (1-based indexing),
  //!         or empty array if surface is null or no parameters set
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD2> EvaluateGridD2() const;

  //! Evaluate all grid points with derivatives up to third order.
  //! Uses Adaptor3d_Surface::D3 for evaluation.
  //! @return 2D array of SurfD3 (1-based indexing),
  //!         or empty array if surface is null or no parameters set
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD3> EvaluateGridD3() const;

  //! Evaluate partial derivative ∂^(NU+NV)S/(∂U^NU ∂V^NV) at all grid points.
  //! Uses Adaptor3d_Surface::DN for evaluation.
  //! @param theNU derivative order in U direction
  //! @param theNV derivative order in V direction
  //! @return 2D array of derivative vectors (1-based indexing)
  Standard_EXPORT NCollection_Array2<gp_Vec> EvaluateGridDN(int theNU, int theNV) const;

private:
  std::reference_wrapper<const Adaptor3d_Surface> mySurface;
  NCollection_Array1<double>                      myUParams;
  NCollection_Array1<double>                      myVParams;
};

#endif // _GeomGridEval_OtherSurface_HeaderFile
