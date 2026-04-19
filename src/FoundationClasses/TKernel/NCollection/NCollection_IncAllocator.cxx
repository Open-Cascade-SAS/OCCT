// Copyright (c) 2002-2023 OPEN CASCADE SAS
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

#include <NCollection_IncAllocator.hxx>

#include <Standard_OutOfMemory.hxx>

#include <cmath>

IMPLEMENT_STANDARD_RTTIEXT(NCollection_IncAllocator, NCollection_BaseAllocator)

namespace
{
// Bounds for checking block size level
// clang-format off
  static constexpr unsigned THE_SMALL_BOUND_BLOCK_SIZE = NCollection_IncAllocator::THE_DEFAULT_BLOCK_SIZE * 16;   // 196 KB
  static constexpr unsigned THE_MEDIUM_BOUND_BLOCK_SIZE = NCollection_IncAllocator::THE_DEFAULT_BLOCK_SIZE * 64;  // 786 KB
  static constexpr unsigned THE_LARGE_BOUND_BLOCK_SIZE = NCollection_IncAllocator::THE_DEFAULT_BLOCK_SIZE * 1024; // 12 MB
// clang-format on

//=================================================================================================

NCollection_IncAllocator::IBlockSizeLevel computeLevel(const unsigned int theSize)
{
  if (theSize < NCollection_IncAllocator::THE_DEFAULT_BLOCK_SIZE)
  {
    return NCollection_IncAllocator::IBlockSizeLevel::Min;
  }
  else if (theSize < THE_SMALL_BOUND_BLOCK_SIZE)
  {
    return NCollection_IncAllocator::IBlockSizeLevel::Small;
  }
  else if (theSize < THE_MEDIUM_BOUND_BLOCK_SIZE)
  {
    return NCollection_IncAllocator::IBlockSizeLevel::Medium;
  }
  else if (theSize < THE_LARGE_BOUND_BLOCK_SIZE)
  {
    return NCollection_IncAllocator::IBlockSizeLevel::Large;
  }
  else
  {
    return NCollection_IncAllocator::IBlockSizeLevel::Max;
  }
}
} // namespace

//=================================================================================================

NCollection_IncAllocator::NCollection_IncAllocator(const size_t theDefaultSize)
    : myBlockSize(static_cast<unsigned>(
        theDefaultSize < THE_MINIMUM_BLOCK_SIZE ? THE_DEFAULT_BLOCK_SIZE : theDefaultSize))
{
}

//=================================================================================================

void NCollection_IncAllocator::SetThreadSafe(const bool theIsThreadSafe)
{
  if (theIsThreadSafe)
  {
    if (!myMutex)
    {
      myMutex = new std::shared_mutex();
    }
  }
  else
  {
    delete myMutex;
    myMutex = nullptr;
  }
}

//=================================================================================================

NCollection_IncAllocator::~NCollection_IncAllocator()
{
  clean();
  delete myMutex;
  myMutex = nullptr;
}

//=================================================================================================

void* NCollection_IncAllocator::AllocateOptimal(const size_t theSize)
{
  if (myMutex)
  {
    // Fast path: shared lock allows multiple threads to bump-allocate
    // concurrently via CAS. Block list structure is read-only under shared lock.
    {
      std::shared_lock<std::shared_mutex> aSharedLock(*myMutex);
      IBlock* aBlock = myAllocationHeap;
      if (aBlock)
      {
        size_t anAvail = aBlock->AvailableSize.load(std::memory_order_relaxed);
        while (anAvail >= theSize)
        {
          if (aBlock->AvailableSize.compare_exchange_weak(
                anAvail, anAvail - theSize, std::memory_order_acquire, std::memory_order_relaxed))
          {
            // Won the CAS — claim our disjoint portion of the block.
            char* aRes = aBlock->CurPointer.fetch_add(theSize, std::memory_order_relaxed);
            return aRes;
          }
          // CAS failed (another thread allocated), anAvail updated — retry.
        }
      }
    } // shared lock released before taking exclusive

    // Slow path: exclusive lock for new block allocation and list reordering.
    std::lock_guard<std::shared_mutex> aExclLock(*myMutex);
    return allocateSlow(theSize);
  }

  // Non-thread-safe path: no synchronization overhead at all.
  return allocateSlow(theSize);
}

//=================================================================================================

