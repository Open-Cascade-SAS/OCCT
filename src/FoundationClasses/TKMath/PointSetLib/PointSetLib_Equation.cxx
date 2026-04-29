// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <PointSetLib_Equation.hxx>

#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <MathLin_Jacobi.hxx>
#include <PointSetLib_Props.hxx>
#include <Standard_NoSuchObject.hxx>

//=================================================================================================

PointSetLib_Equation::PointSetLib_Equation(const NCollection_Array1<gp_Pnt>& thePnts, double theTol)
{
  // Compute barycentre and inertia via PointSetLib_Props
  PointSetLib_Props aProps(thePnts);
  myBarycentre = aProps.CentreOfMass();
  myG          = myBarycentre;

  // Get inertia matrix at centre of mass
  const gp_Mat anInertia = aProps.MatrixOfInertia();

  // Eigendecomposition to find principal axes
  math_Matrix aMat(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aMat(i, j) = anInertia.Value(i, j);
    }
  }

  const MathUtils::EigenResult anEigen = MathLin::Jacobi(aMat, true);
  if (!anEigen.IsDone())
  {
    // Jacobi failed: mark type as None and initialize PCA data to safe defaults.
    myType       = Type::None;
    myAxes[0]    = gp_Vec(1.0, 0.0, 0.0);
    myAxes[1]    = gp_Vec(0.0, 1.0, 0.0);
    myAxes[2]    = gp_Vec(0.0, 0.0, 1.0);
    myExtents[0] = 0.0;
    myExtents[1] = 0.0;
    myExtents[2] = 0.0;
    return;
  }

  const math_Matrix& aVecs = *anEigen.EigenVectors;
  myAxes[0]                = gp_Vec(aVecs(1, 1), aVecs(2, 1), aVecs(3, 1));
  myAxes[1]                = gp_Vec(aVecs(1, 2), aVecs(2, 2), aVecs(3, 2));
  myAxes[2]                = gp_Vec(aVecs(1, 3), aVecs(2, 3), aVecs(3, 3));

  // Project all points onto principal axes to find extents
  const double aXg = myG.X();
  const double aYg = myG.Y();
  const double aZg = myG.Z();

  double aMax1 = RealFirst(), aMin1 = RealLast();
  double aMax2 = RealFirst(), aMin2 = RealLast();
  double aMax3 = RealFirst(), aMin3 = RealLast();

  for (int i = thePnts.Lower(); i <= thePnts.Upper(); ++i)
  {
    const double aDX = thePnts(i).X() - aXg;
    const double aDY = thePnts(i).Y() - aYg;
    const double aDZ = thePnts(i).Z() - aZg;

    const double aD1 = aDX * myAxes[0].X() + aDY * myAxes[0].Y() + aDZ * myAxes[0].Z();
    if (aD1 > aMax1)
    {
      aMax1 = aD1;
    }
    if (aD1 < aMin1)
    {
      aMin1 = aD1;
    }

    const double aD2 = aDX * myAxes[1].X() + aDY * myAxes[1].Y() + aDZ * myAxes[1].Z();
    if (aD2 > aMax2)
    {
      aMax2 = aD2;
    }
    if (aD2 < aMin2)
    {
      aMin2 = aD2;
    }

    const double aD3 = aDX * myAxes[2].X() + aDY * myAxes[2].Y() + aDZ * myAxes[2].Z();
    if (aD3 > aMax3)
    {
      aMax3 = aD3;
    }
    if (aD3 < aMin3)
    {
      aMin3 = aD3;
    }
  }

  myExtents[0] = aMax1 - aMin1;
  myExtents[1] = aMax2 - aMin2;
  myExtents[2] = aMax3 - aMin3;

  // Determine dimensionality by checking extent in each principal direction
  int aDimension = 3;
  int aDimCode   = 0;
  if (std::abs(myExtents[0]) <= theTol)
  {
    --aDimension;
    aDimCode = 1;
  }
  if (std::abs(myExtents[1]) <= theTol)
  {
    --aDimension;
    aDimCode = 2 * (aDimCode + 1);
  }
  if (std::abs(myExtents[2]) <= theTol)
  {
    --aDimension;
    aDimCode = 3 * (aDimCode + 1);
  }

  switch (aDimension)
  {
    case 0: {
      myType = Type::Point;
      break;
    }
    case 1: {
      myType = Type::Line;
      if (aDimCode == 4)
      {
        myV1 = myAxes[2];
      }
      else if (aDimCode == 6)
      {
        myV1 = myAxes[1];
      }
      else
      {
        myV1 = myAxes[0];
      }
      break;
    }
    case 2: {
      myType = Type::Plane;
      if (aDimCode == 1)
      {
        myV1 = myAxes[0];
      }
      else if (aDimCode == 2)
      {
        myV1 = myAxes[1];
      }
      else
      {
        myV1 = myAxes[2];
      }
      break;
    }
    case 3: {
      myType = Type::Space;
      myG.SetXYZ(myG.XYZ() + aMin1 * myAxes[0].XYZ() + aMin2 * myAxes[1].XYZ()
                 + aMin3 * myAxes[2].XYZ());
      myV1 = myExtents[0] * myAxes[0];
      myV2 = myExtents[1] * myAxes[1];
      myV3 = myExtents[2] * myAxes[2];
      break;
    }
  }
}

//=================================================================================================

gp_Pln PointSetLib_Equation::Plane() const
{
  if (!IsPlanar())
  {
    throw Standard_NoSuchObject("PointSetLib_Equation::Plane: result is not planar");
  }
  return gp_Pln(myG, myV1);
}

//=================================================================================================

gp_Lin PointSetLib_Equation::Line() const
{
  if (!IsLinear())
  {
    throw Standard_NoSuchObject("PointSetLib_Equation::Line: result is not linear");
  }
  return gp_Lin(myG, gp_Dir(myV1));
}

//=================================================================================================

gp_Pnt PointSetLib_Equation::Point() const
{
  if (!IsPoint())
  {
    throw Standard_NoSuchObject("PointSetLib_Equation::Point: result is not a point");
  }
  return myG;
}

//=================================================================================================

void PointSetLib_Equation::Box(gp_Pnt& theP, gp_Vec& theV1, gp_Vec& theV2, gp_Vec& theV3) const
{
  if (!IsSpace())
  {
    throw Standard_NoSuchObject("PointSetLib_Equation::Box: result is not a 3D space");
  }
  theP  = myG;
  theV1 = myV1;
  theV2 = myV2;
  theV3 = myV3;
}
