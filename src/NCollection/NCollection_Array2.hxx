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

#ifndef NCollection_Array2_HeaderFile
#define NCollection_Array2_HeaderFile

#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfMemory.hxx>
#include <Standard_OutOfRange.hxx>

#include <NCollection_DefineAlloc.hxx>

// *********************************************** Template for Array2 class
/**
* Purpose:   The class Array2 represents bi-dimensional arrays 
*            of fixed size known at run time. 
*            The ranges of indices are user defined.
*            
* Warning:   Programs clients of such class must be independant
*            of the range of the first element. Then, a C++ for
*            loop must be written like this
*            
*            for (i = A.LowerRow(); i <= A.UpperRow(); i++)
*              for (j = A.LowerCol(); j <= A.UpperCol(); j++)
*/            
template <class TheItemType>
class NCollection_Array2
{
public:
  //! STL-compliant typedef for value type
  typedef TheItemType value_type;

public:
  // **************** Implementation of the Iterator interface.
  class Iterator
  {
  public:
    //! Empty constructor - for later Init
    Iterator (void) :
      myCurrent (0),
      mySize    (0),
      myArray   (NULL) {}
    //! Constructor with initialisation
    Iterator  (const NCollection_Array2& theArray) :
      myCurrent (0),
      mySize    (theArray.Length()),
      myArray   ((NCollection_Array2 *) &theArray) {}
    //! Initialisation
    void Init (const NCollection_Array2& theArray)
    { 
      myCurrent = 0;
      mySize    = theArray.Length();
      myArray   = (NCollection_Array2 *) &theArray; 
    }
    //! Check end
    Standard_Boolean More (void) const
    { return (myCurrent < mySize); }
    //! Make step
    void Next (void)
    { myCurrent++; }
    //! Constant value access
    const TheItemType& Value (void) const
    { return myArray->myStart[myCurrent]; }
    //! Variable value access
    TheItemType& ChangeValue (void) const
    { return myArray->myStart[myCurrent]; }
  private:
    Standard_Integer    myCurrent;  //!< Index of the current item
    Standard_Integer    mySize;     //!< Total amount of items
    NCollection_Array2* myArray;    //!< Pointer to the array being iterated
  }; // End of nested class Iterator

 public:
  // ---------- PUBLIC METHODS ------------

  //! Constructor
  NCollection_Array2(const Standard_Integer theRowLower,
                     const Standard_Integer theRowUpper,
                     const Standard_Integer theColLower,
                     const Standard_Integer theColUpper) :
    myLowerRow                                  (theRowLower),
    myUpperRow                                  (theRowUpper),
    myLowerCol                                  (theColLower),
    myUpperCol                                  (theColUpper),
    myDeletable                                 (Standard_True)
  { Allocate(); }

  //! Copy constructor 
  NCollection_Array2 (const NCollection_Array2& theOther) :
    myLowerRow                                  (theOther.LowerRow()),
    myUpperRow                                  (theOther.UpperRow()),
    myLowerCol                                  (theOther.LowerCol()),
    myUpperCol                                  (theOther.UpperCol()),
    myDeletable                                 (Standard_True)
  {
    Allocate();
    *this = theOther;
  }

  //! C array-based constructor
  NCollection_Array2(const TheItemType&     theBegin,
                     const Standard_Integer theRowLower,
                     const Standard_Integer theRowUpper,
                     const Standard_Integer theColLower,
                     const Standard_Integer theColUpper) :
    myLowerRow                                  (theRowLower),
    myUpperRow                                  (theRowUpper),
    myLowerCol                                  (theColLower),
    myUpperCol                                  (theColUpper),
    myDeletable                                 (Standard_False)
  {
    myStart = (TheItemType *) &theBegin;
    Allocate();
  }

  //! Initialise the values
  void Init (const TheItemType& theValue) 
  {
    TheItemType *pCur, *pEnd=myStart+Size();
    for(pCur = myStart; pCur<pEnd; pCur++)
      *pCur = theValue;
  }

  //! Size (number of items)
  Standard_Integer Size (void) const
  { return Length(); }
  //! Length (number of items)
  Standard_Integer Length (void) const
  { return RowLength() * ColLength(); }

  //! Returns length of the row, i.e. number of columns
  Standard_Integer RowLength (void) const
  { return (myUpperCol-myLowerCol+1); }

  //! Returns length of the column, i.e. number of rows
  Standard_Integer ColLength (void) const
  { return (myUpperRow-myLowerRow+1); }

