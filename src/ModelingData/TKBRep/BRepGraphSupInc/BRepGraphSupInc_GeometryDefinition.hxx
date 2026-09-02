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

#ifndef _BRepGraphSupInc_GeometryDefinition_HeaderFile
#define _BRepGraphSupInc_GeometryDefinition_HeaderFile

#include <BRepGraphSupInc_Definition.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>

#include <cstdint>
#include <limits>

//! Typed dense index for a particular geometry representation kind.
//! This transient index is invalid when removed and may change after compaction; use ItemUID for
//! an active-process reference.
template <typename TagT>
struct BRepGraphSupInc_GeometryId
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
  [[nodiscard]] bool operator==(const BRepGraphSupInc_GeometryId& theOther) const noexcept
  {
    return Index == theOther.Index;
  }
  //! Compare dense index values for inequality.
  //! @param[in] theOther ID to compare
  //! @return true when the IDs have different indices
  [[nodiscard]] bool operator!=(const BRepGraphSupInc_GeometryId& theOther) const noexcept
  {
    return !(*this == theOther);
  }
};

//! Type tag distinguishing dense IDs for 3D curve definitions.
struct BRepGraphSupInc_Curve3dTag;
//! Type tag distinguishing dense IDs for 2D curve definitions.
struct BRepGraphSupInc_Curve2dTag;
//! Type tag distinguishing dense IDs for surface definitions.
struct BRepGraphSupInc_SurfaceTag;

//! Dense ID for a reusable 3D curve definition.
using BRepGraphSupInc_Curve3dId = BRepGraphSupInc_GeometryId<BRepGraphSupInc_Curve3dTag>;
//! Dense ID for a reusable 2D curve definition.
using BRepGraphSupInc_Curve2dId = BRepGraphSupInc_GeometryId<BRepGraphSupInc_Curve2dTag>;
//! Dense ID for a reusable surface definition.
using BRepGraphSupInc_SurfaceId = BRepGraphSupInc_GeometryId<BRepGraphSupInc_SurfaceTag>;

namespace BRepGraphSupInc
{
//! Reusable 3D curve definition.
struct Curve3dDef : public BaseDef
{
  //! Referenced 3D curve.
  occ::handle<Geom_Curve> Curve;
  //! True after soft removal; removed records are skipped and later discarded by compaction.
  bool                    IsRemoved = false;
};

//! Reusable 2D curve definition.
struct Curve2dDef : public BaseDef
{
  //! Referenced parametric curve.
  occ::handle<Geom2d_Curve> Curve;
  //! True after soft removal; removed records are skipped and later discarded by compaction.
  bool                      IsRemoved = false;
};

//! Reusable surface definition.
struct SurfaceDef : public BaseDef
{
  //! Referenced surface.
  occ::handle<Geom_Surface> Surface;
  //! True after soft removal; removed records are skipped and later discarded by compaction.
  bool                      IsRemoved = false;
};
} // namespace BRepGraphSupInc

#endif // _BRepGraphSupInc_GeometryDefinition_HeaderFile
