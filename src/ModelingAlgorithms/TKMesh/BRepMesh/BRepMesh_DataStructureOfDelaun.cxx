// Created on: 1993-05-11
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

#include <BRepMesh_DataStructureOfDelaun.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepMesh_Edge.hxx>

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <Standard_ErrorHandler.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_DataStructureOfDelaun, Standard_Transient)

//=================================================================================================

BRepMesh_DataStructureOfDelaun::BRepMesh_DataStructureOfDelaun(
  const occ::handle<NCollection_IncAllocator>& theAllocator,
  const int                                    theReservedNodeSize)
    : myAllocator(theAllocator),
      myNodes(new BRepMesh_VertexTool(myAllocator)),
      myNodeLinks(theReservedNodeSize * 3, myAllocator),
      myLinks(theReservedNodeSize * 3, myAllocator),
      myDelLinks(myAllocator),
      myElements(theReservedNodeSize * 2, myAllocator)
{
}

//=================================================================================================

int BRepMesh_DataStructureOfDelaun::AddNode(const BRepMesh_Vertex& theNode, const bool isForceAdd)
{
  const int aNodeId = myNodes->Add(theNode, isForceAdd);
  if (!myNodeLinks.IsBound(aNodeId))
    myNodeLinks.Bind(aNodeId, IMeshData::ListOfInteger(myAllocator));

  return aNodeId;
}

//=================================================================================================

bool BRepMesh_DataStructureOfDelaun::SubstituteNode(const int              theIndex,
                                                    const BRepMesh_Vertex& theNewNode)
{
  if (myNodes->FindIndex(theNewNode) != 0)
    return false;

  myNodes->Substitute(theIndex, theNewNode);
  return true;
}

//=================================================================================================

int BRepMesh_DataStructureOfDelaun::AddLink(const BRepMesh_Edge& theLink)
{
  int aLinkIndex = IndexOf(theLink);
  if (aLinkIndex > 0)
  {
    return theLink.IsSameOrientation(GetLink(aLinkIndex)) ? aLinkIndex : -aLinkIndex;
  }

  BRepMesh_PairOfIndex aPair;
  if (!myDelLinks.IsEmpty())
  {
    aLinkIndex = myDelLinks.First();
    myLinks.Substitute(aLinkIndex, theLink, aPair);
    myDelLinks.RemoveFirst();
  }
  else
    aLinkIndex = myLinks.Add(theLink, aPair);

  const int aLinkId = std::abs(aLinkIndex);
  linksConnectedTo(theLink.FirstNode()).Append(aLinkId);
  linksConnectedTo(theLink.LastNode()).Append(aLinkId);
  myLinksOfDomain.Add(aLinkIndex);

  return aLinkIndex;
}

//=================================================================================================

bool BRepMesh_DataStructureOfDelaun::SubstituteLink(const int            theIndex,
                                                    const BRepMesh_Edge& theNewLink)
{
  BRepMesh_PairOfIndex aPair;
  BRepMesh_Edge        aLink = GetLink(theIndex);
  if (aLink.Movability() == BRepMesh_Deleted)
  {
    myLinks.Substitute(theIndex, theNewLink, aPair);
    return true;
  }

  if (IndexOf(theNewLink) != 0)
    return false;

  aLink.SetMovability(BRepMesh_Deleted);
  myLinks.Substitute(theIndex, aLink, aPair);
  cleanLink(theIndex, aLink);

  const int aLinkId = std::abs(theIndex);
  linksConnectedTo(theNewLink.FirstNode()).Append(aLinkId);
  linksConnectedTo(theNewLink.LastNode()).Append(aLinkId);
  myLinks.Substitute(theIndex, theNewLink, aPair);

  return true;
}

//=================================================================================================

void BRepMesh_DataStructureOfDelaun::RemoveLink(const int theIndex, const bool isForce)
{
  BRepMesh_Edge& aLink = (BRepMesh_Edge&)GetLink(theIndex);
  if (aLink.Movability() == BRepMesh_Deleted || (!isForce && aLink.Movability() != BRepMesh_Free)
      || ElementsConnectedTo(theIndex).Extent() != 0)
  {
    return;
  }

  cleanLink(theIndex, aLink);
  aLink.SetMovability(BRepMesh_Deleted);

  myLinksOfDomain.Remove(theIndex);
  myDelLinks.Append(theIndex);
}

