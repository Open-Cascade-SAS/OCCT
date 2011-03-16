// File:        NCollection_TListIterator.hxx
// Created:     Tue Apr 23 17:33:02 2002
// Author:      Alexander KARTOMIN
//              <a-kartomin@opencascade.com>

#ifndef NCollection_TListIterator_HeaderFile
#define NCollection_TListIterator_HeaderFile

#include <NCollection_BaseCollection.hxx>
#include <NCollection_BaseList.hxx>
#include <NCollection_TListNode.hxx>

#ifdef WNT
// Disable the warning "operator new unmatched by delete"
#pragma warning (push)
#pragma warning (disable:4291)
#endif

/**
 * Purpose:     This Iterator class iterates on BaseList of TListNode and is 
 *              instantiated in List/Set/Queue/Stack
 * Remark:      TListIterator is internal class
 */
template <class TheItemType> class NCollection_TListIterator 
  : public NCollection_BaseCollection<TheItemType>::Iterator,
    public NCollection_BaseList::Iterator
{
 public:
  //! Empty constructor - for later Init
  NCollection_TListIterator  (void) :
    NCollection_BaseList::Iterator () {}
  //! Constructor with initialisation
  NCollection_TListIterator  (const NCollection_BaseList& theList) :
    NCollection_BaseList::Iterator (theList) {}
  //! Assignment
  NCollection_TListIterator& operator= (const NCollection_TListIterator& theIt)
  {
    NCollection_BaseList::Iterator& me = * this;
    me.operator= (theIt);
    return * this;
  }
  //! Check end
  virtual Standard_Boolean More (void) const
  { return (myCurrent!=NULL); }
  //! Make step
  virtual void Next (void)
  {
    myPrevious = myCurrent;
    myCurrent = myCurrent->Next();
  }
  //! Constant Value access
  virtual const TheItemType& Value (void) const
  { return ((const NCollection_TListNode<TheItemType>*) myCurrent)->Value(); }
  //! Variable Value access
  virtual TheItemType& ChangeValue (void) const
  { return ((NCollection_TListNode<TheItemType> *)myCurrent)->ChangeValue(); }
  //! Operator new for allocating iterators
  void* operator new(size_t theSize,
                     const Handle(NCollection_BaseAllocator)& theAllocator) 
  { return theAllocator->Allocate(theSize); }
};

#ifdef WNT
#pragma warning (pop)
#endif

#endif
