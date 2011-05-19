// File:        QANCollection_PerfArrays.hxx
// Created:     Wed May 15 15:29:20 2002
// Author:      Alexander KARTOMIN (akm)
//              <a-kartomin@opencascade.com>


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
