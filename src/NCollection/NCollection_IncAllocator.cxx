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

#include <Standard_Mutex.hxx>
#include <Standard_OutOfMemory.hxx>

#include <cmath>

IMPLEMENT_STANDARD_RTTIEXT(NCollection_IncAllocator, NCollection_BaseAllocator)

namespace
{
  // Bounds for checking block size level
  static constexpr unsigned THE_SMALL_BOUND_BLOCK_SIZE = NCollection_IncAllocator::THE_DEFAULT_BLOCK_SIZE * 16;   // 196 KB
  static constexpr unsigned THE_MEDIUM_BOUND_BLOCK_SIZE = NCollection_IncAllocator::THE_DEFAULT_BLOCK_SIZE * 64;  // 786 KB
  static constexpr unsigned THE_LARGE_BOUND_BLOCK_SIZE = NCollection_IncAllocator::THE_DEFAULT_BLOCK_SIZE * 1024; // 12 MB

  //=======================================================================
  //function : computeLevel
  //purpose  :
  //=======================================================================
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
}

//=======================================================================
//function : NCollection_IncAllocator
//purpose  : Constructor
//=======================================================================
NCollection_IncAllocator::NCollection_IncAllocator(const size_t theDefaultSize) :
  myBlockSize(static_cast<unsigned>(theDefaultSize < THE_MINIMUM_BLOCK_SIZE ? THE_DEFAULT_BLOCK_SIZE : theDefaultSize))
{}

//=======================================================================
//function : SetThreadSafe
//purpose  : Constructor
//=======================================================================
void NCollection_IncAllocator::SetThreadSafe (const bool theIsThreadSafe)
{
  if(theIsThreadSafe)
  {
    if (!myMutex)
    {
      myMutex = new Standard_Mutex;
    }
  }
  else
  {
    delete myMutex;
    myMutex = nullptr;
  }
}

//=======================================================================
//function : ~NCollection_IncAllocator
//purpose  : Destructor
//=======================================================================
NCollection_IncAllocator::~NCollection_IncAllocator()
{
  clean();
  delete myMutex;
}

//=======================================================================
//function : AllocateOptimal
//purpose  : allocate a memory
//=======================================================================
void* NCollection_IncAllocator::AllocateOptimal(const size_t theSize)
{
  Standard_Mutex::Sentry aLock(myMutex);
  // Allocating using general block
  IBlock* aBlock = nullptr;
  // Use allocated blocks
  if (myAllocationHeap && myAllocationHeap->AvailableSize >= theSize)
  {
    aBlock = myAllocationHeap;
  }
  else // Allocate new general block
  {
    if (++myBlockCount % 5 == 0) // increase count before checking
    {
      increaseBlockSize();
    }
    if (myBlockSize < theSize)
    {
      myBlockSize = static_cast<unsigned>(theSize);
    }
    void* aBufferBlock = Standard::AllocateOptimal(myBlockSize + sizeof(IBlock));
    aBlock = new (aBufferBlock) IBlock(aBufferBlock, myBlockSize);
    aBlock->NextBlock = myAllocationHeap;
    aBlock->NextOrderedBlock = myOrderedBlocks;
    myOrderedBlocks = aBlock;
    myAllocationHeap = aBlock;
  }
  void* aRes = aBlock->CurPointer;
  aBlock->CurPointer += theSize;
  aBlock->AvailableSize -= theSize;
  if (aBlock->AvailableSize < 16)
  {
    myAllocationHeap = aBlock->NextBlock;
    aBlock->NextBlock = myUsedHeap;
    myUsedHeap = aBlock;
  }
  else
  {
    IBlock* aBlockIter = aBlock->NextBlock;
    IBlock* aBlockToReplaceAfter = nullptr;
    while (aBlockIter) // Search new sorted position
    {
      if (aBlockIter->AvailableSize > aBlock->AvailableSize)
      {
        aBlockToReplaceAfter = aBlockIter;
        aBlockIter = aBlockIter->NextBlock;
        continue;
      }
      break;
    }
    if (aBlockToReplaceAfter) // Update list order
    {
      IBlock* aNext = aBlockToReplaceAfter->NextBlock;
      aBlockToReplaceAfter->NextBlock = aBlock;
      myAllocationHeap = aBlock->NextBlock;
      aBlock->NextBlock = aNext;
    }
  }
  return aRes;
}

//=======================================================================
//function : Allocate
//purpose  : Allocate a memory
//=======================================================================
void* NCollection_IncAllocator::Allocate(const size_t theSize)
{
  return AllocateOptimal(theSize);
}

//=======================================================================
//function : clean
//purpose  : 
//=======================================================================
void NCollection_IncAllocator::clean()
{
  Standard_Mutex::Sentry aLock(myMutex);
  IBlock* aHeapIter = myOrderedBlocks;
  while (aHeapIter)
  {
    IBlock* aCur = aHeapIter;
    aHeapIter = aHeapIter->NextOrderedBlock;
    Standard::Free(aCur);
  }
  myOrderedBlocks = nullptr;
  myAllocationHeap = nullptr;
  myUsedHeap = nullptr;
  myBlockCount = 0;
  myBlockSize = THE_DEFAULT_BLOCK_SIZE;
}

//=======================================================================
//function : increaseBlockSize
//purpose  : 
//=======================================================================
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

//=======================================================================
//function : resetBlock
//purpose  : 
//=======================================================================
void NCollection_IncAllocator::resetBlock(IBlock* theBlock) const
{
  theBlock->AvailableSize = theBlock->AvailableSize + (theBlock->CurPointer - (reinterpret_cast<char*>(theBlock) + sizeof(IBlock)));
  theBlock->CurPointer = reinterpret_cast<char*>(theBlock) + sizeof(IBlock);
}

//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================
void NCollection_IncAllocator::Reset(const Standard_Boolean theReleaseMemory)
{
  if (theReleaseMemory)
  {
    clean();
    return;
  }
  Standard_Mutex::Sentry aLock(myMutex);
  IBlock* aHeapIter = myOrderedBlocks;
  while (aHeapIter)
  {
    IBlock* aCur = aHeapIter;
    aHeapIter = aHeapIter->NextOrderedBlock;
    aCur->NextBlock = aHeapIter;
    resetBlock(aCur); // reset size and pointer
  }
  myAllocationHeap = myOrderedBlocks;
  myUsedHeap = nullptr;
}

//=======================================================================
//function : IBlockSmall::IBlockSmall
//purpose  : 
//=======================================================================
NCollection_IncAllocator::IBlock::IBlock(void* thePointer,
                                         const size_t theSize) :
  CurPointer(static_cast<char*>(thePointer) + sizeof(IBlock)),
  AvailableSize(theSize)
{}
