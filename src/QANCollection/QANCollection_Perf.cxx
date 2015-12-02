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

#include <NCollection_DefineArray1.hxx>
#include <NCollection_DefineHArray1.hxx>
////////////////////////////////DEFINE_ARRAY1(QANCollection_Array1,QANCollection_BaseCol,ItemType)
////////////////////////////////DEFINE_HARRAY1(QANCollection_HArray1,QANCollection_Array1)
DEFINE_ARRAY1(QANCollection_Array1Perf,QANCollection_BaseColPerf,ItemType)
DEFINE_HARRAY1(QANCollection_HArray1Perf,QANCollection_Array1Perf)

#include <NCollection_DefineArray2.hxx>
#include <NCollection_DefineHArray2.hxx>
////////////////////////////////DEFINE_ARRAY2(QANCollection_Array2,QANCollection_BaseCol,ItemType)
////////////////////////////////DEFINE_HARRAY2(QANCollection_HArray2,QANCollection_Array2)
DEFINE_ARRAY2(QANCollection_Array2Perf,QANCollection_BaseColPerf,ItemType)
DEFINE_HARRAY2(QANCollection_HArray2Perf,QANCollection_Array2Perf)

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
DEFINE_MAP(QANCollection_MapPerf,QANCollection_Key1BaseColPerf,Key1Type)
DEFINE_DATAMAP(QANCollection_DataMapPerf,QANCollection_BaseColPerf,Key1Type,ItemType)
DEFINE_DOUBLEMAP(QANCollection_DoubleMapPerf,QANCollection_Key2BaseColPerf,Key1Type,Key2Type)
DEFINE_INDEXEDMAP(QANCollection_IndexedMapPerf,QANCollection_Key1BaseColPerf,Key1Type)
DEFINE_INDEXEDDATAMAP(QANCollection_IDMapPerf,QANCollection_BaseColPerf,Key1Type,ItemType)

#include <NCollection_DefineList.hxx>
////////////////////////////////DEFINE_LIST(QANCollection_List,QANCollection_BaseCol,ItemType)
DEFINE_LIST(QANCollection_ListPerf,QANCollection_BaseColPerf,ItemType)

#include <NCollection_DefineSequence.hxx>
#include <NCollection_DefineHSequence.hxx>
////////////////////////////////DEFINE_SEQUENCE(QANCollection_Sequence,QANCollection_BaseCol,ItemType)
////////////////////////////////DEFINE_HSEQUENCE(QANCollection_HSequence,QANCollection_Sequence)
DEFINE_SEQUENCE(QANCollection_SequencePerf,QANCollection_BaseColPerf,ItemType)
DEFINE_HSEQUENCE(QANCollection_HSequencePerf,QANCollection_SequencePerf)

static void printAllMeters (Draw_Interpretor& theDI)
{
  char buffer[25600];
  perf_sprint_all_meters (buffer, 25600 - 1, 1);
  theDI << buffer;
}

// ===================== Test perform of Array1 type ==========================
static void CompArray1 (Draw_Interpretor& theDI,
                        const Standard_Integer theRep,
                        const Standard_Integer theSize)
{
  Standard_Integer i,j;

  ////////////////////////////////Perf_Meter aNCrea ("NCollection_Array1 creation",0);
  ////////////////////////////////Perf_Meter aTCrea ("TCollection_Array1 creation",0);
  ////////////////////////////////Perf_Meter aNFill ("NCollection_Array1 filling",0);
  ////////////////////////////////Perf_Meter aTFill ("TCollection_Array1 filling",0);
  ////////////////////////////////Perf_Meter aNFind ("NCollection_Array1 finding",0);
  ////////////////////////////////Perf_Meter aTFind ("TCollection_Array1 finding",0);
  ////////////////////////////////Perf_Meter aNOper ("NCollection_Array1 operator=",0);
  ////////////////////////////////Perf_Meter aTOper ("TCollection_Array1 operator=",0);
  ////////////////////////////////Perf_Meter aNAssi ("NCollection_Array1 Assign",0);
  for (i=0; i<theRep; i++)
    {
      ////////////////////////////////aNCrea.Start();
      PERF_START_METER("NCollection_Array1 creation")
      ////////////////////////////////QANCollection_Array1 a1(1,theSize), a2(1,theSize);
      QANCollection_Array1Perf a1(1,theSize), a2(1,theSize);
      ////////////////////////////////aNCrea.Stop();
      PERF_STOP_METER("NCollection_Array1 creation")
      ////////////////////////////////aNFill.Start();
      PERF_START_METER("NCollection_Array1 filling")
      for (j=1; j<=theSize; j++)
        Random(a1(j));
      ////////////////////////////////aNFill.Stop();
      PERF_STOP_METER("NCollection_Array1 filling")
      ////////////////////////////////aNFind.Start();
      PERF_START_METER("NCollection_Array1 finding")
      for (j=1; j<=theSize; j++)
        {
          Standard_Integer iIndex;
          Random(iIndex,theSize);
          a1.Value(iIndex+1);
        }
      ////////////////////////////////aNFind.Stop();
      PERF_STOP_METER("NCollection_Array1 finding")
      ////////////////////////////////aNOper.Start();
      PERF_START_METER("NCollection_Array1 operator=")
      a2 = a1;
      ////////////////////////////////aNOper.Stop();
      PERF_STOP_METER("NCollection_Array1 operator=")
      ////////////////////////////////aNAssi.Start();
      PERF_START_METER("NCollection_Array1 Assign")
      a2.Assign(a1);
      ////////////////////////////////aNAssi.Stop();
      PERF_STOP_METER("NCollection_Array1 Assign")
    }

  for (i=0; i<theRep; i++)
    {
      ////////////////////////////////aTCrea.Start();
      PERF_START_METER("TCollection_Array1 creation")
      TColgp_Array1OfPnt a1(1,theSize), a2(1,theSize);
      ////////////////////////////////aTCrea.Stop();
      PERF_STOP_METER("TCollection_Array1 creation")
      ////////////////////////////////aTFill.Start();
      PERF_START_METER("TCollection_Array1 filling")
      for (j=1; j<=theSize; j++)
        Random(a1(j));
      ////////////////////////////////aTFill.Stop();
      PERF_STOP_METER("TCollection_Array1 filling")
      ////////////////////////////////aTFind.Start();
      PERF_START_METER("TCollection_Array1 finding")
      for (j=1; j<=theSize; j++)
        {
          Standard_Integer iIndex;
          Random(iIndex,theSize);
          a1.Value(iIndex+1);
        }
      ////////////////////////////////aTFind.Stop();
      PERF_STOP_METER("TCollection_Array1 finding")
      ////////////////////////////////aTOper.Start();
      PERF_START_METER("TCollection_Array1 operator=")
      a2 = a1;
      ////////////////////////////////aTOper.Stop();
      PERF_STOP_METER("TCollection_Array1 operator=")
    }
  printAllMeters(theDI);
}

