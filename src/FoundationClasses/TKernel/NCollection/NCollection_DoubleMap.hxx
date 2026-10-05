// Created on: 2002-04-24
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

#ifndef NCollection_DoubleMap_HeaderFile
#define NCollection_DoubleMap_HeaderFile

#include <NCollection_BaseDoubleMap.hxx>
#include <NCollection_TListNode.hxx>
#include <Standard_MultiplyDefined.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>

#include <NCollection_DefaultHasher.hxx>

#include <cstddef>
#include <type_traits>
#include <utility>

/**
 * Purpose:     The DoubleMap  is used to  bind  pairs (Key1,Key2)
 *              and retrieve them in linear time.
 *
 *              See Map from NCollection for a discussion about the number
 *              of buckets
 */

template <class TheKey1Type,
          class TheKey2Type,
          class Hasher1 = NCollection_DefaultHasher<TheKey1Type>,
          class Hasher2 = NCollection_DefaultHasher<TheKey2Type>>
class NCollection_DoubleMap : public NCollection_BaseDoubleMap
{
public:
  //! STL-compliant typedef for key1 type
  typedef TheKey1Type key1_type;
  //! STL-compliant typedef for key2 type
  typedef TheKey2Type key2_type;
  typedef TheKey2Type value_type;
  using size_type       = size_t;
  using difference_type = std::ptrdiff_t;
  using reference       = const TheKey2Type&;
  using const_reference = const TheKey2Type&;
  using pointer         = const TheKey2Type*;
  using const_pointer   = const TheKey2Type*;

public:
  // **************** Adaptation of the TListNode to the DOUBLEmap
  class DoubleMapNode : public NCollection_TListNode<TheKey2Type>
  {
  public:
    //! Constructor with 'Next'
    DoubleMapNode(const TheKey1Type&    theKey1,
                  const TheKey2Type&    theKey2,
                  NCollection_ListNode* theNext1,
                  NCollection_ListNode* theNext2)
        : NCollection_TListNode<TheKey2Type>(theKey2, theNext1),
          myKey1(theKey1),
          myNext2((DoubleMapNode*)theNext2)
    {
    }

    //! Constructor with 'Next' (move Key1)
    DoubleMapNode(TheKey1Type&&         theKey1,
                  const TheKey2Type&    theKey2,
                  NCollection_ListNode* theNext1,
                  NCollection_ListNode* theNext2)
        : NCollection_TListNode<TheKey2Type>(theKey2, theNext1),
          myKey1(std::forward<TheKey1Type>(theKey1)),
          myNext2((DoubleMapNode*)theNext2)
    {
    }

    //! Constructor with 'Next' (move Key2)
    DoubleMapNode(const TheKey1Type&    theKey1,
                  TheKey2Type&&         theKey2,
                  NCollection_ListNode* theNext1,
                  NCollection_ListNode* theNext2)
        : NCollection_TListNode<TheKey2Type>(std::forward<TheKey2Type>(theKey2), theNext1),
          myKey1(theKey1),
          myNext2((DoubleMapNode*)theNext2)
    {
    }

    //! Constructor with 'Next' (move both keys)
    DoubleMapNode(TheKey1Type&&         theKey1,
                  TheKey2Type&&         theKey2,
                  NCollection_ListNode* theNext1,
                  NCollection_ListNode* theNext2)
        : NCollection_TListNode<TheKey2Type>(std::forward<TheKey2Type>(theKey2), theNext1),
          myKey1(std::forward<TheKey1Type>(theKey1)),
          myNext2((DoubleMapNode*)theNext2)
    {
    }

    //! Constructor with in-place Key2 construction
    template <typename K1, typename... Args2>
    DoubleMapNode(K1&& theKey1,
                  std::in_place_t,
                  NCollection_ListNode* theNext1,
                  NCollection_ListNode* theNext2,
                  Args2&&... theArgs2)
        : NCollection_TListNode<TheKey2Type>(std::in_place,
                                             theNext1,
                                             std::forward<Args2>(theArgs2)...),
          myKey1(std::forward<K1>(theKey1)),
          myNext2((DoubleMapNode*)theNext2)
    {
    }

