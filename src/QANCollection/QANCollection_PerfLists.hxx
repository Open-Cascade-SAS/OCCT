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



#ifndef QANCollection_PerfLists_HeaderFile
#define QANCollection_PerfLists_HeaderFile

#include <QANCollection_ListOfPnt.hxx>
#include <QANCollection_SListOfPnt.hxx>
#include <QANCollection_QueueOfPnt.hxx>
#include <QANCollection_StackOfPnt.hxx>
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
  PERF_PRINT_ALL_METERS
}

// ===================== Test perform of Queue type ==========================
void CompQueue (const Standard_Integer theRep,
                const Standard_Integer theSize)
{
  Standard_Integer i,j;

  ////////////////////////////////Perf_Meter aNPush ("NCollection_Queue pushing",0);
  ////////////////////////////////Perf_Meter aTPush ("TCollection_Queue pushing",0);
  ////////////////////////////////Perf_Meter aNPopp ("NCollection_Queue popping",0);
  ////////////////////////////////Perf_Meter aTPopp ("TCollection_Queue popping",0);
  ////////////////////////////////Perf_Meter aNOper ("NCollection_Queue operator=",0);
  ////////////////////////////////Perf_Meter aTOper ("TCollection_Queue operator=",0);
  ////////////////////////////////Perf_Meter aNClea ("NCollection_Queue clearing",0);
  ////////////////////////////////Perf_Meter aTClea ("TCollection_Queue clearing",0);
  ////////////////////////////////Perf_Meter aNAssi ("NCollection_Queue Assign",0);
  for (i=0; i<theRep; i++)
    {
      ////////////////////////////////QANCollection_Queue a1, a2;
      QANCollection_QueuePerf a1, a2;
      ////////////////////////////////aNPush.Start();
      PERF_START_METER("NCollection_Queue pushing")
      for (j=1; j<=theSize; j++)
        {
          ItemType anItem;
          Random(anItem);
          a1.Push(anItem);
        }
      ////////////////////////////////aNPush.Stop();
      PERF_STOP_METER("NCollection_Queue pushing")
      ////////////////////////////////aNOper.Start();
      PERF_START_METER("NCollection_Queue operator=")
      a2 = a1;
      ////////////////////////////////aNOper.Stop();
      PERF_STOP_METER("NCollection_Queue operator=")
      ////////////////////////////////aNAssi.Start();
      PERF_START_METER("NCollection_Queue Assign")
      a2.Assign(a1);
      ////////////////////////////////aNAssi.Stop();
      PERF_STOP_METER("NCollection_Queue Assign")
      ////////////////////////////////aNPopp.Start();
      PERF_START_METER("NCollection_Queue popping")
      for (j=1; j<=theSize; j++)
        a1.Pop();
      ////////////////////////////////aNPopp.Stop();
      PERF_STOP_METER("NCollection_Queue popping")
      ////////////////////////////////aNClea.Start();
      PERF_START_METER("NCollection_Queue clearing")
      a2.Clear();
      ////////////////////////////////aNClea.Stop();
      PERF_STOP_METER("NCollection_Queue clearing")
    }

  for (i=0; i<theRep; i++)
    {
      QANCollection_QueueOfPnt a1, a2;
      ////////////////////////////////aTPush.Start();
      PERF_START_METER("TCollection_Queue pushing")
      for (j=1; j<=theSize; j++)
        {
          ItemType anItem;
          Random(anItem);
          a1.Push(anItem);
        }
      ////////////////////////////////aTPush.Stop();
      PERF_STOP_METER("TCollection_Queue pushing")
      ////////////////////////////////aTOper.Start();
      PERF_START_METER("TCollection_Queue operator=")
      a2 = a1;
      ////////////////////////////////aTOper.Stop();
      PERF_STOP_METER("TCollection_Queue operator=")
      ////////////////////////////////aTPopp.Start();
      PERF_START_METER("TCollection_Queue popping")
      for (j=1; j<=theSize; j++)
        a1.Pop();
      ////////////////////////////////aTPopp.Stop();
      PERF_STOP_METER("TCollection_Queue popping")
      ////////////////////////////////aTClea.Start();
      PERF_START_METER("TCollection_Queue clearing")
      a2.Clear();
      ////////////////////////////////aTClea.Stop();
      PERF_STOP_METER("TCollection_Queue clearing")
    }
  PERF_PRINT_ALL_METERS
}

