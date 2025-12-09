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

#ifndef _GeomGridEvaluator_OtherCurve_HeaderFile
#define _GeomGridEvaluator_OtherCurve_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <TColStd_Array1OfReal.hxx>

//! @brief Fallback evaluator for unknown curve types.
//!
//! Uses Adaptor3d_Curve::D0 for point-by-point evaluation.
//! This is the slowest evaluator but handles any curve type.
//!
//! Usage:
//! @code
//!   GeomGridEvaluator_OtherCurve anEvaluator;
//!   anEvaluator.Initialize(myCurveAdaptor);
//!   anEvaluator.SetParams(myParams);
//!   NCollection_Array1<gp_Pnt> aGrid = anEvaluator.EvaluateGrid();
//! @endcode
class GeomGridEvaluator_OtherCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Default constructor - creates uninitialized evaluator.
  GeomGridEvaluator_OtherCurve()
      : myIsInitialized(false)
  {
  }

  //! Initialize with curve adaptor.
  //! @param theCurve handle to curve adaptor (takes ownership via ShallowCopy)
  void Initialize(const Handle(Adaptor3d_Curve)& theCurve)
  {
    myCurve         = theCurve;
    myIsInitialized = !theCurve.IsNull();
  }

  //! Set parameters for grid evaluation.
  //! @param theParams array of parameter values (1-based)
  void SetParams(const TColStd_Array1OfReal& theParams);

  //! Returns true if the evaluator is properly initialized.
  bool IsInitialized() const { return myIsInitialized; }

  //! Returns number of parameters.
  int NbParams() const { return myParams.Size(); }

  //! Evaluate all grid points.
  //! @return array of evaluated points (1-based indexing),
  //!         or empty array if not initialized or no parameters set
  Standard_EXPORT NCollection_Array1<gp_Pnt> EvaluateGrid() const;

private:
  Handle(Adaptor3d_Curve)    myCurve;
  NCollection_Array1<double> myParams;
  bool                       myIsInitialized;
};

#endif // _GeomGridEvaluator_OtherCurve_HeaderFile
