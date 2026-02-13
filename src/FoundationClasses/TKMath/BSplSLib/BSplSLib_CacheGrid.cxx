// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <BSplSLib_CacheGrid.hxx>
#include <BSplCLib.hxx>
#include <BSplCLib_CacheParams.hxx>

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BSplSLib_CacheGrid, Standard_Transient)

//==================================================================================================

BSplSLib_CacheGrid::BSplSLib_CacheGrid(int                               theDegreeU,
                                       bool                              thePeriodicU,
                                       const NCollection_Array1<double>& theFlatKnotsU,
                                       int                               theDegreeV,
                                       bool                              thePeriodicV,
                                       const NCollection_Array1<double>& theFlatKnotsV,
                                       const NCollection_Array2<gp_Pnt>& thePoles,
                                       const NCollection_Array2<double>* theWeights)
    : myDegreeU(theDegreeU),
      myDegreeV(theDegreeV),
      myIsPeriodicU(thePeriodicU),
      myIsPeriodicV(thePeriodicV),
      myFirstParamU(theFlatKnotsU.Value(theFlatKnotsU.Lower() + theDegreeU)),
      myLastParamU(theFlatKnotsU.Value(theFlatKnotsU.Upper() - theDegreeU)),
      myFirstParamV(theFlatKnotsV.Value(theFlatKnotsV.Lower() + theDegreeV)),
      myLastParamV(theFlatKnotsV.Value(theFlatKnotsV.Upper() - theDegreeV)),
      mySpanIndexMinU(theFlatKnotsU.Lower() + theDegreeU),
      mySpanIndexMaxU(theFlatKnotsU.Upper() - theDegreeU - 1),
      mySpanIndexMinV(theFlatKnotsV.Lower() + theDegreeV),
      mySpanIndexMaxV(theFlatKnotsV.Upper() - theDegreeV - 1),
      myFlatKnotsU(&theFlatKnotsU),
      myFlatKnotsV(&theFlatKnotsV),
      myPoles(&thePoles),
      myWeights(theWeights)
{
  myLastCellU    = 1;
  myLastCellV    = 1;
  myLastCacheIdx = cacheIndex(1, 1);
  for (int i = 0; i < THE_GRID_SIZE; ++i)
  {
    myCellSpanIndexU[i]  = -1;
    myCellSpanStartU[i]  = 0.0;
    myCellSpanLengthU[i] = 0.0;
    myCellSpanIndexV[i]  = -1;
    myCellSpanStartV[i]  = 0.0;
    myCellSpanLengthV[i] = 0.0;
  }
  for (int i = 0; i < THE_GRID_SIZE * THE_GRID_SIZE; ++i)
  {
    myCellValid[i] = false;
    myCache[i]     = new BSplSLib_Cache(myDegreeU,
                                    myIsPeriodicU,
                                    *myFlatKnotsU,
                                    myDegreeV,
                                    myIsPeriodicV,
                                    *myFlatKnotsV,
                                    myWeights);
  }

  rebuildGrid(myFirstParamU, myFirstParamV);
}

//==================================================================================================

int BSplSLib_CacheGrid::neighborSpanIndex(int  theCenterSpanIndex,
                                          int  theOffset,
                                          int  theSpanMin,
                                          int  theSpanMax,
                                          bool theIsPeriodic)
{
  const int aNeighborIdx = theCenterSpanIndex + theOffset;
  if (theIsPeriodic)
  {
    const int aRange = theSpanMax - theSpanMin + 1;
    if (aRange <= 0)
      return -1;
    int aWrapped = aNeighborIdx - theSpanMin;
    aWrapped     = ((aWrapped % aRange) + aRange) % aRange;
    return theSpanMin + aWrapped;
  }

  if (aNeighborIdx < theSpanMin || aNeighborIdx > theSpanMax)
    return -1;
  return aNeighborIdx;
}

//==================================================================================================

void BSplSLib_CacheGrid::centerGridDir(int                               theCenterSpanIndex,
                                       int                               theSpanMin,
                                       int                               theSpanMax,
                                       bool                              theIsPeriodic,
                                       const NCollection_Array1<double>& theFlatKnots,
                                       int*                              theCellSpanIndex,
                                       double*                           theCellSpanStart,
                                       double*                           theCellSpanLength)
{
  const int aSpanIndices[THE_GRID_SIZE] = {
    neighborSpanIndex(theCenterSpanIndex, -1, theSpanMin, theSpanMax, theIsPeriodic),
    theCenterSpanIndex,
    neighborSpanIndex(theCenterSpanIndex, +1, theSpanMin, theSpanMax, theIsPeriodic)};

  for (int i = 0; i < THE_GRID_SIZE; ++i)
  {
    theCellSpanIndex[i] = aSpanIndices[i];
    if (aSpanIndices[i] >= 0)
    {
      theCellSpanStart[i]  = theFlatKnots.Value(aSpanIndices[i]);
      theCellSpanLength[i] = theFlatKnots.Value(aSpanIndices[i] + 1) - theCellSpanStart[i];
    }
    else
    {
      theCellSpanStart[i]  = 0.0;
      theCellSpanLength[i] = 0.0;
    }
  }
}

