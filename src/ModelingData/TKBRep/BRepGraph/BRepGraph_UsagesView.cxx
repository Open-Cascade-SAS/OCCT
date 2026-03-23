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

#include <BRepGraph_UsagesView.hxx>
#include <BRepGraph_Data.hxx>

//=================================================================================================

int BRepGraph::UsagesView::NbSolids() const { return myGraph->myData->mySolids.Usages.Length(); }

//=================================================================================================

int BRepGraph::UsagesView::NbShells() const { return myGraph->myData->myShells.Usages.Length(); }

//=================================================================================================

int BRepGraph::UsagesView::NbFaces() const { return myGraph->myData->myFaces.Usages.Length(); }

//=================================================================================================

int BRepGraph::UsagesView::NbWires() const { return myGraph->myData->myWires.Usages.Length(); }

//=================================================================================================

int BRepGraph::UsagesView::NbEdges() const { return myGraph->myData->myEdges.Usages.Length(); }

//=================================================================================================

int BRepGraph::UsagesView::NbVertices() const { return myGraph->myData->myVertices.Usages.Length(); }

//=================================================================================================

int BRepGraph::UsagesView::NbCompounds() const { return myGraph->myData->myCompounds.Usages.Length(); }

//=================================================================================================

int BRepGraph::UsagesView::NbCompSolids() const { return myGraph->myData->myCompSolids.Usages.Length(); }

//=================================================================================================

const BRepGraph_TopoNode::SolidUsage& BRepGraph::UsagesView::Solid(int theIdx) const
{
  return myGraph->myData->mySolids.Usages.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::ShellUsage& BRepGraph::UsagesView::Shell(int theIdx) const
{
  return myGraph->myData->myShells.Usages.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::FaceUsage& BRepGraph::UsagesView::Face(int theIdx) const
{
  return myGraph->myData->myFaces.Usages.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::WireUsage& BRepGraph::UsagesView::Wire(int theIdx) const
{
  return myGraph->myData->myWires.Usages.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::EdgeUsage& BRepGraph::UsagesView::Edge(int theIdx) const
{
  return myGraph->myData->myEdges.Usages.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::VertexUsage& BRepGraph::UsagesView::Vertex(int theIdx) const
{
  return myGraph->myData->myVertices.Usages.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::CompoundUsage& BRepGraph::UsagesView::Compound(int theIdx) const
{
  return myGraph->myData->myCompounds.Usages.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::CompSolidUsage& BRepGraph::UsagesView::CompSolid(int theIdx) const
{
  return myGraph->myData->myCompSolids.Usages.Value(theIdx);
}
