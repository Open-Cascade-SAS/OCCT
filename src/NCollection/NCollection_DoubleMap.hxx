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

#ifndef NCollection_DoubleMap_HeaderFile
#define NCollection_DoubleMap_HeaderFile

#include <NCollection_TypeDef.hxx>
#include <NCollection_BaseMap.hxx>
#include <NCollection_TListNode.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Standard_MultiplyDefined.hxx>
#include <Standard_ImmutableObject.hxx>
#include <Standard_NoSuchObject.hxx>

#include <NCollection_DefaultHasher.hxx>

/**
* Purpose:     The DoubleMap  is used to  bind  pairs (Key1,Key2)
*              and retrieve them in linear time.
*              
*              See Map from NCollection for a discussion about the number
*              of buckets
*/              

template < class TheKey1Type, 
           class TheKey2Type, 
           class Hasher1 = NCollection_DefaultHasher<TheKey1Type>, 
           class Hasher2 = NCollection_DefaultHasher<TheKey2Type> >
class NCollection_DoubleMap : public NCollection_BaseMap
{
  // **************** Adaptation of the TListNode to the DOUBLEmap
 public:
  class DoubleMapNode : public NCollection_TListNode<TheKey2Type>
  {
  public:
    //! Constructor with 'Next'
    DoubleMapNode (const TheKey1Type&    theKey1, 
                   const TheKey2Type&    theKey2, 
                   NCollection_ListNode* theNext1, 
                   NCollection_ListNode* theNext2) :
      NCollection_TListNode<TheKey2Type> (theKey2, theNext1),
      myKey1(theKey1),
      myNext2((DoubleMapNode*)theNext2)
    { 
    }
    //! Key1
    const TheKey1Type& Key1 (void)
    { return myKey1; }
    //! Key2
    const TheKey2Type& Key2 (void)
    { return this->myValue; }
    //! Next2
    DoubleMapNode*& Next2 (void)
    { return myNext2; }
    
    //! Static deleter to be passed to BaseList
    static void delNode (NCollection_ListNode * theNode, 
                         Handle(NCollection_BaseAllocator)& theAl)
    {
      ((DoubleMapNode *) theNode)->~DoubleMapNode();
      theAl->Free(theNode);
    }

  private:
    TheKey1Type    myKey1;
    DoubleMapNode *myNext2;
  };

 public:
  // **************** Implementation of the Iterator interface.
  class Iterator : public NCollection_BaseMap::Iterator
  {
  public:
    //! Empty constructor
    Iterator (void) {}
    //! Constructor
    Iterator (const NCollection_DoubleMap& theMap) :
      NCollection_BaseMap::Iterator(theMap) {}
    //! Query if the end of collection is reached by iterator
    Standard_Boolean More(void) const
    { return PMore(); }
    //! Make a step along the collection
    void Next(void)
    { PNext(); }
    //! Key1 inquiry
    const TheKey1Type& Key1(void) const
    {
      Standard_NoSuchObject_Raise_if (!More(), "NCollection_DoubleMap::Iterator::Key1");
      return ((DoubleMapNode *) myNode)->Key1();
    }
    //! Key2 inquiry
    const TheKey2Type& Key2(void) const
    {  
      Standard_NoSuchObject_Raise_if (!More(), "NCollection_DoubleMap::Iterator::Key2");
      return ((DoubleMapNode *) myNode)->Key2();
    }
    //! Value access
    const TheKey2Type& Value(void) const
    {  
      Standard_NoSuchObject_Raise_if (!More(), "NCollection_DoubleMap::Iterator::Value");
      return ((DoubleMapNode *) myNode)->Value();
    }
    //! Value change access - denied
    TheKey2Type& ChangeValue(void) const
    {  
      Standard_ImmutableObject::Raise("NCollection_DoubleMap::Iterator::ChangeValue");
      return * (TheKey2Type *) NULL; // For compiler
    }
  };

 public:
  // ---------- PUBLIC METHODS ------------

  //! Constructor
  NCollection_DoubleMap (const Standard_Integer NbBuckets=1,
                     const Handle(NCollection_BaseAllocator)& theAllocator = 0L)
    : NCollection_BaseMap (NbBuckets, Standard_False, theAllocator) {}

  //! Copy constructor
  NCollection_DoubleMap (const NCollection_DoubleMap& theOther)
    : NCollection_BaseMap (theOther.NbBuckets(), Standard_False, theOther.myAllocator) 
  { *this = theOther; }

  //! Exchange the content of two maps without re-allocations.
  //! Notice that allocators will be swapped as well!
  void Exchange (NCollection_DoubleMap& theOther)
  {
    this->exchangeMapsData (theOther);
  }

