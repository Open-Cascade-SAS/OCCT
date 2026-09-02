// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef _NCollection_PagedArray_HeaderFile
#define _NCollection_PagedArray_HeaderFile

#include <NCollection_Array1.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_OutOfRange.hxx>

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>
#include <variant>

//! Paged array with page-granular copy-on-write ownership.
//!
//! Copying the array retains one radix-tree root. Const reads address shared
//! immutable pages directly; the first write clones the radix path and the
//! touched page. This gives snapshots O(1) retention and bounded indexed reads.
//! The container is dense: extending its visible range initializes every
//! intervening value and therefore costs O(number of appended values).
//! Concurrent const access to retained copies is safe. Mutation requires an
//! externally serialized, privately owned array; atomic reference counts do
//! not make simultaneous writes to the same array safe.
//! Iterators retain the visible size captured at construction. Structural
//! changes invalidate STL iterators; a legacy Iterator may safely observe
//! value changes and deliberately ignores values appended after initialization.
template <typename TheItemType>
class NCollection_PagedArray
{
public:
  DEFINE_STANDARD_ALLOC

  using value_type      = TheItemType;
  using size_type       = size_t;
  using difference_type = std::ptrdiff_t;
  using pointer         = TheItemType*;
  using const_pointer   = const TheItemType*;
  using reference       = TheItemType&;
  using const_reference = const TheItemType&;

private:
  template <bool IsConstant>
  class BasicIterator
  {
  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = TheItemType;
    using difference_type   = std::ptrdiff_t;
    using pointer           = std::conditional_t<IsConstant, const TheItemType*, TheItemType*>;
    using reference         = std::conditional_t<IsConstant, const TheItemType&, TheItemType&>;
    using container_pointer =
      std::conditional_t<IsConstant, const NCollection_PagedArray*, NCollection_PagedArray*>;

    BasicIterator() noexcept = default;

    template <bool B = IsConstant, std::enable_if_t<!B>* = nullptr>
    explicit BasicIterator(NCollection_PagedArray& theArray, const size_t theIndex = 0) noexcept
        : myContainer(&theArray),
          myIndex(theIndex),
          myEnd(theArray.Size())
    {
    }

    template <bool B = IsConstant, std::enable_if_t<B>* = nullptr>
    explicit BasicIterator(const NCollection_PagedArray& theArray,
                           const size_t                  theIndex = 0) noexcept
        : myContainer(&theArray),
          myIndex(theIndex),
          myEnd(theArray.Size())
    {
    }

    template <bool B = IsConstant, std::enable_if_t<B>* = nullptr>
    BasicIterator(const BasicIterator<false>& theOther) noexcept
        : myContainer(theOther.myContainer),
          myIndex(theOther.myIndex),
          myEnd(theOther.myEnd)
    {
    }

    template <bool B = IsConstant, std::enable_if_t<B>* = nullptr>
    BasicIterator& operator=(const BasicIterator<false>& theOther) noexcept
    {
      myContainer = theOther.myContainer;
      myIndex     = theOther.myIndex;
      myEnd       = theOther.myEnd;
      return *this;
    }

    reference operator*() const noexcept(IsConstant)
    {
      if constexpr (IsConstant)
      {
        return myContainer->Value(myIndex);
      }
      else
      {
        return myContainer->ChangeValue(myIndex);
      }
    }

    pointer operator->() const noexcept(IsConstant) { return &operator*(); }

    reference Value() const noexcept(IsConstant) { return operator*(); }

    template <bool B = IsConstant, std::enable_if_t<!B>* = nullptr>
    reference ChangeValue() const
    {
      return operator*();
    }

    bool More() const noexcept { return myContainer != nullptr && myIndex < myEnd; }

    BasicIterator& operator++() noexcept
    {
      ++myIndex;
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
      --myIndex;
      return *this;
    }

    BasicIterator operator--(int) noexcept
    {
      BasicIterator anOld(*this);
      --(*this);
      return anOld;
    }

