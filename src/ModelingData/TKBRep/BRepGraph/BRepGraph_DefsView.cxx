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

//=================================================================================================

int BRepGraph::DefsView::NbSolids() const { return myGraph->myData->mySolidDefs.Length(); }

//=================================================================================================

int BRepGraph::DefsView::NbShells() const { return myGraph->myData->myShellDefs.Length(); }

//=================================================================================================

int BRepGraph::DefsView::NbFaces() const { return myGraph->myData->myFaceDefs.Length(); }

//=================================================================================================

int BRepGraph::DefsView::NbWires() const { return myGraph->myData->myWireDefs.Length(); }

//=================================================================================================

int BRepGraph::DefsView::NbEdges() const { return myGraph->myData->myEdgeDefs.Length(); }

//=================================================================================================

int BRepGraph::DefsView::NbVertices() const { return myGraph->myData->myVertexDefs.Length(); }

//=================================================================================================

int BRepGraph::DefsView::NbCompounds() const { return myGraph->myData->myCompoundDefs.Length(); }

//=================================================================================================

int BRepGraph::DefsView::NbCompSolids() const { return myGraph->myData->myCompSolidDefs.Length(); }

//=================================================================================================

const BRepGraph_TopoNode::SolidDef& BRepGraph::DefsView::Solid(int theIdx) const
{
  return myGraph->myData->mySolidDefs.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::ShellDef& BRepGraph::DefsView::Shell(int theIdx) const
{
  return myGraph->myData->myShellDefs.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::FaceDef& BRepGraph::DefsView::Face(int theIdx) const
{
  return myGraph->myData->myFaceDefs.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::WireDef& BRepGraph::DefsView::Wire(int theIdx) const
{
  return myGraph->myData->myWireDefs.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::EdgeDef& BRepGraph::DefsView::Edge(int theIdx) const
{
  return myGraph->myData->myEdgeDefs.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::VertexDef& BRepGraph::DefsView::Vertex(int theIdx) const
{
  return myGraph->myData->myVertexDefs.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::CompoundDef& BRepGraph::DefsView::Compound(int theIdx) const
{
  return myGraph->myData->myCompoundDefs.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::CompSolidDef& BRepGraph::DefsView::CompSolid(int theIdx) const
{
  return myGraph->myData->myCompSolidDefs.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::BaseDef* BRepGraph::DefsView::TopoDef(BRepGraph_NodeId theId) const
{
  return myGraph->TopoDef(theId);
}

//=================================================================================================

size_t BRepGraph::DefsView::NbNodes() const
{
  return static_cast<size_t>(myGraph->myData->mySolidDefs.Length())
         + static_cast<size_t>(myGraph->myData->myShellDefs.Length())
         + static_cast<size_t>(myGraph->myData->myFaceDefs.Length())
         + static_cast<size_t>(myGraph->myData->myWireDefs.Length())
         + static_cast<size_t>(myGraph->myData->myEdgeDefs.Length())
         + static_cast<size_t>(myGraph->myData->myVertexDefs.Length())
         + static_cast<size_t>(myGraph->myData->myCompoundDefs.Length())
         + static_cast<size_t>(myGraph->myData->myCompSolidDefs.Length())
         + static_cast<size_t>(myGraph->myData->mySurfaces.Length())
         + static_cast<size_t>(myGraph->myData->myCurves.Length())
         + static_cast<size_t>(myGraph->myData->myPCurves.Length());
}
