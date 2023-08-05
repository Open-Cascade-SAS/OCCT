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

#include <NCollection_BaseMap.hxx>
#include <NCollection_TListNode.hxx>
#include <Standard_MultiplyDefined.hxx>
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
public:
  //! STL-compliant typedef for key1 type
  typedef TheKey1Type key1_type;
  //! STL-compliant typedef for key2 type
  typedef TheKey2Type key2_type;

public:
  // **************** Adaptation of the TListNode to the DOUBLEmap
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
  };

 public:
  // ---------- PUBLIC METHODS ------------

  //! Empty constructor.
  NCollection_DoubleMap() : NCollection_BaseMap (1, Standard_False, Handle(NCollection_BaseAllocator)()) {}

  //! Constructor
  explicit NCollection_DoubleMap (const Standard_Integer theNbBuckets,
                                  const Handle(NCollection_BaseAllocator)& theAllocator = 0L)
  : NCollection_BaseMap (theNbBuckets, Standard_False, theAllocator) {}

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
    Standard_Integer anExt = theOther.Extent();
    if (anExt)
    {
      ReSize (anExt-1);
      Iterator anIter(theOther);
      for (; anIter.More(); anIter.Next())
      {
        TheKey1Type aKey1 = anIter.Key1();
        TheKey2Type aKey2 = anIter.Key2();
        const size_t iK1 = HashCode1 (aKey1, NbBuckets());
        const size_t iK2 = HashCode2 (aKey2, NbBuckets());
        DoubleMapNode * pNode = new (this->myAllocator) DoubleMapNode (aKey1, aKey2, 
          myData1[iK1], 
          myData2[iK2]);
        myData1[iK1] = pNode;
        myData2[iK2] = pNode;
        Increment();
      }
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
        for (int i = 0; i <= NbBuckets(); i++) 
        {
          if (myData1[i]) 
          {
            p = (DoubleMapNode *) myData1[i];
            while (p) 
            {
              const size_t iK1 = HashCode1 (p->Key1(), newBuck);
              const size_t iK2 = HashCode2 (p->Key2(), newBuck);
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
    const size_t iK1 = HashCode1 (theKey1, NbBuckets());
    const size_t iK2 = HashCode2 (theKey2, NbBuckets());
    DoubleMapNode * pNode;
    pNode = (DoubleMapNode *) myData1[iK1];
    while (pNode) 
    {
      if (IsEqual1 (pNode->Key1(), theKey1))
        throw Standard_MultiplyDefined("NCollection_DoubleMap:Bind");
      pNode = (DoubleMapNode *) pNode->Next();
    }
    pNode = (DoubleMapNode *) myData2[iK2];
    while (pNode) 
    {
      if (IsEqual2 (pNode->Key2(), theKey2))
        throw Standard_MultiplyDefined("NCollection_DoubleMap:Bind");
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
    const size_t iK1 = HashCode1 (theKey1, NbBuckets());
    const size_t iK2 = HashCode2 (theKey2, NbBuckets());
    DoubleMapNode * pNode1, * pNode2;
    pNode1 = (DoubleMapNode *) myData1[iK1];
    while (pNode1) 
    {
      if (IsEqual1(pNode1->Key1(), theKey1)) 
        break;
      pNode1 = (DoubleMapNode *) pNode1->Next();
    }
    if (pNode1 == NULL)
      return Standard_False;
    pNode2 = (DoubleMapNode *) myData2[iK2];
    while (pNode2) 
    {
      if (IsEqual2(pNode2->Key2(), theKey2)) 
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
    const size_t iK1 = HashCode1 (theKey1, NbBuckets());
    DoubleMapNode * pNode1;
    pNode1 = (DoubleMapNode *) myData1[iK1];
    while (pNode1) 
    {
      if (IsEqual1(pNode1->Key1(), theKey1)) 
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
    const size_t iK2 = HashCode2 (theKey2, NbBuckets());
    DoubleMapNode * pNode2;
    pNode2 = (DoubleMapNode *) myData2[iK2];
    while (pNode2) 
    {
      if (IsEqual2(pNode2->Key2(), theKey2)) 
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
    const size_t iK1 = HashCode1 (theKey1, NbBuckets());
    DoubleMapNode * p1, * p2, * q1, *q2;
    q1 = q2 = NULL;
    p1 = (DoubleMapNode *) myData1[iK1];
    while (p1) 
    {
      if (IsEqual1 (p1->Key1(), theKey1)) 
      {
        // remove from the data1
        if (q1) 
          q1->Next() = p1->Next();
        else
          myData1[iK1] = (DoubleMapNode*) p1->Next();
        const size_t iK2 = HashCode2 (p1->Key2(), NbBuckets());
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
    const size_t iK2 = HashCode2 (theKey2, NbBuckets());
    DoubleMapNode * p1, * p2, * q1, *q2;
    q1 = q2 = NULL;
    p2 = (DoubleMapNode *) myData2[iK2];
    while (p2) 
    {
      if (IsEqual2 (p2->Key2(), theKey2)) 
      {
        // remove from the data2
        if (q2)
        {
          q2->Next2() = p2->Next2();
        }
        else
          myData2[iK2] = (DoubleMapNode*) p2->Next2();
        const size_t iK1 = HashCode1 (p2->Key1(), NbBuckets());
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

  //! Find the Key1 and return Key2 value.
  //! Raises an exception if Key1 was not bound.
  const TheKey2Type& Find1(const TheKey1Type& theKey1) const
  {
    if (const TheKey2Type* aKey2 = Seek1 (theKey1))
    {
      return *aKey2;
    }
    throw Standard_NoSuchObject("NCollection_DoubleMap::Find1");
  }

  //! Find the Key1 and return Key2 value (by copying its value).
  //! @param [in]  theKey1 Key1 to find
  //! @param [out] theKey2 Key2 to return
  //! @return TRUE if Key1 has been found
  Standard_Boolean Find1 (const TheKey1Type& theKey1,
                          TheKey2Type& theKey2) const
  {
    if (const TheKey2Type* aKey2 = Seek1 (theKey1))
    {
      theKey2 = *aKey2;
      return true;
    }
    return false;
  }

  //! Find the Key1 and return pointer to Key2 or NULL if Key1 is not bound.
  //! @param [in]  theKey1 Key1 to find
  //! @return pointer to Key2 or NULL if Key1 is not found
  const TheKey2Type* Seek1 (const TheKey1Type& theKey1) const
  {
    for (DoubleMapNode* aNode1 = !IsEmpty() ? (DoubleMapNode* )myData1[HashCode1 (theKey1, NbBuckets())] : NULL;
         aNode1 != NULL; aNode1 = (DoubleMapNode* )aNode1->Next())
    {
      if (IsEqual1 (aNode1->Key1(), theKey1))
      {
        return &aNode1->Key2();
      }
    }
    return NULL;
  }

  //! Find the Key2 and return Key1 value.
  //! Raises an exception if Key2 was not bound.
  const TheKey1Type& Find2(const TheKey2Type& theKey2) const
  {
    if (const TheKey1Type* aVal1 = Seek2 (theKey2))
    {
      return *aVal1;
    }
    throw Standard_NoSuchObject("NCollection_DoubleMap::Find2");
  }

  //! Find the Key2 and return Key1 value (by copying its value).
  //! @param [in]  theKey2 Key2 to find
  //! @param [out] theKey1 Key1 to return
  //! @return TRUE if Key2 has been found
  Standard_Boolean Find2 (const TheKey2Type& theKey2,
                          TheKey1Type& theKey1) const
  {
    if (const TheKey1Type* aVal1 = Seek2 (theKey2))
    {
      theKey1 = *aVal1;
      return Standard_True;
    }
    return Standard_False;
  }

  //! Find the Key2 and return pointer to Key1 or NULL if not bound.
  //! @param [in] theKey2 Key2 to find
  //! @return pointer to Key1 if Key2 has been found
  const TheKey1Type* Seek2 (const TheKey2Type& theKey2) const
  {
    for (DoubleMapNode* aNode2 = !IsEmpty() ? (DoubleMapNode* )myData2[HashCode2 (theKey2, NbBuckets())] : NULL;
         aNode2 != NULL; aNode2 = (DoubleMapNode* )aNode2->Next2())
    {
      if (IsEqual2 (aNode2->Key2(), theKey2))
      {
        return &aNode2->Key1();
      }
    }
    return NULL;
  }

  //! Clear data. If doReleaseMemory is false then the table of
  //! buckets is not released and will be reused.
  void Clear(const Standard_Boolean doReleaseMemory = Standard_False)
  { Destroy (DoubleMapNode::delNode, doReleaseMemory); }

  //! Clear data and reset allocator
  void Clear (const Handle(NCollection_BaseAllocator)& theAllocator)
  { 
    Clear(true);
    this->myAllocator = ( ! theAllocator.IsNull() ? theAllocator :
                    NCollection_BaseAllocator::CommonBaseAllocator() );
  }

  //! Destructor
  ~NCollection_DoubleMap (void)
  { Clear(true); }

  //! Size
  Standard_Integer Size(void) const
  { return Extent(); }
protected:

  bool IsEqual1(const TheKey1Type& theKey1,
                const TheKey1Type& theKey2) const
  {
    return myHasher1(theKey1, theKey2);
  }

  size_t HashCode1(const TheKey1Type& theKey,
                   const int theUpperBound) const
  {
    return myHasher1(theKey) % theUpperBound + 1;
  }

  bool IsEqual2(const TheKey2Type& theKey1,
                const TheKey2Type& theKey2) const
  {
    return myHasher2(theKey1, theKey2);
  }

  size_t HashCode2(const TheKey2Type& theKey,
                   const int theUpperBound) const
  {
    return myHasher2(theKey) % theUpperBound + 1;
  }

protected:

  Hasher1 myHasher1;
  Hasher2 myHasher2;
};

#endif
