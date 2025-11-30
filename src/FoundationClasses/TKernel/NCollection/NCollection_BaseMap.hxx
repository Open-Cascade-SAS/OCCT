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

#include <cstddef>

typedef void (*NCollection_DelMapNode)(NCollection_ListNode*,
                                       Handle(NCollection_BaseAllocator)& theAl);

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
          myBucket(SIZE_MAX),
          myNode(nullptr)
    {
      if (myBuckets)
      {
        PNext();
      }
    }

  public:
    //! Initialize
    void Initialize(const NCollection_BaseMap& theMap) noexcept
    {
      myNbBuckets = theMap.myNbBuckets;
      myBuckets   = theMap.myData1;
      myBucket    = SIZE_MAX;
      myNode      = nullptr;
      if (myBuckets)
      {
        PNext();
      }
    }

    //! Reset
    void Reset() noexcept
    {
      myBucket = SIZE_MAX;
      myNode   = nullptr;
      if (myBuckets)
      {
        PNext();
      }
    }

    //! Performs comparison of two iterators.
    bool IsEqual(const Iterator& theOther) const noexcept
    {
      return myBucket == theOther.myBucket && myNode == theOther.myNode;
    }

  protected:
    //! PMore
    bool PMore() const noexcept { return myNode != nullptr; }

    //! PNext
    void PNext() noexcept
    {
      if (myNode)
      {
        myNode = myNode->Next();
        if (myNode)
        {
          return;
        }
      }
      ++myBucket;
      while (myBucket < myNbBuckets)
      {
        myNode = myBuckets[myBucket];
        if (myNode)
        {
          return;
        }
        ++myBucket;
      }
    }

  protected:
    // ---------- PRIVATE FIELDS ------------
    size_t                 myNbBuckets; //!< Total buckets in the map
    NCollection_ListNode** myBuckets;   //!< Location in memory
    size_t                 myBucket;    //!< Current bucket
    NCollection_ListNode*  myNode;      //!< Current node
  };

public:
  // ---------- PUBLIC METHODS ------------

  //! NbBuckets
  Standard_Integer NbBuckets() const noexcept { return static_cast<Standard_Integer>(myNbBuckets); }

  //! Extent
  Standard_Integer Extent() const noexcept { return static_cast<Standard_Integer>(mySize); }

  //! IsEmpty
  bool IsEmpty() const noexcept { return mySize == 0; }

  //! Statistics
  Standard_EXPORT void Statistics(Standard_OStream& S) const;

  //! Returns attached allocator
  const Handle(NCollection_BaseAllocator)& Allocator() const noexcept { return myAllocator; }

protected:
  // -------- PROTECTED METHODS -----------

  //! Constructor
  NCollection_BaseMap(const size_t                             theNbBuckets,
                      const bool                               theSingle,
                      const Handle(NCollection_BaseAllocator)& theAllocator)
      : myAllocator(theAllocator.IsNull() ? NCollection_BaseAllocator::CommonBaseAllocator()
                                          : theAllocator),
        myData1(nullptr),
        myData2(nullptr),
        myNbBuckets(theNbBuckets),
        mySize(0),
        myIsDouble(!theSingle)
  {
  }

  //! Move Constructor
  NCollection_BaseMap(NCollection_BaseMap&& theOther) noexcept
      : myAllocator(theOther.myAllocator),
        myData1(theOther.myData1),
        myData2(theOther.myData2),
        myNbBuckets(theOther.myNbBuckets),
        mySize(theOther.mySize),
        myIsDouble(theOther.myIsDouble)
  {
    theOther.myData1     = nullptr;
    theOther.myData2     = nullptr;
    theOther.mySize      = 0;
    theOther.myNbBuckets = 0;
  }

  //! Destructor
  virtual ~NCollection_BaseMap() {}

  //! BeginResize
  Standard_EXPORT bool BeginResize(const size_t            theNbBuckets,
                                   size_t&                 theNewBuckets,
                                   NCollection_ListNode**& theData1,
                                   NCollection_ListNode**& theData2) const;

  //! EndResize
  Standard_EXPORT void EndResize(const size_t           theNewBuckets,
                                 NCollection_ListNode** theData1,
                                 NCollection_ListNode** theData2) noexcept;

  //! Resizable
  bool Resizable() const noexcept { return IsEmpty() || (mySize > myNbBuckets); }

  //! Increment
  size_t Increment() noexcept { return ++mySize; }

  //! Decrement
  size_t Decrement() noexcept { return --mySize; }

  //! Destroy
  Standard_EXPORT void Destroy(NCollection_DelMapNode fDel, bool doReleaseMemory = true);

  //! NextPrimeForMap
  Standard_EXPORT size_t NextPrimeForMap(const size_t theN) const noexcept;

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
  Handle(NCollection_BaseAllocator) myAllocator;
  NCollection_ListNode**            myData1;
  NCollection_ListNode**            myData2;

private:
  // ---------- PRIVATE FIELDS ------------
  size_t     myNbBuckets;
  size_t     mySize;
  const bool myIsDouble;

  // ---------- FRIEND CLASSES ------------
  friend class Iterator;
};

#endif
