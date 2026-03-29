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

#include <BRepGraph_ParamLayer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepGraph_ParamLayer, BRepGraph_Layer)

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
  for (int anIdx = 0; anIdx < aVertices.Length(); ++anIdx)
  {
    if (aVertices.Value(anIdx) == theVertex)
      return;
  }
  aVertices.Append(theVertex);
}

template <typename KeyT>
void removeVertex(NCollection_DataMap<KeyT, NCollection_Vector<BRepGraph_VertexId>>& theMap,
                  const KeyT                                                         theKey,
                  const BRepGraph_VertexId                                           theVertex) noexcept
{
  if (!theMap.IsBound(theKey))
    return;

  NCollection_Vector<BRepGraph_VertexId>& aVertices = theMap.ChangeFind(theKey);
  for (int anIdx = 0; anIdx < aVertices.Length(); ++anIdx)
  {
    if (aVertices.Value(anIdx) != theVertex)
      continue;
    if (anIdx < aVertices.Length() - 1)
      aVertices.ChangeValue(anIdx) = aVertices.Value(aVertices.Length() - 1);
    aVertices.EraseLast();
    break;
  }

  if (aVertices.IsEmpty())
    theMap.UnBind(theKey);
}

static BRepGraph_VertexId remapVertex(const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap,
                                      const BRepGraph_VertexId                                        theVertex)
{
  const BRepGraph_NodeId* aNewId = theRemapMap.Seek(BRepGraph_NodeId::Vertex(theVertex.Index));
  if (aNewId == nullptr || aNewId->NodeKind != BRepGraph_NodeId::Kind::Vertex)
    return BRepGraph_VertexId();
  return BRepGraph_VertexId(aNewId->Index);
}

static BRepGraph_EdgeId remapEdge(const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap,
                                  const BRepGraph_EdgeId                                          theEdge)
{
  const BRepGraph_NodeId* aNewId = theRemapMap.Seek(BRepGraph_NodeId::Edge(theEdge.Index));
  if (aNewId == nullptr || aNewId->NodeKind != BRepGraph_NodeId::Kind::Edge)
    return BRepGraph_EdgeId();
  return BRepGraph_EdgeId(aNewId->Index);
}

static BRepGraph_FaceId remapFace(const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap,
                                  const BRepGraph_FaceId                                          theFace)
{
  const BRepGraph_NodeId* aNewId = theRemapMap.Seek(BRepGraph_NodeId::Face(theFace.Index));
  if (aNewId == nullptr || aNewId->NodeKind != BRepGraph_NodeId::Kind::Face)
    return BRepGraph_FaceId();
  return BRepGraph_FaceId(aNewId->Index);
}

static BRepGraph_CoEdgeId remapCoEdge(
  const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap,
  const BRepGraph_CoEdgeId                                        theCoEdge)
{
  const BRepGraph_NodeId* aNewId = theRemapMap.Seek(BRepGraph_NodeId::CoEdge(theCoEdge.Index));
  if (aNewId == nullptr || aNewId->NodeKind != BRepGraph_NodeId::Kind::CoEdge)
    return BRepGraph_CoEdgeId();
  return BRepGraph_CoEdgeId(aNewId->Index);
}

} // namespace

//=================================================================================================

const Standard_GUID& BRepGraph_ParamLayer::GetID()
{
  static const Standard_GUID THE_LAYER_ID("2f9b6a5c-1f2d-4a88-9c1c-7a0c16a10001");
  return THE_LAYER_ID;
}

//=================================================================================================

const Standard_GUID& BRepGraph_ParamLayer::ID() const
{
  return GetID();
}

//=================================================================================================

const TCollection_AsciiString& BRepGraph_ParamLayer::Name() const
{
  return THE_LAYER_NAME;
}

//=================================================================================================

int BRepGraph_ParamLayer::SubscribedKinds() const
{
  return KindBit(BRepGraph_NodeId::Kind::Vertex) | KindBit(BRepGraph_NodeId::Kind::Edge)
         | KindBit(BRepGraph_NodeId::Kind::Face) | KindBit(BRepGraph_NodeId::Kind::CoEdge);
}

//=================================================================================================

