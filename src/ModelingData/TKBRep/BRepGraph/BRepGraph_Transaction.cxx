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

#include <BRepGraph_Transaction.hxx>

#include <BRepGraph.hxx>
#include <BRepGraph_RevisionStatus.hxx>
#include <BRepGraph_TopoView.hxx>

#include <NCollection_FlatDataMap.hxx>
#include <NCollection_FlatMap.hxx>

#include <algorithm>
#include <utility>

namespace
{

void appendDiagnostic(BRepGraph_RevisionStatus::Diagnostics& theDiagnostics,
                      const char*                            theCode,
                      const char*                            theMessage)
{
  BRepGraph_RevisionStatus::Diagnostic aDiagnostic;
  aDiagnostic.Code    = theCode;
  aDiagnostic.Message = theMessage;
  theDiagnostics.Append(aDiagnostic);
}

} // namespace

//=================================================================================================

BRepGraph_Transaction::~BRepGraph_Transaction() = default;

//=================================================================================================

BRepGraph_Transaction::BRepGraph_Transaction(const occ::handle<BRepGraph_Revision>& theBaseRevision)
    : myBaseRevision(theBaseRevision),
      myStatus(BRepGraph_RevisionStatus::Code::InvalidBase),
      myFinished(false)
{
  if (myBaseRevision.IsNull() || !myBaseRevision->IsValid())
  {
    appendDiagnostic(myDiagnostics,
                     "InvalidBase",
                     "The transaction has no valid immutable base revision");
    return;
  }

  myNextVertexCounter    = myBaseRevision->NextVertexCounter();
  myNextEdgeCounter      = myBaseRevision->NextEdgeCounter();
  myNextVertexRefCounter = myBaseRevision->NextVertexRefCounter();
  myNextCoEdgeCounter    = myBaseRevision->NextCoEdgeCounter();
  myNextWireCounter      = myBaseRevision->NextWireCounter();
  myNextFaceCounter      = myBaseRevision->NextFaceCounter();
  myNextWireRefCounter   = myBaseRevision->NextWireRefCounter();
  myNextVertexIndex      = myBaseRevision->VertexCount();
  myNextEdgeIndex        = myBaseRevision->EdgeCount();
  myNextVertexRefIndex   = myBaseRevision->VertexRefCount();
  myNextCoEdgeIndex      = myBaseRevision->CoEdgeCount();
  myNextWireIndex        = myBaseRevision->WireCount();
  myNextFaceIndex        = myBaseRevision->FaceCount();
  myNextWireRefIndex     = myBaseRevision->WireRefCount();
  for (const occ::handle<BRepGraph_RevisionComponent>& aComponent : myBaseRevision->Components())
  {
    myComponentIndices.Bind(aComponent->Descriptor().StableGUID,
                            static_cast<uint32_t>(myComponents.Size()));
    myComponents.Append(aComponent);
  }
  myStatus = BRepGraph_RevisionStatus::Code::Ok;
}

//=================================================================================================

BRepGraph_Transaction::BRepGraph_Transaction(BRepGraph_Transaction&& theOther) noexcept
    : myBaseRevision(std::move(theOther.myBaseRevision)),
      myWorkingGraph(std::move(theOther.myWorkingGraph)),
      myChanges(std::move(theOther.myChanges)),
      myEdgeChanges(std::move(theOther.myEdgeChanges)),
      myVertexRefChanges(std::move(theOther.myVertexRefChanges)),
      myCoEdgeChanges(std::move(theOther.myCoEdgeChanges)),
      myWireChanges(std::move(theOther.myWireChanges)),
      myFaceChanges(std::move(theOther.myFaceChanges)),
      myWireRefChanges(std::move(theOther.myWireRefChanges)),
      myVertexChangeIndices(std::move(theOther.myVertexChangeIndices)),
      myEdgeChangeIndices(std::move(theOther.myEdgeChangeIndices)),
      myVertexRefChangeIndices(std::move(theOther.myVertexRefChangeIndices)),
      myCoEdgeChangeIndices(std::move(theOther.myCoEdgeChangeIndices)),
      myWireChangeIndices(std::move(theOther.myWireChangeIndices)),
      myFaceChangeIndices(std::move(theOther.myFaceChangeIndices)),
      myWireRefChangeIndices(std::move(theOther.myWireRefChangeIndices)),
      myComponents(std::move(theOther.myComponents)),
      myComponentIndices(std::move(theOther.myComponentIndices)),
      myChangedComponentGUIDs(std::move(theOther.myChangedComponentGUIDs)),
      myAllocatedRanges(std::move(theOther.myAllocatedRanges)),
      myDiagnostics(std::move(theOther.myDiagnostics)),
      myStatus(theOther.myStatus),
      myNextVertexCounter(theOther.myNextVertexCounter),
      myNextEdgeCounter(theOther.myNextEdgeCounter),
      myNextVertexRefCounter(theOther.myNextVertexRefCounter),
      myNextCoEdgeCounter(theOther.myNextCoEdgeCounter),
      myNextWireCounter(theOther.myNextWireCounter),
      myNextFaceCounter(theOther.myNextFaceCounter),
      myNextWireRefCounter(theOther.myNextWireRefCounter),
      myNextVertexIndex(theOther.myNextVertexIndex),
      myNextEdgeIndex(theOther.myNextEdgeIndex),
      myNextVertexRefIndex(theOther.myNextVertexRefIndex),
      myNextCoEdgeIndex(theOther.myNextCoEdgeIndex),
      myNextWireIndex(theOther.myNextWireIndex),
      myNextFaceIndex(theOther.myNextFaceIndex),
      myNextWireRefIndex(theOther.myNextWireRefIndex),
      myFinished(theOther.myFinished),
      myComponentsChanged(theOther.myComponentsChanged)
{
  theOther.clearChangeIndices();
  theOther.myFinished = true;
}

//=================================================================================================

BRepGraph_Transaction& BRepGraph_Transaction::operator=(BRepGraph_Transaction&& theOther) noexcept
{
  if (this != &theOther)
  {
    myBaseRevision           = std::move(theOther.myBaseRevision);
    myWorkingGraph           = std::move(theOther.myWorkingGraph);
    myChanges                = std::move(theOther.myChanges);
    myEdgeChanges            = std::move(theOther.myEdgeChanges);
    myVertexRefChanges       = std::move(theOther.myVertexRefChanges);
    myCoEdgeChanges          = std::move(theOther.myCoEdgeChanges);
    myWireChanges            = std::move(theOther.myWireChanges);
    myFaceChanges            = std::move(theOther.myFaceChanges);
    myWireRefChanges         = std::move(theOther.myWireRefChanges);
    myVertexChangeIndices    = std::move(theOther.myVertexChangeIndices);
    myEdgeChangeIndices      = std::move(theOther.myEdgeChangeIndices);
    myVertexRefChangeIndices = std::move(theOther.myVertexRefChangeIndices);
    myCoEdgeChangeIndices    = std::move(theOther.myCoEdgeChangeIndices);
    myWireChangeIndices      = std::move(theOther.myWireChangeIndices);
    myFaceChangeIndices      = std::move(theOther.myFaceChangeIndices);
    myWireRefChangeIndices   = std::move(theOther.myWireRefChangeIndices);
    myComponents             = std::move(theOther.myComponents);
    myComponentIndices       = std::move(theOther.myComponentIndices);
    myChangedComponentGUIDs  = std::move(theOther.myChangedComponentGUIDs);
    myAllocatedRanges        = std::move(theOther.myAllocatedRanges);
    myDiagnostics            = std::move(theOther.myDiagnostics);
    myStatus                 = theOther.myStatus;
    myNextVertexCounter      = theOther.myNextVertexCounter;
    myNextEdgeCounter        = theOther.myNextEdgeCounter;
    myNextVertexRefCounter   = theOther.myNextVertexRefCounter;
    myNextCoEdgeCounter      = theOther.myNextCoEdgeCounter;
    myNextWireCounter        = theOther.myNextWireCounter;
    myNextFaceCounter        = theOther.myNextFaceCounter;
    myNextWireRefCounter     = theOther.myNextWireRefCounter;
    myNextVertexIndex        = theOther.myNextVertexIndex;
    myNextEdgeIndex          = theOther.myNextEdgeIndex;
    myNextVertexRefIndex     = theOther.myNextVertexRefIndex;
    myNextCoEdgeIndex        = theOther.myNextCoEdgeIndex;
    myNextWireIndex          = theOther.myNextWireIndex;
    myNextFaceIndex          = theOther.myNextFaceIndex;
    myNextWireRefIndex       = theOther.myNextWireRefIndex;
    myFinished               = theOther.myFinished;
    myComponentsChanged      = theOther.myComponentsChanged;
    theOther.clearChangeIndices();
    theOther.myFinished = true;
  }
  return *this;
}

//=================================================================================================

BRepGraph_Transaction BRepGraph_Transaction::Begin(
  const occ::handle<BRepGraph_Revision>& theBaseRevision)
{
  return BRepGraph_Transaction(theBaseRevision);
}

//=================================================================================================

BRepGraph* BRepGraph_Transaction::Graph()
{
  if (!IsValid())
  {
    return nullptr;
  }
  if (myWorkingGraph != nullptr)
  {
    return myWorkingGraph.get();
  }
  if (hasSparseChanges() || myComponentsChanged || !myAllocatedRanges.IsEmpty())
  {
    appendDiagnostic(myDiagnostics,
                     "MixedEditModes",
                     "Complete graph editing cannot follow sparse transaction changes");
    return nullptr;
  }

  myWorkingGraph = myBaseRevision->copyGraph();
  if (myWorkingGraph == nullptr)
  {
    myStatus = BRepGraph_RevisionStatus::Code::CopyFailed;
    appendDiagnostic(myDiagnostics,
                     "TransactionCopy",
                     "The immutable revision could not be copied for complete graph editing");
  }
  return myWorkingGraph.get();
}

//=================================================================================================

