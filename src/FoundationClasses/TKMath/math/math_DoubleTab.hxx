// Created on: 1992-02-07
// Created by: Laurent PAINNOT
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _math_DoubleTab_HeaderFile
#define _math_DoubleTab_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <NCollection_Array2.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>

#include <array>
#include <utility>

class math_DoubleTab
{
  static const Standard_Integer THE_BUFFER_SIZE = 64;

public:
  DEFINE_STANDARD_ALLOC;
  DEFINE_NCOLLECTION_ALLOC;

public:
  //! Constructor for ranges [theLowerRow..theUpperRow, theLowerCol..theUpperCol]
  math_DoubleTab(const Standard_Integer theLowerRow,
                 const Standard_Integer theUpperRow,
                 const Standard_Integer theLowerCol,
                 const Standard_Integer theUpperCol)
      : myBuffer{},
        myArray(
          (theUpperRow - theLowerRow + 1) * (theUpperCol - theLowerCol + 1) <= THE_BUFFER_SIZE
            ? NCollection_Array2<Standard_Real>(*myBuffer.data(),
                                                theLowerRow,
                                                theUpperRow,
                                                theLowerCol,
                                                theUpperCol)
            : NCollection_Array2<Standard_Real>(theLowerRow, theUpperRow, theLowerCol, theUpperCol))
  {
  }

public:
  //! Constructor from external data array
  math_DoubleTab(const Standard_Address theTab,
                 const Standard_Integer theLowerRow,
                 const Standard_Integer theUpperRow,
                 const Standard_Integer theLowerCol,
                 const Standard_Integer theUpperCol)
      : myArray(*static_cast<const Standard_Real*>(theTab),
                theLowerRow,
                theUpperRow,
                theLowerCol,
                theUpperCol)
  {
  }

  //! Initialize all elements with theInitValue
  void Init(const Standard_Real theInitValue) { myArray.Init(theInitValue); }

  //! Copy constructor
  math_DoubleTab(const math_DoubleTab& theOther)
      : myArray(theOther.myArray)
  {
  }

  //! Move constructor
  math_DoubleTab(math_DoubleTab&& theOther) noexcept
      : myBuffer{},
        myArray(theOther.myArray.IsDeletable()
                  ? std::move(theOther.myArray)
                  : (theOther.NbRows() * theOther.NbColumns() <= THE_BUFFER_SIZE
                       ? NCollection_Array2<Standard_Real>(*myBuffer.data(),
                                                           theOther.LowerRow(),
                                                           theOther.UpperRow(),
                                                           theOther.LowerCol(),
                                                           theOther.UpperCol())
                       : NCollection_Array2<Standard_Real>(theOther.LowerRow(),
                                                           theOther.UpperRow(),
                                                           theOther.LowerCol(),
                                                           theOther.UpperCol())))
  {
    if (!theOther.myArray.IsEmpty())
    {
      myArray.Assign(theOther.myArray);
    }
  }

  //! Copy data to theOther
  void Copy(math_DoubleTab& theOther) const { theOther.myArray.Assign(myArray); }

  //! Returns true if the internal array is deletable (heap-allocated)
  Standard_Boolean IsDeletable() const { return myArray.IsDeletable(); }

  //! Set lower row index
  void SetLowerRow(const Standard_Integer theLowerRow) { myArray.UpdateLowerRow(theLowerRow); }

  //! Set lower column index
  void SetLowerCol(const Standard_Integer theLowerCol) { myArray.UpdateLowerCol(theLowerCol); }

  //! Get lower row index
  Standard_Integer LowerRow() const noexcept { return myArray.LowerRow(); }

  //! Get upper row index
  Standard_Integer UpperRow() const noexcept { return myArray.UpperRow(); }

  //! Get lower column index
  Standard_Integer LowerCol() const noexcept { return myArray.LowerCol(); }

  //! Get upper column index
  Standard_Integer UpperCol() const noexcept { return myArray.UpperCol(); }

  //! Get number of rows
  Standard_Integer NbRows() const noexcept { return myArray.NbRows(); }

  //! Get number of columns
  Standard_Integer NbColumns() const noexcept { return myArray.NbColumns(); }

  //! Access element at (theRowIndex, theColIndex)
  const Standard_Real& Value(const Standard_Integer theRowIndex,
                             const Standard_Integer theColIndex) const
  {
    return myArray.Value(theRowIndex, theColIndex);
  }

  //! Change element at (theRowIndex, theColIndex)
  Standard_Real& Value(const Standard_Integer theRowIndex, const Standard_Integer theColIndex)
  {
    return myArray.ChangeValue(theRowIndex, theColIndex);
  }

  //! Operator() - alias to Value
  const Standard_Real& operator()(const Standard_Integer theRowIndex,
                                  const Standard_Integer theColIndex) const
  {
    return Value(theRowIndex, theColIndex);
  }

  //! Operator() - alias to ChangeValue
  Standard_Real& operator()(const Standard_Integer theRowIndex, const Standard_Integer theColIndex)
  {
    return Value(theRowIndex, theColIndex);
  }

  //! Assignment operator
  math_DoubleTab& operator=(const math_DoubleTab& theOther)
  {
    if (this != &theOther)
    {
      myArray = theOther.myArray;
    }
    return *this;
  }

  //! Move assignment operator
  math_DoubleTab& operator=(math_DoubleTab&& theOther) noexcept
  {
    if (this == &theOther)
    {
      return *this;
    }

    if (myArray.IsDeletable() && theOther.myArray.IsDeletable()
        && myArray.NbRows() == theOther.myArray.NbRows()
        && myArray.NbColumns() == theOther.myArray.NbColumns()
        && myArray.LowerRow() == theOther.myArray.LowerRow()
        && myArray.LowerCol() == theOther.myArray.LowerCol())
    {
      myArray.Move(theOther.myArray);
    }
    else
    {
      myArray = theOther.myArray;
    }
    return *this;
  }

  //! Destructor
  ~math_DoubleTab() = default;

private:
  std::array<Standard_Real, THE_BUFFER_SIZE> myBuffer;
  NCollection_Array2<Standard_Real>          myArray;
};

#endif // _math_DoubleTab_HeaderFile
