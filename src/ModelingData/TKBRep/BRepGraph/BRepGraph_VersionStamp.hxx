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

#include <BRepGraph_UID.hxx>
#include <Standard_GUID.hxx>

#include <cstdint>

//! @brief Snapshot of a node's identity and version at a point in time.
//!
//! Combines a persistent UID (stable identity = Kind + Counter) with
//! per-node MutationGen (version counter) and graph Generation (Build cycle).
//! Computed on demand via BRepGraph::UIDs().StampOf(), never stored on entities.
//!
//! Usage pattern:
//! @code
//!   BRepGraph_VersionStamp aStamp = aGraph.UIDs().StampOf(aFaceId);
//!   // ... later, after mutations ...
//!   if (aGraph.UIDs().IsStale(aStamp))
//!     recomputeDerivedData();
//! @endcode
//!
//! Trivially copyable, 32 bytes, cheap to pass by value.
struct BRepGraph_VersionStamp
{
  BRepGraph_UID myUID;         //!< Stable persistent identity (Kind, Counter).
  uint32_t      myMutationGen; //!< Per-node mutation counter at snapshot time.
  uint32_t      myGeneration;  //!< Graph Build() generation at snapshot time.

  //! Default constructor. Creates an invalid stamp (invalid UID, zero counters).
  BRepGraph_VersionStamp()
      : myUID(),
        myMutationGen(0),
        myGeneration(0)
  {
  }

  //! Construct a stamp from components.
  //! @param[in] theUID        persistent node identity
  //! @param[in] theMutationGen per-node mutation counter
  //! @param[in] theGeneration  graph Build() generation
  BRepGraph_VersionStamp(const BRepGraph_UID& theUID,
                         const uint32_t       theMutationGen,
                         const uint32_t       theGeneration)
      : myUID(theUID),
        myMutationGen(theMutationGen),
        myGeneration(theGeneration)
  {
  }

  //! Check if the stamp has a valid UID.
  //! Invalidity is determined solely by UID (counter = 0 is invalid sentinel).
  //! @return true if the UID is valid
  [[nodiscard]] bool IsValid() const { return myUID.IsValid(); }

  //! Full equality: same UID, MutationGen, and Generation.
  //! Two invalid stamps are equal.
  bool operator==(const BRepGraph_VersionStamp& theOther) const
  {
    return myUID == theOther.myUID && myMutationGen == theOther.myMutationGen
           && myGeneration == theOther.myGeneration;
  }

  bool operator!=(const BRepGraph_VersionStamp& theOther) const { return !(*this == theOther); }

  //! Check if two stamps refer to the same node regardless of version.
  //! Compares UID only, ignoring MutationGen and Generation.
  //! @param[in] theOther stamp to compare with
  //! @return true if both stamps have the same UID
  [[nodiscard]] bool IsSameNode(const BRepGraph_VersionStamp& theOther) const
  {
    return myUID == theOther.myUID;
  }

  //! Derive a deterministic Standard_GUID from this stamp.
  //! The graph GUID is incorporated into the hash, making per-node GUIDs
  //! globally unique across different graph instances.
  //! One-way: cannot reconstruct stamp fields from the resulting GUID.
  //! @param[in] theGraphGUID the owning graph's identity GUID
  //! @return deterministic Standard_GUID derived from stamp + graph GUID
  [[nodiscard]] Standard_EXPORT Standard_GUID ToGUID(const Standard_GUID& theGraphGUID) const;

  //! Compute hash value consistent with operator==.
  //! @return hash combining UID, MutationGen, and Generation
  [[nodiscard]] size_t HashValue() const
  {
    size_t aCombination[3];
    aCombination[0] = myUID.HashValue();
    aCombination[1] = opencascade::hash(myMutationGen);
    aCombination[2] = opencascade::hash(myGeneration);
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
