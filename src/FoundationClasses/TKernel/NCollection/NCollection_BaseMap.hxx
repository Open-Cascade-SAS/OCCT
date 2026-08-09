// Created on: 2002-04-18
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

#ifndef NCollection_BaseMap_HeaderFile
#define NCollection_BaseMap_HeaderFile

#include <Standard.hxx>
#include <Standard_OutOfRange.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <NCollection_DefineAlloc.hxx>
#include <NCollection_ListNode.hxx>

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

typedef void (*NCollection_DelMapNode)(NCollection_ListNode*,
                                       occ::handle<NCollection_BaseAllocator>& theAl);

/**
 * Purpose:     This is a base class for all Maps:
 *                Map
 *                DataMap
 *                IndexedMap
 *                IndexedDataMap
 *              Provides utilitites for managing the buckets.
 */

class NCollection_BaseMap
{
public:
  //! Memory allocation
  DEFINE_STANDARD_ALLOC
  DEFINE_NCOLLECTION_ALLOC

public:
  // **************************************** Class Iterator ****************
  class Iterator
  {
  protected:
    //! Empty constructor
    Iterator() noexcept
        : myNbBuckets(0),
          myBuckets(nullptr),
          myBucket(0),
          myNode(nullptr)
    {
    }

    //! Constructor
    Iterator(const NCollection_BaseMap& theMap) noexcept
        : myNbBuckets(theMap.myNbBuckets),
          myBuckets(theMap.myData1),
          myBucket(0),
          myNode(nullptr)
    {
      findFirst();
    }

  public:
    //! Initialize
    void Initialize(const NCollection_BaseMap& theMap) noexcept
    {
      myNbBuckets = theMap.myNbBuckets;
      myBuckets   = theMap.myData1;
      myBucket    = 0;
      myNode      = nullptr;
      findFirst();
    }

    //! Reset
    void Reset() noexcept
    {
      myBucket = 0;
      myNode   = nullptr;
      findFirst();
    }

    //! Performs comparison of two iterators.
    bool IsEqual(const Iterator& theOther) const noexcept
    {
      return myBucket == theOther.myBucket && myNode == theOther.myNode;
    }

  protected:
    //! pMore
    bool pMore() const noexcept { return (myNode != nullptr); }

    //! pNext
    void pNext() noexcept
    {
      if (!myBuckets)
        return;
      if (myNode)
      {
        myNode = myNode->Next();
        if (myNode)
          return;
      }
      ++myBucket;
      while (myBucket <= myNbBuckets)
      {
        myNode = myBuckets[myBucket];
        if (myNode)
          return;
        ++myBucket;
      }
    }

  private:
    //! Find the first non-empty bucket starting from myBucket.
    void findFirst() noexcept
    {
      if (!myBuckets)
        return;
      for (; myBucket <= myNbBuckets; ++myBucket)
      {
        myNode = myBuckets[myBucket];
        if (myNode)
          return;
      }
    }

  protected:
    // ---------- PRIVATE FIELDS ------------
    size_t                 myNbBuckets; //!< Total buckets in the map
    NCollection_ListNode** myBuckets;   //!< Location in memory
    size_t                 myBucket;    //!< Current bucket
    NCollection_ListNode*  myNode;      //!< Current node
  };

protected:
  //! STL-style iterator state shared by bucket-chain map containers.
  template <class TheContainer,
            class TheNode,
            class TheKey,
            class TheValue,
            bool IsKeyOnly,
            bool IsConstant,
            bool IsOrdered = false>
  class BasicIterator
  {
  private:
    template <bool theIsConstant>
    class State
    {
    public:
      using node_pointer = typename std::conditional<theIsConstant, const TheNode*, TheNode*>::type;
      using bucket_pointer = NCollection_ListNode* const*;
      using reference =
        typename std::conditional<IsKeyOnly || theIsConstant, const TheValue&, TheValue&>::type;

      State() noexcept = default;

      explicit State(node_pointer theNode) noexcept
          : myNode(theNode)
      {
      }

      template <bool B = theIsConstant, typename std::enable_if<!B, int>::type = 0>
      explicit State(TheContainer& theContainer) noexcept
      {
        if constexpr (IsOrdered)
        {
          myNode = static_cast<node_pointer>(theContainer.FirstNode());
        }
        else
        {
          myBucket = theContainer.Buckets();
          myEnd    = myBucket == nullptr ? nullptr : myBucket + theContainer.NbBuckets() + 1;
          skipEmptyBuckets();
        }
      }

