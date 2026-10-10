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

#include <BRepGraph_Revision.hxx>

#include <BRepGraph.hxx>
#include <BRepGraph_Transaction.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_LayerSupplementRegistry.hxx>
#include <BRepGraph_MeshView.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_RevisionComponent.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraph_Validate.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <BRepGraphSupInc_Store.hxx>
#include <BRepGraphSupInc_StoreRegistry.hxx>
#include <BRepGraphSupInc_Storage.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Failure.hxx>
#include <Standard_ProgramError.hxx>

#include <algorithm>
#include <atomic>
#include <cstring>
#include <functional>
#include <utility>

IMPLEMENT_STANDARD_RTTIEXT(BRepGraph_Revision, Standard_Transient)

struct BRepGraph_Revision::HashState
{
  BRepGraph_RevisionHash                                       Semantic;
  BRepGraph_RevisionHash                                       Storage;
  BRepGraph_RevisionHash                                       CoreSemantic;
  BRepGraph_RevisionHash                                       CoreStorage;
  std::shared_ptr<const BRepGraph_RevisionHash::Hasher::Index> Index;
};

namespace
{

int compareGUIDs(const Standard_GUID& theLeft, const Standard_GUID& theRight)
{
  const Standard_UUID aLeft  = theLeft.ToUUID();
  const Standard_UUID aRight = theRight.ToUUID();
  if (aLeft.Data1 != aRight.Data1)
  {
    return aLeft.Data1 < aRight.Data1 ? -1 : 1;
  }
  if (aLeft.Data2 != aRight.Data2)
  {
    return aLeft.Data2 < aRight.Data2 ? -1 : 1;
  }
  if (aLeft.Data3 != aRight.Data3)
  {
    return aLeft.Data3 < aRight.Data3 ? -1 : 1;
  }
  return std::memcmp(aLeft.Data4, aRight.Data4, sizeof(aLeft.Data4));
}

BRepGraph_RevisionHash combineComponentHashes(
  const BRepGraph_RevisionHash&                                             theCoreHash,
  const NCollection_LinearVector<occ::handle<BRepGraph_RevisionComponent>>& theComponents,
  const bool                                                                theToUseStorageHash)
{
  constexpr size_t THE_PREFIX_SIZE = sizeof(theCoreHash.Words) + sizeof(uint32_t);
  constexpr size_t THE_COMPONENT_SIZE =
    sizeof(uint32_t) + 16 + sizeof(uint32_t) + sizeof(BRepGraph_RevisionHash::Words);
  BRepGraph_RevisionHash::Hasher::ByteAccumulator anAccumulator(
    THE_PREFIX_SIZE + THE_COMPONENT_SIZE * theComponents.Size());

  const auto storeUInt32 = [](uint8_t*& thePosition, const uint32_t theValue) {
    for (int aByte = 0; aByte < 4; ++aByte)
    {
      *thePosition++ = static_cast<uint8_t>(theValue >> (aByte * 8));
    }
  };
  const auto storeUInt16 = [](uint8_t*& thePosition, const uint16_t theValue) {
    *thePosition++ = static_cast<uint8_t>(theValue);
    *thePosition++ = static_cast<uint8_t>(theValue >> 8);
  };
  const auto storeUInt64 = [](uint8_t*& thePosition, const uint64_t theValue) {
    for (int aByte = 0; aByte < 8; ++aByte)
    {
      *thePosition++ = static_cast<uint8_t>(theValue >> (aByte * 8));
    }
  };

  std::array<uint8_t, THE_PREFIX_SIZE> aPrefix;
  uint8_t*                             aPosition = aPrefix.data();
  for (const uint64_t aWord : theCoreHash.Words)
  {
    storeUInt64(aPosition, aWord);
  }
  storeUInt32(aPosition, static_cast<uint32_t>(theComponents.Size()));
  if (!anAccumulator.Append(aPrefix.data(), aPrefix.size()))
  {
    return BRepGraph_RevisionHash();
  }

  for (const occ::handle<BRepGraph_RevisionComponent>& aRevision : theComponents)
  {
    const BRepGraph_RevisionComponent::ComponentDescriptor& aDescriptor = aRevision->Descriptor();
    std::array<uint8_t, THE_COMPONENT_SIZE>                 aComponentBytes;
    aPosition = aComponentBytes.data();
    storeUInt32(aPosition, static_cast<uint32_t>(aDescriptor.ComponentDomain));
    const Standard_UUID aUUID = aDescriptor.StableGUID.ToUUID();
    storeUInt32(aPosition, aUUID.Data1);
    storeUInt16(aPosition, aUUID.Data2);
    storeUInt16(aPosition, aUUID.Data3);
    for (const uint8_t aByte : aUUID.Data4)
    {
      *aPosition++ = aByte;
    }
    storeUInt32(aPosition, aDescriptor.SchemaVersion);
    const BRepGraph_RevisionHash aHash =
      theToUseStorageHash ? aRevision->StorageHash() : aRevision->SemanticHash();
    for (const uint64_t aWord : aHash.Words)
    {
      storeUInt64(aPosition, aWord);
    }
    if (!anAccumulator.Append(aComponentBytes.data(), aComponentBytes.size()))
    {
      return BRepGraph_RevisionHash();
    }
  }
  return anAccumulator.Finish().value_or(BRepGraph_RevisionHash());
}

void appendDiagnostic(BRepGraph_RevisionStatus::Diagnostics& theDiagnostics,
                      const char*                            theCode,
                      const char*                            theMessage)
{
  BRepGraph_RevisionStatus::Diagnostic aDiagnostic;
  aDiagnostic.Code    = theCode;
  aDiagnostic.Message = theMessage;
  theDiagnostics.Append(aDiagnostic);
}

void appendValidationDiagnostics(const BRepGraph_Validate::Result&      theResult,
                                 BRepGraph_RevisionStatus::Diagnostics& theDiagnostics)
{
  for (const BRepGraph_Validate::Issue& anIssue : theResult.Issues)
  {
    BRepGraph_RevisionStatus::Diagnostic aDiagnostic;
    aDiagnostic.Severity = anIssue.Sev == BRepGraph_Validate::Severity::Error
                             ? BRepGraph_RevisionStatus::DiagnosticSeverity::Error
                             : BRepGraph_RevisionStatus::DiagnosticSeverity::Warning;
    aDiagnostic.Code     = "GraphValidation";
    aDiagnostic.Message  = anIssue.Description;
    theDiagnostics.Append(aDiagnostic);
  }
}

template <class TheChangeType>
bool hasStructuralChanges(const NCollection_LinearVector<TheChangeType>& theChanges)
{
  for (const TheChangeType& aChange : theChanges)
  {
    if (aChange.Kind != BRepGraph_Revision::VertexChange::Operation::Modify)
    {
      return true;
    }
  }
  return false;
}

template <typename TheId>
bool containsRelationId(const NCollection_LinearVector<TheId>& theIds, const TheId theId)
{
  for (const TheId anId : theIds)
  {
    if (anId == theId)
    {
      return true;
    }
  }
  return false;
}

bool hasOnlySparseTopology(const BRepGraph& theGraph)
{
  const BRepGraph::TopoView& aTopo = theGraph.Topo();
  if (aTopo.Gen().NbNodes()
      != aTopo.Vertices().Nb() + aTopo.Edges().Nb() + aTopo.CoEdges().Nb() + aTopo.Wires().Nb()
           + aTopo.Faces().Nb())
  {
    return false;
  }

  if (theGraph.Refs().Gen().Nb(BRepGraph_RefId::Kind::Shell) != 0
      || theGraph.Refs().Gen().Nb(BRepGraph_RefId::Kind::Face) != 0
      || theGraph.Refs().Gen().Nb(BRepGraph_RefId::Kind::Vertex) != 2u * aTopo.Edges().Nb()
      || theGraph.Refs().Gen().Nb(BRepGraph_RefId::Kind::Solid) != 0
      || theGraph.Refs().Gen().Nb(BRepGraph_RefId::Kind::Child) != 0
      || theGraph.Refs().Gen().Nb(BRepGraph_RefId::Kind::Occurrence) != 0)
  {
    return false;
  }

  if (aTopo.Geometry().NbCoEdgeCurves2D() != 0 || aTopo.Geometry().NbFaceSurfaces() != 0
      || theGraph.Mesh().Poly().NbEdgePolygons3D() != 0
      || theGraph.Mesh().Poly().NbCoEdgePolygons2D() != 0
      || theGraph.Mesh().Poly().NbCoEdgePolygonsOnTri() != 0
      || theGraph.Mesh().Poly().NbFaceTriangulations() != 0)
  {
    return false;
  }

  for (uint32_t anIndex = 0; anIndex < aTopo.Vertices().Nb(); ++anIndex)
  {
    const BRepGraph_VertexId aVertex(anIndex);
    if (aTopo.Gen().IsActive(aVertex) && theGraph.Shapes().HasOriginal(aVertex))
    {
      return false;
    }
  }
  for (uint32_t anIndex = 0; anIndex < aTopo.Edges().Nb(); ++anIndex)
  {
    const BRepGraph_EdgeId anEdge(anIndex);
    if (!aTopo.Gen().IsActive(anEdge))
    {
      continue;
    }
    if (theGraph.Shapes().HasOriginal(anEdge))
    {
      return false;
    }
    const BRepGraphInc::EdgeDef& aDefinition = aTopo.Edges().Definition(anEdge);
    if (!aDefinition.Curve3DRepId.IsValid() || aTopo.Edges().Curve3D(anEdge).IsNull())
    {
      return false;
    }
    if (!aDefinition.StartVertexRefId.IsValid() || !aDefinition.EndVertexRefId.IsValid()
        || !theGraph.Refs().Gen().IsActive(aDefinition.StartVertexRefId)
        || !theGraph.Refs().Gen().IsActive(aDefinition.EndVertexRefId))
    {
      return false;
    }
    const BRepGraphInc::VertexRef& aStart =
      theGraph.Refs().Vertices().Entry(aDefinition.StartVertexRefId);
    const BRepGraphInc::VertexRef& anEnd =
      theGraph.Refs().Vertices().Entry(aDefinition.EndVertexRefId);
    if (aStart.ParentEdgeId != anEdge || anEnd.ParentEdgeId != anEdge
        || !aStart.ChildVertexId.IsValid() || !anEnd.ChildVertexId.IsValid()
        || !aTopo.Gen().IsActive(aStart.ChildVertexId)
        || !aTopo.Gen().IsActive(anEnd.ChildVertexId))
    {
      return false;
    }
  }

  for (uint32_t anIndex = 0; anIndex < aTopo.CoEdges().Nb(); ++anIndex)
  {
    const BRepGraph_CoEdgeId aCoEdge(anIndex);
    if (!aTopo.Gen().IsActive(BRepGraph_NodeId(aCoEdge)))
    {
      continue;
    }
    if (theGraph.Shapes().HasOriginal(aCoEdge))
    {
      return false;
    }
    const BRepGraphInc::CoEdgeDef& aDefinition = aTopo.CoEdges().Definition(aCoEdge);
    if (aDefinition.FaceId.IsValid() || aDefinition.Curve2DRepId.IsValid()
        || aDefinition.Polygon2DRepId.IsValid() || aDefinition.PolygonOnTriRepId.IsValid()
        || !aDefinition.ChildEdgeId.IsValid(aTopo.Edges().Nb())
        || !aTopo.Gen().IsActive(BRepGraph_NodeId(aDefinition.ChildEdgeId)))
    {
      return false;
    }
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges =
      aTopo.Edges().Relations(aDefinition.ChildEdgeId).CoEdgeIds;
    bool isLinked = false;
    for (const BRepGraph_CoEdgeId aLinkedCoEdge : aCoEdges)
    {
      if (aLinkedCoEdge == aCoEdge)
      {
        isLinked = true;
        break;
      }
    }
    if (!isLinked)
    {
      return false;
    }
    if (aDefinition.ParentWireId.IsValid(aTopo.Wires().Nb()))
    {
      if (!aTopo.Gen().IsActive(aDefinition.ParentWireId))
      {
        return false;
      }
      const NCollection_LinearVector<BRepGraph_CoEdgeId>& aWireCoEdges =
        aTopo.Wires().Relations(aDefinition.ParentWireId).CoEdgeIds;
      if (!containsRelationId(aWireCoEdges, aCoEdge))
      {
        return false;
      }
    }
  }

  for (uint32_t anIndex = 0; anIndex < aTopo.Wires().Nb(); ++anIndex)
  {
    const BRepGraph_WireId aWire(anIndex);
    if (!aTopo.Gen().IsActive(aWire))
    {
      continue;
    }
    if (theGraph.Shapes().HasOriginal(aWire))
    {
      return false;
    }
    for (const BRepGraph_CoEdgeId aCoEdge : aTopo.Wires().Relations(aWire).CoEdgeIds)
    {
      if (!aCoEdge.IsValid(aTopo.CoEdges().Nb()) || !aTopo.Gen().IsActive(BRepGraph_NodeId(aCoEdge))
          || aTopo.CoEdges().Definition(aCoEdge).ParentWireId != aWire)
      {
        return false;
      }
    }
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges =
      aTopo.Wires().Relations(aWire).CoEdgeIds;
    for (size_t anOuter = 0; anOuter < aCoEdges.Size(); ++anOuter)
    {
      for (size_t anInner = anOuter + 1; anInner < aCoEdges.Size(); ++anInner)
      {
        if (aCoEdges.Value(anOuter) == aCoEdges.Value(anInner))
        {
          return false;
        }
      }
    }
  }

  for (uint32_t anIndex = 0; anIndex < aTopo.Faces().Nb(); ++anIndex)
  {
    const BRepGraph_FaceId aFace(anIndex);
    if (!aTopo.Gen().IsActive(aFace))
    {
      continue;
    }
    const BRepGraphInc::FaceDef& aDefinition = aTopo.Faces().Definition(aFace);
    if (theGraph.Shapes().HasOriginal(aFace) || aDefinition.SurfaceRepId.IsValid()
        || aDefinition.TriangulationRepId.IsValid())
    {
      return false;
    }
    for (const BRepGraph_WireRefId aWireRef : aTopo.Faces().Relations(aFace).WireRefIds)
    {
      if (!aWireRef.IsValid(theGraph.Refs().Wires().Nb())
          || !theGraph.Refs().Gen().IsActive(aWireRef))
      {
        return false;
      }
      const BRepGraphInc::WireRef& aRef = theGraph.Refs().Wires().Entry(aWireRef);
      if (aRef.ParentFaceId != aFace || !aRef.ChildWireId.IsValid(aTopo.Wires().Nb())
          || !aTopo.Gen().IsActive(aRef.ChildWireId))
      {
        return false;
      }
      if (!containsRelationId(aTopo.Wires().Relations(aRef.ChildWireId).ParentWireRefIds, aWireRef))
      {
        return false;
      }
    }
  }

  for (uint32_t anIndex = 0; anIndex < theGraph.Refs().Wires().Nb(); ++anIndex)
  {
    const BRepGraph_WireRefId aWireRef(anIndex);
    if (!theGraph.Refs().Gen().IsActive(aWireRef))
    {
      continue;
    }
    const BRepGraphInc::WireRef& aRef = theGraph.Refs().Wires().Entry(aWireRef);
    if (!aRef.ParentFaceId.IsValid(aTopo.Faces().Nb()) || !aTopo.Gen().IsActive(aRef.ParentFaceId)
        || !aRef.ChildWireId.IsValid(aTopo.Wires().Nb()) || !aTopo.Gen().IsActive(aRef.ChildWireId)
        || !containsRelationId(aTopo.Faces().Relations(aRef.ParentFaceId).WireRefIds, aWireRef))
    {
      return false;
    }
  }

  return true;
}

bool supportsSparseEdits(const BRepGraph& theGraph)
{
  return hasOnlySparseTopology(theGraph);
}

bool hasOnlyFreeCoEdgeValidationIssues(const BRepGraph&                  theGraph,
                                       const BRepGraph_Validate::Result& theResult)
{
  if (!hasOnlySparseTopology(theGraph))
  {
    return false;
  }

  bool hasError = false;
  for (const BRepGraph_Validate::Issue& anIssue : theResult.Issues)
  {
    if (anIssue.Sev != BRepGraph_Validate::Severity::Error)
    {
      continue;
    }
    hasError = true;
    if (anIssue.Code != BRepGraph_Validate::IssueCode::OrphanCoEdge)
    {
      return false;
    }
  }
  return hasError;
}

} // namespace

