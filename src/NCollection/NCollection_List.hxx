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

#ifndef NCollection_List_HeaderFile
#define NCollection_List_HeaderFile

#include <NCollection_TListIterator.hxx>
#include <NCollection_StlIterator.hxx>

#include <Standard_NoSuchObject.hxx>

/**
 * Purpose:      Simple list to link  items together keeping the first 
 *               and the last one.
 *               Inherits BaseList, adding the data item to each node.
 */               
template <class TheItemType>
class NCollection_List : public NCollection_BaseList
{
public:
  //! STL-compliant typedef for value type
  typedef TheItemType value_type;

public:
  typedef NCollection_TListNode<TheItemType>     ListNode;
  typedef NCollection_TListIterator<TheItemType> Iterator;

  //! Shorthand for a regular iterator type.
  typedef NCollection_StlIterator<std::forward_iterator_tag, Iterator, TheItemType, false> iterator;

  //! Shorthand for a constant iterator type.
  typedef NCollection_StlIterator<std::forward_iterator_tag, Iterator, TheItemType, true> const_iterator;

  //! Returns an iterator pointing to the first element in the list.
  iterator begin() const { return Iterator (*this); }

  //! Returns an iterator referring to the past-the-end element in the list.
  iterator end() const { return Iterator(); }

  //! Returns a const iterator pointing to the first element in the list.
  const_iterator cbegin() const { return Iterator (*this); }

  //! Returns a const iterator referring to the past-the-end element in the list.
  const_iterator cend() const { return Iterator(); }

 public:
  // ---------- PUBLIC METHODS ------------

  //! Empty constructor.
  NCollection_List() : NCollection_BaseList(Handle(NCollection_BaseAllocator)()) {}

  //! Constructor
  explicit NCollection_List(const Handle(NCollection_BaseAllocator)& theAllocator) : NCollection_BaseList(theAllocator) {}

  //! Copy constructor
  NCollection_List (const NCollection_List& theOther) :
    NCollection_BaseList(theOther.myAllocator)
  {
    Assign (theOther);
  }

  //! Size - Number of items
  Standard_Integer Size (void) const
  { return Extent(); }

  //! Replace this list by the items of another list (theOther parameter).
  //! This method does not change the internal allocator.
  NCollection_List& Assign (const NCollection_List& theOther)
  {
    if (this != &theOther) {
      Clear();
      appendList(theOther.PFirst());
    }
    return *this;
  }

  //! Replacement operator
  NCollection_List& operator= (const NCollection_List& theOther)
  {
    return Assign (theOther);
  }

  //! Clear this list
  void Clear (const Handle(NCollection_BaseAllocator)& theAllocator=0L)
  {
    PClear (ListNode::delNode);
    if (!theAllocator.IsNull())
      this->myAllocator = theAllocator;
  }

  //! First item
  const TheItemType& First (void) const
  {
    Standard_NoSuchObject_Raise_if (IsEmpty(), "NCollection_List::First");
    return ((const ListNode *) PFirst())->Value();
  }

  //! First item (non-const)
  TheItemType& First (void)
  {
    Standard_NoSuchObject_Raise_if (IsEmpty(), "NCollection_List::First");
    return ((ListNode *) PFirst())->ChangeValue();
  }

  //! Last item
  const TheItemType& Last (void) const
  { 
    Standard_NoSuchObject_Raise_if (IsEmpty(), "NCollection_List::Last");
    return ((const ListNode *) PLast())->Value();
  }

  //! Last item (non-const)
  TheItemType& Last (void)
  { 
    Standard_NoSuchObject_Raise_if (IsEmpty(), "NCollection_List::Last");
    return ((ListNode *) PLast())->ChangeValue();
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

  //! Append another list at the end.
  //! After this operation, theOther list will be cleared.
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
  { PRemoveFirst (ListNode::delNode); }

  //! Remove item pointed by iterator theIter; 
  //! theIter is then set to the next item
  void Remove (Iterator& theIter) 
  { 
    PRemove (theIter, ListNode::delNode); 
  }

  //! Remove the first occurrence of the object.
  template<typename TheValueType> // instantiate this method on first call only for types defining equality operator
  Standard_Boolean Remove (const TheValueType& theObject)
  {
    for (Iterator anIter (*this); anIter.More(); anIter.Next())
    {
      if (anIter.Value() == theObject)
      {
        Remove (anIter);
        return Standard_True;
      }
    }
    return Standard_False;
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

  //! Return true if object is stored in the list.
  template<typename TheValueType> // instantiate this method on first call only for types defining equality operator
  Standard_Boolean Contains (const TheValueType& theObject) const
  {
    for (Iterator anIter (*this); anIter.More(); anIter.Next())
    {
      if (anIter.Value() == theObject)
      {
        return Standard_True;
      }
    }
    return Standard_False;
  }

  //! Destructor - clears the List
  virtual ~NCollection_List (void)
  { Clear(); }

 private:
  // ----------- PRIVATE METHODS -----------

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
