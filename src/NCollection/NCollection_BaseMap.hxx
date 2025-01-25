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
    Iterator(void)
        : myNbBuckets(0),
          myBuckets(nullptr),
          myBucket(0),
          myNode(nullptr)
    {
    }

    //! Constructor
    Iterator(const NCollection_BaseMap& theMap)
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
    void Initialize(const NCollection_BaseMap& theMap)
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
    void Reset(void)
    {
      myBucket = -1;
      myNode   = nullptr;
      PNext();
    }

    //! Performs comparison of two iterators.
    Standard_Boolean IsEqual(const Iterator& theOther) const
    {
      return myBucket == theOther.myBucket && myNode == theOther.myNode;
    }

  protected:
    //! PMore
    Standard_Boolean PMore(void) const { return (myNode != nullptr); }

    //! PNext
    void PNext(void)
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
    Standard_Integer       myNbBuckets; //!< Total buckets in the map
    NCollection_ListNode** myBuckets;   //!< Location in memory
    Standard_Integer       myBucket;    //!< Current bucket
    NCollection_ListNode*  myNode;      //!< Current node
  };

public:
  // ---------- PUBLIC METHODS ------------

  //! NbBuckets
  Standard_Integer NbBuckets() const { return myNbBuckets; }

  //! Extent
  Standard_Integer Extent() const { return mySize; }

  //! IsEmpty
  Standard_Boolean IsEmpty() const { return mySize == 0; }

  //! Statistics
  Standard_EXPORT void Statistics(Standard_OStream& S) const;

  //! Returns attached allocator
  const Handle(NCollection_BaseAllocator)& Allocator() const { return myAllocator; }

protected:
  // -------- PROTECTED METHODS -----------

  //! Constructor
  NCollection_BaseMap(const Standard_Integer                   NbBuckets,
                      const Standard_Boolean                   single,
                      const Handle(NCollection_BaseAllocator)& theAllocator)
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
  virtual ~NCollection_BaseMap() {}

  //! BeginResize
  Standard_EXPORT Standard_Boolean BeginResize(const Standard_Integer  NbBuckets,
                                               Standard_Integer&       NewBuckets,
                                               NCollection_ListNode**& data1,
                                               NCollection_ListNode**& data2) const;

  //! EndResize
  Standard_EXPORT void EndResize(const Standard_Integer NbBuckets,
                                 const Standard_Integer NewBuckets,
                                 NCollection_ListNode** data1,
                                 NCollection_ListNode** data2);

  //! Resizable
  Standard_Boolean Resizable() const { return IsEmpty() || (mySize > myNbBuckets); }

  //! Increment
  Standard_Integer Increment() { return ++mySize; }

  //! Decrement
  Standard_Integer Decrement() { return --mySize; }

  //! Destroy
  Standard_EXPORT void Destroy(NCollection_DelMapNode fDel,
                               Standard_Boolean       doReleaseMemory = Standard_True);

  //! NextPrimeForMap
  Standard_EXPORT Standard_Integer NextPrimeForMap(const Standard_Integer N) const;

  //! Exchange content of two maps without data copying
  void exchangeMapsData(NCollection_BaseMap& theOther)
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
  Standard_Integer       myNbBuckets;
  Standard_Integer       mySize;
  const Standard_Boolean isDouble;

  // ---------- FRIEND CLASSES ------------
  friend class Iterator;
};

#endif
