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

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

typedef void (*NCollection_DelListNode)(NCollection_ListNode*,
                                        occ::handle<NCollection_BaseAllocator>& theAl);

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
    Iterator() noexcept
        : myCurrent(nullptr),
          myPrevious(nullptr)
    {
    }

    // ******** Constructor with initialisation
    Iterator(const NCollection_BaseList& theList) noexcept
        : myCurrent(theList.myFirst),
          myPrevious(nullptr)
    {
    }

    // ******** Initialisation
    void Init(const NCollection_BaseList& theList) noexcept
    {
      myCurrent  = theList.myFirst;
      myPrevious = nullptr;
    }

    // ******** Initialisation
    void Initialize(const NCollection_BaseList& theList) noexcept { Init(theList); }

    // ******** More
    bool More() const noexcept { return (myCurrent != nullptr); }

    // ******** Comparison operator
    bool operator==(const Iterator& theIt) const noexcept { return myCurrent == theIt.myCurrent; }

    //! Performs comparison of two iterators
    bool IsEqual(const Iterator& theOther) const noexcept { return *this == theOther; }

  protected:
    void init(const NCollection_BaseList& theList, NCollection_ListNode* const thePrev) noexcept
    {
      myCurrent  = thePrev ? thePrev->Next() : (NCollection_ListNode*)theList.pLast();
      myPrevious = thePrev;
    }

    void Init(const NCollection_BaseList& theList, NCollection_ListNode* const thePrev) noexcept
    {
      init(theList, thePrev);
    }

  public:
    NCollection_ListNode* myCurrent;  // Pointer to the current node
    NCollection_ListNode* myPrevious; // Pointer to the previous one
    friend class NCollection_BaseList;
  }; // End of nested class Iterator

