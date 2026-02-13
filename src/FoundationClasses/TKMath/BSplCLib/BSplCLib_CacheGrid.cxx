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

#include <BSplCLib_CacheGrid.hxx>
#include <BSplCLib.hxx>
#include <BSplCLib_CacheParams.hxx>

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BSplCLib_CacheGrid, Standard_Transient)

//==================================================================================================

BSplCLib_CacheGrid::BSplCLib_CacheGrid(int                               theDegree,
                                       bool                              thePeriodic,
                                       const NCollection_Array1<double>& theFlatKnots,
                                       const NCollection_Array1<gp_Pnt>& thePoles,
                                       const NCollection_Array1<double>* theWeights)
    : myDegree(theDegree),
      myIsPeriodic(thePeriodic),
      myIs3D(true),
      myFirstParam(theFlatKnots.Value(theFlatKnots.Lower() + theDegree)),
      myLastParam(theFlatKnots.Value(theFlatKnots.Upper() - theDegree)),
      mySpanIndexMin(theFlatKnots.Lower() + theDegree),
      mySpanIndexMax(theFlatKnots.Upper() - theDegree - 1),
      myFlatKnots(&theFlatKnots),
      myPoles3D(&thePoles),
      myPoles2D(nullptr),
      myWeights(theWeights)
{
  myLastCell = 1;
  for (int i = 0; i < THE_GRID_SIZE; ++i)
  {
    myCellSpanIndex[i]  = -1;
    myCellSpanStart[i]  = 0.0;
    myCellSpanLength[i] = 0.0;
    myCellValid[i]      = false;
    myCache[i] = new BSplCLib_Cache(myDegree, myIsPeriodic, *myFlatKnots, *myPoles3D, myWeights);
  }

  rebuildGrid(myFirstParam);
}

//==================================================================================================

BSplCLib_CacheGrid::BSplCLib_CacheGrid(int                                 theDegree,
                                       bool                                thePeriodic,
                                       const NCollection_Array1<double>&   theFlatKnots,
                                       const NCollection_Array1<gp_Pnt2d>& thePoles2d,
                                       const NCollection_Array1<double>*   theWeights)
    : myDegree(theDegree),
      myIsPeriodic(thePeriodic),
      myIs3D(false),
      myFirstParam(theFlatKnots.Value(theFlatKnots.Lower() + theDegree)),
      myLastParam(theFlatKnots.Value(theFlatKnots.Upper() - theDegree)),
      mySpanIndexMin(theFlatKnots.Lower() + theDegree),
      mySpanIndexMax(theFlatKnots.Upper() - theDegree - 1),
      myFlatKnots(&theFlatKnots),
      myPoles3D(nullptr),
      myPoles2D(&thePoles2d),
      myWeights(theWeights)
{
  myLastCell = 1;
  for (int i = 0; i < THE_GRID_SIZE; ++i)
  {
    myCellSpanIndex[i]  = -1;
    myCellSpanStart[i]  = 0.0;
    myCellSpanLength[i] = 0.0;
    myCellValid[i]      = false;
    myCache[i] = new BSplCLib_Cache(myDegree, myIsPeriodic, *myFlatKnots, *myPoles2D, myWeights);
  }

  rebuildGrid(myFirstParam);
}

//==================================================================================================

int BSplCLib_CacheGrid::neighborSpanIndex(int theCenterSpanIndex, int theOffset) const
{
  const int aNeighborIdx = theCenterSpanIndex + theOffset;
  if (myIsPeriodic)
  {
    const int aRange = mySpanIndexMax - mySpanIndexMin + 1;
    if (aRange <= 0)
      return -1;
    int aWrapped = aNeighborIdx - mySpanIndexMin;
    aWrapped     = ((aWrapped % aRange) + aRange) % aRange;
    return mySpanIndexMin + aWrapped;
  }

  if (aNeighborIdx < mySpanIndexMin || aNeighborIdx > mySpanIndexMax)
    return -1;
  return aNeighborIdx;
}

//==================================================================================================