//=================================================================================================

BRepGraph_Revision::BRepGraph_Revision(
  std::shared_ptr<const BRepGraph>                                          theGraph,
  const uint32_t                                                            theSchemaVersion,
  const bool                                                                theSupportsSparseEdits,
  const NCollection_LinearVector<occ::handle<BRepGraph_RevisionComponent>>& theComponents,
  std::shared_ptr<const HashState>                                          theHashState)
    : myCoreGraph(std::move(theGraph)),
      mySchemaVersion(theSchemaVersion),
      mySupportsSparseEdits(theSupportsSparseEdits),
      myComponents(theComponents)
{
  for (uint32_t anIndex = 0; anIndex < myComponents.Size(); ++anIndex)
  {
    const occ::handle<BRepGraph_RevisionComponent>& aComponent = myComponents.Value(anIndex);
    if (!aComponent.IsNull())
    {
      myComponentIndices.Bind(aComponent->Descriptor().StableGUID, anIndex);
      ComponentState aState;
      aState.Descriptor   = aComponent->Descriptor();
      aState.SemanticHash = aComponent->SemanticHash();
      aState.StorageHash  = aComponent->StorageHash();
      myComponentStates.Append(std::move(aState));
    }
  }
  std::atomic_store(&myHashState, std::move(theHashState));
}

//=================================================================================================

std::shared_ptr<const BRepGraph_Revision::HashState> BRepGraph_Revision::hashStateIfAvailable()
  const noexcept
{
  return std::atomic_load(&myHashState);
}

//=================================================================================================

const BRepGraph_Revision::HashState& BRepGraph_Revision::ensureHashState() const
{
  std::call_once(myHashOnce, [this]() {
    if (hashStateIfAvailable() != nullptr || myCoreGraph == nullptr)
    {
      return;
    }

    const BRepGraph_RevisionHash::Hasher::Result aHashes =
      BRepGraph_RevisionHash::Hasher::Compute(*myCoreGraph);
    std::shared_ptr<HashState> aState = std::make_shared<HashState>();
    aState->CoreSemantic              = aHashes.Semantic;
    aState->CoreStorage               = aHashes.Storage;
    aState->Index                     = aHashes.HashIndex;
    aState->Semantic                  = myComponents.IsEmpty()
                                          ? aState->CoreSemantic
                                          : combineComponentHashes(aState->CoreSemantic, myComponents, false);
    aState->Storage                   = myComponents.IsEmpty()
                                          ? aState->CoreStorage
                                          : combineComponentHashes(aState->CoreStorage, myComponents, true);
    std::atomic_store(&myHashState, std::shared_ptr<const HashState>(std::move(aState)));
  });

  const std::shared_ptr<const HashState> aState = hashStateIfAvailable();
  if (aState == nullptr)
  {
    throw Standard_ProgramError("BRepGraph_Revision: cannot hash an invalid revision");
  }
  return *aState;
}

//=================================================================================================

bool BRepGraph_Revision::HasContentHashes() const noexcept
{
  return hashStateIfAvailable() != nullptr;
}

//=================================================================================================

const BRepGraph_RevisionHash& BRepGraph_Revision::SemanticHash() const
{
  return ensureHashState().Semantic;
}

//=================================================================================================

const BRepGraph_RevisionHash& BRepGraph_Revision::StorageRootHash() const
{
  return ensureHashState().Storage;
}

//=================================================================================================

const Standard_GUID& BRepGraph_Revision::GraphGUID() const
{
  Standard_ProgramError_Raise_if(myCoreGraph == nullptr,
                                 "BRepGraph_Revision: invalid revision has no graph GUID");
  return myCoreGraph->UIDs().GraphGUID();
}

//=================================================================================================

occ::handle<BRepGraph_RevisionComponent> BRepGraph_Revision::FindComponent(
  const Standard_GUID& theGUID) const
{
  const uint32_t* anIndex = myComponentIndices.Seek(theGUID);
  return anIndex == nullptr ? occ::handle<BRepGraph_RevisionComponent>()
                            : myComponents.Value(*anIndex);
}

//=================================================================================================

BRepGraph_RevisionHash BRepGraph_Revision::ComponentSemanticHash(const Standard_GUID& theGUID) const
{
  const uint32_t* anIndex = myComponentIndices.Seek(theGUID);
  return anIndex == nullptr ? BRepGraph_RevisionHash()
                            : myComponentStates.Value(*anIndex).SemanticHash;
}

//=================================================================================================

