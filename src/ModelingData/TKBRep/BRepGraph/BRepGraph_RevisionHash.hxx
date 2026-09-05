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

#ifndef _BRepGraph_RevisionHash_HeaderFile
#define _BRepGraph_RevisionHash_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RefId.hxx>
#include <BRepGraph_RefUID.hxx>
#include <BRepGraph_UID.hxx>
#include <Geom_Curve.hxx>
#include <NCollection_LinearVector.hxx>
#include <Standard_HashUtils.hxx>
#include <TCollection_AsciiString.hxx>

#include <cstdint>
#include <memory>
#include <optional>

class BRepGraph;

//! Fixed-width SHA-256 value and hashing helpers used by BRepGraph revisions.
//!
//! The four words are deliberately independent of pointer size and object
//! addresses.  Semantic hashes use canonical durable identities; storage hashes
//! additionally include physical local-address information.
class BRepGraph_RevisionHash
{
public:
  //! Canonical graph hashing helpers used by roots and root-to-root diffs.
  class Hasher
  {
  public:
    class ByteAccumulator;
    class Index;
    struct Result;

    //! Compute semantic and storage roots together without repeating leaf hashing.
    [[nodiscard]] Standard_EXPORT static Result Compute(const BRepGraph& theGraph);

    //! Path-copy the base hash index for a bounded set of changed durable records.
    [[nodiscard]] Standard_EXPORT static Result Update(
      const std::shared_ptr<const Index>&               theBaseIndex,
      const BRepGraph&                                  theBaseGraph,
      const BRepGraph&                                  theResultGraph,
      const NCollection_LinearVector<BRepGraph_UID>&    theChangedNodes,
      const NCollection_LinearVector<BRepGraph_RefUID>& theChangedRefs);

    //! Compute the core semantic hash independent of local IDs and physical storage layout.
    //! BRepGraph_Revision combines this value with captured persistent-component hashes.
    [[nodiscard]] Standard_EXPORT static BRepGraph_RevisionHash Semantic(const BRepGraph& theGraph);

    //! Compute a physical storage hash including local IDs, removed slots, and allocation
    //! watermarks.
    [[nodiscard]] Standard_EXPORT static BRepGraph_RevisionHash Storage(const BRepGraph& theGraph);

    //! Compute a durable-keyed signature for one active definition.
    [[nodiscard]] Standard_EXPORT static BRepGraph_RevisionHash Node(
      const BRepGraph&       theGraph,
      const BRepGraph_NodeId theNode);

    //! Compute a durable-keyed signature for one active reference entry.
    [[nodiscard]] Standard_EXPORT static BRepGraph_RevisionHash Reference(
      const BRepGraph&      theGraph,
      const BRepGraph_RefId theRef);

    //! Compute the canonical complete-content hash of a 3D curve.
    [[nodiscard]] Standard_EXPORT static BRepGraph_RevisionHash Curve(
      const occ::handle<Geom_Curve>& theCurve);

    //! Compute a fixed-width content hash over a byte sequence.
    [[nodiscard]] Standard_EXPORT static BRepGraph_RevisionHash Bytes(const void*  theData,
                                                                      const size_t theSize);

    Hasher() = delete;
  };

  uint64_t Words[4] = {0, 0, 0, 0};

  [[nodiscard]] bool IsNull() const noexcept
  {
    return Words[0] == 0 && Words[1] == 0 && Words[2] == 0 && Words[3] == 0;
  }

  [[nodiscard]] size_t HashValue() const noexcept
  {
    return opencascade::hashBytes(Words, sizeof(Words));
  }

  [[nodiscard]] Standard_EXPORT TCollection_AsciiString ToString() const;

  //! Parse the canonical 64-character hexadecimal representation.
  [[nodiscard]] Standard_EXPORT static bool Parse(const TCollection_AsciiString& theText,
                                                  BRepGraph_RevisionHash&        theHash);

  friend bool operator==(const BRepGraph_RevisionHash& theLeft,
                         const BRepGraph_RevisionHash& theRight) noexcept
  {
    return theLeft.Words[0] == theRight.Words[0] && theLeft.Words[1] == theRight.Words[1]
           && theLeft.Words[2] == theRight.Words[2] && theLeft.Words[3] == theRight.Words[3];
  }

  friend bool operator!=(const BRepGraph_RevisionHash& theLeft,
                         const BRepGraph_RevisionHash& theRight) noexcept
  {
    return !(theLeft == theRight);
  }
};

class BRepGraph_RevisionHash::Hasher::ByteAccumulator
{
public:
  Standard_EXPORT explicit ByteAccumulator(const size_t theSize);
  Standard_EXPORT ~ByteAccumulator();

  ByteAccumulator(const ByteAccumulator&)            = delete;
  ByteAccumulator& operator=(const ByteAccumulator&) = delete;

  [[nodiscard]] Standard_EXPORT bool Append(const void* theData, const size_t theSize);
  [[nodiscard]] Standard_EXPORT std::optional<BRepGraph_RevisionHash> Finish();

private:
  class Data;
  std::unique_ptr<Data> myData;
};

//! Core graph hashes and the persistent Merkle indexes used to update them.
struct BRepGraph_RevisionHash::Hasher::Result
{
  BRepGraph_RevisionHash       Semantic;
  BRepGraph_RevisionHash       Storage;
  std::shared_ptr<const Index> HashIndex;
};

#endif // _BRepGraph_RevisionHash_HeaderFile
