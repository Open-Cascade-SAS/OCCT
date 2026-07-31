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

#ifndef _BRepGraph_LayerTopoSupplement_HeaderFile
#define _BRepGraph_LayerTopoSupplement_HeaderFile

#include <BRepGraph_Layer.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_LinearVector.hxx>
#include <TopoDS_Shape.hxx>

//! @brief Runtime-only storage for supplemental TopoDS topology fragments.
//!
//! This layer stores non-core topology extracted from a source shape and
//! attached to supported core graph owners. These attachments are not
//! serialized and are intended only to preserve live
//! `TopoDS -> Graph -> TopoDS` behavior.
class BRepGraph_LayerTopoSupplement : public BRepGraph_Layer
{
public:
  //! @brief Semantic role of one supplemental attachment.
  enum class AttachmentKind
  {
    VertexSupplementShape,
    EdgeInternalVertex,
    FaceDirectVertex,
    SolidAuxShape,
    ShellAuxShape,
    CompSolidAuxShape,
    CompoundAuxShape,
    GenericSupplementShape
  };

  //! @brief Stored runtime attachment record.
  struct Entry
  {
    BRepGraph_NodeId BaseOwner;
    uint64_t         LocalUid = 0;
    AttachmentKind   Kind     = AttachmentKind::GenericSupplementShape;
    TopoDS_Shape     Shape;
  };

  //! @brief Return the fixed layer type GUID.
  [[nodiscard]] Standard_EXPORT static const Standard_GUID& GetID();

  //! @brief Return the runtime type GUID for this layer instance.
  [[nodiscard]] Standard_EXPORT const Standard_GUID& ID() const override;

  //! @brief Return a short stable layer name for diagnostics and registry lookup.
  [[nodiscard]] Standard_EXPORT const TCollection_AsciiString& Name() const override;

  //! @brief Find one attachment entry by its layer-local uid.
  //! @param[in] theUid layer-local attachment uid
  //! @return pointer to the entry, or `nullptr` when not found
  [[nodiscard]] Standard_EXPORT const Entry* FindByUid(uint64_t theUid) const;

  //! @brief Return all attachment uids currently owned by one core node.
  //! @param[in] theOwner core topology owner node
  //! @return owner-local insertion-ordered list of attachment uids
  [[nodiscard]] Standard_EXPORT const NCollection_LinearVector<uint64_t>& AttachedTo(
    BRepGraph_NodeId theOwner) const;

  //! @brief Add one supplemental shape attachment to a supported core owner node.
  //! Supported owner kinds are vertex, edge, face, shell, solid, compsolid, and compound.
  //! @param[in] theOwner active core topology owner
  //! @param[in] theKind semantic attachment kind
  //! @param[in] theShape attached supplemental shape
  //! @return non-zero layer-local uid on success, `0` on rejection
  Standard_EXPORT uint64_t AddAttachment(BRepGraph_NodeId    theOwner,
                                         AttachmentKind      theKind,
                                         const TopoDS_Shape& theShape);

  //! @brief Add one supplemental shape attachment with an explicitly preserved uid.
  //! Supported owner kinds are vertex, edge, face, shell, solid, compsolid, and compound.
  //! @param[in] theOwner active core topology owner
  //! @param[in] theUid layer-local attachment uid to preserve
  //! @param[in] theKind semantic attachment kind
  //! @param[in] theShape attached supplemental shape
  //! @return `true` on success, `false` when the uid or input is rejected
  Standard_EXPORT bool AddAttachmentWithUid(BRepGraph_NodeId    theOwner,
                                            uint64_t            theUid,
                                            AttachmentKind      theKind,
                                            const TopoDS_Shape& theShape);

  //! @brief Remove one supplemental attachment by uid.
  //! @param[in] theUid layer-local attachment uid
  //! @return `true` when the attachment existed and was removed
  Standard_EXPORT bool RemoveAttachment(uint64_t theUid);

  //! @brief Validate internal owner/uid bookkeeping invariants.
  //! @throws Standard_ProgramError on inconsistent internal state
  Standard_EXPORT void Validate() const;

  //! @brief Drop all attachments owned by a removed node.
  //! @param[in] theNode removed core node
  Standard_EXPORT void OnNodeRemoved(const BRepGraph_NodeId theNode) noexcept override;

  //! @brief Migrate attachments from one owner node to another compatible node.
  //! @param[in] theOldNode previous owner node
  //! @param[in] theNewNode replacement owner node
  Standard_EXPORT void OnNodeReplaced(const BRepGraph_NodeId theOldNode,
                                      const BRepGraph_NodeId theNewNode) noexcept override;

  //! @brief Copy remapped attachments to the target graph.
  Standard_EXPORT void CopyTo(const BRepGraph_CopyRemap& theCopy) const override;

  //! @brief Invalidate all cached state in the layer.
  Standard_EXPORT void InvalidateAll() noexcept override;

  //! @brief Remove every stored supplemental attachment.
  Standard_EXPORT void Clear() noexcept override;

  DEFINE_STANDARD_RTTIEXT(BRepGraph_LayerTopoSupplement, BRepGraph_Layer)

private:
  //! @brief Remove all attachments belonging to one owner node.
  //! @param[in] theOwner owner node to erase
  void removeOwner(BRepGraph_NodeId theOwner) noexcept;

private:
  NCollection_DataMap<uint64_t, Entry>                                      myEntries;
  NCollection_DataMap<BRepGraph_NodeId, NCollection_LinearVector<uint64_t>> myOwnerToUids;
  NCollection_LinearVector<uint64_t>                                        myEmptyUids;
  uint64_t                                                                  myNextUid = 1;
};

#endif // _BRepGraph_LayerTopoSupplement_HeaderFile