const BRepGraph_ParamLayer::VertexParams* BRepGraph_ParamLayer::FindVertexParams(
  const BRepGraph_VertexId theVertex) const
{
  return myVertexParams.Seek(theVertex);
}

//=================================================================================================

bool BRepGraph_ParamLayer::FindPointOnCurve(const BRepGraph_VertexId theVertex,
                                            const BRepGraph_EdgeId   theEdge,
                                            double* const            theParameter) const
{
  const VertexParams* aParams = FindVertexParams(theVertex);
  if (aParams == nullptr)
    return false;

  for (int anIdx = 0; anIdx < aParams->PointsOnCurve.Length(); ++anIdx)
  {
    const PointOnCurveEntry& anEntry = aParams->PointsOnCurve.Value(anIdx);
    if (anEntry.EdgeDefId != theEdge)
      continue;
    if (theParameter != nullptr)
      *theParameter = anEntry.Parameter;
    return true;
  }
  return false;
}

//=================================================================================================

bool BRepGraph_ParamLayer::FindPointOnSurface(const BRepGraph_VertexId theVertex,
                                              const BRepGraph_FaceId   theFace,
                                              gp_Pnt2d* const          theUV) const
{
  const VertexParams* aParams = FindVertexParams(theVertex);
  if (aParams == nullptr)
    return false;

  for (int anIdx = 0; anIdx < aParams->PointsOnSurface.Length(); ++anIdx)
  {
    const PointOnSurfaceEntry& anEntry = aParams->PointsOnSurface.Value(anIdx);
    if (anEntry.FaceDefId != theFace)
      continue;
    if (theUV != nullptr)
      *theUV = gp_Pnt2d(anEntry.ParameterU, anEntry.ParameterV);
    return true;
  }
  return false;
}

//=================================================================================================

bool BRepGraph_ParamLayer::FindPointOnPCurve(const BRepGraph_VertexId theVertex,
                                             const BRepGraph_CoEdgeId theCoEdge,
                                             double* const            theParameter) const
{
  const VertexParams* aParams = FindVertexParams(theVertex);
  if (aParams == nullptr)
    return false;

  for (int anIdx = 0; anIdx < aParams->PointsOnPCurve.Length(); ++anIdx)
  {
    const PointOnPCurveEntry& anEntry = aParams->PointsOnPCurve.Value(anIdx);
    if (anEntry.CoEdgeDefId != theCoEdge)
      continue;
    if (theParameter != nullptr)
      *theParameter = anEntry.Parameter;
    return true;
  }
  return false;
}

//=================================================================================================

int BRepGraph_ParamLayer::NbPointsOnCurve(const BRepGraph_VertexId theVertex) const
{
  const VertexParams* aParams = FindVertexParams(theVertex);
  return aParams == nullptr ? 0 : aParams->PointsOnCurve.Length();
}

//=================================================================================================

int BRepGraph_ParamLayer::NbPointsOnSurface(const BRepGraph_VertexId theVertex) const
{
  const VertexParams* aParams = FindVertexParams(theVertex);
  return aParams == nullptr ? 0 : aParams->PointsOnSurface.Length();
}

//=================================================================================================

int BRepGraph_ParamLayer::NbPointsOnPCurve(const BRepGraph_VertexId theVertex) const
{
  const VertexParams* aParams = FindVertexParams(theVertex);
  return aParams == nullptr ? 0 : aParams->PointsOnPCurve.Length();
}

//=================================================================================================

BRepGraph_ParamLayer::VertexParams& BRepGraph_ParamLayer::changeVertexParams(
  const BRepGraph_VertexId theVertex)
{
  if (!myVertexParams.IsBound(theVertex))
    myVertexParams.Bind(theVertex, VertexParams());
  return myVertexParams.ChangeFind(theVertex);
}

//=================================================================================================

void BRepGraph_ParamLayer::bindEdgeToVertex(const BRepGraph_EdgeId theEdge,
                                            const BRepGraph_VertexId theVertex)
{
  appendUnique(myEdgeToVertices, theEdge, theVertex);
}

//=================================================================================================

void BRepGraph_ParamLayer::bindFaceToVertex(const BRepGraph_FaceId theFace,
                                            const BRepGraph_VertexId theVertex)
{
  appendUnique(myFaceToVertices, theFace, theVertex);
}

