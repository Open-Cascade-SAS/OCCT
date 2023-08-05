// Created on: 2002-04-23
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

#ifndef NCollection_Map_HeaderFile
#define NCollection_Map_HeaderFile

#include <NCollection_DataMap.hxx>
#include <NCollection_TListNode.hxx>
#include <NCollection_StlIterator.hxx>
#include <NCollection_DefaultHasher.hxx>

#include <Standard_NoSuchObject.hxx>
#include <utility>

/**
 * Purpose:     Single hashed Map. This  Map is used  to store and
 *              retrieve keys in linear time.
 *              
 *              The ::Iterator class can be  used to explore  the
 *              content of the map. It is not  wise to iterate and
 *              modify a map in parallel.
 *               
 *              To compute  the hashcode of  the key the  function
 *              ::HashCode must be defined in the global namespace
 *               
 *              To compare two keys the function ::IsEqual must be
 *              defined in the global namespace.
 *               
 *              The performance of  a Map is conditioned  by  its
 *              number of buckets that  should be kept greater  to
 *              the number   of keys.  This  map has  an automatic
 *              management of the number of buckets. It is resized
 *              when  the number of Keys  becomes greater than the
 *              number of buckets.
 *              
 *              If you have a fair  idea of the number of  objects
 *              you  can save on automatic   resizing by giving  a
 *              number of buckets  at creation or using the ReSize
 *              method. This should be  consider only for  crucial
 *              optimisation issues.
 */            

template < class TheKeyType, 
           class Hasher = NCollection_DefaultHasher<TheKeyType> >
class NCollection_Map : public NCollection_BaseMap
{
public:
  //! STL-compliant typedef for key type
  typedef TheKeyType key_type;
  typedef Hasher hasher;

public:
  //!   Adaptation of the TListNode to the map notations
  class MapNode : public NCollection_TListNode<TheKeyType>
  {
  public:
    //! Constructor with 'Next'
    MapNode (const TheKeyType& theKey, 
             NCollection_ListNode* theNext) :
      NCollection_TListNode<TheKeyType> (theKey, theNext) {}
    //! Constructor with 'Next'
    MapNode (TheKeyType&& theKey,
             NCollection_ListNode* theNext) :
      NCollection_TListNode<TheKeyType> (std::forward<TheKeyType>(theKey), theNext) {}
    //! Key
    const TheKeyType& Key (void)
    { return this->Value(); }

  };

 public:
  //!   Implementation of the Iterator interface.
  class Iterator : public NCollection_BaseMap::Iterator
  {
  public:
    //! Empty constructor
    Iterator (void) :
      NCollection_BaseMap::Iterator() {}
    //! Constructor
    Iterator (const NCollection_Map& theMap) :
      NCollection_BaseMap::Iterator(theMap) {}
    //! Query if the end of collection is reached by iterator
    Standard_Boolean More(void) const
    { return PMore(); }
    //! Make a step along the collection
    void Next(void)
    { PNext(); }
    //! Value inquiry
    const TheKeyType& Value(void) const
    {
      Standard_NoSuchObject_Raise_if (!More(), "NCollection_Map::Iterator::Value");  
      return ((MapNode *) myNode)->Value();
    }

    //! Key
    const TheKeyType& Key (void) const
    { 
      Standard_NoSuchObject_Raise_if (!More(), "NCollection_Map::Iterator::Key");  
      return ((MapNode *) myNode)->Value();
    }
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
  NCollection_Map() : NCollection_BaseMap (1, Standard_True, Handle(NCollection_BaseAllocator)()) {}

  //! Constructor
  explicit NCollection_Map (const Standard_Integer theNbBuckets,
                            const Handle(NCollection_BaseAllocator)& theAllocator = 0L)
  : NCollection_BaseMap (theNbBuckets, Standard_True, theAllocator) {}

  //! Copy constructor
  NCollection_Map(const NCollection_Map& theOther) :
    NCollection_BaseMap(theOther.NbBuckets(), Standard_True, theOther.myAllocator)
  {
    const int anExt = theOther.Extent();
    if (anExt <= 0)
      return;
    ReSize(anExt - 1);
    for (Iterator anIter(theOther); anIter.More(); anIter.Next())
      Add(anIter.Key());
  }

  //! Move constructor
  NCollection_Map (NCollection_Map&& theOther) noexcept :
    NCollection_BaseMap (std::forward<NCollection_BaseMap>(theOther))
  {}

  //! Exchange the content of two maps without re-allocations.
  //! Notice that allocators will be swapped as well!
  void Exchange (NCollection_Map& theOther)
  {
    this->exchangeMapsData (theOther);
  }

