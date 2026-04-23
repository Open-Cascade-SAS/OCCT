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

#include <BRepGraph_LayerParam.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepGraph_LayerParam, BRepGraph_Layer)

namespace
{

static const TCollection_AsciiString THE_LAYER_NAME("Param");

template <typename KeyT>
void appendUnique(NCollection_DataMap<KeyT, NCollection_Vector<BRepGraph_VertexId>>& theMap,
                  const KeyT                                                         theKey,
                  const BRepGraph_VertexId                                           theVertex)
{
  if (!theMap.IsBound(theKey))
  {
    NCollection_Vector<BRepGraph_VertexId> aVertices;
    aVertices.Append(theVertex);
    theMap.Bind(theKey, aVertices);
    return;
  }

  NCollection_Vector<BRepGraph_VertexId>& aVertices = theMap.ChangeFind(theKey);
  for (const BRepGraph_VertexId& aVtx : aVertices)
  {
    if (aVtx == theVertex)
      return;
  }
  aVertices.Append(theVertex);
}

template <typename KeyT>
void removeVertex(NCollection_DataMap<KeyT, NCollection_Vector<BRepGraph_VertexId>>& theMap,
                  const KeyT                                                         theKey,
                  const BRepGraph_VertexId theVertex) noexcept
{
  if (!theMap.IsBound(theKey))
    return;

  NCollection_Vector<BRepGraph_VertexId>& aVertices = theMap.ChangeFind(theKey);
  uint32_t anIdx = 0;
  for (NCollection_Vector<BRepGraph_VertexId>::Iterator anIt(aVertices); anIt.More();
       anIt.Next(), ++anIdx)
  {
    if (anIt.Value() != theVertex)
      continue;
    if (anIdx < static_cast<uint32_t>(aVertices.Size()) - 1u)
      aVertices.ChangeValue(static_cast<size_t>(anIdx)) = aVertices.Value(aVertices.Size() - 1u);
    aVertices.EraseLast();
    break;
  }

  if (aVertices.IsEmpty())
    theMap.UnBind(theKey);
}

static BRepGraph_VertexId remapVertex(
  const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap,
  const BRepGraph_VertexId                                       theVertex)
{
  const BRepGraph_NodeId* aNewId = theRemapMap.Seek(theVertex);
  if (aNewId == nullptr || aNewId->NodeKind != BRepGraph_NodeId::Kind::Vertex)
    return BRepGraph_VertexId();
  return BRepGraph_VertexId(*aNewId);
}

static BRepGraph_EdgeId remapEdge(
  const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap,
  const BRepGraph_EdgeId                                         theEdge)
{
  const BRepGraph_NodeId* aNewId = theRemapMap.Seek(theEdge);
  if (aNewId == nullptr || aNewId->NodeKind != BRepGraph_NodeId::Kind::Edge)
    return BRepGraph_EdgeId();
  return BRepGraph_EdgeId(*aNewId);
}

static BRepGraph_FaceId remapFace(
  const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap,
  const BRepGraph_FaceId                                         theFace)
{
  const BRepGraph_NodeId* aNewId = theRemapMap.Seek(theFace);
  if (aNewId == nullptr || aNewId->NodeKind != BRepGraph_NodeId::Kind::Face)
    return BRepGraph_FaceId();
  return BRepGraph_FaceId(*aNewId);
}

static BRepGraph_CoEdgeId remapCoEdge(
  const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap,
  const BRepGraph_CoEdgeId                                       theCoEdge)
{
  const BRepGraph_NodeId* aNewId = theRemapMap.Seek(theCoEdge);
  if (aNewId == nullptr || aNewId->NodeKind != BRepGraph_NodeId::Kind::CoEdge)
    return BRepGraph_CoEdgeId();
  return BRepGraph_CoEdgeId(*aNewId);
}

} // namespace

//=================================================================================================

const Standard_GUID& BRepGraph_LayerParam::GetID()
{
  static const Standard_GUID THE_LAYER_ID("2f9b6a5c-1f2d-4a88-9c1c-7a0c16a10001");
  return THE_LAYER_ID;
}

//=================================================================================================

const Standard_GUID& BRepGraph_LayerParam::ID() const
{
  return GetID();
}

