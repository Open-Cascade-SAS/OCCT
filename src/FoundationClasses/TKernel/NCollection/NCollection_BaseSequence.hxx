// Created on: 2002-04-10
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

#ifndef NCollection_BaseSequence_HeaderFile
#define NCollection_BaseSequence_HeaderFile

#include <Standard.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <NCollection_DefineAlloc.hxx>

// **************************************** Class SeqNode ********************

class NCollection_SeqNode
{
public:
  // define new operator for use with NCollection allocators
  DEFINE_NCOLLECTION_ALLOC
public:
  NCollection_SeqNode() noexcept
      : myNext(nullptr),
        myPrevious(nullptr)
  {
  }

  NCollection_SeqNode* Next() const noexcept { return myNext; }

  NCollection_SeqNode* Previous() const noexcept { return myPrevious; }

  void SetNext(NCollection_SeqNode* theNext) noexcept { myNext = theNext; }

  void SetPrevious(NCollection_SeqNode* thePrev) noexcept { myPrevious = thePrev; }

private:
  NCollection_SeqNode* myNext;
  NCollection_SeqNode* myPrevious;
};

typedef void (*NCollection_DelSeqNode)(NCollection_SeqNode*,
                                       occ::handle<NCollection_BaseAllocator>& theAl);

//! Destruct-only callback: destructs node values but does NOT free memory.
//! Used by free-list caching to retain node memory for reuse.
typedef void (*NCollection_DestroySeqNode)(NCollection_SeqNode*);

/**
 * Purpose:     This  is  a base  class  for  the  Sequence.  It  deals with
 *              an indexed bidirectional list of NCollection_SeqNode's.
 */
class NCollection_BaseSequence
{
public:
  //! Memory allocation
  DEFINE_STANDARD_ALLOC
  DEFINE_NCOLLECTION_ALLOC

public:
  class Iterator
  {
  public:
    //! Empty constructor
    Iterator() noexcept
        : myCurrent(nullptr),
          myPrevious(nullptr)
    {
    }

    //! Constructor with initialisation
    Iterator(const NCollection_BaseSequence& theSeq, const bool isStart) noexcept
    {
      Init(theSeq, isStart);
    }

    //! Initialisation
    void Init(const NCollection_BaseSequence& theSeq, const bool isStart = true) noexcept
    {
      myCurrent  = (isStart ? theSeq.myFirstItem : nullptr);
      myPrevious = (isStart ? nullptr : theSeq.myLastItem);
    }

    //! Switch to previous element; note that it will reset
    void Previous() noexcept
    {
      myCurrent = myPrevious;
      if (myCurrent)
        myPrevious = myCurrent->Previous();
    }

  protected:
    NCollection_SeqNode* myCurrent;  //!< Pointer to the current node
    NCollection_SeqNode* myPrevious; //!< Pointer to the previous node
    friend class NCollection_BaseSequence;
  };

public:
  // Methods PUBLIC
  //
  bool IsEmpty() const noexcept { return (mySize == 0); }

  int Length() const noexcept { return mySize; }

  //! Returns attached allocator
  const occ::handle<NCollection_BaseAllocator>& Allocator() const noexcept { return myAllocator; }

protected:
  // Methods PROTECTED
  //
  NCollection_BaseSequence(const occ::handle<NCollection_BaseAllocator>& theAllocator)
      : myFirstItem(nullptr),
        myLastItem(nullptr),
        myCurrentItem(nullptr),
        myCachedData(nullptr),
        myCurrentIndex(0),
        mySize(0)
  {
    myAllocator =
      (theAllocator.IsNull() ? NCollection_BaseAllocator::CommonBaseAllocator() : theAllocator);
  }

  //! Destructor
  virtual ~NCollection_BaseSequence() = default;

  Standard_EXPORT void ClearSeq(NCollection_DelSeqNode fDel);
  Standard_EXPORT void PAppend(NCollection_SeqNode*);
  Standard_EXPORT void PAppend(NCollection_BaseSequence& S);
  Standard_EXPORT void PPrepend(NCollection_SeqNode*);
  Standard_EXPORT void PPrepend(NCollection_BaseSequence& S);
  Standard_EXPORT void PInsertAfter(Iterator& thePosition, NCollection_SeqNode*);
  Standard_EXPORT void PInsertAfter(const int Index, NCollection_SeqNode*);
  Standard_EXPORT void PInsertAfter(const int Index, NCollection_BaseSequence& S);
  Standard_EXPORT void PSplit(const int Index, NCollection_BaseSequence& Sub);
  Standard_EXPORT void RemoveSeq(Iterator& thePosition, NCollection_DelSeqNode fDel);
  Standard_EXPORT void RemoveSeq(const int Index, NCollection_DelSeqNode fDel);
  Standard_EXPORT void RemoveSeq(const int From, const int To, NCollection_DelSeqNode fDel);
  Standard_EXPORT void PReverse() noexcept;
  Standard_EXPORT void PExchange(const int I, const int J);
  Standard_EXPORT NCollection_SeqNode* Find(const int) const noexcept;

  //! Clear sequence, caching nodes for reuse (destruct values only).
  Standard_EXPORT void ClearSeqToCache(NCollection_DestroySeqNode fDestroy);

  //! Remove at iterator position, caching node for reuse.
  Standard_EXPORT void RemoveSeqToCache(Iterator& thePosition, NCollection_DestroySeqNode fDestroy);

  //! Remove by index, caching node for reuse.
  Standard_EXPORT void RemoveSeqToCache(const int theIndex, NCollection_DestroySeqNode fDestroy);

  //! Remove range, caching nodes for reuse.
  Standard_EXPORT void RemoveSeqToCache(const int                  theFrom,
                                        const int                  theTo,
                                        NCollection_DestroySeqNode fDestroy);

  //! Pop a node from the free-list cache, or return nullptr if empty.
  NCollection_SeqNode* allocateFromSeqCache() noexcept
  {
    if (myCachedData == nullptr)
    {
      return nullptr;
    }
    NCollection_SeqNode* aNode = myCachedData;
    myCachedData               = myCachedData->Next();
    return aNode;
  }

  //! Walk the free-list calling allocator Free on each cached node.
  Standard_EXPORT void freeCachedSeqNodes();

protected:
  // Fields PROTECTED
  //
  occ::handle<NCollection_BaseAllocator> myAllocator;
  NCollection_SeqNode*                   myFirstItem;
  NCollection_SeqNode*                   myLastItem;
  NCollection_SeqNode*                   myCurrentItem;
  NCollection_SeqNode*                   myCachedData; //!< Free-list cache of removed nodes
  int                                    myCurrentIndex;
  int                                    mySize;

private:
  // Methods PRIVATE
  //
  NCollection_BaseSequence(const NCollection_BaseSequence& Other) = delete;

  void Nullify() noexcept
  {
    myFirstItem = myLastItem = myCurrentItem = nullptr;
    myCurrentIndex = mySize = 0;
  }

  friend class Iterator;
};

#endif