  //! Assignment.
  //! This method does not change the internal allocator.
  NCollection_DoubleMap& Assign (const NCollection_DoubleMap& theOther)
  { 
    if (this == &theOther)
      return *this;

    Clear();
    ReSize (theOther.Extent()-1);
    Iterator anIter(theOther);
    for (; anIter.More(); anIter.Next())
    {
      TheKey1Type aKey1 = anIter.Key1();
      TheKey2Type aKey2 = anIter.Key2();
      Standard_Integer iK1 = Hasher1::HashCode (aKey1, NbBuckets());
      Standard_Integer iK2 = Hasher2::HashCode (aKey2, NbBuckets());
      DoubleMapNode * pNode = new (this->myAllocator) DoubleMapNode (aKey1, aKey2, 
                                                                     myData1[iK1], 
                                                                     myData2[iK2]);
      myData1[iK1] = pNode;
      myData2[iK2] = pNode;
      Increment();
    }
    return *this;
  }

  //! Assignment operator
  NCollection_DoubleMap& operator= (const NCollection_DoubleMap& theOther)
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
        DoubleMapNode *p, *q;
        Standard_Integer i, iK1, iK2;
        for (i = 0; i <= NbBuckets(); i++) 
        {
          if (myData1[i]) 
          {
            p = (DoubleMapNode *) myData1[i];
            while (p) 
            {
              iK1 = Hasher1::HashCode (p->Key1(), newBuck);
              iK2 = Hasher2::HashCode (p->Key2(), newBuck);
              q = (DoubleMapNode*) p->Next();
              p->Next()  = ppNewData1[iK1];
              p->Next2() = (DoubleMapNode*)ppNewData2[iK2];
              ppNewData1[iK1] = p;
              ppNewData2[iK2] = p;
              p = q;
            }
          }
        }
      }
      EndResize (N, newBuck, ppNewData1, ppNewData2);
    }
  }

  //! Bind
  void Bind (const TheKey1Type& theKey1, const TheKey2Type& theKey2)
  {
    if (Resizable()) 
      ReSize(Extent());
    Standard_Integer iK1 = Hasher1::HashCode (theKey1, NbBuckets());
    Standard_Integer iK2 = Hasher2::HashCode (theKey2, NbBuckets());
    DoubleMapNode * pNode;
    pNode = (DoubleMapNode *) myData1[iK1];
    while (pNode) 
    {
      if (Hasher1::IsEqual (pNode->Key1(), theKey1))
        Standard_MultiplyDefined::Raise("NCollection_DoubleMap:Bind");
      pNode = (DoubleMapNode *) pNode->Next();
    }
    pNode = (DoubleMapNode *) myData2[iK2];
    while (pNode) 
    {
      if (Hasher2::IsEqual (pNode->Key2(), theKey2))
        Standard_MultiplyDefined::Raise("NCollection_DoubleMap:Bind");
      pNode = (DoubleMapNode *) pNode->Next();
    }
    pNode = new (this->myAllocator) DoubleMapNode (theKey1, theKey2, 
                                                   myData1[iK1], myData2[iK2]);
    myData1[iK1] = pNode;
    myData2[iK2] = pNode;
    Increment();
  }

  //!* AreBound
  Standard_Boolean AreBound (const TheKey1Type& theKey1,
                             const TheKey2Type& theKey2) const
  {
    if (IsEmpty()) 
      return Standard_False;
    Standard_Integer iK1 = Hasher1::HashCode (theKey1, NbBuckets());
    Standard_Integer iK2 = Hasher2::HashCode (theKey2, NbBuckets());
    DoubleMapNode * pNode1, * pNode2;
    pNode1 = (DoubleMapNode *) myData1[iK1];
    while (pNode1) 
    {
      if (Hasher1::IsEqual(pNode1->Key1(), theKey1)) 
        break;
      pNode1 = (DoubleMapNode *) pNode1->Next();
    }
    if (pNode1 == NULL)
      return Standard_False;
    pNode2 = (DoubleMapNode *) myData2[iK2];
    while (pNode2) 
    {
      if (Hasher2::IsEqual(pNode2->Key2(), theKey2)) 
        break;
      pNode2 = (DoubleMapNode *) pNode2->Next();
    }
    if (pNode2 == NULL)
      return Standard_False;

    return (pNode1 == pNode2);
  }

  //! IsBound1
  Standard_Boolean IsBound1 (const TheKey1Type& theKey1) const
  {
    if (IsEmpty()) 
      return Standard_False;
    Standard_Integer iK1 = Hasher1::HashCode (theKey1, NbBuckets());
    DoubleMapNode * pNode1;
    pNode1 = (DoubleMapNode *) myData1[iK1];
    while (pNode1) 
    {
      if (Hasher1::IsEqual(pNode1->Key1(), theKey1)) 
        return Standard_True;
      pNode1 = (DoubleMapNode *) pNode1->Next();
    }
    return Standard_False;
  }

  //! IsBound2
  Standard_Boolean IsBound2 (const TheKey2Type& theKey2) const
  {
    if (IsEmpty()) 
      return Standard_False;
    Standard_Integer iK2 = Hasher2::HashCode (theKey2, NbBuckets());
    DoubleMapNode * pNode2;
    pNode2 = (DoubleMapNode *) myData2[iK2];
    while (pNode2) 
    {
      if (Hasher2::IsEqual(pNode2->Key2(), theKey2)) 
        return Standard_True;
      pNode2 = (DoubleMapNode *) pNode2->Next2();
    }
    return Standard_False;
  }

  //! UnBind1
  Standard_Boolean UnBind1 (const TheKey1Type& theKey1)
  {
    if (IsEmpty()) 
      return Standard_False;
    Standard_Integer iK1 = Hasher1::HashCode (theKey1, NbBuckets());
    DoubleMapNode * p1, * p2, * q1, *q2;
    q1 = q2 = NULL;
    p1 = (DoubleMapNode *) myData1[iK1];
    while (p1) 
    {
      if (Hasher1::IsEqual (p1->Key1(), theKey1)) 
      {
        // remove from the data1
        if (q1) 
          q1->Next() = p1->Next();
        else
          myData1[iK1] = (DoubleMapNode*) p1->Next();
        Standard_Integer iK2 = Hasher2::HashCode (p1->Key2(), NbBuckets());
        p2 = (DoubleMapNode *) myData2[iK2];
        while (p2)
        {
          if (p2 == p1) 
          {
            // remove from the data2
            if (q2) 
              q2->Next2() = p2->Next2();
            else
              myData2[iK2] = (DoubleMapNode*) p2->Next2();
            break;
          }
          q2 = p2;
          p2 = (DoubleMapNode*) p2->Next2();
        }
        p1->~DoubleMapNode();
        this->myAllocator->Free(p1);
        Decrement();
        return Standard_True;
      }
      q1 = p1;
      p1 = (DoubleMapNode*) p1->Next();
    }
    return Standard_False;
  }

  //! UnBind2
  Standard_Boolean UnBind2 (const TheKey2Type& theKey2)
  {
    if (IsEmpty()) 
      return Standard_False;
    Standard_Integer iK2 = Hasher2::HashCode (theKey2, NbBuckets());
    DoubleMapNode * p1, * p2, * q1, *q2;
    q1 = q2 = NULL;
    p2 = (DoubleMapNode *) myData2[iK2];
    while (p2) 
    {
      if (Hasher2::IsEqual (p2->Key2(), theKey2)) 
      {
        // remove from the data2
        if (q2)
          q2->Next() = p2->Next();
        else
          myData2[iK2] = (DoubleMapNode*) p2->Next2();
        Standard_Integer iK1 = Hasher1::HashCode (p2->Key1(), NbBuckets());
        p1 = (DoubleMapNode *) myData1[iK1];
        while (p1)
        {
          if (p1 == p2) 
          {
            // remove from the data1
            if (q1)
              q1->Next() = p1->Next();
            else
              myData1[iK1] = (DoubleMapNode*) p1->Next();
            break;
          }
          q1 = p1;
          p1 = (DoubleMapNode*) p1->Next();
        }
        p2->~DoubleMapNode();
        this->myAllocator->Free(p2);
        Decrement();
        return Standard_True;
      }
      q2 = p2;
      p2 = (DoubleMapNode*) p2->Next2();
    }
    return Standard_False;
  }

  //! Find1
  const TheKey2Type& Find1(const TheKey1Type& theKey1) const
  {
    Standard_NoSuchObject_Raise_if (IsEmpty(), "NCollection_DoubleMap::Find1");
    DoubleMapNode * pNode1 = 
      (DoubleMapNode *) myData1[Hasher1::HashCode(theKey1,NbBuckets())];
    while (pNode1)
    {
      if (Hasher1::IsEqual (pNode1->Key1(), theKey1)) 
        return pNode1->Key2();
      pNode1 = (DoubleMapNode*) pNode1->Next();
    }
    Standard_NoSuchObject::Raise("NCollection_DoubleMap::Find1");
    return pNode1->Key2(); // This for compiler
  }

  //! Find2
  const TheKey1Type& Find2(const TheKey2Type& theKey2) const
  {
    Standard_NoSuchObject_Raise_if (IsEmpty(), "NCollection_DoubleMap::Find2");
    DoubleMapNode * pNode2 = 
      (DoubleMapNode *) myData2[Hasher2::HashCode(theKey2,NbBuckets())];
    while (pNode2)
    {
      if (Hasher2::IsEqual (pNode2->Key2(), theKey2)) 
        return pNode2->Key1();
      pNode2 = (DoubleMapNode*) pNode2->Next2();
    }
    Standard_NoSuchObject::Raise("NCollection_DoubleMap::Find2");
    return pNode2->Key1(); // This for compiler
  }

  //! Clear data. If doReleaseMemory is false then the table of
  //! buckets is not released and will be reused.
  void Clear(const Standard_Boolean doReleaseMemory = Standard_True)
  { Destroy (DoubleMapNode::delNode, doReleaseMemory); }

  //! Clear data and reset allocator
  void Clear (const Handle(NCollection_BaseAllocator)& theAllocator)
  { 
    Clear();
    this->myAllocator = ( ! theAllocator.IsNull() ? theAllocator :
                    NCollection_BaseAllocator::CommonBaseAllocator() );
  }

  //! Destructor
  ~NCollection_DoubleMap (void)
  { Clear(); }

  //! Size
  Standard_Integer Size(void) const
  { return Extent(); }
};

#endif
