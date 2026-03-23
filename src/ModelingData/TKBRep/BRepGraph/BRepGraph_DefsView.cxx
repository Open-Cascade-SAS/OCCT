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

#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_UsagesView.hxx>

//=================================================================================================

int BRepGraph::DefsView::NbSolids() const { return myGraph->myData->mySolids.Defs.Length(); }

//=================================================================================================

int BRepGraph::DefsView::NbShells() const { return myGraph->myData->myShells.Defs.Length(); }

//=================================================================================================

int BRepGraph::DefsView::NbFaces() const { return myGraph->myData->myFaces.Defs.Length(); }

//=================================================================================================

int BRepGraph::DefsView::NbWires() const { return myGraph->myData->myWires.Defs.Length(); }

//=================================================================================================

int BRepGraph::DefsView::NbEdges() const { return myGraph->myData->myEdges.Defs.Length(); }

//=================================================================================================

int BRepGraph::DefsView::NbVertices() const { return myGraph->myData->myVertices.Defs.Length(); }

//=================================================================================================

int BRepGraph::DefsView::NbCompounds() const { return myGraph->myData->myCompounds.Defs.Length(); }

//=================================================================================================

int BRepGraph::DefsView::NbCompSolids() const { return myGraph->myData->myCompSolids.Defs.Length(); }

//=================================================================================================

const BRepGraph_TopoNode::SolidDef& BRepGraph::DefsView::Solid(int theIdx) const
{
  return myGraph->myData->mySolids.Defs.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::ShellDef& BRepGraph::DefsView::Shell(int theIdx) const
{
  return myGraph->myData->myShells.Defs.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::FaceDef& BRepGraph::DefsView::Face(int theIdx) const
{
  return myGraph->myData->myFaces.Defs.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::WireDef& BRepGraph::DefsView::Wire(int theIdx) const
{
  return myGraph->myData->myWires.Defs.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::EdgeDef& BRepGraph::DefsView::Edge(int theIdx) const
{
  return myGraph->myData->myEdges.Defs.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::VertexDef& BRepGraph::DefsView::Vertex(int theIdx) const
{
  return myGraph->myData->myVertices.Defs.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::CompoundDef& BRepGraph::DefsView::Compound(int theIdx) const
{
  return myGraph->myData->myCompounds.Defs.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::CompSolidDef& BRepGraph::DefsView::CompSolid(int theIdx) const
{
  return myGraph->myData->myCompSolids.Defs.Value(theIdx);
}

//=================================================================================================

int BRepGraph::DefsView::NbShellFaces(int theShellDefIdx) const
{
  const BRepGraph_TopoNode::ShellDef& aShellDef = myGraph->myData->myShells.Defs.Value(theShellDefIdx);
  if (aShellDef.Usages.IsEmpty())
    return 0;
  const BRepGraph_TopoNode::ShellUsage& aSU =
    myGraph->myData->myShells.Usages.Value(aShellDef.Usages.Value(0).Index);
  return aSU.FaceUsages.Length();
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::DefsView::ShellFaceDef(int theShellDefIdx, int theFaceIdx) const
{
  const BRepGraph_TopoNode::ShellDef& aShellDef = myGraph->myData->myShells.Defs.Value(theShellDefIdx);
  if (aShellDef.Usages.IsEmpty())
    return BRepGraph_NodeId();
  const BRepGraph_TopoNode::ShellUsage& aSU =
    myGraph->myData->myShells.Usages.Value(aShellDef.Usages.Value(0).Index);
  if (theFaceIdx < 0 || theFaceIdx >= aSU.FaceUsages.Length())
    return BRepGraph_NodeId();
  return myGraph->DefOf(aSU.FaceUsages.Value(theFaceIdx));
}

//=================================================================================================

const BRepGraph_TopoNode::BaseDef* BRepGraph::DefsView::TopoDef(BRepGraph_NodeId theId) const
{
  return myGraph->TopoDef(theId);
}

//=================================================================================================

size_t BRepGraph::DefsView::NbNodes() const
{
  return static_cast<size_t>(myGraph->myData->mySolids.Defs.Length())
         + static_cast<size_t>(myGraph->myData->myShells.Defs.Length())
         + static_cast<size_t>(myGraph->myData->myFaces.Defs.Length())
         + static_cast<size_t>(myGraph->myData->myWires.Defs.Length())
         + static_cast<size_t>(myGraph->myData->myEdges.Defs.Length())
         + static_cast<size_t>(myGraph->myData->myVertices.Defs.Length())
         + static_cast<size_t>(myGraph->myData->myCompounds.Defs.Length())
         + static_cast<size_t>(myGraph->myData->myCompSolids.Defs.Length())
         + static_cast<size_t>(myGraph->myData->mySurfaces.Nodes.Length())
         + static_cast<size_t>(myGraph->myData->myCurves.Nodes.Length());
}