    BasicIterator& operator+=(const difference_type theOffset) noexcept
    {
      myIndex = static_cast<size_t>(static_cast<difference_type>(myIndex) + theOffset);
      return *this;
    }

    BasicIterator operator+(const difference_type theOffset) const noexcept
    {
      BasicIterator anIterator(*this);
      anIterator += theOffset;
      return anIterator;
    }

    BasicIterator& operator-=(const difference_type theOffset) noexcept
    {
      return *this += -theOffset;
    }

    BasicIterator operator-(const difference_type theOffset) const noexcept
    {
      BasicIterator anIterator(*this);
      anIterator -= theOffset;
      return anIterator;
    }

    template <bool TheOtherIsConstant>
    difference_type operator-(const BasicIterator<TheOtherIsConstant>& theOther) const noexcept
    {
      return static_cast<difference_type>(myIndex) - static_cast<difference_type>(theOther.myIndex);
    }

    reference operator[](const difference_type theOffset) const noexcept(IsConstant)
    {
      return *(*this + theOffset);
    }

    template <bool TheOtherIsConstant>
    bool operator==(const BasicIterator<TheOtherIsConstant>& theOther) const noexcept
    {
      return myContainer == theOther.myContainer && myIndex == theOther.myIndex;
    }

    template <bool TheOtherIsConstant>
    bool operator!=(const BasicIterator<TheOtherIsConstant>& theOther) const noexcept
    {
      return !(*this == theOther);
    }

    template <bool TheOtherIsConstant>
    bool operator<(const BasicIterator<TheOtherIsConstant>& theOther) const noexcept
    {
      return (*this - theOther) < 0;
    }

    template <bool TheOtherIsConstant>
    bool operator>(const BasicIterator<TheOtherIsConstant>& theOther) const noexcept
    {
      return theOther < *this;
    }

    template <bool TheOtherIsConstant>
    bool operator<=(const BasicIterator<TheOtherIsConstant>& theOther) const noexcept
    {
      return !(theOther < *this);
    }

    template <bool TheOtherIsConstant>
    bool operator>=(const BasicIterator<TheOtherIsConstant>& theOther) const noexcept
    {
      return !(*this < theOther);
    }

    friend BasicIterator operator+(const difference_type theOffset,
                                   const BasicIterator&  theIterator) noexcept
    {
      return theIterator + theOffset;
    }

  private:
    template <bool>
    friend class BasicIterator;

    container_pointer myContainer = nullptr;
    size_t            myIndex     = 0;
    size_t            myEnd       = 0;
  };

public:
  using iterator       = BasicIterator<false>;
  using const_iterator = BasicIterator<true>;

  class Iterator : public const_iterator
  {
  public:
    Iterator() noexcept = default;

    explicit Iterator(const NCollection_PagedArray& theArray) noexcept
        : const_iterator(theArray)
    {
    }

    bool More() const noexcept { return const_iterator::More(); }

    void Next() noexcept { ++(*this); }

    const_reference Value() const noexcept { return const_iterator::Value(); }

    void Init(const NCollection_PagedArray& theArray) noexcept { *this = Iterator(theArray); }

    void Initialize(const NCollection_PagedArray& theArray) noexcept { Init(theArray); }
  };

  iterator begin() noexcept { return iterator(*this); }

  const_iterator begin() const noexcept { return const_iterator(*this); }

  const_iterator cbegin() const noexcept { return const_iterator(*this); }

  iterator end() noexcept { return iterator(*this, mySize); }

  const_iterator end() const noexcept { return const_iterator(*this, mySize); }

  const_iterator cend() const noexcept { return const_iterator(*this, mySize); }

  //! Construct an empty array.
  explicit NCollection_PagedArray(const size_t thePageSize = 256)
      : myRoot(std::make_shared<PageTableNode>(true)),
        myPageSize(thePageSize == 0 ? 1 : thePageSize)
  {
  }

  //! Return the number of visible values.
  [[nodiscard]] size_t Size() const noexcept { return mySize; }