//=================================================================================================

const TCollection_AsciiString& BRepGraph_LayerParam::Name() const
{
  return THE_LAYER_NAME;
}

//=================================================================================================

int BRepGraph_LayerParam::SubscribedKinds() const
{
  return KindBit(BRepGraph_NodeId::Kind::Vertex) | KindBit(BRepGraph_NodeId::Kind::Edge)
         | KindBit(BRepGraph_NodeId::Kind::Face) | KindBit(BRepGraph_NodeId::Kind::CoEdge);
}

//=================================================================================================

const BRepGraph_LayerParam::VertexParams* BRepGraph_LayerParam::FindVertexParams(
  const BRepGraph_VertexId theVertex) const
{
  return myVertexParams.Seek(theVertex);
}

//=================================================================================================

bool BRepGraph_LayerParam::FindPointOnCurve(const BRepGraph_VertexId theVertex,
                                            const BRepGraph_EdgeId   theEdge,
                                            double* const            theParameter) const
{
  const VertexParams* aParams = FindVertexParams(theVertex);
  if (aParams == nullptr)
    return false;

  for (const PointOnCurveEntry& anEntry : aParams->PointsOnCurve)
  {
    if (anEntry.EdgeDefId != theEdge)
      continue;
    if (theParameter != nullptr)
      *theParameter = anEntry.Parameter;
    return true;
  }
  return false;
}

//=================================================================================================

bool BRepGraph_LayerParam::FindPointOnSurface(const BRepGraph_VertexId theVertex,
                                              const BRepGraph_FaceId   theFace,
                                              gp_Pnt2d* const          theUV) const
{
  const VertexParams* aParams = FindVertexParams(theVertex);
  if (aParams == nullptr)
    return false;

  for (const PointOnSurfaceEntry& anEntry : aParams->PointsOnSurface)
  {
    if (anEntry.FaceDefId != theFace)
      continue;
    if (theUV != nullptr)
      *theUV = gp_Pnt2d(anEntry.ParameterU, anEntry.ParameterV);
    return true;
  }
  return false;
}

//=================================================================================================

bool BRepGraph_LayerParam::FindPointOnPCurve(const BRepGraph_VertexId theVertex,
                                             const BRepGraph_CoEdgeId theCoEdge,
                                             double* const            theParameter) const
{
  const VertexParams* aParams = FindVertexParams(theVertex);
  if (aParams == nullptr)
    return false;

  for (const PointOnPCurveEntry& anEntry : aParams->PointsOnPCurve)
  {
    if (anEntry.CoEdgeDefId != theCoEdge)
      continue;
    if (theParameter != nullptr)
      *theParameter = anEntry.Parameter;
    return true;
  }
  return false;
}

//=================================================================================================

uint32_t BRepGraph_LayerParam::NbPointsOnCurve(const BRepGraph_VertexId theVertex) const
{
  const VertexParams* aParams = FindVertexParams(theVertex);
  return aParams == nullptr ? 0u : static_cast<uint32_t>(aParams->PointsOnCurve.Size());
}

//=================================================================================================

uint32_t BRepGraph_LayerParam::NbPointsOnSurface(const BRepGraph_VertexId theVertex) const
{
  const VertexParams* aParams = FindVertexParams(theVertex);
  return aParams == nullptr ? 0u : static_cast<uint32_t>(aParams->PointsOnSurface.Size());
}

//=================================================================================================

uint32_t BRepGraph_LayerParam::NbPointsOnPCurve(const BRepGraph_VertexId theVertex) const
{
  const VertexParams* aParams = FindVertexParams(theVertex);
  return aParams == nullptr ? 0u : static_cast<uint32_t>(aParams->PointsOnPCurve.Size());
}

//=================================================================================================

BRepGraph_LayerParam::VertexParams& BRepGraph_LayerParam::changeVertexParams(
  const BRepGraph_VertexId theVertex)
{
  if (!myVertexParams.IsBound(theVertex))
    myVertexParams.Bind(theVertex, VertexParams());
  return myVertexParams.ChangeFind(theVertex);
}

//=================================================================================================

void BRepGraph_LayerParam::bindEdgeToVertex(const BRepGraph_EdgeId   theEdge,
                                            const BRepGraph_VertexId theVertex)
{
  appendUnique(myEdgeToVertices, theEdge, theVertex);
}

