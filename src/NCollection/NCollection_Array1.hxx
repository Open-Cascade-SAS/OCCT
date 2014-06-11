// Created on: 2002-04-15
// Created by: Alexander Kartomin (akm)
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

#ifndef NCollection_Array1_HeaderFile
#define NCollection_Array1_HeaderFile

#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfMemory.hxx>
#include <Standard_OutOfRange.hxx>

#include <NCollection_DefineAlloc.hxx>
#include <NCollection_StlIterator.hxx>

// *********************************************** Template for Array1 class

/**
* Purpose:     The class Array1 represents unidimensional arrays 
*              of fixed size known at run time. 
*              The range of the index is user defined.
*              An array1 can be constructed with a "C array".
*              This functionality is useful to call methods expecting
*              an Array1. It allows to carry the bounds inside the arrays.
*              
* Examples:    Item tab[100]; //  An example with a C array
*              Array1OfItem ttab (tab[0],1,100);
*              
*              Array1OfItem tttab (ttab(10),10,20); // a slice of ttab
*              
*              If you want to reindex an array from 1 to Length do :
*              
*              Array1 tab1(tab(tab.Lower()),1,tab.Length());
*                          
* Warning:     Programs client of such a class must be independant
*              of the range of the first element. Then, a C++ for
*              loop must be written like this
*              
*              for (i = A.Lower(); i <= A.Upper(); i++)
*              
* Changes:     In  comparison  to  TCollection  the  flag  isAllocated  was
*              renamed into myDeletable (alike in  the Array2).  For naming
*              compatibility the method IsAllocated remained in class along
*              with IsDeletable.
*/              
template <class TheItemType>
class NCollection_Array1
{
public:
  //! STL-compliant typedef for value type
  typedef TheItemType value_type;

public:
  //! Implementation of the Iterator interface.
  class Iterator
  {
  public:

    //! Empty constructor - for later Init
    Iterator (void) :
      myPtrCur (NULL),
      myPtrEnd (NULL)
    {
      //
    }

    //! Constructor with initialization
    Iterator (const NCollection_Array1& theArray, Standard_Boolean theToEnd = Standard_False) :
      myPtrEnd (const_cast<TheItemType*> (&theArray.Last() + 1))
    {
      myPtrCur = theToEnd ? myPtrEnd : const_cast<TheItemType*> (&theArray.First());
    }

    //! Initialisation
    void Init (const NCollection_Array1& theArray)
    { 
      myPtrCur = const_cast<TheItemType*> (&theArray.First());
      myPtrEnd = const_cast<TheItemType*> (&theArray.Last() + 1);
    }

    //! Assignment
    Iterator& operator= (const Iterator& theOther)
    {
      myPtrCur = theOther.myPtrCur;
      myPtrEnd = theOther.myPtrEnd;
      return *this;
    }

    //! Check end
    Standard_Boolean More (void) const
    { return myPtrCur < myPtrEnd; }
    
    //! Increment operator
    void Next (void)
    { ++myPtrCur; }

    //! Decrement operator
    void Previous()
    { --myPtrCur; }

    //! Offset operator.
    void Offset (ptrdiff_t theOffset)
    { myPtrCur += theOffset; }

    //! Difference operator.
    ptrdiff_t Differ (const Iterator& theOther) const
    { return myPtrCur - theOther.myPtrCur; }

    //! Constant value access
    const TheItemType& Value (void) const
    { return *myPtrCur; }

    //! Variable value access
    TheItemType& ChangeValue (void) const 
    { return *myPtrCur; }

    //! Performs comparison of two iterators
    Standard_Boolean IsEqual (const Iterator& theOther) const
    { return myPtrCur == theOther.myPtrCur; }

  private:
    TheItemType* myPtrCur; //!< Pointer to the current element in the array
    TheItemType* myPtrEnd; //!< Pointer to the past-the-end element in the array
  }; // End of the nested class Iterator

  //! Shorthand for a regular iterator type.
  typedef NCollection_StlIterator<std::random_access_iterator_tag, Iterator, TheItemType, false> iterator;

  //! Shorthand for a constant iterator type.
  typedef NCollection_StlIterator<std::random_access_iterator_tag, Iterator, TheItemType, true> const_iterator;

  //! Returns an iterator pointing to the first element in the array.
  iterator begin() const { return Iterator (*this, false); }

  //! Returns an iterator referring to the past-the-end element in the array.
  iterator end() const { return Iterator (*this, true); }
  
  //! Returns a const iterator pointing to the first element in the array.
  const_iterator cbegin() const { return Iterator (*this, false); }

  //! Returns a const iterator referring to the past-the-end element in the array.
  const_iterator cend() const { return Iterator (*this, true); }

 public:
  // ---------- PUBLIC METHODS ------------

