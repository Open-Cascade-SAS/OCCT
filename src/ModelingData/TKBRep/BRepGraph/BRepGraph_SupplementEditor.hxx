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

#ifndef _BRepGraph_SupplementEditor_HeaderFile
#define _BRepGraph_SupplementEditor_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_LayerTopoSupplement.hxx>

//! @brief Lightweight mutation facade for runtime supplement attachments.
class BRepGraph_SupplementEditor
{
public:
  //! @brief Create an editor facade bound to one graph instance.
  //! @param[in] theGraph graph receiving supplement attachments
  explicit BRepGraph_SupplementEditor(BRepGraph& theGraph)
      : myGraph(theGraph)
  {
  }

  //! @brief Attach one supplemental shape to an arbitrary supported core owner.
  //! @param[in] theOwner active owner node
  //! @param[in] theKind semantic attachment kind
  //! @param[in] theShape supplemental shape to attach
  //! @return non-zero attachment uid on success, `0` on rejection
  [[nodiscard]] Standard_EXPORT uint64_t
    Attach(BRepGraph_NodeId                              theOwner,
           BRepGraph_LayerTopoSupplement::AttachmentKind theKind,
           const TopoDS_Shape&                           theShape);

  //! @brief Attach a supplemental shape to a vertex owner.
  //! @param[in] theVertex active vertex owner
  //! @param[in] theShape supplemental shape to attach
  //! @param[in] theKind semantic attachment kind
  //! @return non-zero attachment uid on success, `0` on rejection
  [[nodiscard]] Standard_EXPORT uint64_t
    AttachToVertex(BRepGraph_VertexId                            theVertex,
                   const TopoDS_Shape&                           theShape,
                   BRepGraph_LayerTopoSupplement::AttachmentKind theKind =
                     BRepGraph_LayerTopoSupplement::AttachmentKind::VertexSupplementShape);

  //! @brief Attach a supplemental shape to an edge owner.
  //! @param[in] theEdge active edge owner
  //! @param[in] theShape supplemental shape to attach
  //! @param[in] theKind semantic attachment kind
  //! @return non-zero attachment uid on success, `0` on rejection
  [[nodiscard]] Standard_EXPORT uint64_t
    AttachToEdge(BRepGraph_EdgeId                              theEdge,
                 const TopoDS_Shape&                           theShape,
                 BRepGraph_LayerTopoSupplement::AttachmentKind theKind =
                   BRepGraph_LayerTopoSupplement::AttachmentKind::EdgeInternalVertex);

  //! @brief Attach a supplemental shape to a face owner.
  //! @param[in] theFace active face owner
  //! @param[in] theShape supplemental shape to attach
  //! @param[in] theKind semantic attachment kind
  //! @return non-zero attachment uid on success, `0` on rejection
  [[nodiscard]] Standard_EXPORT uint64_t
    AttachToFace(BRepGraph_FaceId                              theFace,
                 const TopoDS_Shape&                           theShape,
                 BRepGraph_LayerTopoSupplement::AttachmentKind theKind =
                   BRepGraph_LayerTopoSupplement::AttachmentKind::FaceDirectVertex);

  //! @brief Attach a supplemental shape to a solid owner.
  //! @param[in] theSolid active solid owner
  //! @param[in] theShape supplemental shape to attach
  //! @param[in] theKind semantic attachment kind
  //! @return non-zero attachment uid on success, `0` on rejection
  [[nodiscard]] Standard_EXPORT uint64_t
    AttachToSolid(BRepGraph_SolidId                             theSolid,
                  const TopoDS_Shape&                           theShape,
                  BRepGraph_LayerTopoSupplement::AttachmentKind theKind =
                    BRepGraph_LayerTopoSupplement::AttachmentKind::SolidAuxShape);

  //! @brief Attach a supplemental shape to a compsolid owner.
  //! @param[in] theCompSolid active compsolid owner
  //! @param[in] theShape supplemental shape to attach
  //! @param[in] theKind semantic attachment kind
  //! @return non-zero attachment uid on success, `0` on rejection
  [[nodiscard]] Standard_EXPORT uint64_t
    AttachToCompSolid(BRepGraph_CompSolidId                         theCompSolid,
                      const TopoDS_Shape&                           theShape,
                      BRepGraph_LayerTopoSupplement::AttachmentKind theKind =
                        BRepGraph_LayerTopoSupplement::AttachmentKind::CompSolidAuxShape);

  //! @brief Attach a supplemental shape to a shell owner.
  //! @param[in] theShell active shell owner
  //! @param[in] theShape supplemental shape to attach
  //! @param[in] theKind semantic attachment kind
  //! @return non-zero attachment uid on success, `0` on rejection
  [[nodiscard]] Standard_EXPORT uint64_t
    AttachToShell(BRepGraph_ShellId                             theShell,
                  const TopoDS_Shape&                           theShape,
                  BRepGraph_LayerTopoSupplement::AttachmentKind theKind =
                    BRepGraph_LayerTopoSupplement::AttachmentKind::ShellAuxShape);

  //! @brief Attach a supplemental shape to a compound owner.
  //! @param[in] theCompound active compound owner
  //! @param[in] theShape supplemental shape to attach
  //! @param[in] theKind semantic attachment kind
  //! @return non-zero attachment uid on success, `0` on rejection
  [[nodiscard]] Standard_EXPORT uint64_t
    AttachToCompound(BRepGraph_CompoundId                          theCompound,
                     const TopoDS_Shape&                           theShape,
                     BRepGraph_LayerTopoSupplement::AttachmentKind theKind =
                       BRepGraph_LayerTopoSupplement::AttachmentKind::CompoundAuxShape);

  //! @brief Remove one attachment by uid.
  //! @param[in] theUid layer-local attachment uid
  //! @return `true` when the attachment existed and was removed
  Standard_EXPORT bool RemoveAttachment(uint64_t theUid);

private:
  BRepGraph& myGraph;
};

#endif // _BRepGraph_SupplementEditor_HeaderFile