//=================================================================================================

void BRepGraph_LayerParam::bindFaceToVertex(const BRepGraph_FaceId   theFace,
                                            const BRepGraph_VertexId theVertex)
{
  appendUnique(myFaceToVertices, theFace, theVertex);
}

//=================================================================================================

void BRepGraph_LayerParam::bindCoEdgeToVertex(const BRepGraph_CoEdgeId theCoEdge,
                                              const BRepGraph_VertexId theVertex)
{
  appendUnique(myCoEdgeToVertices, theCoEdge, theVertex);
}

//=================================================================================================

void BRepGraph_LayerParam::unbindEdgeFromVertex(const BRepGraph_EdgeId   theEdge,
                                                const BRepGraph_VertexId theVertex) noexcept
{
  removeVertex(myEdgeToVertices, theEdge, theVertex);
}

//=================================================================================================

void BRepGraph_LayerParam::unbindFaceFromVertex(const BRepGraph_FaceId   theFace,
                                                const BRepGraph_VertexId theVertex) noexcept
{
  removeVertex(myFaceToVertices, theFace, theVertex);
}

//=================================================================================================

void BRepGraph_LayerParam::unbindCoEdgeFromVertex(const BRepGraph_CoEdgeId theCoEdge,
                                                  const BRepGraph_VertexId theVertex) noexcept
{
  removeVertex(myCoEdgeToVertices, theCoEdge, theVertex);
}

//=================================================================================================

void BRepGraph_LayerParam::SetPointOnCurve(const BRepGraph_VertexId theVertex,
                                           const BRepGraph_EdgeId   theEdge,
                                           const double             theParameter)
{
  VertexParams& aParams = changeVertexParams(theVertex);
  for (NCollection_Vector<PointOnCurveEntry>::Iterator anIt(aParams.PointsOnCurve); anIt.More();
       anIt.Next())
  {
    if (anIt.Value().EdgeDefId != theEdge)
      continue;
    anIt.ChangeValue().Parameter = theParameter;
    return;
  }

  PointOnCurveEntry& anEntry = aParams.PointsOnCurve.Appended();
  anEntry.Parameter          = theParameter;
  anEntry.EdgeDefId          = theEdge;
  bindEdgeToVertex(theEdge, theVertex);
}

//=================================================================================================

void BRepGraph_LayerParam::SetPointOnSurface(const BRepGraph_VertexId theVertex,
                                             const BRepGraph_FaceId   theFace,
                                             const double             theParameterU,
                                             const double             theParameterV)
{
  VertexParams& aParams = changeVertexParams(theVertex);
  for (NCollection_Vector<PointOnSurfaceEntry>::Iterator anIt(aParams.PointsOnSurface);
       anIt.More();
       anIt.Next())
  {
    if (anIt.Value().FaceDefId != theFace)
      continue;
    anIt.ChangeValue().ParameterU = theParameterU;
    anIt.ChangeValue().ParameterV = theParameterV;
    return;
  }

  PointOnSurfaceEntry& anEntry = aParams.PointsOnSurface.Appended();
  anEntry.ParameterU           = theParameterU;
  anEntry.ParameterV           = theParameterV;
  anEntry.FaceDefId            = theFace;
  bindFaceToVertex(theFace, theVertex);
}

//=================================================================================================

void BRepGraph_LayerParam::SetPointOnPCurve(const BRepGraph_VertexId theVertex,
                                            const BRepGraph_CoEdgeId theCoEdge,
                                            const double             theParameter)
{
  VertexParams& aParams = changeVertexParams(theVertex);
  for (NCollection_Vector<PointOnPCurveEntry>::Iterator anIt(aParams.PointsOnPCurve); anIt.More();
       anIt.Next())
  {
    if (anIt.Value().CoEdgeDefId != theCoEdge)
      continue;
    anIt.ChangeValue().Parameter = theParameter;
    return;
  }

  PointOnPCurveEntry& anEntry = aParams.PointsOnPCurve.Appended();
  anEntry.Parameter           = theParameter;
  anEntry.CoEdgeDefId         = theCoEdge;
  bindCoEdgeToVertex(theCoEdge, theVertex);
}

