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

#ifndef NCollection_IndexedDataMap_HeaderFile
#define NCollection_IndexedDataMap_HeaderFile

#include <NCollection_BaseMap.hxx>
#include <NCollection_TListNode.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Standard_NoSuchObject.hxx>
#include <NCollection_StlIterator.hxx>
#include <NCollection_DefaultHasher.hxx>

#include <Standard_OutOfRange.hxx>
#include <utility>

/**
 * Purpose:     An indexed map is used  to store keys and to  bind
 *              an index to them.  Each  new key stored in the map
 *              gets an index.  Index are  incremented as keys are
 *              stored in the map. A key can be found by the index
 *              and an index by the key.  No  key but the last can
 *              be  removed so the  indices   are in the range 1..
 *              Extent.  An Item is stored with each key.
 *
 *              This   class is   similar  to  IndexedMap     from
 *              NCollection  with the Item as  a new feature. Note
 *              the important difference on  the operator  ().  In
 *              the IndexedMap this operator returns  the Key.  In
 *              the IndexedDataMap this operator returns the Item.
 *
 *              See  the  class   Map   from NCollection   for   a
 *              discussion about the number of buckets.
 */

template <class TheKeyType, class TheItemType, class Hasher = NCollection_DefaultHasher<TheKeyType>>
class NCollection_IndexedDataMap : public NCollection_BaseMap
{
public:
  //! STL-compliant typedef for key type
  typedef TheKeyType key_type;
  //! STL-compliant typedef for value type
  typedef TheItemType value_type;
  typedef Hasher      hasher;

private:
  //!    Adaptation of the TListNode to the INDEXEDDatamap
  class IndexedDataMapNode : public NCollection_TListNode<TheItemType>
  {
  public:
    //! Constructor with 'Next'
    IndexedDataMapNode(const TheKeyType&      theKey1,
                       const Standard_Integer theIndex,
                       const TheItemType&     theItem,
                       NCollection_ListNode*  theNext1)
        : NCollection_TListNode<TheItemType>(theItem, theNext1),
          myKey1(theKey1),
          myIndex(theIndex)
    {
    }

    //! Constructor with 'Next'
    IndexedDataMapNode(TheKeyType&&           theKey1,
                       const Standard_Integer theIndex,
                       const TheItemType&     theItem,
                       NCollection_ListNode*  theNext1)
        : NCollection_TListNode<TheItemType>(theItem, theNext1),
          myKey1(std::forward<TheKeyType>(theKey1)),
          myIndex(theIndex)
    {
    }

    //! Constructor with 'Next'
    IndexedDataMapNode(const TheKeyType&      theKey1,
                       const Standard_Integer theIndex,
                       TheItemType&&          theItem,
                       NCollection_ListNode*  theNext1)
        : NCollection_TListNode<TheItemType>(std::forward<TheItemType>(theItem), theNext1),
          myKey1(theKey1),
          myIndex(theIndex)
    {
    }

    //! Constructor with 'Next'
    IndexedDataMapNode(TheKeyType&&           theKey1,
                       const Standard_Integer theIndex,
                       TheItemType&&          theItem,
                       NCollection_ListNode*  theNext1)
        : NCollection_TListNode<TheItemType>(std::forward<TheItemType>(theItem), theNext1),
          myKey1(std::forward<TheKeyType>(theKey1)),
          myIndex(theIndex)
    {
    }

    //! Key1
    TheKeyType& Key1() { return myKey1; }

    //! Index
    Standard_Integer& Index() { return myIndex; }

    //! Static deleter to be passed to BaseList
    static void delNode(NCollection_ListNode* theNode, Handle(NCollection_BaseAllocator)& theAl)
    {
      ((IndexedDataMapNode*)theNode)->~IndexedDataMapNode();
      theAl->Free(theNode);
    }

  private:
    TheKeyType       myKey1;
    Standard_Integer myIndex;
  };

public:
  //!   Implementation of the Iterator interface.
  class Iterator
  {
  public:
    //! Empty constructor
    Iterator()
        : myMap(NULL),
          myIndex(0)
    {
    }

    //! Constructor
    Iterator(const NCollection_IndexedDataMap& theMap)
        : myMap((NCollection_IndexedDataMap*)&theMap),
          myIndex(1)
    {
    }

    //! Query if the end of collection is reached by iterator
    Standard_Boolean More(void) const { return (myMap != NULL) && (myIndex <= myMap->Extent()); }

    //! Make a step along the collection
    void Next(void) { ++myIndex; }

