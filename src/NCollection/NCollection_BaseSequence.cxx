// Created on: 2002-03-29
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

// Purpose:   Implementation of the BaseSequence class

#include <NCollection_BaseSequence.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_DomainError.hxx>

//=======================================================================
//function : ClearSeq
//purpose  : removes all items from the current sequence
//=======================================================================

void NCollection_BaseSequence::ClearSeq 
  (NCollection_DelSeqNode fDel, Handle(NCollection_BaseAllocator)& theAl)
{
  const NCollection_SeqNode * p = myFirstItem;
  NCollection_SeqNode * q;
  while (p) {
    q = (NCollection_SeqNode *) p;
    p = p->Next();
    fDel (q, theAl);
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
    ((NCollection_SeqNode *) myLastItem)->SetNext(theItem);
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
  if (mySize == 0) {
    mySize         = Other.mySize;
    myFirstItem    = Other.myFirstItem;
    myLastItem     = Other.myLastItem;
    myCurrentItem  = myFirstItem;
    myCurrentIndex = 1;
  } else {
    mySize += Other.mySize;
    ((NCollection_SeqNode *) myLastItem)->SetNext(Other.myFirstItem);
    if (Other.myFirstItem) {
      ((NCollection_SeqNode *) Other.myFirstItem)->SetPrevious(myLastItem);
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
    ((NCollection_SeqNode *) myFirstItem)->SetPrevious (theItem);
    ((NCollection_SeqNode *) theItem)->SetNext (myFirstItem); 
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
  if (mySize == 0) {
    mySize         = Other.mySize;
    myFirstItem    = Other.myFirstItem;
    myLastItem     = Other.myLastItem;
    myCurrentIndex = 1;
    myCurrentItem  = myFirstItem;
  } else {
    mySize += Other.mySize;
    if (Other.myLastItem)
      ((NCollection_SeqNode *) Other.myLastItem)->SetNext (myFirstItem);
    ((NCollection_SeqNode *) myFirstItem)->SetPrevious(Other.myLastItem);
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
  const NCollection_SeqNode * p = myFirstItem;
  const NCollection_SeqNode * tmp;
  while (p) {
    tmp = p->Next();
    ((NCollection_SeqNode *) p)->SetNext (p->Previous());
    ((NCollection_SeqNode *) p)->SetPrevious (tmp);
    p = tmp;
  }
  tmp = myFirstItem;
  myFirstItem = myLastItem;
  myLastItem = tmp;
  if (mySize != 0) myCurrentIndex = mySize + 1 - myCurrentIndex;
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
    if (aPos->Next() == NULL) myLastItem = theItem;
    else ((NCollection_SeqNode *) aPos->Next())->SetPrevious(theItem);
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
    const NCollection_SeqNode * p = Find (theIndex);
    theItem->SetNext(p->Next());
    theItem->SetPrevious(p);
    if (theIndex == mySize) myLastItem = theItem;
    else ((NCollection_SeqNode *) p->Next())->SetPrevious(theItem);
    ((NCollection_SeqNode *) p)->SetNext(theItem);
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
    Standard_OutOfRange::Raise();
  if (Other.mySize != 0) {
    if (theIndex == 0) 
      PPrepend (Other);
    else {
      const NCollection_SeqNode * p = Find (theIndex);
      ((NCollection_SeqNode *) Other.myFirstItem)->SetPrevious (p);
      ((NCollection_SeqNode *) Other.myLastItem)->SetNext (p->Next());
      if (theIndex == mySize)
        myLastItem = Other.myLastItem;
      else
        ((NCollection_SeqNode *) p->Next())->SetPrevious (Other.myLastItem);
      ((NCollection_SeqNode *) p)->SetNext (Other.myFirstItem);
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
    const NCollection_SeqNode * pi = Find(I);
    const NCollection_SeqNode * pj = Find(J);

    // update the node before I
    if (pi->Previous())
      ((NCollection_SeqNode *) pi->Previous())->SetNext (pj);
    else 
      myFirstItem = pj;

    // update the node after J
    if (pj->Next())
      ((NCollection_SeqNode *) pj->Next())->SetPrevious(pi);
    else
      myLastItem = pi;

    if (pi->Next() == pj) {          // I and J are consecutives, update them
      ((NCollection_SeqNode *) pj)->SetPrevious (pi->Previous());
      ((NCollection_SeqNode *) pi)->SetPrevious (pj);
      ((NCollection_SeqNode *) pi)->SetNext (pj->Next());
      ((NCollection_SeqNode *) pj)->SetNext (pi);
    }
    else {                        // I and J are not consecutive
      // update the node after I
      ((NCollection_SeqNode *) pi->Next())->SetPrevious (pj);
      // update the node before J
      ((NCollection_SeqNode *) pj->Previous())->SetNext (pi);
      // update nodes I and J
      const NCollection_SeqNode* tmp = pi->Next();       
      ((NCollection_SeqNode *) pi)->SetNext (pj->Next());
      ((NCollection_SeqNode *) pj)->SetNext (tmp);
      tmp = pi->Previous();
      ((NCollection_SeqNode *) pi)->SetPrevious (pj->Previous());
      ((NCollection_SeqNode *) pj)->SetPrevious (tmp);
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

  const NCollection_SeqNode * p = Find (theIndex);

  Sub.myLastItem = myLastItem;
  Sub.mySize = mySize - theIndex + 1;

  myLastItem = p->Previous();
  if (myLastItem) {
    ((NCollection_SeqNode *) myLastItem)->SetNext(NULL);
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
  ((NCollection_SeqNode *) p)->SetPrevious (NULL);
  Sub.myCurrentIndex = 1;
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void NCollection_BaseSequence::RemoveSeq 
                              (NCollection_BaseSequence::Iterator& thePosition,
                               NCollection_DelSeqNode              fDel, 
                               Handle(NCollection_BaseAllocator)&  theAl)
{
  NCollection_SeqNode * aPos = thePosition.myCurrent;
  if (aPos == NULL)
    return;
  thePosition.myCurrent = (NCollection_SeqNode *) aPos -> Next();

  if (aPos->Previous())
    ((NCollection_SeqNode *) aPos->Previous())->SetNext (aPos->Next());
  else
    myFirstItem = aPos->Next();

  if (aPos->Next())
    ((NCollection_SeqNode *) aPos->Next())->SetPrevious (aPos->Previous());
  else
    myLastItem = aPos->Previous();

  -- mySize;
  myCurrentItem  = myLastItem;
  myCurrentIndex = mySize;

  fDel (aPos, theAl);
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void NCollection_BaseSequence::RemoveSeq 
                              (const Standard_Integer theIndex,
                               NCollection_DelSeqNode fDel, 
                               Handle(NCollection_BaseAllocator)& theAl)
{
  Standard_OutOfRange_Raise_if (theIndex <= 0 || theIndex > mySize, "");
  
  const NCollection_SeqNode * p = Find (theIndex);
  if (p->Previous())
    ((NCollection_SeqNode *) p->Previous())->SetNext (p->Next());
  else
    myFirstItem = p->Next();
  if (p->Next())
    ((NCollection_SeqNode *) p->Next())->SetPrevious (p->Previous());
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
  fDel ((NCollection_SeqNode *) p, theAl);
}

//=======================================================================
//function : Remove
//purpose  : remove a set of items
//=======================================================================

void NCollection_BaseSequence::RemoveSeq 
                              (const Standard_Integer From,
                               const Standard_Integer To, 
                               NCollection_DelSeqNode fDel,
                               Handle(NCollection_BaseAllocator)& theAl)
{
  Standard_OutOfRange_Raise_if (From <= 0 || To > mySize || From > To, "");

  const NCollection_SeqNode * pfrom = Find(From);
  const NCollection_SeqNode * pto   = Find(To);
  
  if (pfrom->Previous())
    ((NCollection_SeqNode *) pfrom->Previous())->SetNext (pto->Next());
  else
    myFirstItem = pto->Next();
  if (pto->Next())
    ((NCollection_SeqNode *) pto->Next())->SetPrevious (pfrom->Previous());
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
    NCollection_SeqNode * tmp = (NCollection_SeqNode *)pfrom;
    pfrom = pfrom->Next();
    fDel (tmp, theAl);
  }
}

//=======================================================================
//function : Find
//purpose  : 
//=======================================================================

const NCollection_SeqNode * NCollection_BaseSequence::Find
                                        (const Standard_Integer theIndex) const 
{
  Standard_Integer i;
  const NCollection_SeqNode * p;
  if (theIndex <= myCurrentIndex) {
    if (theIndex < myCurrentIndex / 2) {
      p = myFirstItem;
      for (i = 1; i < theIndex; i++) p = p->Next();
    } else {
      p = myCurrentItem;
      for (i = myCurrentIndex; i > theIndex; i--) p = p->Previous();
    }
  } else {
    if (theIndex < (myCurrentIndex + mySize) / 2) {
      p = myCurrentItem;
      for (i = myCurrentIndex; i < theIndex; i++) p = p->Next();
    } else {
      p = myLastItem;
      for (i = mySize; i > theIndex; i--) p = p->Previous();
    }
  }
  return p;
}
