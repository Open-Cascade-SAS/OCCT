// Created on: 2013-11-12
// Created by: Maxim YAKUNIN (myn)
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#include <NCollection_AccAllocator.hxx>
#include <Standard_OutOfMemory.hxx>
#include <Standard_Assert.hxx>


IMPLEMENT_STANDARD_RTTIEXT(NCollection_AccAllocator,NCollection_BaseAllocator)

//=======================================================================
//function : NCollection_AccAllocator
//purpose  : Constructor
//=======================================================================
NCollection_AccAllocator::NCollection_AccAllocator(const size_t theBlockSize)
: myBlockSize(theBlockSize), mypLastBlock(0L)
{
  allocateNewBlock(myBlockSize);
}

//=======================================================================
//function : ~NCollection_AccAllocator
//purpose  : Destructor
//=======================================================================
NCollection_AccAllocator::~NCollection_AccAllocator()
{
  for (Block* aBlock = mypLastBlock; aBlock; aBlock = aBlock->prevBlock)
  {
    Standard::Free(aBlock->address);
  }
}

//=======================================================================
//function : Allocate
//purpose  : Allocate a memory
//=======================================================================
void* NCollection_AccAllocator::Allocate(const size_t theSize)
{
  const AlignedSize aSize(theSize);
  Block* aBlock;

  if (aSize <= mypLastBlock->FreeSize())
  {
    aBlock = mypLastBlock;
  }
  else if (aSize > myBlockSize)
  {
    // If the requested size exceeds normal allocation size,
    // allocate a separate block
    aBlock = allocateNewBlock(aSize);
  }
  else
  {
    // Search for a block in the list with enough free space
    Standard_Integer aBlocksRest = MaxLookupBlocks;
    for (aBlock = mypLastBlock->prevBlock;
         aBlock != 0L && --aBlocksRest;
         aBlock = aBlock->prevBlock)
    {
      if (aSize <= aBlock->FreeSize())
        break;
    }
    if (aBlock == 0L || !aBlocksRest)
      // There is no available block with enough free space, create a new one
      aBlock = allocateNewBlock(myBlockSize);
  }

  void* anAddress = aBlock->Allocate(aSize);
#ifdef OCCT_DEBUG_FINDBLOCK
  Key aKey;
  Standard_ASSERT_VOID(aBlock == findBlock(anAddress, aKey),
                       "improper work of NCollection_AccAllocator::findBlock");
#endif
  return anAddress;
}

//=======================================================================
//function : Free
//purpose  : Free a previously allocated memory
//=======================================================================
void NCollection_AccAllocator::Free(void* theAddress)
{
  Key aKey;
  Block* aBlock = findBlock(theAddress, aKey);

#if !defined No_Exception && !defined No_Standard_ProgramError
  if (aBlock == 0L || aBlock->IsEmpty())
  {
    throw Standard_ProgramError("NCollection_AccAllocator::Free: \
                                Trying to free an invalid address");
  }
#endif

  aBlock->Free();
  if (aBlock->IsEmpty())
  {
    Standard_Address anAddress = aBlock->address;

    // Deallocate and remove the free block if there are more blocks
    if (myBlocks.Size() > 1)
    {
      Standard::Free(anAddress);
      Block** appBlock;
      for (appBlock = &mypLastBlock;
          *appBlock != 0L;
           appBlock = &(*appBlock)->prevBlock)
      {
        if (*appBlock == aBlock)
        {
          *appBlock = aBlock->prevBlock;
          break;
        }
      }
      myBlocks.UnBind(aKey);
    }
    // If there are no more blocks, reallocate the block to the default size
    else
    {
      Standard_Address aNewAddress = Standard::Reallocate(anAddress,
                                                          myBlockSize);
      if (aNewAddress == anAddress)
      {
        // Normally, the reallocation keeps the block at the same address
        // (since no block can be smaller than the default size, and thus
        // the allocated memory is just shrunk or untouched).
        // In this case, just update the block's free size.
        aBlock->SetFreeSize(myBlockSize);
      }
      else
      {
        // Reallocation still may return a different address even if the new
        // size is equal to or smaller than the old one (this can happen in
        // debug mode).
        Key aNewKey = getKey(aNewAddress);
        if (aNewKey.Value == aKey.Value)
        {
          // If the new address have the same key,
          // just update the block's address and free size
          aBlock->address = aNewAddress;
          aBlock->SetFreeSize(myBlockSize);
        }
        else
        {
          // If the new address have different key,
          // rebind the block to the map of blocks with the new key.
          myBlocks.Clear(Standard_False);
          mypLastBlock = myBlocks.Bound(aNewKey,
                                        Block(aNewAddress, myBlockSize));
        }
      }
    }
  }
}

//=======================================================================
//function : findBlock
//purpose  : Find a block that the given allocation unit belongs to
//=======================================================================
NCollection_AccAllocator::Block*
NCollection_AccAllocator::findBlock(const Standard_Address theAddress, Key& theKey)
{
  theKey = getKey(theAddress);

  Block* aBlock = myBlocks.ChangeSeek(theKey);
  if (aBlock && aBlock->address <= theAddress)
  {
    return aBlock;
  }

  theKey.Value--;
  aBlock = myBlocks.ChangeSeek(theKey);
  if (aBlock &&
    (Standard_Byte*)aBlock->address + (Standard_Size)myBlockSize > theAddress)
  {
    return aBlock;
  }

  return 0L;
}

//=======================================================================
//function : allocateNewBlock
//purpose  : Allocate a new block and return a pointer to it
//=======================================================================
NCollection_AccAllocator::Block*
NCollection_AccAllocator::allocateNewBlock(const Standard_Size theSize)
{
  Standard_Address anAddress = Standard::Allocate(theSize);
  // we depend on the fact that Standard::Allocate always returns
  // a pointer aligned to a 4 byte boundary
  mypLastBlock = myBlocks.Bound(getKey(anAddress),
                                Block(anAddress, theSize, mypLastBlock));
#ifdef OCCT_DEBUG_FINDBLOCK
  Key aKey;
  Standard_ASSERT_VOID(
    mypLastBlock == findBlock((Standard_Byte*)mypLastBlock->allocStart-1, aKey),
    "improper work of NCollection_AccAllocator::findBlock");
#endif
  return mypLastBlock;
}