occ::handle<BRepGraph_RevisionComponent> BRepGraph_Transaction::Component(
  const Standard_GUID& theGUID) const
{
  const uint32_t* anIndex = myComponentIndices.Seek(theGUID);
  return anIndex == nullptr ? occ::handle<BRepGraph_RevisionComponent>()
                            : myComponents.Value(*anIndex);
}

//=================================================================================================

bool BRepGraph_Transaction::ReplaceComponent(
  const occ::handle<BRepGraph_RevisionComponent>& theComponent)
{
  if (!IsValid() || myWorkingGraph != nullptr || theComponent.IsNull())
  {
    return false;
  }
  const BRepGraph_RevisionComponent::ComponentDescriptor& aDescriptor = theComponent->Descriptor();
  if (aDescriptor.RetentionKind != BRepGraph_RevisionComponent::Retention::Persistent
      || !aDescriptor.IsEditable)
  {
    return false;
  }
  const uint32_t* anIndex = myComponentIndices.Seek(aDescriptor.StableGUID);
  if (anIndex == nullptr)
  {
    return false;
  }
  const occ::handle<BRepGraph_RevisionComponent>&         aPrevious = myComponents.Value(*anIndex);
  const BRepGraph_RevisionComponent::ComponentDescriptor& aPreviousDescriptor =
    aPrevious->Descriptor();
  if (aPreviousDescriptor.ComponentDomain != aDescriptor.ComponentDomain
      || aPreviousDescriptor.SchemaVersion != aDescriptor.SchemaVersion)
  {
    return false;
  }
  if (aPrevious == theComponent)
  {
    return true;
  }
  const BRepGraph_RevisionHash aPreviousSemantic  = aPrevious->SemanticHash();
  const BRepGraph_RevisionHash aComponentSemantic = theComponent->SemanticHash();
  const bool                   hasSemanticChange  = aPreviousSemantic != aComponentSemantic;
  if (!hasSemanticChange && aPrevious->StorageHash() == theComponent->StorageHash())
  {
    return true;
  }
  myComponents.ChangeValue(*anIndex) = theComponent;
  myComponentsChanged                = true;
  if (hasSemanticChange)
  {
    myChangedComponentGUIDs.Add(aDescriptor.StableGUID);
  }
  return true;
}

//=================================================================================================

BRepGraph_VertexId BRepGraph_Transaction::AddVertex(const gp_Pnt& thePoint,
                                                    const double  theTolerance)
{
  if (!canEditTopology() || myNextVertexCounter == 0xffffffffu)
  {
    return BRepGraph_VertexId();
  }

  BRepGraph_Revision::VertexChange aChange;
  aChange.Kind             = BRepGraph_Revision::VertexChange::Operation::Create;
  aChange.UID              = BRepGraph_UID(BRepGraph_NodeId::Kind::Vertex, myNextVertexCounter++);
  aChange.LocalId          = BRepGraph_VertexId(myNextVertexIndex++);
  aChange.Definition.UID   = aChange.UID.Counter;
  aChange.Definition.Point = thePoint;
  aChange.Definition.Tolerance = theTolerance;
  recordChange(aChange);

  UIDRange aRange;
  aRange.IsReference = false;
  aRange.NodeKind    = BRepGraph_NodeId::Kind::Vertex;
  aRange.First       = aChange.UID.Counter;
  aRange.Last        = aChange.UID.Counter;
  recordAllocation(aRange);
  return aChange.LocalId;
}

//=================================================================================================

BRepGraph_EdgeId BRepGraph_Transaction::AddEdge(const BRepGraph_UID&           theStartVertexUID,
                                                const BRepGraph_UID&           theEndVertexUID,
                                                const occ::handle<Geom_Curve>& theCurve,
                                                const double                   theParamFirst,
                                                const double                   theParamLast,
                                                const double                   theTolerance)
{
  if (!canEditTopology() || myNextEdgeCounter == 0xffffffffu
      || myNextVertexRefCounter >= 0xfffffffeu || theCurve.IsNull())
  {
    return BRepGraph_EdgeId();
  }
  const occ::handle<Geom_Curve> aCurveCopy = occ::down_cast<Geom_Curve>(theCurve->Copy());
  if (aCurveCopy.IsNull())
  {
    return BRepGraph_EdgeId();
  }

  BRepGraph_Revision::VertexChange aStartVertex;
  BRepGraph_Revision::VertexChange anEndVertex;
  if (!resolveVertexChange(theStartVertexUID, aStartVertex)
      || !resolveVertexChange(theEndVertexUID, anEndVertex)
      || aStartVertex.Kind == BRepGraph_Revision::VertexChange::Operation::Remove
      || anEndVertex.Kind == BRepGraph_Revision::VertexChange::Operation::Remove)
  {
    return BRepGraph_EdgeId();
  }

  const uint32_t anEdgeIndex    = myNextEdgeIndex++;
  const uint32_t aStartRefIndex = myNextVertexRefIndex;
  const uint32_t anEndRefIndex  = aStartRefIndex + 1u;
  myNextVertexRefIndex += 2u;

  BRepGraph_Revision::EdgeChange anEdgeChange;
  anEdgeChange.Kind           = BRepGraph_Revision::VertexChange::Operation::Create;
  anEdgeChange.UID            = BRepGraph_UID(BRepGraph_NodeId::Kind::Edge, myNextEdgeCounter++);
  anEdgeChange.LocalId        = BRepGraph_EdgeId(anEdgeIndex);
  anEdgeChange.Definition.UID = anEdgeChange.UID.Counter;
  anEdgeChange.Definition.Tolerance        = theTolerance;
  anEdgeChange.Definition.StartVertexRefId = BRepGraph_VertexRefId(aStartRefIndex);
  anEdgeChange.Definition.EndVertexRefId   = BRepGraph_VertexRefId(anEndRefIndex);
  anEdgeChange.Curve                       = aCurveCopy;
  anEdgeChange.CurveParamFirst             = theParamFirst;
  anEdgeChange.CurveParamLast              = theParamLast;

  BRepGraph_Revision::VertexRefChange aStartRefChange;
  aStartRefChange.Kind = BRepGraph_Revision::VertexChange::Operation::Create;
  aStartRefChange.UID  = BRepGraph_RefUID(BRepGraph_RefId::Kind::Vertex, myNextVertexRefCounter++);
  aStartRefChange.LocalId                  = BRepGraph_VertexRefId(aStartRefIndex);
  aStartRefChange.Definition.UID           = aStartRefChange.UID.Counter;
  aStartRefChange.Definition.ChildVertexId = aStartVertex.LocalId;
  aStartRefChange.Definition.ParentEdgeId  = anEdgeChange.LocalId;

  BRepGraph_Revision::VertexRefChange anEndRefChange;
  anEndRefChange.Kind = BRepGraph_Revision::VertexChange::Operation::Create;
  anEndRefChange.UID  = BRepGraph_RefUID(BRepGraph_RefId::Kind::Vertex, myNextVertexRefCounter++);
  anEndRefChange.LocalId                           = BRepGraph_VertexRefId(anEndRefIndex);
  anEndRefChange.Definition.UID                    = anEndRefChange.UID.Counter;
  anEndRefChange.Definition.ChildVertexId          = anEndVertex.LocalId;
  anEndRefChange.Definition.ParentEdgeId           = anEdgeChange.LocalId;
  anEndRefChange.Definition.Orientation.IsReversed = true;

  recordEdgeChange(anEdgeChange);
  recordVertexRefChange(aStartRefChange);
  recordVertexRefChange(anEndRefChange);

  UIDRange anEdgeRange;
  anEdgeRange.NodeKind = BRepGraph_NodeId::Kind::Edge;
  anEdgeRange.First    = anEdgeChange.UID.Counter;
  anEdgeRange.Last     = anEdgeChange.UID.Counter;
  recordAllocation(anEdgeRange);

  UIDRange aRefRange;
  aRefRange.IsReference = true;
  aRefRange.RefKind     = BRepGraph_RefId::Kind::Vertex;
  aRefRange.First       = aStartRefChange.UID.Counter;
  aRefRange.Last        = anEndRefChange.UID.Counter;
  recordAllocation(aRefRange);
  return anEdgeChange.LocalId;
}

//=================================================================================================

BRepGraph_CoEdgeId BRepGraph_Transaction::AddCoEdge(
  const BRepGraph_UID&                  theEdgeUID,
  const BRepGraphInc::ParityOrientation theOrientation)
{
  if (!canEditTopology() || myNextCoEdgeCounter == 0xffffffffu || !theEdgeUID.IsValid()
      || theEdgeUID.Kind != BRepGraph_NodeId::Kind::Edge)
  {
    return BRepGraph_CoEdgeId();
  }

  BRepGraph_Revision::EdgeChange anEdgeChange;
  if (!resolveEdgeChange(theEdgeUID, anEdgeChange)
      || anEdgeChange.Kind == BRepGraph_Revision::VertexChange::Operation::Remove)
  {
    return BRepGraph_CoEdgeId();
  }

  BRepGraph_Revision::CoEdgeChange aChange;
  aChange.Kind           = BRepGraph_Revision::VertexChange::Operation::Create;
  aChange.UID            = BRepGraph_UID(BRepGraph_NodeId::Kind::CoEdge, myNextCoEdgeCounter++);
  aChange.LocalId        = BRepGraph_CoEdgeId(myNextCoEdgeIndex++);
  aChange.Definition.UID = aChange.UID.Counter;
  aChange.Definition.ChildEdgeId = anEdgeChange.LocalId;
  aChange.Definition.Orientation = theOrientation;
  recordCoEdgeChange(aChange);

  UIDRange aRange;
  aRange.NodeKind = BRepGraph_NodeId::Kind::CoEdge;
  aRange.First    = aChange.UID.Counter;
  aRange.Last     = aChange.UID.Counter;
  recordAllocation(aRange);
  return aChange.LocalId;
}

//=================================================================================================

