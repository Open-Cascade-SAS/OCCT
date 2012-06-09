// Created on: 2002-04-24
// Created by: Alexander GRIGORIEV
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



#ifndef NCollection_BaseVector_HeaderFile
#define NCollection_BaseVector_HeaderFile

#include <Standard_TypeDef.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <stddef.h>

#if !defined No_Exception && !defined No_Standard_OutOfRange
#include <Standard_OutOfRange.hxx>
#endif

#ifdef WNT
#pragma warning(push, 1)
#pragma warning(disable:4355)
#endif

// this value defines the number of blocks that are reserved
// when the capacity of vector is increased
inline Standard_Integer GetCapacity (const Standard_Integer theIncrement)
{
  return Max(theIncrement/8, 1);
}

/**
 *  Class NCollection_BaseVector - base for generic vector
 */
class NCollection_BaseVector
{
 public:
  // ------------ Class MemBlock ------------
  class MemBlock {
  protected:
    MemBlock (NCollection_BaseAllocator* theAlloc)
      : myAlloc(theAlloc),
        myData(NULL),
        myFirstInd(0),
        myLength(0),
        mySize(0) {}
    MemBlock (const Standard_Integer theFirstInd,
              const Standard_Integer theLength,
              NCollection_BaseAllocator* theAlloc)
      : myAlloc(theAlloc),
        myData(NULL),
        myFirstInd(theFirstInd),
        myLength(0),
        mySize(theLength) {}
    virtual void        Reinit     (const Standard_Integer,
                                    const Standard_Integer) {}
    Standard_Integer    FirstIndex () const     { return myFirstInd; }
    Standard_Integer    Size       () const     { return mySize; }
  public:
    virtual             ~MemBlock () {}
    void                SetLength  (const Standard_Integer theLen)
                                                { myLength = theLen; }
    Standard_Integer    Length     () const     { return myLength; }
    void *              Find       (const Standard_Integer theInd,
                                    const size_t           theSize) const
                                    { return ((char *) myData)+theInd*theSize;}
    Standard_EXPORT Standard_Integer
                        GetIndexV  (void * theItem, const size_t theSz) const;
  protected:
    NCollection_BaseAllocator*   myAlloc;
    void*                        myData;
    Standard_Integer             myFirstInd;
    Standard_Integer             myLength;
    Standard_Integer             mySize;
    friend class NCollection_BaseVector;
  };

  class Iterator {
  protected:
    Iterator () :
      myICurBlock (0), myIEndBlock (0), myCurIndex (0), myEndIndex (0) {}
    Iterator (const NCollection_BaseVector& theVector) { InitV (theVector); }
    Iterator (const Iterator& theVector)               { CopyV (theVector); }
    Standard_EXPORT void InitV (const NCollection_BaseVector& theVector);
    Standard_EXPORT void CopyV (const Iterator&);
    Standard_Boolean     MoreV () const
        { return (myICurBlock < myIEndBlock || myCurIndex < myEndIndex); }
    void                 NextV ()
        { if (++myCurIndex >= myVector -> myData[myICurBlock].Length() &&
              myICurBlock < myIEndBlock)
          { ++myICurBlock; myCurIndex = 0; } }
    const MemBlock *     CurBlockV () const
        { return &myVector -> myData[myICurBlock]; }

    const NCollection_BaseVector * myVector;   // the Master vector
    Standard_Integer             myICurBlock;  // # of the current block
    Standard_Integer             myIEndBlock;
    Standard_Integer             myCurIndex;   // Index in the current block
    Standard_Integer             myEndIndex;
  };

 protected:
  // ------------ Block initializer ---------
  typedef MemBlock * (* FuncPtrDataInit) (const NCollection_BaseVector&,
                                          const Standard_Integer aCapacity,
                                          const void             * aSource,
                                          const Standard_Integer aSize);
  typedef void (* FuncPtrDataFree)       (const NCollection_BaseVector&,
                                          MemBlock *);

  friend class Iterator;

  // ---------- PROTECTED METHODS ----------

  //! Empty constructor
  NCollection_BaseVector (const size_t           theSize,
                          const Standard_Integer theInc,
                          FuncPtrDataInit        theDataInit,
                          FuncPtrDataFree        theDataFree)
     : myItemSize  (theSize),
       myIncrement (theInc),
       myLength    (0),
       myCapacity  (GetCapacity(myIncrement)),
       myNBlocks   (0),
       myData      (theDataInit (* this, myCapacity, NULL, 0)),
       myDataInit  (theDataInit),
       myDataFree  (theDataFree)
  {
//    myData = (MemBlock *) new char [myCapacity * sizeof(MemBlock)];
//    for (Standard_Integer i = 0; i < myCapacity; i++)
//      new (&myData[i]) MemBlock;
  }

  //! Copy constructor
  NCollection_BaseVector (const NCollection_BaseVector& theOther,
                          FuncPtrDataInit               theDataInit,
                          FuncPtrDataFree               theDataFree)
    : myItemSize  (theOther.myItemSize),
      myIncrement (theOther.myIncrement),
      myLength    (theOther.Length()),
      myCapacity  (GetCapacity(myIncrement)+theOther.Length()/theOther.myIncrement),
      myNBlocks   (1 + (theOther.Length() - 1)/theOther.myIncrement),
      myData      (theDataInit (* this, myCapacity, NULL, 0)),
      myDataInit  (theDataInit),
      myDataFree  (theDataFree)  {}

  //! Destructor
  Standard_EXPORT ~NCollection_BaseVector ();

  //! Operator =
  Standard_EXPORT NCollection_BaseVector& operator =
                                 (const NCollection_BaseVector&);

  //! ExpandV: returns pointer to memory where to put the new item
  Standard_EXPORT void * ExpandV (const Standard_Integer theIndex);

  //! Find: locate the memory holding the desired value
  inline          void * Find    (const Standard_Integer theIndex) const;

 public:
  //! Total number of items
  Standard_Integer      Length  () const      { return myLength; }

  //! Empty the vector of its objects
  Standard_EXPORT void  Clear   ();

 protected:
  // ---------- PRIVATE FIELDS ----------
  size_t                myItemSize;
  Standard_Integer      myIncrement;
  Standard_Integer      myLength;
  Standard_Integer      myCapacity;
  Standard_Integer      myNBlocks;
  MemBlock              * myData;
  FuncPtrDataInit       myDataInit;
  FuncPtrDataFree       myDataFree;
};

//=======================================================================
//function : Find
//purpose  : locate the memory holding the desired value
//=======================================================================

inline void * NCollection_BaseVector::Find
                                        (const Standard_Integer theIndex) const
{
#if !defined No_Exception && !defined No_Standard_OutOfRange
  if (theIndex < 0 || theIndex >= myLength)
    Standard_OutOfRange::Raise ("NCollection_BaseVector::Find");
#endif
  const Standard_Integer aBlock = theIndex / myIncrement;
  return myData[aBlock].Find (theIndex - aBlock * myIncrement, myItemSize);
}

#ifdef WNT
#pragma warning(pop)
#endif

#endif
