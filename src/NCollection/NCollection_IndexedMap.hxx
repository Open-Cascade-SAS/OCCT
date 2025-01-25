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

#ifndef NCollection_IndexedMap_HeaderFile
#define NCollection_IndexedMap_HeaderFile

#include <NCollection_BaseMap.hxx>
#include <NCollection_TListNode.hxx>
#include <NCollection_StlIterator.hxx>
#include <Standard_NoSuchObject.hxx>

#include <NCollection_DefaultHasher.hxx>

#include <Standard_OutOfRange.hxx>

/**
 * Purpose:     An indexed map is used to  store  keys and to bind
 *              an index to them.  Each new key stored in  the map
 *              gets an index.  Index are incremented  as keys are
 *              stored in the map. A key can be found by the index
 *              and an index by the  key. No key  but the last can
 *              be removed so the indices are in the range 1..Extent.
 *              See  the  class   Map   from NCollection   for   a
 *              discussion about the number of buckets.
 */

template <class TheKeyType, class Hasher = NCollection_DefaultHasher<TheKeyType>>
class NCollection_IndexedMap : public NCollection_BaseMap
{
public:
  //! STL-compliant typedef for key type
  typedef TheKeyType key_type;

protected:
  //! Adaptation of the TListNode to the INDEXEDmap
  class IndexedMapNode : public NCollection_TListNode<TheKeyType>
  {
  public:
    //! Constructor with 'Next'
    IndexedMapNode(const TheKeyType&      theKey1,
                   const Standard_Integer theIndex,
                   NCollection_ListNode*  theNext1)
        : NCollection_TListNode<TheKeyType>(theKey1, theNext1),
          myIndex(theIndex)
    {
    }

    //! Constructor with 'Next'
    IndexedMapNode(TheKeyType&&           theKey1,
                   const Standard_Integer theIndex,
                   NCollection_ListNode*  theNext1)
        : NCollection_TListNode<TheKeyType>(std::forward<TheKeyType>(theKey1), theNext1),
          myIndex(theIndex)
    {
    }

    //! Key1
    TheKeyType& Key1() { return this->ChangeValue(); }

    //! Index
    Standard_Integer& Index() { return myIndex; }

    //! Static deleter to be passed to BaseList
    static void delNode(NCollection_ListNode* theNode, Handle(NCollection_BaseAllocator)& theAl)
    {
      ((IndexedMapNode*)theNode)->~IndexedMapNode();
      theAl->Free(theNode);
    }

  private:
    Standard_Integer myIndex;
  };

public:
  // **************** Implementation of the Iterator interface.
  class Iterator
  {
  public:
    //! Empty constructor
    Iterator(void)
        : myMap(NULL),
          myIndex(0)
    {
    }

    //! Constructor
    Iterator(const NCollection_IndexedMap& theMap)
        : myMap((NCollection_IndexedMap*)&theMap),
          myIndex(1)
    {
    }

    //! Query if the end of collection is reached by iterator
    Standard_Boolean More(void) const { return (myMap != NULL) && (myIndex <= myMap->Extent()); }

    //! Make a step along the collection
    void Next(void) { myIndex++; }

    //! Value access
    const TheKeyType& Value(void) const
    {
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_IndexedMap::Iterator::Value");
      return myMap->FindKey(myIndex);
    }

    //! Performs comparison of two iterators.
    Standard_Boolean IsEqual(const Iterator& theOther) const
    {
      return myMap == theOther.myMap && myIndex == theOther.myIndex;
    }

  private:
    NCollection_IndexedMap* myMap;   // Pointer to the map being iterated
    Standard_Integer        myIndex; // Current index
  };

  //! Shorthand for a constant iterator type.
  typedef NCollection_StlIterator<std::forward_iterator_tag, Iterator, TheKeyType, true>
    const_iterator;

  //! Returns a const iterator pointing to the first element in the map.
  const_iterator cbegin() const { return Iterator(*this); }

  //! Returns a const iterator referring to the past-the-end element in the map.
  const_iterator cend() const { return Iterator(); }

public:
  // ---------- PUBLIC METHODS ------------

