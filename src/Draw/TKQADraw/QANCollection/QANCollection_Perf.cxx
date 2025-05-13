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

#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_MapOfReal.hxx>
#include <TColStd_IndexedMapOfReal.hxx>
#include <TColgp_SequenceOfPnt.hxx>

#include <QANCollection_DataMapOfRealPnt.hxx>
#include <QANCollection_DoubleMapOfRealInteger.hxx>
#include <QANCollection_IndexedDataMapOfRealPnt.hxx>
#include <QANCollection_ListOfPnt.hxx>

#include <NCollection_SparseArray.hxx>
#include <NCollection_SparseArrayBase.hxx>

#define PERF_ENABLE_METERS
#include <OSD_PerfMeter.hxx>

#define ItemType gp_Pnt
#define Key1Type Standard_Real
#define Key2Type Standard_Integer

// =====================               INSTANTIATIONS               ===========
// ===================== The Types must be defined before this line ===========
// These are: TheItemType, TheKey1Type, TheKey2Type
// So must be defined ::HashCode and ::IsEqual too

#include <NCollection_DefineHArray1.hxx>
#define DEFINE_ARRAY1(_ClassName_, _BaseCollection_, TheItemType)                                  \
  typedef NCollection_Array1<TheItemType> _ClassName_;

////////////////////////////////DEFINE_ARRAY1(QANCollection_Array1,QANCollection_BaseCol,ItemType)
////////////////////////////////DEFINE_HARRAY1(QANCollection_HArray1,QANCollection_Array1)
DEFINE_ARRAY1(QANCollection_Array1Perf, QANCollection_BaseColPerf, ItemType)
DEFINE_HARRAY1(QANCollection_HArray1Perf, QANCollection_Array1Perf)

#include <NCollection_DefineHArray2.hxx>
#define DEFINE_ARRAY2(_ClassName_, _BaseCollection_, TheItemType)                                  \
  typedef NCollection_Array2<TheItemType> _ClassName_;
////////////////////////////////DEFINE_ARRAY2(QANCollection_Array2,QANCollection_BaseCol,ItemType)
////////////////////////////////DEFINE_HARRAY2(QANCollection_HArray2,QANCollection_Array2)
DEFINE_ARRAY2(QANCollection_Array2Perf, QANCollection_BaseColPerf, ItemType)
DEFINE_HARRAY2(QANCollection_HArray2Perf, QANCollection_Array2Perf)
////////////////////////////////DEFINE_MAP(QANCollection_Map,QANCollection_Key1BaseCol,Key1Type)
////////////////////////////////DEFINE_DATAMAP(QANCollection_DataMap,QANCollection_BaseCol,Key1Type,ItemType)
////////////////////////////////DEFINE_DOUBLEMAP(QANCollection_DoubleMap,QANCollection_Key2BaseCol,Key1Type,Key2Type)
////////////////////////////////DEFINE_INDEXEDMAP(QANCollection_IndexedMap,QANCollection_Key1BaseCol,Key1Type)
////////////////////////////////DEFINE_INDEXEDDATAMAP(QANCollection_IDMap,QANCollection_BaseCol,Key1Type,ItemType)
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
DEFINE_MAP(QANCollection_MapPerf, QANCollection_Key1BaseColPerf, Key1Type)
DEFINE_DATAMAP(QANCollection_DataMapPerf, QANCollection_BaseColPerf, Key1Type, ItemType)
DEFINE_DOUBLEMAP(QANCollection_DoubleMapPerf, QANCollection_Key2BaseColPerf, Key1Type, Key2Type)
DEFINE_INDEXEDMAP(QANCollection_IndexedMapPerf, QANCollection_Key1BaseColPerf, Key1Type)
DEFINE_INDEXEDDATAMAP(QANCollection_IDMapPerf, QANCollection_BaseColPerf, Key1Type, ItemType)

////////////////////////////////DEFINE_LIST(QANCollection_List,QANCollection_BaseCol,ItemType)
#define DEFINE_LIST(_ClassName_, _BaseCollection_, TheItemType)                                    \
  typedef NCollection_List<TheItemType> _ClassName_;
DEFINE_LIST(QANCollection_ListPerf, QANCollection_BaseColPerf, ItemType)

