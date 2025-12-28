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

//: o4 abv 17.02.99: r0301_db.stp #53082: treatment of open wires implemented
// pdn 11.03.99 S4135 changing reordering algorithm in order to make it independent on tolerance
// szv#4 S4163
//    pdn 09.05.99: S4174: preserve order of edges for complete torus

#include <gp_Pnt.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_WireOrder.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Standard_Transient.hxx>

//=================================================================================================

ShapeAnalysis_WireOrder::ShapeAnalysis_WireOrder()
    : myGap(0.0),
      myStat(0),
      myKeepLoops(false),
      myMode(Mode3D)
{
  myTol = Precision::Confusion();
  Clear();
}

//=================================================================================================

ShapeAnalysis_WireOrder::ShapeAnalysis_WireOrder(const bool   theMode3D,
                                                 const double theTolerance,
                                                 const bool   theModeBoth)
    : myTol(theTolerance),
      myGap(0.0),
      myStat(0),
      myKeepLoops(false)
{
  if (theModeBoth)
  {
    myMode = ModeBoth;
  }
  else
  {
    if (theMode3D)
    {
      myMode = Mode3D;
    }
    else
    {
      myMode = Mode2D;
    }
  }
  Clear();
}

//=================================================================================================

void ShapeAnalysis_WireOrder::SetMode(const bool   theMode3D,
                                      const double theTolerance,
                                      const bool   theModeBoth)
{
  ModeType aNewMode;

  if (theModeBoth)
  {
    aNewMode = ModeBoth;
  }
  else
  {
    if (theMode3D)
    {
      aNewMode = Mode3D;
    }
    else
    {
      aNewMode = Mode2D;
    }
  }
  if (myMode != aNewMode)
  {
    Clear();
  }
  myMode = aNewMode;
  myOrd.Nullify();
  myStat = 0;
  myGap  = 0.0;
  myTol  = (theTolerance > 0.0) ? theTolerance : 1.e-08;
}

//=================================================================================================

double ShapeAnalysis_WireOrder::Tolerance() const
{
  return myTol;
}

//=================================================================================================

void ShapeAnalysis_WireOrder::Clear()
{
  myXYZ  = new NCollection_HSequence<gp_XYZ>();
  myXY   = new NCollection_HSequence<gp_XY>();
  myStat = 0;
  myGap  = 0.0;
}

//=================================================================================================

void ShapeAnalysis_WireOrder::Add(const gp_XYZ& theStart3d, const gp_XYZ& theEnd3d)
{
  if (myMode == Mode3D)
  {
    myXYZ->Append(theStart3d);
    myXYZ->Append(theEnd3d);
  }
}

//=================================================================================================

void ShapeAnalysis_WireOrder::Add(const gp_XY& theStart2d, const gp_XY& theEnd2d)
{
  if (myMode == Mode2D)
  {
    gp_XYZ val;
    val.SetCoord(theStart2d.X(), theStart2d.Y(), 0.0);
    myXYZ->Append(val);
    val.SetCoord(theEnd2d.X(), theEnd2d.Y(), 0.0);
    myXYZ->Append(val);
  }
}

//=================================================================================================

void ShapeAnalysis_WireOrder::Add(const gp_XYZ& theStart3d,
                                  const gp_XYZ& theEnd3d,
                                  const gp_XY&  theStart2d,
                                  const gp_XY&  theEnd2d)
{
  if (myMode == ModeBoth)
  {
    myXYZ->Append(theStart3d);
    myXYZ->Append(theEnd3d);

    myXY->Append(theStart2d);
    myXY->Append(theEnd2d);
  }
}

//=================================================================================================

int ShapeAnalysis_WireOrder::NbEdges() const
{
  return myXYZ->Length() / 2;
}

static double DISTABS(const gp_XYZ& v1, const gp_XYZ& v2)
{
  return std::abs(v1.X() - v2.X()) + std::abs(v1.Y() - v2.Y()) + std::abs(v1.Z() - v2.Z());
}

