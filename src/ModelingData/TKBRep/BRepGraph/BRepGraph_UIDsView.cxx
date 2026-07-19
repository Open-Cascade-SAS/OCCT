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

#include <BRepGraph_UIDsView.hxx>
#include <BRepGraph_Data.hxx>

#include <shared_mutex>
#include <type_traits>

namespace
{

//=================================================================================================

BRepGraph_NodeId resolveRefParentNodeId(const BRepGraphInc_Storage& theStorage,
                                        const BRepGraph_RefId       theRefId)
{
  switch (theRefId.RefKind)
  {
    case BRepGraph_RefId::Kind::Shell: {
      const BRepGraph_ShellRefId aRefId(theRefId);
      if (!aRefId.IsValid(theStorage.NbShellRefs()))
      {
        return BRepGraph_NodeId();
      }
      return theStorage.ShellRef(aRefId).ParentSolidId;
    }
    case BRepGraph_RefId::Kind::Face: {
      const BRepGraph_FaceRefId aRefId(theRefId);
      if (!aRefId.IsValid(theStorage.NbFaceRefs()))
      {
        return BRepGraph_NodeId();
      }
      return theStorage.FaceRef(aRefId).ParentShellId;
    }
    case BRepGraph_RefId::Kind::Wire: {
      const BRepGraph_WireRefId aRefId(theRefId);
      if (!aRefId.IsValid(theStorage.NbWireRefs()))
      {
        return BRepGraph_NodeId();
      }
      return theStorage.WireRef(aRefId).ParentFaceId;
    }
    case BRepGraph_RefId::Kind::Solid: {
      const BRepGraph_SolidRefId aRefId(theRefId);
      if (!aRefId.IsValid(theStorage.NbSolidRefs()))
      {
        return BRepGraph_NodeId();
      }
      return theStorage.SolidRef(aRefId).ParentCompSolidId;
    }
    case BRepGraph_RefId::Kind::Child: {
      const BRepGraph_ChildRefId aRefId(theRefId);
      if (!aRefId.IsValid(theStorage.NbChildRefs()))
      {
        return BRepGraph_NodeId();
      }
      return theStorage.ChildRef(aRefId).ParentCompoundId;
    }
    case BRepGraph_RefId::Kind::Occurrence: {
      const BRepGraph_OccurrenceRefId aRefId(theRefId);
      if (!aRefId.IsValid(theStorage.NbOccurrenceRefs()))
      {
        return BRepGraph_NodeId();
      }
      return theStorage.OccurrenceRef(aRefId).ParentProductId;
    }
    case BRepGraph_RefId::Kind::Vertex: {
      const BRepGraph_VertexRefId aRefId(theRefId);
      if (!aRefId.IsValid(theStorage.NbVertexRefs()))
      {
        return BRepGraph_NodeId();
      }
      return theStorage.VertexRef(aRefId).ParentEdgeId;
    }
    default:
      break;
  }
  return BRepGraph_NodeId();
}

} // namespace

//=================================================================================================

BRepGraph_UID BRepGraph::UIDsView::Of(const BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
  {
    return BRepGraph_UID();
  }

  const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(theNode);
  if (aDef == nullptr || theNode.IsRemoved(*myGraph))
  {
    return BRepGraph_UID();
  }

  return BRepGraph_UID(theNode.NodeKind, aDef->UID);
}

//=================================================================================================

BRepGraph_RefUID BRepGraph::UIDsView::Of(const BRepGraph_RefId theRefId) const
{
  if (!theRefId.IsValid())
  {
    return BRepGraph_RefUID();
  }

  if (theRefId.IsRemoved(*myGraph))
  {
    return BRepGraph_RefUID();
  }

  const BRepGraphInc::BaseRef* aRef = myGraph->refEntity(theRefId);
  if (aRef == nullptr)
  {
    return BRepGraph_RefUID();
  }

  return BRepGraph_RefUID(theRefId.RefKind, aRef->UID);
}

//=================================================================================================

