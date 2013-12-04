// Created on: 1993-06-15
// Created by: Didier PIFFAULT
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BRepMesh_CircleTool.ixx>
#include <gp_XY.hxx>
#include <Precision.hxx>
#include <BRepMesh_Circ.hxx>
#include <BRepMesh_CircleInspector.hxx>
#include <BRepMesh_BaseAllocator.hxx>


//=======================================================================
//function : BRepMesh_CircleInspector
//purpose  : Constructor
//=======================================================================

BRepMesh_CircleInspector::BRepMesh_CircleInspector (Standard_Real theTol,
                                                    Standard_Integer nbComp,
                                                    const BRepMesh_BaseAllocator& theAlloc)
                                                    : myTol(theTol*theTol),
                                                    myResInd(theAlloc),
                                                    myInitCircle(nbComp)
{
  //  myTol = theTol*theTol;
}

//=======================================================================
//function : Inspect
//purpose  : 
//
//=======================================================================
NCollection_CellFilter_Action BRepMesh_CircleInspector::Inspect (const Standard_Integer theTarget)
{
  const BRepMesh_Circ& Circ = myInitCircle(theTarget);
  Standard_Real R = Circ.Radius();
  if(R < 0)
    return CellFilter_Purge;
  Standard_Real dx,dy;
  const gp_XY& aLoc=Circ.Location();
  dx=myCurrent.X()-aLoc.X();
  dy=myCurrent.Y()-aLoc.Y();
  if ((dx*dx+dy*dy)-(R*R) <= myTol)
    myResInd.Append(theTarget);
  return CellFilter_Keep;
}


//=======================================================================
//function : BRepMesh_CircleTool
//purpose  : 
//=======================================================================
BRepMesh_CircleTool::BRepMesh_CircleTool(const BRepMesh_BaseAllocator& theAlloc)
: Tolerance(Precision::PConfusion()),
Allocator(theAlloc),
CellFilter(10, theAlloc),
Selector(Tolerance,64,theAlloc)
{
  Tolerance=Tolerance*Tolerance;
}

//=======================================================================
//function : BRepMesh_CircleTool
//purpose  : 
//=======================================================================
BRepMesh_CircleTool::BRepMesh_CircleTool(const Standard_Integer nbComp,
                                         const BRepMesh_BaseAllocator& theAlloc)
                                         : Tolerance(Precision::PConfusion()),
                                         Allocator(theAlloc),
                                         CellFilter(10, theAlloc),
                                         Selector(Tolerance,Max(nbComp,64),theAlloc)
{
  Tolerance=Tolerance*Tolerance;
}


//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================
void BRepMesh_CircleTool::Initialize(const Standard_Integer /*nbComp*/)
{
  Tolerance=Precision::PConfusion();
  Tolerance=Tolerance*Tolerance;
}

void BRepMesh_CircleTool::SetCellSize(const Standard_Real theSize)
{
  CellFilter.Reset(theSize, Allocator);
}

void BRepMesh_CircleTool::SetCellSize(const Standard_Real theXSize, 
                                      const Standard_Real theYSize)
{
  Standard_Real aCellSize[2];
  aCellSize[0] = theXSize;
  aCellSize[1] = theYSize;

  CellFilter.Reset(aCellSize, Allocator);
}


