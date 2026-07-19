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

#include <BRepGraph_EditorView.hxx>

#include <BRepGraph.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_ReverseIterator.hxx>
#include <BRepGraph_TopoView.hxx>
#include <Standard_ProgramError.hxx>

namespace
{

static bool isNodeIndexInRange(const BRepGraphInc_Storage& theStorage,
                               const BRepGraph_NodeId      theNode)
{
  if (!theNode.IsValid())
  {
    return false;
  }

  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return BRepGraph_VertexId(theNode).IsValid(theStorage.NbVertices());
    case BRepGraph_NodeId::Kind::Edge:
      return BRepGraph_EdgeId(theNode).IsValid(theStorage.NbEdges());
    case BRepGraph_NodeId::Kind::CoEdge:
      return BRepGraph_CoEdgeId(theNode).IsValid(theStorage.NbCoEdges());
    case BRepGraph_NodeId::Kind::Wire:
      return BRepGraph_WireId(theNode).IsValid(theStorage.NbWires());
    case BRepGraph_NodeId::Kind::Face:
      return BRepGraph_FaceId(theNode).IsValid(theStorage.NbFaces());
    case BRepGraph_NodeId::Kind::Shell:
      return BRepGraph_ShellId(theNode).IsValid(theStorage.NbShells());
    case BRepGraph_NodeId::Kind::Solid:
      return BRepGraph_SolidId(theNode).IsValid(theStorage.NbSolids());
    case BRepGraph_NodeId::Kind::Compound:
      return BRepGraph_CompoundId(theNode).IsValid(theStorage.NbCompounds());
    case BRepGraph_NodeId::Kind::CompSolid:
      return BRepGraph_CompSolidId(theNode).IsValid(theStorage.NbCompSolids());
    case BRepGraph_NodeId::Kind::Product:
      return BRepGraph_ProductId(theNode).IsValid(theStorage.NbProducts());
    case BRepGraph_NodeId::Kind::Occurrence:
      return BRepGraph_OccurrenceId(theNode).IsValid(theStorage.NbOccurrences());
  }

  return false;
}

//==================================================================================================

static bool isRefIndexInRange(const BRepGraphInc_Storage& theStorage,
                              const BRepGraph_RefId       theRefId)
{
  if (!theRefId.IsValid())
  {
    return false;
  }

  switch (theRefId.RefKind)
  {
    case BRepGraph_RefId::Kind::Shell:
      return theRefId.IsValid(theStorage.NbShellRefs());
    case BRepGraph_RefId::Kind::Face:
      return theRefId.IsValid(theStorage.NbFaceRefs());
    case BRepGraph_RefId::Kind::Wire:
      return theRefId.IsValid(theStorage.NbWireRefs());
    case BRepGraph_RefId::Kind::Vertex:
      return theRefId.IsValid(theStorage.NbVertexRefs());
    case BRepGraph_RefId::Kind::Solid:
      return theRefId.IsValid(theStorage.NbSolidRefs());
    case BRepGraph_RefId::Kind::Child:
      return theRefId.IsValid(theStorage.NbChildRefs());
    case BRepGraph_RefId::Kind::Occurrence:
      return theRefId.IsValid(theStorage.NbOccurrenceRefs());
  }

  return false;
}

//==================================================================================================

[[maybe_unused]] static bool isActiveNode(const BRepGraphInc_Storage& theStorage,
                                          const BRepGraph_NodeId      theNode)
{
  if (!isNodeIndexInRange(theStorage, theNode))
  {
    return false;
  }
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return !theStorage.IsRemoved(BRepGraph_VertexId(theNode));
    case BRepGraph_NodeId::Kind::Edge:
      return !theStorage.IsRemoved(BRepGraph_EdgeId(theNode));
    case BRepGraph_NodeId::Kind::CoEdge:
      return !theStorage.IsRemoved(BRepGraph_CoEdgeId(theNode));
    case BRepGraph_NodeId::Kind::Wire:
      return !theStorage.IsRemoved(BRepGraph_WireId(theNode));
    case BRepGraph_NodeId::Kind::Face:
      return !theStorage.IsRemoved(BRepGraph_FaceId(theNode));
    case BRepGraph_NodeId::Kind::Shell:
      return !theStorage.IsRemoved(BRepGraph_ShellId(theNode));
    case BRepGraph_NodeId::Kind::Solid:
      return !theStorage.IsRemoved(BRepGraph_SolidId(theNode));
    case BRepGraph_NodeId::Kind::Compound:
      return !theStorage.IsRemoved(BRepGraph_CompoundId(theNode));
    case BRepGraph_NodeId::Kind::CompSolid:
      return !theStorage.IsRemoved(BRepGraph_CompSolidId(theNode));
    case BRepGraph_NodeId::Kind::Product:
      return !theStorage.IsRemoved(BRepGraph_ProductId(theNode));
    case BRepGraph_NodeId::Kind::Occurrence:
      return !theStorage.IsRemoved(BRepGraph_OccurrenceId(theNode));
  }
  return false;
}

