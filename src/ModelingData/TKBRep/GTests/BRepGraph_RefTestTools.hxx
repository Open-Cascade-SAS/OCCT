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

#ifndef _BRepGraph_RefTestTools_HeaderFile
#define _BRepGraph_RefTestTools_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_RepId.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraphInc_Storage.hxx>

#include <cstdint>

#include <NCollection_LinearVector.hxx>

namespace BRepGraph_TestTools
{

//=================================================================================================

//! Connectivity-derived seam predicate: storage-only equivalent of
//! BRepGraph_Tool::CoEdge::SeamPair. Returns the partner CoEdge id if theCoEdgeId
//! is one half of a seam pair on its face, else an invalid id.
inline BRepGraph_CoEdgeId SeamPairFromStorage(const BRepGraphInc_Storage& theStorage,
                                              const BRepGraph_CoEdgeId    theCoEdgeId)
{
  const BRepGraphInc::CoEdgeDef& aDef = theStorage.CoEdge(theCoEdgeId);
  if (theStorage.IsRemoved(theCoEdgeId) || !aDef.ChildEdgeId.IsValid() || !aDef.FaceId.IsValid())
  {
    return BRepGraph_CoEdgeId();
  }
  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aSiblings =
    theStorage.EdgeRelations(aDef.ChildEdgeId).CoEdgeIds;
  for (size_t i = 0; i < aSiblings.Size(); ++i)
  {
    const BRepGraph_CoEdgeId aOther = aSiblings.Value(i);
    if (aOther == theCoEdgeId)
    {
      continue;
    }
    const BRepGraphInc::CoEdgeDef& aOtherDef = theStorage.CoEdge(aOther);
    if (theStorage.IsRemoved(aOther))
    {
      continue;
    }
    if (aOtherDef.FaceId == aDef.FaceId && aOtherDef.Orientation != aDef.Orientation)
    {
      return aOther;
    }
  }
  return BRepGraph_CoEdgeId();
}

inline bool IsSeamCoEdgeFromStorage(const BRepGraphInc_Storage& theStorage,
                                    const BRepGraph_CoEdgeId    theCoEdgeId)
{
  return SeamPairFromStorage(theStorage, theCoEdgeId).IsValid();
}

template <typename RefIdT>
inline const NCollection_LinearVector<RefIdT>& EmptyIds()
{
  static const NCollection_LinearVector<RefIdT> THE_EMPTY;
  return THE_EMPTY;
}

//=================================================================================================
inline const NCollection_LinearVector<BRepGraph_CoEdgeId>& CoEdgesOfWire(
  const BRepGraph&       theGraph,
  const BRepGraph_WireId theWireId)
{
  return theGraph.Topo().Wires().Relations(theWireId).CoEdgeIds;
}

//=================================================================================================

inline uint32_t CountCoEdgesOfWire(const BRepGraph& theGraph, const BRepGraph_WireId theWireId)
{
  return static_cast<uint32_t>(CoEdgesOfWire(theGraph, theWireId).Size());
}

//=================================================================================================

inline const NCollection_LinearVector<BRepGraph_WireRefId>& WireRefsOfFace(
  const BRepGraph&       theGraph,
  const BRepGraph_FaceId theFaceId)
{
  return theGraph.Refs().Wires().IdsOf(theFaceId);
}

//=================================================================================================

inline uint32_t CountWireRefsOfFace(const BRepGraph& theGraph, const BRepGraph_FaceId theFaceId)
{
  return static_cast<uint32_t>(WireRefsOfFace(theGraph, theFaceId).Size());
}

//=================================================================================================

inline bool FaceUsesWire(const BRepGraph&       theGraph,
                         const BRepGraph_FaceId theFaceId,
                         const BRepGraph_WireId theWireId)
{
  const BRepGraph::RefsView&                           aRefs = theGraph.Refs();
  const NCollection_LinearVector<BRepGraph_WireRefId>& aWireRefs =
    WireRefsOfFace(theGraph, theFaceId);
  for (const BRepGraph_WireRefId& aWireRefId : aWireRefs)
  {
    if (aRefs.Wires().Entry(aWireRefId).ChildWireId == theWireId)
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

inline const NCollection_LinearVector<BRepGraph_FaceRefId>& FaceRefsOfShell(
  const BRepGraph&        theGraph,
  const BRepGraph_ShellId theShellId)
{
  return theGraph.Refs().Faces().IdsOf(theShellId);
}

//=================================================================================================

inline uint32_t CountFaceRefsOfShell(const BRepGraph& theGraph, const BRepGraph_ShellId theShellId)
{
  return static_cast<uint32_t>(FaceRefsOfShell(theGraph, theShellId).Size());
}

//=================================================================================================

inline const NCollection_LinearVector<BRepGraph_ShellRefId>& ShellRefsOfSolid(
  const BRepGraph&        theGraph,
  const BRepGraph_SolidId theSolidId)
{
  return theGraph.Refs().Shells().IdsOf(theSolidId);
}

//=================================================================================================

inline uint32_t CountShellRefsOfSolid(const BRepGraph& theGraph, const BRepGraph_SolidId theSolidId)
{
  return static_cast<uint32_t>(ShellRefsOfSolid(theGraph, theSolidId).Size());
}

//=================================================================================================

inline const NCollection_LinearVector<BRepGraph_SolidRefId>& SolidRefsOfCompSolid(
  const BRepGraph&            theGraph,
  const BRepGraph_CompSolidId theCompSolidId)
{
  return theGraph.Refs().Solids().IdsOf(theCompSolidId);
}

//=================================================================================================

inline uint32_t CountSolidRefsOfCompSolid(const BRepGraph&            theGraph,
                                          const BRepGraph_CompSolidId theCompSolidId)
{
  return static_cast<uint32_t>(SolidRefsOfCompSolid(theGraph, theCompSolidId).Size());
}

//=================================================================================================

inline const NCollection_LinearVector<BRepGraph_ChildRefId>& ChildRefsOfParent(
  const BRepGraph&       theGraph,
  const BRepGraph_NodeId theParentId)
{
  if (theParentId.NodeKind != BRepGraph_NodeId::Kind::Compound)
  {
    return EmptyIds<BRepGraph_ChildRefId>();
  }
  return theGraph.Refs().Children().IdsOf(BRepGraph_CompoundId(theParentId));
}

//=================================================================================================

inline uint32_t CountChildRefsOfParent(const BRepGraph&       theGraph,
                                       const BRepGraph_NodeId theParentId)
{
  return static_cast<uint32_t>(ChildRefsOfParent(theGraph, theParentId).Size());
}

//=================================================================================================

inline BRepGraph_WireId OuterWireOfFace(const BRepGraph& theGraph, const BRepGraph_FaceId theFaceId)
{
  return BRepGraph_Tool::Face::OuterWire(theGraph, theFaceId);
}

//=================================================================================================

inline const NCollection_LinearVector<BRepGraph_CoEdgeId>& CoEdgesOfWire(
  const BRepGraphInc_Storage& theStorage,
  const BRepGraph_WireId      theWireId)
{
  return theStorage.WireRelations(theWireId).CoEdgeIds;
}

//=================================================================================================

inline uint32_t CountCoEdgesOfWire(const BRepGraphInc_Storage& theStorage,
                                   const BRepGraph_WireId      theWireId)
{
  return static_cast<uint32_t>(CoEdgesOfWire(theStorage, theWireId).Size());
}

//=================================================================================================

inline const NCollection_LinearVector<BRepGraph_WireRefId>& WireRefsOfFace(
  const BRepGraphInc_Storage& theStorage,
  const BRepGraph_FaceId      theFaceId)
{
  return theStorage.FaceRelations(theFaceId).WireRefIds;
}

//=================================================================================================

inline uint32_t CountWireRefsOfFace(const BRepGraphInc_Storage& theStorage,
                                    const BRepGraph_FaceId      theFaceId)
{
  return static_cast<uint32_t>(WireRefsOfFace(theStorage, theFaceId).Size());
}

//=================================================================================================

inline bool FaceUsesWire(const BRepGraphInc_Storage& theStorage,
                         const BRepGraph_FaceId      theFaceId,
                         const BRepGraph_WireId      theWireId)
{
  const NCollection_LinearVector<BRepGraph_WireRefId>& aWireRefs =
    WireRefsOfFace(theStorage, theFaceId);
  for (const BRepGraph_WireRefId& aWireRefId : aWireRefs)
  {
    const BRepGraphInc::WireRef& aWireRef = theStorage.WireRef(aWireRefId);
    if (aWireRef.ChildWireId == theWireId)
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

inline const NCollection_LinearVector<BRepGraph_FaceRefId>& FaceRefsOfShell(
  const BRepGraphInc_Storage& theStorage,
  const BRepGraph_ShellId     theShellId)
{
  return theStorage.ShellRelations(theShellId).FaceRefIds;
}

//=================================================================================================

inline uint32_t CountFaceRefsOfShell(const BRepGraphInc_Storage& theStorage,
                                     const BRepGraph_ShellId     theShellId)
{
  return static_cast<uint32_t>(FaceRefsOfShell(theStorage, theShellId).Size());
}

//=================================================================================================

inline const NCollection_LinearVector<BRepGraph_ShellRefId>& ShellRefsOfSolid(
  const BRepGraphInc_Storage& theStorage,
  const BRepGraph_SolidId     theSolidId)
{
  return theStorage.SolidRelations(theSolidId).ShellRefIds;
}

//=================================================================================================

inline uint32_t CountShellRefsOfSolid(const BRepGraphInc_Storage& theStorage,
                                      const BRepGraph_SolidId     theSolidId)
{
  return static_cast<uint32_t>(ShellRefsOfSolid(theStorage, theSolidId).Size());
}

//=================================================================================================

inline const NCollection_LinearVector<BRepGraph_SolidRefId>& SolidRefsOfCompSolid(
  const BRepGraphInc_Storage& theStorage,
  const BRepGraph_CompSolidId theCompSolidId)
{
  return theStorage.CompSolidRelations(theCompSolidId).SolidRefIds;
}

//=================================================================================================

inline uint32_t CountSolidRefsOfCompSolid(const BRepGraphInc_Storage& theStorage,
                                          const BRepGraph_CompSolidId theCompSolidId)
{
  return static_cast<uint32_t>(SolidRefsOfCompSolid(theStorage, theCompSolidId).Size());
}

//=================================================================================================

inline const NCollection_LinearVector<BRepGraph_ChildRefId>& ChildRefsOfParent(
  const BRepGraphInc_Storage& theStorage,
  const BRepGraph_NodeId      theParentId)
{
  if (theParentId.NodeKind != BRepGraph_NodeId::Kind::Compound)
  {
    return EmptyIds<BRepGraph_ChildRefId>();
  }
  return theStorage.CompoundRelations(BRepGraph_CompoundId(theParentId)).ChildRefIds;
}

//=================================================================================================

inline uint32_t CountChildRefsOfParent(const BRepGraphInc_Storage& theStorage,
                                       const BRepGraph_NodeId      theParentId)
{
  return static_cast<uint32_t>(ChildRefsOfParent(theStorage, theParentId).Size());
}

//=================================================================================================

inline BRepGraph_WireId OuterWireOfFace(const BRepGraphInc_Storage& theStorage,
                                        const BRepGraph_FaceId      theFaceId)
{
  const NCollection_LinearVector<BRepGraph_WireRefId>& aWireRefs =
    WireRefsOfFace(theStorage, theFaceId);
  for (const BRepGraph_WireRefId& aWireRefId : aWireRefs)
  {
    const BRepGraphInc::WireRef& aWireRef = theStorage.WireRef(aWireRefId);
    return aWireRef.ChildWireId;
  }
  return BRepGraph_WireId();
}

} // namespace BRepGraph_TestTools

#endif // _BRepGraph_RefTestTools_HeaderFile
