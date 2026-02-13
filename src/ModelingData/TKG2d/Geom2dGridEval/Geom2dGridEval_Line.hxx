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

#ifndef _Geom2dGridEval_Line_HeaderFile
#define _Geom2dGridEval_Line_HeaderFile

#include <Geom2d_Line.hxx>
#include <Geom2dGridEval.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Efficient batch evaluator for 2D line grid points.
//!
//! Uses direct analytical formula: P(t) = Location + t * Direction
//!
//! Usage:
//! @code
//!   Geom2dGridEval_Line anEvaluator(myGeom2dLine);
//!   NCollection_Array1<gp_Pnt2d> aGrid = anEvaluator.EvaluateGrid(myParams);
//! @endcode
class Geom2dGridEval_Line
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param theLine the 2D line geometry to evaluate
  Geom2dGridEval_Line(const occ::handle<Geom2d_Line>& theLine)
      : myGeom(theLine)
  {
  }

  //! Non-copyable and non-movable.
  Geom2dGridEval_Line(const Geom2dGridEval_Line&)            = delete;
  Geom2dGridEval_Line& operator=(const Geom2dGridEval_Line&) = delete;
  Geom2dGridEval_Line(Geom2dGridEval_Line&&)                 = delete;
  Geom2dGridEval_Line& operator=(Geom2dGridEval_Line&&)      = delete;

  //! Returns the geometry handle.
  const occ::handle<Geom2d_Line>& Geometry() const { return myGeom; }

  //! Evaluate all grid points.
  //! @param theParams array of parameter values
  //! @return array of evaluated points (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  NCollection_Array1<gp_Pnt2d> EvaluateGrid(const NCollection_Array1<double>& theParams) const
  {
    if (myGeom.IsNull() || theParams.IsEmpty())
    {
      return NCollection_Array1<gp_Pnt2d>();
    }

    NCollection_Array1<gp_Pnt2d> aResult(1, theParams.Size());

    const gp_Lin2d& aLin = myGeom->Lin2d();
    const gp_Pnt2d& aLoc = aLin.Location();
    const gp_Dir2d& aDir = aLin.Direction();

    const double aLocX = aLoc.X();
    const double aLocY = aLoc.Y();
    const double aDirX = aDir.X();
    const double aDirY = aDir.Y();

    for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
    {
      const double t = theParams.Value(i);
      aResult.SetValue(i - theParams.Lower() + 1, gp_Pnt2d(aLocX + t * aDirX, aLocY + t * aDirY));
    }
    return aResult;
  }

  //! Evaluate all grid points with first derivative.
  //! For a line, D1 is constant (the direction vector).
  //! @param theParams array of parameter values
  //! @return array of CurveD1 (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  NCollection_Array1<Geom2dGridEval::CurveD1> EvaluateGridD1(
    const NCollection_Array1<double>& theParams) const
  {
    if (myGeom.IsNull() || theParams.IsEmpty())
    {
      return NCollection_Array1<Geom2dGridEval::CurveD1>();
    }

    NCollection_Array1<Geom2dGridEval::CurveD1> aResult(1, theParams.Size());

    const gp_Lin2d& aLin = myGeom->Lin2d();
    const gp_Pnt2d& aLoc = aLin.Location();
    const gp_Dir2d& aDir = aLin.Direction();

    const double aLocX = aLoc.X();
    const double aLocY = aLoc.Y();
    const double aDirX = aDir.X();
    const double aDirY = aDir.Y();

    const gp_Vec2d aD1(aDirX, aDirY);

    for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
    {
      const double t           = theParams.Value(i);
      aResult.ChangeValue(i - theParams.Lower()
                          + 1) = {gp_Pnt2d(aLocX + t * aDirX, aLocY + t * aDirY), aD1};
    }
    return aResult;
  }

  //! Evaluate all grid points with first and second derivatives.
  //! For a line, D1 is constant and D2 is zero.
  //! @param theParams array of parameter values
  //! @return array of CurveD2 (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  NCollection_Array1<Geom2dGridEval::CurveD2> EvaluateGridD2(
    const NCollection_Array1<double>& theParams) const
  {
    if (myGeom.IsNull() || theParams.IsEmpty())
    {
      return NCollection_Array1<Geom2dGridEval::CurveD2>();
    }

    NCollection_Array1<Geom2dGridEval::CurveD2> aResult(1, theParams.Size());

    const gp_Lin2d& aLin = myGeom->Lin2d();
    const gp_Pnt2d& aLoc = aLin.Location();
    const gp_Dir2d& aDir = aLin.Direction();

    const double aLocX = aLoc.X();
    const double aLocY = aLoc.Y();
    const double aDirX = aDir.X();
    const double aDirY = aDir.Y();

    const gp_Vec2d aD1(aDirX, aDirY);
    const gp_Vec2d aD2(0, 0);

    for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
    {
      const double t           = theParams.Value(i);
      aResult.ChangeValue(i - theParams.Lower()
                          + 1) = {gp_Pnt2d(aLocX + t * aDirX, aLocY + t * aDirY), aD1, aD2};
    }
    return aResult;
  }

  //! Evaluate all grid points with first, second, and third derivatives.
  //! For a line, D1 is constant, D2 and D3 are zero.
  //! @param theParams array of parameter values
  //! @return array of CurveD3 (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  NCollection_Array1<Geom2dGridEval::CurveD3> EvaluateGridD3(
    const NCollection_Array1<double>& theParams) const
  {
    if (myGeom.IsNull() || theParams.IsEmpty())
    {
      return NCollection_Array1<Geom2dGridEval::CurveD3>();
    }

    NCollection_Array1<Geom2dGridEval::CurveD3> aResult(1, theParams.Size());

    const gp_Lin2d& aLin = myGeom->Lin2d();
    const gp_Pnt2d& aLoc = aLin.Location();
    const gp_Dir2d& aDir = aLin.Direction();

    const double aLocX = aLoc.X();
    const double aLocY = aLoc.Y();
    const double aDirX = aDir.X();
    const double aDirY = aDir.Y();

    const gp_Vec2d aD1(aDirX, aDirY);
    const gp_Vec2d aZero(0, 0);

    for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
    {
      const double t = theParams.Value(i);
      aResult.ChangeValue(
        i - theParams.Lower()
        + 1) = {gp_Pnt2d(aLocX + t * aDirX, aLocY + t * aDirY), aD1, aZero, aZero};
    }
    return aResult;
  }

  //! Evaluate Nth derivative at all grid points.
  //! For a line: D1 = Direction, DN = 0 for N > 1.
  //! @param theParams array of parameter values
  //! @param theN derivative order (N >= 1)
  //! @return array of derivative vectors (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  NCollection_Array1<gp_Vec2d> EvaluateGridDN(const NCollection_Array1<double>& theParams,
                                              int                               theN) const
  {
    if (myGeom.IsNull() || theParams.IsEmpty() || theN < 1)
    {
      return NCollection_Array1<gp_Vec2d>();
    }

    NCollection_Array1<gp_Vec2d> aResult(1, theParams.Size());

    if (theN == 1)
    {
      const gp_Dir2d aDir = myGeom->Lin2d().Direction();
      const gp_Vec2d aD1(aDir.X(), aDir.Y());
      for (int i = 1; i <= theParams.Size(); ++i)
      {
        aResult.SetValue(i, aD1);
      }
    }
    else
    {
      const gp_Vec2d aZero(0, 0);
      for (int i = 1; i <= theParams.Size(); ++i)
      {
        aResult.SetValue(i, aZero);
      }
    }
    return aResult;
  }

private:
  occ::handle<Geom2d_Line> myGeom;
};

#endif // _Geom2dGridEval_Line_HeaderFile