  //! Empty constructor.
  NCollection_IndexedMap()
      : NCollection_BaseMap(1, true, Handle(NCollection_BaseAllocator)())
  {
  }

  //! Constructor
  explicit NCollection_IndexedMap(const Standard_Integer                   theNbBuckets,
                                  const Handle(NCollection_BaseAllocator)& theAllocator = 0L)
      : NCollection_BaseMap(theNbBuckets, true, theAllocator)
  {
  }

  //! Copy constructor
  NCollection_IndexedMap(const NCollection_IndexedMap& theOther)
      : NCollection_BaseMap(theOther.NbBuckets(), true, theOther.myAllocator)
  {
    *this = theOther;
  }

  //! Move constructor
  NCollection_IndexedMap(NCollection_IndexedMap&& theOther) noexcept
      : NCollection_BaseMap(std::forward<NCollection_BaseMap>(theOther))
  {
  }

  //! Exchange the content of two maps without re-allocations.
  //! Notice that allocators will be swapped as well!
  void Exchange(NCollection_IndexedMap& theOther) { this->exchangeMapsData(theOther); }

  //! Assign.
  //! This method does not change the internal allocator.
  NCollection_IndexedMap& Assign(const NCollection_IndexedMap& theOther)
  {
    if (this == &theOther)
      return *this;

    Clear();
    Standard_Integer anExt = theOther.Extent();
    if (anExt)
    {
      ReSize(anExt - 1); // mySize is same after resize
      for (Standard_Integer anIndexIter = 1; anIndexIter <= anExt; ++anIndexIter)
      {
        const TheKeyType& aKey1 = theOther.FindKey(anIndexIter);
        const size_t      iK1   = HashCode(aKey1, NbBuckets());
        IndexedMapNode*   pNode =
          new (this->myAllocator) IndexedMapNode(aKey1, anIndexIter, myData1[iK1]);
        myData1[iK1]             = pNode;
        myData2[anIndexIter - 1] = pNode;
        Increment();
      }
    }
    return *this;
  }

  //! Assignment operator
  NCollection_IndexedMap& operator=(const NCollection_IndexedMap& theOther)
  {
    return Assign(theOther);
  }

  //! Move operator
  NCollection_IndexedMap& operator=(NCollection_IndexedMap&& theOther) noexcept
  {
    if (this == &theOther)
      return *this;
    exchangeMapsData(theOther);
    return *this;
  }

  //! ReSize
  void ReSize(const Standard_Integer theExtent)
  {
    NCollection_ListNode** ppNewData1 = NULL;
    NCollection_ListNode** ppNewData2 = NULL;
    Standard_Integer       newBuck;
    if (BeginResize(theExtent, newBuck, ppNewData1, ppNewData2))
    {
      if (myData1)
      {
        for (Standard_Integer aBucketIter = 0; aBucketIter <= NbBuckets(); ++aBucketIter)
        {
          if (myData1[aBucketIter])
          {
            IndexedMapNode* p = (IndexedMapNode*)myData1[aBucketIter];
            while (p)
            {
              const size_t    iK1 = HashCode(p->Key1(), newBuck);
              IndexedMapNode* q   = (IndexedMapNode*)p->Next();
              p->Next()           = ppNewData1[iK1];
              ppNewData1[iK1]     = p;
              p                   = q;
            }
          }
        }
      }
      EndResize(theExtent,
                newBuck,
                ppNewData1,
                (NCollection_ListNode**)
                  Standard::Reallocate(myData2, (newBuck + 1) * sizeof(NCollection_ListNode*)));
    }
  }

  //! Add
  Standard_Integer Add(const TheKeyType& theKey1)
  {
    if (Resizable())
    {
      ReSize(Extent());
    }
    IndexedMapNode* aNode;
    size_t          aHash;
    if (lookup(theKey1, aNode, aHash))
    {
      return aNode->Index();
    }
    const Standard_Integer aNewIndex = Increment();
    aNode          = new (this->myAllocator) IndexedMapNode(theKey1, aNewIndex, myData1[aHash]);
    myData1[aHash] = aNode;
    myData2[aNewIndex - 1] = aNode;
    return aNewIndex;
  }

