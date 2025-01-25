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

#include <OSD_Path.hxx>
#include <Precision.hxx>
#include <Standard_Overflow.hxx>

#include <NCollection_Vector.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_Array2.hxx>

#define ItemType gp_Pnt
#define Key1Type Standard_Real
#define Key2Type Standard_Integer

#include <NCollection_DefineHArray1.hxx>
#define DEFINE_ARRAY1(_ClassName_, _BaseCollection_, TheItemType)                                  \
  typedef NCollection_Array1<TheItemType> _ClassName_;
////////////////////////////////DEFINE_ARRAY1(QANCollection_Array1,QANCollection_BaseCol,ItemType)
////////////////////////////////DEFINE_HARRAY1(QANCollection_HArray1,QANCollection_Array1)
DEFINE_ARRAY1(QANCollection_Array1Func, QANCollection_BaseColFunc, ItemType)
DEFINE_HARRAY1(QANCollection_HArray1Func, QANCollection_Array1Func)

#include <NCollection_DefineHArray2.hxx>
#define DEFINE_ARRAY2(_ClassName_, _BaseCollection_, TheItemType)                                  \
  typedef NCollection_Array2<TheItemType> _ClassName_;
////////////////////////////////DEFINE_ARRAY2(QANCollection_Array2,QANCollection_BaseCol,ItemType)
////////////////////////////////DEFINE_HARRAY2(QANCollection_HArray2,QANCollection_Array2)
DEFINE_ARRAY2(QANCollection_Array2Func, QANCollection_BaseColFunc, ItemType)
DEFINE_HARRAY2(QANCollection_HArray2Func, QANCollection_Array2Func)

////////////////////////////////DEFINE_MAP(QANCollection_Map,QANCollection_Key1BaseCol,Key1Type)
////////////////////////////////DEFINE_DATAMAP(QANCollection_DataMap,QANCollection_BaseCol,Key1Type,ItemType)
////////////////////////////////DEFINE_DOUBLEMAP(QANCollection_DoubleMap,QANCollection_Key2BaseCol,Key1Type,Key2Type)
////////////////////////////////DEFINE_INDEXEDMAP(QANCollection_IndexedMap,QANCollection_Key1BaseCol,Key1Type)
////////////////////////////////DEFINE_INDEXEDDATAMAP(QANCollection_IDMap,QANCollection_BaseCol,Key1Type,ItemType)
#include <NCollection_DoubleMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_IndexedDataMap.hxx>
#define DEFINE_DATAMAP(_ClassName_, _BaseCollection_, TheKeyType, TheItemType)                     \
  typedef NCollection_DataMap<TheKeyType, TheItemType> _ClassName_;
#define DEFINE_DOUBLEMAP(_ClassName_, _BaseCollection_, TheKey1Type, TheKey2Type)                  \
  typedef NCollection_DoubleMap<TheKey1Type, TheKey2Type> _ClassName_;
#define DEFINE_INDEXEDDATAMAP(_ClassName_, _BaseCollection_, TheKeyType, TheItemType)              \
  typedef NCollection_IndexedDataMap<TheKeyType, TheItemType> _ClassName_;
#define DEFINE_INDEXEDMAP(_ClassName_, _BaseCollection_, TheKeyType)                               \
  typedef NCollection_IndexedMap<TheKeyType> _ClassName_;
#define DEFINE_MAP(_ClassName_, _BaseCollection_, TheKeyType)                                      \
  typedef NCollection_Map<TheKeyType> _ClassName_;
DEFINE_MAP(QANCollection_MapFunc, QANCollection_Key1BaseColFunc, Key1Type)
DEFINE_DATAMAP(QANCollection_DataMapFunc, QANCollection_BaseColFunc, Key1Type, ItemType)
DEFINE_DOUBLEMAP(QANCollection_DoubleMapFunc, QANCollection_Key2BaseColFunc, Key1Type, Key2Type)
DEFINE_INDEXEDMAP(QANCollection_IndexedMapFunc, QANCollection_Key1BaseColFunc, Key1Type)
DEFINE_INDEXEDDATAMAP(QANCollection_IDMapFunc, QANCollection_BaseColFunc, Key1Type, ItemType)

////////////////////////////////DEFINE_LIST(QANCollection_List,QANCollection_BaseCol,ItemType)
#define DEFINE_LIST(_ClassName_, _BaseCollection_, TheItemType)                                    \
  typedef NCollection_List<TheItemType> _ClassName_;
DEFINE_LIST(QANCollection_ListPerf, QANCollection_BaseColPerf, ItemType)
DEFINE_LIST(QANCollection_ListFunc, QANCollection_BaseColFunc, ItemType)

#include <NCollection_DefineHSequence.hxx>
#include <NCollection_Sequence.hxx>
#define DEFINE_SEQUENCE(_ClassName_, _BaseCollection_, TheItemType)                                \
  typedef NCollection_Sequence<TheItemType> _ClassName_;
////////////////////////////////DEFINE_SEQUENCE(QANCollection_Sequence,QANCollection_BaseCol,ItemType)
////////////////////////////////DEFINE_HSEQUENCE(QANCollection_HSequence,QANCollection_Sequence)
DEFINE_SEQUENCE(QANCollection_SequenceFunc, QANCollection_BaseColFunc, ItemType)
DEFINE_HSEQUENCE(QANCollection_HSequenceFunc, QANCollection_SequenceFunc)

////////////////////////////////void printCollection (QANCollection_Key1BaseCol& aColl,
template <class Coll>
void printCollection(Coll& aColl, const char* str)
{
  printf("%s:\n", str);
  Standard_Integer iSize = aColl.Size();
  ////////////////////////////////QANCollection_Key1BaseCol::Iterator& anIter =
  /// aColl.CreateIterator();
  typename Coll::Iterator anIter(aColl);
  if (!anIter.More())
  {
    if (iSize == 0)
      printf("   <Empty collection>\n");
    else
      printf("Error   : empty collection has size==%d", iSize);
  }
  else
  {
    printf("   Size==%d\n", iSize);
    for (; anIter.More(); anIter.Next())
      PrintItem(anIter.Value());
  }
}