BRepGraph_WireId BRepGraph_Transaction::AddWire(
  const NCollection_LinearVector<BRepGraph_UID>& theCoEdgeUIDs)
{
  if (!canEditTopology() || myNextWireCounter == 0xffffffffu)
  {
    return BRepGraph_WireId();
  }

  NCollection_LinearVector<BRepGraph_Revision::CoEdgeChange> aCoEdgeChanges;
  NCollection_FlatMap<BRepGraph_UID>                         aSeenCoEdges;
  aSeenCoEdges.Reserve(theCoEdgeUIDs.Size());
  for (const BRepGraph_UID& aCoEdgeUID : theCoEdgeUIDs)
  {
    if (!aCoEdgeUID.IsValid() || aCoEdgeUID.Kind != BRepGraph_NodeId::Kind::CoEdge)
    {
      return BRepGraph_WireId();
    }
    BRepGraph_Revision::CoEdgeChange aCoEdgeChange;
    if (!resolveCoEdgeChange(aCoEdgeUID, aCoEdgeChange)
        || aCoEdgeChange.Kind == BRepGraph_Revision::VertexChange::Operation::Remove
        || aCoEdgeChange.Definition.ParentWireId.IsValid())
    {
      return BRepGraph_WireId();
    }
    if (!aSeenCoEdges.Add(aCoEdgeUID))
    {
      return BRepGraph_WireId();
    }
    aCoEdgeChanges.Append(aCoEdgeChange);
  }

  BRepGraph_Revision::WireChange aWireChange;
  aWireChange.Kind           = BRepGraph_Revision::VertexChange::Operation::Create;
  aWireChange.UID            = BRepGraph_UID(BRepGraph_NodeId::Kind::Wire, myNextWireCounter++);
  aWireChange.LocalId        = BRepGraph_WireId(myNextWireIndex++);
  aWireChange.Definition.UID = aWireChange.UID.Counter;
  aWireChange.CoEdgeUIDs     = theCoEdgeUIDs;
  recordWireChange(aWireChange);

  for (const BRepGraph_Revision::CoEdgeChange& aResolvedChange : aCoEdgeChanges)
  {
    BRepGraph_Revision::CoEdgeChange        aChange        = aResolvedChange;
    const BRepGraph_Revision::CoEdgeChange* aPendingChange = coEdgeChangeOf(aResolvedChange.UID);
    if (aPendingChange == nullptr)
    {
      aChange.Kind = BRepGraph_Revision::VertexChange::Operation::Modify;
    }
    aChange.Definition.ParentWireId = aWireChange.LocalId;
    ++aChange.Definition.OwnGen;
    ++aChange.Definition.SubtreeGen;
    recordCoEdgeChange(aChange);
  }

  UIDRange aRange;
  aRange.NodeKind = BRepGraph_NodeId::Kind::Wire;
  aRange.First    = aWireChange.UID.Counter;
  aRange.Last     = aWireChange.UID.Counter;
  recordAllocation(aRange);
  return aWireChange.LocalId;
}

//=================================================================================================

BRepGraph_FaceId BRepGraph_Transaction::AddFace(
  const BRepGraph_UID&                           theOuterWireUID,
  const NCollection_LinearVector<BRepGraph_UID>& theInnerWireUIDs,
  const double                                   theTolerance)
{
  const size_t aWireCount = (theOuterWireUID.IsValid() ? 1u : 0u) + theInnerWireUIDs.Size();
  if (!canEditTopology() || myNextFaceCounter == 0xffffffffu
      || aWireCount > static_cast<size_t>(0xffffffffu - myNextWireRefCounter))
  {
    return BRepGraph_FaceId();
  }

  NCollection_LinearVector<BRepGraph_UID> aWireUIDs;
  NCollection_FlatMap<BRepGraph_UID>      aSeenWires;
  aSeenWires.Reserve(aWireCount);
  if (theOuterWireUID.IsValid())
  {
    aWireUIDs.Append(theOuterWireUID);
  }
  for (const BRepGraph_UID& aWireUID : theInnerWireUIDs)
  {
    aWireUIDs.Append(aWireUID);
  }
  if (!theOuterWireUID.IsValid() && !theInnerWireUIDs.IsEmpty())
  {
    return BRepGraph_FaceId();
  }
  for (size_t anIndex = 0; anIndex < aWireUIDs.Size(); ++anIndex)
  {
    const BRepGraph_UID& aWireUID = aWireUIDs.Value(anIndex);
    if (!aWireUID.IsValid() || aWireUID.Kind != BRepGraph_NodeId::Kind::Wire)
    {
      return BRepGraph_FaceId();
    }
    BRepGraph_Revision::WireChange aWireChange;
    if (!resolveWireChange(aWireUID, aWireChange)
        || aWireChange.Kind == BRepGraph_Revision::VertexChange::Operation::Remove)
    {
      return BRepGraph_FaceId();
    }
    if (!aSeenWires.Add(aWireUID))
    {
      return BRepGraph_FaceId();
    }
  }

  BRepGraph_Revision::FaceChange aFaceChange;
  aFaceChange.Kind           = BRepGraph_Revision::VertexChange::Operation::Create;
  aFaceChange.UID            = BRepGraph_UID(BRepGraph_NodeId::Kind::Face, myNextFaceCounter++);
  aFaceChange.LocalId        = BRepGraph_FaceId(myNextFaceIndex++);
  aFaceChange.Definition.UID = aFaceChange.UID.Counter;
  aFaceChange.Definition.Tolerance = theTolerance;

  for (size_t anIndex = 0; anIndex < aWireUIDs.Size(); ++anIndex)
  {
    BRepGraph_Revision::WireChange aWireChange;
    if (!resolveWireChange(aWireUIDs.Value(anIndex), aWireChange))
    {
      return BRepGraph_FaceId();
    }

    BRepGraph_Revision::WireRefChange aWireRefChange;
    aWireRefChange.Kind    = BRepGraph_Revision::VertexChange::Operation::Create;
    aWireRefChange.UID     = BRepGraph_RefUID(BRepGraph_RefId::Kind::Wire, myNextWireRefCounter++);
    aWireRefChange.LocalId = BRepGraph_WireRefId(myNextWireRefIndex++);
    aWireRefChange.Definition.UID          = aWireRefChange.UID.Counter;
    aWireRefChange.Definition.ParentFaceId = aFaceChange.LocalId;
    aWireRefChange.Definition.ChildWireId  = aWireChange.LocalId;
    aWireRefChange.Role                    = anIndex == 0 ? BRepGraph_Revision::BoundaryRole::Outer
                                                          : BRepGraph_Revision::BoundaryRole::Inner;
    aFaceChange.WireRefUIDs.Append(aWireRefChange.UID);
    recordWireRefChange(aWireRefChange);

    UIDRange aRefRange;
    aRefRange.IsReference = true;
    aRefRange.RefKind     = BRepGraph_RefId::Kind::Wire;
    aRefRange.First       = aWireRefChange.UID.Counter;
    aRefRange.Last        = aWireRefChange.UID.Counter;
    recordAllocation(aRefRange);
  }
  recordFaceChange(aFaceChange);

  UIDRange aFaceRange;
  aFaceRange.NodeKind = BRepGraph_NodeId::Kind::Face;
  aFaceRange.First    = aFaceChange.UID.Counter;
  aFaceRange.Last     = aFaceChange.UID.Counter;
  recordAllocation(aFaceRange);
  return aFaceChange.LocalId;
}

//=================================================================================================

bool BRepGraph_Transaction::SetVertexPoint(const BRepGraph_UID& theUID, const gp_Pnt& thePoint)
{
  BRepGraph_Revision::VertexChange aChange;
  if (!canEditTopology() || !theUID.IsValid() || theUID.Kind != BRepGraph_NodeId::Kind::Vertex
      || !resolveVertexChange(theUID, aChange)
      || aChange.Kind == BRepGraph_Revision::VertexChange::Operation::Remove)
  {
    return false;
  }

  const BRepGraph_Revision::VertexChange* aPendingChange = changeOf(theUID);
  if (aPendingChange == nullptr
      || aPendingChange->Kind != BRepGraph_Revision::VertexChange::Operation::Create)
  {
    aChange.Kind = BRepGraph_Revision::VertexChange::Operation::Modify;
  }
  aChange.Definition.Point = thePoint;
  ++aChange.Definition.OwnGen;
  ++aChange.Definition.SubtreeGen;
  recordChange(aChange);
  return true;
}

//=================================================================================================

bool BRepGraph_Transaction::SetVertexTolerance(const BRepGraph_UID& theUID,
                                               const double         theTolerance)
{
  BRepGraph_Revision::VertexChange aChange;
  if (!canEditTopology() || !theUID.IsValid() || theUID.Kind != BRepGraph_NodeId::Kind::Vertex
      || !resolveVertexChange(theUID, aChange)
      || aChange.Kind == BRepGraph_Revision::VertexChange::Operation::Remove)
  {
    return false;
  }

  const BRepGraph_Revision::VertexChange* aPendingChange = changeOf(theUID);
  if (aPendingChange == nullptr
      || aPendingChange->Kind != BRepGraph_Revision::VertexChange::Operation::Create)
  {
    aChange.Kind = BRepGraph_Revision::VertexChange::Operation::Modify;
  }
  aChange.Definition.Tolerance = theTolerance;
  ++aChange.Definition.OwnGen;
  ++aChange.Definition.SubtreeGen;
  recordChange(aChange);
  return true;
}

//=================================================================================================