BRepGraph_Revision::CreateResult BRepGraph_Revision::Create(const BRepGraph& theGraph,
                                                            const Options&   theOptions)
{
  CreateResult aResult;
  if (!theGraph.IsValid())
  {
    appendDiagnostic(aResult.Diagnostics,
                     "InvalidGraph",
                     "Cannot create an immutable revision from an invalid graph");
    return aResult;
  }
  if (theOptions.SchemaVersion == 0 || theOptions.SchemaVersion > THE_SCHEMA_VERSION)
  {
    aResult.Status = BRepGraph_RevisionStatus::Code::UnsupportedComponent;
    appendDiagnostic(aResult.Diagnostics,
                     "SchemaVersion",
                     "Requested revision schema is not supported");
    return aResult;
  }
  if (theOptions.ValidateSource)
  {
    const BRepGraph_Validate::Result aValidation =
      BRepGraph_Validate::Perform(theGraph, BRepGraph_Validate::Mode::Audit);
    if (!aValidation.IsValid() && !hasOnlyFreeCoEdgeValidationIssues(theGraph, aValidation))
    {
      aResult.Status = BRepGraph_RevisionStatus::Code::ValidationFailed;
      appendValidationDiagnostics(aValidation, aResult.Diagnostics);
      return aResult;
    }
  }
  NCollection_LinearVector<occ::handle<BRepGraph_RevisionComponent>> aComponents;
  if (!CaptureComponents(theGraph, aComponents, aResult.Diagnostics))
  {
    aResult.Status = BRepGraph_RevisionStatus::Code::UnsupportedComponent;
    return aResult;
  }

  try
  {
    // This fork also rebuilds shape maps around detached TopoDS shapes.
    std::unique_ptr<BRepGraph> aCoreGraph(new BRepGraph(theGraph.incStorage(), true));

    const bool aSupportsSparseEdits = supportsSparseEdits(*aCoreGraph);

    std::shared_ptr<const BRepGraph> aSharedGraph(std::move(aCoreGraph));
    aResult.Revision = new BRepGraph_Revision(std::move(aSharedGraph),
                                              theOptions.SchemaVersion,
                                              aSupportsSparseEdits,
                                              aComponents);
    aResult.Status   = BRepGraph_RevisionStatus::Code::Ok;
    return aResult;
  }
  catch (const Standard_Failure& theFailure)
  {
    aResult.Status = BRepGraph_RevisionStatus::Code::CopyFailed;
    BRepGraph_RevisionStatus::Diagnostic aDiagnostic;
    aDiagnostic.Code    = "RevisionCopyConstruction";
    aDiagnostic.Message = theFailure.what();
    aResult.Diagnostics.Append(aDiagnostic);
  }
  return aResult;
}

//=================================================================================================

occ::handle<BRepGraph_Revision> BRepGraph_Revision::FromGraph(const BRepGraph& theGraph)
{
  return Create(theGraph).Revision;
}

//=================================================================================================

BRepGraph_Revision::CreateResult BRepGraph_Revision::FromTransactionGraph(
  std::unique_ptr<BRepGraph> theGraph,
  const uint32_t             theSchemaVersion)
{
  CreateResult aResult;
  if (theGraph == nullptr || !theGraph->IsValid() || theSchemaVersion == 0
      || theSchemaVersion > THE_SCHEMA_VERSION)
  {
    appendDiagnostic(aResult.Diagnostics,
                     "InvalidTransactionGraph",
                     "The transaction graph is invalid or uses an unsupported schema");
    return aResult;
  }

  const BRepGraph_Validate::Result aValidation =
    BRepGraph_Validate::Perform(*theGraph, BRepGraph_Validate::Mode::Audit);
  if (!aValidation.IsValid() && !hasOnlyFreeCoEdgeValidationIssues(*theGraph, aValidation))
  {
    aResult.Status = BRepGraph_RevisionStatus::Code::ValidationFailed;
    appendValidationDiagnostics(aValidation, aResult.Diagnostics);
    return aResult;
  }

  NCollection_LinearVector<occ::handle<BRepGraph_RevisionComponent>> aComponents;
  if (!CaptureComponents(*theGraph, aComponents, aResult.Diagnostics))
  {
    aResult.Status = BRepGraph_RevisionStatus::Code::UnsupportedComponent;
    return aResult;
  }

  // Never adopt the transaction's externally exposed working graph as immutable
  // revision storage. Graph() returns a raw pointer and representation handles obtained
  // from that graph may outlive Commit(); adopting it would let those aliases
  // mutate a committed revision without invalidating its cached hashes.
  std::unique_ptr<BRepGraph> aDetachedCore;
  try
  {
    aDetachedCore.reset(new BRepGraph(theGraph->incStorage(), true));
  }
  catch (const Standard_Failure& theFailure)
  {
    aResult.Status = BRepGraph_RevisionStatus::Code::CopyFailed;
    BRepGraph_RevisionStatus::Diagnostic aDiagnostic;
    aDiagnostic.Code    = "TransactionRepresentationIsolation";
    aDiagnostic.Message = theFailure.what();
    aResult.Diagnostics.Append(std::move(aDiagnostic));
    return aResult;
  }
  try
  {
    const bool                       aSupportsSparseEdits = supportsSparseEdits(*aDetachedCore);
    std::shared_ptr<const BRepGraph> aCoreGraph(std::move(aDetachedCore));
    aResult.Revision = new BRepGraph_Revision(std::move(aCoreGraph),
                                              theSchemaVersion,
                                              aSupportsSparseEdits,
                                              aComponents);
    aResult.Status   = BRepGraph_RevisionStatus::Code::Ok;
  }
  catch (const Standard_Failure& theFailure)
  {
    aResult.Revision.Nullify();
    aResult.Status = BRepGraph_RevisionStatus::Code::HashFailed;
    BRepGraph_RevisionStatus::Diagnostic aDiagnostic;
    aDiagnostic.Code    = "RevisionComponentHash";
    aDiagnostic.Message = theFailure.what();
    aResult.Diagnostics.Append(aDiagnostic);
  }
  return aResult;
}

//=================================================================================================

occ::handle<BRepGraph_Revision> BRepGraph_Revision::Empty()
{
  BRepGraph aGraph;
  return FromGraph(aGraph);
}

//=================================================================================================

const BRepGraph& BRepGraph_Revision::Graph() const
{
  const BRepGraph* aGraph = TryGraph();
  if (aGraph == nullptr)
  {
    throw Standard_ProgramError("BRepGraph_Revision::Graph(): graph copy unavailable");
  }
  return *aGraph;
}

//=================================================================================================

const BRepGraph* BRepGraph_Revision::TryGraph() const
{
  if (!IsValid())
  {
    return nullptr;
  }
  std::call_once(myGraphOnce, [this]() { myCachedGraph = copyGraph(); });
  return myCachedGraph.get();
}

//=================================================================================================

BRepGraph_Transaction BRepGraph_Revision::BeginTransaction() const
{
  occ::handle<BRepGraph_Revision> aRevision(const_cast<BRepGraph_Revision*>(this));
  return BRepGraph_Transaction::Begin(aRevision);
}

//=================================================================================================

bool BRepGraph_Revision::IsEmpty() const
{
  if (!IsValid())
  {
    return true;
  }
  return myCoreGraph->IsEmpty();
}

//=================================================================================================

uint32_t BRepGraph_Revision::NbVisibleVertices() const noexcept
{
  return myCoreGraph == nullptr ? 0 : myCoreGraph->incStorage().NbActiveVertices();
}

//=================================================================================================

uint32_t BRepGraph_Revision::NbVisibleEdges() const noexcept
{
  return myCoreGraph == nullptr ? 0 : myCoreGraph->incStorage().NbActiveEdges();
}

//=================================================================================================

uint32_t BRepGraph_Revision::NbVisibleVertexRefs() const noexcept
{
  return myCoreGraph == nullptr ? 0 : myCoreGraph->incStorage().NbActiveVertexRefs();
}

//=================================================================================================

uint32_t BRepGraph_Revision::NbVisibleCoEdges() const noexcept
{
  return myCoreGraph == nullptr ? 0 : myCoreGraph->incStorage().NbActiveCoEdges();
}

//=================================================================================================

uint32_t BRepGraph_Revision::NbVisibleWires() const noexcept
{
  return myCoreGraph == nullptr ? 0 : myCoreGraph->incStorage().NbActiveWires();
}

//=================================================================================================

uint32_t BRepGraph_Revision::NbVisibleFaces() const noexcept
{
  return myCoreGraph == nullptr ? 0 : myCoreGraph->incStorage().NbActiveFaces();
}

//=================================================================================================

uint32_t BRepGraph_Revision::NbVisibleWireRefs() const noexcept
{
  return myCoreGraph == nullptr ? 0 : myCoreGraph->incStorage().NbActiveWireRefs();
}

//=================================================================================================

bool BRepGraph_Revision::ResolveVertex(const BRepGraph_UID& theUID, VertexChange& theChange) const
{
  if (myCoreGraph == nullptr || theUID.Kind != BRepGraph_NodeId::Kind::Vertex)
  {
    return false;
  }
  const BRepGraph_NodeId aNode = myCoreGraph->UIDs().NodeIdFrom(theUID);
  return aNode.IsValid() && ResolveVertexLocal(aNode.Index, theChange);
}

//=================================================================================================

bool BRepGraph_Revision::ResolveVertexLocal(const uint32_t theIndex, VertexChange& theChange) const
{
  if (myCoreGraph == nullptr || theIndex >= myCoreGraph->Topo().Vertices().Nb())
  {
    return false;
  }
  const BRepGraph_VertexId aVertex(theIndex);
  theChange.Kind    = myCoreGraph->Topo().Gen().IsActive(aVertex) ? VertexChange::Operation::Create
                                                                  : VertexChange::Operation::Remove;
  theChange.LocalId = aVertex;
  theChange.Definition = myCoreGraph->Topo().Vertices().Definition(aVertex);
  theChange.UID        = BRepGraph_UID(BRepGraph_NodeId::Kind::Vertex, theChange.Definition.UID);
  return theChange.UID.IsValid();
}

//=================================================================================================

uint32_t BRepGraph_Revision::VertexCount() const noexcept
{
  return myCoreGraph->incStorage().NbVertices();
}

//=================================================================================================

uint32_t BRepGraph_Revision::NextVertexCounter() const noexcept
{
  return myCoreGraph->incStorage().NextNodeUIDCounter(BRepGraph_NodeId::Kind::Vertex);
}

//=================================================================================================

uint32_t BRepGraph_Revision::EdgeCount() const noexcept
{
  return myCoreGraph->incStorage().NbEdges();
}

//=================================================================================================

uint32_t BRepGraph_Revision::NextEdgeCounter() const noexcept
{
  return myCoreGraph->incStorage().NextNodeUIDCounter(BRepGraph_NodeId::Kind::Edge);
}

//=================================================================================================

uint32_t BRepGraph_Revision::VertexRefCount() const noexcept
{
  return myCoreGraph->incStorage().NbVertexRefs();
}

//=================================================================================================

uint32_t BRepGraph_Revision::NextVertexRefCounter() const noexcept
{
  return myCoreGraph->incStorage().NextRefUIDCounter(BRepGraph_RefId::Kind::Vertex);
}

//=================================================================================================

uint32_t BRepGraph_Revision::CoEdgeCount() const noexcept
{
  return myCoreGraph->incStorage().NbCoEdges();
}

//=================================================================================================

uint32_t BRepGraph_Revision::NextCoEdgeCounter() const noexcept
{
  return myCoreGraph->incStorage().NextNodeUIDCounter(BRepGraph_NodeId::Kind::CoEdge);
}

//=================================================================================================

uint32_t BRepGraph_Revision::WireCount() const noexcept
{
  return myCoreGraph->incStorage().NbWires();
}

//=================================================================================================

uint32_t BRepGraph_Revision::NextWireCounter() const noexcept
{
  return myCoreGraph->incStorage().NextNodeUIDCounter(BRepGraph_NodeId::Kind::Wire);
}

//=================================================================================================

uint32_t BRepGraph_Revision::FaceCount() const noexcept
{
  return myCoreGraph->incStorage().NbFaces();
}

//=================================================================================================

uint32_t BRepGraph_Revision::NextFaceCounter() const noexcept
{
  return myCoreGraph->incStorage().NextNodeUIDCounter(BRepGraph_NodeId::Kind::Face);
}

//=================================================================================================