    //! Key1 (mutable for destroy+reconstruct)
    TheKey1Type& Key1() noexcept { return myKey1; }

    //! Key1
    const TheKey1Type& Key1() const noexcept { return myKey1; }

    //! Key2
    const TheKey2Type& Key2() const noexcept { return this->myValue; }

    //! Next2
    DoubleMapNode*& Next2() noexcept { return myNext2; }

    //! Static deleter to be passed to BaseList
    static void delNode(NCollection_ListNode*                   theNode,
                        occ::handle<NCollection_BaseAllocator>& theAl) noexcept
    {
      ((DoubleMapNode*)theNode)->~DoubleMapNode();
      theAl->Free(theNode);
    }

  private:
    TheKey1Type    myKey1;
    DoubleMapNode* myNext2;
  };

  using iterator = NCollection_BaseMap::
    BasicIterator<NCollection_DoubleMap, DoubleMapNode, TheKey2Type, TheKey2Type, true, true>;
  using const_iterator = iterator;

public:
  // **************** Implementation of the Iterator interface.
  class Iterator : public const_iterator
  {
  public:
    //! Empty constructor
    Iterator() noexcept = default;

    //! Constructor
    Iterator(const NCollection_DoubleMap& theMap) noexcept
        : const_iterator(theMap),
          myMap(&theMap)
    {
    }

    //! Query if the end of collection is reached by iterator
    bool More() const noexcept { return !const_iterator::operator==(const_iterator()); }

    //! Make a step along the collection
    void Next() noexcept { ++(*this); }

    //! Initialize
    void Initialize(const NCollection_DoubleMap& theMap) noexcept { *this = Iterator(theMap); }

    //! Reset
    void Reset() noexcept { *this = myMap != nullptr ? Iterator(*myMap) : Iterator(); }

    //! Performs comparison of two iterators.
    bool IsEqual(const Iterator& theOther) const noexcept
    {
      return const_iterator::operator==(static_cast<const const_iterator&>(theOther));
    }

    //! Key1 inquiry
    const TheKey1Type& Key1() const
    {
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_DoubleMap::Iterator::Key1");
      return const_iterator::Node()->Key1();
    }

    //! Key2 inquiry
    const TheKey2Type& Key2() const
    {
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_DoubleMap::Iterator::Key2");
      return const_iterator::operator*();
    }

    //! Value access
    const TheKey2Type& Value() const
    {
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_DoubleMap::Iterator::Value");
      return const_iterator::operator*();
    }

  private:
    const NCollection_DoubleMap* myMap = nullptr;
  };

  //! Returns a mutable iterator over the stored second keys.
  iterator begin() noexcept { return iterator(*this); }

  //! Returns a const iterator over the stored second keys.
  const_iterator begin() const noexcept { return const_iterator(*this); }

  //! Returns the past-the-end iterator.
  iterator end() noexcept { return iterator(); }

  //! Returns the const past-the-end iterator.
  const_iterator end() const noexcept { return const_iterator(); }

  const_iterator cbegin() const noexcept { return const_iterator(*this); }

  const_iterator cend() const noexcept { return const_iterator(); }

public:
  // ---------- PUBLIC METHODS ------------

  //! Empty constructor.
  NCollection_DoubleMap()
      : NCollection_BaseDoubleMap(1, occ::handle<NCollection_BaseAllocator>())
  {
  }

  //! Constructor
  explicit NCollection_DoubleMap(
    const size_t                                  theNbBuckets,
    const occ::handle<NCollection_BaseAllocator>& theAllocator = nullptr)
      : NCollection_BaseDoubleMap(theNbBuckets, theAllocator)
  {
  }

  //! Constructor (legacy int-taking).
  explicit NCollection_DoubleMap(
    const int                                     theNbBuckets,
    const occ::handle<NCollection_BaseAllocator>& theAllocator = nullptr)
      : NCollection_DoubleMap(NCollection_BaseMap::nbBucketsFromInt(theNbBuckets), theAllocator)
  {
  }

