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



#ifndef QANCollection_PerfMaps_HeaderFile
#define QANCollection_PerfMaps_HeaderFile

#include <TColStd_MapOfReal.hxx>
#include <TColStd_IndexedMapOfReal.hxx>
#include <QANCollection_DataMapOfRealPnt.hxx>
#include <QANCollection_DoubleMapOfRealInteger.hxx>
#include <QANCollection_IndexedDataMapOfRealPnt.hxx>

// ===================== Test perform of Map type ==========================
void CompMap (const Standard_Integer theRep,
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
  PERF_PRINT_ALL_METERS
}

// ===================== Test perform of DataMap type ==========================
void CompDataMap (const Standard_Integer theRep,
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
  PERF_PRINT_ALL_METERS
}

// ===================== Test perform of DoubleMap type ==========================
void CompDoubleMap (const Standard_Integer theRep,
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
  PERF_PRINT_ALL_METERS
  if (iFail1 || iFail2)
    cout << "Warning : N map failed " << iFail1 << " times, T map - " << 
      iFail2 << endl;
}

// ===================== Test perform of IndexedMap type ==========================
void CompIndexedMap (const Standard_Integer theRep,
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
  PERF_PRINT_ALL_METERS
}

// ===================== Test perform of IndexedDataMap type ==========================
void CompIndexedDataMap (const Standard_Integer theRep,
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
  PERF_PRINT_ALL_METERS
}

#endif
