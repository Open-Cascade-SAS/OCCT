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
#include <BRepGraphInc_Representation.hxx>
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
  const BRepGraph::RefsView&                aRefs         = theGraph.Refs();
  const BRepGraph_NodeId                    aParentNode   = theWireId;
  const int                                 aNbCoEdgeRefs = aRefs.CoEdges().Nb();
  for (BRepGraph_CoEdgeRefId aRefId(0); aRefId.IsValid(aNbCoEdgeRefs); ++aRefId)
  {
    const BRepGraphInc::CoEdgeRef& aRef = aRefs.CoEdges().Entry(aRefId);
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
  const BRepGraph_NodeId                  aParentNode = theFaceId;
  const int                               aNbWireRefs = aRefs.Wires().Nb();
  for (BRepGraph_WireRefId aRefId(0); aRefId.IsValid(aNbWireRefs); ++aRefId)
  {
    const BRepGraphInc::WireRef& aRef = aRefs.Wires().Entry(aRefId);
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
  for (const BRepGraph_WireRefId& aWireRefId : aWireRefs)
  {
    if (aRefs.Wires().Entry(aWireRefId).WireDefId == theWireId)
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
  const BRepGraph_NodeId                  aParentNode = theShellId;
  const int                               aNbFaceRefs = aRefs.Faces().Nb();
  for (BRepGraph_FaceRefId aRefId(0); aRefId.IsValid(aNbFaceRefs); ++aRefId)
  {
    const BRepGraphInc::FaceRef& aRef = aRefs.Faces().Entry(aRefId);
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
  const BRepGraph::RefsView&               aRefs        = theGraph.Refs();
  const BRepGraph_NodeId                   aParentNode  = theSolidId;
  const int                                aNbShellRefs = aRefs.Shells().Nb();
  for (BRepGraph_ShellRefId aRefId(0); aRefId.IsValid(aNbShellRefs); ++aRefId)
  {
    const BRepGraphInc::ShellRef& aRef = aRefs.Shells().Entry(aRefId);
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
  const BRepGraph::RefsView&               aRefs        = theGraph.Refs();
  const BRepGraph_NodeId                   aParentNode  = theCompSolidId;
  const int                                aNbSolidRefs = aRefs.Solids().Nb();
  for (BRepGraph_SolidRefId aRefId(0); aRefId.IsValid(aNbSolidRefs); ++aRefId)
  {
    const BRepGraphInc::SolidRef& aRef = aRefs.Solids().Entry(aRefId);
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
  const BRepGraph::RefsView&               aRefs        = theGraph.Refs();
  const int                                aNbChildRefs = aRefs.Children().Nb();
  for (BRepGraph_ChildRefId aRefId(0); aRefId.IsValid(aNbChildRefs); ++aRefId)
  {
    const BRepGraphInc::ChildRef& aRef = aRefs.Children().Entry(aRefId);
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
  return theGraph.Topo().Faces().OuterWire(theFaceId);
}

//=================================================================================================

inline NCollection_Vector<BRepGraph_CoEdgeRefId> CoEdgeRefsOfWire(
  const BRepGraphInc_Storage& theStorage,
  const BRepGraph_WireId      theWireId)
{
  NCollection_Vector<BRepGraph_CoEdgeRefId> aRefIds;
  const BRepGraph_NodeId                    aParentNode   = theWireId;
  const int                                 aNbCoEdgeRefs = theStorage.NbCoEdgeRefs();
  for (BRepGraph_CoEdgeRefId aRefId(0); aRefId.IsValid(aNbCoEdgeRefs); ++aRefId)
  {
    const BRepGraphInc::CoEdgeRef& aRef = theStorage.CoEdgeRef(aRefId);
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
  const BRepGraph_NodeId                  aParentNode = theFaceId;
  const int                               aNbWireRefs = theStorage.NbWireRefs();
  for (BRepGraph_WireRefId aRefId(0); aRefId.IsValid(aNbWireRefs); ++aRefId)
  {
    const BRepGraphInc::WireRef& aRef = theStorage.WireRef(aRefId);
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
  for (const BRepGraph_WireRefId& aWireRefId : aWireRefs)
  {
    const BRepGraphInc::WireRef& aWireRef = theStorage.WireRef(aWireRefId);
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
  const BRepGraph_NodeId                  aParentNode = theShellId;
  const int                               aNbFaceRefs = theStorage.NbFaceRefs();
  for (BRepGraph_FaceRefId aRefId(0); aRefId.IsValid(aNbFaceRefs); ++aRefId)
  {
    const BRepGraphInc::FaceRef& aRef = theStorage.FaceRef(aRefId);
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
  const BRepGraph_NodeId                   aParentNode  = theSolidId;
  const int                                aNbShellRefs = theStorage.NbShellRefs();
  for (BRepGraph_ShellRefId aRefId(0); aRefId.IsValid(aNbShellRefs); ++aRefId)
  {
    const BRepGraphInc::ShellRef& aRef = theStorage.ShellRef(aRefId);
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
  const BRepGraph_NodeId                   aParentNode  = theCompSolidId;
  const int                                aNbSolidRefs = theStorage.NbSolidRefs();
  for (BRepGraph_SolidRefId aRefId(0); aRefId.IsValid(aNbSolidRefs); ++aRefId)
  {
    const BRepGraphInc::SolidRef& aRef = theStorage.SolidRef(aRefId);
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
  const int                                aNbChildRefs = theStorage.NbChildRefs();
  for (BRepGraph_ChildRefId aRefId(0); aRefId.IsValid(aNbChildRefs); ++aRefId)
  {
    const BRepGraphInc::ChildRef& aRef = theStorage.ChildRef(aRefId);
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
  for (const BRepGraph_WireRefId& aWireRefId : aWireRefs)
  {
    const BRepGraphInc::WireRef& aWireRef = theStorage.WireRef(aWireRefId);
    if (aWireRef.IsOuter)
      return aWireRef.WireDefId;
  }
  return BRepGraph_WireId();
}

} // namespace BRepGraph_TestTools

#endif // _BRepGraph_RefTestTools_HeaderFile
