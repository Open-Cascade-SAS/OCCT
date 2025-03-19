// Created on: 2016-08-22
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#include <BRepMesh_MeshTool.hxx>
#include <BRepMesh_SelectorOfDataStructureOfDelaun.hxx>
#include <stack>

#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <BRepTools.hxx>
#include <gp_Pln.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_MeshTool, Standard_Transient)

namespace
{
//! Returns index of triangle node opposite to the given link.
Standard_Integer findApexIndex(const Standard_Integer (&aNodes)[3], const BRepMesh_Edge& theLink)
{
  Standard_Integer i = 0;
  for (; i < 3; ++i)
  {
    if (aNodes[i] != theLink.FirstNode() && aNodes[i] != theLink.LastNode())
    {
      break;
    }
  }

  return i;
}
} // namespace

//=================================================================================================

BRepMesh_MeshTool::BRepMesh_MeshTool(const Handle(BRepMesh_DataStructureOfDelaun)& theStructure)
    : myStructure(theStructure)
{
}

//=================================================================================================

BRepMesh_MeshTool::~BRepMesh_MeshTool() {}

//=================================================================================================

void BRepMesh_MeshTool::Legalize(const Standard_Integer theLinkIndex)
{
  std::stack<Standard_Integer> aStack;
  aStack.push(theLinkIndex);

  IMeshData::MapOfInteger aUsedLinks;
  while (!aStack.empty())
  {
    const Standard_Integer aLinkIndex = aStack.top();
    aStack.pop();

    aUsedLinks.Add(aLinkIndex);
    const BRepMesh_Edge& aLink = myStructure->GetLink(aLinkIndex);
    if (aLink.Movability() != BRepMesh_Frontier)
    {
      const BRepMesh_PairOfIndex& aPair = myStructure->ElementsConnectedTo(aLinkIndex);
      if (aPair.Extent() == 2)
      {
        const BRepMesh_Triangle& aTriangle1 = myStructure->GetElement(aPair.FirstIndex());
        const BRepMesh_Triangle& aTriangle2 = myStructure->GetElement(aPair.LastIndex());

        Standard_Integer aNodes[2][3];
        myStructure->ElementNodes(aTriangle1, aNodes[0]);
        myStructure->ElementNodes(aTriangle2, aNodes[1]);

        const Standard_Integer aApexIndex[2] = {findApexIndex(aNodes[0], aLink),
                                                findApexIndex(aNodes[1], aLink)};

        if (checkCircle(aNodes[0], aNodes[1][aApexIndex[1]])
            || checkCircle(aNodes[1], aNodes[0][aApexIndex[0]]))
        {
          myStructure->RemoveElement(aPair.FirstIndex());
          myStructure->RemoveElement(aPair.LastIndex());
          myStructure->RemoveLink(aLinkIndex);

          addTriangleAndUpdateStack(aNodes[0][(aApexIndex[0])],
                                    aNodes[0][(aApexIndex[0] + 1) % 3],
                                    aNodes[1][(aApexIndex[1])],
                                    aUsedLinks,
                                    aStack);

          addTriangleAndUpdateStack(aNodes[1][(aApexIndex[1])],
                                    aNodes[1][(aApexIndex[1] + 1) % 3],
                                    aNodes[0][(aApexIndex[0])],
                                    aUsedLinks,
                                    aStack);
        }
      }
    }
  }
}

//=================================================================================================

void BRepMesh_MeshTool::EraseItemsConnectedTo(const Standard_Integer theNodeIndex)
{
  BRepMesh_SelectorOfDataStructureOfDelaun aSelector(myStructure);
  aSelector.NeighboursOfNode(theNodeIndex);

  IMeshData::MapOfIntegerInteger aLoopEdges(1, new NCollection_IncAllocator);
  EraseTriangles(aSelector.Elements(), aLoopEdges);
  EraseFreeLinks(aLoopEdges);
  myStructure->RemoveNode(theNodeIndex);
}

//=================================================================================================

