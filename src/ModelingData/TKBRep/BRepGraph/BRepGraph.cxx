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

#include <BRepGraph.hxx>

#include <BRepGraph_Builder.hxx>
#include <BRepGraph_Mutator.hxx>
#include <BRepGraph_Reconstruct.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <Standard_ProgramError.hxx>
#include <TopAbs_Orientation.hxx>

// Initial capacities for BRepGraph internal storage vectors and maps.
namespace
{
constexpr int THE_INIT_CAPACITY_SMALL    = 16;  // solids, shells
constexpr int THE_INIT_CAPACITY_MEDIUM   = 128; // faces, wires, pcurves
constexpr int THE_INIT_CAPACITY_LARGE    = 256; // edges, vertices, history
constexpr int THE_INIT_CAPACITY_SURFACES = 64;  // surface and curve storage
constexpr int THE_INIT_CAPACITY_REGISTRY = 100; // registries and hash maps
} // anonymous namespace

//=================================================================================================

BRepGraph::BRepGraph()
    : myAllocator(NCollection_BaseAllocator::CommonBaseAllocator()),
      mySolidDefs(THE_INIT_CAPACITY_SMALL, myAllocator),
      myShellDefs(THE_INIT_CAPACITY_SMALL, myAllocator),
      myFaceDefs(THE_INIT_CAPACITY_MEDIUM, myAllocator),
      myWireDefs(THE_INIT_CAPACITY_MEDIUM, myAllocator),
      myEdgeDefs(THE_INIT_CAPACITY_LARGE, myAllocator),
      myVertexDefs(THE_INIT_CAPACITY_LARGE, myAllocator),
      mySolidUsages(THE_INIT_CAPACITY_SMALL, myAllocator),
      myShellUsages(THE_INIT_CAPACITY_SMALL, myAllocator),
      myFaceUsages(THE_INIT_CAPACITY_MEDIUM, myAllocator),
      myWireUsages(THE_INIT_CAPACITY_MEDIUM, myAllocator),
      myEdgeUsages(THE_INIT_CAPACITY_LARGE, myAllocator),
      myVertexUsages(THE_INIT_CAPACITY_LARGE, myAllocator),
      mySurfaces(THE_INIT_CAPACITY_SURFACES, myAllocator),
      myCurves(THE_INIT_CAPACITY_SURFACES, myAllocator),
      myPCurves(THE_INIT_CAPACITY_MEDIUM, myAllocator),
      mySurfRegistry(THE_INIT_CAPACITY_REGISTRY, myAllocator),
      myCurveRegistry(THE_INIT_CAPACITY_REGISTRY, myAllocator),
      myTShapeToDefId(THE_INIT_CAPACITY_REGISTRY, myAllocator),
      myUIDToNodeId(BRepGraph_UID::Hasher(), THE_INIT_CAPACITY_REGISTRY, myAllocator),
      myIsDone(false)
{
}

//=================================================================================================

BRepGraph::BRepGraph(const Handle(NCollection_BaseAllocator)& theAlloc)
    : myAllocator(!theAlloc.IsNull() ? theAlloc : NCollection_BaseAllocator::CommonBaseAllocator()),
      mySolidDefs(THE_INIT_CAPACITY_SMALL, myAllocator),
      myShellDefs(THE_INIT_CAPACITY_SMALL, myAllocator),
      myFaceDefs(THE_INIT_CAPACITY_MEDIUM, myAllocator),
      myWireDefs(THE_INIT_CAPACITY_MEDIUM, myAllocator),
      myEdgeDefs(THE_INIT_CAPACITY_LARGE, myAllocator),
      myVertexDefs(THE_INIT_CAPACITY_LARGE, myAllocator),
      mySolidUsages(THE_INIT_CAPACITY_SMALL, myAllocator),
      myShellUsages(THE_INIT_CAPACITY_SMALL, myAllocator),
      myFaceUsages(THE_INIT_CAPACITY_MEDIUM, myAllocator),
      myWireUsages(THE_INIT_CAPACITY_MEDIUM, myAllocator),
      myEdgeUsages(THE_INIT_CAPACITY_LARGE, myAllocator),
      myVertexUsages(THE_INIT_CAPACITY_LARGE, myAllocator),
      mySurfaces(THE_INIT_CAPACITY_SURFACES, myAllocator),
      myCurves(THE_INIT_CAPACITY_SURFACES, myAllocator),
      myPCurves(THE_INIT_CAPACITY_MEDIUM, myAllocator),
      mySurfRegistry(THE_INIT_CAPACITY_REGISTRY, myAllocator),
      myCurveRegistry(THE_INIT_CAPACITY_REGISTRY, myAllocator),
      myTShapeToDefId(THE_INIT_CAPACITY_REGISTRY, myAllocator),
      myUIDToNodeId(BRepGraph_UID::Hasher(), THE_INIT_CAPACITY_REGISTRY, myAllocator),
      myIsDone(false)
{
}

//=================================================================================================

void BRepGraph::Build(const TopoDS_Shape& theShape, bool theParallel)
{
  BRepGraph_Builder::Perform(*this, theShape, theParallel);
}

//=================================================================================================