    //! Value access
    const TheItemType& Value(void) const
    {
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_IndexedDataMap::Iterator::Value");
      return myMap->FindFromIndex(myIndex);
    }

    //! ChangeValue access
    TheItemType& ChangeValue(void) const
    {
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_IndexedDataMap::Iterator::ChangeValue");
      return myMap->ChangeFromIndex(myIndex);
    }

    //! Key
    const TheKeyType& Key() const
    {
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_IndexedDataMap::Iterator::Key");
      return myMap->FindKey(myIndex);
    }

    //! Performs comparison of two iterators.
    Standard_Boolean IsEqual(const Iterator& theOther) const
    {
      return myMap == theOther.myMap && myIndex == theOther.myIndex;
    }

  private:
    NCollection_IndexedDataMap* myMap;   //!< Pointer to current node
    Standard_Integer            myIndex; //!< Current index
  };

  //! Shorthand for a regular iterator type.
  typedef NCollection_StlIterator<std::forward_iterator_tag, Iterator, TheItemType, false> iterator;

  //! Shorthand for a constant iterator type.
  typedef NCollection_StlIterator<std::forward_iterator_tag, Iterator, TheItemType, true>
    const_iterator;

  //! Returns an iterator pointing to the first element in the map.
  iterator begin() const { return Iterator(*this); }

  //! Returns an iterator referring to the past-the-end element in the map.
  iterator end() const { return Iterator(); }

  //! Returns a const iterator pointing to the first element in the map.
  const_iterator cbegin() const { return Iterator(*this); }

  //! Returns a const iterator referring to the past-the-end element in the map.
  const_iterator cend() const { return Iterator(); }

public:
  // ---------- PUBLIC METHODS ------------

  //! Empty constructor.
  NCollection_IndexedDataMap()
      : NCollection_BaseMap(1, true, Handle(NCollection_BaseAllocator)())
  {
  }

  //! Constructor
  explicit NCollection_IndexedDataMap(const Standard_Integer                   theNbBuckets,
                                      const Handle(NCollection_BaseAllocator)& theAllocator = 0L)
      : NCollection_BaseMap(theNbBuckets, true, theAllocator)
  {
  }

  //! Copy constructor
  NCollection_IndexedDataMap(const NCollection_IndexedDataMap& theOther)
      : NCollection_BaseMap(theOther.NbBuckets(), true, theOther.myAllocator)
  {
    *this = theOther;
  }

  //! Move constructor
  NCollection_IndexedDataMap(NCollection_IndexedDataMap&& theOther) noexcept
      : NCollection_BaseMap(std::forward<NCollection_BaseMap>(theOther))
  {
  }

  //! Exchange the content of two maps without re-allocations.
  //! Notice that allocators will be swapped as well!
  void Exchange(NCollection_IndexedDataMap& theOther) { this->exchangeMapsData(theOther); }