// ===================== Test perform of Array2 type ==========================
static void CompArray2 (Draw_Interpretor& theDI,
                        const Standard_Integer theRep,
                        const Standard_Integer theSize)
{
  Standard_Integer i,j,k;
  
  ////////////////////////////////Perf_Meter aNCrea ("NCollection_Array2 creation",0);
  ////////////////////////////////Perf_Meter aTCrea ("TCollection_Array2 creation",0);
  ////////////////////////////////Perf_Meter aNFill ("NCollection_Array2 filling",0);
  ////////////////////////////////Perf_Meter aTFill ("TCollection_Array2 filling",0);
  ////////////////////////////////Perf_Meter aNFind ("NCollection_Array2 finding",0);
  ////////////////////////////////Perf_Meter aTFind ("TCollection_Array2 finding",0);
  ////////////////////////////////Perf_Meter aNOper ("NCollection_Array2 operator=",0);
  ////////////////////////////////Perf_Meter aTOper ("TCollection_Array2 operator=",0);
  ////////////////////////////////Perf_Meter aNAssi ("NCollection_Array2 Assign",0);
  for (i=0; i<theRep; i++)
    {
      ////////////////////////////////aNCrea.Start();
      PERF_START_METER("NCollection_Array2 creation")
      ////////////////////////////////QANCollection_Array2 a1(1,theSize,1,theSize), a2(1,theSize,1,theSize);
      QANCollection_Array2Perf a1(1,theSize,1,theSize), a2(1,theSize,1,theSize);
      ////////////////////////////////aNCrea.Stop();
      PERF_STOP_METER("NCollection_Array2 creation")
      ////////////////////////////////aNFill.Start();
      PERF_START_METER("NCollection_Array2 filling")
      for (j=1; j<=theSize; j++)
        for (k=1; k<=theSize; k++)
          Random(a1(j,k));
      ////////////////////////////////aNFill.Stop();
      PERF_STOP_METER("NCollection_Array2 filling")
      ////////////////////////////////aNFind.Start();
      PERF_START_METER("NCollection_Array2 finding")
      for (j=1; j<=theSize*theSize; j++)
        {
          Standard_Integer m,n;
          Random(m,theSize);
          Random(n,theSize);
          a1.Value(m+1,n+1);
        }
      ////////////////////////////////aNFind.Stop();
      PERF_STOP_METER("NCollection_Array2 finding")
      ////////////////////////////////aNOper.Start();
      PERF_START_METER("NCollection_Array2 operator=")
      a2 = a1;
      ////////////////////////////////aNOper.Stop();
      PERF_STOP_METER("NCollection_Array2 operator=")
      ////////////////////////////////aNAssi.Start();
      PERF_START_METER("NCollection_Array2 Assign")
      a2.Assign(a1);
      ////////////////////////////////aNAssi.Stop();
      PERF_STOP_METER("NCollection_Array2 Assign")
    }

  for (i=0; i<theRep; i++)
    {
      ////////////////////////////////aTCrea.Start();
      PERF_START_METER("TCollection_Array2 creation")
      TColgp_Array2OfPnt a1(1,theSize,1,theSize), a2(1,theSize,1,theSize);
      ////////////////////////////////aTCrea.Stop();
      PERF_STOP_METER("TCollection_Array2 creation")
      ////////////////////////////////aTFill.Start();
      PERF_START_METER("TCollection_Array2 filling")
      for (j=1; j<=theSize; j++)
        for (k=1; k<=theSize; k++)
          Random(a1(j,k));
      ////////////////////////////////aTFill.Stop();
      PERF_STOP_METER("TCollection_Array2 filling")
      ////////////////////////////////aTFind.Start();
      PERF_START_METER("TCollection_Array2 finding")
      for (j=1; j<=theSize*theSize; j++)
        {
          Standard_Integer m,n;
          Random(m,theSize);
          Random(n,theSize);
          a1.Value(m+1,n+1);
        }
      ////////////////////////////////aTFind.Stop();
      PERF_STOP_METER("TCollection_Array2 finding")
      ////////////////////////////////aTOper.Start();
      PERF_START_METER("TCollection_Array2 operator=")
      a2 = a1;
      ////////////////////////////////aTOper.Stop();
      PERF_STOP_METER("TCollection_Array2 operator=")
    }
  printAllMeters(theDI);
}

