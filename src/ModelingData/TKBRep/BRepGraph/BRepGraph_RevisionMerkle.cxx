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

#include <BRepGraph_RevisionMerkle.hxx>

#include <array>

namespace
{
constexpr uint32_t THE_FORMAT_VERSION = 1;
constexpr size_t   THE_MAX_INPUT_SIZE = 4 + 4 + 1 + 2 + 256 * (1 + sizeof(BRepGraph_RevisionHash));

struct HashInput
{
  void AppendByte(const uint8_t theValue) { Bytes[Size++] = theValue; }

  void AppendUInt16(const uint16_t theValue)
  {
    AppendByte(static_cast<uint8_t>(theValue));
    AppendByte(static_cast<uint8_t>(theValue >> 8));
  }

  void AppendUInt32(const uint32_t theValue)
  {
    for (int aByte = 0; aByte < 4; ++aByte)
    {
      AppendByte(static_cast<uint8_t>(theValue >> (aByte * 8)));
    }
  }

  void AppendUInt64(const uint64_t theValue)
  {
    for (int aByte = 0; aByte < 8; ++aByte)
    {
      AppendByte(static_cast<uint8_t>(theValue >> (aByte * 8)));
    }
  }

  void AppendHash(const BRepGraph_RevisionHash& theHash)
  {
    for (const uint64_t aWord : theHash.Words)
    {
      AppendUInt64(aWord);
    }
  }

  BRepGraph_RevisionHash Hash() const
  {
    return BRepGraph_RevisionHash::Hasher::Bytes(Bytes.data(), Size);
  }

  std::array<uint8_t, THE_MAX_INPUT_SIZE> Bytes;
  size_t                                  Size = 0;
};
} // namespace

BRepGraph_RevisionHash BRepGraph_RevisionMerkleHasher::EmptyHash()
{
  HashInput anInput;
  anInput.AppendUInt32(0x4d454d50u); // MEMP
  anInput.AppendUInt32(THE_FORMAT_VERSION);
  return anInput.Hash();
}

BRepGraph_RevisionHash BRepGraph_RevisionMerkleHasher::LeafHash(
  const uint64_t                theKey,
  const BRepGraph_RevisionHash& theValue)
{
  HashInput anInput;
  anInput.AppendUInt32(0x4d4c4546u); // MLEF
  anInput.AppendUInt32(THE_FORMAT_VERSION);
  anInput.AppendUInt64(theKey);
  anInput.AppendHash(theValue);
  return anInput.Hash();
}

BRepGraph_RevisionHash BRepGraph_RevisionMerkleHasher::BranchHash(
  const size_t     theDepth,
  const ChildHash* theChildren,
  const size_t     theNbChildren)
{
  HashInput anInput;
  anInput.AppendUInt32(0x4d42524eu); // MBRN
  anInput.AppendUInt32(THE_FORMAT_VERSION);
  anInput.AppendByte(static_cast<uint8_t>(theDepth));
  anInput.AppendUInt16(static_cast<uint16_t>(theNbChildren));
  for (size_t anIndex = 0; anIndex < theNbChildren; ++anIndex)
  {
    const auto& [aSlot, aHash] = theChildren[anIndex];
    anInput.AppendByte(aSlot);
    anInput.AppendHash(aHash);
  }
  return anInput.Hash();
}
