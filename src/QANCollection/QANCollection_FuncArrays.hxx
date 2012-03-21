// Created on: 2002-04-30
// Created by: Alexander KARTOMIN (akm)
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