      template <bool B = theIsConstant, typename std::enable_if<B, int>::type = 0>
      explicit State(const TheContainer& theContainer) noexcept
      {
        if constexpr (IsOrdered)
        {
          myNode = static_cast<node_pointer>(theContainer.FirstNode());
        }
        else
        {
          myBucket = theContainer.Buckets();
          myEnd    = myBucket == nullptr ? nullptr : myBucket + theContainer.NbBuckets() + 1;
          skipEmptyBuckets();
        }
      }

      State(const State&) noexcept            = default;
      State& operator=(const State&) noexcept = default;

      template <bool B = theIsConstant, typename std::enable_if<B, int>::type = 0>
      State(const State<false>& theOther) noexcept
          : myBucket(theOther.myBucket),
            myEnd(theOther.myEnd),
            myNode(theOther.myNode)
      {
      }

      template <bool B = theIsConstant, typename std::enable_if<B, int>::type = 0>
      State& operator=(const State<false>& theOther) noexcept
      {
        myBucket = theOther.myBucket;
        myEnd    = theOther.myEnd;
        myNode   = theOther.myNode;
        return *this;
      }

      reference Value() const noexcept
      {
        if constexpr (IsKeyOnly || theIsConstant)
        {
          return myNode->Value();
        }
        else
        {
          return myNode->ChangeValue();
        }
      }

      const TheKey& Key() const noexcept
      {
        if constexpr (IsOrdered)
        {
          return myNode->Key();
        }
        else if constexpr (IsKeyOnly)
        {
          return myNode->Value();
        }
        else
        {
          return myNode->Key();
        }
      }

      const TheNode* Node() const noexcept { return myNode; }

      void Next() noexcept
      {
        if (myNode == nullptr)
        {
          return;
        }
        if constexpr (IsOrdered)
        {
          myNode = static_cast<node_pointer>(myNode->OrderNext());
        }
        else
        {
          myNode = static_cast<node_pointer>(myNode->Next());
        }
        if (myNode == nullptr && !IsOrdered && myBucket != nullptr)
        {
          ++myBucket;
          skipEmptyBuckets();
        }
      }

      template <bool theOtherIsConstant>
      bool IsEqual(const State<theOtherIsConstant>& theOther) const noexcept
      {
        return myNode == theOther.myNode;
      }

    private:
      template <bool>
      friend class State;

      void skipEmptyBuckets() noexcept
      {
        myNode = nullptr;
        while (myBucket != nullptr && myBucket != myEnd)
        {
          myNode = static_cast<node_pointer>(*myBucket);
          if (myNode != nullptr)
          {
            return;
          }
          ++myBucket;
        }
      }

      bucket_pointer myBucket = nullptr;
      bucket_pointer myEnd    = nullptr;
      node_pointer   myNode   = nullptr;
    };

  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = TheValue;
    using difference_type   = std::ptrdiff_t;
    using pointer =
      typename std::conditional<IsKeyOnly || IsConstant, const TheValue*, TheValue*>::type;
    using reference =
      typename std::conditional<IsKeyOnly || IsConstant, const TheValue&, TheValue&>::type;
    using node_pointer = typename std::conditional<IsConstant, const TheNode*, TheNode*>::type;
    using container_type =
      typename std::conditional<IsConstant, const TheContainer, TheContainer>::type;

    BasicIterator() noexcept = default;

    explicit BasicIterator(container_type& theContainer) noexcept
        : myState(theContainer)
    {
    }

    explicit BasicIterator(node_pointer theNode) noexcept
        : myState(theNode)
    {
    }

    BasicIterator(const BasicIterator&) noexcept            = default;
    BasicIterator& operator=(const BasicIterator&) noexcept = default;

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    BasicIterator(
      const BasicIterator<TheContainer, TheNode, TheKey, TheValue, IsKeyOnly, false, IsOrdered>&
        theOther) noexcept
        : myState(theOther.myState)
    {
    }

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    BasicIterator& operator=(
      const BasicIterator<TheContainer, TheNode, TheKey, TheValue, IsKeyOnly, false, IsOrdered>&
        theOther) noexcept
    {
      myState = theOther.myState;
      return *this;
    }

    reference operator*() const noexcept { return myState.Value(); }

    pointer operator->() const noexcept { return &myState.Value(); }

    const TheNode* Node() const noexcept { return myState.Node(); }