//==================================================================================================

int BSplSLib_CacheGrid::locateCellDir(double        theParam,
                                      const int*    theSpanIdx,
                                      const double* theSpanStart,
                                      const double* theSpanLength,
                                      int           theSpanMax,
                                      double        theFirst,
                                      double        theLast,
                                      bool          theIsPeriodic)
{
  double aParam = theParam;
  if (theIsPeriodic)
  {
    const double aPeriod = theLast - theFirst;
    if (aParam < theFirst)
    {
      const double aScale = std::trunc((theFirst - aParam) / aPeriod);
      aParam += aPeriod * (aScale + 1.0);
    }
    else if (aParam > theLast)
    {
      const double aScale = std::trunc((aParam - theLast) / aPeriod);
      aParam -= aPeriod * (aScale + 1.0);
    }
  }

  for (int i = 0; i < THE_GRID_SIZE; ++i)
  {
    if (theSpanIdx[i] < 0)
      continue;

    const double aDelta = aParam - theSpanStart[i];

    if (aDelta < 0.0)
      continue;

    if (theSpanIdx[i] == theSpanMax)
    {
      // Last span: parameter is valid if within [start, end]
      if (aDelta <= theSpanLength[i])
        return i;
      continue;
    }

    // Non-last span: parameter must be in [start, end)
    if (aDelta >= theSpanLength[i])
      continue;

    // Check proximity to span end -- if within machine epsilon of the next
    // knot boundary, prefer the next span (consistent with BSplCLib::LocateParameter)
    const double anEps = Epsilon((std::min)(std::fabs(theLast), std::fabs(aParam)));
    if (theSpanLength[i] - aDelta <= anEps)
      continue;

    return i;
  }
  return -1;
}

//==================================================================================================

bool BSplSLib_CacheGrid::locateCell(double theU, double theV, int& theCellU, int& theCellV) const
{
  theCellU = locateCellDir(theU,
                           myCellSpanIndexU,
                           myCellSpanStartU,
                           myCellSpanLengthU,
                           mySpanIndexMaxU,
                           myFirstParamU,
                           myLastParamU,
                           myIsPeriodicU);
  if (theCellU < 0)
    return false;

  theCellV = locateCellDir(theV,
                           myCellSpanIndexV,
                           myCellSpanStartV,
                           myCellSpanLengthV,
                           mySpanIndexMaxV,
                           myFirstParamV,
                           myLastParamV,
                           myIsPeriodicV);
  return theCellV >= 0;
}

//==================================================================================================

void BSplSLib_CacheGrid::rebuildGrid(double theU, double theV) const
{
  // Find the span indices for U and V
  BSplCLib_CacheParams aParamsU(myDegreeU, myIsPeriodicU, *myFlatKnotsU);
  double               aParamU = aParamsU.PeriodicNormalization(theU);
  aParamsU.LocateParameter(aParamU, *myFlatKnotsU);

  BSplCLib_CacheParams aParamsV(myDegreeV, myIsPeriodicV, *myFlatKnotsV);
  double               aParamV = aParamsV.PeriodicNormalization(theV);
  aParamsV.LocateParameter(aParamV, *myFlatKnotsV);

  // Save old span indices to detect which cells can keep their cache
  int aOldSpanU[THE_GRID_SIZE], aOldSpanV[THE_GRID_SIZE];
  for (int i = 0; i < THE_GRID_SIZE; ++i)
  {
    aOldSpanU[i] = myCellSpanIndexU[i];
    aOldSpanV[i] = myCellSpanIndexV[i];
  }

  // Center grids in both directions
  centerGridDir(aParamsU.SpanIndex,
                mySpanIndexMinU,
                mySpanIndexMaxU,
                myIsPeriodicU,
                *myFlatKnotsU,
                myCellSpanIndexU,
                myCellSpanStartU,
                myCellSpanLengthU);
  centerGridDir(aParamsV.SpanIndex,
                mySpanIndexMinV,
                mySpanIndexMaxV,
                myIsPeriodicV,
                *myFlatKnotsV,
                myCellSpanIndexV,
                myCellSpanStartV,
                myCellSpanLengthV);

  // Mark cells whose span changed as invalid. Cache objects stay in place --
  // never moved or released. Invalid cells are rebuilt on demand by calling
  // BuildCache on the existing cache object (no reallocation).
  for (int iU = 0; iU < THE_GRID_SIZE; ++iU)
  {
    const bool aSameU = (myCellSpanIndexU[iU] == aOldSpanU[iU]);
    for (int iV = 0; iV < THE_GRID_SIZE; ++iV)
    {
      const int aIdx    = cacheIndex(iU, iV);
      myCellValid[aIdx] = (aSameU && myCellSpanIndexV[iV] == aOldSpanV[iV] && myCellValid[aIdx]);
    }
  }

  // Build center cell if not still valid
  const int aCenterIdx = cacheIndex(1, 1);
  if (!myCellValid[aCenterIdx])
    buildCell(1, 1);
}

