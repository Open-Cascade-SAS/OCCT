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
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraphInc_Storage.hxx>

#include <NCollection_Vector.hxx>

namespace BRepGraph_TestTools
{

//=================================================================================================
inline NCollection_Vector<BRepGraph_CoEdgeRefId> CoEdgeRefsOfWire(const BRepGraph&       theGraph,
                                                                  const BRepGraph_WireId theWireId)
{
  NCollection_Vector<BRepGraph_CoEdgeRefId> aRefIds;
  const BRepGraph::RefsView&                aRefs       = theGraph.Refs();
  const BRepGraph_NodeId                    aParentNode = BRepGraph_NodeId::Wire(theWireId.Index);
  for (int aRefIdx = 0; aRefIdx < aRefs.NbCoEdgeRefs(); ++aRefIdx)
  {
    const BRepGraph_CoEdgeRefId         aRefId(aRefIdx);
    const BRepGraphInc::CoEdgeRefEntry& aRef = aRefs.CoEdge(aRefId);
    if (aRef.ParentId == aParentNode && !aRef.IsRemoved)
      aRefIds.Append(aRefId);
  }
  return aRefIds;
}

//=================================================================================================

inline int CountCoEdgeRefsOfWire(const BRepGraph& theGraph, const BRepGraph_WireId theWireId)
{
  return CoEdgeRefsOfWire(theGraph, theWireId).Length();
}

//=================================================================================================

inline NCollection_Vector<BRepGraph_WireRefId> WireRefsOfFace(const BRepGraph&       theGraph,
                                                              const BRepGraph_FaceId theFaceId)
{
  NCollection_Vector<BRepGraph_WireRefId> aRefIds;
  const BRepGraph::RefsView&              aRefs       = theGraph.Refs();
  const BRepGraph_NodeId                  aParentNode = BRepGraph_NodeId::Face(theFaceId.Index);
  for (int aRefIdx = 0; aRefIdx < aRefs.NbWireRefs(); ++aRefIdx)
  {
    const BRepGraph_WireRefId         aRefId(aRefIdx);
    const BRepGraphInc::WireRefEntry& aRef = aRefs.Wire(aRefId);
    if (aRef.ParentId == aParentNode && !aRef.IsRemoved)
      aRefIds.Append(aRefId);
  }
  return aRefIds;
}

//=================================================================================================

inline int CountWireRefsOfFace(const BRepGraph& theGraph, const BRepGraph_FaceId theFaceId)
{
  return WireRefsOfFace(theGraph, theFaceId).Length();
}

//=================================================================================================

inline bool FaceUsesWire(const BRepGraph&       theGraph,
                         const BRepGraph_FaceId theFaceId,
                         const BRepGraph_WireId theWireId)
{
  const BRepGraph::RefsView&                    aRefs     = theGraph.Refs();
  const NCollection_Vector<BRepGraph_WireRefId> aWireRefs = WireRefsOfFace(theGraph, theFaceId);
  for (int aRefIdx = 0; aRefIdx < aWireRefs.Length(); ++aRefIdx)
  {
    if (aRefs.Wire(aWireRefs.Value(aRefIdx)).WireDefId == theWireId)
      return true;
  }
  return false;
}

//=================================================================================================

inline NCollection_Vector<BRepGraph_FaceRefId> FaceRefsOfShell(const BRepGraph&        theGraph,
                                                               const BRepGraph_ShellId theShellId)
{
  NCollection_Vector<BRepGraph_FaceRefId> aRefIds;
  const BRepGraph::RefsView&              aRefs       = theGraph.Refs();
  const BRepGraph_NodeId                  aParentNode = BRepGraph_NodeId::Shell(theShellId.Index);
  for (int aRefIdx = 0; aRefIdx < aRefs.NbFaceRefs(); ++aRefIdx)
  {
    const BRepGraph_FaceRefId         aRefId(aRefIdx);
    const BRepGraphInc::FaceRefEntry& aRef = aRefs.Face(aRefId);
    if (aRef.ParentId == aParentNode && !aRef.IsRemoved)
      aRefIds.Append(aRefId);
  }
  return aRefIds;
}

//=================================================================================================

inline int CountFaceRefsOfShell(const BRepGraph& theGraph, const BRepGraph_ShellId theShellId)
{
  return FaceRefsOfShell(theGraph, theShellId).Length();
}

//=================================================================================================

inline NCollection_Vector<BRepGraph_ShellRefId> ShellRefsOfSolid(const BRepGraph&        theGraph,
                                                                 const BRepGraph_SolidId theSolidId)
{
  NCollection_Vector<BRepGraph_ShellRefId> aRefIds;
  const BRepGraph::RefsView&               aRefs       = theGraph.Refs();
  const BRepGraph_NodeId                   aParentNode = BRepGraph_NodeId::Solid(theSolidId.Index);
  for (int aRefIdx = 0; aRefIdx < aRefs.NbShellRefs(); ++aRefIdx)
  {
    const BRepGraph_ShellRefId         aRefId(aRefIdx);
    const BRepGraphInc::ShellRefEntry& aRef = aRefs.Shell(aRefId);
    if (aRef.ParentId == aParentNode && !aRef.IsRemoved)
      aRefIds.Append(aRefId);
  }
  return aRefIds;
}

//=================================================================================================

inline int CountShellRefsOfSolid(const BRepGraph& theGraph, const BRepGraph_SolidId theSolidId)
{
  return ShellRefsOfSolid(theGraph, theSolidId).Length();
}

//=================================================================================================

inline NCollection_Vector<BRepGraph_SolidRefId> SolidRefsOfCompSolid(
  const BRepGraph&            theGraph,
  const BRepGraph_CompSolidId theCompSolidId)
{
  NCollection_Vector<BRepGraph_SolidRefId> aRefIds;
  const BRepGraph::RefsView&               aRefs = theGraph.Refs();
  const BRepGraph_NodeId aParentNode = BRepGraph_NodeId::CompSolid(theCompSolidId.Index);
  for (int aRefIdx = 0; aRefIdx < aRefs.NbSolidRefs(); ++aRefIdx)
  {
    const BRepGraph_SolidRefId         aRefId(aRefIdx);
    const BRepGraphInc::SolidRefEntry& aRef = aRefs.Solid(aRefId);
    if (aRef.ParentId == aParentNode && !aRef.IsRemoved)
      aRefIds.Append(aRefId);
  }
  return aRefIds;
}

//=================================================================================================

inline int CountSolidRefsOfCompSolid(const BRepGraph&            theGraph,
                                     const BRepGraph_CompSolidId theCompSolidId)
{
  return SolidRefsOfCompSolid(theGraph, theCompSolidId).Length();
}

//=================================================================================================

inline NCollection_Vector<BRepGraph_ChildRefId> ChildRefsOfParent(
  const BRepGraph&       theGraph,
  const BRepGraph_NodeId theParentId)
{
  NCollection_Vector<BRepGraph_ChildRefId> aRefIds;
  const BRepGraph::RefsView&               aRefs = theGraph.Refs();
  for (int aRefIdx = 0; aRefIdx < aRefs.NbChildRefs(); ++aRefIdx)
  {
    const BRepGraph_ChildRefId         aRefId(aRefIdx);
    const BRepGraphInc::ChildRefEntry& aRef = aRefs.Child(aRefId);
    if (aRef.ParentId == theParentId && !aRef.IsRemoved)
      aRefIds.Append(aRefId);
  }
  return aRefIds;
}

//=================================================================================================

inline int CountChildRefsOfParent(const BRepGraph& theGraph, const BRepGraph_NodeId theParentId)
{
  return ChildRefsOfParent(theGraph, theParentId).Length();
}

//=================================================================================================

inline BRepGraph_WireId OuterWireOfFace(const BRepGraph& theGraph, const BRepGraph_FaceId theFaceId)
{
  return theGraph.Topo().OuterWireOfFace(theFaceId);
}

//=================================================================================================

inline NCollection_Vector<BRepGraph_CoEdgeRefId> CoEdgeRefsOfWire(
  const BRepGraphInc_Storage& theStorage,
  const BRepGraph_WireId      theWireId)
{
  NCollection_Vector<BRepGraph_CoEdgeRefId> aRefIds;
  const BRepGraph_NodeId                    aParentNode = BRepGraph_NodeId::Wire(theWireId.Index);
  for (int aRefIdx = 0; aRefIdx < theStorage.NbCoEdgeRefs(); ++aRefIdx)
  {
    const BRepGraph_CoEdgeRefId         aRefId(aRefIdx);
    const BRepGraphInc::CoEdgeRefEntry& aRef = theStorage.CoEdgeRefEntry(aRefId);
    if (aRef.ParentId == aParentNode && !aRef.IsRemoved)
      aRefIds.Append(aRefId);
  }
  return aRefIds;
}

//=================================================================================================

inline int CountCoEdgeRefsOfWire(const BRepGraphInc_Storage& theStorage,
                                 const BRepGraph_WireId      theWireId)
{
  return CoEdgeRefsOfWire(theStorage, theWireId).Length();
}

//=================================================================================================

inline NCollection_Vector<BRepGraph_WireRefId> WireRefsOfFace(
  const BRepGraphInc_Storage& theStorage,
  const BRepGraph_FaceId      theFaceId)
{
  NCollection_Vector<BRepGraph_WireRefId> aRefIds;
  const BRepGraph_NodeId                  aParentNode = BRepGraph_NodeId::Face(theFaceId.Index);
  for (int aRefIdx = 0; aRefIdx < theStorage.NbWireRefs(); ++aRefIdx)
  {
    const BRepGraph_WireRefId         aRefId(aRefIdx);
    const BRepGraphInc::WireRefEntry& aRef = theStorage.WireRefEntry(aRefId);
    if (aRef.ParentId == aParentNode && !aRef.IsRemoved)
      aRefIds.Append(aRefId);
  }
  return aRefIds;
}

//=================================================================================================

inline int CountWireRefsOfFace(const BRepGraphInc_Storage& theStorage,
                               const BRepGraph_FaceId      theFaceId)
{
  return WireRefsOfFace(theStorage, theFaceId).Length();
}

//=================================================================================================

inline bool FaceUsesWire(const BRepGraphInc_Storage& theStorage,
                         const BRepGraph_FaceId      theFaceId,
                         const BRepGraph_WireId      theWireId)
{
  const NCollection_Vector<BRepGraph_WireRefId> aWireRefs = WireRefsOfFace(theStorage, theFaceId);
  for (int aRefIdx = 0; aRefIdx < aWireRefs.Length(); ++aRefIdx)
  {
    const BRepGraphInc::WireRefEntry& aWireRef = theStorage.WireRefEntry(aWireRefs.Value(aRefIdx));
    if (aWireRef.WireDefId == theWireId)
      return true;
  }
  return false;
}

//=================================================================================================

inline NCollection_Vector<BRepGraph_FaceRefId> FaceRefsOfShell(
  const BRepGraphInc_Storage& theStorage,
  const BRepGraph_ShellId     theShellId)
{
  NCollection_Vector<BRepGraph_FaceRefId> aRefIds;
  const BRepGraph_NodeId                  aParentNode = BRepGraph_NodeId::Shell(theShellId.Index);
  for (int aRefIdx = 0; aRefIdx < theStorage.NbFaceRefs(); ++aRefIdx)
  {
    const BRepGraph_FaceRefId         aRefId(aRefIdx);
    const BRepGraphInc::FaceRefEntry& aRef = theStorage.FaceRefEntry(aRefId);
    if (aRef.ParentId == aParentNode && !aRef.IsRemoved)
      aRefIds.Append(aRefId);
  }
  return aRefIds;
}

//=================================================================================================

inline int CountFaceRefsOfShell(const BRepGraphInc_Storage& theStorage,
                                const BRepGraph_ShellId     theShellId)
{
  return FaceRefsOfShell(theStorage, theShellId).Length();
}

//=================================================================================================

inline NCollection_Vector<BRepGraph_ShellRefId> ShellRefsOfSolid(
  const BRepGraphInc_Storage& theStorage,
  const BRepGraph_SolidId     theSolidId)
{
  NCollection_Vector<BRepGraph_ShellRefId> aRefIds;
  const BRepGraph_NodeId                   aParentNode = BRepGraph_NodeId::Solid(theSolidId.Index);
  for (int aRefIdx = 0; aRefIdx < theStorage.NbShellRefs(); ++aRefIdx)
  {
    const BRepGraph_ShellRefId         aRefId(aRefIdx);
    const BRepGraphInc::ShellRefEntry& aRef = theStorage.ShellRefEntry(aRefId);
    if (aRef.ParentId == aParentNode && !aRef.IsRemoved)
      aRefIds.Append(aRefId);
  }
  return aRefIds;
}

//=================================================================================================

inline int CountShellRefsOfSolid(const BRepGraphInc_Storage& theStorage,
                                 const BRepGraph_SolidId     theSolidId)
{
  return ShellRefsOfSolid(theStorage, theSolidId).Length();
}

//=================================================================================================

inline NCollection_Vector<BRepGraph_SolidRefId> SolidRefsOfCompSolid(
  const BRepGraphInc_Storage& theStorage,
  const BRepGraph_CompSolidId theCompSolidId)
{
  NCollection_Vector<BRepGraph_SolidRefId> aRefIds;
  const BRepGraph_NodeId aParentNode = BRepGraph_NodeId::CompSolid(theCompSolidId.Index);
  for (int aRefIdx = 0; aRefIdx < theStorage.NbSolidRefs(); ++aRefIdx)
  {
    const BRepGraph_SolidRefId         aRefId(aRefIdx);
    const BRepGraphInc::SolidRefEntry& aRef = theStorage.SolidRefEntry(aRefId);
    if (aRef.ParentId == aParentNode && !aRef.IsRemoved)
      aRefIds.Append(aRefId);
  }
  return aRefIds;
}

//=================================================================================================

inline int CountSolidRefsOfCompSolid(const BRepGraphInc_Storage& theStorage,
                                     const BRepGraph_CompSolidId theCompSolidId)
{
  return SolidRefsOfCompSolid(theStorage, theCompSolidId).Length();
}

//=================================================================================================

inline NCollection_Vector<BRepGraph_ChildRefId> ChildRefsOfParent(
  const BRepGraphInc_Storage& theStorage,
  const BRepGraph_NodeId      theParentId)
{
  NCollection_Vector<BRepGraph_ChildRefId> aRefIds;
  for (int aRefIdx = 0; aRefIdx < theStorage.NbChildRefs(); ++aRefIdx)
  {
    const BRepGraph_ChildRefId         aRefId(aRefIdx);
    const BRepGraphInc::ChildRefEntry& aRef = theStorage.ChildRefEntry(aRefId);
    if (aRef.ParentId == theParentId && !aRef.IsRemoved)
      aRefIds.Append(aRefId);
  }
  return aRefIds;
}

//=================================================================================================

inline int CountChildRefsOfParent(const BRepGraphInc_Storage& theStorage,
                                  const BRepGraph_NodeId      theParentId)
{
  return ChildRefsOfParent(theStorage, theParentId).Length();
}

//=================================================================================================

inline BRepGraph_WireId OuterWireOfFace(const BRepGraphInc_Storage& theStorage,
                                        const BRepGraph_FaceId      theFaceId)
{
  const NCollection_Vector<BRepGraph_WireRefId> aWireRefs = WireRefsOfFace(theStorage, theFaceId);
  for (int aRefIdx = 0; aRefIdx < aWireRefs.Length(); ++aRefIdx)
  {
    const BRepGraphInc::WireRefEntry& aWireRef = theStorage.WireRefEntry(aWireRefs.Value(aRefIdx));
    if (aWireRef.IsOuter)
      return aWireRef.WireDefId;
  }
  return BRepGraph_WireId();
}

} // namespace BRepGraph_TestTools

#endif // _BRepGraph_RefTestTools_HeaderFile
