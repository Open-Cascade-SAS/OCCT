// Copyright (c) 1997-1999 Matra Datavision
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

// Lpa, le 7/02/92

#include <math_DoubleTab.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OutOfRange.hxx>

// macro to get size of C array
#define CARRAY_LENGTH(arr) (int)(sizeof(arr)/sizeof(arr[0]))

void math_DoubleTab::Allocate()
{
  Standard_Integer RowNumber = UppR - LowR + 1;
  Standard_Integer ColNumber = UppC - LowC + 1;

  if(isAllocated) 
    Addr = (Standard_Real*) Standard::Allocate(RowNumber * ColNumber * sizeof(Standard_Real));
}

math_DoubleTab::math_DoubleTab(const Standard_Integer LowerRow,
			       const Standard_Integer UpperRow,
			       const Standard_Integer LowerCol,
			       const Standard_Integer UpperCol) :
  Addr(Buf),
  isAllocated((UpperRow - LowerRow + 1) * (UpperCol - LowerCol + 1) > CARRAY_LENGTH(Buf)),
  LowR(LowerRow),
  UppR(UpperRow),
  LowC(LowerCol),
  UppC(UpperCol)
{
  Allocate();
}

math_DoubleTab::math_DoubleTab(const Standard_Address Tab,
			       const Standard_Integer LowerRow,
			       const Standard_Integer UpperRow,
			       const Standard_Integer LowerCol,
			       const Standard_Integer UpperCol) :
  Addr(Tab),
  isAllocated(Standard_False),
  LowR(LowerRow),
  UppR(UpperRow),
  LowC(LowerCol),
  UppC(UpperCol)
{
  Allocate();
}

void math_DoubleTab::Init(const Standard_Real InitValue) 
{
  for (Standard_Integer anIndex = 0; anIndex < (UppR - LowR + 1) * (UppC - LowC + 1); anIndex++)
  {
    ((Standard_Real* )Addr)[anIndex] = InitValue;
  }
}

math_DoubleTab::math_DoubleTab(const math_DoubleTab& Other) :
  Addr(Buf),
  isAllocated((Other.UppR - Other.LowR + 1) * 
              (Other.UppC - Other.LowC + 1) > CARRAY_LENGTH(Buf)),
  LowR(Other.LowR),
  UppR(Other.UppR),
  LowC(Other.LowC),
  UppC(Other.UppC)
{
  Allocate();
  memmove (Addr, Other.Addr, (int)((UppR - LowR + 1) * (UppC - LowC + 1) * sizeof(Standard_Real)));
}

void math_DoubleTab::Free()
{
  // free the data
  if(isAllocated)
  {
    Standard::Free (Addr);
  }

  Addr = 0;
}

void math_DoubleTab::SetLowerRow(const Standard_Integer LowerRow)
{
  UppR = UppR - LowR + LowerRow;
  LowR = LowerRow;
}

void math_DoubleTab::SetLowerCol(const Standard_Integer LowerCol)
{
  UppC = UppC - LowC + LowerCol;
  LowC = LowerCol;
}