  //! Return true when no values are visible.
  [[nodiscard]] bool IsEmpty() const noexcept { return mySize == 0; }

  //! Return a value without detaching its page.
  [[nodiscard]] const_reference Value(const size_t theIndex) const noexcept
  {
    return page(pageIndex(theIndex)).Values.At(pageOffset(theIndex));
  }

  [[nodiscard]] const_reference operator[](const size_t theIndex) const noexcept
  {
    return Value(theIndex);
  }

  //! Return mutable access, cloning a shared page before exposing it.
  [[nodiscard]] reference ChangeValue(const size_t theIndex)
  {
    return changePage(pageIndex(theIndex)).Values.ChangeAt(pageOffset(theIndex));
  }

  [[nodiscard]] reference operator[](const size_t theIndex) { return ChangeValue(theIndex); }

  [[nodiscard]] const_reference First() const noexcept { return Value(0); }

  [[nodiscard]] reference ChangeFirst() { return ChangeValue(0); }

  [[nodiscard]] const_reference Last() const noexcept { return Value(mySize - 1); }

  [[nodiscard]] reference ChangeLast() { return ChangeValue(mySize - 1); }

  //! Append a default-constructed value.
  reference Appended()
  {
    ensurePage(pageIndex(mySize));
    reference aValue = ChangeValue(mySize);
    aValue           = TheItemType();
    ++mySize;
    return aValue;
  }

  //! Append a copied value.
  reference Append(const TheItemType& theValue)
  {
    ensurePage(pageIndex(mySize));
    reference aValue = ChangeValue(mySize);
    aValue           = theValue;
    ++mySize;
    return aValue;
  }

  //! Append a moved value.
  reference Append(TheItemType&& theValue)
  {
    ensurePage(pageIndex(mySize));
    reference aValue = ChangeValue(mySize);
    aValue           = std::move(theValue);
    ++mySize;
    return aValue;
  }

  //! Set a value, extending the visible range with default values when needed.
  reference SetValue(const size_t theIndex, const TheItemType& theValue)
  {
    if (theIndex >= mySize)
    {
      Standard_OutOfRange_Raise_if(theIndex == std::numeric_limits<size_t>::max(),
                                   "NCollection_PagedArray::SetValue: index is too large");
      Resize(theIndex + 1);
    }
    reference aValue = ChangeValue(theIndex);
    aValue           = theValue;
    return aValue;
  }

  //! Set a moved value, extending the visible range when needed.
  reference SetValue(const size_t theIndex, TheItemType&& theValue)
  {
    if (theIndex >= mySize)
    {
      Standard_OutOfRange_Raise_if(theIndex == std::numeric_limits<size_t>::max(),
                                   "NCollection_PagedArray::SetValue: index is too large");
      Resize(theIndex + 1);
    }
    reference aValue = ChangeValue(theIndex);
    aValue           = std::move(theValue);
    return aValue;
  }

  //! Change the visible size.
  void Resize(const size_t theSize, const TheItemType& theValue = TheItemType())
  {
    if (theSize > mySize)
    {
      const size_t anOldSize = mySize;
      ensurePage(pageIndex(theSize - 1));
      size_t anIndex = anOldSize;
      while (anIndex < theSize)
      {
        Page& aPage = changePage(pageIndex(anIndex));
        const size_t aPageEnd = anIndex + std::min(theSize - anIndex,
                                                   myPageSize - pageOffset(anIndex));
        for (; anIndex < aPageEnd; ++anIndex)
        {
          aPage.Values.ChangeAt(pageOffset(anIndex)) = theValue;
          mySize                                      = anIndex + 1;
        }
      }
      return;
    }
    if (theSize == mySize)
    {
      return;
    }
    if (theSize == 0)
    {
      Clear();
      return;
    }

    const size_t aPageCount = pageIndex(theSize - 1) + 1;
    const size_t aTailSize  = std::min(mySize - theSize, myPageSize - pageOffset(theSize));
    const size_t anEnd      = theSize + aTailSize;
    for (size_t anIndex = theSize; anIndex < anEnd; ++anIndex)
    {
      ChangeValue(anIndex) = TheItemType();
    }
    for (size_t aPageIndex = aPageCount; aPageIndex < myPageCount; ++aPageIndex)
    {
      releasePage(aPageIndex);
    }
    mySize      = theSize;
    myPageCount = aPageCount;
    collapseRoot();
  }