#include <NCollection_DefineHSequence.hxx>
#define DEFINE_SEQUENCE(_ClassName_, _BaseCollection_, TheItemType)                                \
  typedef NCollection_Sequence<TheItemType> _ClassName_;
////////////////////////////////DEFINE_SEQUENCE(QANCollection_Sequence,QANCollection_BaseCol,ItemType)
////////////////////////////////DEFINE_HSEQUENCE(QANCollection_HSequence,QANCollection_Sequence)
DEFINE_SEQUENCE(QANCollection_SequencePerf, QANCollection_BaseColPerf, ItemType)
DEFINE_HSEQUENCE(QANCollection_HSequencePerf, QANCollection_SequencePerf)

static void printAllMeters(Draw_Interpretor& theDI)
{
  const TCollection_AsciiString aStr = OSD_PerfMeter::PrintALL();
  theDI << aStr << "\n";
  OSD_PerfMeter::ResetALL();
}

// ===================== Test perform of Array1 type ==========================
static void CompArray1(Draw_Interpretor&      theDI,
                       const Standard_Integer theRep,
                       const Standard_Integer theSize)
{
  Standard_Integer i, j;

  for (i = 0; i < theRep; i++)
  {
    OSD_PerfMeter            aCreationMeter("NCollection_Array1 creation");
    QANCollection_Array1Perf a1(1, theSize), a2(1, theSize);
    aCreationMeter.Stop();

    OSD_PerfMeter aFillingMeter("NCollection_Array1 filling");
    for (j = 1; j <= theSize; j++)
      Random(a1(j));
    aFillingMeter.Stop();

    OSD_PerfMeter aFindingMeter("NCollection_Array1 finding");
    for (j = 1; j <= theSize; j++)
    {
      Standard_Integer iIndex;
      Random(iIndex, theSize);
      a1.Value(iIndex + 1);
    }
    aFindingMeter.Stop();

    OSD_PerfMeter aAssignOperMeter("NCollection_Array1 operator=");
    a2 = a1;
    aAssignOperMeter.Stop();

    OSD_PerfMeter aAssignFnMeter("NCollection_Array1 Assign");
    a2.Assign(a1);
    aAssignFnMeter.Stop();
  }

  for (i = 0; i < theRep; i++)
  {
    OSD_PerfMeter      aCreationMeter("TCollection_Array1 creation");
    TColgp_Array1OfPnt a1(1, theSize), a2(1, theSize);
    aCreationMeter.Stop();

    OSD_PerfMeter aFillingMeter("TCollection_Array1 filling");
    for (j = 1; j <= theSize; j++)
    {
      Random(a1(j));
    }
    aFillingMeter.Stop();

    OSD_PerfMeter aFindingMeter("TCollection_Array1 finding");
    for (j = 1; j <= theSize; j++)
    {
      Standard_Integer iIndex;
      Random(iIndex, theSize);
      a1.Value(iIndex + 1);
    }
    aFindingMeter.Stop();

    OSD_PerfMeter aAssignOperMeter("TCollection_Array1 operator=");
    a2 = a1;
    aAssignOperMeter.Stop();
  }
  printAllMeters(theDI);
}

// ===================== Test perform of Array2 type ==========================
static void CompArray2(Draw_Interpretor&      theDI,
                       const Standard_Integer theRep,
                       const Standard_Integer theSize)
{
  Standard_Integer i, j, k;
  for (i = 0; i < theRep; i++)
  {
    OSD_PerfMeter            aCreationMeter("NCollection_Array2 creation");
    QANCollection_Array2Perf a1(1, theSize, 1, theSize), a2(1, theSize, 1, theSize);
    aCreationMeter.Stop();

    OSD_PerfMeter aFillingMeter("NCollection_Array2 filling");
    for (j = 1; j <= theSize; j++)
      for (k = 1; k <= theSize; k++)
        Random(a1(j, k));
    aFillingMeter.Stop();

    OSD_PerfMeter aFindingMeter("NCollection_Array2 finding");
    for (j = 1; j <= theSize * theSize; j++)
    {
      Standard_Integer m, n;
      Random(m, theSize);
      Random(n, theSize);
      a1.Value(m + 1, n + 1);
    }
    aFindingMeter.Stop();

    OSD_PerfMeter aAssignOperMeter("NCollection_Array2 operator=");
    a2 = a1;
    aAssignOperMeter.Stop();

    OSD_PerfMeter aAssignFnMeter("NCollection_Array2 Assign");
    a2.Assign(a1);
    aAssignFnMeter.Stop();
  }

  for (i = 0; i < theRep; i++)
  {
    OSD_PerfMeter      aCreationMeter("TCollection_Array2 creation");
    TColgp_Array2OfPnt a1(1, theSize, 1, theSize), a2(1, theSize, 1, theSize);
    aCreationMeter.Stop();

    OSD_PerfMeter aFillingMeter("TCollection_Array2 filling");
    for (j = 1; j <= theSize; j++)
      for (k = 1; k <= theSize; k++)
        Random(a1(j, k));
    aFillingMeter.Stop();

    OSD_PerfMeter aFindingMeter("TCollection_Array2 finding");
    for (j = 1; j <= theSize * theSize; j++)
    {
      Standard_Integer m, n;
      Random(m, theSize);
      Random(n, theSize);
      a1.Value(m + 1, n + 1);
    }
    aFindingMeter.Stop();

    OSD_PerfMeter aAssignOperMeter("TCollection_Array2 operator=");
    a2 = a1;
    aAssignOperMeter.Stop();
  }
  printAllMeters(theDI);
}