void* NCollection_IncAllocator::allocateSlow(const size_t theSize)
{
  IBlock* aBlock = nullptr;
  // Use existing head block if it has enough space.
  if (myAllocationHeap
      && myAllocationHeap->AvailableSize.load(std::memory_order_relaxed) >= theSize)
  {
    aBlock = myAllocationHeap;
  }
  else // Allocate new block from OS.
  {
    if (++myBlockCount % 5 == 0)
    {
      increaseBlockSize();
    }
    if (myBlockSize < theSize)
    {
      myBlockSize = static_cast<unsigned>(theSize);
    }
    void* aBufferBlock       = Standard::AllocateOptimal(myBlockSize + sizeof(IBlock));
    aBlock                   = new (aBufferBlock) IBlock(aBufferBlock, myBlockSize);
    aBlock->NextBlock        = myAllocationHeap;
    aBlock->NextOrderedBlock = myOrderedBlocks;
    myOrderedBlocks          = aBlock;
    myAllocationHeap         = aBlock;
  }
  // Bump allocate within the selected block.
  const size_t anAvail = aBlock->AvailableSize.load(std::memory_order_relaxed);
  void*        aRes    = aBlock->CurPointer.load(std::memory_order_relaxed);
  aBlock->CurPointer.store(
    static_cast<char*>(aRes) + theSize, std::memory_order_relaxed);
  aBlock->AvailableSize.store(anAvail - theSize, std::memory_order_relaxed);
  if (anAvail - theSize < 16)
  {
    myAllocationHeap  = aBlock->NextBlock;
    aBlock->NextBlock = myUsedHeap;
    myUsedHeap        = aBlock;
  }
  else
  {
    IBlock* aBlockIter           = aBlock->NextBlock;
    IBlock* aBlockToReplaceAfter = nullptr;
    while (aBlockIter) // Search new sorted position
    {
      if (aBlockIter->AvailableSize.load(std::memory_order_relaxed)
          > aBlock->AvailableSize.load(std::memory_order_relaxed))
      {
        aBlockToReplaceAfter = aBlockIter;
        aBlockIter           = aBlockIter->NextBlock;
        continue;
      }
      break;
    }
    if (aBlockToReplaceAfter) // Update list order
    {
      IBlock* aNext                   = aBlockToReplaceAfter->NextBlock;
      aBlockToReplaceAfter->NextBlock = aBlock;
      myAllocationHeap                = aBlock->NextBlock;
      aBlock->NextBlock               = aNext;
    }
  }
  return aRes;
}

//=================================================================================================

void* NCollection_IncAllocator::Allocate(const size_t theSize)
{
  return AllocateOptimal(theSize);
}

//=================================================================================================

void NCollection_IncAllocator::clean()
{
  // Exclusive lock — no allocations can be in progress.
  if (myMutex)
    myMutex->lock();
  IBlock* aHeapIter = myOrderedBlocks;
  while (aHeapIter)
  {
    IBlock* aCur = aHeapIter;
    aHeapIter    = aHeapIter->NextOrderedBlock;
    Standard::Free(aCur);
  }
  myOrderedBlocks  = nullptr;
  myAllocationHeap = nullptr;
  myUsedHeap       = nullptr;
  myBlockCount     = 0;
  myBlockSize      = THE_DEFAULT_BLOCK_SIZE;
  if (myMutex)
    myMutex->unlock();
}

//=================================================================================================

void NCollection_IncAllocator::increaseBlockSize()
{
  switch (computeLevel(myBlockSize))
  {
    case NCollection_IncAllocator::IBlockSizeLevel::Min:
      myBlockSize *= 8;
      break;
    case NCollection_IncAllocator::IBlockSizeLevel::Small:
      myBlockSize *= 4;
      break;
    case NCollection_IncAllocator::IBlockSizeLevel::Medium:
      myBlockSize *= 2;
      break;
    case NCollection_IncAllocator::IBlockSizeLevel::Large:
      myBlockSize = static_cast<unsigned>(std::lround(myBlockSize * 1.5));
      break;
    case NCollection_IncAllocator::IBlockSizeLevel::Max:
      break;
  }
}

//=================================================================================================

void NCollection_IncAllocator::resetBlock(IBlock* theBlock) const
{
  char*  aBlockStart = reinterpret_cast<char*>(theBlock) + sizeof(IBlock);
  size_t anAvail     = theBlock->AvailableSize.load(std::memory_order_relaxed);
  char*  aCurPtr     = theBlock->CurPointer.load(std::memory_order_relaxed);
  theBlock->AvailableSize.store(anAvail + static_cast<size_t>(aCurPtr - aBlockStart),
                                std::memory_order_relaxed);
  theBlock->CurPointer.store(aBlockStart, std::memory_order_relaxed);
}

//=================================================================================================

void NCollection_IncAllocator::Reset(const bool theReleaseMemory)
{
  if (theReleaseMemory)
  {
    clean();
    return;
  }
  if (myMutex)
    myMutex->lock();
  IBlock* aHeapIter = myOrderedBlocks;
  while (aHeapIter)
  {
    IBlock* aCur    = aHeapIter;
    aHeapIter       = aHeapIter->NextOrderedBlock;
    aCur->NextBlock = aHeapIter;
    resetBlock(aCur); // reset size and pointer
  }
  myAllocationHeap = myOrderedBlocks;
  myUsedHeap       = nullptr;
  if (myMutex)
    myMutex->unlock();
}

//=================================================================================================

NCollection_IncAllocator::IBlock::IBlock(void* thePointer, const size_t theSize)
    : CurPointer(static_cast<char*>(thePointer) + sizeof(IBlock)),
      AvailableSize(theSize)
{
}
