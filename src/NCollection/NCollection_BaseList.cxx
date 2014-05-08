// Created on: 2002-04-23
// Created by: Alexander KARTOMIN (akm)
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

// Purpose:     Implementation of the BaseList class

#include <NCollection_BaseList.hxx>

//=======================================================================
//function : PClear
//purpose  : Deletes all nodes from the list
//=======================================================================

void NCollection_BaseList::PClear (NCollection_DelListNode fDel)
{ 
  NCollection_ListNode* pCur  = myFirst;
  NCollection_ListNode* pNext = NULL;
  while(pCur) 
  {
    pNext = pCur->Next();
    fDel (pCur, myAllocator);
    pCur = pNext;
  }
  myLength = 0;
  myFirst = myLast = NULL;
}

//=======================================================================
//function : PAppend
//purpose  : Appends one item at the end
//=======================================================================

void NCollection_BaseList::PAppend (NCollection_ListNode* theNode)
{ 
  if (myLength) 
    myLast->Next() = theNode;
  else 
    myFirst = theNode;
  theNode->Next() = NULL;
  myLast = theNode;
  myLength++;
}

//=======================================================================
//function : PAppend
//purpose  : Appends another list at the end
//=======================================================================

void NCollection_BaseList::PAppend (NCollection_BaseList& theOther)
{ 
  if (this == &theOther || theOther.IsEmpty()) 
    return;
  
  if (IsEmpty())
    myFirst = theOther.myFirst;
  else
    myLast->Next() = theOther.myFirst;
  myLast = theOther.myLast;
  theOther.myFirst = theOther.myLast = NULL;

  myLength += theOther.myLength;
  theOther.myLength = 0;
}

//=======================================================================
//function : PPrepend
//purpose  : Prepends one item at the beginning
//=======================================================================

void NCollection_BaseList::PPrepend (NCollection_ListNode* theNode)
{ 
  theNode->Next() = myFirst;
  myFirst = theNode;
  if (myLast==NULL)
    myLast = myFirst;
  myLength++;
}

//=======================================================================
//function : PPrepend
//purpose  : 
//=======================================================================

void NCollection_BaseList::PPrepend (NCollection_BaseList& theOther)
{ 
  if (this == &theOther || theOther.IsEmpty()) 
    return;

  if (IsEmpty())
    myLast = theOther.myLast;
  else
    theOther.myLast->Next() = myFirst;
  myFirst = theOther.myFirst;
  theOther.myFirst = theOther.myLast = NULL;

  myLength += theOther.myLength;
  theOther.myLength = 0;
}

//=======================================================================
//function : PRemoveFirst
//purpose  : 
//=======================================================================

void NCollection_BaseList::PRemoveFirst (NCollection_DelListNode fDel) 
{
  Standard_NoSuchObject_Raise_if(IsEmpty(),
                                 "NCollection_BaseList::PRemoveFirst");
  NCollection_ListNode* pItem = myFirst;
  myFirst = pItem->Next();
  fDel (pItem, myAllocator);
  myLength--;
  if (myLength == 0) 
    myLast = NULL;
}

//=======================================================================
//function : PRemove
//purpose  : 
//=======================================================================

void NCollection_BaseList::PRemove (Iterator& theIter, NCollection_DelListNode fDel) 
{
  Standard_NoSuchObject_Raise_if(!theIter.More(),
                                 "NCollection_BaseList::PRemove");
  if (theIter.myPrevious == NULL) 
  {
    PRemoveFirst (fDel);
    theIter.myCurrent = myFirst;
  }
  else 
  {
    NCollection_ListNode* pNode = (theIter.myCurrent)->Next();
    (theIter.myPrevious)->Next() = pNode;
    fDel (theIter.myCurrent, myAllocator);
    theIter.myCurrent = pNode;
    if (pNode == NULL) 
      myLast = theIter.myPrevious;
    myLength--;
  }
}

//=======================================================================
//function : PInsertBefore
//purpose  : 
//=======================================================================

void NCollection_BaseList::PInsertBefore (NCollection_ListNode* theNode,
                                          Iterator& theIter)
{
  Standard_NoSuchObject_Raise_if(!theIter.More(),
                                 "NCollection_BaseList::PInsertBefore");
  if (theIter.myPrevious == NULL) 
  {
    PPrepend(theNode);
    theIter.myPrevious = myFirst;
  }
  else 
  {
    (theIter.myPrevious)->Next() = theNode;
    theNode->Next() = theIter.myCurrent;
    theIter.myPrevious = theNode;
    myLength++;
  }
}

//=======================================================================
//function : PInsertBefore
//purpose  : 
//=======================================================================

void NCollection_BaseList::PInsertBefore (NCollection_BaseList& theOther,
                                          Iterator& theIter)
{
  Standard_NoSuchObject_Raise_if(!theIter.More(),
                                 "NCollection_BaseList::PInsertBefore");
  if (theIter.myPrevious == NULL) 
  {
    theIter.myPrevious = theOther.myLast;
    PPrepend(theOther);
  }
  else if (!theOther.IsEmpty())
  {
    myLength += theOther.myLength;
    (theIter.myPrevious)->Next() = theOther.myFirst;
    (theOther.myLast)->Next() = theIter.myCurrent;
    theIter.myPrevious = theOther.myLast;
    theOther.myLast = theOther.myFirst = NULL;
    theOther.myLength = 0;
  }
}

//=======================================================================
//function : PInsertAfter
//purpose  : 
//=======================================================================

void NCollection_BaseList::PInsertAfter (NCollection_ListNode* theNode,
                                         Iterator& theIter)
{
  Standard_NoSuchObject_Raise_if(!theIter.More(),
                                 "NCollection_BaseList::PInsertAfter");
  if (theIter.myCurrent == myLast)
  {
    PAppend(theNode);
  }
  else
  {
    theNode->Next() = (theIter.myCurrent)->Next();
    (theIter.myCurrent)->Next() = theNode;
    myLength++;
  }
}

//=======================================================================
//function : PInsertAfter
//purpose  : 
//=======================================================================

void NCollection_BaseList::PInsertAfter(NCollection_BaseList& theOther,
                                        Iterator& theIter)
{
  Standard_NoSuchObject_Raise_if(!theIter.More(),
                                 "NCollection_BaseList::PInsertAfter");
  if (theIter.myCurrent == myLast)
  {
    PAppend(theOther);
  }
  else if (!theOther.IsEmpty())
  {
    myLength += theOther.myLength;
    (theOther.myLast)->Next() = (theIter.myCurrent)->Next();
    (theIter.myCurrent)->Next() = theOther.myFirst;
    theOther.myLast = theOther.myFirst = NULL;
    theOther.myLength = 0;
  }
}

//=======================================================================
//function : PReverse
//purpose  : reverse the list
//=======================================================================

void NCollection_BaseList::PReverse ()
{
  if (myLength > 1) {
    NCollection_ListNode * aHead = myFirst->Next();
    NCollection_ListNode * aNeck = myFirst;
    aNeck->Next() = NULL;
    while (aHead) {
      NCollection_ListNode * aTmp = aHead->Next();
      aHead->Next() = aNeck;
      aNeck = aHead;
      aHead = aTmp;
    }
    myLast  = myFirst;
    myFirst = aNeck;
  }
}