//=================================================================================================

void BRepMesh_DataStructureOfDelaun::cleanLink(const int theIndex, const BRepMesh_Edge& theLink)
{
  for (int i = 0; i < 2; ++i)
  {
    const int aNodeId = (i == 0) ? theLink.FirstNode() : theLink.LastNode();

    IMeshData::ListOfInteger&          aLinkList = linksConnectedTo(aNodeId);
    IMeshData::ListOfInteger::Iterator aLinkIt(aLinkList);
    for (; aLinkIt.More(); aLinkIt.Next())
    {
      if (aLinkIt.Value() == theIndex)
      {
        aLinkList.Remove(aLinkIt);
        break;
      }
    }
  }
}

//=================================================================================================

int BRepMesh_DataStructureOfDelaun::AddElement(const BRepMesh_Triangle& theElement)
{
  myElements.Append(theElement);
  int aElementIndex = myElements.Size();
  myElementsOfDomain.Add(aElementIndex);

  const int (&e)[3] = theElement.myEdges;
  for (int i = 0; i < 3; ++i)
    myLinks(e[i]).Append(aElementIndex);

  return aElementIndex;
}

//=================================================================================================

void BRepMesh_DataStructureOfDelaun::RemoveElement(const int theIndex)
{
  BRepMesh_Triangle& aElement = (BRepMesh_Triangle&)GetElement(theIndex);
  if (aElement.Movability() == BRepMesh_Deleted)
    return;

  cleanElement(theIndex, aElement);
  aElement.SetMovability(BRepMesh_Deleted);
  myElementsOfDomain.Remove(theIndex);
}

//=================================================================================================

void BRepMesh_DataStructureOfDelaun::cleanElement(const int                theIndex,
                                                  const BRepMesh_Triangle& theElement)
{
  if (theElement.Movability() != BRepMesh_Free)
    return;

  const int (&e)[3] = theElement.myEdges;
  for (int i = 0; i < 3; ++i)
    removeElementIndex(theIndex, myLinks(e[i]));
}

//=================================================================================================

void BRepMesh_DataStructureOfDelaun::removeElementIndex(const int             theIndex,
                                                        BRepMesh_PairOfIndex& thePair)
{
  for (int i = 1, n = thePair.Extent(); i <= n; ++i)
  {
    if (thePair.Index(i) == theIndex)
    {
      thePair.RemoveIndex(i);
      return;
    }
  }
}

//=================================================================================================

bool BRepMesh_DataStructureOfDelaun::SubstituteElement(const int                theIndex,
                                                       const BRepMesh_Triangle& theNewElement)
{
  const BRepMesh_Triangle& aElement = GetElement(theIndex);
  if (aElement.Movability() == BRepMesh_Deleted)
  {
    myElements(theIndex) = theNewElement;
    return true;
  }

  cleanElement(theIndex, aElement);
  // Warning: here new element and old element should have different Hash code
  myElements(theIndex) = theNewElement;

  const int (&e)[3] = theNewElement.myEdges;
  for (int i = 0; i < 3; ++i)
    myLinks(e[i]).Append(theIndex);

  return true;
}

//=================================================================================================

void BRepMesh_DataStructureOfDelaun::ElementNodes(const BRepMesh_Triangle& theElement,
                                                  int (&theNodes)[3])
{
  const int (&e)[3]  = theElement.myEdges;
  const bool (&o)[3] = theElement.myOrientations;

  const BRepMesh_Edge& aLink1 = GetLink(e[0]);
  if (o[0])
  {
    theNodes[0] = aLink1.FirstNode();
    theNodes[1] = aLink1.LastNode();
  }
  else
  {
    theNodes[1] = aLink1.FirstNode();
    theNodes[0] = aLink1.LastNode();
  }

  const BRepMesh_Edge& aLink2 = GetLink(e[2]);
  if (o[2])
    theNodes[2] = aLink2.FirstNode();
  else
    theNodes[2] = aLink2.LastNode();
}

//=================================================================================================

