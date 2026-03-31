// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <BRepGraph_ChildIterator.hxx>

#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>

#include <TopAbs.hxx>

//=================================================================================================

BRepGraph_ChildIterator::BRepGraph_ChildIterator(const BRepGraph&       theGraph,
                                                 const BRepGraph_NodeId theParent)
    : BRepGraph_ChildIterator(theGraph, theParent, TopLoc_Location(), TopAbs_FORWARD)
{
}

//=================================================================================================

BRepGraph_ChildIterator::BRepGraph_ChildIterator(const BRepGraph&         theGraph,
                                                 const BRepGraph_NodeId   theParent,
                                                 const TopLoc_Location&   theParentLoc,
                                                 const TopAbs_Orientation theParentOri)
    : myGraph(&theGraph),
      myParent(theParent),
      myParentLocation(theParentLoc),
      myParentOrientation(theParentOri),
      myLocation(theParentLoc),
      myOrientation(theParentOri)
{
  if (!myParent.IsValid())
    return;

  resolve1to1(myParent, myParentLocation, myParentOrientation);
  if (!myParent.IsValid())
    return;

  const BRepGraphInc::BaseDef* aParentDef = myGraph->Topo().TopoEntity(myParent);
  if (aParentDef == nullptr || aParentDef->IsRemoved)
    return;

  myLocation    = myParentLocation;
  myOrientation = myParentOrientation;
  advance();
}

//=================================================================================================

void BRepGraph_ChildIterator::Next()
{
  advance();
}

//=================================================================================================