//=================================================================================================

void BRepGraph_ParamLayer::bindCoEdgeToVertex(const BRepGraph_CoEdgeId theCoEdge,
                                              const BRepGraph_VertexId theVertex)
{
  appendUnique(myCoEdgeToVertices, theCoEdge, theVertex);
}

//=================================================================================================

void BRepGraph_ParamLayer::unbindEdgeFromVertex(const BRepGraph_EdgeId theEdge,
                                                const BRepGraph_VertexId theVertex) noexcept
{
  removeVertex(myEdgeToVertices, theEdge, theVertex);
}

//=================================================================================================

void BRepGraph_ParamLayer::unbindFaceFromVertex(const BRepGraph_FaceId theFace,
                                                const BRepGraph_VertexId theVertex) noexcept
{
  removeVertex(myFaceToVertices, theFace, theVertex);
}

//=================================================================================================

void BRepGraph_ParamLayer::unbindCoEdgeFromVertex(const BRepGraph_CoEdgeId theCoEdge,
                                                  const BRepGraph_VertexId theVertex) noexcept
{
  removeVertex(myCoEdgeToVertices, theCoEdge, theVertex);
}

//=================================================================================================

void BRepGraph_ParamLayer::SetPointOnCurve(const BRepGraph_VertexId theVertex,
                                           const BRepGraph_EdgeId   theEdge,
                                           const double             theParameter)
{
  VertexParams& aParams = changeVertexParams(theVertex);
  for (int anIdx = 0; anIdx < aParams.PointsOnCurve.Length(); ++anIdx)
  {
    PointOnCurveEntry& anEntry = aParams.PointsOnCurve.ChangeValue(anIdx);
    if (anEntry.EdgeDefId != theEdge)
      continue;
    anEntry.Parameter = theParameter;
    return;
  }

  PointOnCurveEntry& anEntry = aParams.PointsOnCurve.Appended();
  anEntry.Parameter          = theParameter;
  anEntry.EdgeDefId          = theEdge;
  bindEdgeToVertex(theEdge, theVertex);
}

//=================================================================================================

void BRepGraph_ParamLayer::SetPointOnSurface(const BRepGraph_VertexId theVertex,
                                             const BRepGraph_FaceId   theFace,
                                             const double             theParameterU,
                                             const double             theParameterV)
{
  VertexParams& aParams = changeVertexParams(theVertex);
  for (int anIdx = 0; anIdx < aParams.PointsOnSurface.Length(); ++anIdx)
  {
    PointOnSurfaceEntry& anEntry = aParams.PointsOnSurface.ChangeValue(anIdx);
    if (anEntry.FaceDefId != theFace)
      continue;
    anEntry.ParameterU = theParameterU;
    anEntry.ParameterV = theParameterV;
    return;
  }

  PointOnSurfaceEntry& anEntry = aParams.PointsOnSurface.Appended();
  anEntry.ParameterU          = theParameterU;
  anEntry.ParameterV          = theParameterV;
  anEntry.FaceDefId           = theFace;
  bindFaceToVertex(theFace, theVertex);
}

//=================================================================================================

void BRepGraph_ParamLayer::SetPointOnPCurve(const BRepGraph_VertexId theVertex,
                                            const BRepGraph_CoEdgeId theCoEdge,
                                            const double             theParameter)
{
  VertexParams& aParams = changeVertexParams(theVertex);
  for (int anIdx = 0; anIdx < aParams.PointsOnPCurve.Length(); ++anIdx)
  {
    PointOnPCurveEntry& anEntry = aParams.PointsOnPCurve.ChangeValue(anIdx);
    if (anEntry.CoEdgeDefId != theCoEdge)
      continue;
    anEntry.Parameter = theParameter;
    return;
  }

  PointOnPCurveEntry& anEntry = aParams.PointsOnPCurve.Appended();
  anEntry.Parameter          = theParameter;
  anEntry.CoEdgeDefId        = theCoEdge;
  bindCoEdgeToVertex(theCoEdge, theVertex);
}

//=================================================================================================