void BRepMesh_DataStructureOfDelaun::ClearDomain()
{
  IMeshData::MapOfInteger           aFreeEdges;
  IMeshData::IteratorOfMapOfInteger aElementIt(myElementsOfDomain);
  for (; aElementIt.More(); aElementIt.Next())
  {
    const int          aElementId = aElementIt.Key();
    BRepMesh_Triangle& aElement   = (BRepMesh_Triangle&)GetElement(aElementId);

    const int (&e)[3] = aElement.myEdges;

    for (int i = 0; i < 3; ++i)
      aFreeEdges.Add(e[i]);

    cleanElement(aElementId, aElement);
    aElement.SetMovability(BRepMesh_Deleted);
  }
  myElementsOfDomain.Clear();

  IMeshData::IteratorOfMapOfInteger aEdgeIt(aFreeEdges);
  for (; aEdgeIt.More(); aEdgeIt.Next())
    RemoveLink(aEdgeIt.Key());
}

//=================================================================================================

void BRepMesh_DataStructureOfDelaun::clearDeletedLinks()
{
  int aLastLiveItem = NbLinks();
  while (!myDelLinks.IsEmpty())
  {
    while (aLastLiveItem > 0)
    {
      if (GetLink(aLastLiveItem).Movability() != BRepMesh_Deleted)
        break;

      myLinks.RemoveLast();
      --aLastLiveItem;
    }

    int aDelItem = myDelLinks.First();
    myDelLinks.RemoveFirst();

    if (aDelItem > aLastLiveItem)
      continue;

    BRepMesh_Edge         aLink = GetLink(aLastLiveItem);
    BRepMesh_PairOfIndex& aPair = myLinks(aLastLiveItem);

    myLinks.RemoveLast();
    myLinks.Substitute(aDelItem, aLink, aPair);

    myLinksOfDomain.Remove(aLastLiveItem);
    myLinksOfDomain.Add(aDelItem);
    --aLastLiveItem;

    const int                          aLastLiveItemId = aLastLiveItem + 1;
    IMeshData::ListOfInteger::Iterator aLinkIt;
    // update link references
    for (int i = 0; i < 2; ++i)
    {
      const int aCurNodeId = (i == 0) ? aLink.FirstNode() : aLink.LastNode();

      for (aLinkIt.Init(linksConnectedTo(aCurNodeId)); aLinkIt.More(); aLinkIt.Next())
      {
        int& aLinkId = aLinkIt.ChangeValue();
        if (aLinkId == aLastLiveItemId)
        {
          aLinkId = aDelItem;
          break;
        }
      }
    }

    // update elements references
    for (int j = 1, jn = aPair.Extent(); j <= jn; ++j)
    {
      int                      e[3];
      bool                     o[3];
      const BRepMesh_Triangle& aElement = GetElement(aPair.Index(j));
      aElement.Edges(e, o);
      for (int i = 0; i < 3; ++i)
      {
        if (e[i] == aLastLiveItemId)
        {
          e[i] = aDelItem;
          break;
        }
      }

      myElements(aLinkIt.Value()) = BRepMesh_Triangle(e, o, aElement.Movability());
    }
  }
}

//=================================================================================================

void BRepMesh_DataStructureOfDelaun::clearDeletedNodes()
{
  IMeshData::ListOfInteger& aDelNodes = (IMeshData::ListOfInteger&)myNodes->GetListOfDelNodes();

  int aLastLiveItem = NbNodes();
  while (!aDelNodes.IsEmpty())
  {
    while (aLastLiveItem > 0)
    {
      if (GetNode(aLastLiveItem).Movability() != BRepMesh_Deleted)
        break;

      myNodes->RemoveLast();
      --aLastLiveItem;
    }

    int aDelItem = aDelNodes.First();
    aDelNodes.RemoveFirst();

    if (aDelItem > aLastLiveItem)
      continue;

    BRepMesh_Vertex           aNode     = GetNode(aLastLiveItem);
    IMeshData::ListOfInteger& aLinkList = linksConnectedTo(aLastLiveItem);

    myNodes->RemoveLast();
    --aLastLiveItem;

    myNodes->Substitute(aDelItem, aNode);
    myNodeLinks.ChangeFind(aDelItem) = aLinkList;

    const int                          aLastLiveItemId = aLastLiveItem + 1;
    IMeshData::ListOfInteger::Iterator aLinkIt(aLinkList);
    for (; aLinkIt.More(); aLinkIt.Next())
    {
      const int             aLinkId = aLinkIt.Value();
      const BRepMesh_Edge&  aLink   = GetLink(aLinkId);
      BRepMesh_PairOfIndex& aPair   = myLinks(aLinkId);

      int v[2] = {aLink.FirstNode(), aLink.LastNode()};
      if (v[0] == aLastLiveItemId)
        v[0] = aDelItem;
      else if (v[1] == aLastLiveItemId)
        v[1] = aDelItem;

      myLinks.Substitute(aLinkId, BRepMesh_Edge(v[0], v[1], aLink.Movability()), aPair);
    }
  }
}