void BRepMesh_CircleTool::SetMinMaxSize(const gp_XY& theMin,
                                        const gp_XY& theMax)
{
  FaceMin = theMin;
  FaceMax = theMax;
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void  BRepMesh_CircleTool::Add(const gp_Circ2d& theCirc,
                               const Standard_Integer theIndex)
{
  gp_XY aCoord(theCirc.Location().Coord());
  Standard_Real R = theCirc.Radius();
  BRepMesh_Circ aCir(aCoord, R);

  //compute coords
  Standard_Real xMax=Min(aCoord.X()+R,FaceMax.X());
  Standard_Real xMin=Max(aCoord.X()-R,FaceMin.X());
  Standard_Real yMax=Min(aCoord.Y()+R,FaceMax.Y());
  Standard_Real yMin=Max(aCoord.Y()-R,FaceMin.Y());

  gp_XY MinPnt(xMin,yMin);
  gp_XY MaxPnt(xMax,yMax);

  CellFilter.Add(theIndex, MinPnt, MaxPnt);
  Selector.Add(theIndex, aCir);
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_CircleTool::Add(const gp_XY& p1,
                                          const gp_XY& p2,
                                          const gp_XY& p3,
                                          const Standard_Integer theIndex)
{
  gp_XY m1((p1.X()+p2.X())/2., (p1.Y()+p2.Y())/2.);
  gp_XY m2((p2.X()+p3.X())/2., (p2.Y()+p3.Y())/2.);
  gp_XY m3((p3.X()+p1.X())/2., (p3.Y()+p1.Y())/2.);
  Standard_Real dx=m1.X()-m2.X();
  Standard_Real dy=m1.Y()-m2.Y();
  Standard_Real d12=(dx*dx)+(dy*dy);
  dx=m2.X()-m3.X();
  dy=m2.Y()-m3.Y();
  Standard_Real d23=(dx*dx)+(dy*dy);
  dx=m3.X()-m1.X();
  dy=m3.Y()-m1.Y();
  Standard_Real d31=(dx*dx)+(dy*dy);
  gp_XY pl11, pl12, pl21, pl22;

  if (d12>d23 && d12>d31) {
    dy=p2.Y()-p1.Y();
    dx=p1.X()-p2.X();
    if (dy!=0. || dx!=0.) {
      pl11 = m1;
      pl12 = gp_XY(dy, dx);
    }
    else return Standard_False;

    dy=p3.Y()-p2.Y();
    dx=p2.X()-p3.X();
    if (dy!=0. || dx!=0.) {
      pl21 = m2;
      pl22 = gp_XY(dy, dx);
    }
    else return Standard_False;
  }
  else {
    if (d23>d31) {
      dy=p3.Y()-p2.Y();
      dx=p2.X()-p3.X();
      if (dy!=0. || dx!=0.) {
        pl11 = m2;
        pl12 = gp_XY(dy, dx);
      }
      else return Standard_False;

      dy=p1.Y()-p3.Y();
      dx=p3.X()-p1.X();
      if (dy!=0. || dx!=0.) {
        pl21 = m3;
        pl22 = gp_XY(dy, dx);
      }
      else return Standard_False;
    }
    else {
      dy=p1.Y()-p3.Y();
      dx=p3.X()-p1.X();
      if (dy!=0. || dx!=0.) {
        pl11 = m3;
        pl12 = gp_XY(dy, dx);
      }
      else return Standard_False;

      dy=p2.Y()-p1.Y();
      dx=p1.X()-p2.X();
      if (dy!=0. || dx!=0.) {
        pl21 = m1;
        pl22 = gp_XY(dy, dx);
      }
      else return Standard_False;
    }
  }

  gp_XY aVecO1O2 = pl21 - pl11;
  Standard_Real aCrossD1D2 = pl12 ^ pl22;
  Standard_Real theSinAngle = Abs(aCrossD1D2);
  if (theSinAngle < gp::Resolution())
    return Standard_False;
  Standard_Real theParam1 = (aVecO1O2 ^ pl22) / aCrossD1D2;
  gp_XY pInt = pl11+pl12*theParam1;
  dx=p1.X()-pInt.X();
  dy=p1.Y()-pInt.Y();
  Standard_Real R = Sqrt(dx*dx+dy*dy);
  BRepMesh_Circ aCir(pInt, R);

  //compute coords
  Standard_Real xMax=Min(pInt.X()+R,FaceMax.X());
  Standard_Real xMin=Max(pInt.X()-R,FaceMin.X());
  Standard_Real yMax=Min(pInt.Y()+R,FaceMax.Y());
  Standard_Real yMin=Max(pInt.Y()-R,FaceMin.Y());

  gp_XY MinPnt(xMin,yMin);
  gp_XY MaxPnt(xMax,yMax);    

  CellFilter.Add(theIndex, MinPnt, MaxPnt);

  Selector.Add(theIndex, aCir);
  return Standard_True;
}

//=======================================================================
//function : Delete
//purpose  : 
//=======================================================================
void  BRepMesh_CircleTool::Delete(const Standard_Integer theIndex)
{
  BRepMesh_Circ& Circ = Selector.GetCirc(theIndex);
  if(Circ.Radius() > 0.) {
    Circ.SetRadius(-1);
  }
}

//=======================================================================
//function : Select
//purpose  : 
//=======================================================================
BRepMesh_ListOfInteger&  BRepMesh_CircleTool::Select(const gp_XY& thePnt)
{
  Selector.ClerResList();
  Selector.SetCurrent(thePnt);
  CellFilter.Inspect (thePnt, Selector);
  return Selector.GetCoincidentInd();
}

void BRepMesh_CircleTool::MocAdd(const Standard_Integer theIndex)
{
  gp_XY nullPnt(0.,0.);
  BRepMesh_Circ theNullCir(nullPnt, -1.);
  Selector.Add(theIndex, theNullCir);
}