//==================================================================================================

[[maybe_unused]] static bool isActiveRef(const BRepGraphInc_Storage& theStorage,
                                         const BRepGraph_RefId       theRefId)
{
  if (!isRefIndexInRange(theStorage, theRefId))
  {
    return false;
  }

  switch (theRefId.RefKind)
  {
    case BRepGraph_RefId::Kind::Shell:
      return !theStorage.IsRemoved(BRepGraph_ShellRefId(theRefId));
    case BRepGraph_RefId::Kind::Face:
      return !theStorage.IsRemoved(BRepGraph_FaceRefId(theRefId));
    case BRepGraph_RefId::Kind::Wire:
      return !theStorage.IsRemoved(BRepGraph_WireRefId(theRefId));
    case BRepGraph_RefId::Kind::Vertex:
      return !theStorage.IsRemoved(BRepGraph_VertexRefId(theRefId));
    case BRepGraph_RefId::Kind::Solid:
      return !theStorage.IsRemoved(BRepGraph_SolidRefId(theRefId));
    case BRepGraph_RefId::Kind::Child:
      return !theStorage.IsRemoved(BRepGraph_ChildRefId(theRefId));
    case BRepGraph_RefId::Kind::Occurrence:
      return !theStorage.IsRemoved(BRepGraph_OccurrenceRefId(theRefId));
  }

  return false;
}

//==================================================================================================

[[maybe_unused]] static bool isOwnedNode(const BRepGraphInc_Storage& theStorage,
                                         const BRepGraph_NodeId      theNodeId)
{
  if (!isNodeIndexInRange(theStorage, theNodeId))
  {
    return false;
  }
  switch (theNodeId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return theStorage.IsOwned(BRepGraph_VertexId(theNodeId));
    case BRepGraph_NodeId::Kind::Edge:
      return theStorage.IsOwned(BRepGraph_EdgeId(theNodeId));
    case BRepGraph_NodeId::Kind::CoEdge:
      return theStorage.IsOwned(BRepGraph_CoEdgeId(theNodeId));
    case BRepGraph_NodeId::Kind::Wire:
      return theStorage.IsOwned(BRepGraph_WireId(theNodeId));
    case BRepGraph_NodeId::Kind::Face:
      return theStorage.IsOwned(BRepGraph_FaceId(theNodeId));
    case BRepGraph_NodeId::Kind::Shell:
      return theStorage.IsOwned(BRepGraph_ShellId(theNodeId));
    case BRepGraph_NodeId::Kind::Solid:
      return theStorage.IsOwned(BRepGraph_SolidId(theNodeId));
    case BRepGraph_NodeId::Kind::Compound:
      return theStorage.IsOwned(BRepGraph_CompoundId(theNodeId));
    case BRepGraph_NodeId::Kind::CompSolid:
      return theStorage.IsOwned(BRepGraph_CompSolidId(theNodeId));
    case BRepGraph_NodeId::Kind::Product:
      return theStorage.IsOwned(BRepGraph_ProductId(theNodeId));
    case BRepGraph_NodeId::Kind::Occurrence:
      return theStorage.IsOwned(BRepGraph_OccurrenceId(theNodeId));
  }
  return false;
}

//==================================================================================================

static void validateMutableNodeId(const BRepGraphInc_Storage& theStorage [[maybe_unused]],
                                  const BRepGraph_NodeId      theNodeId [[maybe_unused]],
                                  const BRepGraph& /*theGraph*/)
{
  Standard_ProgramError_Raise_if(!isActiveNode(theStorage, theNodeId),
                                 "BRepGraph::EditorView::Mut*(): invalid node id");
  Standard_ProgramError_Raise_if(isOwnedNode(theStorage, theNodeId),
                                 "BRepGraph::EditorView::Mut*(): owned node");
}

//==================================================================================================