bool BRepGraph_Transaction::RemoveVertex(const BRepGraph_UID& theUID)
{
  BRepGraph_Revision::VertexChange aChange;
  if (!canEditTopology() || !theUID.IsValid() || theUID.Kind != BRepGraph_NodeId::Kind::Vertex
      || !resolveVertexChange(theUID, aChange)
      || aChange.Kind == BRepGraph_Revision::VertexChange::Operation::Remove)
  {
    return false;
  }

  const BRepGraph_Revision::VertexChange* aPendingChange = changeOf(theUID);
  if (aPendingChange != nullptr
      && aPendingChange->Kind == BRepGraph_Revision::VertexChange::Operation::Create)
  {
    return false;
  }

  const auto resolveRefLocal = [&](const uint32_t                       theIndex,
                                   BRepGraph_Revision::VertexRefChange& theChange) {
    for (size_t anIndex = myVertexRefChanges.Size(); anIndex > 0; --anIndex)
    {
      const BRepGraph_Revision::VertexRefChange& aChange = myVertexRefChanges.Value(anIndex - 1);
      if (aChange.LocalId.Index == theIndex)
      {
        theChange = aChange;
        return true;
      }
    }
    return myBaseRevision->ResolveVertexRefLocal(theIndex, theChange);
  };
  const auto hasEndpoint = [&](const BRepGraph_Revision::EdgeChange& theEdge) {
    if (theEdge.Kind == BRepGraph_Revision::VertexChange::Operation::Remove)
    {
      return false;
    }
    BRepGraph_Revision::VertexRefChange aStartRef;
    BRepGraph_Revision::VertexRefChange anEndRef;
    if (!resolveRefLocal(theEdge.Definition.StartVertexRefId.Index, aStartRef)
        || !resolveRefLocal(theEdge.Definition.EndVertexRefId.Index, anEndRef))
    {
      return false;
    }
    return aStartRef.Definition.ChildVertexId == aChange.LocalId
           || anEndRef.Definition.ChildVertexId == aChange.LocalId;
  };
  for (const BRepGraph_Revision::EdgeView& aBaseEdge : myBaseRevision->VisibleEdges())
  {
    const BRepGraph_UID&                  anEdgeUID = aBaseEdge.UID;
    BRepGraph_Revision::EdgeChange        anEdge;
    const BRepGraph_Revision::EdgeChange* aPendingEdge = edgeChangeOf(anEdgeUID);
    if (aPendingEdge != nullptr)
    {
      anEdge = *aPendingEdge;
    }
    else
    {
      anEdge.Kind            = aBaseEdge.Kind;
      anEdge.UID             = aBaseEdge.UID;
      anEdge.LocalId         = aBaseEdge.LocalId;
      anEdge.Definition      = aBaseEdge.Definition;
      anEdge.CurveParamFirst = aBaseEdge.CurveParamFirst;
      anEdge.CurveParamLast  = aBaseEdge.CurveParamLast;
    }
    if (hasEndpoint(anEdge))
    {
      return false;
    }
  }
  for (const BRepGraph_Revision::EdgeChange& anEdge : myEdgeChanges)
  {
    if (hasEndpoint(anEdge))
    {
      return false;
    }
  }

  aChange.Kind = BRepGraph_Revision::VertexChange::Operation::Remove;
  recordChange(aChange);
  return true;
}

//=================================================================================================

bool BRepGraph_Transaction::SetEdgeTolerance(const BRepGraph_UID& theUID, const double theTolerance)
{
  BRepGraph_Revision::EdgeChange aChange;
  if (!canEditTopology() || !theUID.IsValid() || theUID.Kind != BRepGraph_NodeId::Kind::Edge
      || !resolveEdgeChange(theUID, aChange)
      || aChange.Kind == BRepGraph_Revision::VertexChange::Operation::Remove)
  {
    return false;
  }

  const BRepGraph_Revision::EdgeChange* aPendingChange = edgeChangeOf(theUID);
  if (aPendingChange == nullptr
      || aPendingChange->Kind != BRepGraph_Revision::VertexChange::Operation::Create)
  {
    aChange.Kind = BRepGraph_Revision::VertexChange::Operation::Modify;
  }
  aChange.Definition.Tolerance = theTolerance;
  ++aChange.Definition.OwnGen;
  ++aChange.Definition.SubtreeGen;
  recordEdgeChange(aChange);
  return true;
}

//=================================================================================================

bool BRepGraph_Transaction::SetEdgeCurve(const BRepGraph_UID&           theUID,
                                         const occ::handle<Geom_Curve>& theCurve,
                                         const double                   theParamFirst,
                                         const double                   theParamLast)
{
  if (!canEditTopology() || !theUID.IsValid() || theUID.Kind != BRepGraph_NodeId::Kind::Edge
      || theCurve.IsNull())
  {
    return false;
  }

  const occ::handle<Geom_Curve> aCurveCopy = occ::down_cast<Geom_Curve>(theCurve->Copy());
  if (aCurveCopy.IsNull())
  {
    return false;
  }

  BRepGraph_Revision::EdgeChange aChange;
  if (!resolveEdgeChange(theUID, aChange)
      || aChange.Kind == BRepGraph_Revision::VertexChange::Operation::Remove)
  {
    return false;
  }

  const BRepGraph_Revision::EdgeChange* aPendingChange = edgeChangeOf(theUID);
  if (aPendingChange == nullptr
      || aPendingChange->Kind != BRepGraph_Revision::VertexChange::Operation::Create)
  {
    aChange.Kind = BRepGraph_Revision::VertexChange::Operation::Modify;
  }
  aChange.Curve           = aCurveCopy;
  aChange.CurveParamFirst = theParamFirst;
  aChange.CurveParamLast  = theParamLast;
  aChange.HasCurveUpdate  = true;
  ++aChange.Definition.OwnGen;
  ++aChange.Definition.SubtreeGen;
  recordEdgeChange(aChange);
  return true;
}

//=================================================================================================

bool BRepGraph_Transaction::RemoveEdge(const BRepGraph_UID& theUID)
{
  BRepGraph_Revision::EdgeChange anEdgeChange;
  if (!canEditTopology() || !theUID.IsValid() || theUID.Kind != BRepGraph_NodeId::Kind::Edge
      || !resolveEdgeChange(theUID, anEdgeChange)
      || anEdgeChange.Kind == BRepGraph_Revision::VertexChange::Operation::Remove)
  {
    return false;
  }
  const BRepGraph_Revision::EdgeChange* aPendingChange = edgeChangeOf(theUID);
  if (aPendingChange != nullptr
      && aPendingChange->Kind == BRepGraph_Revision::VertexChange::Operation::Create)
  {
    return false;
  }

  const auto hasCoEdgeUse = [&](const BRepGraph_Revision::CoEdgeChange& theCoEdge) {
    return theCoEdge.Kind != BRepGraph_Revision::VertexChange::Operation::Remove
           && theCoEdge.Definition.ChildEdgeId == anEdgeChange.LocalId;
  };
  for (const BRepGraph_Revision::CoEdgeChange& aBaseCoEdge : myBaseRevision->VisibleCoEdges())
  {
    if (aBaseCoEdge.Definition.ChildEdgeId == anEdgeChange.LocalId)
    {
      const BRepGraph_Revision::CoEdgeChange* aPendingCoEdge = coEdgeChangeOf(aBaseCoEdge.UID);
      if (aPendingCoEdge == nullptr
          || aPendingCoEdge->Kind != BRepGraph_Revision::VertexChange::Operation::Remove)
      {
        return false;
      }
    }
  }
  for (const BRepGraph_Revision::CoEdgeChange& aCoEdgeChange : myCoEdgeChanges)
  {
    if (hasCoEdgeUse(aCoEdgeChange))
    {
      return false;
    }
  }

  anEdgeChange.Kind = BRepGraph_Revision::VertexChange::Operation::Remove;
  recordEdgeChange(anEdgeChange);

  const BRepGraph_VertexRefId aStartRefId = anEdgeChange.Definition.StartVertexRefId;
  const BRepGraph_VertexRefId anEndRefId  = anEdgeChange.Definition.EndVertexRefId;
  const BRepGraph_VertexRefId aRefIds[2]  = {aStartRefId, anEndRefId};
  for (const BRepGraph_VertexRefId aRefId : aRefIds)
  {
    BRepGraph_Revision::VertexRefChange aRefChange;
    if (!myBaseRevision->ResolveVertexRefLocal(aRefId.Index, aRefChange)
        || aRefChange.Kind == BRepGraph_Revision::VertexChange::Operation::Remove)
    {
      return false;
    }
    aRefChange.Kind = BRepGraph_Revision::VertexChange::Operation::Remove;
    recordVertexRefChange(aRefChange);
  }
  return true;
}

//=================================================================================================

bool BRepGraph_Transaction::SetCoEdgeOrientation(
  const BRepGraph_UID&                  theUID,
  const BRepGraphInc::ParityOrientation theOrientation)
{
  BRepGraph_Revision::CoEdgeChange aChange;
  if (!canEditTopology() || !theUID.IsValid() || theUID.Kind != BRepGraph_NodeId::Kind::CoEdge
      || !resolveCoEdgeChange(theUID, aChange)
      || aChange.Kind == BRepGraph_Revision::VertexChange::Operation::Remove
      || aChange.Definition.ParentWireId.IsValid())
  {
    return false;
  }

  const BRepGraph_Revision::CoEdgeChange* aPendingChange = coEdgeChangeOf(theUID);
  if (aPendingChange == nullptr
      || aPendingChange->Kind != BRepGraph_Revision::VertexChange::Operation::Create)
  {
    aChange.Kind = BRepGraph_Revision::VertexChange::Operation::Modify;
  }
  aChange.Definition.Orientation = theOrientation;
  ++aChange.Definition.OwnGen;
  ++aChange.Definition.SubtreeGen;
  recordCoEdgeChange(aChange);
  return true;
}

//=================================================================================================

bool BRepGraph_Transaction::RemoveCoEdge(const BRepGraph_UID& theUID)
{
  BRepGraph_Revision::CoEdgeChange aChange;
  if (!canEditTopology() || !theUID.IsValid() || theUID.Kind != BRepGraph_NodeId::Kind::CoEdge
      || !resolveCoEdgeChange(theUID, aChange)
      || aChange.Kind == BRepGraph_Revision::VertexChange::Operation::Remove
      || aChange.Definition.ParentWireId.IsValid())
  {
    return false;
  }

  const BRepGraph_Revision::CoEdgeChange* aPendingChange = coEdgeChangeOf(theUID);
  if (aPendingChange != nullptr
      && aPendingChange->Kind == BRepGraph_Revision::VertexChange::Operation::Create)
  {
    return false;
  }

  aChange.Kind = BRepGraph_Revision::VertexChange::Operation::Remove;
  recordCoEdgeChange(aChange);
  return true;
}

//=================================================================================================

