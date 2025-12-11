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

#ifndef _GeomGridEval_Plane_HeaderFile
#define _GeomGridEval_Plane_HeaderFile

#include <Geom_Plane.hxx>
#include <GeomGridEval.hxx>
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
//!   GeomGridEval_Plane anEvaluator(myGeomPlane);
//!   anEvaluator.SetUVParams(myUParams, myVParams);
//!   NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid();
//! @endcode
class GeomGridEval_Plane
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param thePlane the plane geometry to evaluate
  GeomGridEval_Plane(const Handle(Geom_Plane)& thePlane)
      : myGeom(thePlane)
  {
  }

  //! Set UV parameters from two 1D arrays.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  void SetUVParams(const TColStd_Array1OfReal& theUParams, const TColStd_Array1OfReal& theVParams)
  {
    const int aNbU = theUParams.Size();
    const int aNbV = theVParams.Size();

    myUParams.Resize(1, aNbU, false);
    for (int i = 1; i <= aNbU; ++i)
    {
      myUParams.SetValue(i, theUParams.Value(theUParams.Lower() + i - 1));
    }

    myVParams.Resize(1, aNbV, false);
    for (int j = 1; j <= aNbV; ++j)
    {
      myVParams.SetValue(j, theVParams.Value(theVParams.Lower() + j - 1));
    }
  }

  //! Returns the geometry handle.
  const Handle(Geom_Plane)& Geometry() const { return myGeom; }

  //! Returns number of U parameters.
  int NbUParams() const { return myUParams.Size(); }

  //! Returns number of V parameters.
  int NbVParams() const { return myVParams.Size(); }

  //! Evaluate all grid points.
  //! @return 2D array of evaluated points (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  NCollection_Array2<gp_Pnt> EvaluateGrid() const
  {
    if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
    {
      return NCollection_Array2<gp_Pnt>();
    }

    const int aNbU = myUParams.Size();
    const int aNbV = myVParams.Size();

    NCollection_Array2<gp_Pnt> aResult(1, aNbU, 1, aNbV);

    // Extract plane data from geometry
    const gp_Pln& aPln  = myGeom->Pln();
    const gp_Pnt& aLoc  = aPln.Location();
    const gp_Dir& aXDir = aPln.Position().XDirection();
    const gp_Dir& aYDir = aPln.Position().YDirection();

    // Pre-extract coordinates for performance
    const double aLocX = aLoc.X();
    const double aLocY = aLoc.Y();
    const double aLocZ = aLoc.Z();
    const double aXX   = aXDir.X();
    const double aXY   = aXDir.Y();
    const double aXZ   = aXDir.Z();
    const double aYX   = aYDir.X();
    const double aYY   = aYDir.Y();
    const double aYZ   = aYDir.Z();

    for (int iU = 1; iU <= aNbU; ++iU)
    {
      const double u  = myUParams.Value(iU);
      const double uX = aLocX + u * aXX;
      const double uY = aLocY + u * aXY;
      const double uZ = aLocZ + u * aXZ;

      for (int iV = 1; iV <= aNbV; ++iV)
      {
        const double v = myVParams.Value(iV);
        aResult.SetValue(iU, iV, gp_Pnt(uX + v * aYX, uY + v * aYY, uZ + v * aYZ));
      }
    }
    return aResult;
  }

  //! Evaluate all grid points with first partial derivatives.
  //! For a plane, D1U = XDir (constant), D1V = YDir (constant).
  //! @return 2D array of SurfD1 (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  NCollection_Array2<GeomGridEval::SurfD1> EvaluateGridD1() const
  {
    if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
    {
      return NCollection_Array2<GeomGridEval::SurfD1>();
    }

    const int aNbU = myUParams.Size();
    const int aNbV = myVParams.Size();

    NCollection_Array2<GeomGridEval::SurfD1> aResult(1, aNbU, 1, aNbV);

    const gp_Pln& aPln  = myGeom->Pln();
    const gp_Pnt& aLoc  = aPln.Location();
    const gp_Dir& aXDir = aPln.Position().XDirection();
    const gp_Dir& aYDir = aPln.Position().YDirection();

    const double aLocX = aLoc.X();
    const double aLocY = aLoc.Y();
    const double aLocZ = aLoc.Z();
    const double aXX   = aXDir.X();
    const double aXY   = aXDir.Y();
    const double aXZ   = aXDir.Z();
    const double aYX   = aYDir.X();
    const double aYY   = aYDir.Y();
    const double aYZ   = aYDir.Z();

    // D1U and D1V are constant for a plane
    const gp_Vec aD1U(aXX, aXY, aXZ);
    const gp_Vec aD1V(aYX, aYY, aYZ);

    for (int iU = 1; iU <= aNbU; ++iU)
    {
      const double u  = myUParams.Value(iU);
      const double uX = aLocX + u * aXX;
      const double uY = aLocY + u * aXY;
      const double uZ = aLocZ + u * aXZ;

      for (int iV = 1; iV <= aNbV; ++iV)
      {
        const double v          = myVParams.Value(iV);
        aResult.ChangeValue(iU,
                            iV) = {gp_Pnt(uX + v * aYX, uY + v * aYY, uZ + v * aYZ), aD1U, aD1V};
      }
    }
    return aResult;
  }

  //! Evaluate all grid points with first and second partial derivatives.
  //! For a plane, all second derivatives are zero.
  //! @return 2D array of SurfD2 (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  NCollection_Array2<GeomGridEval::SurfD2> EvaluateGridD2() const
  {
    if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
    {
      return NCollection_Array2<GeomGridEval::SurfD2>();
    }

    const int aNbU = myUParams.Size();
    const int aNbV = myVParams.Size();

    NCollection_Array2<GeomGridEval::SurfD2> aResult(1, aNbU, 1, aNbV);

    const gp_Pln& aPln  = myGeom->Pln();
    const gp_Pnt& aLoc  = aPln.Location();
    const gp_Dir& aXDir = aPln.Position().XDirection();
    const gp_Dir& aYDir = aPln.Position().YDirection();

    const double aLocX = aLoc.X();
    const double aLocY = aLoc.Y();
    const double aLocZ = aLoc.Z();
    const double aXX   = aXDir.X();
    const double aXY   = aXDir.Y();
    const double aXZ   = aXDir.Z();
    const double aYX   = aYDir.X();
    const double aYY   = aYDir.Y();
    const double aYZ   = aYDir.Z();

    const gp_Vec aD1U(aXX, aXY, aXZ);
    const gp_Vec aD1V(aYX, aYY, aYZ);
    const gp_Vec aZero(0, 0, 0); // All second derivatives are zero for a plane

    for (int iU = 1; iU <= aNbU; ++iU)
    {
      const double u  = myUParams.Value(iU);
      const double uX = aLocX + u * aXX;
      const double uY = aLocY + u * aXY;
      const double uZ = aLocZ + u * aXZ;

      for (int iV = 1; iV <= aNbV; ++iV)
      {
        const double v = myVParams.Value(iV);
        aResult.ChangeValue(
          iU,
          iV) = {gp_Pnt(uX + v * aYX, uY + v * aYY, uZ + v * aYZ), aD1U, aD1V, aZero, aZero, aZero};
      }
    }
    return aResult;
  }

  //! Evaluate all grid points with derivatives up to third order.
  //! For a plane, all derivatives of order >= 2 are zero.
  //! @return 2D array of SurfD3 (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  NCollection_Array2<GeomGridEval::SurfD3> EvaluateGridD3() const
  {
    if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
    {
      return NCollection_Array2<GeomGridEval::SurfD3>();
    }

    const int aNbU = myUParams.Size();
    const int aNbV = myVParams.Size();

    NCollection_Array2<GeomGridEval::SurfD3> aResult(1, aNbU, 1, aNbV);

    const gp_Pln& aPln  = myGeom->Pln();
    const gp_Pnt& aLoc  = aPln.Location();
    const gp_Dir& aXDir = aPln.Position().XDirection();
    const gp_Dir& aYDir = aPln.Position().YDirection();

    const double aLocX = aLoc.X();
    const double aLocY = aLoc.Y();
    const double aLocZ = aLoc.Z();
    const double aXX   = aXDir.X();
    const double aXY   = aXDir.Y();
    const double aXZ   = aXDir.Z();
    const double aYX   = aYDir.X();
    const double aYY   = aYDir.Y();
    const double aYZ   = aYDir.Z();

    const gp_Vec aD1U(aXX, aXY, aXZ);
    const gp_Vec aD1V(aYX, aYY, aYZ);
    const gp_Vec aZero(0, 0, 0); // All derivatives of order >= 2 are zero for a plane

    for (int iU = 1; iU <= aNbU; ++iU)
    {
      const double u  = myUParams.Value(iU);
      const double uX = aLocX + u * aXX;
      const double uY = aLocY + u * aXY;
      const double uZ = aLocZ + u * aXZ;

      for (int iV = 1; iV <= aNbV; ++iV)
      {
        const double v = myVParams.Value(iV);
        aResult.ChangeValue(iU, iV) = {gp_Pnt(uX + v * aYX, uY + v * aYY, uZ + v * aYZ),
                                       aD1U,
                                       aD1V,
                                       aZero,
                                       aZero,
                                       aZero,
                                       aZero,
                                       aZero,
                                       aZero,
                                       aZero};
      }
    }
    return aResult;
  }

private:
  Handle(Geom_Plane)         myGeom;
  NCollection_Array1<double> myUParams;
  NCollection_Array1<double> myVParams;
};

#endif // _GeomGridEval_Plane_HeaderFile
