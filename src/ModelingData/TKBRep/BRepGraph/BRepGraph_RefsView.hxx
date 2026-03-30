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
//! - RefUID lookup and reverse lookup
//! - stale tracking via BRepGraph_VersionStamp
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
//! the typed RefIdsOf accessors. When flat iteration is needed, there is no
//! dedicated BRepGraph_Iterator for reference types; iterate using a counted
//! loop over the appropriate NbXxxRefs() or NbActiveXxxRefs() count:
//! @code
//!   const BRepGraph::RefsView& aRefs = aGraph.Refs();
//!   for (int i = 0; i < aRefs.NbFaceRefs(); ++i)
//!   {
//!     const BRepGraphInc::FaceRef& aFR = aRefs.Face(BRepGraph_FaceRefId(i));
//!     if (aFR.IsRemoved)
//!       continue;
//!     // use aFR.FaceDefId, aFR.Orientation, aFR.Location ...
//!   }
//! @endcode
//!
//! To iterate refs belonging to a specific parent, use the typed RefIdsOf
//! accessors (e.g. FaceRefIdsOf, WireRefIdsOf):
//! @code
//!   for (const BRepGraph_WireRefId& aWireRefId : aRefs.WireRefIdsOf(aFaceId))
//!   {
//!     const BRepGraphInc::WireRef& aWR = aRefs.Wire(aWireRefId);
//!     // ...
//!   }
//! @endcode
class BRepGraph::RefsView
{
public:
  //! @name Reference counts
  //! Return the total number of reference entries (including removed).
  //! @{

  //! Number of shell references.
  [[nodiscard]] Standard_EXPORT int NbShellRefs() const;
  //! Number of active shell references.
  [[nodiscard]] Standard_EXPORT int NbActiveShellRefs() const;
  //! Number of face references.
  [[nodiscard]] Standard_EXPORT int NbFaceRefs() const;
  //! Number of active face references.
  [[nodiscard]] Standard_EXPORT int NbActiveFaceRefs() const;
  //! Number of wire references.
  [[nodiscard]] Standard_EXPORT int NbWireRefs() const;
  //! Number of active wire references.
  [[nodiscard]] Standard_EXPORT int NbActiveWireRefs() const;
  //! Number of coedge references.
  [[nodiscard]] Standard_EXPORT int NbCoEdgeRefs() const;
  //! Number of active coedge references.
  [[nodiscard]] Standard_EXPORT int NbActiveCoEdgeRefs() const;
  //! Number of vertex references.
  [[nodiscard]] Standard_EXPORT int NbVertexRefs() const;
  //! Number of active vertex references.
  [[nodiscard]] Standard_EXPORT int NbActiveVertexRefs() const;
  //! Number of solid references.
  [[nodiscard]] Standard_EXPORT int NbSolidRefs() const;
  //! Number of active solid references.
  [[nodiscard]] Standard_EXPORT int NbActiveSolidRefs() const;
  //! Number of generic child references.
  [[nodiscard]] Standard_EXPORT int NbChildRefs() const;
  //! Number of active generic child references.
  [[nodiscard]] Standard_EXPORT int NbActiveChildRefs() const;
  //! Number of occurrence references.
  [[nodiscard]] Standard_EXPORT int NbOccurrenceRefs() const;
  //! Number of active occurrence references.
  [[nodiscard]] Standard_EXPORT int NbActiveOccurrenceRefs() const;

  //! @}

  //! @name Reference entry access
  //! Access reference entries by typed identifier.
  //! @{