//=================================================================================================

void BRepMesh_DataStructureOfDelaun::Statistics(Standard_OStream& theStream) const
{
  theStream << " Map of nodes : \n";
  myNodes->Statistics(theStream);
  theStream << "\n Deleted nodes : " << myNodes->GetListOfDelNodes().Extent() << std::endl;

  theStream << "\n\n Map of Links : \n";
  myLinks.Statistics(theStream);
  theStream << "\n Deleted links : " << myDelLinks.Extent() << std::endl;

  theStream << "\n\n Map of elements : \n";
  theStream << "\n Elements : " << myElements.Size() << std::endl;
}

//=======================================================================
// function : BRepMesh_Write
// purpose  :
//  Global function not declared in any public header, intended for use
//  from debugger prompt (Command Window in Visual Studio).
//
//  Stores the mesh data structure to BRep file with the given name.
//=======================================================================
const char* BRepMesh_Dump(void* theMeshHandlePtr, const char* theFileNameStr)
{
  if (theMeshHandlePtr == nullptr || theFileNameStr == nullptr)
  {
    return "Error: file name or mesh data is null";
  }

  occ::handle<BRepMesh_DataStructureOfDelaun> aMeshData =
    *(occ::handle<BRepMesh_DataStructureOfDelaun>*)theMeshHandlePtr;

  if (aMeshData.IsNull())
    return "Error: mesh data is empty";

  TopoDS_Compound aMesh;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aMesh);

  try
  {
    OCC_CATCH_SIGNALS

    if (aMeshData->LinksOfDomain().IsEmpty())
    {
      const int aNodesNb = aMeshData->NbNodes();
      for (int i = 1; i <= aNodesNb; ++i)
      {
        const gp_XY& aNode = aMeshData->GetNode(i).Coord();
        gp_Pnt       aPnt(aNode.X(), aNode.Y(), 0.);
        aBuilder.Add(aMesh, BRepBuilderAPI_MakeVertex(aPnt));
      }
    }
    else
    {
      IMeshData::IteratorOfMapOfInteger aLinksIt(aMeshData->LinksOfDomain());
      for (; aLinksIt.More(); aLinksIt.Next())
      {
        const BRepMesh_Edge& aLink = aMeshData->GetLink(aLinksIt.Key());
        gp_Pnt               aPnt[2];
        for (int i = 0; i < 2; ++i)
        {
          const int aNodeId = (i == 0) ? aLink.FirstNode() : aLink.LastNode();

          const gp_XY& aNode = aMeshData->GetNode(aNodeId).Coord();
          aPnt[i]            = gp_Pnt(aNode.X(), aNode.Y(), 0.);
        }

        if (aPnt[0].SquareDistance(aPnt[1]) < Precision::SquareConfusion())
          continue;

        aBuilder.Add(aMesh, BRepBuilderAPI_MakeEdge(aPnt[0], aPnt[1]));
      }
    }

    if (!BRepTools::Write(aMesh, theFileNameStr))
      return "Error: write failed";
  }
  catch (Standard_Failure const& anException)
  {
    return anException.GetMessageString();
  }

  return theFileNameStr;
}

void BRepMesh_DataStructureOfDelaun::Dump(const char* theFileNameStr)
{
  occ::handle<BRepMesh_DataStructureOfDelaun> aMeshData(this);
  BRepMesh_Dump((void*)&aMeshData, theFileNameStr);
}