////////////////////////////////void AssignCollection (QANCollection_BaseCol& aCollSrc,
////////////////////////////////                       QANCollection_BaseCol& aCollDst)
template <class Coll>
void AssignCollection(Coll& aCollSrc, Coll& aCollDst)
{
  printCollection(aCollSrc, "Source collection");
  aCollDst.Assign(aCollSrc);
  printCollection(aCollDst, "Target collection");
}

// ===================== Test methods of Array1 type ==========================
////////////////////////////////void TestArray1  (QANCollection_Array1&     theA1)
static void TestArray1(QANCollection_Array1Func& theA1)
{
  // Bounds
  Standard_Integer iLow = theA1.Lower();
  Standard_Integer iUpp = theA1.Upper();
  Standard_Integer i;

  printf("Info: testing Array1(%d,%d), %s\n",
         iLow,
         iUpp,
         (theA1.IsDeletable() ? "deletable" : "frozen"));
  // C-array constructor, Length, Init
  ItemType anItem;
  Random(anItem);
  theA1.Init(anItem);
  ItemType* rBlock = new ItemType[theA1.Length()];
  ////////////////////////////////QANCollection_Array1 aCArr(*rBlock, iLow-100, iUpp-100);
  QANCollection_Array1Func aCArr(*rBlock, iLow - 100, iUpp - 100);
  printf("      created the same sized preallocated array (%d,%d), %s\n",
         aCArr.Lower(),
         aCArr.Upper(),
         (aCArr.IsDeletable() ? "deletable" : "frozen"));
  // *Value, operator()
  for (i = iLow + 1; i < iUpp; i++)
  {
    Random(aCArr.ChangeValue(i - 101));
    aCArr.SetValue(i - 100, ItemType(aCArr.Value(i - 101)));
    aCArr(i - 99) = aCArr(i - 100) = aCArr(i - 101);
  }
  // Handle, copy constructor (including operator=)
  ////////////////////////////////Handle(QANCollection_HArray1) aHa = new
  /// QANCollection_HArray1(aCArr);
  Handle(QANCollection_HArray1Func) aHa = new QANCollection_HArray1Func(aCArr);
  // Assign
  AssignCollection(aHa->ChangeArray1(), theA1);
}

// ===================== Test methods of Array2 type ==========================
////////////////////////////////void TestArray2  (QANCollection_Array2&     theA2)
static void TestArray2(QANCollection_Array2Func& theA2)
{
  // Bounds
  Standard_Integer iLR = theA2.LowerRow(), iLC = theA2.LowerCol();
  Standard_Integer iUR = theA2.UpperRow(), iUC = theA2.UpperCol();
  Standard_Integer i, j;

  printf("Info: testing Array2 (%d,%d)(%d,%d), %s\n",
         iLR,
         iUR,
         iLC,
         iUC,
         (theA2.IsDeletable() ? "deletable" : "frozen"));
  // C-array constructor, Length, Init, RowLength, ColLength
  ItemType anItem;
  Random(anItem);
  theA2.Init(anItem);
  ItemType* rBlock = new ItemType[theA2.Length()];
  ////////////////////////////////QANCollection_Array2 aCArr(*rBlock, iLR-100, iUR-100, iLC, iUC);
  QANCollection_Array2Func aCArr(*rBlock, iLR - 100, iUR - 100, iLC, iUC);
  printf("      created the same sized preallocated array (%d*%d), %s\n",
         aCArr.RowLength(),
         aCArr.ColLength(),
         (aCArr.IsDeletable() ? "deletable" : "frozen"));
  // *Value, operator()
  for (i = iLR + 1; i < iUR; i++)
  {
    for (j = iLC; j <= iUC; j++)
    {
      Random(aCArr.ChangeValue(i - 101, j));
      aCArr.SetValue(i - 100, j, ItemType(aCArr.Value(i - 101, j)));
      aCArr(i - 99, j) = aCArr(i - 100, j) = aCArr(i - 101, j);
    }
  }
  // Handle, copy constructor (including operator=)
  ////////////////////////////////Handle(QANCollection_HArray2) aHa = new
  /// QANCollection_HArray2(aCArr);
  Handle(QANCollection_HArray2Func) aHa = new QANCollection_HArray2Func(aCArr);
  // Assign
  AssignCollection(aHa->ChangeArray2(), theA2);

  delete[] rBlock;
}

// ===================== Test methods of List type ==========================
////////////////////////////////void TestList (QANCollection_List&     theL)
static void TestList(QANCollection_ListFunc& theL)
{
  // Extent
  Standard_Integer iExt = theL.Extent();
  Standard_Integer i;

  printf("Info: testing List(%d)\n", iExt);
  // Append(2), Prepend(2), InsertBefore(2), InsertAfter(2),
  // Remove, RemoveFirst, First, Last
  ItemType anItem;
  ////////////////////////////////QANCollection_List aL, aL1;
  QANCollection_ListFunc aL, aL1;
  for (i = 0; i < 4; i++)
  {
    Random(anItem);
    aL.Append(anItem); // #1
    aL.Append(aL1);    // #2
    Random(anItem);
    aL1.Prepend(anItem); // #3
    aL1.Prepend(aL);     // #4
    ////////////////////////////////QANCollection_List::Iterator anI(theL);
    QANCollection_ListFunc::Iterator anI(theL);
    if (anI.More())
    {
      Random(anItem);
      theL.InsertBefore(anItem, anI); // #5
      theL.InsertBefore(aL1, anI);    // #6
      Random(anItem);
      theL.InsertAfter(anItem, anI); // #7
      theL.InsertAfter(aL, anI);     // #8
      theL.Remove(anI);              // #9
      if (theL.Extent() > 0)
        theL.RemoveFirst(); // #10
    }
    else
    {
      theL.Prepend(anItem);
      PrintItem(theL.First());
      PrintItem(theL.Last());
    }
  }
  // Copy constructor + operator=
  ////////////////////////////////aL = QANCollection_List(theL);
  aL = QANCollection_ListFunc(theL);

  // Assign
  AssignCollection(theL, aL);

  // Different allocators
  {
    // The joining of list having different allocator can cause memory error
    // if the fact of different allocator is not taken into account.
    Handle(NCollection_IncAllocator) anAlloc = new NCollection_IncAllocator;
    QANCollection_ListFunc           aS2(anAlloc);
    aS2.Append(anItem);
    theL.Prepend(aS2);
    aS2.Append(anItem);
    theL.Append(aS2);
    aS2.Append(anItem);
    QANCollection_ListFunc::Iterator anIter(theL);
    theL.InsertBefore(aS2, anIter);
    aS2.Append(anItem);
    theL.InsertAfter(aS2, anIter);
  }

  // Clear
  aL.Clear();
}

