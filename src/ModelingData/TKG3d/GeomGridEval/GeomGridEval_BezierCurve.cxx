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

#include <BSplCLib.hxx>
#include <gp_Pnt.hxx>

//==================================================================================================

void GeomGridEval_BezierCurve::SetParams(const TColStd_Array1OfReal& theParams)
{
  const int aNb = theParams.Size();
  myParams.Resize(1, aNb, false);
  for (int i = 1; i <= aNb; ++i)
  {
    myParams.SetValue(i, theParams.Value(theParams.Lower() + i - 1));
  }

  // Invalidate cache when parameters change
  myCache.Nullify();
}

//==================================================================================================

void GeomGridEval_BezierCurve::buildCache() const
{
  if (myGeom.IsNull())
  {
    return;
  }

  const int            aDegree = myGeom->Degree();
  TColStd_Array1OfReal aFlatKnots(BSplCLib::FlatBezierKnots(aDegree), 1, 2 * (aDegree + 1));

  // Create cache following the pattern from GeomAdaptor_Curve
  myCache = new BSplCLib_Cache(aDegree,
                               myGeom->IsPeriodic(),
                               aFlatKnots,
                               myGeom->Poles(),
                               myGeom->Weights());

  // Build cache at parameter 0.5 (middle of the single span)
  myCache->BuildCache(0.5, aFlatKnots, myGeom->Poles(), myGeom->Weights());
}

//==================================================================================================

NCollection_Array1<gp_Pnt> GeomGridEval_BezierCurve::EvaluateGrid() const
{
  if (myGeom.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt>();
  }

  // Build cache if needed
  if (myCache.IsNull())
  {
    buildCache();
  }

  const int                  aNb = myParams.Size();
  NCollection_Array1<gp_Pnt> aResult(1, aNb);

  // Single span Bezier - use cache for all points
  for (int i = 1; i <= aNb; ++i)
  {
    gp_Pnt aP;
    myCache->D0(myParams.Value(i), aP);
    aResult.SetValue(i, aP);
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD1> GeomGridEval_BezierCurve::EvaluateGridD1() const
{
  if (myGeom.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD1>();
  }

  // Build cache if needed
  if (myCache.IsNull())
  {
    buildCache();
  }

  const int                                 aNb = myParams.Size();
  NCollection_Array1<GeomGridEval::CurveD1> aResult(1, aNb);

  // Single span Bezier - use cache for all points
  for (int i = 1; i <= aNb; ++i)
  {
    gp_Pnt aP;
    gp_Vec aD1;
    myCache->D1(myParams.Value(i), aP, aD1);
    aResult.ChangeValue(i) = {aP, aD1};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD2> GeomGridEval_BezierCurve::EvaluateGridD2() const
{
  if (myGeom.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD2>();
  }

  // Build cache if needed
  if (myCache.IsNull())
  {
    buildCache();
  }

  const int                                 aNb = myParams.Size();
  NCollection_Array1<GeomGridEval::CurveD2> aResult(1, aNb);

  // Single span Bezier - use cache for all points
  for (int i = 1; i <= aNb; ++i)
  {
    gp_Pnt aP;
    gp_Vec aD1, aD2;
    myCache->D2(myParams.Value(i), aP, aD1, aD2);
    aResult.ChangeValue(i) = {aP, aD1, aD2};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD3> GeomGridEval_BezierCurve::EvaluateGridD3() const
{
  if (myGeom.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD3>();
  }

  // Build cache if needed
  if (myCache.IsNull())
  {
    buildCache();
  }

  const int                                 aNb = myParams.Size();
  NCollection_Array1<GeomGridEval::CurveD3> aResult(1, aNb);

  // Single span Bezier - use cache for all points
  for (int i = 1; i <= aNb; ++i)
  {
    gp_Pnt aP;
    gp_Vec aD1, aD2, aD3;
    myCache->D3(myParams.Value(i), aP, aD1, aD2, aD3);
    aResult.ChangeValue(i) = {aP, aD1, aD2, aD3};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<gp_Vec> GeomGridEval_BezierCurve::EvaluateGridDN(int theN) const
{
  if (myGeom.IsNull() || myParams.IsEmpty() || theN < 1)
  {
    return NCollection_Array1<gp_Vec>();
  }

  const int                  aNb = myParams.Size();
  NCollection_Array1<gp_Vec> aResult(1, aNb);

  // Reuse existing grid evaluators for orders 1-3
  if (theN == 1)
  {
    NCollection_Array1<GeomGridEval::CurveD1> aD1Grid = EvaluateGridD1();
    for (int i = 1; i <= aNb; ++i)
    {
      aResult.SetValue(i, aD1Grid.Value(i).D1);
    }
  }
  else if (theN == 2)
  {
    NCollection_Array1<GeomGridEval::CurveD2> aD2Grid = EvaluateGridD2();
    for (int i = 1; i <= aNb; ++i)
    {
      aResult.SetValue(i, aD2Grid.Value(i).D2);
    }
  }
  else if (theN == 3)
  {
    NCollection_Array1<GeomGridEval::CurveD3> aD3Grid = EvaluateGridD3();
    for (int i = 1; i <= aNb; ++i)
    {
      aResult.SetValue(i, aD3Grid.Value(i).D3);
    }
  }
  else
  {
    // For orders > 3, check if derivative exists (depends on degree)
    // A Bezier of degree n has DN = 0 for N > n
    const int aDegree = myGeom->Degree();
    if (theN > aDegree)
    {
      const gp_Vec aZero(0.0, 0.0, 0.0);
      for (int i = 1; i <= aNb; ++i)
      {
        aResult.SetValue(i, aZero);
      }
    }
    else
    {
      // Use geometry DN method for higher orders
      for (int i = 1; i <= aNb; ++i)
      {
        aResult.SetValue(i, myGeom->DN(myParams.Value(i), theN));
      }
    }
  }
  return aResult;
}
