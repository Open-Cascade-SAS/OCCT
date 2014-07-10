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
#include <Precision.hxx>

// TODO: remove this variable after implementation of LoopChecker2d.
static const Standard_Real MIN_LOOP_S = 2 * M_PI * 2.E-5;

#ifdef HAVE_TBB
//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepMesh_WireInterferenceChecker::BRepMesh_WireInterferenceChecker(
  const std::vector<BRepMesh_WireChecker::SegmentsTree>& theWires,
  BRepMesh_Status*                                       theStatus,
  Standard_Mutex*                                        theMutex)
  : myWires(&theWires.front()),
    myWiresNb(theWires.size()),
    myStatus(theStatus),
    myMutex(theMutex)
{
}

//=======================================================================
//function : Checker's body
//purpose  : 
//=======================================================================
void BRepMesh_WireInterferenceChecker::operator ()(
  const tbb::blocked_range<Standard_Integer>& theWireRange) const
{
  for (Standard_Integer i = theWireRange.begin(); i != theWireRange.end(); ++i)
    this->operator ()(i);
}
#else
//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepMesh_WireInterferenceChecker::BRepMesh_WireInterferenceChecker(
  const std::vector<BRepMesh_WireChecker::SegmentsTree>& theWires,
  BRepMesh_Status*                                       theStatus)
  : myWires(&theWires.front()),
    myWiresNb(theWires.size()),
    myStatus(theStatus)
{
}
#endif

