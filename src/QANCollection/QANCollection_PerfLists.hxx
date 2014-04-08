// Created on: 2002-04-30
// Created by: Alexander KARTOMIN (akm)
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

#ifndef QANCollection_PerfLists_HeaderFile
#define QANCollection_PerfLists_HeaderFile

#include <QANCollection_ListOfPnt.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TColStd_SetOfInteger.hxx>

// ===================== Test perform of List type ==========================
void CompList (const Standard_Integer theRep,
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
  PERF_PRINT_ALL
}


// ===================== Test perform of Set type ==========================
void CompSet (const Standard_Integer theRep,
              const Standard_Integer theSize)
{
  Standard_Integer i,j;

  ////////////////////////////////Perf_Meter aNPush ("NCollection_Set pushing",0);
  ////////////////////////////////Perf_Meter aTPush ("TCollection_Set pushing",0);
  ////////////////////////////////Perf_Meter aNFind ("NCollection_Set finding",0);
  ////////////////////////////////Perf_Meter aTFind ("TCollection_Set finding",0);
  ////////////////////////////////Perf_Meter aNOper ("NCollection_Set operator=",0);
  ////////////////////////////////Perf_Meter aTOper ("TCollection_Set operator=",0);
  ////////////////////////////////Perf_Meter aNClea ("NCollection_Set clearing",0);
  ////////////////////////////////Perf_Meter aTClea ("TCollection_Set clearing",0);
  ////////////////////////////////Perf_Meter aNAssi ("NCollection_Set Assign",0);
  for (i=0; i<theRep; i++)
    {
      ////////////////////////////////QANCollection_Set a1, a2;
      QANCollection_SetPerf a1, a2;
      ////////////////////////////////aNPush.Start();
      PERF_START_METER("NCollection_Set pushing")
      for (j=1; j<=theSize; j++)
        {
          Key2Type anItem;
          Random(anItem);
          a1.Add(anItem);
        }
      ////////////////////////////////aNPush.Stop();
      PERF_STOP_METER("NCollection_Set pushing")
      ////////////////////////////////aNFind.Start();
      PERF_START_METER("NCollection_Set finding")
      for (j=1; j<=theSize; j++)
        {
          Key2Type anItem;
          Random(anItem);
          a1.Contains(anItem);
        }
      ////////////////////////////////aNFind.Stop();
      PERF_STOP_METER("NCollection_Set finding")
      ////////////////////////////////aNOper.Start();
      PERF_START_METER("NCollection_Set operator=")
      a2 = a1;
      ////////////////////////////////aNOper.Stop();
      PERF_STOP_METER("NCollection_Set operator=")
      ////////////////////////////////aNAssi.Start();
      PERF_START_METER("NCollection_Set Assign")
      a2.Assign(a1);
      ////////////////////////////////aNAssi.Stop();
      PERF_STOP_METER("NCollection_Set Assign")
      ////////////////////////////////aNClea.Start();
      PERF_START_METER("NCollection_Set clearing")
      a2.Clear();
      ////////////////////////////////aNClea.Stop();
      PERF_STOP_METER("NCollection_Set clearing")
    }

  for (i=0; i<theRep; i++)
    {
      TColStd_SetOfInteger a1, a2;
      ////////////////////////////////aTPush.Start();
      PERF_START_METER("TCollection_Set pushing")
      for (j=1; j<=theSize; j++)
        {
          Key2Type anItem;
          Random(anItem);
          a1.Add(anItem);
        }
      ////////////////////////////////aTPush.Stop();
      PERF_STOP_METER("TCollection_Set pushing")
      ////////////////////////////////aTFind.Start();
      PERF_START_METER("TCollection_Set finding")
        for (j=1; j<=theSize; j++)
          {
            Key2Type anItem;
            Random(anItem);
            a1.Contains(anItem);
          }
      ////////////////////////////////aTFind.Stop();
      PERF_STOP_METER("TCollection_Set finding")
      ////////////////////////////////aTOper.Start();
      PERF_START_METER("TCollection_Set operator=")
      a2 = a1;
      ////////////////////////////////aTOper.Stop();
      PERF_STOP_METER("TCollection_Set operator=")
      ////////////////////////////////aTClea.Start();
      PERF_START_METER("TCollection_Set clearing")
      a2.Clear();
      ////////////////////////////////aTClea.Stop();
      PERF_STOP_METER("TCollection_Set clearing")
    }
  PERF_PRINT_ALL
}

// ===================== Test perform of Sequence type ==========================
void CompSequence (const Standard_Integer theRep,
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
  PERF_PRINT_ALL
}

#endif
