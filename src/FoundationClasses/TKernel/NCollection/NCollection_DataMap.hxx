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
#include <type_traits>
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

template <class TheKeyType, class TheItemType, class Hasher = NCollection_DefaultHasher<TheKeyType>>
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
    DataMapNode(const TheKeyType& theKey, const TheItemType& theItem, NCollection_ListNode* theNext)
        : NCollection_TListNode<TheItemType>(theItem, theNext),
          myKey(theKey)
    {
    }

    //! Constructor with 'Next'
    DataMapNode(const TheKeyType& theKey, TheItemType&& theItem, NCollection_ListNode* theNext)
        : NCollection_TListNode<TheItemType>(std::forward<TheItemType>(theItem), theNext),
          myKey(theKey)
    {
    }

    //! Constructor with 'Next'
    DataMapNode(TheKeyType&& theKey, const TheItemType& theItem, NCollection_ListNode* theNext)
        : NCollection_TListNode<TheItemType>(theItem, theNext),
          myKey(std::forward<TheKeyType>(theKey))
    {
    }

    //! Constructor with 'Next'
    DataMapNode(TheKeyType&& theKey, TheItemType&& theItem, NCollection_ListNode* theNext)
        : NCollection_TListNode<TheItemType>(std::forward<TheItemType>(theItem), theNext),
          myKey(std::forward<TheKeyType>(theKey))
    {
    }

    //! Constructor with in-place value construction
    template <typename K, typename... Args>
    DataMapNode(K&& theKey, std::in_place_t, NCollection_ListNode* theNext, Args&&... theArgs)
        : NCollection_TListNode<TheItemType>(std::in_place,
                                             theNext,
                                             std::forward<Args>(theArgs)...),
          myKey(std::forward<K>(theKey))
    {
    }

    //! Key
    const TheKeyType& Key() const noexcept { return myKey; }

    //! Static deleter to be passed to BaseMap
    static void delNode(NCollection_ListNode*                   theNode,
                        occ::handle<NCollection_BaseAllocator>& theAl) noexcept
    {
      ((DataMapNode*)theNode)->~DataMapNode();
      theAl->Free(theNode);
    }

  private:
    TheKeyType myKey;
  };

public:
  // **************** Implementation of the Iterator interface.
  class Iterator : public NCollection_BaseMap::Iterator
  {
  public:
    //! Empty constructor
    Iterator()
        : NCollection_BaseMap::Iterator()
    {
    }

    //! Constructor
    Iterator(const NCollection_DataMap& theMap)
        : NCollection_BaseMap::Iterator(theMap)
    {
    }

    //! Query if the end of collection is reached by iterator
    bool More() const noexcept { return PMore(); }

    //! Make a step along the collection
    void Next() noexcept { PNext(); }

    //! Value inquiry
    const TheItemType& Value() const
    {
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_DataMap::Iterator::Value");
      return ((DataMapNode*)myNode)->Value();
    }

    //! Value change access
    TheItemType& ChangeValue() const
    {
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_DataMap::Iterator::ChangeValue");
      return ((DataMapNode*)myNode)->ChangeValue();
    }

    //! Key
    const TheKeyType& Key() const
    {
      Standard_NoSuchObject_Raise_if(!More(), "NCollection_DataMap::Iterator::Key");
      return ((DataMapNode*)myNode)->Key();
    }
  };

  //! Shorthand for a regular iterator type.
  typedef NCollection_StlIterator<std::forward_iterator_tag, Iterator, TheItemType, false> iterator;

  //! Shorthand for a constant iterator type.
  typedef NCollection_StlIterator<std::forward_iterator_tag, Iterator, TheItemType, true>
    const_iterator;

  //! Returns an iterator pointing to the first element in the map.
  iterator begin() const noexcept { return Iterator(*this); }

  //! Returns an iterator referring to the past-the-end element in the map.
  iterator end() const noexcept { return Iterator(); }

  //! Returns a const iterator pointing to the first element in the map.
  const_iterator cbegin() const noexcept { return Iterator(*this); }

  //! Returns a const iterator referring to the past-the-end element in the map.
  const_iterator cend() const noexcept { return Iterator(); }

