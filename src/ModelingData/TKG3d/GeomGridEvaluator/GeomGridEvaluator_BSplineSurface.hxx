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

#ifndef _GeomGridEvaluator_BSplineSurface_HeaderFile
#define _GeomGridEvaluator_BSplineSurface_HeaderFile

#include <BSplSLib_GridEvaluator.hxx>
#include <Geom_BSplineSurface.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TColStd_Array1OfReal.hxx>

//! @brief Wrapper around BSplSLib_GridEvaluator for B-spline surfaces.
//!
//! Provides the unified GridEvaluator interface for Geom_BSplineSurface.
//! Internally delegates to BSplSLib_GridEvaluator for optimized evaluation.
//!
//! Usage:
//! @code
//!   GeomGridEvaluator_BSplineSurface anEvaluator;
//!   anEvaluator.Initialize(myBSplineSurface);
//!   anEvaluator.SetUParams(myUParams);
//!   anEvaluator.SetVParams(myVParams);
//!   NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid();
//! @endcode
class GeomGridEvaluator_BSplineSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Default constructor - creates uninitialized evaluator.
  GeomGridEvaluator_BSplineSurface()
      : myIsInitialized(false)
  {
  }

  //! Initialize with B-spline surface.
  //! @param theSurface the B-spline surface to evaluate
  Standard_EXPORT void Initialize(const Handle(Geom_BSplineSurface)& theSurface);

  //! Set U parameters for grid evaluation.
  //! @param theParams array of U parameter values (1-based)
  void SetUParams(const TColStd_Array1OfReal& theParams) { myEvaluator.SetUParams(theParams); }

  //! Set V parameters for grid evaluation.
  //! @param theParams array of V parameter values (1-based)
  void SetVParams(const TColStd_Array1OfReal& theParams) { myEvaluator.SetVParams(theParams); }

  //! Returns true if the evaluator is properly initialized.
  bool IsInitialized() const { return myIsInitialized && myEvaluator.IsInitialized(); }

  //! Returns number of U parameters.
  int NbUParams() const { return myEvaluator.NbUParams(); }

  //! Returns number of V parameters.
  int NbVParams() const { return myEvaluator.NbVParams(); }

  //! Evaluate all grid points.
  //! @return 2D array of evaluated points (1-based indexing),
  //!         or empty array if not initialized or no parameters set
  NCollection_Array2<gp_Pnt> EvaluateGrid() const { return myEvaluator.EvaluateGrid(); }

private:
  BSplSLib_GridEvaluator myEvaluator;
  bool                   myIsInitialized;
};

#endif // _GeomGridEvaluator_BSplineSurface_HeaderFile