void BRepGraph_ParamLayer::removePointOnCurve(const BRepGraph_VertexId theVertex,
                                              const BRepGraph_EdgeId   theEdge) noexcept
{
  if (!myVertexParams.IsBound(theVertex))
    return;

  VertexParams& aParams = myVertexParams.ChangeFind(theVertex);
  for (int anIdx = 0; anIdx < aParams.PointsOnCurve.Length(); ++anIdx)
  {
    if (aParams.PointsOnCurve.Value(anIdx).EdgeDefId != theEdge)
      continue;
    if (anIdx < aParams.PointsOnCurve.Length() - 1)
      aParams.PointsOnCurve.ChangeValue(anIdx) = aParams.PointsOnCurve.Value(aParams.PointsOnCurve.Length() - 1);
    aParams.PointsOnCurve.EraseLast();
    unbindEdgeFromVertex(theEdge, theVertex);
    break;
  }

  if (aParams.IsEmpty())
    myVertexParams.UnBind(theVertex);
}

//=================================================================================================

void BRepGraph_ParamLayer::removePointOnSurface(const BRepGraph_VertexId theVertex,
                                                const BRepGraph_FaceId   theFace) noexcept
{
  if (!myVertexParams.IsBound(theVertex))
    return;

  VertexParams& aParams = myVertexParams.ChangeFind(theVertex);
  for (int anIdx = 0; anIdx < aParams.PointsOnSurface.Length(); ++anIdx)
  {
    if (aParams.PointsOnSurface.Value(anIdx).FaceDefId != theFace)
      continue;
    if (anIdx < aParams.PointsOnSurface.Length() - 1)
    {
      aParams.PointsOnSurface.ChangeValue(anIdx) =
        aParams.PointsOnSurface.Value(aParams.PointsOnSurface.Length() - 1);
    }
    aParams.PointsOnSurface.EraseLast();
    unbindFaceFromVertex(theFace, theVertex);
    break;
  }

  if (aParams.IsEmpty())
    myVertexParams.UnBind(theVertex);
}

//=================================================================================================

void BRepGraph_ParamLayer::removePointOnPCurve(const BRepGraph_VertexId theVertex,
                                               const BRepGraph_CoEdgeId theCoEdge) noexcept
{
  if (!myVertexParams.IsBound(theVertex))
    return;

  VertexParams& aParams = myVertexParams.ChangeFind(theVertex);
  for (int anIdx = 0; anIdx < aParams.PointsOnPCurve.Length(); ++anIdx)
  {
    if (aParams.PointsOnPCurve.Value(anIdx).CoEdgeDefId != theCoEdge)
      continue;
    if (anIdx < aParams.PointsOnPCurve.Length() - 1)
    {
      aParams.PointsOnPCurve.ChangeValue(anIdx) =
        aParams.PointsOnPCurve.Value(aParams.PointsOnPCurve.Length() - 1);
    }
    aParams.PointsOnPCurve.EraseLast();
    unbindCoEdgeFromVertex(theCoEdge, theVertex);
    break;
  }

  if (aParams.IsEmpty())
    myVertexParams.UnBind(theVertex);
}

//=================================================================================================

void BRepGraph_ParamLayer::removeVertexBindings(const BRepGraph_VertexId theVertex) noexcept
{
  const VertexParams* aParams = myVertexParams.Seek(theVertex);
  if (aParams == nullptr)
    return;

  for (int anIdx = 0; anIdx < aParams->PointsOnCurve.Length(); ++anIdx)
    unbindEdgeFromVertex(aParams->PointsOnCurve.Value(anIdx).EdgeDefId, theVertex);
  for (int anIdx = 0; anIdx < aParams->PointsOnSurface.Length(); ++anIdx)
    unbindFaceFromVertex(aParams->PointsOnSurface.Value(anIdx).FaceDefId, theVertex);
  for (int anIdx = 0; anIdx < aParams->PointsOnPCurve.Length(); ++anIdx)
    unbindCoEdgeFromVertex(aParams->PointsOnPCurve.Value(anIdx).CoEdgeDefId, theVertex);
  myVertexParams.UnBind(theVertex);
}

//=================================================================================================

void BRepGraph_ParamLayer::invalidateEdgeBindings(const BRepGraph_EdgeId theEdge) noexcept
{
  const NCollection_Vector<BRepGraph_VertexId>* aVertices = myEdgeToVertices.Seek(theEdge);
  if (aVertices == nullptr)
    return;

  const NCollection_Vector<BRepGraph_VertexId> aBoundVertices = *aVertices;
  for (int anIdx = 0; anIdx < aBoundVertices.Length(); ++anIdx)
    removePointOnCurve(aBoundVertices.Value(anIdx), theEdge);
}