void BSplCLib_CacheGrid::centerGrid(int theCenterSpanIndex) const
{
  // Cell 0 = prev span, Cell 1 = center span, Cell 2 = next span
  const int aSpanIndices[THE_GRID_SIZE] = {neighborSpanIndex(theCenterSpanIndex, -1),
                                           theCenterSpanIndex,
                                           neighborSpanIndex(theCenterSpanIndex, +1)};

  // Update span boundaries. Cache objects stay in place -- never moved or released.
  // Cells whose span changed are marked invalid and will be rebuilt on demand
  // by calling BuildCache on the existing cache object (no reallocation).
  for (int i = 0; i < THE_GRID_SIZE; ++i)
  {
    myCellValid[i]     = (myCellSpanIndex[i] == aSpanIndices[i] && myCellValid[i]);
    myCellSpanIndex[i] = aSpanIndices[i];
    if (aSpanIndices[i] >= 0)
    {
      myCellSpanStart[i]  = myFlatKnots->Value(aSpanIndices[i]);
      myCellSpanLength[i] = myFlatKnots->Value(aSpanIndices[i] + 1) - myCellSpanStart[i];
    }
    else
    {
      myCellSpanStart[i]  = 0.0;
      myCellSpanLength[i] = 0.0;
    }
  }
}

//==================================================================================================

int BSplCLib_CacheGrid::locateCell(double theParameter) const
{
  // Normalize for periodic curves
  double aParam = theParameter;
  if (myIsPeriodic)
  {
    const double aPeriod = myLastParam - myFirstParam;
    if (aParam < myFirstParam)
    {
      const double aScale = std::trunc((myFirstParam - aParam) / aPeriod);
      aParam += aPeriod * (aScale + 1.0);
    }
    else if (aParam > myLastParam)
    {
      const double aScale = std::trunc((aParam - myLastParam) / aPeriod);
      aParam -= aPeriod * (aScale + 1.0);
    }
  }

  for (int i = 0; i < THE_GRID_SIZE; ++i)
  {
    if (myCellSpanIndex[i] < 0)
      continue;

    const double aDelta = aParam - myCellSpanStart[i];

    if (aDelta < 0.0)
      continue;

    if (myCellSpanIndex[i] == mySpanIndexMax)
    {
      // Last span: parameter is valid if within [start, end]
      if (aDelta <= myCellSpanLength[i])
        return i;
      continue;
    }

    // Non-last span: parameter must be in [start, end)
    if (aDelta >= myCellSpanLength[i])
      continue;

    // Check proximity to span end -- if within machine epsilon of the next
    // knot boundary, prefer the next span (consistent with BSplCLib::LocateParameter)
    const double anEps = Epsilon((std::min)(std::fabs(myLastParam), std::fabs(aParam)));
    if (myCellSpanLength[i] - aDelta <= anEps)
      continue;

    return i;
  }
  return -1;
}

//==================================================================================================

void BSplCLib_CacheGrid::rebuildGrid(double theParameter) const
{
  BSplCLib_CacheParams aParams(myDegree, myIsPeriodic, *myFlatKnots);
  double               aParam = aParams.PeriodicNormalization(theParameter);
  aParams.LocateParameter(aParam, *myFlatKnots);

  centerGrid(aParams.SpanIndex);

  // Build center cell if not reused from a previous grid position
  if (!myCellValid[1])
    buildCell(1);
}

//==================================================================================================

void BSplCLib_CacheGrid::buildCell(int theCellIndex) const
{
  const double aParam = myCellSpanStart[theCellIndex];
  if (myIs3D)
    myCache[theCellIndex]->BuildCache(aParam, *myFlatKnots, *myPoles3D, myWeights);
  else
    myCache[theCellIndex]->BuildCache(aParam, *myFlatKnots, *myPoles2D, myWeights);
  myCellValid[theCellIndex] = true;
}

//==================================================================================================

int BSplCLib_CacheGrid::ensureCell(double theParameter) const
{
  const int aCell = locateCell(theParameter);
  if (aCell < 0)
  {
    // Parameter outside grid -- recenter on this span
    rebuildGrid(theParameter);
    myLastCell = 1; // center cell
    return 1;       // center cell is always built by rebuildGrid
  }
  if (!myCellValid[aCell])
  {
    // Cell span is known but cache not yet built (lazy neighbor)
    buildCell(aCell);
  }
  myLastCell = aCell;
  return aCell;
}

//==================================================================================================

void BSplCLib_CacheGrid::D0(const double& theParameter, gp_Pnt& thePoint) const
{
  // Fast path: check if parameter is still in the last-used cell
  if (myCellValid[myLastCell])
  {
    const double aDelta = theParameter - myCellSpanStart[myLastCell];
    if (aDelta >= 0.0 && aDelta <= myCellSpanLength[myLastCell])
    {
      myCache[myLastCell]->D0(theParameter, thePoint);
      return;
    }
  }
  const int aCell = ensureCell(theParameter);
  myCache[aCell]->D0(theParameter, thePoint);
}

//==================================================================================================