    const TheKey& Key() const noexcept { return myState.Key(); }

    bool More() const noexcept { return myState.Node() != nullptr; }

    BasicIterator& operator++() noexcept
    {
      myState.Next();
      return *this;
    }

    BasicIterator operator++(int) noexcept
    {
      BasicIterator anOld(*this);
      ++(*this);
      return anOld;
    }

    void Next() noexcept { ++(*this); }

    template <bool theOtherIsConstant>
    bool operator==(const BasicIterator<TheContainer,
                                        TheNode,
                                        TheKey,
                                        TheValue,
                                        IsKeyOnly,
                                        theOtherIsConstant,
                                        IsOrdered>& theOther) const noexcept
    {
      return myState.Node() == theOther.myState.Node();
    }

    template <bool theOtherIsConstant>
    bool operator!=(const BasicIterator<TheContainer,
                                        TheNode,
                                        TheKey,
                                        TheValue,
                                        IsKeyOnly,
                                        theOtherIsConstant,
                                        IsOrdered>& theOther) const noexcept
    {
      return !(*this == theOther);
    }

  private:
    template <class, class, class, class, bool, bool, bool>
    friend class BasicIterator;

    State<IsConstant> myState;
  };

public:
  //! STL-style iterator state shared by indexed map containers.
  template <class TheContainer, class TheValue, bool IsConstant>
  class IndexedIterator
  {
  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = TheValue;
    using difference_type   = std::ptrdiff_t;
    using container_type =
      typename std::conditional<IsConstant, const TheContainer, TheContainer>::type;
    using reference = decltype(std::declval<container_type&>()(std::declval<size_t>()));
    using pointer =
      typename std::add_pointer<typename std::remove_reference<reference>::type>::type;
    using key_reference =
      decltype(std::declval<const TheContainer&>().FindKey(std::declval<size_t>()));

    IndexedIterator() noexcept = default;

    template <bool B = IsConstant, typename std::enable_if<!B, int>::type = 0>
    explicit IndexedIterator(TheContainer& theContainer, const size_t theIndex = 1) noexcept
        : myContainer(&theContainer),
          myIndex(theIndex)
    {
    }

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    explicit IndexedIterator(const TheContainer& theContainer, const size_t theIndex = 1) noexcept
        : myContainer(&theContainer),
          myIndex(theIndex)
    {
    }

    IndexedIterator(const IndexedIterator&) noexcept            = default;
    IndexedIterator& operator=(const IndexedIterator&) noexcept = default;

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    IndexedIterator(const IndexedIterator<TheContainer, TheValue, false>& theOther) noexcept
        : myContainer(theOther.myContainer),
          myIndex(theOther.myIndex)
    {
    }

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    IndexedIterator& operator=(
      const IndexedIterator<TheContainer, TheValue, false>& theOther) noexcept
    {
      myContainer = theOther.myContainer;
      myIndex     = theOther.myIndex;
      return *this;
    }

    reference operator*() const { return (*myContainer)(myIndex); }

    pointer operator->() const { return &operator*(); }

    key_reference Key() const { return myContainer->FindKey(myIndex); }

    size_t Index() const noexcept { return myIndex; }

    const IndexedIterator& Iterator() const noexcept { return *this; }

    IndexedIterator& ChangeIterator() noexcept { return *this; }

    bool More() const noexcept { return myContainer != nullptr && myIndex <= myContainer->Size(); }

    IndexedIterator& operator++() noexcept
    {
      ++myIndex;
      return *this;
    }

    IndexedIterator operator++(int) noexcept
    {
      IndexedIterator anOld(*this);
      ++(*this);
      return anOld;
    }

    IndexedIterator& operator--() noexcept
    {
      --myIndex;
      return *this;
    }

    IndexedIterator operator--(int) noexcept
    {
      IndexedIterator anOld(*this);
      --(*this);
      return anOld;
    }

    IndexedIterator& operator+=(const difference_type theOffset) noexcept
    {
      myIndex = static_cast<size_t>(static_cast<difference_type>(myIndex) + theOffset);
      return *this;
    }

    IndexedIterator operator+(const difference_type theOffset) const noexcept
    {
      IndexedIterator anIter(*this);
      anIter += theOffset;
      return anIter;
    }

    IndexedIterator& operator-=(const difference_type theOffset) noexcept
    {
      return *this += -theOffset;
    }

    IndexedIterator operator-(const difference_type theOffset) const noexcept
    {
      IndexedIterator anIter(*this);
      anIter -= theOffset;
      return anIter;
    }

