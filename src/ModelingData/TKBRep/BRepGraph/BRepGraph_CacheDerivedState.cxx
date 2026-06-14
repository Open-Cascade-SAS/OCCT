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

#include "BRepGraph_CacheDerivedState.hxx"

#include "BRepGraph.hxx"
#include <BRepGraph_CacheRegistry.hxx>
#include <BRepGraph_CopyRemap.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_TopoView.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_FlatMap.hxx>

#include <cmath>

IMPLEMENT_STANDARD_RTTIEXT(BRepGraph_CacheDerivedState, BRepGraph_Cache)

namespace
{

const Standard_GUID& theGUID()
{
  static const Standard_GUID aGUID("a1c2e3f4-5678-4abc-9def-0123456789ab");
  return aGUID;
}

template <typename T>
void ensureSize(NCollection_DynamicArray<T>& theArray, uint32_t theIndex)
{
  if (static_cast<size_t>(theIndex) >= theArray.Size())
  {
    theArray.SetValue(static_cast<size_t>(theIndex), T());
  }
}

template <typename IdT>
IdT remappedNode(const BRepGraph_CopyRemap& theCopy, const IdT theId)
{
  if (!theId.IsValid())
  {
    return IdT();
  }
  const BRepGraph_ItemId aTarget = theCopy.TargetItem(BRepGraph_ItemId(theId));
  if (!aTarget.IsNode())
  {
    return IdT();
  }
  return IdT::FromNodeId(aTarget.NodeId());
}

//=================================================================================================

BRepGraph_VertexId resolveChildVertex(const BRepGraph& theGraph, BRepGraph_VertexRefId theRef)
{
  if (!theRef.IsValid(theGraph.Refs().Vertices().Nb()) || theRef.IsRemoved(theGraph))
  {
    return BRepGraph_VertexId::Invalid();
  }
  const BRepGraphInc::VertexRef& aVRef = theGraph.Refs().Vertices().Entry(theRef);
  return aVRef.ChildVertexId.IsValid(theGraph.Topo().Vertices().Nb())
             && !aVRef.ChildVertexId.IsRemoved(theGraph)
           ? aVRef.ChildVertexId
           : BRepGraph_VertexId::Invalid();
}

//=================================================================================================

const occ::handle<Geom_Curve>& edgeCurve3D(const BRepGraph&       theGraph,
                                           const BRepGraph_EdgeId theEdge)
{
  return BRepGraph_Tool::Edge::Curve(theGraph, theEdge);
}

//=================================================================================================

const occ::handle<Geom2d_Curve>& coEdgePCurve(const BRepGraph&         theGraph,
                                              const BRepGraph_CoEdgeId theCoEdge)
{
  return BRepGraph_Tool::CoEdge::PCurve(theGraph, theCoEdge);
}

//=================================================================================================

occ::handle<Geom_Surface> faceSurface(const BRepGraph& theGraph, const BRepGraph_FaceId theFace)
{
  if (!theFace.IsValid(theGraph.Topo().Faces().Nb()) || theFace.IsRemoved(theGraph))
  {
    return occ::handle<Geom_Surface>();
  }
  return BRepGraph_Tool::Face::Surface(theGraph, theFace);
}

//=================================================================================================

bool coEdgeOrientedVertices(const BRepGraph&         theGraph,
                            const BRepGraph_CoEdgeId theCoEdge,
                            BRepGraph_VertexId&      theStartVertex,
                            BRepGraph_VertexId&      theEndVertex)
{
  if (!theCoEdge.IsValid(theGraph.Topo().CoEdges().Nb()) || theCoEdge.IsRemoved(theGraph))
  {
    return false;
  }

  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(theCoEdge);
  if (!aCoEdge.ChildEdgeId.IsValid(theGraph.Topo().Edges().Nb())
      || aCoEdge.ChildEdgeId.IsRemoved(theGraph))
  {
    return false;
  }

  const BRepGraphInc::EdgeDef& anEdge = theGraph.Topo().Edges().Definition(aCoEdge.ChildEdgeId);
  const BRepGraph_VertexRefId  aStartRef =
    aCoEdge.Orientation == TopAbs_REVERSED ? anEdge.EndVertexRefId : anEdge.StartVertexRefId;
  const BRepGraph_VertexRefId anEndRef =
    aCoEdge.Orientation == TopAbs_REVERSED ? anEdge.StartVertexRefId : anEdge.EndVertexRefId;

  theStartVertex = resolveChildVertex(theGraph, aStartRef);
  theEndVertex   = resolveChildVertex(theGraph, anEndRef);
  return theStartVertex.IsValid() && theEndVertex.IsValid();
}

//=================================================================================================

bool isCoEdgeSameRange(const BRepGraph&         theGraph,
                       const BRepGraph_CoEdgeId theCoEdge,
                       const BRepGraph_EdgeId   theEdge)
{
  if (!theCoEdge.IsValid(theGraph.Topo().CoEdges().Nb()) || theCoEdge.IsRemoved(theGraph))
  {
    return true;
  }

  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(theCoEdge);
  if (!aCoEdge.FaceId.IsValid(theGraph.Topo().Faces().Nb()) || aCoEdge.FaceId.IsRemoved(theGraph))
  {
    return true;
  }

  if (coEdgePCurve(theGraph, theCoEdge).IsNull())
  {
    return true;
  }

  const std::pair<double, double> anEdgeRange  = BRepGraph_Tool::Edge::Range(theGraph, theEdge);
  const std::pair<double, double> aPCurveRange = BRepGraph_Tool::CoEdge::Range(theGraph, theCoEdge);
  return std::abs(aPCurveRange.first - anEdgeRange.first) <= Precision::PConfusion()
         && std::abs(aPCurveRange.second - anEdgeRange.second) <= Precision::PConfusion();
}

//=================================================================================================

bool isCoEdgeSameParameter(const BRepGraph&                 theGraph,
                           const BRepGraph_CoEdgeId         theCoEdge,
                           const BRepGraphInc::EdgeDef&     theEdgeDef,
                           const occ::handle<Geom_Curve>&   theCurve3D,
                           const std::pair<double, double>& theEdgeRange)
{
  if (!theCoEdge.IsValid(theGraph.Topo().CoEdges().Nb()) || theCoEdge.IsRemoved(theGraph))
  {
    return true;
  }

  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(theCoEdge);
  if (!aCoEdge.FaceId.IsValid(theGraph.Topo().Faces().Nb()) || aCoEdge.FaceId.IsRemoved(theGraph))
  {
    return true;
  }

  const occ::handle<Geom2d_Curve>& aPCurve = coEdgePCurve(theGraph, theCoEdge);
  if (aPCurve.IsNull())
  {
    return true;
  }

  const occ::handle<Geom_Surface> aSurface = faceSurface(theGraph, aCoEdge.FaceId);
  if (aSurface.IsNull())
  {
    return true;
  }

  const double  aTol           = theEdgeDef.Tolerance + Precision::Confusion();
  constexpr int THE_NB_SAMPLES = 5;

  GeomAdaptor_Curve   aCurveAdaptor(theCurve3D, theEdgeRange.first, theEdgeRange.second);
  Geom2dAdaptor_Curve aPCurveAdaptor(aPCurve);
  GeomAdaptor_Surface aSurfAdaptor(aSurface);

  for (int anIdx = 0; anIdx <= THE_NB_SAMPLES; ++anIdx)
  {
    const double aParam =
      theEdgeRange.first + (theEdgeRange.second - theEdgeRange.first) * anIdx / THE_NB_SAMPLES;
    const gp_Pnt   aPoint3D      = aCurveAdaptor.EvalD0(aParam);
    const gp_Pnt2d aUV           = aPCurveAdaptor.EvalD0(aParam);
    const gp_Pnt   aSurfacePoint = aSurfAdaptor.EvalD0(aUV.X(), aUV.Y());
    if (aPoint3D.Distance(aSurfacePoint) > aTol)
    {
      return false;
    }
  }
  return true;
}

} // namespace