//=================================================================================================

void BRepGraph_ParamLayer::invalidateFaceBindings(const BRepGraph_FaceId theFace) noexcept
{
  const NCollection_Vector<BRepGraph_VertexId>* aVertices = myFaceToVertices.Seek(theFace);
  if (aVertices == nullptr)
    return;

  const NCollection_Vector<BRepGraph_VertexId> aBoundVertices = *aVertices;
  for (int anIdx = 0; anIdx < aBoundVertices.Length(); ++anIdx)
    removePointOnSurface(aBoundVertices.Value(anIdx), theFace);
}

//=================================================================================================

void BRepGraph_ParamLayer::invalidateCoEdgeBindings(const BRepGraph_CoEdgeId theCoEdge) noexcept
{
  const NCollection_Vector<BRepGraph_VertexId>* aVertices = myCoEdgeToVertices.Seek(theCoEdge);
  if (aVertices == nullptr)
    return;

  const NCollection_Vector<BRepGraph_VertexId> aBoundVertices = *aVertices;
  for (int anIdx = 0; anIdx < aBoundVertices.Length(); ++anIdx)
    removePointOnPCurve(aBoundVertices.Value(anIdx), theCoEdge);
}

//=================================================================================================

void BRepGraph_ParamLayer::migrateVertexBindings(const BRepGraph_VertexId theOldVertex,
                                                 const BRepGraph_VertexId theNewVertex) noexcept
{
  const VertexParams* aParams = myVertexParams.Seek(theOldVertex);
  if (aParams == nullptr)
    return;

  const VertexParams aOldParams = *aParams;
  removeVertexBindings(theOldVertex);

  for (int anIdx = 0; anIdx < aOldParams.PointsOnCurve.Length(); ++anIdx)
  {
    const PointOnCurveEntry& anEntry = aOldParams.PointsOnCurve.Value(anIdx);
    SetPointOnCurve(theNewVertex, anEntry.EdgeDefId, anEntry.Parameter);
  }
  for (int anIdx = 0; anIdx < aOldParams.PointsOnSurface.Length(); ++anIdx)
  {
    const PointOnSurfaceEntry& anEntry = aOldParams.PointsOnSurface.Value(anIdx);
    SetPointOnSurface(theNewVertex, anEntry.FaceDefId, anEntry.ParameterU, anEntry.ParameterV);
  }
  for (int anIdx = 0; anIdx < aOldParams.PointsOnPCurve.Length(); ++anIdx)
  {
    const PointOnPCurveEntry& anEntry = aOldParams.PointsOnPCurve.Value(anIdx);
    SetPointOnPCurve(theNewVertex, anEntry.CoEdgeDefId, anEntry.Parameter);
  }
}

//=================================================================================================

void BRepGraph_ParamLayer::migrateEdgeBindings(const BRepGraph_EdgeId theOldEdge,
                                               const BRepGraph_EdgeId theNewEdge) noexcept
{
  const NCollection_Vector<BRepGraph_VertexId>* aVertices = myEdgeToVertices.Seek(theOldEdge);
  if (aVertices == nullptr)
    return;

  const NCollection_Vector<BRepGraph_VertexId> aBoundVertices = *aVertices;
  for (int anIdx = 0; anIdx < aBoundVertices.Length(); ++anIdx)
  {
    double aParameter = 0.0;
    if (!FindPointOnCurve(aBoundVertices.Value(anIdx), theOldEdge, &aParameter))
      continue;
    removePointOnCurve(aBoundVertices.Value(anIdx), theOldEdge);
    SetPointOnCurve(aBoundVertices.Value(anIdx), theNewEdge, aParameter);
  }
}

//=================================================================================================

