// Created on: 2002-04-24
// Created by: Alexander GRIGORIEV
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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
#ifdef DEB
#include <Standard_OutOfRange.hxx>
#endif
#include <stdlib.h>

//=======================================================================
//function : GetIndexV
//purpose  : 
//=======================================================================

Standard_Integer NCollection_BaseVector::MemBlock::GetIndexV
                   (void * theItem, const size_t theItemSize) const
{
  const ptrdiff_t anOffset = (char *) theItem - (char *) myData;
  const Standard_Integer anIndex = anOffset / theItemSize;
#ifdef DEB
  if (anOffset < 0 || anOffset != Standard_Integer (anIndex * theItemSize)
      || anIndex > Standard_Integer (myLength))
    Standard_OutOfRange::Raise ("NCollection_BaseVector: "
                                "Wrong object passed to GetIndex");
#endif
  return anIndex + myFirstInd;
}

//=======================================================================
//function : ~NCollection_BaseVector
//purpose  : Destructor
//=======================================================================

NCollection_BaseVector::~NCollection_BaseVector()
{
  for (Standard_Integer i = 0; i < myCapacity; i++)
    myData[i].Reinit (0, 0);
  myDataFree (* this, myData);
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void NCollection_BaseVector::Clear()
{
  if (myLength > 0) {
    for (Standard_Integer i = 0; i < myCapacity; i++)
      myData[i].Reinit (0, 0);
    myLength = 0;
    myNBlocks = 0;
  }
}

//=======================================================================
//function : NCollection_BaseVector::Iterator::CopyV
//purpose  : Copy from another iterator
//=======================================================================

void NCollection_BaseVector::Iterator::CopyV
                                (const NCollection_BaseVector::Iterator& theOth)
{
  myVector      = theOth.myVector;
  myICurBlock   = theOth.myICurBlock;
  myIEndBlock   = theOth.myIEndBlock;
  myCurIndex    = theOth.myCurIndex;
  myEndIndex    = theOth.myEndIndex;
}

//=======================================================================
//function : InitV
//purpose  : Initialisation of iterator by a vector
//=======================================================================

void NCollection_BaseVector::Iterator::InitV
                                      (const NCollection_BaseVector& theVector)
{
  myVector      = &theVector;
  myICurBlock   = 0;
  myCurIndex    = 0;
  if (theVector.myNBlocks == 0) {
    myIEndBlock   = 0;
    myEndIndex    = 0;
  } else {
    myIEndBlock   = theVector.myNBlocks - 1;
    myEndIndex    = theVector.myData[myIEndBlock].Length();
  }
}

//=======================================================================
//function : operator =
//purpose  : assignment
//=======================================================================

NCollection_BaseVector& NCollection_BaseVector::operator =
                                        (const NCollection_BaseVector& theOther)
{
//    if (this != &theOther) {
  myIncrement = theOther.myIncrement;
  myLength    = theOther.Length();
  myNBlocks   = (myLength == 0) ? 0 : (1 + (myLength - 1)/myIncrement);
  for (Standard_Integer i = 0; i < myCapacity; i++)
    myData[i].Reinit (0, 0);
  myDataFree (* this, myData);
  myCapacity  = GetCapacity(myIncrement) + myLength / myIncrement;
  myData = myDataInit (* this, myCapacity, NULL, 0);
//    }
  return * this;
}

//=======================================================================
//function : ExpandV
//purpose  : returns the pointer where the new data item is supposed to be put
//=======================================================================

void * NCollection_BaseVector::ExpandV (const Standard_Integer theIndex)
{
  const Standard_Integer aNewLength = theIndex + 1;
  if (myNBlocks) {
    //  Take the last array in the vector of arrays
    MemBlock& aLastBlock = myData [myNBlocks - 1];
    Standard_RangeError_Raise_if (theIndex < aLastBlock.FirstIndex(),
                                  "NColelction_BaseVector::ExpandV");
    Standard_Integer anIndLastBlock = theIndex - aLastBlock.FirstIndex();
    //  Is there still room for 1 item in the last array?
    if (anIndLastBlock < aLastBlock.Size()) {
      myLength = aNewLength;
      aLastBlock.SetLength (anIndLastBlock + 1);
      return aLastBlock.Find (anIndLastBlock, myItemSize);
    }
    myLength = aLastBlock.FirstIndex() + aLastBlock.Size();
  }

  //    There is no room in the last array or the whole vector
  //    is not yet initialised. Initialise a new array, but before that
  //    check whether it is available within myCapacity

  const Standard_Integer nNewBlock =
    myNBlocks + 1 + (theIndex - myLength) / myIncrement;
  if (myCapacity < nNewBlock) {
    // Reallocate the array myData 
    do myCapacity += GetCapacity(myIncrement); while (myCapacity <= nNewBlock);
    MemBlock * aNewData = myDataInit (* this, myCapacity, myData, myNBlocks);
    myDataFree (* this, myData);
    myData = aNewData;
  }
  if (myNBlocks > 0) {
    // Change length of old last block to myIncrement
    MemBlock * aLastBlock = (MemBlock *) &myData[myNBlocks-1];
    aLastBlock -> SetLength (myIncrement);
  }
  // Initialise new blocks
  MemBlock * aNewBlock = (MemBlock *) &myData[myNBlocks++];
  aNewBlock -> Reinit (myLength, myIncrement);
  while (myNBlocks < nNewBlock) {
    aNewBlock -> SetLength (myIncrement);
    myLength += myIncrement;
    aNewBlock = (MemBlock *) &myData[myNBlocks++];
    aNewBlock -> Reinit (myLength, myIncrement);
  }
  aNewBlock -> SetLength (aNewLength - myLength);
  myLength = aNewLength;
  return aNewBlock -> Find (theIndex - aNewBlock -> FirstIndex(), myItemSize);
}

//=======================================================================
//function : Find
//purpose  : locate the memory holding the desired value
//remark   : This is only useful when the blocks can have holes (i.e., deletion
//           is enabled at any location)
//         : Currently this method is replaced by a direct one (inline)
//=======================================================================
#ifdef THIS_IS_NOW_DISABLED
void * NCollection_BaseVector::Find (const Standard_Integer theIndex) const
{
#ifdef DEB
  if (theIndex < 0 || theIndex >= myLength) Standard_OutOfRange::Raise (NULL);
#endif
  //    Binary search for the last memory block whose 'FirstIndex'
  //    is not greater than 'theIndex'
  const MemBlock * ptrArrays = myData;
  Standard_Integer aLow = 0;
  Standard_Integer anUp = myNBlocks-1;
  if (theIndex >= ptrArrays[anUp].FirstIndex()) aLow = anUp;
  else while (1) {
    Standard_Integer aMid = (aLow + anUp)/2;
    if (aMid == aLow) break;
    if (ptrArrays[aMid].FirstIndex() > theIndex)
      anUp = aMid;
    else
      aLow = aMid;
  }

  //    Find the item at the proper offset in the found MemBlock-type block
  const Standard_Integer anOffset = theIndex - ptrArrays[aLow].FirstIndex();
  return ptrArrays[aLow].Find (anOffset, myItemSize);
}
#endif