// ===================== Test perform of List type ==========================
static void CompList (Draw_Interpretor& theDI,
                      const Standard_Integer theRep,
                      const Standard_Integer theSize)
{
  Standard_Integer i,j;

  ////////////////////////////////Perf_Meter aNAppe ("NCollection_List appending",0);
  ////////////////////////////////Perf_Meter aTAppe ("TCollection_List appending",0);
  ////////////////////////////////Perf_Meter aNOper ("NCollection_List operator=",0);
  ////////////////////////////////Perf_Meter aTOper ("TCollection_List operator=",0);
  ////////////////////////////////Perf_Meter aNClea ("NCollection_List clearing",0);
  ////////////////////////////////Perf_Meter aTClea ("TCollection_List clearing",0);
  ////////////////////////////////Perf_Meter aNAssi ("NCollection_List Assign",0);
  for (i=0; i<theRep; i++)
    {
      ////////////////////////////////QANCollection_List a1, a2;
      QANCollection_ListPerf a1, a2;
      ////////////////////////////////aNAppe.Start();
      PERF_START_METER("NCollection_List appending")
      for (j=1; j<=theSize; j++)
        {
          ItemType anItem;
          Random(anItem);
          a1.Append(anItem);
        }
      ////////////////////////////////aNAppe.Stop();
      PERF_STOP_METER("NCollection_List appending")
      ////////////////////////////////aNOper.Start();
      PERF_START_METER("NCollection_List operator=")
      a2 = a1;
      ////////////////////////////////aNOper.Stop();
      PERF_STOP_METER("NCollection_List operator=")
      ////////////////////////////////aNAssi.Start();
      PERF_START_METER("NCollection_List Assign")
      a2.Assign(a1);
      ////////////////////////////////aNAssi.Stop();
      PERF_STOP_METER("NCollection_List Assign")
      ////////////////////////////////aNClea.Start();
      PERF_START_METER("NCollection_List clearing")
      a2.Clear();
      ////////////////////////////////aNClea.Stop();
      PERF_STOP_METER("NCollection_List clearing")
    }

  for (i=0; i<theRep; i++)
    {
      QANCollection_ListOfPnt a1, a2;
      ////////////////////////////////aTAppe.Start();
      PERF_START_METER("TCollection_List appending")
      for (j=1; j<=theSize; j++)
        {
          ItemType anItem;
          Random(anItem);
          a1.Append(anItem);
        }
      ////////////////////////////////aTAppe.Stop();
      PERF_STOP_METER("TCollection_List appending")
      ////////////////////////////////aTOper.Start();
      PERF_START_METER("TCollection_List operator=")
      a2 = a1;
      ////////////////////////////////aTOper.Stop();
      PERF_STOP_METER("TCollection_List operator=")
      ////////////////////////////////aTClea.Start();
      PERF_START_METER("TCollection_List clearing")
      a2.Clear();
      ////////////////////////////////aTClea.Stop();
      PERF_STOP_METER("TCollection_List clearing")
    }
  printAllMeters(theDI);
}

// ===================== Test perform of Sequence type ==========================
static void CompSequence (Draw_Interpretor& theDI,
                          const Standard_Integer theRep,
                          const Standard_Integer theSize)
{
  Standard_Integer i,j;

  ////////////////////////////////Perf_Meter aNAppe ("NCollection_Sequence appending",0);
  ////////////////////////////////Perf_Meter aTAppe ("TCollection_Sequence appending",0);
  ////////////////////////////////Perf_Meter aNFind ("NCollection_Sequence finding",0);
  ////////////////////////////////Perf_Meter aTFind ("TCollection_Sequence finding",0);
  ////////////////////////////////Perf_Meter aNOper ("NCollection_Sequence operator=",0);
  ////////////////////////////////Perf_Meter aTOper ("TCollection_Sequence operator=",0);
  ////////////////////////////////Perf_Meter aNClea ("NCollection_Sequence clearing",0);
  ////////////////////////////////Perf_Meter aTClea ("TCollection_Sequence clearing",0);
  ////////////////////////////////Perf_Meter aNAssi ("NCollection_Sequence Assign",0);
  for (i=0; i<theRep; i++)
    {
      ////////////////////////////////QANCollection_Sequence a1, a2;
      QANCollection_SequencePerf a1, a2;
      ////////////////////////////////aNAppe.Start();
      PERF_START_METER("NCollection_Sequence appending")
      for (j=1; j<=theSize; j++)
        {
          ItemType anItem;
          Random(anItem);
          a1.Append(anItem);
        }
      ////////////////////////////////aNAppe.Stop();
      PERF_STOP_METER("NCollection_Sequence appending")
      ////////////////////////////////aNFind.Start();
      PERF_START_METER("NCollection_Sequence finding")
      for (j=1; j<=theSize; j++)
        {
          Standard_Integer iIndex;
          Random(iIndex,theSize);
          a1.Value(iIndex+1);
        }
      ////////////////////////////////aNFind.Stop();
      PERF_STOP_METER("NCollection_Sequence finding")
      ////////////////////////////////aNOper.Start();
      PERF_START_METER("NCollection_Sequence operator=")
      a2 = a1;
      ////////////////////////////////aNOper.Stop();
      PERF_STOP_METER("NCollection_Sequence operator=")
      ////////////////////////////////aNAssi.Start();
      PERF_START_METER("NCollection_Sequence Assign")
      a2.Assign(a1);
      ////////////////////////////////aNAssi.Stop();
      PERF_STOP_METER("NCollection_Sequence Assign")
      ////////////////////////////////aNClea.Start();
      PERF_START_METER("NCollection_Sequence clearing")
      a2.Clear();
      ////////////////////////////////aNClea.Stop();
      PERF_STOP_METER("NCollection_Sequence clearing")
    }

  for (i=0; i<theRep; i++)
    {
      TColgp_SequenceOfPnt a1, a2;
      ////////////////////////////////aTAppe.Start();
      PERF_START_METER("TCollection_Sequence appending")
      for (j=1; j<=theSize; j++)
        {
          ItemType anItem;
          Random(anItem);
          a1.Append(anItem);
        }
      ////////////////////////////////aTAppe.Stop();
      PERF_STOP_METER("TCollection_Sequence appending")
      ////////////////////////////////aTFind.Start();
      PERF_START_METER("TCollection_Sequence finding")
      for (j=1; j<=theSize; j++)
        {
          Standard_Integer iIndex;
          Random(iIndex,theSize);
          a1.Value(iIndex+1);
        }
      ////////////////////////////////aTFind.Stop();
      PERF_STOP_METER("TCollection_Sequence finding")
      ////////////////////////////////aTOper.Start();
      PERF_START_METER("TCollection_Sequence operator=")
      a2 = a1;
      ////////////////////////////////aTOper.Stop();
      PERF_STOP_METER("TCollection_Sequence operator=")
      ////////////////////////////////aTClea.Start();
      PERF_START_METER("TCollection_Sequence clearing")
      a2.Clear();
      ////////////////////////////////aTClea.Stop();
      PERF_STOP_METER("TCollection_Sequence clearing")
    }
  printAllMeters(theDI);
}

