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
#include <Draw_Interpretor.hxx>

#include <OSD_PerfMeter.hxx>

#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_SequenceOfPnt.hxx>

#include <NCollection_Sequence.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_IncAllocator.hxx>

typedef NCollection_Array1<gp_Pnt> MyArray1;
typedef NCollection_Sequence<gp_Pnt> MySequence;

const Standard_Integer REPEAT = 100;

void printAllMeters ()
{
  PERF_PRINT_ALL
}

static void createArray (TColgp_Array1OfPnt& anArrPnt)
{
  OSD_PerfMeter aPerfMeter("Create array");

  for (Standard_Integer j = 0; j < 2*REPEAT; j++) {
    PERF_START_METER("Create array")
      for (Standard_Integer i = anArrPnt.Lower(); i <= anArrPnt.Upper(); i++)
        anArrPnt(i).SetCoord ((double)i, (double)(i+1), (double)(i+2));
    PERF_STOP_METER("Create array")
  }
}

static void createSequence (TColgp_SequenceOfPnt& aSeq)
{
  for (Standard_Integer j = 0; j < REPEAT; j++) {
    PERF_START_METER("Clear sequence")
    aSeq.Clear();
    PERF_STOP_METER("Clear sequence")
    PERF_START_METER("Create sequence")
    for (Standard_Integer i = 0; i < 100000; i++)
      aSeq.Append (gp_Pnt((double)i, (double)(i+1), (double)(i+2)));
    PERF_STOP_METER("Create sequence")
  }
}

static void createSequence (MySequence& aSeq)
{
  for (Standard_Integer j = 0; j < REPEAT; j++) {
    PERF_START_METER("Clear sequence")
    aSeq.Clear();
    PERF_STOP_METER("Clear sequence")
    PERF_START_METER("Create sequence")
    for (Standard_Integer i = 0; i < 100000; i++)
      aSeq.Append (gp_Pnt((double)i, (double)(i+1), (double)(i+2)));
    PERF_STOP_METER("Create sequence")
  }
}

static void assignSequence (TColgp_SequenceOfPnt& aDest,
                            const TColgp_SequenceOfPnt& aSrc)
{
  for (Standard_Integer i = 0; i < REPEAT; i++) {
    PERF_START_METER("Assign sequence to sequence")
    aDest = aSrc;
    PERF_STOP_METER("Assign sequence to sequence")
  }
}

static void assignSequence (MySequence& aDest, const MySequence& aSrc)
{
  for (Standard_Integer i = 0; i < REPEAT; i++) {
    PERF_START_METER("Assign sequence to sequence")
    aDest = aSrc;
    PERF_STOP_METER("Assign sequence to sequence")
  }
}

static void createArray (MyArray1& anArrPnt)
{
  for (Standard_Integer j = 0; j < 2*REPEAT; j++) {
    PERF_START_METER("Create array")
      for (Standard_Integer i = anArrPnt.Lower(); i <= anArrPnt.Upper(); i++)
        anArrPnt(i).SetCoord ((double)i, (double)(i+1), (double)(i+2));
    PERF_STOP_METER("Create array")
  }
}

static void assignArray (TColgp_Array1OfPnt& aDest, const TColgp_Array1OfPnt& aSrc)
{
  for (Standard_Integer i = 0; i < 2*REPEAT; i++) {
    PERF_START_METER("Assign array to array")
    aDest = aSrc;
    PERF_STOP_METER("Assign array to array")
  }
}

template <class MyBaseCollPnt>
void assignCollection (MyBaseCollPnt&           aDest,
                       const MyBaseCollPnt&     aSrc,
                       const char               * MeterName)
{
  for (Standard_Integer i = 0; i < REPEAT; i++) {
    perf_start_meter (MeterName);
    aDest.Assign(aSrc);
    perf_stop_meter (MeterName);
  }
}

static void assignArray (MyArray1& aDest, const MyArray1& aSrc)
{
  for (Standard_Integer i = 0; i < 2*REPEAT; i++) {
    PERF_START_METER("Assign array to array")
    aDest = aSrc;
    PERF_STOP_METER("Assign array to array")
  }
}

static void checkArray (const Standard_Boolean isNewColl)
{
  if (isNewColl) {
    MyArray1 anArrPnt (1, 100000), anArrPnt1 (1, 100000);
    createArray (anArrPnt);
    assignArray (anArrPnt1, anArrPnt);
    assignCollection (anArrPnt1, anArrPnt, "Assign collect to array");
  } else {
    TColgp_Array1OfPnt anArrPnt (1,100000), anArrPnt1 (1, 100000);
    createArray (anArrPnt);
    assignArray (anArrPnt1, anArrPnt);
  }
  printAllMeters ();
}

static void checkSequence (const Standard_Boolean isNewColl,
                           const Standard_Boolean isIncr)
{
  if (isNewColl) {
    Handle(NCollection_BaseAllocator) anAlloc[2];
    if (isIncr) {
      anAlloc[0] = new NCollection_IncAllocator;
      anAlloc[1] = new NCollection_IncAllocator;
    }
    MySequence aSeqPnt (anAlloc[0]), aSeqPnt1(anAlloc[1]);
    createSequence (aSeqPnt);
    assignSequence (aSeqPnt1, aSeqPnt);
    assignCollection (aSeqPnt1, aSeqPnt, "Assign collect to sequence");
  } else {
    TColgp_SequenceOfPnt aSeqPnt, aSeqPnt1;
    createSequence (aSeqPnt);
    assignSequence (aSeqPnt1, aSeqPnt);
  }
  printAllMeters ();
}

//=======================================================================
//function : QANColCheckArray1
//purpose  : 
//=======================================================================
static Standard_Integer QANColCheckArray1(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc > 2) {
    di << "Usage : " << argv[0] << " [-n]" << "\n";
    return 1;
  }

  Standard_Boolean isNewColl = Standard_False;
  if (argc > 1) {
    if (strcmp (argv[1], "-n") == 0) isNewColl = Standard_True;
  }
  checkArray (isNewColl);
  return 0;
}

//=======================================================================
//function : QANColCheckSequence
//purpose  : 
//=======================================================================
static Standard_Integer QANColCheckSequence(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc > 2) {
    di << "Usage : " << argv[0] << " [-n]/[-ni]/[-in]" << "\n";
    return 1;
  }

  Standard_Boolean isNewColl = Standard_False, isIncr = Standard_False;
  if (argc > 1) {
    if (strcmp (argv[1], "-n") == 0) isNewColl = Standard_True;
    if (strcmp (argv[1], "-ni") == 0 || strcmp (argv[1], "-in") == 0)
    {
      isNewColl = Standard_True;
      isIncr = Standard_True;
    }
  }
  checkSequence (isNewColl, isIncr);
  return 0;
}

void QANCollection::CommandsSimple(Draw_Interpretor& theCommands) {
  const char *group = "QANCollection";

  // from agvCollTest/src/AgvColEXE/TestEXE.cxx
  theCommands.Add("QANColCheckArray1",   "QANColCheckArray1 [-n]",               __FILE__, QANColCheckArray1,   group);  
  theCommands.Add("QANColCheckSequence", "QANColCheckSequence [-n]/[-ni]/[-in]", __FILE__, QANColCheckSequence, group);  

  return;
}
