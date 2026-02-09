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
  using value_type      = typename NCollection_Array1<TheItemType>::value_type;
  using size_type       = typename NCollection_Array1<TheItemType>::size_type;
  using difference_type = typename NCollection_Array1<TheItemType>::difference_type;
  using pointer         = typename NCollection_Array1<TheItemType>::pointer;
  using const_pointer   = typename NCollection_Array1<TheItemType>::const_pointer;
  using reference       = typename NCollection_Array1<TheItemType>::reference;
  using const_reference = typename NCollection_Array1<TheItemType>::const_reference;

  using iterator       = typename NCollection_Array1<TheItemType>::iterator;
  using const_iterator = typename NCollection_Array1<TheItemType>::const_iterator;

  static int BeginPosition(int theRowLower,
                           int /*theRowUpper*/,
                           int theColLower,
                           int theColUpper) noexcept
  {
    // Calculate the offset for the beginning position
    return theColLower + (theRowLower * (theColUpper - theColLower + 1));
  }

  static int LastPosition(int theRowLower,
                          int theRowUpper,
                          int theColLower,
                          int theColUpper) noexcept
  {
    return ((theRowUpper - theRowLower + 1) * (theColUpper - theColLower + 1)) + theColLower
           + (theRowLower * (theColUpper - theColLower + 1)) - 1;
  }

