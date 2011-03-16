// File:         NCollection_Queue.hxx
// Created:      17.04.02 10:12:48
// Author:       Alexander Kartomin (akm)
//               <a-kartomin@opencascade.com>
// Copyright:    Open Cascade 2002

#ifndef NCollection_Queue_HeaderFile
#define NCollection_Queue_HeaderFile

#include <NCollection_TListIterator.hxx>

#if !defined No_Exception && !defined No_Standard_NoSuchObject
#include <Standard_NoSuchObject.hxx>
#endif

#ifdef WNT
// Disable the warning "operator new unmatched by delete"
#pragma warning (disable:4291)
#endif

/**
 * Purpose:      A queue is  a structure where Items are  added  at
 *               the end  and removed from   the  front. The  first
 *               entered Item  will be the  first removed. This  is
 *               called a FIFO (First In First Out).
 *               Inherits BaseList, adds the data item to each node.
 */               
template <class TheItemType> class NCollection_Queue
  : public NCollection_BaseCollection<TheItemType>,
    public NCollection_BaseList
{
 public:
  typedef NCollection_TListNode<TheItemType> QueueNode;
  typedef NCollection_TListIterator<TheItemType> Iterator;

 public:
  // ---------- PUBLIC METHODS ------------

  //! Constructor
  NCollection_Queue(const Handle(NCollection_BaseAllocator)& theAllocator=0L) :
    NCollection_BaseCollection<TheItemType>(theAllocator),
    NCollection_BaseList() {}

  //! Copy constructor
  NCollection_Queue (const NCollection_Queue& theOther) :
    NCollection_BaseCollection<TheItemType>(theOther.myAllocator),
    NCollection_BaseList()
  { *this = theOther; }

  //! Size - Number of items
  virtual Standard_Integer Size (void) const
  { return Extent(); }

  //! Length - number of items
  Standard_Integer Length (void) const
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
      QueueNode* pNew = new (this->myAllocator) QueueNode(anIter.Value());
      PAppend(pNew);
    }
  }

  //! Replace this list by the items of theOther queue
  NCollection_Queue& operator= (const NCollection_Queue& theOther)
  { 
    if (this == &theOther) 
      return *this;
    Clear ();
    QueueNode * pCur = (QueueNode *) theOther.PFirst();
    while (pCur)
    {
      QueueNode* pNew = new (this->myAllocator) QueueNode(pCur->Value());
      PAppend(pNew);
      pCur = (QueueNode *) pCur->Next();
    }
    return *this;
  }

  //! Clear this queue
  void Clear (void)
  { PClear (QueueNode::delNode, this->myAllocator); }

  //! Frontal item - constant
  const TheItemType& Front (void) const
  {
#if !defined No_Exception && !defined No_Standard_NoSuchObject
    if (IsEmpty())
      Standard_NoSuchObject::Raise ("NCollection_Queue::Front");
#endif
    return ((const QueueNode *) PFirst())->Value(); 
  }

  //! Frontal item - variable
  TheItemType& ChangeFront (void)
  {
#if !defined No_Exception && !defined No_Standard_NoSuchObject
    if (IsEmpty())
      Standard_NoSuchObject::Raise ("NCollection_Queue::ChangeFront");
#endif
    return ((QueueNode *) PFirst())->ChangeValue();
  }

  //! Push one item
  void Push (const TheItemType& theItem)
  { 
    QueueNode * pNew = new (this->myAllocator) QueueNode(theItem);
    PAppend(pNew);
  }

  //! Pop first item
  void Pop (void) 
  { PRemoveFirst (QueueNode::delNode, this->myAllocator); }

  //! Destructor - clears the List
  ~NCollection_Queue (void)
  { Clear(); }

 private:
  // ----------- PRIVATE METHODS -----------

  //! Creates Iterator for use on BaseCollection
  virtual TYPENAME NCollection_BaseCollection<TheItemType>::Iterator& 
    CreateIterator(void) const
  { return *(new (this->IterAllocator()) Iterator(*this)); }

};

#ifdef WNT
#pragma warning (default:4291)
#endif

#endif
