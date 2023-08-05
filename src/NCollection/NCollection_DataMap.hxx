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

#include <NCollection_BaseMap.hxx>
#include <NCollection_TListNode.hxx>
#include <NCollection_StlIterator.hxx>
#include <NCollection_DefaultHasher.hxx>

#include <Standard_TypeMismatch.hxx>
#include <Standard_NoSuchObject.hxx>
#include <utility>

#include <Message.hxx>

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
           class Hasher = NCollection_DefaultHasher<TheKeyType> >
class NCollection_DataMap : public NCollection_BaseMap
{
public:
  //! STL-compliant typedef for key type
  typedef TheKeyType key_type;
  //! STL-compliant typedef for value type
  typedef TheItemType value_type;

public:
  // **************** Adaptation of the TListNode to the DATAmap
  class DataMapNode : public NCollection_TListNode<TheItemType>
  {
  public:
    //! Constructor with 'Next'
    DataMapNode (const TheKeyType&     theKey, 
                 const TheItemType&    theItem, 
                 NCollection_ListNode* theNext) :
      NCollection_TListNode<TheItemType> (theItem, theNext),
      myKey(theKey)
    {}
    //! Constructor with 'Next'
    DataMapNode (const TheKeyType&     theKey, 
                 TheItemType&&         theItem, 
                 NCollection_ListNode* theNext) :
      NCollection_TListNode<TheItemType> (std::forward<TheItemType>(theItem), theNext),
      myKey(theKey)
    {}
    //! Constructor with 'Next'
    DataMapNode (TheKeyType&&          theKey, 
                 const TheItemType&    theItem, 
                 NCollection_ListNode* theNext) :
      NCollection_TListNode<TheItemType> (theItem, theNext),
      myKey(std::forward<TheKeyType>(theKey))
    {}
    //! Constructor with 'Next'
    DataMapNode (TheKeyType&&          theKey, 
                 TheItemType&&         theItem, 
                 NCollection_ListNode* theNext) :
      NCollection_TListNode<TheItemType> (std::forward<TheItemType>(theItem), theNext),
      myKey(std::forward<TheKeyType>(theKey))
    {}

    //! Key
    const TheKeyType& Key (void) const
    { return myKey; }
    
    //! Static deleter to be passed to BaseMap
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
  class Iterator : public NCollection_BaseMap::Iterator
  {
  public:
    //! Empty constructor
    Iterator (void) :
      NCollection_BaseMap::Iterator() {}
    //! Constructor
    Iterator (const NCollection_DataMap& theMap) :
      NCollection_BaseMap::Iterator(theMap) {}
    //! Query if the end of collection is reached by iterator
    Standard_Boolean More(void) const
    { return PMore(); }
    //! Make a step along the collection
    void Next(void)
    { PNext(); }
    //! Value inquiry
    const TheItemType& Value(void) const
    {  
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_DataMap::Iterator::Value");  
      return ((DataMapNode *) myNode)->Value();
    }
    //! Value change access
    TheItemType& ChangeValue(void) const
    {  
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_DataMap::Iterator::ChangeValue");  
      return ((DataMapNode *) myNode)->ChangeValue();
    }
    //! Key
    const TheKeyType& Key (void) const
    { 
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_DataMap::Iterator::Key");  
      return ((DataMapNode *) myNode)->Key();
    }
  };
  
  //! Shorthand for a regular iterator type.
  typedef NCollection_StlIterator<std::forward_iterator_tag, Iterator, TheItemType, false> iterator;

  //! Shorthand for a constant iterator type.
  typedef NCollection_StlIterator<std::forward_iterator_tag, Iterator, TheItemType, true> const_iterator;

  //! Returns an iterator pointing to the first element in the map.
  iterator begin() const { return Iterator (*this); }

  //! Returns an iterator referring to the past-the-end element in the map.
  iterator end() const { return Iterator(); }

  //! Returns a const iterator pointing to the first element in the map.
  const_iterator cbegin() const { return Iterator (*this); }

  //! Returns a const iterator referring to the past-the-end element in the map.
  const_iterator cend() const { return Iterator(); }

 public:
  // ---------- PUBLIC METHODS ------------

  //! Empty Constructor.
  NCollection_DataMap() : NCollection_BaseMap (1, Standard_True, Handle(NCollection_BaseAllocator)()) {}

  //! Constructor
  explicit NCollection_DataMap (const Standard_Integer theNbBuckets,
                                const Handle(NCollection_BaseAllocator)& theAllocator = 0L)
  : NCollection_BaseMap (theNbBuckets, Standard_True, theAllocator) {}

