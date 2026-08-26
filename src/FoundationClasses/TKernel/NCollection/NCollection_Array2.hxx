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
#include <Standard_RangeError.hxx>
#include <NCollection_Array1.hxx>

#include <NCollection_DefineAlloc.hxx>

#include <algorithm>
#include <cstddef>
#include <utility>

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
 *
 * Zero-based (size_t) construction mode:
 *   NCollection_Array2(size_t theNbRows, size_t theNbCols) creates a zero-based array
 *   (LowerRow()==0, LowerCol()==0). In this mode At()/ChangeAt() and STL iterators are
 *   the preferred access path -- they address elements directly without any offset subtraction.
 *   Buffer-reuse variant NCollection_Array2(pointer, size_t, size_t) wraps an existing
 *   flat row-major buffer and does NOT own the memory.
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
  using value_type      = TheItemType;
  using size_type       = size_t;
  using difference_type = std::ptrdiff_t;
  using pointer         = TheItemType*;
  using const_pointer   = const TheItemType*;
  using reference       = TheItemType&;
  using const_reference = const TheItemType&;

  using iterator       = pointer;
  using const_iterator = const_pointer;

  static size_t ArraySize(const size_t theNbRows, const size_t theNbCols)
  {
    return theNbRows * theNbCols;
  }

  static size_t RangeSize(const int theLower, const int theUpper)
  {
    return static_cast<size_t>(theUpper - theLower + 1);
  }

  static int UpperForSize(const int theLower, const size_t theSize)
  {
    return theLower + static_cast<int>(theSize) - 1;
  }

private:
  size_t offsetOf(const int theRow, const int theCol, [[maybe_unused]] const char* theMessage) const
  {
    const std::ptrdiff_t aRowOffset =
      static_cast<std::ptrdiff_t>(theRow) - static_cast<std::ptrdiff_t>(myLowerRow);
    const std::ptrdiff_t aColOffset =
      static_cast<std::ptrdiff_t>(theCol) - static_cast<std::ptrdiff_t>(myLowerCol);
    Standard_OutOfRange_Raise_if(aRowOffset < 0 || aColOffset < 0
                                   || static_cast<size_t>(aRowOffset) >= mySizeRow
                                   || static_cast<size_t>(aColOffset) >= mySizeCol,
                                 theMessage);
    return static_cast<size_t>(aRowOffset) * mySizeCol + static_cast<size_t>(aColOffset);
  }

