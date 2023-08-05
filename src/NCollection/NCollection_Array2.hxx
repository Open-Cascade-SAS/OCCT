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
#include <NCollection_Allocator.hxx>
#include <Standard_OutOfRange.hxx>
#include <NCollection_Array1.hxx>

#include <NCollection_DefineAlloc.hxx>

// *********************************************** Template for Array2 class
/**
* Purpose:   The class Array2 represents bi-dimensional arrays
*            of fixed size known at run time.
*            The ranges of indices are user defined.
*
*            Class allocates one 1D array storing full data (all Rows and Columns)
*            and extra 1D array storing pointers to each Row.
*
* Warning:   Programs clients of such class must be independent
*            of the range of the first element. Then, a C++ for
*            loop must be written like this
*
*            for (i = A.LowerRow(); i <= A.UpperRow(); i++)
*              for (j = A.LowerCol(); j <= A.UpperCol(); j++)
*/
template <class TheItemType>
class NCollection_Array2 : public NCollection_Array1<TheItemType>
{
public:
  //! Memory allocation
  DEFINE_STANDARD_ALLOC;
  DEFINE_NCOLLECTION_ALLOC;
public:
  typedef NCollection_Allocator<TheItemType> allocator_type;
public:

  // Define various type aliases for convenience
  using value_type = typename NCollection_Array1<TheItemType>::value_type;
  using size_type = typename NCollection_Array1<TheItemType>::size_type;
  using difference_type = typename NCollection_Array1<TheItemType>::difference_type;
  using pointer = typename NCollection_Array1<TheItemType>::pointer;
  using const_pointer = typename NCollection_Array1<TheItemType>::const_pointer;
  using reference = typename NCollection_Array1<TheItemType>::reference;
  using const_reference = typename NCollection_Array1<TheItemType>::const_reference;

  using iterator = typename NCollection_Array1<TheItemType>::iterator;
  using const_iterator = typename NCollection_Array1<TheItemType>::const_iterator;

  static int BeginPosition(Standard_Integer theRowLower,
                           Standard_Integer /*theRowUpper*/,
                           Standard_Integer theColLower,
                           Standard_Integer theColUpper)
  {
    // Calculate the offset for the beginning position
    return theColLower + (theRowLower * (theColUpper - theColLower + 1));
  }

  static int LastPosition(Standard_Integer theRowLower,
                          Standard_Integer theRowUpper,
                          Standard_Integer theColLower,
                          Standard_Integer theColUpper)
  {
    return ((theRowUpper - theRowLower + 1) * (theColUpper - theColLower + 1)) + theColLower + (theRowLower * (theColUpper - theColLower + 1)) - 1;
  }

public:
  // ---------- PUBLIC METHODS ------------

  //! Empty constructor; should be used with caution.
  //! @sa methods Resize() and Move().
  NCollection_Array2() :
    NCollection_Array1<TheItemType>(),
    myLowerRow(1),
    mySizeRow(0),
    myLowerCol(1),
    mySizeCol(0)
  {}

  //! Constructor
  NCollection_Array2(const Standard_Integer theRowLower,
                     const Standard_Integer theRowUpper,
                     const Standard_Integer theColLower,
                     const Standard_Integer theColUpper) :
    NCollection_Array1<TheItemType>(BeginPosition(theRowLower, theRowUpper, theColLower, theColUpper),
                                    LastPosition(theRowLower, theRowUpper, theColLower, theColUpper)),
    myLowerRow(theRowLower),
    mySizeRow(theRowUpper - theRowLower + 1),
    myLowerCol(theColLower),
    mySizeCol(theColUpper - theColLower + 1)
  {}

  //! Constructor
  explicit NCollection_Array2(const allocator_type& theAlloc,
                              const Standard_Integer theRowLower,
                              const Standard_Integer theRowUpper,
                              const Standard_Integer theColLower,
                              const Standard_Integer theColUpper) :
    NCollection_Array1<TheItemType>(theAlloc,
                                    BeginPosition(theRowLower, theRowUpper, theColLower, theColUpper),
                                    LastPosition(theRowLower, theRowUpper, theColLower, theColUpper)),
    myLowerRow(theRowLower),
    mySizeRow(theRowUpper - theRowLower + 1),
    myLowerCol(theColLower),
    mySizeCol(theColUpper - theColLower + 1)
  {}

