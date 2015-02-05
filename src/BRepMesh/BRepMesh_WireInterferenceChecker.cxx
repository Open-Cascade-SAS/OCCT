// Created on: 2014-06-18
// Created by: Oleg AGASHIN
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#include <BRepMesh_WireInterferenceChecker.hxx>
#include <BRepMesh_GeomTool.hxx>
#include <Precision.hxx>

// TODO: remove this variable after implementation of LoopChecker2d.
static const Standard_Real MIN_LOOP_S = 2 * M_PI * 2.E-5;

//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================
BRepMesh_WireInterferenceChecker::BRepMesh_WireInterferenceChecker(
  const BRepMesh::Array1OfSegmentsTree& theWires,
  BRepMesh_Status*                      theStatus,
  Standard_Mutex*                       theMutex)
: myWires (theWires),
  myStatus(theStatus),
  myMutex (theMutex)
{
}

//=======================================================================
//function : Checker's body
//purpose  : 
//=======================================================================
void BRepMesh_WireInterferenceChecker::operator ()(
  const Standard_Integer& theWireId) const
{
  if (*myStatus == BRepMesh_SelfIntersectingWire)
    return;

  const BRepMesh::SegmentsTree&      aWireSegTree1  = myWires(theWireId);
  const BRepMesh::HArray1OfSegments& aWireSegments1 = aWireSegTree1.first;
  const BRepMesh::HBndBox2dTree&     aWireBoxTree1  = aWireSegTree1.second;

  for (Standard_Integer aWireIt = theWireId; aWireIt <= myWires.Upper(); ++aWireIt)
  {
    // Break execution in case if flag was raised by another thread.
    if (*myStatus == BRepMesh_SelfIntersectingWire)
      return;

    const Standard_Boolean isSelfIntCheck = (aWireIt == theWireId);
    const BRepMesh::SegmentsTree& aWireSegTree2 = 
      isSelfIntCheck ? aWireSegTree1 : myWires(aWireIt);

    const BRepMesh::HArray1OfSegments& aWireSegments2 = aWireSegTree2.first;
    const BRepMesh::HBndBox2dTree&     aWireBoxTree2  = aWireSegTree2.second;

    BRepMesh_WireChecker::BndBox2dTreeSelector aSelector (aWireSegments2->Size());

    Standard_Integer aSegmentId1 = aWireSegments1->Lower();
    for (; aSegmentId1 <= aWireSegments1->Upper(); ++aSegmentId1)
    {
      // Break execution in case if flag was raised by another thread
      if (*myStatus == BRepMesh_SelfIntersectingWire)
        return;

      aSelector.Clear();
      aSelector.SetBox(aWireBoxTree1->FindNode(aSegmentId1).Bnd());
      if (isSelfIntCheck)
        aSelector.SetSkippedIndex(aSegmentId1);

      if (aWireBoxTree2->Select(aSelector) == 0)
        continue;

      const BRepMesh::Segment& aSegment1 = aWireSegments1->Value(aSegmentId1);
      const BRepMesh::Array1OfInteger& aSelected = aSelector.Indices();
      const Standard_Integer aSelectedNb = aSelector.IndicesNb();
      for (Standard_Integer aBndIt = 0; aBndIt < aSelectedNb; ++aBndIt)
      {
        // Break execution in case if flag was raised by another thread
        if (*myStatus == BRepMesh_SelfIntersectingWire)
          return;

        const Standard_Integer aSegmentId2 = aSelected(aBndIt);
        const BRepMesh::Segment& aSegment2 = aWireSegments2->Value(aSegmentId2);

        gp_Pnt2d aIntPnt;
        BRepMesh_GeomTool::IntFlag aIntStatus = BRepMesh_GeomTool::IntSegSeg(
          aSegment1.StartPnt, aSegment1.EndPnt, 
          aSegment2.StartPnt, aSegment2.EndPnt,
          Standard_False, Standard_False,
          aIntPnt);

        if (aIntStatus == BRepMesh_GeomTool::Cross)
        {
          // TODO: remove this block after implementation of LoopChecker2d.
          if (isSelfIntCheck)
          {
            gp_XY aPrevVec;
            Standard_Real aSumS = 0.;
            const gp_XY& aRefPnt  = aIntPnt.Coord();
            for (Standard_Integer i = aSegmentId1; i < aSegmentId2; ++i)
            {
              const BRepMesh::Segment& aSeg = aWireSegments1->Value(i);
              gp_XY aCurVec = aSeg.EndPnt - aRefPnt;

              if (aCurVec.SquareModulus() < gp::Resolution())
                continue;

              if (aPrevVec.SquareModulus() > gp::Resolution())
                aSumS += aPrevVec ^ aCurVec;

              aPrevVec = aCurVec;
            }

            if (Abs(aSumS / 2.) < MIN_LOOP_S)
              continue;
          }

          Standard_Mutex::Sentry aSentry(myMutex);
          *myStatus = BRepMesh_SelfIntersectingWire;

          return;
        }
      }
    }
  }
}