void BRepGraph_ParamLayer::migrateFaceBindings(const BRepGraph_FaceId theOldFace,
                                               const BRepGraph_FaceId theNewFace) noexcept
{
  const NCollection_Vector<BRepGraph_VertexId>* aVertices = myFaceToVertices.Seek(theOldFace);
  if (aVertices == nullptr)
    return;

  const NCollection_Vector<BRepGraph_VertexId> aBoundVertices = *aVertices;
  for (int anIdx = 0; anIdx < aBoundVertices.Length(); ++anIdx)
  {
    gp_Pnt2d aUV;
    if (!FindPointOnSurface(aBoundVertices.Value(anIdx), theOldFace, &aUV))
      continue;
    removePointOnSurface(aBoundVertices.Value(anIdx), theOldFace);
    SetPointOnSurface(aBoundVertices.Value(anIdx), theNewFace, aUV.X(), aUV.Y());
  }
}

//=================================================================================================

void BRepGraph_ParamLayer::migrateCoEdgeBindings(const BRepGraph_CoEdgeId theOldCoEdge,
                                                 const BRepGraph_CoEdgeId theNewCoEdge) noexcept
{
  const NCollection_Vector<BRepGraph_VertexId>* aVertices = myCoEdgeToVertices.Seek(theOldCoEdge);
  if (aVertices == nullptr)
    return;

  const NCollection_Vector<BRepGraph_VertexId> aBoundVertices = *aVertices;
  for (int anIdx = 0; anIdx < aBoundVertices.Length(); ++anIdx)
  {
    double aParameter = 0.0;
    if (!FindPointOnPCurve(aBoundVertices.Value(anIdx), theOldCoEdge, &aParameter))
      continue;
    removePointOnPCurve(aBoundVertices.Value(anIdx), theOldCoEdge);
    SetPointOnPCurve(aBoundVertices.Value(anIdx), theNewCoEdge, aParameter);
  }
}

//=================================================================================================

void BRepGraph_ParamLayer::OnNodeModified(const BRepGraph_NodeId theNode) noexcept
{
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      removeVertexBindings(BRepGraph_VertexId(theNode.Index));
      break;
    case BRepGraph_NodeId::Kind::Edge:
      invalidateEdgeBindings(BRepGraph_EdgeId(theNode.Index));
      break;
    case BRepGraph_NodeId::Kind::Face:
      invalidateFaceBindings(BRepGraph_FaceId(theNode.Index));
      break;
    case BRepGraph_NodeId::Kind::CoEdge:
      invalidateCoEdgeBindings(BRepGraph_CoEdgeId(theNode.Index));
      break;
    default:
      break;
  }
}

//=================================================================================================

void BRepGraph_ParamLayer::OnNodesModified(
  const NCollection_Vector<BRepGraph_NodeId>& theModifiedNodes) noexcept
{
  for (int anIdx = 0; anIdx < theModifiedNodes.Length(); ++anIdx)
    OnNodeModified(theModifiedNodes.Value(anIdx));
}

//=================================================================================================

void BRepGraph_ParamLayer::OnNodeRemoved(const BRepGraph_NodeId theNode,
                                         const BRepGraph_NodeId theReplacement) noexcept
{
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      if (theReplacement.NodeKind == BRepGraph_NodeId::Kind::Vertex && theReplacement.IsValid())
        migrateVertexBindings(BRepGraph_VertexId(theNode.Index), BRepGraph_VertexId(theReplacement.Index));
      else
        removeVertexBindings(BRepGraph_VertexId(theNode.Index));
      break;
    case BRepGraph_NodeId::Kind::Edge:
      if (theReplacement.NodeKind == BRepGraph_NodeId::Kind::Edge && theReplacement.IsValid())
        migrateEdgeBindings(BRepGraph_EdgeId(theNode.Index), BRepGraph_EdgeId(theReplacement.Index));
      else
        invalidateEdgeBindings(BRepGraph_EdgeId(theNode.Index));
      break;
    case BRepGraph_NodeId::Kind::Face:
      if (theReplacement.NodeKind == BRepGraph_NodeId::Kind::Face && theReplacement.IsValid())
        migrateFaceBindings(BRepGraph_FaceId(theNode.Index), BRepGraph_FaceId(theReplacement.Index));
      else
        invalidateFaceBindings(BRepGraph_FaceId(theNode.Index));
      break;
    case BRepGraph_NodeId::Kind::CoEdge:
      if (theReplacement.NodeKind == BRepGraph_NodeId::Kind::CoEdge && theReplacement.IsValid())
      {
        migrateCoEdgeBindings(BRepGraph_CoEdgeId(theNode.Index),
                              BRepGraph_CoEdgeId(theReplacement.Index));
      }
      else
      {
        invalidateCoEdgeBindings(BRepGraph_CoEdgeId(theNode.Index));
      }
      break;
    default:
      break;
  }
}