  //! Assignment.
  //! This method does not change the internal allocator.
  NCollection_IndexedDataMap& Assign(const NCollection_IndexedDataMap& theOther)
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
        const TheKeyType&   aKey1  = theOther.FindKey(anIndexIter);
        const TheItemType&  anItem = theOther.FindFromIndex(anIndexIter);
        const size_t        iK1    = HashCode(aKey1, NbBuckets());
        IndexedDataMapNode* pNode =
          new (this->myAllocator) IndexedDataMapNode(aKey1, anIndexIter, anItem, myData1[iK1]);
        myData1[iK1]             = pNode;
        myData2[anIndexIter - 1] = pNode;
        Increment();
      }
    }
    return *this;
  }

  //! Assignment operator
  NCollection_IndexedDataMap& operator=(const NCollection_IndexedDataMap& theOther)
  {
    return Assign(theOther);
  }

  //! Move operator
  NCollection_IndexedDataMap& operator=(NCollection_IndexedDataMap&& theOther) noexcept
  {
    if (this == &theOther)
      return *this;
    exchangeMapsData(theOther);
    return *this;
  }

  //! ReSize
  void ReSize(const Standard_Integer N)
  {
    NCollection_ListNode** ppNewData1 = NULL;
    NCollection_ListNode** ppNewData2 = NULL;
    Standard_Integer       newBuck;
    if (BeginResize(N, newBuck, ppNewData1, ppNewData2))
    {
      if (myData1)
      {
        for (Standard_Integer aBucketIter = 0; aBucketIter <= NbBuckets(); ++aBucketIter)
        {
          if (myData1[aBucketIter])
          {
            IndexedDataMapNode* p = (IndexedDataMapNode*)myData1[aBucketIter];
            while (p)
            {
              const size_t        iK1 = HashCode(p->Key1(), newBuck);
              IndexedDataMapNode* q   = (IndexedDataMapNode*)p->Next();
              p->Next()               = ppNewData1[iK1];
              ppNewData1[iK1]         = p;
              p                       = q;
            }
          }
        }
      }
      EndResize(N,
                newBuck,
                ppNewData1,
                (NCollection_ListNode**)
                  Standard::Reallocate(myData2, (newBuck + 1) * sizeof(NCollection_ListNode*)));
    }
  }

  //! Returns the Index of already bound Key or appends new Key with specified Item value.
  //! @param theKey1 Key to search (and to bind, if it was not bound already)
  //! @param theItem Item value to set for newly bound Key; ignored if Key was already bound
  //! @return index of Key
  Standard_Integer Add(const TheKeyType& theKey1, const TheItemType& theItem)
  {
    if (Resizable())
    {
      ReSize(Extent());
    }
    IndexedDataMapNode* aNode;
    size_t              aHash;
    if (lookup(theKey1, aNode, aHash))
    {
      return aNode->Index();
    }
    const Standard_Integer aNewIndex = Increment();
    aNode = new (this->myAllocator) IndexedDataMapNode(theKey1, aNewIndex, theItem, myData1[aHash]);
    myData1[aHash]         = aNode;
    myData2[aNewIndex - 1] = aNode;
    return aNewIndex;
  }

  //! Returns the Index of already bound Key or appends new Key with specified Item value.
  //! @param theKey1 Key to search (and to bind, if it was not bound already)
  //! @param theItem Item value to set for newly bound Key; ignored if Key was already bound
  //! @return index of Key
  Standard_Integer Add(TheKeyType&& theKey1, const TheItemType& theItem)
  {
    if (Resizable())
    {
      ReSize(Extent());
    }
    IndexedDataMapNode* aNode;
    size_t              aHash;
    if (lookup(theKey1, aNode, aHash))
    {
      return aNode->Index();
    }
    const Standard_Integer aNewIndex = Increment();
    aNode                            = new (this->myAllocator)
      IndexedDataMapNode(std::forward<TheKeyType>(theKey1), aNewIndex, theItem, myData1[aHash]);
    myData1[aHash]         = aNode;
    myData2[aNewIndex - 1] = aNode;
    return aNewIndex;
  }

  //! Returns the Index of already bound Key or appends new Key with specified Item value.
  //! @param theKey1 Key to search (and to bind, if it was not bound already)
  //! @param theItem Item value to set for newly bound Key; ignored if Key was already bound
  //! @return index of Key
  Standard_Integer Add(const TheKeyType& theKey1, TheItemType&& theItem)
  {
    if (Resizable())
    {
      ReSize(Extent());
    }
    IndexedDataMapNode* aNode;
    size_t              aHash;
    if (lookup(theKey1, aNode, aHash))
    {
      return aNode->Index();
    }
    const Standard_Integer aNewIndex = Increment();
    aNode                            = new (this->myAllocator)
      IndexedDataMapNode(theKey1, aNewIndex, std::forward<TheItemType>(theItem), myData1[aHash]);
    myData1[aHash]         = aNode;
    myData2[aNewIndex - 1] = aNode;
    return aNewIndex;
  }

  //! Returns the Index of already bound Key or appends new Key with specified Item value.
  //! @param theKey1 Key to search (and to bind, if it was not bound already)
  //! @param theItem Item value to set for newly bound Key; ignored if Key was already bound
  //! @return index of Key
  Standard_Integer Add(TheKeyType&& theKey1, TheItemType&& theItem)
  {
    if (Resizable())
    {
      ReSize(Extent());
    }
    IndexedDataMapNode* aNode;
    size_t              aHash;
    if (lookup(theKey1, aNode, aHash))
    {
      return aNode->Index();
    }
    const Standard_Integer aNewIndex = Increment();
    aNode          = new (this->myAllocator) IndexedDataMapNode(std::forward<TheKeyType>(theKey1),
                                                       aNewIndex,
                                                       std::forward<TheItemType>(theItem),
                                                       myData1[aHash]);
    myData1[aHash] = aNode;
    myData2[aNewIndex - 1] = aNode;
    return aNewIndex;
  }

  //! Contains
  Standard_Boolean Contains(const TheKeyType& theKey1) const
  {
    IndexedDataMapNode* aNode;
    if (lookup(theKey1, aNode))
    {
      return true;
    }
    return false;
  }

  //! Substitute
  void Substitute(const Standard_Integer theIndex,
                  const TheKeyType&      theKey1,
                  const TheItemType&     theItem)
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > Extent(),
                                 "NCollection_IndexedDataMap::Substitute : "
                                 "Index is out of range");

    // check if theKey1 is not already in the map
    size_t              aHash;
    IndexedDataMapNode* aNode;
    if (lookup(theKey1, aNode, aHash))
    {
      if (aNode->Index() != theIndex)
      {
        throw Standard_DomainError("NCollection_IndexedDataMap::Substitute : "
                                   "Attempt to substitute existing key");
      }
      aNode->Key1()        = theKey1;
      aNode->ChangeValue() = theItem;
      return;
    }

    // Find the node for the index I
    aNode = (IndexedDataMapNode*)myData2[theIndex - 1];

    // remove the old key
    const size_t        iK = HashCode(aNode->Key1(), NbBuckets());
    IndexedDataMapNode* q  = (IndexedDataMapNode*)myData1[iK];
    if (q == aNode)
      myData1[iK] = (IndexedDataMapNode*)aNode->Next();
    else
    {
      while (q->Next() != aNode)
        q = (IndexedDataMapNode*)q->Next();
      q->Next() = aNode->Next();
    }

    // update the node
    aNode->Key1()        = theKey1;
    aNode->ChangeValue() = theItem;
    aNode->Next()        = myData1[aHash];
    myData1[aHash]       = aNode;
  }

  //! Swaps two elements with the given indices.
  void Swap(const Standard_Integer theIndex1, const Standard_Integer theIndex2)
  {
    Standard_OutOfRange_Raise_if(theIndex1 < 1 || theIndex1 > Extent() || theIndex2 < 1
                                   || theIndex2 > Extent(),
                                 "NCollection_IndexedDataMap::Swap");

    if (theIndex1 == theIndex2)
    {
      return;
    }

    IndexedDataMapNode* aP1 = (IndexedDataMapNode*)myData2[theIndex1 - 1];
    IndexedDataMapNode* aP2 = (IndexedDataMapNode*)myData2[theIndex2 - 1];
    std::swap(aP1->Index(), aP2->Index());
    myData2[theIndex2 - 1] = aP1;
    myData2[theIndex1 - 1] = aP2;
  }

  //! RemoveLast
  void RemoveLast(void)
  {
    const Standard_Integer aLastIndex = Extent();
    Standard_OutOfRange_Raise_if(aLastIndex == 0, "NCollection_IndexedDataMap::RemoveLast");

    // Find the node for the last index and remove it
    IndexedDataMapNode* p   = (IndexedDataMapNode*)myData2[aLastIndex - 1];
    myData2[aLastIndex - 1] = NULL;

    // remove the key
    const size_t        iK1 = HashCode(p->Key1(), NbBuckets());
    IndexedDataMapNode* q   = (IndexedDataMapNode*)myData1[iK1];
    if (q == p)
      myData1[iK1] = (IndexedDataMapNode*)p->Next();
    else
    {
      while (q->Next() != p)
        q = (IndexedDataMapNode*)q->Next();
      q->Next() = p->Next();
    }
    p->~IndexedDataMapNode();
    this->myAllocator->Free(p);
    Decrement();
  }

  //! Remove the key of the given index.
  //! Caution! The index of the last key can be changed.
  void RemoveFromIndex(const Standard_Integer theIndex)
  {
    const Standard_Integer aLastInd = Extent();
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > aLastInd,
                                 "NCollection_IndexedDataMap::Remove");
    if (theIndex != aLastInd)
    {
      Swap(theIndex, aLastInd);
    }
    RemoveLast();
  }

  //! Remove the given key.
  //! Caution! The index of the last key can be changed.
  void RemoveKey(const TheKeyType& theKey1)
  {
    Standard_Integer anIndToRemove = FindIndex(theKey1);
    if (anIndToRemove > 0)
    {
      RemoveFromIndex(anIndToRemove);
    }
  }

  //! FindKey
  const TheKeyType& FindKey(const Standard_Integer theIndex) const
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > Extent(),
                                 "NCollection_IndexedDataMap::FindKey");
    IndexedDataMapNode* aNode = (IndexedDataMapNode*)myData2[theIndex - 1];
    return aNode->Key1();
  }

  //! FindFromIndex
  const TheItemType& FindFromIndex(const Standard_Integer theIndex) const
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > Extent(),
                                 "NCollection_IndexedDataMap::FindFromIndex");
    IndexedDataMapNode* aNode = (IndexedDataMapNode*)myData2[theIndex - 1];
    return aNode->Value();
  }

  //! operator ()
  const TheItemType& operator()(const Standard_Integer theIndex) const
  {
    return FindFromIndex(theIndex);
  }

  //! ChangeFromIndex
  TheItemType& ChangeFromIndex(const Standard_Integer theIndex)
  {
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > Extent(),
                                 "NCollection_IndexedDataMap::ChangeFromIndex");
    IndexedDataMapNode* aNode = (IndexedDataMapNode*)myData2[theIndex - 1];
    return aNode->ChangeValue();
  }

  //! operator ()
  TheItemType& operator()(const Standard_Integer theIndex) { return ChangeFromIndex(theIndex); }

  //! FindIndex
  Standard_Integer FindIndex(const TheKeyType& theKey1) const
  {
    IndexedDataMapNode* aNode;
    if (lookup(theKey1, aNode))
    {
      return aNode->Index();
    }
    return 0;
  }

  //! FindFromKey
  const TheItemType& FindFromKey(const TheKeyType& theKey1) const
  {
    Standard_NoSuchObject_Raise_if(IsEmpty(), "NCollection_IndexedDataMap::FindFromKey");
    IndexedDataMapNode* aNode;
    if (lookup(theKey1, aNode))
    {
      return aNode->Value();
    }
    throw Standard_NoSuchObject("NCollection_IndexedDataMap::FindFromKey");
  }

  //! ChangeFromKey
  TheItemType& ChangeFromKey(const TheKeyType& theKey1)
  {
    Standard_NoSuchObject_Raise_if(IsEmpty(), "NCollection_IndexedDataMap::ChangeFromKey");
    IndexedDataMapNode* aNode;
    if (lookup(theKey1, aNode))
    {
      return aNode->ChangeValue();
    }
    throw Standard_NoSuchObject("NCollection_IndexedDataMap::ChangeFromKey");
  }

  //! Seek returns pointer to Item by Key. Returns
  //! NULL if Key was not found.
  const TheItemType* Seek(const TheKeyType& theKey1) const
  {
    return const_cast<NCollection_IndexedDataMap*>(this)->ChangeSeek(theKey1);
  }

  //! ChangeSeek returns modifiable pointer to Item by Key. Returns
  //! NULL if Key was not found.
  TheItemType* ChangeSeek(const TheKeyType& theKey1)
  {
    IndexedDataMapNode* aNode;
    if (lookup(theKey1, aNode))
    {
      return &aNode->ChangeValue();
    }
    return nullptr;
  }

  //! Find value for key with copying.
  //! @return true if key was found
  Standard_Boolean FindFromKey(const TheKeyType& theKey1, TheItemType& theValue) const
  {
    IndexedDataMapNode* aNode;
    if (lookup(theKey1, aNode))
    {
      theValue = aNode->Value();
      return Standard_True;
    }
    return Standard_False;
  }

  //! Clear data. If doReleaseMemory is false then the table of
  //! buckets is not released and will be reused.
  void Clear(const Standard_Boolean doReleaseMemory = Standard_False)
  {
    Destroy(IndexedDataMapNode::delNode, doReleaseMemory);
  }

  //! Clear data and reset allocator
  void Clear(const Handle(NCollection_BaseAllocator)& theAllocator)
  {
    Clear(theAllocator != this->myAllocator);
    this->myAllocator =
      (!theAllocator.IsNull() ? theAllocator : NCollection_BaseAllocator::CommonBaseAllocator());
  }

  //! Destructor
  virtual ~NCollection_IndexedDataMap(void) { Clear(true); }

  //! Size
  Standard_Integer Size(void) const { return Extent(); }

protected:
  //! Lookup for particular key in map.
  //! @param[in] theKey key to compute hash
  //! @param[out] theNode the detected node with equal key. Can be null.
  //! @param[out] theHash computed bounded hash code for current key.
  //! @return true if key is found
  Standard_Boolean lookup(const TheKeyType&    theKey,
                          IndexedDataMapNode*& theNode,
                          size_t&              theHash) const
  {
    theHash = HashCode(theKey, NbBuckets());
    if (IsEmpty())
      return Standard_False; // Not found
    for (theNode = (IndexedDataMapNode*)myData1[theHash]; theNode;
         theNode = (IndexedDataMapNode*)theNode->Next())
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
  Standard_Boolean lookup(const TheKeyType& theKey, IndexedDataMapNode*& theNode) const
  {
    if (IsEmpty())
      return Standard_False; // Not found
    for (theNode = (IndexedDataMapNode*)myData1[HashCode(theKey, NbBuckets())]; theNode;
         theNode = (IndexedDataMapNode*)theNode->Next())
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
