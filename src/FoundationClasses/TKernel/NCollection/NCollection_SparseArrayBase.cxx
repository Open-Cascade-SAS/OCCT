// Created on: 2007-02-06
// Created by: Andrey BETENEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#include <NCollection_SparseArrayBase.hxx>
#include <Standard.hxx>

#include <algorithm>
#include <cstring>

//==================================================================================================

void NCollection_SparseArrayBase::allocData(const size_t iBlock)
{
  if (iBlock < myNbBlocks)
    return;

  // the allocation of blocks starts from myBlockSize items
  // and then is multiplied by 2 every time reallocation is needed
  size_t newNbBlocks = (myNbBlocks ? myNbBlocks * 2 : myBlockSize);
  while (iBlock >= newNbBlocks)
    newNbBlocks *= 2;

  void** newData = (void**)Standard::AllocateOptimal(newNbBlocks * sizeof(void*));
  if (myNbBlocks > 0)
    memcpy(newData, myData, myNbBlocks * sizeof(void*));
  memset(newData + myNbBlocks, 0, (newNbBlocks - myNbBlocks) * sizeof(void*));

  Standard::Free(myData);
  myData     = newData;
  myNbBlocks = newNbBlocks;
}

//==================================================================================================

void NCollection_SparseArrayBase::freeBlock(const size_t iBlock, DestroyItemFunc theDestroyItem)
{
  void*& anAddr = myData[iBlock];
  if (!anAddr)
    return;

  Block aBlock = getBlock(anAddr);
  // Destroy all items in the block
  for (size_t anInd = 0; anInd < myBlockSize; anInd++)
  {
    if (aBlock.IsSet(anInd))
    {
      theDestroyItem(getItem(aBlock, anInd));
      mySize--;
    }
  }
  Standard::Free(anAddr);
  anAddr = nullptr;
}

//==================================================================================================

void NCollection_SparseArrayBase::clearItems(DestroyItemFunc theDestroyItem)
{
  // free block data
  for (size_t iBlock = 0; iBlock < myNbBlocks; iBlock++)
  {
    if (myData[iBlock])
    {
      freeBlock(iBlock, theDestroyItem);
    }
  }

  // free blocks array and reset counters
  Standard::Free(myData);
  myData     = nullptr;
  myNbBlocks = 0;
  mySize     = 0;
}

//==================================================================================================

void NCollection_SparseArrayBase::assign(const NCollection_SparseArrayBase& theOther,
                                         CreateItemFunc                     theCreateItem,
                                         DestroyItemFunc                    theDestroyItem,
                                         CopyItemFunc                       theCopyItem)
{
  if (this == &theOther)
    return;

  // if block size is different, clear all data
  if (myBlockSize != theOther.myBlockSize)
    clearItems(theDestroyItem);
  myBlockSize = theOther.myBlockSize;

  // iterate by blocks in theOther
  size_t iBlock = 0;
  for (; iBlock < theOther.myNbBlocks; iBlock++)
  {
    if (!theOther.myData[iBlock])
    {
      // if other block is empty, just make sure to empty that block in "this"
      if (iBlock < myNbBlocks && myData[iBlock])
        freeBlock(iBlock, theDestroyItem);
      continue;
    }

    if (iBlock >= myNbBlocks)
      allocData(iBlock);
    Block anOtherBlock = getBlock(theOther.myData[iBlock]);

    // if block not yet allocated, just allocate and fill
    void*& anAddr = myData[iBlock];
    if (!anAddr)
    {
      const size_t aBlockSize = Block::Size(myBlockSize, myItemSize);
      anAddr                  = Standard::AllocateOptimal(aBlockSize);
      memset(anAddr, 0, aBlockSize);
      Block aBlock(getBlock(anAddr));
      for (size_t anInd = 0; anInd < myBlockSize; anInd++)
        if (anOtherBlock.IsSet(anInd))
        {
          void* anItem = getItem(aBlock, anInd);
          aBlock.Set(anInd);
          (*aBlock.Count)++;
          mySize++;
          theCreateItem(anItem, getItem(anOtherBlock, anInd));
        }
    }
    // else perform copying item-by-item
    else
    {
      Block aBlock(getBlock(anAddr));
      for (size_t anInd = 0; anInd < myBlockSize; anInd++)
      {
        void* anItem = getItem(aBlock, anInd);
        if (anOtherBlock.IsSet(anInd))
        {
          void* anOtherItem = getItem(anOtherBlock, anInd);
          if (aBlock.IsSet(anInd)) // copy
          {
            theCopyItem(anItem, anOtherItem);
          }
          else // create
          {
            aBlock.Set(anInd);
            (*aBlock.Count)++;
            mySize++;
            theCreateItem(anItem, getItem(anOtherBlock, anInd));
          }
        }
        else if (aBlock.IsSet(anInd)) // delete
        {
          aBlock.Set(anInd);
          (*aBlock.Count)--;
          mySize--;
          theDestroyItem(anItem);
        }
      }
    }
  }

  // clear any remaining blocks in this
  for (; iBlock < myNbBlocks; iBlock++)
    if (myData[iBlock])
      freeBlock(iBlock, theDestroyItem);
}