// ===================== Test methods of Sequence type ========================
////////////////////////////////void TestSequence (QANCollection_Sequence& theS)
static void TestSequence(QANCollection_SequenceFunc& theS)
{
  Standard_Integer i;

  printf("Info: testing Sequence\n");
  // Append(2)
  ItemType anItem;
  ////////////////////////////////QANCollection_Sequence aS, aS1;
  QANCollection_SequenceFunc aS, aS1;
  // Append(2), Prepend(2), InsertBefore(2), InsertAfter(2),
  // Remove, RemoveFirst, First, Last
  for (i = 0; i < 4; i++)
  {
    Random(anItem);
    aS.Append(anItem); // #1
    aS.Append(aS1);    // #2
    Random(anItem);
    aS1.Prepend(anItem); // #3
    aS1.Prepend(aS);     // #4
    if (theS.Length() > 0)
    {
      Random(anItem);
      theS.InsertBefore(1, anItem); // #5
      theS.InsertBefore(2, aS1);    // #6
      Random(anItem);
      theS.InsertAfter(1, anItem); // #7
      theS.InsertAfter(2, aS);     // #8
      theS.Remove(1);              // #9
      if (theS.Length() > 0)
        theS.Remove(1); // #10
    }
    else
    {
      theS.Prepend(anItem);
      PrintItem(theS.First());
      PrintItem(theS.Last());
    }
  }

  // ()
  PrintItem(theS(1));

  // Handle, Split
  ////////////////////////////////Handle(QANCollection_HSequence) aHS = new
  /// QANCollection_HSequence(aS1);
  Handle(QANCollection_HSequenceFunc) aHS = new QANCollection_HSequenceFunc(aS1);
  theS.Split(3, aHS->ChangeSequence());

  // Assign
  AssignCollection(theS, aS);

  // Different allocators
  {
    // The joining of sequence having different allocator can cause memory error
    // if the fact of different allocator is not taken into account.
    Handle(NCollection_IncAllocator) anAlloc = new NCollection_IncAllocator;
    QANCollection_SequenceFunc       aS2(anAlloc);
    aS2.Append(anItem);
    theS.Prepend(aS2);
    aS2.Append(anItem);
    theS.Append(aS2);
    aS2.Append(anItem);
    theS.InsertBefore(1, aS2);
    aS2.Append(anItem);
    theS.InsertAfter(1, aS2);
  }

  // Clear
  aS.Clear();
}

// ===================== Test methods of Map type =============================
////////////////////////////////void TestMap  (QANCollection_Map& theM)
static void TestMap(QANCollection_MapFunc& theM, Draw_Interpretor& theDI)
{
  {
    // Extent
    Standard_Integer iExt = theM.Extent();
    Standard_Integer i;

    printf("Info: testing Map(l=%d)\n", iExt);
    theM.Statistics(std::cout);
    // Resize
    theM.ReSize(8);
    theM.Statistics(std::cout);
    std::cout.flush();
    // Constructor
    ////////////////////////////////QANCollection_Map aM;
    QANCollection_MapFunc aM;
    // Add
    Key1Type aKey;
    for (i = 0; i < 8; i++)
    {
      Random(aKey);
      aM.Add(aKey);
    }
    // Contains, Remove
    if (!aM.Contains(aKey))
    {
      theDI << "Error: map says that it does not contain its key " << aKey;
    }
    else
    {
      aM.Remove(aKey);
      std::cout << "      successfully removed item, l=%d\n" << aM.Size() << "\n";
    }
    // Copy constructor (including operator=)
    ////////////////////////////////QANCollection_Map aM2 = QANCollection_Map(aM);
    QANCollection_MapFunc aM2 = QANCollection_MapFunc(aM);
    // Assign
    AssignCollection(aM2, theM);

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

    if (!NCollection_MapAlgo::HasIntersection(aM1, aM2)
        || !NCollection_MapAlgo::HasIntersection(aM2, aM1)
        || NCollection_MapAlgo::HasIntersection(aM1, aM3)
        || NCollection_MapAlgo::HasIntersection(aM3, aM1))
    {
      theDI << "Error: method 'HasIntersection' failed.";
    }
  }
}

