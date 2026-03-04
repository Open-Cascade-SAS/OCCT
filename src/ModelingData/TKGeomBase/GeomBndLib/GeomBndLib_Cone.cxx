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

#include <GeomBndLib_Cone.hxx>

#include <ElSLib.hxx>
#include <GeomBndLib_Circle.hxx>
#include <GeomBndLib_InfiniteHelpers.pxx>
#include <gp_Circ.hxx>
#include <gp_Cone.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>

//=================================================================================================

//! Compute bounding box for a finite cone patch.
static void computeCone(const gp_Cone& theCone,
                         const double   theUMin,
                         const double   theUMax,
                         const double   theVMin,
                         const double   theVMax,
                         Bnd_Box&       theBox)
{
  const gp_Ax3& aPos  = theCone.Position();
  const double  aR    = theCone.RefRadius();
  const double  aSAng = theCone.SemiAngle();

  // Add circle at VMin.
  gp_Circ aC = ElSLib::ConeVIso(aPos, aR, aSAng, theVMin);
  if (aC.Radius() > Precision::Confusion())
  {
    GeomBndLib_Circle::Add(aC,theUMin, theUMax, 0., theBox);
  }
  else
  {
    theBox.Add(aC.Location());
  }

  // Add circle at VMax.
  aC = ElSLib::ConeVIso(aPos, aR, aSAng, theVMax);
  if (aC.Radius() > Precision::Confusion())
  {
    GeomBndLib_Circle::Add(aC,theUMin, theUMax, 0., theBox);
  }
  else
  {
    theBox.Add(aC.Location());
  }
}

//=================================================================================================

void GeomBndLib_Cone::Add(double theTol, Bnd_Box& theBox) const
{
  double aU1, aU2, aV1, aV2;
  myGeom->Bounds(aU1, aU2, aV1, aV2);
  Add(aU1, aU2, aV1, aV2, theTol, theBox);
}

//=================================================================================================

void GeomBndLib_Cone::Add(double   theUMin,
                           double   theUMax,
                           double   theVMin,
                           double   theVMax,
                           double   theTol,
                           Bnd_Box& theBox) const
{
  const gp_Cone aCone = myGeom->Cone();
  const gp_Dir& aDir = aCone.Axis().Direction();

  if (Precision::IsNegativeInfinite(theVMin))
  {
    if (Precision::IsNegativeInfinite(theVMax))
    {
      throw Standard_Failure("GeomBndLib_Cone::Add - bad parameter");
    }
    else if (Precision::IsPositiveInfinite(theVMax))
    {
      GeomBndLib_InfiniteHelpers::OpenMinMax(aDir, theBox);
    }
    else
    {
      computeCone(aCone, theUMin, theUMax, 0., theVMax, theBox);
      GeomBndLib_InfiniteHelpers::OpenMin(aDir, theBox);
    }
  }
  else if (Precision::IsPositiveInfinite(theVMin))
  {
    if (Precision::IsNegativeInfinite(theVMax))
    {
      GeomBndLib_InfiniteHelpers::OpenMinMax(aDir, theBox);
    }
    else if (Precision::IsPositiveInfinite(theVMax))
    {
      throw Standard_Failure("GeomBndLib_Cone::Add - bad parameter");
    }
    else
    {
      computeCone(aCone, theUMin, theUMax, 0., theVMax, theBox);
      GeomBndLib_InfiniteHelpers::OpenMax(aDir, theBox);
    }
  }
  else
  {
    if (Precision::IsNegativeInfinite(theVMax))
    {
      computeCone(aCone, theUMin, theUMax, theVMin, 0., theBox);
      GeomBndLib_InfiniteHelpers::OpenMin(aDir, theBox);
    }
    else if (Precision::IsPositiveInfinite(theVMax))
    {
      computeCone(aCone, theUMin, theUMax, theVMin, 0., theBox);
      GeomBndLib_InfiniteHelpers::OpenMax(aDir, theBox);
    }
    else
    {
      computeCone(aCone, theUMin, theUMax, theVMin, theVMax, theBox);
    }
  }

  theBox.Enlarge(theTol);
}