//=================================================================================================

const Standard_GUID& BRepGraph_CacheDerivedState::GetID()
{
  return theGUID();
}

//=================================================================================================

const Standard_GUID& BRepGraph_CacheDerivedState::ID() const
{
  return theGUID();
}

//=================================================================================================

const TCollection_AsciiString& BRepGraph_CacheDerivedState::Name() const
{
  static const TCollection_AsciiString aName("BRepGraph_CacheDerivedState");
  return aName;
}

//=================================================================================================

void BRepGraph_CacheDerivedState::Clear() noexcept
{
  std::lock_guard aLock(myMutex);
  for (EdgeEntry& aEntry : myEdgeEntries)
  {
    aEntry.Reset();
  }
  for (CoEdgeSameRangeEntry& aEntry : myCoEdgeSameRangeEntries)
  {
    aEntry.Reset();
  }
  for (WireEntry& aEntry : myWireEntries)
  {
    aEntry.Reset();
  }
  for (ShellEntry& aEntry : myShellEntries)
  {
    aEntry.Reset();
  }
}

//=================================================================================================

void BRepGraph_CacheDerivedState::CopyFreshTo(const BRepGraph_CopyRemap& theCopy) const
{
  occ::handle<BRepGraph_CacheDerivedState> aTargetCache =
    theCopy.TargetGraph().CacheRegistry().Ensure<BRepGraph_CacheDerivedState>();

  std::lock_guard aSourceLock(myMutex);
  std::lock_guard aTargetLock(aTargetCache->myMutex);

  for (const EdgeEntry& aSourceEntry : myEdgeEntries)
  {
    if (!aSourceEntry.IsFreshOwn(*this, aSourceEntry.Node()))
    {
      continue;
    }
    const BRepGraph_EdgeId aSourceEdge = BRepGraph_EdgeId::FromNodeId(aSourceEntry.Node());
    const BRepGraph_EdgeId aTargetEdge = remappedNode(theCopy, aSourceEdge);
    if (!aTargetEdge.IsValidIn(theCopy.TargetGraph().Topo().Edges()))
    {
      continue;
    }
    ensureSize(aTargetCache->myEdgeEntries, aTargetEdge.Index);
    EdgeEntry aTargetEntry = aSourceEntry;
    if (aTargetEntry.BindOwnGen(*aTargetCache, BRepGraph_NodeId(aTargetEdge)))
    {
      aTargetCache->myEdgeEntries.ChangeValue(static_cast<size_t>(aTargetEdge.Index)) =
        aTargetEntry;
    }
  }

  for (const CoEdgeSameRangeEntry& aSourceEntry : myCoEdgeSameRangeEntries)
  {
    if (!aSourceEntry.IsFreshOwn(*this, aSourceEntry.Node()))
    {
      continue;
    }
    const BRepGraph_CoEdgeId aSourceCoEdge = BRepGraph_CoEdgeId::FromNodeId(aSourceEntry.Node());
    const BRepGraph_CoEdgeId aTargetCoEdge = remappedNode(theCopy, aSourceCoEdge);
    if (!aTargetCoEdge.IsValidIn(theCopy.TargetGraph().Topo().CoEdges()))
    {
      continue;
    }
    ensureSize(aTargetCache->myCoEdgeSameRangeEntries, aTargetCoEdge.Index);
    CoEdgeSameRangeEntry aTargetEntry = aSourceEntry;
    if (aTargetEntry.BindOwnGen(*aTargetCache, BRepGraph_NodeId(aTargetCoEdge)))
    {
      aTargetCache->myCoEdgeSameRangeEntries.ChangeValue(static_cast<size_t>(aTargetCoEdge.Index)) =
        aTargetEntry;
    }
  }

  for (const WireEntry& aSourceEntry : myWireEntries)
  {
    if (!aSourceEntry.IsFreshOwn(*this, aSourceEntry.Node()))
    {
      continue;
    }
    const BRepGraph_WireId aSourceWire = BRepGraph_WireId::FromNodeId(aSourceEntry.Node());
    const BRepGraph_WireId aTargetWire = remappedNode(theCopy, aSourceWire);
    if (!aTargetWire.IsValidIn(theCopy.TargetGraph().Topo().Wires()))
    {
      continue;
    }
    ensureSize(aTargetCache->myWireEntries, aTargetWire.Index);
    WireEntry aTargetEntry = aSourceEntry;
    if (aTargetEntry.BindOwnGen(*aTargetCache, BRepGraph_NodeId(aTargetWire)))
    {
      aTargetCache->myWireEntries.ChangeValue(static_cast<size_t>(aTargetWire.Index)) =
        aTargetEntry;
    }
  }

  for (const ShellEntry& aSourceEntry : myShellEntries)
  {
    if (!aSourceEntry.IsFreshOwn(*this, aSourceEntry.Node()))
    {
      continue;
    }
    const BRepGraph_ShellId aSourceShell = BRepGraph_ShellId::FromNodeId(aSourceEntry.Node());
    const BRepGraph_ShellId aTargetShell = remappedNode(theCopy, aSourceShell);
    if (!aTargetShell.IsValidIn(theCopy.TargetGraph().Topo().Shells()))
    {
      continue;
    }
    ensureSize(aTargetCache->myShellEntries, aTargetShell.Index);
    ShellEntry aTargetEntry = aSourceEntry;
    if (aTargetEntry.BindOwnGen(*aTargetCache, BRepGraph_NodeId(aTargetShell)))
    {
      aTargetCache->myShellEntries.ChangeValue(static_cast<size_t>(aTargetShell.Index)) =
        aTargetEntry;
    }
  }
}