void BRepMesh_MeshTool::CleanFrontierLinks()
{
  Handle(NCollection_IncAllocator) aAlloc = new NCollection_IncAllocator;
  IMeshData::MapOfInteger          aTrianglesToErase;
  IMeshData::MapOfIntegerInteger   aLoopEdges(1, aAlloc);

  Handle(IMeshData::MapOfInteger)   aFrontier = GetEdgesByType(BRepMesh_Frontier);
  IMeshData::IteratorOfMapOfInteger aFrontierIt(*aFrontier);
  for (; aFrontierIt.More(); aFrontierIt.Next())
  {
    Standard_Integer     aFrontierId = aFrontierIt.Key();
    const BRepMesh_Edge& aLink       = myStructure->GetLink(aFrontierId);

    Standard_Boolean            isTriangleFound = Standard_False;
    const BRepMesh_PairOfIndex& aPair           = myStructure->ElementsConnectedTo(aFrontierId);
    for (Standard_Integer aElemIt = 1; aElemIt <= aPair.Extent() && !isTriangleFound; ++aElemIt)
    {
      const Standard_Integer   aPriorElemId = aPair.Index(aElemIt);
      const BRepMesh_Triangle& aElement     = myStructure->GetElement(aPriorElemId);
      const Standard_Integer(&e)[3]         = aElement.myEdges;
      const Standard_Boolean(&o)[3]         = aElement.myOrientations;

      for (Standard_Integer n = 0; n < 3 && !isTriangleFound; ++n)
      {
        if (aFrontierId == e[n] && !o[n])
        {
          // Destruction of external triangles on boundary edges
          isTriangleFound = Standard_True;
          aTrianglesToErase.Add(aPriorElemId);

          collectTrianglesOnFreeLinksAroundNodesOf(aLink, e[(n + 1) % 3], aTrianglesToErase);
          collectTrianglesOnFreeLinksAroundNodesOf(aLink, e[(n + 2) % 3], aTrianglesToErase);
        }
      }
    }
  }

  EraseTriangles(aTrianglesToErase, aLoopEdges);
  EraseFreeLinks(aLoopEdges);
}

//=================================================================================================

void BRepMesh_MeshTool::EraseTriangles(const IMeshData::MapOfInteger&  theTriangles,
                                       IMeshData::MapOfIntegerInteger& theLoopEdges)
{
  IMeshData::IteratorOfMapOfInteger aFreeTriangles(theTriangles);
  for (; aFreeTriangles.More(); aFreeTriangles.Next())
  {
    EraseTriangle(aFreeTriangles.Key(), theLoopEdges);
  }
}

//=================================================================================================

void BRepMesh_MeshTool::EraseTriangle(const Standard_Integer          theTriangleIndex,
                                      IMeshData::MapOfIntegerInteger& theLoopEdges)
{
  const BRepMesh_Triangle& aElement = myStructure->GetElement(theTriangleIndex);
  const Standard_Integer(&e)[3]     = aElement.myEdges;
  const Standard_Boolean(&o)[3]     = aElement.myOrientations;

  myStructure->RemoveElement(theTriangleIndex);

  for (Standard_Integer i = 0; i < 3; ++i)
  {
    if (!theLoopEdges.Bind(e[i], o[i]))
    {
      theLoopEdges.UnBind(e[i]);
      myStructure->RemoveLink(e[i]);
    }
  }
}

//=================================================================================================

void BRepMesh_MeshTool::EraseFreeLinks()
{
  for (Standard_Integer i = 1; i <= myStructure->NbLinks(); i++)
  {
    if (myStructure->ElementsConnectedTo(i).IsEmpty())
    {
      BRepMesh_Edge& anEdge = (BRepMesh_Edge&)myStructure->GetLink(i);
      if (anEdge.Movability() == BRepMesh_Deleted)
      {
        continue;
      }

      anEdge.SetMovability(BRepMesh_Free);
      myStructure->RemoveLink(i);
    }
  }
}

//=================================================================================================

