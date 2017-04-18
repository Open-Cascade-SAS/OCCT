// Created on: 2002-03-29
// Created by: Alexander GRIGORIEV
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

// Purpose:   Implementation of the BaseSequence class

#include <NCollection_BaseSequence.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_DomainError.hxx>

inline void NCollection_BaseSequence::Nullify ()
{
  myFirstItem = myLastItem = myCurrentItem = NULL;
  myCurrentIndex = mySize = 0;
}

//=======================================================================
//function : ClearSeq
//purpose  : removes all items from the current sequence
//=======================================================================

void NCollection_BaseSequence::ClearSeq (NCollection_DelSeqNode fDel)
{
  NCollection_SeqNode* p = myFirstItem;
  while (p) {
    NCollection_SeqNode* q = p;
    p = p->Next();
    fDel (q, myAllocator);
  }
  Nullify();
}

//=======================================================================
//function : PAppend
//purpose  : append an item to sequence
//=======================================================================

void NCollection_BaseSequence::PAppend (NCollection_SeqNode * theItem)
{
  if (mySize == 0) {
    myFirstItem = myLastItem = myCurrentItem = theItem;
    myCurrentIndex = mySize = 1;
  } else {
    myLastItem->SetNext(theItem);
    theItem->SetPrevious(myLastItem);
    theItem->SetNext(NULL);
    myLastItem = theItem;
    ++ mySize;               
  }
}

//=======================================================================
//function : PAppend
//purpose  : push a sequence at the end of the sequence
//=======================================================================

void NCollection_BaseSequence::PAppend(NCollection_BaseSequence& Other)
{
  if (Other.mySize == 0)
    return;
  if (mySize == 0) {
    mySize         = Other.mySize;
    myFirstItem    = Other.myFirstItem;
    myLastItem     = Other.myLastItem;
    myCurrentItem  = myFirstItem;
    myCurrentIndex = 1;
  } else {
    mySize += Other.mySize;
    myLastItem->SetNext(Other.myFirstItem);
    if (Other.myFirstItem) {
      Other.myFirstItem->SetPrevious(myLastItem);
      myLastItem = Other.myLastItem;
    }
  }
  Other.Nullify();
}

//=======================================================================
//function : PPrepend
//purpose  : prepend an item to sequence
//=======================================================================

void NCollection_BaseSequence::PPrepend (NCollection_SeqNode * theItem)
{
  if (mySize == 0) {
    myFirstItem = myLastItem = myCurrentItem = theItem;
    myCurrentIndex = mySize = 1;
  } else {
    myFirstItem->SetPrevious (theItem);
    theItem->SetNext (myFirstItem); 
    theItem->SetPrevious(NULL);
    theItem->SetNext(myFirstItem);
    myFirstItem = theItem;
    ++ mySize;
    ++ myCurrentIndex;
  }
}

//=======================================================================
//function : PPrepend
//purpose  : push a sequence in the beginning of the sequence
//=======================================================================

void NCollection_BaseSequence::PPrepend (NCollection_BaseSequence& Other)
{
  if (Other.mySize == 0)
    return;
  if (mySize == 0) {
    mySize         = Other.mySize;
    myFirstItem    = Other.myFirstItem;
    myLastItem     = Other.myLastItem;
    myCurrentIndex = 1;
    myCurrentItem  = myFirstItem;
  } else {
    mySize += Other.mySize;
    if (Other.myLastItem)
      Other.myLastItem->SetNext (myFirstItem);
    myFirstItem->SetPrevious(Other.myLastItem);
    myFirstItem = Other.myFirstItem;
    myCurrentIndex += Other.mySize;
  }
  Other.Nullify();
}

//=======================================================================
//function : PReverse
//purpose  : reverse the order of a given sequence
//=======================================================================

void NCollection_BaseSequence::PReverse()
{
  NCollection_SeqNode* p = myFirstItem;
  while (p) {
    NCollection_SeqNode* tmp = p->Next();
    p->SetNext (p->Previous());
    p->SetPrevious (tmp);
    p = tmp;
  }
  NCollection_SeqNode* tmp = myFirstItem;
  myFirstItem = myLastItem;
  myLastItem = tmp;
  if (mySize != 0)
    myCurrentIndex = mySize + 1 - myCurrentIndex;
}


//=======================================================================
//function : PInsertAfter
//purpose  : 
//=======================================================================

