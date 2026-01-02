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

#ifndef _GeomGridEval_Line_HeaderFile
#define _GeomGridEval_Line_HeaderFile

#include <Geom_Line.hxx>
#include <GeomGridEval.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Efficient batch evaluator for line grid points.
//!
//! Uses direct analytical formula: P(t) = Location + t * Direction
//!
//! Usage:
//! @code
//!   GeomGridEval_Line anEvaluator(myGeomLine);
//!   NCollection_Array1<gp_Pnt> aGrid = anEvaluator.EvaluateGrid(myParams);
//! @endcode
class GeomGridEval_Line
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param theLine the line geometry to evaluate
  GeomGridEval_Line(const occ::handle<Geom_Line>& theLine)
      : myGeom(theLine)
  {
  }

  //! Non-copyable and non-movable.
  GeomGridEval_Line(const GeomGridEval_Line&)            = delete;
  GeomGridEval_Line& operator=(const GeomGridEval_Line&) = delete;
  GeomGridEval_Line(GeomGridEval_Line&&)                 = delete;
  GeomGridEval_Line& operator=(GeomGridEval_Line&&)      = delete;

  //! Returns the geometry handle.
  const occ::handle<Geom_Line>& Geometry() const { return myGeom; }

  //! Evaluate all grid points.
  //! @param theParams array of parameter values
  //! @return array of evaluated points (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  NCollection_Array1<gp_Pnt> EvaluateGrid(const NCollection_Array1<double>& theParams) const
  {
    if (myGeom.IsNull() || theParams.IsEmpty())
    {
      return NCollection_Array1<gp_Pnt>();
    }

    NCollection_Array1<gp_Pnt> aResult(1, theParams.Size());

    // Extract line data
    const gp_Lin& aLin = myGeom->Lin();
    const gp_Pnt& aLoc = aLin.Location();
    const gp_Dir& aDir = aLin.Direction();

    // Pre-extract coordinates for vectorization potential
    const double aLocX = aLoc.X();
    const double aLocY = aLoc.Y();
    const double aLocZ = aLoc.Z();
    const double aDirX = aDir.X();
    const double aDirY = aDir.Y();
    const double aDirZ = aDir.Z();

    for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
    {
      const double t = theParams.Value(i);
      aResult.SetValue(i - theParams.Lower() + 1,
                       gp_Pnt(aLocX + t * aDirX, aLocY + t * aDirY, aLocZ + t * aDirZ));
    }
    return aResult;
  }

  //! Evaluate all grid points with first derivative.
  //! For a line, D1 is constant (the direction vector).
  //! @param theParams array of parameter values
  //! @return array of CurveD1 (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  NCollection_Array1<GeomGridEval::CurveD1> EvaluateGridD1(
    const NCollection_Array1<double>& theParams) const
  {
    if (myGeom.IsNull() || theParams.IsEmpty())
    {
      return NCollection_Array1<GeomGridEval::CurveD1>();
    }

    NCollection_Array1<GeomGridEval::CurveD1> aResult(1, theParams.Size());

    const gp_Lin& aLin = myGeom->Lin();
    const gp_Pnt& aLoc = aLin.Location();
    const gp_Dir& aDir = aLin.Direction();

    const double aLocX = aLoc.X();
    const double aLocY = aLoc.Y();
    const double aLocZ = aLoc.Z();
    const double aDirX = aDir.X();
    const double aDirY = aDir.Y();
    const double aDirZ = aDir.Z();

    // D1 is constant for a line
    const gp_Vec aD1(aDirX, aDirY, aDirZ);

    for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
    {
      const double t           = theParams.Value(i);
      aResult.ChangeValue(i - theParams.Lower()
                          + 1) = {gp_Pnt(aLocX + t * aDirX, aLocY + t * aDirY, aLocZ + t * aDirZ),
                                  aD1};
    }
    return aResult;
  }

  //! Evaluate all grid points with first and second derivatives.
  //! For a line, D1 is constant and D2 is zero.
  //! @param theParams array of parameter values
  //! @return array of CurveD2 (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  NCollection_Array1<GeomGridEval::CurveD2> EvaluateGridD2(
    const NCollection_Array1<double>& theParams) const
  {
    if (myGeom.IsNull() || theParams.IsEmpty())
    {
      return NCollection_Array1<GeomGridEval::CurveD2>();
    }

    NCollection_Array1<GeomGridEval::CurveD2> aResult(1, theParams.Size());

    const gp_Lin& aLin = myGeom->Lin();
    const gp_Pnt& aLoc = aLin.Location();
    const gp_Dir& aDir = aLin.Direction();

    const double aLocX = aLoc.X();
    const double aLocY = aLoc.Y();
    const double aLocZ = aLoc.Z();
    const double aDirX = aDir.X();
    const double aDirY = aDir.Y();
    const double aDirZ = aDir.Z();

    const gp_Vec aD1(aDirX, aDirY, aDirZ);
    const gp_Vec aD2(0, 0, 0); // Second derivative is zero for a line

    for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
    {
      const double t = theParams.Value(i);
      aResult.ChangeValue(
        i - theParams.Lower()
        + 1) = {gp_Pnt(aLocX + t * aDirX, aLocY + t * aDirY, aLocZ + t * aDirZ), aD1, aD2};
    }
    return aResult;
  }

  //! Evaluate all grid points with first, second, and third derivatives.
  //! For a line, D1 is constant, D2 and D3 are zero.
  //! @param theParams array of parameter values
  //! @return array of CurveD3 (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  NCollection_Array1<GeomGridEval::CurveD3> EvaluateGridD3(
    const NCollection_Array1<double>& theParams) const
  {
    if (myGeom.IsNull() || theParams.IsEmpty())
    {
      return NCollection_Array1<GeomGridEval::CurveD3>();
    }

    NCollection_Array1<GeomGridEval::CurveD3> aResult(1, theParams.Size());

    const gp_Lin& aLin = myGeom->Lin();
    const gp_Pnt& aLoc = aLin.Location();
    const gp_Dir& aDir = aLin.Direction();

    const double aLocX = aLoc.X();
    const double aLocY = aLoc.Y();
    const double aLocZ = aLoc.Z();
    const double aDirX = aDir.X();
    const double aDirY = aDir.Y();
    const double aDirZ = aDir.Z();

    const gp_Vec aD1(aDirX, aDirY, aDirZ);
    const gp_Vec aZero(0, 0, 0);

    for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
    {
      const double t = theParams.Value(i);
      aResult.ChangeValue(
        i - theParams.Lower()
        + 1) = {gp_Pnt(aLocX + t * aDirX, aLocY + t * aDirY, aLocZ + t * aDirZ), aD1, aZero, aZero};
    }
    return aResult;
  }

  //! Evaluate Nth derivative at all grid points.
  //! For a line: D1 = Direction, DN = 0 for N > 1.
  //! @param theParams array of parameter values
  //! @param theN derivative order (N >= 1)
  //! @return array of derivative vectors (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  NCollection_Array1<gp_Vec> EvaluateGridDN(const NCollection_Array1<double>& theParams,
                                            int                               theN) const
  {
    if (myGeom.IsNull() || theParams.IsEmpty() || theN < 1)
    {
      return NCollection_Array1<gp_Vec>();
    }

    NCollection_Array1<gp_Vec> aResult(1, theParams.Size());

    if (theN == 1)
    {
      // D1 is constant for a line (the direction)
      const gp_Dir aDir = myGeom->Lin().Direction();
      const gp_Vec  aD1(aDir.X(), aDir.Y(), aDir.Z());
      for (int i = 1; i <= theParams.Size(); ++i)
      {
        aResult.SetValue(i, aD1);
      }
    }
    else
    {
      // All higher derivatives are zero for a line
      const gp_Vec aZero(0, 0, 0);
      for (int i = 1; i <= theParams.Size(); ++i)
      {
        aResult.SetValue(i, aZero);
      }
    }
    return aResult;
  }

private:
  occ::handle<Geom_Line> myGeom;
};

#endif // _GeomGridEval_Line_HeaderFile