// ===================== Test perform of Map type ==========================
static void CompMap (Draw_Interpretor& theDI,
                     const Standard_Integer theRep,
                     const Standard_Integer theSize)
{
  Standard_Integer i,j;

  ////////////////////////////////Perf_Meter aNBind ("NCollection_Map adding",0);
  ////////////////////////////////Perf_Meter aTBind ("TCollection_Map adding",0);
  ////////////////////////////////Perf_Meter aNOper ("NCollection_Map operator=",0);
  ////////////////////////////////Perf_Meter aTOper ("TCollection_Map operator=",0);
  ////////////////////////////////Perf_Meter aNFind ("NCollection_Map finding",0);
  ////////////////////////////////Perf_Meter aTFind ("TCollection_Map finding",0);
  ////////////////////////////////Perf_Meter aNClea ("NCollection_Map clearing",0);
  ////////////////////////////////Perf_Meter aTClea ("TCollection_Map clearing",0);
  ////////////////////////////////Perf_Meter aNAssi ("NCollection_Map Assign",0);
  for (i=0; i<theRep; i++)
    {
      ////////////////////////////////QANCollection_Map a1, a2;
      QANCollection_MapPerf a1, a2;
      ////////////////////////////////aNBind.Start();
      PERF_START_METER("NCollection_Map adding")
      for (j=1; j<=theSize; j++)
        {
          Key1Type aKey1;
          Random(aKey1);
          a1.Add(aKey1);
        }
      ////////////////////////////////aNBind.Stop();
      PERF_STOP_METER("NCollection_Map adding")
      ////////////////////////////////aNFind.Start();
      PERF_START_METER("NCollection_Map finding")
      for (j=1; j<=theSize; j++)
        {
          Key1Type aKey1;
          Random(aKey1);
          a1.Contains(aKey1);
        }
      ////////////////////////////////aNFind.Stop();
      PERF_STOP_METER("NCollection_Map finding")
      ////////////////////////////////aNOper.Start();
      PERF_START_METER("NCollection_Map operator=")
      a2 = a1;
      ////////////////////////////////aNOper.Stop();
      PERF_STOP_METER("NCollection_Map operator=")
      ////////////////////////////////aNAssi.Start();
      PERF_START_METER("NCollection_Map Assign")
      a2.Assign(a1);
      ////////////////////////////////aNAssi.Stop();
      PERF_STOP_METER("NCollection_Map Assign")
      ////////////////////////////////aNClea.Start();
      PERF_START_METER("NCollection_Map clearing")
      a2.Clear();
      ////////////////////////////////aNClea.Stop();
      PERF_STOP_METER("NCollection_Map clearing")
    }

  for (i=0; i<theRep; i++)
    {
      TColStd_MapOfReal a1, a2;
      ////////////////////////////////aTBind.Start();
      PERF_START_METER("TCollection_Map adding")
      for (j=1; j<=theSize; j++)
        {
          Key1Type aKey1;
          Random(aKey1);
          a1.Add(aKey1);
        }
      ////////////////////////////////aTBind.Stop();
      PERF_STOP_METER("TCollection_Map adding")
      ////////////////////////////////aTFind.Start();
      PERF_START_METER("TCollection_Map finding")
      for (j=1; j<=theSize; j++)
        {
          Key1Type aKey1;
          Random(aKey1);
          a1.Contains(aKey1);
        }
      ////////////////////////////////aTFind.Stop();
      PERF_STOP_METER("TCollection_Map finding")
      ////////////////////////////////aTOper.Start();
      PERF_START_METER("TCollection_Map operator=")
      a2 = a1;
      ////////////////////////////////aTOper.Stop();
      PERF_STOP_METER("TCollection_Map operator=")
      ////////////////////////////////aTClea.Start();
      PERF_START_METER("TCollection_Map clearing")
      a2.Clear();
      ////////////////////////////////aTClea.Stop();
      PERF_STOP_METER("TCollection_Map clearing")
    }
  printAllMeters(theDI);
}

