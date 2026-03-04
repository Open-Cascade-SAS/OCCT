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

#include <GeomBndLib_Cylinder.hxx>

#include <ElSLib.hxx>
#include <GeomBndLib_Circle.hxx>
#include <GeomBndLib_InfiniteHelpers.pxx>
#include <gp_Circ.hxx>
#include <gp_Cylinder.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>

//=================================================================================================

void GeomBndLib_Cylinder::Add(double theTol, Bnd_Box& theBox) const
{
  double aU1, aU2, aV1, aV2;
  myGeom->Bounds(aU1, aU2, aV1, aV2);
  Add(aU1, aU2, aV1, aV2, theTol, theBox);
}

//=================================================================================================

//! Compute bounding box for a finite cylinder patch.
static void computeCylinder(const gp_Cylinder& theCyl,
                             const double       theUMin,
                             const double       theUMax,
                             const double       theVMin,
                             const double       theVMax,
                             Bnd_Box&           theBox)
{
  // Add circle at VMin.
  gp_Circ aC = ElSLib::CylinderVIso(theCyl.Position(), theCyl.Radius(), theVMin);
  GeomBndLib_Circle::Add(aC,theUMin, theUMax, 0., theBox);
  // Add circle at VMax by translating.
  gp_Vec aT = (theVMax - theVMin) * theCyl.Axis().Direction();
  aC.Translate(aT);
  GeomBndLib_Circle::Add(aC,theUMin, theUMax, 0., theBox);
}

//=================================================================================================

void GeomBndLib_Cylinder::Add(double   theUMin,
                              double   theUMax,
                              double   theVMin,
                              double   theVMax,
                              double   theTol,
                              Bnd_Box& theBox) const
{
  const gp_Cylinder aCyl = myGeom->Cylinder();
  const gp_Dir& aDir = aCyl.Axis().Direction();

  if (Precision::IsNegativeInfinite(theVMin))
  {
    if (Precision::IsNegativeInfinite(theVMax))
    {
      throw Standard_Failure("GeomBndLib_Cylinder::Add - bad parameter");
    }
    else if (Precision::IsPositiveInfinite(theVMax))
    {
      GeomBndLib_InfiniteHelpers::OpenMinMax(aDir, theBox);
    }
    else
    {
      computeCylinder(aCyl, theUMin, theUMax, 0., theVMax, theBox);
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
      throw Standard_Failure("GeomBndLib_Cylinder::Add - bad parameter");
    }
    else
    {
      computeCylinder(aCyl, theUMin, theUMax, 0., theVMax, theBox);
      GeomBndLib_InfiniteHelpers::OpenMax(aDir, theBox);
    }
  }
  else
  {
    if (Precision::IsNegativeInfinite(theVMax))
    {
      computeCylinder(aCyl, theUMin, theUMax, theVMin, 0., theBox);
      GeomBndLib_InfiniteHelpers::OpenMin(aDir, theBox);
    }
    else if (Precision::IsPositiveInfinite(theVMax))
    {
      computeCylinder(aCyl, theUMin, theUMax, theVMin, 0., theBox);
      GeomBndLib_InfiniteHelpers::OpenMax(aDir, theBox);
    }
    else
    {
      computeCylinder(aCyl, theUMin, theUMax, theVMin, theVMax, theBox);
    }
  }

  theBox.Enlarge(theTol);
}