// ===================== Test methods of DataMap type =========================
////////////////////////////////void TestDataMap  (QANCollection_DataMap& theM)
static void TestDataMap(QANCollection_DataMapFunc& theM)
{
  // Extent
  Standard_Integer iExt = theM.Extent();
  Standard_Integer i;

  printf("Info: testing DataMap(l=%d)\n", iExt);
  theM.Statistics(std::cout);
  // Resize
  theM.ReSize(8);
  theM.Statistics(std::cout);
  std::cout.flush();
  // Constructor
  ////////////////////////////////QANCollection_DataMap aM;
  QANCollection_DataMapFunc aM;
  // Bind, Find, ChangeFind, ()
  Key1Type aKey;
  ItemType anItem;
  for (i = 0; i < 8; i++)
  {
    Random(aKey);
    Random(anItem);
    aM.Bind(aKey, anItem);
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
  printCollection(theM, "DataMap:");

  // Clear
  aM.Clear();
}

// ===================== Test methods of DoubleMap type =======================
////////////////////////////////void TestDoubleMap  (QANCollection_DoubleMap& theM)
static void TestDoubleMap(QANCollection_DoubleMapFunc& theM)
{
  // Extent
  Standard_Integer iExt = theM.Extent();
  Standard_Integer i;

  printf("Info: testing DoubleMap(l=%d)\n", iExt);
  theM.Statistics(std::cout);
  // Resize
  theM.ReSize(8);
  theM.Statistics(std::cout);
  std::cout.flush();
  // Constructor
  ////////////////////////////////QANCollection_DoubleMap aM;
  QANCollection_DoubleMapFunc aM;
  // Bind, Find?,
  Key1Type aKey1;
  Key2Type aKey2;
  for (i = 0; i < 8; i++)
  {
    Random(aKey1);
    Random(aKey2);
    aM.Bind(aKey1, aKey2);
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
  if (!aM.AreBound(aKey1, aKey2))
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
  printCollection(theM, "DoubleMap:");

  // Clear
  aM.Clear();
}

// ===================== Test methods of IndexedMap type ======================
////////////////////////////////void TestIndexedMap  (QANCollection_IndexedMap& theM)
static void TestIndexedMap(QANCollection_IndexedMapFunc& theM)
{
  // Extent
  Standard_Integer iExt = theM.Extent();
  Standard_Integer i;

  printf("Info: testing IndexedMap(l=%d)\n", iExt);
  theM.Statistics(std::cout);
  // Resize
  theM.ReSize(8);
  theM.Statistics(std::cout);
  std::cout.flush();
  // Constructor
  ////////////////////////////////QANCollection_IndexedMap aM;
  QANCollection_IndexedMapFunc aM;
  // Add, FindKey, FindIndex
  Key1Type aKey;
  for (i = 0; i < 8; i++)
  {
    Random(aKey);
    aM.Add(aKey);
    Standard_Integer iIndex = aM.FindIndex(aKey);
    printf("     added a key, i=%d, k=", iIndex);
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
  aM.Substitute(1, aKey);
  if (!aM.Contains(aKey) || aM.FindIndex(aKey) != 1)
  {
    printf("Error   : map does not contain valid key after substitute");
  }
  // Invoke substitute with the same key
  aM.Substitute(1, aKey);
  if (!aM.Contains(aKey) || aM.FindIndex(aKey) != 1)
  {
    printf("Error   : map does not contain valid key after substitute");
  }
  // Copy constructor (including operator=)
  ////////////////////////////////QANCollection_IndexedMap aM2 = QANCollection_IndexedMap(aM);
  QANCollection_IndexedMapFunc aM2 = QANCollection_IndexedMapFunc(aM);
  // Assign
  AssignCollection(aM2, theM);

  // Clear
  aM.Clear();
}

// ===================== Test methods of IndexedDataMap type ==================
////////////////////////////////void TestIndexedDataMap  (QANCollection_IDMap& theM)
static void TestIndexedDataMap(QANCollection_IDMapFunc& theM)
{
  // Extent
  Standard_Integer iExt = theM.Extent();
  Standard_Integer i;

  printf("Info: testing IndexedDataMap(l=%d)\n", iExt);
  theM.Statistics(std::cout);
  // Resize
  theM.ReSize(8);
  theM.Statistics(std::cout);
  std::cout.flush();
  // Constructor
  ////////////////////////////////QANCollection_IDMap aM;
  QANCollection_IDMapFunc aM;
  // Add, FindKey, FindIndex, FindFromIndex, Change..., ()
  Key1Type aKey;
  ItemType anItem;
  for (i = 0; i < 8; i++)
  {
    Random(aKey);
    Random(anItem);
    aM.Add(aKey, anItem);
    Standard_Integer iIndex = aM.FindIndex(aKey);
    printf("     added a key, i=%d, k=", iIndex);
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
  aM.Substitute(1, aKey, anItem);
  if (!aM.Contains(aKey) || aM.FindIndex(aKey) != 1
      || !aM.FindFromKey(aKey).IsEqual(anItem, Precision::Confusion()))
  {
    printf("Error   : map does not contain valid key and item after substitute");
  }
  // Substitute with equal keys
  Random(anItem);
  aM.Substitute(1, aKey, anItem);
  if (!aM.Contains(aKey) || aM.FindIndex(aKey) != 1
      || !aM.FindFromKey(aKey).IsEqual(anItem, Precision::Confusion()))
  {
    printf("Error   : map does not contain valid key and item after substitute");
  }
  // Copy constructor (including operator=)
  ////////////////////////////////theM = QANCollection_IDMap(aM);
  theM = QANCollection_IDMapFunc(aM);
  // Assign - prohibited
  // AssignCollection (aM2,theM);
  printCollection(theM, "DoubleMap:");

  // Clear
  aM.Clear();
}

//=================================================================================================

Standard_Integer CheckArguments1(Draw_Interpretor& di,
                                 Standard_Integer  argc,
                                 const char**      argv,
                                 Standard_Integer& Lower,
                                 Standard_Integer& Upper)
{
  if (argc != 3)
  {
    di << "Usage : " << argv[0] << " Lower Upper\n";
    return 1;
  }
  Lower = Draw::Atoi(argv[1]);
  Upper = Draw::Atoi(argv[2]);
  if (Lower > Upper)
  {
    di << "Lower > Upper\n";
    return 1;
  }
  return 0;
}

//=================================================================================================

Standard_Integer CheckArguments2(Draw_Interpretor& di,
                                 Standard_Integer  argc,
                                 const char**      argv,
                                 Standard_Integer& LowerRow,
                                 Standard_Integer& UpperRow,
                                 Standard_Integer& LowerCol,
                                 Standard_Integer& UpperCol)
{
  if (argc != 5)
  {
    di << "Usage : " << argv[0] << " LowerRow UpperRow LowerCol UpperCol\n";
    return 1;
  }
  LowerRow = Draw::Atoi(argv[1]);
  UpperRow = Draw::Atoi(argv[2]);
  LowerCol = Draw::Atoi(argv[3]);
  UpperCol = Draw::Atoi(argv[4]);
  if (LowerRow > UpperRow)
  {
    di << "LowerRow > UpperRow\n";
    return 1;
  }
  if (LowerCol > UpperCol)
  {
    di << "LowerCol UpperCol> \n";
    return 1;
  }
  return 0;
}

//=================================================================================================

static Standard_Integer QANColTestArray1(Draw_Interpretor& di,
                                         Standard_Integer  argc,
                                         const char**      argv)
{
  Standard_Integer Lower, Upper;
  if (CheckArguments1(di, argc, argv, Lower, Upper))
  {
    return 1;
  }
  QANCollection_Array1Func anArr1(Lower, Upper);
  TestArray1(anArr1);
  return 0;
}

//=================================================================================================

static Standard_Integer QANColTestArray2(Draw_Interpretor& di,
                                         Standard_Integer  argc,
                                         const char**      argv)
{
  Standard_Integer LowerRow, UpperRow, LowerCol, UpperCol;
  if (CheckArguments2(di, argc, argv, LowerRow, UpperRow, LowerCol, UpperCol))
  {
    return 1;
  }
  QANCollection_Array2Func anArr2(LowerRow, UpperRow, LowerCol, UpperCol);
  TestArray2(anArr2);

  // check resize
  for (int aPass = 0; aPass <= 5; ++aPass)
  {
    Standard_Integer aNewLowerRow = LowerRow, aNewUpperRow = UpperRow, aNewLowerCol = LowerCol,
                     aNewUpperCol = UpperCol;
    switch (aPass)
    {
      case 0:
        aNewLowerRow -= 1;
        break;
      case 1:
        aNewLowerCol -= 1;
        break;
      case 2:
        aNewLowerRow -= 1;
        aNewLowerCol -= 1;
        break;
      case 3:
        aNewUpperRow += 1;
        break;
      case 4:
        aNewUpperCol += 1;
        break;
      case 5:
        aNewUpperRow += 1;
        aNewUpperCol += 1;
        break;
    }
    QANCollection_Array2Func anArr2Copy = anArr2;
    anArr2Copy.Resize(aNewLowerRow, aNewUpperRow, aNewLowerCol, aNewUpperCol, true);
    const Standard_Integer aNbRowsMin = Min(anArr2.NbRows(), anArr2Copy.NbRows());
    const Standard_Integer aNbColsMin = Min(anArr2.NbColumns(), anArr2Copy.NbColumns());
    for (Standard_Integer aRowIter = 0; aRowIter < aNbRowsMin; ++aRowIter)
    {
      for (Standard_Integer aColIter = 0; aColIter < aNbColsMin; ++aColIter)
      {
        const gp_Pnt& aPnt1 =
          anArr2.Value(aRowIter + anArr2.LowerRow(), aColIter + anArr2.LowerCol());
        const gp_Pnt& aPnt2 =
          anArr2Copy.Value(aRowIter + anArr2Copy.LowerRow(), aColIter + anArr2Copy.LowerCol());
        if (!aPnt1.IsEqual(aPnt2, gp::Resolution()))
        {
          std::cerr << "Error: 2D array is not properly resized\n";
          return 1;
        }
      }
    }
  }

  {
    QANCollection_Array2Func anArr2Copy2 = anArr2;
    anArr2Copy2.Resize(LowerRow - 1, UpperRow - 1, LowerCol + 1, UpperCol + 1, false);
  }

  {
    // empty array resize
    QANCollection_Array2Func anArr2Copy3;
    anArr2Copy3.Resize(LowerRow, UpperRow, LowerCol, UpperCol, false);
    anArr2Copy3 = anArr2;
  }

  return 0;
}

//=================================================================================================

static Standard_Integer QANColTestMap(Draw_Interpretor& di,
                                      Standard_Integer  argc,
                                      const char**      argv)
{
  if (argc != 1)
  {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_MapFunc aMap;
  TestMap(aMap, di);
  return 0;
}

//=================================================================================================

static Standard_Integer QANColTestDataMap(Draw_Interpretor& di,
                                          Standard_Integer  argc,
                                          const char**      argv)
{
  if (argc != 1)
  {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_DataMapFunc aDataMap;
  TestDataMap(aDataMap);
  return 0;
}

//=================================================================================================

static Standard_Integer QANColTestDoubleMap(Draw_Interpretor& di,
                                            Standard_Integer  argc,
                                            const char**      argv)
{
  if (argc != 1)
  {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_DoubleMapFunc aDoubleMap;
  TestDoubleMap(aDoubleMap);
  return 0;
}

//=================================================================================================

static Standard_Integer QANColTestIndexedMap(Draw_Interpretor& di,
                                             Standard_Integer  argc,
                                             const char**      argv)
{
  if (argc != 1)
  {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_IndexedMapFunc aIndexedMap;
  TestIndexedMap(aIndexedMap);
  return 0;
}

//=================================================================================================

static Standard_Integer QANColTestIndexedDataMap(Draw_Interpretor& di,
                                                 Standard_Integer  argc,
                                                 const char**      argv)
{
  if (argc != 1)
  {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_IDMapFunc aIDMap;
  TestIndexedDataMap(aIDMap);
  return 0;
}

//=================================================================================================

static Standard_Integer QANColTestList(Draw_Interpretor& di,
                                       Standard_Integer  argc,
                                       const char**      argv)
{
  if (argc != 1)
  {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_ListFunc aList;
  TestList(aList);
  return 0;
}

//=================================================================================================

static Standard_Integer QANColTestVector(Draw_Interpretor&, Standard_Integer, const char**)
{
  // test method Append and copying of empty vector
  NCollection_Vector<int> aVec;
  NCollection_Vector<int> aVec2(aVec);
  NCollection_Vector<int> aVec3;
  aVec3 = aVec;

  aVec.Append(5);
  if (aVec(0) != 5)
    std::cout << "Error: wrong value in original vector!" << std::endl;
  aVec2.Append(5);
  if (aVec2(0) != 5)
    std::cout << "Error: wrong value in copy-constructed vector!" << std::endl;
  aVec3.Append(5);
  if (aVec3(0) != 5)
    std::cout << "Error: wrong value in copied vector!" << std::endl;
  std::cout << "Test OK" << std::endl;

  return 0;
}

//=================================================================================================

static Standard_Integer QANColTestSequence(Draw_Interpretor& di,
                                           Standard_Integer  argc,
                                           const char**      argv)
{
  if (argc != 1)
  {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_SequenceFunc aSeq;
  TestSequence(aSeq);
  return 0;
}

//=================================================================================================

// Return array based on local C array buffer by value.
// Note that this is expected to cause errors due
// to the fact that returned copy will keep reference to the
// buffer allocated in the stack of this function.
// Unfortunately, this cannot be prevented due to the fact that
// modern compilers use return value optimization in release mode
// (object that is returned is constructed once at its target
// place and never copied).
static NCollection_Array1<double> GetArrayByValue()
{
  const int                  aLen          = 1024;
  double                     aCArray[aLen] = {};
  NCollection_Array1<double> anArray(aCArray[0], 1, aLen);
  for (int i = 1; i <= aLen; i++)
    anArray.SetValue(i, i + 113.);
  return anArray;
}

// check array for possible corruption
static bool CheckArrayByValue(NCollection_Array1<double> theArray)
{
  for (int i = 1; i <= theArray.Length(); i++)
  {
    if (theArray.Value(i) != i + 113.)
    {
      std::cout << "Error at item " << i << ": value = " << theArray.Value(i) << ", expected "
                << i + 113. << std::endl;
      return false;
    }
  }
  return true;
}

static Standard_Integer QANColTestArrayMove(Draw_Interpretor& di,
                                            Standard_Integer  argc,
                                            const char**      argv)
{
  if (argc != 1)
  {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  NCollection_Array1<double> anArray = GetArrayByValue();
  di << (CheckArrayByValue(anArray) ? "Error: memory corruption is not detected"
                                    : "Expected behavior: memory is corrupted");
  return 0;
}

#include <math_BullardGenerator.hxx>
#include <OSD_Timer.hxx>

static inline double test_atof(const char* theStr)
{
  return atof(theStr);
}

static inline double test_Atof(const char* theStr)
{
  return Atof(theStr);
}

static inline double test_strtod(const char* theStr)
{
  char* end;
  return strtod(theStr, &end);
}

static inline double test_Strtod(const char* theStr)
{
  char* end;
  return Strtod(theStr, &end);
}

static inline double test_sscanf(const char* theStr)
{
  double val = 0.;
  sscanf(theStr, "%lf", &val);
  return val;
}

static int check_atof(const NCollection_Array2<char>& theStrings,
                      const char*                     theFormat,
                      double (*test_func)(const char*),
                      Draw_Interpretor& di)
{
  int aNbErr = 0;
  for (int i = 0; i < theStrings.UpperRow(); i++)
  {
    const char* aStr = &theStrings(i, 0);
    char        buff[256];
    double      aVal = test_func(aStr);
    Sprintf(buff, theFormat, aVal);
    if (strcasecmp(buff, &theStrings(i, 0)))
    {
#if defined(_MSC_VER) && _MSC_VER < 1900
      // MSVC < 2015 prints nan and inf as 1.#NAN or 1.INF, and noes not recognize nan or inf on
      // read
      if (strstr(aStr, "1.#") || strstr(aStr, "nan") || strstr(aStr, "inf") || strstr(aStr, "NAN")
          || strstr(aStr, "INF"))
        continue;
#endif
      if (aNbErr < 5)
      {
        di << "Deviation parsing " << aStr << " and print back: " << buff << "\n";
      }
      aNbErr++;
    }
  }
  return aNbErr;
}

// Test speed of standard and OCCT-specific (accelerated) functions to parse string to double
static Standard_Integer QATestAtof(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  int    aNbToTest = Max(100, (argc > 1 ? Draw::Atoi(argv[1]) : 1000000));
  int    aNbDigits = (argc > 2 ? Draw::Atoi(argv[2]) : 10);
  double aRangeMin = (argc > 3 ? Draw::Atof(argv[3]) : -1e9);
  double aRangeMax = (argc > 4 ? Draw::Atof(argv[4]) : 1e9);

  char aFormat[256];
  Sprintf(aFormat, "%%.%dlg", Max(2, Min(20, aNbDigits)));

  // prepare data
  const int                MAXLEN = 256;
  NCollection_Array2<char> aValuesStr(0, aNbToTest - 1, 0, MAXLEN);
  math_BullardGenerator    aRandom;

  if (aRangeMin < aRangeMax)
  {
    // random values within specified range
    //    std::default_random_engine aRandomEngine;
    //    std::uniform_real_distribution<double> aRandomDistr (aRangeMin, aRangeMax);
    // clang-format off
    const uint64_t aMaxUInt64 = ~(uint64_t)0; // could be (not supported by old GCC): std::numeric_limits<uint64_t>::max()
    // clang-format on
    for (int i = 0; i < aNbToTest; i++)
    {
      //      double aVal = aRandomDistr (aRandomEngine);
      uint64_t aIVal = ((uint64_t)aRandom.NextInt() << 32) + aRandom.NextInt();
      double   aVal  = aRangeMin + (aIVal / (double)aMaxUInt64) * (aRangeMax - aRangeMin);
      Sprintf(&aValuesStr(i, 0), aFormat, aVal);
    }
  }
  else
  {
    // special values
    int i = 0;

    strcpy(&aValuesStr(i++, 0), "nan");
    strcpy(&aValuesStr(i++, 0), "nan(qnan)");
    strcpy(&aValuesStr(i++, 0), "NAN");
    strcpy(&aValuesStr(i++, 0), "-nan");
    strcpy(&aValuesStr(i++, 0), "-NAN");
    strcpy(&aValuesStr(i++, 0), "inf");
    strcpy(&aValuesStr(i++, 0), "INF");
    strcpy(&aValuesStr(i++, 0), "-inf");
    strcpy(&aValuesStr(i++, 0), "-INF");

    strcpy(&aValuesStr(i++, 0), "  ."); // standalone period should not be considered as a  number
                                        // clang-format off
    strcpy (&aValuesStr(i++,0), "nanabcdef_128  xx"); // extra non-parenthised sequence after "nan"

    strcpy (&aValuesStr(i++,0), "905791934.11394954"); // value that gets rounded in a wrong way by fast Strtod()
    strcpy (&aValuesStr(i++,0), "9.343962790444495e+148"); // value where strtod() and Strtod() differ by 2 Epsilon

    strcpy (&aValuesStr(i++,0), "     12345.67text"); // test for leading whitespaces and trailing text
    strcpy (&aValuesStr(i++,0), "000.000"); // test for zero
    strcpy (&aValuesStr(i++,0), "000.000e-0002"); // test for zero

    strcpy (&aValuesStr(i++,0), "1000000000000000000000000000012345678901234567890"); // huge mantissa
    strcpy (&aValuesStr(i++,0), "0000000000.00000000000000000012345678901234567890"); // leading zeros
    strcpy (&aValuesStr(i++,0), "1.00000000000000000000000000012345678901234567890"); // long fractional part
                                                                 // clang-format on

    strcpy(&aValuesStr(i++, 0), "0.0000000001e318");         // large exponent but no overflow
    strcpy(&aValuesStr(i++, 0), "-1.7976931348623158e+308"); // -DBL_MAX
    strcpy(&aValuesStr(i++, 0), "1.79769313486232e+308");    // overflow

    strcpy(&aValuesStr(i++, 0), "10000000000e-310"); // large negative exponent but no underflow
    strcpy(&aValuesStr(i++, 0), "1.1e-310");         // underflow
    strcpy(&aValuesStr(i++, 0), "0.000001e-310");    // underflow
    strcpy(&aValuesStr(i++, 0), "2.2250738585072014e-308"); // underflow, DBL_MIN
    strcpy(&aValuesStr(i++, 0), "2.2250738585e-308");       // underflow, value less than DBL_MIN

    strcpy(&aValuesStr(i++, 0), "2.2204460492503131e-016"); // DBL_EPSILON

    // random binary data
    //    std::default_random_engine aRandomEngine;
    //    std::uniform_int_distribution<uint64_t> aRandomDistr (0, ~(uint64_t)0);
    for (; i < aNbToTest; i++)
    {
      union {
        uint64_t valint;
        double   valdbl;
      } aVal;

      //      aVal.valint = aRandomDistr (aRandomEngine);
      aVal.valint = ((uint64_t)aRandom.NextInt() << 32) + aRandom.NextInt();
      Sprintf(&aValuesStr(i, 0), aFormat, aVal.valdbl);
    }
  }

  // test different methods
#define TEST_ATOF(method)                                                                          \
  OSD_Timer aT_##method;                                                                           \
  aT_##method.Start();                                                                             \
  double aRes_##method = 0.;                                                                       \
  for (int i = 0; i < aNbToTest; i++)                                                              \
  {                                                                                                \
    aRes_##method += test_##method(&aValuesStr(i, 0));                                             \
  }                                                                                                \
  aT_##method.Stop()

  TEST_ATOF(sscanf);
  TEST_ATOF(strtod);
  TEST_ATOF(atof);
  TEST_ATOF(Strtod);
  TEST_ATOF(Atof);
#undef TEST_ATOF

  // test different methods
#define CHECK_ATOF(method)                                                                         \
  int aNbErr_##method = check_atof(aValuesStr, aFormat, test_##method, di);                        \
  di << "Checking " << #method << ": " << aNbErr_##method << " deviations\n"

  CHECK_ATOF(sscanf);
  CHECK_ATOF(strtod);
  CHECK_ATOF(atof);
  CHECK_ATOF(Strtod);
  CHECK_ATOF(Atof);
#undef CHECK_ATOF

/* compare results with atof */
#ifdef _MSC_VER
  #define ISFINITE _finite
#else
  #define ISFINITE std::isfinite
#endif
  int nbErr = 0;
  for (int i = 0; i < aNbToTest; i++)
  {
    char * aStr = &aValuesStr(i, 0), *anEndOCCT, *anEndStd;
    double aRes = Strtod(aStr, &anEndOCCT);
    double aRef = strtod(aStr, &anEndStd);
    if (ISFINITE(aRes) != ISFINITE(aRef))
    {
      nbErr++;
#if defined(_MSC_VER) && _MSC_VER < 1900
      // MSVC < 2015 prints nan and inf as 1.#NAN or 1.INF, and noes not recognize nan or inf on
      // read
      if (strstr(aStr, "1.#") || strstr(aStr, "nan") || strstr(aStr, "inf") || strstr(aStr, "NAN")
          || strstr(aStr, "INF"))
        continue;
#endif
      if (nbErr < 5)
      {
        char aBuff[256];
        Sprintf(aBuff, "Error parsing %s: %.20lg / %.20lg\n", aStr, aRes, aRef);
        di << aBuff;
      }
    }
    else if (ISFINITE(aRef) && Abs(aRes - aRef) > Epsilon(aRef))
    {
      nbErr++;
      if (nbErr < 5)
      {
        char aBuff[256];
        Sprintf(aBuff, "Error parsing %s: %.20lg / %.20lg\n", aStr, aRes, aRef);
        di << aBuff;
        Sprintf(aBuff, "[Delta = %.8lg, Epsilon = %.8lg]\n", Abs(aRes - aRef), Epsilon(aRef));
        di << aBuff;
      }
    }

    // check that Strtod() and strtod() stop at the same place;
    // this makes sense for reading special values such as "nan" and thus
    // is not relevant for MSVC 2010 and earlier than do not support these
#if !defined(_MSC_VER) || _MSC_VER >= 1700
    if (anEndOCCT != anEndStd)
    {
      nbErr++;
      if (nbErr < 5)
        di << "Error: different number of symbols parsed in " << aStr << ": "
           << (int)(anEndOCCT - aStr) << " / " << (int)(anEndStd - aStr) << "\n";
    }
#endif
  }
  di << "Total " << nbErr << " defiations from strtod() found\n";
  /* */

  // print results
  di << "Method\t      CPU\t  Elapsed   \t    Deviations \tChecksum\n";

#define PRINT_RES(method)                                                                          \
  di << #method "\t" << aT_##method.UserTimeCPU() << "  \t" << aT_##method.ElapsedTime() << "\t"   \
     << aNbErr_##method << "\t" << aRes_##method << "\n"
  PRINT_RES(sscanf);
  PRINT_RES(strtod);
  PRINT_RES(atof);
  PRINT_RES(Strtod);
  PRINT_RES(Atof);
#undef PRINT_RES

  return 0;
}

// Test operations with NCollection_Vec4 that caused generation of invalid code by GCC
// due to reinterpret_cast conversions of Vec4 internal buffer to Vec3 (see #29825)
static Standard_Integer QANColTestVec4(Draw_Interpretor& theDI,
                                       Standard_Integer /*theNbArgs*/,
                                       const char** /*theArgVec*/)
{
  NCollection_Mat4<float> aMatrix;
  aMatrix.Translate(NCollection_Vec3<float>(4.0f, 3.0f, 1.0f));

  NCollection_Vec4<float> aPoints1[8];
  for (int aX = 0; aX < 2; ++aX)
  {
    for (int aY = 0; aY < 2; ++aY)
    {
      for (int aZ = 0; aZ < 2; ++aZ)
      {
        aPoints1[aX * 2 * 2 + aY * 2 + aZ] = NCollection_Vec4<float>(-1.0f + 2.0f * float(aX),
                                                                     -1.0f + 2.0f * float(aY),
                                                                     -1.0f + 2.0f * float(aZ),
                                                                     1.0f);
      }
    }
  }

  NCollection_Vec3<float> aPoints2[8];
  for (int aPntIdx = 0; aPntIdx < 8; ++aPntIdx)
  {
    // NB: the evaluation of line below could be dropped by GCC optimizer
    // while retrieving xyz() value the line after
    aPoints1[aPntIdx] = aMatrix * aPoints1[aPntIdx];
    aPoints2[aPntIdx] = aPoints1[aPntIdx].xyz() / aPoints1[aPntIdx].w();
    // aPoints2[aPntIdx] = NCollection_Vec3<float> (aPoints1[aPntIdx].x(), aPoints1[aPntIdx].y(),
    // aPoints1[aPntIdx].z()) / aPoints1[aPntIdx].w();
  }

  for (int aPntIter = 0; aPntIter < 8; ++aPntIter)
  {
    theDI << aPoints2[aPntIter].SquareModulus() << " ";
  }
  if ((int)(aPoints2[7].SquareModulus() + 0.5f) != 45)
  {
    theDI << "Error: method 'NCollection_Vec4::xyz()' failed.";
  }
  return 0;
}

//! Print file path flags deduced from path string.
static Standard_Integer QAOsdPathType(Draw_Interpretor& theDI,
                                      Standard_Integer  theNbArgs,
                                      const char**      theArgVec)
{
  if (theNbArgs != 2)
  {
    std::cout << "Syntax error: wrong number of arguments\n";
    return 1;
  }

  TCollection_AsciiString aPath(theArgVec[1]);
  if (OSD_Path::IsAbsolutePath(aPath.ToCString()))
  {
    theDI << "absolute ";
  }
  if (OSD_Path::IsRelativePath(aPath.ToCString()))
  {
    theDI << "relative ";
  }
  if (OSD_Path::IsUnixPath(aPath.ToCString()))
  {
    theDI << "unix ";
  }
  if (OSD_Path::IsDosPath(aPath.ToCString()))
  {
    theDI << "dos ";
  }
  if (OSD_Path::IsUncPath(aPath.ToCString()))
  {
    theDI << "unc ";
  }
  if (OSD_Path::IsNtExtendedPath(aPath.ToCString()))
  {
    theDI << "ntextended ";
  }
  if (OSD_Path::IsUncExtendedPath(aPath.ToCString()))
  {
    theDI << "uncextended ";
  }
  if (OSD_Path::IsRemoteProtocolPath(aPath.ToCString()))
  {
    theDI << "protocol ";
  }
  if (OSD_Path::IsContentProtocolPath(aPath.ToCString()))
  {
    theDI << "content ";
  }
  return 0;
}

//! Print file path part deduced from path string.
static Standard_Integer QAOsdPathPart(Draw_Interpretor& theDI,
                                      Standard_Integer  theNbArgs,
                                      const char**      theArgVec)
{
  TCollection_AsciiString aPath;

  enum PathPart
  {
    PathPart_NONE,
    PathPart_Folder,
    PathPart_FileName,
  };

  PathPart aPart = PathPart_NONE;
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArgCase(theArgVec[anArgIter]);
    anArgCase.LowerCase();
    if (aPart == PathPart_NONE && anArgCase == "-folder")
    {
      aPart = PathPart_Folder;
    }
    else if (aPart == PathPart_NONE && anArgCase == "-filename")
    {
      aPart = PathPart_FileName;
    }
    else if (aPath.IsEmpty())
    {
      aPath = theArgVec[anArgIter];
    }
    else
    {
      std::cout << "Syntax error at '" << theArgVec[anArgIter] << "'\n";
      return 1;
    }
  }
  if (aPath.IsEmpty() || aPart == PathPart_NONE)
  {
    std::cout << "Syntax error: wrong number of arguments\n";
    return 1;
  }

  TCollection_AsciiString aFolder, aFileName;
  OSD_Path::FolderAndFileFromPath(aPath, aFolder, aFileName);
  switch (aPart)
  {
    case PathPart_Folder:
      theDI << aFolder;
      return 0;
    case PathPart_FileName:
      theDI << aFileName;
      return 0;
    case PathPart_NONE:
    default:
      return 1;
  }
}

void QANCollection::CommandsTest(Draw_Interpretor& theCommands)
{
  const char* group = "QANCollection";

  theCommands.Add("QANColTestArray1",
                  "QANColTestArray1 Lower Upper",
                  __FILE__,
                  QANColTestArray1,
                  group);
  theCommands.Add("QANColTestArray2",
                  "QANColTestArray2 LowerRow UpperRow LowerCol UpperCol",
                  __FILE__,
                  QANColTestArray2,
                  group);
  theCommands.Add("QANColTestMap", "QANColTestMap", __FILE__, QANColTestMap, group);
  theCommands.Add("QANColTestDataMap", "QANColTestDataMap", __FILE__, QANColTestDataMap, group);
  theCommands.Add("QANColTestDoubleMap",
                  "QANColTestDoubleMap",
                  __FILE__,
                  QANColTestDoubleMap,
                  group);
  theCommands.Add("QANColTestIndexedMap",
                  "QANColTestIndexedMap",
                  __FILE__,
                  QANColTestIndexedMap,
                  group);
  theCommands.Add("QANColTestIndexedDataMap",
                  "QANColTestIndexedDataMap",
                  __FILE__,
                  QANColTestIndexedDataMap,
                  group);
  theCommands.Add("QANColTestList", "QANColTestList", __FILE__, QANColTestList, group);
  theCommands.Add("QANColTestSequence", "QANColTestSequence", __FILE__, QANColTestSequence, group);
  theCommands.Add("QANColTestVector", "QANColTestVector", __FILE__, QANColTestVector, group);
  theCommands.Add("QANColTestArrayMove",
                  "QANColTestArrayMove (is expected to give error)",
                  __FILE__,
                  QANColTestArrayMove,
                  group);
  theCommands.Add("QANColTestVec4",
                  "QANColTestVec4 test Vec4 implementation",
                  __FILE__,
                  QANColTestVec4,
                  group);
  theCommands.Add("QATestAtof",
                  "QATestAtof [nbvalues [nbdigits [min [max]]]]",
                  __FILE__,
                  QATestAtof,
                  group);
  theCommands.Add("QAOsdPathType",
                  "QAOsdPathType path : Print file path flags deduced from path string",
                  __FILE__,
                  QAOsdPathType,
                  group);
  theCommands.Add("QAOsdPathPart",
                  "QAOsdPathPart path [-folder][-fileName] : Print file path part",
                  __FILE__,
                  QAOsdPathPart,
                  group);
}