uint32_t BRepGraph_Revision::WireRefCount() const noexcept
{
  return myCoreGraph->incStorage().NbWireRefs();
}

//=================================================================================================

uint32_t BRepGraph_Revision::NextWireRefCounter() const noexcept
{
  return myCoreGraph->incStorage().NextRefUIDCounter(BRepGraph_RefId::Kind::Wire);
}

//=================================================================================================

bool BRepGraph_Revision::ReadVertex(const BRepGraph_UID& theUID, VertexChange& theChange) const
{
  return ResolveVertex(theUID, theChange) && theChange.Kind != VertexChange::Operation::Remove;
}

//=================================================================================================

bool BRepGraph_Revision::ResolveEdge(const BRepGraph_UID& theUID, EdgeChange& theChange) const
{
  if (myCoreGraph == nullptr || theUID.Kind != BRepGraph_NodeId::Kind::Edge)
  {
    return false;
  }
  const BRepGraph_NodeId aNode = myCoreGraph->UIDs().NodeIdFrom(theUID);
  return aNode.IsValid() && ResolveEdgeLocal(aNode.Index, theChange);
}

//=================================================================================================

bool BRepGraph_Revision::ResolveEdgeLocal(const uint32_t theIndex, EdgeChange& theChange) const
{
  if (myCoreGraph == nullptr || theIndex >= myCoreGraph->Topo().Edges().Nb())
  {
    return false;
  }
  const BRepGraph_EdgeId anEdge(theIndex);
  theChange.Kind    = myCoreGraph->Topo().Gen().IsActive(anEdge) ? VertexChange::Operation::Create
                                                                 : VertexChange::Operation::Remove;
  theChange.LocalId = anEdge;
  theChange.Definition = myCoreGraph->Topo().Edges().Definition(anEdge);
  theChange.UID        = BRepGraph_UID(BRepGraph_NodeId::Kind::Edge, theChange.Definition.UID);
  if (theChange.Kind != VertexChange::Operation::Remove
      && theChange.Definition.Curve3DRepId.IsValid())
  {
    const BRepGraphInc::EdgeCurve3DRep& aCurve =
      myCoreGraph->incStorage().EdgeCurve3DRep(theChange.Definition.Curve3DRepId);
    theChange.Curve           = aCurve.Curve;
    theChange.CurveParamFirst = aCurve.ParamFirst;
    theChange.CurveParamLast  = aCurve.ParamLast;
  }
  return theChange.UID.IsValid();
}

//=================================================================================================

bool BRepGraph_Revision::ResolveVertexRef(const BRepGraph_RefUID& theUID,
                                          VertexRefChange&        theChange) const
{
  if (myCoreGraph == nullptr || theUID.Kind != BRepGraph_RefId::Kind::Vertex)
  {
    return false;
  }
  const BRepGraph_RefId aRef = myCoreGraph->UIDs().RefIdFrom(theUID);
  return aRef.IsValid() && ResolveVertexRefLocal(aRef.Index, theChange);
}

//=================================================================================================

bool BRepGraph_Revision::ResolveVertexRefLocal(const uint32_t   theIndex,
                                               VertexRefChange& theChange) const
{
  if (myCoreGraph == nullptr || theIndex >= myCoreGraph->Refs().Vertices().Nb())
  {
    return false;
  }
  const BRepGraph_VertexRefId aRef(theIndex);
  theChange.Kind       = myCoreGraph->Refs().Gen().IsActive(aRef) ? VertexChange::Operation::Create
                                                                  : VertexChange::Operation::Remove;
  theChange.LocalId    = aRef;
  theChange.Definition = myCoreGraph->Refs().Vertices().Entry(aRef);
  theChange.UID        = BRepGraph_RefUID(BRepGraph_RefId::Kind::Vertex, theChange.Definition.UID);
  return theChange.UID.IsValid();
}

//=================================================================================================

bool BRepGraph_Revision::ResolveCoEdge(const BRepGraph_UID& theUID, CoEdgeChange& theChange) const
{
  if (myCoreGraph == nullptr || theUID.Kind != BRepGraph_NodeId::Kind::CoEdge)
  {
    return false;
  }
  const BRepGraph_NodeId aNode = myCoreGraph->UIDs().NodeIdFrom(theUID);
  return aNode.IsValid() && ResolveCoEdgeLocal(aNode.Index, theChange);
}

//=================================================================================================

bool BRepGraph_Revision::ResolveCoEdgeLocal(const uint32_t theIndex, CoEdgeChange& theChange) const
{
  if (myCoreGraph == nullptr || theIndex >= myCoreGraph->Topo().CoEdges().Nb())
  {
    return false;
  }
  const BRepGraph_CoEdgeId aCoEdge(theIndex);
  theChange.Kind       = myCoreGraph->Topo().Gen().IsActive(BRepGraph_NodeId(aCoEdge))
                           ? VertexChange::Operation::Create
                           : VertexChange::Operation::Remove;
  theChange.LocalId    = aCoEdge;
  theChange.Definition = myCoreGraph->Topo().CoEdges().Definition(aCoEdge);
  theChange.UID        = BRepGraph_UID(BRepGraph_NodeId::Kind::CoEdge, theChange.Definition.UID);
  return theChange.UID.IsValid();
}

//=================================================================================================

bool BRepGraph_Revision::ResolveWire(const BRepGraph_UID& theUID, WireChange& theChange) const
{
  if (myCoreGraph == nullptr || theUID.Kind != BRepGraph_NodeId::Kind::Wire)
  {
    return false;
  }
  const BRepGraph_NodeId aNode = myCoreGraph->UIDs().NodeIdFrom(theUID);
  return aNode.IsValid() && ResolveWireLocal(aNode.Index, theChange);
}

//=================================================================================================

bool BRepGraph_Revision::ResolveWireLocal(const uint32_t theIndex, WireChange& theChange) const
{
  if (myCoreGraph == nullptr || theIndex >= myCoreGraph->Topo().Wires().Nb())
  {
    return false;
  }
  const BRepGraph_WireId aWire(theIndex);
  theChange.Kind       = myCoreGraph->Topo().Gen().IsActive(aWire) ? VertexChange::Operation::Create
                                                                   : VertexChange::Operation::Remove;
  theChange.LocalId    = aWire;
  theChange.Definition = myCoreGraph->Topo().Wires().Definition(aWire);
  theChange.UID        = BRepGraph_UID(BRepGraph_NodeId::Kind::Wire, theChange.Definition.UID);
  for (const BRepGraph_CoEdgeId aCoEdge : myCoreGraph->Topo().Wires().Relations(aWire).CoEdgeIds)
  {
    const BRepGraph_UID aUID = myCoreGraph->UIDs().Of(aCoEdge);
    if (aUID.IsValid())
    {
      theChange.CoEdgeUIDs.Append(aUID);
    }
  }
  return theChange.UID.IsValid();
}

//=================================================================================================

bool BRepGraph_Revision::ResolveFace(const BRepGraph_UID& theUID, FaceChange& theChange) const
{
  if (myCoreGraph == nullptr || theUID.Kind != BRepGraph_NodeId::Kind::Face)
  {
    return false;
  }
  const BRepGraph_NodeId aNode = myCoreGraph->UIDs().NodeIdFrom(theUID);
  return aNode.IsValid() && ResolveFaceLocal(aNode.Index, theChange);
}

//=================================================================================================

bool BRepGraph_Revision::ResolveFaceLocal(const uint32_t theIndex, FaceChange& theChange) const
{
  if (myCoreGraph == nullptr || theIndex >= myCoreGraph->Topo().Faces().Nb())
  {
    return false;
  }
  const BRepGraph_FaceId aFace(theIndex);
  theChange.Kind       = myCoreGraph->Topo().Gen().IsActive(aFace) ? VertexChange::Operation::Create
                                                                   : VertexChange::Operation::Remove;
  theChange.LocalId    = aFace;
  theChange.Definition = myCoreGraph->Topo().Faces().Definition(aFace);
  theChange.UID        = BRepGraph_UID(BRepGraph_NodeId::Kind::Face, theChange.Definition.UID);
  for (const BRepGraph_WireRefId aRef : myCoreGraph->Topo().Faces().Relations(aFace).WireRefIds)
  {
    const BRepGraph_RefUID aUID = myCoreGraph->UIDs().Of(aRef);
    if (aUID.IsValid())
    {
      theChange.WireRefUIDs.Append(aUID);
    }
  }
  return theChange.UID.IsValid();
}

//=================================================================================================

bool BRepGraph_Revision::ResolveWireRef(const BRepGraph_RefUID& theUID,
                                        WireRefChange&          theChange) const
{
  if (myCoreGraph == nullptr || theUID.Kind != BRepGraph_RefId::Kind::Wire)
  {
    return false;
  }
  const BRepGraph_RefId aRef = myCoreGraph->UIDs().RefIdFrom(theUID);
  return aRef.IsValid() && ResolveWireRefLocal(aRef.Index, theChange);
}

//=================================================================================================

bool BRepGraph_Revision::ResolveWireRefLocal(const uint32_t theIndex,
                                             WireRefChange& theChange) const
{
  if (myCoreGraph == nullptr || theIndex >= myCoreGraph->Refs().Wires().Nb())
  {
    return false;
  }
  const BRepGraph_WireRefId aRef(theIndex);
  theChange.Kind       = myCoreGraph->Refs().Gen().IsActive(aRef) ? VertexChange::Operation::Create
                                                                  : VertexChange::Operation::Remove;
  theChange.LocalId    = aRef;
  theChange.Definition = myCoreGraph->Refs().Wires().Entry(aRef);
  theChange.UID        = BRepGraph_RefUID(BRepGraph_RefId::Kind::Wire, theChange.Definition.UID);
  if (theChange.Definition.ParentFaceId.IsValid(myCoreGraph->Topo().Faces().Nb()))
  {
    const NCollection_LinearVector<BRepGraph_WireRefId>& aRefs =
      myCoreGraph->Topo().Faces().Relations(theChange.Definition.ParentFaceId).WireRefIds;
    theChange.Role =
      !aRefs.IsEmpty() && aRefs.First() == aRef ? BoundaryRole::Outer : BoundaryRole::Inner;
  }
  return theChange.UID.IsValid();
}

//=================================================================================================

bool BRepGraph_Revision::ReadEdge(const BRepGraph_UID& theUID, EdgeChange& theChange) const
{
  if (!ResolveEdge(theUID, theChange) || theChange.Kind == VertexChange::Operation::Remove)
  {
    return false;
  }
  if (!theChange.Curve.IsNull())
  {
    const occ::handle<Geom_Curve> aCurveCopy = occ::down_cast<Geom_Curve>(theChange.Curve->Copy());
    if (aCurveCopy.IsNull())
    {
      return false;
    }
    theChange.Curve = aCurveCopy;
  }
  return true;
}

//=================================================================================================

bool BRepGraph_Revision::ReadVertexRef(const BRepGraph_RefUID& theUID,
                                       VertexRefChange&        theChange) const
{
  return ResolveVertexRef(theUID, theChange) && theChange.Kind != VertexChange::Operation::Remove;
}

//=================================================================================================