public:
  // ---------- PUBLIC METHODS ------------

  //! Empty constructor; should be used with caution.
  //! @sa methods Resize() and Move().
  NCollection_Array2() noexcept
      : NCollection_Array1<TheItemType>(),
        myLowerRow(1),
        mySizeRow(0),
        myLowerCol(1),
        mySizeCol(0)
  {
  }

  //! Constructor
  NCollection_Array2(const int theRowLower,
                     const int theRowUpper,
                     const int theColLower,
                     const int theColUpper)
      : NCollection_Array1<TheItemType>(
          BeginPosition(theRowLower, theRowUpper, theColLower, theColUpper),
          LastPosition(theRowLower, theRowUpper, theColLower, theColUpper)),
        myLowerRow(theRowLower),
        mySizeRow(theRowUpper - theRowLower + 1),
        myLowerCol(theColLower),
        mySizeCol(theColUpper - theColLower + 1)
  {
  }

  //! Constructor
  explicit NCollection_Array2(const allocator_type& theAlloc,
                              const int             theRowLower,
                              const int             theRowUpper,
                              const int             theColLower,
                              const int             theColUpper)
      : NCollection_Array1<TheItemType>(
          theAlloc,
          BeginPosition(theRowLower, theRowUpper, theColLower, theColUpper),
          LastPosition(theRowLower, theRowUpper, theColLower, theColUpper)),
        myLowerRow(theRowLower),
        mySizeRow(theRowUpper - theRowLower + 1),
        myLowerCol(theColLower),
        mySizeCol(theColUpper - theColLower + 1)
  {
  }

  //! Copy constructor
  NCollection_Array2(const NCollection_Array2& theOther)
      : NCollection_Array1<TheItemType>(theOther),
        myLowerRow(theOther.LowerRow()),
        mySizeRow(theOther.NbRows()),
        myLowerCol(theOther.LowerCol()),
        mySizeCol(theOther.NbColumns())
  {
  }

  //! Move constructor
  NCollection_Array2(NCollection_Array2&& theOther) noexcept
      : NCollection_Array1<TheItemType>(std::forward<NCollection_Array2>(theOther)),
        myLowerRow(theOther.LowerRow()),
        mySizeRow(theOther.NbRows()),
        myLowerCol(theOther.LowerCol()),
        mySizeCol(theOther.NbColumns())
  {
    theOther.myLowerRow = 1;
    theOther.mySizeRow  = 0;
    theOther.myLowerCol = 1;
    theOther.mySizeCol  = 0;
  }

  //! C array-based constructor
  explicit NCollection_Array2(const TheItemType& theBegin,
                              const int          theRowLower,
                              const int          theRowUpper,
                              const int          theColLower,
                              const int          theColUpper)
      : NCollection_Array1<TheItemType>(
          theBegin,
          BeginPosition(theRowLower, theRowUpper, theColLower, theColUpper),
          LastPosition(theRowLower, theRowUpper, theColLower, theColUpper)),
        myLowerRow(theRowLower),
        mySizeRow(theRowUpper - theRowLower + 1),
        myLowerCol(theColLower),
        mySizeCol(theColUpper - theColLower + 1)
  {
  }

  //! Size (number of items)
  int Size() const noexcept { return Length(); }

  //! Length (number of items)
  int Length() const noexcept { return NbRows() * NbColumns(); }

  //! Returns number of rows
  int NbRows() const noexcept { return static_cast<int>(mySizeRow); }

  //! Returns number of columns
  int NbColumns() const noexcept { return static_cast<int>(mySizeCol); }

  //! Returns length of the row, i.e. number of columns
  int RowLength() const noexcept { return NbColumns(); }

  //! Returns length of the column, i.e. number of rows
  int ColLength() const noexcept { return NbRows(); }

  //! LowerRow
  int LowerRow() const noexcept { return myLowerRow; }

  //! UpperRow
  int UpperRow() const noexcept { return myLowerRow + static_cast<int>(mySizeRow) - 1; }

  //! LowerCol
  int LowerCol() const noexcept { return myLowerCol; }

  //! UpperCol
  int UpperCol() const noexcept { return myLowerCol + static_cast<int>(mySizeCol) - 1; }

  //! Updates lower row
  void UpdateLowerRow(const int theLowerRow) noexcept { myLowerRow = theLowerRow; }

  //! Updates lower column
  void UpdateLowerCol(const int theLowerCol) noexcept { myLowerCol = theLowerCol; }

  //! Updates upper row
  void UpdateUpperRow(const int theUpperRow) noexcept
  {
    myLowerRow = myLowerRow - UpperRow() + theUpperRow;
  }

  //! Updates upper column
  void UpdateUpperCol(const int theUpperCol) noexcept
  {
    myLowerCol = myLowerCol - UpperCol() + theUpperCol;
  }

  //! Assignment
  NCollection_Array2& Assign(const NCollection_Array2& theOther)
  {
    if (&theOther == this)
    {
      return *this;
    }
    NCollection_Array1<TheItemType>::Assign(theOther);
    // Current implementation disable changing bounds by assigning
    return *this;
  }

  //! Move assignment.
  //! This array will borrow all the data from theOther.
  //! The moved object will be left uninitialized and should not be used anymore.
  NCollection_Array2& Move(NCollection_Array2&& theOther) noexcept
  {
    if (&theOther == this)
    {
      return *this;
    }
    NCollection_Array1<TheItemType>::Move(theOther);
    myLowerRow          = theOther.myLowerRow;
    mySizeRow           = theOther.mySizeRow;
    myLowerCol          = theOther.myLowerCol;
    mySizeCol           = theOther.mySizeCol;
    theOther.myLowerRow = 1;
    theOther.mySizeRow  = 0;
    theOther.myLowerCol = 1;
    theOther.mySizeCol  = 0;
    return *this;
  }

  //! Move assignment.
  //! This array will borrow all the data from theOther.
  //! The moved object will be left uninitialized and should not be used anymore.
  NCollection_Array2& Move(NCollection_Array2& theOther) noexcept
  {
    return Move(std::move(theOther));
  }

  //! Assignment operator
  NCollection_Array2& operator=(const NCollection_Array2& theOther) { return Assign(theOther); }

  //! Move assignment operator; @sa Move()
  NCollection_Array2& operator=(NCollection_Array2&& theOther) noexcept
  {
    return Move(std::forward<NCollection_Array2>(theOther));
  }

  //! Constant value access
  const_reference Value(const int theRow, const int theCol) const
  {
    const size_t aPos = (theRow - myLowerRow) * mySizeCol + (theCol - myLowerCol);
    return NCollection_Array1<TheItemType>::at(aPos);
  }

  //! operator() - alias to ChangeValue
  const_reference operator()(const int theRow, const int theCol) const
  {
    const size_t aPos = (theRow - myLowerRow) * mySizeCol + (theCol - myLowerCol);
    return NCollection_Array1<TheItemType>::at(aPos);
  }

  //! Variable value access
  reference ChangeValue(const int theRow, const int theCol)
  {
    const size_t aPos = (theRow - myLowerRow) * mySizeCol + (theCol - myLowerCol);
    return NCollection_Array1<TheItemType>::at(aPos);
  }

  //! operator() - alias to ChangeValue
  reference operator()(const int theRow, const int theCol) { return ChangeValue(theRow, theCol); }

  //! SetValue
  void SetValue(const int theRow, const int theCol, const TheItemType& theItem)
  {
    const size_t aPos = (theRow - myLowerRow) * mySizeCol + (theCol - myLowerCol);
    NCollection_Array1<TheItemType>::at(aPos) = theItem;
  }

  //! SetValue
  void SetValue(const int theRow, const int theCol, TheItemType&& theItem)
  {
    const size_t aPos = (theRow - myLowerRow) * mySizeCol + (theCol - myLowerCol);
    NCollection_Array1<TheItemType>::at(aPos) = std::forward<TheItemType>(theItem);
  }

  //! Emplace value at the specified row and column, constructing it in-place
  //! @param theRow row index at which to emplace the value
  //! @param theCol column index at which to emplace the value
  //! @param theArgs arguments forwarded to TheItemType constructor
  //! @return reference to the newly constructed item
  template <typename... Args>
  reference EmplaceValue(const int theRow, const int theCol, Args&&... theArgs)
  {
    const size_t aPos = (theRow - myLowerRow) * mySizeCol + (theCol - myLowerCol);
    Standard_OutOfRange_Raise_if(aPos >= this->mySize, "NCollection_Array2::EmplaceValue");
    pointer aPnt = this->myPointer + aPos;
    this->myAllocator.destroy(aPnt);
    this->myAllocator.construct(aPnt, std::forward<Args>(theArgs)...);
    return *aPnt;
  }

  //! Resizes the array to specified bounds.
  //! When theToCopyData is false, the array is re-allocated without preserving data.
  //! When theToCopyData is true, copies elements in linear (row-major) order.
  //! No re-allocation is done if dimensions are unchanged.
  //! @param theRowLower new lower Row of array
  //! @param theRowUpper new upper Row of array
  //! @param theColLower new lower Column of array
  //! @param theColUpper new upper Column of array
  //! @param theToCopyData flag to copy existing data into new array
  void Resize(int  theRowLower,
              int  theRowUpper,
              int  theColLower,
              int  theColUpper,
              bool theToCopyData)
  {
    if (!theToCopyData)
    {
      resizeNoData(theRowLower, theRowUpper, theColLower, theColUpper);
      return;
    }
    resizeImpl<false>(theRowLower, theRowUpper, theColLower, theColUpper);
  }

  //! Resizes the array preserving 2D element layout.
  //! When theToCopyData is false, the array is re-allocated without preserving data.
  //! When theToCopyData is true, copies min(oldNbRows,newNbRows) x min(oldNbCols,newNbCols)
  //! elements from the top-left corner of the old array to the top-left corner of the new,
  //! preserving relative (row, col) offsets from lower bounds. Trimming or growing as needed.
  //! No re-allocation is done if dimensions are unchanged.
  //! @param theRowLower new lower Row of array
  //! @param theRowUpper new upper Row of array
  //! @param theColLower new lower Column of array
  //! @param theColUpper new upper Column of array
  //! @param theToCopyData flag to copy existing data into new array
  void ResizeWithTrim(int  theRowLower,
                      int  theRowUpper,
                      int  theColLower,
                      int  theColUpper,
                      bool theToCopyData)
  {
    if (!theToCopyData)
    {
      resizeNoData(theRowLower, theRowUpper, theColLower, theColUpper);
      return;
    }
    resizeImpl<true>(theRowLower, theRowUpper, theColLower, theColUpper);
  }

