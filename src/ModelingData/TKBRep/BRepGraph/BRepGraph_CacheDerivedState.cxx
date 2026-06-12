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
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_FlatMap.hxx>
#include <NCollection_Map.hxx>
#include <cmath>

IMPLEMENT_STANDARD_RTTIEXT(BRepGraph_CacheDerivedState, BRepGraph_Cache)

namespace
{

const Standard_GUID& theGUID()
{
  static const Standard_GUID aGUID("a1c2e3f4-5678-4abc-9def-0123456789ab");
  return aGUID;
}

template <typename IdT>
IdT remappedNode(const BRepGraph_CopyRemap& theCopy, const IdT theId)
{
  if (!theId.IsValid())
  {
    return IdT();
  }
  const BRepGraph_ItemId* aTarget = theCopy.TargetItem(BRepGraph_ItemId(theId));
  if (aTarget == nullptr || !aTarget->IsNode())
  {
    return IdT();
  }
  return IdT::FromNodeId(aTarget->NodeId());
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

bool isEdgeSameRange(const BRepGraph&                 theGraph,
                     const BRepGraph_EdgeId           theEdge,
                     const std::pair<double, double>& theEdgeRange)
{
  const BRepGraphInc::EdgeRelations& anEdgeRel = theGraph.Topo().Edges().Relations(theEdge);
  for (const BRepGraph_CoEdgeId& aCoEdgeId : anEdgeRel.CoEdgeIds)
  {
    if (!aCoEdgeId.IsValid(theGraph.Topo().CoEdges().Nb()) || aCoEdgeId.IsRemoved(theGraph))
    {
      continue;
    }

    const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(aCoEdgeId);
    if (!aCoEdge.FaceId.IsValid(theGraph.Topo().Faces().Nb()) || aCoEdge.FaceId.IsRemoved(theGraph))
    {
      continue;
    }

    if (coEdgePCurve(theGraph, aCoEdgeId).IsNull())
    {
      continue;
    }

    const std::pair<double, double> aPCurveRange =
      BRepGraph_Tool::CoEdge::Range(theGraph, aCoEdgeId);
    if (std::abs(aPCurveRange.first - theEdgeRange.first) > Precision::PConfusion()
        || std::abs(aPCurveRange.second - theEdgeRange.second) > Precision::PConfusion())
    {
      return false;
    }
  }
  return true;
}

//=================================================================================================

bool isEdgeSameParameter(const BRepGraph&                 theGraph,
                         const BRepGraph_EdgeId           theEdge,
                         const BRepGraphInc::EdgeDef&     theEdgeDef,
                         const occ::handle<Geom_Curve>&   theCurve3D,
                         const std::pair<double, double>& theEdgeRange)
{
  const double  aTol           = theEdgeDef.Tolerance + Precision::Confusion();
  constexpr int THE_NB_SAMPLES = 5;

  const BRepGraphInc::EdgeRelations& anEdgeRel = theGraph.Topo().Edges().Relations(theEdge);
  for (int anIdx = 0; anIdx <= THE_NB_SAMPLES; ++anIdx)
  {
    const double aParam =
      theEdgeRange.first + (theEdgeRange.second - theEdgeRange.first) * anIdx / THE_NB_SAMPLES;
    const gp_Pnt aPoint3D = theCurve3D->Value(aParam);

    for (const BRepGraph_CoEdgeId& aCoEdgeId : anEdgeRel.CoEdgeIds)
    {
      if (!aCoEdgeId.IsValid(theGraph.Topo().CoEdges().Nb()) || aCoEdgeId.IsRemoved(theGraph))
      {
        continue;
      }

      const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(aCoEdgeId);
      if (!aCoEdge.FaceId.IsValid(theGraph.Topo().Faces().Nb())
          || aCoEdge.FaceId.IsRemoved(theGraph))
      {
        continue;
      }

      const occ::handle<Geom2d_Curve>& aPCurve = coEdgePCurve(theGraph, aCoEdgeId);
      if (aPCurve.IsNull())
      {
        continue;
      }

      const occ::handle<Geom_Surface> aSurface = faceSurface(theGraph, aCoEdge.FaceId);
      if (aSurface.IsNull())
      {
        continue;
      }

      const gp_Pnt2d aUV           = aPCurve->Value(aParam);
      const gp_Pnt   aSurfacePoint = aSurface->Value(aUV.X(), aUV.Y());
      if (aPoint3D.Distance(aSurfacePoint) > aTol)
      {
        return false;
      }
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
  std::unique_lock aLock(myMutex);
  myEdgeEntries.Clear();
  myWireEntries.Clear();
  myShellEntries.Clear();
}

//=================================================================================================

void BRepGraph_CacheDerivedState::CopyFreshTo(const BRepGraph_CopyRemap& theCopy) const
{
  occ::handle<BRepGraph_CacheDerivedState> aTargetCache =
    theCopy.TargetGraph().CacheRegistry().Ensure<BRepGraph_CacheDerivedState>();

  std::shared_lock aSourceLock(myMutex);
  std::unique_lock aTargetLock(aTargetCache->myMutex);

  for (NCollection_DataMap<BRepGraph_EdgeId, EdgeEntry>::Iterator anIt(myEdgeEntries); anIt.More();
       anIt.Next())
  {
    const BRepGraph_EdgeId aSourceEdge  = anIt.Key();
    const EdgeEntry&       aSourceEntry = anIt.Value();
    if (!aSourceEntry.IsFreshOwn(*this, BRepGraph_NodeId(aSourceEdge)))
    {
      continue;
    }
    const BRepGraph_EdgeId aTargetEdge = remappedNode(theCopy, aSourceEdge);
    if (!aTargetEdge.IsValidIn(theCopy.TargetGraph().Topo().Edges()))
    {
      continue;
    }
    EdgeEntry aTargetEntry = aSourceEntry;
    if (aTargetEntry.BindOwnGen(*aTargetCache, BRepGraph_NodeId(aTargetEdge)))
    {
      aTargetCache->myEdgeEntries.Bind(aTargetEdge, aTargetEntry);
    }
  }

  for (NCollection_DataMap<BRepGraph_WireId, WireEntry>::Iterator anIt(myWireEntries); anIt.More();
       anIt.Next())
  {
    const BRepGraph_WireId aSourceWire  = anIt.Key();
    const WireEntry&       aSourceEntry = anIt.Value();
    if (!aSourceEntry.IsFreshOwn(*this, BRepGraph_NodeId(aSourceWire)))
    {
      continue;
    }
    const BRepGraph_WireId aTargetWire = remappedNode(theCopy, aSourceWire);
    if (!aTargetWire.IsValidIn(theCopy.TargetGraph().Topo().Wires()))
    {
      continue;
    }
    WireEntry aTargetEntry = aSourceEntry;
    if (aTargetEntry.BindOwnGen(*aTargetCache, BRepGraph_NodeId(aTargetWire)))
    {
      aTargetCache->myWireEntries.Bind(aTargetWire, aTargetEntry);
    }
  }

  for (NCollection_DataMap<BRepGraph_ShellId, ShellEntry>::Iterator anIt(myShellEntries);
       anIt.More();
       anIt.Next())
  {
    const BRepGraph_ShellId aSourceShell = anIt.Key();
    const ShellEntry&       aSourceEntry = anIt.Value();
    if (!aSourceEntry.IsFreshOwn(*this, BRepGraph_NodeId(aSourceShell)))
    {
      continue;
    }
    const BRepGraph_ShellId aTargetShell = remappedNode(theCopy, aSourceShell);
    if (!aTargetShell.IsValidIn(theCopy.TargetGraph().Topo().Shells()))
    {
      continue;
    }
    ShellEntry aTargetEntry = aSourceEntry;
    if (aTargetEntry.BindOwnGen(*aTargetCache, BRepGraph_NodeId(aTargetShell)))
    {
      aTargetCache->myShellEntries.Bind(aTargetShell, aTargetEntry);
    }
  }
}

//=================================================================================================

bool BRepGraph_CacheDerivedState::ComputeEdgeStatus(const BRepGraph& theGraph,
                                                    BRepGraph_EdgeId theEdge,
                                                    EdgeEntry&       theEntry)
{
  theEntry.Status        = EdgeGeometryStatus::Invalid;
  theEntry.IsClosed      = false;
  theEntry.SameRange     = false;
  theEntry.SameParameter = false;

  if (!theEdge.IsValid(theGraph.Topo().Edges().Nb()) || theEdge.IsRemoved(theGraph))
  {
    return false;
  }

  const BRepGraphInc::EdgeDef& aDef = theGraph.Topo().Edges().Definition(theEdge);

  const BRepGraph_VertexId aStartV = resolveChildVertex(theGraph, aDef.StartVertexRefId);
  const BRepGraph_VertexId aEndV   = resolveChildVertex(theGraph, aDef.EndVertexRefId);
  theEntry.IsClosed                = aStartV.IsValid() && aStartV == aEndV;

  const occ::handle<Geom_Curve>& aCurve3D   = edgeCurve3D(theGraph, theEdge);
  const bool                     hasCurve3D = !aCurve3D.IsNull();
  theEntry.Status =
    hasCurve3D ? EdgeGeometryStatus::HasCurve3D : EdgeGeometryStatus::MissingCurve3D;

  if (hasCurve3D)
  {
    const std::pair<double, double> anEdgeRange = BRepGraph_Tool::Edge::Range(theGraph, theEdge);
    theEntry.SameRange                          = isEdgeSameRange(theGraph, theEdge, anEdgeRange);
    theEntry.SameParameter =
      theEntry.SameRange && isEdgeSameParameter(theGraph, theEdge, aDef, aCurve3D, anEdgeRange);
    return true;
  }

  if (theEntry.IsClosed)
  {
    theEntry.Status        = EdgeGeometryStatus::DegenerateOnSurface;
    theEntry.SameRange     = true;
    theEntry.SameParameter = true;
    return true;
  }

  if (aStartV.IsValid() && aEndV.IsValid())
  {
    const BRepGraphInc::VertexDef& aStartDef = theGraph.Topo().Vertices().Definition(aStartV);
    const BRepGraphInc::VertexDef& aEndDef   = theGraph.Topo().Vertices().Definition(aEndV);

    const double aDist = aStartDef.Point.Distance(aEndDef.Point);
    if (aDist <= aDef.Tolerance)
    {
      theEntry.Status        = EdgeGeometryStatus::DegenerateOnSurface;
      theEntry.SameRange     = true;
      theEntry.SameParameter = true;
      return true;
    }
  }
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

bool BRepGraph_CacheDerivedState::ComputeShellStatus(const BRepGraph&  theGraph,
                                                     BRepGraph_ShellId theShell,
                                                     ShellEntry&       theEntry)
{
  theEntry.Status = ShellClosureStatus::Invalid;

  if (!theShell.IsValid(theGraph.Topo().Shells().Nb()) || theShell.IsRemoved(theGraph))
  {
    return false;
  }

  const BRepGraphInc::ShellRelations& aSR = theGraph.Topo().Shells().Relations(theShell);
  if (aSR.FaceRefIds.IsEmpty())
  {
    theEntry.Status = ShellClosureStatus::Empty;
    return true;
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
    theEntry.Status = ShellClosureStatus::Empty;
    return true;
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

        EdgeEntry anEdgeEntry;
        if (!ComputeEdgeStatus(theGraph, anEdgeId, anEdgeEntry)
            || anEdgeEntry.Status == EdgeGeometryStatus::DegenerateOnSurface)
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
    theEntry.Status = ShellClosureStatus::Closed;
    return true;
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
    theEntry.Status = ShellClosureStatus::NonManifold;
  }
  else if (hasOpen)
  {
    theEntry.Status = ShellClosureStatus::Open;
  }
  else
  {
    theEntry.Status = ShellClosureStatus::Closed;
  }

  return true;
}

//=================================================================================================

bool BRepGraph_CacheDerivedState::GetEdgeStatus(BRepGraph_EdgeId theEdge, EdgeEntry& theEntry)
{
  EdgeEntry aComputed;
  if (!ComputeEdgeStatus(Graph(), theEdge, aComputed))
  {
    return false;
  }

  std::unique_lock aLock(myMutex);
  EdgeEntry*       aStored = myEdgeEntries.ChangeSeek(theEdge);
  if (aStored != nullptr)
  {
    *aStored = aComputed;
  }
  else
  {
    myEdgeEntries.Bind(theEdge, aComputed);
    aStored = myEdgeEntries.ChangeSeek(theEdge);
  }
  if (!aStored->BindOwnGen(*this, theEdge))
  {
    myEdgeEntries.UnBind(theEdge);
    return false;
  }
  theEntry = *aStored;
  return true;
}

//=================================================================================================

bool BRepGraph_CacheDerivedState::GetWireIsClosed(BRepGraph_WireId theWire, bool& theClosed)
{
  const bool aClosed = ComputeWireIsClosed(Graph(), theWire);

  std::unique_lock aLock(myMutex);
  WireEntry*       aStored = myWireEntries.ChangeSeek(theWire);
  if (aStored == nullptr)
  {
    myWireEntries.Bind(theWire, WireEntry());
    aStored = myWireEntries.ChangeSeek(theWire);
  }
  aStored->IsClosed = aClosed;
  if (!aStored->BindOwnGen(*this, theWire))
  {
    myWireEntries.UnBind(theWire);
    return false;
  }
  theClosed = aClosed;
  return true;
}

//=================================================================================================

bool BRepGraph_CacheDerivedState::GetShellStatus(BRepGraph_ShellId theShell, ShellEntry& theEntry)
{
  ShellEntry aComputed;
  if (!ComputeShellStatus(Graph(), theShell, aComputed))
  {
    return false;
  }

  std::unique_lock aLock(myMutex);
  ShellEntry*      aStored = myShellEntries.ChangeSeek(theShell);
  if (aStored != nullptr)
  {
    *aStored = aComputed;
  }
  else
  {
    myShellEntries.Bind(theShell, aComputed);
    aStored = myShellEntries.ChangeSeek(theShell);
  }
  if (!aStored->BindOwnGen(*this, theShell))
  {
    myShellEntries.UnBind(theShell);
    return false;
  }
  theEntry = *aStored;
  return true;
}

//=================================================================================================

void BRepGraph_CacheDerivedState::SetEdgeStatus(BRepGraph_EdgeId theEdge, const EdgeEntry& theEntry)
{
  std::unique_lock aLock(myMutex);
  EdgeEntry*       aStored = myEdgeEntries.ChangeSeek(theEdge);
  if (aStored != nullptr)
  {
    *aStored = theEntry;
  }
  else
  {
    myEdgeEntries.Bind(theEdge, theEntry);
    aStored = myEdgeEntries.ChangeSeek(theEdge);
  }
  [[maybe_unused]] const bool isBound = aStored->BindOwnGen(*this, theEdge);
}

//=================================================================================================

void BRepGraph_CacheDerivedState::SetWireIsClosed(BRepGraph_WireId theWire, bool theClosed)
{
  std::unique_lock aLock(myMutex);
  WireEntry*       aStored = myWireEntries.ChangeSeek(theWire);
  if (aStored == nullptr)
  {
    myWireEntries.Bind(theWire, WireEntry());
    aStored = myWireEntries.ChangeSeek(theWire);
  }
  aStored->IsClosed                   = theClosed;
  [[maybe_unused]] const bool isBound = aStored->BindOwnGen(*this, theWire);
}

//=================================================================================================

void BRepGraph_CacheDerivedState::SetShellStatus(BRepGraph_ShellId theShell,
                                                 const ShellEntry& theEntry)
{
  std::unique_lock aLock(myMutex);
  ShellEntry*      aStored = myShellEntries.ChangeSeek(theShell);
  if (aStored != nullptr)
  {
    *aStored = theEntry;
  }
  else
  {
    myShellEntries.Bind(theShell, theEntry);
    aStored = myShellEntries.ChangeSeek(theShell);
  }
  [[maybe_unused]] const bool isBound = aStored->BindOwnGen(*this, theShell);
}