  //! Copy constructor 
  NCollection_Array2(const NCollection_Array2& theOther) :
    NCollection_Array1<TheItemType>(theOther),
    myLowerRow(theOther.LowerRow()),
    mySizeRow(theOther.NbRows()),
    myLowerCol(theOther.LowerCol()),
    mySizeCol(theOther.NbColumns())
  {}

  //! Move constructor
  NCollection_Array2(NCollection_Array2&& theOther) noexcept:
    NCollection_Array1<TheItemType>(std::forward<NCollection_Array2>(theOther)),
    myLowerRow(theOther.LowerRow()),
    mySizeRow(theOther.NbRows()),
    myLowerCol(theOther.LowerCol()),
    mySizeCol(theOther.NbColumns())
  {}

  //! C array-based constructor
  explicit NCollection_Array2(const TheItemType& theBegin,
                              const Standard_Integer theRowLower,
                              const Standard_Integer theRowUpper,
                              const Standard_Integer theColLower,
                              const Standard_Integer theColUpper) :
    NCollection_Array1<TheItemType>(theBegin,
                                    BeginPosition(theRowLower, theRowUpper, theColLower, theColUpper),
                                    LastPosition(theRowLower, theRowUpper, theColLower, theColUpper)),
    myLowerRow(theRowLower),
    mySizeRow(theRowUpper - theRowLower + 1),
    myLowerCol(theColLower),
    mySizeCol(theColUpper - theColLower + 1)
  {}

  //! Size (number of items)
  Standard_Integer Size() const
  {
    return Length();
  }
  //! Length (number of items)
  Standard_Integer Length() const
  {
    return NbRows() * NbColumns();
  }

  //! Returns number of rows
  Standard_Integer NbRows() const
  {
    return static_cast<int>(mySizeRow);
  }

  //! Returns number of columns
  Standard_Integer NbColumns() const
  {
    return static_cast<int>(mySizeCol);
  }

  //! Returns length of the row, i.e. number of columns
  Standard_Integer RowLength() const
  {
    return NbColumns();
  }

  //! Returns length of the column, i.e. number of rows
  Standard_Integer ColLength() const
  {
    return NbRows();
  }

  //! LowerRow
  Standard_Integer LowerRow() const
  {
    return myLowerRow;
  }
  //! UpperRow
  Standard_Integer UpperRow() const
  {
    return myLowerRow + static_cast<int>(mySizeRow) - 1;
  }
  //! LowerCol
  Standard_Integer LowerCol() const
  {
    return myLowerCol;
  }
  //! UpperCol
  Standard_Integer UpperCol() const
  {
    return myLowerCol + static_cast<int>(mySizeCol) - 1;
  }

  //! Assignment
  NCollection_Array2& Assign(const NCollection_Array2& theOther)
  {
    if (&theOther == this)
    {
      return *this;
    }
    NCollection_Array1<TheItemType>::Assign(theOther);
    return *this;
  }


  //! Move assignment.
  //! This array will borrow all the data from theOther.
  //! The moved object will be left uninitialized and should not be used anymore.
  NCollection_Array2& Move(NCollection_Array2&& theOther)
  {
    if (&theOther == this)
    {
      return *this;
    }
    NCollection_Array1<TheItemType>::Move(theOther);
    myLowerRow = theOther.myLowerRow;
    mySizeRow = theOther.mySizeRow;
    myLowerCol = theOther.myLowerCol;
    mySizeCol = theOther.mySizeCol;
    return *this;
  }

  //! Move assignment.
  //! This array will borrow all the data from theOther.
  //! The moved object will be left uninitialized and should not be used anymore.
  NCollection_Array2& Move(NCollection_Array2& theOther)
  {
    return Move(std::move(theOther));
  }

  //! Assignment operator
  NCollection_Array2& operator= (const NCollection_Array2& theOther)
  {
    return Assign(theOther);
  }

  //! Move assignment operator; @sa Move()
  NCollection_Array2& operator= (NCollection_Array2&& theOther)
  {
    return Move(std::forward<NCollection_Array2>(theOther));
  }

  //! Constant value access
  const_reference Value(const Standard_Integer theRow,
                        const Standard_Integer theCol) const
  {
    const size_t aPos = (theRow - myLowerRow) * mySizeCol + (theCol - myLowerCol);
    return NCollection_Array1<TheItemType>::at(aPos);
  }