protected:
  //! STL-style iterator state shared by concrete linked-list containers.
  template <class TheNode, class TheItemType, bool IsConstant>
  class BasicIterator : public Iterator
  {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = TheItemType;
    using difference_type   = std::ptrdiff_t;
    using pointer = typename std::conditional<IsConstant, const TheItemType*, TheItemType*>::type;
    using reference = typename std::conditional<IsConstant, const TheItemType&, TheItemType&>::type;

    BasicIterator() noexcept = default;

    template <bool B = IsConstant, typename std::enable_if<!B, int>::type = 0>
    explicit BasicIterator(NCollection_BaseList& theList) noexcept
        : NCollection_BaseList::Iterator(theList)
    {
    }

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    explicit BasicIterator(const NCollection_BaseList& theList) noexcept
        : NCollection_BaseList::Iterator(theList)
    {
    }

    BasicIterator(const BasicIterator&) noexcept = default;
    BasicIterator& operator=(const BasicIterator&) noexcept = default;

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    BasicIterator(const BasicIterator<TheNode, TheItemType, false>& theOther) noexcept
        : NCollection_BaseList::Iterator()
    {
      this->myCurrent  = theOther.myCurrent;
      this->myPrevious = theOther.myPrevious;
    }

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    BasicIterator& operator=(const BasicIterator<TheNode, TheItemType, false>& theOther) noexcept
    {
      this->myCurrent  = theOther.myCurrent;
      this->myPrevious = theOther.myPrevious;
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

    reference Value() const noexcept { return operator*(); }

    template <bool B = IsConstant, typename std::enable_if<!B, int>::type = 0>
    reference ChangeValue() const noexcept
    {
      return operator*();
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

    void Next() noexcept { ++(*this); }

    template <bool B = IsConstant, typename std::enable_if<!B, int>::type = 0>
    void Init(NCollection_BaseList& theList) noexcept
    {
      NCollection_BaseList::Iterator::Init(theList);
    }

    template <bool B = IsConstant, typename std::enable_if<!B, int>::type = 0>
    void Initialize(NCollection_BaseList& theList) noexcept
    {
      Init(theList);
    }

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    void Init(const NCollection_BaseList& theList) noexcept
    {
      NCollection_BaseList::Iterator::Init(theList);
    }

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    void Initialize(const NCollection_BaseList& theList) noexcept
    {
      Init(theList);
    }

    //! Access the native iterator state for legacy cursor construction.
    const BasicIterator& Iterator() const noexcept { return *this; }
    BasicIterator&       ChangeIterator() noexcept { return *this; }

    template <bool theOtherIsConstant>
    bool IsEqual(const BasicIterator<TheNode, TheItemType, theOtherIsConstant>& theOther)
      const noexcept
    {
      return this->myCurrent == theOther.myCurrent;
    }

    template <bool theOtherIsConstant>
    bool operator==(const BasicIterator<TheNode, TheItemType, theOtherIsConstant>& theOther)
      const noexcept
    {
      return this->myCurrent == theOther.myCurrent;
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
  // ---------- PUBLIC METHODS ------------
  // ******** Extent
  // Purpose: Returns the number of nodes in the list
  int Extent() const noexcept { return static_cast<int>(myLength); }

  //! Length - number of nodes (legacy int-returning API, synonym of Extent()).
  int Length() const noexcept { return static_cast<int>(myLength); }

  //! Size - number of nodes.
  size_t Size() const noexcept { return myLength; }

  // ******** IsEmpty
  // Purpose: Query if the list is empty
  bool IsEmpty() const noexcept { return (myFirst == nullptr); }

  // ******** Allocator
  //! Returns attached allocator
  const occ::handle<NCollection_BaseAllocator>& Allocator() const noexcept { return myAllocator; }

  // ******** Destructor
  // Purpose: defines virtual interface
  virtual ~NCollection_BaseList() = default;

protected:
  // --------- PROTECTED METHODS ----------

  // ******** Constructor
  // Purpose: Initializes an empty list
  NCollection_BaseList(const occ::handle<NCollection_BaseAllocator>& theAllocator = nullptr)
      : myFirst(nullptr),
        myLast(nullptr),
        myLength(0)
  {
    myAllocator =
      (theAllocator.IsNull() ? NCollection_BaseAllocator::CommonBaseAllocator() : theAllocator);
  }

  // ******** pClear
  // Purpose: deletes all nodes
  Standard_EXPORT void pClear(NCollection_DelListNode fDel);
  void PClear(NCollection_DelListNode fDel) { pClear(fDel); }

  // ******** pFirst
  // Purpose: Returns pointer to the first node
  const NCollection_ListNode* pFirst() const noexcept { return myFirst; }
  const NCollection_ListNode* PFirst() const noexcept { return pFirst(); }

  //! Returns a mutable pointer to the first node for mutable native iterators.
  NCollection_ListNode* pFirst() noexcept { return myFirst; }

  // ******** pLast
  // Purpose: Returns pointer to the last node
  const NCollection_ListNode* pLast() const noexcept { return myLast; }
  const NCollection_ListNode* PLast() const noexcept { return pLast(); }

  //! Returns a mutable pointer to the last node for mutable native iterators.
  NCollection_ListNode* pLast() noexcept { return myLast; }

  // ******** pAppend
  // Purpose: Appends theNode at the end
  Standard_EXPORT void pAppend(NCollection_ListNode* theNode) noexcept;
  void PAppend(NCollection_ListNode* theNode) noexcept { pAppend(theNode); }

  // ******** pAppend
  // Purpose: Appends theNode at the end, returns iterator to the previous
  void pAppend(NCollection_ListNode* theNode, Iterator& theIt) noexcept
  {
    NCollection_ListNode* aPrev = myLast;
    pAppend(theNode);
    theIt.init(*this, aPrev);
  }
  void PAppend(NCollection_ListNode* theNode, Iterator& theIt) noexcept
  {
    pAppend(theNode, theIt);
  }

  // ******** pAppend
  // Purpose: Appends theOther list at the end (clearing it)
  Standard_EXPORT void pAppend(NCollection_BaseList& theOther) noexcept;
  void PAppend(NCollection_BaseList& theOther) noexcept { pAppend(theOther); }

  // ******** pPrepend
  // Purpose: Prepends theNode at the beginning
  Standard_EXPORT void pPrepend(NCollection_ListNode* theNode) noexcept;
  void PPrepend(NCollection_ListNode* theNode) noexcept { pPrepend(theNode); }

  // ******** pPrepend
  // Purpose: Prepends theOther list at the beginning (clearing it)
  Standard_EXPORT void pPrepend(NCollection_BaseList& theOther) noexcept;
  void PPrepend(NCollection_BaseList& theOther) noexcept { pPrepend(theOther); }

  // ******** pRemoveFirst
  // Purpose: Removes first node
  Standard_EXPORT void pRemoveFirst(NCollection_DelListNode fDel);
  void PRemoveFirst(NCollection_DelListNode fDel) { pRemoveFirst(fDel); }

  // ******** pRemove
  // Purpose: Removes the node pointed by theIter[ator]
  Standard_EXPORT void pRemove(Iterator& theIter, NCollection_DelListNode fDel);
  void PRemove(Iterator& theIter, NCollection_DelListNode fDel) { pRemove(theIter, fDel); }

  // ******** pInsertBefore
  // Purpose: Inserts theNode before one pointed by theIter[ator]
  Standard_EXPORT void pInsertBefore(NCollection_ListNode* theNode, Iterator& theIter);
  void PInsertBefore(NCollection_ListNode* theNode, Iterator& theIter)
  {
    pInsertBefore(theNode, theIter);
  }

  // ******** pInsertBefore
  // Purpose: Inserts theOther list before the node pointed by theIter[ator]
  Standard_EXPORT void pInsertBefore(NCollection_BaseList& theOther, Iterator& theIter);
  void PInsertBefore(NCollection_BaseList& theOther, Iterator& theIter)
  {
    pInsertBefore(theOther, theIter);
  }

  // ******** pInsertAfter
  // Purpose: Inserts theNode after one pointed by theIter[ator]
  Standard_EXPORT void pInsertAfter(NCollection_ListNode* theNode, Iterator& theIter);
  void PInsertAfter(NCollection_ListNode* theNode, Iterator& theIter)
  {
    pInsertAfter(theNode, theIter);
  }

  // ******** pInsertAfter
  // Purpose: Inserts theOther list after the node pointed by theIter[ator]
  Standard_EXPORT void pInsertAfter(NCollection_BaseList& theOther, Iterator& theIter);
  void PInsertAfter(NCollection_BaseList& theOther, Iterator& theIter)
  {
    pInsertAfter(theOther, theIter);
  }

  // ******** pReverse
  // Purpose: Reverse the list
  Standard_EXPORT void pReverse() noexcept;
  void PReverse() noexcept { pReverse(); }

  // ******** pExchange
  // Purpose: Exchange contents with another list.
  //          Swaps all internal state including allocators, ensuring that
  //          nodes are always deallocated by their original allocator.
  void pExchange(NCollection_BaseList& theOther) noexcept
  {
    std::swap(myAllocator, theOther.myAllocator);
    std::swap(myFirst, theOther.myFirst);
    std::swap(myLast, theOther.myLast);
    std::swap(myLength, theOther.myLength);
  }
  void PExchange(NCollection_BaseList& theOther) noexcept { pExchange(theOther); }

protected:
  // ------------ PROTECTED FIELDS ------------
  occ::handle<NCollection_BaseAllocator> myAllocator;
  NCollection_ListNode*                  myFirst;  // Pointer to the head
  NCollection_ListNode*                  myLast;   // Pointer to the tail
  size_t                                 myLength; // Actual length

  // ------------ FRIEND CLASSES ------------
  friend class Iterator;
};

#endif