    template <bool theOtherIsConstant>
    difference_type operator-(
      const IndexedIterator<TheContainer, TheValue, theOtherIsConstant>& theOther) const noexcept
    {
      return static_cast<difference_type>(myIndex) - static_cast<difference_type>(theOther.myIndex);
    }

    reference operator[](const difference_type theOffset) const { return *(*this + theOffset); }

    template <bool theOtherIsConstant>
    bool operator==(
      const IndexedIterator<TheContainer, TheValue, theOtherIsConstant>& theOther) const noexcept
    {
      return (!More() && !theOther.More())
             || (myContainer == theOther.myContainer && myIndex == theOther.myIndex);
    }

    template <bool theOtherIsConstant>
    bool operator!=(
      const IndexedIterator<TheContainer, TheValue, theOtherIsConstant>& theOther) const noexcept
    {
      return !(*this == theOther);
    }

    template <bool theOtherIsConstant>
    bool operator<(
      const IndexedIterator<TheContainer, TheValue, theOtherIsConstant>& theOther) const noexcept
    {
      return (*this - theOther) < 0;
    }

    template <bool theOtherIsConstant>
    bool operator>(
      const IndexedIterator<TheContainer, TheValue, theOtherIsConstant>& theOther) const noexcept
    {
      return theOther < *this;
    }

    template <bool theOtherIsConstant>
    bool operator<=(
      const IndexedIterator<TheContainer, TheValue, theOtherIsConstant>& theOther) const noexcept
    {
      return !(theOther < *this);
    }

    template <bool theOtherIsConstant>
    bool operator>=(
      const IndexedIterator<TheContainer, TheValue, theOtherIsConstant>& theOther) const noexcept
    {
      return !(*this < theOther);
    }

    friend IndexedIterator operator+(const difference_type  theOffset,
                                     const IndexedIterator& theIter) noexcept
    {
      return theIter + theOffset;
    }

  private:
    template <class, class, bool>
    friend class IndexedIterator;

    container_type* myContainer = nullptr;
    size_t          myIndex     = 1;
  };

public:
  // ---------- PUBLIC METHODS ------------

  //! NbBuckets
  size_t NbBuckets() const noexcept { return myNbBuckets; }

  //! Raw bucket storage for direct standard iterator states and derived maps.
  NCollection_ListNode** Buckets() noexcept { return myData1; }

  //! Raw bucket storage for const direct standard iterator states.
  NCollection_ListNode* const* Buckets() const noexcept { return myData1; }

  //! Extent (number of elements, legacy int-returning API).
  int Extent() const noexcept { return static_cast<int>(mySize); }

  //! Length - number of elements (legacy int-returning API, synonym of Extent()).
  int Length() const noexcept { return static_cast<int>(mySize); }

  //! Size - number of elements.
  size_t Size() const noexcept { return mySize; }

  //! IsEmpty
  bool IsEmpty() const noexcept { return mySize == 0; }

  //! Returns attached allocator
  const occ::handle<NCollection_BaseAllocator>& Allocator() const noexcept { return myAllocator; }

protected:
  // -------- PROTECTED METHODS -----------

  //! Converts legacy int bucket count to size_t with validation.
  static size_t nbBucketsFromInt(const int theNbBuckets)
  {
    Standard_OutOfRange_Raise_if(theNbBuckets < 0, "NCollection_BaseMap: negative bucket count");
    return static_cast<size_t>(theNbBuckets);
  }

  static size_t NbBucketsFromInt(const int theNbBuckets) { return nbBucketsFromInt(theNbBuckets); }

  //! Constructor
  NCollection_BaseMap(const size_t                                  theNbBuckets,
                      const occ::handle<NCollection_BaseAllocator>& theAllocator)
      : myAllocator(theAllocator.IsNull() ? NCollection_BaseAllocator::CommonBaseAllocator()
                                          : theAllocator),
        myData1(nullptr),
        myNbBuckets(theNbBuckets),
        mySize(0)
  {
  }

  //! Legacy constructor retained for source compatibility with custom maps.
  NCollection_BaseMap(const size_t theNbBuckets,
                      const bool,
                      const occ::handle<NCollection_BaseAllocator>& theAllocator)
      : NCollection_BaseMap(theNbBuckets, theAllocator)
  {
  }

