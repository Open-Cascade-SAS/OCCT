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
//! - typed reference entry access (ShellRef, FaceRef, ...)
//! - reference counts
//! - RefUID lookup and reverse lookup
//! - stale tracking via BRepGraph_VersionStamp
class BRepGraph::RefsView
{
public:
  //! @name Reference counts
  [[nodiscard]] Standard_EXPORT int NbShellRefs() const;
  [[nodiscard]] Standard_EXPORT int NbFaceRefs() const;
  [[nodiscard]] Standard_EXPORT int NbWireRefs() const;
  [[nodiscard]] Standard_EXPORT int NbCoEdgeRefs() const;
  [[nodiscard]] Standard_EXPORT int NbVertexRefs() const;
  [[nodiscard]] Standard_EXPORT int NbSolidRefs() const;
  [[nodiscard]] Standard_EXPORT int NbChildRefs() const;
  [[nodiscard]] Standard_EXPORT int NbOccurrenceRefs() const;

  //! @name Reference entry access
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::ShellRef& Shell(
    const BRepGraph_ShellRefId theRefId) const;
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::FaceRef& Face(
    const BRepGraph_FaceRefId theRefId) const;
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::WireRef& Wire(
    const BRepGraph_WireRefId theRefId) const;
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::CoEdgeRef& CoEdge(
    const BRepGraph_CoEdgeRefId theRefId) const;
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::VertexRef& Vertex(
    const BRepGraph_VertexRefId theRefId) const;
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::SolidRef& Solid(
    const BRepGraph_SolidRefId theRefId) const;
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::ChildRef& Child(
    const BRepGraph_ChildRefId theRefId) const;
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::OccurrenceRef& Occurrence(
    const BRepGraph_OccurrenceRefId theRefId) const;

  //! @name RefUID operations
  [[nodiscard]] Standard_EXPORT BRepGraph_RefUID UIDOf(const BRepGraph_RefId theRefId) const;
  [[nodiscard]] Standard_EXPORT BRepGraph_RefId  RefIdFrom(const BRepGraph_RefUID& theUID) const;
  [[nodiscard]] Standard_EXPORT bool             Has(const BRepGraph_RefUID& theUID) const;

  //! @name Ref version stamping
  [[nodiscard]] Standard_EXPORT BRepGraph_VersionStamp
                                     StampOf(const BRepGraph_RefId theRefId) const;
  [[nodiscard]] Standard_EXPORT bool IsStale(const BRepGraph_VersionStamp& theStamp) const;

  //! @name Parent-to-ref typed vector access
  //! Return the typed RefId vector of refs belonging to a parent entity.

  //! @param[in] theShell shell entity identifier
  //! @return face ref ids owned by this shell
  [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_FaceRefId>& FaceRefIdsOf(
    const BRepGraph_ShellId theShell) const;

  //! @param[in] theFace face entity identifier
  //! @return wire ref ids owned by this face
  [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_WireRefId>& WireRefIdsOf(
    const BRepGraph_FaceId theFace) const;

  //! @param[in] theWire wire entity identifier
  //! @return coedge ref ids owned by this wire
  [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_CoEdgeRefId>& CoEdgeRefIdsOf(
    const BRepGraph_WireId theWire) const;

  //! @param[in] theSolid solid entity identifier
  //! @return shell ref ids owned by this solid
  [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_ShellRefId>& ShellRefIdsOf(
    const BRepGraph_SolidId theSolid) const;

  //! @param[in] theCompound compound entity identifier
  //! @return child ref ids owned by this compound
  [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_ChildRefId>& ChildRefIdsOf(
    const BRepGraph_CompoundId theCompound) const;

  //! @param[in] theProduct product entity identifier
  //! @return occurrence ref ids owned by this product
  [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_OccurrenceRefId>&
    OccurrenceRefIdsOf(const BRepGraph_ProductId theProduct) const;

  //! @param[in] theCompSolid comp-solid entity identifier
  //! @return solid ref ids owned by this comp-solid
  [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_SolidRefId>& SolidRefIdsOf(
    const BRepGraph_CompSolidId theCompSolid) const;

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
