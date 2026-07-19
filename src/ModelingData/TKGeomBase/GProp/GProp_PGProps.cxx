// Created on: 1992-02-14
// Created by: Jean Claude VAUTHIER
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <GProp_PGProps.hxx>

#include <Standard_DimensionError.hxx>
#include <Standard_DomainError.hxx>

//=================================================================================================

GProp_PGProps::GProp_PGProps()
{
  g       = gp::Origin();
  loc     = gp::Origin();
  dim     = 0.0;
  inertia = gp_Mat(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
}

//=================================================================================================

GProp_PGProps::GProp_PGProps(const NCollection_Array1<gp_Pnt>& thePnts)
    : GProp_PGProps()
{
  for (int i = thePnts.Lower(); i <= thePnts.Upper(); ++i)
  {
    AddPoint(thePnts(i));
  }
}

//=================================================================================================

GProp_PGProps::GProp_PGProps(const NCollection_Array2<gp_Pnt>& thePnts)
    : GProp_PGProps()
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

GProp_PGProps::GProp_PGProps(const NCollection_Array1<gp_Pnt>& thePnts,
                             const NCollection_Array1<double>& theDensity)
    : GProp_PGProps()
{
  if (thePnts.Length() != theDensity.Length())
  {
    throw Standard_DomainError("GProp_PGProps: points and density arrays have different lengths");
  }
  int aPntIdx  = thePnts.Lower();
  int aDensIdx = theDensity.Lower();
  for (; aPntIdx <= thePnts.Upper(); ++aPntIdx, ++aDensIdx)
  {
    AddPoint(thePnts(aPntIdx), theDensity(aDensIdx));
  }
}

//=================================================================================================

GProp_PGProps::GProp_PGProps(const NCollection_Array2<gp_Pnt>& thePnts,
                             const NCollection_Array2<double>& theDensity)
    : GProp_PGProps()
{
  if (thePnts.ColLength() != theDensity.ColLength()
      || thePnts.RowLength() != theDensity.RowLength())
  {
    throw Standard_DomainError(
      "GProp_PGProps: points and density arrays have different dimensions");
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

void GProp_PGProps::AddPoint(const gp_Pnt& thePnt)
{
  const double aX = thePnt.X();
  const double aY = thePnt.Y();
  const double aZ = thePnt.Z();

  const gp_Mat aMp(gp_XYZ(aY * aY + aZ * aZ, -aX * aY, -aX * aZ),
                   gp_XYZ(-aX * aY, aX * aX + aZ * aZ, -aY * aZ),
                   gp_XYZ(-aX * aZ, -aY * aZ, aX * aX + aY * aY));

  if (dim == 0.0)
  {
    dim     = 1.0;
    g       = thePnt;
    inertia = aMp;
  }
  else
  {
    const double aNewMass = dim + 1.0;
    const double aNewX    = (g.X() * dim + aX) / aNewMass;
    const double aNewY    = (g.Y() * dim + aY) / aNewMass;
    const double aNewZ    = (g.Z() * dim + aZ) / aNewMass;
    g.SetCoord(aNewX, aNewY, aNewZ);
    dim     = aNewMass;
    inertia = inertia + aMp;
  }
}

//=================================================================================================

void GProp_PGProps::AddPoint(const gp_Pnt& thePnt, double theDensity)
{
  if (theDensity <= gp::Resolution())
  {
    throw Standard_DomainError("GProp_PGProps::AddPoint: density must be positive");
  }

  const double aX = thePnt.X();
  const double aY = thePnt.Y();
  const double aZ = thePnt.Z();

  const gp_Mat aMp(gp_XYZ(aY * aY + aZ * aZ, -aX * aY, -aX * aZ),
                   gp_XYZ(-aX * aY, aX * aX + aZ * aZ, -aY * aZ),
                   gp_XYZ(-aX * aZ, -aY * aZ, aX * aX + aY * aY));

  if (dim == 0.0)
  {
    dim     = theDensity;
    g       = thePnt;
    inertia = aMp * theDensity;
  }
  else
  {
    const double aNewMass = dim + theDensity;
    const double aNewX    = (g.X() * dim + aX * theDensity) / aNewMass;
    const double aNewY    = (g.Y() * dim + aY * theDensity) / aNewMass;
    const double aNewZ    = (g.Z() * dim + aZ * theDensity) / aNewMass;
    g.SetCoord(aNewX, aNewY, aNewZ);
    dim     = aNewMass;
    inertia = inertia + aMp * theDensity;
  }
}

//=================================================================================================

gp_Pnt GProp_PGProps::Barycentre(const NCollection_Array1<gp_Pnt>& thePnts)
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

gp_Pnt GProp_PGProps::Barycentre(const NCollection_Array2<gp_Pnt>& thePnts)
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

void GProp_PGProps::Barycentre(const NCollection_Array1<gp_Pnt>& thePnts,
                               const NCollection_Array1<double>& theDensity,
                               double&                           theMass,
                               gp_Pnt&                           theG)
{
  if (thePnts.Length() != theDensity.Length())
  {
    throw Standard_DimensionError(
      "GProp_PGProps::Barycentre: points and density arrays have different lengths");
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

void GProp_PGProps::Barycentre(const NCollection_Array2<gp_Pnt>& thePnts,
                               const NCollection_Array2<double>& theDensity,
                               double&                           theMass,
                               gp_Pnt&                           theG)
{
  if (thePnts.RowLength() != theDensity.RowLength()
      || thePnts.ColLength() != theDensity.ColLength())
  {
    throw Standard_DimensionError(
      "GProp_PGProps::Barycentre: points and density arrays have different dimensions");
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
