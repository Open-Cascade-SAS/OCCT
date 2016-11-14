// Created on: 2004-03-05
// Created by: Mikhail KUZMITCHEV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#include <QANCollection.hxx>
#include <QANCollection_Common.hxx>

#include <Draw.hxx>
#include <Draw_Interpretor.hxx>

#include <gp_Pnt.hxx>

#include <Precision.hxx>

#include <NCollection_Vector.hxx>

#define ItemType gp_Pnt
#define Key1Type Standard_Real
#define Key2Type Standard_Integer

#include <NCollection_DefineArray1.hxx>
#include <NCollection_DefineHArray1.hxx>
////////////////////////////////DEFINE_ARRAY1(QANCollection_Array1,QANCollection_BaseCol,ItemType)
////////////////////////////////DEFINE_HARRAY1(QANCollection_HArray1,QANCollection_Array1)
DEFINE_ARRAY1(QANCollection_Array1Func,QANCollection_BaseColFunc,ItemType)
DEFINE_HARRAY1(QANCollection_HArray1Func,QANCollection_Array1Func)

#include <NCollection_DefineArray2.hxx>
#include <NCollection_DefineHArray2.hxx>
////////////////////////////////DEFINE_ARRAY2(QANCollection_Array2,QANCollection_BaseCol,ItemType)
////////////////////////////////DEFINE_HARRAY2(QANCollection_HArray2,QANCollection_Array2)
DEFINE_ARRAY2(QANCollection_Array2Func,QANCollection_BaseColFunc,ItemType)
DEFINE_HARRAY2(QANCollection_HArray2Func,QANCollection_Array2Func)

#include <NCollection_DefineMap.hxx>
#include <NCollection_DefineDataMap.hxx>
#include <NCollection_DefineDoubleMap.hxx>
#include <NCollection_DefineIndexedMap.hxx>
#include <NCollection_DefineIndexedDataMap.hxx>
////////////////////////////////DEFINE_MAP(QANCollection_Map,QANCollection_Key1BaseCol,Key1Type)
////////////////////////////////DEFINE_DATAMAP(QANCollection_DataMap,QANCollection_BaseCol,Key1Type,ItemType)
////////////////////////////////DEFINE_DOUBLEMAP(QANCollection_DoubleMap,QANCollection_Key2BaseCol,Key1Type,Key2Type)
////////////////////////////////DEFINE_INDEXEDMAP(QANCollection_IndexedMap,QANCollection_Key1BaseCol,Key1Type)
////////////////////////////////DEFINE_INDEXEDDATAMAP(QANCollection_IDMap,QANCollection_BaseCol,Key1Type,ItemType)
DEFINE_MAP(QANCollection_MapFunc,QANCollection_Key1BaseColFunc,Key1Type)
DEFINE_DATAMAP(QANCollection_DataMapFunc,QANCollection_BaseColFunc,Key1Type,ItemType)
DEFINE_DOUBLEMAP(QANCollection_DoubleMapFunc,QANCollection_Key2BaseColFunc,Key1Type,Key2Type)
DEFINE_INDEXEDMAP(QANCollection_IndexedMapFunc,QANCollection_Key1BaseColFunc,Key1Type)
DEFINE_INDEXEDDATAMAP(QANCollection_IDMapFunc,QANCollection_BaseColFunc,Key1Type,ItemType)

#include <NCollection_DefineList.hxx>
////////////////////////////////DEFINE_LIST(QANCollection_List,QANCollection_BaseCol,ItemType)
DEFINE_LIST(QANCollection_ListFunc,QANCollection_BaseColFunc,ItemType)

#include <NCollection_DefineSequence.hxx>
#include <NCollection_DefineHSequence.hxx>
////////////////////////////////DEFINE_SEQUENCE(QANCollection_Sequence,QANCollection_BaseCol,ItemType)
////////////////////////////////DEFINE_HSEQUENCE(QANCollection_HSequence,QANCollection_Sequence)
DEFINE_SEQUENCE(QANCollection_SequenceFunc,QANCollection_BaseColFunc,ItemType)
DEFINE_HSEQUENCE(QANCollection_HSequenceFunc,QANCollection_SequenceFunc)

// HashCode and IsEquel must be defined for key types of maps
Standard_Integer HashCode(const gp_Pnt thePnt, int theUpper)
{
  return HashCode(thePnt.X(),theUpper);
}

