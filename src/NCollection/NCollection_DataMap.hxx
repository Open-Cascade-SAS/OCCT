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

#ifndef NCollection_DataMap_HeaderFile
#define NCollection_DataMap_HeaderFile

#include <NCollection_BaseCollection.hxx>
#include <NCollection_BaseMap.hxx>
#include <NCollection_TListNode.hxx>

#include <NCollection_DefaultHasher.hxx>

#include <Standard_TypeMismatch.hxx>
#include <Standard_NoSuchObject.hxx>

/**
* Purpose:     The DataMap is a Map to store keys with associated
*              Items. See Map  from NCollection for  a discussion
*              about the number of buckets.
*
*              The DataMap can be seen as an extended array where
*              the Keys  are the   indices.  For this reason  the
*              operator () is defined on DataMap to fetch an Item
*              from a Key. So the following syntax can be used :
*
*              anItem = aMap(aKey);
*              aMap(aKey) = anItem;
*
*              This analogy has its  limit.   aMap(aKey) = anItem
*              can  be done only  if aKey was previously bound to
*              an item in the map.
*/              

template < class TheKeyType, 
           class TheItemType, 
           class Hasher = NCollection_DefaultHasher<TheKeyType> >  class NCollection_DataMap 
  
  : public NCollection_BaseCollection<TheItemType>,
    public NCollection_BaseMap
{
  // **************** Adaptation of the TListNode to the DATAmap
 public:
  class DataMapNode : public NCollection_TListNode<TheItemType>
  {
  public:
    //! Constructor with 'Next'
    DataMapNode (const TheKeyType&     theKey, 
                 const TheItemType&    theItem, 
                 NCollection_ListNode* theNext) :
                   NCollection_TListNode<TheItemType> (theItem, theNext) 
    { myKey = theKey; }
    //! Key
    const TheKeyType& Key (void) const
    { return myKey; }
    
    //! Static deleter to be passed to BaseList
    static void delNode (NCollection_ListNode * theNode, 
                         Handle(NCollection_BaseAllocator)& theAl)
    {
      ((DataMapNode *) theNode)->~DataMapNode();
      theAl->Free(theNode);
    }

  private:
    TheKeyType    myKey;
  };

 public:
  // **************** Implementation of the Iterator interface.
  class Iterator 
    : public NCollection_BaseCollection<TheItemType>::Iterator,
      public NCollection_BaseMap::Iterator
  {
  public:
    //! Empty constructor
    Iterator (void) :
      NCollection_BaseMap::Iterator() {}
    //! Constructor
    Iterator (const NCollection_DataMap& theMap) :
      NCollection_BaseMap::Iterator(theMap) {}
    //! Query if the end of collection is reached by iterator
    virtual Standard_Boolean More(void) const
    { return PMore(); }
    //! Make a step along the collection
    virtual void Next(void)
    { PNext(); }
    //! Value inquiry
    virtual const TheItemType& Value(void) const
    {  
#if !defined No_Exception && !defined No_Standard_NoSuchObject
      if (!More())
        Standard_NoSuchObject::Raise("NCollection_DataMap::Iterator::Value");  
#endif
      return ((DataMapNode *) myNode)->Value();
    }
    //! Value change access
    virtual TheItemType& ChangeValue(void) const
    {  
#if !defined No_Exception && !defined No_Standard_NoSuchObject
      if (!More())
        Standard_NoSuchObject::Raise("NCollection_DataMap::Iterator::ChangeValue");  
#endif
      return ((DataMapNode *) myNode)->ChangeValue();
    }
    //! Key
    const TheKeyType& Key (void) const
    { 
#if !defined No_Exception && !defined No_Standard_NoSuchObject
      if (!More())
        Standard_NoSuchObject::Raise("NCollection_DataMap::Iterator::Key");  
#endif
      return ((DataMapNode *) myNode)->Key();
    }
  };

 public:
  // ---------- PUBLIC METHODS ------------

  //! Constructor
  NCollection_DataMap (const Standard_Integer NbBuckets=1,
                     const Handle(NCollection_BaseAllocator)& theAllocator = 0L)
    : NCollection_BaseCollection<TheItemType>(theAllocator),
      NCollection_BaseMap (NbBuckets, Standard_True) {}

  //! Copy constructor
  NCollection_DataMap (const NCollection_DataMap& theOther)
    : NCollection_BaseCollection<TheItemType>(theOther.myAllocator),
      NCollection_BaseMap (theOther.NbBuckets(), Standard_True) 
  { *this = theOther; }

  //! Assign another collection
  virtual void Assign(const NCollection_BaseCollection<TheItemType>& theOther)
  { 
    if (this == &theOther)
      return;
    Standard_TypeMismatch::Raise ("NCollection_DataMap::Assign impossible");
  }

  //! Exchange the content of two maps without re-allocations.
  //! Notice that allocators will be swapped as well!
  void Exchange (NCollection_DataMap& theOther)
  {
    this->exchangeAllocators (theOther);
    this->exchangeMapsData   (theOther);
  }

  //! = another map
  NCollection_DataMap& operator= (const NCollection_DataMap& theOther)
  { 
    if (this == &theOther)
      return *this;

    Clear(theOther.myAllocator);
    ReSize (theOther.Extent()-1);
    Iterator anIter(theOther);
    for (; anIter.More(); anIter.Next())
      Bind (anIter.Key(), anIter.Value());
    return *this;
  }

  //! ReSize
  void ReSize (const Standard_Integer N)
  {
    NCollection_ListNode** newdata = NULL;
    NCollection_ListNode** dummy   = NULL;
    Standard_Integer newBuck;
    if (BeginResize (N, newBuck, newdata, dummy, this->myAllocator))
    {
      if (myData1) 
      {
        DataMapNode** olddata = (DataMapNode**) myData1;
        DataMapNode *p, *q;
        Standard_Integer i,k;
        for (i = 0; i <= NbBuckets(); i++) 
        {
          if (olddata[i]) 
          {
            p = olddata[i];
            while (p) 
            {
              k = Hasher::HashCode(p->Key(),newBuck);
              q = (DataMapNode*) p->Next();
              p->Next() = newdata[k];
              newdata[k] = p;
              p = q;
            }
          }
        }
      }
      EndResize (N, newBuck, newdata, dummy, this->myAllocator);
    }
  }

  //! Bind
  Standard_Boolean Bind (const TheKeyType& theKey, const TheItemType& theItem)
  {
    if (Resizable()) 
      ReSize(Extent());
    DataMapNode** data = (DataMapNode**)myData1;
    Standard_Integer k = Hasher::HashCode (theKey, NbBuckets());
    DataMapNode* p = data[k];
    while (p) 
    {
      if (Hasher::IsEqual(p->Key(), theKey))
      {
        p->ChangeValue() = theItem;
        return Standard_False;
      }
      p = (DataMapNode *) p->Next();
    }
    data[k] = new (this->myAllocator) DataMapNode (theKey, theItem, data[k]);
    Increment();
    return Standard_True;
  }

  //! IsBound
  Standard_Boolean IsBound(const TheKeyType& K) const
  {
    if (IsEmpty()) 
      return Standard_False;
    DataMapNode** data = (DataMapNode**) myData1;
    DataMapNode* p = data[Hasher::HashCode(K,NbBuckets())];
    while (p) 
    {
      if (Hasher::IsEqual(p->Key(),K)) 
        return Standard_True;
      p = (DataMapNode *) p->Next();
    }
    return Standard_False;
  }

  //! UnBind
  Standard_Boolean UnBind(const TheKeyType& K)
  {
    if (IsEmpty()) 
      return Standard_False;
    DataMapNode** data = (DataMapNode**) myData1;
    Standard_Integer k = Hasher::HashCode(K,NbBuckets());
    DataMapNode* p = data[k];
    DataMapNode* q = NULL;
    while (p) 
    {
      if (Hasher::IsEqual(p->Key(),K)) 
      {
        Decrement();
        if (q) 
          q->Next() = p->Next();
        else
          data[k] = (DataMapNode*) p->Next();
        p->~DataMapNode();
        this->myAllocator->Free(p);
        return Standard_True;
      }
      q = p;
      p = (DataMapNode*) p->Next();
    }
    return Standard_False;
  }

  //! Find
  const TheItemType& Find(const TheKeyType& theKey) const
  {
#if !defined No_Exception && !defined No_Standard_NoSuchObject
    if (IsEmpty())
      Standard_NoSuchObject::Raise ("NCollection_DataMap::Find");
#endif
    DataMapNode* p = (DataMapNode*) myData1[Hasher::HashCode(theKey,NbBuckets())];
    while (p) 
    {
      if (Hasher::IsEqual(p->Key(),theKey)) 
        return p->Value();
      p = (DataMapNode*) p->Next();
    }
    Standard_NoSuchObject::Raise("NCollection_DataMap::Find");
    return p->Value(); // This for compiler
  }

  //! Find value for key with copying.
  //! @return true if key was found
  Standard_Boolean Find (const TheKeyType& theKey,
                         TheItemType&      theValue) const
  {
    if (IsEmpty())
    {
      return Standard_False;
    }

    for (DataMapNode* aNodeIter = (DataMapNode* )myData1[Hasher::HashCode (theKey, NbBuckets())];
         aNodeIter != NULL;
         aNodeIter = (DataMapNode* )aNodeIter->Next())
    {
      if (Hasher::IsEqual (aNodeIter->Key(), theKey))
      {
        theValue = aNodeIter->Value();
        return Standard_True;
      }
    }
    return Standard_False;
  }

  //! operator ()
  const TheItemType& operator() (const TheKeyType& theKey) const
  { return Find(theKey); }

  //! ChangeFind
  TheItemType& ChangeFind (const TheKeyType& theKey)
  {
#if !defined No_Exception && !defined No_Standard_NoSuchObject
    if (IsEmpty())
      Standard_NoSuchObject::Raise ("NCollection_DataMap::Find");
#endif
    DataMapNode*  p = (DataMapNode*) myData1[Hasher::HashCode(theKey,NbBuckets())];
    while (p) 
    {
      if (Hasher::IsEqual(p->Key(),theKey)) 
        return p->ChangeValue();
      p = (DataMapNode*) p->Next();
    }
    Standard_NoSuchObject::Raise("NCollection_DataMap::Find");
    return p->ChangeValue(); // This for compiler
  }

  //! operator ()
  TheItemType& operator() (const TheKeyType& theKey)
  { return ChangeFind(theKey); }

  //! Clear data. If doReleaseMemory is false then the table of
  //! buckets is not released and will be reused.
  void Clear(const Standard_Boolean doReleaseMemory = Standard_True)
  { Destroy (DataMapNode::delNode, this->myAllocator, doReleaseMemory); }

  //! Clear data and reset allocator
  void Clear (const Handle(NCollection_BaseAllocator)& theAllocator)
  { 
    Clear();
    this->myAllocator = ( ! theAllocator.IsNull() ? theAllocator :
                    NCollection_BaseAllocator::CommonBaseAllocator() );
  }

  //! Destructor
  ~NCollection_DataMap (void)
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

