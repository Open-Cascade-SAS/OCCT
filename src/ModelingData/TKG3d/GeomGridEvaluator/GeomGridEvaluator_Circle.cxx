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

#include <GeomGridEvaluator_Circle.hxx>

#include <cmath>

//==================================================================================================

void GeomGridEvaluator_Circle::SetParams(const TColStd_Array1OfReal& theParams)
{
  myParams.Resize(theParams.Lower(), theParams.Upper(), false);
  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    myParams.SetValue(i, theParams.Value(i));
  }
}

//==================================================================================================

NCollection_Array1<gp_Pnt> GeomGridEvaluator_Circle::EvaluateGrid() const
{
  if (myGeom.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt>();
  }

  NCollection_Array1<gp_Pnt> aResult(myParams.Lower(), myParams.Upper());

  // Extract circle data from geometry
  const gp_Circ& aCirc   = myGeom->Circ();
  const gp_Pnt&  aCenter = aCirc.Location();
  const gp_Dir&  aXDir   = aCirc.Position().XDirection();
  const gp_Dir&  aYDir   = aCirc.Position().YDirection();
  const double   aRadius = aCirc.Radius();

  // Pre-extract coordinates for performance
  const double aCX = aCenter.X();
  const double aCY = aCenter.Y();
  const double aCZ = aCenter.Z();
  const double aXX = aXDir.X();
  const double aXY = aXDir.Y();
  const double aXZ = aXDir.Z();
  const double aYX = aYDir.X();
  const double aYY = aYDir.Y();
  const double aYZ = aYDir.Z();

  for (int i = myParams.Lower(); i <= myParams.Upper(); ++i)
  {
    const double u    = myParams.Value(i);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    aResult.SetValue(i,
                     gp_Pnt(aCX + aRadius * (cosU * aXX + sinU * aYX),
                            aCY + aRadius * (cosU * aXY + sinU * aYY),
                            aCZ + aRadius * (cosU * aXZ + sinU * aYZ)));
  }
  return aResult;
}