// ===================== Test perform of DataMap type ==========================
static void CompDataMap (Draw_Interpretor& theDI,
                         const Standard_Integer theRep,
                         const Standard_Integer theSize)
{
  Standard_Integer i,j;

  ////////////////////////////////Perf_Meter aNBind ("NCollection_DataMap binding",0);
  ////////////////////////////////Perf_Meter aTBind ("TCollection_DataMap binding",0);
  ////////////////////////////////Perf_Meter aNFind ("NCollection_DataMap finding",0);
  ////////////////////////////////Perf_Meter aTFind ("TCollection_DataMap finding",0);
  ////////////////////////////////Perf_Meter aNOper ("NCollection_DataMap operator=",0);
  ////////////////////////////////Perf_Meter aTOper ("TCollection_DataMap operator=",0);
  ////////////////////////////////Perf_Meter aNClea ("NCollection_DataMap clearing",0);
  ////////////////////////////////Perf_Meter aTClea ("TCollection_DataMap clearing",0);
  //////////////////////////////////Perf_Meter aNAssi ("NCollection_DataMap Assign",0);
  for (i=0; i<theRep; i++)
    {
      ////////////////////////////////QANCollection_DataMap a1, a2;
      QANCollection_DataMapPerf a1, a2;
      ////////////////////////////////aNBind.Start();
      PERF_START_METER("NCollection_DataMap binding")
      for (j=1; j<=theSize; j++)
        {
          Key1Type aKey1;
          ItemType anItem;
          Random(aKey1);
          Random(anItem);
          a1.Bind(aKey1,anItem);
        }
      ////////////////////////////////aNBind.Stop();
      PERF_STOP_METER("NCollection_DataMap binding")
      ////////////////////////////////aNFind.Start();
      PERF_START_METER("NCollection_DataMap finding")
      for (j=1; j<=theSize; j++)
        {
          Key1Type aKey1;
          Random(aKey1);
          a1.IsBound(aKey1);
        }
      ////////////////////////////////aNFind.Stop();
      PERF_STOP_METER("NCollection_DataMap finding")
      ////////////////////////////////aNOper.Start();
      PERF_START_METER("NCollection_DataMap operator=")
      a2 = a1;
      ////////////////////////////////aNOper.Stop();
      PERF_STOP_METER("NCollection_DataMap operator=")
      //aNAssi.Start();
      //a2.Assign(a1);
      //aNAssi.Stop();
      ////////////////////////////////aNClea.Start();
      PERF_START_METER("NCollection_DataMap clearing")
      a2.Clear();
      ////////////////////////////////aNClea.Stop();
      PERF_STOP_METER("NCollection_DataMap clearing")
    }

  for (i=0; i<theRep; i++)
    {
      QANCollection_DataMapOfRealPnt a1, a2;
      ////////////////////////////////aTBind.Start();
      PERF_START_METER("TCollection_DataMap binding")
      for (j=1; j<=theSize; j++)
        {
          Key1Type aKey1;
          ItemType anItem;
          Random(aKey1);
          Random(anItem);
          a1.Bind(aKey1,anItem);
        }
      ////////////////////////////////aTBind.Stop();
      PERF_STOP_METER("TCollection_DataMap binding")
      ////////////////////////////////aTFind.Start();
      PERF_START_METER("TCollection_DataMap finding")
      for (j=1; j<=theSize; j++)
        {
          Key1Type aKey1;
          Random(aKey1);
          a1.IsBound(aKey1);
        }
      ////////////////////////////////aTFind.Stop();
      PERF_STOP_METER("TCollection_DataMap finding")
      ////////////////////////////////aTOper.Start();
      PERF_START_METER("TCollection_DataMap operator=")
      a2 = a1;
      ////////////////////////////////aTOper.Stop();
      PERF_STOP_METER("TCollection_DataMap operator=")
      ////////////////////////////////aTClea.Start();
      PERF_START_METER("TCollection_DataMap clearing")
      a2.Clear();
      ////////////////////////////////aTClea.Stop();
      PERF_STOP_METER("TCollection_DataMap clearing")
    }
  printAllMeters(theDI);
}

// ===================== Test perform of DoubleMap type ==========================
static void CompDoubleMap (Draw_Interpretor& theDI,
                           const Standard_Integer theRep,
                           const Standard_Integer theSize)
{
  Standard_Integer i,j;
  Standard_Integer iFail1=0, iFail2=0;

  ////////////////////////////////Perf_Meter aNBind ("NCollection_DoubleMap binding",0);
  ////////////////////////////////Perf_Meter aTBind ("TCollection_DoubleMap binding",0);
  ////////////////////////////////Perf_Meter aNFind ("NCollection_DoubleMap finding",0);
  ////////////////////////////////Perf_Meter aTFind ("TCollection_DoubleMap finding",0);
  ////////////////////////////////Perf_Meter aNOper ("NCollection_DoubleMap operator=",0);
  ////////////////////////////////Perf_Meter aTOper ("TCollection_DoubleMap operator=",0);
  ////////////////////////////////Perf_Meter aNClea ("NCollection_DoubleMap clearing",0);
  ////////////////////////////////Perf_Meter aTClea ("TCollection_DoubleMap clearing",0);
  //////////////////////////////////Perf_Meter aNAssi ("NCollection_DoubleMap Assign",0);
  for (i=0; i<theRep; i++)
    {
      ////////////////////////////////QANCollection_DoubleMap a1, a2;
      QANCollection_DoubleMapPerf a1, a2;
      ////////////////////////////////aNBind.Start();
      PERF_START_METER("NCollection_DoubleMap binding")
      for (j=1; j<=theSize; j++)
        {
          Key1Type aKey1;
          Key2Type aKey2;
          do {
            Random(aKey1);
            Random(aKey2);
            iFail1++;
          }
          while (a1.IsBound1(aKey1) || a1.IsBound2(aKey2));
          iFail1--;
          a1.Bind(aKey1,aKey2);
        }
      ////////////////////////////////aNBind.Stop();
      PERF_STOP_METER("NCollection_DoubleMap binding")
      ////////////////////////////////aNFind.Start();
      PERF_START_METER("NCollection_DoubleMap finding")
      for (j=1; j<=theSize; j++)
        {
          Key1Type aKey1;
          Key2Type aKey2;
          Random(aKey1);
          Random(aKey2);
          a1.AreBound(aKey1,aKey2);
        }
      ////////////////////////////////aNFind.Stop();
      PERF_STOP_METER("NCollection_DoubleMap finding")
      ////////////////////////////////aNOper.Start();
      PERF_START_METER("NCollection_DoubleMap operator=")
      a2 = a1;
      ////////////////////////////////aNOper.Stop();
      PERF_STOP_METER("NCollection_DoubleMap operator=")
      //aNAssi.Start();
      //a2.Assign(a1);
      //aNAssi.Stop();
      ////////////////////////////////aNClea.Start();
      PERF_START_METER("NCollection_DoubleMap clearing")
      a2.Clear();
      ////////////////////////////////aNClea.Stop();
      PERF_STOP_METER("NCollection_DoubleMap clearing")
    }

  for (i=0; i<theRep; i++)
    {
      QANCollection_DoubleMapOfRealInteger a1, a2;
      ////////////////////////////////aTBind.Start();
      PERF_START_METER("TCollection_DoubleMap binding")
      for (j=1; j<=theSize; j++)
        {
          Key1Type aKey1;
          Key2Type aKey2;
          do {
            Random(aKey1);
            Random(aKey2);
            iFail2++;
          }
          while (a1.IsBound1(aKey1) || a1.IsBound2(aKey2));
          iFail2--;
          a1.Bind(aKey1,aKey2);
        }
      ////////////////////////////////aTBind.Stop();
      PERF_STOP_METER("TCollection_DoubleMap binding")
      ////////////////////////////////aTFind.Start();
      PERF_START_METER("TCollection_DoubleMap finding")
      for (j=1; j<=theSize; j++)
        {
          Key1Type aKey1;
          Key2Type aKey2;
          Random(aKey1);
          Random(aKey2);
          a1.AreBound(aKey1,aKey2);
        }
      ////////////////////////////////aTFind.Stop();
      PERF_STOP_METER("TCollection_DoubleMap finding")
      ////////////////////////////////aTOper.Start();
      PERF_START_METER("TCollection_DoubleMap operator=")
      a2 = a1;
      ////////////////////////////////aTOper.Stop();
      PERF_STOP_METER("TCollection_DoubleMap operator=")
      ////////////////////////////////aTClea.Start();
      PERF_START_METER("TCollection_DoubleMap clearing")
      a2.Clear();
      ////////////////////////////////aTClea.Stop();
      PERF_STOP_METER("TCollection_DoubleMap clearing")
    }
  printAllMeters(theDI);
  if (iFail1 || iFail2)
    cout << "Warning : N map failed " << iFail1 << " times, T map - " << 
      iFail2 << endl;
}