//  La routine qui suit gere les boucles internes a un wire. Questce a dire ?
//  Un wire normalement chaine (meme pas dans l ordre et avec des inverses)
//   balaie toutes ses edges au moins une fois dans une seule liste
//  En 3D il peut y avoir des COUTURES ... une, mais evt plusieurs ...
//  En ce cas le critere fin-debut peut definir des sous-parties fermees du
//  wire, ce sont les boucles en question
//  Exemple (cylindre gentil) : la couture (balayee deux fois) : 1 boucle
//   chaque limite (haute et basse) definit aussi une boucle (1 edge ou +)

//  En cours de chainage, il faut donc :
//  1/ sauter la boucle, pour ne pas la rebalayer 36 fois : NextFree y pourvoit
//    en notant les tetes de boucles, on n a pas le droit de les revoir
//   NB: ca marche car en cours de constitution de liste, on s interdit de
//   repasser plus d une fois sur chaque edge (test fol-pre non nul)
//  2/ reprendre les boucles pour les fusionner : pas encore fait
//   (pour l instant, on imprime un petit message, c est tout)

//=================================================================================================

bool& ShapeAnalysis_WireOrder::KeepLoopsMode()
{
  return myKeepLoops;
}

//=======================================================================
// function : Perform
// purpose  : Make wire order analysis and propose the better order of the edges
//           taking into account the gaps between edges.
//=======================================================================

