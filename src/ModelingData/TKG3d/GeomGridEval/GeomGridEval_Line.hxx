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
#include <TColStd_Array1OfReal.hxx>

//! @brief Efficient batch evaluator for line grid points.
//!
//! Uses direct analytical formula: P(t) = Location + t * Direction
//!
//! Usage:
//! @code
//!   GeomGridEval_Line anEvaluator(myGeomLine);
//!   anEvaluator.SetParams(myParams);
//!   NCollection_Array1<gp_Pnt> aGrid = anEvaluator.EvaluateGrid();
//! @endcode
class GeomGridEval_Line
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param theLine the line geometry to evaluate
  GeomGridEval_Line(const Handle(Geom_Line)& theLine)
      : myGeom(theLine)
  {
  }

  //! Set parameters for grid evaluation (by const reference).
  //! @param theParams array of parameter values
  void SetParams(const TColStd_Array1OfReal& theParams)
  {
    myParams.Resize(theParams.Lower(), theParams.Upper(), false);
    for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
    {
      myParams.SetValue(i, theParams.Value(i));
    }
  }

  //! Set parameters for grid evaluation (by move).
  //! @param theParams array of parameter values to move
  void SetParams(NCollection_Array1<double>&& theParams) { myParams = std::move(theParams); }

  //! Returns the geometry handle.
  const Handle(Geom_Line)& Geometry() const { return myGeom; }

  //! Returns number of parameters.
  int NbParams() const { return myParams.Size(); }

  //! Evaluate all grid points.
  //! @return array of evaluated points (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  NCollection_Array1<gp_Pnt> EvaluateGrid() const
  {
    if (myGeom.IsNull() || myParams.IsEmpty())
    {
      return NCollection_Array1<gp_Pnt>();
    }

    NCollection_Array1<gp_Pnt> aResult(myParams.Lower(), myParams.Upper());

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

    for (int i = myParams.Lower(); i <= myParams.Upper(); ++i)
    {
      const double t = myParams.Value(i);
      aResult.SetValue(i, gp_Pnt(aLocX + t * aDirX, aLocY + t * aDirY, aLocZ + t * aDirZ));
    }
    return aResult;
  }

  //! Evaluate all grid points with first derivative.
  //! For a line, D1 is constant (the direction vector).
  //! @return array of CurveD1 (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  NCollection_Array1<GeomGridEval::CurveD1> EvaluateGridD1() const
  {
    if (myGeom.IsNull() || myParams.IsEmpty())
    {
      return NCollection_Array1<GeomGridEval::CurveD1>();
    }

    NCollection_Array1<GeomGridEval::CurveD1> aResult(myParams.Lower(), myParams.Upper());

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

    for (int i = myParams.Lower(); i <= myParams.Upper(); ++i)
    {
      const double t         = myParams.Value(i);
      aResult.ChangeValue(i) = {gp_Pnt(aLocX + t * aDirX, aLocY + t * aDirY, aLocZ + t * aDirZ),
                                aD1};
    }
    return aResult;
  }

  //! Evaluate all grid points with first and second derivatives.
  //! For a line, D1 is constant and D2 is zero.
  //! @return array of CurveD2 (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  NCollection_Array1<GeomGridEval::CurveD2> EvaluateGridD2() const
  {
    if (myGeom.IsNull() || myParams.IsEmpty())
    {
      return NCollection_Array1<GeomGridEval::CurveD2>();
    }

    NCollection_Array1<GeomGridEval::CurveD2> aResult(myParams.Lower(), myParams.Upper());

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

    for (int i = myParams.Lower(); i <= myParams.Upper(); ++i)
    {
      const double t = myParams.Value(i);
      aResult.ChangeValue(
        i) = {gp_Pnt(aLocX + t * aDirX, aLocY + t * aDirY, aLocZ + t * aDirZ), aD1, aD2};
    }
    return aResult;
  }

  //! Evaluate all grid points with first, second, and third derivatives.
  //! For a line, D1 is constant, D2 and D3 are zero.
  //! @return array of CurveD3 (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  NCollection_Array1<GeomGridEval::CurveD3> EvaluateGridD3() const
  {
    if (myGeom.IsNull() || myParams.IsEmpty())
    {
      return NCollection_Array1<GeomGridEval::CurveD3>();
    }

    NCollection_Array1<GeomGridEval::CurveD3> aResult(myParams.Lower(), myParams.Upper());

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

    for (int i = myParams.Lower(); i <= myParams.Upper(); ++i)
    {
      const double t = myParams.Value(i);
      aResult.ChangeValue(
        i) = {gp_Pnt(aLocX + t * aDirX, aLocY + t * aDirY, aLocZ + t * aDirZ), aD1, aZero, aZero};
    }
    return aResult;
  }

private:
  Handle(Geom_Line)          myGeom;
  NCollection_Array1<double> myParams;
};

#endif // _GeomGridEval_Line_HeaderFile