static void validateMutableRefId(const BRepGraphInc_Storage& theStorage [[maybe_unused]],
                                 const BRepGraph_RefId       theRefId [[maybe_unused]],
                                 const BRepGraph& /*theGraph*/)
{
  Standard_ProgramError_Raise_if(!isActiveRef(theStorage, theRefId),
                                 "BRepGraph::EditorView::Mut*(): invalid reference id");
  switch (theRefId.RefKind)
  {
    case BRepGraph_RefId::Kind::Shell:
      Standard_ProgramError_Raise_if(theStorage.IsOwned(BRepGraph_ShellRefId(theRefId)),
                                     "BRepGraph::EditorView::Mut*(): owned reference");
      break;
    case BRepGraph_RefId::Kind::Face:
      Standard_ProgramError_Raise_if(theStorage.IsOwned(BRepGraph_FaceRefId(theRefId)),
                                     "BRepGraph::EditorView::Mut*(): owned reference");
      break;
    case BRepGraph_RefId::Kind::Wire:
      Standard_ProgramError_Raise_if(theStorage.IsOwned(BRepGraph_WireRefId(theRefId)),
                                     "BRepGraph::EditorView::Mut*(): owned reference");
      break;
    case BRepGraph_RefId::Kind::Vertex:
      Standard_ProgramError_Raise_if(theStorage.IsOwned(BRepGraph_VertexRefId(theRefId)),
                                     "BRepGraph::EditorView::Mut*(): owned reference");
      break;
    case BRepGraph_RefId::Kind::Solid:
      Standard_ProgramError_Raise_if(theStorage.IsOwned(BRepGraph_SolidRefId(theRefId)),
                                     "BRepGraph::EditorView::Mut*(): owned reference");
      break;
    case BRepGraph_RefId::Kind::Child:
      Standard_ProgramError_Raise_if(theStorage.IsOwned(BRepGraph_ChildRefId(theRefId)),
                                     "BRepGraph::EditorView::Mut*(): owned reference");
      break;
    case BRepGraph_RefId::Kind::Occurrence:
      Standard_ProgramError_Raise_if(theStorage.IsOwned(BRepGraph_OccurrenceRefId(theRefId)),
                                     "BRepGraph::EditorView::Mut*(): owned reference");
      break;
  }
}