  //! Copy constructor
  NCollection_DoubleMap(const NCollection_DoubleMap& theOther)
      : NCollection_BaseDoubleMap(theOther.NbBuckets(), theOther.myAllocator),
        myHasher1(theOther.myHasher1),
        myHasher2(theOther.myHasher2)
  {
    *this = theOther;
  }

  //! Move constructor.
  NCollection_DoubleMap(NCollection_DoubleMap&& theOther) noexcept(
    std::is_nothrow_move_constructible<Hasher1>::value
    && std::is_nothrow_move_constructible<Hasher2>::value)
      : NCollection_BaseDoubleMap(theOther.NbBuckets(), theOther.myAllocator),
        myHasher1(std::move(theOther.myHasher1)),
        myHasher2(std::move(theOther.myHasher2))
  {
    exchangeDoubleMapsData(theOther);
  }

  //! Exchange the content of two maps without re-allocations.
  //! Notice that allocators will be swapped as well!
  void Exchange(NCollection_DoubleMap& theOther) noexcept(
    noexcept(std::swap(std::declval<Hasher1&>(), std::declval<Hasher1&>()))
    && noexcept(std::swap(std::declval<Hasher2&>(), std::declval<Hasher2&>())))
  {
    std::swap(myHasher1, theOther.myHasher1);
    std::swap(myHasher2, theOther.myHasher2);
    this->exchangeDoubleMapsData(theOther);
  }

  //! Assignment.
  //! This method does not change the internal allocator.
  NCollection_DoubleMap& Assign(const NCollection_DoubleMap& theOther)
  {
    if (this == &theOther)
      return *this;

    Clear();
    int anExt = theOther.Extent();
    if (anExt)
    {
      ReSize(anExt - 1);
      Iterator anIter(theOther);
      for (; anIter.More(); anIter.Next())
      {
        TheKey1Type    aKey1 = anIter.Key1();
        TheKey2Type    aKey2 = anIter.Key2();
        const size_t   iK1   = hashCode1(aKey1, NbBuckets());
        const size_t   iK2   = hashCode2(aKey2, NbBuckets());
        DoubleMapNode* pNode =
          new (this->myAllocator) DoubleMapNode(aKey1, aKey2, myData1[iK1], myData2[iK2]);
        myData1[iK1] = pNode;
        myData2[iK2] = pNode;
        increment();
      }
    }
    return *this;
  }

  //! Assignment operator
  NCollection_DoubleMap& operator=(const NCollection_DoubleMap& theOther)
  {
    return Assign(theOther);
  }

  //! Move assignment.
  NCollection_DoubleMap& operator=(NCollection_DoubleMap&& theOther) noexcept(
    noexcept(std::swap(std::declval<Hasher1&>(), std::declval<Hasher1&>()))
    && noexcept(std::swap(std::declval<Hasher2&>(), std::declval<Hasher2&>())))
  {
    if (this != &theOther)
    {
      std::swap(myHasher1, theOther.myHasher1);
      std::swap(myHasher2, theOther.myHasher2);
      Clear(true);
      exchangeDoubleMapsData(theOther);
    }
    return *this;
  }

  //! ReSize
  void ReSize(const size_t N)
  {
    NCollection_ListNode** ppNewData1 = nullptr;
    NCollection_ListNode** ppNewData2 = nullptr;
    size_t                 newBuck;
    if (beginResize(N, newBuck, ppNewData1, ppNewData2))
    {
      if (myData1)
      {
        DoubleMapNode *p, *q;
        for (size_t i = 0; i <= NbBuckets(); ++i)
        {
          if (myData1[i])
          {
            p = (DoubleMapNode*)myData1[i];
            while (p)
            {
              const size_t iK1 = hashCode1(p->Key1(), newBuck);
              const size_t iK2 = hashCode2(p->Key2(), newBuck);
              q                = (DoubleMapNode*)p->Next();
              p->Next()        = ppNewData1[iK1];
              p->Next2()       = (DoubleMapNode*)ppNewData2[iK2];
              ppNewData1[iK1]  = p;
              ppNewData2[iK2]  = p;
              p                = q;
            }
          }
        }
      }
      endResize(N, newBuck, ppNewData1, ppNewData2);
    }
  }