public:
  iterator begin() noexcept { return NCollection_Array1<TheItemType>::begin(); }

  const_iterator begin() const noexcept { return NCollection_Array1<TheItemType>::begin(); }

  const_iterator cbegin() const noexcept { return NCollection_Array1<TheItemType>::cbegin(); }

  iterator end() noexcept { return NCollection_Array1<TheItemType>::end(); }

  const_iterator end() const noexcept { return NCollection_Array1<TheItemType>::end(); }

  const_iterator cend() const noexcept { return NCollection_Array1<TheItemType>::cend(); }

  //! Return the flat storage pointer.
  pointer Data() noexcept { return NCollection_Array1<TheItemType>::Data(); }

  //! Return the flat storage pointer.
  const_pointer Data() const noexcept { return NCollection_Array1<TheItemType>::Data(); }

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
        mySizeRow(RangeSize(theRowLower, theRowUpper)),
        myLowerCol(theColLower),
        mySizeCol(RangeSize(theColLower, theColUpper))
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
      : NCollection_Array1<TheItemType>(std::move(theOther)),
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
          LastPosition(theRowLower, theRowUpper, theColLower, theColUpper),
          true),
        myLowerRow(theRowLower),
        mySizeRow(RangeSize(theRowLower, theRowUpper)),
        myLowerCol(theColLower),
        mySizeCol(RangeSize(theColLower, theColUpper))
  {
  }

  //! Zero-based constructor: allocates theNbRows x theNbCols elements with lower bounds 0.
  //! Use At()/ChangeAt() or STL iterators for optimal access (no offset subtraction).
  explicit NCollection_Array2(const size_t theNbRows, const size_t theNbCols)
      : NCollection_Array1<TheItemType>(ArraySize(theNbRows, theNbCols)),
        myLowerRow(0),
        mySizeRow(theNbRows),
        myLowerCol(0),
        mySizeCol(theNbCols)
  {
  }

  //! Zero-based buffer-reuse constructor: wraps an existing flat row-major C array.
  //! The array does NOT own the buffer and will NOT free it on destruction.
  //! Use At()/ChangeAt() or STL iterators for optimal access (no offset subtraction).
  explicit NCollection_Array2(pointer theBegin, const size_t theNbRows, const size_t theNbCols)
      : NCollection_Array1<TheItemType>(theBegin, ArraySize(theNbRows, theNbCols)),
        myLowerRow(0),
        mySizeRow(theNbRows),
        myLowerCol(0),
        mySizeCol(theNbCols)
  {
  }

  //! Size (number of items).
  size_t Size() const noexcept { return NCollection_Array1<TheItemType>::Size(); }

  //! Returns true when the array has no items.
  bool IsEmpty() const noexcept { return NCollection_Array1<TheItemType>::IsEmpty(); }

  //! Length (legacy int-returning API).
  int Length() const noexcept { return NbRows() * NbColumns(); }

  //! Returns number of rows
  int NbRows() const noexcept { return static_cast<int>(mySizeRow); }

  //! Returns number of rows as an unsigned size.
  size_t RowSize() const noexcept { return mySizeRow; }

  //! Returns number of columns
  int NbColumns() const noexcept { return static_cast<int>(mySizeCol); }

  //! Returns number of columns as an unsigned size.
  size_t ColSize() const noexcept { return mySizeCol; }

  //! Returns length of the row, i.e. number of columns
  int RowLength() const noexcept { return NbColumns(); }

  //! Returns length of the column, i.e. number of rows
  int ColLength() const noexcept { return NbRows(); }

  //! Return the explicit one-dimensional view of the row-major storage.
  NCollection_Array1<TheItemType>& ChangeArray1() noexcept
  {
    return static_cast<NCollection_Array1<TheItemType>&>(*this);
  }

  //! Return the explicit read-only one-dimensional view of the row-major storage.
  const NCollection_Array1<TheItemType>& Array1() const noexcept
  {
    return static_cast<const NCollection_Array1<TheItemType>&>(*this);
  }

  //! Initialize all elements.
  void Init(const_reference theValue) { NCollection_Array1<TheItemType>::Init(theValue); }

  //! Return the first flat element.
  const_reference First() const noexcept { return NCollection_Array1<TheItemType>::First(); }

  //! Return the first flat element.
  reference ChangeFirst() noexcept { return NCollection_Array1<TheItemType>::ChangeFirst(); }

  //! Return the last flat element.
  const_reference Last() const noexcept { return NCollection_Array1<TheItemType>::Last(); }

  //! Return the last flat element.
  reference ChangeLast() noexcept { return NCollection_Array1<TheItemType>::ChangeLast(); }

  //! Return whether the flat view owns its storage.
  bool IsDeletable() const noexcept { return NCollection_Array1<TheItemType>::IsDeletable(); }

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

  //! Replaces this array by a copy of theOther array.
  //! Row and column bounds are copied from theOther.
  NCollection_Array2& Assign(const NCollection_Array2& theOther)
  {
    if (&theOther == this)
    {
      return *this;
    }
    NCollection_Array1<TheItemType>::Assign(theOther);
    myLowerRow = theOther.myLowerRow;
    mySizeRow  = theOther.mySizeRow;
    myLowerCol = theOther.myLowerCol;
    mySizeCol  = theOther.mySizeCol;
    return *this;
  }

  //! Copies values from theOther array without changing this array bounds.
  //! This array should be pre-allocated and have the same dimensions as theOther;
  //! otherwise exception Standard_DimensionMismatch is thrown.
  NCollection_Array2& CopyValues(const NCollection_Array2& theOther)
  {
    if (&theOther == this)
    {
      return *this;
    }
    Standard_DimensionMismatch_Raise_if(mySizeRow != theOther.mySizeRow
                                          || mySizeCol != theOther.mySizeCol,
                                        "NCollection_Array2::CopyValues");
    NCollection_Array1<TheItemType>::CopyValues(theOther);
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
    NCollection_Array1<TheItemType>::Move(std::move(theOther));
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
    return Data()[offsetOf(theRow, theCol, "NCollection_Array2::Value")];
  }

  //! operator() - alias to ChangeValue
  const_reference operator()(const int theRow, const int theCol) const
  {
    return Data()[offsetOf(theRow, theCol, "NCollection_Array2::operator()")];
  }

  //! Variable value access
  reference ChangeValue(const int theRow, const int theCol)
  {
    return Data()[offsetOf(theRow, theCol, "NCollection_Array2::ChangeValue")];
  }

  //! operator() - alias to ChangeValue
  reference operator()(const int theRow, const int theCol) { return ChangeValue(theRow, theCol); }

  //! SetValue
  void SetValue(const int theRow, const int theCol, const TheItemType& theItem)
  {
    Data()[offsetOf(theRow, theCol, "NCollection_Array2::SetValue")] = theItem;
  }

  //! SetValue
  void SetValue(const int theRow, const int theCol, TheItemType&& theItem)
  {
    Data()[offsetOf(theRow, theCol, "NCollection_Array2::SetValue")] =
      std::forward<TheItemType>(theItem);
  }

  //! 0-based checked access to the flat row-major storage.
  const_reference At(const size_t theIndex) const
  {
    return NCollection_Array1<TheItemType>::At(theIndex);
  }

  //! 0-based checked mutable access to the flat row-major storage.
  reference ChangeAt(const size_t theIndex)
  {
    return NCollection_Array1<TheItemType>::ChangeAt(theIndex);
  }

  //! 0-based checked access independent of LowerRow()/LowerCol().
  //! @param[in] theRow 0-based row index in [0, NbRows()-1]
  //! @param[in] theCol 0-based column index in [0, NbColumns()-1]
  const_reference At(const size_t theRow, const size_t theCol) const
  {
    Standard_OutOfRange_Raise_if(theRow >= mySizeRow || theCol >= mySizeCol,
                                 "NCollection_Array2::At");
    return Data()[theRow * mySizeCol + theCol];
  }

  //! 0-based checked mutable access independent of LowerRow()/LowerCol().
  //! @param[in] theRow 0-based row index in [0, NbRows()-1]
  //! @param[in] theCol 0-based column index in [0, NbColumns()-1]
  reference ChangeAt(const size_t theRow, const size_t theCol)
  {
    Standard_OutOfRange_Raise_if(theRow >= mySizeRow || theCol >= mySizeCol,
                                 "NCollection_Array2::ChangeAt");
    return Data()[theRow * mySizeCol + theCol];
  }

  //! Emplace value at the specified row and column, constructing it in-place
  //! @param theRow row index at which to emplace the value
  //! @param theCol column index at which to emplace the value
  //! @param theArgs arguments forwarded to TheItemType constructor
  //! @return reference to the newly constructed item
  template <typename... Args>
  reference EmplaceValue(const int theRow, const int theCol, Args&&... theArgs)
  {
    const size_t aPos = offsetOf(theRow, theCol, "NCollection_Array2::EmplaceValue");
    NCollection_Array1<TheItemType>::ChangeAt(aPos) = value_type(std::forward<Args>(theArgs)...);
    return NCollection_Array1<TheItemType>::ChangeAt(aPos);
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

  //! Zero-based Resize: resizes to theNbRows x theNbCols, keeping lower bounds unchanged.
  //! No re-allocation is done if dimensions are unchanged.
  //! @param theNbRows new number of rows
  //! @param theNbCols new number of columns
  //! @param theToCopyData flag to copy existing data into new array
  void Resize(const size_t theNbRows, const size_t theNbCols, const bool theToCopyData)
  {
    if (theNbRows == 0 || theNbCols == 0)
    {
      NCollection_Array1<TheItemType>::Resize(0, false);
      mySizeRow = theNbRows;
      mySizeCol = theNbCols;
      return;
    }
    Resize(myLowerRow,
           UpperForSize(myLowerRow, theNbRows),
           myLowerCol,
           UpperForSize(myLowerCol, theNbCols),
           theToCopyData);
  }

  //! Zero-based ResizeWithTrim: resizes preserving 2D layout, keeping lower bounds unchanged.
  //! No re-allocation is done if dimensions are unchanged.
  //! @param theNbRows new number of rows
  //! @param theNbCols new number of columns
  //! @param theToCopyData flag to copy existing data into new array
  void ResizeWithTrim(const size_t theNbRows, const size_t theNbCols, const bool theToCopyData)
  {
    if (theNbRows == 0 || theNbCols == 0)
    {
      NCollection_Array1<TheItemType>::Resize(0, false);
      mySizeRow = theNbRows;
      mySizeCol = theNbCols;
      return;
    }
    ResizeWithTrim(myLowerRow,
                   UpperForSize(myLowerRow, theNbRows),
                   myLowerCol,
                   UpperForSize(myLowerCol, theNbCols),
                   theToCopyData);
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
    mySizeRow  = RangeSize(theRowLower, theRowUpper);
    mySizeCol  = RangeSize(theColLower, theColUpper);
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
    const size_t aNewNbRows = RangeSize(theRowLower, theRowUpper);
    const size_t aNewNbCols = RangeSize(theColLower, theColUpper);
    if (mySizeRow == aNewNbRows && mySizeCol == aNewNbCols)
    {
      NCollection_Array1<TheItemType>::UpdateLowerBound(
        BeginPosition(theRowLower, theRowUpper, theColLower, theColUpper));
      myLowerRow = theRowLower;
      myLowerCol = theColLower;
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
        NCollection_Array1<TheItemType>::ChangeAt(aRowIter * aNewNbCols + aColIter) =
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
};

#endif