bool BRepGraph_Transaction::SetWireCoEdgeOrder(
  const BRepGraph_UID&                           theUID,
  const NCollection_LinearVector<BRepGraph_UID>& theCoEdgeUIDs)
{
  BRepGraph_Revision::WireChange aWireChange;
  if (!canEditTopology() || !theUID.IsValid() || theUID.Kind != BRepGraph_NodeId::Kind::Wire
      || !resolveWireChange(theUID, aWireChange)
      || aWireChange.Kind == BRepGraph_Revision::VertexChange::Operation::Remove
      || aWireChange.CoEdgeUIDs.Size() != theCoEdgeUIDs.Size())
  {
    return false;
  }
  for (const BRepGraph_UID& aCoEdgeUID : theCoEdgeUIDs)
  {
    BRepGraph_Revision::CoEdgeChange aCoEdgeChange;
    if (!aCoEdgeUID.IsValid() || aCoEdgeUID.Kind != BRepGraph_NodeId::Kind::CoEdge
        || !resolveCoEdgeChange(aCoEdgeUID, aCoEdgeChange)
        || aCoEdgeChange.Kind == BRepGraph_Revision::VertexChange::Operation::Remove
        || aCoEdgeChange.Definition.ParentWireId != aWireChange.LocalId)
    {
      return false;
    }
    for (const BRepGraph_UID& anotherUID : theCoEdgeUIDs)
    {
      if (&anotherUID != &aCoEdgeUID && anotherUID == aCoEdgeUID)
      {
        return false;
      }
    }
  }
  for (const BRepGraph_UID& aCurrentUID : aWireChange.CoEdgeUIDs)
  {
    bool isPresent = false;
    for (const BRepGraph_UID& aCandidateUID : theCoEdgeUIDs)
    {
      if (aCurrentUID == aCandidateUID)
      {
        isPresent = true;
        break;
      }
    }
    if (!isPresent)
    {
      return false;
    }
  }

  const BRepGraph_Revision::WireChange* aPendingChange = wireChangeOf(theUID);
  if (aPendingChange == nullptr)
  {
    aWireChange.Kind = BRepGraph_Revision::VertexChange::Operation::Modify;
  }
  aWireChange.CoEdgeUIDs = theCoEdgeUIDs;
  ++aWireChange.Definition.OwnGen;
  ++aWireChange.Definition.SubtreeGen;
  recordWireChange(aWireChange);
  return true;
}

//=================================================================================================

bool BRepGraph_Transaction::SetFaceWireOrder(
  const BRepGraph_UID&                              theUID,
  const NCollection_LinearVector<BRepGraph_RefUID>& theWireRefUIDs)
{
  BRepGraph_Revision::FaceChange aFaceChange;
  if (!canEditTopology() || !theUID.IsValid() || theUID.Kind != BRepGraph_NodeId::Kind::Face
      || !resolveFaceChange(theUID, aFaceChange)
      || aFaceChange.Kind == BRepGraph_Revision::VertexChange::Operation::Remove
      || aFaceChange.WireRefUIDs.Size() != theWireRefUIDs.Size())
  {
    return false;
  }
  for (size_t anIndex = 0; anIndex < theWireRefUIDs.Size(); ++anIndex)
  {
    const BRepGraph_RefUID&           aWireRefUID = theWireRefUIDs.Value(anIndex);
    BRepGraph_Revision::WireRefChange aWireRefChange;
    if (!aWireRefUID.IsValid() || aWireRefUID.Kind != BRepGraph_RefId::Kind::Wire
        || !resolveWireRefChange(aWireRefUID, aWireRefChange)
        || aWireRefChange.Kind == BRepGraph_Revision::VertexChange::Operation::Remove
        || aWireRefChange.Definition.ParentFaceId != aFaceChange.LocalId)
    {
      return false;
    }
    for (size_t anotherIndex = anIndex + 1; anotherIndex < theWireRefUIDs.Size(); ++anotherIndex)
    {
      if (theWireRefUIDs.Value(anotherIndex) == aWireRefUID)
      {
        return false;
      }
    }
  }
  for (const BRepGraph_RefUID& aCurrentUID : aFaceChange.WireRefUIDs)
  {
    bool isPresent = false;
    for (const BRepGraph_RefUID& aCandidateUID : theWireRefUIDs)
    {
      if (aCurrentUID == aCandidateUID)
      {
        isPresent = true;
        break;
      }
    }
    if (!isPresent)
    {
      return false;
    }
  }
  if (!aFaceChange.WireRefUIDs.IsEmpty() && !theWireRefUIDs.IsEmpty())
  {
    BRepGraph_Revision::WireRefChange aOldOuter;
    BRepGraph_Revision::WireRefChange aNewOuter;
    if (!resolveWireRefChange(aFaceChange.WireRefUIDs.First(), aOldOuter)
        || !resolveWireRefChange(theWireRefUIDs.First(), aNewOuter)
        || aOldOuter.Role != BRepGraph_Revision::BoundaryRole::Outer
        || aNewOuter.UID != aOldOuter.UID)
    {
      return false;
    }
  }

  const BRepGraph_Revision::FaceChange* aPendingChange = faceChangeOf(theUID);
  if (aPendingChange == nullptr)
  {
    aFaceChange.Kind = BRepGraph_Revision::VertexChange::Operation::Modify;
  }
  aFaceChange.WireRefUIDs = theWireRefUIDs;
  ++aFaceChange.Definition.OwnGen;
  ++aFaceChange.Definition.SubtreeGen;
  recordFaceChange(aFaceChange);
  return true;
}

//=================================================================================================

bool BRepGraph_Transaction::SetWireRefOrientation(
  const BRepGraph_RefUID&               theUID,
  const BRepGraphInc::ParityOrientation theOrientation)
{
  BRepGraph_Revision::WireRefChange aChange;
  if (!canEditTopology() || !theUID.IsValid() || theUID.Kind != BRepGraph_RefId::Kind::Wire
      || !resolveWireRefChange(theUID, aChange)
      || aChange.Kind == BRepGraph_Revision::VertexChange::Operation::Remove)
  {
    return false;
  }
  const BRepGraph_Revision::WireRefChange* aPendingChange = wireRefChangeOf(theUID);
  if (aPendingChange == nullptr)
  {
    aChange.Kind = BRepGraph_Revision::VertexChange::Operation::Modify;
  }
  aChange.Definition.Orientation = theOrientation;
  recordWireRefChange(aChange);
  return true;
}

//=================================================================================================

bool BRepGraph_Transaction::RemoveWireRef(const BRepGraph_RefUID& theUID)
{
  BRepGraph_Revision::WireRefChange aChange;
  if (!canEditTopology() || !theUID.IsValid() || theUID.Kind != BRepGraph_RefId::Kind::Wire
      || !resolveWireRefChange(theUID, aChange)
      || aChange.Kind == BRepGraph_Revision::VertexChange::Operation::Remove)
  {
    return false;
  }
  const BRepGraph_Revision::WireRefChange* aPendingChange = wireRefChangeOf(theUID);
  if (aPendingChange != nullptr
      && aPendingChange->Kind == BRepGraph_Revision::VertexChange::Operation::Create)
  {
    return false;
  }

  BRepGraph_Revision::FaceChange aFaceChange;
  bool                           hasFace = false;
  for (size_t anIndex = myFaceChanges.Size(); anIndex > 0; --anIndex)
  {
    const BRepGraph_Revision::FaceChange& aPending = myFaceChanges.Value(anIndex - 1);
    if (aPending.LocalId == aChange.Definition.ParentFaceId)
    {
      aFaceChange = aPending;
      hasFace     = aPending.Kind != BRepGraph_Revision::VertexChange::Operation::Remove;
      break;
    }
  }
  if (!hasFace)
  {
    hasFace = myBaseRevision->ResolveFaceLocal(aChange.Definition.ParentFaceId.Index, aFaceChange)
              && aFaceChange.Kind != BRepGraph_Revision::VertexChange::Operation::Remove;
  }
  if (!hasFace)
  {
    return false;
  }
  NCollection_LinearVector<BRepGraph_RefUID> aRemaining;
  for (const BRepGraph_RefUID& aWireRefUID : aFaceChange.WireRefUIDs)
  {
    if (aWireRefUID != theUID)
    {
      aRemaining.Append(aWireRefUID);
    }
  }
  if (aRemaining.Size() == aFaceChange.WireRefUIDs.Size())
  {
    return false;
  }
  if (aChange.Role == BRepGraph_Revision::BoundaryRole::Outer && !aRemaining.IsEmpty())
  {
    return false;
  }
  aChange.Kind = BRepGraph_Revision::VertexChange::Operation::Remove;
  recordWireRefChange(aChange);

  const BRepGraph_Revision::FaceChange* aPendingFace = faceChangeOf(aFaceChange.UID);
  if (aPendingFace == nullptr)
  {
    aFaceChange.Kind = BRepGraph_Revision::VertexChange::Operation::Modify;
  }
  aFaceChange.WireRefUIDs = aRemaining;
  ++aFaceChange.Definition.OwnGen;
  ++aFaceChange.Definition.SubtreeGen;
  recordFaceChange(aFaceChange);
  return true;
}

//=================================================================================================

bool BRepGraph_Transaction::RemoveWire(const BRepGraph_UID& theUID)
{
  BRepGraph_Revision::WireChange aChange;
  if (!canEditTopology() || !theUID.IsValid() || theUID.Kind != BRepGraph_NodeId::Kind::Wire
      || !resolveWireChange(theUID, aChange)
      || aChange.Kind == BRepGraph_Revision::VertexChange::Operation::Remove)
  {
    return false;
  }
  const BRepGraph_Revision::WireChange* aPendingChange = wireChangeOf(theUID);
  if (aPendingChange != nullptr
      && aPendingChange->Kind == BRepGraph_Revision::VertexChange::Operation::Create)
  {
    return false;
  }
  if (!aChange.CoEdgeUIDs.IsEmpty())
  {
    return false;
  }
  for (const BRepGraph_Revision::WireRefChange& aBaseWireRef : myBaseRevision->VisibleWireRefs())
  {
    if (aBaseWireRef.Definition.ChildWireId == aChange.LocalId)
    {
      const BRepGraph_Revision::WireRefChange* aPendingRef = wireRefChangeOf(aBaseWireRef.UID);
      if (aPendingRef == nullptr
          || aPendingRef->Kind != BRepGraph_Revision::VertexChange::Operation::Remove)
      {
        return false;
      }
    }
  }
  for (const BRepGraph_Revision::WireRefChange& aWireRefChange : myWireRefChanges)
  {
    if (aWireRefChange.Kind != BRepGraph_Revision::VertexChange::Operation::Remove
        && aWireRefChange.Definition.ChildWireId == aChange.LocalId)
    {
      return false;
    }
  }
  aChange.Kind = BRepGraph_Revision::VertexChange::Operation::Remove;
  recordWireChange(aChange);
  return true;
}