void NCollection_BaseSequence::PInsertAfter
                             (NCollection_BaseSequence::Iterator& thePosition,
                              NCollection_SeqNode                 * theItem)
{
  NCollection_SeqNode * aPos = thePosition.myCurrent;
  if (aPos == NULL)
    PPrepend (theItem);
  else {
    theItem->SetNext (aPos->Next());
    theItem->SetPrevious (aPos);
    if (aPos->Next() == NULL)
      myLastItem = theItem;
    else
      aPos->Next()->SetPrevious(theItem);
    aPos->SetNext(theItem);
    ++ mySize;
    myCurrentItem = myFirstItem;
    myCurrentIndex = 1;
  }
}

//=======================================================================
//function : PInsertAfter
//purpose  : 
//=======================================================================

void NCollection_BaseSequence::PInsertAfter(const Standard_Integer theIndex,
                                            NCollection_SeqNode * theItem)
{
  if (theIndex == 0)
    PPrepend (theItem);
  else {
    NCollection_SeqNode * p = Find (theIndex);
    theItem->SetNext(p->Next());
    theItem->SetPrevious(p);
    if (theIndex == mySize)
      myLastItem = theItem;
    else
      p->Next()->SetPrevious(theItem);
    p->SetNext(theItem);
    ++ mySize;
    if (theIndex < myCurrentIndex)
      ++ myCurrentIndex;
  }
}

//=======================================================================
//function : PInsertAfter
//purpose  : insert a sequence after a given index in the sequence
//=======================================================================

void NCollection_BaseSequence::PInsertAfter (const Standard_Integer theIndex,
                                             NCollection_BaseSequence& Other)
{
  if (theIndex < 0 || theIndex > mySize)
    throw Standard_OutOfRange();
  if (Other.mySize != 0) {
    if (theIndex == 0) 
      PPrepend (Other);
    else {
      NCollection_SeqNode * p = Find (theIndex);
      Other.myFirstItem->SetPrevious (p);
      Other.myLastItem->SetNext (p->Next());
      if (theIndex == mySize)
        myLastItem = Other.myLastItem;
      else
        p->Next()->SetPrevious (Other.myLastItem);
      p->SetNext (Other.myFirstItem);
      mySize += Other.mySize;
      if (theIndex < myCurrentIndex)
        myCurrentIndex += Other.mySize;
      Other.Nullify();
    }
  }
}

//=======================================================================
//function : PExchange
//purpose  : exchange two elements in the sequence
//=======================================================================

void NCollection_BaseSequence::PExchange (const Standard_Integer I,
                                          const Standard_Integer J)
{
  Standard_OutOfRange_Raise_if (I <= 0 || J <= 0 || I > mySize || J > mySize,
                                "" );

  // Assume I < J
  if (J < I)
    PExchange(J,I);
  else if (I < J) {
    NCollection_SeqNode * pi = Find(I);
    NCollection_SeqNode * pj = Find(J);

    // update the node before I
    if (pi->Previous())
      pi->Previous()->SetNext (pj);
    else 
      myFirstItem = pj;

    // update the node after J
    if (pj->Next())
      pj->Next()->SetPrevious(pi);
    else
      myLastItem = pi;

    if (pi->Next() == pj) {          // I and J are consecutives, update them
      pj->SetPrevious (pi->Previous());
      pi->SetPrevious (pj);
      pi->SetNext (pj->Next());
      pj->SetNext (pi);
    }
    else {                        // I and J are not consecutive
      // update the node after I
      pi->Next()->SetPrevious (pj);
      // update the node before J
      pj->Previous()->SetNext (pi);
      // update nodes I and J
      NCollection_SeqNode* tmp = pi->Next();       
      pi->SetNext (pj->Next());
      pj->SetNext (tmp);
      tmp = pi->Previous();
      pi->SetPrevious (pj->Previous());
      pj->SetPrevious (tmp);
    }

    if      (myCurrentIndex == I) myCurrentItem = pj;
    else if (myCurrentIndex == J) myCurrentItem = pi;
  }
}

//=======================================================================
//function : PSplit
//purpose  : 
//=======================================================================

