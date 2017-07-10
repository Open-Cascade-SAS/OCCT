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

 private:
   // **************** Adaptation of the TListNode to the INDEXEDmap
   class IndexedMapNode : public NCollection_TListNode<TheKeyType>
  {
  public:
    //! Constructor with 'Next'
    IndexedMapNode (const TheKeyType&      theKey1, 
                    const Standard_Integer theKey2, 
                    NCollection_ListNode*  theNext1, 
                    NCollection_ListNode*  theNext2) :
      NCollection_TListNode<TheKeyType> (theKey1, theNext1),
      myKey2(theKey2),
      myNext2((IndexedMapNode*)theNext2)
    { 
    }
    //! Key1
    TheKeyType& Key1 (void)
    { return this->ChangeValue(); }
    //! Key2
    Standard_Integer& Key2 (void)
    { return myKey2; }
    //! Next2
    IndexedMapNode*& Next2 (void)
    { return myNext2; }
    
    //! Static deleter to be passed to BaseList
    static void delNode (NCollection_ListNode * theNode, 
                         Handle(NCollection_BaseAllocator)& theAl)
    {
      ((IndexedMapNode *) theNode)->~IndexedMapNode();
      theAl->Free(theNode);
    }

  private:
    Standard_Integer myKey2;
    IndexedMapNode * myNext2;
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

  //! Constructor
  NCollection_IndexedMap (const Standard_Integer NbBuckets=1,
                          const Handle(NCollection_BaseAllocator)& theAllocator=0L)
  : NCollection_BaseMap (NbBuckets, Standard_False, theAllocator) {}

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
    ReSize (theOther.Extent()-1);
    Standard_Integer i, iLength=theOther.Extent();
    for (i=1; i<=iLength; i++)
    {
      TheKeyType aKey1 = theOther(i);
      Standard_Integer iK1 = Hasher::HashCode (aKey1, NbBuckets());
      Standard_Integer iK2 = ::HashCode (i, NbBuckets());
      IndexedMapNode * pNode = new (this->myAllocator) IndexedMapNode (aKey1, i, 
                                                                       myData1[iK1], 
                                                                       myData2[iK2]);
      myData1[iK1] = pNode;
      myData2[iK2] = pNode;
      Increment();
    }
    return *this;
  }

  //! Assignment operator
  NCollection_IndexedMap& operator= (const NCollection_IndexedMap& theOther)
  {
    return Assign (theOther);
  }

  //! ReSize
  void ReSize (const Standard_Integer N)
  {
    NCollection_ListNode** ppNewData1 = NULL;
    NCollection_ListNode** ppNewData2 = NULL;
    Standard_Integer newBuck;
    if (BeginResize (N, newBuck, ppNewData1, ppNewData2))
    {
      if (myData1) 
      {
        IndexedMapNode *p, *q;
        Standard_Integer i, iK1, iK2;
        for (i = 0; i <= NbBuckets(); i++) 
        {
          if (myData1[i]) 
          {
            p = (IndexedMapNode *) myData1[i];
            while (p) 
            {
              iK1 =Hasher::HashCode(p->Key1(), newBuck);
              q = (IndexedMapNode*) p->Next();
              p->Next()  = ppNewData1[iK1];
              ppNewData1[iK1] = p;
              if (p->Key2() > 0) 
              {
                iK2 = ::HashCode (p->Key2(), newBuck);
                p->Next2() = (IndexedMapNode*)ppNewData2[iK2];
                ppNewData2[iK2] = p;
              }
              p = q;
            }
          }
        }
      }
      EndResize (N, newBuck, ppNewData1, ppNewData2);
    }
  }

  //! Add
  Standard_Integer Add (const TheKeyType& theKey1)
  {
    if (Resizable()) 
      ReSize(Extent());
    Standard_Integer iK1 = Hasher::HashCode (theKey1, NbBuckets());
    IndexedMapNode * pNode;
    pNode = (IndexedMapNode *) myData1[iK1];
    while (pNode)
    {
      if (Hasher::IsEqual (pNode->Key1(), theKey1))
        return pNode->Key2();
      pNode = (IndexedMapNode *) pNode->Next();
    }
    Increment();
    Standard_Integer iK2 = ::HashCode(Extent(),NbBuckets());
    pNode = new (this->myAllocator) IndexedMapNode (theKey1, Extent(), 
                                                    myData1[iK1], myData2[iK2]);
    myData1[iK1] = pNode;
    myData2[iK2] = pNode;
    return Extent();
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

    IndexedMapNode * p;
    // check if theKey1 is not already in the map
    Standard_Integer iK1 = Hasher::HashCode (theKey1, NbBuckets());
    p = (IndexedMapNode *) myData1[iK1];
    while (p)
    {
      if (Hasher::IsEqual (p->Key1(), theKey1))
      {
        if (p->Key2() != theIndex)
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
    Standard_Integer iK2 = ::HashCode (theIndex, NbBuckets());
    p = (IndexedMapNode *) myData2[iK2];
    while (p) 
    {
      if (p->Key2() == theIndex) 
        break;
      p = (IndexedMapNode*) p->Next2();
    }
    
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

    const Standard_Integer aK1 = ::HashCode (theIndex1, NbBuckets());
    const Standard_Integer aK2 = ::HashCode (theIndex2, NbBuckets());

    IndexedMapNode* aP1 = (IndexedMapNode*) myData2[aK1];
    IndexedMapNode* aP2 = (IndexedMapNode*) myData2[aK2];

    if (aP1->Key2() == theIndex1)
    {
      myData2[aK1] = (IndexedMapNode *) aP1->Next2();
    }
    else
    {
      IndexedMapNode* aQ = aP1;
      for (aP1 = aQ->Next2(); aP1->Key2() != theIndex1; aQ = aP1, aP1 = aQ->Next2()) { }

      aQ->Next2() = aP1->Next2();
    }

    if (aP2->Key2() == theIndex2)
    {
      myData2[aK2] = (IndexedMapNode *) aP2->Next2();
    }
    else
    {
      IndexedMapNode* aQ = aP2;
      for (aP2 = aQ->Next2(); aP2->Key2() != theIndex2; aQ = aP2, aP2 = aQ->Next2()) { }

      aQ->Next2() = aP2->Next2();
    }

    std::swap (aP1->Key2(),
               aP2->Key2());

    aP1->Next2() = (IndexedMapNode*) myData2[aK2];
    myData2[aK2] = aP1;

    aP2->Next2() = (IndexedMapNode*) myData2[aK1];
    myData2[aK1] = aP2;
  }

  //! RemoveLast
  void RemoveLast (void)
  {
    Standard_OutOfRange_Raise_if (Extent() == 0, "NCollection_IndexedMap::RemoveLast");

    IndexedMapNode * p, * q;
    // Find the node for the last index and remove it
    Standard_Integer iK2 = ::HashCode (Extent(), NbBuckets());
    p = (IndexedMapNode *) myData2[iK2];
    q = NULL;
    while (p) 
    {
      if (p->Key2() == Extent()) 
        break;
      q = p;
      p = (IndexedMapNode*) p->Next2();
    }
    if (q == NULL) 
      myData2[iK2] = (IndexedMapNode *) p->Next2();
    else 
      q->Next2() = p->Next2();
    
    // remove the key
    Standard_Integer iK1 = Hasher::HashCode (p->Key1(), NbBuckets());
    q = (IndexedMapNode *) myData1[iK1];
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
  void RemoveFromIndex(const Standard_Integer theKey2)
  {
    Standard_OutOfRange_Raise_if(theKey2 < 1 || theKey2 > Extent(), "NCollection_IndexedMap::Remove");
    Standard_Integer aLastInd = Extent();
    if (theKey2 != aLastInd)
      Swap(theKey2, aLastInd);
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
  const TheKeyType& FindKey (const Standard_Integer theKey2) const
  {
    Standard_OutOfRange_Raise_if (theKey2 < 1 || theKey2 > Extent(), "NCollection_IndexedMap::FindKey");

    IndexedMapNode * pNode2 =
      (IndexedMapNode *) myData2[::HashCode(theKey2,NbBuckets())];
    while (pNode2)
    {
      if (pNode2->Key2() == theKey2) 
        return pNode2->Key1();
      pNode2 = (IndexedMapNode*) pNode2->Next2();
    }
    throw Standard_NoSuchObject("NCollection_IndexedMap::FindKey");
  }

  //! operator ()
  const TheKeyType& operator() (const Standard_Integer theKey2) const
  { return FindKey (theKey2); }

  //! FindIndex
  Standard_Integer FindIndex(const TheKeyType& theKey1) const
  {
    if (IsEmpty()) return 0;
    IndexedMapNode * pNode1 = 
      (IndexedMapNode *) myData1[Hasher::HashCode(theKey1,NbBuckets())];
    while (pNode1)
    {
      if (Hasher::IsEqual (pNode1->Key1(), theKey1)) 
        return pNode1->Key2();
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