protected:
  //! Resize without copying data.
  void resizeNoData(int theRowLower, int theRowUpper, int theColLower, int theColUpper)
  {
    Standard_RangeError_Raise_if(theRowUpper < theRowLower || theColUpper < theColLower,
                                 "NCollection_Array2::Resize");
    NCollection_Array1<TheItemType>::Resize(
      BeginPosition(theRowLower, theRowUpper, theColLower, theColUpper),
      LastPosition(theRowLower, theRowUpper, theColLower, theColUpper),
      false);
    mySizeRow  = theRowUpper - theRowLower + 1;
    mySizeCol  = theColUpper - theColLower + 1;
    myLowerRow = theRowLower;
    myLowerCol = theColLower;
  }

  //! Internal resize with data copy.
  //! @tparam thePreserve2D if true, copies the common sub-matrix preserving
  //!   2D element positions (row, col); if false, copies elements in linear order.
  template <bool thePreserve2D>
  void resizeImpl(int theRowLower, int theRowUpper, int theColLower, int theColUpper)
  {
    Standard_RangeError_Raise_if(theRowUpper < theRowLower || theColUpper < theColLower,
                                 "NCollection_Array2::Resize");
    const size_t aNewNbRows = theRowUpper - theRowLower + 1;
    const size_t aNewNbCols = theColUpper - theColLower + 1;
    if (mySizeRow == aNewNbRows && mySizeCol == aNewNbCols)
    {
      myLowerRow = theRowLower;
      myLowerCol = theColLower;
      NCollection_Array1<TheItemType>::UpdateLowerBound(
        BeginPosition(theRowLower, theRowUpper, theColLower, theColUpper));
      return;
    }
    if (mySizeRow == 0 || mySizeCol == 0)
    {
      resizeNoData(theRowLower, theRowUpper, theColLower, theColUpper);
      return;
    }
    const size_t aNbRowsToCopy = (std::min)(mySizeRow, aNewNbRows);
    const size_t aNbColsToCopy = (std::min)(mySizeCol, aNewNbCols);
    const size_t aOldStride    = thePreserve2D ? mySizeCol : aNbColsToCopy;

    NCollection_Array2<TheItemType> aTmpMovedCopy(std::move(*this));
    TheItemType*                    anOldPointer = &aTmpMovedCopy.ChangeFirst();
    NCollection_Array1<TheItemType>::Resize(
      BeginPosition(theRowLower, theRowUpper, theColLower, theColUpper),
      LastPosition(theRowLower, theRowUpper, theColLower, theColUpper),
      false);
    mySizeRow  = aNewNbRows;
    mySizeCol  = aNewNbCols;
    myLowerRow = theRowLower;
    myLowerCol = theColLower;
    for (size_t aRowIter = 0; aRowIter < aNbRowsToCopy; ++aRowIter)
    {
      for (size_t aColIter = 0; aColIter < aNbColsToCopy; ++aColIter)
      {
        NCollection_Array1<TheItemType>::at(aRowIter * aNewNbCols + aColIter) =
          std::move(anOldPointer[aRowIter * aOldStride + aColIter]);
      }
    }
  }

protected:
  // ---------- PROTECTED FIELDS -----------
  int    myLowerRow;
  size_t mySizeRow;
  int    myLowerCol;
  size_t mySizeCol;

  // ----------- FRIEND CLASSES ------------
  friend iterator;
  friend const_iterator;
};

#endif