  //! Clear the visible array and release this array's page references.
  void Clear()
  {
    myRoot      = std::make_shared<PageTableNode>(true);
    myDepth     = 0;
    myPageCount = 0;
    mySize      = 0;
  }

  //! Return the physical page size.
  [[nodiscard]] size_t PageSize() const noexcept { return myPageSize; }

  //! Return the number of retained pages.
  [[nodiscard]] size_t NbPages() const noexcept { return myPageCount; }

private:
  struct Page
  {
    explicit Page(const size_t theSize)
        : Values(theSize)
    {
    }

    NCollection_Array1<TheItemType> Values;
  };

  static constexpr size_t THE_BRANCH_BITS = 6;
  static constexpr size_t THE_BRANCH_SIZE = size_t(1) << THE_BRANCH_BITS;
  static constexpr size_t THE_BRANCH_MASK = THE_BRANCH_SIZE - 1;
  static constexpr size_t THE_MAX_DEPTH   = (sizeof(size_t) * 8 - 1) / THE_BRANCH_BITS;

  struct PageTableNode
  {
    using BranchSlots = std::array<std::shared_ptr<PageTableNode>, THE_BRANCH_SIZE>;
    using PageSlots   = std::array<std::shared_ptr<Page>, THE_BRANCH_SIZE>;
    using SlotStorage = std::variant<BranchSlots, PageSlots>;

    explicit PageTableNode(const bool theIsLeaf)
        : Slots(theIsLeaf ? SlotStorage(std::in_place_index<1>)
                          : SlotStorage(std::in_place_index<0>))
    {
    }

    [[nodiscard]] BranchSlots& ChangeBranches() { return std::get<BranchSlots>(Slots); }

    [[nodiscard]] const BranchSlots& Branches() const { return std::get<BranchSlots>(Slots); }

    [[nodiscard]] PageSlots& ChangePages() { return std::get<PageSlots>(Slots); }

    [[nodiscard]] const PageSlots& Pages() const { return std::get<PageSlots>(Slots); }

    [[nodiscard]] bool IsEmpty() const
    {
      if (Slots.index() == 0)
      {
        return std::all_of(
          Branches().begin(),
          Branches().end(),
          [](const std::shared_ptr<PageTableNode>& theChild) { return theChild == nullptr; });
      }
      return std::all_of(Pages().begin(), Pages().end(), [](const std::shared_ptr<Page>& thePage) {
        return thePage == nullptr;
      });
    }

    SlotStorage Slots;
  };

  [[nodiscard]] size_t pageIndex(const size_t theIndex) const noexcept
  {
    return theIndex / myPageSize;
  }

  [[nodiscard]] size_t pageOffset(const size_t theIndex) const noexcept
  {
    return theIndex % myPageSize;
  }

  void ensurePage(const size_t thePageIndex)
  {
    while (myDepth < THE_MAX_DEPTH && (thePageIndex >> ((myDepth + 1) * THE_BRANCH_BITS)) != 0)
    {
      std::shared_ptr<PageTableNode> aRoot = std::make_shared<PageTableNode>(false);
      aRoot->ChangeBranches()[0]           = myRoot;
      myRoot                               = std::move(aRoot);
      ++myDepth;
    }
    while (myPageCount <= thePageIndex)
    {
      (void)changePage(myPageCount);
      ++myPageCount;
    }
  }