//=================================================================================================

bool BRepGraph_Transaction::RemoveFace(const BRepGraph_UID& theUID)
{
  BRepGraph_Revision::FaceChange aChange;
  if (!canEditTopology() || !theUID.IsValid() || theUID.Kind != BRepGraph_NodeId::Kind::Face
      || !resolveFaceChange(theUID, aChange)
      || aChange.Kind == BRepGraph_Revision::VertexChange::Operation::Remove)
  {
    return false;
  }
  const BRepGraph_Revision::FaceChange* aPendingChange = faceChangeOf(theUID);
  if (aPendingChange != nullptr
      && aPendingChange->Kind == BRepGraph_Revision::VertexChange::Operation::Create)
  {
    return false;
  }
  if (!aChange.WireRefUIDs.IsEmpty())
  {
    return false;
  }
  aChange.Kind = BRepGraph_Revision::VertexChange::Operation::Remove;
  recordFaceChange(aChange);
  return true;
}

//=================================================================================================

bool BRepGraph_Transaction::RaiseVertexUIDWatermark(const uint32_t theNextCounter)
{
  if (!canEditTopology() || theNextCounter == 0 || theNextCounter < myNextVertexCounter)
  {
    return false;
  }
  if (theNextCounter == myNextVertexCounter)
  {
    return true;
  }

  UIDRange aRange;
  aRange.IsReference = false;
  aRange.NodeKind    = BRepGraph_NodeId::Kind::Vertex;
  aRange.First       = myNextVertexCounter;
  aRange.Last        = theNextCounter - 1u;
  recordAllocation(aRange);
  myNextVertexCounter = theNextCounter;
  return true;
}

//=================================================================================================

bool BRepGraph_Transaction::Vertex(const BRepGraph_UID&     theUID,
                                   BRepGraphInc::VertexDef& theDefinition) const
{
  return IsValid() && theUID.IsValid() && theUID.Kind == BRepGraph_NodeId::Kind::Vertex
         && resolveVertex(theUID, theDefinition);
}

//=================================================================================================

bool BRepGraph_Transaction::Edge(const BRepGraph_UID&   theUID,
                                 BRepGraphInc::EdgeDef& theDefinition) const
{
  BRepGraph_Revision::EdgeChange aChange;
  if (!IsValid() || !theUID.IsValid() || theUID.Kind != BRepGraph_NodeId::Kind::Edge
      || !resolveEdgeChange(theUID, aChange)
      || aChange.Kind == BRepGraph_Revision::VertexChange::Operation::Remove)
  {
    return false;
  }
  theDefinition = aChange.Definition;
  return true;
}

//=================================================================================================

bool BRepGraph_Transaction::Wire(const BRepGraph_UID&            theUID,
                                 BRepGraph_Revision::WireChange& theChange) const
{
  return IsValid() && theUID.IsValid() && theUID.Kind == BRepGraph_NodeId::Kind::Wire
         && resolveWireChange(theUID, theChange)
         && theChange.Kind != BRepGraph_Revision::VertexChange::Operation::Remove;
}

//=================================================================================================

bool BRepGraph_Transaction::WireUID(const BRepGraph_WireId theLocalId, BRepGraph_UID& theUID) const
{
  if (!IsValid() || !theLocalId.IsValid())
  {
    return false;
  }
  for (size_t anIndex = myWireChanges.Size(); anIndex > 0; --anIndex)
  {
    const BRepGraph_Revision::WireChange& aChange = myWireChanges.Value(anIndex - 1);
    if (aChange.LocalId == theLocalId
        && aChange.Kind != BRepGraph_Revision::VertexChange::Operation::Remove)
    {
      theUID = aChange.UID;
      return true;
    }
  }
  BRepGraph_Revision::WireChange aChange;
  if (!myBaseRevision->ResolveWireLocal(theLocalId.Index, aChange)
      || aChange.Kind == BRepGraph_Revision::VertexChange::Operation::Remove)
  {
    return false;
  }
  theUID = aChange.UID;
  return true;
}

//=================================================================================================

bool BRepGraph_Transaction::Face(const BRepGraph_UID&            theUID,
                                 BRepGraph_Revision::FaceChange& theChange) const
{
  return IsValid() && theUID.IsValid() && theUID.Kind == BRepGraph_NodeId::Kind::Face
         && resolveFaceChange(theUID, theChange)
         && theChange.Kind != BRepGraph_Revision::VertexChange::Operation::Remove;
}

//=================================================================================================

BRepGraph_Transaction::CommitResult BRepGraph_Transaction::Commit()
{
  CommitResult aResult;
  if (myFinished)
  {
    aResult.Status = BRepGraph_RevisionStatus::Code::AlreadyFinished;
    appendDiagnostic(aResult.Diagnostics,
                     "EditFinished",
                     "The transaction has already been committed or aborted");
    return aResult;
  }
  if (myStatus != BRepGraph_RevisionStatus::Code::Ok || myBaseRevision.IsNull())
  {
    aResult.Status      = myStatus;
    aResult.Diagnostics = myDiagnostics;
    finish();
    return aResult;
  }

  if (myWorkingGraph != nullptr)
  {
    BRepGraph_Revision::CreateResult aCreated =
      BRepGraph_Revision::FromTransactionGraph(std::move(myWorkingGraph),
                                               myBaseRevision->SchemaVersion());
    if (!aCreated.IsOk())
    {
      aResult.Status      = aCreated.Status;
      aResult.Diagnostics = std::move(aCreated.Diagnostics);
      finish();
      return aResult;
    }

    aResult.Revision = aCreated.Revision;
    aResult.Diff     = myBaseRevision->Diff(*aResult.Revision);
    // A semantic diff intentionally omits physical metadata such as durable UID
    // watermarks and tombstone layout.  Reuse the base only when both semantic
    // and physical revision identities are unchanged.
    if (aResult.Diff.IsEmpty() && aResult.Revision->GraphGUID() == myBaseRevision->GraphGUID()
        && aResult.Revision->StorageRootHash() == myBaseRevision->StorageRootHash())
    {
      aResult.Revision = myBaseRevision;
    }
    aResult.Status = BRepGraph_RevisionStatus::Code::Ok;
    finish();
    return aResult;
  }

  const bool isWatermarkChanged =
    myNextVertexCounter != myBaseRevision->NextVertexCounter()
    || myNextEdgeCounter != myBaseRevision->NextEdgeCounter()
    || myNextVertexRefCounter != myBaseRevision->NextVertexRefCounter()
    || myNextCoEdgeCounter != myBaseRevision->NextCoEdgeCounter()
    || myNextWireCounter != myBaseRevision->NextWireCounter()
    || myNextFaceCounter != myBaseRevision->NextFaceCounter()
    || myNextWireRefCounter != myBaseRevision->NextWireRefCounter();
  if (myChanges.IsEmpty() && myEdgeChanges.IsEmpty() && myVertexRefChanges.IsEmpty()
      && myCoEdgeChanges.IsEmpty() && myWireChanges.IsEmpty() && myFaceChanges.IsEmpty()
      && myWireRefChanges.IsEmpty() && !isWatermarkChanged && !myComponentsChanged)
  {
    aResult.Revision           = myBaseRevision;
    aResult.AllocatedUIDRanges = myAllocatedRanges;
    aResult.Status             = BRepGraph_RevisionStatus::Code::Ok;
    finish();
    return aResult;
  }

  const bool hasCoreChanges = !myChanges.IsEmpty() || !myEdgeChanges.IsEmpty()
                              || !myVertexRefChanges.IsEmpty() || !myCoEdgeChanges.IsEmpty()
                              || !myWireChanges.IsEmpty() || !myFaceChanges.IsEmpty()
                              || !myWireRefChanges.IsEmpty() || isWatermarkChanged;
  if (hasCoreChanges && !myBaseRevision->SupportsSparseEdits())
  {
    aResult.Status = BRepGraph_RevisionStatus::Code::UnsupportedComponent;
    appendDiagnostic(aResult.Diagnostics,
                     "SparseComponentUnavailable",
                     "This revision does not support sparse topology operations; use Graph()");
    finish();
    return aResult;
  }
  occ::handle<BRepGraph_Revision> aNewRevision =
    hasCoreChanges
      ? BRepGraph_Revision::FromNativeChanges(myBaseRevision,
                                              myChanges,
                                              myEdgeChanges,
                                              myVertexRefChanges,
                                              myCoEdgeChanges,
                                              myWireChanges,
                                              myFaceChanges,
                                              myWireRefChanges,
                                              myNextVertexCounter,
                                              myNextEdgeCounter,
                                              myNextVertexRefCounter,
                                              myNextCoEdgeCounter,
                                              myNextWireCounter,
                                              myNextFaceCounter,
                                              myNextWireRefCounter,
                                              myComponents,
                                              aResult.Diagnostics)
      : BRepGraph_Revision::FromComponents(myBaseRevision, myComponents, aResult.Diagnostics);
  if (aNewRevision.IsNull())
  {
    aResult.Status = BRepGraph_RevisionStatus::Code::ValidationFailed;
    finish();
    return aResult;
  }

  aResult.Revision           = aNewRevision;
  aResult.Diff               = sparseDiff(*aNewRevision);
  aResult.AllocatedUIDRanges = myAllocatedRanges;
  aResult.Status             = BRepGraph_RevisionStatus::Code::Ok;
  finish();
  return aResult;
}

