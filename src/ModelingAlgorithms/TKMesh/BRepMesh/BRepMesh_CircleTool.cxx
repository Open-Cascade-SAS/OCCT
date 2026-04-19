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
      mySelector(myTolerance, 64, theAllocator)
{
}

//=================================================================================================

BRepMesh_CircleTool::BRepMesh_CircleTool(const int theReservedSize,
                                         const occ::handle<NCollection_IncAllocator>& theAllocator)
    : myTolerance(Precision::PConfusion()),
      myAllocator(theAllocator),
      mySelector(myTolerance, std::max(theReservedSize, 64), theAllocator)
{
}

//=================================================================================================

void BRepMesh_CircleTool::initGrid()
{
  if (myCellSizeX <= 0. || myCellSizeY <= 0.)
    return;

  const double aDeltaX = myFaceMax.X() - myFaceMin.X();
  const double aDeltaY = myFaceMax.Y() - myFaceMin.Y();
  if (aDeltaX <= 0. || aDeltaY <= 0.)
    return;

  myGridSizeX = std::max(1, static_cast<int>(std::ceil(aDeltaX / myCellSizeX)));
  myGridSizeY = std::max(1, static_cast<int>(std::ceil(aDeltaY / myCellSizeY)));

  // Safety cap to avoid excessive memory for pathological cases.
  constexpr int THE_MAX_GRID_SIZE = 10000;
  myGridSizeX = std::min(myGridSizeX, THE_MAX_GRID_SIZE);
  myGridSizeY = std::min(myGridSizeY, THE_MAX_GRID_SIZE);

  const int aTotalCells = myGridSizeX * myGridSizeY;
  myGrid.Resize(0, aTotalCells - 1, false);
  for (int i = 0; i < aTotalCells; ++i)
  {
    myGrid.SetValue(i, nullptr);
  }
  myGridReady = true;
}

//=================================================================================================

void BRepMesh_CircleTool::bind(const int theIndex, const gp_XY& theLocation, const double theRadius)
{
  BRepMesh_Circle aCircle(theLocation, theRadius);

  if (myGridReady)
  {
    // Clamp circle bounding box to the face bounds.
    const double aMaxX = std::min(theLocation.X() + theRadius, myFaceMax.X());
    const double aMinX = std::max(theLocation.X() - theRadius, myFaceMin.X());
    const double aMaxY = std::min(theLocation.Y() + theRadius, myFaceMax.Y());
    const double aMinY = std::max(theLocation.Y() - theRadius, myFaceMin.Y());

    // Insert into all cells the circle overlaps.
    int anIXMin = static_cast<int>(std::floor((aMinX - myFaceMin.X()) / myCellSizeX));
    int anIXMax = static_cast<int>(std::floor((aMaxX - myFaceMin.X()) / myCellSizeX));
    int anIYMin = static_cast<int>(std::floor((aMinY - myFaceMin.Y()) / myCellSizeY));
    int anIYMax = static_cast<int>(std::floor((aMaxY - myFaceMin.Y()) / myCellSizeY));

    anIXMin = std::clamp(anIXMin, 0, myGridSizeX - 1);
    anIXMax = std::clamp(anIXMax, 0, myGridSizeX - 1);
    anIYMin = std::clamp(anIYMin, 0, myGridSizeY - 1);
    anIYMax = std::clamp(anIYMax, 0, myGridSizeY - 1);

    for (int anIY = anIYMin; anIY <= anIYMax; ++anIY)
    {
      const int aRowOffset = anIY * myGridSizeX;
      for (int anIX = anIXMin; anIX <= anIXMax; ++anIX)
      {
        const int aIdx  = aRowOffset + anIX;
        GridNode* aNode = static_cast<GridNode*>(myAllocator->AllocateOptimal(sizeof(GridNode)));
        aNode->TriIndex = theIndex;
        aNode->Next     = myGrid.Value(aIdx);
        myGrid.SetValue(aIdx, aNode);
      }
    }
  }

  mySelector.Bind(theIndex, aCircle);
}

//=================================================================================================

void BRepMesh_CircleTool::Bind(const int theIndex, const gp_Circ2d& theCircle)
{
  gp_XY        aCoord  = theCircle.Location().Coord();
  const double aRadius = theCircle.Radius();
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

std::vector<int, NCollection_Allocator<int>>& BRepMesh_CircleTool::Select(const gp_XY& thePoint)
{
  mySelector.SetPoint(thePoint);

  if (myGridReady)
  {
    const int     aIdx  = cellIndex(thePoint.X(), thePoint.Y());
    GridNode** aPrev = &myGrid.ChangeValue(aIdx);
    GridNode*  aNode = *aPrev;
    while (aNode != nullptr)
    {
      GridNode* aNext = aNode->Next;
      const NCollection_CellFilter_Action anAction = mySelector.Inspect(aNode->TriIndex);
      if (anAction == CellFilter_Purge)
      {
        // Unlink purged node (lazy removal).
        // Node memory is reclaimed when IncAllocator resets.
        *aPrev = aNext;
      }
      else
      {
        aPrev = &aNode->Next;
      }
      aNode = aNext;
    }
  }
  else
  {
    // Fallback: grid not yet initialized, inspect all bound circles (O(N)).
    const int aLen = static_cast<int>(mySelector.Circles().Size());
    for (int i = 0; i < aLen; ++i)
    {
      mySelector.Inspect(i);
    }
  }

  return mySelector.GetShotCircles();
}

//=================================================================================================

void BRepMesh_CircleTool::MocBind(const int theIndex)
{
  BRepMesh_Circle aNullCir(gp::Origin2d().Coord(), -1.);
  mySelector.Bind(theIndex, aNullCir);
}