Standard_Boolean IsEqual(const gp_Pnt& theP1, const gp_Pnt& theP2)
{
  return theP1.IsEqual(theP2,gp::Resolution());
}

////////////////////////////////void printCollection (QANCollection_Key1BaseCol& aColl, 
template <class Coll>
void printCollection (Coll& aColl, const char * str)
{
  printf ("%s:\n",str);
  Standard_Integer iSize = aColl.Size();
  ////////////////////////////////QANCollection_Key1BaseCol::Iterator& anIter = aColl.CreateIterator();
  typename Coll::Iterator anIter (aColl);
  if (!anIter.More())
  {
    if (iSize==0)
      printf ("   <Empty collection>\n");
    else
      printf ("Error   : empty collection has size==%d",iSize);
  }
  else
  {
    printf ("   Size==%d\n",iSize);
    for (; anIter.More(); anIter.Next())
      PrintItem(anIter.Value());
  }
}

////////////////////////////////void AssignCollection (QANCollection_BaseCol& aCollSrc,
////////////////////////////////                       QANCollection_BaseCol& aCollDst)
template <class Coll>
void AssignCollection (Coll& aCollSrc, Coll& aCollDst)
{
  printCollection (aCollSrc,"Source collection");
  aCollDst.Assign(aCollSrc);
  printCollection (aCollDst,"Target collection");
}

// ===================== Test methods of Array1 type ==========================
////////////////////////////////void TestArray1  (QANCollection_Array1&     theA1)
static void TestArray1  (QANCollection_Array1Func&     theA1)
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
static void TestArray2  (QANCollection_Array2Func&     theA2)
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

// ===================== Test methods of List type ==========================
////////////////////////////////void TestList (QANCollection_List&     theL)
static void TestList (QANCollection_ListFunc&     theL)
{
  // Extent
  Standard_Integer iExt=theL.Extent();
  Standard_Integer i;

  printf ("Info: testing List(%d)\n", iExt);
  // Append(2), Prepend(2), InsertBefore(2), InsertAfter(2), 
  // Remove, RemoveFirst, First, Last
  ItemType anItem;
  ////////////////////////////////QANCollection_List aL, aL1;
  QANCollection_ListFunc aL, aL1;
  for (i=0; i<4; i++)
  {
    Random (anItem);
    aL.Append (anItem); // #1
    aL.Append (aL1);    // #2
    Random (anItem);
    aL1.Prepend (anItem); // #3
    aL1.Prepend (aL);     // #4
    ////////////////////////////////QANCollection_List::Iterator anI(theL);
    QANCollection_ListFunc::Iterator anI(theL);
    if (anI.More())
    {
      Random (anItem);
      theL.InsertBefore (anItem, anI); // #5
      theL.InsertBefore (aL1, anI);    // #6
      Random (anItem);
      theL.InsertAfter (anItem, anI); // #7
      theL.InsertAfter (aL, anI);     // #8
      theL.Remove (anI);  // #9
      if (theL.Extent() > 0)
        theL.RemoveFirst(); // #10
    }
    else
    {
      theL.Prepend (anItem);
      PrintItem(theL.First());
      PrintItem(theL.Last());
    }
  }
  // Copy constructor + operator=
  ////////////////////////////////aL = QANCollection_List(theL);
  aL = QANCollection_ListFunc(theL);

  // Assign
  AssignCollection (theL, aL);

  // Clear
  aL.Clear();
}

// ===================== Test methods of Sequence type ========================
////////////////////////////////void TestSequence (QANCollection_Sequence& theS)
static void TestSequence (QANCollection_SequenceFunc& theS)
{
  Standard_Integer i;

  printf ("Info: testing Sequence\n");
  // Append(2)
  ItemType anItem;
  ////////////////////////////////QANCollection_Sequence aS, aS1;
  QANCollection_SequenceFunc aS, aS1;
  // Append(2), Prepend(2), InsertBefore(2), InsertAfter(2), 
  // Remove, RemoveFirst, First, Last
  for (i=0; i<4; i++)
  {
    Random (anItem);
    aS.Append (anItem); // #1
    aS.Append (aS1);    // #2
    Random (anItem);
    aS1.Prepend (anItem); // #3
    aS1.Prepend (aS);     // #4
    if (theS.Length() > 0)
    {
      Random (anItem);
      theS.InsertBefore (1, anItem); // #5
      theS.InsertBefore (2, aS1);    // #6
      Random (anItem);
      theS.InsertAfter (1, anItem); // #7
      theS.InsertAfter (2, aS);     // #8
      theS.Remove (1);  // #9
      if (theS.Length() > 0)
        theS.Remove(1); // #10
    }
    else
    {
      theS.Prepend (anItem);
      PrintItem(theS.First());
      PrintItem(theS.Last());
    }
  }

  // ()
  PrintItem(theS(1));

  // Handle, Split
  ////////////////////////////////Handle(QANCollection_HSequence) aHS = new QANCollection_HSequence(aS1);
  Handle(QANCollection_HSequenceFunc) aHS = new QANCollection_HSequenceFunc(aS1);
  theS.Split (3, aHS->ChangeSequence());

  // Assign
  AssignCollection (theS, aS);

  // Clear
  aS.Clear();
}

