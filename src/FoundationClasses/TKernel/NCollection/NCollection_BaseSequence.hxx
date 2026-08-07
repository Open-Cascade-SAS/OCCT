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

#include <cstddef>
#include <iterator>
#include <type_traits>

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

    NCollection_SeqNode* CurrentNode() const noexcept { return myCurrent; }

    NCollection_SeqNode* PreviousNode() const noexcept { return myPrevious; }

  protected:
    NCollection_SeqNode* myCurrent;  //!< Pointer to the current node
    NCollection_SeqNode* myPrevious; //!< Pointer to the previous node
    friend class NCollection_BaseSequence;
    template <class, class, bool>
    friend class BasicIterator;
  };

protected:
  //! STL-style iterator state shared by concrete sequence containers.
  template <class TheNode, class TheItemType, bool IsConstant>
  class BasicIterator : public Iterator
  {
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = TheItemType;
    using difference_type   = std::ptrdiff_t;
    using pointer = typename std::conditional<IsConstant, const TheItemType*, TheItemType*>::type;
    using reference = typename std::conditional<IsConstant, const TheItemType&, TheItemType&>::type;
    using node_pointer = typename std::conditional<IsConstant,
                                                   const NCollection_SeqNode*,
                                                   NCollection_SeqNode*>::type;

    BasicIterator() noexcept = default;

    template <bool B = IsConstant, typename std::enable_if<!B, int>::type = 0>
    explicit BasicIterator(NCollection_BaseSequence& theSequence,
                           const bool              isStart = true) noexcept
        : Iterator(theSequence, isStart)
    {
    }

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    explicit BasicIterator(const NCollection_BaseSequence& theSequence,
                           const bool                    isStart = true) noexcept
        : Iterator(theSequence, isStart)
    {
    }

    BasicIterator(const BasicIterator&) noexcept = default;
    BasicIterator& operator=(const BasicIterator&) noexcept = default;

    explicit BasicIterator(const Iterator& theOther) noexcept
        : Iterator(theOther)
    {
    }

    void CopyTo(Iterator& theOther) const noexcept
    {
      theOther.myCurrent  = this->myCurrent;
      theOther.myPrevious = this->myPrevious;
    }

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    BasicIterator(const BasicIterator<TheNode, TheItemType, false>& theOther) noexcept
        : Iterator()
    {
      this->myCurrent  = theOther.MutableNode();
      this->myPrevious = theOther.MutablePreviousNode();
    }

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    BasicIterator& operator=(const BasicIterator<TheNode, TheItemType, false>& theOther) noexcept
    {
      this->myCurrent  = theOther.MutableNode();
      this->myPrevious = theOther.MutablePreviousNode();
      return *this;
    }

    reference operator*() const noexcept
    {
      if constexpr (IsConstant)
      {
        return static_cast<const TheNode*>(this->myCurrent)->Value();
      }
      else
      {
        return static_cast<TheNode*>(this->myCurrent)->ChangeValue();
      }
    }

    pointer operator->() const noexcept { return &operator*(); }

    const TheItemType& Value() const noexcept
    {
      return static_cast<const TheNode*>(this->myCurrent)->Value();
    }

    template <bool B = IsConstant, typename std::enable_if<!B, int>::type = 0>
    TheItemType& ChangeValue() const noexcept
    {
      return static_cast<TheNode*>(this->myCurrent)->ChangeValue();
    }

    bool More() const noexcept { return this->myCurrent != nullptr; }

    BasicIterator& operator++() noexcept
    {
      if (this->myCurrent != nullptr)
      {
        this->myPrevious = this->myCurrent;
        this->myCurrent  = this->myCurrent->Next();
      }
      return *this;
    }

    BasicIterator operator++(int) noexcept
    {
      BasicIterator anOld(*this);
      ++(*this);
      return anOld;
    }

    BasicIterator& operator--() noexcept
    {
      Iterator::Previous();
      return *this;
    }

    BasicIterator operator--(int) noexcept
    {
      BasicIterator anOld(*this);
      --(*this);
      return anOld;
    }

    void Next() noexcept { ++(*this); }
    void Previous() noexcept { Iterator::Previous(); }

    node_pointer Node() const noexcept
    {
      return static_cast<node_pointer>(this->myCurrent);
    }
    const NCollection_SeqNode* CurrentNode() const noexcept { return this->myCurrent; }
    const NCollection_SeqNode* PreviousNode() const noexcept { return this->myPrevious; }

    template <bool B = IsConstant, typename std::enable_if<!B, int>::type = 0>
    NCollection_SeqNode* MutableNode() const noexcept
    {
      return this->myCurrent;
    }

    template <bool B = IsConstant, typename std::enable_if<!B, int>::type = 0>
    NCollection_SeqNode* MutablePreviousNode() const noexcept
    {
      return this->myPrevious;
    }

    template <bool B = IsConstant, typename std::enable_if<!B, int>::type = 0>
    void Init(NCollection_BaseSequence& theSequence, const bool isStart = true) noexcept
    {
      Iterator::Init(theSequence, isStart);
    }

    template <bool B = IsConstant, typename std::enable_if<!B, int>::type = 0>
    void Initialize(NCollection_BaseSequence& theSequence, const bool isStart = true) noexcept
    {
      Init(theSequence, isStart);
    }

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    void Init(const NCollection_BaseSequence& theSequence, const bool isStart = true) noexcept
    {
      Iterator::Init(theSequence, isStart);
    }

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    void Initialize(const NCollection_BaseSequence& theSequence,
                    const bool                    isStart = true) noexcept
    {
      Init(theSequence, isStart);
    }

    template <bool theOtherIsConstant>
    bool IsEqual(const BasicIterator<TheNode, TheItemType, theOtherIsConstant>& theOther)
      const noexcept
    {
      return this->myCurrent == theOther.CurrentNode();
    }

    template <bool theOtherIsConstant>
    bool operator==(const BasicIterator<TheNode, TheItemType, theOtherIsConstant>& theOther)
      const noexcept
    {
      return this->myCurrent == theOther.CurrentNode();
    }

    template <bool theOtherIsConstant>
    bool operator!=(const BasicIterator<TheNode, TheItemType, theOtherIsConstant>& theOther)
      const noexcept
    {
      return !(*this == theOther);
    }

  private:
    template <class, class, bool>
    friend class BasicIterator;
  };

