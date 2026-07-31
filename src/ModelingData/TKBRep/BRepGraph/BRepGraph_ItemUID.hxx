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

#ifndef _BRepGraph_ItemUID_HeaderFile
#define _BRepGraph_ItemUID_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RefId.hxx>
#include <Standard_Assert.hxx>
#include <Standard_HashUtils.hxx>

#include <cstdint>
#include <cstddef>
#include <functional>
#include <limits>

//! Durable BRepGraph item identity covering definition nodes and reference entries.
//!
//! BRepGraph_ItemId is a transient structural address. BRepGraph_ItemUID is the persistent
//! identity assigned at item creation and kept stable across compaction and vector reordering.
//! Representation/use records are not addressed here because they do not have persisted identity.
class BRepGraph_ItemUID
{
public:
  //! Addressed persistent identity domain.
  enum class Domain : uint8_t
  {
    None,
    Node,
    Reference
  };

  //! Construct an invalid UID.
  BRepGraph_ItemUID() = default;

  //! Construct a node UID.
  static BRepGraph_ItemUID Node(const BRepGraph_NodeId::Kind theKind, const size_t theCounter)
  {
    return BRepGraph_ItemUID(Domain::Node, static_cast<uint8_t>(theKind), theCounter);
  }

  //! Construct a reference UID.
  static BRepGraph_ItemUID Reference(const BRepGraph_RefId::Kind theKind, const size_t theCounter)
  {
    return BRepGraph_ItemUID(Domain::Reference, static_cast<uint8_t>(theKind), theCounter);
  }

  //! Return an invalid sentinel UID.
  static BRepGraph_ItemUID Invalid() { return BRepGraph_ItemUID(); }

  //! Return true if this UID has a non-sentinel counter and a valid domain/kind pair.
  [[nodiscard]] bool IsValid() const noexcept
  {
    if (myCounter == 0)
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

  //! Return the addressed identity domain.
  [[nodiscard]] Domain ItemDomain() const noexcept { return myDomain; }

  [[nodiscard]] bool IsNode() const noexcept { return myDomain == Domain::Node; }

  [[nodiscard]] bool IsReference() const noexcept { return myDomain == Domain::Reference; }

  //! Return node kind. Valid only for node UIDs.
  [[nodiscard]] BRepGraph_NodeId::Kind NodeKind() const noexcept
  {
    Standard_ASSERT_RETURN(
      IsNode() && BRepGraph_NodeId::IsValidKind(static_cast<BRepGraph_NodeId::Kind>(myKind)),
      "BRepGraph_ItemUID::NodeKind(): UID is not a valid node UID",
      BRepGraph_NodeId::Kind::Solid);
    return static_cast<BRepGraph_NodeId::Kind>(myKind);
  }

  //! Return reference kind. Valid only for reference UIDs.
  [[nodiscard]] BRepGraph_RefId::Kind RefKind() const noexcept
  {
    Standard_ASSERT_RETURN(
      IsReference() && BRepGraph_RefId::IsValidKind(static_cast<BRepGraph_RefId::Kind>(myKind)),
      "BRepGraph_ItemUID::RefKind(): UID is not a valid reference UID",
      BRepGraph_RefId::Kind::Shell);
    return static_cast<BRepGraph_RefId::Kind>(myKind);
  }

  //! Return item kind encoded in its own domain enum space.
  [[nodiscard]] uint8_t RawKind() const noexcept { return myKind; }

  //! Return the graph-wide monotonic UID counter.
  [[nodiscard]] size_t Counter() const noexcept { return myCounter; }

  friend bool operator==(const BRepGraph_ItemUID& theLeft,
                         const BRepGraph_ItemUID& theRight) noexcept
  {
    if (theLeft.myCounter == 0 || theRight.myCounter == 0)
    {
      return (theLeft.myCounter == 0) == (theRight.myCounter == 0);
    }
    return theLeft.myDomain == theRight.myDomain && theLeft.myKind == theRight.myKind
           && theLeft.myCounter == theRight.myCounter;
  }

  friend bool operator!=(const BRepGraph_ItemUID& theLeft,
                         const BRepGraph_ItemUID& theRight) noexcept
  {
    return !(theLeft == theRight);
  }

  friend bool operator<(const BRepGraph_ItemUID& theLeft,
                        const BRepGraph_ItemUID& theRight) noexcept
  {
    if (theLeft.myDomain != theRight.myDomain)
    {
      return static_cast<uint8_t>(theLeft.myDomain) < static_cast<uint8_t>(theRight.myDomain);
    }
    if (theLeft.myKind != theRight.myKind)
    {
      return theLeft.myKind < theRight.myKind;
    }
    return theLeft.myCounter < theRight.myCounter;
  }

  //! Compute a hash value compatible with operator==.
  [[nodiscard]] size_t HashValue() const noexcept
  {
    if (myCounter == 0)
    {
      return opencascade::hash(0);
    }

    size_t aCombination[3];
    aCombination[0] = opencascade::hash(static_cast<uint32_t>(myDomain));
    aCombination[1] = opencascade::hash(static_cast<uint32_t>(myKind));
    aCombination[2] = opencascade::hash(myCounter);
    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }

private:
  BRepGraph_ItemUID(const Domain theDomain, const uint8_t theKind, const size_t theCounter)
      : myCounter(0),
        myDomain(theDomain),
        myKind(theKind)
  {
    Standard_ASSERT_VOID(theCounter > 0, "BRepGraph_ItemUID: counter must be > 0 for valid UIDs");
    Standard_ASSERT_VOID(theCounter <= std::numeric_limits<uint32_t>::max(),
                         "BRepGraph_ItemUID: counter exceeds 32-bit storage");
    if (theCounter > 0 && theCounter <= std::numeric_limits<uint32_t>::max())
    {
      myCounter = static_cast<uint32_t>(theCounter);
    }
  }

  uint32_t myCounter = 0;            //!< 0 = invalid sentinel; valid counters start at 1.
  Domain   myDomain  = Domain::None; //!< Identity domain.
  uint8_t  myKind    = 0;            //!< Kind encoded in the selected domain enum space.
};

static_assert(sizeof(BRepGraph_ItemUID) <= 8, "BRepGraph_ItemUID must stay compact");

//! std::hash specialization for NCollection_DefaultHasher support.
template <>
struct std::hash<BRepGraph_ItemUID>
{
  size_t operator()(const BRepGraph_ItemUID& theUID) const noexcept { return theUID.HashValue(); }
};

#endif // _BRepGraph_ItemUID_HeaderFile
