// Created on: 2009-10-22
// Created by: Mikhail SAZONOV
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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

#include <Poly_MakeLoops.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_DataMap.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>

#ifdef OCCT_DEBUG
static Standard_Integer doDebug = 0;
#endif

//=======================================================================
//function : Poly_MakeLoops
//purpose  : 
//=======================================================================

Poly_MakeLoops::Poly_MakeLoops(const Helper* theHelper,
                                     const Handle(NCollection_BaseAllocator)& theAlloc)
: myHelper (theHelper),
  myAlloc (theAlloc),
  myMapLink (4000, myAlloc),
  myLoops (myAlloc),
  myStartIndices (4000)
{
}

//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void Poly_MakeLoops::Reset
                   (const Helper* theHelper,
                    const Handle(NCollection_BaseAllocator)& theAlloc)
{
  myHelper = theHelper;
  myMapLink.Clear();
  myLoops.Clear(theAlloc);
  myStartIndices.Clear();
  myAlloc = theAlloc;
}

//=======================================================================
//function : AddLink
//purpose  : 
//=======================================================================

void Poly_MakeLoops::AddLink(const Link& theLink)
{
  if (theLink.node1 == theLink.node2)
    return;
  Standard_Integer aInd = myMapLink.Add(theLink);
  Link& aLink = const_cast<Link&>(myMapLink(aInd));
  aLink.flags |= theLink.flags;
#ifdef OCCT_DEBUG
  myHelper->OnAddLink (aInd, aLink);
#endif
}

//=======================================================================
//function : ReplaceLink
//purpose  : 
//=======================================================================

void Poly_MakeLoops::ReplaceLink(const Link& theLink, const Link& theNewLink)
{
  if (theNewLink.node1 == theNewLink.node2)
    return;
  Standard_Integer aInd = myMapLink.Add(theLink);
  if (aInd > 0)
  {
    Link aLink;
    // replace with a null link first (workaround exception)
    myMapLink.Substitute(aInd, aLink);
    aLink = theNewLink;
    // and now put there the final value of link
    myMapLink.Substitute(aInd, aLink);
#ifdef OCCT_DEBUG
    myHelper->OnAddLink (aInd, aLink);
#endif
  }
}

//=======================================================================
//function : SetLinkOrientation
//purpose  : 
//=======================================================================

Poly_MakeLoops::LinkFlag Poly_MakeLoops::SetLinkOrientation
                   (const Link& theLink,
                    const LinkFlag theOrient)
{
  Standard_Integer aInd = myMapLink.FindIndex(theLink);
  LinkFlag aOri = LF_None;
  if (aInd > 0)
  {
    Link& aLink = const_cast<Link&>(myMapLink(aInd));
    aOri = (LinkFlag) (aLink.flags & LF_Both);
    aLink.flags = theOrient;
#ifdef OCCT_DEBUG
    myHelper->OnAddLink (aInd, aLink);
#endif
  }
  return aOri;
}

//=======================================================================
//function : FindLink
//purpose  : 
//=======================================================================