//=================================================================================================

void BRepGraph_LayerParam::removePointOnCurve(const BRepGraph_VertexId theVertex,
                                              const BRepGraph_EdgeId   theEdge) noexcept
{
  if (!myVertexParams.IsBound(theVertex))
    return;

  VertexParams& aParams  = myVertexParams.ChangeFind(theVertex);
  uint32_t      anIdx    = 0;
  for (NCollection_Vector<PointOnCurveEntry>::Iterator anIt(aParams.PointsOnCurve); anIt.More();
       anIt.Next(), ++anIdx)
  {
    if (anIt.Value().EdgeDefId != theEdge)
      continue;
    if (anIdx < static_cast<uint32_t>(aParams.PointsOnCurve.Size()) - 1u)
      aParams.PointsOnCurve.ChangeValue(static_cast<size_t>(anIdx)) =
        aParams.PointsOnCurve.Value(aParams.PointsOnCurve.Size() - 1u);
    aParams.PointsOnCurve.EraseLast();
    unbindEdgeFromVertex(theEdge, theVertex);
    break;
  }

  if (aParams.IsEmpty())
    myVertexParams.UnBind(theVertex);
}

//=================================================================================================

void BRepGraph_LayerParam::removePointOnSurface(const BRepGraph_VertexId theVertex,
                                                const BRepGraph_FaceId   theFace) noexcept
{
  if (!myVertexParams.IsBound(theVertex))
    return;

  VertexParams& aParams  = myVertexParams.ChangeFind(theVertex);
  uint32_t      anIdx    = 0;
  for (NCollection_Vector<PointOnSurfaceEntry>::Iterator anIt(aParams.PointsOnSurface);
       anIt.More();
       anIt.Next(), ++anIdx)
  {
    if (anIt.Value().FaceDefId != theFace)
      continue;
    if (anIdx < static_cast<uint32_t>(aParams.PointsOnSurface.Size()) - 1u)
      aParams.PointsOnSurface.ChangeValue(static_cast<size_t>(anIdx)) =
        aParams.PointsOnSurface.Value(aParams.PointsOnSurface.Size() - 1u);
    aParams.PointsOnSurface.EraseLast();
    unbindFaceFromVertex(theFace, theVertex);
    break;
  }

  if (aParams.IsEmpty())
    myVertexParams.UnBind(theVertex);
}

//=================================================================================================

void BRepGraph_LayerParam::removePointOnPCurve(const BRepGraph_VertexId theVertex,
                                               const BRepGraph_CoEdgeId theCoEdge) noexcept
{
  if (!myVertexParams.IsBound(theVertex))
    return;

  VertexParams& aParams  = myVertexParams.ChangeFind(theVertex);
  uint32_t      anIdx    = 0;
  for (NCollection_Vector<PointOnPCurveEntry>::Iterator anIt(aParams.PointsOnPCurve); anIt.More();
       anIt.Next(), ++anIdx)
  {
    if (anIt.Value().CoEdgeDefId != theCoEdge)
      continue;
    if (anIdx < static_cast<uint32_t>(aParams.PointsOnPCurve.Size()) - 1u)
      aParams.PointsOnPCurve.ChangeValue(static_cast<size_t>(anIdx)) =
        aParams.PointsOnPCurve.Value(aParams.PointsOnPCurve.Size() - 1u);
    aParams.PointsOnPCurve.EraseLast();
    unbindCoEdgeFromVertex(theCoEdge, theVertex);
    break;
  }

  if (aParams.IsEmpty())
    myVertexParams.UnBind(theVertex);
}

//=================================================================================================

void BRepGraph_LayerParam::removeVertexBindings(const BRepGraph_VertexId theVertex) noexcept
{
  const VertexParams* aParams = myVertexParams.Seek(theVertex);
  if (aParams == nullptr)
    return;

  for (const PointOnCurveEntry& anEntry : aParams->PointsOnCurve)
    unbindEdgeFromVertex(anEntry.EdgeDefId, theVertex);
  for (const PointOnSurfaceEntry& anEntry : aParams->PointsOnSurface)
    unbindFaceFromVertex(anEntry.FaceDefId, theVertex);
  for (const PointOnPCurveEntry& anEntry : aParams->PointsOnPCurve)
    unbindCoEdgeFromVertex(anEntry.CoEdgeDefId, theVertex);
  myVertexParams.UnBind(theVertex);
}

