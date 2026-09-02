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

#ifndef _BRepGraph_RevisionMerkle_HeaderFile
#define _BRepGraph_RevisionMerkle_HeaderFile

#include <BRepGraph_RevisionHash.hxx>
#include <NCollection_PersistentRadixMap.hxx>
#include <Standard_Macro.hxx>

#include <cstdint>
#include <utility>

//! Canonical BRepGraph Merkle encoding policy.
//! Persistent radix mechanics live in NCollection; only the graph's stable byte
//! format and digest choice remain in the BRepGraph layer.
class BRepGraph_RevisionMerkleHasher
{
public:
  using ChildHash = std::pair<uint8_t, BRepGraph_RevisionHash>;

  [[nodiscard]] Standard_EXPORT static BRepGraph_RevisionHash EmptyHash();
  [[nodiscard]] Standard_EXPORT static BRepGraph_RevisionHash LeafHash(
    const uint64_t                theKey,
    const BRepGraph_RevisionHash& theValue);
  [[nodiscard]] Standard_EXPORT static BRepGraph_RevisionHash BranchHash(
    const size_t     theDepth,
    const ChildHash* theChildren,
    const size_t     theNbChildren);
};

using BRepGraph_RevisionMerkle = NCollection_PersistentRadixMap<uint64_t,
                                                                BRepGraph_RevisionHash,
                                                                BRepGraph_RevisionHash,
                                                                BRepGraph_RevisionMerkleHasher>;

#endif // _BRepGraph_RevisionMerkle_HeaderFile