// ===================== Test perform of List type ==========================
static void CompList(Draw_Interpretor&      theDI,
                     const Standard_Integer theRep,
                     const Standard_Integer theSize)
{
  Standard_Integer i, j;

  for (i = 0; i < theRep; i++)
  {
    QANCollection_ListPerf a1, a2;

    OSD_PerfMeter aAppendingMeter("NCollection_List appending");
    for (j = 1; j <= theSize; j++)
    {
      ItemType anItem;
      Random(anItem);
      a1.Append(anItem);
    }
    aAppendingMeter.Stop();

    OSD_PerfMeter aAssignOperMeter("NCollection_List operator=");
    a2 = a1;
    aAssignOperMeter.Stop();

    OSD_PerfMeter aAssignFnMeter("NCollection_List Assign");
    a2.Assign(a1);
    aAssignFnMeter.Stop();

    OSD_PerfMeter aClearingMeter("NCollection_List clearing");
    a2.Clear();
    aClearingMeter.Stop();
  }

  for (i = 0; i < theRep; i++)
  {
    QANCollection_ListOfPnt a1, a2;

    OSD_PerfMeter aAppendingMeter("TCollection_List appending");
    for (j = 1; j <= theSize; j++)
    {
      ItemType anItem;
      Random(anItem);
      a1.Append(anItem);
    }
    aAppendingMeter.Stop();

    OSD_PerfMeter aAssignOperMeter("TCollection_List operator=");
    a2 = a1;
    aAssignOperMeter.Stop();

    OSD_PerfMeter aClearMeter("TCollection_List clearing");
    a2.Clear();
    aClearMeter.Stop();
  }
  printAllMeters(theDI);
}

// ===================== Test perform of Sequence type ==========================
static void CompSequence(Draw_Interpretor&      theDI,
                         const Standard_Integer theRep,
                         const Standard_Integer theSize)
{
  Standard_Integer i, j;

  for (i = 0; i < theRep; i++)
  {
    QANCollection_SequencePerf a1, a2;

    OSD_PerfMeter aAppendingMeter("NCollection_Sequence appending");
    for (j = 1; j <= theSize; j++)
    {
      ItemType anItem;
      Random(anItem);
      a1.Append(anItem);
    }
    aAppendingMeter.Stop();

    OSD_PerfMeter aFindingMeter("NCollection_Sequence finding");
    for (j = 1; j <= theSize; j++)
    {
      Standard_Integer iIndex;
      Random(iIndex, theSize);
      a1.Value(iIndex + 1);
    }
    aFindingMeter.Stop();

    OSD_PerfMeter aAssignOperMeter("NCollection_Sequence operator=");
    a2 = a1;
    aAssignOperMeter.Stop();

    OSD_PerfMeter aAssignFnMeter("NCollection_Sequence Assign");
    a2.Assign(a1);
    aAssignFnMeter.Stop();

    OSD_PerfMeter aClearingMeter("NCollection_Sequence clearing");
    a2.Clear();
    aClearingMeter.Stop();
  }

  for (i = 0; i < theRep; i++)
  {
    TColgp_SequenceOfPnt a1, a2;

    OSD_PerfMeter aAppendingMeter("TCollection_Sequence appending");
    for (j = 1; j <= theSize; j++)
    {
      ItemType anItem;
      Random(anItem);
      a1.Append(anItem);
    }
    aAppendingMeter.Stop();

    OSD_PerfMeter aFindingMeter("TCollection_Sequence finding");
    for (j = 1; j <= theSize; j++)
    {
      Standard_Integer iIndex;
      Random(iIndex, theSize);
      a1.Value(iIndex + 1);
    }
    aFindingMeter.Stop();

    OSD_PerfMeter aAssignOperMeter("TCollection_Sequence operator=");
    a2 = a1;
    aAssignOperMeter.Stop();

    OSD_PerfMeter aClearMeter("TCollection_Sequence clearing");
    a2.Clear();
    aClearMeter.Stop();
  }
  printAllMeters(theDI);
}

