// Created on: 2002-04-24
// Created by: Alexander GRIGORIEV
// Copyright (c) 2002-2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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

void NCollection_BaseVector::Iterator::initV (const NCollection_BaseVector& theVector)
{
  myVector    = &theVector;
  myICurBlock = 0;
  myCurIndex  = 0;
  if (theVector.myNBlocks == 0)
  {
    myIEndBlock = 0;
    myEndIndex  = 0;
  }
  else
  {
    myIEndBlock = theVector.myNBlocks - 1;
    myEndIndex  = theVector.myData[myIEndBlock].Length;
  }
}

//=======================================================================
//function : allocMemBlocks
//purpose  :
//=======================================================================

NCollection_BaseVector::MemBlock* NCollection_BaseVector
  ::allocMemBlocks (Handle(NCollection_BaseAllocator)& theAllocator,
                    const Standard_Integer             theCapacity,
                    MemBlock*                          theSource,
                    const Standard_Integer             theSourceSize)
{
  MemBlock* aData = (MemBlock* )theAllocator->Allocate (theCapacity * sizeof(MemBlock));

  // copy content from source array
  Standard_Integer aCapacity = 0;
  if (theSource != NULL)
  {
    memcpy (aData, theSource, theSourceSize * sizeof(MemBlock));
    aCapacity = theSourceSize;
    theAllocator->Free (theSource);
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

void* NCollection_BaseVector::expandV (Handle(NCollection_BaseAllocator)& theAllocator,
                                       const Standard_Integer             theIndex)
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

    myData = allocMemBlocks (theAllocator, myCapacity, myData, myNBlocks);
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