  //! Access shell reference by typed identifier.
  //! @param[in] theRefId typed shell reference identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::ShellRef& Shell(
    const BRepGraph_ShellRefId theRefId) const;
  //! Access face reference by typed identifier.
  //! @param[in] theRefId typed face reference identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::FaceRef& Face(
    const BRepGraph_FaceRefId theRefId) const;
  //! Access wire reference by typed identifier.
  //! @param[in] theRefId typed wire reference identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::WireRef& Wire(
    const BRepGraph_WireRefId theRefId) const;
  //! Access coedge reference by typed identifier.
  //! @param[in] theRefId typed coedge reference identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::CoEdgeRef& CoEdge(
    const BRepGraph_CoEdgeRefId theRefId) const;
  //! Access vertex reference by typed identifier.
  //! @param[in] theRefId typed vertex reference identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::VertexRef& Vertex(
    const BRepGraph_VertexRefId theRefId) const;
  //! Access solid reference by typed identifier.
  //! @param[in] theRefId typed solid reference identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::SolidRef& Solid(
    const BRepGraph_SolidRefId theRefId) const;
  //! Access child reference by typed identifier.
  //! @param[in] theRefId typed child reference identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::ChildRef& Child(
    const BRepGraph_ChildRefId theRefId) const;
  //! Access occurrence reference by typed identifier.
  //! @param[in] theRefId typed occurrence reference identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::OccurrenceRef& Occurrence(
    const BRepGraph_OccurrenceRefId theRefId) const;

  //! @}

  //! @name RefUID operations
  //! Bidirectional conversion between RefId and persistent RefUID.
  //! @{

  //! Return the RefUID assigned to a reference.
  //! @param[in] theRefId reference identifier
  //! @return RefUID for the reference
  [[nodiscard]] Standard_EXPORT BRepGraph_RefUID UIDOf(const BRepGraph_RefId theRefId) const;
  //! Resolve a RefUID back to a RefId.
  //! @param[in] theUID unique identifier to resolve
  //! @return corresponding RefId, or invalid RefId if not found
  [[nodiscard]] Standard_EXPORT BRepGraph_RefId  RefIdFrom(const BRepGraph_RefUID& theUID) const;
  //! Check if a RefUID is valid and exists in this graph generation.
  //! @param[in] theUID unique identifier to check
  //! @return true if the RefUID belongs to this graph generation
  [[nodiscard]] Standard_EXPORT bool             Has(const BRepGraph_RefUID& theUID) const;

  //! @}

  //! @name Ref version stamping
  //! Track reference staleness across graph mutations.
  //! @{

  //! Produce a version stamp for the given reference.
  //! @param[in] theRefId reference identifier
  //! @return version stamp, or invalid stamp if theRefId is invalid
  [[nodiscard]] Standard_EXPORT BRepGraph_VersionStamp
                                     StampOf(const BRepGraph_RefId theRefId) const;
  //! Check if a previously-taken stamp is stale.
  //! @param[in] theStamp version stamp to check
  //! @return true if the stamp no longer matches the current reference state
  [[nodiscard]] Standard_EXPORT bool IsStale(const BRepGraph_VersionStamp& theStamp) const;

  //! @}

  //! @name Parent-to-ref typed vector access
  //! Return the typed RefId vector of refs belonging to a parent entity.

  //! @param[in] theShell shell entity identifier
  //! @return face ref ids owned by this shell
  [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_FaceRefId>& FaceRefIdsOf(
    const BRepGraph_ShellId theShell) const;

  //! @param[in] theShell shell entity identifier
  //! @param[in] theAllocator allocator for the result vector
  //! @return active (non-removed) face ref ids owned by this shell
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_FaceRefId> ActiveFaceRefIdsOf(
    const BRepGraph_ShellId                         theShell,
    const occ::handle<NCollection_BaseAllocator>& theAllocator) const;

  //! @param[in] theFace face entity identifier
  //! @return wire ref ids owned by this face
  [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_WireRefId>& WireRefIdsOf(
    const BRepGraph_FaceId theFace) const;

  //! @param[in] theFace face entity identifier
  //! @param[in] theAllocator allocator for the result vector
  //! @return active (non-removed) wire ref ids owned by this face
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_WireRefId> ActiveWireRefIdsOf(
    const BRepGraph_FaceId                          theFace,
    const occ::handle<NCollection_BaseAllocator>& theAllocator) const;

  //! @param[in] theWire wire entity identifier
  //! @return coedge ref ids owned by this wire
  [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_CoEdgeRefId>& CoEdgeRefIdsOf(
    const BRepGraph_WireId theWire) const;

  //! @param[in] theWire wire entity identifier
  //! @param[in] theAllocator allocator for the result vector
  //! @return active (non-removed) coedge ref ids owned by this wire
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_CoEdgeRefId> ActiveCoEdgeRefIdsOf(
    const BRepGraph_WireId                          theWire,
    const occ::handle<NCollection_BaseAllocator>& theAllocator) const;

  //! @param[in] theSolid solid entity identifier
  //! @return shell ref ids owned by this solid
  [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_ShellRefId>& ShellRefIdsOf(
    const BRepGraph_SolidId theSolid) const;

  //! @param[in] theSolid solid entity identifier
  //! @param[in] theAllocator allocator for the result vector
  //! @return active (non-removed) shell ref ids owned by this solid
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_ShellRefId> ActiveShellRefIdsOf(
    const BRepGraph_SolidId                         theSolid,
    const occ::handle<NCollection_BaseAllocator>& theAllocator) const;

  //! @param[in] theCompound compound entity identifier
  //! @return child ref ids owned by this compound
  [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_ChildRefId>& ChildRefIdsOf(
    const BRepGraph_CompoundId theCompound) const;

  //! @param[in] theCompound compound entity identifier
  //! @param[in] theAllocator allocator for the result vector
  //! @return active (non-removed) child ref ids owned by this compound
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_ChildRefId> ActiveChildRefIdsOf(
    const BRepGraph_CompoundId                      theCompound,
    const occ::handle<NCollection_BaseAllocator>& theAllocator) const;

  //! @param[in] theProduct product entity identifier
  //! @return occurrence ref ids owned by this product
  [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_OccurrenceRefId>&
    OccurrenceRefIdsOf(const BRepGraph_ProductId theProduct) const;

  //! @param[in] theProduct product entity identifier
  //! @param[in] theAllocator allocator for the result vector
  //! @return active (non-removed) occurrence ref ids owned by this product
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_OccurrenceRefId>
    ActiveOccurrenceRefIdsOf(const BRepGraph_ProductId                  theProduct,
                             const occ::handle<NCollection_BaseAllocator>& theAllocator) const;

  //! @param[in] theCompSolid comp-solid entity identifier
  //! @return solid ref ids owned by this comp-solid
  [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_SolidRefId>& SolidRefIdsOf(
    const BRepGraph_CompSolidId theCompSolid) const;

  //! @param[in] theCompSolid comp-solid entity identifier
  //! @param[in] theAllocator allocator for the result vector
  //! @return active (non-removed) solid ref ids owned by this comp-solid
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_SolidRefId> ActiveSolidRefIdsOf(
    const BRepGraph_CompSolidId                     theCompSolid,
    const occ::handle<NCollection_BaseAllocator>& theAllocator) const;

  //! @param[in] theEdge edge entity identifier
  //! @param[in] theAllocator allocator for the result vector
  //! @return boundary and internal vertex ref ids referenced by this edge
  //!         (start, end, then internals; duplicate ids removed)
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_VertexRefId> VertexRefIdsOf(
    const BRepGraph_EdgeId                          theEdge,
    const occ::handle<NCollection_BaseAllocator>& theAllocator) const;

private:
  friend class BRepGraph;
  friend struct BRepGraph_Data;

  explicit RefsView(const BRepGraph* theGraph)
      : myGraph(theGraph)
  {
  }

  const BRepGraph* myGraph;
};

#endif // _BRepGraph_RefsView_HeaderFile
