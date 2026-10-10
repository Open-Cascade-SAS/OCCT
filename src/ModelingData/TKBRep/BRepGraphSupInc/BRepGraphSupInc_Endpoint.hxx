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

#ifndef _BRepGraphSupInc_Endpoint_HeaderFile
#define _BRepGraphSupInc_Endpoint_HeaderFile

#include <BRepGraph_ItemId.hxx>
#include <BRepGraphSupInc_ItemUID.hxx>
#include <Standard_HashUtils.hxx>

#include <cstdint>
#include <variant>

//! Compact tagged endpoint used by supplemental relation layers.
//! It stores exactly one core graph item or supplemental item.
class BRepGraphSupInc_Endpoint
{
public:
  //! Domain derived from the active endpoint item.
  enum class Domain : uint8_t
  {
    //! No endpoint item; the endpoint is invalid.
    None,
    //! The endpoint refers to CoreItem().
    CoreItem,
    //! The endpoint refers to SupplementalItem().
    SupplementalItem
  };

  //! Construct an invalid endpoint.
  BRepGraphSupInc_Endpoint() = default;

  //! Create an endpoint for a valid core graph item.
  [[nodiscard]] static BRepGraphSupInc_Endpoint Core(const BRepGraph_ItemId theItem)
  {
    BRepGraphSupInc_Endpoint anEndpoint;
    if (theItem.IsValid())
    {
      anEndpoint.myItem = theItem;
    }
    return anEndpoint;
  }

  //! Create an endpoint for a valid supplemental item.
  [[nodiscard]] static BRepGraphSupInc_Endpoint Supplemental(const BRepGraphSupInc_ItemUID& theItem)
  {
    BRepGraphSupInc_Endpoint anEndpoint;
    if (theItem.IsValid())
    {
      anEndpoint.myItem = theItem;
    }
    return anEndpoint;
  }

  //! Return the active endpoint domain.
  [[nodiscard]] Domain ItemDomain() const noexcept
  {
    if (std::holds_alternative<BRepGraph_ItemId>(myItem))
    {
      return Domain::CoreItem;
    }
    if (std::holds_alternative<BRepGraphSupInc_ItemUID>(myItem))
    {
      return Domain::SupplementalItem;
    }
    return Domain::None;
  }

  //! Return the core item, or null when this is not a core endpoint.
  [[nodiscard]] const BRepGraph_ItemId* CoreItem() const noexcept
  {
    return std::get_if<BRepGraph_ItemId>(&myItem);
  }

  //! Return the supplemental item, or null when this is not a supplemental endpoint.
  [[nodiscard]] const BRepGraphSupInc_ItemUID* SupplementalItem() const noexcept
  {
    return std::get_if<BRepGraphSupInc_ItemUID>(&myItem);
  }

  //! Return true when the selected item is valid for the endpoint domain.
  [[nodiscard]] bool IsValid() const noexcept
  {
    const BRepGraph_ItemId* aCoreItem = CoreItem();
    if (aCoreItem != nullptr)
    {
      return aCoreItem->IsValid();
    }
    const BRepGraphSupInc_ItemUID* aSupplementalItem = SupplementalItem();
    return aSupplementalItem != nullptr && aSupplementalItem->IsValid();
  }

  //! Compare endpoint domains and their active items.
  friend bool operator==(const BRepGraphSupInc_Endpoint& theLeft,
                         const BRepGraphSupInc_Endpoint& theRight) noexcept
  {
    return theLeft.myItem == theRight.myItem;
  }

  //! Compute a hash from the endpoint domain and active item.
  [[nodiscard]] size_t HashValue() const noexcept
  {
    const Domain aDomain = ItemDomain();
    switch (aDomain)
    {
      case Domain::CoreItem: {
        const BRepGraph_ItemId& anItem         = *CoreItem();
        const uint32_t          aCombination[] = {static_cast<uint32_t>(aDomain),
                                                  static_cast<uint32_t>(anItem.ItemDomain()),
                                                  static_cast<uint32_t>(anItem.RawKind()),
                                                  anItem.Index()};
        return opencascade::hashBytes(aCombination, sizeof(aCombination));
      }
      case Domain::SupplementalItem: {
        const BRepGraphSupInc_ItemUID& anItem         = *SupplementalItem();
        const uint32_t                 aCombination[] = {static_cast<uint32_t>(aDomain),
                                                         anItem.StoreId,
                                                         anItem.Kind,
                                                         anItem.Counter};
        return opencascade::hashBytes(aCombination, sizeof(aCombination));
      }
      case Domain::None:
        return opencascade::hash(static_cast<uint32_t>(aDomain));
    }
    return opencascade::hash(static_cast<uint32_t>(Domain::None));
  }

private:
  std::variant<std::monostate, BRepGraph_ItemId, BRepGraphSupInc_ItemUID> myItem;
};

static_assert(sizeof(BRepGraphSupInc_Endpoint) <= 16,
              "BRepGraphSupInc_Endpoint must remain a compact tagged item");

namespace std
{
template <>
struct hash<BRepGraphSupInc_Endpoint>
{
  size_t operator()(const BRepGraphSupInc_Endpoint& theEndpoint) const noexcept
  {
    return theEndpoint.HashValue();
  }
};
} // namespace std

#endif // _BRepGraphSupInc_Endpoint_HeaderFile
