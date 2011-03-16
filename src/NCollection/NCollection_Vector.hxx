// File:      NCollection_Vector.hxx
// Created:   23.04.02 19:24:33
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2002


#ifndef NCollection_Vector_HeaderFile
#define NCollection_Vector_HeaderFile

#include <NCollection_BaseVector.hxx>
#include <NCollection_BaseCollection.hxx>

#if !defined No_Exception && !defined No_Standard_OutOfRange
#include <Standard_OutOfRange.hxx>
#endif

#ifdef WNT
// Disable the warning: "operator new unmatched by delete"
#pragma warning (push)
#pragma warning (disable:4291)
#endif

/**
* Class NCollection_Vector (dynamic array of objects)
*
* This class is similar to NCollection_Array1  though the indices always start
* at 0 (in Array1 the first index must be specified)
*
* The Vector is always created with 0 length. It can be enlarged by two means:
*   1. Calling the method Append (val) - then "val" is added to the end of the
*      vector (the vector length is incremented)
*   2. Calling the method SetValue (i, val)  - if "i" is greater than or equal
*      to the current length of the vector,  the vector is enlarged to accomo-
*      date this index
*
* The methods Append and SetValue return  a non-const reference  to the copied
* object  inside  the vector.  This reference  is guaranteed to be valid until
* the vector is destroyed. It can be used to access the vector member directly
* or to pass its address to other data structures.
*
* The vector iterator remembers the length of the vector  at the moment of the
* creation or initialisation of the iterator.   Therefore the iteration begins
* at index 0  and stops at the index equal to (remembered_length-1).  It is OK
* to enlarge the vector during the iteration.
*/