//=================================================================================================

void BRepGraph_LayerParam::invalidateEdgeBindings(const BRepGraph_EdgeId theEdge) noexcept
{
  const NCollection_Vector<BRepGraph_VertexId>* aVertices = myEdgeToVertices.Seek(theEdge);
  if (aVertices == nullptr)
    return;

  const NCollection_Vector<BRepGraph_VertexId> aBoundVertices = *aVertices;
  for (const BRepGraph_VertexId& aVtx : aBoundVertices)
    removePointOnCurve(aVtx, theEdge);
}

//=================================================================================================

void BRepGraph_LayerParam::invalidateFaceBindings(const BRepGraph_FaceId theFace) noexcept
{
  const NCollection_Vector<BRepGraph_VertexId>* aVertices = myFaceToVertices.Seek(theFace);
  if (aVertices == nullptr)
    return;

  const NCollection_Vector<BRepGraph_VertexId> aBoundVertices = *aVertices;
  for (const BRepGraph_VertexId& aVtx : aBoundVertices)
    removePointOnSurface(aVtx, theFace);
}

//=================================================================================================

void BRepGraph_LayerParam::invalidateCoEdgeBindings(const BRepGraph_CoEdgeId theCoEdge) noexcept
{
  const NCollection_Vector<BRepGraph_VertexId>* aVertices = myCoEdgeToVertices.Seek(theCoEdge);
  if (aVertices == nullptr)
    return;

  const NCollection_Vector<BRepGraph_VertexId> aBoundVertices = *aVertices;
  for (const BRepGraph_VertexId& aVtx : aBoundVertices)
    removePointOnPCurve(aVtx, theCoEdge);
}

//=================================================================================================

void BRepGraph_LayerParam::migrateVertexBindings(const BRepGraph_VertexId theOldVertex,
                                                 const BRepGraph_VertexId theNewVertex) noexcept
{
  const VertexParams* aParams = myVertexParams.Seek(theOldVertex);
  if (aParams == nullptr)
    return;

  const VertexParams aOldParams = *aParams;
  removeVertexBindings(theOldVertex);

  for (const PointOnCurveEntry& anEntry : aOldParams.PointsOnCurve)
  {
    SetPointOnCurve(theNewVertex, anEntry.EdgeDefId, anEntry.Parameter);
  }
  for (const PointOnSurfaceEntry& anEntry : aOldParams.PointsOnSurface)
  {
    SetPointOnSurface(theNewVertex, anEntry.FaceDefId, anEntry.ParameterU, anEntry.ParameterV);
  }
  for (const PointOnPCurveEntry& anEntry : aOldParams.PointsOnPCurve)
  {
    SetPointOnPCurve(theNewVertex, anEntry.CoEdgeDefId, anEntry.Parameter);
  }
}

//=================================================================================================

void BRepGraph_LayerParam::migrateEdgeBindings(const BRepGraph_EdgeId theOldEdge,
                                               const BRepGraph_EdgeId theNewEdge) noexcept
{
  const NCollection_Vector<BRepGraph_VertexId>* aVertices = myEdgeToVertices.Seek(theOldEdge);
  if (aVertices == nullptr)
    return;

  const NCollection_Vector<BRepGraph_VertexId> aBoundVertices = *aVertices;
  for (const BRepGraph_VertexId& aVtx : aBoundVertices)
  {
    double aParameter = 0.0;
    if (!FindPointOnCurve(aVtx, theOldEdge, &aParameter))
      continue;
    removePointOnCurve(aVtx, theOldEdge);
    SetPointOnCurve(aVtx, theNewEdge, aParameter);
  }
}

//=================================================================================================

