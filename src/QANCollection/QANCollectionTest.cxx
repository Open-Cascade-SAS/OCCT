// Created on: 2002-04-16
// Created by: Alexander GRIGORIEV
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

#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_SequenceOfPnt.hxx>

#define USE_MACROS
#ifdef USE_MACROS
#include <NCollection_DefineArray1.hxx>
#include <NCollection_DefineSequence.hxx>
DEFINE_BASECOLLECTION (MyBaseCollPnt, gp_Pnt)
DEFINE_ARRAY1   (MyArray1,   MyBaseCollPnt, gp_Pnt)
DEFINE_SEQUENCE (MySequence, MyBaseCollPnt, gp_Pnt)
#else
#include <NCollection_Sequence.hxx>
#include <NCollection_Array1.hxx>
typedef NCollection_Array1<gp_Pnt> MyArray1;
typedef NCollection_Sequence<gp_Pnt> MySequence;
typedef NCollection_BaseCollection<gp_Pnt> MyBaseCollPnt;
#endif

#define PERF_ENABLE_METERS
#include <OSD_PerfMeter.hxx>

extern Handle(NCollection_BaseAllocator) getAlloc (const int i);

const Standard_Integer REPEAT = 100;

void createArray (TColgp_Array1OfPnt& anArrPnt)
{
  OSD_PerfMeter aPerfMeter("Create array");

  for (Standard_Integer j = 0; j < 2*REPEAT; j++) {
    PERF_START_METER("Create array")
      for (Standard_Integer i = anArrPnt.Lower(); i <= anArrPnt.Upper(); i++)
        anArrPnt(i).SetCoord ((double)i, (double)(i+1), (double)(i+2));
    PERF_STOP_METER("Create array")
  }
}

void createArray (MyArray1& anArrPnt)
{
  for (Standard_Integer j = 0; j < 2*REPEAT; j++) {
    PERF_START_METER("Create array")
      for (Standard_Integer i = anArrPnt.Lower(); i <= anArrPnt.Upper(); i++)
        anArrPnt(i).SetCoord ((double)i, (double)(i+1), (double)(i+2));
    PERF_STOP_METER("Create array")
  }
}

void assignArray (TColgp_Array1OfPnt& aDest, const TColgp_Array1OfPnt& aSrc)
{
  for (Standard_Integer i = 0; i < 2*REPEAT; i++) {
    PERF_START_METER("Assign array to array")
    aDest = aSrc;
    PERF_STOP_METER("Assign array to array")
  }
}

void assignCollection (MyBaseCollPnt&           aDest,
                       const MyBaseCollPnt&     aSrc,
                       const char               * MeterName)
{
  for (Standard_Integer i = 0; i < REPEAT; i++) {
    perf_start_meter (MeterName);
//    if (getAlloc(1).IsNull() == Standard_False)
//      if (getAlloc(1)->IsKind(STANDARD_TYPE(NCollection_IncAllocator)))
//        Handle(NCollection_IncAllocator)::DownCast(getAlloc(1)) -> Clean();
    aDest.Assign(aSrc);
    perf_stop_meter (MeterName);
  }
}

void assignArray (MyArray1& aDest,
                  const MyArray1& aSrc)
{
  for (Standard_Integer i = 0; i < 2*REPEAT; i++) {
    PERF_START_METER("Assign array to array")
    aDest = aSrc;
    PERF_STOP_METER("Assign array to array")
  }
}

void createSequence (TColgp_SequenceOfPnt& aSeq)
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

void createSequence (MySequence& aSeq)
{
  for (Standard_Integer j = 0; j < REPEAT; j++) {
    PERF_START_METER("Clear sequence")
    aSeq.Clear();
//    if (getAlloc(0).IsNull() == Standard_False)
//      if (getAlloc(0)->IsKind(STANDARD_TYPE(NCollection_IncAllocator)))
//        Handle(NCollection_IncAllocator)::DownCast(getAlloc(0)) -> Clean();
    PERF_STOP_METER("Clear sequence")
    PERF_START_METER("Create sequence")
    for (Standard_Integer i = 0; i < 100000; i++)
      aSeq.Append (gp_Pnt((double)i, (double)(i+1), (double)(i+2)));
    PERF_STOP_METER("Create sequence")
  }
}

void assignSequence (TColgp_SequenceOfPnt& aDest,
                     const TColgp_SequenceOfPnt& aSrc)
{
  for (Standard_Integer i = 0; i < REPEAT; i++) {
    PERF_START_METER("Assign sequence to sequence")
    aDest = aSrc;
    PERF_STOP_METER("Assign sequence to sequence")
  }
}

void assignSequence (MySequence& aDest, const MySequence& aSrc)
{
  for (Standard_Integer i = 0; i < REPEAT; i++) {
    PERF_START_METER("Assign sequence to sequence")
//    if (getAlloc(1).IsNull() == Standard_False)
//      if (getAlloc(1)->IsKind(STANDARD_TYPE(NCollection_IncAllocator)))
//        Handle(NCollection_IncAllocator)::DownCast(getAlloc(1)) -> Clean();
    aDest = aSrc;
    PERF_STOP_METER("Assign sequence to sequence")
  }
}

void printAllMeters ()
{
  PERF_PRINT_ALL
}