void BRepGraph_ChildIterator::advance()
{
  using Kind = BRepGraph_NodeId::Kind;

  myHasMore = false;

  if (myGraph == nullptr || !myParent.IsValid())
    return;

  const BRepGraph::TopoView& aDefs = myGraph->Topo();
  const BRepGraph::RefsView& aRefs = myGraph->Refs();

  for (;;)
  {
    BRepGraph_NodeId   aChildNode;
    TopLoc_Location    aChildLoc = myParentLocation;
    TopAbs_Orientation aChildOri = myParentOrientation;

    switch (myParent.NodeKind)
    {
      case Kind::Compound: {
        const BRepGraphInc::CompoundDef& aComp =
          aDefs.Compounds().Definition(BRepGraph_CompoundId(myParent.Index));
        int i = myChildIdx;
        for (; i < aComp.ChildRefIds.Length(); ++i)
        {
          const BRepGraphInc::ChildRef& aRef = aRefs.Child(aComp.ChildRefIds.Value(i));
          if (aRef.IsRemoved)
            continue;

          aChildNode = aRef.ChildDefId;
          aChildLoc  = myParentLocation * aRef.LocalLocation;
          aChildOri  = TopAbs::Compose(myParentOrientation, aRef.Orientation);
          break;
        }
        myChildIdx = i + 1;
        break;
      }

      case Kind::CompSolid: {
        const BRepGraphInc::CompSolidDef& aCompSolid =
          aDefs.CompSolids().Definition(BRepGraph_CompSolidId(myParent.Index));
        int i = myChildIdx;
        for (; i < aCompSolid.SolidRefIds.Length(); ++i)
        {
          const BRepGraphInc::SolidRef& aRef = aRefs.Solid(aCompSolid.SolidRefIds.Value(i));
          if (aRef.IsRemoved)
            continue;

          aChildNode = aRef.SolidDefId;
          aChildLoc  = myParentLocation * aRef.LocalLocation;
          aChildOri  = TopAbs::Compose(myParentOrientation, aRef.Orientation);
          break;
        }
        myChildIdx = i + 1;
        break;
      }

      case Kind::Solid: {
        const BRepGraphInc::SolidDef& aSolid = aDefs.Solids().Definition(BRepGraph_SolidId(myParent.Index));
        const int aNbShells = aSolid.ShellRefIds.Length();
        const int aNbFree   = aSolid.FreeChildRefIds.Length();
        int i = myChildIdx;
        for (; i < aNbShells + aNbFree; ++i)
        {
          if (i < aNbShells)
          {
            const BRepGraphInc::ShellRef& aRef = aRefs.Shell(aSolid.ShellRefIds.Value(i));
            if (aRef.IsRemoved)
              continue;

            aChildNode = aRef.ShellDefId;
            aChildLoc  = myParentLocation * aRef.LocalLocation;
            aChildOri  = TopAbs::Compose(myParentOrientation, aRef.Orientation);
            break;
          }

          const int aFreeIdx = i - aNbShells;
          const BRepGraphInc::ChildRef& aRef = aRefs.Child(aSolid.FreeChildRefIds.Value(aFreeIdx));
          if (aRef.IsRemoved)
            continue;

          aChildNode = aRef.ChildDefId;
          aChildLoc  = myParentLocation * aRef.LocalLocation;
          aChildOri  = TopAbs::Compose(myParentOrientation, aRef.Orientation);
          break;
        }
        myChildIdx = i + 1;
        break;
      }

      case Kind::Shell: {
        const BRepGraphInc::ShellDef& aShell = aDefs.Shells().Definition(BRepGraph_ShellId(myParent.Index));
        const int aNbFaces = aShell.FaceRefIds.Length();
        const int aNbFree  = aShell.FreeChildRefIds.Length();
        int i = myChildIdx;
        for (; i < aNbFaces + aNbFree; ++i)
        {
          if (i < aNbFaces)
          {
            const BRepGraphInc::FaceRef& aRef = aRefs.Face(aShell.FaceRefIds.Value(i));
            if (aRef.IsRemoved)
              continue;

            aChildNode = aRef.FaceDefId;
            aChildLoc  = myParentLocation * aRef.LocalLocation;
            aChildOri  = TopAbs::Compose(myParentOrientation, aRef.Orientation);
            break;
          }

          const int aFreeIdx = i - aNbFaces;
          const BRepGraphInc::ChildRef& aRef = aRefs.Child(aShell.FreeChildRefIds.Value(aFreeIdx));
          if (aRef.IsRemoved)
            continue;

          aChildNode = aRef.ChildDefId;
          aChildLoc  = myParentLocation * aRef.LocalLocation;
          aChildOri  = TopAbs::Compose(myParentOrientation, aRef.Orientation);
          break;
        }
        myChildIdx = i + 1;
        break;
      }

      case Kind::Face: {
        const BRepGraphInc::FaceDef& aFace = aDefs.Faces().Definition(BRepGraph_FaceId(myParent.Index));
        const int aNbWires = aFace.WireRefIds.Length();
        const int aNbVerts = aFace.VertexRefIds.Length();
        int i = myChildIdx;
        for (; i < aNbWires + aNbVerts; ++i)
        {
          if (i < aNbWires)
          {
            const BRepGraphInc::WireRef& aRef = aRefs.Wire(aFace.WireRefIds.Value(i));
            if (aRef.IsRemoved)
              continue;

            aChildNode = aRef.WireDefId;
            aChildLoc  = myParentLocation * aRef.LocalLocation;
            aChildOri  = TopAbs::Compose(myParentOrientation, aRef.Orientation);
            break;
          }

          const int aVtxIdx = i - aNbWires;
          const BRepGraphInc::VertexRef& aRef = aRefs.Vertex(aFace.VertexRefIds.Value(aVtxIdx));
          if (aRef.IsRemoved)
            continue;

          aChildNode = aRef.VertexDefId;
          aChildLoc  = myParentLocation * aRef.LocalLocation;
          aChildOri  = TopAbs::Compose(myParentOrientation, aRef.Orientation);
          break;
        }
        myChildIdx = i + 1;
        break;
      }

      case Kind::Wire: {
        const BRepGraphInc::WireDef& aWire = aDefs.Wires().Definition(BRepGraph_WireId(myParent.Index));
        int i = myChildIdx;
        for (; i < aWire.CoEdgeRefIds.Length(); ++i)
        {
          const BRepGraphInc::CoEdgeRef& aRef = aRefs.CoEdge(aWire.CoEdgeRefIds.Value(i));
          if (aRef.IsRemoved)
            continue;

          aChildNode = aRef.CoEdgeDefId;
          aChildLoc  = myParentLocation * aRef.LocalLocation;
          // CoEdgeRef has no Orientation field.
          break;
        }
        myChildIdx = i + 1;
        break;
      }

      case Kind::Edge: {
        const BRepGraphInc::EdgeDef& anEdge = aDefs.Edges().Definition(BRepGraph_EdgeId(myParent.Index));
        const int aNbIntern = anEdge.InternalVertexRefIds.Length();
        const int aNbTotal  = 2 + aNbIntern;
        int i = myChildIdx;
        for (; i < aNbTotal; ++i)
        {
          BRepGraph_VertexRefId aRefId;
          if (i == 0)
            aRefId = anEdge.StartVertexRefId;
          else if (i == 1)
            aRefId = anEdge.EndVertexRefId;
          else
            aRefId = anEdge.InternalVertexRefIds.Value(i - 2);

          if (!aRefId.IsValid())
            continue;

          const BRepGraphInc::VertexRef& aRef = aRefs.Vertex(aRefId);
          if (aRef.IsRemoved)
            continue;

          aChildNode = aRef.VertexDefId;
          aChildLoc  = myParentLocation * aRef.LocalLocation;
          aChildOri  = TopAbs::Compose(myParentOrientation, aRef.Orientation);
          break;
        }
        myChildIdx = i + 1;
        break;
      }

      case Kind::Product: {
        const BRepGraphInc::ProductDef& aProduct =
          aDefs.Products().Definition(BRepGraph_ProductId(myParent.Index));
        int i = myChildIdx;
        for (; i < aProduct.OccurrenceRefIds.Length(); ++i)
        {
          const BRepGraphInc::OccurrenceRef& aRef = aRefs.Occurrence(aProduct.OccurrenceRefIds.Value(i));
          if (aRef.IsRemoved)
            continue;

          aChildNode = aRef.OccurrenceDefId;
          break;
        }
        myChildIdx = i + 1;
        break;
      }

      default:
        return;
    }

    if (!aChildNode.IsValid())
      return;

    // Keep the pre-resolution ref ordinal semantics for this emitted row.
    resolve1to1(aChildNode, aChildLoc, aChildOri);
    if (!aChildNode.IsValid())
      continue;

    const BRepGraphInc::BaseDef* aChildDef = aDefs.TopoEntity(aChildNode);
    if (aChildDef == nullptr || aChildDef->IsRemoved)
      continue;

    myCurrent     = aChildNode;
    myLocation    = aChildLoc;
    myOrientation = aChildOri;
    myHasMore     = true;
    return;
  }
}

