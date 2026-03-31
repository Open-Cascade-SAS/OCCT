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

#include <BRepGraph_RefUID.hxx>
#include <BRepGraph_UID.hxx>
#include <Standard_GUID.hxx>

#include <cstdint>

//! @brief Snapshot of an entity/ref identity and version at a point in time.
//!
//! Combines a persistent UID (entity or reference entry) with
//! OwnGen (own-data version counter) and graph Generation (Build cycle).
//! Computed on demand via BRepGraph::UIDs().StampOf().
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
    None   = 0,
    Entity = 1,
    Ref    = 2
  };

  BRepGraph_UID    myUID;         //!< Entity identity for entity-domain stamps.
  BRepGraph_RefUID myRefUID;      //!< Reference identity for ref-domain stamps.
  uint32_t         myMutationGen; //!< OwnGen counter at snapshot time (maps to BaseDef::OwnGen / BaseRef::OwnGen).
  uint32_t         myGeneration;  //!< Graph Build() generation at snapshot time.
  Domain           myDomain;      //!< Active identity domain.

  //! Default constructor. Creates an invalid stamp (invalid UID, zero counters).
  BRepGraph_VersionStamp()
      : myUID(),
        myRefUID(),
        myMutationGen(0),
        myGeneration(0),
        myDomain(Domain::None)
  {
  }

  //! Construct an entity-domain stamp from components.
  //! @param[in] theUID        persistent entity identity
  //! @param[in] theMutationGen OwnGen counter (own-data mutation counter)
  //! @param[in] theGeneration  graph Build() generation
  BRepGraph_VersionStamp(const BRepGraph_UID& theUID,
                         const uint32_t       theMutationGen,
                         const uint32_t       theGeneration)
      : myUID(theUID),
        myRefUID(),
        myMutationGen(theMutationGen),
        myGeneration(theGeneration),
        myDomain(Domain::Entity)
  {
  }

  //! Construct a reference-domain stamp from components.
  //! @param[in] theRefUID      persistent reference identity
  //! @param[in] theMutationGen OwnGen counter (own-data mutation counter)
  //! @param[in] theGeneration  graph Build() generation
  BRepGraph_VersionStamp(const BRepGraph_RefUID& theRefUID,
                         const uint32_t          theMutationGen,
                         const uint32_t          theGeneration)
      : myUID(),
        myRefUID(theRefUID),
        myMutationGen(theMutationGen),
        myGeneration(theGeneration),
        myDomain(Domain::Ref)
  {
  }

  //! Check if the stamp has a valid identity in its domain.
  [[nodiscard]] bool IsValid() const
  {
    if (myDomain == Domain::Entity)
      return myUID.IsValid();
    if (myDomain == Domain::Ref)
      return myRefUID.IsValid();
    return myUID.IsValid() || myRefUID.IsValid();
  }

  //! True when this is an entity-domain stamp.
  [[nodiscard]] bool IsEntityStamp() const
  {
    if (myDomain == Domain::Entity)
      return myUID.IsValid();
    return myDomain == Domain::None && myUID.IsValid() && !myRefUID.IsValid();
  }

  //! True when this is a reference-domain stamp.
  [[nodiscard]] bool IsRefStamp() const
  {
    if (myDomain == Domain::Ref)
      return myRefUID.IsValid();
    return myDomain == Domain::None && myRefUID.IsValid() && !myUID.IsValid();
  }

  //! Full equality: same domain, UID, OwnGen, and Generation.
  //! Two invalid stamps are equal.
  bool operator==(const BRepGraph_VersionStamp& theOther) const
  {
    if (!IsValid() && !theOther.IsValid())
      return true;
    if (myDomain != theOther.myDomain)
      return false;
    if (myMutationGen != theOther.myMutationGen || myGeneration != theOther.myGeneration)
      return false;
    if (myDomain == Domain::Entity)
      return myUID == theOther.myUID;
    if (myDomain == Domain::Ref)
      return myRefUID == theOther.myRefUID;
    return myUID == theOther.myUID && myRefUID == theOther.myRefUID;
  }

  bool operator!=(const BRepGraph_VersionStamp& theOther) const { return !(*this == theOther); }

  //! Check if two stamps refer to the same entity/reference regardless of version.
  //! Compares active UID only, ignoring OwnGen and Generation.
  //! @param[in] theOther stamp to compare with
  //! @return true if both stamps have the same domain and UID
  [[nodiscard]] bool IsSameNode(const BRepGraph_VersionStamp& theOther) const
  {
    if (myDomain != theOther.myDomain)
      return false;
    if (myDomain == Domain::Entity)
      return myUID == theOther.myUID;
    if (myDomain == Domain::Ref)
      return myRefUID == theOther.myRefUID;
    return myUID == theOther.myUID && myRefUID == theOther.myRefUID;
  }

  //! Derive a deterministic Standard_GUID from this stamp.
  //! The graph GUID is incorporated into the hash, making per-node GUIDs
  //! globally unique across different graph instances.
  //! One-way: cannot reconstruct stamp fields from the resulting GUID.
  //! @param[in] theGraphGUID the owning graph's identity GUID
  //! @return deterministic Standard_GUID derived from stamp + graph GUID
  [[nodiscard]] Standard_EXPORT Standard_GUID ToGUID(const Standard_GUID& theGraphGUID) const;

  //! Compute hash value consistent with operator==.
  //! @return hash combining active UID, domain, OwnGen, and Generation
  [[nodiscard]] size_t HashValue() const
  {
    size_t aCombination[4];
    aCombination[0] = opencascade::hash(static_cast<int>(myDomain));
    if (myDomain == Domain::Entity)
      aCombination[1] = myUID.HashValue();
    else if (myDomain == Domain::Ref)
      aCombination[1] = myRefUID.HashValue();
    else
      aCombination[1] = opencascade::hash(0);
    aCombination[2] = opencascade::hash(myMutationGen);
    aCombination[3] = opencascade::hash(myGeneration);
    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }
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