  void ReSize(const int N)
  {
    Standard_OutOfRange_Raise_if(N < 0, "NCollection_DoubleMap::ReSize: negative size");
    ReSize(static_cast<size_t>(N));
  }

  //! Bind binds the pair (Key1, Key2).
  //! @throw Standard_MultiplyDefined if Key1 or Key2 is already bound
  void Bind(const TheKey1Type& theKey1, const TheKey2Type& theKey2)
  {
    bindImpl(theKey1, theKey2, std::false_type{});
  }

  //! Bind binds the pair (Key1, Key2).
  //! @throw Standard_MultiplyDefined if Key1 or Key2 is already bound
  void Bind(TheKey1Type&& theKey1, const TheKey2Type& theKey2)
  {
    bindImpl(std::move(theKey1), theKey2, std::false_type{});
  }

  //! Bind binds the pair (Key1, Key2).
  //! @throw Standard_MultiplyDefined if Key1 or Key2 is already bound
  void Bind(const TheKey1Type& theKey1, TheKey2Type&& theKey2)
  {
    bindImpl(theKey1, std::move(theKey2), std::false_type{});
  }

  //! Bind binds the pair (Key1, Key2).
  //! @throw Standard_MultiplyDefined if Key1 or Key2 is already bound
  void Bind(TheKey1Type&& theKey1, TheKey2Type&& theKey2)
  {
    bindImpl(std::move(theKey1), std::move(theKey2), std::false_type{});
  }

  //! TryBind binds the pair (Key1, Key2) only if neither key is already bound.
  //! @param theKey1 first key to bind
  //! @param theKey2 second key to bind
  //! @return true if pair was successfully bound, false if either key already exists
  bool TryBind(const TheKey1Type& theKey1, const TheKey2Type& theKey2)
  {
    return bindImpl(theKey1, theKey2, std::true_type{});
  }

  //! TryBind binds the pair (Key1, Key2) only if neither key is already bound.
  //! @param theKey1 first key to bind
  //! @param theKey2 second key to bind
  //! @return true if pair was successfully bound, false if either key already exists
  bool TryBind(TheKey1Type&& theKey1, const TheKey2Type& theKey2)
  {
    return bindImpl(std::move(theKey1), theKey2, std::true_type{});
  }

  //! TryBind binds the pair (Key1, Key2) only if neither key is already bound.
  //! @param theKey1 first key to bind
  //! @param theKey2 second key to bind
  //! @return true if pair was successfully bound, false if either key already exists
  bool TryBind(const TheKey1Type& theKey1, TheKey2Type&& theKey2)
  {
    return bindImpl(theKey1, std::move(theKey2), std::true_type{});
  }

  //! TryBind binds the pair (Key1, Key2) only if neither key is already bound.
  //! @param theKey1 first key to bind
  //! @param theKey2 second key to bind
  //! @return true if pair was successfully bound, false if either key already exists
  bool TryBind(TheKey1Type&& theKey1, TheKey2Type&& theKey2)
  {
    return bindImpl(std::move(theKey1), std::move(theKey2), std::true_type{});
  }

  //! TryEmplace constructs keys in-place only if neither key is already bound.
  //! @param theKey1 first key to bind
  //! @param theKey2 second key to bind
  //! @return true if pair was successfully bound, false if either key already exists
  template <typename K1, typename K2>
  bool TryEmplace(K1&& theKey1, K2&& theKey2)
  {
    return bindImpl(std::forward<K1>(theKey1), std::forward<K2>(theKey2), std::true_type{});
  }