void NCollection_BaseSequence::PSplit (const Standard_Integer theIndex,
                                       NCollection_BaseSequence& Sub)
{
  Standard_OutOfRange_Raise_if (theIndex <= 0 || theIndex > mySize,"" );
  Standard_DomainError_Raise_if (this == &Sub, "No Split on myself!!");

  NCollection_SeqNode * p = Find (theIndex);

  Sub.myLastItem = myLastItem;
  Sub.mySize = mySize - theIndex + 1;

  myLastItem = p->Previous();
  if (myLastItem) {
    myLastItem->SetNext(NULL);
    mySize = theIndex - 1;
    if (myCurrentIndex >= theIndex) {
      myCurrentIndex = 1;
      myCurrentItem  = myFirstItem;
    }
  } else {
    myFirstItem = myCurrentItem = NULL;
    mySize = myCurrentIndex = 0;
  }

  Sub.myFirstItem = Sub.myCurrentItem = p;
  p->SetPrevious (NULL);
  Sub.myCurrentIndex = 1;
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void NCollection_BaseSequence::RemoveSeq 
                              (NCollection_BaseSequence::Iterator& thePosition,
                               NCollection_DelSeqNode              fDel)
{
  NCollection_SeqNode * aPos = thePosition.myCurrent;
  if (aPos == NULL)
    return;
  thePosition.myCurrent = aPos -> Next();

  if (aPos->Previous())
    aPos->Previous()->SetNext (aPos->Next());
  else
    myFirstItem = aPos->Next();

  if (aPos->Next())
    aPos->Next()->SetPrevious (aPos->Previous());
  else
    myLastItem = aPos->Previous();

  -- mySize;
  myCurrentItem  = myLastItem;
  myCurrentIndex = mySize;

  fDel (aPos, myAllocator);
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void NCollection_BaseSequence::RemoveSeq (const Standard_Integer theIndex,
                                          NCollection_DelSeqNode fDel)
{
  Standard_OutOfRange_Raise_if (theIndex <= 0 || theIndex > mySize,
                                "NCollection_BaseSequence::RemoveSeq() - index is out of range");

  NCollection_SeqNode * p = Find (theIndex);
  if (p->Previous())
    p->Previous()->SetNext (p->Next());
  else
    myFirstItem = p->Next();
  if (p->Next())
    p->Next()->SetPrevious (p->Previous());
  else
    myLastItem = p->Previous();

  -- mySize;
  if      (myCurrentIndex > theIndex) -- myCurrentIndex;
  else if (myCurrentIndex == theIndex) {
    if (p->Next()) 
      myCurrentItem = p->Next();
    else {
      myCurrentItem = myLastItem;
      myCurrentIndex = mySize;
    }
  }
  fDel (p, myAllocator);
}

//=======================================================================
//function : Remove
//purpose  : remove a set of items
//=======================================================================

void NCollection_BaseSequence::RemoveSeq (const Standard_Integer From,
                                          const Standard_Integer To, 
                                          NCollection_DelSeqNode fDel)
{
  Standard_OutOfRange_Raise_if (From <= 0 || To > mySize || From > To,
                                "NCollection_BaseSequence::RemoveSeq() - invalid input range");

  NCollection_SeqNode * pfrom = Find(From);
  NCollection_SeqNode * pto   = Find(To);
  
  if (pfrom->Previous())
    pfrom->Previous()->SetNext (pto->Next());
  else
    myFirstItem = pto->Next();
  if (pto->Next())
    pto->Next()->SetPrevious (pfrom->Previous());
  else
    myLastItem = pfrom->Previous();
  
  mySize -= To - From + 1;
  if      (myCurrentIndex > To) 
    myCurrentIndex -= To - From + 1;
  else if (myCurrentIndex >= From) {
    if (pto->Next()) {
      myCurrentItem = pto->Next();
      myCurrentIndex = From;                      // AGV fix 24.05.01
    } else {
      myCurrentItem = myLastItem;
      myCurrentIndex = mySize;
    }
  }
  
  for (Standard_Integer i = From; i <= To; i++) {
    NCollection_SeqNode * tmp = pfrom;
    pfrom = pfrom->Next();
    fDel (tmp, myAllocator);
  }
}

//=======================================================================
//function : Find
//purpose  : 
//=======================================================================

NCollection_SeqNode * NCollection_BaseSequence::Find (const Standard_Integer theIndex) const 
{
  Standard_Integer i;
  NCollection_SeqNode * p;
  if (theIndex <= myCurrentIndex) {
    if (theIndex < myCurrentIndex / 2) {
      p = myFirstItem;
      for (i = 1; i < theIndex; i++)
        p = p->Next();
    } else {
      p = myCurrentItem;
      for (i = myCurrentIndex; i > theIndex; i--)
        p = p->Previous();
    }
  } else {
    if (theIndex < (myCurrentIndex + mySize) / 2) {
      p = myCurrentItem;
      for (i = myCurrentIndex; i < theIndex; i++)
        p = p->Next();
    } else {
      p = myLastItem;
      for (i = mySize; i > theIndex; i--)
        p = p->Previous();
    }
  }
  return p;
}
