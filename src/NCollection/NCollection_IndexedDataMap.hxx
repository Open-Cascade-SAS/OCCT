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


#ifndef NCollection_IndexedDataMap_HeaderFile
#define NCollection_IndexedDataMap_HeaderFile

#include <NCollection_BaseCollection.hxx>
#include <NCollection_BaseMap.hxx>
#include <NCollection_TListNode.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Standard_NoSuchObject.hxx>

#include <NCollection_DefaultHasher.hxx>

#if !defined No_Exception && !defined No_Standard_OutOfRange
#include <Standard_OutOfRange.hxx>
#endif

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

template < class TheKeyType, 
           class TheItemType, 
           class Hasher = NCollection_DefaultHasher<TheKeyType> > 
  class NCollection_IndexedDataMap 
  : public NCollection_BaseCollection<TheItemType>,
    public NCollection_BaseMap
{
  //!    Adaptation of the TListNode to the INDEXEDDatamap
 private:
  class IndexedDataMapNode : public NCollection_TListNode<TheItemType>
  {
  public:
    //! Constructor with 'Next'
    IndexedDataMapNode (const TheKeyType&      theKey1, 
                        const Standard_Integer theKey2,
                        const TheItemType&     theItem,
                        NCollection_ListNode*  theNext1, 
                        NCollection_ListNode*  theNext2) :
                          NCollection_TListNode<TheItemType>(theItem,theNext1)
    { 
      myKey1 = theKey1;
      myKey2 = theKey2;
      myNext2 = (IndexedDataMapNode *) theNext2;
    }
    //! Key1
    TheKeyType& Key1 (void)
    { return myKey1; }
    //! Key2
    const Standard_Integer& Key2 (void)
    { return myKey2; }
    //! Next2
    IndexedDataMapNode*& Next2 (void)
    { return myNext2; }
    
    //! Static deleter to be passed to BaseList
    static void delNode (NCollection_ListNode * theNode, 
                         Handle(NCollection_BaseAllocator)& theAl)
    {
      ((IndexedDataMapNode *) theNode)->~IndexedDataMapNode();
      theAl->Free(theNode);
    }
  private:
    TheKeyType           myKey1;
    Standard_Integer     myKey2;
    IndexedDataMapNode * myNext2;
  };

 public:
  //!   Implementation of the Iterator interface.
  class Iterator : public NCollection_BaseCollection<TheItemType>::Iterator
  {
  public:
    //! Empty constructor
    Iterator (void) :
      myMap(NULL),
      myIndex(0) {}
    //! Constructor
    Iterator (const NCollection_IndexedDataMap& theMap) :
      myMap((NCollection_IndexedDataMap *) &theMap),
      myIndex(1) {}
    //! Query if the end of collection is reached by iterator
    virtual Standard_Boolean More(void) const
    { return (myIndex <= myMap->Extent()); }
    //! Make a step along the collection
    virtual void Next(void)
    { myIndex++; }
    //! Value access
    virtual const TheItemType& Value(void) const
    {  
#if !defined No_Exception && !defined No_Standard_NoSuchObject
      if (!More())
        Standard_NoSuchObject::Raise("NCollection_IndexedDataMap::Iterator::Value");
#endif
      return myMap->FindFromIndex(myIndex);
    }
    //! ChangeValue access
    virtual TheItemType& ChangeValue(void) const
    {  
#if !defined No_Exception && !defined No_Standard_NoSuchObject
      if (!More())
        Standard_NoSuchObject::Raise("NCollection_IndexedDataMap::Iterator::ChangeValue");
#endif
      return myMap->ChangeFromIndex(myIndex);
    }
    
  private:
    NCollection_IndexedDataMap * myMap;   //!< Pointer to the map being iterated
    Standard_Integer             myIndex; //!< Current index
  };
  
 public:
  // ---------- PUBLIC METHODS ------------

  //! Constructor
  NCollection_IndexedDataMap (const Standard_Integer NbBuckets=1,
                     const Handle(NCollection_BaseAllocator)& theAllocator = 0L)
    :  NCollection_BaseCollection<TheItemType>(theAllocator),
       NCollection_BaseMap (NbBuckets, Standard_False) {}

  //! Copy constructor
  NCollection_IndexedDataMap (const NCollection_IndexedDataMap& theOther) 
    : NCollection_BaseCollection<TheItemType>(theOther.myAllocator),
      NCollection_BaseMap (theOther.NbBuckets(), Standard_False) 
  { *this = theOther; }

  //! Assign another collection
  virtual void Assign(const NCollection_BaseCollection<TheItemType>& theOther)
  { 
    if (this == &theOther)
      return;
    Standard_TypeMismatch::Raise("NCollection_IndexedDataMap::Assign");
  }

  //! = another map
  NCollection_IndexedDataMap& operator= 
    (const NCollection_IndexedDataMap& theOther)
  { 
    if (this == &theOther)
      return *this;

    Clear(theOther.myAllocator);
    ReSize (theOther.Extent()-1);
    Standard_Integer i;
    for (i=1; i<=theOther.Extent(); i++)
    {
      TheKeyType aKey1 = theOther.FindKey(i);
      TheItemType anItem = theOther.FindFromIndex(i);
      Standard_Integer iK1 = Hasher::HashCode (aKey1, NbBuckets());
      Standard_Integer iK2 = ::HashCode (i, NbBuckets());
      IndexedDataMapNode * pNode = 
        new (this->myAllocator) IndexedDataMapNode (aKey1, i, anItem,
                                              myData1[iK1], myData2[iK2]);
      myData1[iK1] = pNode;
      myData2[iK2] = pNode;
      Increment();
    }
    return *this;
  }

  //! ReSize
  void ReSize (const Standard_Integer N)
  {
    IndexedDataMapNode** ppNewData1 = NULL;
    IndexedDataMapNode** ppNewData2 = NULL;
    Standard_Integer newBuck;
    if (BeginResize (N, newBuck, 
                     (NCollection_ListNode**&)ppNewData1, 
                     (NCollection_ListNode**&)ppNewData2,
                     this->myAllocator)) 
    {
      if (myData1) 
      {
        IndexedDataMapNode *p, *q;
        Standard_Integer i, iK1, iK2;
        for (i = 0; i <= NbBuckets(); i++) 
        {
          if (myData1[i]) 
          {
            p = (IndexedDataMapNode *) myData1[i];
            while (p) 
            {
              iK1 = Hasher::HashCode (p->Key1(), newBuck);
              iK2 = ::HashCode (p->Key2(), newBuck);
              q = (IndexedDataMapNode*) p->Next();
              p->Next()  = ppNewData1[iK1];
              p->Next2() = ppNewData2[iK2];
              ppNewData1[iK1] = p;
              ppNewData2[iK2] = p;
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
  Standard_Integer Add (const TheKeyType& theKey1, const TheItemType& theItem)
  {
    if (Resizable()) 
      ReSize(Extent());
    Standard_Integer iK1 = Hasher::HashCode (theKey1, NbBuckets());
    IndexedDataMapNode * pNode;
    pNode = (IndexedDataMapNode *) myData1[iK1];
    while (pNode)
    {
      if (Hasher::IsEqual (pNode->Key1(), theKey1))
        return pNode->Key2();
      pNode = (IndexedDataMapNode *) pNode->Next();
    }
    Increment();
    Standard_Integer iK2 = ::HashCode(Extent(),NbBuckets());
    pNode = new (this->myAllocator) IndexedDataMapNode (theKey1, Extent(), theItem,
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
    IndexedDataMapNode * pNode1;
    pNode1 = (IndexedDataMapNode *) myData1[iK1];
    while (pNode1) 
    {
      if (Hasher::IsEqual(pNode1->Key1(), theKey1)) 
        return Standard_True;
      pNode1 = (IndexedDataMapNode *) pNode1->Next();
    }
    return Standard_False;
  }

  //! Substitute
  void Substitute (const Standard_Integer theIndex,
                   const TheKeyType&      theKey1,
                   const TheItemType&     theItem)
  {
#if !defined No_Exception && !defined No_Standard_OutOfRange
    if (theIndex < 1 || theIndex > Extent())
      Standard_OutOfRange::Raise ("NCollection_IndexedDataMap::Substitute");
#endif
    IndexedDataMapNode * p;
    // check if theKey1 is not already in the map
    Standard_Integer iK1 = Hasher::HashCode (theKey1, NbBuckets());
    p = (IndexedDataMapNode *) myData1[iK1];
    while (p) 
    {
      if (Hasher::IsEqual (p->Key1(), theKey1)) 
        Standard_DomainError::Raise("NCollection_IndexedDataMap::Substitute");
      p = (IndexedDataMapNode *) p->Next();
    }

    // Find the node for the index I
    Standard_Integer iK2 = ::HashCode (theIndex, NbBuckets());
    p = (IndexedDataMapNode *) myData2[iK2];
    while (p) 
    {
      if (p->Key2() == theIndex) 
        break;
      p = (IndexedDataMapNode*) p->Next2();
    }
    
    // remove the old key
    Standard_Integer iK = Hasher::HashCode (p->Key1(), NbBuckets());
    IndexedDataMapNode * q = (IndexedDataMapNode *) myData1[iK];
    if (q == p)
      myData1[iK] = (IndexedDataMapNode *) p->Next();
    else 
    {
      while (q->Next() != p) 
        q = (IndexedDataMapNode *) q->Next();
      q->Next() = p->Next();
    }

    // update the node
    p->Key1()  = theKey1;
    p->ChangeValue() = theItem;
    p->Next()  = myData1[iK1];
    myData1[iK1] = p;
  }

  //! RemoveLast
  void RemoveLast (void)
  {
#if !defined No_Exception && !defined No_Standard_OutOfRange
    if (Extent() == 0)
      Standard_OutOfRange::Raise ("NCollection_IndexedDataMap::RemoveLast");
#endif
    IndexedDataMapNode * p, * q;
    // Find the node for the last index and remove it
    Standard_Integer iK2 = ::HashCode (Extent(), NbBuckets());
    p = (IndexedDataMapNode *) myData2[iK2];
    q = NULL;
    while (p) 
    {
      if (p->Key2() == Extent()) 
        break;
      q = p;
      p = (IndexedDataMapNode*) p->Next2();
    }
    if (q == NULL) 
      myData2[iK2] = (IndexedDataMapNode *) p->Next2();
    else 
      q->Next2() = p->Next2();
    
    // remove the key
    Standard_Integer iK1 = Hasher::HashCode (p->Key1(), NbBuckets());
    q = (IndexedDataMapNode *) myData1[iK1];
    if (q == p)
      myData1[iK1] = (IndexedDataMapNode *) p->Next();
    else 
    {
      while (q->Next() != p) 
        q = (IndexedDataMapNode *) q->Next();
      q->Next() = p->Next();
    }
    p->~IndexedDataMapNode();
    this->myAllocator->Free(p);
    Decrement();
  }

  //! FindKey
  const TheKeyType& FindKey (const Standard_Integer theKey2) const
  {
#if !defined No_Exception && !defined No_Standard_OutOfRange
    if (theKey2 < 1 || theKey2 > Extent())
      Standard_OutOfRange::Raise ("NCollection_IndexedDataMap::FindKey");
#endif
    IndexedDataMapNode * pNode2 =
      (IndexedDataMapNode *) myData2[::HashCode(theKey2,NbBuckets())];
    while (pNode2)
    {
      if (pNode2->Key2() == theKey2)
        return pNode2->Key1();
      pNode2 = (IndexedDataMapNode*) pNode2->Next2();
    }
    Standard_NoSuchObject::Raise("NCollection_IndexedDataMap::FindKey");
    return pNode2->Key1(); // This for compiler
  }

  //! FindFromIndex
  const TheItemType& FindFromIndex (const Standard_Integer theKey2) const
  {
#if !defined No_Exception && !defined No_Standard_OutOfRange
    if (theKey2 < 1 || theKey2 > Extent())
      Standard_OutOfRange::Raise ("NCollection_IndexedDataMap::FindFromIndex");
#endif
    IndexedDataMapNode * pNode2 =
      (IndexedDataMapNode *) myData2[::HashCode(theKey2,NbBuckets())];
    while (pNode2)
    {
      if (pNode2->Key2() == theKey2)
        return pNode2->Value();
      pNode2 = (IndexedDataMapNode*) pNode2->Next2();
    }
    Standard_NoSuchObject::Raise("NCollection_IndexedDataMap::FindFromIndex");
    return pNode2->Value(); // This for compiler
  }

  //! operator ()
  const TheItemType& operator() (const Standard_Integer theKey2) const
  { return FindFromIndex (theKey2); }

  //! ChangeFromIndex
  TheItemType& ChangeFromIndex (const Standard_Integer theKey2)
  {
#if !defined No_Exception && !defined No_Standard_OutOfRange
    if (theKey2 < 1 || theKey2 > Extent())
      Standard_OutOfRange::Raise("NCollection_IndexedDataMap::ChangeFromIndex");
#endif
    IndexedDataMapNode * pNode2 =
      (IndexedDataMapNode *) myData2[::HashCode(theKey2,NbBuckets())];
    while (pNode2)
    {
      if (pNode2->Key2() == theKey2)
        return pNode2->ChangeValue();
      pNode2 = (IndexedDataMapNode*) pNode2->Next2();
    }
    Standard_NoSuchObject::Raise("NCollection_IndexedDataMap::FindFromIndex");
    return pNode2->ChangeValue(); // This for compiler
  }

  //! operator ()
  TheItemType& operator() (const Standard_Integer theKey2)
  { return ChangeFromIndex (theKey2); }

  //! FindIndex
  Standard_Integer FindIndex(const TheKeyType& theKey1) const
  {
    if (IsEmpty()) return 0;
    IndexedDataMapNode * pNode1 = 
      (IndexedDataMapNode *) myData1[Hasher::HashCode(theKey1,NbBuckets())];
    while (pNode1)
    {
      if (Hasher::IsEqual (pNode1->Key1(), theKey1)) 
        return pNode1->Key2();
      pNode1 = (IndexedDataMapNode*) pNode1->Next();
    }
    return 0;
  }

  //! FindFromKey
  const TheItemType& FindFromKey(const TheKeyType& theKey1) const
  {
#if !defined No_Exception && !defined No_Standard_NoSuchObject
    if (IsEmpty())
      Standard_NoSuchObject::Raise ("NCollection_IndexedDataMap::FindFromKey");
#endif
    IndexedDataMapNode * pNode1 = 
      (IndexedDataMapNode *) myData1[Hasher::HashCode(theKey1,NbBuckets())];
    while (pNode1)
    {
      if (Hasher::IsEqual (pNode1->Key1(), theKey1)) 
        return pNode1->Value();
      pNode1 = (IndexedDataMapNode*) pNode1->Next();
    }
    Standard_NoSuchObject::Raise("NCollection_IndexedDataMap::FindFromKey");
    return pNode1->Value();
  }

  //! ChangeFromKey
  TheItemType& ChangeFromKey (const TheKeyType& theKey1)
  {
#if !defined No_Exception && !defined No_Standard_NoSuchObject
    if (IsEmpty())
      Standard_NoSuchObject::Raise("NCollection_IndexedDataMap::ChangeFromKey");
#endif
    IndexedDataMapNode * pNode1 = 
      (IndexedDataMapNode *) myData1[Hasher::HashCode(theKey1,NbBuckets())];
    while (pNode1)
    {
      if (Hasher::IsEqual (pNode1->Key1(), theKey1)) 
        return pNode1->ChangeValue();
      pNode1 = (IndexedDataMapNode*) pNode1->Next();
    }
    Standard_NoSuchObject::Raise("NCollection_IndexedDataMap::ChangeFromKey");
    return pNode1->ChangeValue();
  }

  //! Clear data. If doReleaseMemory is false then the table of
  //! buckets is not released and will be reused.
  void Clear(const Standard_Boolean doReleaseMemory = Standard_True)
  { Destroy (IndexedDataMapNode::delNode, this->myAllocator, doReleaseMemory); }

  //! Clear data and reset allocator
  void Clear (const Handle(NCollection_BaseAllocator)& theAllocator)
  { 
    Clear();
    this->myAllocator = ( ! theAllocator.IsNull() ? theAllocator :
                    NCollection_BaseAllocator::CommonBaseAllocator() );
  }

  //! Destructor
  ~NCollection_IndexedDataMap (void)
  { Clear(); }

  //! Size
  virtual Standard_Integer Size(void) const
  { return Extent(); }

 private:
  // ----------- PRIVATE METHODS -----------

  //! Creates Iterator for use on BaseCollection
  virtual TYPENAME NCollection_BaseCollection<TheItemType>::Iterator& 
    CreateIterator(void) const
  { return *(new (this->IterAllocator()) Iterator(*this)); }

};

#endif