// ===================== Test methods of Map type =============================
////////////////////////////////void TestMap  (QANCollection_Map& theM)
static void TestMap(QANCollection_MapFunc& theM, Draw_Interpretor& theDI)
{
  {
    // Extent
    Standard_Integer iExt=theM.Extent();
    Standard_Integer i;

    printf ("Info: testing Map(l=%d)\n", iExt);
    theM.Statistics(cout);
    // Resize
    theM.ReSize(8);
    theM.Statistics(cout);
    cout.flush();
    // Constructor
    ////////////////////////////////QANCollection_Map aM;
    QANCollection_MapFunc aM;
    // Add
    Key1Type aKey;
    for (i=0; i<8; i++)
    {
      Random (aKey);
      aM.Add (aKey);
    }
    // Contains, Remove
    if (!aM.Contains(aKey))
    {
      theDI << "Error: map says that it does not contain its key " << aKey;
    }
    else
    {
      aM.Remove(aKey);
      cout << "      successfully removed item, l=%d\n" << aM.Size() << "\n";
    }
    // Copy constructor (including operator=)
    ////////////////////////////////QANCollection_Map aM2 = QANCollection_Map(aM);
    QANCollection_MapFunc aM2 = QANCollection_MapFunc(aM);
    // Assign
    AssignCollection (aM2,theM);

    // Clear
    aM.Clear();
  }

  // Check method 'HasIntersection'.
  {
    QANCollection_MapFunc aM1, aM2, aM3;

    aM1.Add(6);
    aM1.Add(8);
    aM1.Add(10);

    aM2.Add(4);
    aM2.Add(8);
    aM2.Add(16);

    aM3.Add(1);
    aM3.Add(2);
    aM3.Add(3);

    if (!aM1.HasIntersection(aM2) || !aM2.HasIntersection(aM1) ||
         aM1.HasIntersection(aM3) ||  aM3.HasIntersection(aM1))
    {
      theDI << "Error: method 'HasIntersection' failed.";
    }
  }
}

// ===================== Test methods of DataMap type =========================
////////////////////////////////void TestDataMap  (QANCollection_DataMap& theM)
static void TestDataMap  (QANCollection_DataMapFunc& theM)
{
  // Extent
  Standard_Integer iExt=theM.Extent();
  Standard_Integer i;

  printf ("Info: testing DataMap(l=%d)\n", iExt);
  theM.Statistics(cout);
  // Resize
  theM.ReSize(8);
  theM.Statistics(cout);
  cout.flush();
  // Constructor
  ////////////////////////////////QANCollection_DataMap aM;
  QANCollection_DataMapFunc aM;
  // Bind, Find, ChangeFind, ()
  Key1Type aKey;
  ItemType anItem;
  for (i=0; i<8; i++)
  {
    Random (aKey);
    Random (anItem);
    aM.Bind (aKey, anItem);
    PrintItem(aM.Find(aKey));
    Random(aM(aKey));
  }
  // IsBound, UnBind
  if (!aM.IsBound(aKey))
  {
    printf("Error   : map says that it does not contain its key ");
    PrintItem(aKey);
  }
  else
  {
    aM.UnBind(aKey);
    printf("      successfully unbound the key, l=%d\n", aM.Size());
  }
  // Copy constructor (including operator=)
  ////////////////////////////////theM = QANCollection_DataMap(aM);
  theM = QANCollection_DataMapFunc(aM);
  // Assign - prohibited
  // AssignCollection (aM2,theM);
  printCollection (theM, "DataMap:");

  // Clear
  aM.Clear();
}