// ===================== Test perform of Stack type ==========================
void CompStack (const Standard_Integer theRep,
                const Standard_Integer theSize)
{
  Standard_Integer i,j;

  ////////////////////////////////Perf_Meter aNPush ("NCollection_Stack pushing",0);
  ////////////////////////////////Perf_Meter aTPush ("TCollection_Stack pushing",0);
  ////////////////////////////////Perf_Meter aNPopp ("NCollection_Stack popping",0);
  ////////////////////////////////Perf_Meter aTPopp ("TCollection_Stack popping",0);
  ////////////////////////////////Perf_Meter aNOper ("NCollection_Stack operator=",0);
  ////////////////////////////////Perf_Meter aTOper ("TCollection_Stack operator=",0);
  ////////////////////////////////Perf_Meter aNClea ("NCollection_Stack clearing",0);
  ////////////////////////////////Perf_Meter aTClea ("TCollection_Stack clearing",0);
  ////////////////////////////////Perf_Meter aNAssi ("NCollection_Stack Assign",0);
  for (i=0; i<theRep; i++)
    {
      ////////////////////////////////QANCollection_Stack a1, a2;
      QANCollection_StackPerf a1, a2;
      ////////////////////////////////aNPush.Start();
      PERF_START_METER("NCollection_Stack pushing")
      for (j=1; j<=theSize; j++)
        {
          ItemType anItem;
          Random(anItem);
          a1.Push(anItem);
        }
      ////////////////////////////////aNPush.Stop();
      PERF_STOP_METER("NCollection_Stack pushing")
      ////////////////////////////////aNOper.Start();
      PERF_START_METER("NCollection_Stack operator=")
      a2 = a1;
      ////////////////////////////////aNOper.Stop();
      PERF_STOP_METER("NCollection_Stack operator=")
      ////////////////////////////////aNAssi.Start();
      PERF_START_METER("NCollection_Stack Assign")
      a2.Assign(a1);
      ////////////////////////////////aNAssi.Stop();
      PERF_STOP_METER("NCollection_Stack Assign")
      ////////////////////////////////aNPopp.Start();
      PERF_START_METER("NCollection_Stack popping")
      for (j=1; j<=theSize; j++)
        a1.Pop();
      ////////////////////////////////aNPopp.Stop();
      PERF_STOP_METER("NCollection_Stack popping")
      ////////////////////////////////aNClea.Start();
      PERF_START_METER("NCollection_Stack clearing")
      a2.Clear();
      ////////////////////////////////aNClea.Stop();
      PERF_STOP_METER("NCollection_Stack clearing")
    }

  for (i=0; i<theRep; i++)
    {
      QANCollection_StackOfPnt a1, a2;
      ////////////////////////////////aTPush.Start();
      PERF_START_METER("TCollection_Stack pushing")
      for (j=1; j<=theSize; j++)
        {
          ItemType anItem;
          Random(anItem);
          a1.Push(anItem);
        }
      ////////////////////////////////aTPush.Stop();
      PERF_STOP_METER("TCollection_Stack pushing")
      ////////////////////////////////aTOper.Start();
      PERF_START_METER("TCollection_Stack operator=")
      a2 = a1;
      ////////////////////////////////aTOper.Stop();
      PERF_STOP_METER("TCollection_Stack operator=")
      ////////////////////////////////aTPopp.Start();
      PERF_START_METER("TCollection_Stack popping")
      for (j=1; j<=theSize; j++)
        a1.Pop();
      ////////////////////////////////aTPopp.Stop();
      PERF_STOP_METER("TCollection_Stack popping")
      ////////////////////////////////aTClea.Start();
      PERF_START_METER("TCollection_Stack clearing")
      a2.Clear();
      ////////////////////////////////aTClea.Stop();
      PERF_STOP_METER("TCollection_Stack clearing")
    }
  PERF_PRINT_ALL_METERS
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
  PERF_PRINT_ALL_METERS
}


// ===================== Test perform of SList type ==========================
void CompSList (const Standard_Integer theRep,
                const Standard_Integer theSize)
{
  Standard_Integer i,j;

  ////////////////////////////////Perf_Meter aNAppe ("NCollection_SList constructing",0);
  ////////////////////////////////Perf_Meter aTAppe ("TCollection_SList constructing",0);
  ////////////////////////////////Perf_Meter aNOper ("NCollection_SList operator=",0);
  ////////////////////////////////Perf_Meter aTOper ("TCollection_SList operator=",0);
  ////////////////////////////////Perf_Meter aNAssi ("NCollection_SList Assign",0);
  ////////////////////////////////Perf_Meter aNClea ("NCollection_SList clearing",0);
  ////////////////////////////////Perf_Meter aTClea ("TCollection_SList clearing",0);
  for (i=0; i<theRep; i++)
    {
      ////////////////////////////////QANCollection_SList a1, a2;
      QANCollection_SListPerf a1, a2;
      ////////////////////////////////aNAppe.Start();
      PERF_START_METER("NCollection_SList constructing")
      for (j=1; j<=theSize; j++)
      {
        ItemType anItem;
        Random(anItem);
        a1.Construct(anItem);
      }
    ////////////////////////////////aNAppe.Stop();
    PERF_STOP_METER("NCollection_SList constructing")
    ////////////////////////////////aNOper.Start();
    PERF_START_METER("NCollection_SList operator=")
    a2 = a1;
    ////////////////////////////////aNOper.Stop();
    PERF_STOP_METER("NCollection_SList operator=")
    ////////////////////////////////aNAssi.Start();
    PERF_START_METER("NCollection_SList Assign")
    a2.Assign(a1);
    ////////////////////////////////aNAssi.Stop();
    PERF_STOP_METER("NCollection_SList Assign")
    ////////////////////////////////aNClea.Start();
    PERF_START_METER("NCollection_SList clearing")
    a2.Clear();
    ////////////////////////////////aNClea.Stop();
    PERF_STOP_METER("NCollection_SList clearing")
  }

  for (i=0; i<theRep; i++)
    {
      QANCollection_SListOfPnt a1, a2;
      ////////////////////////////////aTAppe.Start();
      PERF_START_METER("TCollection_SList constructing")
      for (j=1; j<=theSize; j++)
        {
          ItemType anItem;
          Random(anItem);
          a1.Construct(anItem);
        }
      ////////////////////////////////aTAppe.Stop();
      PERF_STOP_METER("TCollection_SList constructing")
      ////////////////////////////////aTOper.Start();
      PERF_START_METER("TCollection_SList operator=")
      a2 = a1;
      ////////////////////////////////aTOper.Stop();
      PERF_STOP_METER("TCollection_SList operator=")
      ////////////////////////////////aTClea.Start();
      PERF_START_METER("TCollection_SList clearing")
      a2.Clear();
      ////////////////////////////////aTClea.Stop();
      PERF_STOP_METER("TCollection_SList clearing")
    }
  PERF_PRINT_ALL_METERS
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
  PERF_PRINT_ALL_METERS
}

#endif