template <class TheItemType> class NCollection_Vector
                            : public NCollection_BaseCollection<TheItemType>,
                              public NCollection_BaseVector
{
  // **************** Implementation of the Iterator interface.
 public:
  // ----------------------------------------------------------------------
  //! Nested class MemBlock
  class MemBlock : public NCollection_BaseVector::MemBlock
  {
  public:
    void * operator new (size_t, void * theAddress) { return theAddress; }
    //! Empty constructor
    MemBlock () : NCollection_BaseVector::MemBlock(0,0) {}
    //! Constructor
    MemBlock (const Standard_Integer theFirstInd,
              const Standard_Integer theSize)
      : NCollection_BaseVector::MemBlock (theFirstInd, theSize)
      { myData = new TheItemType [theSize]; }
    //! Copy constructor
    MemBlock (const MemBlock& theOther)
      : NCollection_BaseVector::MemBlock (theOther.FirstIndex(),theOther.Size())
    {
      myLength = theOther.Length();
      myData = new TheItemType [Size()];
      for (size_t i=0; i < Length(); i++)
        ((TheItemType *) myData)[i] = theOther.Value(i);
    }
    //! Reinit
    virtual void Reinit (const Standard_Integer theFirst,
                         const size_t           theSize)
    {
      if (myData) delete [] (TheItemType *) myData;
      myData = (theSize > 0) ? new TheItemType [theSize] : NULL;
      myFirstInd = theFirst;
      mySize     = theSize;
      myLength   = 0;
    }
    //! Destructor
    virtual ~MemBlock () { if (myData) delete [] (TheItemType *) myData; }
    //! Operator () const
    const TheItemType& Value (const Standard_Integer theIndex) const
    { return ((TheItemType *) myData) [theIndex]; }
    //! Operator ()
    TheItemType& ChangeValue (const Standard_Integer theIndex)
    { return ((TheItemType *) myData) [theIndex]; }
    //! GetIndex
    Standard_Integer GetIndex (const TheItemType& theItem) const {
      return GetIndexV ((void *)&theItem, sizeof(TheItemType));
    }
  }; // End of the nested class MemBlock

  // ----------------------------------------------------------------------
  // ------------------------ Nested class Iterator -----------------------
  // ----------------------------------------------------------------------
  class Iterator : public NCollection_BaseCollection<TheItemType>::Iterator,
                   public NCollection_BaseVector::Iterator
  {
  public:
    //! Empty constructor - for later Init
    Iterator  (void) {}
    //! Constructor with initialisation
    Iterator  (const NCollection_Vector& theVector) :
      NCollection_BaseVector::Iterator (theVector) {}
    //! Copy constructor
    Iterator  (const Iterator& theOther) :
      NCollection_BaseVector::Iterator (theOther) {}
    //! Initialisation
    void Init (const NCollection_Vector& theVector) { InitV (theVector); } 
    //! Assignment
    Iterator& operator =  (const Iterator& theOther) {
      CopyV (theOther);
      return * this;
    }
    //! Check end
    virtual Standard_Boolean More (void) const          { return MoreV (); }
    //! Make step
    virtual void             Next (void)                { NextV(); }
    //! Constant value access
    virtual const TheItemType& Value (void) const       {
      return ((const MemBlock *) CurBlockV()) -> Value(myCurIndex); }
    //! Variable value access
    virtual TheItemType& ChangeValue (void) const       {
      return ((MemBlock *) CurBlockV()) -> ChangeValue(myCurIndex); }
    //! Operator new for allocating iterators
    void* operator new(size_t theSize,
                       const Handle(NCollection_BaseAllocator)& theAllocator) 
    { return theAllocator->Allocate(theSize); }
  }; // End of the nested class Iterator

  // ----------------------------------------------------------------------
  // ------------------------ Class Vector itself -------------------------
  // ----------------------------------------------------------------------
 public:
  // ---------- PUBLIC METHODS ----------

  //! Constructor
  NCollection_Vector (const Standard_Integer theIncrement              = 256,
                      const Handle_NCollection_BaseAllocator& theAlloc = 0L)
    : NCollection_BaseCollection<TheItemType>(theAlloc),
      NCollection_BaseVector (sizeof(TheItemType), theIncrement,
                              FuncDataInit, FuncDataFree) {}

  //! Copy constructor
  NCollection_Vector (const NCollection_Vector& theOther)
    : NCollection_BaseCollection<TheItemType>(theOther.myAllocator),
      NCollection_BaseVector (theOther, FuncDataInit, FuncDataFree)
    { CopyData (theOther); }

  //! Operator =
  NCollection_Vector& operator = (const NCollection_Vector& theOther) {
    if (this != &theOther) {
      this->myAllocator = theOther.myAllocator;
      NCollection_BaseVector::operator = (theOther);
      CopyData (theOther);
    }
    return * this;
  }

  //! Total number of items in the vector
  virtual Standard_Integer Size () const        { return Length(); }

  //! Virtual assignment (any collection to this array)
  virtual void Assign (const NCollection_BaseCollection<TheItemType>& theOther)
  {
    if (this != &theOther) {
      TYPENAME NCollection_BaseCollection<TheItemType>::Iterator& anIter2 = 
        theOther.CreateIterator();
      while (anIter2.More()) {
        Append (anIter2.Value());
        anIter2.Next();
      }
    }
  }

  //! Assignment to the collection of the same type
  void Assign (const NCollection_Vector& theOther)
  {
    if (this != &theOther) {
      NCollection_BaseVector::operator = (theOther);
      CopyData (theOther);
    }
  }

  //! Method to create iterators for base collections
  virtual TYPENAME NCollection_BaseCollection<TheItemType>::Iterator&
    CreateIterator(void) const
  { return *(new (this->IterAllocator()) Iterator(*this)); }

  //! Append
  TheItemType& Append (const TheItemType& theValue) {
    TheItemType& anAppended = * (TheItemType *) ExpandV (myLength);
    anAppended = theValue;
    return anAppended;
  }

  //! Operator () - query the const value
  const TheItemType& operator () (const Standard_Integer theIndex) const
                                        { return Value (theIndex); }
  const TheItemType& Value (const Standard_Integer theIndex) const {
//    if (myNBlocks == 1) return ((MemBlock *) myData) -> Value(theIndex);
    return * (const TheItemType *) Find (theIndex);
  }

  //! Operator () - query the value
  TheItemType& operator () (const Standard_Integer theIndex)
                                        { return ChangeValue (theIndex); }
  TheItemType& ChangeValue (const Standard_Integer theIndex) {
//    if (myNBlocks == 1) return ((MemBlock *) myData) -> ChangeValue(theIndex);
    return * (TheItemType *) Find (theIndex);
  }

  //! SetValue () - set or append a value
  TheItemType& SetValue    (const Standard_Integer theIndex,
                            const TheItemType&     theValue) {
#if !defined No_Exception && !defined No_Standard_OutOfRange
    if (theIndex < 0)
      Standard_OutOfRange::Raise ("NCollection_Vector::SetValue");
#endif
    TheItemType * const aVecValue = 
      (TheItemType *)(theIndex<myLength? Find(theIndex): ExpandV(theIndex));
    * aVecValue = theValue;
    return * aVecValue;
  }

 private:
  // ---------- PRIVATE METHODS ----------
  void CopyData (const NCollection_Vector& theOther) {
    Standard_Integer i, iBlock = 0;
    /*NCollection_Vector::*/Iterator anIter (theOther);
    for (int aLength = 0; aLength < myLength; aLength += myIncrement) {
      MemBlock& aBlock = (MemBlock&) myData[iBlock];
      aBlock.Reinit (aLength, myIncrement);
      for (i = 0; i < myIncrement; i++) {
        if (!anIter.More()) break;
        aBlock.ChangeValue(i) = anIter.Value();
        anIter.Next();
      }
      aBlock.SetLength(i);
      iBlock++;
    }
  }

  static NCollection_BaseVector::MemBlock * FuncDataInit
                                (const NCollection_BaseVector&   theVector,
                                 const Standard_Integer          aCapacity,
                                 const void                      * aSource,
                                 const Standard_Integer          aSize)
  {
    const NCollection_Vector& aSelf =
      static_cast<const NCollection_Vector&> (theVector);
    MemBlock * aData =
      (MemBlock *) aSelf.myAllocator->Allocate(aCapacity * sizeof(MemBlock));
    Standard_Integer i = 0;
    if (aSource != NULL) {
      memcpy (aData, aSource, aSize * sizeof(MemBlock));
      i = aSize;
    }
    while (i < aCapacity)
      new (&aData[i++]) MemBlock;
    return aData;
  }

  static void FuncDataFree (const NCollection_BaseVector&      theVector,
                            NCollection_BaseVector::MemBlock * aData)
  {
    const NCollection_Vector& aSelf =
      static_cast<const NCollection_Vector&> (theVector);
    aSelf.myAllocator->Free(aData);
  }

  friend class Iterator;
};

#ifdef WNT
#pragma warning (pop)
#endif

#endif
