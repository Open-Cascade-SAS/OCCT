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
#include <Standard_ProgramError.hxx>

//=======================================================================
//function : allocData
//purpose  : 
//=======================================================================

void NCollection_SparseArrayBase::allocData (const Standard_Size iBlock)
{
  if ( iBlock < myNbBlocks )
    return;

  // the allocation of blocks starts from myBlockSize items
  // and then is multiplied by 2 every time reallocation is needed
  Standard_Size newNbBlocks = ( myNbBlocks ? myNbBlocks * 2 : myBlockSize );
  while (iBlock >= newNbBlocks) newNbBlocks *= 2;

  Standard_Address* newData = 
    (Standard_Address*)malloc(newNbBlocks*sizeof(Standard_Address));
  if ( myNbBlocks >0 )
    memcpy (newData, myData, myNbBlocks*sizeof(Standard_Address));
  memset (newData+myNbBlocks, 0, (newNbBlocks-myNbBlocks)*sizeof(Standard_Address));

  free (myData);
  myData = newData;
  myNbBlocks = newNbBlocks;
}

//=======================================================================
//function : freeBlock
//purpose  : 
//=======================================================================

void NCollection_SparseArrayBase::freeBlock (const Standard_Size iBlock)
{
  Standard_Address & anAddr = myData[iBlock];
  Block aBlock = getBlock(anAddr);
  for (Standard_Size anInd=0; anInd < myBlockSize; anInd++)
    if ( aBlock.IsSet(anInd) )
    {
      destroyItem (getItem (aBlock, anInd));
      mySize--;
    }
  free (anAddr);
  anAddr = 0;
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void NCollection_SparseArrayBase::Clear ()
{
  // free block data
  for (Standard_Size iBlock=0; iBlock < myNbBlocks; iBlock++)
    if ( myData[iBlock] )
      freeBlock (iBlock);
  
  // free blocks and reset counters
  free (myData);
  myData = 0;
  myNbBlocks = 0;
  
  // consistency check
  Standard_ProgramError_Raise_if (mySize!=0,"NCollection_SparseArrayBase: Implementation error: inconsistent items count")
}

//=======================================================================
//function : assign
//purpose  : 
//=======================================================================

void NCollection_SparseArrayBase::assign (const NCollection_SparseArrayBase& theOther)
{
  if (this == &theOther) 
    return;

  // if block size is different, clear all data
  if ( myBlockSize != theOther.myBlockSize )
    Clear();
  myBlockSize = theOther.myBlockSize;

  // iterate by blocks in theOther
  Standard_Size iBlock=0;
  for (; iBlock < theOther.myNbBlocks; iBlock++)
  {
    if ( ! theOther.myData[iBlock] )
    {
      // if other block is empty, just make sure to empty that block in "this"
      if ( iBlock < myNbBlocks && myData[iBlock] )
	freeBlock (iBlock);
      continue;
    }

    if ( iBlock >= myNbBlocks )
      allocData(iBlock);
    Block anOtherBlock = getBlock(theOther.myData[iBlock]);

    // if block not yet allocated, just allocate and fill
    Standard_Address & anAddr = myData[iBlock];
    if ( ! anAddr ) 
    {
      anAddr = calloc (Block::Size(myBlockSize, myItemSize), sizeof(char));
      Block aBlock ( getBlock(anAddr) );
      for (Standard_Size anInd=0; anInd < myBlockSize; anInd++)
	if ( anOtherBlock.IsSet(anInd) )
	{
	  Standard_Address anItem = getItem (aBlock, anInd);
	  aBlock.Set(anInd);
	  (*aBlock.Count)++;
	  mySize++;
	  createItem (anItem, getItem(anOtherBlock, anInd));
	}
    }
    // else perform copying item-by-item
    else 
    {
      Block aBlock ( getBlock(anAddr) );
      for (Standard_Size anInd=0; anInd < myBlockSize; anInd++)
      {
	Standard_Address anItem = getItem (aBlock, anInd);
	if ( anOtherBlock.IsSet(anInd) )
	{
	  Standard_Address anOtherItem = getItem (anOtherBlock, anInd);
	  if ( aBlock.IsSet(anInd) ) // copy
	  {
	    copyItem (anItem, anOtherItem);
	  }
	  else // create
	  {
	    aBlock.Set(anInd);
	    (*aBlock.Count)++;
	    mySize++;
	    createItem (anItem, getItem(anOtherBlock, anInd));
	  }
	}
	else if ( aBlock.IsSet(anInd) ) // delete 
	{
	  aBlock.Set(anInd);
	  (*aBlock.Count)--;
	  mySize--;
	  destroyItem (anItem);
	}
      }
    }
  }

  // clear any remaining blocks in this
  for (; iBlock < myNbBlocks; iBlock++)
    if ( myData[iBlock] )
      freeBlock (iBlock);
  
  // consistency check
  Standard_ProgramError_Raise_if (mySize!=theOther.mySize,
				 "NCollection_SparseArrayBase: Implementation error: inconsistent items count")
}

//=======================================================================
//function : exchange
//purpose  : 
//=======================================================================

template<class T> static inline void sswap (T &a, T &b) { T c = a; a = b; b = c; }

void NCollection_SparseArrayBase::exchange (NCollection_SparseArrayBase& theOther)
{
  if (this == &theOther) 
    return;

  // swap fields of this and theOther
  sswap (myItemSize, theOther.myItemSize);
  sswap (myBlockSize,theOther.myBlockSize);
  sswap (myNbBlocks, theOther.myNbBlocks);
  sswap (mySize,     theOther.mySize);
  sswap (myData,     theOther.myData);
}

//=======================================================================
//function : setValue
//purpose  : 
//=======================================================================

Standard_Address NCollection_SparseArrayBase::setValue (const Standard_Size theIndex,
							const Standard_Address theValue) 
{
  Standard_Size iBlock = theIndex / myBlockSize;
    
  // resize blocks array if necessary
  if ( iBlock >= myNbBlocks )
    allocData (iBlock);

  // allocate block if necessary
  Standard_Address & anAddr = myData[iBlock];
  if ( ! anAddr )
    anAddr = calloc (Block::Size(myBlockSize, myItemSize), sizeof(char));

  // get a block
  Block aBlock (getBlock (anAddr));

  // mark item as defined 
  Standard_Size anInd = theIndex % myBlockSize;
  Standard_Address anItem = getItem (aBlock, anInd);

  // either create an item by copy constructor if it is new, or assign it
  if ( aBlock.Set(anInd) )
  {
    (*aBlock.Count)++;
    mySize++;
    createItem (anItem, theValue);
  }
  else
    copyItem (anItem, theValue);
    
  return anItem;
}

//=======================================================================
//function : HasValue
//purpose  : 
//=======================================================================

Standard_Boolean NCollection_SparseArrayBase::HasValue (const Standard_Size theIndex) const
{
  Standard_Size iBlock = theIndex / myBlockSize;
  if ( iBlock >= myNbBlocks ||
       ! myData[iBlock] )
    return Standard_False;
  return getBlock(myData[iBlock]).IsSet(theIndex % myBlockSize) ? Standard_True : Standard_False;
}

//=======================================================================
//function : UnsetValue
//purpose  : 
//=======================================================================

Standard_Boolean NCollection_SparseArrayBase::UnsetValue (const Standard_Size theIndex)
{
  // check that the item is defined
  Standard_Size iBlock = theIndex / myBlockSize;
  if ( iBlock >= myNbBlocks || ! myData[iBlock] )
    return Standard_False;

  Block aBlock (getBlock(myData[iBlock]));
  Standard_Size anInd = theIndex % myBlockSize;
  if ( ! aBlock.Unset(anInd) )
    return Standard_False;

  // destroy the item
  destroyItem (getItem (aBlock, anInd));
  (*aBlock.Count)--;
  mySize--;

  // free block if it becomes empty
  if ( ! (*aBlock.Count) )
    freeBlock (iBlock);

  return Standard_True;
}

//=======================================================================
//function : Iterator::Iterator
//purpose  : 
//=======================================================================

NCollection_SparseArrayBase::Iterator::Iterator (const NCollection_SparseArrayBase* theArray)
: myArr((NCollection_SparseArrayBase*)theArray),
  myHasMore(Standard_False), myIBlock(0), myInd(0), 
  myBlock(0,0,0)
{
  init(theArray);
}

//=======================================================================
//function : Iterator::Next
//purpose  : 
//=======================================================================

void NCollection_SparseArrayBase::Iterator::Next ()
{
  if ( ! myArr || ! myHasMore )
    return;

  // iterate by items and blocks  
  for ( myInd++; ; myInd++ ) {
    // if index is over the block size, advance to the next non-empty block
    if ( myInd >= myArr->myBlockSize )
    {
      for ( myIBlock++; ; myIBlock++ ) {
	if ( myIBlock >= myArr->myNbBlocks ) // end
	{
	  myHasMore = Standard_False;
	  return;
	}
	if ( myArr->myData[myIBlock] )
	{
	  myInd = 0;
	  myBlock = Block (myArr->myData[myIBlock], myArr->myBlockSize, myArr->myItemSize );
	  break;
	}
      }
    }
    // check if item is defined
    if ( myBlock.IsSet (myInd) )
      return;
  }
}

//=======================================================================
//function : Iterator::init
//purpose  : 
//=======================================================================

void NCollection_SparseArrayBase::Iterator::init (const NCollection_SparseArrayBase* theArray)
{
  myArr = (NCollection_SparseArrayBase*)theArray;
  myHasMore = Standard_False;
  if ( myArr ) 
  {
    myInd = 0;
    // find first non-empty block
    for ( myIBlock=0; myIBlock < myArr->myNbBlocks; myIBlock++ )
    {
      if ( ! myArr->myData[myIBlock] ) 
	continue;
      myHasMore = Standard_True;
      myBlock = Block (myArr->myData[myIBlock], myArr->myBlockSize, myArr->myItemSize );
      // if first item in the block is not set, advance to the next defined item
      if ( ! myBlock.IsSet(myInd) )
	Next();
      return;
    }
  }
}

