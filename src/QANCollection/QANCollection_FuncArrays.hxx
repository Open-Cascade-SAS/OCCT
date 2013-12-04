// Created on: 2002-04-30
// Created by: Alexander KARTOMIN (akm)
// Copyright (c) 2002-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef QANCollection_FuncArrays_HeaderFile
#define QANCollection_FuncArrays_HeaderFile

// ===================== Test methods of Array1 type ==========================
////////////////////////////////void TestArray1  (QANCollection_Array1&     theA1)
void TestArray1  (QANCollection_Array1Func&     theA1)
{
  // Bounds
  Standard_Integer iLow=theA1.Lower();
  Standard_Integer iUpp=theA1.Upper();
  Standard_Integer i;

  printf ("Info: testing Array1(%d,%d), %s\n",
          iLow, iUpp, (theA1.IsDeletable()?"deletable":"frozen"));
  // C-array constructor, Length, Init
  ItemType anItem;
  Random(anItem);
  theA1.Init (anItem);
  ItemType * rBlock = new ItemType[theA1.Length()];
  ////////////////////////////////QANCollection_Array1 aCArr(*rBlock, iLow-100, iUpp-100);
  QANCollection_Array1Func aCArr(*rBlock, iLow-100, iUpp-100);
  printf ("      created the same sized preallocated array (%d,%d), %s\n",
        aCArr.Lower(),aCArr.Upper(),(aCArr.IsDeletable()?"deletable":"frozen"));
  // *Value, operator()
  for (i=iLow+1; i<iUpp; i++)
  {
    Random (aCArr.ChangeValue (i-101));
    aCArr.SetValue (i-100, ItemType(aCArr.Value(i-101)));
    aCArr(i-99) = aCArr(i-100) = aCArr(i-101);
  }
  // Handle, copy constructor (including operator=)
  ////////////////////////////////Handle(QANCollection_HArray1) aHa = new QANCollection_HArray1(aCArr);
  Handle(QANCollection_HArray1Func) aHa = new QANCollection_HArray1Func(aCArr);
  // Assign
  AssignCollection (aHa->ChangeArray1(), theA1);
}

// ===================== Test methods of Array2 type ==========================
////////////////////////////////void TestArray2  (QANCollection_Array2&     theA2)
void TestArray2  (QANCollection_Array2Func&     theA2)
{
  // Bounds
  Standard_Integer iLR=theA2.LowerRow(), iLC=theA2.LowerCol();
  Standard_Integer iUR=theA2.UpperRow(), iUC=theA2.UpperCol();
  Standard_Integer i,j;

  printf ("Info: testing Array2 (%d,%d)(%d,%d), %s\n",
          iLR, iUR, iLC, iUC, (theA2.IsDeletable()?"deletable":"frozen"));
  // C-array constructor, Length, Init, RowLength, ColLength
  ItemType anItem;
  Random(anItem);
  theA2.Init (anItem);
  ItemType * rBlock = new ItemType[theA2.Length()];
  ////////////////////////////////QANCollection_Array2 aCArr(*rBlock, iLR-100, iUR-100, iLC, iUC);
  QANCollection_Array2Func aCArr(*rBlock, iLR-100, iUR-100, iLC, iUC);
  printf ("      created the same sized preallocated array (%d*%d), %s\n",
          aCArr.RowLength(), aCArr.ColLength(),
          (aCArr.IsDeletable()?"deletable":"frozen"));
  // *Value, operator()
  for (i=iLR+1; i<iUR; i++)
  {
    for (j=iLC; j<=iUC; j++)
    {
      Random (aCArr.ChangeValue (i-101, j));
      aCArr.SetValue (i-100, j, 
                      ItemType(aCArr.Value(i-101,j)));
      aCArr(i-99,j) = aCArr(i-100,j) = aCArr(i-101,j);
    }
  }
  // Handle, copy constructor (including operator=)
  ////////////////////////////////Handle(QANCollection_HArray2) aHa = new QANCollection_HArray2(aCArr);
  Handle(QANCollection_HArray2Func) aHa = new QANCollection_HArray2Func(aCArr);
  // Assign
  AssignCollection (aHa->ChangeArray2(), theA2);
}

#endif
