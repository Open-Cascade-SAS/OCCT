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

#ifndef _GeomGridEvaluator_Plane_HeaderFile
#define _GeomGridEvaluator_Plane_HeaderFile

#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TColStd_Array1OfReal.hxx>

//! @brief Efficient batch evaluator for plane grid points.
//!
//! Uses direct analytical formula: P(u,v) = Location + u * XDir + v * YDir
//! This is a header-only implementation for maximum performance.
//!
//! Usage:
//! @code
//!   GeomGridEvaluator_Plane anEvaluator;
//!   anEvaluator.Initialize(myPlane);
//!   anEvaluator.SetUParams(myUParams);
//!   anEvaluator.SetVParams(myVParams);
//!   NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid();
//! @endcode
class GeomGridEvaluator_Plane
{
public:
  DEFINE_STANDARD_ALLOC

  //! Default constructor - creates uninitialized evaluator.
  GeomGridEvaluator_Plane()
      : myIsInitialized(false)
  {
  }

  //! Initialize with plane geometry.
  //! @param thePlane the plane to evaluate
  void Initialize(const gp_Pln& thePlane)
  {
    myLocation      = thePlane.Location();
    myXDir          = thePlane.Position().XDirection();
    myYDir          = thePlane.Position().YDirection();
    myIsInitialized = true;
  }

  //! Set U parameters for grid evaluation.
  //! @param theParams array of U parameter values (1-based)
  void SetUParams(const TColStd_Array1OfReal& theParams)
  {
    myUParams.Resize(theParams.Lower(), theParams.Upper(), false);
    for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
    {
      myUParams.SetValue(i, theParams.Value(i));
    }
  }

  //! Set V parameters for grid evaluation.
  //! @param theParams array of V parameter values (1-based)
  void SetVParams(const TColStd_Array1OfReal& theParams)
  {
    myVParams.Resize(theParams.Lower(), theParams.Upper(), false);
    for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
    {
      myVParams.SetValue(i, theParams.Value(i));
    }
  }

  //! Returns true if the evaluator is properly initialized.
  bool IsInitialized() const { return myIsInitialized; }

  //! Returns number of U parameters.
  int NbUParams() const { return myUParams.Size(); }

  //! Returns number of V parameters.
  int NbVParams() const { return myVParams.Size(); }

  //! Evaluate all grid points.
  //! @return 2D array of evaluated points (1-based indexing),
  //!         or empty array if not initialized or no parameters set
  NCollection_Array2<gp_Pnt> EvaluateGrid() const
  {
    if (!myIsInitialized || myUParams.IsEmpty() || myVParams.IsEmpty())
    {
      return NCollection_Array2<gp_Pnt>();
    }

    const int aRowLower = myUParams.Lower();
    const int aRowUpper = myUParams.Upper();
    const int aColLower = myVParams.Lower();
    const int aColUpper = myVParams.Upper();

    NCollection_Array2<gp_Pnt> aResult(aRowLower, aRowUpper, aColLower, aColUpper);

    // Pre-extract coordinates for performance
    const double aLocX = myLocation.X();
    const double aLocY = myLocation.Y();
    const double aLocZ = myLocation.Z();
    const double aXX   = myXDir.X();
    const double aXY   = myXDir.Y();
    const double aXZ   = myXDir.Z();
    const double aYX   = myYDir.X();
    const double aYY   = myYDir.Y();
    const double aYZ   = myYDir.Z();

    for (int iU = aRowLower; iU <= aRowUpper; ++iU)
    {
      const double u  = myUParams.Value(iU);
      const double uX = aLocX + u * aXX;
      const double uY = aLocY + u * aXY;
      const double uZ = aLocZ + u * aXZ;

      for (int iV = aColLower; iV <= aColUpper; ++iV)
      {
        const double v = myVParams.Value(iV);
        aResult.SetValue(iU, iV, gp_Pnt(uX + v * aYX, uY + v * aYY, uZ + v * aYZ));
      }
    }
    return aResult;
  }

private:
  gp_Pnt                     myLocation;
  gp_Dir                     myXDir;
  gp_Dir                     myYDir;
  NCollection_Array1<double> myUParams;
  NCollection_Array1<double> myVParams;
  bool                       myIsInitialized;
};

#endif // _GeomGridEvaluator_Plane_HeaderFile