  //!* AreBound
  bool AreBound(const TheKey1Type& theKey1, const TheKey2Type& theKey2) const
  {
    if (IsEmpty())
      return false;
    const size_t   iK1 = hashCode1(theKey1, NbBuckets());
    const size_t   iK2 = hashCode2(theKey2, NbBuckets());
    DoubleMapNode *pNode1, *pNode2;
    pNode1 = (DoubleMapNode*)myData1[iK1];
    while (pNode1)
    {
      if (isEqual1(pNode1->Key1(), theKey1))
        break;
      pNode1 = (DoubleMapNode*)pNode1->Next();
    }
    if (pNode1 == NULL)
      return false;
    pNode2 = (DoubleMapNode*)myData2[iK2];
    while (pNode2)
    {
      if (isEqual2(pNode2->Key2(), theKey2))
        break;
      pNode2 = (DoubleMapNode*)pNode2->Next();
    }
    if (pNode2 == NULL)
      return false;

    return (pNode1 == pNode2);
  }

  //! IsBound1
  bool IsBound1(const TheKey1Type& theKey1) const
  {
    if (IsEmpty())
      return false;
    const size_t   iK1 = hashCode1(theKey1, NbBuckets());
    DoubleMapNode* pNode1;
    pNode1 = (DoubleMapNode*)myData1[iK1];
    while (pNode1)
    {
      if (isEqual1(pNode1->Key1(), theKey1))
        return true;
      pNode1 = (DoubleMapNode*)pNode1->Next();
    }
    return false;
  }

  //! IsBound2
  bool IsBound2(const TheKey2Type& theKey2) const
  {
    if (IsEmpty())
      return false;
    const size_t   iK2 = hashCode2(theKey2, NbBuckets());
    DoubleMapNode* pNode2;
    pNode2 = (DoubleMapNode*)myData2[iK2];
    while (pNode2)
    {
      if (isEqual2(pNode2->Key2(), theKey2))
        return true;
      pNode2 = (DoubleMapNode*)pNode2->Next2();
    }
    return false;
  }

  //! UnBind1
  bool UnBind1(const TheKey1Type& theKey1)
  {
    if (IsEmpty())
      return false;
    const size_t   iK1 = hashCode1(theKey1, NbBuckets());
    DoubleMapNode *p1, *p2, *q1, *q2;
    q1 = q2 = nullptr;
    p1      = (DoubleMapNode*)myData1[iK1];
    while (p1)
    {
      if (isEqual1(p1->Key1(), theKey1))
      {
        // remove from the data1
        if (q1)
          q1->Next() = p1->Next();
        else
          myData1[iK1] = (DoubleMapNode*)p1->Next();
        const size_t iK2 = hashCode2(p1->Key2(), NbBuckets());
        p2               = (DoubleMapNode*)myData2[iK2];
        while (p2)
        {
          if (p2 == p1)
          {
            // remove from the data2
            if (q2)
              q2->Next2() = p2->Next2();
            else
              myData2[iK2] = (DoubleMapNode*)p2->Next2();
            break;
          }
          q2 = p2;
          p2 = (DoubleMapNode*)p2->Next2();
        }
        p1->~DoubleMapNode();
        this->myAllocator->Free(p1);
        decrement();
        return true;
      }
      q1 = p1;
      p1 = (DoubleMapNode*)p1->Next();
    }
    return false;
  }

  //! UnBind2
  bool UnBind2(const TheKey2Type& theKey2)
  {
    if (IsEmpty())
      return false;
    const size_t   iK2 = hashCode2(theKey2, NbBuckets());
    DoubleMapNode *p1, *p2, *q1, *q2;
    q1 = q2 = nullptr;
    p2      = (DoubleMapNode*)myData2[iK2];
    while (p2)
    {
      if (isEqual2(p2->Key2(), theKey2))
      {
        // remove from the data2
        if (q2)
        {
          q2->Next2() = p2->Next2();
        }
        else
          myData2[iK2] = (DoubleMapNode*)p2->Next2();
        const size_t iK1 = hashCode1(p2->Key1(), NbBuckets());
        p1               = (DoubleMapNode*)myData1[iK1];
        while (p1)
        {
          if (p1 == p2)
          {
            // remove from the data1
            if (q1)
              q1->Next() = p1->Next();
            else
              myData1[iK1] = (DoubleMapNode*)p1->Next();
            break;
          }
          q1 = p1;
          p1 = (DoubleMapNode*)p1->Next();
        }
        p2->~DoubleMapNode();
        this->myAllocator->Free(p2);
        decrement();
        return true;
      }
      q2 = p2;
      p2 = (DoubleMapNode*)p2->Next2();
    }
    return false;
  }