// ===================== Test perform of IndexedMap type ==========================
static void CompIndexedMap (Draw_Interpretor& theDI,
                            const Standard_Integer theRep,
                            const Standard_Integer theSize)
{
  Standard_Integer i,j;

  ////////////////////////////////Perf_Meter aNBind ("NCollection_IndexedMap adding",0);
  ////////////////////////////////Perf_Meter aTBind ("TCollection_IndexedMap adding",0);
  ////////////////////////////////Perf_Meter aNOper ("NCollection_IndexedMap operator=",0);
  ////////////////////////////////Perf_Meter aTOper ("TCollection_IndexedMap operator=",0);
  ////////////////////////////////Perf_Meter aNFind ("NCollection_IndexedMap finding",0);
  ////////////////////////////////Perf_Meter aTFind ("TCollection_IndexedMap finding",0);
  ////////////////////////////////Perf_Meter aNClea ("NCollection_IndexedMap clearing",0);
  ////////////////////////////////Perf_Meter aTClea ("TCollection_IndexedMap clearing",0);
  ////////////////////////////////Perf_Meter aNAssi ("NCollection_IndexedMap Assign",0);

  for (i=0; i<theRep; i++)
    {
      ////////////////////////////////QANCollection_IndexedMap a1, a2;
      QANCollection_IndexedMapPerf a1, a2;
      ////////////////////////////////aNBind.Start();
      PERF_START_METER("NCollection_IndexedMap adding")
      for (j=1; j<=theSize; j++)
        {
          Key1Type aKey1;
          Random(aKey1);
          a1.Add(aKey1);
        }
      ////////////////////////////////aNBind.Stop();
      PERF_STOP_METER("NCollection_IndexedMap adding")
      ////////////////////////////////aNFind.Start();
      PERF_START_METER("NCollection_IndexedMap finding")
      for (j=1; j<=theSize; j++)
        {
          Key1Type aKey1;
          Random(aKey1);
          a1.Contains(aKey1);
        }
      ////////////////////////////////aNFind.Stop();
      PERF_STOP_METER("NCollection_IndexedMap finding")
      ////////////////////////////////aNOper.Start();
      PERF_START_METER("NCollection_IndexedMap operator=")
      a2 = a1;
      ////////////////////////////////aNOper.Stop();
      PERF_STOP_METER("NCollection_IndexedMap operator=")
      ////////////////////////////////aNAssi.Start();
      PERF_START_METER("NCollection_IndexedMap Assign")
      a2.Assign(a1);
      ////////////////////////////////aNAssi.Stop();
      PERF_STOP_METER("NCollection_IndexedMap Assign")
      ////////////////////////////////aNClea.Start();
      PERF_START_METER("NCollection_IndexedMap clearing")
      a2.Clear();
      ////////////////////////////////aNClea.Stop();
      PERF_STOP_METER("NCollection_IndexedMap clearing")
    }

  for (i=0; i<theRep; i++)
    {
      TColStd_IndexedMapOfReal a1, a2;
      ////////////////////////////////aTBind.Start();
      PERF_START_METER("TCollection_IndexedMap adding")
      for (j=1; j<=theSize; j++)
        {
          Key1Type aKey1;
          Random(aKey1);
          a1.Add(aKey1);
        }
      ////////////////////////////////aTBind.Stop();
      PERF_STOP_METER("TCollection_IndexedMap adding")
      ////////////////////////////////aTFind.Start();
      PERF_START_METER("TCollection_IndexedMap finding")
      for (j=1; j<=theSize; j++)
        {
          Key1Type aKey1;
          Random(aKey1);
          a1.Contains(aKey1);
        }
      ////////////////////////////////aTFind.Stop();
      PERF_STOP_METER("TCollection_IndexedMap finding")
      ////////////////////////////////aTOper.Start();
      PERF_START_METER("TCollection_IndexedMap operator=")
      a2 = a1;
      ////////////////////////////////aTOper.Stop();
      PERF_STOP_METER("TCollection_IndexedMap operator=")
      ////////////////////////////////aTClea.Start();
      PERF_START_METER("TCollection_IndexedMap clearing")
      a2.Clear();
      ////////////////////////////////aTClea.Stop();
      PERF_STOP_METER("TCollection_IndexedMap clearing")
    }
  printAllMeters(theDI);
}

