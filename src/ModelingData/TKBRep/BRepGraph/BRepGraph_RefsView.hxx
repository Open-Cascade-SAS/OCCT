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

#ifndef _BRepGraph_RefsView_HeaderFile
#define _BRepGraph_RefsView_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraph_VersionStamp.hxx>

//! @brief Read-only view for RefId/RefUID-based reference storage.
//!
//! This view exposes reference-entry storage:
//! - typed reference entry access (Shell, Face, ...)
//! - reference counts
//! - RefUID lookup and reverse lookup through BRepGraph::UIDs()
//! - stale tracking via BRepGraph_VersionStamp through BRepGraph::UIDs()
//!
//! Identity semantics:
//! - RefId (kind + index) is graph-local and may change after Compact().
//!   Use it for in-graph traversal and short-lived mutation logic.
//! - RefUID (kind + counter + generation) is stable across index remapping
//!   and intended for longer-lived identity tracking.
//!
//! ## RefsView vs TopoView naming
//! RefsView accessors take reference IDs (BRepGraph_ShellRefId, BRepGraph_FaceRefId)
//! and return reference-entry structs carrying per-use orientation and location.
//! TopoView accessors take definition IDs (BRepGraph_ShellId, BRepGraph_FaceId)
//! and return definition structs.
//!
//! ## Iterating over references
//! Reference entries are primarily traversed in parent-owned context through
//! the typed grouped IdsOf accessors. When flat iteration is needed, iterate
//! using the matching typed RefId over the appropriate grouped Nb() or NbActive() count:
//! @code
//!   const BRepGraph::RefsView& aRefs = aGraph.Refs();
//!   const BRepGraph_FaceRefId anEndFaceRefId = aRefs.Faces().EndId();
//!   for (BRepGraph_FaceRefId aFaceRefId = aRefs.Faces().StartId();
//!        aFaceRefId < anEndFaceRefId;
//!        ++aFaceRefId)
//!   {
//!     const BRepGraphInc::FaceRef& aFR = aRefs.Faces().Entry(aFaceRefId);
//!     if (aFR.IsRemoved)
//!       continue;
//!     // use aFR.FaceDefId, aFR.Orientation, aFR.Location ...
//!   }
//! @endcode
//!
//! To iterate refs belonging to a specific parent, use the grouped IdsOf
//! accessors:
//! @code
//!   for (const BRepGraph_WireRefId& aWireRefId : aRefs.Wires().IdsOf(aFaceId))
//!   {
//!     const BRepGraphInc::WireRef& aWR = aRefs.Wires().Entry(aWireRefId);
//!     // ...
//!   }
//! @endcode
class BRepGraph::RefsView
{
public:
  //! @brief Shell reference queries.
  class ShellOps
  {
  public:
    [[nodiscard]] Standard_EXPORT int   Nb() const;
    [[nodiscard]] Standard_EXPORT int   NbActive() const;
    [[nodiscard]] BRepGraph_ShellRefId  StartId() const { return BRepGraph_ShellRefId::Start(); }
    [[nodiscard]] BRepGraph_ShellRefId  EndId() const { return BRepGraph_ShellRefId(Nb()); }
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::ShellRef& Entry(
      const BRepGraph_ShellRefId theRefId) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_ShellRefId>& IdsOf(
      const BRepGraph_SolidId theSolid) const;

  private:
    friend class RefsView;

    explicit ShellOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    const BRepGraph* myGraph;
  };

  //! @brief Face reference queries.
  class FaceOps
  {
  public:
    [[nodiscard]] Standard_EXPORT int   Nb() const;
    [[nodiscard]] Standard_EXPORT int   NbActive() const;
    [[nodiscard]] BRepGraph_FaceRefId   StartId() const { return BRepGraph_FaceRefId::Start(); }
    [[nodiscard]] BRepGraph_FaceRefId   EndId() const { return BRepGraph_FaceRefId(Nb()); }
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::FaceRef& Entry(
      const BRepGraph_FaceRefId theRefId) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_FaceRefId>& IdsOf(
      const BRepGraph_ShellId theShell) const;