public:
  // ---------- PUBLIC METHODS ------------

  //! Empty Constructor.
  NCollection_DataMap()
      : NCollection_BaseMap(1, true, occ::handle<NCollection_BaseAllocator>())
  {
  }

  //! Constructor
  explicit NCollection_DataMap(const int                                     theNbBuckets,
                               const occ::handle<NCollection_BaseAllocator>& theAllocator = nullptr)
      : NCollection_BaseMap(theNbBuckets, true, theAllocator)
  {
  }

  //! Copy constructor
  NCollection_DataMap(const NCollection_DataMap& theOther)
      : NCollection_BaseMap(theOther.NbBuckets(), true, theOther.myAllocator)
  {
    const int anExt = theOther.Extent();
    if (anExt <= 0)
      return;
    ReSize(anExt - 1);
    for (Iterator anIter(theOther); anIter.More(); anIter.Next())
      Bind(anIter.Key(), anIter.Value());
  }

  //! Move constructor
  NCollection_DataMap(NCollection_DataMap&& theOther) noexcept
      : NCollection_BaseMap(std::forward<NCollection_BaseMap>(theOther))
  {
  }

  //! Exchange the content of two maps without re-allocations.
  //! Notice that allocators will be swapped as well!
  void Exchange(NCollection_DataMap& theOther) noexcept { this->exchangeMapsData(theOther); }

  //! Assignment.
  //! This method does not change the internal allocator.
  NCollection_DataMap& Assign(const NCollection_DataMap& theOther)
  {
    if (this == &theOther)
      return *this;

    Clear();
    int anExt = theOther.Extent();
    if (anExt)
    {
      ReSize(anExt - 1);
      Iterator anIter(theOther);
      for (; anIter.More(); anIter.Next())
        Bind(anIter.Key(), anIter.Value());
    }
    return *this;
  }

  //! Assignment operator
  NCollection_DataMap& operator=(const NCollection_DataMap& theOther) { return Assign(theOther); }

  //! Move operator
  NCollection_DataMap& operator=(NCollection_DataMap&& theOther) noexcept
  {
    if (this == &theOther)
      return *this;
    exchangeMapsData(theOther);
    return *this;
  }

  //! ReSize
  void ReSize(const int N)
  {
    NCollection_ListNode** newdata = nullptr;
    NCollection_ListNode** dummy   = nullptr;
    int                    newBuck;
    if (BeginResize(N, newBuck, newdata, dummy))
    {
      if (myData1)
      {
        DataMapNode** olddata = (DataMapNode**)myData1;
        DataMapNode * p, *q;
        for (int i = 0; i <= NbBuckets(); i++)
        {
          if (olddata[i])
          {
            p = olddata[i];
            while (p)
            {
              const size_t k = HashCode(p->Key(), newBuck);
              q              = (DataMapNode*)p->Next();
              p->Next()      = newdata[k];
              newdata[k]     = p;
              p              = q;
            }
          }
        }
      }
      EndResize(N, newBuck, newdata, dummy);
    }
  }

  //! Bind binds Item to Key in map.
  //! @param theKey  key to add/update
  //! @param theItem new item; overrides value previously bound to the key (uses
  //! destroy+reconstruct)
  //! @return true if Key was not bound already
  bool Bind(const TheKeyType& theKey, const TheItemType& theItem)
  {
    return emplaceImpl(theKey, std::false_type{}, std::false_type{}, theItem);
  }

  //! Bind binds Item to Key in map.
  //! @param theKey  key to add/update
  //! @param theItem new item; overrides value previously bound to the key (uses
  //! destroy+reconstruct)
  //! @return true if Key was not bound already
  bool Bind(TheKeyType&& theKey, const TheItemType& theItem)
  {
    return emplaceImpl(std::move(theKey), std::false_type{}, std::false_type{}, theItem);
  }

  //! Bind binds Item to Key in map.
  //! @param theKey  key to add/update
  //! @param theItem new item; overrides value previously bound to the key (uses
  //! destroy+reconstruct)
  //! @return true if Key was not bound already
  bool Bind(const TheKeyType& theKey, TheItemType&& theItem)
  {
    return emplaceImpl(theKey, std::false_type{}, std::false_type{}, std::move(theItem));
  }

  //! Bind binds Item to Key in map.
  //! @param theKey  key to add/update
  //! @param theItem new item; overrides value previously bound to the key (uses
  //! destroy+reconstruct)
  //! @return true if Key was not bound already
  bool Bind(TheKeyType&& theKey, TheItemType&& theItem)
  {
    return emplaceImpl(std::move(theKey), std::false_type{}, std::false_type{}, std::move(theItem));
  }

  //! Bound binds Item to Key in map.
  //! @param theKey  key to add/update
  //! @param theItem new item; overrides value previously bound to the key (uses
  //! destroy+reconstruct)
  //! @return pointer to modifiable Item
  TheItemType* Bound(const TheKeyType& theKey, const TheItemType& theItem)
  {
    return &emplaceImpl(theKey, std::false_type{}, std::true_type{}, theItem);
  }

  //! Bound binds Item to Key in map.
  //! @param theKey  key to add/update
  //! @param theItem new item; overrides value previously bound to the key (uses
  //! destroy+reconstruct)
  //! @return pointer to modifiable Item
  TheItemType* Bound(TheKeyType&& theKey, const TheItemType& theItem)
  {
    return &emplaceImpl(std::move(theKey), std::false_type{}, std::true_type{}, theItem);
  }

  //! Bound binds Item to Key in map.
  //! @param theKey  key to add/update
  //! @param theItem new item; overrides value previously bound to the key (uses
  //! destroy+reconstruct)
  //! @return pointer to modifiable Item
  TheItemType* Bound(const TheKeyType& theKey, TheItemType&& theItem)
  {
    return &emplaceImpl(theKey, std::false_type{}, std::true_type{}, std::move(theItem));
  }

  //! Bound binds Item to Key in map.
  //! @param theKey  key to add/update
  //! @param theItem new item; overrides value previously bound to the key (uses
  //! destroy+reconstruct)
  //! @return pointer to modifiable Item
  TheItemType* Bound(TheKeyType&& theKey, TheItemType&& theItem)
  {
    return &emplaceImpl(std::move(theKey), std::false_type{}, std::true_type{}, std::move(theItem));
  }

  //! TryBind binds Item to Key in map only if Key is not yet bound.
  //! @param theKey  key to add
  //! @param theItem item to bind if Key is not yet bound
  //! @return true if Key was newly bound, false if Key already existed (no replacement)
  bool TryBind(const TheKeyType& theKey, const TheItemType& theItem)
  {
    return emplaceImpl(theKey, std::true_type{}, std::false_type{}, theItem);
  }

  //! TryBind binds Item to Key in map only if Key is not yet bound.
  //! @param theKey  key to add
  //! @param theItem item to bind if Key is not yet bound
  //! @return true if Key was newly bound, false if Key already existed (no replacement)
  bool TryBind(TheKeyType&& theKey, const TheItemType& theItem)
  {
    return emplaceImpl(std::move(theKey), std::true_type{}, std::false_type{}, theItem);
  }

  //! TryBind binds Item to Key in map only if Key is not yet bound.
  //! @param theKey  key to add
  //! @param theItem item to bind if Key is not yet bound
  //! @return true if Key was newly bound, false if Key already existed (no replacement)
  bool TryBind(const TheKeyType& theKey, TheItemType&& theItem)
  {
    return emplaceImpl(theKey, std::true_type{}, std::false_type{}, std::move(theItem));
  }

  //! TryBind binds Item to Key in map only if Key is not yet bound.
  //! @param theKey  key to add
  //! @param theItem item to bind if Key is not yet bound
  //! @return true if Key was newly bound, false if Key already existed (no replacement)
  bool TryBind(TheKeyType&& theKey, TheItemType&& theItem)
  {
    return emplaceImpl(std::move(theKey), std::true_type{}, std::false_type{}, std::move(theItem));
  }

  //! TryBound binds Item to Key in map only if Key is not yet bound.
  //! @param theKey  key to add
  //! @param theItem item to bind if Key is not yet bound
  //! @return reference to existing or newly bound Item
  TheItemType& TryBound(const TheKeyType& theKey, const TheItemType& theItem)
  {
    return emplaceImpl(theKey, std::true_type{}, std::true_type{}, theItem);
  }

  //! TryBound binds Item to Key in map only if Key is not yet bound.
  //! @param theKey  key to add
  //! @param theItem item to bind if Key is not yet bound
  //! @return reference to existing or newly bound Item
  TheItemType& TryBound(TheKeyType&& theKey, const TheItemType& theItem)
  {
    return emplaceImpl(std::move(theKey), std::true_type{}, std::true_type{}, theItem);
  }

  //! TryBound binds Item to Key in map only if Key is not yet bound.
  //! @param theKey  key to add
  //! @param theItem item to bind if Key is not yet bound
  //! @return reference to existing or newly bound Item
  TheItemType& TryBound(const TheKeyType& theKey, TheItemType&& theItem)
  {
    return emplaceImpl(theKey, std::true_type{}, std::true_type{}, std::move(theItem));
  }

  //! TryBound binds Item to Key in map only if Key is not yet bound.
  //! @param theKey  key to add
  //! @param theItem item to bind if Key is not yet bound
  //! @return reference to existing or newly bound Item
  TheItemType& TryBound(TheKeyType&& theKey, TheItemType&& theItem)
  {
    return emplaceImpl(std::move(theKey), std::true_type{}, std::true_type{}, std::move(theItem));
  }

  //! Emplace constructs value in-place; if key exists, destroys and reconstructs value.
  //! @param theKey  key to add/update
  //! @param theArgs arguments forwarded to value constructor
  //! @return true if key was newly added, false if key already existed (and value was
  //! reconstructed)
  template <typename K, typename... Args>
  bool Emplace(K&& theKey, Args&&... theArgs)
  {
    return emplaceImpl(std::forward<K>(theKey),
                       std::false_type{},
                       std::false_type{},
                       std::forward<Args>(theArgs)...);
  }

  //! Emplaced constructs value in-place; if key exists, destroys and reconstructs value.
  //! @param theKey  key to add/update
  //! @param theArgs arguments forwarded to value constructor
  //! @return reference to the value (existing reconstructed or newly added)
  template <typename K, typename... Args>
  TheItemType& Emplaced(K&& theKey, Args&&... theArgs)
  {
    return emplaceImpl(std::forward<K>(theKey),
                       std::false_type{},
                       std::true_type{},
                       std::forward<Args>(theArgs)...);
  }

  //! TryEmplace constructs value in-place only if key not already bound.
  //! @param theKey  key to add
  //! @param theArgs arguments forwarded to value constructor
  //! @return true if key was newly added, false if key already existed
  template <typename K, typename... Args>
  bool TryEmplace(K&& theKey, Args&&... theArgs)
  {
    return emplaceImpl(std::forward<K>(theKey),
                       std::true_type{},
                       std::false_type{},
                       std::forward<Args>(theArgs)...);
  }

  //! TryEmplaced constructs value in-place only if key not already bound.
  //! @param theKey  key to add
  //! @param theArgs arguments forwarded to value constructor
  //! @return reference to the value (existing or newly added)
  template <typename K, typename... Args>
  TheItemType& TryEmplaced(K&& theKey, Args&&... theArgs)
  {
    return emplaceImpl(std::forward<K>(theKey),
                       std::true_type{},
                       std::true_type{},
                       std::forward<Args>(theArgs)...);
  }

  //! IsBound
  bool IsBound(const TheKeyType& theKey) const
  {
    DataMapNode* p;
    return lookup(theKey, p);
  }

  //! UnBind removes Item Key pair from map
  bool UnBind(const TheKeyType& theKey)
  {
    if (IsEmpty())
      return false;
    DataMapNode** data = (DataMapNode**)myData1;
    const size_t  k    = HashCode(theKey, NbBuckets());
    DataMapNode*  p    = data[k];
    DataMapNode*  q    = nullptr;
    while (p)
    {
      if (IsEqual(p->Key(), theKey))
      {
        Decrement();
        if (q)
          q->Next() = p->Next();
        else
          data[k] = (DataMapNode*)p->Next();
        p->~DataMapNode();
        this->myAllocator->Free(p);
        return true;
      }
      q = p;
      p = (DataMapNode*)p->Next();
    }
    return false;
  }

  //! Seek returns pointer to Item by Key. Returns
  //! NULL is Key was not bound.
  const TheItemType* Seek(const TheKeyType& theKey) const
  {
    DataMapNode* p = nullptr;
    if (!lookup(theKey, p))
      return nullptr;
    return &p->Value();
  }

  //! Find returns the Item for Key. Raises if Key was not bound
  const TheItemType& Find(const TheKeyType& theKey) const
  {
    DataMapNode* p = nullptr;
    if (!lookup(theKey, p))
      throw Standard_NoSuchObject("NCollection_DataMap::Find");
    return p->Value();
  }

  //! Find Item for key with copying.
  //! @return true if key was found
  bool Find(const TheKeyType& theKey, TheItemType& theValue) const
  {
    DataMapNode* p = nullptr;
    if (!lookup(theKey, p))
      return false;

    theValue = p->Value();
    return true;
  }

  //! operator ()
  const TheItemType& operator()(const TheKeyType& theKey) const { return Find(theKey); }

  //! ChangeSeek returns modifiable pointer to Item by Key. Returns
  //! NULL is Key was not bound.
  TheItemType* ChangeSeek(const TheKeyType& theKey)
  {
    DataMapNode* p = nullptr;
    if (!lookup(theKey, p))
      return nullptr;
    return &p->ChangeValue();
  }

  //! ChangeFind returns mofifiable Item by Key. Raises if Key was not bound
  TheItemType& ChangeFind(const TheKeyType& theKey)
  {
    DataMapNode* p = nullptr;
    if (!lookup(theKey, p))
      throw Standard_NoSuchObject("NCollection_DataMap::Find");
    return p->ChangeValue();
  }

  //! operator ()
  TheItemType& operator()(const TheKeyType& theKey) { return ChangeFind(theKey); }

  //! Clear data. If doReleaseMemory is false then the table of
  //! buckets is not released and will be reused.
  void Clear(const bool doReleaseMemory = false) { Destroy(DataMapNode::delNode, doReleaseMemory); }

  //! Clear data and reset allocator
  void Clear(const occ::handle<NCollection_BaseAllocator>& theAllocator)
  {
    Clear(theAllocator != this->myAllocator);
    this->myAllocator =
      (!theAllocator.IsNull() ? theAllocator : NCollection_BaseAllocator::CommonBaseAllocator());
  }

  //! Destructor
  ~NCollection_DataMap() override { Clear(true); }

  //! Size
  int Size() const noexcept { return Extent(); }