// ===================== Test perform of Map type ==========================
static void CompMap(Draw_Interpretor&      theDI,
                    const Standard_Integer theRep,
                    const Standard_Integer theSize)
{
  Standard_Integer i, j;

  for (i = 0; i < theRep; i++)
  {
    QANCollection_MapPerf a1, a2;

    OSD_PerfMeter aAddingMeter("NCollection_Map adding");
    for (j = 1; j <= theSize; j++)
    {
      Key1Type aKey1;
      Random(aKey1);
      a1.Add(aKey1);
    }
    aAddingMeter.Stop();

    OSD_PerfMeter aFindingMeter("NCollection_Map finding");
    for (j = 1; j <= theSize; j++)
    {
      Key1Type aKey1;
      Random(aKey1);
      a1.Contains(aKey1);
    }
    aFindingMeter.Stop();

    OSD_PerfMeter aAssignOperMeter("NCollection_Map operator=");
    a2 = a1;
    aAssignOperMeter.Stop();

    OSD_PerfMeter aAssignFnMeter("NCollection_Map Assign");
    a2.Assign(a1);
    aAssignFnMeter.Stop();

    OSD_PerfMeter aClearingMeter("NCollection_Map clearing");
    a2.Clear();
    aClearingMeter.Stop();
  }

  for (i = 0; i < theRep; i++)
  {
    TColStd_MapOfReal a1, a2;

    OSD_PerfMeter aAddingMeter("TCollection_Map adding");
    for (j = 1; j <= theSize; j++)
    {
      Key1Type aKey1;
      Random(aKey1);
      a1.Add(aKey1);
    }
    aAddingMeter.Stop();

    OSD_PerfMeter aFindingMeter("TCollection_Map finding");
    for (j = 1; j <= theSize; j++)
    {
      Key1Type aKey1;
      Random(aKey1);
      a1.Contains(aKey1);
    }
    aFindingMeter.Stop();

    OSD_PerfMeter aAssignOperMeter("TCollection_Map operator=");
    a2 = a1;
    aAssignOperMeter.Stop();

    OSD_PerfMeter aClearMeter("TCollection_Map clearing");
    a2.Clear();
    aClearMeter.Stop();
  }
  printAllMeters(theDI);
}

// ===================== Test perform of DataMap type ==========================
static void CompDataMap(Draw_Interpretor&      theDI,
                        const Standard_Integer theRep,
                        const Standard_Integer theSize)
{
  Standard_Integer i, j;

  for (i = 0; i < theRep; i++)
  {
    QANCollection_DataMapPerf a1, a2;

    OSD_PerfMeter aBindingMeter("NCollection_DataMap binding");
    for (j = 1; j <= theSize; j++)
    {
      Key1Type aKey1;
      ItemType anItem;
      Random(aKey1);
      Random(anItem);
      a1.Bind(aKey1, anItem);
    }
    aBindingMeter.Stop();

    OSD_PerfMeter aFindingMeter("NCollection_DataMap finding");
    for (j = 1; j <= theSize; j++)
    {
      Key1Type aKey1;
      Random(aKey1);
      a1.IsBound(aKey1);
    }
    aFindingMeter.Stop();

    OSD_PerfMeter aAssignOperMeter("NCollection_DataMap operator=");
    a2 = a1;
    aAssignOperMeter.Stop();

    OSD_PerfMeter aClearMeter("NCollection_DataMap clearing");
    a2.Clear();
    aClearMeter.Stop();
  }

  for (i = 0; i < theRep; i++)
  {
    QANCollection_DataMapOfRealPnt a1, a2;

    OSD_PerfMeter aBindingMeter("TCollection_DataMap binding");
    for (j = 1; j <= theSize; j++)
    {
      Key1Type aKey1;
      ItemType anItem;
      Random(aKey1);
      Random(anItem);
      a1.Bind(aKey1, anItem);
    }
    aBindingMeter.Stop();

    OSD_PerfMeter aFindingMeter("TCollection_DataMap finding");
    for (j = 1; j <= theSize; j++)
    {
      Key1Type aKey1;
      Random(aKey1);
      a1.IsBound(aKey1);
    }
    aFindingMeter.Stop();

    OSD_PerfMeter aAssignOperMeter("TCollection_DataMap operator=");
    a2 = a1;
    aAssignOperMeter.Stop();

    OSD_PerfMeter aClearMeter("TCollection_DataMap clearing");
    a2.Clear();
    aClearMeter.Stop();
  }
  printAllMeters(theDI);
}

