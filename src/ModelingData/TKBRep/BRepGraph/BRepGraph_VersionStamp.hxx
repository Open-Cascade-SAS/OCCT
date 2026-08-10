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

#ifndef _BRepGraph_VersionStamp_HeaderFile
#define _BRepGraph_VersionStamp_HeaderFile

#include <BRepGraph_ItemUID.hxx>
#include <BRepGraph_RefUID.hxx>
#include <BRepGraph_UID.hxx>
#include <Standard_GUID.hxx>

#include <cstdint>

//! @brief Snapshot of a graph item identity and its freshness generation.
//!
//! Combines a persistent node or reference UID with OwnGen (own-data mutation counter)
//! and graph Generation (BRepGraph::Clear() cycle). It is intended for custom cache and
//! layer freshness checks, not as a separate topology identity model.
//!
//! Usage pattern:
//! @code
//!   BRepGraph_VersionStamp aStamp = aGraph.UIDs().StampOf(aFaceId);
//!   // ... later, after mutations ...
//!   if (aGraph.UIDs().IsStale(aStamp))
//!     recomputeDerivedData();
//! @endcode
//!
struct BRepGraph_VersionStamp
{
  //! Identity domain encoded in this stamp.
  enum class Domain : uint8_t
  {
    None      = 0,
    Node      = 1,
    Reference = 2
  };

  BRepGraph_UID    myNodeUID;     //!< Definition-node identity for node-domain stamps.
  BRepGraph_RefUID myRefUID;      //!< Reference-entry identity for reference-domain stamps.
  uint32_t         myMutationGen; //!< OwnGen counter at snapshot time.
  uint32_t         myGeneration;  //!< Graph BRepGraph::Clear() generation at snapshot time.
  Domain           myDomain;      //!< Active identity domain.

  //! Default constructor. Creates an invalid stamp (invalid UID, zero counters).
  BRepGraph_VersionStamp()
      : myMutationGen(0),
        myGeneration(0),
        myDomain(Domain::None)
  {
  }

  //! Construct a node-domain stamp from components.
  //! @param[in] theUID         persistent definition-node identity
  //! @param[in] theMutationGen OwnGen counter (own-data mutation counter)
  //! @param[in] theGeneration  graph BRepGraph::Clear() generation
  BRepGraph_VersionStamp(const BRepGraph_UID& theUID,
                         const uint32_t       theMutationGen,
                         const uint32_t       theGeneration)
      : myNodeUID(theUID),
        myMutationGen(theMutationGen),
        myGeneration(theGeneration),
        myDomain(Domain::Node)
  {
  }

  //! Construct a reference-domain stamp from components.
  //! @param[in] theRefUID      persistent reference identity
  //! @param[in] theMutationGen OwnGen counter (own-data mutation counter)
  //! @param[in] theGeneration  graph BRepGraph::Clear() generation
  BRepGraph_VersionStamp(const BRepGraph_RefUID& theRefUID,
                         const uint32_t          theMutationGen,
                         const uint32_t          theGeneration)
      : myRefUID(theRefUID),
        myMutationGen(theMutationGen),
        myGeneration(theGeneration),
        myDomain(Domain::Reference)
  {
  }

  //! Check if the stamp has a valid identity in its domain.
  [[nodiscard]] bool IsValid() const
  {
    if (myDomain == Domain::Node)
    {
      return myNodeUID.IsValid();
    }
    if (myDomain == Domain::Reference)
    {
      return myRefUID.IsValid();
    }
    return myNodeUID.IsValid() || myRefUID.IsValid();
  }

  //! True when this is a definition-node-domain stamp.
  [[nodiscard]] bool IsNodeStamp() const
  {
    if (myDomain == Domain::Node)
    {
      return myNodeUID.IsValid();
    }
    return myDomain == Domain::None && myNodeUID.IsValid() && !myRefUID.IsValid();
  }

  //! True when this is a reference-domain stamp.
  [[nodiscard]] bool IsRefStamp() const
  {
    if (myDomain == Domain::Reference)
    {
      return myRefUID.IsValid();
    }
    return myDomain == Domain::None && myRefUID.IsValid() && !myNodeUID.IsValid();
  }

  //! Return the active generic item identity.
  [[nodiscard]] BRepGraph_ItemUID ItemUID() const
  {
    if (myDomain == Domain::Node)
    {
      return BRepGraph_ItemUID::Node(myNodeUID.Kind, myNodeUID.Counter);
    }
    if (myDomain == Domain::Reference)
    {
      return BRepGraph_ItemUID::Reference(myRefUID.Kind, myRefUID.Counter);
    }
    return BRepGraph_ItemUID();
  }

  //! Full equality: same domain, UID, OwnGen, and Generation.
  //! Two invalid stamps are equal.
  Standard_EXPORT bool operator==(const BRepGraph_VersionStamp& theOther) const;

  bool operator!=(const BRepGraph_VersionStamp& theOther) const { return !(*this == theOther); }

  //! Check if two stamps refer to the same graph item regardless of version.
  //! Compares active UID only, ignoring OwnGen and Generation.
  //! @param[in] theOther stamp to compare with
  //! @return true if both stamps have the same domain and UID
  [[nodiscard]] Standard_EXPORT bool IsSameItem(const BRepGraph_VersionStamp& theOther) const;

  //! Derive a deterministic Standard_GUID from this stamp.
  //! The graph GUID is incorporated into the hash, making per-node GUIDs
  //! globally unique across different graph instances.
  //! One-way: cannot reconstruct stamp fields from the resulting GUID.
  //! @param[in] theGraphGUID the owning graph's identity GUID
  //! @return deterministic Standard_GUID derived from stamp + graph GUID
  [[nodiscard]] Standard_EXPORT Standard_GUID ToGUID(const Standard_GUID& theGraphGUID) const;

  //! Compute hash value consistent with operator==.
  //! @return hash combining active UID, domain, OwnGen, and Generation
  [[nodiscard]] Standard_EXPORT size_t HashValue() const;
};

//! std::hash specialization for NCollection_DefaultHasher support.
template <>
struct std::hash<BRepGraph_VersionStamp>
{
  size_t operator()(const BRepGraph_VersionStamp& theStamp) const noexcept
  {
    return theStamp.HashValue();
  }
};

#endif // _BRepGraph_VersionStamp_HeaderFile