void BRepGraph::AppendShape(const TopoDS_Shape& theShape, bool theParallel)
{
  BRepGraph_Builder::Append(*this, theShape, theParallel);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::registerSurface(const Handle(Geom_Surface)&       theSurf,
                                            const Handle(Poly_Triangulation)& theTri)
{
  if (theSurf.IsNull())
    return BRepGraph_NodeId();

  const Geom_Surface* aKey       = theSurf.get();
  const int*          anExisting = mySurfRegistry.Seek(aKey);
  if (anExisting != nullptr)
    return BRepGraph_NodeId(BRepGraph_NodeKind::Surface, *anExisting);

  BRepGraph_GeomNode::Surf& aSurfNode = mySurfaces.Appended();
  const int                 aSurfIdx  = mySurfaces.Length() - 1;
  aSurfNode.Id                        = BRepGraph_NodeId(BRepGraph_NodeKind::Surface, aSurfIdx);
  aSurfNode.Surface                   = theSurf;
  aSurfNode.Triangulation             = theTri;
  aSurfNode.IsMultiLocated            = false;
  allocateUID(aSurfNode.Id);

  mySurfRegistry.Add(aKey, aSurfIdx);
  return aSurfNode.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::registerCurve(const Handle(Geom_Curve)& theCrv)
{
  if (theCrv.IsNull())
    return BRepGraph_NodeId();

  const Geom_Curve* aKey       = theCrv.get();
  const int*        anExisting = myCurveRegistry.Seek(aKey);
  if (anExisting != nullptr)
    return BRepGraph_NodeId(BRepGraph_NodeKind::Curve, *anExisting);

  BRepGraph_GeomNode::Curve& aCurveNode = myCurves.Appended();
  const int                  aCurveIdx  = myCurves.Length() - 1;
  aCurveNode.Id                         = BRepGraph_NodeId(BRepGraph_NodeKind::Curve, aCurveIdx);
  aCurveNode.CurveGeom                  = theCrv;
  aCurveNode.IsMultiLocated             = false;
  allocateUID(aCurveNode.Id);

  myCurveRegistry.Add(aKey, aCurveIdx);
  return aCurveNode.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::createPCurveNode(const Handle(Geom2d_Curve)& theCrv2d,
                                             BRepGraph_NodeId            theEdgeDef,
                                             BRepGraph_NodeId            theFaceDef,
                                             double                      theFirst,
                                             double                      theLast)
{
  BRepGraph_GeomNode::PCurve& aNode = myPCurves.Appended();
  const int                   aIdx  = myPCurves.Length() - 1;
  aNode.Id                          = BRepGraph_NodeId(BRepGraph_NodeKind::PCurve, aIdx);
  aNode.Curve2d                     = theCrv2d;
  aNode.EdgeContext                 = theEdgeDef;
  aNode.FaceContext                 = theFaceDef;
  aNode.ParamFirst                  = theFirst;
  aNode.ParamLast                   = theLast;
  allocateUID(aNode.Id);

  return aNode.Id;
}

//=================================================================================================

BRepGraph_UID BRepGraph::allocateUID(BRepGraph_NodeId theNodeId)
{
  if (!myUIDEnabled)
    return BRepGraph_UID();

  const size_t  aCounter = myNextUIDCounter.fetch_add(1, std::memory_order_relaxed);
  BRepGraph_UID aUID(theNodeId.Kind, aCounter, myGeneration);
  myNodeToUID.Bind(theNodeId, aUID);
  myUIDToNodeId.Bind(aUID, theNodeId);
  return aUID;
}

//=================================================================================================

BRepGraph_NodeCache* BRepGraph::mutableCache(BRepGraph_NodeId theNode)
{
  return dispatchDef(theNode, [](auto& theVec, int theIdx) -> BRepGraph_NodeCache* {
    return &theVec.ChangeValue(theIdx).Cache;
  });
}

//=================================================================================================

bool BRepGraph::IsDone() const
{
  return myIsDone;
}

//=================================================================================================

int BRepGraph::NbSolidDefs() const { return mySolidDefs.Length(); }
int BRepGraph::NbShellDefs() const { return myShellDefs.Length(); }
int BRepGraph::NbFaceDefs() const { return myFaceDefs.Length(); }
int BRepGraph::NbWireDefs() const { return myWireDefs.Length(); }
int BRepGraph::NbEdgeDefs() const { return myEdgeDefs.Length(); }
int BRepGraph::NbVertexDefs() const { return myVertexDefs.Length(); }
int BRepGraph::NbCompoundDefs() const { return myCompoundDefs.Length(); }
int BRepGraph::NbCompSolidDefs() const { return myCompSolidDefs.Length(); }
int BRepGraph::NbSurfaces() const { return mySurfaces.Length(); }
int BRepGraph::NbCurves() const { return myCurves.Length(); }
int BRepGraph::NbPCurves() const { return myPCurves.Length(); }

int BRepGraph::NbSolidUsages() const { return mySolidUsages.Length(); }
int BRepGraph::NbShellUsages() const { return myShellUsages.Length(); }
int BRepGraph::NbFaceUsages() const { return myFaceUsages.Length(); }
int BRepGraph::NbWireUsages() const { return myWireUsages.Length(); }
int BRepGraph::NbEdgeUsages() const { return myEdgeUsages.Length(); }
int BRepGraph::NbVertexUsages() const { return myVertexUsages.Length(); }
int BRepGraph::NbCompoundUsages() const { return myCompoundUsages.Length(); }
int BRepGraph::NbCompSolidUsages() const { return myCompSolidUsages.Length(); }

//=================================================================================================

const BRepGraph_TopoNode::SolidDef& BRepGraph::SolidDefinition(int theIdx) const
{ return mySolidDefs.Value(theIdx); }

const BRepGraph_TopoNode::ShellDef& BRepGraph::ShellDefinition(int theIdx) const
{ return myShellDefs.Value(theIdx); }

const BRepGraph_TopoNode::FaceDef& BRepGraph::FaceDefinition(int theIdx) const
{ return myFaceDefs.Value(theIdx); }

const BRepGraph_TopoNode::WireDef& BRepGraph::WireDefinition(int theIdx) const
{ return myWireDefs.Value(theIdx); }

const BRepGraph_TopoNode::EdgeDef& BRepGraph::EdgeDefinition(int theIdx) const
{ return myEdgeDefs.Value(theIdx); }

const BRepGraph_TopoNode::VertexDef& BRepGraph::VertexDefinition(int theIdx) const
{ return myVertexDefs.Value(theIdx); }

const BRepGraph_TopoNode::CompoundDef& BRepGraph::CompoundDefinition(int theIdx) const
{ return myCompoundDefs.Value(theIdx); }

const BRepGraph_TopoNode::CompSolidDef& BRepGraph::CompSolidDefinition(int theIdx) const
{ return myCompSolidDefs.Value(theIdx); }

const BRepGraph_GeomNode::Surf& BRepGraph::SurfNode(int theIdx) const
{ return mySurfaces.Value(theIdx); }

const BRepGraph_GeomNode::Curve& BRepGraph::CurveNode(int theIdx) const
{ return myCurves.Value(theIdx); }

const BRepGraph_GeomNode::PCurve& BRepGraph::PCurveNode(int theIdx) const
{ return myPCurves.Value(theIdx); }

//=================================================================================================

const BRepGraph_TopoNode::SolidUsage& BRepGraph::SolidUsageNode(int theIdx) const
{ return mySolidUsages.Value(theIdx); }

const BRepGraph_TopoNode::ShellUsage& BRepGraph::ShellUsageNode(int theIdx) const
{ return myShellUsages.Value(theIdx); }

const BRepGraph_TopoNode::FaceUsage& BRepGraph::FaceUsageNode(int theIdx) const
{ return myFaceUsages.Value(theIdx); }

const BRepGraph_TopoNode::WireUsage& BRepGraph::WireUsageNode(int theIdx) const
{ return myWireUsages.Value(theIdx); }

const BRepGraph_TopoNode::EdgeUsage& BRepGraph::EdgeUsageNode(int theIdx) const
{ return myEdgeUsages.Value(theIdx); }

const BRepGraph_TopoNode::VertexUsage& BRepGraph::VertexUsageNode(int theIdx) const
{ return myVertexUsages.Value(theIdx); }

const BRepGraph_TopoNode::CompoundUsage& BRepGraph::CompoundUsageNode(int theIdx) const
{ return myCompoundUsages.Value(theIdx); }

const BRepGraph_TopoNode::CompSolidUsage& BRepGraph::CompSolidUsageNode(int theIdx) const
{ return myCompSolidUsages.Value(theIdx); }

//=================================================================================================

const NCollection_Vector<BRepGraph_UsageId>& BRepGraph::UsagesOf(BRepGraph_NodeId theDefId) const
{
  static const NCollection_Vector<BRepGraph_UsageId> THE_EMPTY;
  const BRepGraph_TopoNode::BaseDef* aDef = TopoDef(theDefId);
  if (aDef == nullptr)
    return THE_EMPTY;
  return aDef->Usages;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::DefOf(BRepGraph_UsageId theUsageId) const
{
  if (!theUsageId.IsValid())
    return BRepGraph_NodeId();

  switch (theUsageId.Kind)
  {
    case BRepGraph_NodeKind::Solid:     return mySolidUsages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeKind::Shell:     return myShellUsages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeKind::Face:      return myFaceUsages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeKind::Wire:      return myWireUsages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeKind::Edge:      return myEdgeUsages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeKind::Vertex:    return myVertexUsages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeKind::Compound:  return myCompoundUsages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeKind::CompSolid: return myCompSolidUsages.Value(theUsageId.Index).DefId;
    default: return BRepGraph_NodeId();
  }
}

//=================================================================================================

const BRepGraph_TopoNode::BaseDef* BRepGraph::TopoDef(BRepGraph_NodeId theId) const
{
  if (!theId.IsValid())
    return nullptr;

  return dispatchDef(theId,
    [](const auto& theVec, int theIdx) -> const BRepGraph_TopoNode::BaseDef* {
      return theIdx < theVec.Length()
               ? static_cast<const BRepGraph_TopoNode::BaseDef*>(&theVec.Value(theIdx))
               : nullptr;
    });
}

//=================================================================================================

size_t BRepGraph::NbNodes() const
{
  return static_cast<size_t>(mySolidDefs.Length()) + static_cast<size_t>(myShellDefs.Length())
         + static_cast<size_t>(myFaceDefs.Length()) + static_cast<size_t>(myWireDefs.Length())
         + static_cast<size_t>(myEdgeDefs.Length()) + static_cast<size_t>(myVertexDefs.Length())
         + static_cast<size_t>(myCompoundDefs.Length()) + static_cast<size_t>(myCompSolidDefs.Length())
         + static_cast<size_t>(mySurfaces.Length()) + static_cast<size_t>(myCurves.Length())
         + static_cast<size_t>(myPCurves.Length());
}

//=================================================================================================

void BRepGraph::SetUIDEnabled(bool theVal) { myUIDEnabled = theVal; }
bool BRepGraph::IsUIDEnabled() const { return myUIDEnabled; }

BRepGraph_UID BRepGraph::UIDOf(BRepGraph_NodeId theNode) const
{
  if (!myUIDEnabled)
    return BRepGraph_UID();
  const BRepGraph_UID* aPtr = myNodeToUID.Seek(theNode);
  return aPtr != nullptr ? *aPtr : BRepGraph_UID();
}

BRepGraph_NodeId BRepGraph::NodeIdFromUID(const BRepGraph_UID& theUID) const
{
  const BRepGraph_NodeId* aPtr = myUIDToNodeId.Seek(theUID);
  return aPtr != nullptr ? *aPtr : BRepGraph_NodeId();
}

bool BRepGraph::HasUID(const BRepGraph_UID& theUID) const
{
  if (!myUIDEnabled || !theUID.IsValid())
    return false;
  if (theUID.Generation() != myGeneration)
    return false;
  return myUIDToNodeId.IsBound(theUID);
}

uint32_t BRepGraph::Generation() const { return myGeneration; }

//=================================================================================================

int BRepGraph::AddRelEdge(BRepGraph_NodeId theFrom, BRepGraph_NodeId theTo, BRepGraph_RelKind theKind)
{
  BRepGraph_RelEdge anEdge;
  anEdge.Kind   = theKind;
  anEdge.Source = theFrom;
  anEdge.Target = theTo;

  if (!myOutRelEdges.IsBound(theFrom))
    myOutRelEdges.Bind(theFrom, NCollection_Vector<BRepGraph_RelEdge>());
  NCollection_Vector<BRepGraph_RelEdge>& anOutVec = myOutRelEdges.ChangeFind(theFrom);
  anOutVec.Append(anEdge);
  const int anIdx = anOutVec.Length() - 1;

  if (!myInRelEdges.IsBound(theTo))
    myInRelEdges.Bind(theTo, NCollection_Vector<BRepGraph_RelEdge>());
  myInRelEdges.ChangeFind(theTo).Append(anEdge);

  return anIdx;
}

//=================================================================================================

void BRepGraph::RemoveRelEdges(BRepGraph_NodeId theFrom, BRepGraph_NodeId theTo, BRepGraph_RelKind theKind)
{
  NCollection_Vector<BRepGraph_RelEdge>* anOutVec = myOutRelEdges.ChangeSeek(theFrom);
  if (anOutVec != nullptr)
  {
    for (int anIdx = anOutVec->Length() - 1; anIdx >= 0; --anIdx)
    {
      const BRepGraph_RelEdge& anEdge = anOutVec->Value(anIdx);
      if (anEdge.Kind == theKind && anEdge.Target == theTo)
      {
        if (anIdx < anOutVec->Length() - 1)
          anOutVec->ChangeValue(anIdx) = anOutVec->Value(anOutVec->Length() - 1);
        anOutVec->EraseLast();
      }
    }
  }

  NCollection_Vector<BRepGraph_RelEdge>* anInVec = myInRelEdges.ChangeSeek(theTo);
  if (anInVec != nullptr)
  {
    for (int anIdx = anInVec->Length() - 1; anIdx >= 0; --anIdx)
    {
      const BRepGraph_RelEdge& anEdge = anInVec->Value(anIdx);
      if (anEdge.Kind == theKind && anEdge.Source == theFrom)
      {
        if (anIdx < anInVec->Length() - 1)
          anInVec->ChangeValue(anIdx) = anInVec->Value(anInVec->Length() - 1);
        anInVec->EraseLast();
      }
    }
  }
}

//=================================================================================================

int BRepGraph::NbRelEdgesFrom(BRepGraph_NodeId theNode) const
{
  const NCollection_Vector<BRepGraph_RelEdge>* aEdges = myOutRelEdges.Seek(theNode);
  return aEdges != nullptr ? aEdges->Length() : 0;
}

int BRepGraph::NbRelEdgesTo(BRepGraph_NodeId theNode) const
{
  const NCollection_Vector<BRepGraph_RelEdge>* aEdges = myInRelEdges.Seek(theNode);
  return aEdges != nullptr ? aEdges->Length() : 0;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::SurfaceOf(BRepGraph_NodeId theFaceDef) const
{
  if (theFaceDef.Kind != BRepGraph_NodeKind::Face || !theFaceDef.IsValid())
    return BRepGraph_NodeId();
  return myFaceDefs.Value(theFaceDef.Index).SurfNodeId;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_NodeId>& BRepGraph::FacesOnSurface(BRepGraph_NodeId theSurf) const
{
  static const NCollection_Vector<BRepGraph_NodeId> THE_EMPTY_VEC;
  if (theSurf.Kind != BRepGraph_NodeKind::Surface || !theSurf.IsValid()
      || theSurf.Index >= mySurfaces.Length())
    return THE_EMPTY_VEC;
  return mySurfaces.Value(theSurf.Index).FaceDefUsers;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::CurveOf(BRepGraph_NodeId theEdgeDef) const
{
  if (theEdgeDef.Kind != BRepGraph_NodeKind::Edge || !theEdgeDef.IsValid())
    return BRepGraph_NodeId();
  return myEdgeDefs.Value(theEdgeDef.Index).CurveNodeId;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_NodeId>& BRepGraph::EdgesOnCurve(BRepGraph_NodeId theCurve) const
{
  static const NCollection_Vector<BRepGraph_NodeId> THE_EMPTY_VEC;
  if (theCurve.Kind != BRepGraph_NodeKind::Curve || !theCurve.IsValid()
      || theCurve.Index >= myCurves.Length())
    return THE_EMPTY_VEC;
  return myCurves.Value(theCurve.Index).EdgeDefUsers;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::PCurveOf(BRepGraph_NodeId theEdgeDef, BRepGraph_NodeId theFaceDef) const
{
  if (theEdgeDef.Kind != BRepGraph_NodeKind::Edge || !theEdgeDef.IsValid())
    return BRepGraph_NodeId();

  const BRepGraph_TopoNode::EdgeDef& anEdgeDef = myEdgeDefs.Value(theEdgeDef.Index);
  for (int aPCurveIter = 0; aPCurveIter < anEdgeDef.PCurves.Length(); ++aPCurveIter)
  {
    if (anEdgeDef.PCurves.Value(aPCurveIter).FaceDefId == theFaceDef)
      return anEdgeDef.PCurves.Value(aPCurveIter).PCurveNodeId;
  }
  return BRepGraph_NodeId();
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::PCurveOf(BRepGraph_NodeId   theEdgeDef,
                                     BRepGraph_NodeId   theFaceDef,
                                     TopAbs_Orientation theEdgeOrientation) const
{
  if (theEdgeDef.Kind != BRepGraph_NodeKind::Edge || !theEdgeDef.IsValid())
    return BRepGraph_NodeId();

  const BRepGraph_TopoNode::EdgeDef& anEdgeDef = myEdgeDefs.Value(theEdgeDef.Index);
  for (int aPCurveIter = 0; aPCurveIter < anEdgeDef.PCurves.Length(); ++aPCurveIter)
  {
    const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry =
      anEdgeDef.PCurves.Value(aPCurveIter);
    if (aPCEntry.FaceDefId == theFaceDef && aPCEntry.EdgeOrientation == theEdgeOrientation)
      return aPCEntry.PCurveNodeId;
  }
  return BRepGraph_NodeId();
}

//=================================================================================================

gp_Trsf BRepGraph::GlobalTransform(BRepGraph_UsageId theUsage) const
{
  if (!theUsage.IsValid())
    return gp_Trsf();

  switch (theUsage.Kind)
  {
    case BRepGraph_NodeKind::Solid:     return mySolidUsages.Value(theUsage.Index).GlobalLocation.Transformation();
    case BRepGraph_NodeKind::Shell:     return myShellUsages.Value(theUsage.Index).GlobalLocation.Transformation();
    case BRepGraph_NodeKind::Face:      return myFaceUsages.Value(theUsage.Index).GlobalLocation.Transformation();
    case BRepGraph_NodeKind::Wire:      return myWireUsages.Value(theUsage.Index).GlobalLocation.Transformation();
    case BRepGraph_NodeKind::Edge:      return myEdgeUsages.Value(theUsage.Index).GlobalLocation.Transformation();
    case BRepGraph_NodeKind::Vertex:    return myVertexUsages.Value(theUsage.Index).GlobalLocation.Transformation();
    case BRepGraph_NodeKind::Compound:  return myCompoundUsages.Value(theUsage.Index).GlobalLocation.Transformation();
    case BRepGraph_NodeKind::CompSolid: return myCompSolidUsages.Value(theUsage.Index).GlobalLocation.Transformation();
    default: return gp_Trsf();
  }
}

//=================================================================================================

gp_Trsf BRepGraph::GlobalTransform(BRepGraph_NodeId theDefId) const
{
  const BRepGraph_TopoNode::BaseDef* aDef = TopoDef(theDefId);
  if (aDef == nullptr || aDef->Usages.IsEmpty())
    return gp_Trsf();
  return GlobalTransform(aDef->Usages.Value(0));
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SameDomainFaces(BRepGraph_NodeId theFaceDef) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  if (theFaceDef.Kind != BRepGraph_NodeKind::Face || !theFaceDef.IsValid())
    return aResult;

  const BRepGraph_TopoNode::FaceDef& aFaceDef = myFaceDefs.Value(theFaceDef.Index);
  if (!aFaceDef.SurfNodeId.IsValid())
    return aResult;

  const NCollection_Vector<BRepGraph_NodeId>& aFaceDefUsers =
    mySurfaces.Value(aFaceDef.SurfNodeId.Index).FaceDefUsers;
  for (int anIdx = 0; anIdx < aFaceDefUsers.Length(); ++anIdx)
  {
    if (aFaceDefUsers.Value(anIdx) != theFaceDef)
      aResult.Append(aFaceDefUsers.Value(anIdx));
  }
  return aResult;
}

//=================================================================================================

Bnd_Box BRepGraph::BoundingBox(BRepGraph_NodeId theNode) const
{
  const BRepGraph_TopoNode::BaseDef* aDef = TopoDef(theNode);
  if (aDef == nullptr)
    return Bnd_Box();

  return aDef->Cache.BoundingBox.Get([&]() -> Bnd_Box {
    Bnd_Box aBox;
    collectVertexPoints(theNode, aBox);
    return aBox;
  });
}

//=================================================================================================

void BRepGraph::collectVertexPoints(BRepGraph_NodeId theNode, Bnd_Box& theBox) const
{
  if (!theNode.IsValid())
    return;

  switch (theNode.Kind)
  {
    case BRepGraph_NodeKind::Vertex:
    {
      const BRepGraph_TopoNode::VertexDef& aVtxDef = myVertexDefs.Value(theNode.Index);
      theBox.Add(aVtxDef.Point);
      theBox.Enlarge(aVtxDef.Tolerance);
      break;
    }
    case BRepGraph_NodeKind::Edge:
    {
      const BRepGraph_TopoNode::EdgeDef& anEdgeDef = myEdgeDefs.Value(theNode.Index);
      collectVertexPoints(anEdgeDef.StartVertexDefId, theBox);
      if (anEdgeDef.EndVertexDefId != anEdgeDef.StartVertexDefId)
        collectVertexPoints(anEdgeDef.EndVertexDefId, theBox);
      break;
    }
    case BRepGraph_NodeKind::Wire:
    {
      const BRepGraph_TopoNode::WireDef& aWireDef = myWireDefs.Value(theNode.Index);
      for (int anIdx = 0; anIdx < aWireDef.OrderedEdges.Length(); ++anIdx)
      {
        collectVertexPoints(aWireDef.OrderedEdges.Value(anIdx).EdgeDefId, theBox);
      }
      break;
    }
    case BRepGraph_NodeKind::Face:
    {
      const BRepGraph_TopoNode::FaceDef& aFaceDef = myFaceDefs.Value(theNode.Index);
      if (aFaceDef.Usages.IsEmpty())
        break;
      const BRepGraph_TopoNode::FaceUsage& aFaceUsage =
        myFaceUsages.Value(aFaceDef.Usages.First().Index);
      if (aFaceUsage.OuterWireUsage.IsValid())
        collectVertexPoints(DefOf(aFaceUsage.OuterWireUsage), theBox);
      for (int anIdx = 0; anIdx < aFaceUsage.InnerWireUsages.Length(); ++anIdx)
      {
        collectVertexPoints(DefOf(aFaceUsage.InnerWireUsages.Value(anIdx)), theBox);
      }
      break;
    }
    case BRepGraph_NodeKind::Shell:
    {
      const BRepGraph_TopoNode::ShellDef& aShellDef = myShellDefs.Value(theNode.Index);
      if (aShellDef.Usages.IsEmpty())
        break;
      const BRepGraph_TopoNode::ShellUsage& aShellUsage =
        myShellUsages.Value(aShellDef.Usages.First().Index);
      for (int anIdx = 0; anIdx < aShellUsage.FaceUsages.Length(); ++anIdx)
      {
        collectVertexPoints(DefOf(aShellUsage.FaceUsages.Value(anIdx)), theBox);
      }
      break;
    }
    case BRepGraph_NodeKind::Solid:
    {
      const BRepGraph_TopoNode::SolidDef& aSolidDef = mySolidDefs.Value(theNode.Index);
      if (aSolidDef.Usages.IsEmpty())
        break;
      const BRepGraph_TopoNode::SolidUsage& aSolidUsage =
        mySolidUsages.Value(aSolidDef.Usages.First().Index);
      for (int anIdx = 0; anIdx < aSolidUsage.ShellUsages.Length(); ++anIdx)
      {
        collectVertexPoints(DefOf(aSolidUsage.ShellUsages.Value(anIdx)), theBox);
      }
      break;
    }
    case BRepGraph_NodeKind::Compound:
    {
      const BRepGraph_TopoNode::CompoundDef& aCompDef = myCompoundDefs.Value(theNode.Index);
      for (int anIdx = 0; anIdx < aCompDef.ChildDefIds.Length(); ++anIdx)
      {
        collectVertexPoints(aCompDef.ChildDefIds.Value(anIdx), theBox);
      }
      break;
    }
    case BRepGraph_NodeKind::CompSolid:
    {
      const BRepGraph_TopoNode::CompSolidDef& aCSolDef = myCompSolidDefs.Value(theNode.Index);
      for (int anIdx = 0; anIdx < aCSolDef.SolidDefIds.Length(); ++anIdx)
      {
        collectVertexPoints(aCSolDef.SolidDefIds.Value(anIdx), theBox);
      }
      break;
    }
    default:
      break;
  }
}

//=================================================================================================

gp_Pnt BRepGraph::Centroid(BRepGraph_NodeId theNode) const
{
  const BRepGraph_TopoNode::BaseDef* aDef = TopoDef(theNode);
  if (aDef == nullptr)
    return gp_Pnt();

  return aDef->Cache.Centroid.Get([&]() -> gp_Pnt {
    Bnd_Box aBox = BoundingBox(theNode);
    if (aBox.IsVoid())
      return gp_Pnt();
    double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
    return gp_Pnt((aXmin + aXmax) * 0.5, (aYmin + aYmax) * 0.5, (aZmin + aZmax) * 0.5);
  });
}

//=================================================================================================

void BRepGraph::Invalidate(BRepGraph_NodeId theNode)
{
  BRepGraph_NodeCache* aCache = mutableCache(theNode);
  if (aCache != nullptr)
    aCache->InvalidateAll();
}

//=================================================================================================

void BRepGraph::InvalidateSubgraph(BRepGraph_NodeId theNode)
{
  invalidateSubgraphImpl(theNode);
}

//=================================================================================================

void BRepGraph::invalidateSubgraphImpl(BRepGraph_NodeId theNode)
{
  Invalidate(theNode);

  switch (theNode.Kind)
  {
    case BRepGraph_NodeKind::Solid: {
      const BRepGraph_TopoNode::SolidDef& aSolidDef = mySolidDefs.Value(theNode.Index);
      for (int aUsIdx = 0; aUsIdx < aSolidDef.Usages.Length(); ++aUsIdx)
      {
        const BRepGraph_TopoNode::SolidUsage& aUsage =
          mySolidUsages.Value(aSolidDef.Usages.Value(aUsIdx).Index);
        for (int aShellIter = 0; aShellIter < aUsage.ShellUsages.Length(); ++aShellIter)
        {
          BRepGraph_NodeId aShellDefId = DefOf(aUsage.ShellUsages.Value(aShellIter));
          invalidateSubgraphImpl(aShellDefId);
        }
      }
      break;
    }
    case BRepGraph_NodeKind::Shell: {
      const BRepGraph_TopoNode::ShellDef& aShellDef = myShellDefs.Value(theNode.Index);
      for (int aUsIdx = 0; aUsIdx < aShellDef.Usages.Length(); ++aUsIdx)
      {
        const BRepGraph_TopoNode::ShellUsage& aUsage =
          myShellUsages.Value(aShellDef.Usages.Value(aUsIdx).Index);
        for (int aFaceIter = 0; aFaceIter < aUsage.FaceUsages.Length(); ++aFaceIter)
        {
          BRepGraph_NodeId aFaceDefId = DefOf(aUsage.FaceUsages.Value(aFaceIter));
          invalidateSubgraphImpl(aFaceDefId);
        }
      }
      break;
    }
    case BRepGraph_NodeKind::Face: {
      const BRepGraph_TopoNode::FaceDef& aFaceDef = myFaceDefs.Value(theNode.Index);
      for (int aUsIdx = 0; aUsIdx < aFaceDef.Usages.Length(); ++aUsIdx)
      {
        const BRepGraph_TopoNode::FaceUsage& aUsage =
          myFaceUsages.Value(aFaceDef.Usages.Value(aUsIdx).Index);
        if (aUsage.OuterWireUsage.IsValid())
          invalidateSubgraphImpl(DefOf(aUsage.OuterWireUsage));
        for (int aWireIter = 0; aWireIter < aUsage.InnerWireUsages.Length(); ++aWireIter)
          invalidateSubgraphImpl(DefOf(aUsage.InnerWireUsages.Value(aWireIter)));
      }
      break;
    }
    case BRepGraph_NodeKind::Wire: {
      const BRepGraph_TopoNode::WireDef& aWireDef = myWireDefs.Value(theNode.Index);
      for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.OrderedEdges.Length(); ++anEdgeIdx)
        invalidateSubgraphImpl(aWireDef.OrderedEdges.Value(anEdgeIdx).EdgeDefId);
      break;
    }
    case BRepGraph_NodeKind::Edge: {
      const BRepGraph_TopoNode::EdgeDef& anEdgeDef = myEdgeDefs.Value(theNode.Index);
      if (anEdgeDef.StartVertexDefId.IsValid())
        Invalidate(anEdgeDef.StartVertexDefId);
      if (anEdgeDef.EndVertexDefId.IsValid())
        Invalidate(anEdgeDef.EndVertexDefId);
      break;
    }
    default:
      break;
  }
}

//=================================================================================================

void BRepGraph::SetUserAttribute(BRepGraph_NodeId theNode, int theKey,
                                 const BRepGraph_UserAttrPtr& theAttr)
{
  BRepGraph_NodeCache* aCache = mutableCache(theNode);
  if (aCache != nullptr)
    aCache->SetUserAttribute(theKey, theAttr);
}

BRepGraph_UserAttrPtr BRepGraph::GetUserAttribute(BRepGraph_NodeId theNode, int theKey) const
{
  const BRepGraph_TopoNode::BaseDef* aDef = TopoDef(theNode);
  if (aDef == nullptr)
    return nullptr;
  return aDef->Cache.GetUserAttribute(theKey);
}

bool BRepGraph::RemoveUserAttribute(BRepGraph_NodeId theNode, int theKey)
{
  BRepGraph_NodeCache* aCache = mutableCache(theNode);
  if (aCache == nullptr)
    return false;
  return aCache->RemoveUserAttribute(theKey);
}

void BRepGraph::InvalidateUserAttribute(BRepGraph_NodeId theNode, int theKey)
{
  BRepGraph_NodeCache* aCache = mutableCache(theNode);
  if (aCache != nullptr)
    aCache->InvalidateUserAttribute(theKey);
}

//=================================================================================================

int BRepGraph::NbHistoryRecords() const { return myHistoryLog.NbRecords(); }

const BRepGraph_HistoryRecord& BRepGraph::HistoryRecord(int theIdx) const
{ return myHistoryLog.Record(theIdx); }

BRepGraph_NodeId BRepGraph::FindOriginal(BRepGraph_NodeId theModified) const
{ return myHistoryLog.FindOriginal(theModified); }

NCollection_Vector<BRepGraph_NodeId> BRepGraph::FindDerived(BRepGraph_NodeId theOriginal) const
{
  // Returns ALL transitively derived nodes (intermediates + leaves).
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const NCollection_Vector<BRepGraph_NodeId>* aDerived =
    myHistoryLog.myOriginalToDerived.Seek(theOriginal);
  if (aDerived == nullptr)
    return aResult;

  for (int anIdx = 0; anIdx < aDerived->Length(); ++anIdx)
  {
    const BRepGraph_NodeId& aDerivedId = aDerived->Value(anIdx);
    aResult.Append(aDerivedId);
    NCollection_Vector<BRepGraph_NodeId> aFurther = FindDerived(aDerivedId);
    for (int aFurtherIter = 0; aFurtherIter < aFurther.Length(); ++aFurtherIter)
      aResult.Append(aFurther.Value(aFurtherIter));
  }
  return aResult;
}

//=================================================================================================

void BRepGraph::ApplyModification(
  BRepGraph_NodeId                                                                  theTarget,
  std::function<NCollection_Vector<BRepGraph_NodeId>(BRepGraph&, BRepGraph_NodeId)> theModifier,
  const TCollection_AsciiString&                                                    theOpLabel)
{
  NCollection_Vector<BRepGraph_NodeId> aReplacements = theModifier(*this, theTarget);

  myHistoryLog.Record(theOpLabel, theTarget, aReplacements);

  InvalidateSubgraph(theTarget);
}

//=================================================================================================

TopoDS_Shape BRepGraph::ReconstructShape(BRepGraph_NodeId theRoot) const
{
  return BRepGraph_Reconstruct::Node(*this, theRoot);
}

TopoDS_Shape BRepGraph::ReconstructFace(int theFaceDefIdx) const
{
  return BRepGraph_Reconstruct::Node(*this, BRepGraph_NodeId(BRepGraph_NodeKind::Face, theFaceDefIdx));
}

TopoDS_Shape BRepGraph::ReconstructFromUsage(BRepGraph_UsageId theRoot) const
{
  return BRepGraph_Reconstruct::Usage(*this, theRoot);
}

//=================================================================================================

BRepGraph_TopoNode::EdgeDef& BRepGraph::MutableEdgeDefinition(int theIdx)
{
  markModified(BRepGraph_NodeId(BRepGraph_NodeKind::Edge, theIdx));
  return myEdgeDefs.ChangeValue(theIdx);
}

BRepGraph_TopoNode::WireDef& BRepGraph::MutableWireDefinition(int theIdx)
{
  markModified(BRepGraph_NodeId(BRepGraph_NodeKind::Wire, theIdx));
  return myWireDefs.ChangeValue(theIdx);
}

BRepGraph_TopoNode::VertexDef& BRepGraph::MutableVertexDefinition(int theIdx)
{
  markModified(BRepGraph_NodeId(BRepGraph_NodeKind::Vertex, theIdx));
  return myVertexDefs.ChangeValue(theIdx);
}

BRepGraph_TopoNode::FaceDef& BRepGraph::MutableFaceDefinition(int theIdx)
{
  markModified(BRepGraph_NodeId(BRepGraph_NodeKind::Face, theIdx));
  return myFaceDefs.ChangeValue(theIdx);
}

BRepGraph_TopoNode::ShellDef& BRepGraph::MutableShellDefinition(int theIdx)
{
  markModified(BRepGraph_NodeId(BRepGraph_NodeKind::Shell, theIdx));
  return myShellDefs.ChangeValue(theIdx);
}

BRepGraph_TopoNode::SolidDef& BRepGraph::MutableSolidDefinition(int theIdx)
{
  markModified(BRepGraph_NodeId(BRepGraph_NodeKind::Solid, theIdx));
  return mySolidDefs.ChangeValue(theIdx);
}

BRepGraph_TopoNode::CompoundDef& BRepGraph::MutableCompoundDefinition(int theIdx)
{
  markModified(BRepGraph_NodeId(BRepGraph_NodeKind::Compound, theIdx));
  return myCompoundDefs.ChangeValue(theIdx);
}

BRepGraph_TopoNode::CompSolidDef& BRepGraph::MutableCompSolidDefinition(int theIdx)
{
  markModified(BRepGraph_NodeId(BRepGraph_NodeKind::CompSolid, theIdx));
  return myCompSolidDefs.ChangeValue(theIdx);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::AddPCurveToEdge(BRepGraph_NodeId            theEdgeDef,
                                            BRepGraph_NodeId            theFaceDef,
                                            const Handle(Geom2d_Curve)& theCurve2d,
                                            double                      theFirst,
                                            double                      theLast,
                                            TopAbs_Orientation          theEdgeOrientation)
{
  BRepGraph_NodeId aPCId = createPCurveNode(theCurve2d, theEdgeDef, theFaceDef, theFirst, theLast);

  BRepGraph_TopoNode::EdgeDef&             anEdgeDef = myEdgeDefs.ChangeValue(theEdgeDef.Index);
  BRepGraph_TopoNode::EdgeDef::PCurveEntry aNewEntry;
  aNewEntry.PCurveNodeId   = aPCId;
  aNewEntry.FaceDefId      = theFaceDef;
  aNewEntry.EdgeOrientation = theEdgeOrientation;
  anEdgeDef.PCurves.Append(aNewEntry);

  markModified(theEdgeDef);
  return aPCId;
}

//=================================================================================================

void BRepGraph::ReplaceEdgeInWire(int              theWireDefIdx,
                                  BRepGraph_NodeId theOldEdgeDef,
                                  BRepGraph_NodeId theNewEdgeDef,
                                  bool             theReversed)
{
  BRepGraph_Mutator::ReplaceEdgeInWire(*this, theWireDefIdx, theOldEdgeDef, theNewEdgeDef, theReversed);
}

//=================================================================================================

void BRepGraph::SplitEdge(BRepGraph_NodeId  theEdgeDef,
                           BRepGraph_NodeId  theSplitVertex,
                           double            theSplitParam,
                           BRepGraph_NodeId& theSubA,
                           BRepGraph_NodeId& theSubB)
{
  BRepGraph_Mutator::SplitEdge(*this, theEdgeDef, theSplitVertex, theSplitParam, theSubA, theSubB);
}

//=================================================================================================

int BRepGraph::FaceCountForEdge(int theEdgeDefIdx) const
{
  const NCollection_Vector<int>& aWires = WiresOfEdge(theEdgeDefIdx);
  NCollection_Map<int>           aFaceSet;
  for (int aWIdx = 0; aWIdx < aWires.Length(); ++aWIdx)
  {
    const BRepGraph_TopoNode::WireDef& aWireDef = myWireDefs.Value(aWires.Value(aWIdx));
    for (int aUsIdx = 0; aUsIdx < aWireDef.Usages.Length(); ++aUsIdx)
    {
      const BRepGraph_TopoNode::WireUsage& aWireUsage =
        myWireUsages.Value(aWireDef.Usages.Value(aUsIdx).Index);
      if (aWireUsage.OwnerFaceUsage.IsValid())
      {
        BRepGraph_NodeId aFaceDefId = DefOf(aWireUsage.OwnerFaceUsage);
        if (aFaceDefId.IsValid())
          aFaceSet.Add(aFaceDefId.Index);
      }
    }
  }
  return aFaceSet.Extent();
}

//=================================================================================================

const NCollection_Vector<int>& BRepGraph::WiresOfEdge(int theEdgeDefIdx) const
{
  static const NCollection_Vector<int> THE_EMPTY;
  const NCollection_Vector<int>*       aResult = myEdgeToWires.Seek(theEdgeDefIdx);
  return aResult != nullptr ? *aResult : THE_EMPTY;
}

//=================================================================================================

void BRepGraph::RecordHistory(const TCollection_AsciiString&              theOpLabel,
                              BRepGraph_NodeId                            theOriginal,
                              const NCollection_Vector<BRepGraph_NodeId>& theReplacements)
{
  myHistoryLog.Record(theOpLabel, theOriginal, theReplacements);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::AddVertexDef(const gp_Pnt& thePoint,
                                         double        theTolerance)
{
  BRepGraph_TopoNode::VertexDef& aVtxDef = myVertexDefs.Appended();
  const int aIdx = myVertexDefs.Length() - 1;
  aVtxDef.Id        = BRepGraph_NodeId(BRepGraph_NodeKind::Vertex, aIdx);
  aVtxDef.Point     = thePoint;
  aVtxDef.Tolerance = theTolerance;
  allocateUID(aVtxDef.Id);
  return aVtxDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::AddEdgeDef(BRepGraph_NodeId          theStartVtx,
                                       BRepGraph_NodeId          theEndVtx,
                                       const Handle(Geom_Curve)& theCurve,
                                       double                    theFirst,
                                       double                    theLast,
                                       double                    theTolerance)
{
  BRepGraph_TopoNode::EdgeDef& anEdgeDef = myEdgeDefs.Appended();
  const int aIdx = myEdgeDefs.Length() - 1;
  anEdgeDef.Id               = BRepGraph_NodeId(BRepGraph_NodeKind::Edge, aIdx);
  anEdgeDef.StartVertexDefId = theStartVtx;
  anEdgeDef.EndVertexDefId   = theEndVtx;
  anEdgeDef.ParamFirst       = theFirst;
  anEdgeDef.ParamLast        = theLast;
  anEdgeDef.Tolerance        = theTolerance;
  anEdgeDef.SameParameter    = true;
  anEdgeDef.SameRange        = true;
  allocateUID(anEdgeDef.Id);

  if (!theCurve.IsNull())
  {
    anEdgeDef.CurveNodeId = registerCurve(theCurve);
    if (anEdgeDef.CurveNodeId.IsValid())
      myCurves.ChangeValue(anEdgeDef.CurveNodeId.Index).EdgeDefUsers.Append(anEdgeDef.Id);
  }

  return anEdgeDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::AddWireDef(
  const NCollection_Vector<BRepGraph_TopoNode::WireDef::EdgeEntry>& theEdges)
{
  BRepGraph_TopoNode::WireDef& aWireDef = myWireDefs.Appended();
  const int aIdx = myWireDefs.Length() - 1;
  aWireDef.Id = BRepGraph_NodeId(BRepGraph_NodeKind::Wire, aIdx);
  allocateUID(aWireDef.Id);

  for (int anEdgeIdx = 0; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    aWireDef.OrderedEdges.Append(theEdges.Value(anEdgeIdx));

    const int anEdgeDefIdx = theEdges.Value(anEdgeIdx).EdgeDefId.Index;
    if (!myEdgeToWires.IsBound(anEdgeDefIdx))
      myEdgeToWires.Bind(anEdgeDefIdx, NCollection_Vector<int>());
    myEdgeToWires.ChangeFind(anEdgeDefIdx).Append(aIdx);
  }

  // Check closure: first edge start == last edge end.
  if (!theEdges.IsEmpty())
  {
    const BRepGraph_TopoNode::WireDef::EdgeEntry& aFirst = theEdges.First();
    const BRepGraph_TopoNode::WireDef::EdgeEntry& aLast  = theEdges.Last();
    const BRepGraph_TopoNode::EdgeDef& aFirstEdge = myEdgeDefs.Value(aFirst.EdgeDefId.Index);
    const BRepGraph_TopoNode::EdgeDef& aLastEdge  = myEdgeDefs.Value(aLast.EdgeDefId.Index);

    BRepGraph_NodeId aFirstVtx = (aFirst.OrientationInWire == TopAbs_FORWARD)
                                   ? aFirstEdge.StartVertexDefId : aFirstEdge.EndVertexDefId;
    BRepGraph_NodeId aLastVtx  = (aLast.OrientationInWire == TopAbs_FORWARD)
                                   ? aLastEdge.EndVertexDefId : aLastEdge.StartVertexDefId;
    aWireDef.IsClosed = aFirstVtx.IsValid() && aLastVtx.IsValid() && aFirstVtx == aLastVtx;
  }

  return aWireDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::AddFaceDef(const Handle(Geom_Surface)&              theSurface,
                                       BRepGraph_NodeId                         theOuterWire,
                                       const NCollection_Vector<BRepGraph_NodeId>& theInnerWires,
                                       double                                   theTolerance)
{
  BRepGraph_TopoNode::FaceDef& aFaceDef = myFaceDefs.Appended();
  const int aIdx = myFaceDefs.Length() - 1;
  aFaceDef.Id        = BRepGraph_NodeId(BRepGraph_NodeKind::Face, aIdx);
  aFaceDef.Tolerance = theTolerance;
  allocateUID(aFaceDef.Id);

  aFaceDef.SurfNodeId = registerSurface(theSurface, Handle(Poly_Triangulation)());
  if (aFaceDef.SurfNodeId.IsValid())
    mySurfaces.ChangeValue(aFaceDef.SurfNodeId.Index).FaceDefUsers.Append(aFaceDef.Id);

  BRepGraph_TopoNode::FaceUsage& aFaceUsage = myFaceUsages.Appended();
  const int aFaceUsageIdx = myFaceUsages.Length() - 1;
  aFaceUsage.UsageId = BRepGraph_UsageId(BRepGraph_NodeKind::Face, aFaceUsageIdx);
  aFaceUsage.DefId   = aFaceDef.Id;
  myFaceDefs.ChangeValue(aIdx).Usages.Append(aFaceUsage.UsageId);

  if (theOuterWire.IsValid())
  {
    BRepGraph_TopoNode::WireUsage& aWireUsage = myWireUsages.Appended();
    const int aWireUsageIdx = myWireUsages.Length() - 1;
    aWireUsage.UsageId        = BRepGraph_UsageId(BRepGraph_NodeKind::Wire, aWireUsageIdx);
    aWireUsage.DefId          = theOuterWire;
    aWireUsage.OwnerFaceUsage = aFaceUsage.UsageId;
    myWireDefs.ChangeValue(theOuterWire.Index).Usages.Append(aWireUsage.UsageId);
    myFaceUsages.ChangeValue(aFaceUsageIdx).OuterWireUsage = aWireUsage.UsageId;
  }
  for (int aWireIdx = 0; aWireIdx < theInnerWires.Length(); ++aWireIdx)
  {
    const BRepGraph_NodeId& aWireDefId = theInnerWires.Value(aWireIdx);
    if (!aWireDefId.IsValid())
      continue;
    BRepGraph_TopoNode::WireUsage& aWireUsage = myWireUsages.Appended();
    const int aWireUsageIdx = myWireUsages.Length() - 1;
    aWireUsage.UsageId        = BRepGraph_UsageId(BRepGraph_NodeKind::Wire, aWireUsageIdx);
    aWireUsage.DefId          = aWireDefId;
    aWireUsage.OwnerFaceUsage = aFaceUsage.UsageId;
    myWireDefs.ChangeValue(aWireDefId.Index).Usages.Append(aWireUsage.UsageId);
    myFaceUsages.ChangeValue(aFaceUsageIdx).InnerWireUsages.Append(aWireUsage.UsageId);
  }

  return aFaceDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::AddShellDef()
{
  BRepGraph_TopoNode::ShellDef& aShellDef = myShellDefs.Appended();
  const int aIdx = myShellDefs.Length() - 1;
  aShellDef.Id = BRepGraph_NodeId(BRepGraph_NodeKind::Shell, aIdx);
  allocateUID(aShellDef.Id);
  return aShellDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::AddSolidDef()
{
  BRepGraph_TopoNode::SolidDef& aSolidDef = mySolidDefs.Appended();
  const int aIdx = mySolidDefs.Length() - 1;
  aSolidDef.Id = BRepGraph_NodeId(BRepGraph_NodeKind::Solid, aIdx);
  allocateUID(aSolidDef.Id);
  return aSolidDef.Id;
}

//=================================================================================================

BRepGraph_UsageId BRepGraph::AddFaceToShell(BRepGraph_NodeId   theShellDef,
                                            BRepGraph_NodeId   theFaceDef,
                                            TopAbs_Orientation theOri)
{
  BRepGraph_TopoNode::ShellDef& aShellDef = myShellDefs.ChangeValue(theShellDef.Index);
  BRepGraph_UsageId aShellUsageId;
  if (aShellDef.Usages.IsEmpty())
  {
    BRepGraph_TopoNode::ShellUsage& aShellUsage = myShellUsages.Appended();
    const int aShellUsIdx = myShellUsages.Length() - 1;
    aShellUsage.UsageId = BRepGraph_UsageId(BRepGraph_NodeKind::Shell, aShellUsIdx);
    aShellUsage.DefId   = theShellDef;
    aShellDef.Usages.Append(aShellUsage.UsageId);
    aShellUsageId = aShellUsage.UsageId;
  }
  else
  {
    aShellUsageId = aShellDef.Usages.Value(0);
  }

  BRepGraph_TopoNode::FaceUsage& aFaceUsage = myFaceUsages.Appended();
  const int aFaceUsIdx = myFaceUsages.Length() - 1;
  aFaceUsage.UsageId     = BRepGraph_UsageId(BRepGraph_NodeKind::Face, aFaceUsIdx);
  aFaceUsage.DefId       = theFaceDef;
  aFaceUsage.Orientation = theOri;
  aFaceUsage.ParentUsage = aShellUsageId;
  myFaceDefs.ChangeValue(theFaceDef.Index).Usages.Append(aFaceUsage.UsageId);
  myShellUsages.ChangeValue(aShellUsageId.Index).FaceUsages.Append(aFaceUsage.UsageId);

  const BRepGraph_TopoNode::FaceDef& aFaceDef = myFaceDefs.Value(theFaceDef.Index);
  if (aFaceDef.Usages.Length() > 1)
  {
    const BRepGraph_TopoNode::FaceUsage& aFirstFaceUsage =
      myFaceUsages.Value(aFaceDef.Usages.Value(0).Index);
    if (aFirstFaceUsage.OuterWireUsage.IsValid())
    {
      BRepGraph_NodeId aOuterWireDef = DefOf(aFirstFaceUsage.OuterWireUsage);
      BRepGraph_TopoNode::WireUsage& aWireUsage = myWireUsages.Appended();
      const int aWireUsIdx = myWireUsages.Length() - 1;
      aWireUsage.UsageId        = BRepGraph_UsageId(BRepGraph_NodeKind::Wire, aWireUsIdx);
      aWireUsage.DefId          = aOuterWireDef;
      aWireUsage.OwnerFaceUsage = aFaceUsage.UsageId;
      myWireDefs.ChangeValue(aOuterWireDef.Index).Usages.Append(aWireUsage.UsageId);
      myFaceUsages.ChangeValue(aFaceUsIdx).OuterWireUsage = aWireUsage.UsageId;
    }
    for (int aWIdx = 0; aWIdx < aFirstFaceUsage.InnerWireUsages.Length(); ++aWIdx)
    {
      BRepGraph_NodeId aInnerWireDef = DefOf(aFirstFaceUsage.InnerWireUsages.Value(aWIdx));
      BRepGraph_TopoNode::WireUsage& aWireUsage = myWireUsages.Appended();
      const int aWireUsIdx = myWireUsages.Length() - 1;
      aWireUsage.UsageId        = BRepGraph_UsageId(BRepGraph_NodeKind::Wire, aWireUsIdx);
      aWireUsage.DefId          = aInnerWireDef;
      aWireUsage.OwnerFaceUsage = aFaceUsage.UsageId;
      myWireDefs.ChangeValue(aInnerWireDef.Index).Usages.Append(aWireUsage.UsageId);
      myFaceUsages.ChangeValue(aFaceUsIdx).InnerWireUsages.Append(aWireUsage.UsageId);
    }
  }

  markModified(theShellDef);
  return aFaceUsage.UsageId;
}

//=================================================================================================

BRepGraph_UsageId BRepGraph::AddShellToSolid(BRepGraph_NodeId   theSolidDef,
                                             BRepGraph_NodeId   theShellDef,
                                             TopAbs_Orientation theOri)
{
  BRepGraph_TopoNode::SolidDef& aSolidDef = mySolidDefs.ChangeValue(theSolidDef.Index);
  BRepGraph_UsageId aSolidUsageId;
  if (aSolidDef.Usages.IsEmpty())
  {
    BRepGraph_TopoNode::SolidUsage& aSolidUsage = mySolidUsages.Appended();
    const int aSolidUsIdx = mySolidUsages.Length() - 1;
    aSolidUsage.UsageId = BRepGraph_UsageId(BRepGraph_NodeKind::Solid, aSolidUsIdx);
    aSolidUsage.DefId   = theSolidDef;
    aSolidDef.Usages.Append(aSolidUsage.UsageId);
    aSolidUsageId = aSolidUsage.UsageId;
  }
  else
  {
    aSolidUsageId = aSolidDef.Usages.Value(0);
  }

  BRepGraph_TopoNode::ShellUsage& aShellUsage = myShellUsages.Appended();
  const int aShellUsIdx = myShellUsages.Length() - 1;
  aShellUsage.UsageId     = BRepGraph_UsageId(BRepGraph_NodeKind::Shell, aShellUsIdx);
  aShellUsage.DefId       = theShellDef;
  aShellUsage.Orientation = theOri;
  aShellUsage.ParentUsage = aSolidUsageId;
  myShellDefs.ChangeValue(theShellDef.Index).Usages.Append(aShellUsage.UsageId);
  mySolidUsages.ChangeValue(aSolidUsageId.Index).ShellUsages.Append(aShellUsage.UsageId);

  markModified(theSolidDef);
  return aShellUsage.UsageId;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::AddCompoundDef(
  const NCollection_Vector<BRepGraph_NodeId>& theChildDefs)
{
  BRepGraph_TopoNode::CompoundDef& aCompDef = myCompoundDefs.Appended();
  const int aIdx = myCompoundDefs.Length() - 1;
  aCompDef.Id = BRepGraph_NodeId(BRepGraph_NodeKind::Compound, aIdx);
  allocateUID(aCompDef.Id);

  for (int aChildIdx = 0; aChildIdx < theChildDefs.Length(); ++aChildIdx)
    aCompDef.ChildDefIds.Append(theChildDefs.Value(aChildIdx));

  BRepGraph_TopoNode::CompoundUsage& aCompUsage = myCompoundUsages.Appended();
  const int aCompUsIdx = myCompoundUsages.Length() - 1;
  aCompUsage.UsageId = BRepGraph_UsageId(BRepGraph_NodeKind::Compound, aCompUsIdx);
  aCompUsage.DefId   = aCompDef.Id;
  myCompoundDefs.ChangeValue(aIdx).Usages.Append(aCompUsage.UsageId);

  return aCompDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::AddCompSolidDef(
  const NCollection_Vector<BRepGraph_NodeId>& theSolidDefs)
{
  BRepGraph_TopoNode::CompSolidDef& aCSolDef = myCompSolidDefs.Appended();
  const int aIdx = myCompSolidDefs.Length() - 1;
  aCSolDef.Id = BRepGraph_NodeId(BRepGraph_NodeKind::CompSolid, aIdx);
  allocateUID(aCSolDef.Id);

  for (int aSolIdx = 0; aSolIdx < theSolidDefs.Length(); ++aSolIdx)
    aCSolDef.SolidDefIds.Append(theSolidDefs.Value(aSolIdx));

  BRepGraph_TopoNode::CompSolidUsage& aCSolUsage = myCompSolidUsages.Appended();
  const int aCSolUsIdx = myCompSolidUsages.Length() - 1;
  aCSolUsage.UsageId = BRepGraph_UsageId(BRepGraph_NodeKind::CompSolid, aCSolUsIdx);
  aCSolUsage.DefId   = aCSolDef.Id;
  myCompSolidDefs.ChangeValue(aIdx).Usages.Append(aCSolUsage.UsageId);

  return aCSolDef.Id;
}

//=================================================================================================

void BRepGraph::RemoveNode(BRepGraph_NodeId theNode)
{
  BRepGraph_TopoNode::BaseDef* aDef =
    const_cast<BRepGraph_TopoNode::BaseDef*>(TopoDef(theNode));
  if (aDef == nullptr)
    return;

  aDef->IsRemoved = true;

  myOutRelEdges.UnBind(theNode);
  myInRelEdges.UnBind(theNode);

  Invalidate(theNode);

  {
    std::unique_lock<std::shared_mutex> aWriteLock(myCurrentShapesMutex);
    myCurrentShapes.UnBind(theNode);
  }
}

//=================================================================================================

bool BRepGraph::IsRemoved(BRepGraph_NodeId theNode) const
{
  const BRepGraph_TopoNode::BaseDef* aDef = TopoDef(theNode);
  if (aDef == nullptr)
    return false;
  return aDef->IsRemoved;
}

//=================================================================================================

void BRepGraph::RemoveSubgraph(BRepGraph_NodeId theNode)
{
  RemoveNode(theNode);

  switch (theNode.Kind)
  {
    case BRepGraph_NodeKind::Compound: {
      const BRepGraph_TopoNode::CompoundDef& aCompDef = myCompoundDefs.Value(theNode.Index);
      for (int aChildIdx = 0; aChildIdx < aCompDef.ChildDefIds.Length(); ++aChildIdx)
        RemoveSubgraph(aCompDef.ChildDefIds.Value(aChildIdx));
      break;
    }
    case BRepGraph_NodeKind::CompSolid: {
      const BRepGraph_TopoNode::CompSolidDef& aCSolDef = myCompSolidDefs.Value(theNode.Index);
      for (int aSolIdx = 0; aSolIdx < aCSolDef.SolidDefIds.Length(); ++aSolIdx)
        RemoveSubgraph(aCSolDef.SolidDefIds.Value(aSolIdx));
      break;
    }
    case BRepGraph_NodeKind::Solid: {
      const BRepGraph_TopoNode::SolidDef& aSolidDef = mySolidDefs.Value(theNode.Index);
      for (int aUsIdx = 0; aUsIdx < aSolidDef.Usages.Length(); ++aUsIdx)
      {
        const BRepGraph_TopoNode::SolidUsage& aUsage =
          mySolidUsages.Value(aSolidDef.Usages.Value(aUsIdx).Index);
        for (int aShellIter = 0; aShellIter < aUsage.ShellUsages.Length(); ++aShellIter)
          RemoveSubgraph(DefOf(aUsage.ShellUsages.Value(aShellIter)));
      }
      break;
    }
    case BRepGraph_NodeKind::Shell: {
      const BRepGraph_TopoNode::ShellDef& aShellDef = myShellDefs.Value(theNode.Index);
      for (int aUsIdx = 0; aUsIdx < aShellDef.Usages.Length(); ++aUsIdx)
      {
        const BRepGraph_TopoNode::ShellUsage& aUsage =
          myShellUsages.Value(aShellDef.Usages.Value(aUsIdx).Index);
        for (int aFaceIter = 0; aFaceIter < aUsage.FaceUsages.Length(); ++aFaceIter)
          RemoveSubgraph(DefOf(aUsage.FaceUsages.Value(aFaceIter)));
      }
      break;
    }
    case BRepGraph_NodeKind::Face: {
      const BRepGraph_TopoNode::FaceDef& aFaceDef = myFaceDefs.Value(theNode.Index);
      for (int aUsIdx = 0; aUsIdx < aFaceDef.Usages.Length(); ++aUsIdx)
      {
        const BRepGraph_TopoNode::FaceUsage& aUsage =
          myFaceUsages.Value(aFaceDef.Usages.Value(aUsIdx).Index);
        if (aUsage.OuterWireUsage.IsValid())
          RemoveSubgraph(DefOf(aUsage.OuterWireUsage));
        for (int aWireIter = 0; aWireIter < aUsage.InnerWireUsages.Length(); ++aWireIter)
          RemoveSubgraph(DefOf(aUsage.InnerWireUsages.Value(aWireIter)));
      }
      break;
    }
    case BRepGraph_NodeKind::Wire: {
      const BRepGraph_TopoNode::WireDef& aWireDef = myWireDefs.Value(theNode.Index);
      for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.OrderedEdges.Length(); ++anEdgeIdx)
      {
        BRepGraph_NodeId anEdgeDefId = aWireDef.OrderedEdges.Value(anEdgeIdx).EdgeDefId;
        RemoveSubgraph(anEdgeDefId);
      }
      break;
    }
    case BRepGraph_NodeKind::Edge: {
      const BRepGraph_TopoNode::EdgeDef& anEdgeDef = myEdgeDefs.Value(theNode.Index);
      if (anEdgeDef.StartVertexDefId.IsValid())
        RemoveNode(anEdgeDef.StartVertexDefId);
      if (anEdgeDef.EndVertexDefId.IsValid())
        RemoveNode(anEdgeDef.EndVertexDefId);
      break;
    }
    default:
      break;
  }
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::FacesOfEdge(BRepGraph_NodeId theEdgeDef) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const NCollection_Vector<int>& aWires = WiresOfEdge(theEdgeDef.Index);
  NCollection_Map<int> aFaceSet;
  for (int aWIdx = 0; aWIdx < aWires.Length(); ++aWIdx)
  {
    const BRepGraph_TopoNode::WireDef& aWireDef = myWireDefs.Value(aWires.Value(aWIdx));
    for (int aUsIdx = 0; aUsIdx < aWireDef.Usages.Length(); ++aUsIdx)
    {
      const BRepGraph_TopoNode::WireUsage& aWireUsage =
        myWireUsages.Value(aWireDef.Usages.Value(aUsIdx).Index);
      if (aWireUsage.OwnerFaceUsage.IsValid())
      {
        BRepGraph_NodeId aFaceDefId = DefOf(aWireUsage.OwnerFaceUsage);
        if (aFaceDefId.IsValid() && aFaceSet.Add(aFaceDefId.Index))
          aResult.Append(aFaceDefId);
      }
    }
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SharedEdges(BRepGraph_NodeId theFaceA,
                                                            BRepGraph_NodeId theFaceB) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;

  NCollection_Map<int> aEdgesA;
  for (int aUsIdx = 0; aUsIdx < myFaceDefs.Value(theFaceA.Index).Usages.Length(); ++aUsIdx)
  {
    const BRepGraph_TopoNode::FaceUsage& aFaceUsage =
      myFaceUsages.Value(myFaceDefs.Value(theFaceA.Index).Usages.Value(aUsIdx).Index);
    auto collectWireEdges = [&](BRepGraph_UsageId theWireUsageId)
    {
      if (!theWireUsageId.IsValid())
        return;
      BRepGraph_NodeId aWireDefId = DefOf(theWireUsageId);
      const BRepGraph_TopoNode::WireDef& aWireDef = myWireDefs.Value(aWireDefId.Index);
      for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.OrderedEdges.Length(); ++anEdgeIdx)
        aEdgesA.Add(aWireDef.OrderedEdges.Value(anEdgeIdx).EdgeDefId.Index);
    };
    collectWireEdges(aFaceUsage.OuterWireUsage);
    for (int aWIdx = 0; aWIdx < aFaceUsage.InnerWireUsages.Length(); ++aWIdx)
      collectWireEdges(aFaceUsage.InnerWireUsages.Value(aWIdx));
  }

  NCollection_Map<int> aAdded;
  for (int aUsIdx = 0; aUsIdx < myFaceDefs.Value(theFaceB.Index).Usages.Length(); ++aUsIdx)
  {
    const BRepGraph_TopoNode::FaceUsage& aFaceUsage =
      myFaceUsages.Value(myFaceDefs.Value(theFaceB.Index).Usages.Value(aUsIdx).Index);
    auto checkWireEdges = [&](BRepGraph_UsageId theWireUsageId)
    {
      if (!theWireUsageId.IsValid())
        return;
      BRepGraph_NodeId aWireDefId = DefOf(theWireUsageId);
      const BRepGraph_TopoNode::WireDef& aWireDef = myWireDefs.Value(aWireDefId.Index);
      for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.OrderedEdges.Length(); ++anEdgeIdx)
      {
        int anEdgeDefIdx = aWireDef.OrderedEdges.Value(anEdgeIdx).EdgeDefId.Index;
        if (aEdgesA.Contains(anEdgeDefIdx) && aAdded.Add(anEdgeDefIdx))
          aResult.Append(BRepGraph_NodeId(BRepGraph_NodeKind::Edge, anEdgeDefIdx));
      }
    };
    checkWireEdges(aFaceUsage.OuterWireUsage);
    for (int aWIdx = 0; aWIdx < aFaceUsage.InnerWireUsages.Length(); ++aWIdx)
      checkWireEdges(aFaceUsage.InnerWireUsages.Value(aWIdx));
  }

  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::AdjacentFaces(BRepGraph_NodeId theFaceDef) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  NCollection_Map<int> aFaceSet;

  for (int aUsIdx = 0; aUsIdx < myFaceDefs.Value(theFaceDef.Index).Usages.Length(); ++aUsIdx)
  {
    const BRepGraph_TopoNode::FaceUsage& aFaceUsage =
      myFaceUsages.Value(myFaceDefs.Value(theFaceDef.Index).Usages.Value(aUsIdx).Index);
    auto processWire = [&](BRepGraph_UsageId theWireUsageId)
    {
      if (!theWireUsageId.IsValid())
        return;
      BRepGraph_NodeId aWireDefId = DefOf(theWireUsageId);
      const BRepGraph_TopoNode::WireDef& aWireDef = myWireDefs.Value(aWireDefId.Index);
      for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.OrderedEdges.Length(); ++anEdgeIdx)
      {
        BRepGraph_NodeId anEdgeDefId = aWireDef.OrderedEdges.Value(anEdgeIdx).EdgeDefId;
        NCollection_Vector<BRepGraph_NodeId> aFaces = FacesOfEdge(anEdgeDefId);
        for (int aFIdx = 0; aFIdx < aFaces.Length(); ++aFIdx)
        {
          if (aFaces.Value(aFIdx) != theFaceDef && aFaceSet.Add(aFaces.Value(aFIdx).Index))
            aResult.Append(aFaces.Value(aFIdx));
        }
      }
    };
    processWire(aFaceUsage.OuterWireUsage);
    for (int aWIdx = 0; aWIdx < aFaceUsage.InnerWireUsages.Length(); ++aWIdx)
      processWire(aFaceUsage.InnerWireUsages.Value(aWIdx));
  }

  return aResult;
}

//=================================================================================================

void BRepGraph::SetAllocator(const Handle(NCollection_BaseAllocator)& theAlloc)
{
  myAllocator = !theAlloc.IsNull() ? theAlloc : NCollection_BaseAllocator::CommonBaseAllocator();

  mySolidDefs       = NCollection_Vector<BRepGraph_TopoNode::SolidDef>(16, myAllocator);
  myShellDefs       = NCollection_Vector<BRepGraph_TopoNode::ShellDef>(16, myAllocator);
  myFaceDefs        = NCollection_Vector<BRepGraph_TopoNode::FaceDef>(128, myAllocator);
  myWireDefs        = NCollection_Vector<BRepGraph_TopoNode::WireDef>(128, myAllocator);
  myEdgeDefs        = NCollection_Vector<BRepGraph_TopoNode::EdgeDef>(256, myAllocator);
  myVertexDefs      = NCollection_Vector<BRepGraph_TopoNode::VertexDef>(256, myAllocator);
  myCompoundDefs    = NCollection_Vector<BRepGraph_TopoNode::CompoundDef>(8, myAllocator);
  myCompSolidDefs   = NCollection_Vector<BRepGraph_TopoNode::CompSolidDef>(8, myAllocator);
  mySolidUsages     = NCollection_Vector<BRepGraph_TopoNode::SolidUsage>(16, myAllocator);
  myShellUsages     = NCollection_Vector<BRepGraph_TopoNode::ShellUsage>(16, myAllocator);
  myFaceUsages      = NCollection_Vector<BRepGraph_TopoNode::FaceUsage>(128, myAllocator);
  myWireUsages      = NCollection_Vector<BRepGraph_TopoNode::WireUsage>(128, myAllocator);
  myEdgeUsages      = NCollection_Vector<BRepGraph_TopoNode::EdgeUsage>(256, myAllocator);
  myVertexUsages    = NCollection_Vector<BRepGraph_TopoNode::VertexUsage>(256, myAllocator);
  myCompoundUsages  = NCollection_Vector<BRepGraph_TopoNode::CompoundUsage>(8, myAllocator);
  myCompSolidUsages = NCollection_Vector<BRepGraph_TopoNode::CompSolidUsage>(8, myAllocator);
  mySurfaces       = NCollection_Vector<BRepGraph_GeomNode::Surf>(64, myAllocator);
  myCurves         = NCollection_Vector<BRepGraph_GeomNode::Curve>(64, myAllocator);
  myPCurves        = NCollection_Vector<BRepGraph_GeomNode::PCurve>(128, myAllocator);
  mySurfRegistry   = NCollection_IndexedDataMap<const Geom_Surface*, int,
                       BRepGraph_PtrHasher<Geom_Surface>>(100, myAllocator);
  myCurveRegistry  = NCollection_IndexedDataMap<const Geom_Curve*, int,
                       BRepGraph_PtrHasher<Geom_Curve>>(100, myAllocator);
  myTShapeToDefId  = NCollection_DataMap<const TopoDS_TShape*, BRepGraph_NodeId,
                       BRepGraph_PtrHasher<TopoDS_TShape>>(100, myAllocator);
  myUIDToNodeId    = NCollection_DataMap<BRepGraph_UID, BRepGraph_NodeId, BRepGraph_UID::Hasher>(
    BRepGraph_UID::Hasher(), 100, myAllocator);
  myEdgeToWires = NCollection_DataMap<int, NCollection_Vector<int>>(100, myAllocator);
}

const Handle(NCollection_BaseAllocator)& BRepGraph::Allocator() const { return myAllocator; }
void BRepGraph::SetHistoryEnabled(bool theVal) { myHistoryLog.SetEnabled(theVal); }
bool BRepGraph::IsHistoryEnabled() const { return myHistoryLog.IsEnabled(); }

//=================================================================================================

void BRepGraph::markModified(BRepGraph_NodeId theDefId)
{
  if (!theDefId.IsValid())
    return;

  const BRepGraph_TopoNode::BaseDef* aDef = TopoDef(theDefId);
  if (aDef == nullptr)
    return;

  const_cast<BRepGraph_TopoNode::BaseDef*>(aDef)->IsModified = true;
  {
    std::unique_lock<std::shared_mutex> aWriteLock(myCurrentShapesMutex);
    myCurrentShapes.UnBind(theDefId);
  }

  // Propagate upward via usages' parent chain.
  for (int aUsIdx = 0; aUsIdx < aDef->Usages.Length(); ++aUsIdx)
  {
    BRepGraph_UsageId aParent;
    switch (theDefId.Kind)
    {
      case BRepGraph_NodeKind::Edge: {
        const NCollection_Vector<int>* aWires = myEdgeToWires.Seek(theDefId.Index);
        if (aWires != nullptr)
        {
          for (int aWIdx = 0; aWIdx < aWires->Length(); ++aWIdx)
            markModified(BRepGraph_NodeId(BRepGraph_NodeKind::Wire, aWires->Value(aWIdx)));
        }
        return;
      }
      case BRepGraph_NodeKind::Vertex: {
        return;
      }
      default: {
        const BRepGraph_UsageId& aUsageId = aDef->Usages.Value(aUsIdx);
        switch (aUsageId.Kind)
        {
          case BRepGraph_NodeKind::Solid:  aParent = mySolidUsages.Value(aUsageId.Index).ParentUsage; break;
          case BRepGraph_NodeKind::Shell:  aParent = myShellUsages.Value(aUsageId.Index).ParentUsage; break;
          case BRepGraph_NodeKind::Face:   aParent = myFaceUsages.Value(aUsageId.Index).ParentUsage; break;
          case BRepGraph_NodeKind::Wire: {
            const BRepGraph_TopoNode::WireUsage& aWu = myWireUsages.Value(aUsageId.Index);
            if (aWu.OwnerFaceUsage.IsValid())
              markModified(DefOf(aWu.OwnerFaceUsage));
            continue;
          }
          default: continue;
        }
        if (aParent.IsValid())
          markModified(DefOf(aParent));
      }
    }
  }
}

//=================================================================================================

TopoDS_Shape BRepGraph::Shape(BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
    return TopoDS_Shape();

  // Fast path: check unmodified originals (read-only map, no lock needed).
  const BRepGraph_TopoNode::BaseDef* aDef = TopoDef(theNode);
  if (aDef != nullptr && !aDef->IsModified)
  {
    const TopoDS_Shape* anOrig = myOriginalShapes.Seek(theNode);
    if (anOrig != nullptr)
      return *anOrig;
  }

  // Check mutable cache under shared lock.
  {
    std::shared_lock<std::shared_mutex> aReadLock(myCurrentShapesMutex);
    const TopoDS_Shape* aCached = myCurrentShapes.Seek(theNode);
    if (aCached != nullptr)
      return *aCached;
  }

  // Reconstruct outside the lock to avoid holding it during expensive computation.
  TopoDS_Shape aReconstructed = BRepGraph_Reconstruct::Node(*this, theNode);

  // Store under exclusive lock with double-check.
  if (!aReconstructed.IsNull())
  {
    std::unique_lock<std::shared_mutex> aWriteLock(myCurrentShapesMutex);
    if (!myCurrentShapes.IsBound(theNode))
      myCurrentShapes.Bind(theNode, aReconstructed);
  }
  return aReconstructed;
}

bool BRepGraph::HasOriginalShape(BRepGraph_NodeId theNode) const
{ return myOriginalShapes.IsBound(theNode); }

const TopoDS_Shape& BRepGraph::OriginalOf(BRepGraph_NodeId theNode) const
{
  const TopoDS_Shape* aShape = myOriginalShapes.Seek(theNode);
  if (aShape == nullptr)
    throw Standard_ProgramError("BRepGraph::OriginalOf() -- no original shape for this node.");
  return *aShape;
}