//=================================================================================================

void BRepGraph_CacheDerivedState::computeStatusOnly(const BRepGraph& theGraph,
                                                    BRepGraph_EdgeId theEdge,
                                                    EdgeEntry&       theEntry)
{
  const BRepGraphInc::EdgeDef& aDef = theGraph.Topo().Edges().Definition(theEdge);

  const BRepGraph_VertexId aStartV = resolveChildVertex(theGraph, aDef.StartVertexRefId);
  const BRepGraph_VertexId aEndV   = resolveChildVertex(theGraph, aDef.EndVertexRefId);
  const bool               aClosed = aStartV.IsValid() && aStartV == aEndV;

  const occ::handle<Geom_Curve>& aCurve3D = edgeCurve3D(theGraph, theEdge);
  EdgeEntry::GeomStatus          aStatus;
  if (!aCurve3D.IsNull())
  {
    aStatus = EdgeEntry::GeomStatus::HasCurve3D;
  }
  else if (aClosed)
  {
    aStatus = EdgeEntry::GeomStatus::DegenerateOnSurface;
  }
  else if (aStartV.IsValid() && aEndV.IsValid())
  {
    const BRepGraphInc::VertexDef& aStartDef = theGraph.Topo().Vertices().Definition(aStartV);
    const BRepGraphInc::VertexDef& aEndDef   = theGraph.Topo().Vertices().Definition(aEndV);
    aStatus = aStartDef.Point.Distance(aEndDef.Point) <= aDef.Tolerance
                ? EdgeEntry::GeomStatus::DegenerateOnSurface
                : EdgeEntry::GeomStatus::MissingCurve3D;
  }
  else
  {
    aStatus = EdgeEntry::GeomStatus::MissingCurve3D;
  }

  theEntry.Set(aStatus, aClosed);
}

