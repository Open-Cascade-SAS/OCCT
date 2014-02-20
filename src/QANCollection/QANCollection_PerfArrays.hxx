// Created on: 2002-05-15
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

#ifndef QANCollection_PerfArrays_HeaderFile
#define QANCollection_PerfArrays_HeaderFile

#define PERF_ENABLE_METERS
#include <OSD_PerfMeter.hxx>
////////////////////////////////#include <Perf_Meter.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>

// ===================== Test perform of Array1 type ==========================
void CompArray1 (const Standard_Integer theRep,
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
  PERF_PRINT_ALL
}

// ===================== Test perform of Array2 type ==========================
void CompArray2 (const Standard_Integer theRep,
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
  PERF_PRINT_ALL
}


#endif
