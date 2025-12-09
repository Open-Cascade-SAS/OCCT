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

GeomGridEvaluator_Circle::GeomGridEvaluator_Circle()
    : myRadius(0.0),
      myIsInitialized(false)
{
}

//==================================================================================================

void GeomGridEvaluator_Circle::Initialize(const gp_Circ& theCircle)
{
  myCenter        = theCircle.Location();
  myXDir          = theCircle.Position().XDirection();
  myYDir          = theCircle.Position().YDirection();
  myRadius        = theCircle.Radius();
  myIsInitialized = true;
}

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
  if (!myIsInitialized || myParams.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt>();
  }

  NCollection_Array1<gp_Pnt> aResult(myParams.Lower(), myParams.Upper());

  // Pre-extract coordinates for performance
  const double aCX = myCenter.X();
  const double aCY = myCenter.Y();
  const double aCZ = myCenter.Z();
  const double aXX = myXDir.X();
  const double aXY = myXDir.Y();
  const double aXZ = myXDir.Z();
  const double aYX = myYDir.X();
  const double aYY = myYDir.Y();
  const double aYZ = myYDir.Z();
  const double aR  = myRadius;

  for (int i = myParams.Lower(); i <= myParams.Upper(); ++i)
  {
    const double u    = myParams.Value(i);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    aResult.SetValue(i,
                     gp_Pnt(aCX + aR * (cosU * aXX + sinU * aYX),
                            aCY + aR * (cosU * aXY + sinU * aYY),
                            aCZ + aR * (cosU * aXZ + sinU * aYZ)));
  }
  return aResult;
}