//=================================================================================================

void BRepGraph_CacheDerivedState::computeSameRange(const BRepGraph&      theGraph,
                                                   BRepGraph_CoEdgeId    theCoEdge,
                                                   CoEdgeSameRangeEntry& theEntry)
{
  if (!theCoEdge.IsValid(theGraph.Topo().CoEdges().Nb()) || theCoEdge.IsRemoved(theGraph))
  {
    theEntry.SetSameRange(true);
    return;
  }

  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(theCoEdge);
  const BRepGraph_EdgeId         anEdge  = aCoEdge.ChildEdgeId;

  if (!anEdge.IsValid(theGraph.Topo().Edges().Nb()) || anEdge.IsRemoved(theGraph))
  {
    theEntry.SetSameRange(true);
    return;
  }

  const occ::handle<Geom_Curve>& aCurve3D = edgeCurve3D(theGraph, anEdge);
  if (!aCurve3D.IsNull())
  {
    theEntry.SetSameRange(isCoEdgeSameRange(theGraph, theCoEdge, anEdge));
  }
  else
  {
    const BRepGraphInc::EdgeDef& aDef    = theGraph.Topo().Edges().Definition(anEdge);
    const BRepGraph_VertexId     aStartV = resolveChildVertex(theGraph, aDef.StartVertexRefId);
    const BRepGraph_VertexId     aEndV   = resolveChildVertex(theGraph, aDef.EndVertexRefId);
    theEntry.SetSameRange(aStartV.IsValid() && aStartV == aEndV);
  }
}

//=================================================================================================

