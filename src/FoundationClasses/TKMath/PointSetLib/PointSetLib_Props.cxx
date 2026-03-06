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

#include <PointSetLib_Props.hxx>

#include <Standard_DimensionError.hxx>
#include <Standard_DomainError.hxx>

namespace
{
//! Compute Huygens (parallel axis theorem) operator for shifting inertia.
//! Returns the inertia contribution of a point mass theMass at theG,
//! relative to the point theQ.
static gp_Mat huygensOperator(const gp_Pnt& theG, const gp_Pnt& theQ, double theMass)
{
  const gp_XYZ aQG = theG.XYZ() - theQ.XYZ();
  const double aX  = aQG.X();
  const double aY  = aQG.Y();
  const double aZ  = aQG.Z();
  gp_Mat       aMat(gp_XYZ(aY * aY + aZ * aZ, -aX * aY, -aX * aZ),
              gp_XYZ(-aX * aY, aX * aX + aZ * aZ, -aY * aZ),
              gp_XYZ(-aX * aZ, -aY * aZ, aX * aX + aY * aY));
  aMat.Multiply(theMass);
  return aMat;
}
} // namespace

//=================================================================================================

PointSetLib_Props::PointSetLib_Props()
    : myMass(0.0),
      myInertiaAtOrigin(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0)
{
}

//=================================================================================================

PointSetLib_Props::PointSetLib_Props(const NCollection_Array1<gp_Pnt>& thePnts)
    : PointSetLib_Props()
{
  for (int i = thePnts.Lower(); i <= thePnts.Upper(); ++i)
  {
    AddPoint(thePnts(i));
  }
}

//=================================================================================================

PointSetLib_Props::PointSetLib_Props(const NCollection_Array2<gp_Pnt>& thePnts)
    : PointSetLib_Props()
{
  for (int j = thePnts.LowerCol(); j <= thePnts.UpperCol(); ++j)
  {
    for (int i = thePnts.LowerRow(); i <= thePnts.UpperRow(); ++i)
    {
      AddPoint(thePnts(i, j));
    }
  }
}

//=================================================================================================

PointSetLib_Props::PointSetLib_Props(const NCollection_Array1<gp_Pnt>& thePnts,
                                     const NCollection_Array1<double>& theDensity)
    : PointSetLib_Props()
{
  if (thePnts.Length() != theDensity.Length())
  {
    throw Standard_DomainError(
      "PointSetLib_Props: points and density arrays have different lengths");
  }
  int aPntIdx  = thePnts.Lower();
  int aDensIdx = theDensity.Lower();
  for (; aPntIdx <= thePnts.Upper(); ++aPntIdx, ++aDensIdx)
  {
    AddPoint(thePnts(aPntIdx), theDensity(aDensIdx));
  }
}

//=================================================================================================

PointSetLib_Props::PointSetLib_Props(const NCollection_Array2<gp_Pnt>& thePnts,
                                     const NCollection_Array2<double>& theDensity)
    : PointSetLib_Props()
{
  if (thePnts.ColLength() != theDensity.ColLength()
      || thePnts.RowLength() != theDensity.RowLength())
  {
    throw Standard_DomainError(
      "PointSetLib_Props: points and density arrays have different dimensions");
  }
  int aPntCol  = thePnts.LowerCol();
  int aDensCol = theDensity.LowerCol();
  for (; aPntCol <= thePnts.UpperCol(); ++aPntCol, ++aDensCol)
  {
    int aPntRow  = thePnts.LowerRow();
    int aDensRow = theDensity.LowerRow();
    for (; aPntRow <= thePnts.UpperRow(); ++aPntRow, ++aDensRow)
    {
      AddPoint(thePnts(aPntRow, aPntCol), theDensity(aDensRow, aDensCol));
    }
  }
}

//=================================================================================================

void PointSetLib_Props::AddPoint(const gp_Pnt& thePnt)
{
  const double aX = thePnt.X();
  const double aY = thePnt.Y();
  const double aZ = thePnt.Z();

  // Inertia contribution at origin
  const gp_Mat aMp(gp_XYZ(aY * aY + aZ * aZ, -aX * aY, -aX * aZ),
                   gp_XYZ(-aX * aY, aX * aX + aZ * aZ, -aY * aZ),
                   gp_XYZ(-aX * aZ, -aY * aZ, aX * aX + aY * aY));

  if (myMass == 0.0)
  {
    myMass            = 1.0;
    myCentreOfMass    = thePnt;
    myInertiaAtOrigin = aMp;
  }
  else
  {
    const double aNewMass = myMass + 1.0;
    const double aNewX    = (myCentreOfMass.X() * myMass + aX) / aNewMass;
    const double aNewY    = (myCentreOfMass.Y() * myMass + aY) / aNewMass;
    const double aNewZ    = (myCentreOfMass.Z() * myMass + aZ) / aNewMass;
    myCentreOfMass.SetCoord(aNewX, aNewY, aNewZ);
    myMass            = aNewMass;
    myInertiaAtOrigin = myInertiaAtOrigin + aMp;
  }
}

//=================================================================================================

