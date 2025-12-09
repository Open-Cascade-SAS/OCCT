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

#ifndef _GeomGridEvaluator_OtherSurface_HeaderFile
#define _GeomGridEvaluator_OtherSurface_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <TColStd_Array1OfReal.hxx>

//! @brief Fallback evaluator for unknown surface types.
//!
//! Uses Adaptor3d_Surface::D0 for point-by-point evaluation.
//! This is the slowest evaluator but handles any surface type.
//!
//! Usage:
//! @code
//!   GeomGridEvaluator_OtherSurface anEvaluator(mySurfaceAdaptor);
//!   anEvaluator.SetUVParams(myUParams, myVParams);
//!   NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid();
//! @endcode
class GeomGridEvaluator_OtherSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with surface adaptor.
  //! @param theSurface handle to surface adaptor
  GeomGridEvaluator_OtherSurface(const Handle(Adaptor3d_Surface)& theSurface)
      : mySurface(theSurface)
  {
  }

  //! Set UV parameters from two 1D arrays.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  Standard_EXPORT void SetUVParams(const TColStd_Array1OfReal& theUParams,
                                   const TColStd_Array1OfReal& theVParams);

  //! Returns the surface adaptor handle.
  const Handle(Adaptor3d_Surface)& Surface() const { return mySurface; }

  //! Returns number of U parameters.
  int NbUParams() const { return myUParams.Size(); }

  //! Returns number of V parameters.
  int NbVParams() const { return myVParams.Size(); }

  //! Evaluate all grid points.
  //! @return 2D array of evaluated points (1-based indexing),
  //!         or empty array if surface is null or no parameters set
  Standard_EXPORT NCollection_Array2<gp_Pnt> EvaluateGrid() const;

private:
  Handle(Adaptor3d_Surface)   mySurface;
  NCollection_Array1<double>  myUParams;
  NCollection_Array1<double>  myVParams;
};

#endif // _GeomGridEvaluator_OtherSurface_HeaderFile