// ===================== Test methods of DoubleMap type =======================
////////////////////////////////void TestDoubleMap  (QANCollection_DoubleMap& theM)
static void TestDoubleMap  (QANCollection_DoubleMapFunc& theM)
{
  // Extent
  Standard_Integer iExt=theM.Extent();
  Standard_Integer i;

  printf ("Info: testing DoubleMap(l=%d)\n", iExt);
  theM.Statistics(cout);
  // Resize
  theM.ReSize(8);
  theM.Statistics(cout);
  cout.flush();
  // Constructor
  ////////////////////////////////QANCollection_DoubleMap aM;
  QANCollection_DoubleMapFunc aM;
  // Bind, Find?, 
  Key1Type aKey1;
  Key2Type aKey2;
  for (i=0; i<8; i++)
  {
    Random (aKey1);
    Random (aKey2);
    aM.Bind (aKey1, aKey2);
    PrintItem(aM.Find1(aKey1));
    if (!aM.IsBound1(aKey1))
    {
      printf("Error   : map says that it does not contain its key ");
      PrintItem(aKey1);
    }
    PrintItem(aM.Find2(aKey2));
    if (!aM.IsBound2(aKey2))
    {
      printf("Error   : map says that it does not contain its key ");
      PrintItem(aKey2);
    }
  }
  // AreBound, UnBind
  if (!aM.AreBound(aKey1,aKey2))
  {
    printf("Error   : map says that it does not contain its keys ");
    PrintItem(aKey1);
    PrintItem(aKey2);
  }
  else
  {
    if (aM.UnBind2(aKey2))
      printf("      successfully unbound the key, l=%d\n", aM.Size());
    if (aM.UnBind1(aKey1))
      printf("Error   : unbound both keys?!\n");
  }
  // Copy constructor (including operator=)
  ////////////////////////////////theM = QANCollection_DoubleMap(aM);
  theM = QANCollection_DoubleMapFunc(aM);
  // Assign - prohibited
  // AssignCollection (aM2,theM);
  printCollection (theM, "DoubleMap:");

  // Clear
  aM.Clear();
}

// ===================== Test methods of IndexedMap type ======================
////////////////////////////////void TestIndexedMap  (QANCollection_IndexedMap& theM)
static void TestIndexedMap  (QANCollection_IndexedMapFunc& theM)
{
  // Extent
  Standard_Integer iExt=theM.Extent();
  Standard_Integer i;

  printf ("Info: testing IndexedMap(l=%d)\n", iExt);
  theM.Statistics(cout);
  // Resize
  theM.ReSize(8);
  theM.Statistics(cout);
  cout.flush();
  // Constructor
  ////////////////////////////////QANCollection_IndexedMap aM;
  QANCollection_IndexedMapFunc aM;
  // Add, FindKey, FindIndex
  Key1Type aKey;
  for (i=0; i<8; i++)
  {
    Random (aKey);
    aM.Add (aKey);
    Standard_Integer iIndex=aM.FindIndex(aKey);
    printf ("     added a key, i=%d, k=",iIndex);
    PrintItem(aM(iIndex));
  }
  // Contains, Remove
  if (!aM.Contains(aM.FindKey(aM.FindIndex(aKey))))
  {
    printf("Error   : map says that it does not contain its key ");
    PrintItem(aKey);
  }
  else
  {
    aM.RemoveLast();
    printf("      successfully removed item, l=%d\n", aM.Size());
  }
  // Substitute
  Random(aKey);
  aM.Substitute(1,aKey);
  if (!aM.Contains (aKey) || aM.FindIndex (aKey) != 1)
  {
    printf("Error   : map does not contain valid key after substitute"); 
  }
  // Invoke substitute with the same key
  aM.Substitute(1,aKey);
  if (!aM.Contains (aKey) || aM.FindIndex (aKey) != 1)
  {
    printf("Error   : map does not contain valid key after substitute"); 
  }
  // Copy constructor (including operator=)
  ////////////////////////////////QANCollection_IndexedMap aM2 = QANCollection_IndexedMap(aM);
  QANCollection_IndexedMapFunc aM2 = QANCollection_IndexedMapFunc(aM);
  // Assign
  AssignCollection (aM2,theM);

  // Clear
  aM.Clear();
}