//=================================================================================================

void BRepGraph_Transaction::Abort() noexcept
{
  finish();
}

//=================================================================================================

void BRepGraph_Transaction::finish() noexcept
{
  myWorkingGraph.reset();
  myChanges.Clear();
  myEdgeChanges.Clear();
  myVertexRefChanges.Clear();
  myCoEdgeChanges.Clear();
  myWireChanges.Clear();
  myFaceChanges.Clear();
  myWireRefChanges.Clear();
  clearChangeIndices();
  myComponents.Clear();
  myComponentIndices.Clear();
  myChangedComponentGUIDs.Clear();
  clearAllocations();
  myBaseRevision.Nullify();
  myFinished = true;
}

//=================================================================================================

BRepGraph_RevisionDiff BRepGraph_Transaction::sparseDiff(
  const BRepGraph_Revision& theResultRevision) const
{
  using Operation = BRepGraph_Revision::VertexChange::Operation;

  BRepGraph_RevisionDiff aDiff;
  const BRepGraph&       aBaseGraph   = myBaseRevision->CoreGraph();
  const BRepGraph&       aResultGraph = theResultRevision.CoreGraph();

  const auto appendNode = [&aDiff, &aBaseGraph, &aResultGraph](const BRepGraph_UID&   theUID,
                                                               const BRepGraph_NodeId theLocalId,
                                                               const Operation theOperation) {
    switch (theOperation)
    {
      case Operation::Create:
        aDiff.CreatedUIDs.Append(theUID);
        break;
      case Operation::Modify:
        if (BRepGraph_RevisionHash::Hasher::Node(aBaseGraph, theLocalId)
            != BRepGraph_RevisionHash::Hasher::Node(aResultGraph, theLocalId))
        {
          aDiff.ModifiedUIDs.Append(theUID);
        }
        break;
      case Operation::Remove:
        aDiff.RemovedUIDs.Append(theUID);
        break;
    }
  };
  const auto appendReference =
    [&aDiff, &aBaseGraph, &aResultGraph](const BRepGraph_RefUID& theUID,
                                         const BRepGraph_RefId   theLocalId,
                                         const Operation         theOperation) {
      switch (theOperation)
      {
        case Operation::Create:
          aDiff.CreatedRefUIDs.Append(theUID);
          break;
        case Operation::Modify:
          if (BRepGraph_RevisionHash::Hasher::Reference(aBaseGraph, theLocalId)
              != BRepGraph_RevisionHash::Hasher::Reference(aResultGraph, theLocalId))
          {
            aDiff.ModifiedRefUIDs.Append(theUID);
          }
          break;
        case Operation::Remove:
          aDiff.RemovedRefUIDs.Append(theUID);
          break;
      }
    };
  const auto changeKind = [](const Operation theOperation) {
    switch (theOperation)
    {
      case Operation::Create:
        return BRepGraph_RevisionDiff::ChangeKind::Created;
      case Operation::Modify:
        return BRepGraph_RevisionDiff::ChangeKind::Modified;
      case Operation::Remove:
        return BRepGraph_RevisionDiff::ChangeKind::Removed;
    }
    return BRepGraph_RevisionDiff::ChangeKind::Modified;
  };

  for (const BRepGraph_Revision::VertexChange& aChange : myChanges)
  {
    appendNode(aChange.UID, BRepGraph_NodeId(aChange.LocalId), aChange.Kind);
  }
  for (const BRepGraph_Revision::EdgeChange& aChange : myEdgeChanges)
  {
    appendNode(aChange.UID, BRepGraph_NodeId(aChange.LocalId), aChange.Kind);
  }
  for (const BRepGraph_Revision::CoEdgeChange& aChange : myCoEdgeChanges)
  {
    const size_t aPreviousChangeCount = aDiff.ModifiedUIDs.Size();
    appendNode(aChange.UID, BRepGraph_NodeId(aChange.LocalId), aChange.Kind);
    if (aChange.Kind != Operation::Modify || aDiff.ModifiedUIDs.Size() != aPreviousChangeCount)
    {
      aDiff.UsageLinkChanges.Append({aChange.UID, changeKind(aChange.Kind)});
    }
  }
  for (const BRepGraph_Revision::WireChange& aChange : myWireChanges)
  {
    appendNode(aChange.UID, BRepGraph_NodeId(aChange.LocalId), aChange.Kind);
  }
  for (const BRepGraph_Revision::FaceChange& aChange : myFaceChanges)
  {
    appendNode(aChange.UID, BRepGraph_NodeId(aChange.LocalId), aChange.Kind);
  }
  for (const BRepGraph_Revision::VertexRefChange& aChange : myVertexRefChanges)
  {
    appendReference(aChange.UID, BRepGraph_RefId(aChange.LocalId), aChange.Kind);
  }
  for (const BRepGraph_Revision::WireRefChange& aChange : myWireRefChanges)
  {
    appendReference(aChange.UID, BRepGraph_RefId(aChange.LocalId), aChange.Kind);
  }

  for (const Standard_GUID& aGUID : myChangedComponentGUIDs)
  {
    const uint32_t* anIndex = myComponentIndices.Seek(aGUID);
    if (anIndex == nullptr)
    {
      continue;
    }
    const occ::handle<BRepGraph_RevisionComponent>& aComponent = myComponents.Value(*anIndex);
    if (aComponent->Descriptor().ComponentDomain
        == BRepGraph_RevisionComponent::Domain::SupplementalStore)
    {
      aDiff.SupplementChangesByStoreGUID.Append(aGUID);
    }
    else
    {
      aDiff.LayerChangesByGUID.Append(aGUID);
    }
  }

  std::sort(aDiff.CreatedUIDs.begin(), aDiff.CreatedUIDs.end());
  std::sort(aDiff.ModifiedUIDs.begin(), aDiff.ModifiedUIDs.end());
  std::sort(aDiff.RemovedUIDs.begin(), aDiff.RemovedUIDs.end());
  std::sort(aDiff.CreatedRefUIDs.begin(), aDiff.CreatedRefUIDs.end());
  std::sort(aDiff.ModifiedRefUIDs.begin(), aDiff.ModifiedRefUIDs.end());
  std::sort(aDiff.RemovedRefUIDs.begin(), aDiff.RemovedRefUIDs.end());
  std::sort(aDiff.UsageLinkChanges.begin(),
            aDiff.UsageLinkChanges.end(),
            [](const BRepGraph_RevisionDiff::UsageLinkChange& theLeft,
               const BRepGraph_RevisionDiff::UsageLinkChange& theRight) {
              return theLeft.LinkUID < theRight.LinkUID;
            });
  return aDiff;
}

//=================================================================================================

bool BRepGraph_Transaction::hasSparseChanges() const noexcept
{
  return !myChanges.IsEmpty() || !myEdgeChanges.IsEmpty() || !myVertexRefChanges.IsEmpty()
         || !myCoEdgeChanges.IsEmpty() || !myWireChanges.IsEmpty() || !myFaceChanges.IsEmpty()
         || !myWireRefChanges.IsEmpty()
         || (!myBaseRevision.IsNull()
             && (myNextVertexCounter != myBaseRevision->NextVertexCounter()
                 || myNextEdgeCounter != myBaseRevision->NextEdgeCounter()
                 || myNextVertexRefCounter != myBaseRevision->NextVertexRefCounter()
                 || myNextCoEdgeCounter != myBaseRevision->NextCoEdgeCounter()
                 || myNextWireCounter != myBaseRevision->NextWireCounter()
                 || myNextFaceCounter != myBaseRevision->NextFaceCounter()
                 || myNextWireRefCounter != myBaseRevision->NextWireRefCounter()));
}

//=================================================================================================

bool BRepGraph_Transaction::resolveVertex(const BRepGraph_UID&     theUID,
                                          BRepGraphInc::VertexDef& theDefinition) const
{
  BRepGraph_Revision::VertexChange aChange;
  if (!resolveVertexChange(theUID, aChange))
  {
    return false;
  }
  if (aChange.Kind == BRepGraph_Revision::VertexChange::Operation::Remove)
  {
    return false;
  }
  theDefinition = aChange.Definition;
  return true;
}

//=================================================================================================

bool BRepGraph_Transaction::resolveVertexChange(const BRepGraph_UID&              theUID,
                                                BRepGraph_Revision::VertexChange& theChange) const
{
  const uint32_t* anIndex = myVertexChangeIndices.Seek(theUID);
  if (anIndex != nullptr)
  {
    theChange = myChanges.Value(*anIndex);
    return true;
  }
  return !myBaseRevision.IsNull() && myBaseRevision->ResolveVertex(theUID, theChange);
}

//=================================================================================================

BRepGraph_Revision::VertexChange* BRepGraph_Transaction::changeOf(const BRepGraph_UID& theUID)
{
  const uint32_t* anIndex = myVertexChangeIndices.Seek(theUID);
  return anIndex == nullptr ? nullptr : &myChanges.ChangeValue(*anIndex);
}

//=================================================================================================

bool BRepGraph_Transaction::resolveEdgeChange(const BRepGraph_UID&            theUID,
                                              BRepGraph_Revision::EdgeChange& theChange) const
{
  const uint32_t* anIndex = myEdgeChangeIndices.Seek(theUID);
  if (anIndex != nullptr)
  {
    theChange = myEdgeChanges.Value(*anIndex);
    return true;
  }
  return !myBaseRevision.IsNull() && myBaseRevision->ResolveEdge(theUID, theChange);
}

//=================================================================================================

bool BRepGraph_Transaction::resolveVertexRefChange(
  const BRepGraph_RefUID&              theUID,
  BRepGraph_Revision::VertexRefChange& theChange) const
{
  const uint32_t* anIndex = myVertexRefChangeIndices.Seek(theUID);
  if (anIndex != nullptr)
  {
    theChange = myVertexRefChanges.Value(*anIndex);
    return true;
  }
  return !myBaseRevision.IsNull() && myBaseRevision->ResolveVertexRef(theUID, theChange);
}

//=================================================================================================