void BRepGraph_LayerParam::migrateFaceBindings(const BRepGraph_FaceId theOldFace,
                                               const BRepGraph_FaceId theNewFace) noexcept
{
  const NCollection_Vector<BRepGraph_VertexId>* aVertices = myFaceToVertices.Seek(theOldFace);
  if (aVertices == nullptr)
    return;

  const NCollection_Vector<BRepGraph_VertexId> aBoundVertices = *aVertices;
  for (const BRepGraph_VertexId& aVtx : aBoundVertices)
  {
    gp_Pnt2d aUV;
    if (!FindPointOnSurface(aVtx, theOldFace, &aUV))
      continue;
    removePointOnSurface(aVtx, theOldFace);
    SetPointOnSurface(aVtx, theNewFace, aUV.X(), aUV.Y());
  }
}

//=================================================================================================

void BRepGraph_LayerParam::migrateCoEdgeBindings(const BRepGraph_CoEdgeId theOldCoEdge,
                                                 const BRepGraph_CoEdgeId theNewCoEdge) noexcept
{
  const NCollection_Vector<BRepGraph_VertexId>* aVertices = myCoEdgeToVertices.Seek(theOldCoEdge);
  if (aVertices == nullptr)
    return;

  const NCollection_Vector<BRepGraph_VertexId> aBoundVertices = *aVertices;
  for (const BRepGraph_VertexId& aVtx : aBoundVertices)
  {
    double aParameter = 0.0;
    if (!FindPointOnPCurve(aVtx, theOldCoEdge, &aParameter))
      continue;
    removePointOnPCurve(aVtx, theOldCoEdge);
    SetPointOnPCurve(aVtx, theNewCoEdge, aParameter);
  }
}

//=================================================================================================

void BRepGraph_LayerParam::OnNodeModified(const BRepGraph_NodeId theNode) noexcept
{
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      removeVertexBindings(BRepGraph_VertexId(theNode));
      break;
    case BRepGraph_NodeId::Kind::Edge:
      invalidateEdgeBindings(BRepGraph_EdgeId(theNode));
      break;
    case BRepGraph_NodeId::Kind::Face:
      invalidateFaceBindings(BRepGraph_FaceId(theNode));
      break;
    case BRepGraph_NodeId::Kind::CoEdge:
      invalidateCoEdgeBindings(BRepGraph_CoEdgeId(theNode));
      break;
    default:
      break;
  }
}

//=================================================================================================

void BRepGraph_LayerParam::OnNodesModified(
  const NCollection_Vector<BRepGraph_NodeId>& theModifiedNodes) noexcept
{
  for (const BRepGraph_NodeId& aModifiedNode : theModifiedNodes)
    OnNodeModified(aModifiedNode);
}

//=================================================================================================

void BRepGraph_LayerParam::OnNodeRemoved(const BRepGraph_NodeId theNode,
                                         const BRepGraph_NodeId theReplacement) noexcept
{
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      if (theReplacement.NodeKind == BRepGraph_NodeId::Kind::Vertex && theReplacement.IsValid())
        migrateVertexBindings(BRepGraph_VertexId(theNode), BRepGraph_VertexId(theReplacement));
      else
        removeVertexBindings(BRepGraph_VertexId(theNode));
      break;
    case BRepGraph_NodeId::Kind::Edge:
      if (theReplacement.NodeKind == BRepGraph_NodeId::Kind::Edge && theReplacement.IsValid())
        migrateEdgeBindings(BRepGraph_EdgeId(theNode), BRepGraph_EdgeId(theReplacement));
      else
        invalidateEdgeBindings(BRepGraph_EdgeId(theNode));
      break;
    case BRepGraph_NodeId::Kind::Face:
      if (theReplacement.NodeKind == BRepGraph_NodeId::Kind::Face && theReplacement.IsValid())
        migrateFaceBindings(BRepGraph_FaceId(theNode), BRepGraph_FaceId(theReplacement));
      else
        invalidateFaceBindings(BRepGraph_FaceId(theNode));
      break;
    case BRepGraph_NodeId::Kind::CoEdge:
      if (theReplacement.NodeKind == BRepGraph_NodeId::Kind::CoEdge && theReplacement.IsValid())
      {
        migrateCoEdgeBindings(BRepGraph_CoEdgeId(theNode), BRepGraph_CoEdgeId(theReplacement));
      }
      else
      {
        invalidateCoEdgeBindings(BRepGraph_CoEdgeId(theNode));
      }
      break;
    default:
      break;
  }
}

//=================================================================================================

