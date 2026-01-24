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
#include <gp_Pln.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Efficient batch evaluator for plane grid points.
//!
//! Uses direct analytical formula: P(u,v) = Location + u * XDir + v * YDir
//! This is a header-only implementation for maximum performance.
//!
//! Usage:
//! @code
//!   GeomGridEval_Plane anEvaluator(myGeomPlane);
//!   NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid(myUParams, myVParams);
//! @endcode
class GeomGridEval_Plane
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param thePlane the plane geometry to evaluate
  GeomGridEval_Plane(const occ::handle<Geom_Plane>& thePlane)
      : myGeom(thePlane)
  {
  }

  //! Non-copyable and non-movable.
  GeomGridEval_Plane(const GeomGridEval_Plane&)            = delete;
  GeomGridEval_Plane& operator=(const GeomGridEval_Plane&) = delete;
  GeomGridEval_Plane(GeomGridEval_Plane&&)                 = delete;
  GeomGridEval_Plane& operator=(GeomGridEval_Plane&&)      = delete;

  //! Returns the geometry handle.
  const occ::handle<Geom_Plane>& Geometry() const { return myGeom; }

  //! Evaluate grid points at Cartesian product of U and V parameters.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  //! @return 2D array of evaluated points (1-based indexing)
  NCollection_Array2<gp_Pnt> EvaluateGrid(const NCollection_Array1<double>& theUParams,
                                          const NCollection_Array1<double>& theVParams) const
  {
    const int aNbU = theUParams.Size();
    const int aNbV = theVParams.Size();
    if (myGeom.IsNull() || aNbU == 0 || aNbV == 0)
    {
      return NCollection_Array2<gp_Pnt>();
    }

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
      const double u  = theUParams.Value(theUParams.Lower() + iU - 1);
      const double uX = aLocX + u * aXX;
      const double uY = aLocY + u * aXY;
      const double uZ = aLocZ + u * aXZ;

      for (int iV = 1; iV <= aNbV; ++iV)
      {
        const double v = theVParams.Value(theVParams.Lower() + iV - 1);
        aResult.SetValue(iU, iV, gp_Pnt(uX + v * aYX, uY + v * aYY, uZ + v * aYZ));
      }
    }
    return aResult;
  }

  //! Evaluate grid points with first partial derivatives.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  //! @return 2D array of SurfD1 (1-based indexing)
  NCollection_Array2<GeomGridEval::SurfD1> EvaluateGridD1(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const
  {
    const int aNbU = theUParams.Size();
    const int aNbV = theVParams.Size();
    if (myGeom.IsNull() || aNbU == 0 || aNbV == 0)
    {
      return NCollection_Array2<GeomGridEval::SurfD1>();
    }

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
      const double u  = theUParams.Value(theUParams.Lower() + iU - 1);
      const double uX = aLocX + u * aXX;
      const double uY = aLocY + u * aXY;
      const double uZ = aLocZ + u * aXZ;

      for (int iV = 1; iV <= aNbV; ++iV)
      {
        const double v          = theVParams.Value(theVParams.Lower() + iV - 1);
        aResult.ChangeValue(iU,
                            iV) = {gp_Pnt(uX + v * aYX, uY + v * aYY, uZ + v * aYZ), aD1U, aD1V};
      }
    }
    return aResult;
  }

  //! Evaluate grid points with first and second partial derivatives.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  //! @return 2D array of SurfD2 (1-based indexing)
  NCollection_Array2<GeomGridEval::SurfD2> EvaluateGridD2(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const
  {
    const int aNbU = theUParams.Size();
    const int aNbV = theVParams.Size();
    if (myGeom.IsNull() || aNbU == 0 || aNbV == 0)
    {
      return NCollection_Array2<GeomGridEval::SurfD2>();
    }

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
      const double u  = theUParams.Value(theUParams.Lower() + iU - 1);
      const double uX = aLocX + u * aXX;
      const double uY = aLocY + u * aXY;
      const double uZ = aLocZ + u * aXZ;

      for (int iV = 1; iV <= aNbV; ++iV)
      {
        const double v = theVParams.Value(theVParams.Lower() + iV - 1);
        aResult.ChangeValue(
          iU,
          iV) = {gp_Pnt(uX + v * aYX, uY + v * aYY, uZ + v * aYZ), aD1U, aD1V, aZero, aZero, aZero};
      }
    }
    return aResult;
  }

  //! Evaluate grid points with derivatives up to third order.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  //! @return 2D array of SurfD3 (1-based indexing)
  NCollection_Array2<GeomGridEval::SurfD3> EvaluateGridD3(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const
  {
    const int aNbU = theUParams.Size();
    const int aNbV = theVParams.Size();
    if (myGeom.IsNull() || aNbU == 0 || aNbV == 0)
    {
      return NCollection_Array2<GeomGridEval::SurfD3>();
    }

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
      const double u  = theUParams.Value(theUParams.Lower() + iU - 1);
      const double uX = aLocX + u * aXX;
      const double uY = aLocY + u * aXY;
      const double uZ = aLocZ + u * aXZ;

      for (int iV = 1; iV <= aNbV; ++iV)
      {
        const double v              = theVParams.Value(theVParams.Lower() + iV - 1);
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

  //! Evaluate partial derivative d^(NU+NV)S/(dU^NU dV^NV) at all grid points.
  //! @param theUParams array of U parameter values
  //! @param theVParams array of V parameter values
  //! @param theNU derivative order in U direction
  //! @param theNV derivative order in V direction
  //! @return 2D array of derivative vectors (1-based indexing)
  NCollection_Array2<gp_Vec> EvaluateGridDN(const NCollection_Array1<double>& theUParams,
                                            const NCollection_Array1<double>& theVParams,
                                            int                               theNU,
                                            int                               theNV) const
  {
    const int aNbU = theUParams.Size();
    const int aNbV = theVParams.Size();
    if (myGeom.IsNull() || aNbU == 0 || aNbV == 0 || theNU < 0 || theNV < 0 || (theNU + theNV) < 1)
    {
      return NCollection_Array2<gp_Vec>();
    }

    NCollection_Array2<gp_Vec> aResult(1, aNbU, 1, aNbV);

    // For a plane, only D1U (1,0) and D1V (0,1) are non-zero
    gp_Vec aDerivative(0, 0, 0);

    if (theNU == 1 && theNV == 0)
    {
      // D1U = XDir
      const gp_Dir aXDir = myGeom->Pln().Position().XDirection();
      aDerivative        = gp_Vec(aXDir.X(), aXDir.Y(), aXDir.Z());
    }
    else if (theNU == 0 && theNV == 1)
    {
      // D1V = YDir
      const gp_Dir aYDir = myGeom->Pln().Position().YDirection();
      aDerivative        = gp_Vec(aYDir.X(), aYDir.Y(), aYDir.Z());
    }
    // All other derivatives are zero

    for (int iU = 1; iU <= aNbU; ++iU)
    {
      for (int iV = 1; iV <= aNbV; ++iV)
      {
        aResult.SetValue(iU, iV, aDerivative);
      }
    }
    return aResult;
  }

private:
  occ::handle<Geom_Plane> myGeom;
};

#endif // _GeomGridEval_Plane_HeaderFile
