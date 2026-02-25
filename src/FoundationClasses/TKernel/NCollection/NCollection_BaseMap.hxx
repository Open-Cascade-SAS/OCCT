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

typedef void (*NCollection_DelMapNode)(NCollection_ListNode*,
                                       occ::handle<NCollection_BaseAllocator>& theAl);

//! Destruct-only callback: destructs node values but does NOT free memory.
//! Used by free-list caching to retain node memory for reuse.
typedef void (*NCollection_DestroyMapNode)(NCollection_ListNode*);

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
        myCachedData(nullptr),
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
        myCachedData(theOther.myCachedData),
        myNbBuckets(theOther.myNbBuckets),
        mySize(theOther.mySize),
        isDouble(theOther.isDouble)
  {
    theOther.myData1      = nullptr;
    theOther.myData2      = nullptr;
    theOther.myCachedData = nullptr;
    theOther.mySize       = 0;
    theOther.myNbBuckets  = 0;
  }

  //! Destructor
  virtual ~NCollection_BaseMap() = default;

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
  Standard_EXPORT void Destroy(NCollection_DelMapNode fDel,
                               bool                   doReleaseMemory = true);

  //! NextPrimeForMap
  Standard_EXPORT int NextPrimeForMap(const int N) const noexcept;

  //! Pop a node from the free-list cache, or return nullptr if empty.
  NCollection_ListNode* allocateFromCache() noexcept
  {
    if (myCachedData == nullptr)
    {
      return nullptr;
    }
    NCollection_ListNode* aNode = myCachedData;
    myCachedData                = myCachedData->Next();
    return aNode;
  }

  //! Destruct node values and push node memory onto the free-list cache.
  void cacheNode(NCollection_ListNode* theNode, NCollection_DestroyMapNode fDestroy) noexcept
  {
    fDestroy(theNode);
    theNode->Next() = myCachedData;
    myCachedData    = theNode;
  }

  //! Walk the free-list calling allocator Free on each cached node.
  Standard_EXPORT void freeCachedNodes();

  //! Exchange content of two maps without data copying
  void exchangeMapsData(NCollection_BaseMap& theOther) noexcept
  {
    std::swap(myAllocator, theOther.myAllocator);
    std::swap(myData1, theOther.myData1);
    std::swap(myData2, theOther.myData2);
    std::swap(myCachedData, theOther.myCachedData);
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
  NCollection_ListNode*                  myCachedData; //!< Free-list cache of removed nodes

private:
  // ---------- PRIVATE FIELDS ------------
  int        myNbBuckets;
  int        mySize;
  const bool isDouble;

  // ---------- FRIEND CLASSES ------------
  friend class Iterator;
};

#endif