  //! operator() - alias to ChangeValue
  const_reference operator() (const Standard_Integer theRow,
                              const Standard_Integer theCol) const
  {
    const size_t aPos = (theRow - myLowerRow) * mySizeCol + (theCol - myLowerCol);
    return NCollection_Array1<TheItemType>::at(aPos);
  }

  //! Variable value access
  reference ChangeValue(const Standard_Integer theRow,
                        const Standard_Integer theCol)
  {
    const size_t aPos = (theRow - myLowerRow) * mySizeCol + (theCol - myLowerCol);
    return NCollection_Array1<TheItemType>::at(aPos);
  }

  //! operator() - alias to ChangeValue
  reference operator() (const Standard_Integer theRow,
                        const Standard_Integer theCol)
  {
    return ChangeValue(theRow, theCol);
  }

  //! SetValue
  void SetValue(const Standard_Integer theRow,
                const Standard_Integer theCol,
                const TheItemType& theItem)
  {
    const size_t aPos = (theRow - myLowerRow) * mySizeCol + (theCol - myLowerCol);
    NCollection_Array1<TheItemType>::at(aPos) = theItem;
  }

  //! SetValue
  void SetValue(const Standard_Integer theRow,
                const Standard_Integer theCol,
                TheItemType&& theItem)
  {
    const size_t aPos = (theRow - myLowerRow) * mySizeCol + (theCol - myLowerCol);
    NCollection_Array1<TheItemType>::at(aPos) = std::forward<TheItemType>(theItem);
  }

  //! Resizes the array to specified bounds.
  //! No re-allocation will be done if length of array does not change,
  //! but existing values will not be discarded if theToCopyData set to FALSE.
  //! @param theRowLower new lower Row of array
  //! @param theRowUpper new upper Row of array
  //! @param theColLower new lower Column of array
  //! @param theColUpper new upper Column of array
  //! @param theToCopyData flag to copy existing data into new array
  void Resize(Standard_Integer theRowLower,
              Standard_Integer theRowUpper,
              Standard_Integer theColLower,
              Standard_Integer theColUpper,
              Standard_Boolean theToCopyData)
  {
    Standard_RangeError_Raise_if(theRowUpper < theRowLower ||
                                 theColUpper < theColLower, "NCollection_Array2::Resize");
    myLowerRow = theRowLower;
    myLowerCol = theColLower;
    if (!theToCopyData)
    {
      NCollection_Array1<TheItemType>::Resize(BeginPosition(theRowLower, theRowUpper, theColLower, theColUpper),
                                              LastPosition(theRowLower, theRowUpper, theColLower, theColUpper),
                                              false);
      mySizeRow = theRowUpper - theRowLower + 1;
      mySizeCol = theColUpper - theColLower + 1;
      return;
    }
    NCollection_Array1<TheItemType> aTmpMovedCopy(std::move(*this));
    TheItemType* anOldPointer = &aTmpMovedCopy.ChangeFirst();
    NCollection_Array1<TheItemType>::Resize(BeginPosition(theRowLower, theRowUpper, theColLower, theColUpper),
                                            LastPosition(theRowLower, theRowUpper, theColLower, theColUpper),
                                            false);
    const size_t aNewNbRows = theRowUpper - theRowLower + 1;
    const size_t aNewNbCols = theColUpper - theColLower + 1;
    const size_t aNbRowsToCopy = std::min<size_t>(mySizeRow, aNewNbRows);
    const size_t aNbColsToCopy = std::min<size_t>(mySizeCol, aNewNbCols);
    mySizeRow = aNewNbRows;
    mySizeCol = aNewNbCols;
    size_t aOldInter = 0;
    for (size_t aRowIter = 0; aRowIter < aNbRowsToCopy; ++aRowIter)
    {
      for (size_t aColIter = 0; aColIter < aNbColsToCopy; ++aColIter)
      {
        NCollection_Array1<TheItemType>::at(aRowIter * aNewNbCols + aColIter) = std::move(anOldPointer[aOldInter++]);
      }
    }
  }

protected:
  // ---------- PROTECTED FIELDS -----------
  Standard_Integer myLowerRow;
  size_t mySizeRow;
  Standard_Integer myLowerCol;
  size_t mySizeCol;

  // ----------- FRIEND CLASSES ------------
  friend iterator;
  friend const_iterator;

};

#endif