  private:
    friend class RefsView;

    explicit FaceOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    const BRepGraph* myGraph;
  };

  //! @brief Wire reference queries.
  class WireOps
  {
  public:
    [[nodiscard]] Standard_EXPORT int   Nb() const;
    [[nodiscard]] Standard_EXPORT int   NbActive() const;
    [[nodiscard]] BRepGraph_WireRefId   StartId() const { return BRepGraph_WireRefId::Start(); }
    [[nodiscard]] BRepGraph_WireRefId   EndId() const { return BRepGraph_WireRefId(Nb()); }
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::WireRef& Entry(
      const BRepGraph_WireRefId theRefId) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_WireRefId>& IdsOf(
      const BRepGraph_FaceId theFace) const;

  private:
    friend class RefsView;

    explicit WireOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    const BRepGraph* myGraph;
  };

  //! @brief Coedge reference queries.
  class CoEdgeOps
  {
  public:
    [[nodiscard]] Standard_EXPORT int   Nb() const;
    [[nodiscard]] Standard_EXPORT int   NbActive() const;
    [[nodiscard]] BRepGraph_CoEdgeRefId StartId() const { return BRepGraph_CoEdgeRefId::Start(); }
    [[nodiscard]] BRepGraph_CoEdgeRefId EndId() const { return BRepGraph_CoEdgeRefId(Nb()); }
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::CoEdgeRef& Entry(
      const BRepGraph_CoEdgeRefId theRefId) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_CoEdgeRefId>& IdsOf(
      const BRepGraph_WireId theWire) const;

  private:
    friend class RefsView;

    explicit CoEdgeOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    const BRepGraph* myGraph;
  };

  //! @brief Vertex reference queries.
  class VertexOps
  {
  public:
    [[nodiscard]] Standard_EXPORT int   Nb() const;
    [[nodiscard]] Standard_EXPORT int   NbActive() const;
    [[nodiscard]] BRepGraph_VertexRefId StartId() const { return BRepGraph_VertexRefId::Start(); }
    [[nodiscard]] BRepGraph_VertexRefId EndId() const { return BRepGraph_VertexRefId(Nb()); }
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::VertexRef& Entry(
      const BRepGraph_VertexRefId theRefId) const;

  private:
    friend class RefsView;

    explicit VertexOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    const BRepGraph* myGraph;
  };

  //! @brief Solid reference queries.
  class SolidOps
  {
  public:
    [[nodiscard]] Standard_EXPORT int   Nb() const;
    [[nodiscard]] Standard_EXPORT int   NbActive() const;
    [[nodiscard]] BRepGraph_SolidRefId  StartId() const { return BRepGraph_SolidRefId::Start(); }
    [[nodiscard]] BRepGraph_SolidRefId  EndId() const { return BRepGraph_SolidRefId(Nb()); }
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::SolidRef& Entry(
      const BRepGraph_SolidRefId theRefId) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_SolidRefId>& IdsOf(
      const BRepGraph_CompSolidId theCompSolid) const;

  private:
    friend class RefsView;

    explicit SolidOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    const BRepGraph* myGraph;
  };

  //! @brief Generic child reference queries.
  class ChildOps
  {
  public:
    [[nodiscard]] Standard_EXPORT int   Nb() const;
    [[nodiscard]] Standard_EXPORT int   NbActive() const;
    [[nodiscard]] BRepGraph_ChildRefId  StartId() const { return BRepGraph_ChildRefId::Start(); }
    [[nodiscard]] BRepGraph_ChildRefId  EndId() const { return BRepGraph_ChildRefId(Nb()); }
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::ChildRef& Entry(
      const BRepGraph_ChildRefId theRefId) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_ChildRefId>& IdsOf(
      const BRepGraph_CompoundId theCompound) const;