  //! Constructor
  NCollection_Array1(const Standard_Integer theLower,
                     const Standard_Integer theUpper) :
                myLowerBound                             (theLower),
                myUpperBound                             (theUpper),
                myDeletable                              (Standard_True)
  {
    Standard_RangeError_Raise_if (theUpper < theLower, "NCollection_Array1::Create");
    TheItemType* pBegin = new TheItemType[Length()];
    Standard_OutOfMemory_Raise_if (!pBegin, "NCollection_Array1 : Allocation failed");

    myData = pBegin - theLower;
  }

  //! Copy constructor 
  NCollection_Array1 (const NCollection_Array1& theOther) :
    myLowerBound                                (theOther.Lower()),
    myUpperBound                                (theOther.Upper()),
    myDeletable                                 (Standard_True)
  {
    TheItemType* pBegin = new TheItemType[Length()];
    Standard_OutOfMemory_Raise_if (!pBegin, "NCollection_Array1 : Allocation failed");
    myData = pBegin - myLowerBound;

    *this = theOther;
  }

  //! C array-based constructor
  NCollection_Array1 (const TheItemType& theBegin,
                      const Standard_Integer theLower,
                      const Standard_Integer theUpper) :
    myLowerBound                                (theLower),
    myUpperBound                                (theUpper),
    myDeletable                                 (Standard_False)
  {
    Standard_RangeError_Raise_if (theUpper < theLower, "NCollection_Array1::Create");
    myData = (TheItemType *) &theBegin - theLower; 
  }

  //! Initialise the items with theValue
  void Init (const TheItemType& theValue) 
  {
    TheItemType *pCur = &myData[myLowerBound], *pEnd=&myData[myUpperBound];
    for(; pCur <= pEnd; pCur++)
      *pCur = (TheItemType&) theValue;
  }

  //! Size query
  Standard_Integer Size (void) const
  { return Length(); }
  //! Length query (the same)
  Standard_Integer Length (void) const
  { return (myUpperBound-myLowerBound+1); }

  //! Lower bound
  Standard_Integer Lower (void) const
  { return myLowerBound; }
  //! Upper bound
  Standard_Integer Upper (void) const
  { return myUpperBound; }

  //! myDeletable flag
  Standard_Boolean IsDeletable (void) const
  { return myDeletable; }

  //! IsAllocated flag - for naming compatibility
  Standard_Boolean IsAllocated (void) const
  { return myDeletable; }

  //! Assignment
  NCollection_Array1& Assign (const NCollection_Array1& theOther)
  {
    if (&theOther == this)
      return *this;
    Standard_DimensionMismatch_Raise_if (Length() != theOther.Length(), "NCollection_Array1::operator=");
    TheItemType * pMyItem        = &myData[myLowerBound];
    TheItemType * const pEndItem = &(theOther.myData)[theOther.myUpperBound];
    TheItemType * pItem          = &(theOther.myData)[theOther.myLowerBound];
    while (pItem <= pEndItem) * pMyItem ++ = * pItem ++;
    return *this; 
  }

  //! Assignment operator
  NCollection_Array1& operator= (const NCollection_Array1& theOther)
  { 
    return Assign (theOther);
  }

  //! @return first element
  const TheItemType& First() const
  {
    return myData[myLowerBound];
  }

  //! @return first element
  TheItemType& ChangeFirst()
  {
    return myData[myLowerBound];
  }

  //! @return last element
  const TheItemType& Last() const
  {
    return myData[myUpperBound];
  }

  //! @return last element
  TheItemType& ChangeLast()
  {
    return myData[myUpperBound];
  }

  //! Constant value access
  const TheItemType& Value (const Standard_Integer theIndex) const
  {
    Standard_OutOfRange_Raise_if (theIndex < myLowerBound || theIndex > myUpperBound, "NCollection_Array1::Value");
    return myData[theIndex];
  }

  //! operator() - alias to Value
  const TheItemType& operator() (const Standard_Integer theIndex) const
  { return Value (theIndex); }

  //! Variable value access
  TheItemType& ChangeValue (const Standard_Integer theIndex)
  {
    Standard_OutOfRange_Raise_if (theIndex < myLowerBound || theIndex > myUpperBound, "NCollection_Array1::ChangeValue");
    return myData[theIndex];
  }

  //! operator() - alias to ChangeValue
  TheItemType& operator() (const Standard_Integer theIndex)
  { return ChangeValue (theIndex); }

  //! Set value 
  void SetValue (const Standard_Integer theIndex,
                 const TheItemType&     theItem)
  {
    Standard_OutOfRange_Raise_if (theIndex < myLowerBound || theIndex > myUpperBound, "NCollection_Array1::SetValue");
    myData[theIndex] = theItem;
  }

  //! Destructor - releases the memory
  ~NCollection_Array1 (void)
  { if (myDeletable) delete [] &(myData[myLowerBound]); }

 protected:
  // ---------- PROTECTED FIELDS -----------
  Standard_Integer     myLowerBound;
  Standard_Integer     myUpperBound;
  Standard_Boolean     myDeletable; //!< Flag showing who allocated the array
  TheItemType*         myData;      //!< Pointer to '0'th array item
};

#endif