void BRepGraph_CacheDerivedState::computeSameParameter(const BRepGraph&      theGraph,
                                                       BRepGraph_CoEdgeId    theCoEdge,
                                                       CoEdgeSameRangeEntry& theEntry)
{
  if ((theEntry.Computed() & CoEdgeSameRangeEntry::ComputedSameRange) == 0)
  {
    computeSameRange(theGraph, theCoEdge, theEntry);
  }

  if (!theCoEdge.IsValid(theGraph.Topo().CoEdges().Nb()) || theCoEdge.IsRemoved(theGraph))
  {
    theEntry.SetSameParameter(true);
    return;
  }

  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(theCoEdge);
  const BRepGraph_EdgeId         anEdge  = aCoEdge.ChildEdgeId;

  if (!anEdge.IsValid(theGraph.Topo().Edges().Nb()) || anEdge.IsRemoved(theGraph))
  {
    theEntry.SetSameParameter(true);
    return;
  }

  const BRepGraphInc::EdgeDef&   aDef     = theGraph.Topo().Edges().Definition(anEdge);
  const occ::handle<Geom_Curve>& aCurve3D = edgeCurve3D(theGraph, anEdge);

  if (!aCurve3D.IsNull())
  {
    if (theEntry.SameRange())
    {
      const std::pair<double, double> aRange = BRepGraph_Tool::Edge::Range(theGraph, anEdge);
      theEntry.SetSameParameter(isCoEdgeSameParameter(theGraph, theCoEdge, aDef, aCurve3D, aRange));
    }
    else
    {
      theEntry.SetSameParameter(false);
    }
  }
  else
  {
    const BRepGraph_VertexId aStartV = resolveChildVertex(theGraph, aDef.StartVertexRefId);
    const BRepGraph_VertexId aEndV   = resolveChildVertex(theGraph, aDef.EndVertexRefId);
    theEntry.SetSameParameter(aStartV.IsValid() && aStartV == aEndV);
  }
}

//=================================================================================================

bool BRepGraph_CacheDerivedState::ensureEdgeEntry(BRepGraph_EdgeId theEdge, EdgeEntry& theEntry)
{
  // Lock-free read path: check packed flags without mutex.
  if (static_cast<size_t>(theEdge.Index) < myEdgeEntries.Size())
  {
    const EdgeEntry& aCached = myEdgeEntries.Value(static_cast<size_t>(theEdge.Index));
    if (aCached.IsFreshOwn(*this, theEdge) && aCached.IsComputed())
    {
      theEntry = aCached;
      return true;
    }
  }

  const BRepGraph& aGraph = Graph();
  if (!theEdge.IsValid(aGraph.Topo().Edges().Nb()) || theEdge.IsRemoved(aGraph))
  {
    return false;
  }

  EdgeEntry aEntry;
  computeStatusOnly(aGraph, theEdge, aEntry);

  {
    std::unique_lock aLock(myMutex);
    ensureSize(myEdgeEntries, theEdge.Index);
    EdgeEntry& aStored = myEdgeEntries.ChangeValue(static_cast<size_t>(theEdge.Index));
    if (aStored.IsFreshOwn(*this, theEdge) && aStored.IsComputed())
    {
      theEntry = aStored;
      return true;
    }
    aStored = aEntry;
    if (!aStored.BindOwnGen(*this, theEdge))
    {
      aStored.Reset();
      return false;
    }
    theEntry = aStored;
  }

  return true;
}

//=================================================================================================

bool BRepGraph_CacheDerivedState::ensureCoEdgeSameRangeEntry(BRepGraph_CoEdgeId    theCoEdge,
                                                             uint8_t               theRequiredFlags,
                                                             CoEdgeSameRangeEntry& theEntry)
{
  // Lock-free read path: check packed flags without mutex.
  if (static_cast<size_t>(theCoEdge.Index) < myCoEdgeSameRangeEntries.Size())
  {
    const CoEdgeSameRangeEntry& aCached =
      myCoEdgeSameRangeEntries.Value(static_cast<size_t>(theCoEdge.Index));
    if (aCached.IsFreshOwn(*this, theCoEdge))
    {
      const uint8_t aComputed = aCached.Computed();
      if ((aComputed & theRequiredFlags) == theRequiredFlags)
      {
        theEntry = aCached;
        return true;
      }
    }
  }

  const BRepGraph& aGraph = Graph();
  if (!theCoEdge.IsValid(aGraph.Topo().CoEdges().Nb()) || theCoEdge.IsRemoved(aGraph))
  {
    return false;
  }

  CoEdgeSameRangeEntry aEntry;
  if (static_cast<size_t>(theCoEdge.Index) < myCoEdgeSameRangeEntries.Size())
  {
    const CoEdgeSameRangeEntry& aCached =
      myCoEdgeSameRangeEntries.Value(static_cast<size_t>(theCoEdge.Index));
    if (aCached.IsFreshOwn(*this, theCoEdge))
    {
      aEntry = aCached;
    }
  }

  if ((aEntry.Computed() & CoEdgeSameRangeEntry::ComputedSameRange) == 0
      && (theRequiredFlags & CoEdgeSameRangeEntry::ComputedSameRange))
  {
    computeSameRange(aGraph, theCoEdge, aEntry);
  }
  if ((aEntry.Computed() & CoEdgeSameRangeEntry::ComputedSameParam) == 0
      && (theRequiredFlags & CoEdgeSameRangeEntry::ComputedSameParam))
  {
    computeSameParameter(aGraph, theCoEdge, aEntry);
  }

  {
    std::unique_lock aLock(myMutex);
    ensureSize(myCoEdgeSameRangeEntries, theCoEdge.Index);
    CoEdgeSameRangeEntry& aStored =
      myCoEdgeSameRangeEntries.ChangeValue(static_cast<size_t>(theCoEdge.Index));
    if (aStored.IsFreshOwn(*this, theCoEdge))
    {
      const uint8_t aMyPacked = aEntry.Packed.load(std::memory_order_relaxed);
      const uint8_t aMyComputed =
        aMyPacked
        & (CoEdgeSameRangeEntry::ComputedSameRange | CoEdgeSameRangeEntry::ComputedSameParam);
      const uint8_t aStoredPacked = aStored.Packed.load(std::memory_order_relaxed);
      const uint8_t aNewBits      = aMyComputed & ~aStoredPacked;
      if (aNewBits != 0)
      {
        // Merge value bits (shifted by 2 from computed) and computed bits into a single store.
        const uint8_t aMerged = (aStoredPacked & ~((aNewBits << 2) | aNewBits))
                                | (aMyPacked & (aNewBits << 2)) | aNewBits;
        aStored.Packed.store(aMerged, std::memory_order_release);
      }
    }
    else
    {
      aStored = aEntry;
    }
    if (!aStored.BindOwnGen(*this, theCoEdge))
    {
      aStored.Reset();
      return false;
    }
    theEntry = aStored;
  }

  return true;
}