void ShapeAnalysis_WireOrder::Perform(const bool /*closed*/)
{
  myStat       = 0;
  int aNbEdges = NbEdges();
  // no edges loaded, nothing to do -- return with status OK
  if (aNbEdges == 0)
  {
    return;
  }
  myOrd = new NCollection_HArray1<int>(1, aNbEdges);
  myOrd->Init(0);

  // sequence of the edge nums in the right order
  occ::handle<NCollection_HSequence<int>> anEdgeSeq = new NCollection_HSequence<int>;
  NCollection_Sequence<occ::handle<NCollection_HSequence<int>>> aLoops;

  // the beginnings and ends of the edges
  NCollection_Array1<gp_XYZ> aBegins3D(1, aNbEdges);
  NCollection_Array1<gp_XYZ> anEnds3D(1, aNbEdges);
  NCollection_Array1<gp_XY>  aBegins2D(1, aNbEdges);
  NCollection_Array1<gp_XY>  anEnds2D(1, aNbEdges);
  for (int i = 1; i <= aNbEdges; i++)
  {
    aBegins3D(i) = myXYZ->Value(2 * i - 1);
    anEnds3D(i)  = myXYZ->Value(2 * i);
    if (myMode == ModeBoth)
    {
      aBegins2D(i) = myXY->Value(2 * i - 1);
      anEnds2D(i)  = myXY->Value(2 * i);
    }
  }
  // the flags that the edges was considered
  NCollection_Array1<bool> isEdgeUsed(1, aNbEdges);
  isEdgeUsed.Init(false);

  constexpr double aTol2  = Precision::SquareConfusion();
  constexpr double aTolP2 = Precision::SquarePConfusion();

  // take the first edge to the constructed chain
  isEdgeUsed(1)        = true;
  gp_Pnt   aFirstPnt3D = aBegins3D(1);
  gp_Pnt   aLastPnt3D  = anEnds3D(1);
  gp_Pnt2d aFirstPnt2D;
  gp_Pnt2d aLastPnt2D;
  if (myMode == ModeBoth)
  {
    aFirstPnt2D = aBegins2D(1);
    aLastPnt2D  = anEnds2D(1);
  }
  anEdgeSeq->Append(1);

  // cycle until all edges are considered
  for (;;)
  {
    // joint type
    // 0 - the start of the best edge to the end of constructed sequence (nothing to do)
    // 1 - the end of the best edge to the start of constructed sequence (need move the edge)
    // 2 - the end of the best edge to the end of constructed sequence (need to reverse)
    // 3 - the start of the best edge to the start of constructed sequence (need to reverse and move
    // the edge)
    int aBestJointType = 3;
    // the best minimum distance between constructed sequence and the best edge
    double aBestMin3D = RealLast();
    // number of the best edge
    int aBestEdgeNum = 0;
    // the best edge was found
    bool isFound     = false;
    bool isConnected = false;
    // loop to find the best edge among all the remaining
    for (int i = 1; i <= aNbEdges; i++)
    {
      if (isEdgeUsed(i))
      {
        continue;
      }

      // find minimum distance and joint type for 3D and 2D (if necessary) modes
      int    aCurJointType;
      double aCurMin;
      // distance for four possible cases
      double aSeqTailEdgeHead = aLastPnt3D.SquareDistance(aBegins3D(i));
      double aSeqTailEdgeTail = aLastPnt3D.SquareDistance(anEnds3D(i));
      double aSeqHeadEdgeTail = aFirstPnt3D.SquareDistance(anEnds3D(i));
      double aSeqHeadEdgeHead = aFirstPnt3D.SquareDistance(aBegins3D(i));
      // the best distances for joints with head and tail of sequence
      double aMinDistToTail, aMinDistToHead;
      int    aTailJoinType, aHeadJointType;
      if (aSeqTailEdgeHead <= aSeqTailEdgeTail)
      {
        aTailJoinType  = 0;
        aMinDistToTail = aSeqTailEdgeHead;
      }
      else
      {
        aTailJoinType  = 2;
        aMinDistToTail = aSeqTailEdgeTail;
      }
      if (aSeqHeadEdgeTail <= aSeqHeadEdgeHead)
      {
        aHeadJointType = 1;
        aMinDistToHead = aSeqHeadEdgeTail;
      }
      else
      {
        aHeadJointType = 3;
        aMinDistToHead = aSeqHeadEdgeHead;
      }
      // comparing the head and the tail cases
      // if distances are close enough then we use rule for joint type: 0 < 1 < 2 < 3
      if (fabs(aMinDistToTail - aMinDistToHead) < aTol2)
      {
        if (aTailJoinType < aHeadJointType)
        {
          aCurJointType = aTailJoinType;
          aCurMin       = aMinDistToTail;
        }
        else
        {
          aCurJointType = aHeadJointType;
          aCurMin       = aMinDistToHead;
        }
      }
      else
      {
        if (aMinDistToTail <= aMinDistToHead)
        {
          aCurJointType = aTailJoinType;
          aCurMin       = aMinDistToTail;
        }
        else
        {
          aCurJointType = aHeadJointType;
          aCurMin       = aMinDistToHead;
        }
      }
      // update for the best values
      if (myMode == ModeBoth)
      {
        // distances in 2D
        int aJointMask3D = 0, aJointMask2D = 0;
        if (aSeqTailEdgeHead < aTol2)
        {
          aJointMask3D |= (1 << 0);
        }
        if (aSeqTailEdgeTail < aTol2)
        {
          aJointMask3D |= (1 << 2);
        }
        if (aSeqHeadEdgeTail < aTol2)
        {
          aJointMask3D |= (1 << 1);
        }
        if (aSeqHeadEdgeHead < aTol2)
        {
          aJointMask3D |= (1 << 3);
        }
        double aSeqTailEdgeHead2D = aLastPnt2D.SquareDistance(aBegins2D(i));
        double aSeqTailEdgeTail2D = aLastPnt2D.SquareDistance(anEnds2D(i));
        double aSeqHeadEdgeTail2D = aFirstPnt2D.SquareDistance(anEnds2D(i));
        double aSeqHeadEdgeHead2D = aFirstPnt2D.SquareDistance(aBegins2D(i));
        if (aSeqTailEdgeHead2D < aTolP2)
        {
          aJointMask2D |= (1 << 0);
        }
        if (aSeqTailEdgeTail2D < aTolP2)
        {
          aJointMask2D |= (1 << 2);
        }
        if (aSeqHeadEdgeTail2D < aTolP2)
        {
          aJointMask2D |= (1 << 1);
        }
        if (aSeqHeadEdgeHead2D < aTolP2)
        {
          aJointMask2D |= (1 << 3);
        }
        // new approch for detecting best edge connection, for all other cases used old 3D
        // algorithm
        int aFullMask = aJointMask3D & aJointMask2D;
        if (aFullMask != 0)
        {
          // find the best current joint type
          aCurJointType = 3;
          for (int j = 0; j < 4; j++)
          {
            if (aFullMask & (1 << j))
            {
              aCurJointType = j;
              break;
            }
          }
          if (!isConnected || aCurJointType < aBestJointType)
          {
            isFound     = true;
            isConnected = true;
            switch (aCurJointType)
            {
              case 0:
                aBestMin3D = aSeqTailEdgeHead;
                break;
              case 1:
                aBestMin3D = aSeqHeadEdgeTail;
                break;
              case 2:
                aBestMin3D = aSeqTailEdgeTail;
                break;
              case 3:
                aBestMin3D = aSeqHeadEdgeHead;
                break;
            }
            aBestJointType = aCurJointType;
            aBestEdgeNum   = i;
          }
        }
        // if there is still no connection, continue to use ald 3D algorithm
        if (isConnected)
        {
          continue;
        }
      }
      // if the best distance is still not reached (aBestMin3D > aTol2) or we found a better joint
      // type
      if (aBestMin3D > aTol2 || aCurJointType < aBestJointType)
      {
        // make a decision that this edge is good enough:
        // - it gets the best distance but there is fabs(aCurMin3d - aBestMin3d) < aTol2 &&
        // (aCurJointType < aBestJointType) ?
        // - it gets the best joint in some cases
        if (aCurMin < aBestMin3D
            || ((aCurMin == aBestMin3D || aCurMin < aTol2) && (aCurJointType < aBestJointType)))
        {
          isFound        = true;
          aBestMin3D     = aCurMin;
          aBestJointType = aCurJointType;
          aBestEdgeNum   = i;
        }
      }
    }

    // check that we found edge for connecting
    if (isFound)
    {
      // distance between first and last point in sequence
      double aCloseDist = aFirstPnt3D.SquareDistance(aLastPnt3D);
      // if it's better to insert the edge than to close the loop, just insert the edge according to
      // joint type
      if (aBestMin3D <= RealSmall() || aBestMin3D < aCloseDist)
      {
        switch (aBestJointType)
        {
          case 0:
            anEdgeSeq->Append(aBestEdgeNum);
            aLastPnt3D = anEnds3D(aBestEdgeNum);
            break;
          case 1:
            anEdgeSeq->Prepend(aBestEdgeNum);
            aFirstPnt3D = aBegins3D(aBestEdgeNum);
            break;
          case 2:
            anEdgeSeq->Append(-aBestEdgeNum);
            aLastPnt3D = aBegins3D(aBestEdgeNum);
            break;
          case 3:
            anEdgeSeq->Prepend(-aBestEdgeNum);
            aFirstPnt3D = anEnds3D(aBestEdgeNum);
            break;
        }
        if (myMode == ModeBoth)
        {
          switch (aBestJointType)
          {
            case 0:
              aLastPnt2D = anEnds2D(aBestEdgeNum);
              break;
            case 1:
              aFirstPnt2D = aBegins2D(aBestEdgeNum);
              break;
            case 2:
              aLastPnt2D = aBegins2D(aBestEdgeNum);
              break;
            case 3:
              aFirstPnt2D = anEnds2D(aBestEdgeNum);
              break;
          }
        }
      }
      // closing loop and creating new one
      else
      {
        aLoops.Append(anEdgeSeq);
        anEdgeSeq   = new NCollection_HSequence<int>;
        aFirstPnt3D = aBegins3D(aBestEdgeNum);
        aLastPnt3D  = anEnds3D(aBestEdgeNum);
        if (myMode == ModeBoth)
        {
          aFirstPnt2D = aBegins2D(aBestEdgeNum);
          aLastPnt2D  = anEnds2D(aBestEdgeNum);
        }
        anEdgeSeq->Append(aBestEdgeNum);
      }
      // mark the edge as used
      isEdgeUsed(aBestEdgeNum) = true;
    }
    else
    {
      // the only condition under which we can't find an edge is when all edges are done
      break;
    }
  }
  // append the last loop
  aLoops.Append(anEdgeSeq);

  // handling with constructed loops
  occ::handle<NCollection_HSequence<int>> aMainLoop;
  if (myKeepLoops)
  {
    // keeping the loops, adding one after another.
    aMainLoop = new NCollection_HSequence<int>;
    for (int i = 1; i <= aLoops.Length(); i++)
    {
      const occ::handle<NCollection_HSequence<int>>& aCurLoop = aLoops(i);
      aMainLoop->Append(aCurLoop);
    }
  }
  else
  {
    // connecting loops
    aMainLoop = aLoops.First();
    aLoops.Remove(1);
    while (aLoops.Length())
    {
      // iterate over all loops to find the closest one
      double aMinDist1    = RealLast();
      int    aLoopNum1    = 0;
      int    aCurLoopIt1  = 0;
      bool   aDirect1     = false;
      int    aMainLoopIt1 = 0;
      for (int aLoopIt = 1; aLoopIt <= aLoops.Length(); aLoopIt++)
      {
        const occ::handle<NCollection_HSequence<int>>& aCurLoop = aLoops.Value(aLoopIt);
        // iterate over all gaps between edges in current loop
        int    aCurLoopIt2    = 0;
        int    aMainLoopIt2   = 0;
        bool   aDirect2       = false;
        double aMinDist2      = RealLast();
        int    aCurLoopLength = aCurLoop->Length();
        for (int aCurEdgeIt = 1; aCurEdgeIt <= aCurLoopLength; aCurEdgeIt++)
        {
          // get the distance between the current edge and the previous edge taking into account the
          // edge's orientation
          int    aPrevEdgeIt   = aCurEdgeIt == 1 ? aCurLoopLength : aCurEdgeIt - 1;
          int    aCurEdgeIdx   = aCurLoop->Value(aCurEdgeIt);
          int    aPrevEdgeIdx  = aCurLoop->Value(aPrevEdgeIt);
          gp_Pnt aCurLoopFirst = aCurEdgeIdx > 0 ? aBegins3D(aCurEdgeIdx) : anEnds3D(-aCurEdgeIdx);
          gp_Pnt aCurLoopLast =
            aPrevEdgeIdx > 0 ? anEnds3D(aPrevEdgeIdx) : aBegins3D(-aPrevEdgeIdx);
          // iterate over all gaps between edges in main loop
          double aMinDist3       = RealLast();
          int    aMainLoopIt3    = 0;
          bool   aDirect3        = false;
          int    aMainLoopLength = aMainLoop->Length();
          for (int aCurEdgeIt2 = 1; (aCurEdgeIt2 <= aMainLoopLength) && aMinDist3 != 0.0;
               aCurEdgeIt2++)
          {
            // get the distance between the current edge and the next edge taking into account the
            // edge's orientation
            int    aNextEdgeIt2  = aCurEdgeIt2 == aMainLoopLength ? 1 : aCurEdgeIt2 + 1;
            int    aCurEdgeIdx2  = aMainLoop->Value(aCurEdgeIt2);
            int    aNextEdgeIdx2 = aMainLoop->Value(aNextEdgeIt2);
            gp_Pnt aMainLoopFirst =
              (aCurEdgeIdx2 > 0 ? anEnds3D(aCurEdgeIdx2) : aBegins3D(-aCurEdgeIdx2));
            gp_Pnt aMainLoopLast =
              (aNextEdgeIdx2 > 0 ? aBegins3D(aNextEdgeIdx2) : anEnds3D(-aNextEdgeIdx2));
            // getting the sum of square distances if we try to sew the current loop with the main
            // loop in current positions
            double aDirectDist = aCurLoopFirst.SquareDistance(aMainLoopFirst)
                                 + aCurLoopLast.SquareDistance(aMainLoopLast);
            double aReverseDist = aCurLoopFirst.SquareDistance(aMainLoopLast)
                                  + aCurLoopLast.SquareDistance(aMainLoopFirst);
            // take the best result
            double aJoinDist;
            if ((aDirectDist < aTol2) || (aDirectDist < 2.0 * aReverseDist))
            {
              aJoinDist    = aDirectDist;
              aReverseDist = aDirectDist;
            }
            else
            {
              aJoinDist = aReverseDist;
            }
            // check if we found a better distance
            if (aJoinDist < aMinDist3 && std::abs(aMinDist3 - aJoinDist) > aTol2)
            {
              aMinDist3    = aJoinDist;
              aDirect3     = (aDirectDist <= aReverseDist);
              aMainLoopIt3 = aCurEdgeIt2;
            }
          }
          // check if we found a better distance
          if (aMinDist3 < aMinDist2 && std::abs(aMinDist2 - aMinDist3) > aTol2)
          {
            aMinDist2    = aMinDist3;
            aDirect2     = aDirect3;
            aMainLoopIt2 = aMainLoopIt3;
            aCurLoopIt2  = aCurEdgeIt;
          }
        }
        // check if we found a better distance
        if (aMinDist2 < aMinDist1 && std::abs(aMinDist1 - aMinDist2) > aTol2)
        {
          aMinDist1    = aMinDist2;
          aLoopNum1    = aLoopIt;
          aDirect1     = aDirect2;
          aMainLoopIt1 = aMainLoopIt2;
          aCurLoopIt1  = aCurLoopIt2;
        }
      }
      // insert the found loop into main loop
      occ::handle<NCollection_HSequence<int>> aLoop   = aLoops.Value(aLoopNum1);
      int                                     aFactor = (aDirect1 ? 1 : -1);
      for (int i = 0; i < aLoop->Length(); i++)
      {
        int anIdx =
          (aCurLoopIt1 + i > aLoop->Length() ? aCurLoopIt1 + i - aLoop->Length() : aCurLoopIt1 + i);
        aMainLoop->InsertAfter(aMainLoopIt1 + i, aLoop->Value(anIdx) * aFactor);
      }
      aLoops.Remove(aLoopNum1);
    }
  }

  // checking the new order of the edges
  //  0 - order is the same
  //  1 - some edges were reordered
  // -1 - some edges were reversed
  int aTempStatus = 0;
  for (int i = 1; i <= aMainLoop->Length(); i++)
  {
    if (i != aMainLoop->Value(i) && aTempStatus >= 0)
    {
      aTempStatus = (aMainLoop->Value(i) > 0 ? 1 : -1);
    }
    myOrd->SetValue(i, aMainLoop->Value(i));
  }
  if (aTempStatus == 0)
  {
    myStat = aTempStatus;
    return;
  }
  else
  {
    // check if edges were only shifted in reverse or forward, not reordered
    bool isShiftReverse = true;
    bool isShiftForward = true;
    int  aFirstIdx, aSecondIdx;
    int  aLength = aMainLoop->Length();
    for (int i = 1; i <= aLength - 1; i++)
    {
      aFirstIdx  = aMainLoop->Value(i);
      aSecondIdx = aMainLoop->Value(i + 1);
      if (!(aSecondIdx - aFirstIdx == 1 || (aFirstIdx == aLength && aSecondIdx == 1)))
      {
        isShiftForward = false;
      }
      if (!(aFirstIdx - aSecondIdx == 1 || (aSecondIdx == aLength && aFirstIdx == 1)))
      {
        isShiftReverse = false;
      }
    }
    aFirstIdx  = aMainLoop->Value(aLength);
    aSecondIdx = aMainLoop->Value(1);
    if (!(aSecondIdx - aFirstIdx == 1 || (aFirstIdx == aLength && aSecondIdx == 1)))
    {
      isShiftForward = false;
    }
    if (!(aFirstIdx - aSecondIdx == 1 || (aSecondIdx == aLength && aFirstIdx == 1)))
    {
      isShiftReverse = false;
    }
    if (isShiftForward || isShiftReverse)
    {
      aTempStatus = 3;
    }
    myStat = aTempStatus;
    return;
  }
}

