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

#ifndef _BRepGraph_ItemId_HeaderFile
#define _BRepGraph_ItemId_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RefId.hxx>
#include <Standard_Assert.hxx>
#include <Standard_HashUtils.hxx>
#include <Standard_TypeDef.hxx>

#include <cstdint>
#include <functional>
#include <limits>

//! Generic BRepGraph item identifier covering definitions and references.
//! Use-records are NOT included - they are session-local, not graph identity.
class BRepGraph_ItemId
{
public:
  //! Addressed graph item domain.
  enum class Domain : uint8_t
  {
    None,
    Node,
    Reference
  };

  //! Construct an invalid item id.
  BRepGraph_ItemId() = default;

  //! Construct a node item id.
  BRepGraph_ItemId(const BRepGraph_NodeId theNode)
  {
    if (theNode.IsValid())
    {
      myDomain = Domain::Node;
      myIndex  = theNode.Index;
      myKind   = static_cast<uint8_t>(theNode.NodeKind);
    }
  }

  //! Construct a reference item id.
  BRepGraph_ItemId(const BRepGraph_RefId theRef)
  {
    if (theRef.IsValid())
    {
      myDomain = Domain::Reference;
      myIndex  = theRef.Index;
      myKind   = static_cast<uint8_t>(theRef.RefKind);
    }
  }

  //! Return true if this item addresses a graph object.
  [[nodiscard]] bool IsValid() const noexcept
  {
    if (myIndex == THE_INVALID_INDEX)
    {
      return false;
    }

    switch (myDomain)
    {
      case Domain::Node:
        return BRepGraph_NodeId::IsValidKind(static_cast<BRepGraph_NodeId::Kind>(myKind));
      case Domain::Reference:
        return BRepGraph_RefId::IsValidKind(static_cast<BRepGraph_RefId::Kind>(myKind));
      case Domain::None:
        return false;
    }
    return false;
  }

  //! Return the addressed domain.
  [[nodiscard]] Domain ItemDomain() const noexcept { return myDomain; }

  //! Return true if this item addresses a definition node.
  [[nodiscard]] bool IsNode() const noexcept { return myDomain == Domain::Node; }

  //! Return true if this item addresses a reference entry.
  [[nodiscard]] bool IsReference() const noexcept { return myDomain == Domain::Reference; }

  //! Convert to node id. Returns invalid id for non-node items.
  [[nodiscard]] BRepGraph_NodeId NodeId() const noexcept
  {
    return IsNode() && BRepGraph_NodeId::IsValidKind(static_cast<BRepGraph_NodeId::Kind>(myKind))
             ? BRepGraph_NodeId(static_cast<BRepGraph_NodeId::Kind>(myKind), myIndex)
             : BRepGraph_NodeId();
  }

  //! Convert to reference id. Returns invalid id for non-reference items.
  [[nodiscard]] BRepGraph_RefId RefId() const noexcept
  {
    return IsReference() && BRepGraph_RefId::IsValidKind(static_cast<BRepGraph_RefId::Kind>(myKind))
             ? BRepGraph_RefId(static_cast<BRepGraph_RefId::Kind>(myKind), myIndex)
             : BRepGraph_RefId();
  }

  //! Return node kind. Valid only when IsNode() is true.
  [[nodiscard]] BRepGraph_NodeId::Kind NodeKind() const noexcept
  {
    Standard_ASSERT_RETURN(
      IsNode() && BRepGraph_NodeId::IsValidKind(static_cast<BRepGraph_NodeId::Kind>(myKind)),
      "BRepGraph_ItemId::NodeKind(): item is not a valid node",
      BRepGraph_NodeId::Kind::Solid);
    return static_cast<BRepGraph_NodeId::Kind>(myKind);
  }

  //! Return reference kind. Valid only when IsReference() is true.
  [[nodiscard]] BRepGraph_RefId::Kind RefKind() const noexcept
  {
    Standard_ASSERT_RETURN(
      IsReference() && BRepGraph_RefId::IsValidKind(static_cast<BRepGraph_RefId::Kind>(myKind)),
      "BRepGraph_ItemId::RefKind(): item is not a valid reference",
      BRepGraph_RefId::Kind::Shell);
    return static_cast<BRepGraph_RefId::Kind>(myKind);
  }

  //! Return item kind encoded in its own domain enum space.
  [[nodiscard]] uint8_t RawKind() const noexcept { return myKind; }

  //! Return item kind encoded in its own domain enum space.
  [[nodiscard]] uint8_t Kind() const noexcept { return RawKind(); }

  //! Return item per-kind index.
  [[nodiscard]] uint32_t Index() const noexcept { return myIndex; }

  friend bool operator==(const BRepGraph_ItemId& theLeft, const BRepGraph_ItemId& theRight) noexcept
  {
    return theLeft.myDomain == theRight.myDomain && theLeft.myKind == theRight.myKind
           && theLeft.myIndex == theRight.myIndex;
  }

  friend bool operator!=(const BRepGraph_ItemId& theLeft, const BRepGraph_ItemId& theRight) noexcept
  {
    return !(theLeft == theRight);
  }

private:
  static constexpr uint32_t THE_INVALID_INDEX = std::numeric_limits<uint32_t>::max();

  Domain   myDomain = Domain::None;
  uint32_t myIndex  = THE_INVALID_INDEX;
  uint8_t  myKind   = 0;
};

//! std::hash specialization for BRepGraph_ItemId.
template <>
struct std::hash<BRepGraph_ItemId>
{
  size_t operator()(const BRepGraph_ItemId& theId) const noexcept
  {
    size_t aCombination[3];
    aCombination[0] = opencascade::hash(static_cast<uint32_t>(theId.ItemDomain()));
    aCombination[1] = opencascade::hash(static_cast<uint32_t>(theId.RawKind()));
    aCombination[2] = opencascade::hash(theId.Index());
    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }
};

#endif // _BRepGraph_ItemId_HeaderFile
