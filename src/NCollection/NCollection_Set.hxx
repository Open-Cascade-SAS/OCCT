// Created on: 2002-04-17
// Created by: Alexander Kartomin (akm)
// Copyright (c) 2002-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef NCollection_Set_HeaderFile
#define NCollection_Set_HeaderFile

#include <NCollection_BaseCollection.hxx>
#include <NCollection_BaseList.hxx>
#include <NCollection_TListNode.hxx>
#include <NCollection_TListIterator.hxx>

/**
 * Purpose:      A set is an  unordered  collection  of items without
 *               duplications. To test for duplications the operators == and !=
 *               are used on the items.
 *               Inherits BaseList, adding the data item to each node.
 */
template <class TheItemType> class NCollection_Set
  : public NCollection_BaseCollection<TheItemType>,
    public NCollection_BaseList
{
 public:
  typedef NCollection_TListNode<TheItemType> SetNode;
  typedef NCollection_TListIterator<TheItemType> Iterator;

 public:
  // ---------- PUBLIC METHODS ------------

  //! Constructor
  NCollection_Set(const Handle(NCollection_BaseAllocator)& theAllocator=0L) :
    NCollection_BaseCollection<TheItemType>(theAllocator),
    NCollection_BaseList() {}

  //! Copy constructor
  NCollection_Set (const NCollection_Set& theOther) :
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
      SetNode* pNew = new (this->myAllocator) SetNode(anIter.Value());
      PAppend (pNew);
    }
  }

  //! Replace this list by the items of theOther Set
  NCollection_Set& operator= (const NCollection_Set& theOther)
  { 
    if (this == &theOther) 
      return *this;
    Clear ();
    SetNode * pCur = (SetNode *) theOther.PFirst();
    while (pCur)
    {
      SetNode* pNew = new (this->myAllocator) SetNode(pCur->Value());
      PAppend (pNew);
      pCur = (SetNode *) pCur->Next();
    }
    return *this;
  }

  //! Clear this set
  void Clear (void)
  { PClear (SetNode::delNode, this->myAllocator); }

  //! Add item
  Standard_Boolean Add (const TheItemType& theItem)
  { 
    Iterator anIter(*this);
    while (anIter.More())
    {
      if (anIter.Value() == theItem)
        return Standard_False;
      anIter.Next();
    }
    SetNode * pNew = new (this->myAllocator) SetNode(theItem);
    PPrepend (pNew);
    return Standard_True;
  }

  //! Remove item
  Standard_Boolean Remove (const TheItemType& theItem)
  {
    Iterator anIter(*this);
    while (anIter.More())
    {
      if (anIter.Value() == theItem)
      {
        PRemove (anIter, SetNode::delNode, this->myAllocator);
        return Standard_True;
      }
      anIter.Next();
    }
    return Standard_False;
  }

  //! Remove - wrapper against 'hiding' warnings
  void Remove (Iterator& theIter) 
  { NCollection_BaseList::PRemove (theIter,
                                   SetNode::delNode,
                                   this->myAllocator); }

  //! Contains - item inclusion query
  Standard_Boolean Contains (const TheItemType& theItem) const
  {
    Iterator anIter(*this);
    for (; anIter.More(); anIter.Next())
      if (anIter.Value() == theItem)
        return Standard_True;
    return Standard_False;
  }

  //! IsASubset
  Standard_Boolean IsASubset (const NCollection_Set& theOther)
  { 
    if (this == &theOther) 
      return Standard_True;
    Iterator anIter(theOther);
    for (; anIter.More(); anIter.Next())
      if (!Contains(anIter.Value()))
        return Standard_False;
    return Standard_True;
  }

  //! IsAProperSubset
  Standard_Boolean IsAProperSubset (const NCollection_Set& theOther)
  {
    if (myLength <= theOther.Extent())
      return Standard_False;
    Iterator anIter(theOther);
    for (; anIter.More(); anIter.Next())
      if (!Contains(anIter.Value()))
        return Standard_False;
    return Standard_True;
  }

  //! Union
  void Union (const NCollection_Set& theOther)
  { 
    if (this == &theOther) 
      return;
    Iterator anIter(theOther);
    Iterator aMyIter;
    Standard_Integer i, iLength=myLength;
    for (; anIter.More(); anIter.Next())
    {
      Standard_Boolean isIn=Standard_False;
      const TheItemType& theItem = anIter.Value();
      for (aMyIter.Init(*this), i=1; 
           i<=iLength;
           aMyIter.Next(), i++)
        if (theItem == aMyIter.Value())
          isIn = Standard_True;
      if (!isIn)
      {
        SetNode * pNew = new (this->myAllocator) SetNode(theItem);
        PAppend (pNew);
      }
    }
  }

  //! Intersection
  void Intersection (const NCollection_Set& theOther)
  { 
    if (this == &theOther) 
      return;
    Iterator anIter(*this);
    while (anIter.More())
      if (theOther.Contains(anIter.Value()))
        anIter.Next();
      else
        NCollection_BaseList::PRemove (anIter, SetNode::delNode, this->myAllocator);
  }

  //! Difference (Subtraction)
  void Difference (const NCollection_Set& theOther)
  { 
    if (this == &theOther) 
      return;
    Iterator anIter(*this);
    while (anIter.More())
      if (theOther.Contains(anIter.Value()))
        NCollection_BaseList::PRemove (anIter, SetNode::delNode, this->myAllocator);
      else
        anIter.Next();
  }

  //! Destructor - clears the List
  ~NCollection_Set (void)
  { Clear(); }

 private:
  // ----------- PRIVATE METHODS -----------

  //! Creates Iterator for use on BaseCollection
  virtual TYPENAME NCollection_BaseCollection<TheItemType>::Iterator& 
    CreateIterator(void) const
  { return *(new (this->IterAllocator()) Iterator(*this)); }

};

#endif
