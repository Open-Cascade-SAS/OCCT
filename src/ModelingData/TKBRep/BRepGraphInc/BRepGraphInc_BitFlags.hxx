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

#ifndef _BRepGraphInc_BitFlags_HeaderFile
#define _BRepGraphInc_BitFlags_HeaderFile

#include <NCollection_LinearVector.hxx>

#include <cstdint>
#include <cstddef>

//! @brief Contiguous bit-vector for per-entity boolean flags.
//!
//! Stores one bit per entity index in a flat array of 64-bit blocks.
//! Provides O(1) Set/Clear/Test operations and cache-friendly sequential
//! traversal (512 flags per 64-byte cache line via eight 64-bit blocks).
//!
//! Used by BRepGraphInc_Storage to store IsRemoved and IsOwned flags
//! outside the entity structs, improving cache locality during traversal
//! and reducing struct size by eliminating bool-field padding.
//!
//! Public helpers in BRepGraphInc_Storage validate indices before reaching this
//! low-level container. Set, Clear, and Test remain unchecked for hot internal
//! paths that already proved the index is in range.
//!
//! @code
//!   BRepGraphInc_BitFlags aFlags;
//!   aFlags.Resize(1000);
//!   aFlags.Set(42);
//!   if (aFlags.Test(42)) { ... }
//!   aFlags.Clear(42);
//! @endcode
class BRepGraphInc_BitFlags
{
  static constexpr uint32_t THE_BITS_PER_BLOCK = 64;
  using BlockType                              = uint64_t;

public:
  //! Construct an empty bit-vector.
  BRepGraphInc_BitFlags() = default;

  //! Resize the bit-vector to hold at least theCount bits.
  //! Newly added bits are initialized to false.
  void Resize(const size_t theCount)
  {
    const size_t aBlockCount = (theCount + THE_BITS_PER_BLOCK - 1) / THE_BITS_PER_BLOCK;
    myBlocks.Resize(aBlockCount, 0);
    myBitCount = theCount;
    maskTailBits();
  }

  //! Set the bit at theIndex to true.
  void Set(const uint32_t theIndex)
  {
    const size_t   aBlock = theIndex / THE_BITS_PER_BLOCK;
    const uint32_t aBit   = theIndex % THE_BITS_PER_BLOCK;
    myBlocks[aBlock] |= (BlockType(1) << aBit);
  }

  //! Clear the bit at theIndex to false.
  void Clear(const uint32_t theIndex)
  {
    const size_t   aBlock = theIndex / THE_BITS_PER_BLOCK;
    const uint32_t aBit   = theIndex % THE_BITS_PER_BLOCK;
    myBlocks[aBlock] &= ~(BlockType(1) << aBit);
  }

  //! Return the value of the bit at theIndex.
  [[nodiscard]] bool Test(const uint32_t theIndex) const
  {
    const size_t   aBlock = theIndex / THE_BITS_PER_BLOCK;
    const uint32_t aBit   = theIndex % THE_BITS_PER_BLOCK;
    return (myBlocks[aBlock] & (BlockType(1) << aBit)) != 0;
  }

  //! Set all bits to true.
  void SetAll()
  {
    for (size_t i = 0; i < myBlocks.Size(); ++i)
    {
      myBlocks[i] = ~BlockType(0);
    }
    maskTailBits();
  }

  //! Clear all bits to false.
  void ClearAll()
  {
    for (size_t i = 0; i < myBlocks.Size(); ++i)
    {
      myBlocks[i] = 0;
    }
  }

  //! Return true if any bit is set.
  [[nodiscard]] bool HasAnyBitSet() const
  {
    for (size_t i = 0; i < myBlocks.Size(); ++i)
    {
      if (myBlocks[i] != 0)
      {
        return true;
      }
    }
    return false;
  }

  //! Return the number of blocks allocated.
  [[nodiscard]] size_t NbBlocks() const { return myBlocks.Size(); }

  //! Return the number of valid bits represented by this vector.
  [[nodiscard]] size_t BitCount() const { return myBitCount; }

  //! Return true if theIndex is inside the valid bit range.
  [[nodiscard]] bool IsValidIndex(const uint32_t theIndex) const { return theIndex < myBitCount; }

  //! Return the raw block array for direct iteration.
  [[nodiscard]] const BlockType* Blocks() const { return myBlocks.Data(); }

private:
  void maskTailBits()
  {
    const uint32_t aTailBits = static_cast<uint32_t>(myBitCount % THE_BITS_PER_BLOCK);
    if (aTailBits == 0u || myBlocks.Size() == 0)
    {
      return;
    }

    const BlockType aTailMask = (BlockType(1) << aTailBits) - BlockType(1);
    myBlocks[myBlocks.Size() - 1] &= aTailMask;
  }

  NCollection_LinearVector<BlockType> myBlocks;
  size_t                              myBitCount = 0;
};

#endif // _BRepGraphInc_BitFlags_HeaderFile
