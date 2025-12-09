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

#ifndef _GeomGridEvaluator_BSplineCurve_HeaderFile
#define _GeomGridEvaluator_BSplineCurve_HeaderFile

#include <BSplCLib_GridEvaluator.hxx>
#include <Geom_BSplineCurve.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TColStd_Array1OfReal.hxx>

//! @brief Wrapper around BSplCLib_GridEvaluator for B-spline curves.
//!
//! Provides the unified GridEvaluator interface for Geom_BSplineCurve.
//! Internally delegates to BSplCLib_GridEvaluator for optimized evaluation.
//!
//! Usage:
//! @code
//!   GeomGridEvaluator_BSplineCurve anEvaluator;
//!   anEvaluator.Initialize(myBSplineCurve);
//!   anEvaluator.SetParams(myParams);
//!   NCollection_Array1<gp_Pnt> aGrid = anEvaluator.EvaluateGrid();
//! @endcode
class GeomGridEvaluator_BSplineCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Default constructor - creates uninitialized evaluator.
  GeomGridEvaluator_BSplineCurve()
      : myIsInitialized(false)
  {
  }

  //! Initialize with B-spline curve.
  //! @param theCurve the B-spline curve to evaluate
  Standard_EXPORT void Initialize(const Handle(Geom_BSplineCurve)& theCurve);

  //! Set parameters for grid evaluation.
  //! @param theParams array of parameter values (1-based)
  void SetParams(const TColStd_Array1OfReal& theParams) { myEvaluator.SetParams(theParams); }

  //! Returns true if the evaluator is properly initialized.
  bool IsInitialized() const { return myIsInitialized && myEvaluator.IsInitialized(); }

  //! Returns number of parameters.
  int NbParams() const { return myEvaluator.NbParams(); }

  //! Evaluate all grid points.
  //! @return array of evaluated points (1-based indexing),
  //!         or empty array if not initialized or no parameters set
  NCollection_Array1<gp_Pnt> EvaluateGrid() const { return myEvaluator.EvaluateGrid(); }

private:
  BSplCLib_GridEvaluator myEvaluator;
  bool                   myIsInitialized;
};

#endif // _GeomGridEvaluator_BSplineCurve_HeaderFile
