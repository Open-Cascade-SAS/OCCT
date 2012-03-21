// Created on: 2002-04-17
// Created by: Alexander Kartomin (akm)
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


#ifndef NCollection_List_HeaderFile
#define NCollection_List_HeaderFile

#include <NCollection_TListIterator.hxx>

#if !defined No_Exception && !defined No_Standard_NoSuchObject
#include <Standard_NoSuchObject.hxx>
#endif

/**
 * Purpose:      Simple list to link  items together keeping the first 
 *               and the last one.
 *               Inherits BaseList, adding the data item to each node.
 */               
template <class TheItemType> class NCollection_List
  : public NCollection_BaseCollection<TheItemType>,
    public NCollection_BaseList
{
 public:
  typedef NCollection_TListNode<TheItemType>     ListNode;
  typedef NCollection_TListIterator<TheItemType> Iterator;

 public:
  // ---------- PUBLIC METHODS ------------

  //! Constructor
  NCollection_List(const Handle(NCollection_BaseAllocator)& theAllocator=0L) :
    NCollection_BaseCollection<TheItemType>(theAllocator),
    NCollection_BaseList() {}

  //! Copy constructor
  NCollection_List (const NCollection_List& theOther) :
    NCollection_BaseCollection<TheItemType>(theOther.myAllocator),
    NCollection_BaseList()
  { *this = theOther; }

  //! Size - Number of items
  virtual Standard_Integer Size (void) const
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
      ListNode* pNew = new (this->myAllocator) ListNode(anIter.Value());
      PAppend(pNew);
    }
  }

  //! Replace this list by the items of another list (theOther parameter)
  void Assign (const NCollection_List& theOther)
  {
    if (this != &theOther) {
      Clear();
      appendList(theOther.PFirst());
    }
  }

  //! Replace this list by the items of theOther list
  NCollection_List& operator= (const NCollection_List& theOther)
  { 
    if (this != &theOther) {
      Clear (theOther.myAllocator);
      appendList(theOther.PFirst());
    }
    return *this;
  }

  //! Clear this list
  void Clear (const Handle(NCollection_BaseAllocator)& theAllocator=0L)
  {
    PClear (ListNode::delNode, this->myAllocator);
    if (!theAllocator.IsNull())
      this->myAllocator = theAllocator;
  }

  //! First item
  const TheItemType& First (void) const
  {
#if !defined No_Exception && !defined No_Standard_NoSuchObject
    if (IsEmpty())
      Standard_NoSuchObject::Raise ("NCollection_List::First");
#endif
    return ((const ListNode *) PFirst())->Value();
  }

  //! Last item
  const TheItemType& Last (void) const
  { 
#if !defined No_Exception && !defined No_Standard_NoSuchObject
    if (IsEmpty())
      Standard_NoSuchObject::Raise ("NCollection_List::Last");
#endif
    return ((const ListNode *) PLast())->Value();
  }

  //! Append one item at the end
  TheItemType& Append (const TheItemType& theItem)
  { 
    ListNode * pNew = new (this->myAllocator) ListNode(theItem);
    PAppend(pNew);
    return ((ListNode *) PLast())->ChangeValue();
  }

  //! Append one item at the end and output iterator
  //!   pointing at the appended item
  void Append (const TheItemType& theItem, Iterator& theIter)
  { 
    ListNode * pNew = new (this->myAllocator) ListNode(theItem);
    PAppend(pNew, theIter);
  }

  //! Append another list at the end
  void Append (NCollection_List& theOther)
  { 
    if (this == &theOther || theOther.Extent()<1)
      return;
    if (this->myAllocator == theOther.myAllocator)
    {
      // Then we take the list and glue it to our end - 
      // deallocation will bring no problem
      PAppend(theOther);
    }
    else
    {
      // No - this list has different memory scope
      appendList(theOther.myFirst);
      theOther.Clear();
    }
  }

  //! Prepend one item at the beginning
  TheItemType& Prepend (const TheItemType& theItem)
  { 
    ListNode * pNew = new (this->myAllocator) ListNode(theItem);
    PPrepend(pNew);
    return ((ListNode *) PFirst())->ChangeValue();
  }

  //! Prepend another list at the beginning
  void Prepend (NCollection_List& theOther)
  { 
    if (this == &theOther || theOther.Extent()<1) 
      return;
    if (this->myAllocator == theOther.myAllocator)
    {
      // Then we take the list and glue it to our head - 
      // deallocation will bring no problem
      PPrepend(theOther);
    }
    else
    {
      // No - this list has different memory scope
      Iterator it(*this);
      prependList(theOther.PFirst(), it);
      theOther.Clear();
    }
  }

  //! RemoveFirst item
  void RemoveFirst (void) 
  { PRemoveFirst (ListNode::delNode, this->myAllocator); }

  //! Remove item
  void Remove (Iterator& theIter) 
  { 
    PRemove (theIter, ListNode::delNode, this->myAllocator); 
  }

  //! InsertBefore
  TheItemType& InsertBefore (const TheItemType& theItem,
                             Iterator& theIter) 
  { 
    ListNode * pNew = new (this->myAllocator) ListNode(theItem);
    PInsertBefore (pNew, theIter);
    return pNew -> ChangeValue();
  }

  //! InsertBefore
  void InsertBefore (NCollection_List& theOther,
                     Iterator& theIter) 
  {
    if (this == &theOther) 
      return;
  
    if (this->myAllocator == theOther.myAllocator)
    {
      // Then we take the list and glue it to our head - 
      // deallocation will bring no problem
      PInsertBefore (theOther, theIter);
    }
    else
    {
      // No - this list has different memory scope
      prependList(theOther.myFirst, theIter);
      theOther.Clear();
    }
  }

  //! InsertAfter
  TheItemType& InsertAfter (const TheItemType& theItem,
                            Iterator& theIter) 
  {
    ListNode * pNew = new (this->myAllocator) ListNode(theItem);
    PInsertAfter (pNew, theIter);
    return pNew -> ChangeValue();
  }

  //! InsertAfter
  void InsertAfter (NCollection_List& theOther,
                    Iterator& theIter) 
  {
    if (!theIter.More())
    {
      Append(theOther);
      return;
    }
    if (this->myAllocator == theOther.myAllocator)
    {
      // Then we take the list and glue it to our head - 
      // deallocation will bring no problem
      PInsertAfter (theOther, theIter);
    }
    else
    {
      // No - this list has different memory scope
#if !defined No_Exception && !defined No_Standard_NoSuchObject
      if (!theIter.More())
        Standard_NoSuchObject::Raise ("NCollection_List::InsertAfter");
#endif
      Iterator anIter;
      anIter.myPrevious = theIter.myCurrent;
      anIter.myCurrent = theIter.myCurrent->Next();
      prependList(theOther.PFirst(), anIter);
      theOther.Clear();
    }
  }

  //! Reverse the list
  void Reverse ()
  { PReverse(); }

  //! Destructor - clears the List
  ~NCollection_List (void)
  { Clear(); }

 private:
  // ----------- PRIVATE METHODS -----------

  //! Creates Iterator for use on BaseCollection
  virtual TYPENAME NCollection_BaseCollection<TheItemType>::Iterator& 
    CreateIterator(void) const
  { return *(new (this->IterAllocator()) Iterator(*this)); }

  //! append the list headed by the given ListNode
  void appendList(const NCollection_ListNode * pCur) {
    while (pCur) {
      NCollection_ListNode * pNew =
        new (this->myAllocator) ListNode(((const ListNode *)(pCur))->Value());
      PAppend(pNew);
      pCur = pCur->Next();
    }
  }

  //! insert the list headed by the given ListNode before the given iterator
  void prependList(const NCollection_ListNode * pCur, Iterator& theIter) {
    while (pCur) {
      NCollection_ListNode * pNew =
        new (this->myAllocator) ListNode (((const ListNode *)(pCur))->Value());
      PInsertBefore(pNew, theIter);
      pCur = pCur->Next();
    }
  }
};

#endif