bool BRepGraph_Revision::ReadCoEdge(const BRepGraph_UID& theUID, CoEdgeChange& theChange) const
{
  return ResolveCoEdge(theUID, theChange) && theChange.Kind != VertexChange::Operation::Remove;
}

//=================================================================================================

bool BRepGraph_Revision::ReadWire(const BRepGraph_UID& theUID, WireChange& theChange) const
{
  return ResolveWire(theUID, theChange) && theChange.Kind != VertexChange::Operation::Remove;
}

//=================================================================================================

bool BRepGraph_Revision::ReadFace(const BRepGraph_UID& theUID, FaceChange& theChange) const
{
  return ResolveFace(theUID, theChange) && theChange.Kind != VertexChange::Operation::Remove;
}

//=================================================================================================

bool BRepGraph_Revision::ReadWireRef(const BRepGraph_RefUID& theUID, WireRefChange& theChange) const
{
  return ResolveWireRef(theUID, theChange) && theChange.Kind != VertexChange::Operation::Remove;
}

//=================================================================================================

occ::handle<BRepGraph_Revision> BRepGraph_Revision::FromNativeChanges(
  const occ::handle<BRepGraph_Revision>&                                    theBaseRevision,
  const NCollection_LinearVector<VertexChange>&                             theChanges,
  const NCollection_LinearVector<EdgeChange>&                               theEdgeChanges,
  const NCollection_LinearVector<VertexRefChange>&                          theVertexRefChanges,
  const NCollection_LinearVector<CoEdgeChange>&                             theCoEdgeChanges,
  const NCollection_LinearVector<WireChange>&                               theWireChanges,
  const NCollection_LinearVector<FaceChange>&                               theFaceChanges,
  const NCollection_LinearVector<WireRefChange>&                            theWireRefChanges,
  const uint32_t                                                            theNextVertexCounter,
  const uint32_t                                                            theNextEdgeCounter,
  const uint32_t                                                            theNextVertexRefCounter,
  const uint32_t                                                            theNextCoEdgeCounter,
  const uint32_t                                                            theNextWireCounter,
  const uint32_t                                                            theNextFaceCounter,
  const uint32_t                                                            theNextWireRefCounter,
  const NCollection_LinearVector<occ::handle<BRepGraph_RevisionComponent>>& theComponents,
  BRepGraph_RevisionStatus::Diagnostics&                                    theDiagnostics)
{
  if (theBaseRevision.IsNull() || !theBaseRevision->SupportsSparseEdits())
  {
    appendDiagnostic(theDiagnostics,
                     "InvalidNativeBase",
                     "The base revision cannot accept sparse topology changes");
    return occ::handle<BRepGraph_Revision>();
  }

  try
  {
    std::unique_ptr<BRepGraph> aResultGraph(
      new BRepGraph(theBaseRevision->CoreGraph().incStorage(), false));
    const bool hasStructuralChange =
      hasStructuralChanges(theChanges) || hasStructuralChanges(theEdgeChanges)
      || hasStructuralChanges(theVertexRefChanges) || hasStructuralChanges(theCoEdgeChanges)
      || hasStructuralChanges(theWireChanges) || hasStructuralChanges(theFaceChanges)
      || hasStructuralChanges(theWireRefChanges);
    // A valid immutable base remains relation-valid after modification-only sparse edits:
    // these APIs change values, representations, orientations, or the order of an unchanged
    // membership set. Creations and removals still require the complete relation audit.
    if (!ApplyCoreChanges(*aResultGraph,
                          theChanges,
                          theEdgeChanges,
                          theVertexRefChanges,
                          theCoEdgeChanges,
                          theWireChanges,
                          theFaceChanges,
                          theWireRefChanges,
                          theNextVertexCounter,
                          theNextEdgeCounter,
                          theNextVertexRefCounter,
                          theNextCoEdgeCounter,
                          theNextWireCounter,
                          theNextFaceCounter,
                          theNextWireRefCounter)
        || (hasStructuralChange && !aResultGraph->ValidateRelations()))
    {
      appendDiagnostic(theDiagnostics,
                       "RevisionUpdate",
                       "The updated graph is incomplete or has invalid relations");
      return occ::handle<BRepGraph_Revision>();
    }

    for (const occ::handle<BRepGraph_RevisionComponent>& aComponent : theComponents)
    {
      if (aComponent.IsNull() || !aComponent->Validate(*aResultGraph, theDiagnostics))
      {
        appendDiagnostic(theDiagnostics,
                         "NativeComponentValidation",
                         "A persistent component is invalid for the updated graph");
        return occ::handle<BRepGraph_Revision>();
      }
    }

    std::shared_ptr<const HashState>       aHashState;
    const std::shared_ptr<const HashState> aBaseHashState = theBaseRevision->hashStateIfAvailable();
    if (aBaseHashState != nullptr && aBaseHashState->Index != nullptr)
    {
      NCollection_LinearVector<BRepGraph_UID>    aChangedNodes;
      NCollection_LinearVector<BRepGraph_RefUID> aChangedRefs;
      for (const VertexChange& aChange : theChanges)
        aChangedNodes.Append(aChange.UID);
      for (const EdgeChange& aChange : theEdgeChanges)
        aChangedNodes.Append(aChange.UID);
      for (const CoEdgeChange& aChange : theCoEdgeChanges)
        aChangedNodes.Append(aChange.UID);
      for (const WireChange& aChange : theWireChanges)
        aChangedNodes.Append(aChange.UID);
      for (const FaceChange& aChange : theFaceChanges)
        aChangedNodes.Append(aChange.UID);
      for (const VertexRefChange& aChange : theVertexRefChanges)
        aChangedRefs.Append(aChange.UID);
      for (const WireRefChange& aChange : theWireRefChanges)
        aChangedRefs.Append(aChange.UID);

      const BRepGraph_RevisionHash::Hasher::Result aHashes =
        BRepGraph_RevisionHash::Hasher::Update(aBaseHashState->Index,
                                               theBaseRevision->CoreGraph(),
                                               *aResultGraph,
                                               aChangedNodes,
                                               aChangedRefs);
      if (aHashes.HashIndex != nullptr)
      {
        std::shared_ptr<HashState> aNewHashState = std::make_shared<HashState>();
        aNewHashState->CoreSemantic              = aHashes.Semantic;
        aNewHashState->CoreStorage               = aHashes.Storage;
        aNewHashState->Index                     = aHashes.HashIndex;
        aNewHashState->Semantic =
          theComponents.IsEmpty() ? aHashes.Semantic
                                  : combineComponentHashes(aHashes.Semantic, theComponents, false);
        aNewHashState->Storage = theComponents.IsEmpty()
                                   ? aHashes.Storage
                                   : combineComponentHashes(aHashes.Storage, theComponents, true);
        aHashState             = std::move(aNewHashState);
      }
    }

    std::shared_ptr<const BRepGraph> aCoreGraph(std::move(aResultGraph));
    return new BRepGraph_Revision(std::move(aCoreGraph),
                                  theBaseRevision->SchemaVersion(),
                                  true,
                                  theComponents,
                                  std::move(aHashState));
  }
  catch (const Standard_Failure& theFailure)
  {
    BRepGraph_RevisionStatus::Diagnostic aDiagnostic;
    aDiagnostic.Code    = "RevisionUpdateConstruction";
    aDiagnostic.Message = theFailure.what();
    theDiagnostics.Append(aDiagnostic);
  }
  return occ::handle<BRepGraph_Revision>();
}

//=================================================================================================

occ::handle<BRepGraph_Revision> BRepGraph_Revision::FromComponents(
  const occ::handle<BRepGraph_Revision>&                                    theBaseRevision,
  const NCollection_LinearVector<occ::handle<BRepGraph_RevisionComponent>>& theComponents,
  BRepGraph_RevisionStatus::Diagnostics&                                    theDiagnostics)
{
  if (theBaseRevision.IsNull() || !theBaseRevision->IsValid())
  {
    appendDiagnostic(theDiagnostics,
                     "InvalidComponentBase",
                     "Components require a valid immutable base revision");
    return occ::handle<BRepGraph_Revision>();
  }
  const BRepGraph& aCoreGraph = theBaseRevision->CoreGraph();
  for (const occ::handle<BRepGraph_RevisionComponent>& aComponent : theComponents)
  {
    if (aComponent.IsNull() || !aComponent->Validate(aCoreGraph, theDiagnostics))
    {
      appendDiagnostic(theDiagnostics,
                       "ComponentValidation",
                       "A persistent component is invalid for the retained core graph");
      return occ::handle<BRepGraph_Revision>();
    }
  }
  try
  {
    const std::shared_ptr<const HashState> aBaseHashState = theBaseRevision->hashStateIfAvailable();
    std::shared_ptr<const HashState>       aHashState;
    if (aBaseHashState != nullptr)
    {
      std::shared_ptr<HashState> aNewHashState = std::make_shared<HashState>(*aBaseHashState);
      aNewHashState->Semantic =
        theComponents.IsEmpty()
          ? aNewHashState->CoreSemantic
          : combineComponentHashes(aNewHashState->CoreSemantic, theComponents, false);
      aNewHashState->Storage =
        theComponents.IsEmpty()
          ? aNewHashState->CoreStorage
          : combineComponentHashes(aNewHashState->CoreStorage, theComponents, true);
      aHashState = std::move(aNewHashState);
    }
    return new BRepGraph_Revision(theBaseRevision->myCoreGraph,
                                  theBaseRevision->SchemaVersion(),
                                  theBaseRevision->SupportsSparseEdits(),
                                  theComponents,
                                  std::move(aHashState));
  }
  catch (const Standard_Failure& theFailure)
  {
    BRepGraph_RevisionStatus::Diagnostic aDiagnostic;
    aDiagnostic.Code    = "ComponentHashConstruction";
    aDiagnostic.Message = theFailure.what();
    theDiagnostics.Append(aDiagnostic);
  }
  return occ::handle<BRepGraph_Revision>();
}

//=================================================================================================

occ::handle<BRepGraph_Revision> BRepGraph_Revision::FromCoreGraph(
  std::shared_ptr<const BRepGraph> theGraph,
  const uint32_t                   theSchemaVersion)
{
  if (theGraph == nullptr || theSchemaVersion == 0 || theSchemaVersion > THE_SCHEMA_VERSION)
  {
    return occ::handle<BRepGraph_Revision>();
  }
  const bool aSupportsSparseEdits = supportsSparseEdits(*theGraph);
  try
  {
    occ::handle<BRepGraph_Revision> aRevision =
      new BRepGraph_Revision(std::move(theGraph), theSchemaVersion, aSupportsSparseEdits);
    return aRevision;
  }
  catch (const Standard_Failure&)
  {
    return occ::handle<BRepGraph_Revision>();
  }
}

//=================================================================================================