//=================================================================================================

bool BRepGraph_CacheDerivedState::IsDegenerated(BRepGraph_EdgeId theEdge)
{
  EdgeEntry aEntry;
  if (!ensureEdgeEntry(theEdge, aEntry))
  {
    return false;
  }
  return aEntry.GetStatus() == EdgeEntry::GeomStatus::DegenerateOnSurface;
}

//=================================================================================================

bool BRepGraph_CacheDerivedState::SameParameter(BRepGraph_CoEdgeId theCoEdge)
{
  CoEdgeSameRangeEntry aEntry;
  if (!ensureCoEdgeSameRangeEntry(theCoEdge, CoEdgeSameRangeEntry::ComputedSameParam, aEntry))
  {
    return false;
  }
  return aEntry.SameParameter();
}

//=================================================================================================

bool BRepGraph_CacheDerivedState::SameRange(BRepGraph_CoEdgeId theCoEdge)
{
  CoEdgeSameRangeEntry aEntry;
  if (!ensureCoEdgeSameRangeEntry(theCoEdge, CoEdgeSameRangeEntry::ComputedSameRange, aEntry))
  {
    return false;
  }
  return aEntry.SameRange();
}

//=================================================================================================

bool BRepGraph_CacheDerivedState::IsClosed(BRepGraph_EdgeId theEdge)
{
  EdgeEntry aEntry;
  if (!ensureEdgeEntry(theEdge, aEntry))
  {
    return false;
  }
  return aEntry.IsClosed();
}

//=================================================================================================

bool BRepGraph_CacheDerivedState::ComputeEdgeProperties(const BRepGraph& theGraph,
                                                        BRepGraph_EdgeId theEdge,
                                                        bool&            theIsDegenerated,
                                                        bool&            theIsClosed)
{
  if (!theEdge.IsValid(theGraph.Topo().Edges().Nb()) || theEdge.IsRemoved(theGraph))
  {
    return false;
  }

  EdgeEntry aStatusEntry;
  computeStatusOnly(theGraph, theEdge, aStatusEntry);

  theIsDegenerated = aStatusEntry.GetStatus() == EdgeEntry::GeomStatus::DegenerateOnSurface;
  theIsClosed      = aStatusEntry.IsClosed();
  return true;
}

//=================================================================================================