BRepGraph_ItemUID BRepGraph::UIDsView::Of(const BRepGraph_ItemId theItem) const
{
  if (!theItem.IsValid())
  {
    return BRepGraph_ItemUID();
  }

  switch (theItem.ItemDomain())
  {
    case BRepGraph_ItemId::Domain::Node: {
      const BRepGraph_UID aUID = Of(theItem.NodeId());
      return aUID.IsValid() ? BRepGraph_ItemUID::Node(theItem.NodeKind(), aUID.Counter)
                            : BRepGraph_ItemUID();
    }
    case BRepGraph_ItemId::Domain::Reference: {
      const BRepGraph_RefUID aUID = Of(theItem.RefId());
      return aUID.IsValid() ? BRepGraph_ItemUID::Reference(theItem.RefKind(), aUID.Counter)
                            : BRepGraph_ItemUID();
    }
    case BRepGraph_ItemId::Domain::None:
      return BRepGraph_ItemUID();
  }
  return BRepGraph_ItemUID();
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::UIDsView::NodeIdFrom(const BRepGraph_UID& theUID) const
{
  if (!theUID.IsValid())
  {
    return BRepGraph_NodeId();
  }

  myGraph->myData->myIncStorage.EnsureUIDReverseIndex();

  std::shared_lock<std::shared_mutex> aReadLock(myGraph->myData->myIncStorage.myUIDToNodeIdMutex);
  const BRepGraph_NodeId* aNodeId = myGraph->myData->myIncStorage.myUIDToNodeId.Seek(theUID);
  if (aNodeId == nullptr)
  {
    return BRepGraph_NodeId();
  }

  const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(*aNodeId);
  if (aDef == nullptr || aNodeId->IsRemoved(*myGraph))
  {
    return BRepGraph_NodeId();
  }

  return *aNodeId;
}

//=================================================================================================

BRepGraph_RefId BRepGraph::UIDsView::RefIdFrom(const BRepGraph_RefUID& theUID) const
{
  if (!theUID.IsValid())
  {
    return BRepGraph_RefId();
  }

  myGraph->myData->myIncStorage.EnsureRefUIDReverseIndex();

  std::shared_lock<std::shared_mutex> aReadLock(myGraph->myData->myIncStorage.myRefUIDToRefIdMutex);
  const BRepGraph_RefId* aRefId = myGraph->myData->myIncStorage.myRefUIDToRefId.Seek(theUID);
  if (aRefId == nullptr)
  {
    return BRepGraph_RefId();
  }

  if (aRefId->IsRemoved(*myGraph))
  {
    return BRepGraph_RefId();
  }

  return *aRefId;
}

//=================================================================================================

BRepGraph_ItemId BRepGraph::UIDsView::ItemIdFrom(const BRepGraph_ItemUID& theUID) const
{
  if (!theUID.IsValid())
  {
    return BRepGraph_ItemId();
  }

  switch (theUID.ItemDomain())
  {
    case BRepGraph_ItemUID::Domain::Node:
      return BRepGraph_ItemId(
        NodeIdFrom(BRepGraph_UID(theUID.NodeKind(), static_cast<uint32_t>(theUID.Counter()))));
    case BRepGraph_ItemUID::Domain::Reference:
      return BRepGraph_ItemId(
        RefIdFrom(BRepGraph_RefUID(theUID.RefKind(), static_cast<uint32_t>(theUID.Counter()))));
    case BRepGraph_ItemUID::Domain::None:
      return BRepGraph_ItemId();
  }
  return BRepGraph_ItemId();
}

//=================================================================================================

bool BRepGraph::UIDsView::Has(const BRepGraph_UID& theUID) const
{
  return NodeIdFrom(theUID).IsValid();
}

//=================================================================================================

bool BRepGraph::UIDsView::Has(const BRepGraph_RefUID& theUID) const
{
  return RefIdFrom(theUID).IsValid();
}

//=================================================================================================

bool BRepGraph::UIDsView::Has(const BRepGraph_ItemUID& theUID) const
{
  return ItemIdFrom(theUID).IsValid();
}

//=================================================================================================

uint32_t BRepGraph::UIDsView::Generation() const
{
  return myGraph->myData->myIncStorage.Generation();
}

//=================================================================================================

const Standard_GUID& BRepGraph::UIDsView::GraphGUID() const
{
  return myGraph->myData->myIncStorage.GraphGUID();
}

//=================================================================================================

BRepGraph_VersionStamp BRepGraph::UIDsView::StampOf(const BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
  {
    return BRepGraph_VersionStamp();
  }

  const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(theNode);
  if (aDef == nullptr || theNode.IsRemoved(*myGraph))
  {
    return BRepGraph_VersionStamp();
  }

  const BRepGraph_UID aUID(theNode.NodeKind, aDef->UID);
  if (!aUID.IsValid())
  {
    return BRepGraph_VersionStamp();
  }

  return BRepGraph_VersionStamp(aUID, aDef->OwnGen, myGraph->myData->myIncStorage.Generation());
}

//=================================================================================================

BRepGraph_VersionStamp BRepGraph::UIDsView::StampOf(const BRepGraph_RefId theRefId) const
{
  if (!theRefId.IsValid())
  {
    return BRepGraph_VersionStamp();
  }

  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  const BRepGraph_NodeId aParentNodeId = resolveRefParentNodeId(aStorage, theRefId);
  if (!aParentNodeId.IsValid() || theRefId.IsRemoved(*myGraph))
  {
    return BRepGraph_VersionStamp();
  }

  const BRepGraphInc::BaseDef* aParentDef = myGraph->topoEntity(aParentNodeId);
  if (aParentDef == nullptr || aParentNodeId.IsRemoved(*myGraph))
  {
    return BRepGraph_VersionStamp();
  }

  const BRepGraphInc::BaseRef* aRef = myGraph->refEntity(theRefId);
  if (aRef == nullptr)
  {
    return BRepGraph_VersionStamp();
  }

  const BRepGraph_RefUID aRefUID(theRefId.RefKind, aRef->UID);
  if (!aRefUID.IsValid())
  {
    return BRepGraph_VersionStamp();
  }

  return BRepGraph_VersionStamp(aRefUID, aParentDef->OwnGen, aStorage.Generation());
}

//=================================================================================================

BRepGraph_VersionStamp BRepGraph::UIDsView::StampOf(const BRepGraph_RepId theRepId) const
{
  if (!theRepId.IsValid())
  {
    return BRepGraph_VersionStamp();
  }

  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  BRepGraph_NodeId            aParentId;
  switch (theRepId.RepKind)
  {
    case BRepGraph_RepId::Kind::EdgeCurve3D: {
      const BRepGraph_EdgeCurve3DRepId aId(theRepId.Index);
      if (!aId.IsValid(aStorage.NbEdgeCurves3D()) || aStorage.IsRemoved(aId))
      {
        return BRepGraph_VersionStamp();
      }
      aParentId = BRepGraph_NodeId(aStorage.EdgeCurve3DRep(aId).ParentEdgeId);
      break;
    }
    case BRepGraph_RepId::Kind::EdgePolygon3D: {
      const BRepGraph_EdgePolygon3DRepId aId(theRepId.Index);
      if (!aId.IsValid(aStorage.NbEdgePolygons3D()) || aStorage.IsRemoved(aId))
      {
        return BRepGraph_VersionStamp();
      }
      aParentId = BRepGraph_NodeId(aStorage.EdgePolygon3DRep(aId).ParentEdgeId);
      break;
    }
    case BRepGraph_RepId::Kind::CoEdgeCurve2D: {
      const BRepGraph_CoEdgeCurve2DRepId aId(theRepId.Index);
      if (!aId.IsValid(aStorage.NbCoEdgeCurves2D()) || aStorage.IsRemoved(aId))
      {
        return BRepGraph_VersionStamp();
      }
      aParentId = BRepGraph_NodeId(aStorage.CoEdgeCurve2DRep(aId).ParentCoEdgeId);
      break;
    }
    case BRepGraph_RepId::Kind::CoEdgePolygon2D: {
      const BRepGraph_CoEdgePolygon2DRepId aId(theRepId.Index);
      if (!aId.IsValid(aStorage.NbCoEdgePolygons2D()) || aStorage.IsRemoved(aId))
      {
        return BRepGraph_VersionStamp();
      }
      aParentId = BRepGraph_NodeId(aStorage.CoEdgePolygon2DRep(aId).ParentCoEdgeId);
      break;
    }
    case BRepGraph_RepId::Kind::CoEdgePolygonOnTri: {
      const BRepGraph_CoEdgePolygonOnTriRepId aId(theRepId.Index);
      if (!aId.IsValid(aStorage.NbCoEdgePolygonsOnTri()) || aStorage.IsRemoved(aId))
      {
        return BRepGraph_VersionStamp();
      }
      aParentId = BRepGraph_NodeId(aStorage.CoEdgePolygonOnTriRep(aId).ParentCoEdgeId);
      break;
    }
    case BRepGraph_RepId::Kind::FaceSurface: {
      const BRepGraph_FaceSurfaceRepId aId(theRepId.Index);
      if (!aId.IsValid(aStorage.NbFaceSurfaces()) || aStorage.IsRemoved(aId))
      {
        return BRepGraph_VersionStamp();
      }
      aParentId = BRepGraph_NodeId(aStorage.FaceSurfaceRep(aId).ParentFaceId);
      break;
    }
    case BRepGraph_RepId::Kind::FaceTriangulation: {
      const BRepGraph_FaceTriangulationRepId aId(theRepId.Index);
      if (!aId.IsValid(aStorage.NbFaceTriangulations()) || aStorage.IsRemoved(aId))
      {
        return BRepGraph_VersionStamp();
      }
      aParentId = BRepGraph_NodeId(aStorage.FaceTriangulationRep(aId).ParentFaceId);
      break;
    }
    default:
      return BRepGraph_VersionStamp();
  }

  const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(aParentId);
  if (aDef == nullptr || aParentId.IsRemoved(*myGraph))
  {
    return BRepGraph_VersionStamp();
  }

  return BRepGraph_VersionStamp(myGraph->UIDs().Of(aParentId),
                                aDef->OwnGen,
                                myGraph->myData->myIncStorage.Generation());
}

//=================================================================================================

BRepGraph_VersionStamp BRepGraph::UIDsView::StampOf(const BRepGraph_ItemId theItem) const
{
  if (!theItem.IsValid())
  {
    return BRepGraph_VersionStamp();
  }

  switch (theItem.ItemDomain())
  {
    case BRepGraph_ItemId::Domain::Node:
      return StampOf(theItem.NodeId());
    case BRepGraph_ItemId::Domain::Reference:
      return StampOf(theItem.RefId());
    case BRepGraph_ItemId::Domain::None:
      return BRepGraph_VersionStamp();
  }
  return BRepGraph_VersionStamp();
}

//=================================================================================================

bool BRepGraph::UIDsView::IsStale(const BRepGraph_VersionStamp& theStamp) const
{
  if (!theStamp.IsValid())
  {
    return true;
  }

  if (theStamp.myGeneration != myGraph->myData->myIncStorage.Generation())
  {
    return true;
  }

  if (theStamp.IsNodeStamp())
  {
    const BRepGraph_NodeId aNodeId = NodeIdFrom(theStamp.myNodeUID);
    if (!aNodeId.IsValid())
    {
      return true;
    }

    const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(aNodeId);
    if (aDef == nullptr || aNodeId.IsRemoved(*myGraph))
    {
      return true;
    }

    return aDef->OwnGen != theStamp.myMutationGen;
  }

  if (theStamp.IsRefStamp())
  {
    const BRepGraph_RefId aRefId = RefIdFrom(theStamp.myRefUID);
    if (!aRefId.IsValid())
    {
      return true;
    }

    const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

    const BRepGraph_NodeId aParentNodeId = resolveRefParentNodeId(aStorage, aRefId);
    if (!aParentNodeId.IsValid() || aRefId.IsRemoved(*myGraph))
    {
      return true;
    }

    const BRepGraphInc::BaseDef* aParentDef = myGraph->topoEntity(aParentNodeId);
    if (aParentDef == nullptr || aParentNodeId.IsRemoved(*myGraph))
    {
      return true;
    }

    return aParentDef->OwnGen != theStamp.myMutationGen;
  }

  return true;
}
