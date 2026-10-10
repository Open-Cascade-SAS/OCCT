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

#ifndef _BRepGraphSupInc_ItemUID_HeaderFile
#define _BRepGraphSupInc_ItemUID_HeaderFile

#include <Standard_HashUtils.hxx>

#include <cstdint>
#include <functional>

//! Active-process identifier of an item held by a supplemental-incidence store.
//! An active item's identifier remains stable through compaction; removal deactivates the UID and
//! clearing a store deactivates its records without reusing their identifiers. It is the stable
//! alternative to a dense item ID while the process remains active. It is not serializable.
struct BRepGraphSupInc_ItemUID
{
  //! Process-unique runtime identifier of the registered store that allocated this item.
  uint32_t StoreId = 0;
  //! Store-defined nonzero item-kind discriminator.
  uint32_t Kind = 0;
  //! Nonzero counter monotonically allocated by the owning store for this kind.
  uint32_t Counter = 0;

  //! Return true when all UID components have valid nonzero values.
  //! This does not verify that the item remains active in a particular storage instance.
  [[nodiscard]] bool IsValid() const noexcept { return StoreId != 0 && Kind != 0 && Counter != 0; }

  //! Return true when this UID has valid component values.
  explicit operator bool() const noexcept { return IsValid(); }

  //! Compare all runtime identifier components.
  //! @param[in] theOther UID to compare
  //! @return true when both UIDs denote the same registered store, kind, and counter
  [[nodiscard]] bool operator==(const BRepGraphSupInc_ItemUID& theOther) const noexcept
  {
    return StoreId == theOther.StoreId && Kind == theOther.Kind && Counter == theOther.Counter;
  }

  //! Compare all runtime identifier components for inequality.
  //! @param[in] theOther UID to compare
  //! @return true when the UIDs differ
  [[nodiscard]] bool operator!=(const BRepGraphSupInc_ItemUID& theOther) const noexcept
  {
    return !(*this == theOther);
  }

  //! Compute a hash from all runtime identifier components.
  [[nodiscard]] size_t HashValue() const noexcept
  {
    const uint32_t aCombination[] = {StoreId, Kind, Counter};
    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }
};

namespace std
{
template <>
struct hash<BRepGraphSupInc_ItemUID>
{
  //! Compute a hash from all runtime identifier components.
  //! @param[in] theUID UID to hash
  //! @return hash value suitable for standard unordered containers
  size_t operator()(const BRepGraphSupInc_ItemUID& theUID) const noexcept
  {
    return theUID.HashValue();
  }
};
} // namespace std

#endif // _BRepGraphSupInc_ItemUID_HeaderFile
