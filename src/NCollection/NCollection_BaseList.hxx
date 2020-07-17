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

// Purpose:     This is a base  class  for the  List, Set, Queue  and Stack
//              collections. It offers operations on abstract lists (of the
//              objects of class NCollection_ListNode).
//              Apart from this class being  brand new (in TCollection said
//              collections were independent, only using the same class for
//              node representation),  here is an  important new  feature - 
//              the  list  length is  continuously updated,  so the  method 
//              Extent is quite quick.

#ifndef NCollection_BaseList_HeaderFile
#define NCollection_BaseList_HeaderFile

#include <Standard_NoSuchObject.hxx>
#include <NCollection_DefineAlloc.hxx>
#include <NCollection_ListNode.hxx>

typedef void (* NCollection_DelListNode) 
     (NCollection_ListNode*, Handle(NCollection_BaseAllocator)& theAl);

// ********************************************************** BaseList class
class NCollection_BaseList
{
public:
  //! Memory allocation
  DEFINE_STANDARD_ALLOC
  DEFINE_NCOLLECTION_ALLOC

public:
  class Iterator
  {
  public:
    // ******** Empty constructor
    Iterator  (void) :
      myCurrent (NULL),
      myPrevious(NULL) {}
    // ******** Constructor with initialisation
    Iterator  (const NCollection_BaseList& theList) :
      myCurrent (theList.myFirst),
      myPrevious(NULL) {}
    // ******** Initialisation
    void Init (const NCollection_BaseList& theList)
    {
      myCurrent  = theList.myFirst;
      myPrevious = NULL;
    }
    // ******** Initialisation
    void Initialize (const NCollection_BaseList& theList)
    {
      Init(theList);
    }
    // ******** More
    Standard_Boolean More (void) const
    { return (myCurrent!=NULL); }

    // ******** Comparison operator
    Standard_Boolean operator== (const Iterator& theIt) const
    {
      return myCurrent == theIt.myCurrent;
    }

    //! Performs comparison of two iterators
    Standard_Boolean IsEqual (const Iterator& theOther) const
    {
      return *this == theOther;
    }
  protected:
    void Init (const NCollection_BaseList& theList,
               NCollection_ListNode * const thePrev)
    {
      myCurrent  = thePrev ? thePrev -> Next() :
                             (NCollection_ListNode *)theList.PLast();
      myPrevious = thePrev;
    }
  public:
    NCollection_ListNode * myCurrent; // Pointer to the current node
    NCollection_ListNode * myPrevious;// Pointer to the previous one
    friend class NCollection_BaseList;
  }; // End of nested class Iterator

 public:
  // ---------- PUBLIC METHODS ------------
  // ******** Extent
  // Purpose: Returns the number of nodes in the list
  Standard_Integer Extent (void) const
  { return myLength; }

  // ******** IsEmpty
  // Purpose: Query if the list is empty
  Standard_Boolean IsEmpty (void) const
  { return (myFirst == NULL); }

  // ******** Allocator
  //! Returns attached allocator
  const Handle(NCollection_BaseAllocator)& Allocator() const 
  { return myAllocator; }

  // ******** Destructor
  // Purpose: defines virtual interface
  virtual ~NCollection_BaseList (void)
  {}

 protected:
  // --------- PROTECTED METHODS ----------

  // ******** Constructor
  // Purpose: Initializes an empty list
  NCollection_BaseList (const Handle(NCollection_BaseAllocator)& theAllocator=0L) :
    myFirst(NULL),
    myLast(NULL),
    myLength(0)
  {
    myAllocator = (theAllocator.IsNull() ? NCollection_BaseAllocator::CommonBaseAllocator() : theAllocator);
  }

  // ******** PClear
  // Purpose: deletes all nodes
  Standard_EXPORT void PClear (NCollection_DelListNode fDel);

  // ******** PFirst
  // Purpose: Returns pointer to the first node
  const NCollection_ListNode* PFirst (void) const
  { return myFirst; }

  // ******** PLast
  // Purpose: Returns pointer to the last node
  const NCollection_ListNode* PLast (void) const
  { return myLast; }

  // ******** PAppend
  // Purpose: Appends theNode at the end
  Standard_EXPORT void PAppend (NCollection_ListNode* theNode);

  // ******** PAppend
  // Purpose: Appends theNode at the end, returns iterator to the previous
  void                 PAppend (NCollection_ListNode* theNode,
                                Iterator&             theIt)
  {
    NCollection_ListNode * aPrev = myLast;
    PAppend (theNode);
    theIt.Init (* this, aPrev);
  }

  // ******** PAppend
  // Purpose: Appends theOther list at the end (clearing it)
  Standard_EXPORT void PAppend (NCollection_BaseList& theOther);

  // ******** PPrepend
  // Purpose: Prepends theNode at the beginning
  Standard_EXPORT void PPrepend (NCollection_ListNode* theNode);

  // ******** PPrepend
  // Purpose: Prepends theOther list at the beginning (clearing it)
  Standard_EXPORT void PPrepend (NCollection_BaseList& theOther);

  // ******** PRemoveFirst
  // Purpose: Removes first node
  Standard_EXPORT void PRemoveFirst 
    (NCollection_DelListNode fDel);

  // ******** PRemove
  // Purpose: Removes the node pointed by theIter[ator]
  Standard_EXPORT void PRemove 
    (Iterator& theIter,
     NCollection_DelListNode fDel);

  // ******** PInsertBefore
  // Purpose: Inserts theNode before one pointed by theIter[ator]
  Standard_EXPORT void PInsertBefore (NCollection_ListNode* theNode,
                                      Iterator& theIter);

  // ******** PInsertBefore
  // Purpose: Inserts theOther list before the node pointed by theIter[ator]
  Standard_EXPORT void PInsertBefore (NCollection_BaseList& theOther,
                                      Iterator& theIter);

  // ******** PInsertAfter
  // Purpose: Inserts theNode after one pointed by theIter[ator]
  Standard_EXPORT void PInsertAfter (NCollection_ListNode* theNode,
                                     Iterator& theIter);

  // ******** PInsertAfter
  // Purpose: Inserts theOther list after the node pointed by theIter[ator]
  Standard_EXPORT void PInsertAfter (NCollection_BaseList& theOther,
                                     Iterator& theIter);

  // ******** PReverse
  // Purpose: Reverse the list
  Standard_EXPORT void PReverse     ();

 protected:
  // ------------ PROTECTED FIELDS ------------
  Handle(NCollection_BaseAllocator) myAllocator;
  NCollection_ListNode * myFirst;  // Pointer to the head
  NCollection_ListNode * myLast;   // Pointer to the tail
  Standard_Integer       myLength; // Actual length

  // ------------ FRIEND CLASSES ------------
  friend class Iterator;
};

#endif
