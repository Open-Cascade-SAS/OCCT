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

#include <GeomGridEval_BezierSurface.hxx>

#include <BSplSLib.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array2OfReal.hxx>

//==================================================================================================

void GeomGridEval_BezierSurface::SetUVParams(const TColStd_Array1OfReal& theUParams,
                                             const TColStd_Array1OfReal& theVParams)
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

//==================================================================================================

NCollection_Array2<gp_Pnt> GeomGridEval_BezierSurface::EvaluateGrid() const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  const int                  aNbU = myUParams.Size();
  const int                  aNbV = myVParams.Size();
  NCollection_Array2<gp_Pnt> aResult(1, aNbU, 1, aNbV);

  const TColgp_Array2OfPnt&   aPoles = myGeom->Poles();
  const TColStd_Array2OfReal* aW     = myGeom->Weights();

  for (int i = 1; i <= aNbU; ++i)
  {
    const double u = myUParams.Value(i);
    for (int j = 1; j <= aNbV; ++j)
    {
      const double v = myVParams.Value(j);
      gp_Pnt       aP;
      BSplSLib::BezierD0(u, v, aPoles, aW, aP);
      aResult.SetValue(i, j, aP);
    }
  }
  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEval_BezierSurface::EvaluateGridD1() const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }

  const int                                aNbU = myUParams.Size();
  const int                                aNbV = myVParams.Size();
  NCollection_Array2<GeomGridEval::SurfD1> aResult(1, aNbU, 1, aNbV);

  const TColgp_Array2OfPnt&   aPoles = myGeom->Poles();
  const TColStd_Array2OfReal* aW     = myGeom->Weights();

  for (int i = 1; i <= aNbU; ++i)
  {
    const double u = myUParams.Value(i);
    for (int j = 1; j <= aNbV; ++j)
    {
      const double v = myVParams.Value(j);
      gp_Pnt       aP;
      gp_Vec       aD1U, aD1V;
      BSplSLib::BezierD1(u, v, aPoles, aW, aP, aD1U, aD1V);
      aResult.ChangeValue(i, j) = {aP, aD1U, aD1V};
    }
  }
  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_BezierSurface::EvaluateGridD2() const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  const int                                aNbU = myUParams.Size();
  const int                                aNbV = myVParams.Size();
  NCollection_Array2<GeomGridEval::SurfD2> aResult(1, aNbU, 1, aNbV);

  const TColgp_Array2OfPnt&   aPoles = myGeom->Poles();
  const TColStd_Array2OfReal* aW     = myGeom->Weights();

  for (int i = 1; i <= aNbU; ++i)
  {
    const double u = myUParams.Value(i);
    for (int j = 1; j <= aNbV; ++j)
    {
      const double v = myVParams.Value(j);
      gp_Pnt       aP;
      gp_Vec       aD1U, aD1V, aD2U, aD2V, aD2UV;
      BSplSLib::BezierD2(u, v, aPoles, aW, aP, aD1U, aD1V, aD2U, aD2V, aD2UV);
      aResult.ChangeValue(i, j) = {aP, aD1U, aD1V, aD2U, aD2V, aD2UV};
    }
  }
  return aResult;
}