  //! Copy constructor
  NCollection_DataMap (const NCollection_DataMap& theOther)
    : NCollection_BaseMap (theOther.NbBuckets(), Standard_True, theOther.myAllocator) 
  { 
    const int anExt = theOther.Extent();
    if (anExt <= 0)
      return;
    ReSize(anExt - 1);
    for (Iterator anIter(theOther); anIter.More(); anIter.Next())
      Bind(anIter.Key(), anIter.Value());
  }

  //! Move constructor
  NCollection_DataMap(NCollection_DataMap&& theOther) noexcept :
    NCollection_BaseMap(std::forward<NCollection_BaseMap>(theOther))
  {}

  //! Exchange the content of two maps without re-allocations.
  //! Notice that allocators will be swapped as well!
  void Exchange (NCollection_DataMap& theOther)
  {
    this->exchangeMapsData (theOther);
  }

  //! Assignment.
  //! This method does not change the internal allocator.
  NCollection_DataMap& Assign (const NCollection_DataMap& theOther)
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
        Bind (anIter.Key(), anIter.Value());
    }
    return *this;
  }

  //! Assignment operator
  NCollection_DataMap& operator= (const NCollection_DataMap& theOther)
  { 
    return Assign (theOther);
  }

  //! Move operator
  NCollection_DataMap& operator= (NCollection_DataMap&& theOther) noexcept
  {
    if (this == &theOther)
      return *this;
    exchangeMapsData(theOther);
    return *this;
  }

  //! ReSize
  void ReSize (const Standard_Integer N)
  {
    NCollection_ListNode** newdata = NULL;
    NCollection_ListNode** dummy   = NULL;
    Standard_Integer newBuck;
    if (BeginResize (N, newBuck, newdata, dummy))
    {
      if (myData1) 
      {
        DataMapNode** olddata = (DataMapNode**) myData1;
        DataMapNode *p, *q;
        for (int i = 0; i <= NbBuckets(); i++) 
        {
          if (olddata[i]) 
          {
            p = olddata[i];
            while (p) 
            {
              const size_t k = HashCode(p->Key(),newBuck);
              q = (DataMapNode*) p->Next();
              p->Next() = newdata[k];
              newdata[k] = p;
              p = q;
            }
          }
        }
      }
      EndResize (N, newBuck, newdata, dummy);
    }
  }

  //! Bind binds Item to Key in map.
  //! @param theKey  key to add/update
  //! @param theItem new item; overrides value previously bound to the key
  //! @return Standard_True if Key was not bound already
  Standard_Boolean Bind (const TheKeyType& theKey, const TheItemType& theItem)
  {
    if (Resizable()) 
      ReSize(Extent());
    size_t aHash;
    DataMapNode* aNode;
    if (lookup(theKey, aNode, aHash))
    {
      aNode->ChangeValue() = theItem;
      return Standard_False;
    }
    DataMapNode** data = (DataMapNode**)myData1;
    data[aHash] = new (this->myAllocator) DataMapNode (theKey, theItem, data[aHash]);
    Increment();
    return Standard_True;
  }

  //! Bind binds Item to Key in map.
  //! @param theKey  key to add/update
  //! @param theItem new item; overrides value previously bound to the key
  //! @return Standard_True if Key was not bound already
  Standard_Boolean Bind (TheKeyType&& theKey, const TheItemType& theItem)
  {
    if (Resizable()) 
      ReSize(Extent());
    size_t aHash;
    DataMapNode* aNode;
    if (lookup(theKey, aNode, aHash))
    {
      aNode->ChangeValue() = theItem;
      return Standard_False;
    }
    DataMapNode** data = (DataMapNode**)myData1;
    data[aHash] = new (this->myAllocator) DataMapNode (std::forward<TheKeyType>(theKey), theItem, data[aHash]);
    Increment();
    return Standard_True;
  }

  //! Bind binds Item to Key in map.
  //! @param theKey  key to add/update
  //! @param theItem new item; overrides value previously bound to the key
  //! @return Standard_True if Key was not bound already
  Standard_Boolean Bind (const TheKeyType& theKey, TheItemType&& theItem)
  {
    if (Resizable()) 
      ReSize(Extent());
    size_t aHash;
    DataMapNode* aNode;
    if (lookup(theKey, aNode, aHash))
    {
      aNode->ChangeValue() = std::forward<TheItemType>(theItem);
      return Standard_False;
    }
    DataMapNode** data = (DataMapNode**)myData1;
    data[aHash] = new (this->myAllocator) DataMapNode (theKey, std::forward<TheItemType>(theItem), data[aHash]);
    Increment();
    return Standard_True;
  }

  //! Bind binds Item to Key in map.
  //! @param theKey  key to add/update
  //! @param theItem new item; overrides value previously bound to the key
  //! @return Standard_True if Key was not bound already
  Standard_Boolean Bind (TheKeyType&& theKey, TheItemType&& theItem)
  {
    if (Resizable()) 
      ReSize(Extent());
    size_t aHash;
    DataMapNode* aNode;
    if (lookup(theKey, aNode, aHash))
    {
      aNode->ChangeValue() = theItem;
      return Standard_False;
    }
    DataMapNode** data = (DataMapNode**)myData1;
    data[aHash] = new (this->myAllocator) DataMapNode (std::forward<TheKeyType>(theKey),
                                                       std::forward<TheItemType>(theItem), data[aHash]);
    Increment();
    return Standard_True;
  }

  //! Bound binds Item to Key in map.
  //! @param theKey  key to add/update
  //! @param theItem new item; overrides value previously bound to the key
  //! @return pointer to modifiable Item
  TheItemType* Bound (const TheKeyType& theKey, const TheItemType& theItem)
  {
    if (Resizable()) 
      ReSize(Extent());
    size_t aHash;
    DataMapNode* aNode;
    if (lookup(theKey, aNode, aHash))
    {
      aNode->ChangeValue() = theItem;
      return &aNode->ChangeValue();
    }
    DataMapNode** data = (DataMapNode**)myData1;
    data[aHash] = new (this->myAllocator) DataMapNode (theKey, theItem, data[aHash]);
    Increment();
    return &data[aHash]->ChangeValue();
  }

  //! Bound binds Item to Key in map.
  //! @param theKey  key to add/update
  //! @param theItem new item; overrides value previously bound to the key
  //! @return pointer to modifiable Item
  TheItemType* Bound (TheKeyType&& theKey, const TheItemType& theItem)
  {
    if (Resizable()) 
      ReSize(Extent());
    size_t aHash;
    DataMapNode* aNode;
    if (lookup(theKey, aNode, aHash))
    {
      aNode->ChangeValue() = theItem;
      return &aNode->ChangeValue();
    }
    DataMapNode** data = (DataMapNode**)myData1;
    data[aHash] = new (this->myAllocator) DataMapNode (std::forward<TheKeyType>(theKey), theItem, data[aHash]);
    Increment();
    return &data[aHash]->ChangeValue();
  }

  //! Bound binds Item to Key in map.
  //! @param theKey  key to add/update
  //! @param theItem new item; overrides value previously bound to the key
  //! @return pointer to modifiable Item
  TheItemType* Bound (const TheKeyType& theKey, TheItemType&& theItem)
  {
    if (Resizable()) 
      ReSize(Extent());
    size_t aHash;
    DataMapNode* aNode;
    if (lookup(theKey, aNode, aHash))
    {
      aNode->ChangeValue() = std::forward<TheItemType>(theItem);
      return &aNode->ChangeValue();
    }
    DataMapNode** data = (DataMapNode**)myData1;
    data[aHash] = new (this->myAllocator) DataMapNode (theKey, std::forward<TheItemType>(theItem), data[aHash]);
    Increment();
    return &data[aHash]->ChangeValue();
  }

  //! Bound binds Item to Key in map.
  //! @param theKey  key to add/update
  //! @param theItem new item; overrides value previously bound to the key
  //! @return pointer to modifiable Item
  TheItemType* Bound (TheKeyType&& theKey, TheItemType&& theItem)
  {
    if (Resizable()) 
      ReSize(Extent());
    size_t aHash;
    DataMapNode* aNode;
    if (lookup(theKey, aNode, aHash))
    {
      aNode->ChangeValue() = std::forward<TheItemType>(theItem);
      return &aNode->ChangeValue();
    }
    DataMapNode** data = (DataMapNode**)myData1;
    data[aHash] = new (this->myAllocator) DataMapNode (std::forward<TheKeyType>(theKey), 
                                                       std::forward<TheItemType>(theItem), data[aHash]);
    Increment();
    return &data[aHash]->ChangeValue();
  }

  //! IsBound
  Standard_Boolean IsBound(const TheKeyType& theKey) const
  {
    DataMapNode* p;
    return lookup(theKey, p);
  }

  //! UnBind removes Item Key pair from map
  Standard_Boolean UnBind(const TheKeyType& theKey)
  {
    if (IsEmpty()) 
      return Standard_False;
    DataMapNode** data = (DataMapNode**) myData1;
    const size_t k = HashCode(theKey,NbBuckets());
    DataMapNode* p = data[k];
    DataMapNode* q = NULL;
    while (p) 
    {
      if (IsEqual(p->Key(), theKey))
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

  //! Seek returns pointer to Item by Key. Returns
  //! NULL is Key was not bound.
  const TheItemType* Seek(const TheKeyType& theKey) const
  {
    DataMapNode* p = 0;
    if (!lookup(theKey, p))
      return 0L;
    return &p->Value();
  }

  //! Find returns the Item for Key. Raises if Key was not bound
  const TheItemType& Find(const TheKeyType& theKey) const
  {
    DataMapNode* p = 0;
    if (!lookup(theKey, p))
      throw Standard_NoSuchObject("NCollection_DataMap::Find");
    return p->Value();
  }

  //! Find Item for key with copying.
  //! @return true if key was found
  Standard_Boolean Find (const TheKeyType& theKey,
                         TheItemType&      theValue) const
  {
    DataMapNode* p = 0;
    if (!lookup(theKey, p))
      return Standard_False;

    theValue = p->Value();
    return Standard_True;
  }

  //! operator ()
  const TheItemType& operator() (const TheKeyType& theKey) const
  { return Find(theKey); }

  //! ChangeSeek returns modifiable pointer to Item by Key. Returns
  //! NULL is Key was not bound.
  TheItemType* ChangeSeek(const TheKeyType& theKey)
  {
    DataMapNode* p = 0;
    if (!lookup(theKey, p))
      return 0L;
    return &p->ChangeValue();
  }

  //! ChangeFind returns mofifiable Item by Key. Raises if Key was not bound
  TheItemType& ChangeFind (const TheKeyType& theKey)
  {
    DataMapNode* p = 0;
    if (!lookup(theKey, p))
      throw Standard_NoSuchObject("NCollection_DataMap::Find");
    return p->ChangeValue();
  }

  //! operator ()
  TheItemType& operator() (const TheKeyType& theKey)
  { return ChangeFind(theKey); }

  //! Clear data. If doReleaseMemory is false then the table of
  //! buckets is not released and will be reused.
  void Clear(const Standard_Boolean doReleaseMemory = Standard_False)
  { Destroy (DataMapNode::delNode, doReleaseMemory); }

  //! Clear data and reset allocator
  void Clear (const Handle(NCollection_BaseAllocator)& theAllocator)
  { 
    Clear(theAllocator != this->myAllocator);
    this->myAllocator = ( ! theAllocator.IsNull() ? theAllocator :
                    NCollection_BaseAllocator::CommonBaseAllocator() );
  }

  //! Destructor
  virtual ~NCollection_DataMap (void)
  { Clear(true); }

  //! Size
  Standard_Integer Size(void) const
  { return Extent(); }

  
 protected:

  //! Lookup for particular key in map.
  //! @param[in] theKey key to compute hash
  //! @param[out] theNode the detected node with equal key. Can be null.
  //! @return true if key is found
  Standard_Boolean lookup(const TheKeyType& theKey,DataMapNode*& theNode) const
  {
    if (IsEmpty())
      return Standard_False; // Not found
    for (theNode = (DataMapNode*)myData1[HashCode(theKey, NbBuckets())];
         theNode; theNode = (DataMapNode*)theNode->Next())
    {
      if (IsEqual(theNode->Key(), theKey))
        return Standard_True;
    }
    return Standard_False; // Not found
  }

  //! Lookup for particular key in map.
  //! @param[in] theKey key to compute hash
  //! @param[out] theNode the detected node with equal key. Can be null.
  //! @param[out] theHash computed bounded hash code for current key.
  //! @return true if key is found
  Standard_Boolean lookup(const TheKeyType& theKey, DataMapNode*& theNode, size_t& theHash) const
  {
    theHash = HashCode(theKey, NbBuckets());
    if (IsEmpty())
      return Standard_False; // Not found
    for (theNode = (DataMapNode*)myData1[theHash];
         theNode; theNode = (DataMapNode*)theNode->Next())
    {
      if (IsEqual(theNode->Key(), theKey))
      {
        return Standard_True;
      }
    }
    return Standard_False; // Not found
  }

  bool IsEqual(const TheKeyType& theKey1,
               const TheKeyType& theKey2) const
  {
    return myHasher(theKey1, theKey2);
  }

  size_t HashCode(const TheKeyType& theKey,
                  const int theUpperBound) const
  {
    return myHasher(theKey) % theUpperBound + 1;
  }

private:

  Hasher myHasher;
};

#endif

