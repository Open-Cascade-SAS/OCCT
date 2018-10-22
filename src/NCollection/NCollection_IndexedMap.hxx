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

template < class TheKeyType, 
           class Hasher = NCollection_DefaultHasher<TheKeyType> > 
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
    IndexedMapNode (const TheKeyType&      theKey1, 
                    const Standard_Integer theIndex,
                    NCollection_ListNode*  theNext1)
    : NCollection_TListNode<TheKeyType> (theKey1, theNext1),
      myIndex (theIndex)
    {
    }
    //! Key1
    TheKeyType& Key1() { return this->ChangeValue(); }

    //! Index
    Standard_Integer& Index() { return myIndex; }
    
    //! Static deleter to be passed to BaseList
    static void delNode (NCollection_ListNode * theNode, 
                         Handle(NCollection_BaseAllocator)& theAl)
    {
      ((IndexedMapNode *) theNode)->~IndexedMapNode();
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
    Iterator (void) :
      myMap(NULL),
      myIndex(0) {}
    //! Constructor
    Iterator (const NCollection_IndexedMap& theMap) :
      myMap((NCollection_IndexedMap *) &theMap),
      myIndex(1) {}
    //! Query if the end of collection is reached by iterator
    Standard_Boolean More(void) const
    { return (myMap != NULL) && (myIndex <= myMap->Extent()); }
    //! Make a step along the collection
    void Next(void)
    { myIndex++; }
    //! Value access
    const TheKeyType& Value(void) const
    {
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_IndexedMap::Iterator::Value");
      return myMap->FindKey(myIndex);
    }

    //! Performs comparison of two iterators.
    Standard_Boolean IsEqual (const Iterator& theOther) const
    {
      return myMap == theOther.myMap && myIndex == theOther.myIndex;
    }
    
  private:
    NCollection_IndexedMap * myMap;   // Pointer to the map being iterated
    Standard_Integer         myIndex; // Current index
  };
  
  //! Shorthand for a constant iterator type.
  typedef NCollection_StlIterator<std::forward_iterator_tag, Iterator, TheKeyType, true> const_iterator;

  //! Returns a const iterator pointing to the first element in the map.
  const_iterator cbegin() const { return Iterator (*this); }

  //! Returns a const iterator referring to the past-the-end element in the map.
  const_iterator cend() const { return Iterator(); }
  
 public:
  // ---------- PUBLIC METHODS ------------

  //! Empty constructor.
  NCollection_IndexedMap() : NCollection_BaseMap (1, Standard_False, Handle(NCollection_BaseAllocator)()) {}

  //! Constructor
  explicit NCollection_IndexedMap (const Standard_Integer theNbBuckets,
                                   const Handle(NCollection_BaseAllocator)& theAllocator=0L)
  : NCollection_BaseMap (theNbBuckets, Standard_False, theAllocator) {}

  //! Copy constructor
  NCollection_IndexedMap (const NCollection_IndexedMap& theOther)
  : NCollection_BaseMap (theOther.NbBuckets(), Standard_False, theOther.myAllocator)
  { *this = theOther; }

  //! Exchange the content of two maps without re-allocations.
  //! Notice that allocators will be swapped as well!
  void Exchange (NCollection_IndexedMap& theOther)
  {
    this->exchangeMapsData (theOther);
  }

  //! Assign.
  //! This method does not change the internal allocator.
  NCollection_IndexedMap& Assign (const NCollection_IndexedMap& theOther)
  { 
    if (this == &theOther)
      return *this;

    Clear();
    Standard_Integer anExt = theOther.Extent();
    if (anExt)
    {
      ReSize (anExt-1); //mySize is same after resize
      for (Standard_Integer anIndexIter = 1; anIndexIter <= anExt; ++anIndexIter)
      {
        const TheKeyType& aKey1 = theOther.FindKey (anIndexIter);
        const Standard_Integer iK1 = Hasher::HashCode (aKey1, NbBuckets());
        IndexedMapNode* pNode = new (this->myAllocator) IndexedMapNode (aKey1, anIndexIter, myData1[iK1]);
        myData1[iK1]             = pNode;
        myData2[anIndexIter - 1] = pNode;
        Increment();
      }
    }
    return *this;
  }

  //! Assignment operator
  NCollection_IndexedMap& operator= (const NCollection_IndexedMap& theOther)
  {
    return Assign (theOther);
  }

  //! ReSize
  void ReSize (const Standard_Integer theExtent)
  {
    NCollection_ListNode** ppNewData1 = NULL;
    NCollection_ListNode** ppNewData2 = NULL;
    Standard_Integer newBuck;
    if (BeginResize (theExtent, newBuck, ppNewData1, ppNewData2))
    {
      if (myData1) 
      {
        memcpy (ppNewData2, myData2, sizeof(IndexedMapNode*) * Extent());
        for (Standard_Integer aBucketIter = 0; aBucketIter <= NbBuckets(); ++aBucketIter)
        {
          if (myData1[aBucketIter])
          {
            IndexedMapNode* p = (IndexedMapNode* )myData1[aBucketIter];
            while (p) 
            {
              const Standard_Integer iK1 = Hasher::HashCode (p->Key1(), newBuck);
              IndexedMapNode* q = (IndexedMapNode* )p->Next();
              p->Next() = ppNewData1[iK1];
              ppNewData1[iK1] = p;
              p = q;
            }
          }
        }
      }
      EndResize (theExtent, newBuck, ppNewData1, ppNewData2);
    }
  }

  //! Add
  Standard_Integer Add (const TheKeyType& theKey1)
  {
    if (Resizable())
    {
      ReSize (Extent());
    }

    Standard_Integer iK1 = Hasher::HashCode (theKey1, NbBuckets());
    IndexedMapNode* pNode = (IndexedMapNode* )myData1[iK1];
    while (pNode)
    {
      if (Hasher::IsEqual (pNode->Key1(), theKey1))
      {
        return pNode->Index();
      }
      pNode = (IndexedMapNode *) pNode->Next();
    }

    const Standard_Integer aNewIndex = Increment();
    pNode = new (this->myAllocator) IndexedMapNode (theKey1, aNewIndex, myData1[iK1]);
    myData1[iK1]           = pNode;
    myData2[aNewIndex - 1] = pNode;
    return aNewIndex;
  }

  //! Contains
  Standard_Boolean Contains (const TheKeyType& theKey1) const
  {
    if (IsEmpty()) 
      return Standard_False;
    Standard_Integer iK1 = Hasher::HashCode (theKey1, NbBuckets());
    IndexedMapNode * pNode1;
    pNode1 = (IndexedMapNode *) myData1[iK1];
    while (pNode1) 
    {
      if (Hasher::IsEqual(pNode1->Key1(), theKey1)) 
        return Standard_True;
      pNode1 = (IndexedMapNode *) pNode1->Next();
    }
    return Standard_False;
  }

  //! Substitute
  void Substitute (const Standard_Integer theIndex,
                   const TheKeyType& theKey1)
  {
    Standard_OutOfRange_Raise_if (theIndex < 1 || theIndex > Extent(),
                                  "NCollection_IndexedMap::Substitute : "
                                  "Index is out of range");

    // check if theKey1 is not already in the map
    Standard_Integer iK1 = Hasher::HashCode (theKey1, NbBuckets());
    IndexedMapNode* p = (IndexedMapNode *) myData1[iK1];
    while (p)
    {
      if (Hasher::IsEqual (p->Key1(), theKey1))
      {
        if (p->Index() != theIndex)
        {
          throw Standard_DomainError ("NCollection_IndexedMap::Substitute : "
                                      "Attempt to substitute existing key");
        }
        p->Key1() = theKey1;
        return;
      }
      p = (IndexedMapNode *) p->Next();
    }

    // Find the node for the index I
    p = (IndexedMapNode* )myData2[theIndex - 1];
    
    // remove the old key
    Standard_Integer iK = Hasher::HashCode (p->Key1(), NbBuckets());
    IndexedMapNode * q = (IndexedMapNode *) myData1[iK];
    if (q == p)
      myData1[iK] = (IndexedMapNode *) p->Next();
    else 
    {
      while (q->Next() != p) 
        q = (IndexedMapNode *) q->Next();
      q->Next() = p->Next();
    }

    // update the node
    p->Key1() = theKey1;
    p->Next() = myData1[iK1];
    myData1[iK1] = p;
  }

  //! Swaps two elements with the given indices.
  void Swap (const Standard_Integer theIndex1,
             const Standard_Integer theIndex2)
  {
    Standard_OutOfRange_Raise_if (theIndex1 < 1 || theIndex1 > Extent()
                               || theIndex2 < 1 || theIndex2 > Extent(), "NCollection_IndexedMap::Swap");

    if (theIndex1 == theIndex2)
    {
      return;
    }

    IndexedMapNode* aP1 = (IndexedMapNode* )myData2[theIndex1 - 1];
    IndexedMapNode* aP2 = (IndexedMapNode* )myData2[theIndex2 - 1];
    std::swap (aP1->Index(), aP2->Index());
    myData2[theIndex2 - 1] = aP1;
    myData2[theIndex1 - 1] = aP2;
  }

  //! RemoveLast
  void RemoveLast (void)
  {
    const Standard_Integer aLastIndex = Extent();
    Standard_OutOfRange_Raise_if (aLastIndex == 0, "NCollection_IndexedMap::RemoveLast");

    // Find the node for the last index and remove it
    IndexedMapNode* p = (IndexedMapNode* )myData2[aLastIndex - 1];
    myData2[aLastIndex - 1] = NULL;

    // remove the key
    Standard_Integer iK1 = Hasher::HashCode (p->Key1(), NbBuckets());
    IndexedMapNode* q = (IndexedMapNode *) myData1[iK1];
    if (q == p)
      myData1[iK1] = (IndexedMapNode *) p->Next();
    else 
    {
      while (q->Next() != p) 
        q = (IndexedMapNode *) q->Next();
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
    Standard_OutOfRange_Raise_if(theIndex < 1 || theIndex > Extent(), "NCollection_IndexedMap::RemoveFromIndex");
    const Standard_Integer aLastInd = Extent();
    if (theIndex != aLastInd)
    {
      Swap(theIndex, aLastInd);
    }
    RemoveLast();
  }

  //! Remove the given key.
  //! Caution! The index of the last key can be changed.
  Standard_Boolean RemoveKey (const TheKeyType& theKey1)
  {
    Standard_Integer anIndToRemove = FindIndex(theKey1);
    if (anIndToRemove < 1)
    {
      return Standard_False;
    }

    RemoveFromIndex (anIndToRemove);
    return Standard_True;
  }

  //! FindKey
  const TheKeyType& FindKey (const Standard_Integer theIndex) const
  {
    Standard_OutOfRange_Raise_if (theIndex < 1 || theIndex > Extent(), "NCollection_IndexedMap::FindKey");
    IndexedMapNode* pNode2 = (IndexedMapNode* )myData2[theIndex - 1];
    return pNode2->Key1();
  }

  //! operator ()
  const TheKeyType& operator() (const Standard_Integer theIndex) const
  { return FindKey (theIndex); }

  //! FindIndex
  Standard_Integer FindIndex(const TheKeyType& theKey1) const
  {
    if (IsEmpty()) return 0;
    IndexedMapNode* pNode1 = (IndexedMapNode* )myData1[Hasher::HashCode(theKey1,NbBuckets())];
    while (pNode1)
    {
      if (Hasher::IsEqual (pNode1->Key1(), theKey1))
      {
        return pNode1->Index();
      }
      pNode1 = (IndexedMapNode*) pNode1->Next();
    }
    return 0;
  }

  //! Clear data. If doReleaseMemory is false then the table of
  //! buckets is not released and will be reused.
  void Clear(const Standard_Boolean doReleaseMemory = Standard_True)
  { Destroy (IndexedMapNode::delNode, doReleaseMemory); }

  //! Clear data and reset allocator
  void Clear (const Handle(NCollection_BaseAllocator)& theAllocator)
  { 
    Clear();
    this->myAllocator = ( ! theAllocator.IsNull() ? theAllocator :
                    NCollection_BaseAllocator::CommonBaseAllocator() );
  }

  //! Destructor
  virtual ~NCollection_IndexedMap (void)
  { Clear(); }

  //! Size
  Standard_Integer Size(void) const
  { return Extent(); }
};

#endif