// ===================== Test perform of DoubleMap type ==========================
static void CompDoubleMap(Draw_Interpretor&      theDI,
                          const Standard_Integer theRep,
                          const Standard_Integer theSize)
{
  Standard_Integer i, j;
  Standard_Integer iFail1 = 0, iFail2 = 0;

  for (i = 0; i < theRep; i++)
  {
    QANCollection_DoubleMapPerf a1, a2;

    OSD_PerfMeter aBindingMeter("NCollection_DoubleMap binding");
    for (j = 1; j <= theSize; j++)
    {
      Key1Type aKey1;
      Key2Type aKey2;
      do
      {
        Random(aKey1);
        Random(aKey2);
        iFail1++;
      } while (a1.IsBound1(aKey1) || a1.IsBound2(aKey2));
      iFail1--;
      a1.Bind(aKey1, aKey2);
    }
    aBindingMeter.Stop();

    OSD_PerfMeter aFindingMeter("NCollection_DoubleMap finding");
    for (j = 1; j <= theSize; j++)
    {
      Key1Type aKey1;
      Key2Type aKey2;
      Random(aKey1);
      Random(aKey2);
      a1.AreBound(aKey1, aKey2);
    }
    aFindingMeter.Stop();

    OSD_PerfMeter aAssignOperMeter("NCollection_DoubleMap operator=");
    a2 = a1;
    aAssignOperMeter.Stop();

    OSD_PerfMeter aClearMeter("NCollection_DoubleMap clearing");
    a2.Clear();
    aClearMeter.Stop();
  }

  for (i = 0; i < theRep; i++)
  {
    QANCollection_DoubleMapOfRealInteger a1, a2;

    OSD_PerfMeter aBindingMeter("TCollection_DoubleMap binding");
    for (j = 1; j <= theSize; j++)
    {
      Key1Type aKey1;
      Key2Type aKey2;
      do
      {
        Random(aKey1);
        Random(aKey2);
        iFail2++;
      } while (a1.IsBound1(aKey1) || a1.IsBound2(aKey2));
      iFail2--;
      a1.Bind(aKey1, aKey2);
    }
    aBindingMeter.Stop();

    OSD_PerfMeter aFindingMeter("TCollection_DoubleMap finding");
    for (j = 1; j <= theSize; j++)
    {
      Key1Type aKey1;
      Key2Type aKey2;
      Random(aKey1);
      Random(aKey2);
      a1.AreBound(aKey1, aKey2);
    }
    aFindingMeter.Stop();

    OSD_PerfMeter aAssignOperMeter("TCollection_DoubleMap operator=");
    a2 = a1;
    aAssignOperMeter.Stop();

    OSD_PerfMeter aClearMeter("TCollection_DoubleMap clearing");
    a2.Clear();
    aClearMeter.Stop();
  }
  printAllMeters(theDI);
  if (iFail1 || iFail2)
    std::cout << "Warning : N map failed " << iFail1 << " times, T map - " << iFail2 << std::endl;
}