  private:
    friend class RefsView;

    explicit ChildOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    const BRepGraph* myGraph;
  };

  //! @brief Occurrence reference queries.
  class OccurrenceOps
  {
  public:
    [[nodiscard]] Standard_EXPORT int   Nb() const;
    [[nodiscard]] Standard_EXPORT int   NbActive() const;
    [[nodiscard]] BRepGraph_OccurrenceRefId StartId() const
    {
      return BRepGraph_OccurrenceRefId::Start();
    }
    [[nodiscard]] BRepGraph_OccurrenceRefId EndId() const
    {
      return BRepGraph_OccurrenceRefId(Nb());
    }
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::OccurrenceRef& Entry(
      const BRepGraph_OccurrenceRefId theRefId) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_OccurrenceRefId>& IdsOf(
      const BRepGraph_ProductId theProduct) const;

  private:
    friend class RefsView;

    explicit OccurrenceOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    const BRepGraph* myGraph;
  };

  //! Grouped shell reference queries.
  [[nodiscard]] const ShellOps& Shells() const { return myShells; }

  //! Grouped face reference queries.
  [[nodiscard]] const FaceOps& Faces() const { return myFaces; }

  //! Grouped wire reference queries.
  [[nodiscard]] const WireOps& Wires() const { return myWires; }

  //! Grouped coedge reference queries.
  [[nodiscard]] const CoEdgeOps& CoEdges() const { return myCoEdges; }

  //! Grouped vertex reference queries.
  [[nodiscard]] const VertexOps& Vertices() const { return myVertices; }

  //! Grouped solid reference queries.
  [[nodiscard]] const SolidOps& Solids() const { return mySolids; }

  //! Grouped child reference queries.
  [[nodiscard]] const ChildOps& Children() const { return myChildren; }

  //! Grouped occurrence reference queries.
  [[nodiscard]] const OccurrenceOps& Occurrences() const { return myOccurrences; }

  //! Return the direct parent-owned RefId stored at the specified child step.
  //! This is a structural lookup over the parent's raw ref arrays and does not
  //! skip removed refs or refs targeting removed child defs.
  [[nodiscard]] Standard_EXPORT BRepGraph_RefId RefAtStep(const BRepGraph_NodeId theParent,
                                                          const int              theStep) const;

  //! Resolve the child definition node referenced by any typed RefId.
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId ChildNode(const BRepGraph_RefId theRef) const;

  //! Return true if the specified typed RefId is marked removed.
  [[nodiscard]] Standard_EXPORT bool IsRemoved(const BRepGraph_RefId theRef) const;

  //! Return the local location carried by the specified typed RefId.
  //! OccurrenceRef and invalid refs return identity.
  [[nodiscard]] Standard_EXPORT TopLoc_Location LocalLocation(const BRepGraph_RefId theRef) const;

  //! Return the orientation carried by the specified typed RefId.
  //! CoEdgeRef, OccurrenceRef, and invalid refs return TopAbs_FORWARD.
  [[nodiscard]] Standard_EXPORT TopAbs_Orientation Orientation(const BRepGraph_RefId theRef) const;

private:
  friend class BRepGraph;
  friend struct BRepGraph_Data;

  explicit RefsView(const BRepGraph* theGraph)
      : myGraph(theGraph),
        myShells(theGraph),
        myFaces(theGraph),
        myWires(theGraph),
        myCoEdges(theGraph),
        myVertices(theGraph),
        mySolids(theGraph),
        myChildren(theGraph),
        myOccurrences(theGraph)
  {
  }

  const BRepGraph* myGraph;
  ShellOps         myShells;
  FaceOps          myFaces;
  WireOps          myWires;
  CoEdgeOps        myCoEdges;
  VertexOps        myVertices;
  SolidOps         mySolids;
  ChildOps         myChildren;
  OccurrenceOps    myOccurrences;
};

#endif // _BRepGraph_RefsView_HeaderFile
