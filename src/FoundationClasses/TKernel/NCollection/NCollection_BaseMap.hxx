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
#include <NCollection_BaseAllocator.hxx>
#include <NCollection_DefineAlloc.hxx>
#include <NCollection_ListNode.hxx>

#include <cstring>
#include <type_traits>

typedef void (*NCollection_DelMapNode)(NCollection_ListNode*,
                                       occ::handle<NCollection_BaseAllocator>& theAl);

/**
 * Purpose:     This is a base class for all Maps:
 *                Map
 *                DataMap
 *                DoubleMap
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
          myBucket(-1),
          myNode(nullptr)
    {
      if (!myBuckets)
        myNbBuckets = -1;
      else
        do
        {
          myBucket++;
          if (myBucket > myNbBuckets)
            return;
          myNode = myBuckets[myBucket];
        } while (!myNode);
    }

  public:
    //! Initialize
    void Initialize(const NCollection_BaseMap& theMap) noexcept
    {
      myNbBuckets = theMap.myNbBuckets;
      myBuckets   = theMap.myData1;
      myBucket    = -1;
      myNode      = nullptr;
      if (!myBuckets)
        myNbBuckets = -1;
      PNext();
    }

    //! Reset
    void Reset() noexcept
    {
      myBucket = -1;
      myNode   = nullptr;
      PNext();
    }

    //! Performs comparison of two iterators.
    bool IsEqual(const Iterator& theOther) const noexcept
    {
      return myBucket == theOther.myBucket && myNode == theOther.myNode;
    }

  protected:
    //! PMore
    bool PMore() const noexcept { return (myNode != nullptr); }

    //! PNext
    void PNext() noexcept
    {
      if (!myBuckets)
        return;
      if (myNode)
      {
        myNode = myNode->Next();
        if (myNode)
          return;
      }
      while (!myNode)
      {
        myBucket++;
        if (myBucket > myNbBuckets)
          return;
        myNode = myBuckets[myBucket];
      }
    }

  protected:
    // ---------- PRIVATE FIELDS ------------
    int                    myNbBuckets; //!< Total buckets in the map
    NCollection_ListNode** myBuckets;   //!< Location in memory
    int                    myBucket;    //!< Current bucket
    NCollection_ListNode*  myNode;      //!< Current node
  };

public:
  // ---------- PUBLIC METHODS ------------

  //! NbBuckets
  int NbBuckets() const noexcept { return myNbBuckets; }

  //! Extent
  int Extent() const noexcept { return mySize; }

  //! IsEmpty
  bool IsEmpty() const noexcept { return mySize == 0; }

  //! Statistics
  Standard_DEPRECATED("Statistics() is deprecated and will be removed.")
  Standard_EXPORT void Statistics(Standard_OStream& S) const;

  //! Returns attached allocator
  const occ::handle<NCollection_BaseAllocator>& Allocator() const noexcept { return myAllocator; }

protected:
  // -------- PROTECTED METHODS -----------

  //! Constructor
  NCollection_BaseMap(const int                                     NbBuckets,
                      const bool                                    single,
                      const occ::handle<NCollection_BaseAllocator>& theAllocator)
      : myAllocator(theAllocator.IsNull() ? NCollection_BaseAllocator::CommonBaseAllocator()
                                          : theAllocator),
        myData1(nullptr),
        myData2(nullptr),
        myNbBuckets(NbBuckets),
        mySize(0),
        isDouble(!single)
  {
  }

  //! Move Constructor
  NCollection_BaseMap(NCollection_BaseMap&& theOther) noexcept
      : myAllocator(theOther.myAllocator),
        myData1(theOther.myData1),
        myData2(theOther.myData2),
        myNbBuckets(theOther.myNbBuckets),
        mySize(theOther.mySize),
        isDouble(theOther.isDouble)
  {
    theOther.myData1     = nullptr;
    theOther.myData2     = nullptr;
    theOther.mySize      = 0;
    theOther.myNbBuckets = 0;
  }

  //! Destructor
  ~NCollection_BaseMap() = default;

  //! BeginResize
  Standard_EXPORT bool BeginResize(const int               NbBuckets,
                                   int&                    NewBuckets,
                                   NCollection_ListNode**& data1,
                                   NCollection_ListNode**& data2) const;

  //! EndResize
  Standard_EXPORT void EndResize(const int              NbBuckets,
                                 const int              NewBuckets,
                                 NCollection_ListNode** data1,
                                 NCollection_ListNode** data2) noexcept;

  //! Resizable
  bool Resizable() const noexcept { return IsEmpty() || (mySize > myNbBuckets); }

  //! Increment
  int Increment() noexcept { return ++mySize; }

  //! Decrement
  int Decrement() noexcept { return --mySize; }

  //! Destroy
  Standard_EXPORT void Destroy(NCollection_DelMapNode fDel, bool doReleaseMemory = true);

  //! Template-based clear for derived map classes.
  //! Destroys all nodes inline (no function pointer indirection), skips
  //! unnecessary zeroing on the release-memory path, and uses if constexpr
  //! to skip destructor calls for trivially destructible node types.
  //! @tparam NodeType the concrete node type used by the map
  //! @param doReleaseMemory if true, free bucket arrays after clearing
  template <class NodeType>
  void clearNodes(const bool doReleaseMemory)
  {
    if (!IsEmpty())
    {
      const int aNbBuckets = NbBuckets();
      for (int anInd = 0; anInd <= aNbBuckets; anInd++)
      {
        NCollection_ListNode* aCur = myData1[anInd];
        while (aCur)
        {
          NCollection_ListNode* aNext = aCur->Next();
          if constexpr (!std::is_trivially_destructible_v<NodeType>)
            static_cast<NodeType*>(aCur)->~NodeType();
          myAllocator->Free(aCur);
          aCur = aNext;
        }
      }
      if (!doReleaseMemory)
      {
        memset(myData1, 0, (aNbBuckets + 1) * sizeof(NCollection_ListNode*));
        if (myData2)
          memset(myData2, 0, (aNbBuckets + 1) * sizeof(NCollection_ListNode*));
      }
      mySize = 0;
    }
    if (doReleaseMemory)
    {
      if (myData1)
        Standard::Free(myData1);
      if (myData2)
        Standard::Free(myData2);
      myData1 = myData2 = nullptr;
    }
  }

  //! NextPrimeForMap
  Standard_EXPORT int NextPrimeForMap(const int N) const noexcept;

  //! Exchange content of two maps without data copying
  void exchangeMapsData(NCollection_BaseMap& theOther) noexcept
  {
    std::swap(myAllocator, theOther.myAllocator);
    std::swap(myData1, theOther.myData1);
    std::swap(myData2, theOther.myData2);
    std::swap(myNbBuckets, theOther.myNbBuckets);
    std::swap(mySize, theOther.mySize);
  }

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
  NCollection_ListNode**                 myData2;

private:
  // ---------- PRIVATE FIELDS ------------
  int        myNbBuckets;
  int        mySize;
  const bool isDouble;

  // ---------- FRIEND CLASSES ------------
  friend class Iterator;
};

#endif