Poly_MakeLoops::Link Poly_MakeLoops::FindLink(const Link& theLink) const
{
  Standard_Integer aInd = myMapLink.FindIndex(theLink);
  Poly_MakeLoops::Link aLink;
  if (aInd > 0)
    aLink = myMapLink(aInd);
  return aLink;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

Standard_Integer Poly_MakeLoops::Perform()
{
  // prepare the set of start indices
  myStartIndices.Clear();
  Standard_Integer i;
  for (i = 1; i <= myMapLink.Extent(); i++)
  {
    const Link& aLink = myMapLink(i);
    if (aLink.flags & LF_Fwd)
      myStartIndices.Add(i);
    if (aLink.flags & LF_Rev)
      myStartIndices.Add(-i);
  }

#ifdef OCCT_DEBUG
  if (doDebug)
    showBoundaryBreaks();
#endif

  Standard_Integer aResult = 0;

  Handle(NCollection_IncAllocator) aTempAlloc = new NCollection_IncAllocator(4000);
  Handle(NCollection_IncAllocator) aTempAlloc1 = new NCollection_IncAllocator(4000);

  // two pass loop
  Standard_Integer aPassNum, nbLoopsOnPass2 = 0;
  for (aPassNum=0; aPassNum < 2; aPassNum++)
  {
    myHangIndices.Clear();
    // main loop
    while (!myStartIndices.IsEmpty())
    {
      Standard_Integer aIndexS = myStartIndices.Top();

      aTempAlloc->Reset();
      NCollection_IndexedMap<Standard_Integer> aContour (100, aTempAlloc);
      Standard_Integer aStartNumber = findContour (aIndexS, aContour, aTempAlloc, aTempAlloc1);
#ifdef OCCT_DEBUG
      if (aStartNumber > 1)
        if (doDebug)
        {
          cout << "--- found contour with hanging links:" << endl;
          for (i = 1; i <= aContour.Extent(); i++)
            cout << " " << aContour(i);
          cout << endl;
        }
#endif
      if (aStartNumber == 0)
      {      // error
        aResult |= RC_Failure;
        return aResult;
      }
      if (aStartNumber <= aContour.Extent())
      {
        // there is a closed loop in the contour
        if (aPassNum == 1)
          nbLoopsOnPass2++;
        acceptContour (aContour, aStartNumber);
      }
      if (aStartNumber > 1)
      {
        // it is required to mark hanging edges
        Standard_Integer aNode;
        if (aStartNumber <= aContour.Extent())
          // mark hanging edges starting from the first one till a bifurcation
          aNode = getFirstNode(aIndexS);
        else
        {
          // open contour - mark from the end back till a bifurcation
          aIndexS = aContour(aStartNumber - 1);
          aNode = getLastNode(aIndexS);
        }
        markHangChain(aNode, aIndexS);
      }
    }

    if (aPassNum == 0)
    {
      // move hanging links to start indices to make the second pass
      TColStd_MapIteratorOfPackedMapOfInteger it(myHangIndices);
      for (; it.More(); it.Next())
        myStartIndices.Add(it.Key());
    }
  }
#ifdef OCCT_DEBUG
  if (doDebug && nbLoopsOnPass2)
    cout << "MakeLoops: " << nbLoopsOnPass2
      << " contours accepted on the second pass" << endl;
#endif

  if (!myLoops.IsEmpty())
    aResult |= RC_LoopsDone;
  if (!myHangIndices.IsEmpty())
    aResult |= RC_HangingLinks;
  return aResult;
}

//=======================================================================
//function : findContour
//purpose  : Collects edges in chain until they form a closed contour.
// Returns the index in the map theContour where the loop starts.
// It may return the number greater than the extent of the map by 1,
// what means that the contour is open
//=======================================================================

Standard_Integer Poly_MakeLoops::findContour
                   (Standard_Integer theIndexS,
                    NCollection_IndexedMap<Standard_Integer> &theContour,
                    const Handle(NCollection_BaseAllocator)& theTempAlloc,
                    const Handle(NCollection_IncAllocator)& theTempAlloc1) const
{
  theContour.Clear();
  Standard_Integer aStartIndex = 0;
  Standard_Integer aIndexS = theIndexS;
  NCollection_DataMap<Standard_Integer,Standard_Integer> aNodeLink(100, theTempAlloc);
  Standard_Integer aLastNode  = getLastNode (aIndexS);

  for (;;) {
    theContour.Add(aIndexS);
    aNodeLink.Bind(getFirstNode(aIndexS), aIndexS);

    Standard_Integer aIndex = Abs (aIndexS);

    // collect the list of links from this node able to participate
    // in this contour
    theTempAlloc1->Reset();
    NCollection_List<Standard_Integer> aLstIndS (theTempAlloc1);
    const ListOfLink& aLinks = myHelper->GetAdjacentLinks (aLastNode);
    Poly_MakeLoops::ListOfLink::Iterator itLinks (aLinks);
    for (; itLinks.More(); itLinks.Next()) {
      Standard_Integer aInd = myMapLink.FindIndex(itLinks.Value());
      if (aInd == 0 || aInd == aIndex)
        continue;
      // determine the orientation in which the link is to be taken
      Standard_Integer aIndS = aInd;
      Standard_Integer aNode1 = getFirstNode(aInd);
      if (aNode1 != aLastNode)
        aIndS = -aIndS;

      if (canLinkBeTaken(aIndS))
        aLstIndS.Append(aIndS);
    }

    if (aLstIndS.IsEmpty()) {
      // no more ways: open contour
      aStartIndex = theContour.Extent() + 1;
      break;
    }

    Standard_Integer aIndexSNext = 0;
    if (aLstIndS.First() == aLstIndS.Last())
      // only one possible way
      aIndexSNext = aLstIndS.First();
    else
      // find the most left way
      aIndexSNext = chooseLeftWay (aLastNode, aIndexS, aLstIndS);

    aIndexS = aIndexSNext;

    if (aIndexS == 0)
    {
      // no more ways: open contour
      aStartIndex = theContour.Extent() + 1;
      break;
    }
    if (theContour.Contains(aIndexS))
    {
      // entering the loop second time, stop search
      aStartIndex = theContour.FindIndex (aIndexS);
      break;
    }
    if (theContour.Contains (-aIndexS))
    {
      // leaving the loop, stop search
      aStartIndex = theContour.FindIndex (-aIndexS) + 1;
      break;
    }

    aLastNode  = getLastNode (aIndexS);

    if (aNodeLink.IsBound(aLastNode))
    {
      // closing the loop, stop search
      theContour.Add(aIndexS);
      aStartIndex = theContour.FindIndex(aNodeLink.Find(aLastNode));
      break;
    }
  }

  return aStartIndex;
}

//=======================================================================
//function : acceptContour
//purpose  : Builds a wire from a given set of edge indices (starting with
// theStartNumber) and appends it to the result list. 
// Also updates the start indices.
//=======================================================================

void Poly_MakeLoops::acceptContour
                   (const NCollection_IndexedMap<Standard_Integer>& theContour,
                    Standard_Integer theStartNumber)
{
  // append a new loop to the result
  Loop anEmptyLoop(myAlloc);
  myLoops.Append(anEmptyLoop);
  Loop& aLoop = myLoops.ChangeValue(myLoops.Length());

  // build a loop, mark links as taken,
  // remove them from the set of start indices
  Standard_Integer i;
  for (i = theStartNumber; i <= theContour.Extent(); i++)
  {
    Standard_Integer aIndexS = theContour(i);   // index with sign
    Standard_Integer aIndex = Abs (aIndexS);
    const Link& aLink = myMapLink(aIndex);
    Link aOrientedLink = aLink;
    if (aIndexS < 0)
      aOrientedLink.Reverse();
    aLoop.Append(aOrientedLink);
    // remove from start set
    myStartIndices.Remove(aIndexS);
  }
}

//=======================================================================
//function : getFirstNode
//purpose  : Returns the first node of the given link
// taking into account its orientation (the sign of index)
//=======================================================================

Standard_Integer Poly_MakeLoops::getFirstNode(Standard_Integer theIndexS) const
{
  Standard_Integer aIndex = Abs(theIndexS);
  const Link& aLink = myMapLink(aIndex);
  if (theIndexS > 0)
    return aLink.node1;
  return aLink.node2;
}

//=======================================================================
//function : getLastNode
//purpose  : Returns the last node of the given link
// taking into account its orientation (the sign of index)
//=======================================================================

Standard_Integer Poly_MakeLoops::getLastNode(int theIndexS) const
{
  Standard_Integer aIndex = Abs(theIndexS);
  const Link& aLink = myMapLink(aIndex);
  if (theIndexS > 0)
    return aLink.node2;
  return aLink.node1;
}

//=======================================================================
//function : markHangChain
//purpose  : Marks hanging links starting from the given node.
// Also removes such links from the start indices.
//=======================================================================

void Poly_MakeLoops::markHangChain(Standard_Integer theNode, Standard_Integer theIndexS)
{
  Standard_Integer aNode1 = theNode;
  Standard_Integer aIndexS = theIndexS;
  Standard_Integer aIndex = Abs(aIndexS);
  Standard_Boolean isOut = (aNode1 == getFirstNode(aIndexS));
  for (;;)
  {
    // check if the current link is hanging:
    // if it is outcoming from aNode1 then count the number of
    // other incoming links and vise versa;
    // if the number is zero than it is hanging
    const ListOfLink& aLinks = myHelper->GetAdjacentLinks (aNode1);
    Standard_Integer nEdges = 0;
    Poly_MakeLoops::ListOfLink::Iterator itLinks (aLinks);
    for (; itLinks.More() && nEdges == 0; itLinks.Next())
    {
      const Link &aL = itLinks.Value();
      Standard_Integer aInd = myMapLink.FindIndex(aL);
      if (aInd == 0 || aInd == aIndex)
        continue;
      if ((isOut && aNode1 == aL.node1) ||
          (!isOut && aNode1 == aL.node2))
        aInd = -aInd;
      if (canLinkBeTaken(aInd))
        nEdges++;
    }
    if (nEdges > 0)
      // leave this chain
      break;

    // mark the current link as hanging
    myStartIndices.Remove(aIndexS);
    myHangIndices.Add(aIndexS);

    // get other node of the link and the next link
    if (isOut)
      aNode1 = getLastNode(aIndexS);
    else
      aNode1 = getFirstNode(aIndexS);
    const ListOfLink& aNextLinks = myHelper->GetAdjacentLinks (aNode1);
    Standard_Integer aNextIndexS = 0;
    for (itLinks.Init(aNextLinks); itLinks.More(); itLinks.Next())
    {
      const Link &aL = itLinks.Value();
      Standard_Integer aInd = myMapLink.FindIndex(aL);
      if (aInd == 0 || aInd == aIndex)
        continue;
      if ((isOut && aNode1 == aL.node2) ||
          (!isOut && aNode1 == aL.node1))
        aInd = -aInd;
      if (canLinkBeTaken(aInd))
      {
        if (aNextIndexS == 0)
          aNextIndexS = aInd;
        else
        {
          // more than 1 ways, stop the chain
          aNextIndexS = 0;
          break;
        }
      }
    }
    if (aNextIndexS == 0)
      break;
    aIndexS = aNextIndexS;
    aIndex = Abs(aIndexS);
  }
}

//=======================================================================
//function : canLinkBeTaken
//purpose  : Returns True if the link appointed by the index can participate
// in a loop in given orientation (it is the sign of index).
// Remark: A boundary edge can be taken only once
//=======================================================================

Standard_Boolean Poly_MakeLoops::canLinkBeTaken(Standard_Integer theIndexS) const
{
  return myStartIndices.Contains(theIndexS);
}

//=======================================================================
//function : showBoundaryBreaks
//purpose  : 
//=======================================================================

#ifdef OCCT_DEBUG
void Poly_MakeLoops::showBoundaryBreaks() const
{
  // collect nodes of boundary links
  TColStd_PackedMapOfInteger aNodesMap;
  Standard_Integer i;
  for (i = 1; i <= myMapLink.Extent(); i++)
  {
    const Link& aLink = myMapLink(i);
    Standard_Integer aFlags = aLink.flags & LF_Both;
    if (aFlags && aFlags != LF_Both)
    {
      // take only oriented links
      aNodesMap.Add(aLink.node1);
      aNodesMap.Add(aLink.node2);
    }
  }

  // check each node if the number of input and output links are equal
  Standard_Boolean isFirst = Standard_True;
  TColStd_MapIteratorOfPackedMapOfInteger it(aNodesMap);
  for (; it.More(); it.Next())
  {
    Standard_Integer aNode = it.Key();
    Standard_Integer nb = 0;
    const ListOfLink& aLinks = myHelper->GetAdjacentLinks(aNode);
    Poly_MakeLoops::ListOfLink::Iterator itLinks (aLinks);
    for (; itLinks.More(); itLinks.Next())
    {
      const Poly_MakeLoops::Link& aLink = itLinks.Value();
      if (myMapLink.FindIndex(aLink) == 0)
        continue;
      Standard_Integer aFlags = aLink.flags & LF_Both;
      if (aFlags && aFlags != LF_Both)
      {
        if (aNode == aLink.node1) // output?
        {
          if (aFlags & LF_Fwd)
            nb++;  // yes, output
          else
            nb--;  // reversed, so input
        }
        else if (aNode == aLink.node2) // input?
        {
          if (aFlags & LF_Fwd)
            nb--;  // yes, input
          else
            nb++;  // reversed, so output
        }
        else
          // inconsistent
          nb += 100;
      }
    }
    if (nb != 0)
    {
      // indicate this node
      if (isFirst)
      {
        isFirst = Standard_False;
        cout << "boundary breaks are found in the following nodes:" << endl;
      }
      cout << aNode << " ";
    }
  }
  if (!isFirst)
    cout << endl;
}
#endif

//=======================================================================
//function : GetHangingLinks
//purpose  : 
//=======================================================================

void Poly_MakeLoops::GetHangingLinks(ListOfLink& theLinks) const
{
  TColStd_MapIteratorOfPackedMapOfInteger it(myHangIndices);
  for (; it.More(); it.Next())
  {
    Standard_Integer aIndexS = it.Key();
    Link aLink = myMapLink(Abs(aIndexS));
    if (aIndexS < 0)
      aLink.Reverse();
    theLinks.Append(aLink);
  }
}

//=======================================================================
//function : Poly_MakeLoops3D
//purpose  : 
//=======================================================================

Poly_MakeLoops3D::Poly_MakeLoops3D(const Helper* theHelper,
                                         const Handle(NCollection_BaseAllocator)& theAlloc)
: Poly_MakeLoops (theHelper, theAlloc)
{
}

//=======================================================================
//function : Poly_MakeLoops3D::chooseLeftWay
//purpose  : 
//=======================================================================

Standard_Integer Poly_MakeLoops3D::chooseLeftWay
                   (const Standard_Integer theNode,
                    const Standard_Integer theSegIndex,
                    const NCollection_List<Standard_Integer>& theLstIndS) const
{
  Standard_Real aAngleMin = M_PI * 2;
  gp_Dir aNormal;
  const Helper* aHelper = getHelper();
  if (!aHelper->GetNormal (theNode, aNormal))
    return theLstIndS.First();

  Link aLink = getLink(theSegIndex);
  gp_Dir aTgtRef;
  if (!aHelper->GetLastTangent (aLink, aTgtRef))
    return theLstIndS.First();

  // project tangent vector to the plane orthogonal to normal
  // to get the reference direction
  gp_XYZ aTgtRefXYZ = aNormal.XYZ().CrossCrossed (aTgtRef.XYZ(), aNormal.XYZ());
  if (aTgtRefXYZ.SquareModulus() < 1e-14)
    // a problem with defining reference direction, take first way
    return theLstIndS.First();
  aTgtRef = aTgtRefXYZ;

  // find the way with minimal angle to the reference direction
  // (the angle is in range ]-PI;PI])
  Standard_Integer aResIndex = 0;
  NCollection_List<Standard_Integer>::Iterator aItI (theLstIndS);
  for (; aItI.More(); aItI.Next())
  {
    Standard_Integer aIndS = aItI.Value();

    aLink = getLink(aIndS);
    gp_Dir aTgt;
    if (!aHelper->GetFirstTangent (aLink, aTgt))
      continue;

    gp_XYZ aTgtXYZ = aNormal.XYZ().CrossCrossed (aTgt.XYZ(), aNormal.XYZ());
    if (aTgtXYZ.SquareModulus() < 1e-14)
      // skip a problem way
      continue;
    aTgt = aTgtXYZ;

    Standard_Real aAngle = aTgt.AngleWithRef(aTgtRef, aNormal);
    if (aAngle < 1e-4 - M_PI)
      aAngle = M_PI;
    if (aAngle < aAngleMin)
    {
      aAngleMin = aAngle;
      aResIndex = aIndS;
    }
  }
  return aResIndex == 0 ? theLstIndS.First() : aResIndex;
}

//=======================================================================
//function : Poly_MakeLoops2D
//purpose  : 
//=======================================================================

Poly_MakeLoops2D::Poly_MakeLoops2D(const Standard_Boolean theLeftWay,
                                         const Helper* theHelper,
                                         const Handle(NCollection_BaseAllocator)& theAlloc)
: Poly_MakeLoops (theHelper, theAlloc),
  myRightWay(!theLeftWay)
{
}

//=======================================================================
//function : Poly_MakeLoops2D::chooseLeftWay
//purpose  : 
//=======================================================================

Standard_Integer Poly_MakeLoops2D::chooseLeftWay
                   (const Standard_Integer /*theNode*/,
                    const Standard_Integer theSegIndex,
                    const NCollection_List<Standard_Integer>& theLstIndS) const
{
  Standard_Real aAngleMin = M_PI * 2;
  const Helper* aHelper = getHelper();
  Link aLink = getLink(theSegIndex);
  gp_Dir2d aTgtRef;
  if (!aHelper->GetLastTangent (aLink, aTgtRef))
    // a problem with defining reference direction, take first way
    return theLstIndS.First();

  // find the way with minimal angle to the reference direction
  // (the angle is in range ]-PI;PI])
  Standard_Integer aResIndex = 0;
  NCollection_List<Standard_Integer>::Iterator aItI (theLstIndS);
  for (; aItI.More(); aItI.Next())
  {
    Standard_Integer aIndS = aItI.Value();

    aLink = getLink(aIndS);
    gp_Dir2d aTgt;
    if (!aHelper->GetFirstTangent (aLink, aTgt))
      // skip a problem way
      continue;

    Standard_Real aAngle = aTgt.Angle(aTgtRef);
    if (myRightWay)
      aAngle = -aAngle;
    if (aAngle < 1e-4 - M_PI)
      aAngle = M_PI;
    if (aAngle < aAngleMin)
    {
      aAngleMin = aAngle;
      aResIndex = aIndS;
    }
  }
  return aResIndex == 0 ? theLstIndS.First() : aResIndex;
}
