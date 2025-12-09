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

#ifndef _GeomGridEvaluator_Line_HeaderFile
#define _GeomGridEvaluator_Line_HeaderFile

#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TColStd_Array1OfReal.hxx>

//! @brief Efficient batch evaluator for line grid points.
//!
//! Uses direct analytical formula: P(t) = Location + t * Direction
//! This is a header-only implementation for maximum performance.
//!
//! Usage:
//! @code
//!   GeomGridEvaluator_Line anEvaluator;
//!   anEvaluator.Initialize(myLine);
//!   anEvaluator.SetParams(myParams);
//!   NCollection_Array1<gp_Pnt> aGrid = anEvaluator.EvaluateGrid();
//! @endcode
class GeomGridEvaluator_Line
{
public:
  DEFINE_STANDARD_ALLOC

  //! Default constructor - creates uninitialized evaluator.
  GeomGridEvaluator_Line()
      : myIsInitialized(false)
  {
  }

  //! Initialize with line geometry.
  //! @param theLine the line to evaluate
  void Initialize(const gp_Lin& theLine)
  {
    myLocation      = theLine.Location();
    myDirection     = theLine.Direction();
    myIsInitialized = true;
  }

  //! Set parameters for grid evaluation.
  //! @param theParams array of parameter values (1-based)
  void SetParams(const TColStd_Array1OfReal& theParams)
  {
    myParams.Resize(theParams.Lower(), theParams.Upper(), false);
    for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
    {
      myParams.SetValue(i, theParams.Value(i));
    }
  }

  //! Returns true if the evaluator is properly initialized.
  bool IsInitialized() const { return myIsInitialized; }

  //! Returns number of parameters.
  int NbParams() const { return myParams.Size(); }

  //! Evaluate all grid points.
  //! @return array of evaluated points (1-based indexing),
  //!         or empty array if not initialized or no parameters set
  NCollection_Array1<gp_Pnt> EvaluateGrid() const
  {
    if (!myIsInitialized || myParams.IsEmpty())
    {
      return NCollection_Array1<gp_Pnt>();
    }

    NCollection_Array1<gp_Pnt> aResult(myParams.Lower(), myParams.Upper());

    // Pre-extract coordinates for vectorization potential
    const double aLocX = myLocation.X();
    const double aLocY = myLocation.Y();
    const double aLocZ = myLocation.Z();
    const double aDirX = myDirection.X();
    const double aDirY = myDirection.Y();
    const double aDirZ = myDirection.Z();

    for (int i = myParams.Lower(); i <= myParams.Upper(); ++i)
    {
      const double t = myParams.Value(i);
      aResult.SetValue(i, gp_Pnt(aLocX + t * aDirX, aLocY + t * aDirY, aLocZ + t * aDirZ));
    }
    return aResult;
  }

private:
  gp_Pnt                       myLocation;
  gp_Dir                       myDirection;
  NCollection_Array1<double> myParams;
  bool                         myIsInitialized;
};

#endif // _GeomGridEvaluator_Line_HeaderFile
