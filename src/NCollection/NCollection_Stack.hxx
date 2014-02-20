// Created on: 2002-04-17
// Created by: Alexander Kartomin (akm)
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

#ifndef NCollection_Stack_HeaderFile
#define NCollection_Stack_HeaderFile

#include <NCollection_BaseCollection.hxx>
#include <NCollection_BaseList.hxx>
#include <NCollection_TListNode.hxx>
#include <NCollection_TListIterator.hxx>
#if !defined No_Exception && !defined No_Standard_NoSuchObject
#include <Standard_NoSuchObject.hxx>
#endif

/**
 * Purpose:      A stack is a structure where item can be added and
 *               removed from the top. Like a stack of plates  in a
 *               kitchen. The   last entered item  will be   be the
 *               first  removed. This  is  called  a  LIFO (last In First Out).
 *               Inherits BaseList, adding the data item to each node.
 */               
template <class TheItemType> class NCollection_Stack
  : public NCollection_BaseCollection<TheItemType>,
    public NCollection_BaseList
{
 public:
  typedef NCollection_TListNode<TheItemType> StackNode;
  typedef NCollection_TListIterator<TheItemType> Iterator;

 public:
  // ---------- PUBLIC METHODS ------------

  //! Constructor
  NCollection_Stack(const Handle(NCollection_BaseAllocator)& theAllocator=0L) :
    NCollection_BaseCollection<TheItemType>(theAllocator),
    NCollection_BaseList() {}

  //! Copy constructor
  NCollection_Stack (const NCollection_Stack& theOther) :
    NCollection_BaseCollection<TheItemType>(theOther.myAllocator),
    NCollection_BaseList()
  { *this = theOther; }

  //! Size - Number of items
  virtual Standard_Integer Size (void) const
  { return Extent(); }

  //! Depth - Number of items
  Standard_Integer Depth (void) const
  { return Extent(); }

  //! Replace this list by the items of theOther collection
  virtual void Assign (const NCollection_BaseCollection<TheItemType>& theOther)
  {
    if (this == &theOther) 
      return;
    Clear();
    TYPENAME NCollection_BaseCollection<TheItemType>::Iterator& anIter = 
      theOther.CreateIterator();
    for (; anIter.More(); anIter.Next())
    {
      StackNode* pNew = new (this->myAllocator) StackNode(anIter.Value());
      PAppend(pNew);
    }
  }

  //! Replace this list by the items of theOther Stack
  NCollection_Stack& operator= (const NCollection_Stack& theOther)
  { 
    if (this == &theOther) 
      return *this;
    Clear ();
    StackNode * pCur = (StackNode *) theOther.PFirst();
    while (pCur)
    {
      StackNode* pNew = new (this->myAllocator) StackNode(pCur->Value());
      PAppend(pNew);
      pCur = (StackNode *) pCur->Next();
    }
    return *this;
  }

  //! Clear this stack
  void Clear (void)
  { PClear (StackNode::delNode, this->myAllocator); }

  //! Top item - constant
  const TheItemType& Top (void) const
  {
#if !defined No_Exception && !defined No_Standard_NoSuchObject
    if (IsEmpty()) Standard_NoSuchObject::Raise ("NCollection_Stack::Top");
#endif
    return ((StackNode *) PFirst())->Value();
  }

  //! Top item - variable
  TheItemType& ChangeTop (void)
  { 
#if !defined No_Exception && !defined No_Standard_NoSuchObject
    if (IsEmpty()) Standard_NoSuchObject::Raise("NCollection_Stack::ChangeTop");
#endif
    return ((StackNode *) PFirst())->ChangeValue();
  }

  //! Push one item
  void Push (const TheItemType& theItem)
  { 
    StackNode * pNew = new (this->myAllocator) StackNode(theItem);
    PPrepend(pNew);
  }

  //! Pop top item
  void Pop (void) 
  { PRemoveFirst (StackNode::delNode, this->myAllocator); }

  //! Destructor - clears the List
  ~NCollection_Stack (void)
  { Clear(); }

 private:
  // ----------- PRIVATE METHODS -----------

  //! Creates Iterator for use on BaseCollection
  virtual TYPENAME NCollection_BaseCollection<TheItemType>::Iterator& 
    CreateIterator(void) const
  { return *(new (this->IterAllocator()) Iterator(*this)); }

};

#endif
