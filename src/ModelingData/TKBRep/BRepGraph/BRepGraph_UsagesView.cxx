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

int BRepGraph::UsagesView::NbSolids() const { return myGraph->myData->mySolidUsages.Length(); }

//=================================================================================================

int BRepGraph::UsagesView::NbShells() const { return myGraph->myData->myShellUsages.Length(); }

//=================================================================================================

int BRepGraph::UsagesView::NbFaces() const { return myGraph->myData->myFaceUsages.Length(); }

//=================================================================================================

int BRepGraph::UsagesView::NbWires() const { return myGraph->myData->myWireUsages.Length(); }

//=================================================================================================

int BRepGraph::UsagesView::NbEdges() const { return myGraph->myData->myEdgeUsages.Length(); }

//=================================================================================================

int BRepGraph::UsagesView::NbVertices() const { return myGraph->myData->myVertexUsages.Length(); }

//=================================================================================================

int BRepGraph::UsagesView::NbCompounds() const { return myGraph->myData->myCompoundUsages.Length(); }

//=================================================================================================

int BRepGraph::UsagesView::NbCompSolids() const { return myGraph->myData->myCompSolidUsages.Length(); }

//=================================================================================================

const BRepGraph_TopoNode::SolidUsage& BRepGraph::UsagesView::Solid(int theIdx) const
{
  return myGraph->myData->mySolidUsages.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::ShellUsage& BRepGraph::UsagesView::Shell(int theIdx) const
{
  return myGraph->myData->myShellUsages.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::FaceUsage& BRepGraph::UsagesView::Face(int theIdx) const
{
  return myGraph->myData->myFaceUsages.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::WireUsage& BRepGraph::UsagesView::Wire(int theIdx) const
{
  return myGraph->myData->myWireUsages.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::EdgeUsage& BRepGraph::UsagesView::Edge(int theIdx) const
{
  return myGraph->myData->myEdgeUsages.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::VertexUsage& BRepGraph::UsagesView::Vertex(int theIdx) const
{
  return myGraph->myData->myVertexUsages.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::CompoundUsage& BRepGraph::UsagesView::Compound(int theIdx) const
{
  return myGraph->myData->myCompoundUsages.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::CompSolidUsage& BRepGraph::UsagesView::CompSolid(int theIdx) const
{
  return myGraph->myData->myCompSolidUsages.Value(theIdx);
}