void PointSetLib_Props::AddPoint(const gp_Pnt& thePnt, double theDensity)
{
  if (theDensity <= gp::Resolution())
  {
    throw Standard_DomainError("PointSetLib_Props::AddPoint: density must be positive");
  }

  const double aX = thePnt.X();
  const double aY = thePnt.Y();
  const double aZ = thePnt.Z();

  gp_Mat aMp(gp_XYZ(aY * aY + aZ * aZ, -aX * aY, -aX * aZ),
             gp_XYZ(-aX * aY, aX * aX + aZ * aZ, -aY * aZ),
             gp_XYZ(-aX * aZ, -aY * aZ, aX * aX + aY * aY));

  if (myMass == 0.0)
  {
    myMass            = theDensity;
    myCentreOfMass    = thePnt;
    myInertiaAtOrigin = aMp * theDensity;
  }
  else
  {
    const double aNewMass = myMass + theDensity;
    const double aNewX    = (myCentreOfMass.X() * myMass + aX * theDensity) / aNewMass;
    const double aNewY    = (myCentreOfMass.Y() * myMass + aY * theDensity) / aNewMass;
    const double aNewZ    = (myCentreOfMass.Z() * myMass + aZ * theDensity) / aNewMass;
    myCentreOfMass.SetCoord(aNewX, aNewY, aNewZ);
    myMass            = aNewMass;
    myInertiaAtOrigin = myInertiaAtOrigin + aMp * theDensity;
  }
}

//=================================================================================================

gp_Mat PointSetLib_Props::MatrixOfInertia() const
{
  if (myMass == 0.0)
  {
    return gp_Mat(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  }
  // Shift inertia from origin to centre of mass using Huygens theorem
  return myInertiaAtOrigin - huygensOperator(myCentreOfMass, gp::Origin(), myMass);
}

//=================================================================================================

gp_Pnt PointSetLib_Props::Barycentre(const NCollection_Array1<gp_Pnt>& thePnts)
{
  gp_XYZ aSum = thePnts(thePnts.Lower()).XYZ();
  for (int i = thePnts.Lower() + 1; i <= thePnts.Upper(); ++i)
  {
    aSum.Add(thePnts(i).XYZ());
  }
  aSum.Divide(thePnts.Length());
  return gp_Pnt(aSum);
}

//=================================================================================================

gp_Pnt PointSetLib_Props::Barycentre(const NCollection_Array2<gp_Pnt>& thePnts)
{
  gp_XYZ aSum(0.0, 0.0, 0.0);
  for (int j = thePnts.LowerCol(); j <= thePnts.UpperCol(); ++j)
  {
    for (int i = thePnts.LowerRow(); i <= thePnts.UpperRow(); ++i)
    {
      aSum.Add(thePnts(i, j).XYZ());
    }
  }
  aSum.Divide(thePnts.RowLength() * thePnts.ColLength());
  return gp_Pnt(aSum);
}

//=================================================================================================

void PointSetLib_Props::Barycentre(const NCollection_Array1<gp_Pnt>& thePnts,
                                   const NCollection_Array1<double>& theDensity,
                                   double&                           theMass,
                                   gp_Pnt&                           theG)
{
  if (thePnts.Length() != theDensity.Length())
  {
    throw Standard_DimensionError(
      "PointSetLib_Props::Barycentre: points and density arrays have different lengths");
  }

  int aPntIdx  = thePnts.Lower();
  int aDensIdx = theDensity.Lower();
  theMass      = theDensity(aDensIdx);
  gp_XYZ aSum  = thePnts(aPntIdx).XYZ();
  aSum.Multiply(theMass);
  ++aPntIdx;
  ++aDensIdx;
  for (; aPntIdx <= thePnts.Upper(); ++aPntIdx, ++aDensIdx)
  {
    theMass += theDensity(aDensIdx);
    aSum.Add(thePnts(aPntIdx).XYZ().Multiplied(theDensity(aDensIdx)));
  }
  aSum.Divide(theMass);
  theG.SetXYZ(aSum);
}

//=================================================================================================

void PointSetLib_Props::Barycentre(const NCollection_Array2<gp_Pnt>& thePnts,
                                   const NCollection_Array2<double>& theDensity,
                                   double&                           theMass,
                                   gp_Pnt&                           theG)
{
  if (thePnts.RowLength() != theDensity.RowLength()
      || thePnts.ColLength() != theDensity.ColLength())
  {
    throw Standard_DimensionError(
      "PointSetLib_Props::Barycentre: points and density arrays have different dimensions");
  }

  theMass = 0.0;
  gp_XYZ aSum(0.0, 0.0, 0.0);
  int    aPntCol  = thePnts.LowerCol();
  int    aDensCol = theDensity.LowerCol();
  for (; aPntCol <= thePnts.UpperCol(); ++aPntCol, ++aDensCol)
  {
    int aPntRow  = thePnts.LowerRow();
    int aDensRow = theDensity.LowerRow();
    for (; aPntRow <= thePnts.UpperRow(); ++aPntRow, ++aDensRow)
    {
      theMass += theDensity(aDensRow, aDensCol);
      aSum.Add(thePnts(aPntRow, aPntCol).XYZ().Multiplied(theDensity(aDensRow, aDensCol)));
    }
  }
  aSum.Divide(theMass);
  theG.SetXYZ(aSum);
}