  //! Add
  Standard_Integer Add(TheKeyType&& theKey1)
  {
    if (Resizable())
    {
      ReSize(Extent());
    }
    size_t          aHash;
    IndexedMapNode* aNode;
    if (lookup(theKey1, aNode, aHash))
    {
      return aNode->Index();
    }
    const Standard_Integer aNewIndex = Increment();
    aNode                            = new (this->myAllocator)
      IndexedMapNode(std::forward<TheKeyType>(theKey1), aNewIndex, myData1[aHash]);
    myData1[aHash]         = aNode;
    myData2[aNewIndex - 1] = aNode;
    return aNewIndex;
  }

  //! Contains
  Standard_Boolean Contains(const TheKeyType& theKey1) const
  {
    IndexedMapNode* p;
    return lookup(theKey1, p);
  }

  //! Substitute
  void Substitute(const Standard_Integer theIndex, const TheKeyType& theKey1)
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > Extent(),
                                 "NCollection_IndexedMap::Substitute : "
                                 "Index is out of range");

    // check if theKey1 is not already in the map
    IndexedMapNode* aNode;
    size_t          aHash;
    if (lookup(theKey1, aNode, aHash))
    {
      if (aNode->Index() != theIndex)
      {
        throw Standard_DomainError("NCollection_IndexedMap::Substitute : "
                                   "Attempt to substitute existing key");
      }
      aNode->Key1() = theKey1;
      return;
    }
    // Find the node for the index I
    aNode = (IndexedMapNode*)myData2[theIndex - 1];

    // remove the old key
    const size_t    iK = HashCode(aNode->Key1(), NbBuckets());
    IndexedMapNode* q  = (IndexedMapNode*)myData1[iK];
    if (q == aNode)
      myData1[iK] = (IndexedMapNode*)aNode->Next();
    else
    {
      while (q->Next() != aNode)
        q = (IndexedMapNode*)q->Next();
      q->Next() = aNode->Next();
    }

    // update the node
    aNode->Key1()  = theKey1;
    aNode->Next()  = myData1[aHash];
    myData1[aHash] = aNode;
  }

  //! Swaps two elements with the given indices.
  void Swap(const Standard_Integer theIndex1, const Standard_Integer theIndex2)
  {
    Standard_OutOfRange_Raise_if(theIndex1 < 1 || theIndex1 > Extent() || theIndex2 < 1
                                   || theIndex2 > Extent(),
                                 "NCollection_IndexedMap::Swap");

    if (theIndex1 == theIndex2)
    {
      return;
    }

    IndexedMapNode* aP1 = (IndexedMapNode*)myData2[theIndex1 - 1];
    IndexedMapNode* aP2 = (IndexedMapNode*)myData2[theIndex2 - 1];
    std::swap(aP1->Index(), aP2->Index());
    myData2[theIndex2 - 1] = aP1;
    myData2[theIndex1 - 1] = aP2;
  }

  //! RemoveLast
  void RemoveLast(void)
  {
    const Standard_Integer aLastIndex = Extent();
    Standard_OutOfRange_Raise_if(aLastIndex == 0, "NCollection_IndexedMap::RemoveLast");

    // Find the node for the last index and remove it
    IndexedMapNode* p       = (IndexedMapNode*)myData2[aLastIndex - 1];
    myData2[aLastIndex - 1] = NULL;

    // remove the key
    const size_t    iK1 = HashCode(p->Key1(), NbBuckets());
    IndexedMapNode* q   = (IndexedMapNode*)myData1[iK1];
    if (q == p)
      myData1[iK1] = (IndexedMapNode*)p->Next();
    else
    {
      while (q->Next() != p)
        q = (IndexedMapNode*)q->Next();
      q->Next() = p->Next();
    }
    p->~IndexedMapNode();
    this->myAllocator->Free(p);
    Decrement();
  }

  //! Remove the key of the given index.
  //! Caution! The index of the last key can be changed.
  void RemoveFromIndex(const Standard_Integer theIndex)
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > Extent(),
                                 "NCollection_IndexedMap::RemoveFromIndex");
    const Standard_Integer aLastInd = Extent();
    if (theIndex != aLastInd)
    {
      Swap(theIndex, aLastInd);
    }
    RemoveLast();
  }

  //! Remove the given key.
  //! Caution! The index of the last key can be changed.
  Standard_Boolean RemoveKey(const TheKeyType& theKey1)
  {
    Standard_Integer anIndToRemove = FindIndex(theKey1);
    if (anIndToRemove < 1)
    {
      return Standard_False;
    }

    RemoveFromIndex(anIndToRemove);
    return Standard_True;
  }

  //! FindKey
  const TheKeyType& FindKey(const Standard_Integer theIndex) const
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > Extent(),
                                 "NCollection_IndexedMap::FindKey");
    IndexedMapNode* pNode2 = (IndexedMapNode*)myData2[theIndex - 1];
    return pNode2->Key1();
  }

  //! operator ()
  const TheKeyType& operator()(const Standard_Integer theIndex) const { return FindKey(theIndex); }

  //! FindIndex
  Standard_Integer FindIndex(const TheKeyType& theKey1) const
  {
    IndexedMapNode* aNode;
    if (lookup(theKey1, aNode))
    {
      return aNode->Index();
    }
    return 0;
  }

  //! Clear data. If doReleaseMemory is false then the table of
  //! buckets is not released and will be reused.
  void Clear(const Standard_Boolean doReleaseMemory = Standard_False)
  {
    Destroy(IndexedMapNode::delNode, doReleaseMemory);
  }

  //! Clear data and reset allocator
  void Clear(const Handle(NCollection_BaseAllocator)& theAllocator)
  {
    Clear(theAllocator != this->myAllocator);
    this->myAllocator =
      (!theAllocator.IsNull() ? theAllocator : NCollection_BaseAllocator::CommonBaseAllocator());
  }

  //! Destructor
  virtual ~NCollection_IndexedMap(void) { Clear(true); }

  //! Size
  Standard_Integer Size(void) const { return Extent(); }