void BSplCLib_CacheGrid::D0(const double& theParameter, gp_Pnt2d& thePoint) const
{
  if (myCellValid[myLastCell])
  {
    const double aDelta = theParameter - myCellSpanStart[myLastCell];
    if (aDelta >= 0.0 && aDelta <= myCellSpanLength[myLastCell])
    {
      myCache[myLastCell]->D0(theParameter, thePoint);
      return;
    }
  }
  const int aCell = ensureCell(theParameter);
  myCache[aCell]->D0(theParameter, thePoint);
}

//==================================================================================================

void BSplCLib_CacheGrid::D1(const double& theParameter, gp_Pnt& thePoint, gp_Vec& theTangent) const
{
  if (myCellValid[myLastCell])
  {
    const double aDelta = theParameter - myCellSpanStart[myLastCell];
    if (aDelta >= 0.0 && aDelta <= myCellSpanLength[myLastCell])
    {
      myCache[myLastCell]->D1(theParameter, thePoint, theTangent);
      return;
    }
  }
  const int aCell = ensureCell(theParameter);
  myCache[aCell]->D1(theParameter, thePoint, theTangent);
}

//==================================================================================================

void BSplCLib_CacheGrid::D1(const double& theParameter,
                            gp_Pnt2d&     thePoint,
                            gp_Vec2d&     theTangent) const
{
  if (myCellValid[myLastCell])
  {
    const double aDelta = theParameter - myCellSpanStart[myLastCell];
    if (aDelta >= 0.0 && aDelta <= myCellSpanLength[myLastCell])
    {
      myCache[myLastCell]->D1(theParameter, thePoint, theTangent);
      return;
    }
  }
  const int aCell = ensureCell(theParameter);
  myCache[aCell]->D1(theParameter, thePoint, theTangent);
}

//==================================================================================================

void BSplCLib_CacheGrid::D2(const double& theParameter,
                            gp_Pnt&       thePoint,
                            gp_Vec&       theTangent,
                            gp_Vec&       theCurvature) const
{
  if (myCellValid[myLastCell])
  {
    const double aDelta = theParameter - myCellSpanStart[myLastCell];
    if (aDelta >= 0.0 && aDelta <= myCellSpanLength[myLastCell])
    {
      myCache[myLastCell]->D2(theParameter, thePoint, theTangent, theCurvature);
      return;
    }
  }
  const int aCell = ensureCell(theParameter);
  myCache[aCell]->D2(theParameter, thePoint, theTangent, theCurvature);
}

//==================================================================================================

void BSplCLib_CacheGrid::D2(const double& theParameter,
                            gp_Pnt2d&     thePoint,
                            gp_Vec2d&     theTangent,
                            gp_Vec2d&     theCurvature) const
{
  if (myCellValid[myLastCell])
  {
    const double aDelta = theParameter - myCellSpanStart[myLastCell];
    if (aDelta >= 0.0 && aDelta <= myCellSpanLength[myLastCell])
    {
      myCache[myLastCell]->D2(theParameter, thePoint, theTangent, theCurvature);
      return;
    }
  }
  const int aCell = ensureCell(theParameter);
  myCache[aCell]->D2(theParameter, thePoint, theTangent, theCurvature);
}

//==================================================================================================

void BSplCLib_CacheGrid::D3(const double& theParameter,
                            gp_Pnt&       thePoint,
                            gp_Vec&       theTangent,
                            gp_Vec&       theCurvature,
                            gp_Vec&       theTorsion) const
{
  if (myCellValid[myLastCell])
  {
    const double aDelta = theParameter - myCellSpanStart[myLastCell];
    if (aDelta >= 0.0 && aDelta <= myCellSpanLength[myLastCell])
    {
      myCache[myLastCell]->D3(theParameter, thePoint, theTangent, theCurvature, theTorsion);
      return;
    }
  }
  const int aCell = ensureCell(theParameter);
  myCache[aCell]->D3(theParameter, thePoint, theTangent, theCurvature, theTorsion);
}

//==================================================================================================

void BSplCLib_CacheGrid::D3(const double& theParameter,
                            gp_Pnt2d&     thePoint,
                            gp_Vec2d&     theTangent,
                            gp_Vec2d&     theCurvature,
                            gp_Vec2d&     theTorsion) const
{
  if (myCellValid[myLastCell])
  {
    const double aDelta = theParameter - myCellSpanStart[myLastCell];
    if (aDelta >= 0.0 && aDelta <= myCellSpanLength[myLastCell])
    {
      myCache[myLastCell]->D3(theParameter, thePoint, theTangent, theCurvature, theTorsion);
      return;
    }
  }
  const int aCell = ensureCell(theParameter);
  myCache[aCell]->D3(theParameter, thePoint, theTangent, theCurvature, theTorsion);
}