  //! Move Constructor
  NCollection_BaseMap(NCollection_BaseMap&& theOther) noexcept
      : myAllocator(theOther.myAllocator),
        myData1(theOther.myData1),
        myNbBuckets(theOther.myNbBuckets),
        mySize(theOther.mySize)
  {
    theOther.myData1     = nullptr;
    theOther.mySize      = 0;
    theOther.myNbBuckets = 0;
  }

  //! Destructor.
  //!
  //! Bucket storage is owned by the base itself.  Concrete map destructors
  //! normally release it through Clear(true), while direct users of the base
  //! (for example custom maps with allocator-owned nodes) still need the raw
  //! bucket table to be reclaimed here.
  virtual ~NCollection_BaseMap() { Standard::Free(myData1); }

  //! beginResize
  Standard_EXPORT bool beginResize(const size_t            theExtent,
                                   size_t&                 theNewBuckets,
                                   NCollection_ListNode**& data1) const;

  //! Legacy two-table resize entry point. The second table is obsolete for
  //! the current base-map storage and is returned as null.
  bool beginResize(const size_t            theExtent,
                   size_t&                 theNewBuckets,
                   NCollection_ListNode**& data1,
                   NCollection_ListNode**& data2) const
  {
    data2 = nullptr;
    return beginResize(theExtent, theNewBuckets, data1);
  }

  bool BeginResize(const size_t            theExtent,
                   size_t&                 theNewBuckets,
                   NCollection_ListNode**& theData1,
                   NCollection_ListNode**& theData2) const
  {
    return beginResize(theExtent, theNewBuckets, theData1, theData2);
  }

  //! endResize
  Standard_EXPORT void endResize(const size_t           theExtent,
                                 const size_t           theNewBuckets,
                                 NCollection_ListNode** data1) noexcept;

  //! Legacy two-table resize entry point.
  void endResize(const size_t           theExtent,
                 const size_t           theNewBuckets,
                 NCollection_ListNode** data1,
                 NCollection_ListNode** data2) noexcept
  {
    (void)data2;
    endResize(theExtent, theNewBuckets, data1);
  }

  void EndResize(const size_t           theExtent,
                 const size_t           theNewBuckets,
                 NCollection_ListNode** theData1,
                 NCollection_ListNode** theData2) noexcept
  {
    endResize(theExtent, theNewBuckets, theData1, theData2);
  }

  //! resizable
  bool resizable() const noexcept { return IsEmpty() || (mySize > myNbBuckets); }

  bool Resizable() const noexcept { return resizable(); }

  //! increment
  size_t increment() noexcept { return ++mySize; }

  size_t Increment() noexcept { return increment(); }

  //! decrement
  size_t decrement() noexcept { return --mySize; }

  size_t Decrement() noexcept { return decrement(); }

  //! destroy
  Standard_EXPORT void destroy(NCollection_DelMapNode fDel, bool doReleaseMemory = true);

  void Destroy(NCollection_DelMapNode fDel, bool doReleaseMemory = true)
  {
    destroy(fDel, doReleaseMemory);
  }

  //! nextPrimeForMap
  Standard_EXPORT size_t nextPrimeForMap(const size_t N) const noexcept;

  size_t NextPrimeForMap(const size_t theExtent) const noexcept
  {
    return nextPrimeForMap(theExtent);
  }

  //! Exchange content of two maps without data copying
  void exchangeMapsData(NCollection_BaseMap& theOther) noexcept
  {
    std::swap(myAllocator, theOther.myAllocator);
    std::swap(myData1, theOther.myData1);
    std::swap(myNbBuckets, theOther.myNbBuckets);
    std::swap(mySize, theOther.mySize);
  }

  void ExchangeMapsData(NCollection_BaseMap& theOther) noexcept { exchangeMapsData(theOther); }

  //! Move operator
  NCollection_BaseMap& operator=(NCollection_BaseMap&&) noexcept = delete;

  //! Copy Constructor
  NCollection_BaseMap(const NCollection_BaseMap&) = delete;

  //! Assign operator
  NCollection_BaseMap& operator=(const NCollection_BaseMap&) = delete;

protected:
  // --------- PROTECTED FIELDS -----------
  occ::handle<NCollection_BaseAllocator> myAllocator;
  NCollection_ListNode**                 myData1;

protected:
  // ---------- PRIVATE FIELDS ------------
  size_t myNbBuckets;
  size_t mySize;

private:
  // ---------- FRIEND CLASSES ------------
  friend class Iterator;
};

#endif