//=================================================================================================

bool ShapeAnalysis_WireOrder::IsDone() const
{
  return !myOrd.IsNull();
}

//=================================================================================================

int ShapeAnalysis_WireOrder::Status() const
{
  return myStat;
}

//=================================================================================================

int ShapeAnalysis_WireOrder::Ordered(const int theIdx) const
{
  if (myOrd.IsNull() || myOrd->Upper() < theIdx)
    return theIdx;
  int anOldIdx = myOrd->Value(theIdx);
  return (anOldIdx == 0 ? theIdx : anOldIdx);
}

//=================================================================================================

void ShapeAnalysis_WireOrder::XYZ(const int theIdx, gp_XYZ& theStart3D, gp_XYZ& theEnd3D) const
{
  theStart3D = myXYZ->Value((theIdx > 0 ? 2 * theIdx - 1 : -2 * theIdx));
  theEnd3D   = myXYZ->Value((theIdx > 0 ? 2 * theIdx : -2 * theIdx - 1));
}

//=================================================================================================

void ShapeAnalysis_WireOrder::XY(const int theIdx, gp_XY& theStart2D, gp_XY& theEnd2D) const
{
  if (myMode == ModeBoth)
  {
    theStart2D = myXY->Value((theIdx > 0 ? 2 * theIdx - 1 : -2 * theIdx));
    theEnd2D   = myXY->Value((theIdx > 0 ? 2 * theIdx : -2 * theIdx - 1));
  }
  else
  {
    const gp_XYZ& aStart3d = myXYZ->Value((theIdx > 0 ? 2 * theIdx - 1 : -2 * theIdx));
    theStart2D.SetCoord(aStart3d.X(), aStart3d.Y());
    const gp_XYZ& anEnd3d = myXYZ->Value((theIdx > 0 ? 2 * theIdx : -2 * theIdx - 1));
    theEnd2D.SetCoord(anEnd3d.X(), anEnd3d.Y());
  }
}