//=======================================================================
//function : Checker's body
//purpose  : 
//=======================================================================
void BRepMesh_WireInterferenceChecker::operator ()(
  const Standard_Integer& theWireId) const
{
  if (*myStatus == BRepMesh_SelfIntersectingWire)
    return;

  const BRepMesh_WireChecker::SegmentsTree&  aWireSegTree1  = myWires[theWireId];
  const BRepMesh_WireChecker::Segment*       aWireSegments1 = &aWireSegTree1.first->front();
  const BRepMesh_WireChecker::HBndBox2dTree& aWireBoxTree1  = aWireSegTree1.second;
  const Standard_Integer aWireLen1 = aWireSegTree1.first->size();

  for (Standard_Integer aWireIt = theWireId; aWireIt < myWiresNb; ++aWireIt)
  {
#ifdef HAVE_TBB
    // Break execution in case if flag was raised by another thread
    if (*myStatus == BRepMesh_SelfIntersectingWire)
      return;
#endif

    const Standard_Boolean isSelfIntCheck = (aWireIt == theWireId);
    const BRepMesh_WireChecker::SegmentsTree& aWireSegTree2 = 
      isSelfIntCheck ? aWireSegTree1 : myWires[aWireIt];

    const BRepMesh_WireChecker::Segment*       aWireSegments2 = &aWireSegTree2.first->front();
    const BRepMesh_WireChecker::HBndBox2dTree& aWireBoxTree2  = aWireSegTree2.second;

    BRepMesh_WireChecker::BndBox2dTreeSelector aSelector(aWireSegTree2.first->size());
    for (Standard_Integer aSegmentId1 = 0; aSegmentId1 < aWireLen1; ++aSegmentId1)
    {
#ifdef HAVE_TBB
      // Break execution in case if flag was raised by another thread
      if (*myStatus == BRepMesh_SelfIntersectingWire)
        return;
#endif

      aSelector.Clear();
      aSelector.SetBox(aWireBoxTree1->FindNode(aSegmentId1).Bnd());
      if (isSelfIntCheck)
        aSelector.SetSkippedIndex(aSegmentId1);

      if (aWireBoxTree2->Select(aSelector) == 0)
        continue;

      const BRepMesh_WireChecker::Segment& aSegment1 = aWireSegments1[aSegmentId1];
      const BRepMesh_WireChecker::ArrayOfInteger& aSelected = aSelector.Indices();
      const Standard_Integer aSelectedNb = aSelector.IndicesNb();
      for (Standard_Integer aBndIt = 0; aBndIt < aSelectedNb; ++aBndIt)
      {
#ifdef HAVE_TBB
        // Break execution in case if flag was raised by another thread
        if (*myStatus == BRepMesh_SelfIntersectingWire)
          return;
#endif

        const Standard_Integer aSegmentId2 = aSelected(aBndIt);
        const BRepMesh_WireChecker::Segment& aSegment2 = aWireSegments2[aSegmentId2];

        gp_Pnt2d aIntPnt;
        BRepMesh_WireInterferenceChecker::IntFlag aIntStatus = Intersect(
          aSegment1.StartPnt, aSegment1.EndPnt, 
          aSegment2.StartPnt, aSegment2.EndPnt,
          Standard_False, Standard_False,
          aIntPnt);

        if (aIntStatus == Cross)
        {
          // TODO: remove this block after implementation of LoopChecker2d.
          if (isSelfIntCheck)
          {
            gp_XY aPrevVec;
            Standard_Real aSumS = 0.;
            const gp_XY& aRefPnt  = aIntPnt.Coord();
            for (Standard_Integer i = aSegmentId1; i < aSegmentId2; ++i)
            {
              const BRepMesh_WireChecker::Segment& aSeg = aWireSegments1[i];
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

#ifdef HAVE_TBB
          Standard_Mutex::Sentry aSentry(myMutex);
#endif
          *myStatus = BRepMesh_SelfIntersectingWire;
          return;
        }
      }
    }
  }
}

//=============================================================================
//function : Intersect
//purpose  : 
//=============================================================================
BRepMesh_WireInterferenceChecker::IntFlag 
  BRepMesh_WireInterferenceChecker::Intersect(
  const gp_XY&           theStartPnt1,
  const gp_XY&           theEndPnt1,
  const gp_XY&           theStartPnt2,
  const gp_XY&           theEndPnt2,
  const Standard_Boolean isConsiderEndPointTouch,
  const Standard_Boolean isConsiderPointOnSegment,
  gp_Pnt2d&              theIntPnt)
{
  Standard_Integer aPointHash[] = {
    classifyPoint(theStartPnt1, theEndPnt1, theStartPnt2),
    classifyPoint(theStartPnt1, theEndPnt1, theEndPnt2  ),
    classifyPoint(theStartPnt2, theEndPnt2, theStartPnt1),
    classifyPoint(theStartPnt2, theEndPnt2, theEndPnt1  )
  };

  // Consider case when edges have shared vertex
  if ( isConsiderEndPointTouch )
  {
    if ( aPointHash[0] < 0 || aPointHash[1] < 0 )
      return BRepMesh_WireInterferenceChecker::EndPointTouch;
  }

  Standard_Integer aPosHash = 
    aPointHash[0] + aPointHash[1] + aPointHash[2] + aPointHash[3];

  /*=========================================*/
  /*  1) hash code == 1:

                    0+
                    /
           0      1/         0
           +======+==========+
  
      2) hash code == 2:

           0    1        1   0
        a) +----+========+---+

           0       1   1     0
        b) +-------+===+=====+

                                             */
  /*=========================================*/
  if ( aPosHash == 1 )
  {
    if (isConsiderPointOnSegment)
    {
      if (aPointHash[0] == 1)
        theIntPnt = theStartPnt1;
      else if (aPointHash[1] == 1)
        theIntPnt = theEndPnt1;
      else if (aPointHash[2] == 1)
        theIntPnt = theStartPnt2;
      else
        theIntPnt = theEndPnt2;

      return BRepMesh_WireInterferenceChecker::PointOnSegment;
    }

    return BRepMesh_WireInterferenceChecker::NoIntersection;
  }
  else if ( aPosHash == 2 )
    return BRepMesh_WireInterferenceChecker::Glued;

  gp_XY aVec1           = theEndPnt1   - theStartPnt1;
  gp_XY aVec2           = theEndPnt2   - theStartPnt2;
  gp_XY aVecStartPoints = theStartPnt2 - theStartPnt1;
    
  Standard_Real aCrossD1D2 = aVec1           ^ aVec2;
  Standard_Real aCrossD1D3 = aVecStartPoints ^ aVec2;

  const Standard_Real aPrec = Precision::PConfusion();
  // Are edgegs codirectional
  if ( Abs( aCrossD1D2 ) < aPrec )
  {
    // Just a parallel case?
    if( Abs( aCrossD1D3 ) < aPrec )
    {
      /*=========================================*/
      /*  Here the following cases are possible:
          1) hash code == -4:

               -1                -1
                +=================+
               -1                -1

          2) hash code == -2:

                0       -1        0
                +--------+========+
                        -1

          3) hash code == -1:

                0        1        -1
                +--------+========+
                                  -1

          4) hash code == 0:

                0      0  0       0
                +------+  +=======+
                0      0  0       0
                                                 */
      /*=========================================*/

      if ( aPosHash < -2 )
        return BRepMesh_WireInterferenceChecker::Same;
      else if ( aPosHash == -1 )
        return BRepMesh_WireInterferenceChecker::Glued;

      return BRepMesh_WireInterferenceChecker::NoIntersection;
    }
    else
      return BRepMesh_WireInterferenceChecker::NoIntersection;
  }

  Standard_Real aPar = aCrossD1D3 / aCrossD1D2;
  const Standard_Real aEndPrec = 1 - aPrec;

  // Intersection is out of first segment range
  if( aPar < aPrec || aPar > aEndPrec )
    return BRepMesh_WireInterferenceChecker::NoIntersection;
 
  Standard_Real aCrossD2D3 = aVecStartPoints.Reversed() ^ aVec1;
  aPar = aCrossD2D3 / -aCrossD1D2;

  // Intersection is out of second segment range
  if( aPar < aPrec || aPar > aEndPrec )
    return BRepMesh_WireInterferenceChecker::NoIntersection;
 
  theIntPnt = theStartPnt2 + aPar * aVec2;
  return BRepMesh_WireInterferenceChecker::Cross;
}

//=============================================================================
//function : classifyPoint
//purpose  : 
//=============================================================================
Standard_Integer BRepMesh_WireInterferenceChecker::classifyPoint(
  const gp_XY& thePoint1,
  const gp_XY& thePoint2,
  const gp_XY& thePointToCheck)
{
  gp_XY aP1 = thePoint2       - thePoint1;
  gp_XY aP2 = thePointToCheck - thePoint1;
  
  const Standard_Real aPrec   = Precision::PConfusion();
  const Standard_Real aSqPrec = aPrec * aPrec;
  Standard_Real aDist = Abs(aP1 ^ aP2);
  if (aDist > aPrec)
  {
    aDist = (aDist * aDist) / aP1.SquareModulus();
    if (aDist > aSqPrec)
      return 0; //out
  }
    
  gp_XY aMult = aP1.Multiplied(aP2);
  if ( aMult.X() < 0.0 || aMult.Y() < 0.0 )
    return 0; //out
    
  if (aP1.SquareModulus() < aP2.SquareModulus())
    return 0; //out
    
  if (thePointToCheck.IsEqual(thePoint1, aPrec) || 
      thePointToCheck.IsEqual(thePoint2, aPrec))
  {
    return -1; //coinsides with an end point
  }
    
  return 1;
}
