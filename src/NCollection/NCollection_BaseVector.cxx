// Created on: 2002-04-24
// Created by: Alexander GRIGORIEV
// Copyright (c) 2002-2013 OPEN CASCADE SAS
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

#include <NCollection_BaseVector.hxx>
#include <Standard_RangeError.hxx>
#include <cstdlib>

//=======================================================================
//function : NCollection_BaseVector::Iterator::copyV
//purpose  : Copy from another iterator
//=======================================================================

void NCollection_BaseVector::Iterator::copyV (const NCollection_BaseVector::Iterator& theOth)
{
  myVector    = theOth.myVector;
  myICurBlock = theOth.myICurBlock;
  myIEndBlock = theOth.myIEndBlock;
  myCurIndex  = theOth.myCurIndex;
  myEndIndex  = theOth.myEndIndex;
}

//=======================================================================
//function : initV
//purpose  : Initialisation of iterator by a vector
//=======================================================================

void NCollection_BaseVector::Iterator::initV (const NCollection_BaseVector& theVector, Standard_Boolean theToEnd)
{
  myVector = &theVector;

  if (theVector.myNBlocks == 0)
  {
    myCurIndex  = 0;
    myEndIndex  = 0;
    myICurBlock = 0;
    myIEndBlock = 0;
  }
  else
  {
    myIEndBlock = theVector.myNBlocks - 1;
    myEndIndex  = theVector.myData[myIEndBlock].Length;

    myICurBlock = !theToEnd ? 0 : myIEndBlock;
    myCurIndex  = !theToEnd ? 0 : myEndIndex;
  }
}

//=======================================================================
//function : allocMemBlocks
//purpose  :
//=======================================================================

NCollection_BaseVector::MemBlock* NCollection_BaseVector
  ::allocMemBlocks (const Standard_Integer             theCapacity,
                    MemBlock*                          theSource,
                    const Standard_Integer             theSourceSize)
{
  MemBlock* aData = (MemBlock* )myAllocator->Allocate (theCapacity * sizeof(MemBlock));

  // copy content from source array
  Standard_Integer aCapacity = 0;
  if (theSource != NULL)
  {
    memcpy (aData, theSource, theSourceSize * sizeof(MemBlock));
    aCapacity = theSourceSize;
    myAllocator->Free (theSource);
  }

  // Nullify newly allocated blocks
  if (aCapacity < theCapacity)
  {
    memset (&aData[aCapacity], 0, (theCapacity - aCapacity) * sizeof(MemBlock));
  }
  return aData;
}

//=======================================================================
//function : Clear
//purpose  :
//=======================================================================

void NCollection_BaseVector::Clear()
{
  if (myLength > 0)
  {
    for (Standard_Integer anItemIter = 0; anItemIter < myCapacity; ++anItemIter)
    {
      myInitBlocks (*this, myData[anItemIter], 0, 0);
    }
    myLength  = 0;
    myNBlocks = 0;
  }
}

//=======================================================================
//function : expandV
//purpose  : returns the pointer where the new data item is supposed to be put
//=======================================================================

void* NCollection_BaseVector::expandV (const Standard_Integer theIndex)
{
  const Standard_Integer aNewLength = theIndex + 1;
  if (myNBlocks > 0)
  {
    // Take the last array in the vector of arrays
    MemBlock& aLastBlock = myData[myNBlocks - 1];
    Standard_RangeError_Raise_if (theIndex < aLastBlock.FirstIndex,
                                  "NColelction_BaseVector::expandV");
    Standard_Integer anIndLastBlock = theIndex - aLastBlock.FirstIndex;
    // Is there still room for 1 item in the last array?
    if (anIndLastBlock < aLastBlock.Size)
    {
      myLength = aNewLength;
      aLastBlock.Length = anIndLastBlock + 1;
      return aLastBlock.findV (anIndLastBlock, myItemSize);
    }
    myLength = aLastBlock.FirstIndex + aLastBlock.Size;
  }

  // There is no room in the last array
  // or the whole vector is not yet initialised.
  // Initialise a new array, but before that check whether it is available within myCapacity.
  const Standard_Integer nNewBlock = myNBlocks + 1 + (theIndex - myLength) / myIncrement;
  if (myCapacity < nNewBlock)
  {
    // Reallocate the array myData 
    do myCapacity += GetCapacity(myIncrement); while (myCapacity <= nNewBlock);

    myData = allocMemBlocks (myCapacity, myData, myNBlocks);
  }
  if (myNBlocks > 0)
  {
    // Change length of old last block to myIncrement
    MemBlock& aLastBlock = myData[myNBlocks - 1];
    aLastBlock.Length = myIncrement;
  }

  // Initialise new blocks
  MemBlock* aNewBlock = &myData[myNBlocks++];
  myInitBlocks (*this, *aNewBlock, myLength, myIncrement);
  while (myNBlocks < nNewBlock)
  {
    aNewBlock->Length = myIncrement;
    myLength += myIncrement;
    aNewBlock = &myData[myNBlocks++];
    myInitBlocks (*this, *aNewBlock, myLength, myIncrement);
  }
  aNewBlock->Length = aNewLength - myLength;
  myLength = aNewLength;
  return aNewBlock->findV (theIndex - aNewBlock->FirstIndex, myItemSize);
}