  //! Find the Key1 and return Key2 value.
  //! Raises an exception if Key1 was not bound.
  const TheKey2Type& Find1(const TheKey1Type& theKey1) const
  {
    if (const TheKey2Type* aKey2 = Seek1(theKey1))
    {
      return *aKey2;
    }
    throw Standard_NoSuchObject("NCollection_DoubleMap::Find1");
  }

  //! Find the Key1 and return Key2 value (by copying its value).
  //! @param[in]   theKey1 Key1 to find
  //! @param[out]  theKey2 Key2 to return
  //! @return TRUE if Key1 has been found
  bool Find1(const TheKey1Type& theKey1, TheKey2Type& theKey2) const
  {
    if (const TheKey2Type* aKey2 = Seek1(theKey1))
    {
      theKey2 = *aKey2;
      return true;
    }
    return false;
  }

  //! Find the Key1 and return pointer to Key2 or NULL if Key1 is not bound.
  //! @param[in]   theKey1 Key1 to find
  //! @return pointer to Key2 or NULL if Key1 is not found
  const TheKey2Type* Seek1(const TheKey1Type& theKey1) const
  {
    for (DoubleMapNode* aNode1 =
           !IsEmpty() ? (DoubleMapNode*)myData1[hashCode1(theKey1, NbBuckets())] : nullptr;
         aNode1 != nullptr;
         aNode1 = (DoubleMapNode*)aNode1->Next())
    {
      if (isEqual1(aNode1->Key1(), theKey1))
      {
        return &aNode1->Key2();
      }
    }
    return nullptr;
  }

  //! Find the Key2 and return Key1 value.
  //! Raises an exception if Key2 was not bound.
  const TheKey1Type& Find2(const TheKey2Type& theKey2) const
  {
    if (const TheKey1Type* aVal1 = Seek2(theKey2))
    {
      return *aVal1;
    }
    throw Standard_NoSuchObject("NCollection_DoubleMap::Find2");
  }

  //! Find the Key2 and return Key1 value (by copying its value).
  //! @param[in]   theKey2 Key2 to find
  //! @param[out]  theKey1 Key1 to return
  //! @return TRUE if Key2 has been found
  bool Find2(const TheKey2Type& theKey2, TheKey1Type& theKey1) const
  {
    if (const TheKey1Type* aVal1 = Seek2(theKey2))
    {
      theKey1 = *aVal1;
      return true;
    }
    return false;
  }

  //! Find the Key2 and return pointer to Key1 or NULL if not bound.
  //! @param[in]  theKey2 Key2 to find
  //! @return pointer to Key1 if Key2 has been found
  const TheKey1Type* Seek2(const TheKey2Type& theKey2) const
  {
    for (DoubleMapNode* aNode2 =
           !IsEmpty() ? (DoubleMapNode*)myData2[hashCode2(theKey2, NbBuckets())] : nullptr;
         aNode2 != nullptr;
         aNode2 = (DoubleMapNode*)aNode2->Next2())
    {
      if (isEqual2(aNode2->Key2(), theKey2))
      {
        return &aNode2->Key1();
      }
    }
    return nullptr;
  }

  //! Clear data. If doReleaseMemory is false then the table of
  //! buckets is not released and will be reused.
  void Clear(const bool doReleaseMemory = false)
  {
    destroy(DoubleMapNode::delNode, doReleaseMemory);
  }