bool BRepGraph_CacheDerivedState::ComputeWireIsClosed(const BRepGraph& theGraph,
                                                      BRepGraph_WireId theWire)
{
  if (!theWire.IsValid(theGraph.Topo().Wires().Nb()) || theWire.IsRemoved(theGraph))
  {
    return false;
  }

  const BRepGraphInc::WireRelations& aWR = theGraph.Topo().Wires().Relations(theWire);
  if (aWR.CoEdgeIds.IsEmpty())
  {
    return false;
  }

  BRepGraph_VertexId aFirstStart;
  BRepGraph_VertexId aPreviousEnd;
  bool               hasFirst = false;
  for (const BRepGraph_CoEdgeId& aCoEdgeId : aWR.CoEdgeIds)
  {
    BRepGraph_VertexId aStart;
    BRepGraph_VertexId anEnd;
    if (!coEdgeOrientedVertices(theGraph, aCoEdgeId, aStart, anEnd))
    {
      return false;
    }
    if (!hasFirst)
    {
      aFirstStart = aStart;
      hasFirst    = true;
    }
    else if (aStart != aPreviousEnd)
    {
      return false;
    }
    aPreviousEnd = anEnd;
  }

  return hasFirst && aPreviousEnd == aFirstStart;
}

//=================================================================================================

BRepGraph_CacheDerivedState::ShellEntry::ClosureStatus BRepGraph_CacheDerivedState::
  computeShellClosure(const BRepGraph& theGraph, BRepGraph_ShellId theShell)
{
  if (!theShell.IsValid(theGraph.Topo().Shells().Nb()) || theShell.IsRemoved(theGraph))
  {
    return ShellEntry::ClosureStatus::Invalid;
  }

  const BRepGraphInc::ShellRelations& aSR = theGraph.Topo().Shells().Relations(theShell);
  if (aSR.FaceRefIds.IsEmpty())
  {
    return ShellEntry::ClosureStatus::Empty;
  }

  NCollection_FlatMap<BRepGraph_FaceId> anActiveFaces;

  for (const BRepGraph_FaceRefId& aFaceRefId : aSR.FaceRefIds)
  {
    if (!aFaceRefId.IsValid(theGraph.Refs().Faces().Nb()) || aFaceRefId.IsRemoved(theGraph))
    {
      continue;
    }

    const BRepGraphInc::FaceRef& aFaceRef = theGraph.Refs().Faces().Entry(aFaceRefId);
    const BRepGraph_FaceId       aFaceId  = aFaceRef.ChildFaceId;
    if (aFaceId.IsValid(theGraph.Topo().Faces().Nb()) && !aFaceId.IsRemoved(theGraph))
    {
      anActiveFaces.Add(aFaceId);
    }
  }

  if (anActiveFaces.IsEmpty())
  {
    return ShellEntry::ClosureStatus::Empty;
  }

  NCollection_DataMap<BRepGraph_EdgeId, uint32_t> anEdgeUsage;

  for (NCollection_FlatMap<BRepGraph_FaceId>::Iterator aFaceIter(anActiveFaces); aFaceIter.More();
       aFaceIter.Next())
  {
    const BRepGraph_FaceId             aFaceId = aFaceIter.Value();
    const BRepGraphInc::FaceRelations& aFR     = theGraph.Topo().Faces().Relations(aFaceId);
    for (const BRepGraph_WireRefId& aWireRefId : aFR.WireRefIds)
    {
      if (!aWireRefId.IsValid(theGraph.Refs().Wires().Nb()) || aWireRefId.IsRemoved(theGraph))
      {
        continue;
      }

      const BRepGraphInc::WireRef& aWireRef = theGraph.Refs().Wires().Entry(aWireRefId);
      const BRepGraph_WireId       aWireId  = aWireRef.ChildWireId;
      if (!aWireId.IsValid(theGraph.Topo().Wires().Nb()) || aWireId.IsRemoved(theGraph))
      {
        continue;
      }

      const BRepGraphInc::WireRelations& aWR = theGraph.Topo().Wires().Relations(aWireId);
      for (const BRepGraph_CoEdgeId& aCoEdgeId : aWR.CoEdgeIds)
      {
        if (!aCoEdgeId.IsValid(theGraph.Topo().CoEdges().Nb()) || aCoEdgeId.IsRemoved(theGraph))
        {
          continue;
        }

        const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(aCoEdgeId);
        if (aCoEdge.FaceId != aFaceId)
        {
          continue;
        }

        const BRepGraph_EdgeId anEdgeId = aCoEdge.ChildEdgeId;
        if (!anEdgeId.IsValid(theGraph.Topo().Edges().Nb()) || anEdgeId.IsRemoved(theGraph))
        {
          continue;
        }

        EdgeEntry aEdgeEntry;
        computeStatusOnly(theGraph, anEdgeId, aEdgeEntry);
        if (aEdgeEntry.GetStatus() == EdgeEntry::GeomStatus::DegenerateOnSurface)
        {
          continue;
        }

        uint32_t* aCount = anEdgeUsage.ChangeSeek(anEdgeId);
        if (aCount != nullptr)
        {
          ++(*aCount);
        }
        else
        {
          anEdgeUsage.Bind(anEdgeId, 1u);
        }
      }
    }
  }

  if (anEdgeUsage.IsEmpty())
  {
    return ShellEntry::ClosureStatus::Closed;
  }

  bool hasOpen        = false;
  bool hasNonManifold = false;

  for (NCollection_DataMap<BRepGraph_EdgeId, uint32_t>::Iterator anIter(anEdgeUsage); anIter.More();
       anIter.Next())
  {
    const uint32_t aCount = anIter.Value();
    if (aCount == 1u)
    {
      hasOpen = true;
    }
    else if (aCount > 2u)
    {
      hasNonManifold = true;
    }
  }

  if (hasNonManifold)
  {
    return ShellEntry::ClosureStatus::NonManifold;
  }
  if (hasOpen)
  {
    return ShellEntry::ClosureStatus::Open;
  }
  return ShellEntry::ClosureStatus::Closed;
}

