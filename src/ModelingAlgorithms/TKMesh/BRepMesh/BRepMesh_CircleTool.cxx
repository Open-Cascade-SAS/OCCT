// Created on: 1993-06-15
// Created by: Didier PIFFAULT
// Copyright (c) 1993-1999 Matra Datavision
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

#include <BRepMesh_CircleTool.hxx>
#include <BRepMesh_GeomTool.hxx>
#include <gp_Circ2d.hxx>
#include <Precision.hxx>
#include <BRepMesh_Circle.hxx>
#include <BRepMesh_CircleInspector.hxx>

//=================================================================================================

BRepMesh_CircleTool::BRepMesh_CircleTool(const occ::handle<NCollection_IncAllocator>& theAllocator)
    : myTolerance(Precision::PConfusion()),
      myAllocator(theAllocator),
      myCellFilter(10.0, theAllocator),
      mySelector(myTolerance, 64, theAllocator)
{
}

//=================================================================================================

BRepMesh_CircleTool::BRepMesh_CircleTool(const int theReservedSize,
                                         const occ::handle<NCollection_IncAllocator>& theAllocator)
    : myTolerance(Precision::PConfusion()),
      myAllocator(theAllocator),
      myCellFilter(10.0, theAllocator),
      mySelector(myTolerance, std::max(theReservedSize, 64), theAllocator)
{
}

//=================================================================================================

void BRepMesh_CircleTool::bind(const int theIndex, const gp_XY& theLocation, const double theRadius)
{
  BRepMesh_Circle aCirle(theLocation, theRadius);

  // compute coords
  double aMaxX = std::min(theLocation.X() + theRadius, myFaceMax.X());
  double aMinX = std::max(theLocation.X() - theRadius, myFaceMin.X());
  double aMaxY = std::min(theLocation.Y() + theRadius, myFaceMax.Y());
  double aMinY = std::max(theLocation.Y() - theRadius, myFaceMin.Y());

  gp_XY aMinPnt(aMinX, aMinY);
  gp_XY aMaxPnt(aMaxX, aMaxY);

  myCellFilter.Add(theIndex, aMinPnt, aMaxPnt);
  mySelector.Bind(theIndex, aCirle);
}

//=================================================================================================

void BRepMesh_CircleTool::Bind(const int theIndex, const gp_Circ2d& theCircle)
{
  gp_XY  aCoord  = theCircle.Location().Coord();
  double aRadius = theCircle.Radius();
  bind(theIndex, aCoord, aRadius);
}

//=================================================================================================

bool BRepMesh_CircleTool::MakeCircle(const gp_XY& thePoint1,
                                     const gp_XY& thePoint2,
                                     const gp_XY& thePoint3,
                                     gp_XY&       theLocation,
                                     double&      theRadius)
{
  static const double aPrecision   = Precision::PConfusion();
  static const double aSqPrecision = aPrecision * aPrecision;

  gp_XY aLink1(
    const_cast<gp_XY&>(thePoint3).ChangeCoord(1) - const_cast<gp_XY&>(thePoint2).ChangeCoord(1),
    const_cast<gp_XY&>(thePoint2).ChangeCoord(2) - const_cast<gp_XY&>(thePoint3).ChangeCoord(2));
  if (aLink1.SquareModulus() < aSqPrecision)
    return false;

  gp_XY aLink2(
    const_cast<gp_XY&>(thePoint1).ChangeCoord(1) - const_cast<gp_XY&>(thePoint3).ChangeCoord(1),
    const_cast<gp_XY&>(thePoint3).ChangeCoord(2) - const_cast<gp_XY&>(thePoint1).ChangeCoord(2));
  if (aLink2.SquareModulus() < aSqPrecision)
    return false;

  gp_XY aLink3(
    const_cast<gp_XY&>(thePoint2).ChangeCoord(1) - const_cast<gp_XY&>(thePoint1).ChangeCoord(1),
    const_cast<gp_XY&>(thePoint1).ChangeCoord(2) - const_cast<gp_XY&>(thePoint2).ChangeCoord(2));
  if (aLink3.SquareModulus() < aSqPrecision)
    return false;

  const double aD = 2
                    * (const_cast<gp_XY&>(thePoint1).ChangeCoord(1) * aLink1.Y()
                       + const_cast<gp_XY&>(thePoint2).ChangeCoord(1) * aLink2.Y()
                       + const_cast<gp_XY&>(thePoint3).ChangeCoord(1) * aLink3.Y());

  if (std::abs(aD) < gp::Resolution())
    return false;

  const double aInvD   = 1. / aD;
  const double aSqMod1 = thePoint1.SquareModulus();
  const double aSqMod2 = thePoint2.SquareModulus();
  const double aSqMod3 = thePoint3.SquareModulus();
  theLocation.ChangeCoord(1) =
    (aSqMod1 * aLink1.Y() + aSqMod2 * aLink2.Y() + aSqMod3 * aLink3.Y()) * aInvD;

  theLocation.ChangeCoord(2) =
    (aSqMod1 * aLink1.X() + aSqMod2 * aLink2.X() + aSqMod3 * aLink3.X()) * aInvD;

  theRadius = std::sqrt(std::max(std::max((thePoint1 - theLocation).SquareModulus(),
                                          (thePoint2 - theLocation).SquareModulus()),
                                 (thePoint3 - theLocation).SquareModulus()))
              + 2 * RealEpsilon();

  return true;
}

//=================================================================================================

bool BRepMesh_CircleTool::Bind(const int    theIndex,
                               const gp_XY& thePoint1,
                               const gp_XY& thePoint2,
                               const gp_XY& thePoint3)
{
  gp_XY  aLocation;
  double aRadius;
  if (!MakeCircle(thePoint1, thePoint2, thePoint3, aLocation, aRadius))
    return false;

  bind(theIndex, aLocation, aRadius);
  return true;
}

//=================================================================================================

void BRepMesh_CircleTool::Delete(const int theIndex)
{
  BRepMesh_Circle& aCircle = mySelector.Circle(theIndex);
  if (aCircle.Radius() > 0.)
    aCircle.SetRadius(-1);
}

//=================================================================================================

IMeshData::ListOfInteger& BRepMesh_CircleTool::Select(const gp_XY& thePoint)
{
  mySelector.SetPoint(thePoint);
  myCellFilter.Inspect(thePoint, mySelector);
  return mySelector.GetShotCircles();
}

//=================================================================================================

void BRepMesh_CircleTool::MocBind(const int theIndex)
{
  BRepMesh_Circle aNullCir(gp::Origin2d().Coord(), -1.);
  mySelector.Bind(theIndex, aNullCir);
}