  //! Assign.
  //! This method does not change the internal allocator.
  NCollection_Map& Assign (const NCollection_Map& theOther)
  { 
    if (this == &theOther)
      return *this;

    Clear();
    int anExt = theOther.Extent();
    if (anExt)
    {
      ReSize (anExt-1);
      Iterator anIter(theOther);
      for (; anIter.More(); anIter.Next())
        Add (anIter.Key());
    }
    return *this;
  }

  //! Assign operator
  NCollection_Map& operator= (const NCollection_Map& theOther)
  {
    return Assign(theOther);
  }

  //! Move operator
  NCollection_Map& operator= (NCollection_Map&& theOther) noexcept
  {
    if (this == &theOther)
      return *this;
    exchangeMapsData(theOther);
    return *this;
  }

  //! ReSize
  void ReSize (const Standard_Integer N)
  {
    NCollection_ListNode** newdata = 0L;
    NCollection_ListNode** dummy = 0L;
    Standard_Integer newBuck;
    if (BeginResize (N, newBuck, newdata, dummy))
    {
      if (myData1) 
      {
        MapNode** olddata = (MapNode**) myData1;
        MapNode *p, *q;
        for (int i = 0; i <= NbBuckets(); i++) 
        {
          if (olddata[i]) 
          {
            p = olddata[i];
            while (p) 
            {
              const size_t k = HashCode(p->Key(),newBuck);
              q = (MapNode*) p->Next();
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

  //! Add
  Standard_Boolean Add(const TheKeyType& theKey)
  {
    if (Resizable()) 
      ReSize(Extent());
    MapNode* aNode;
    size_t aHash;
    if (lookup(theKey, aNode, aHash))
    {
      return Standard_False;
    }
    MapNode** data = (MapNode**)myData1;
    data[aHash] = new (this->myAllocator) MapNode(theKey,data[aHash]);
    Increment();
    return Standard_True;
  }

  //! Add
  Standard_Boolean Add(TheKeyType&& theKey)
  {
    if (Resizable()) 
      ReSize(Extent());
    MapNode* aNode;
    size_t aHash;
    if (lookup(theKey, aNode, aHash))
    {
      return Standard_False;
    }
    MapNode** data = (MapNode**)myData1;
    data[aHash] = new (this->myAllocator) MapNode(std::forward<TheKeyType>(theKey),data[aHash]);
    Increment();
    return Standard_True;
  }

  //! Added: add a new key if not yet in the map, and return 
  //! reference to either newly added or previously existing object
  const TheKeyType& Added(const TheKeyType& theKey)
  {
    if (Resizable()) 
      ReSize(Extent());
    MapNode* aNode;
    size_t aHash;
    if (lookup(theKey, aNode, aHash))
    {
      return aNode->Key();
    }
    MapNode** data = (MapNode**)myData1;
    data[aHash] = new (this->myAllocator) MapNode(theKey,data[aHash]);
    Increment();
    return data[aHash]->Key();
  }

  //! Added: add a new key if not yet in the map, and return 
  //! reference to either newly added or previously existing object
  const TheKeyType& Added(TheKeyType&& theKey)
  {
    if (Resizable()) 
      ReSize(Extent());
    MapNode* aNode;
    size_t aHash;
    if (lookup(theKey, aNode, aHash))
    {
      return aNode->Key();
    }
    MapNode** data = (MapNode**)myData1;
    data[aHash] = new (this->myAllocator) MapNode(std::forward<TheKeyType>(theKey),data[aHash]);
    Increment();
    return data[aHash]->Key();
  }

  //! Contains
  Standard_Boolean Contains(const TheKeyType& theKey) const
  {
    MapNode* p;
    return lookup(theKey, p);
  }

  //! Remove
  Standard_Boolean Remove(const TheKeyType& K)
  {
    if (IsEmpty()) 
      return Standard_False;
    MapNode** data = (MapNode**) myData1;
    const size_t k = HashCode(K,NbBuckets());
    MapNode* p = data[k];
    MapNode* q = NULL;
    while (p) 
    {
      if (IsEqual(p->Key(),K)) 
      {
        Decrement();
        if (q) 
          q->Next() = p->Next();
        else
          data[k] = (MapNode*) p->Next();
        p->~MapNode();
        this->myAllocator->Free(p);
        return Standard_True;
      }
      q = p;
      p = (MapNode*) p->Next();
    }
    return Standard_False;
  }

  //! Clear data. If doReleaseMemory is false then the table of
  //! buckets is not released and will be reused.
  void Clear(const Standard_Boolean doReleaseMemory = Standard_False)
  { Destroy (MapNode::delNode, doReleaseMemory); }

  //! Clear data and reset allocator
  void Clear (const Handle(NCollection_BaseAllocator)& theAllocator)
  { 
    Clear(theAllocator != this->myAllocator);
    this->myAllocator = ( ! theAllocator.IsNull() ? theAllocator :
                    NCollection_BaseAllocator::CommonBaseAllocator() );
  }

  //! Destructor
  virtual ~NCollection_Map (void)
  { Clear(true); }

  //! Size
  Standard_Integer Size(void) const
  { return Extent(); }

 public:
  //!@name Boolean operations with maps as sets of keys
  //!@{

  //! @return true if two maps contains exactly the same keys
  Standard_Boolean IsEqual (const NCollection_Map& theOther) const
  {
    return Extent() == theOther.Extent()
        && Contains (theOther);
  }

  //! @return true if this map contains ALL keys of another map.
  Standard_Boolean Contains (const NCollection_Map& theOther) const
  {
    if (this == &theOther
     || theOther.IsEmpty())
    {
      return Standard_True;
    }
    else if (Extent() < theOther.Extent())
    {
      return Standard_False;
    }

    for (Iterator anIter (theOther); anIter.More(); anIter.Next())
    {
      if (!Contains (anIter.Key()))
      {
        return Standard_False;
      }
    }

    return Standard_True;
  }

  //! Sets this Map to be the result of union (aka addition, fuse, merge, boolean OR) operation between two given Maps
  //! The new Map contains the values that are contained either in the first map or in the second map or in both.
  //! All previous content of this Map is cleared.
  //! This map (result of the boolean operation) can also be passed as one of operands.
  void Union (const NCollection_Map& theLeft,
              const NCollection_Map& theRight)
  {
    if (&theLeft == &theRight)
    {
      Assign (theLeft);
      return;
    }

    if (this != &theLeft
     && this != &theRight)
    {
      Clear();
    }

    if (this != &theLeft)
    {
      for (Iterator anIter (theLeft); anIter.More(); anIter.Next())
      {
        Add (anIter.Key());
      }
    }
    if (this != &theRight)
    {
      for (Iterator anIter (theRight); anIter.More(); anIter.Next())
      {
        Add (anIter.Key());
      }
    }
  }

  //! Apply to this Map the boolean operation union (aka addition, fuse, merge, boolean OR) with another (given) Map.
  //! The result contains the values that were previously contained in this map or contained in the given (operand) map.
  //! This algorithm is similar to method Union().
  //! Returns True if contents of this map is changed.
  Standard_Boolean Unite (const NCollection_Map& theOther)
  {
    if (this == &theOther)
    {
      return Standard_False;
    }

    const Standard_Integer anOldExtent = Extent();
    Union (*this, theOther);
    return anOldExtent != Extent();
  }

  //! Returns true if this and theMap have common elements.
  Standard_Boolean HasIntersection (const NCollection_Map& theMap) const
  {
    const NCollection_Map* aMap1 = this;
    const NCollection_Map* aMap2 = &theMap;
    if (theMap.Size() < Size())
    {
      aMap1 = &theMap;
      aMap2 = this;
    }

    for (NCollection_Map::Iterator aIt(*aMap1); aIt.More(); aIt.Next())
    {
      if (aMap2->Contains(aIt.Value()))
      {
        return Standard_True;
      }
    }

    return Standard_False;
  }

  //! Sets this Map to be the result of intersection (aka multiplication, common, boolean AND) operation between two given Maps.
  //! The new Map contains only the values that are contained in both map operands.
  //! All previous content of this Map is cleared.
  //! This same map (result of the boolean operation) can also be used as one of operands.
  void Intersection (const NCollection_Map& theLeft,
                     const NCollection_Map& theRight)
  {
    if (&theLeft == &theRight)
    {
      Assign (theLeft);
      return;
    }

    if (this == &theLeft)
    {
      NCollection_Map aCopy (1, this->myAllocator);
      Exchange     (aCopy);
      Intersection (aCopy, theRight);
      return;
    }
    else if (this == &theRight)
    {
      NCollection_Map aCopy (1, this->myAllocator);
      Exchange     (aCopy);
      Intersection (theLeft, aCopy);
      return;
    }

    Clear();
    if (theLeft.Extent() < theRight.Extent())
    {
      for (Iterator anIter (theLeft); anIter.More(); anIter.Next())
      {
        if (theRight.Contains (anIter.Key()))
        {
          Add (anIter.Key());
        }
      }
    }
    else
    {
      for (Iterator anIter (theRight); anIter.More(); anIter.Next())
      {
        if (theLeft.Contains (anIter.Key()))
        {
          Add (anIter.Key());
        }
      }
    }
  }

  //! Apply to this Map the intersection operation (aka multiplication, common, boolean AND) with another (given) Map.
  //! The result contains only the values that are contained in both this and the given maps.
  //! This algorithm is similar to method Intersection().
  //! Returns True if contents of this map is changed.
  Standard_Boolean Intersect (const NCollection_Map& theOther)
  {
    if (this == &theOther
     || IsEmpty())
    {
      return Standard_False;
    }

    const Standard_Integer anOldExtent = Extent();
    Intersection (*this, theOther);
    return anOldExtent != Extent();
  }

  //! Sets this Map to be the result of subtraction (aka set-theoretic difference, relative complement,
  //! exclude, cut, boolean NOT) operation between two given Maps.
  //! The new Map contains only the values that are contained in the first map operands and not contained in the second one.
  //! All previous content of this Map is cleared.
  void Subtraction (const NCollection_Map& theLeft,
                    const NCollection_Map& theRight)
  {
    if (this == &theLeft)
    {
      Subtract (theRight);
      return;
    }
    else if (this == &theRight)
    {
      NCollection_Map aCopy (1, this->myAllocator);
      Exchange    (aCopy);
      Subtraction (theLeft, aCopy);
      return;
    }

    Assign   (theLeft);
    Subtract (theRight);
  }

  //! Apply to this Map the subtraction (aka set-theoretic difference, relative complement,
  //! exclude, cut, boolean NOT) operation with another (given) Map.
  //! The result contains only the values that were previously contained in this map and not contained in this map.
  //! This algorithm is similar to method Subtract() with two operands.
  //! Returns True if contents of this map is changed.
  Standard_Boolean Subtract (const NCollection_Map& theOther)
  {
    if (this == &theOther)
    {
      if (IsEmpty())
      {
        return Standard_False;
      }

      Clear();
      return Standard_True;
    }

    const Standard_Integer anOldExtent = Extent();
    for (Iterator anIter (theOther); anIter.More(); anIter.Next())
    {
      Remove (anIter.Key());
    }
    return anOldExtent != Extent();
  }

  //! Sets this Map to be the result of symmetric difference (aka exclusive disjunction, boolean XOR) operation between two given Maps.
  //! The new Map contains the values that are contained only in the first or the second operand maps but not in both.
  //! All previous content of this Map is cleared. This map (result of the boolean operation) can also be used as one of operands.
  void Difference (const NCollection_Map& theLeft,
                   const NCollection_Map& theRight)
  {
    if (&theLeft == &theRight)
    {
      Clear();
      return;
    }
    else if (this == &theLeft)
    {
      NCollection_Map aCopy (1, this->myAllocator);
      Exchange   (aCopy);
      Difference (aCopy, theRight);
      return;
    }
    else if (this == &theRight)
    {
      NCollection_Map aCopy (1, this->myAllocator);
      Exchange   (aCopy);
      Difference (theLeft, aCopy);
      return;
    }

    Clear();
    for (Iterator anIter (theLeft); anIter.More(); anIter.Next())
    {
      if (!theRight.Contains (anIter.Key()))
      {
        Add (anIter.Key());
      }
    }
    for (Iterator anIter (theRight); anIter.More(); anIter.Next())
    {
      if (!theLeft.Contains (anIter.Key()))
      {
        Add (anIter.Key());
      }
    }
  }

  //! Apply to this Map the symmetric difference (aka exclusive disjunction, boolean XOR) operation with another (given) Map.
  //! The result contains the values that are contained only in this or the operand map, but not in both.
  //! This algorithm is similar to method Difference().
  //! Returns True if contents of this map is changed.
  Standard_Boolean Differ (const NCollection_Map& theOther)
  {
    if (this == &theOther)
    {
      if (IsEmpty())
      {
        return Standard_False;
      }
      Clear();
      return Standard_True;
    }

    const Standard_Integer anOldExtent = Extent();
    Difference (*this, theOther);
    return anOldExtent != Extent();
  }

protected:

  //! Lookup for particular key in map.
  //! @param[in] theKey key to compute hash
  //! @param[out] theNode the detected node with equal key. Can be null.
  //! @param[out] theHash computed bounded hash code for current key.
  //! @return true if key is found
  Standard_Boolean lookup(const TheKeyType& theKey, MapNode*& theNode, size_t& theHash) const
  {
    theHash = HashCode(theKey, NbBuckets());
    if (IsEmpty())
      return Standard_False; // Not found
    for (theNode = (MapNode*)myData1[theHash];
         theNode; theNode = (MapNode*)theNode->Next())
    {
      if (IsEqual(theNode->Key(), theKey)) 
        return Standard_True;
    }
    return Standard_False; // Not found
  }

  //! Lookup for particular key in map.
  //! @param[in] theKey key to compute hash
  //! @param[out] theNode the detected node with equal key. Can be null.
  //! @return true if key is found
  Standard_Boolean lookup(const TheKeyType& theKey, MapNode*& theNode) const
  {
    if (IsEmpty())
      return Standard_False; // Not found
    for (theNode = (MapNode*)myData1[HashCode(theKey, NbBuckets())];
         theNode; theNode = (MapNode*)theNode->Next())
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
protected:

  Hasher myHasher;
};

#endif