//=================================================================================================

bool BRepGraph_CacheDerivedState::GetWireIsClosed(BRepGraph_WireId theWire, bool& theClosed)
{
  // Lock-free read path: check packed flags without mutex.
  if (static_cast<size_t>(theWire.Index) < myWireEntries.Size())
  {
    const WireEntry& aCached = myWireEntries.Value(static_cast<size_t>(theWire.Index));
    if (aCached.IsComputed() && aCached.IsFreshOwn(*this, theWire))
    {
      theClosed = aCached.IsClosed();
      return true;
    }
  }

  const bool aClosed = ComputeWireIsClosed(Graph(), theWire);

  std::lock_guard aLock(myMutex);
  ensureSize(myWireEntries, theWire.Index);
  WireEntry& aStored = myWireEntries.ChangeValue(static_cast<size_t>(theWire.Index));
  if (!aStored.BindOwnGen(*this, theWire))
  {
    aStored.Reset();
    return false;
  }
  aStored.SetClosed(aClosed);
  theClosed = aClosed;
  return true;
}

//=================================================================================================

bool BRepGraph_CacheDerivedState::IsShellClosed(BRepGraph_ShellId theShell)
{
  // Lock-free read path: check atomic Status without mutex.
  if (static_cast<size_t>(theShell.Index) < myShellEntries.Size())
  {
    const ShellEntry& aCached = myShellEntries.Value(static_cast<size_t>(theShell.Index));
    const auto        aStatus = aCached.Status.load(std::memory_order_acquire);
    if (aStatus != ShellEntry::ClosureStatus::Invalid && aCached.IsFreshOwn(*this, theShell))
    {
      return aStatus == ShellEntry::ClosureStatus::Closed;
    }
  }

  const ShellEntry::ClosureStatus aStatus = computeShellClosure(Graph(), theShell);

  std::lock_guard aLock(myMutex);
  ensureSize(myShellEntries, theShell.Index);
  ShellEntry& aStored = myShellEntries.ChangeValue(static_cast<size_t>(theShell.Index));
  if (aStored.Status.load(std::memory_order_acquire) != ShellEntry::ClosureStatus::Invalid
      && aStored.IsFreshOwn(*this, theShell))
  {
    return aStored.Status.load(std::memory_order_acquire) == ShellEntry::ClosureStatus::Closed;
  }
  if (!aStored.BindOwnGen(*this, theShell))
  {
    aStored.Reset();
    return false;
  }
  aStored.Status.store(aStatus, std::memory_order_release);
  return aStatus == ShellEntry::ClosureStatus::Closed;
}

//=================================================================================================

bool BRepGraph_CacheDerivedState::ComputeShellIsClosed(const BRepGraph&  theGraph,
                                                       BRepGraph_ShellId theShell)
{
  return computeShellClosure(theGraph, theShell) == ShellEntry::ClosureStatus::Closed;
}

//=================================================================================================

void BRepGraph_CacheDerivedState::SetWireIsClosed(BRepGraph_WireId theWire, bool theClosed)
{
  std::lock_guard aLock(myMutex);
  ensureSize(myWireEntries, theWire.Index);
  WireEntry& aStored = myWireEntries.ChangeValue(static_cast<size_t>(theWire.Index));
  if (!aStored.BindOwnGen(*this, theWire))
  {
    aStored.Reset();
    return;
  }
  aStored.SetClosed(theClosed);
}