// ===================== Test perform of IndexedMap type ==========================
static void CompIndexedMap(Draw_Interpretor&      theDI,
                           const Standard_Integer theRep,
                           const Standard_Integer theSize)
{
  Standard_Integer i, j;

  for (i = 0; i < theRep; i++)
  {
    QANCollection_IndexedMapPerf a1, a2;

    OSD_PerfMeter aAddingMeter("NCollection_IndexedMap adding");
    for (j = 1; j <= theSize; j++)
    {
      Key1Type aKey1;
      Random(aKey1);
      a1.Add(aKey1);
    }
    aAddingMeter.Stop();

    OSD_PerfMeter aFindingMeter("NCollection_IndexedMap finding");
    for (j = 1; j <= theSize; j++)
    {
      Key1Type aKey1;
      Random(aKey1);
      a1.Contains(aKey1);
    }
    aFindingMeter.Stop();

    OSD_PerfMeter aAssignOperMeter("NCollection_IndexedMap operator=");
    a2 = a1;
    aAssignOperMeter.Stop();

    OSD_PerfMeter aAssignFnMeter("NCollection_IndexedMap Assign");
    a2.Assign(a1);
    aAssignFnMeter.Stop();

    OSD_PerfMeter aClearingMeter("NCollection_IndexedMap clearing");
    a2.Clear();
    aClearingMeter.Stop();
  }

  for (i = 0; i < theRep; i++)
  {
    TColStd_IndexedMapOfReal a1, a2;

    OSD_PerfMeter aAddingMeter("TCollection_IndexedMap adding");
    for (j = 1; j <= theSize; j++)
    {
      Key1Type aKey1;
      Random(aKey1);
      a1.Add(aKey1);
    }
    aAddingMeter.Stop();

    OSD_PerfMeter aFindingMeter("TCollection_IndexedMap finding");
    for (j = 1; j <= theSize; j++)
    {
      Key1Type aKey1;
      Random(aKey1);
      a1.Contains(aKey1);
    }
    aFindingMeter.Stop();

    OSD_PerfMeter aAssignOperMeter("TCollection_IndexedMap operator=");
    a2 = a1;
    aAssignOperMeter.Stop();

    OSD_PerfMeter aClearingMeter("TCollection_IndexedMap clearing");
    a2.Clear();
    aClearingMeter.Stop();
  }
  printAllMeters(theDI);
}

// ===================== Test perform of IndexedDataMap type ==========================
static void CompIndexedDataMap(Draw_Interpretor&      theDI,
                               const Standard_Integer theRep,
                               const Standard_Integer theSize)
{
  Standard_Integer i, j;

  for (i = 0; i < theRep; i++)
  {

    QANCollection_IDMapPerf a1, a2;

    OSD_PerfMeter aBindingMeter("NCollection_IndexedDataMap binding");
    for (j = 1; j <= theSize; j++)
    {
      Key1Type aKey1;
      ItemType anItem;
      Random(aKey1);
      Random(anItem);
      a1.Add(aKey1, anItem);
    }
    aBindingMeter.Stop();

    OSD_PerfMeter aFindingMeter("NCollection_IndexedDataMap finding");
    for (j = 1; j <= theSize; j++)
    {
      Key1Type aKey1;
      Random(aKey1);
      a1.Contains(aKey1);
    }
    aFindingMeter.Stop();

    OSD_PerfMeter aAssignOperMeter("NCollection_IndexedDataMap operator=");
    a2 = a1;
    aAssignOperMeter.Stop();

    OSD_PerfMeter aClearingMeter("NCollection_IndexedDataMap clearing");
    a2.Clear();
    aClearingMeter.Stop();
  }

  for (i = 0; i < theRep; i++)
  {
    QANCollection_IndexedDataMapOfRealPnt a1, a2;

    OSD_PerfMeter aBindingMeter("TCollection_IndexedDataMap binding");
    for (j = 1; j <= theSize; j++)
    {
      Key1Type aKey1;
      ItemType anItem;
      Random(aKey1);
      Random(anItem);
      a1.Add(aKey1, anItem);
    }
    aBindingMeter.Stop();

    OSD_PerfMeter aFindingMeter("TCollection_IndexedDataMap finding");
    for (j = 1; j <= theSize; j++)
    {
      Key1Type aKey1;
      Random(aKey1);
      a1.Contains(aKey1);
    }
    aFindingMeter.Stop();

    OSD_PerfMeter aAssignOperMeter("TCollection_IndexedDataMap operator=");
    a2 = a1;
    aAssignOperMeter.Stop();

    OSD_PerfMeter aClearingMeter("TCollection_IndexedDataMap clearing");
    a2.Clear();
    aClearingMeter.Stop();
  }
  printAllMeters(theDI);
}