occ::handle<BRepGraph_Revision> BRepGraph_Revision::FromDecodedCore(
  std::shared_ptr<const BRepGraph>                                          theGraph,
  const uint32_t                                                            theSchemaVersion,
  const NCollection_LinearVector<occ::handle<BRepGraph_RevisionComponent>>& theComponents,
  BRepGraph_RevisionStatus::Diagnostics&                                    theDiagnostics)
{
  if (theGraph == nullptr || !theGraph->IsValid() || !theGraph->ValidateRelations()
      || theSchemaVersion == 0 || theSchemaVersion > THE_SCHEMA_VERSION)
  {
    appendDiagnostic(theDiagnostics,
                     "DecodedCoreValidation",
                     "The decoded revision core is invalid or uses an unsupported schema");
    return occ::handle<BRepGraph_Revision>();
  }

  for (size_t anIndex = 0; anIndex < theComponents.Size(); ++anIndex)
  {
    const occ::handle<BRepGraph_RevisionComponent>& aComponent = theComponents.Value(anIndex);
    if (aComponent.IsNull())
    {
      appendDiagnostic(theDiagnostics,
                       "DecodedComponentValidation",
                       "A decoded persistent component is null");
      return occ::handle<BRepGraph_Revision>();
    }
    const BRepGraph_RevisionComponent::ComponentDescriptor& aDescriptor = aComponent->Descriptor();
    if (aDescriptor.RetentionKind != BRepGraph_RevisionComponent::Retention::Persistent
        || aDescriptor.SchemaVersion == 0 || !aComponent->Validate(*theGraph, theDiagnostics))
    {
      appendDiagnostic(theDiagnostics,
                       "DecodedComponentValidation",
                       "A decoded persistent component is invalid for the retained core graph");
      return occ::handle<BRepGraph_Revision>();
    }
    for (size_t aPreviousIndex = 0; aPreviousIndex < anIndex; ++aPreviousIndex)
    {
      if (theComponents.Value(aPreviousIndex)->Descriptor().StableGUID == aDescriptor.StableGUID)
      {
        appendDiagnostic(theDiagnostics,
                         "DecodedComponentDuplicate",
                         "Decoded persistent components contain a duplicate stable GUID");
        return occ::handle<BRepGraph_Revision>();
      }
    }
    for (const Standard_GUID& aDependency : aComponent->Dependencies())
    {
      bool hasDependency = false;
      for (size_t aPreviousIndex = 0; aPreviousIndex < anIndex; ++aPreviousIndex)
      {
        if (theComponents.Value(aPreviousIndex)->Descriptor().StableGUID == aDependency)
        {
          hasDependency = true;
          break;
        }
      }
      if (!hasDependency)
      {
        appendDiagnostic(theDiagnostics,
                         "DecodedComponentDependency",
                         "A decoded persistent component dependency is missing or out of order");
        return occ::handle<BRepGraph_Revision>();
      }
    }
  }

  try
  {
    const bool                      aSupportsSparseEdits = supportsSparseEdits(*theGraph);
    occ::handle<BRepGraph_Revision> aRevision = new BRepGraph_Revision(std::move(theGraph),
                                                                       theSchemaVersion,
                                                                       aSupportsSparseEdits,
                                                                       theComponents);
    return aRevision;
  }
  catch (const Standard_Failure& theFailure)
  {
    BRepGraph_RevisionStatus::Diagnostic aDiagnostic;
    aDiagnostic.Code    = "DecodedComponentHash";
    aDiagnostic.Message = theFailure.what();
    theDiagnostics.Append(aDiagnostic);
  }
  return occ::handle<BRepGraph_Revision>();
}

//=================================================================================================