// ===================== Test methods of IndexedDataMap type ==================
////////////////////////////////void TestIndexedDataMap  (QANCollection_IDMap& theM)
static void TestIndexedDataMap  (QANCollection_IDMapFunc& theM)
{
  // Extent
  Standard_Integer iExt=theM.Extent();
  Standard_Integer i;

  printf ("Info: testing IndexedDataMap(l=%d)\n", iExt);
  theM.Statistics(cout);
  // Resize
  theM.ReSize(8);
  theM.Statistics(cout);
  cout.flush();
  // Constructor
  ////////////////////////////////QANCollection_IDMap aM;
  QANCollection_IDMapFunc aM;
  // Add, FindKey, FindIndex, FindFromIndex, Change..., ()
  Key1Type aKey;
  ItemType anItem;
  for (i=0; i<8; i++)
  {
    Random (aKey);
    Random (anItem);
    aM.Add (aKey, anItem);
    Standard_Integer iIndex=aM.FindIndex(aKey);
    printf ("     added a key, i=%d, k=",iIndex);
    PrintItem(aM.FindKey(iIndex));
    PrintItem(aM(iIndex));
    Random(aM.ChangeFromIndex(iIndex));
  }
  // Contains, Remove, FindFromKey
  if (!aM.Contains(aM.FindKey(aM.FindIndex(aKey))))
  {
    printf("Error   : map says that it does not contain its key ");
    PrintItem(aKey);
  }
  else
  {
    anItem = aM.FindFromKey(aKey);
    aM.RemoveLast();
    printf("      successfully removed item, l=%d\n", aM.Size());
  }
  // Substitute with different keys
  Random(aKey);
  aM.Substitute (1, aKey, anItem);
  if (!aM.Contains (aKey) || aM.FindIndex (aKey) != 1 || !aM.FindFromKey (aKey).IsEqual (anItem, Precision::Confusion()))
  {
    printf("Error   : map does not contain valid key and item after substitute"); 
  }
  // Substitute with equal keys
  Random(anItem);
  aM.Substitute (1, aKey, anItem);
  if (!aM.Contains (aKey) || aM.FindIndex (aKey) != 1 || !aM.FindFromKey (aKey).IsEqual (anItem, Precision::Confusion()))
  {
    printf("Error   : map does not contain valid key and item after substitute"); 
  }
  // Copy constructor (including operator=)
  ////////////////////////////////theM = QANCollection_IDMap(aM);
  theM = QANCollection_IDMapFunc(aM);
  // Assign - prohibited
  // AssignCollection (aM2,theM);
  printCollection (theM, "DoubleMap:");

  // Clear
  aM.Clear();
}

//=======================================================================
//function : CheckArguments1
//purpose  : 
//=======================================================================
Standard_Integer CheckArguments1(Draw_Interpretor& di, Standard_Integer argc, const char ** argv, Standard_Integer& Lower, Standard_Integer& Upper)
{
  if ( argc != 3) {
    di << "Usage : " << argv[0] << " Lower Upper\n";
    return 1;
  }
  Lower = Draw::Atoi(argv[1]);
  Upper = Draw::Atoi(argv[2]);
  if ( Lower > Upper ) {
    di << "Lower > Upper\n";
    return 1;
  }
  return 0;
}

//=======================================================================
//function : CheckArguments2
//purpose  : 
//=======================================================================
Standard_Integer CheckArguments2(Draw_Interpretor& di, Standard_Integer argc, const char ** argv, Standard_Integer& LowerRow, Standard_Integer& UpperRow, Standard_Integer& LowerCol, Standard_Integer& UpperCol)
{
  if ( argc != 5) {
    di << "Usage : " << argv[0] << " LowerRow UpperRow LowerCol UpperCol\n";
    return 1;
  }
  LowerRow = Draw::Atoi(argv[1]);
  UpperRow = Draw::Atoi(argv[2]);
  LowerCol = Draw::Atoi(argv[3]);
  UpperCol = Draw::Atoi(argv[4]);
  if ( LowerRow > UpperRow ) {
    di << "LowerRow > UpperRow\n";
    return 1;
  }
  if ( LowerCol > UpperCol ) {
    di << "LowerCol UpperCol> \n";
    return 1;
  }
  return 0;
}


//=======================================================================
//function : QANColTestArray1
//purpose  : 
//=======================================================================
static Standard_Integer QANColTestArray1(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Lower, Upper;
  if ( CheckArguments1(di, argc, argv, Lower, Upper) ) {
    return 1;
  }
  QANCollection_Array1Func anArr1(Lower, Upper);
  TestArray1(anArr1);
  return 0;
}

