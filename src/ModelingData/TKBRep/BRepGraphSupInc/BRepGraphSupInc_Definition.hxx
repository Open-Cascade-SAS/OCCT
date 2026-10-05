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

#ifndef _BRepGraphSupInc_Definition_HeaderFile
#define _BRepGraphSupInc_Definition_HeaderFile

#include <cstdint>
#include <limits>

//! Store-local dense identifier of an active supplemental definition.
//! It is transient and may change when the owning store is compacted.
struct BRepGraphSupInc_DefinitionId
{
  //! Sentinel kind representing an invalid dense definition ID.
  static constexpr uint32_t THE_INVALID_KIND = std::numeric_limits<uint32_t>::max();
  //! Sentinel index representing an invalid dense definition ID.
  static constexpr uint32_t THE_INVALID_INDEX = std::numeric_limits<uint32_t>::max();

  //! Store-defined definition kind.
  uint32_t Kind = THE_INVALID_KIND;
  //! Zero-based position in the current storage for the kind.
  uint32_t Index = THE_INVALID_INDEX;

  //! Return true when both kind and index are valid.
  [[nodiscard]] bool IsValid() const noexcept
  {
    return Kind != THE_INVALID_KIND && Index != THE_INVALID_INDEX;
  }

  //! Compare dense definition ID components.
  //! @param[in] theOther ID to compare
  //! @return true when both IDs have the same kind and index
  [[nodiscard]] bool operator==(const BRepGraphSupInc_DefinitionId& theOther) const noexcept
  {
    return Kind == theOther.Kind && Index == theOther.Index;
  }

  //! Compare dense definition ID components for inequality.
  //! @param[in] theOther ID to compare
  //! @return true when the IDs differ
  [[nodiscard]] bool operator!=(const BRepGraphSupInc_DefinitionId& theOther) const noexcept
  {
    return !(*this == theOther);
  }
};

namespace BRepGraphSupInc
{
//! Persistent fields common to every supplemental-store record.
struct BaseDef
{
  //! Nonzero counter that, with the registered store ID and item kind, forms the runtime ItemUID.
  uint32_t UID = 0;

  //! Generation incremented when this record is changed through its store API.
  uint32_t OwnGen = 0;
};
} // namespace BRepGraphSupInc

#endif // _BRepGraphSupInc_Definition_HeaderFile