  //! LowerRow
  Standard_Integer LowerRow (void) const
  { return myLowerRow; }
  //! UpperRow
  Standard_Integer UpperRow (void) const
  { return myUpperRow; }
  //! LowerCol
  Standard_Integer LowerCol (void) const
  { return myLowerCol; }
  //! UpperCol
  Standard_Integer UpperCol (void) const
  { return myUpperCol; }

  //! myDeletable flag
  Standard_Boolean IsDeletable (void) const
  { return myDeletable; }

  //! Assignment
  NCollection_Array2& Assign (const NCollection_Array2& theOther)
  { 
    if (&theOther == this)
      return *this;
    Standard_DimensionMismatch_Raise_if (Length() != theOther.Length(), "NCollection_Array2::operator=");
    TheItemType * pMyItem  = myStart;
    TheItemType * pItem    = theOther.myStart;
    const Standard_Integer iSize = Length();
    for (Standard_Integer i=0; i < iSize; i++, pItem++, pMyItem++)
      *pMyItem = *pItem;
    return *this; 
  }

  //! Assignment operator
  NCollection_Array2& operator= (const NCollection_Array2& theOther)
  { 
    return Assign (theOther);
  }

  //! Constant value access
  const TheItemType& Value (const Standard_Integer theRow,
                            const Standard_Integer theCol) const
  {
    Standard_OutOfRange_Raise_if (theRow < myLowerRow || theRow > myUpperRow ||
                                  theCol < myLowerCol || theCol > myUpperCol, "NCollection_Array2::Value");
    return myData[theRow][theCol];
  }

  //! operator() - alias to ChangeValue
  const TheItemType& operator() (const Standard_Integer theRow,
                                 const Standard_Integer theCol) const
  { return Value (theRow,theCol); }

  //! Variable value access
  TheItemType& ChangeValue (const Standard_Integer theRow,
                            const Standard_Integer theCol)
  {
    Standard_OutOfRange_Raise_if (theRow < myLowerRow || theRow > myUpperRow ||
                                  theCol < myLowerCol || theCol > myUpperCol, "NCollection_Array2::ChangeValue");
    return myData[theRow][theCol];
  }

  //! operator() - alias to ChangeValue
  TheItemType& operator() (const Standard_Integer theRow,
                           const Standard_Integer theCol)
  { return ChangeValue (theRow,theCol); }

  //! SetValue
  void SetValue (const Standard_Integer theRow,
                 const Standard_Integer theCol,
                 const TheItemType&     theItem)
  {
    Standard_OutOfRange_Raise_if (theRow < myLowerRow || theRow > myUpperRow ||
                                  theCol < myLowerCol || theCol > myUpperCol, "NCollection_Array2::SetValue");
    myData[theRow][theCol] = theItem;
  }
  
  //! Destructor - releases the memory
  ~NCollection_Array2 (void)
  { 
    if (myDeletable) delete [] myStart;
    delete [] &(myData[myLowerRow]);
  }

 private:
  // ----------- PRIVATE METHODS -----------

  //! Allocate memory for the array, set up indirection table
  void Allocate (void)
  {
    const Standard_Integer iRowSize = myUpperCol - myLowerCol + 1;
    const Standard_Integer iColSize = myUpperRow - myLowerRow + 1;
    Standard_RangeError_Raise_if (iRowSize <= 0  || iColSize <= 0, "NCollection_Array2::Allocate");
    if (myDeletable) {
      // allocation of the data in the array
      myStart = new TheItemType[iRowSize * iColSize];
      Standard_OutOfMemory_Raise_if (!myStart, "NCollection_Array2 : Allocation failed");
    }
    // else myStart is set to the beginning of the given array
    TheItemType** pTable = new TheItemType* [iColSize];
    Standard_OutOfMemory_Raise_if (!pTable, "NCollection_Array2 : Allocation failed");

    // Items of pTable point to the '0'th items in the rows of the array
    TheItemType* pRow = myStart - myLowerCol;
    for (Standard_Integer i = 0; i < iColSize; i++) 
    {
      pTable[i] = pRow;
      pRow += iRowSize;
    }

    // Set myData to the '0'th row pointer of the pTable
    myData = pTable - myLowerRow;
  }

 protected:
  // ---------- PROTECTED FIELDS -----------
  Standard_Integer myLowerRow;
  Standard_Integer myUpperRow;
  Standard_Integer myLowerCol;
  Standard_Integer myUpperCol;

  TheItemType**    myData;      //!< Pointer to the row pointers table
  TheItemType*     myStart;     //!< Pointer to the memory array
  Standard_Boolean myDeletable; //!< Flag showing who allocated the array

  // ----------- FRIEND CLASSES ------------
 friend class Iterator;

};

#endif