protected:
  //! Lookup for particular key in map.
  //! @param[in] theKey key to compute hash
  //! @param[out] theNode the detected node with equal key. Can be null.
  //! @param[out] theHash computed bounded hash code for current key.
  //! @return true if key is found
  Standard_Boolean lookup(const TheKeyType& theKey, IndexedMapNode*& theNode, size_t& theHash) const
  {
    theHash = HashCode(theKey, NbBuckets());
    if (IsEmpty())
      return Standard_False; // Not found
    for (theNode = (IndexedMapNode*)myData1[theHash]; theNode;
         theNode = (IndexedMapNode*)theNode->Next())
    {
      if (IsEqual(theNode->Key1(), theKey))
        return Standard_True;
    }
    return Standard_False; // Not found
  }

  //! Lookup for particular key in map.
  //! @param[in] theKey key to compute hash
  //! @param[out] theNode the detected node with equal key. Can be null.
  //! @return true if key is found
  Standard_Boolean lookup(const TheKeyType& theKey, IndexedMapNode*& theNode) const
  {
    if (IsEmpty())
      return Standard_False; // Not found
    for (theNode = (IndexedMapNode*)myData1[HashCode(theKey, NbBuckets())]; theNode;
         theNode = (IndexedMapNode*)theNode->Next())
    {
      if (IsEqual(theNode->Key1(), theKey))
      {
        return Standard_True;
      }
    }
    return Standard_False; // Not found
  }

  bool IsEqual(const TheKeyType& theKey1, const TheKeyType& theKey2) const
  {
    return myHasher(theKey1, theKey2);
  }

  size_t HashCode(const TheKeyType& theKey, const int theUpperBound) const
  {
    return myHasher(theKey) % theUpperBound + 1;
  }

protected:
  Hasher myHasher;
};

#endif