bool BRepGraph_Revision::ApplyCoreChanges(
  BRepGraph&                                       theGraph,
  const NCollection_LinearVector<VertexChange>&    theChanges,
  const NCollection_LinearVector<EdgeChange>&      theEdgeChanges,
  const NCollection_LinearVector<VertexRefChange>& theVertexRefChanges,
  const NCollection_LinearVector<CoEdgeChange>&    theCoEdgeChanges,
  const NCollection_LinearVector<WireChange>&      theWireChanges,
  const NCollection_LinearVector<FaceChange>&      theFaceChanges,
  const NCollection_LinearVector<WireRefChange>&   theWireRefChanges,
  const uint32_t                                   theNextVertexCounter,
  const uint32_t                                   theNextEdgeCounter,
  const uint32_t                                   theNextVertexRefCounter,
  const uint32_t                                   theNextCoEdgeCounter,
  const uint32_t                                   theNextWireCounter,
  const uint32_t                                   theNextFaceCounter,
  const uint32_t                                   theNextWireRefCounter)
{
  NCollection_DataMap<uint32_t, size_t>         aVertexRefChangesByLocalId;
  NCollection_DataMap<BRepGraph_UID, size_t>    aCoEdgeChangesByUID;
  NCollection_DataMap<BRepGraph_RefUID, size_t> aWireRefChangesByUID;
  for (size_t anIndex = 0; anIndex < theVertexRefChanges.Size(); ++anIndex)
  {
    aVertexRefChangesByLocalId.Bind(theVertexRefChanges.Value(anIndex).LocalId.Index, anIndex);
  }
  for (size_t anIndex = 0; anIndex < theCoEdgeChanges.Size(); ++anIndex)
  {
    aCoEdgeChangesByUID.Bind(theCoEdgeChanges.Value(anIndex).UID, anIndex);
  }
  for (size_t anIndex = 0; anIndex < theWireRefChanges.Size(); ++anIndex)
  {
    aWireRefChangesByUID.Bind(theWireRefChanges.Value(anIndex).UID, anIndex);
  }

  const auto resolveVertexRef =
    [&theGraph, &theVertexRefChanges, &aVertexRefChangesByLocalId](const uint32_t   theIndex,
                                                                   VertexRefChange& theChange) {
      const size_t* aChangeIndex = aVertexRefChangesByLocalId.Seek(theIndex);
      if (aChangeIndex != nullptr)
      {
        theChange = theVertexRefChanges.Value(*aChangeIndex);
        return true;
      }
      if (theIndex >= theGraph.Refs().Vertices().Nb())
      {
        return false;
      }
      const BRepGraph_VertexRefId aRef(theIndex);
      theChange.Kind       = VertexChange::Operation::Create;
      theChange.LocalId    = aRef;
      theChange.Definition = theGraph.Refs().Vertices().Entry(aRef);
      theChange.UID        = theGraph.UIDs().Of(aRef);
      return theChange.UID.IsValid();
    };
  const auto resolveCoEdge =
    [&theGraph, &theCoEdgeChanges, &aCoEdgeChangesByUID](const BRepGraph_UID& theUID,
                                                         CoEdgeChange&        theChange) {
      const size_t* aChangeIndex = aCoEdgeChangesByUID.Seek(theUID);
      if (aChangeIndex != nullptr)
      {
        theChange = theCoEdgeChanges.Value(*aChangeIndex);
        return true;
      }
      const BRepGraph_NodeId aNode = theGraph.UIDs().NodeIdFrom(theUID);
      if (!aNode.IsValid() || aNode.NodeKind != BRepGraph_NodeId::Kind::CoEdge)
      {
        return false;
      }
      theChange.Kind       = VertexChange::Operation::Create;
      theChange.UID        = theUID;
      theChange.LocalId    = BRepGraph_CoEdgeId(aNode);
      theChange.Definition = theGraph.Topo().CoEdges().Definition(theChange.LocalId);
      return true;
    };
  const auto resolveWireRef =
    [&theGraph, &theWireRefChanges, &aWireRefChangesByUID](const BRepGraph_RefUID& theUID,
                                                           WireRefChange&          theChange) {
      const size_t* aChangeIndex = aWireRefChangesByUID.Seek(theUID);
      if (aChangeIndex != nullptr)
      {
        theChange = theWireRefChanges.Value(*aChangeIndex);
        return true;
      }
      const BRepGraph_RefId aRef = theGraph.UIDs().RefIdFrom(theUID);
      if (!aRef.IsValid() || aRef.RefKind != BRepGraph_RefId::Kind::Wire)
      {
        return false;
      }
      theChange.Kind       = VertexChange::Operation::Create;
      theChange.UID        = theUID;
      theChange.LocalId    = BRepGraph_WireRefId(aRef);
      theChange.Definition = theGraph.Refs().Wires().Entry(theChange.LocalId);
      return true;
    };

  for (const VertexChange& aChange : theChanges)
  {
    if (aChange.Kind == VertexChange::Operation::Create)
    {
      if (!theGraph.RaiseNextNodeUIDCounter(BRepGraph_NodeId::Kind::Vertex, aChange.UID.Counter))
      {
        return false;
      }
      const BRepGraph_VertexId aVertex =
        theGraph.Editor().Vertices().Add(aChange.Definition.Point, aChange.Definition.Tolerance);
      if (!aVertex.IsValid() || aVertex != aChange.LocalId)
      {
        return false;
      }
      theGraph.incStorage().ChangeVertex(aVertex) = aChange.Definition;
    }
    else
    {
      const BRepGraph_NodeId aNode = theGraph.UIDs().NodeIdFrom(aChange.UID);
      if (!aNode.IsValid() || aNode.NodeKind != BRepGraph_NodeId::Kind::Vertex)
      {
        return false;
      }
      if (aChange.Kind == VertexChange::Operation::Remove)
      {
        theGraph.Editor().Gen().RemoveNode(aNode);
      }
      else
      {
        theGraph.incStorage().ChangeVertex(BRepGraph_VertexId(aNode)) = aChange.Definition;
      }
    }
  }

  for (const EdgeChange& aChange : theEdgeChanges)
  {
    if (aChange.Kind == VertexChange::Operation::Create)
    {
      VertexRefChange aStartRef;
      VertexRefChange anEndRef;
      if (!resolveVertexRef(aChange.Definition.StartVertexRefId.Index, aStartRef)
          || !resolveVertexRef(aChange.Definition.EndVertexRefId.Index, anEndRef)
          || aChange.Curve.IsNull()
          || !theGraph.RaiseNextNodeUIDCounter(BRepGraph_NodeId::Kind::Edge, aChange.UID.Counter)
          || !theGraph.RaiseNextRefUIDCounter(BRepGraph_RefId::Kind::Vertex, aStartRef.UID.Counter))
      {
        return false;
      }
      const occ::handle<Geom_Curve> aCurveCopy = occ::down_cast<Geom_Curve>(aChange.Curve->Copy());
      if (aCurveCopy.IsNull())
      {
        return false;
      }
      const BRepGraph_EdgeId anEdge =
        theGraph.Editor().Edges().Add(aStartRef.Definition.ChildVertexId,
                                      anEndRef.Definition.ChildVertexId,
                                      aCurveCopy,
                                      aChange.CurveParamFirst,
                                      aChange.CurveParamLast,
                                      aChange.Definition.Tolerance);
      if (!anEdge.IsValid() || anEdge != aChange.LocalId)
      {
        return false;
      }
      const BRepGraphInc::EdgeDef& anAllocated = theGraph.incStorage().Edge(anEdge);
      if (anAllocated.StartVertexRefId != aStartRef.LocalId
          || anAllocated.EndVertexRefId != anEndRef.LocalId)
      {
        return false;
      }
      BRepGraphInc::EdgeDef aDefinition                        = aChange.Definition;
      aDefinition.Curve3DRepId                                 = anAllocated.Curve3DRepId;
      theGraph.incStorage().ChangeEdge(anEdge)                 = aDefinition;
      theGraph.incStorage().ChangeVertexRef(aStartRef.LocalId) = aStartRef.Definition;
      theGraph.incStorage().ChangeVertexRef(anEndRef.LocalId)  = anEndRef.Definition;
    }
    else
    {
      const BRepGraph_NodeId aNode = theGraph.UIDs().NodeIdFrom(aChange.UID);
      if (!aNode.IsValid() || aNode.NodeKind != BRepGraph_NodeId::Kind::Edge)
      {
        return false;
      }
      if (aChange.Kind == VertexChange::Operation::Remove)
      {
        theGraph.Editor().Gen().RemoveNode(aNode);
        continue;
      }
      BRepGraphInc::EdgeDef aDefinition = aChange.Definition;
      if (aChange.HasCurveUpdate)
      {
        if (aChange.Curve.IsNull())
        {
          return false;
        }
        const occ::handle<Geom_Curve> aCurveCopy =
          occ::down_cast<Geom_Curve>(aChange.Curve->Copy());
        if (aCurveCopy.IsNull())
        {
          return false;
        }
        theGraph.Editor().Edges().SetCurve(BRepGraph_EdgeId(aNode),
                                           aCurveCopy,
                                           aChange.CurveParamFirst,
                                           aChange.CurveParamLast);
        aDefinition.Curve3DRepId = theGraph.incStorage().Edge(BRepGraph_EdgeId(aNode)).Curve3DRepId;
      }
      if (!aDefinition.Curve3DRepId.IsValid())
      {
        aDefinition.Curve3DRepId = theGraph.incStorage().Edge(BRepGraph_EdgeId(aNode)).Curve3DRepId;
      }
      theGraph.incStorage().ChangeEdge(BRepGraph_EdgeId(aNode)) = aDefinition;
    }
  }

  for (const VertexRefChange& aChange : theVertexRefChanges)
  {
    if (aChange.Kind == VertexChange::Operation::Remove)
    {
      theGraph.Editor().Gen().RemoveRef(aChange.LocalId);
    }
    else if (aChange.Kind == VertexChange::Operation::Modify)
    {
      theGraph.incStorage().ChangeVertexRef(aChange.LocalId) = aChange.Definition;
    }
  }

  for (const CoEdgeChange& aChange : theCoEdgeChanges)
  {
    if (aChange.Kind == VertexChange::Operation::Create)
    {
      if (!theGraph.RaiseNextNodeUIDCounter(BRepGraph_NodeId::Kind::CoEdge, aChange.UID.Counter))
      {
        return false;
      }
      const BRepGraph_CoEdgeId aCoEdge =
        theGraph.Editor().CoEdges().Add(aChange.Definition.ChildEdgeId,
                                        aChange.Definition.Orientation);
      if (!aCoEdge.IsValid() || aCoEdge != aChange.LocalId)
      {
        return false;
      }
      theGraph.incStorage().ChangeCoEdge(aCoEdge) = aChange.Definition;
    }
    else
    {
      const BRepGraph_NodeId aNode = theGraph.UIDs().NodeIdFrom(aChange.UID);
      if (!aNode.IsValid() || aNode.NodeKind != BRepGraph_NodeId::Kind::CoEdge)
      {
        return false;
      }
      if (aChange.Kind == VertexChange::Operation::Remove)
      {
        const BRepGraph_CoEdgeId aCoEdge(aNode);
        theGraph.Editor().CoEdges().SetChildEdgeId(aCoEdge, BRepGraph_EdgeId());
        theGraph.Editor().Gen().RemoveNode(aNode);
      }
      else
      {
        theGraph.incStorage().ChangeCoEdge(BRepGraph_CoEdgeId(aNode)) = aChange.Definition;
      }
    }
  }

  for (const WireChange& aChange : theWireChanges)
  {
    NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdgeIds;
    for (const BRepGraph_UID& aCoEdgeUID : aChange.CoEdgeUIDs)
    {
      CoEdgeChange aCoEdgeChange;
      if (!resolveCoEdge(aCoEdgeUID, aCoEdgeChange)
          || aCoEdgeChange.Kind == VertexChange::Operation::Remove)
      {
        return false;
      }
      aCoEdgeIds.Append(aCoEdgeChange.LocalId);
    }
    if (aChange.Kind == VertexChange::Operation::Create)
    {
      if (!theGraph.RaiseNextNodeUIDCounter(BRepGraph_NodeId::Kind::Wire, aChange.UID.Counter))
      {
        return false;
      }
      for (const BRepGraph_CoEdgeId aCoEdge : aCoEdgeIds)
      {
        theGraph.incStorage().ChangeCoEdge(aCoEdge).ParentWireId = BRepGraph_WireId();
      }
      const BRepGraph_WireId aWire = theGraph.Editor().Wires().Add(aCoEdgeIds.ToArray1());
      if (!aWire.IsValid() || aWire != aChange.LocalId)
      {
        return false;
      }
      theGraph.incStorage().ChangeWire(aWire) = aChange.Definition;
      for (const BRepGraph_UID& aCoEdgeUID : aChange.CoEdgeUIDs)
      {
        CoEdgeChange aCoEdgeChange;
        if (!resolveCoEdge(aCoEdgeUID, aCoEdgeChange))
        {
          return false;
        }
        theGraph.incStorage().ChangeCoEdge(aCoEdgeChange.LocalId) = aCoEdgeChange.Definition;
      }
    }
    else
    {
      const BRepGraph_NodeId aNode = theGraph.UIDs().NodeIdFrom(aChange.UID);
      if (!aNode.IsValid() || aNode.NodeKind != BRepGraph_NodeId::Kind::Wire)
      {
        return false;
      }
      if (aChange.Kind == VertexChange::Operation::Remove)
      {
        theGraph.Editor().Gen().RemoveNode(aNode);
      }
      else
      {
        theGraph.incStorage().SetWireCoEdges(BRepGraph_WireId(aNode), aCoEdgeIds.ToArray1());
        theGraph.incStorage().ChangeWire(BRepGraph_WireId(aNode)) = aChange.Definition;
      }
    }
  }

  for (const FaceChange& aChange : theFaceChanges)
  {
    if (aChange.Kind == VertexChange::Operation::Create)
    {
      if (!theGraph.RaiseNextNodeUIDCounter(BRepGraph_NodeId::Kind::Face, aChange.UID.Counter))
      {
        return false;
      }
      const BRepGraph_FaceId aFace = theGraph.incStorage().AppendFace();
      if (!aFace.IsValid() || aFace != aChange.LocalId
          || theGraph.allocateUID(aFace).Counter != aChange.UID.Counter)
      {
        return false;
      }
      theGraph.incStorage().ChangeFace(aFace) = aChange.Definition;
      for (const BRepGraph_RefUID& aWireRefUID : aChange.WireRefUIDs)
      {
        WireRefChange aWireRefChange;
        if (!resolveWireRef(aWireRefUID, aWireRefChange)
            || aWireRefChange.Kind != VertexChange::Operation::Create
            || !theGraph.RaiseNextRefUIDCounter(BRepGraph_RefId::Kind::Wire,
                                                aWireRefChange.UID.Counter))
        {
          return false;
        }
        const BRepGraph_WireRefId aWireRef =
          theGraph.incStorage().AttachWireToFace(aFace,
                                                 aWireRefChange.Definition.ChildWireId,
                                                 aWireRefChange.Definition.Orientation);
        if (!aWireRef.IsValid() || aWireRef != aWireRefChange.LocalId
            || theGraph.allocateRefUID(aWireRef).Counter != aWireRefChange.UID.Counter)
        {
          return false;
        }
        theGraph.incStorage().ChangeWireRef(aWireRef) = aWireRefChange.Definition;
      }
    }
    else
    {
      const BRepGraph_NodeId aNode = theGraph.UIDs().NodeIdFrom(aChange.UID);
      if (!aNode.IsValid() || aNode.NodeKind != BRepGraph_NodeId::Kind::Face)
      {
        return false;
      }
      if (aChange.Kind == VertexChange::Operation::Remove)
      {
        theGraph.Editor().Gen().RemoveNode(aNode);
        continue;
      }
      NCollection_LinearVector<BRepGraph_WireRefId> aWireRefIds;
      for (const BRepGraph_RefUID& aWireRefUID : aChange.WireRefUIDs)
      {
        WireRefChange aWireRefChange;
        if (!resolveWireRef(aWireRefUID, aWireRefChange)
            || aWireRefChange.Kind == VertexChange::Operation::Remove)
        {
          return false;
        }
        aWireRefIds.Append(aWireRefChange.LocalId);
      }
      theGraph.incStorage().SetFaceWireRefs(BRepGraph_FaceId(aNode), aWireRefIds.ToArray1());
      theGraph.incStorage().ChangeFace(BRepGraph_FaceId(aNode)) = aChange.Definition;
    }
  }

  for (const WireRefChange& aChange : theWireRefChanges)
  {
    if (aChange.Kind == VertexChange::Operation::Remove)
    {
      theGraph.Editor().Gen().RemoveRef(aChange.LocalId);
    }
    else if (aChange.Kind == VertexChange::Operation::Modify)
    {
      theGraph.incStorage().ChangeWireRef(aChange.LocalId) = aChange.Definition;
    }
  }

  theGraph.Editor().Gen().CleanupRemovedReferences();
  return theGraph.RaiseNextNodeUIDCounter(BRepGraph_NodeId::Kind::Vertex, theNextVertexCounter)
         && theGraph.RaiseNextNodeUIDCounter(BRepGraph_NodeId::Kind::Edge, theNextEdgeCounter)
         && theGraph.RaiseNextRefUIDCounter(BRepGraph_RefId::Kind::Vertex, theNextVertexRefCounter)
         && theGraph.RaiseNextNodeUIDCounter(BRepGraph_NodeId::Kind::CoEdge, theNextCoEdgeCounter)
         && theGraph.RaiseNextNodeUIDCounter(BRepGraph_NodeId::Kind::Wire, theNextWireCounter)
         && theGraph.RaiseNextNodeUIDCounter(BRepGraph_NodeId::Kind::Face, theNextFaceCounter)
         && theGraph.RaiseNextRefUIDCounter(BRepGraph_RefId::Kind::Wire, theNextWireRefCounter);
}

//=================================================================================================

bool BRepGraph_Revision::CopyTo(BRepGraph& theGraph) const
{
  std::unique_ptr<BRepGraph> aGraph = copyGraph();
  if (aGraph == nullptr)
  {
    return false;
  }
  theGraph = std::move(*aGraph);
  return true;
}

//=================================================================================================