//=================================================================================================

double ShapeAnalysis_WireOrder::Gap(const int num) const
{
  if (num == 0)
    return myGap;
  int n1 = Ordered(num);
  int n0 = Ordered(num == 1 ? NbEdges() : num - 1);
  //  Distance entre fin (n0) et debut (n1)
  return DISTABS(myXYZ->Value((n0 > 0 ? 2 * n0 : -2 * n0 - 1)),
                 myXYZ->Value((n1 > 0 ? 2 * n1 - 1 : -2 * n1)));
  ////  return (myXYZ->Value(2*n0)).Distance (myXYZ->Value(2*n1-1));
}

//=================================================================================================

void ShapeAnalysis_WireOrder::SetChains(const double gap)
{
  int n0, n1, n2, nb = NbEdges(); // szv#4:S4163:12Mar99 o0,o1,o2 not needed
  if (nb == 0)
    return;
  NCollection_Sequence<int> chain;
  n0 = 0;
  chain.Append(1);             // On demarre la partie
  gp_XYZ f3d, l3d, f13d, l13d; // szv#4:S4163:12Mar99 f03d,l03d unused
  for (n1 = 1; n1 <= nb; n1++)
  {
    if (n0 == 0)
    { // nouvelle boucle
      n0 = n1;
      // szv#4:S4163:12Mar99 optimized
      XYZ(Ordered(n0), f13d, l13d);
    }
    // szv#4:S4163:12Mar99 optimized
    n2 = (n1 == nb) ? n0 : (n1 + 1);
    XYZ(Ordered(n2), f3d, l3d);
    if (!f3d.IsEqual(l13d, gap))
    {
      chain.Append(n2);
      n0 = 0;
    }
    f13d = f3d;
    l13d = l3d;
  }
  nb = chain.Length();
  if (nb == 0)
    return;
  myChains = new NCollection_HArray1<int>(1, nb);
  for (n1 = 1; n1 <= nb; n1++)
    myChains->SetValue(n1, chain.Value(n1));
}