bool BRepGraph_Transaction::resolveCoEdgeChange(const BRepGraph_UID&              theUID,
                                                BRepGraph_Revision::CoEdgeChange& theChange) const
{
  const uint32_t* anIndex = myCoEdgeChangeIndices.Seek(theUID);
  if (anIndex != nullptr)
  {
    theChange = myCoEdgeChanges.Value(*anIndex);
    return true;
  }
  return !myBaseRevision.IsNull() && myBaseRevision->ResolveCoEdge(theUID, theChange);
}

//=================================================================================================

bool BRepGraph_Transaction::resolveWireChange(const BRepGraph_UID&            theUID,
                                              BRepGraph_Revision::WireChange& theChange) const
{
  const uint32_t* anIndex = myWireChangeIndices.Seek(theUID);
  if (anIndex != nullptr)
  {
    theChange = myWireChanges.Value(*anIndex);
    return true;
  }
  return !myBaseRevision.IsNull() && myBaseRevision->ResolveWire(theUID, theChange);
}

//=================================================================================================

bool BRepGraph_Transaction::resolveFaceChange(const BRepGraph_UID&            theUID,
                                              BRepGraph_Revision::FaceChange& theChange) const
{
  const uint32_t* anIndex = myFaceChangeIndices.Seek(theUID);
  if (anIndex != nullptr)
  {
    theChange = myFaceChanges.Value(*anIndex);
    return true;
  }
  return !myBaseRevision.IsNull() && myBaseRevision->ResolveFace(theUID, theChange);
}

//=================================================================================================

bool BRepGraph_Transaction::resolveWireRefChange(const BRepGraph_RefUID&            theUID,
                                                 BRepGraph_Revision::WireRefChange& theChange) const
{
  const uint32_t* anIndex = myWireRefChangeIndices.Seek(theUID);
  if (anIndex != nullptr)
  {
    theChange = myWireRefChanges.Value(*anIndex);
    return true;
  }
  return !myBaseRevision.IsNull() && myBaseRevision->ResolveWireRef(theUID, theChange);
}

//=================================================================================================

BRepGraph_Revision::EdgeChange* BRepGraph_Transaction::edgeChangeOf(const BRepGraph_UID& theUID)
{
  const uint32_t* anIndex = myEdgeChangeIndices.Seek(theUID);
  return anIndex == nullptr ? nullptr : &myEdgeChanges.ChangeValue(*anIndex);
}

//=================================================================================================

BRepGraph_Revision::VertexRefChange* BRepGraph_Transaction::vertexRefChangeOf(
  const BRepGraph_RefUID& theUID)
{
  const uint32_t* anIndex = myVertexRefChangeIndices.Seek(theUID);
  return anIndex == nullptr ? nullptr : &myVertexRefChanges.ChangeValue(*anIndex);
}

//=================================================================================================

BRepGraph_Revision::CoEdgeChange* BRepGraph_Transaction::coEdgeChangeOf(const BRepGraph_UID& theUID)
{
  const uint32_t* anIndex = myCoEdgeChangeIndices.Seek(theUID);
  return anIndex == nullptr ? nullptr : &myCoEdgeChanges.ChangeValue(*anIndex);
}

//=================================================================================================

BRepGraph_Revision::WireChange* BRepGraph_Transaction::wireChangeOf(const BRepGraph_UID& theUID)
{
  const uint32_t* anIndex = myWireChangeIndices.Seek(theUID);
  return anIndex == nullptr ? nullptr : &myWireChanges.ChangeValue(*anIndex);
}

//=================================================================================================

BRepGraph_Revision::FaceChange* BRepGraph_Transaction::faceChangeOf(const BRepGraph_UID& theUID)
{
  const uint32_t* anIndex = myFaceChangeIndices.Seek(theUID);
  return anIndex == nullptr ? nullptr : &myFaceChanges.ChangeValue(*anIndex);
}

//=================================================================================================

BRepGraph_Revision::WireRefChange* BRepGraph_Transaction::wireRefChangeOf(
  const BRepGraph_RefUID& theUID)
{
  const uint32_t* anIndex = myWireRefChangeIndices.Seek(theUID);
  return anIndex == nullptr ? nullptr : &myWireRefChanges.ChangeValue(*anIndex);
}

//=================================================================================================

void BRepGraph_Transaction::recordChange(const BRepGraph_Revision::VertexChange& theChange)
{
  BRepGraph_Revision::VertexChange* anExisting = changeOf(theChange.UID);
  if (anExisting != nullptr)
  {
    const BRepGraph_Revision::VertexChange::Operation anOperation = anExisting->Kind;
    *anExisting                                                   = theChange;
    if (anOperation == BRepGraph_Revision::VertexChange::Operation::Create)
    {
      anExisting->Kind = anOperation;
    }
    return;
  }
  myVertexChangeIndices.Bind(theChange.UID, static_cast<uint32_t>(myChanges.Size()));
  myChanges.Append(theChange);
}

//=================================================================================================

void BRepGraph_Transaction::recordEdgeChange(const BRepGraph_Revision::EdgeChange& theChange)
{
  BRepGraph_Revision::EdgeChange* anExisting = edgeChangeOf(theChange.UID);
  if (anExisting != nullptr)
  {
    const BRepGraph_Revision::VertexChange::Operation anOperation = anExisting->Kind;
    *anExisting                                                   = theChange;
    if (anOperation == BRepGraph_Revision::VertexChange::Operation::Create)
    {
      anExisting->Kind = anOperation;
    }
    return;
  }
  myEdgeChangeIndices.Bind(theChange.UID, static_cast<uint32_t>(myEdgeChanges.Size()));
  myEdgeChanges.Append(theChange);
}

//=================================================================================================

void BRepGraph_Transaction::recordVertexRefChange(
  const BRepGraph_Revision::VertexRefChange& theChange)
{
  BRepGraph_Revision::VertexRefChange* anExisting = vertexRefChangeOf(theChange.UID);
  if (anExisting != nullptr)
  {
    const BRepGraph_Revision::VertexChange::Operation anOperation = anExisting->Kind;
    *anExisting                                                   = theChange;
    if (anOperation == BRepGraph_Revision::VertexChange::Operation::Create)
    {
      anExisting->Kind = anOperation;
    }
    return;
  }
  myVertexRefChangeIndices.Bind(theChange.UID, static_cast<uint32_t>(myVertexRefChanges.Size()));
  myVertexRefChanges.Append(theChange);
}

//=================================================================================================

void BRepGraph_Transaction::recordCoEdgeChange(const BRepGraph_Revision::CoEdgeChange& theChange)
{
  BRepGraph_Revision::CoEdgeChange* anExisting = coEdgeChangeOf(theChange.UID);
  if (anExisting != nullptr)
  {
    const BRepGraph_Revision::VertexChange::Operation anOperation = anExisting->Kind;
    *anExisting                                                   = theChange;
    if (anOperation == BRepGraph_Revision::VertexChange::Operation::Create)
    {
      anExisting->Kind = anOperation;
    }
    return;
  }
  myCoEdgeChangeIndices.Bind(theChange.UID, static_cast<uint32_t>(myCoEdgeChanges.Size()));
  myCoEdgeChanges.Append(theChange);
}

//=================================================================================================

void BRepGraph_Transaction::recordWireChange(const BRepGraph_Revision::WireChange& theChange)
{
  BRepGraph_Revision::WireChange* anExisting = wireChangeOf(theChange.UID);
  if (anExisting != nullptr)
  {
    const BRepGraph_Revision::VertexChange::Operation anOperation = anExisting->Kind;
    *anExisting                                                   = theChange;
    if (anOperation == BRepGraph_Revision::VertexChange::Operation::Create)
    {
      anExisting->Kind = anOperation;
    }
    return;
  }
  myWireChangeIndices.Bind(theChange.UID, static_cast<uint32_t>(myWireChanges.Size()));
  myWireChanges.Append(theChange);
}

//=================================================================================================

void BRepGraph_Transaction::recordFaceChange(const BRepGraph_Revision::FaceChange& theChange)
{
  BRepGraph_Revision::FaceChange* anExisting = faceChangeOf(theChange.UID);
  if (anExisting != nullptr)
  {
    const BRepGraph_Revision::VertexChange::Operation anOperation = anExisting->Kind;
    *anExisting                                                   = theChange;
    if (anOperation == BRepGraph_Revision::VertexChange::Operation::Create)
    {
      anExisting->Kind = anOperation;
    }
    return;
  }
  myFaceChangeIndices.Bind(theChange.UID, static_cast<uint32_t>(myFaceChanges.Size()));
  myFaceChanges.Append(theChange);
}

//=================================================================================================

void BRepGraph_Transaction::recordWireRefChange(const BRepGraph_Revision::WireRefChange& theChange)
{
  BRepGraph_Revision::WireRefChange* anExisting = wireRefChangeOf(theChange.UID);
  if (anExisting != nullptr)
  {
    const BRepGraph_Revision::VertexChange::Operation anOperation = anExisting->Kind;
    *anExisting                                                   = theChange;
    if (anOperation == BRepGraph_Revision::VertexChange::Operation::Create)
    {
      anExisting->Kind = anOperation;
    }
    return;
  }
  myWireRefChangeIndices.Bind(theChange.UID, static_cast<uint32_t>(myWireRefChanges.Size()));
  myWireRefChanges.Append(theChange);
}

//=================================================================================================

void BRepGraph_Transaction::recordAllocation(const UIDRange& theRange)
{
  if (theRange.IsValid())
  {
    myAllocatedRanges.Append(theRange);
  }
}

//=================================================================================================

void BRepGraph_Transaction::clearChangeIndices() noexcept
{
  myVertexChangeIndices.Clear();
  myEdgeChangeIndices.Clear();
  myVertexRefChangeIndices.Clear();
  myCoEdgeChangeIndices.Clear();
  myWireChangeIndices.Clear();
  myFaceChangeIndices.Clear();
  myWireRefChangeIndices.Clear();
}

//=================================================================================================

void BRepGraph_Transaction::clearAllocations() noexcept
{
  myAllocatedRanges.Clear();
}
