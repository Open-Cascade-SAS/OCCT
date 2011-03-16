// File:        NCollection_Map.hxx
// Created:     Thu Apr 23 15:02:53 2002
// Author:      Alexander KARTOMIN (akm)
//              <a-kartomin@opencascade.com>

#ifndef NCollection_Map_HeaderFile
#define NCollection_Map_HeaderFile

#include <NCollection_BaseCollection.hxx>
#include <NCollection_BaseMap.hxx>
#include <NCollection_TListNode.hxx>
#include <Standard_ImmutableObject.hxx>

#if !defined No_Exception && !defined No_Standard_NoSuchObject
#include <Standard_NoSuchObject.hxx>
#endif

#ifdef WNT
// Disable the warning "operator new unmatched by delete"
#pragma warning (push)
#pragma warning (disable:4291)
#endif

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
 *              The performance of  a Map is conditionned  by  its
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
template <class TheKeyType> class NCollection_Map 
  : public NCollection_BaseCollection<TheKeyType>,
    public NCollection_BaseMap
{
  //!   Adaptation of the TListNode to the map notations
 public:
  class MapNode : public NCollection_TListNode<TheKeyType>
  {
  public:
    //! Constructor with 'Next'
    MapNode (const TheKeyType& theKey, 
             NCollection_ListNode* theNext) :
      NCollection_TListNode<TheKeyType> (theKey, theNext) {}
    //! Key
    const TheKeyType& Key (void)
    { return this->Value(); }

  };

 public:
  //!   Implementation of the Iterator interface.
  class Iterator 
    : public NCollection_BaseCollection<TheKeyType>::Iterator,
      public NCollection_BaseMap::Iterator
  {
  public:
    //! Empty constructor
    Iterator (void) :
      NCollection_BaseMap::Iterator() {}
    //! Constructor
    Iterator (const NCollection_Map& theMap) :
      NCollection_BaseMap::Iterator(theMap) {}
    //! Query if the end of collection is reached by iterator
    virtual Standard_Boolean More(void) const
    { return PMore(); }
    //! Make a step along the collection
    virtual void Next(void)
    { PNext(); }
    //! Value inquiry
    virtual const TheKeyType& Value(void) const
    {
#if !defined No_Exception && !defined No_Standard_NoSuchObject
      if (!More())
        Standard_NoSuchObject::Raise ("NCollection_Map::Iterator::Value");  
#endif
      return ((MapNode *) myNode)->Value();
    }
    //! Value change access - denied
    virtual TheKeyType& ChangeValue(void) const
    {  
      Standard_ImmutableObject::Raise("NCollection_Map::Iterator::ChangeValue");
      return * (TheKeyType *) NULL; // For compiler
    }
    //! Key
    const TheKeyType& Key (void)
    { 
#if !defined No_Exception && !defined No_Standard_NoSuchObject
      if (!More())
        Standard_NoSuchObject::Raise ("NCollection_Map::Iterator::Key");  
#endif
      return ((MapNode *) myNode)->Value();
    }
    //! Operator new for allocating iterators
    void* operator new(size_t theSize,
                       const Handle(NCollection_BaseAllocator)& theAllocator)
    { return theAllocator->Allocate(theSize); }
  };

 public:
  // ---------- PUBLIC METHODS ------------

  //! Constructor
  NCollection_Map (const Standard_Integer NbBuckets = 1,
                   const Handle(NCollection_BaseAllocator)& theAllocator = 0L)
    : NCollection_BaseCollection<TheKeyType>(theAllocator),
      NCollection_BaseMap (NbBuckets, Standard_True) {}

  //! Copy constructor
  NCollection_Map (const NCollection_Map& theOther)
    : NCollection_BaseCollection<TheKeyType>(theOther.myAllocator),
      NCollection_BaseMap (theOther.NbBuckets(), Standard_True) 
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
      Add (anIter.Value());
  }

  //! = another map
  NCollection_Map& operator= (const NCollection_Map& theOther)
  { 
    if (this == &theOther)
      return *this;

    Clear(theOther.myAllocator);
    ReSize (theOther.Extent()-1);
    Iterator anIter(theOther);
    for (; anIter.More(); anIter.Next())
      Add (anIter.Key());
    return *this;
  }

  //! ReSize
  void ReSize (const Standard_Integer N)
  {
    MapNode** newdata = 0L;
    MapNode** dummy = 0L;
    Standard_Integer newBuck;
    if (BeginResize (N, newBuck, 
                     (NCollection_ListNode**&)newdata, 
                     (NCollection_ListNode**&)dummy,
                     this->myAllocator)) 
    {
      if (myData1) 
      {
        MapNode** olddata = (MapNode**) myData1;
        MapNode *p, *q;
        Standard_Integer i,k;
        for (i = 0; i <= NbBuckets(); i++) 
        {
          if (olddata[i]) 
          {
            p = olddata[i];
            while (p) 
            {
              k = HashCode(p->Key(),newBuck);
              q = (MapNode*) p->Next();
              p->Next() = newdata[k];
              newdata[k] = p;
              p = q;
            }
          }
        }
      }
      EndResize(N,newBuck,
                (NCollection_ListNode**&)newdata,
                (NCollection_ListNode**&)dummy,
                this->myAllocator);
    }
  }

  //! Add
  Standard_Boolean Add(const TheKeyType& K)
  {
    if (Resizable()) 
      ReSize(Extent());
    MapNode** data = (MapNode**)myData1;
    Standard_Integer k = HashCode(K,NbBuckets());
    MapNode* p = data[k];
    while (p) 
    {
      if (IsEqual(p->Key(),K))
        return Standard_False;
      p = (MapNode *) p->Next();
    }
    data[k] = new (this->myAllocator) MapNode(K,data[k]);
    Increment();
    return Standard_True;
  }

  //! Added: add a new key if not yet in the map, and return 
  //! reference to either newly added or previously existing object
  const TheKeyType& Added(const TheKeyType& K)
  {
    if (Resizable()) 
      ReSize(Extent());
    MapNode** data = (MapNode**)myData1;
    Standard_Integer k = HashCode(K,NbBuckets());
    MapNode* p = data[k];
    while (p) 
    {
      if (IsEqual(p->Key(),K))
        return p->Key();
      p = (MapNode *) p->Next();
    }
    data[k] = new (this->myAllocator) MapNode(K,data[k]);
    Increment();
    return data[k]->Key();
  }

  //! Contains
  Standard_Boolean Contains(const TheKeyType& K) const
  {
    if (IsEmpty()) 
      return Standard_False;
    MapNode** data = (MapNode**) myData1;
    MapNode*  p = data[HashCode(K,NbBuckets())];
    while (p) 
    {
      if (IsEqual(p->Key(),K)) 
        return Standard_True;
      p = (MapNode *) p->Next();
    }
    return Standard_False;
  }

  //! Remove
  Standard_Boolean Remove(const TheKeyType& K)
  {
    if (IsEmpty()) 
      return Standard_False;
    MapNode** data = (MapNode**) myData1;
    Standard_Integer k = HashCode(K,NbBuckets());
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
  void Clear(const Standard_Boolean doReleaseMemory = Standard_True)
  { Destroy (MapNode::delNode, this->myAllocator, doReleaseMemory); }

  //! Clear data and reset allocator
  void Clear (const Handle(NCollection_BaseAllocator)& theAllocator)
  { 
    Clear();
    this->myAllocator = ( ! theAllocator.IsNull() ? theAllocator :
                    NCollection_BaseAllocator::CommonBaseAllocator() );
  }

  //! Destructor
  ~NCollection_Map (void)
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

#ifdef WNT
#pragma warning (pop)
#endif

#endif