//=================================================================================================

void BRepGraph_ParamLayer::OnCompact(
  const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap) noexcept
{
  NCollection_DataMap<BRepGraph_VertexId, VertexParams>                           aNewParams;
  NCollection_DataMap<BRepGraph_EdgeId, NCollection_Vector<BRepGraph_VertexId>>   aNewEdgeToVtx;
  NCollection_DataMap<BRepGraph_FaceId, NCollection_Vector<BRepGraph_VertexId>>   aNewFaceToVtx;
  NCollection_DataMap<BRepGraph_CoEdgeId, NCollection_Vector<BRepGraph_VertexId>> aNewCoEdgeToVtx;

  for (NCollection_DataMap<BRepGraph_VertexId, VertexParams>::Iterator anIter(myVertexParams);
       anIter.More();
       anIter.Next())
  {
    const BRepGraph_VertexId aNewVertex = remapVertex(theRemapMap, anIter.Key());
    if (!aNewVertex.IsValid())
      continue;

    const VertexParams& aOldParams = anIter.Value();
    VertexParams        aNewVP;

    for (int anIdx = 0; anIdx < aOldParams.PointsOnCurve.Length(); ++anIdx)
    {
      const PointOnCurveEntry& anOldEntry = aOldParams.PointsOnCurve.Value(anIdx);
      const BRepGraph_EdgeId   aNewEdge   = remapEdge(theRemapMap, anOldEntry.EdgeDefId);
      if (!aNewEdge.IsValid())
        continue;
      PointOnCurveEntry& anEntry = aNewVP.PointsOnCurve.Appended();
      anEntry.Parameter          = anOldEntry.Parameter;
      anEntry.EdgeDefId          = aNewEdge;
      appendUnique(aNewEdgeToVtx, aNewEdge, aNewVertex);
    }
    for (int anIdx = 0; anIdx < aOldParams.PointsOnSurface.Length(); ++anIdx)
    {
      const PointOnSurfaceEntry& anOldEntry = aOldParams.PointsOnSurface.Value(anIdx);
      const BRepGraph_FaceId     aNewFace   = remapFace(theRemapMap, anOldEntry.FaceDefId);
      if (!aNewFace.IsValid())
        continue;
      PointOnSurfaceEntry& anEntry = aNewVP.PointsOnSurface.Appended();
      anEntry.ParameterU           = anOldEntry.ParameterU;
      anEntry.ParameterV           = anOldEntry.ParameterV;
      anEntry.FaceDefId            = aNewFace;
      appendUnique(aNewFaceToVtx, aNewFace, aNewVertex);
    }
    for (int anIdx = 0; anIdx < aOldParams.PointsOnPCurve.Length(); ++anIdx)
    {
      const PointOnPCurveEntry& anOldEntry = aOldParams.PointsOnPCurve.Value(anIdx);
      const BRepGraph_CoEdgeId  aNewCoEdge = remapCoEdge(theRemapMap, anOldEntry.CoEdgeDefId);
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
        for (int i = 0; i < aNewVP.PointsOnCurve.Length(); ++i)
          anExisting->PointsOnCurve.Append(aNewVP.PointsOnCurve.Value(i));
        for (int i = 0; i < aNewVP.PointsOnSurface.Length(); ++i)
          anExisting->PointsOnSurface.Append(aNewVP.PointsOnSurface.Value(i));
        for (int i = 0; i < aNewVP.PointsOnPCurve.Length(); ++i)
          anExisting->PointsOnPCurve.Append(aNewVP.PointsOnPCurve.Value(i));
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

void BRepGraph_ParamLayer::InvalidateAll() noexcept
{
  Clear();
}

//=================================================================================================

void BRepGraph_ParamLayer::Clear() noexcept
{
  myVertexParams.Clear();
  myEdgeToVertices.Clear();
  myFaceToVertices.Clear();
  myCoEdgeToVertices.Clear();
}