void BRepGraph_LayerParam::OnCompact(
  const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap) noexcept
{
  NCollection_DataMap<BRepGraph_VertexId, VertexParams>                           aNewParams;
  NCollection_DataMap<BRepGraph_EdgeId, NCollection_Vector<BRepGraph_VertexId>>   aNewEdgeToVtx;
  NCollection_DataMap<BRepGraph_FaceId, NCollection_Vector<BRepGraph_VertexId>>   aNewFaceToVtx;
  NCollection_DataMap<BRepGraph_CoEdgeId, NCollection_Vector<BRepGraph_VertexId>> aNewCoEdgeToVtx;

  for (const auto& [aOldVertex, aOldParams] : myVertexParams.Items())
  {
    const BRepGraph_VertexId aNewVertex = remapVertex(theRemapMap, aOldVertex);
    if (!aNewVertex.IsValid())
      continue;

    VertexParams aNewVP;

    for (const PointOnCurveEntry& anOldEntry : aOldParams.PointsOnCurve)
    {
      const BRepGraph_EdgeId aNewEdge = remapEdge(theRemapMap, anOldEntry.EdgeDefId);
      if (!aNewEdge.IsValid())
        continue;
      PointOnCurveEntry& anEntry = aNewVP.PointsOnCurve.Appended();
      anEntry.Parameter          = anOldEntry.Parameter;
      anEntry.EdgeDefId          = aNewEdge;
      appendUnique(aNewEdgeToVtx, aNewEdge, aNewVertex);
    }
    for (const PointOnSurfaceEntry& anOldEntry : aOldParams.PointsOnSurface)
    {
      const BRepGraph_FaceId aNewFace = remapFace(theRemapMap, anOldEntry.FaceDefId);
      if (!aNewFace.IsValid())
        continue;
      PointOnSurfaceEntry& anEntry = aNewVP.PointsOnSurface.Appended();
      anEntry.ParameterU           = anOldEntry.ParameterU;
      anEntry.ParameterV           = anOldEntry.ParameterV;
      anEntry.FaceDefId            = aNewFace;
      appendUnique(aNewFaceToVtx, aNewFace, aNewVertex);
    }
    for (const PointOnPCurveEntry& anOldEntry : aOldParams.PointsOnPCurve)
    {
      const BRepGraph_CoEdgeId aNewCoEdge = remapCoEdge(theRemapMap, anOldEntry.CoEdgeDefId);
      if (!aNewCoEdge.IsValid())
        continue;
      PointOnPCurveEntry& anEntry = aNewVP.PointsOnPCurve.Appended();
      anEntry.Parameter           = anOldEntry.Parameter;
      anEntry.CoEdgeDefId         = aNewCoEdge;
      appendUnique(aNewCoEdgeToVtx, aNewCoEdge, aNewVertex);
    }

    if (!aNewVP.IsEmpty())
    {
      // Merge into existing entry if multiple old vertices remap to the same new vertex.
      VertexParams* anExisting = aNewParams.ChangeSeek(aNewVertex);
      if (anExisting != nullptr)
      {
        for (const PointOnCurveEntry& anEntry : aNewVP.PointsOnCurve)
          anExisting->PointsOnCurve.Append(anEntry);
        for (const PointOnSurfaceEntry& anEntry : aNewVP.PointsOnSurface)
          anExisting->PointsOnSurface.Append(anEntry);
        for (const PointOnPCurveEntry& anEntry : aNewVP.PointsOnPCurve)
          anExisting->PointsOnPCurve.Append(anEntry);
      }
      else
      {
        aNewParams.Bind(aNewVertex, std::move(aNewVP));
      }
    }
  }

  myVertexParams     = std::move(aNewParams);
  myEdgeToVertices   = std::move(aNewEdgeToVtx);
  myFaceToVertices   = std::move(aNewFaceToVtx);
  myCoEdgeToVertices = std::move(aNewCoEdgeToVtx);
}

//=================================================================================================

void BRepGraph_LayerParam::InvalidateAll() noexcept
{
  Clear();
}

//=================================================================================================

void BRepGraph_LayerParam::Clear() noexcept
{
  myVertexParams.Clear();
  myEdgeToVertices.Clear();
  myFaceToVertices.Clear();
  myCoEdgeToVertices.Clear();
}