//==================================================================================================

void NCollection_SparseArrayBase::exchange(NCollection_SparseArrayBase& theOther) noexcept
{
  if (this == &theOther)
    return;

  // swap fields of this and theOther using std::swap for better optimization
  std::swap(myItemSize, theOther.myItemSize);
  std::swap(myBlockSize, theOther.myBlockSize);
  std::swap(myNbBlocks, theOther.myNbBlocks);
  std::swap(mySize, theOther.mySize);
  std::swap(myData, theOther.myData);
  std::swap(myDestroyItem, theOther.myDestroyItem);
}

//==================================================================================================

void* NCollection_SparseArrayBase::setValue(const size_t   theIndex,
                                            void* const    theValue,
                                            CreateItemFunc theCreateItem,
                                            CopyItemFunc   theCopyItem)
{
  size_t iBlock = theIndex / myBlockSize;

  // resize blocks array if necessary
  if (iBlock >= myNbBlocks)
    allocData(iBlock);

  // allocate block if necessary
  void*& anAddr = myData[iBlock];
  if (!anAddr)
  {
    const size_t aBlockSize = Block::Size(myBlockSize, myItemSize);
    anAddr                  = Standard::AllocateOptimal(aBlockSize);
    memset(anAddr, 0, aBlockSize);
  }

  // get a block and calculate item index
  Block  aBlock(getBlock(anAddr));
  size_t anInd  = theIndex % myBlockSize;
  void*  anItem = getItem(aBlock, anInd);

  // either create an item by copy constructor if it is new, or assign it
  // Optimize: Set() returns non-zero if bit was not set previously
  if (aBlock.Set(anInd))
  {
    (*aBlock.Count)++;
    mySize++;
    theCreateItem(anItem, theValue);
  }
  else
  {
    // Item already exists, just copy the value
    theCopyItem(anItem, theValue);
  }

  return anItem;
}

//==================================================================================================

bool NCollection_SparseArrayBase::HasValue(const size_t theIndex) const
{
  size_t iBlock = theIndex / myBlockSize;
  if (iBlock >= myNbBlocks || !myData[iBlock])
    return false;
  return getBlock(myData[iBlock]).IsSet(theIndex % myBlockSize) != 0;
}

//==================================================================================================

bool NCollection_SparseArrayBase::unsetValue(const size_t theIndex, DestroyItemFunc theDestroyItem)
{
  // check that the item is defined
  size_t iBlock = theIndex / myBlockSize;
  if (iBlock >= myNbBlocks || !myData[iBlock])
    return false;

  Block  aBlock(getBlock(myData[iBlock]));
  size_t anInd = theIndex % myBlockSize;
  if (!aBlock.Unset(anInd))
    return false;

  // destroy the item
  theDestroyItem(getItem(aBlock, anInd));
  (*aBlock.Count)--;
  mySize--;

  // free block if it becomes empty
  if (!(*aBlock.Count))
    freeBlock(iBlock, theDestroyItem);

  return true;
}

//==================================================================================================

NCollection_SparseArrayBase::Iterator::Iterator(const NCollection_SparseArrayBase* theArray)
    : myArr((NCollection_SparseArrayBase*)theArray),
      myHasMore(false),
      myIBlock(0),
      myInd(0),
      myBlock(nullptr, 0, 0)
{
  init(theArray);
}

//==================================================================================================

void NCollection_SparseArrayBase::Iterator::Next()
{
  if (!myArr || !myHasMore)
    return;

  // iterate by items and blocks
  for (myInd++;; myInd++)
  {
    // if index is over the block size, advance to the next non-empty block
    if (myInd >= myArr->myBlockSize)
    {
      for (myIBlock++;; myIBlock++)
      {
        if (myIBlock >= myArr->myNbBlocks) // end
        {
          myHasMore = false;
          return;
        }
        if (myArr->myData[myIBlock])
        {
          myInd   = 0;
          myBlock = Block(myArr->myData[myIBlock], myArr->myBlockSize, myArr->myItemSize);
          break;
        }
      }
    }
    // check if item is defined
    if (myBlock.IsSet(myInd))
      return;
  }
}

//==================================================================================================

void NCollection_SparseArrayBase::Iterator::init(const NCollection_SparseArrayBase* theArray)
{
  myArr     = (NCollection_SparseArrayBase*)theArray;
  myHasMore = false;
  if (myArr)
  {
    myInd = 0;
    // find first non-empty block
    for (myIBlock = 0; myIBlock < myArr->myNbBlocks; myIBlock++)
    {
      if (!myArr->myData[myIBlock])
        continue;
      myHasMore = true;
      myBlock   = Block(myArr->myData[myIBlock], myArr->myBlockSize, myArr->myItemSize);
      // if first item in the block is not set, advance to the next defined item
      if (!myBlock.IsSet(myInd))
        Next();
      return;
    }
  }
}