void BRepMesh_MeshTool::collectTrianglesOnFreeLinksAroundNodesOf(
  const BRepMesh_Edge&     theConstraint,
  const Standard_Integer   theStartLink,
  IMeshData::MapOfInteger& theTriangles)
{
  IMeshData::MapOfInteger      aUsedLinks;
  std::stack<Standard_Integer> aStack;
  aStack.push(theStartLink);
  aUsedLinks.Add(theStartLink);

  while (!aStack.empty())
  {
    const Standard_Integer aLinkIndex = aStack.top();
    aStack.pop();

    const BRepMesh_Edge& aLink = myStructure->GetLink(aLinkIndex);
    if (aLink.Movability() == BRepMesh_Free
        && (aLink.FirstNode() == theConstraint.FirstNode()
            || aLink.LastNode() == theConstraint.FirstNode()
            || aLink.FirstNode() == theConstraint.LastNode()
            || aLink.LastNode() == theConstraint.LastNode()))
    {
      const BRepMesh_PairOfIndex& aPair = myStructure->ElementsConnectedTo(aLinkIndex);
      for (Standard_Integer aElemIt = 1; aElemIt <= aPair.Extent(); ++aElemIt)
      {
        const Standard_Integer aIndex = aPair.Index(aElemIt);
        theTriangles.Add(aIndex);

        const BRepMesh_Triangle& aElement  = myStructure->GetElement(aIndex);
        const Standard_Integer(&aEdges)[3] = aElement.myEdges;

        for (Standard_Integer i = 0; i < 3; ++i)
        {
          if (aEdges[i] != aLinkIndex && !aUsedLinks.Contains(aEdges[i]))
          {
            aUsedLinks.Add(aEdges[i]);
            aStack.push(aEdges[i]);
          }
        }
      }
    }
  }
}

//=================================================================================================

void BRepMesh_MeshTool::EraseFreeLinks(const IMeshData::MapOfIntegerInteger& theLinks)
{
  IMeshData::MapOfIntegerInteger::Iterator aFreeEdges(theLinks);
  for (; aFreeEdges.More(); aFreeEdges.Next())
  {
    if (myStructure->ElementsConnectedTo(aFreeEdges.Key()).IsEmpty())
    {
      myStructure->RemoveLink(aFreeEdges.Key());
    }
  }
}

//=================================================================================================

Handle(IMeshData::MapOfInteger) BRepMesh_MeshTool::GetEdgesByType(
  const BRepMesh_DegreeOfFreedom theEdgeType) const
{
  Handle(IMeshData::MapOfInteger)   aResult = new IMeshData::MapOfInteger;
  IMeshData::IteratorOfMapOfInteger aEdgeIt(myStructure->LinksOfDomain());

  for (; aEdgeIt.More(); aEdgeIt.Next())
  {
    const BRepMesh_Edge& aEdge = myStructure->GetLink(aEdgeIt.Key());
    if (aEdge.Movability() == theEdgeType)
    {
      aResult->Add(aEdgeIt.Key());
    }
  }

  return aResult;
}

//=================================================================================================

void BRepMesh_MeshTool::DumpTriangles(const Standard_CString   theFileName,
                                      IMeshData::MapOfInteger* theTriangles)
{
  BRep_Builder    aBuilder;
  TopoDS_Compound aResult;
  aBuilder.MakeCompound(aResult);

  const IMeshData::MapOfInteger& aTriangles = myStructure->ElementsOfDomain();
  for (IMeshData::IteratorOfMapOfInteger aIt(aTriangles); aIt.More(); aIt.Next())
  {
    if (theTriangles != NULL && !theTriangles->Contains(aIt.Key()))
      continue;

    Standard_Integer         aNodes[3];
    const BRepMesh_Triangle& aTri = myStructure->GetElement(aIt.Key());
    myStructure->ElementNodes(aTri, aNodes);

    const gp_XY& aV1 = myStructure->GetNode(aNodes[0]).Coord();
    const gp_XY& aV2 = myStructure->GetNode(aNodes[1]).Coord();
    const gp_XY& aV3 = myStructure->GetNode(aNodes[2]).Coord();

    BRepBuilderAPI_MakePolygon aPoly(gp_Pnt(aV1.X(), aV1.Y(), 0.),
                                     gp_Pnt(aV2.X(), aV2.Y(), 0.),
                                     gp_Pnt(aV3.X(), aV3.Y(), 0.),
                                     Standard_True);

    BRepBuilderAPI_MakeFace aFaceBuilder(gp_Pln(gp::XOY()), aPoly.Wire());
    aBuilder.Add(aResult, aFaceBuilder.Shape());
  }

  BRepTools::Write(aResult, theFileName);
}