// ===================== Test perform of IndexedDataMap type ==========================
static void CompIndexedDataMap (Draw_Interpretor& theDI,
                                const Standard_Integer theRep,
                                const Standard_Integer theSize)
{
  Standard_Integer i,j;

  ////////////////////////////////Perf_Meter aNBind ("NCollection_IndexedDataMap binding",0);
  ////////////////////////////////Perf_Meter aTBind ("TCollection_IndexedDataMap binding",0);
  ////////////////////////////////Perf_Meter aNFind ("NCollection_IndexedDataMap finding",0);
  ////////////////////////////////Perf_Meter aTFind ("TCollection_IndexedDataMap finding",0);
  ////////////////////////////////Perf_Meter aNOper ("NCollection_IndexedDataMap operator=",0);
  ////////////////////////////////Perf_Meter aTOper ("TCollection_IndexedDataMap operator=",0);
  ////////////////////////////////Perf_Meter aNClea ("NCollection_IndexedDataMap clearing",0);
  ////////////////////////////////Perf_Meter aTClea ("TCollection_IndexedDataMap clearing",0);
  //////////////////////////////////Perf_Meter aNAssi ("NCollection_IndexedDataMap Assign",0);

  for (i=0; i<theRep; i++)
    {
      ////////////////////////////////QANCollection_IDMap a1, a2;
      QANCollection_IDMapPerf a1, a2;
      ////////////////////////////////aNBind.Start();
      PERF_START_METER("NCollection_IndexedDataMap binding")
      for (j=1; j<=theSize; j++)
        {
          Key1Type aKey1;
          ItemType anItem;
          Random(aKey1);
          Random(anItem);
          a1.Add(aKey1,anItem);
        }
      ////////////////////////////////aNBind.Stop();
      PERF_STOP_METER("NCollection_IndexedDataMap binding")
      ////////////////////////////////aNFind.Start();
      PERF_START_METER("NCollection_IndexedDataMap finding")
      for (j=1; j<=theSize; j++)
        {
          Key1Type aKey1;
          Random(aKey1);
          a1.Contains(aKey1);
        }
      ////////////////////////////////aNFind.Stop();
      PERF_STOP_METER("NCollection_IndexedDataMap finding")
      ////////////////////////////////aNOper.Start();
      PERF_START_METER("NCollection_IndexedDataMap operator=")
      a2 = a1;
      ////////////////////////////////aNOper.Stop();
      PERF_STOP_METER("NCollection_IndexedDataMap operator=")
      //aNAssi.Start();
      //a2.Assign(a1);
      //aNAssi.Stop();
      ////////////////////////////////aNClea.Start();
      PERF_START_METER("NCollection_IndexedDataMap clearing")
      a2.Clear();
      ////////////////////////////////aNClea.Stop();
      PERF_STOP_METER("NCollection_IndexedDataMap clearing")
    }

  for (i=0; i<theRep; i++)
    {
      QANCollection_IndexedDataMapOfRealPnt a1, a2;
      ////////////////////////////////aTBind.Start();
      PERF_START_METER("TCollection_IndexedDataMap binding")
      for (j=1; j<=theSize; j++)
        {
          Key1Type aKey1;
          ItemType anItem;
          Random(aKey1);
          Random(anItem);
          a1.Add(aKey1,anItem);
        }
      ////////////////////////////////aTBind.Stop();
      PERF_STOP_METER("TCollection_IndexedDataMap binding")
      ////////////////////////////////aTFind.Start();
      PERF_START_METER("TCollection_IndexedDataMap finding")
      for (j=1; j<=theSize; j++)
        {
          Key1Type aKey1;
          Random(aKey1);
          a1.Contains(aKey1);
        }
      ////////////////////////////////aTFind.Stop();
      PERF_STOP_METER("TCollection_IndexedDataMap finding")
      ////////////////////////////////aTOper.Start();
      PERF_START_METER("TCollection_IndexedDataMap operator=")
      a2 = a1;
      ////////////////////////////////aTOper.Stop();
      PERF_STOP_METER("TCollection_IndexedDataMap operator=")
      ////////////////////////////////aTClea.Start();
      PERF_START_METER("TCollection_IndexedDataMap clearing")
      a2.Clear();
      ////////////////////////////////aTClea.Stop();
      PERF_STOP_METER("TCollection_IndexedDataMap clearing")
    }
  printAllMeters(theDI);
}

// ===================== Test perform of SparseArray type ==========================
static void CompSparseArray (Draw_Interpretor& theDI,
                             const Standard_Integer theRep, 
                             const Standard_Integer theSize)
{
  Standard_Integer i,j;
  for (i=0; i<theRep; i++)
    {
      PERF_START_METER("NCollection_SparseArray creation")

      NCollection_SparseArray<Standard_Integer> a1(theSize),a2(theSize);
      
      PERF_STOP_METER("NCollection_SparseArray creation")
        
      PERF_START_METER("NCollection_SparseArray filling")
      for( j=0;j<theSize;j++ )
      {
        Standard_Integer iIndex;
        Random(iIndex,theSize);
        a1.SetValue(j,iIndex+1);
      }
      
      PERF_STOP_METER("NCollection_SparseArray filling")
      
      PERF_START_METER("NCollection_SparseArray size")
      Standard_Size sizeSparseArray=a1.Size();
      (void)sizeSparseArray; // avoid compiler warning on unused variable
      PERF_STOP_METER("NCollection_SparseArray size")
      
      PERF_START_METER("NCollection_Array1 Assign")
        a2.Assign(a1);
      PERF_STOP_METER("NCollection_Array1 Assign")
      PERF_START_METER("NCollection_SparseArray HasValue")
      for (j=0; j<theSize; j++)
        {
          Standard_Integer iIndex;
          Random(iIndex,theSize);
          a2.HasValue(iIndex+1);
        }
      PERF_STOP_METER("NCollection_SparseArray HasValue")
      PERF_START_METER("NCollection_SparseArray UnsetValue")
      for (j=0; j<theSize; j++)
        {
          Standard_Integer iIndex;
          Random(iIndex,theSize);
          a1.UnsetValue(iIndex+1);
        }
      PERF_STOP_METER("NCollection_SparseArray UnsetValue")
      
      PERF_START_METER("NCollection_SparseArray Clear")
        a1.Clear();
        a2.Clear();
      PERF_STOP_METER("NCollection_SparseArray Clear")
      
    }

  printAllMeters(theDI);
}