public:
  // Methods PUBLIC
  //
  bool IsEmpty() const noexcept { return (mySize == 0); }

  //! Number of items (legacy int-returning API).
  int Length() const noexcept { return static_cast<int>(mySize); }

  //! Size - number of items.
  size_t Size() const noexcept { return mySize; }

  //! Returns attached allocator
  const occ::handle<NCollection_BaseAllocator>& Allocator() const noexcept { return myAllocator; }

protected:
  // Methods PROTECTED
  //
  NCollection_BaseSequence(const occ::handle<NCollection_BaseAllocator>& theAllocator)
      : myFirstItem(nullptr),
        myLastItem(nullptr),
        myCurrentItem(nullptr),
        myCurrentIndex(0),
        mySize(0)
  {
    myAllocator =
      (theAllocator.IsNull() ? NCollection_BaseAllocator::CommonBaseAllocator() : theAllocator);
  }

  //! Destructor
  virtual ~NCollection_BaseSequence() = default;

  Standard_EXPORT void clearSeq(NCollection_DelSeqNode fDel);
  void ClearSeq(NCollection_DelSeqNode fDel) { clearSeq(fDel); }
  Standard_EXPORT void pAppend(NCollection_SeqNode*);
  void PAppend(NCollection_SeqNode* theNode) { pAppend(theNode); }
  Standard_EXPORT void pAppend(NCollection_BaseSequence& S);
  void PAppend(NCollection_BaseSequence& theSequence) { pAppend(theSequence); }
  Standard_EXPORT void pPrepend(NCollection_SeqNode*);
  void PPrepend(NCollection_SeqNode* theNode) { pPrepend(theNode); }
  Standard_EXPORT void pPrepend(NCollection_BaseSequence& S);
  void PPrepend(NCollection_BaseSequence& theSequence) { pPrepend(theSequence); }
  Standard_EXPORT void pInsertAfter(Iterator& thePosition, NCollection_SeqNode*);
  void PInsertAfter(Iterator& thePosition, NCollection_SeqNode* theNode)
  {
    pInsertAfter(thePosition, theNode);
  }
  Standard_EXPORT void pInsertAfter(const size_t Index, NCollection_SeqNode*);
  void PInsertAfter(const size_t theIndex, NCollection_SeqNode* theNode)
  {
    pInsertAfter(theIndex, theNode);
  }
  Standard_EXPORT void pInsertAfter(const size_t Index, NCollection_BaseSequence& S);
  void PInsertAfter(const size_t theIndex, NCollection_BaseSequence& theSequence)
  {
    pInsertAfter(theIndex, theSequence);
  }
  Standard_EXPORT void pSplit(const size_t Index, NCollection_BaseSequence& Sub);
  void PSplit(const size_t theIndex, NCollection_BaseSequence& theSubSequence)
  {
    pSplit(theIndex, theSubSequence);
  }
  Standard_EXPORT void removeSeq(Iterator& thePosition, NCollection_DelSeqNode fDel);
  void RemoveSeq(Iterator& thePosition, NCollection_DelSeqNode fDel)
  {
    removeSeq(thePosition, fDel);
  }
  Standard_EXPORT void removeSeq(const size_t Index, NCollection_DelSeqNode fDel);
  void RemoveSeq(const size_t theIndex, NCollection_DelSeqNode fDel)
  {
    removeSeq(theIndex, fDel);
  }
  Standard_EXPORT void removeSeq(const size_t From, const size_t To, NCollection_DelSeqNode fDel);
  void RemoveSeq(const size_t theFrom, const size_t theTo, NCollection_DelSeqNode fDel)
  {
    removeSeq(theFrom, theTo, fDel);
  }
  Standard_EXPORT void pReverse() noexcept;
  void PReverse() noexcept { pReverse(); }
  Standard_EXPORT void pExchange(const size_t I, const size_t J);
  void PExchange(const size_t theFirst, const size_t theSecond)
  {
    pExchange(theFirst, theSecond);
  }
  Standard_EXPORT NCollection_SeqNode* find(const size_t) const noexcept;
  NCollection_SeqNode* Find(const size_t theIndex) const noexcept { return find(theIndex); }

protected:
  // Fields PROTECTED
  //
  occ::handle<NCollection_BaseAllocator> myAllocator;
  NCollection_SeqNode*                   myFirstItem;
  NCollection_SeqNode*                   myLastItem;
  NCollection_SeqNode*                   myCurrentItem;
  size_t                                 myCurrentIndex;
  size_t                                 mySize;

private:
  // Methods PRIVATE
  //
  NCollection_BaseSequence(const NCollection_BaseSequence& Other) = delete;

  void nullify() noexcept
  {
    myFirstItem = myLastItem = myCurrentItem = nullptr;
    myCurrentIndex = mySize = 0;
  }

  friend class Iterator;
};

#endif