//==================================================================================================

void BSplSLib_CacheGrid::buildCell(int theCellU, int theCellV) const
{
  const int aIdx = cacheIndex(theCellU, theCellV);
  myCache[aIdx]->BuildCache(myCellSpanStartU[theCellU],
                            myCellSpanStartV[theCellV],
                            *myFlatKnotsU,
                            *myFlatKnotsV,
                            *myPoles,
                            myWeights);
  myCellValid[aIdx] = true;
}

//==================================================================================================

void BSplSLib_CacheGrid::ensureCell(double theU, double theV, int& theCellU, int& theCellV) const
{
  if (!locateCell(theU, theV, theCellU, theCellV))
  {
    // Parameters outside grid -- recenter
    rebuildGrid(theU, theV);
    theCellU = 1;
    theCellV = 1;
  }
  else
  {
    const int aIdx = cacheIndex(theCellU, theCellV);
    if (!myCellValid[aIdx])
    {
      // Cell span is known but cache not yet built for current span
      buildCell(theCellU, theCellV);
    }
  }
  myLastCellU    = theCellU;
  myLastCellV    = theCellV;
  myLastCacheIdx = cacheIndex(theCellU, theCellV);
}

//==================================================================================================

void BSplSLib_CacheGrid::D0(const double& theU, const double& theV, gp_Pnt& thePoint) const
{
  // Fast path: check if parameters are still in the last-used cell
  if (myCellValid[myLastCacheIdx])
  {
    const double aDeltaU = theU - myCellSpanStartU[myLastCellU];
    const double aDeltaV = theV - myCellSpanStartV[myLastCellV];
    if (aDeltaU >= 0.0 && aDeltaU <= myCellSpanLengthU[myLastCellU] && aDeltaV >= 0.0
        && aDeltaV <= myCellSpanLengthV[myLastCellV])
    {
      myCache[myLastCacheIdx]->D0(theU, theV, thePoint);
      return;
    }
  }
  int aCellU = -1, aCellV = -1;
  ensureCell(theU, theV, aCellU, aCellV);
  myCache[myLastCacheIdx]->D0(theU, theV, thePoint);
}

//==================================================================================================

void BSplSLib_CacheGrid::D1(const double& theU,
                            const double& theV,
                            gp_Pnt&       thePoint,
                            gp_Vec&       theTangentU,
                            gp_Vec&       theTangentV) const
{
  if (myCellValid[myLastCacheIdx])
  {
    const double aDeltaU = theU - myCellSpanStartU[myLastCellU];
    const double aDeltaV = theV - myCellSpanStartV[myLastCellV];
    if (aDeltaU >= 0.0 && aDeltaU <= myCellSpanLengthU[myLastCellU] && aDeltaV >= 0.0
        && aDeltaV <= myCellSpanLengthV[myLastCellV])
    {
      myCache[myLastCacheIdx]->D1(theU, theV, thePoint, theTangentU, theTangentV);
      return;
    }
  }
  int aCellU = -1, aCellV = -1;
  ensureCell(theU, theV, aCellU, aCellV);
  myCache[myLastCacheIdx]->D1(theU, theV, thePoint, theTangentU, theTangentV);
}

//==================================================================================================

void BSplSLib_CacheGrid::D2(const double& theU,
                            const double& theV,
                            gp_Pnt&       thePoint,
                            gp_Vec&       theTangentU,
                            gp_Vec&       theTangentV,
                            gp_Vec&       theCurvatureU,
                            gp_Vec&       theCurvatureV,
                            gp_Vec&       theCurvatureUV) const
{
  if (myCellValid[myLastCacheIdx])
  {
    const double aDeltaU = theU - myCellSpanStartU[myLastCellU];
    const double aDeltaV = theV - myCellSpanStartV[myLastCellV];
    if (aDeltaU >= 0.0 && aDeltaU <= myCellSpanLengthU[myLastCellU] && aDeltaV >= 0.0
        && aDeltaV <= myCellSpanLengthV[myLastCellV])
    {
      myCache[myLastCacheIdx]->D2(theU,
                                  theV,
                                  thePoint,
                                  theTangentU,
                                  theTangentV,
                                  theCurvatureU,
                                  theCurvatureV,
                                  theCurvatureUV);
      return;
    }
  }
  int aCellU = -1, aCellV = -1;
  ensureCell(theU, theV, aCellU, aCellV);
  myCache[myLastCacheIdx]->D2(theU,
                              theV,
                              thePoint,
                              theTangentU,
                              theTangentV,
                              theCurvatureU,
                              theCurvatureV,
                              theCurvatureUV);
}
