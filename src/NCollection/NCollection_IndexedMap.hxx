// Created on: 2002-04-24
// Created by: Alexander KARTOMIN (akm)
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#ifndef NCollection_IndexedMap_HeaderFile
#define NCollection_IndexedMap_HeaderFile

#include <NCollection_BaseCollection.hxx>
#include <NCollection_BaseMap.hxx>
#include <NCollection_TListNode.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_ImmutableObject.hxx>

#include <NCollection_DefaultHasher.hxx>

#if !defined No_Exception && !defined No_Standard_OutOfRange
#include <Standard_OutOfRange.hxx>
#endif

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
  class NCollection_IndexedMap 
  : public NCollection_BaseCollection<TheKeyType>,
    public NCollection_BaseMap
{
  // **************** Adaptation of the TListNode to the INDEXEDmap
 private:
  class IndexedMapNode : public NCollection_TListNode<TheKeyType>
  {
  public:
    //! Constructor with 'Next'
    IndexedMapNode (const TheKeyType&      theKey1, 
                    const Standard_Integer theKey2, 
                    NCollection_ListNode*  theNext1, 
                    NCollection_ListNode*  theNext2) :
                      NCollection_TListNode<TheKeyType> (theKey1, theNext1)
    { 
      myKey2 = theKey2;
      myNext2 = (IndexedMapNode *) theNext2;
    }
    //! Key1
    TheKeyType& Key1 (void)
    { return this->ChangeValue(); }
    //! Key2
    const Standard_Integer& Key2 (void)
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
    : public NCollection_BaseCollection<TheKeyType>::Iterator
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
    virtual Standard_Boolean More(void) const
    { return (myIndex <= myMap->Extent()); }
    //! Make a step along the collection
    virtual void Next(void)
    { myIndex++; }
    //! Value access
    virtual const TheKeyType& Value(void) const
    {
#if !defined No_Exception && !defined No_Standard_NoSuchObject
      if (!More())
        Standard_NoSuchObject::Raise("NCollection_IndexedMap::Iterator::Value");
#endif
      return myMap->FindKey(myIndex);
    }
    //! Value change access denied - use Substitute
    virtual TheKeyType& ChangeValue(void) const
    {  
      Standard_ImmutableObject::Raise ("impossible to ChangeValue");
      return * (TheKeyType *) NULL; // This for compiler
    }
    
  private:
    NCollection_IndexedMap * myMap;   // Pointer to the map being iterated
    Standard_Integer         myIndex; // Current index
  };
  
 public:
  // ---------- PUBLIC METHODS ------------

  //! Constructor
  NCollection_IndexedMap (const Standard_Integer NbBuckets=1,
                          const Handle(NCollection_BaseAllocator)& theAllocator=0L) :
    NCollection_BaseCollection<TheKeyType>(theAllocator),
    NCollection_BaseMap (NbBuckets, Standard_False) {}

  //! Copy constructor
  NCollection_IndexedMap (const NCollection_IndexedMap& theOther) :
    NCollection_BaseCollection<TheKeyType>(theOther.myAllocator),
    NCollection_BaseMap (theOther.NbBuckets(), Standard_False) 
  { *this = theOther; }

  //! Assign another collection
  virtual void Assign (const NCollection_BaseCollection<TheKeyType>& theOther)
  { 
    if (this == &theOther)
      return;
    Clear();
    ReSize (theOther.Size()-1);
    TYPENAME NCollection_BaseCollection<TheKeyType>::Iterator& anIter = 
      theOther.CreateIterator();
    for (; anIter.More(); anIter.Next())
      Add(anIter.Value());
  }

  //! = another map
  NCollection_IndexedMap& operator= (const NCollection_IndexedMap& theOther)
  { 
    if (this == &theOther)
      return *this;

    Clear(theOther.myAllocator);
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

  //! ReSize
  void ReSize (const Standard_Integer N)
  {
    IndexedMapNode** ppNewData1 = NULL;
    IndexedMapNode** ppNewData2 = NULL;
    Standard_Integer newBuck;
    if (BeginResize (N, newBuck, 
                     (NCollection_ListNode**&)ppNewData1, 
                     (NCollection_ListNode**&)ppNewData2,
                     this->myAllocator)) 
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
                p->Next2() = ppNewData2[iK2];
                ppNewData2[iK2] = p;
              }
              p = q;
            }
          }
        }
      }
      EndResize(N,newBuck,
                (NCollection_ListNode**&)ppNewData1,
                (NCollection_ListNode**&)ppNewData2,
                this->myAllocator);
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
#if !defined No_Exception && !defined No_Standard_OutOfRange
    if (theIndex < 1 || theIndex > Extent())
      Standard_OutOfRange::Raise ("NCollection_IndexedMap::Substitute");
#endif
    IndexedMapNode * p;
    // check if theKey1 is not already in the map
    Standard_Integer iK1 = Hasher::HashCode (theKey1, NbBuckets());
    p = (IndexedMapNode *) myData1[iK1];
    while (p) 
    {
      if (Hasher::IsEqual (p->Key1(), theKey1)) 
        Standard_DomainError::Raise("NCollection_IndexedMap::Substitute");
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

  //! RemoveLast
  void RemoveLast (void)
  {
#if !defined No_Exception && !defined No_Standard_OutOfRange
    if (Extent() == 0)
      Standard_OutOfRange::Raise ("NCollection_IndexedMap::RemoveLast");
#endif
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

  //! FindKey
  const TheKeyType& FindKey (const Standard_Integer theKey2) const
  {
#if !defined No_Exception && !defined No_Standard_OutOfRange
    if (theKey2 < 1 || theKey2 > Extent())
      Standard_OutOfRange::Raise ("NCollection_IndexedMap::FindKey");
#endif
    IndexedMapNode * pNode2 =
      (IndexedMapNode *) myData2[::HashCode(theKey2,NbBuckets())];
    while (pNode2)
    {
      if (pNode2->Key2() == theKey2) 
        return pNode2->Key1();
      pNode2 = (IndexedMapNode*) pNode2->Next2();
    }
    Standard_NoSuchObject::Raise("NCollection_IndexedMap::FindKey");
    return pNode2->Key1(); // This for compiler
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
  { Destroy (IndexedMapNode::delNode, this->myAllocator, doReleaseMemory); }

  //! Clear data and reset allocator
  void Clear (const Handle(NCollection_BaseAllocator)& theAllocator)
  { 
    Clear();
    this->myAllocator = ( ! theAllocator.IsNull() ? theAllocator :
                    NCollection_BaseAllocator::CommonBaseAllocator() );
  }

  //! Destructor
  ~NCollection_IndexedMap (void)
  { Clear(); }

  //! Size
  virtual Standard_Integer Size(void) const
  { return Extent(); }

 private:
  // ----------- PRIVATE METHODS -----------

  //! Creates Iterator for use on BaseCollection
  virtual TYPENAME NCollection_BaseCollection<TheKeyType>::Iterator& 
    CreateIterator(void) const
  { return *(new (this->IterAllocator()) Iterator(*this)); }

};

#endif
