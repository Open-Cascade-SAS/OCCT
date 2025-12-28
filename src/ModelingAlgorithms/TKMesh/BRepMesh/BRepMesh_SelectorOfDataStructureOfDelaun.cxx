// Created on: 1993-06-01
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

#include <BRepMesh_SelectorOfDataStructureOfDelaun.hxx>
#include <BRepMesh_PairOfIndex.hxx>
#include <BRepMesh_Edge.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_SelectorOfDataStructureOfDelaun, Standard_Transient)

//=================================================================================================

BRepMesh_SelectorOfDataStructureOfDelaun::BRepMesh_SelectorOfDataStructureOfDelaun() = default;

//=================================================================================================

BRepMesh_SelectorOfDataStructureOfDelaun::BRepMesh_SelectorOfDataStructureOfDelaun(
  const occ::handle<BRepMesh_DataStructureOfDelaun>& theMesh)
    : myMesh(theMesh)
{
}

//=================================================================================================

void BRepMesh_SelectorOfDataStructureOfDelaun::Initialize(
  const occ::handle<BRepMesh_DataStructureOfDelaun>& theMesh)
{
  myMesh = theMesh;
  myNodes.Clear();
  myLinks.Clear();
  myElements.Clear();
  myFrontier.Clear();
}

//=================================================================================================

void BRepMesh_SelectorOfDataStructureOfDelaun::NeighboursOf(const BRepMesh_Vertex& theNode)
{
  NeighboursOfNode(myMesh->IndexOf(theNode));
}

//=================================================================================================

void BRepMesh_SelectorOfDataStructureOfDelaun::NeighboursOfNode(const int theNodeIndex)
{
  IMeshData::ListOfInteger::Iterator aLinkIt(myMesh->LinksConnectedTo(theNodeIndex));

  for (; aLinkIt.More(); aLinkIt.Next())
    elementsOfLink(aLinkIt.Value());
}

//=================================================================================================

void BRepMesh_SelectorOfDataStructureOfDelaun::NeighboursOf(const BRepMesh_Edge& theLink)
{
  NeighboursOfNode(theLink.FirstNode());
  NeighboursOfNode(theLink.LastNode());
}

//=================================================================================================

void BRepMesh_SelectorOfDataStructureOfDelaun::NeighboursOfLink(const int theLinkIndex)
{
  NeighboursOf(myMesh->GetLink(theLinkIndex));
}

//=================================================================================================

void BRepMesh_SelectorOfDataStructureOfDelaun::NeighboursOf(const BRepMesh_Triangle& theElement)
{
  int v[3];
  myMesh->ElementNodes(theElement, v);

  for (int i = 0; i < 3; ++i)
    NeighboursOfNode(v[i]);
}

//=================================================================================================

void BRepMesh_SelectorOfDataStructureOfDelaun::NeighboursOfElement(const int theElementIndex)
{
  NeighboursOf(myMesh->GetElement(theElementIndex));
}

//=================================================================================================

void BRepMesh_SelectorOfDataStructureOfDelaun::NeighboursByEdgeOf(
  const BRepMesh_Triangle& theElement)
{
  const int (&e)[3] = theElement.myEdges;
  for (int i = 0; i < 3; ++i)
    elementsOfLink(e[i]);
}

//=================================================================================================

void BRepMesh_SelectorOfDataStructureOfDelaun::elementsOfLink(const int theIndex)
{
  const BRepMesh_PairOfIndex& aPair = myMesh->ElementsConnectedTo(theIndex);
  for (int j = 1, jn = aPair.Extent(); j <= jn; ++j)
    myElements.Add(aPair.Index(j));
}
