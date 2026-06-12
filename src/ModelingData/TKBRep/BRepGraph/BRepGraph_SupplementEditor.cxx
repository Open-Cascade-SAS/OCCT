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

#include <BRepGraph_SupplementEditor.hxx>

#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_TopoView.hxx>

namespace
{
bool isValidSupplementOwner(const BRepGraph& theGraph, const BRepGraph_NodeId theOwner)
{
  if (!theOwner.IsValid())
  {
    return false;
  }

  switch (theOwner.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
    case BRepGraph_NodeId::Kind::Edge:
    case BRepGraph_NodeId::Kind::Face:
    case BRepGraph_NodeId::Kind::Shell:
    case BRepGraph_NodeId::Kind::Solid:
    case BRepGraph_NodeId::Kind::CompSolid:
    case BRepGraph_NodeId::Kind::Compound:
      break;
    case BRepGraph_NodeId::Kind::Wire:
    case BRepGraph_NodeId::Kind::CoEdge:
    case BRepGraph_NodeId::Kind::Product:
    case BRepGraph_NodeId::Kind::Occurrence:
      return false;
  }

  return !theGraph.Topo().Gen().IsRemoved(theOwner);
}
} // namespace

//=================================================================================================

uint64_t BRepGraph_SupplementEditor::Attach(
  const BRepGraph_NodeId                              theOwner,
  const BRepGraph_LayerTopoSupplement::AttachmentKind theKind,
  const TopoDS_Shape&                                 theShape)
{
  if (!isValidSupplementOwner(myGraph, theOwner) || theShape.IsNull())
  {
    return 0;
  }

  return myGraph.LayerRegistry().Ensure<BRepGraph_LayerTopoSupplement>()->AddAttachment(theOwner,
                                                                                        theKind,
                                                                                        theShape);
}

//=================================================================================================

uint64_t BRepGraph_SupplementEditor::AttachToVertex(
  const BRepGraph_VertexId                            theVertex,
  const TopoDS_Shape&                                 theShape,
  const BRepGraph_LayerTopoSupplement::AttachmentKind theKind)
{
  return Attach(BRepGraph_NodeId(theVertex), theKind, theShape);
}

//=================================================================================================

uint64_t BRepGraph_SupplementEditor::AttachToEdge(
  const BRepGraph_EdgeId                              theEdge,
  const TopoDS_Shape&                                 theShape,
  const BRepGraph_LayerTopoSupplement::AttachmentKind theKind)
{
  return Attach(BRepGraph_NodeId(theEdge), theKind, theShape);
}

//=================================================================================================

uint64_t BRepGraph_SupplementEditor::AttachToFace(
  const BRepGraph_FaceId                              theFace,
  const TopoDS_Shape&                                 theShape,
  const BRepGraph_LayerTopoSupplement::AttachmentKind theKind)
{
  return Attach(BRepGraph_NodeId(theFace), theKind, theShape);
}

//=================================================================================================

uint64_t BRepGraph_SupplementEditor::AttachToSolid(
  const BRepGraph_SolidId                             theSolid,
  const TopoDS_Shape&                                 theShape,
  const BRepGraph_LayerTopoSupplement::AttachmentKind theKind)
{
  return Attach(BRepGraph_NodeId(theSolid), theKind, theShape);
}

//=================================================================================================

uint64_t BRepGraph_SupplementEditor::AttachToCompSolid(
  const BRepGraph_CompSolidId                         theCompSolid,
  const TopoDS_Shape&                                 theShape,
  const BRepGraph_LayerTopoSupplement::AttachmentKind theKind)
{
  return Attach(BRepGraph_NodeId(theCompSolid), theKind, theShape);
}

//=================================================================================================

uint64_t BRepGraph_SupplementEditor::AttachToShell(
  const BRepGraph_ShellId                             theShell,
  const TopoDS_Shape&                                 theShape,
  const BRepGraph_LayerTopoSupplement::AttachmentKind theKind)
{
  return Attach(BRepGraph_NodeId(theShell), theKind, theShape);
}

//=================================================================================================

uint64_t BRepGraph_SupplementEditor::AttachToCompound(
  const BRepGraph_CompoundId                          theCompound,
  const TopoDS_Shape&                                 theShape,
  const BRepGraph_LayerTopoSupplement::AttachmentKind theKind)
{
  return Attach(BRepGraph_NodeId(theCompound), theKind, theShape);
}

//=================================================================================================

bool BRepGraph_SupplementEditor::RemoveAttachment(const uint64_t theUid)
{
  const occ::handle<BRepGraph_LayerTopoSupplement> aLayer =
    myGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
  return !aLayer.IsNull() && aLayer->RemoveAttachment(theUid);
}
