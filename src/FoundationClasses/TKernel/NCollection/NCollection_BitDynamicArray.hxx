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

#ifndef _NCollection_BitDynamicArray_HeaderFile
#define _NCollection_BitDynamicArray_HeaderFile

#include <NCollection_PagedArray.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cstddef>
#include <cstdint>

//! @brief Dynamic bit array with page-granular copy-on-write storage.
//!
//! Stores one bit per entity index in a flat array of 64-bit blocks.
//! Provides O(1) Set/Clear/Test operations and cache-friendly sequential
//! traversal (512 flags per 64-byte cache line via eight 64-bit blocks).
//!
//! Copying retains shared immutable pages. Mutating either copy detaches only
//! the touched page, so snapshots are cheap and concurrent const access to
//! retained copies is safe. Simultaneous mutation of one object requires
//! external synchronization.
//!
//! Set, Clear, and Test are unchecked for hot paths that already validated the
//! index. Use IsValidIndex() at API boundaries.
//!
//! @code
//!   NCollection_BitDynamicArray aFlags;
//!   aFlags.Resize(1000);
//!   aFlags.Set(42);
//!   if (aFlags.Test(42)) { ... }
//!   aFlags.Clear(42);
//! @endcode
class NCollection_BitDynamicArray
{
  static constexpr size_t THE_BITS_PER_BLOCK = 64;
  using BlockType                            = uint64_t;

public:
  DEFINE_STANDARD_ALLOC

  //! Construct an empty bit array.
  NCollection_BitDynamicArray() = default;

  //! Resize the bit array to contain exactly theCount bits.
  //! @param[in] theCount number of bits after resizing
  //! Newly added bits are initialized to false.
  void Resize(const size_t theCount)
  {
    const size_t aBlockCount =
      theCount / THE_BITS_PER_BLOCK + (theCount % THE_BITS_PER_BLOCK != 0 ? 1 : 0);
    myBlocks.Resize(aBlockCount, 0);
    myBitCount = theCount;
    maskTailBits();
  }

  //! Set the bit at theIndex to true.
  //! @param[in] theIndex zero-based bit index
  void Set(const size_t theIndex)
  {
    const size_t aBlock = theIndex / THE_BITS_PER_BLOCK;
    const size_t aBit   = theIndex % THE_BITS_PER_BLOCK;
    myBlocks[aBlock] |= (BlockType(1) << aBit);
  }

  //! Clear the bit at theIndex to false.
  //! @param[in] theIndex zero-based bit index
  void Clear(const size_t theIndex)
  {
    const size_t aBlock = theIndex / THE_BITS_PER_BLOCK;
    const size_t aBit   = theIndex % THE_BITS_PER_BLOCK;
    myBlocks[aBlock] &= ~(BlockType(1) << aBit);
  }

  //! Return the value of the bit at theIndex.
  //! @param[in] theIndex zero-based bit index
  [[nodiscard]] bool Test(const size_t theIndex) const noexcept
  {
    const size_t aBlock = theIndex / THE_BITS_PER_BLOCK;
    const size_t aBit   = theIndex % THE_BITS_PER_BLOCK;
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
  [[nodiscard]] size_t NbBlocks() const noexcept { return myBlocks.Size(); }

  //! Return the number of valid bits represented by this array.
  [[nodiscard]] size_t Size() const noexcept { return myBitCount; }

  //! Return true if theIndex is inside the valid bit range.
  //! @param[in] theIndex zero-based bit index
  [[nodiscard]] bool IsValidIndex(const size_t theIndex) const noexcept
  {
    return theIndex < myBitCount;
  }

  //! Return one raw block for deterministic serialization or hashing.
  [[nodiscard]] BlockType Block(const size_t theIndex) const noexcept
  {
    return myBlocks.Value(theIndex);
  }

private:
  void maskTailBits()
  {
    const size_t aTailBits = myBitCount % THE_BITS_PER_BLOCK;
    if (aTailBits == 0u || myBlocks.Size() == 0)
    {
      return;
    }

    const BlockType aTailMask = (BlockType(1) << aTailBits) - BlockType(1);
    const size_t    aLastBlock = myBlocks.Size() - 1;
    const BlockType aValue     = myBlocks.Value(aLastBlock);
    if ((aValue & ~aTailMask) != 0)
    {
      myBlocks[aLastBlock] = aValue & aTailMask;
    }
  }

  NCollection_PagedArray<BlockType> myBlocks{64};
  size_t                            myBitCount = 0;
};

#endif // _NCollection_BitDynamicArray_HeaderFile