// ===================== Test perform of SparseArray type ==========================
static void CompSparseArray(Draw_Interpretor&      theDI,
                            const Standard_Integer theRep,
                            const Standard_Integer theSize)
{
  Standard_Integer i, j;
  for (i = 0; i < theRep; i++)
  {
    OSD_PerfMeter                             aCreationMeter("NCollection_SparseArray creation");
    NCollection_SparseArray<Standard_Integer> a1(theSize), a2(theSize);
    aCreationMeter.Stop();

    OSD_PerfMeter aFillingMeter("NCollection_SparseArray filling");
    for (j = 0; j < theSize; j++)
    {
      Standard_Integer iIndex;
      Random(iIndex, theSize);
      a1.SetValue(j, iIndex + 1);
    }
    aFillingMeter.Stop();

    OSD_PerfMeter aSizeMeter("NCollection_SparseArray size");
    Standard_Size sizeSparseArray = a1.Size();
    (void)sizeSparseArray; // avoid compiler warning on unused variable
    aSizeMeter.Stop();

    OSD_PerfMeter aAssignFnMeter("NCollection_SparseArray Assign");
    a2.Assign(a1);
    aAssignFnMeter.Stop();

    OSD_PerfMeter aHasValueMeter("NCollection_SparseArray HasValue");
    for (j = 0; j < theSize; j++)
    {
      Standard_Integer iIndex;
      Random(iIndex, theSize);
      a2.HasValue(iIndex + 1);
    }
    aHasValueMeter.Stop();

    OSD_PerfMeter aUnsetValueMeter("NCollection_SparseArray UnsetValue");
    for (j = 0; j < theSize; j++)
    {
      Standard_Integer iIndex;
      Random(iIndex, theSize);
      a1.UnsetValue(iIndex + 1);
    }
    aUnsetValueMeter.Stop();

    OSD_PerfMeter aClearMeter("NCollection_SparseArray Clear");
    a1.Clear();
    a2.Clear();
    aClearMeter.Stop();
  }

  printAllMeters(theDI);
}

//=================================================================================================

Standard_Integer CheckArguments(Draw_Interpretor& di,
                                Standard_Integer  argc,
                                const char**      argv,
                                Standard_Integer& Repeat,
                                Standard_Integer& Size)
{
  if (argc != 3)
  {
    di << "Usage : " << argv[0] << " Repeat Size\n";
    return 1;
  }
  Repeat = Draw::Atoi(argv[1]);
  Size   = Draw::Atoi(argv[2]);
  if (Repeat < 1)
  {
    di << "Repeat > 0\n";
    return 1;
  }
  if (Size < 1)
  {
    di << "Size > 0\n";
    return 1;
  }
  return 0;
}

//=================================================================================================

static Standard_Integer QANColPerfArray1(Draw_Interpretor& di,
                                         Standard_Integer  argc,
                                         const char**      argv)
{
  Standard_Integer Repeat, Size;
  if (CheckArguments(di, argc, argv, Repeat, Size))
  {
    return 1;
  }
  CompArray1(di, Repeat, Size);
  return 0;
}

//=================================================================================================

static Standard_Integer QANColPerfArray2(Draw_Interpretor& di,
                                         Standard_Integer  argc,
                                         const char**      argv)
{
  Standard_Integer Repeat, Size;
  if (CheckArguments(di, argc, argv, Repeat, Size))
  {
    return 1;
  }
  CompArray2(di, Repeat, Size);
  return 0;
}

//=================================================================================================

static Standard_Integer QANColPerfList(Draw_Interpretor& di,
                                       Standard_Integer  argc,
                                       const char**      argv)
{
  Standard_Integer Repeat, Size;
  if (CheckArguments(di, argc, argv, Repeat, Size))
  {
    return 1;
  }
  CompList(di, Repeat, Size);
  return 0;
}

//=================================================================================================

static Standard_Integer QANColPerfSequence(Draw_Interpretor& di,
                                           Standard_Integer  argc,
                                           const char**      argv)
{
  Standard_Integer Repeat, Size;
  if (CheckArguments(di, argc, argv, Repeat, Size))
  {
    return 1;
  }
  CompSequence(di, Repeat, Size);
  return 0;
}