//=================================================================================================

int ShapeAnalysis_WireOrder::NbChains() const
{
  return (myChains.IsNull() ? 0 : myChains->Length());
}

//=================================================================================================

void ShapeAnalysis_WireOrder::Chain(const int num, int& n1, int& n2) const
{
  n1 = n2 = 0;
  if (myChains.IsNull())
    return;
  int nb = myChains->Upper();
  if (num == 0 || num > nb)
    return;
  n1 = myChains->Value(num);
  if (num == nb)
    n2 = NbEdges();
  else
    n2 = myChains->Value(num + 1) - 1;
}

//=================================================================================================

void ShapeAnalysis_WireOrder::SetCouples(const double /*gap*/)
{
#ifdef OCCT_DEBUG
  std::cout << "ShapeAnalysis_WireOrder:SetCouple not yet implemented" << std::endl;
#endif
}

//=================================================================================================

int ShapeAnalysis_WireOrder::NbCouples() const
{
  return (myCouples.IsNull() ? 0 : myCouples->Length());
}

//=================================================================================================

void ShapeAnalysis_WireOrder::Couple(const int num, int& n1, int& n2) const
{
  n1 = n2 = 0;
  if (myCouples.IsNull())
    return;
  int nb = myCouples->Upper();
  if (num == 0 || num * 2 > nb)
    return;
  n1 = myCouples->Value(2 * num - 1);
  n2 = myCouples->Value(2 * num);
}
