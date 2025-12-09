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

#include <GeomGridEvaluator_Sphere.hxx>

#include <cmath>

//==================================================================================================

GeomGridEvaluator_Sphere::GeomGridEvaluator_Sphere()
    : myRadius(0.0),
      myIsInitialized(false)
{
}

//==================================================================================================

void GeomGridEvaluator_Sphere::Initialize(const gp_Sphere& theSphere)
{
  myCenter        = theSphere.Location();
  myXDir          = theSphere.Position().XDirection();
  myYDir          = theSphere.Position().YDirection();
  myZDir          = theSphere.Position().Direction();
  myRadius        = theSphere.Radius();
  myIsInitialized = true;
}

//==================================================================================================

void GeomGridEvaluator_Sphere::SetUParams(const TColStd_Array1OfReal& theParams)
{
  myUParams.Resize(theParams.Lower(), theParams.Upper(), false);
  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    myUParams.SetValue(i, theParams.Value(i));
  }
}

//==================================================================================================

void GeomGridEvaluator_Sphere::SetVParams(const TColStd_Array1OfReal& theParams)
{
  myVParams.Resize(theParams.Lower(), theParams.Upper(), false);
  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    myVParams.SetValue(i, theParams.Value(i));
  }
}

//==================================================================================================

NCollection_Array2<gp_Pnt> GeomGridEvaluator_Sphere::EvaluateGrid() const
{
  if (!myIsInitialized || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  const int aRowLower = myUParams.Lower();
  const int aRowUpper = myUParams.Upper();
  const int aColLower = myVParams.Lower();
  const int aColUpper = myVParams.Upper();

  NCollection_Array2<gp_Pnt> aResult(aRowLower, aRowUpper, aColLower, aColUpper);

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
  const double aZX = myZDir.X();
  const double aZY = myZDir.Y();
  const double aZZ = myZDir.Z();
  const double aR  = myRadius;

  // Pre-compute V-dependent values (sin/cos of latitude)
  NCollection_Array1<double> aCosV(aColLower, aColUpper);
  NCollection_Array1<double> aSinV(aColLower, aColUpper);
  for (int iV = aColLower; iV <= aColUpper; ++iV)
  {
    const double v = myVParams.Value(iV);
    aCosV.SetValue(iV, std::cos(v));
    aSinV.SetValue(iV, std::sin(v));
  }

  for (int iU = aRowLower; iU <= aRowUpper; ++iU)
  {
    const double u    = myUParams.Value(iU);
    const double cosU = std::cos(u);
    const double sinU = std::sin(u);

    for (int iV = aColLower; iV <= aColUpper; ++iV)
    {
      const double cosV = aCosV.Value(iV);
      const double sinV = aSinV.Value(iV);

      // P = Center + R * (cosV*cosU*XDir + cosV*sinU*YDir + sinV*ZDir)
      const double coeff1 = aR * cosV * cosU;
      const double coeff2 = aR * cosV * sinU;
      const double coeff3 = aR * sinV;

      aResult.SetValue(iU,
                       iV,
                       gp_Pnt(aCX + coeff1 * aXX + coeff2 * aYX + coeff3 * aZX,
                              aCY + coeff1 * aXY + coeff2 * aYY + coeff3 * aZY,
                              aCZ + coeff1 * aXZ + coeff2 * aYZ + coeff3 * aZZ));
    }
  }
  return aResult;
}
