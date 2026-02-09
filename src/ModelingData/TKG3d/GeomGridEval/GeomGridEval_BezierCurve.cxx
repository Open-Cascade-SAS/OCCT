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

#include <GeomGridEval_BezierCurve.hxx>

#include <BSplCLib_Cache.hxx>
#include <gp_Pnt.hxx>

namespace
{
//! Creates a BSplCLib_Cache for a Bezier curve.
//! @param theCurve the Bezier curve geometry
//! @return initialized cache ready for evaluation
occ::handle<BSplCLib_Cache> CreateBezierCache(const occ::handle<Geom_BezierCurve>& theCurve)
{
  const NCollection_Array1<double>& aKnotSequence = theCurve->KnotSequence();
  const NCollection_Array1<gp_Pnt>& aPoles        = theCurve->Poles();
  const NCollection_Array1<double>* aWeights      = theCurve->Weights();

  occ::handle<BSplCLib_Cache> aCache =
    new BSplCLib_Cache(theCurve->Degree(), false, aKnotSequence, aPoles, aWeights);
  aCache->BuildCache(0.5, aKnotSequence, aPoles, aWeights);
  return aCache;
}
} // namespace

//==================================================================================================

NCollection_Array1<gp_Pnt> GeomGridEval_BezierCurve::EvaluateGrid(
  const NCollection_Array1<double>& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt>();
  }

  const int                  aNb = theParams.Size();
  NCollection_Array1<gp_Pnt> aResult(1, aNb);

  occ::handle<BSplCLib_Cache> aCache = CreateBezierCache(myGeom);

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    gp_Pnt aP;
    aCache->D0(theParams.Value(i), aP);
    aResult.SetValue(i - theParams.Lower() + 1, aP);
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD1> GeomGridEval_BezierCurve::EvaluateGridD1(
  const NCollection_Array1<double>& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD1>();
  }

  const int                                 aNb = theParams.Size();
  NCollection_Array1<GeomGridEval::CurveD1> aResult(1, aNb);

  occ::handle<BSplCLib_Cache> aCache = CreateBezierCache(myGeom);

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    gp_Pnt aP;
    gp_Vec aD1;
    aCache->D1(theParams.Value(i), aP, aD1);
    aResult.ChangeValue(i - theParams.Lower() + 1) = {aP, aD1};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD2> GeomGridEval_BezierCurve::EvaluateGridD2(
  const NCollection_Array1<double>& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD2>();
  }

  const int                                 aNb = theParams.Size();
  NCollection_Array1<GeomGridEval::CurveD2> aResult(1, aNb);

  occ::handle<BSplCLib_Cache> aCache = CreateBezierCache(myGeom);

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    gp_Pnt aP;
    gp_Vec aD1, aD2;
    aCache->D2(theParams.Value(i), aP, aD1, aD2);
    aResult.ChangeValue(i - theParams.Lower() + 1) = {aP, aD1, aD2};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD3> GeomGridEval_BezierCurve::EvaluateGridD3(
  const NCollection_Array1<double>& theParams) const
{
  if (myGeom.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD3>();
  }

  const int                                 aNb = theParams.Size();
  NCollection_Array1<GeomGridEval::CurveD3> aResult(1, aNb);

  occ::handle<BSplCLib_Cache> aCache = CreateBezierCache(myGeom);

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    gp_Pnt aP;
    gp_Vec aD1, aD2, aD3;
    aCache->D3(theParams.Value(i), aP, aD1, aD2, aD3);
    aResult.ChangeValue(i - theParams.Lower() + 1) = {aP, aD1, aD2, aD3};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<gp_Vec> GeomGridEval_BezierCurve::EvaluateGridDN(
  const NCollection_Array1<double>& theParams,
  int                               theN) const
{
  if (myGeom.IsNull() || theParams.IsEmpty() || theN < 1)
  {
    return NCollection_Array1<gp_Vec>();
  }

  const int                  aNb = theParams.Size();
  NCollection_Array1<gp_Vec> aResult(1, aNb);

  // For Bezier curves, derivatives become zero when order exceeds degree
  const int aDegree = myGeom->Degree();
  if (theN > aDegree)
  {
    const gp_Vec aZeroVec(0.0, 0.0, 0.0);
    for (int i = 1; i <= aNb; ++i)
    {
      aResult.SetValue(i, aZeroVec);
    }
    return aResult;
  }

  // Get poles, weights, and flat knots from geometry
  const NCollection_Array1<gp_Pnt>& aPoles        = myGeom->Poles();
  const NCollection_Array1<double>* aWeights      = myGeom->Weights();
  const NCollection_Array1<double>& aKnotSequence = myGeom->KnotSequence();

  // Bezier has a single span (index 0 with flat knots), non-periodic
  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    gp_Vec aDN;
    BSplCLib::DN(theParams.Value(i),
                 theN,
                 0, // span index (single span for Bezier with flat knots)
                 aDegree,
                 false, // not periodic
                 aPoles,
                 aWeights,
                 aKnotSequence,
                 nullptr, // no multiplicities with flat knots
                 aDN);
    aResult.SetValue(i - theParams.Lower() + 1, aDN);
  }
  return aResult;
}