//=================================================================================================

static Standard_Integer QANColPerfMap(Draw_Interpretor& di,
                                      Standard_Integer  argc,
                                      const char**      argv)
{
  Standard_Integer Repeat, Size;
  if (CheckArguments(di, argc, argv, Repeat, Size))
  {
    return 1;
  }
  CompMap(di, Repeat, Size);
  return 0;
}

//=================================================================================================

static Standard_Integer QANColPerfDataMap(Draw_Interpretor& di,
                                          Standard_Integer  argc,
                                          const char**      argv)
{
  Standard_Integer Repeat, Size;
  if (CheckArguments(di, argc, argv, Repeat, Size))
  {
    return 1;
  }
  CompDataMap(di, Repeat, Size);
  return 0;
}

//=================================================================================================

static Standard_Integer QANColPerfDoubleMap(Draw_Interpretor& di,
                                            Standard_Integer  argc,
                                            const char**      argv)
{
  Standard_Integer Repeat, Size;
  if (CheckArguments(di, argc, argv, Repeat, Size))
  {
    return 1;
  }
  CompDoubleMap(di, Repeat, Size);
  return 0;
}

//=================================================================================================

static Standard_Integer QANColPerfIndexedMap(Draw_Interpretor& di,
                                             Standard_Integer  argc,
                                             const char**      argv)
{
  Standard_Integer Repeat, Size;
  if (CheckArguments(di, argc, argv, Repeat, Size))
  {
    return 1;
  }
  CompIndexedMap(di, Repeat, Size);
  return 0;
}

//=================================================================================================

static Standard_Integer QANColPerfIndexedDataMap(Draw_Interpretor& di,
                                                 Standard_Integer  argc,
                                                 const char**      argv)
{
  Standard_Integer Repeat, Size;
  if (CheckArguments(di, argc, argv, Repeat, Size))
  {
    return 1;
  }
  CompIndexedDataMap(di, Repeat, Size);
  return 0;
}

//=================================================================================================

static Standard_Integer QANColCheckSparseArray(Draw_Interpretor& di,
                                               Standard_Integer  argc,
                                               const char**      argv)
{
  Standard_Integer Repeat, Size;
  if (CheckArguments(di, argc, argv, Repeat, Size))
  {
    return 1;
  }
  CompSparseArray(di, Repeat, Size);
  return 0;
}

void QANCollection::CommandsPerf(Draw_Interpretor& theCommands)
{
  const char* group = "QANCollection";

  // from agvCollTest/src/CollectionEXE/PerfTestEXE.cxx
  theCommands.Add("QANColPerfArray1",
                  "QANColPerfArray1 Repeat Size",
                  __FILE__,
                  QANColPerfArray1,
                  group);
  theCommands.Add("QANColPerfArray2",
                  "QANColPerfArray2 Repeat Size",
                  __FILE__,
                  QANColPerfArray2,
                  group);
  theCommands.Add("QANColPerfList", "QANColPerfList Repeat Size", __FILE__, QANColPerfList, group);
  theCommands.Add("QANColPerfSequence",
                  "QANColPerfSequence Repeat Size",
                  __FILE__,
                  QANColPerfSequence,
                  group);
  theCommands.Add("QANColPerfMap", "QANColPerfMap Repeat Size", __FILE__, QANColPerfMap, group);
  theCommands.Add("QANColPerfDataMap",
                  "QANColPerfDataMap Repeat Size",
                  __FILE__,
                  QANColPerfDataMap,
                  group);
  theCommands.Add("QANColPerfDoubleMap",
                  "QANColPerfDoubleMap Repeat Size",
                  __FILE__,
                  QANColPerfDoubleMap,
                  group);
  theCommands.Add("QANColPerfIndexedMap",
                  "QANColPerfIndexedMap Repeat Size",
                  __FILE__,
                  QANColPerfIndexedMap,
                  group);
  theCommands.Add("QANColPerfIndexedDataMap",
                  "QANColPerfIndexedDataMap Repeat Size",
                  __FILE__,
                  QANColPerfIndexedDataMap,
                  group);

  theCommands.Add("QANColCheckSparseArray",
                  "QANColCheckSparseArray Repeat Size",
                  __FILE__,
                  QANColCheckSparseArray,
                  group);

  return;
}