  [[nodiscard]] const Page& page(const size_t thePageIndex) const noexcept
  {
    const PageTableNode* aNode = myRoot.get();
    for (size_t aLevel = myDepth; aLevel > 0; --aLevel)
    {
      const size_t aSlot = (thePageIndex >> (aLevel * THE_BRANCH_BITS)) & THE_BRANCH_MASK;
      aNode              = aNode->Branches()[aSlot].get();
    }
    return *aNode->Pages()[thePageIndex & THE_BRANCH_MASK];
  }

  Page& changePage(const size_t thePageIndex)
  {
    if (myRoot.use_count() != 1)
    {
      myRoot = std::make_shared<PageTableNode>(*myRoot);
    }

    PageTableNode* aNode = myRoot.get();
    for (size_t aLevel = myDepth; aLevel > 0; --aLevel)
    {
      const size_t aSlot = (thePageIndex >> (aLevel * THE_BRANCH_BITS)) & THE_BRANCH_MASK;
      std::shared_ptr<PageTableNode>& aChild = aNode->ChangeBranches()[aSlot];
      if (aChild == nullptr)
      {
        aChild = std::make_shared<PageTableNode>(aLevel == 1);
      }
      else if (aChild.use_count() != 1)
      {
        aChild = std::make_shared<PageTableNode>(*aChild);
      }
      aNode = aChild.get();
    }

    std::shared_ptr<Page>& aPage = aNode->ChangePages()[thePageIndex & THE_BRANCH_MASK];
    if (aPage == nullptr)
    {
      aPage = std::make_shared<Page>(myPageSize);
    }
    if (aPage.use_count() != 1)
    {
      aPage = std::make_shared<Page>(*aPage);
    }
    return *aPage;
  }

  void releasePage(const size_t thePageIndex)
  {
    if (myRoot.use_count() != 1)
    {
      myRoot = std::make_shared<PageTableNode>(*myRoot);
    }

    std::array<PageTableNode*, THE_MAX_DEPTH> aParents{};
    std::array<size_t, THE_MAX_DEPTH>         aSlots{};
    size_t                                    aPathLength = 0;
    PageTableNode*                            aNode       = myRoot.get();
    for (size_t aLevel = myDepth; aLevel > 0; --aLevel)
    {
      const size_t aSlot = (thePageIndex >> (aLevel * THE_BRANCH_BITS)) & THE_BRANCH_MASK;
      std::shared_ptr<PageTableNode>& aChild = aNode->ChangeBranches()[aSlot];
      if (aChild.use_count() != 1)
      {
        aChild = std::make_shared<PageTableNode>(*aChild);
      }
      aParents[aPathLength] = aNode;
      aSlots[aPathLength]   = aSlot;
      ++aPathLength;
      aNode = aChild.get();
    }
    aNode->ChangePages()[thePageIndex & THE_BRANCH_MASK].reset();

    while (aPathLength > 0 && aNode->IsEmpty())
    {
      --aPathLength;
      PageTableNode* aParent = aParents[aPathLength];
      aParent->ChangeBranches()[aSlots[aPathLength]].reset();
      aNode = aParent;
    }
  }

  void collapseRoot()
  {
    while (myDepth > 0)
    {
      const typename PageTableNode::BranchSlots& aBranches        = myRoot->Branches();
      bool                                       hasOtherBranches = false;
      for (size_t aSlot = 1; aSlot < THE_BRANCH_SIZE; ++aSlot)
      {
        if (aBranches[aSlot] != nullptr)
        {
          hasOtherBranches = true;
          break;
        }
      }
      if (hasOtherBranches || aBranches[0] == nullptr)
      {
        break;
      }
      std::shared_ptr<PageTableNode> aChild = aBranches[0];
      myRoot                                = std::move(aChild);
      --myDepth;
    }
  }

  std::shared_ptr<PageTableNode> myRoot;
  size_t                         myPageSize  = 256;
  size_t                         mySize      = 0;
  size_t                         myPageCount = 0;
  size_t                         myDepth     = 0;
};

#endif // _NCollection_PagedArray_HeaderFile
