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

//=======================================================================
//function : Inspect
//purpose  : 
//=======================================================================
NCollection_CellFilter_Action BRepMesh_CircleInspector::Inspect(
  const Standard_Integer theTargetIndex)
{
  const BRepMesh_Circle& aCircle = myCircles(theTargetIndex);
  Standard_Real aRadius = aCircle.Radius();
  if(aRadius < 0.)
    return CellFilter_Purge;

  const gp_XY& aLoc = aCircle.Location();

  if ((myPoint - aLoc).SquareModulus() - (aRadius * aRadius) <= myTolerance)
    myResIndices.Append(theTargetIndex);

  return CellFilter_Keep;
}


//=======================================================================
//function : BRepMesh_CircleTool
//purpose  : 
//=======================================================================
BRepMesh_CircleTool::BRepMesh_CircleTool(
  const Handle(NCollection_IncAllocator)& theAllocator)
: myTolerance (Precision::PConfusion() * Precision::PConfusion()),
  myAllocator (theAllocator),
  myCellFilter(10, theAllocator),
  mySelector  (myTolerance, 64, theAllocator)
{
}

//=======================================================================
//function : BRepMesh_CircleTool
//purpose  : 
//=======================================================================
BRepMesh_CircleTool::BRepMesh_CircleTool(
  const Standard_Integer                  theReservedSize,
  const Handle(NCollection_IncAllocator)& theAllocator)
: myTolerance (Precision::PConfusion() * Precision::PConfusion()),
  myAllocator (theAllocator),
  myCellFilter(10, theAllocator),
  mySelector  (myTolerance, Max(theReservedSize, 64), theAllocator)
{
}

//=======================================================================
//function : bind
//purpose  : 
//=======================================================================
void BRepMesh_CircleTool::bind(const Standard_Integer theIndex,
                               const gp_XY&           theLocation,
                               const Standard_Real    theRadius)
{
  BRepMesh_Circle aCirle(theLocation, theRadius);

  //compute coords
  Standard_Real aMaxX = Min(theLocation.X() + theRadius, myFaceMax.X());
  Standard_Real aMinX = Max(theLocation.X() - theRadius, myFaceMin.X());
  Standard_Real aMaxY = Min(theLocation.Y() + theRadius, myFaceMax.Y());
  Standard_Real aMinY = Max(theLocation.Y() - theRadius, myFaceMin.Y());

  gp_XY aMinPnt(aMinX, aMinY);
  gp_XY aMaxPnt(aMaxX, aMaxY);

  myCellFilter.Add(theIndex, aMinPnt, aMaxPnt);
  mySelector.Bind(theIndex, aCirle);
}

//=======================================================================
//function : Bind
//purpose  : 
//=======================================================================
void BRepMesh_CircleTool::Bind(const Standard_Integer theIndex,
                               const gp_Circ2d&       theCircle)
{
  gp_XY         aCoord  = theCircle.Location().Coord();
  Standard_Real aRadius = theCircle.Radius();
  bind(theIndex, aCoord, aRadius);
}

//=======================================================================
//function : Bind
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_CircleTool::Bind(const Standard_Integer theIndex,
                                           const gp_XY&           thePoint1,
                                           const gp_XY&           thePoint2,
                                           const gp_XY&           thePoint3)
{
  const Standard_Real aPrecision   = Precision::PConfusion();
  const Standard_Real aSqPrecision = aPrecision * aPrecision;

  const gp_XY aPoints[3] = { thePoint1, thePoint2, thePoint3 };

  gp_XY aNorm[3];
  gp_XY aMidPnt[3];
  for (Standard_Integer i = 0; i < 3; ++i)
  {
    const gp_XY& aPnt1 = aPoints[i];
    const gp_XY& aPnt2 = aPoints[(i + 1) % 3];

    aMidPnt[i] = (aPnt1 + aPnt2) / 2.;

    gp_XY aLink(aPnt2 - aPnt1);
    if (aLink.SquareModulus() < aSqPrecision)
      return Standard_False;

    aNorm[i] = gp_XY(aLink.Y(), -aLink.X());
    aNorm[i].Add(aMidPnt[i]);
  }

  gp_XY aIntPnt;
  Standard_Real aParam[2];
  BRepMesh_GeomTool::IntFlag aIntFlag = 
    BRepMesh_GeomTool::IntLinLin(aMidPnt[0], aNorm[0],
      aMidPnt[1], aNorm[1], aIntPnt, aParam);

  if (aIntFlag != BRepMesh_GeomTool::Cross)
    return Standard_False;

  Standard_Real aRadius = (aPoints[0] - aIntPnt).Modulus();
  bind(theIndex, aIntPnt, aRadius);
  return Standard_True;
}

//=======================================================================
//function : Delete
//purpose  : 
//=======================================================================
void BRepMesh_CircleTool::Delete(const Standard_Integer theIndex)
{
  BRepMesh_Circle& aCircle = mySelector.Circle(theIndex);
  if(aCircle.Radius() > 0.)
    aCircle.SetRadius(-1);
}

//=======================================================================
//function : Select
//purpose  : 
//=======================================================================
BRepMesh::ListOfInteger& BRepMesh_CircleTool::Select(const gp_XY& thePoint)
{
  mySelector.SetPoint(thePoint);
  myCellFilter.Inspect(thePoint, mySelector);
  return mySelector.GetShotCircles();
}

//=======================================================================
//function : MocBind
//purpose  : 
//=======================================================================
void BRepMesh_CircleTool::MocBind(const Standard_Integer theIndex)
{
  BRepMesh_Circle aNullCir(gp::Origin2d().Coord(), -1.);
  mySelector.Bind(theIndex, aNullCir);
}