//=================================================================================================

void BRepGraph_ChildIterator::resolve1to1(BRepGraph_NodeId&   theNode,
                                          TopLoc_Location&    theLoc,
                                          TopAbs_Orientation& theOri) const
{
  using Kind = BRepGraph_NodeId::Kind;
  const BRepGraph::TopoView& aDefs = myGraph->Topo();

  for (;;)
  {
    if (!theNode.IsValid())
      return;

    if (theNode.NodeKind == Kind::CoEdge)
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge = aDefs.CoEdges().Definition(BRepGraph_CoEdgeId(theNode.Index));
      if (aCoEdge.IsRemoved)
      {
        theNode = BRepGraph_NodeId();
        return;
      }
      theOri  = TopAbs::Compose(theOri, aCoEdge.Sense);
      theNode = aCoEdge.EdgeDefId;
      continue;
    }

    if (theNode.NodeKind == Kind::Occurrence)
    {
      const BRepGraphInc::OccurrenceDef& anOcc =
        aDefs.Occurrences().Definition(BRepGraph_OccurrenceId(theNode.Index));
      if (anOcc.IsRemoved)
      {
        theNode = BRepGraph_NodeId();
        return;
      }
      theLoc  = theLoc * anOcc.Placement;
      theNode = anOcc.ProductDefId;
      continue;
    }

    if (theNode.NodeKind == Kind::Product)
    {
      const BRepGraphInc::ProductDef& aProd = aDefs.Products().Definition(BRepGraph_ProductId(theNode.Index));
      if (aProd.IsRemoved)
      {
        theNode = BRepGraph_NodeId();
        return;
      }

      if (aProd.ShapeRootId.IsValid())
      {
        theLoc  = theLoc * aProd.RootLocation;
        theOri  = TopAbs::Compose(theOri, aProd.RootOrientation);
        theNode = aProd.ShapeRootId;
        continue;
      }
      return;
    }

    return;
  }
}