//=======================================================================
//function : QANColTestArray2
//purpose  : 
//=======================================================================
static Standard_Integer QANColTestArray2(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer LowerRow, UpperRow, LowerCol, UpperCol;
  if ( CheckArguments2(di, argc, argv, LowerRow, UpperRow, LowerCol, UpperCol) ) {
    return 1;
  }
  QANCollection_Array2Func anArr2(LowerRow, UpperRow, LowerCol, UpperCol);
  TestArray2(anArr2);
  return 0;
}

//=======================================================================
//function : QANColTestMap
//purpose  : 
//=======================================================================
static Standard_Integer QANColTestMap(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_MapFunc aMap;
  TestMap(aMap, di);
  return 0;
}

//=======================================================================
//function : QANColTestDataMap
//purpose  : 
//=======================================================================
static Standard_Integer QANColTestDataMap(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_DataMapFunc aDataMap;
  TestDataMap(aDataMap);
  return 0;
}

//=======================================================================
//function : QANColTestDoubleMap
//purpose  : 
//=======================================================================
static Standard_Integer QANColTestDoubleMap(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_DoubleMapFunc aDoubleMap;
  TestDoubleMap(aDoubleMap);
  return 0;
}

//=======================================================================
//function : QANColTestIndexedMap
//purpose  : 
//=======================================================================
static Standard_Integer QANColTestIndexedMap(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_IndexedMapFunc aIndexedMap;
  TestIndexedMap(aIndexedMap);
  return 0;
}

//=======================================================================
//function : QANColTestIndexedDataMap
//purpose  : 
//=======================================================================
static Standard_Integer QANColTestIndexedDataMap(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_IDMapFunc aIDMap;
  TestIndexedDataMap(aIDMap);
  return 0;
}

//=======================================================================
//function : QANColTestList
//purpose  : 
//=======================================================================
static Standard_Integer QANColTestList(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_ListFunc aList;
  TestList(aList);
  return 0;
}

//=======================================================================
//function : QANColTestVector
//purpose  : 
//=======================================================================
static Standard_Integer QANColTestVector (Draw_Interpretor&, Standard_Integer, const char**)
{
  // test method Append and copying of empty vector
  NCollection_Vector<int> aVec;
  NCollection_Vector<int> aVec2 (aVec);
  NCollection_Vector<int> aVec3;
  aVec3 = aVec;

  aVec.Append(5);
  if (aVec(0) != 5)
    std::cout << "Error: wrong value in original vector!" << endl;
  aVec2.Append(5);
  if (aVec2(0) != 5)
    std::cout << "Error: wrong value in copy-constructed vector!" << endl;
  aVec3.Append(5);
  if (aVec3(0) != 5)
    std::cout << "Error: wrong value in copied vector!" << endl;
  std::cout << "Test OK" << endl;

  return 0;
}

//=======================================================================
//function : QANColTestSequence
//purpose  : 
//=======================================================================
static Standard_Integer QANColTestSequence(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_SequenceFunc aSeq;
  TestSequence(aSeq);
  return 0;
}

void QANCollection::CommandsTest(Draw_Interpretor& theCommands) {
  const char *group = "QANCollection";

  // from agvCollTest/src/CollectionEXE/FuncTestEXE.cxx
  theCommands.Add("QANColTestArray1",         "QANColTestArray1",         __FILE__, QANColTestArray1,         group);  
  theCommands.Add("QANColTestArray2",         "QANColTestArray2",         __FILE__, QANColTestArray2,         group);  
  theCommands.Add("QANColTestMap",            "QANColTestMap",            __FILE__, QANColTestMap,            group);  
  theCommands.Add("QANColTestDataMap",        "QANColTestDataMap",        __FILE__, QANColTestDataMap,        group);  
  theCommands.Add("QANColTestDoubleMap",      "QANColTestDoubleMap",      __FILE__, QANColTestDoubleMap,      group);  
  theCommands.Add("QANColTestIndexedMap",     "QANColTestIndexedMap",     __FILE__, QANColTestIndexedMap,     group);  
  theCommands.Add("QANColTestIndexedDataMap", "QANColTestIndexedDataMap", __FILE__, QANColTestIndexedDataMap, group);  
  theCommands.Add("QANColTestList",           "QANColTestList",           __FILE__, QANColTestList,           group);  
  theCommands.Add("QANColTestSequence",       "QANColTestSequence",       __FILE__, QANColTestSequence,       group);  
  theCommands.Add("QANColTestVector",         "QANColTestVector",         __FILE__, QANColTestVector,         group);  
}
