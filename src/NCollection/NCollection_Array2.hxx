// Created on: 2002-04-15
// Created by: Alexander Kartomin (akm)
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


#ifndef NCollection_Array2_HeaderFile
#define NCollection_Array2_HeaderFile

#ifndef No_Exception
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfMemory.hxx>
#include <Standard_OutOfRange.hxx>
#endif

#include <NCollection_BaseCollection.hxx>

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
template <class TheItemType> class NCollection_Array2
  : public NCollection_BaseCollection<TheItemType>
{
 public:
  // **************** Implementation of the Iterator interface.
  class Iterator : public NCollection_BaseCollection<TheItemType>::Iterator
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
    virtual Standard_Boolean More (void) const
    { return (myCurrent < mySize); }
    //! Make step
    virtual void Next (void)
    { myCurrent++; }
    //! Constant value access
    virtual const TheItemType& Value (void) const
    { return myArray->myStart[myCurrent]; }
    //! Variable value access
    virtual TheItemType& ChangeValue (void) const
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
    NCollection_BaseCollection<TheItemType>     (),
    myLowerRow                                  (theRowLower),
    myUpperRow                                  (theRowUpper),
    myLowerCol                                  (theColLower),
    myUpperCol                                  (theColUpper),
    myDeletable                                 (Standard_True)
  { Allocate(); }

  //! Copy constructor 
  NCollection_Array2 (const NCollection_Array2& theOther) :
    NCollection_BaseCollection<TheItemType>     (),
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
    NCollection_BaseCollection<TheItemType>     (),
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
  virtual Standard_Integer Size (void) const
  { return Length(); }
  //! Length (number of items)
  Standard_Integer Length (void) const
  { return RowLength() * ColLength(); }

  //! RowLength 
  Standard_Integer RowLength (void) const
  { return (myUpperCol-myLowerCol+1); }
  //! ColLength 
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

  //! Assign 
  // Copies items from the other collection into the allocated
  // storage. Raises an exception when sizes differ.
  virtual void Assign (const NCollection_BaseCollection<TheItemType>& theOther)
  {
    if (&theOther == this)
      return;
#if !defined No_Exception && !defined No_Standard_DimensionMismatch
    if (Length() != theOther.Size())
      Standard_DimensionMismatch::Raise ("NCollection_Array2::Assign");
#endif
    TYPENAME NCollection_BaseCollection<TheItemType>::Iterator& anIter2 = 
      theOther.CreateIterator();
    const TheItemType* pEnd = myStart+Length();
    for (TheItemType* pItem=myStart;
         pItem < pEnd;
         pItem++, anIter2.Next())
      *pItem = anIter2.Value();
  }

  //! operator= (array to array)
  NCollection_Array2& operator= (const NCollection_Array2& theOther)
  { 
    if (&theOther == this)
      return *this;
#if !defined No_Exception && !defined No_Standard_DimensionMismatch
    if (Length() != theOther.Length())
      Standard_DimensionMismatch::Raise ("NCollection_Array2::operator=");
#endif
    TheItemType * pMyItem  = myStart;
    TheItemType * pItem    = theOther.myStart;
    const Standard_Integer iSize = Length();
    for (Standard_Integer i=0; i < iSize; i++, pItem++, pMyItem++)
      *pMyItem = *pItem;
    return *this; 
  }

  //! Constant value access
  const TheItemType& Value (const Standard_Integer theRow,
                            const Standard_Integer theCol) const
  {
#if !defined No_Exception && !defined No_Standard_OutOfRange
    if (theRow < myLowerRow || theRow > myUpperRow ||
        theCol < myLowerCol || theCol > myUpperCol)
      Standard_OutOfRange::Raise ("NCollection_Array2::Value");
#endif
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
#if !defined No_Exception && !defined No_Standard_OutOfRange
    if (theRow < myLowerRow || theRow > myUpperRow ||
        theCol < myLowerCol || theCol > myUpperCol)
      Standard_OutOfRange::Raise ("NCollection_Array2::ChangeValue");
#endif
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
#if !defined No_Exception && !defined No_Standard_OutOfRange
    if (theRow < myLowerRow || theRow > myUpperRow ||
        theCol < myLowerCol || theCol > myUpperCol)
      Standard_OutOfRange::Raise ("NCollection_Array2::SetValue");
#endif
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
#if !defined No_Exception && !defined No_Standard_RangeError
    if (iRowSize <= 0  || iColSize <= 0)
      Standard_RangeError::Raise ("NCollection_Array2::Allocate");
#endif
    if (myDeletable) {
      // allocation of the data in the array
      myStart = new TheItemType[iRowSize * iColSize];
#if !defined No_Exception && !defined No_Standard_OutOfMemory
      if (!myStart)
        Standard_OutOfMemory::Raise ("NCollection_Array2 : Allocation failed");
#endif
    }
    // else myStart is set to the beginning of the given array
    TheItemType** pTable = new TheItemType* [iColSize];
#if !defined No_Exception && !defined No_Standard_OutOfMemory
    if (!pTable)
      Standard_OutOfMemory::Raise ("NCollection_Array2 : Allocation failed");
#endif

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

  //! Creates Iterator for use on BaseCollection
  virtual TYPENAME NCollection_BaseCollection<TheItemType>::Iterator& 
    CreateIterator(void) const
  { return *(new (this->IterAllocator()) Iterator(*this)); }

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
