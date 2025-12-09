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

#include <GeomGridEvaluator_OtherSurface.hxx>

//==================================================================================================

void GeomGridEvaluator_OtherSurface::SetUParams(const TColStd_Array1OfReal& theParams)
{
  myUParams.Resize(theParams.Lower(), theParams.Upper(), false);
  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    myUParams.SetValue(i, theParams.Value(i));
  }
}

//==================================================================================================

void GeomGridEvaluator_OtherSurface::SetVParams(const TColStd_Array1OfReal& theParams)
{
  myVParams.Resize(theParams.Lower(), theParams.Upper(), false);
  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    myVParams.SetValue(i, theParams.Value(i));
  }
}

//==================================================================================================

NCollection_Array2<gp_Pnt> GeomGridEvaluator_OtherSurface::EvaluateGrid() const
{
  if (!myIsInitialized || myUParams.IsEmpty() || myVParams.IsEmpty() || mySurface.IsNull())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  const int aRowLower = myUParams.Lower();
  const int aRowUpper = myUParams.Upper();
  const int aColLower = myVParams.Lower();
  const int aColUpper = myVParams.Upper();

  NCollection_Array2<gp_Pnt> aResult(aRowLower, aRowUpper, aColLower, aColUpper);

  for (int iU = aRowLower; iU <= aRowUpper; ++iU)
  {
    const double u = myUParams.Value(iU);
    for (int iV = aColLower; iV <= aColUpper; ++iV)
    {
      const double v = myVParams.Value(iV);
      aResult.SetValue(iU, iV, mySurface->Value(u, v));
    }
  }

  return aResult;
}
