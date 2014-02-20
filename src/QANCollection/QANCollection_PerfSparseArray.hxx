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

#ifndef QANCollection_PerfSparseArray_HeaderFile
#define QANCollection_PerfSparseArray_HeaderFile

#define PERF_ENABLE_METERS
#include <OSD_PerfMeter.hxx>
////////////////////////////////#include <Perf_Meter.hxx>
#include <NCollection_SparseArray.hxx>
#include <NCollection_SparseArrayBase.hxx>

// ===================== Test perform of SparseArray type ==========================
void CompSparseArray (const Standard_Integer theRep, const Standard_Integer theSize)
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

  PERF_PRINT_ALL
}

#endif