std::unique_ptr<BRepGraph> BRepGraph_Revision::copyGraph() const
{
  if (myCoreGraph == nullptr)
  {
    return nullptr;
  }

  // Restore components only into the new graph copy.
  std::unique_ptr<BRepGraph>            aGraph(new BRepGraph(myCoreGraph->incStorage(), true));
  BRepGraph_RevisionStatus::Diagnostics aDiagnostics;
  for (size_t anIndex = 0; anIndex < myComponents.Size(); ++anIndex)
  {
    const occ::handle<BRepGraph_RevisionComponent>& aComponent = myComponents.Value(anIndex);
    const ComponentState&                           aState     = myComponentStates.Value(anIndex);
    if (aComponent.IsNull())
    {
      return nullptr;
    }
    const BRepGraph_RevisionComponent::ComponentDescriptor& aDescriptor = aComponent->Descriptor();
    if (aDescriptor.StableGUID != aState.Descriptor.StableGUID
        || aDescriptor.SchemaVersion != aState.Descriptor.SchemaVersion
        || aDescriptor.ComponentDomain != aState.Descriptor.ComponentDomain
        || aDescriptor.RetentionKind != aState.Descriptor.RetentionKind
        || aDescriptor.IsEditable != aState.Descriptor.IsEditable
        || aDescriptor.IsArchivable != aState.Descriptor.IsArchivable
        || aComponent->SemanticHash() != aState.SemanticHash
        || aComponent->StorageHash() != aState.StorageHash
        || !aComponent->Restore(*aGraph, aDiagnostics))
    {
      return nullptr;
    }
  }
  return aGraph;
}

//=================================================================================================

bool BRepGraph_Revision::ValidateRelations() const
{
  return myCoreGraph != nullptr && myCoreGraph->ValidateRelations();
}

//=================================================================================================

const BRepGraph& BRepGraph_Revision::CoreGraph() const
{
  const BRepGraph* aGraph = TryCoreGraph();
  if (aGraph == nullptr)
  {
    throw Standard_ProgramError("BRepGraph_Revision::CoreGraph(): invalid revision");
  }
  return *aGraph;
}

//=================================================================================================

const BRepGraph* BRepGraph_Revision::TryCoreGraph() const
{
  return myCoreGraph.get();
}

//=================================================================================================

bool BRepGraph_Revision::ValidateComponents(const BRepGraph&                       theGraph,
                                            BRepGraph_RevisionStatus::Diagnostics& theDiagnostics)
{
  NCollection_LinearVector<occ::handle<BRepGraph_RevisionComponent>> aComponents;
  return CaptureComponents(theGraph, aComponents, theDiagnostics);
}

//=================================================================================================

bool BRepGraph_Revision::CaptureComponents(
  const BRepGraph&                                                    theGraph,
  NCollection_LinearVector<occ::handle<BRepGraph_RevisionComponent>>& theComponents,
  BRepGraph_RevisionStatus::Diagnostics&                              theDiagnostics)
{
  theComponents.Clear();
  bool       isValid = true;
  const auto appendFailure =
    [&theDiagnostics](const char* theCode, const char* theMessage, const Standard_GUID& theGUID) {
      BRepGraph_RevisionStatus::Diagnostic aDiagnostic;
      aDiagnostic.Code          = theCode;
      aDiagnostic.Message       = theMessage;
      aDiagnostic.ComponentGUID = theGUID;
      theDiagnostics.Append(aDiagnostic);
    };
  const auto captureLayer = [&](const occ::handle<BRepGraph_Layer>&       theLayer,
                                const BRepGraph_RevisionComponent::Domain theDomain) {
    if (theLayer.IsNull())
    {
      return;
    }
    const BRepGraph_RevisionComponent::ComponentDescriptor aLiveDescriptor =
      theLayer->RevisionDescriptor();
    if (aLiveDescriptor.RetentionKind == BRepGraph_RevisionComponent::Retention::Transient)
    {
      return;
    }
    if (aLiveDescriptor.RetentionKind == BRepGraph_RevisionComponent::Retention::Unspecified)
    {
      appendFailure("UnspecifiedLayerRetention",
                    "The registered layer has no revision retention policy",
                    theLayer->ID());
      isValid = false;
      return;
    }
    const occ::handle<BRepGraph_RevisionComponent> aRevision =
      theLayer->CaptureRevision(theGraph, theDiagnostics);
    if (aRevision.IsNull())
    {
      appendFailure("UnsupportedPersistentLayer",
                    "The registered persistent layer cannot capture immutable revision",
                    theLayer->ID());
      isValid = false;
      return;
    }
    const BRepGraph_RevisionComponent::ComponentDescriptor& aRevisionDescriptor =
      aRevision->Descriptor();
    if (aRevisionDescriptor.StableGUID != theLayer->ID()
        || aRevisionDescriptor.ComponentDomain != theDomain
        || aRevisionDescriptor.RetentionKind != BRepGraph_RevisionComponent::Retention::Persistent
        || aRevisionDescriptor.SchemaVersion == 0)
    {
      appendFailure("InvalidPersistentLayerGeneration",
                    "The captured layer descriptor does not match its live component",
                    theLayer->ID());
      isValid = false;
      return;
    }
    if (!aRevision->Validate(theGraph, theDiagnostics))
    {
      isValid = false;
      return;
    }
    theComponents.Append(aRevision);
  };

  const BRepGraph_LayerRegistry& aLayerRegistry = theGraph.LayerRegistry();
  for (uint32_t aSlot = 0; aSlot < aLayerRegistry.NbLayers(); ++aSlot)
  {
    captureLayer(aLayerRegistry.Layer(aSlot), BRepGraph_RevisionComponent::Domain::Layer);
  }

  const BRepGraph_LayerSupplementRegistry& aSupplementLayerRegistry =
    theGraph.LayerSupplementRegistry();
  for (uint32_t aSlot = 0; aSlot < aSupplementLayerRegistry.NbLayers(); ++aSlot)
  {
    captureLayer(aSupplementLayerRegistry.Layer(aSlot),
                 BRepGraph_RevisionComponent::Domain::SupplementalLayer);
  }

  const BRepGraphSupInc_StoreRegistry& aStoreRegistry = theGraph.supplementStorage().Registry();
  NCollection_LinearVector<occ::handle<BRepGraphSupInc_Store>> aStores;
  aStores.Reserve(aStoreRegistry.Count());
  {
    BRepGraphSupInc_StoreRegistry::Iterator anIt(aStoreRegistry);
    for (; anIt.More(); anIt.Next())
    {
      aStores.Append(anIt.Value());
    }
  }
  for (const occ::handle<BRepGraphSupInc_Store>& aStore : aStores)
  {
    if (aStore.IsNull())
    {
      continue;
    }
    const BRepGraph_RevisionComponent::ComponentDescriptor aDescriptor =
      aStore->RevisionDescriptor();
    if (aDescriptor.RetentionKind == BRepGraph_RevisionComponent::Retention::Transient)
    {
      continue;
    }
    if (aDescriptor.RetentionKind == BRepGraph_RevisionComponent::Retention::Unspecified)
    {
      appendFailure("UnspecifiedStoreRetention",
                    "The registered supplemental store has no revision retention policy",
                    aStore->ID());
      isValid = false;
      continue;
    }
    const occ::handle<BRepGraph_RevisionComponent> aRevision =
      aStore->CaptureRevision(theGraph, theDiagnostics);
    if (aRevision.IsNull())
    {
      appendFailure("UnsupportedPersistentStore",
                    "The registered persistent store cannot capture immutable revision",
                    aStore->ID());
      isValid = false;
      continue;
    }
    const BRepGraph_RevisionComponent::ComponentDescriptor& aRevisionDescriptor =
      aRevision->Descriptor();
    if (aRevisionDescriptor.StableGUID != aStore->ID()
        || aRevisionDescriptor.ComponentDomain
             != BRepGraph_RevisionComponent::Domain::SupplementalStore
        || aRevisionDescriptor.RetentionKind != BRepGraph_RevisionComponent::Retention::Persistent
        || aRevisionDescriptor.SchemaVersion == 0 || !aRevision->Validate(theGraph, theDiagnostics))
    {
      appendFailure("InvalidPersistentStoreRevision",
                    "The captured store revision is invalid or does not match its live component",
                    aStore->ID());
      isValid = false;
      continue;
    }
    theComponents.Append(aRevision);
  }

  std::sort(theComponents.begin(),
            theComponents.end(),
            [](const occ::handle<BRepGraph_RevisionComponent>& theLeft,
               const occ::handle<BRepGraph_RevisionComponent>& theRight) {
              const BRepGraph_RevisionComponent::ComponentDescriptor& aLeft = theLeft->Descriptor();
              const BRepGraph_RevisionComponent::ComponentDescriptor& aRight =
                theRight->Descriptor();
              if (aLeft.ComponentDomain != aRight.ComponentDomain)
              {
                return static_cast<uint8_t>(aLeft.ComponentDomain)
                       < static_cast<uint8_t>(aRight.ComponentDomain);
              }
              return compareGUIDs(aLeft.StableGUID, aRight.StableGUID) < 0;
            });
  for (size_t anIndex = 0; anIndex < theComponents.Size(); ++anIndex)
  {
    const BRepGraph_RevisionComponent::ComponentDescriptor& aCurrent =
      theComponents.Value(anIndex)->Descriptor();
    for (size_t anotherIndex = 0; anotherIndex < anIndex; ++anotherIndex)
    {
      if (theComponents.Value(anotherIndex)->Descriptor().StableGUID == aCurrent.StableGUID)
      {
        appendFailure("DuplicatePersistentComponent",
                      "Multiple persistent components have the same stable GUID",
                      aCurrent.StableGUID);
        isValid = false;
        break;
      }
    }
  }
  if (!isValid || theComponents.IsEmpty())
  {
    return isValid;
  }

  NCollection_Array1<uint8_t> aVisitState(0, static_cast<int>(theComponents.Size()) - 1);
  aVisitState.Init(0);
  NCollection_LinearVector<occ::handle<BRepGraph_RevisionComponent>> anOrderedComponents;
  std::function<bool(size_t)> visitComponent = [&](const size_t theIndex) {
    if (aVisitState.Value(static_cast<int>(theIndex)) == 2)
    {
      return true;
    }
    if (aVisitState.Value(static_cast<int>(theIndex)) == 1)
    {
      appendFailure("PersistentComponentCycle",
                    "Persistent component dependencies contain a cycle",
                    theComponents.Value(theIndex)->Descriptor().StableGUID);
      return false;
    }
    aVisitState.ChangeValue(static_cast<int>(theIndex)) = 1;
    for (const Standard_GUID& aDependency : theComponents.Value(theIndex)->Dependencies())
    {
      size_t aDependencyIndex = theComponents.Size();
      for (size_t aRootIndex = 0; aRootIndex < theComponents.Size(); ++aRootIndex)
      {
        if (theComponents.Value(aRootIndex)->Descriptor().StableGUID == aDependency)
        {
          aDependencyIndex = aRootIndex;
          break;
        }
      }
      if (aDependencyIndex == theComponents.Size())
      {
        appendFailure("MissingPersistentComponentDependency",
                      "A required persistent component dependency is missing",
                      theComponents.Value(theIndex)->Descriptor().StableGUID);
        return false;
      }
      if (!visitComponent(aDependencyIndex))
      {
        return false;
      }
    }
    aVisitState.ChangeValue(static_cast<int>(theIndex)) = 2;
    anOrderedComponents.Append(theComponents.Value(theIndex));
    return true;
  };
  for (size_t anIndex = 0; anIndex < theComponents.Size(); ++anIndex)
  {
    if (!visitComponent(anIndex))
    {
      isValid = false;
    }
  }
  if (isValid)
  {
    theComponents = std::move(anOrderedComponents);
  }
  return isValid;
}