//=======================================================================
//function : CheckArguments
//purpose  : 
//=======================================================================
Standard_Integer CheckArguments(Draw_Interpretor& di, Standard_Integer argc, const char ** argv, Standard_Integer& Repeat, Standard_Integer& Size)
{
  if ( argc != 3) {
    di << "Usage : " << argv[0] << " Repeat Size\n";
    return 1;
  }
  Repeat = Draw::Atoi(argv[1]);
  Size   = Draw::Atoi(argv[2]);
  if ( Repeat < 1 ) {
    di << "Repeat > 0\n";
    return 1;
  }
  if ( Size < 1 ) {
    di << "Size > 0\n";
    return 1;
  }
  return 0;
}


//=======================================================================
//function : QANColPerfArray1
//purpose  : 
//=======================================================================
static Standard_Integer QANColPerfArray1(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Repeat, Size;
  if ( CheckArguments(di, argc, argv, Repeat, Size) ) {
    return 1;
  }
  CompArray1 (di, Repeat, Size);
  return 0;
}

//=======================================================================
//function : QANColPerfArray2
//purpose  : 
//=======================================================================
static Standard_Integer QANColPerfArray2(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Repeat, Size;
  if ( CheckArguments(di, argc, argv, Repeat, Size) ) {
    return 1;
  }
  CompArray2 (di, Repeat, Size);
  return 0;
}

//=======================================================================
//function : QANColPerfList
//purpose  : 
//=======================================================================
static Standard_Integer QANColPerfList(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Repeat, Size;
  if ( CheckArguments(di, argc, argv, Repeat, Size) ) {
    return 1;
  }
  CompList (di, Repeat, Size);
  return 0;
}

//=======================================================================
//function : QANColPerfSequence
//purpose  : 
//=======================================================================
static Standard_Integer QANColPerfSequence(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Repeat, Size;
  if ( CheckArguments(di, argc, argv, Repeat, Size) ) {
    return 1;
  }
  CompSequence (di, Repeat, Size);
  return 0;
}

//=======================================================================
//function : QANColPerfMap
//purpose  : 
//=======================================================================
static Standard_Integer QANColPerfMap(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Repeat, Size;
  if ( CheckArguments(di, argc, argv, Repeat, Size) ) {
    return 1;
  }
  CompMap (di, Repeat, Size);
  return 0;
}

//=======================================================================
//function : QANColPerfDataMap
//purpose  : 
//=======================================================================
static Standard_Integer QANColPerfDataMap(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Repeat, Size;
  if ( CheckArguments(di, argc, argv, Repeat, Size) ) {
    return 1;
  }
  CompDataMap (di, Repeat, Size);
  return 0;
}

//=======================================================================
//function : QANColPerfDoubleMap
//purpose  : 
//=======================================================================
static Standard_Integer QANColPerfDoubleMap(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Repeat, Size;
  if ( CheckArguments(di, argc, argv, Repeat, Size) ) {
    return 1;
  }
  CompDoubleMap (di, Repeat, Size);
  return 0;
}

//=======================================================================
//function : QANColPerfIndexedMap
//purpose  : 
//=======================================================================
static Standard_Integer QANColPerfIndexedMap(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Repeat, Size;
  if ( CheckArguments(di, argc, argv, Repeat, Size) ) {
    return 1;
  }
  CompIndexedMap (di, Repeat, Size);
  return 0;
}

//=======================================================================
//function : QANColPerfIndexedDataMap
//purpose  : 
//=======================================================================
static Standard_Integer QANColPerfIndexedDataMap(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Repeat, Size;
  if ( CheckArguments(di, argc, argv, Repeat, Size) ) {
    return 1;
  }
  CompIndexedDataMap (di, Repeat, Size);
  return 0;
}

//=======================================================================
//function : QANColCheckSparseArray
//purpose  : 
//=======================================================================
static Standard_Integer QANColCheckSparseArray(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Repeat, Size;
  if ( CheckArguments(di, argc, argv, Repeat, Size) ) {
    return 1;
  }
  CompSparseArray (di, Repeat, Size);
  return 0;
}

void QANCollection::CommandsPerf(Draw_Interpretor& theCommands) {
  const char *group = "QANCollection";

  // from agvCollTest/src/CollectionEXE/PerfTestEXE.cxx
  theCommands.Add("QANColPerfArray1",         "QANColPerfArray1 Repeat Size",         __FILE__, QANColPerfArray1,         group);  
  theCommands.Add("QANColPerfArray2",         "QANColPerfArray2 Repeat Size",         __FILE__, QANColPerfArray2,         group);  
  theCommands.Add("QANColPerfList",           "QANColPerfList Repeat Size",           __FILE__, QANColPerfList,           group);  
  theCommands.Add("QANColPerfSequence",       "QANColPerfSequence Repeat Size",       __FILE__, QANColPerfSequence,       group);  
  theCommands.Add("QANColPerfMap",            "QANColPerfMap Repeat Size",            __FILE__, QANColPerfMap,            group);  
  theCommands.Add("QANColPerfDataMap",        "QANColPerfDataMap Repeat Size",        __FILE__, QANColPerfDataMap,        group);  
  theCommands.Add("QANColPerfDoubleMap",      "QANColPerfDoubleMap Repeat Size",      __FILE__, QANColPerfDoubleMap,      group);  
  theCommands.Add("QANColPerfIndexedMap",     "QANColPerfIndexedMap Repeat Size",     __FILE__, QANColPerfIndexedMap,     group);  
  theCommands.Add("QANColPerfIndexedDataMap", "QANColPerfIndexedDataMap Repeat Size", __FILE__, QANColPerfIndexedDataMap, group);  
  
  theCommands.Add("QANColCheckSparseArray",   "QANColCheckSparseArray Repeat Size",   __FILE__, QANColCheckSparseArray,   group);
  
  return;
}