//==================================================================================================

} // namespace

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::VertexDef> BRepGraph::EditorView::VertexOps::Mut(
  const BRepGraph_VertexId theVertex)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableNodeId(aStorage, BRepGraph_NodeId(theVertex), *myGraph);
  Standard_ProgramError_Raise_if(aStorage.IsGuarded(BRepGraph_ItemId(theVertex)),
                                 "BRepGraph::EditorView::Mut*(): guard already active");
  return BRepGraph_MutGuard<BRepGraphInc::VertexDef>(*myGraph,
                                                     aStorage,
                                                     &aStorage.ChangeVertex(theVertex),
                                                     theVertex);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::EdgeDef> BRepGraph::EditorView::EdgeOps::Mut(
  const BRepGraph_EdgeId theEdge)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableNodeId(aStorage, BRepGraph_NodeId(theEdge), *myGraph);
  Standard_ProgramError_Raise_if(aStorage.IsGuarded(BRepGraph_ItemId(theEdge)),
                                 "BRepGraph::EditorView::Mut*(): guard already active");
  return BRepGraph_MutGuard<BRepGraphInc::EdgeDef>(*myGraph,
                                                   aStorage,
                                                   &aStorage.ChangeEdge(theEdge),
                                                   theEdge);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::WireDef> BRepGraph::EditorView::WireOps::Mut(
  const BRepGraph_WireId theWire)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableNodeId(aStorage, BRepGraph_NodeId(theWire), *myGraph);
  Standard_ProgramError_Raise_if(aStorage.IsGuarded(BRepGraph_ItemId(theWire)),
                                 "BRepGraph::EditorView::Mut*(): guard already active");
  return BRepGraph_MutGuard<BRepGraphInc::WireDef>(*myGraph,
                                                   aStorage,
                                                   &aStorage.ChangeWire(theWire),
                                                   theWire);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::FaceDef> BRepGraph::EditorView::FaceOps::Mut(
  const BRepGraph_FaceId theFace)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableNodeId(aStorage, BRepGraph_NodeId(theFace), *myGraph);
  Standard_ProgramError_Raise_if(aStorage.IsGuarded(BRepGraph_ItemId(theFace)),
                                 "BRepGraph::EditorView::Mut*(): guard already active");
  return BRepGraph_MutGuard<BRepGraphInc::FaceDef>(*myGraph,
                                                   aStorage,
                                                   &aStorage.ChangeFace(theFace),
                                                   theFace);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::ShellDef> BRepGraph::EditorView::ShellOps::Mut(
  const BRepGraph_ShellId theShell)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableNodeId(aStorage, BRepGraph_NodeId(theShell), *myGraph);
  Standard_ProgramError_Raise_if(aStorage.IsGuarded(BRepGraph_ItemId(theShell)),
                                 "BRepGraph::EditorView::Mut*(): guard already active");
  return BRepGraph_MutGuard<BRepGraphInc::ShellDef>(*myGraph,
                                                    aStorage,
                                                    &aStorage.ChangeShell(theShell),
                                                    theShell);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::SolidDef> BRepGraph::EditorView::SolidOps::Mut(
  const BRepGraph_SolidId theSolid)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableNodeId(aStorage, BRepGraph_NodeId(theSolid), *myGraph);
  Standard_ProgramError_Raise_if(aStorage.IsGuarded(BRepGraph_ItemId(theSolid)),
                                 "BRepGraph::EditorView::Mut*(): guard already active");
  return BRepGraph_MutGuard<BRepGraphInc::SolidDef>(*myGraph,
                                                    aStorage,
                                                    &aStorage.ChangeSolid(theSolid),
                                                    theSolid);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::CompoundDef> BRepGraph::EditorView::CompoundOps::Mut(
  const BRepGraph_CompoundId theCompound)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableNodeId(aStorage, BRepGraph_NodeId(theCompound), *myGraph);
  Standard_ProgramError_Raise_if(aStorage.IsGuarded(BRepGraph_ItemId(theCompound)),
                                 "BRepGraph::EditorView::Mut*(): guard already active");
  return BRepGraph_MutGuard<BRepGraphInc::CompoundDef>(*myGraph,
                                                       aStorage,
                                                       &aStorage.ChangeCompound(theCompound),
                                                       theCompound);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::CompSolidDef> BRepGraph::EditorView::CompSolidOps::Mut(
  const BRepGraph_CompSolidId theCompSolid)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableNodeId(aStorage, BRepGraph_NodeId(theCompSolid), *myGraph);
  Standard_ProgramError_Raise_if(aStorage.IsGuarded(BRepGraph_ItemId(theCompSolid)),
                                 "BRepGraph::EditorView::Mut*(): guard already active");
  return BRepGraph_MutGuard<BRepGraphInc::CompSolidDef>(*myGraph,
                                                        aStorage,
                                                        &aStorage.ChangeCompSolid(theCompSolid),
                                                        theCompSolid);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef> BRepGraph::EditorView::CoEdgeOps::Mut(
  const BRepGraph_CoEdgeId theCoEdge)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableNodeId(aStorage, BRepGraph_NodeId(theCoEdge), *myGraph);
  Standard_ProgramError_Raise_if(aStorage.IsGuarded(BRepGraph_ItemId(theCoEdge)),
                                 "BRepGraph::EditorView::Mut*(): guard already active");
  return BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>(*myGraph,
                                                     aStorage,
                                                     &aStorage.ChangeCoEdge(theCoEdge),
                                                     theCoEdge);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::ProductDef> BRepGraph::EditorView::ProductOps::Mut(
  const BRepGraph_ProductId theProduct)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableNodeId(aStorage, BRepGraph_NodeId(theProduct), *myGraph);
  Standard_ProgramError_Raise_if(aStorage.IsGuarded(BRepGraph_ItemId(theProduct)),
                                 "BRepGraph::EditorView::Mut*(): guard already active");
  return BRepGraph_MutGuard<BRepGraphInc::ProductDef>(*myGraph,
                                                      aStorage,
                                                      &aStorage.ChangeProduct(theProduct),
                                                      theProduct);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::OccurrenceDef> BRepGraph::EditorView::OccurrenceOps::Mut(
  const BRepGraph_OccurrenceId theOccurrence)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableNodeId(aStorage, BRepGraph_NodeId(theOccurrence), *myGraph);
  Standard_ProgramError_Raise_if(aStorage.IsGuarded(BRepGraph_ItemId(theOccurrence)),
                                 "BRepGraph::EditorView::Mut*(): guard already active");
  return BRepGraph_MutGuard<BRepGraphInc::OccurrenceDef>(*myGraph,
                                                         aStorage,
                                                         &aStorage.ChangeOccurrence(theOccurrence),
                                                         theOccurrence);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::VertexRef> BRepGraph::EditorView::VertexOps::MutRef(
  const BRepGraph_VertexRefId theVertexRef)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableRefId(aStorage, BRepGraph_RefId(theVertexRef), *myGraph);
  Standard_ProgramError_Raise_if(aStorage.IsGuarded(BRepGraph_ItemId(theVertexRef)),
                                 "BRepGraph::EditorView::Mut*(): guard already active");
  return BRepGraph_MutGuard<BRepGraphInc::VertexRef>(*myGraph,
                                                     aStorage,
                                                     &aStorage.ChangeVertexRef(theVertexRef),
                                                     theVertexRef);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::ShellRef> BRepGraph::EditorView::ShellOps::MutRef(
  const BRepGraph_ShellRefId theShellRef)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableRefId(aStorage, BRepGraph_RefId(theShellRef), *myGraph);
  Standard_ProgramError_Raise_if(aStorage.IsGuarded(BRepGraph_ItemId(theShellRef)),
                                 "BRepGraph::EditorView::Mut*(): guard already active");
  return BRepGraph_MutGuard<BRepGraphInc::ShellRef>(*myGraph,
                                                    aStorage,
                                                    &aStorage.ChangeShellRef(theShellRef),
                                                    theShellRef);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::FaceRef> BRepGraph::EditorView::FaceOps::MutRef(
  const BRepGraph_FaceRefId theFaceRef)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableRefId(aStorage, BRepGraph_RefId(theFaceRef), *myGraph);
  Standard_ProgramError_Raise_if(aStorage.IsGuarded(BRepGraph_ItemId(theFaceRef)),
                                 "BRepGraph::EditorView::Mut*(): guard already active");
  return BRepGraph_MutGuard<BRepGraphInc::FaceRef>(*myGraph,
                                                   aStorage,
                                                   &aStorage.ChangeFaceRef(theFaceRef),
                                                   theFaceRef);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::WireRef> BRepGraph::EditorView::WireOps::MutRef(
  const BRepGraph_WireRefId theWireRef)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableRefId(aStorage, BRepGraph_RefId(theWireRef), *myGraph);
  Standard_ProgramError_Raise_if(aStorage.IsGuarded(BRepGraph_ItemId(theWireRef)),
                                 "BRepGraph::EditorView::Mut*(): guard already active");
  return BRepGraph_MutGuard<BRepGraphInc::WireRef>(*myGraph,
                                                   aStorage,
                                                   &aStorage.ChangeWireRef(theWireRef),
                                                   theWireRef);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::SolidRef> BRepGraph::EditorView::SolidOps::MutRef(
  const BRepGraph_SolidRefId theSolidRef)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableRefId(aStorage, BRepGraph_RefId(theSolidRef), *myGraph);
  Standard_ProgramError_Raise_if(aStorage.IsGuarded(BRepGraph_ItemId(theSolidRef)),
                                 "BRepGraph::EditorView::Mut*(): guard already active");
  return BRepGraph_MutGuard<BRepGraphInc::SolidRef>(*myGraph,
                                                    aStorage,
                                                    &aStorage.ChangeSolidRef(theSolidRef),
                                                    theSolidRef);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::ChildRef> BRepGraph::EditorView::GenOps::MutChildRef(
  const BRepGraph_ChildRefId theChildRef)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableRefId(aStorage, BRepGraph_RefId(theChildRef), *myGraph);
  Standard_ProgramError_Raise_if(aStorage.IsGuarded(BRepGraph_ItemId(theChildRef)),
                                 "BRepGraph::EditorView::Mut*(): guard already active");
  return BRepGraph_MutGuard<BRepGraphInc::ChildRef>(*myGraph,
                                                    aStorage,
                                                    &aStorage.ChangeChildRef(theChildRef),
                                                    theChildRef);
}

//==================================================================================================

BRepGraph_MutGuard<BRepGraphInc::OccurrenceRef> BRepGraph::EditorView::OccurrenceOps::MutRef(
  const BRepGraph_OccurrenceRefId theOccurrenceRef)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  validateMutableRefId(aStorage, BRepGraph_RefId(theOccurrenceRef), *myGraph);
  Standard_ProgramError_Raise_if(aStorage.IsGuarded(BRepGraph_ItemId(theOccurrenceRef)),
                                 "BRepGraph::EditorView::Mut*(): guard already active");
  return BRepGraph_MutGuard<BRepGraphInc::OccurrenceRef>(
    *myGraph,
    aStorage,
    &aStorage.ChangeOccurrenceRef(theOccurrenceRef),
    theOccurrenceRef);
}