protected:
  //! Lookup for particular key in map.
  //! @param[in] theKey key to compute hash
  //! @param[out] theNode the detected node with equal key. Can be null.
  //! @return true if key is found
  bool lookup(const TheKeyType& theKey, DataMapNode*& theNode) const
  {
    if (IsEmpty())
      return false; // Not found
    for (theNode = (DataMapNode*)myData1[HashCode(theKey, NbBuckets())]; theNode;
         theNode = (DataMapNode*)theNode->Next())
    {
      if (IsEqual(theNode->Key(), theKey))
        return true;
    }
    return false; // Not found
  }

  //! Lookup for particular key in map.
  //! @param[in] theKey key to compute hash
  //! @param[out] theNode the detected node with equal key. Can be null.
  //! @param[out] theHash computed bounded hash code for current key.
  //! @return true if key is found
  bool lookup(const TheKeyType& theKey, DataMapNode*& theNode, size_t& theHash) const
  {
    theHash = HashCode(theKey, NbBuckets());
    if (IsEmpty())
      return false; // Not found
    for (theNode = (DataMapNode*)myData1[theHash]; theNode; theNode = (DataMapNode*)theNode->Next())
    {
      if (IsEqual(theNode->Key(), theKey))
      {
        return true;
      }
    }
    return false; // Not found
  }

  bool IsEqual(const TheKeyType& theKey1, const TheKeyType& theKey2) const
  {
    return myHasher(theKey1, theKey2);
  }

  size_t HashCode(const TheKeyType& theKey, const int theUpperBound) const
  {
    return myHasher(theKey) % theUpperBound + 1;
  }

  //! Implementation helper for Bind/TryBind/Bound/TryBound/Emplace/TryEmplace/Emplaced/TryEmplaced.
  //! Uses destroy + reconstruct in-place instead of assignment operator.
  //! @tparam K forwarding reference type for key
  //! @tparam IsTry if true, does not overwrite existing; if false, destroys and reconstructs
  //! @tparam ReturnRef if true, returns reference; if false, returns bool
  //! @param theKey  key to add/update
  //! @param theArgs arguments forwarded to value constructor
  //! @return bool or TheItemType& depending on ReturnRef
  template <typename K, bool IsTry, bool ReturnRef, typename... Args>
  auto emplaceImpl(K&& theKey,
                   std::bool_constant<IsTry>,
                   std::bool_constant<ReturnRef>,
                   Args&&... theArgs) -> std::conditional_t<ReturnRef, TheItemType&, bool>
  {
    if (Resizable())
      ReSize(Extent());
    size_t       aHash;
    DataMapNode* aNode;
    if (lookup(theKey, aNode, aHash))
    {
      if constexpr (!IsTry)
      {
        aNode->ChangeValue() = TheItemType(std::forward<Args>(theArgs)...);
      }
      if constexpr (ReturnRef)
        return aNode->ChangeValue();
      else
        return false;
    }
    DataMapNode** data = (DataMapNode**)myData1;
    data[aHash]        = new (this->myAllocator) DataMapNode(std::forward<K>(theKey),
                                                      std::in_place,
                                                      data[aHash],
                                                      std::forward<Args>(theArgs)...);
    Increment();
    if constexpr (ReturnRef)
      return data[aHash]->ChangeValue();
    else
      return true;
  }

private:
  Hasher myHasher;
};

#endif