  //! Clear data and reset allocator
  void Clear(const occ::handle<NCollection_BaseAllocator>& theAllocator)
  {
    Clear(true);
    this->myAllocator =
      (!theAllocator.IsNull() ? theAllocator : NCollection_BaseAllocator::CommonBaseAllocator());
  }

  //! Destructor
  ~NCollection_DoubleMap() { Clear(true); }

protected:
  bool isEqual1(const TheKey1Type& theKey1, const TheKey1Type& theKey2) const
  {
    return myHasher1(theKey1, theKey2);
  }

  size_t hashCode1(const TheKey1Type& theKey, const size_t theUpperBound) const
  {
    return myHasher1(theKey) % theUpperBound + 1;
  }

  bool isEqual2(const TheKey2Type& theKey1, const TheKey2Type& theKey2) const
  {
    return myHasher2(theKey1, theKey2);
  }

  size_t hashCode2(const TheKey2Type& theKey, const size_t theUpperBound) const
  {
    return myHasher2(theKey) % theUpperBound + 1;
  }

  //! Lookup for particular key1 in map.
  //! @param[in] theKey1 key to search
  //! @param[out] theNode the detected node with equal key. Can be null.
  //! @param[out] theHash computed bounded hash code for current key.
  //! @return true if key1 is found
  bool lookup1(const TheKey1Type& theKey1, DoubleMapNode*& theNode, size_t& theHash) const
  {
    theHash = hashCode1(theKey1, NbBuckets());
    if (IsEmpty())
      return false;
    for (theNode = (DoubleMapNode*)myData1[theHash]; theNode;
         theNode = (DoubleMapNode*)theNode->Next())
    {
      if (isEqual1(theNode->Key1(), theKey1))
        return true;
    }
    return false;
  }

  //! Lookup for particular key2 in map.
  //! @param[in] theKey2 key to search
  //! @param[out] theNode the detected node with equal key. Can be null.
  //! @param[out] theHash computed bounded hash code for current key.
  //! @return true if key2 is found
  bool lookup2(const TheKey2Type& theKey2, DoubleMapNode*& theNode, size_t& theHash) const
  {
    theHash = hashCode2(theKey2, NbBuckets());
    if (IsEmpty())
      return false;
    for (theNode = (DoubleMapNode*)myData2[theHash]; theNode;
         theNode = (DoubleMapNode*)theNode->Next2())
    {
      if (isEqual2(theNode->Key2(), theKey2))
        return true;
    }
    return false;
  }

  //! Implementation helper for Bind/TryBind.
  //! @tparam K1 forwarding reference type for key1
  //! @tparam K2 forwarding reference type for key2
  //! @tparam IsTry if true, returns false on existing key; if false, throws exception
  //! @param theKey1 first key to bind
  //! @param theKey2 second key to bind
  //! @return true if pair was successfully bound, false if either key already exists (only for
  //! IsTry=true)
  template <typename K1, typename K2, bool IsTry>
  bool bindImpl(K1&& theKey1, K2&& theKey2, std::bool_constant<IsTry>)
  {
    if (resizable())
      ReSize(Extent());
    DoubleMapNode* aNode;
    size_t         iK1, iK2;
    if (lookup1(theKey1, aNode, iK1))
    {
      if constexpr (IsTry)
        return false;
      else
        throw Standard_MultiplyDefined("NCollection_DoubleMap:Bind");
    }
    if (lookup2(theKey2, aNode, iK2))
    {
      if constexpr (IsTry)
        return false;
      else
        throw Standard_MultiplyDefined("NCollection_DoubleMap:Bind");
    }
    DoubleMapNode* pNode = new (this->myAllocator) DoubleMapNode(std::forward<K1>(theKey1),
                                                                 std::forward<K2>(theKey2),
                                                                 myData1[iK1],
                                                                 myData2[iK2]);
    myData1[iK1]         = pNode;
    myData2[iK2]         = pNode;
    increment();
    return true;
  }

  Hasher1 myHasher1;
  Hasher2 myHasher2;
};

#endif
