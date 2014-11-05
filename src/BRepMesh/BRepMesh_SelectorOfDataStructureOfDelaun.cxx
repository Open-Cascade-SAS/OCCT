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

//=======================================================================
//function : Default constructor
//purpose  : 
//=======================================================================
BRepMesh_SelectorOfDataStructureOfDelaun::BRepMesh_SelectorOfDataStructureOfDelaun()
: myAllocator(new NCollection_IncAllocator(BRepMesh::MEMORY_BLOCK_SIZE_HUGE)),
  myNodes   (10, myAllocator),
  myLinks   (10, myAllocator),
  myElements(10, myAllocator),
  myFrontier(10, myAllocator)
{
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepMesh_SelectorOfDataStructureOfDelaun::BRepMesh_SelectorOfDataStructureOfDelaun(
  const Handle(BRepMesh_DataStructureOfDelaun)& theMesh)
: myAllocator(new NCollection_IncAllocator(BRepMesh::MEMORY_BLOCK_SIZE_HUGE)),
  myMesh    (theMesh),
  myNodes   (10, myAllocator),
  myLinks   (10, myAllocator),
  myElements(10, myAllocator),
  myFrontier(10, myAllocator)
{
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================
void BRepMesh_SelectorOfDataStructureOfDelaun::Initialize(
  const Handle(BRepMesh_DataStructureOfDelaun)& theMesh)
{
  myMesh = theMesh;
  myNodes.Clear();
  myLinks.Clear();
  myElements.Clear();
  myFrontier.Clear();
}

//=======================================================================
//function : NeighboursOf(Node)
//purpose  : 
//=======================================================================
void BRepMesh_SelectorOfDataStructureOfDelaun::NeighboursOf(
  const BRepMesh_Vertex& theNode)
{
  NeighboursOfNode(myMesh->IndexOf(theNode));
}

//=======================================================================
//function : NeighboursOfNode(NodeIndex)
//purpose  : 
//=======================================================================
void BRepMesh_SelectorOfDataStructureOfDelaun::NeighboursOfNode(
  const Standard_Integer theNodeIndex)
{
  BRepMesh::ListOfInteger::Iterator aLinkIt(
    myMesh->LinksConnectedTo(theNodeIndex));

  for (; aLinkIt.More(); aLinkIt.Next())
    elementsOfLink(aLinkIt.Value());
}

//=======================================================================
//function : NeighboursOf(Link)
//purpose  : 
//=======================================================================
void BRepMesh_SelectorOfDataStructureOfDelaun::NeighboursOf(
  const BRepMesh_Edge& theLink)
{
  NeighboursOfNode(theLink.FirstNode());
  NeighboursOfNode(theLink.LastNode());
}

//=======================================================================
//function : NeighboursOfLink(LinkIndex)
//purpose  : 
//=======================================================================
void BRepMesh_SelectorOfDataStructureOfDelaun::NeighboursOfLink(
  const Standard_Integer theLinkIndex)
{
  NeighboursOf(myMesh->GetLink(theLinkIndex));
}

//=======================================================================
//function : NeighboursOf(Element)
//purpose  :
//=======================================================================
void BRepMesh_SelectorOfDataStructureOfDelaun::NeighboursOf(
  const BRepMesh_Triangle& theElement)
{
  Standard_Integer v[3];
  myMesh->ElementNodes(theElement, v);

  for (Standard_Integer i = 0; i < 3; ++i)
    NeighboursOfNode(v[i]);
}

//=======================================================================
//function : NeighboursOfElement(ElementIndex)
//purpose  :
//=======================================================================
void BRepMesh_SelectorOfDataStructureOfDelaun::NeighboursOfElement(
  const Standard_Integer theElementIndex)
{
  NeighboursOf(myMesh->GetElement(theElementIndex));
}

//=======================================================================
//function : NeighboursByEdgeOf(Element)
//purpose  : 
//=======================================================================
void BRepMesh_SelectorOfDataStructureOfDelaun::NeighboursByEdgeOf(
  const BRepMesh_Triangle& theElement)
{
  Standard_Integer e[3];
  Standard_Boolean o[3];
  theElement.Edges(e, o);

  for (Standard_Integer i = 0; i < 3; ++i)
    elementsOfLink(e[i]);
}

//=======================================================================
//function : elementsOfLink
//purpose  : 
//=======================================================================
void BRepMesh_SelectorOfDataStructureOfDelaun::elementsOfLink(
  const Standard_Integer theIndex)
{
  const BRepMesh_PairOfIndex& aPair = myMesh->ElementsConnectedTo(theIndex);
  for(Standard_Integer j = 1, jn = aPair.Extent(); j <= jn; ++j)
    myElements.Add(aPair.Index(j));
}
