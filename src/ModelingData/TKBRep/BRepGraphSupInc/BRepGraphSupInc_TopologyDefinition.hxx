// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.

#ifndef _BRepGraphSupInc_TopologyDefinition_HeaderFile
#define _BRepGraphSupInc_TopologyDefinition_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <BRepGraphSupInc_Definition.hxx>
#include <BRepGraphSupInc_ItemUID.hxx>
#include <TopoDS_Shape.hxx>

#include <cstdint>
#include <limits>

//! Typed dense index into BRepGraphSupInc_TopologyStore.
//! This transient index is invalid when removed and may change after compaction; use ItemUID for
//! an active-process reference.
struct BRepGraphSupInc_TopologyId
{
  //! Sentinel index representing an invalid dense ID.
  static constexpr uint32_t THE_INVALID_INDEX = std::numeric_limits<uint32_t>::max();

  //! Zero-based position in the current dense storage.
  uint32_t Index = THE_INVALID_INDEX;

  //! Return true when this is not the invalid-index sentinel.
  //! @return true when the ID has an index value
  [[nodiscard]] bool IsValid() const noexcept { return Index != THE_INVALID_INDEX; }
  //! Compare dense index values.
  //! @param[in] theOther ID to compare
  //! @return true when both IDs have the same index
  [[nodiscard]] bool operator==(const BRepGraphSupInc_TopologyId& theOther) const noexcept
  {
    return Index == theOther.Index;
  }
  //! Compare dense index values for inequality.
  //! @param[in] theOther ID to compare
  //! @return true when the IDs have different indices
  [[nodiscard]] bool operator!=(const BRepGraphSupInc_TopologyId& theOther) const noexcept
  {
    return !(*this == theOther);
  }
};

namespace BRepGraphSupInc
{
//! Semantic role of a supplemental topology attachment.
enum class TopologyAttachmentKind : uint32_t
{
  //! Supplementary shape owned by a vertex node.
  VertexSupplementShape,
  //! Internal vertex shape owned by an edge node.
  EdgeInternalVertex,
  //! Direct vertex shape owned by a face node.
  FaceDirectVertex,
  //! Auxiliary shape owned by a solid node.
  SolidAuxShape,
  //! Auxiliary shape owned by a shell node.
  ShellAuxShape,
  //! Auxiliary shape owned by a compsolid node.
  CompSolidAuxShape,
  //! Auxiliary shape owned by a compound node.
  CompoundAuxShape,
  //! Supplementary shape owned by any supported topology node kind.
  GenericSupplementShape
};

//! Active-process runtime topology attachment with its supplemental shape.
struct TopologyDef : public BaseDef
{
  //! Owning core topology node; the attachment lifetime follows this node's ownership operations.
  BRepGraph_NodeId        Owner;
  //! Non-null supplemental topological shape.
  TopoDS_Shape            Shape;
  //! Semantic role that constrains the permitted owner node kind.
  TopologyAttachmentKind  Kind      = TopologyAttachmentKind::GenericSupplementShape;
  //! True after soft removal; removed records are skipped and later discarded by compaction.
  bool                    IsRemoved = false;
};
} // namespace BRepGraphSupInc

#endif // _BRepGraphSupInc_TopologyDefinition_HeaderFile
