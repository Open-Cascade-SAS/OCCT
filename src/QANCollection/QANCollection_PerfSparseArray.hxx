// Created on: 2002-05-15
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



#ifndef QANCollection_PerfSparseArray_HeaderFile
#define QANCollection_PerfSparseArray_HeaderFile

#define PERF_ENABLE_METERS
#include <DebugTools.h>
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
          Standard_Integer sizeSparseArray=a1.Size();
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

  PERF_PRINT_ALL_METERS
}

#endif
