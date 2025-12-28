// Created on: 2016-04-19
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#include <BRepMesh_NURBSRangeSplitter.hxx>

#include <algorithm>
#include <BRepMesh_GeomTool.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomLib.hxx>
#include <IMeshData_Edge.hxx>
#include <IMeshData_Wire.hxx>
#include <NCollection_Handle.hxx>

namespace
{
class AnalyticalFilter
{
public:
  //! Constructor.
  AnalyticalFilter(const IMeshData::IFaceHandle&            theDFace,
                   const GeomAbs_IsoType                    theIsoType,
                   const Handle(IMeshData::SequenceOfReal)& theParams,
                   const Handle(IMeshData::SequenceOfReal)& theControlParams,
                   const Handle(IMeshData::MapOfReal)&      theParamsForbiddenToRemove,
                   const Handle(IMeshData::MapOfReal)&      theControlParamsForbiddenToRemove)
      : myDFace(theDFace),
        mySurface(myDFace->GetSurface()->Surface().Surface()),
        myIsoU(theIsoType == GeomAbs_IsoU),
        myParams(theParams),
        myControlParams(theControlParams),
        myParamsForbiddenToRemove(theParamsForbiddenToRemove),
        myControlParamsForbiddenToRemove(theControlParamsForbiddenToRemove),
        myAllocator(new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE)),
        myControlParamsToRemove(new IMeshData::MapOfReal(1, myAllocator)),
        myCurrParam(0.0),
        myCurrControlParam(0.0),
        myPrevControlParam(0.0)
  {
  }

  //! Returns map of parameters supposed to be removed.
  const Handle(IMeshData::MapOfReal)& GetControlParametersToRemove(
    const IMeshTools_Parameters& theParameters)
  {
    myParameters = theParameters;

    int aStartIndex, aEndIndex;
    if (myIsoU)
    {
      aStartIndex = 1;
      aEndIndex   = myParams->Length();
    }
    else
    {
      aStartIndex = 2;
      aEndIndex   = myParams->Length() - 1;
    }

    for (int i = aStartIndex; i <= aEndIndex; ++i)
    {
      myCurrParam = myParams->Value(i);
      myIso =
        new GeomAdaptor_Curve(myIsoU ? mySurface->UIso(myCurrParam) : mySurface->VIso(myCurrParam));

      myPrevControlParam = myControlParams->Value(1);
      myIso->D1(myPrevControlParam, myPrevControlPnt, myPrevControlVec);
      for (int j = 2; j <= myControlParams->Length();)
      {
        j += checkControlPointAndMoveOn(j);
      }
    }

    return myControlParamsToRemove;
  }

private:
  //! Checks the given control point for deviation.
  //! Returns number of steps to be used to move point iterator.
  int checkControlPointAndMoveOn(const int theIndex)
  {
    int aMoveSteps     = 0;
    myCurrControlParam = myControlParams->Value(theIndex);
    myIso->D1(myCurrControlParam, myCurrControlPnt, myCurrControlVec);

    const double aMidParam = 0.5 * (myPrevControlParam + myCurrControlParam);
    const gp_Pnt aMidPnt   = myIso->Value(aMidParam);

    const double aSqDist =
      BRepMesh_GeomTool::SquareDeflectionOfSegment(myPrevControlPnt, myCurrControlPnt, aMidPnt);

    double anAngle = 0.0;

    if ((myPrevControlVec.SquareMagnitude() > Precision::SquareConfusion())
        && (myCurrControlVec.SquareMagnitude() > Precision::SquareConfusion()))
    {
      anAngle = myPrevControlVec.Angle(myCurrControlVec);
    }

    const double aSqMaxDeflection = myDFace->GetDeflection() * myDFace->GetDeflection();

    if (((aSqDist > aSqMaxDeflection) || (anAngle > myParameters.AngleInterior))
        && aSqDist > myParameters.MinSize * myParameters.MinSize)
    {
      // insertion
      myControlParams->InsertBefore(theIndex, aMidParam);
    }
    else
    {
      // Here we should leave at least 3 parameters as far as
      // we must have at least one parameter related to surface
      // internals in order to prevent movement of triangle body
      // outside the surface in case of highly curved ones, e.g.
      // BSpline springs.
      if (((aSqDist < aSqMaxDeflection) || (anAngle < myParameters.AngleInterior))
          && myControlParams->Length() > 3 && theIndex < myControlParams->Length())
      {
        // Remove too dense points
        const double aTmpParam = myControlParams->Value(theIndex + 1);
        if (checkParameterForDeflectionAndUpdateCache(aTmpParam))
        {
          ++aMoveSteps;
        }
      }

      myPrevControlParam = myCurrControlParam;
      myPrevControlPnt   = myCurrControlPnt;
      myPrevControlVec   = myCurrControlVec;

      ++aMoveSteps;
    }

    return aMoveSteps;
  }

  //! Checks whether the given param suits specified deflection. Updates cache.
  bool checkParameterForDeflectionAndUpdateCache(const double theParam)
  {
    gp_Pnt aTmpPnt;
    gp_Vec aTmpVec;
    myIso->D1(theParam, aTmpPnt, aTmpVec);

    const double aTmpMidParam = 0.5 * (myPrevControlParam + theParam);
    const gp_Pnt aTmpMidPnt   = myIso->Value(aTmpMidParam);

    // Lets check next parameter.
    // If it also fits deflection, we can remove previous parameter.
    const double aSqDist =
      BRepMesh_GeomTool::SquareDeflectionOfSegment(myPrevControlPnt, aTmpPnt, aTmpMidPnt);

    if (aSqDist < myDFace->GetDeflection() * myDFace->GetDeflection())
    {
      // Lets check parameters for angular deflection.
      if (myPrevControlVec.SquareMagnitude() < gp::Resolution()
          || aTmpVec.SquareMagnitude() < gp::Resolution()
          || myPrevControlVec.Angle(aTmpVec) < myParameters.AngleInterior)
      {
        // For current Iso line we can remove this parameter.
        myControlParamsToRemove->Add(myCurrControlParam);
        myCurrControlParam = theParam;
        myCurrControlPnt   = aTmpPnt;
        myCurrControlVec   = aTmpVec;
        return true;
      }
      else
      {
        // We have found a place on the surface refusing
        // removement of this parameter.
        myParamsForbiddenToRemove->Add(myCurrParam);
        myControlParamsForbiddenToRemove->Add(myCurrControlParam);
      }
    }

    return false;
  }

private:
  IMeshData::IFaceHandle            myDFace;
  occ::handle<Geom_Surface>         mySurface;
  bool                              myIsoU;
  Handle(IMeshData::SequenceOfReal) myParams;
  Handle(IMeshData::SequenceOfReal) myControlParams;

  Handle(IMeshData::MapOfReal) myParamsForbiddenToRemove;
  Handle(IMeshData::MapOfReal) myControlParamsForbiddenToRemove;

  occ::handle<NCollection_IncAllocator> myAllocator;
  Handle(IMeshData::MapOfReal)          myControlParamsToRemove;

  IMeshTools_Parameters                 myParameters;
  NCollection_Handle<GeomAdaptor_Curve> myIso;

  double myCurrParam;

  double myCurrControlParam;
  gp_Pnt myCurrControlPnt;
  gp_Vec myCurrControlVec;

  double myPrevControlParam;
  gp_Pnt myPrevControlPnt;
  gp_Vec myPrevControlVec;
};

//! Adds param to map if it fits specified range.
bool addParam(const double&                    theParam,
              const std::pair<double, double>& theRange,
              IMeshData::IMapOfReal&           theParams)
{
  if (theParam < theRange.first || theParam > theRange.second)
  {
    return false;
  }

  theParams.Add(theParam);
  return true;
}

//! Initializes parameters map using CN intervals.
bool initParamsFromIntervals(const NCollection_Array1<double>& theIntervals,
                             const std::pair<double, double>&  theRange,
                             const bool                        isSplitIntervals,
                             IMeshData::IMapOfReal&            theParams)
{
  bool isAdded = false;

  for (int i = theIntervals.Lower(); i <= theIntervals.Upper(); ++i)
  {
    const double aStartParam = theIntervals.Value(i);
    if (addParam(aStartParam, theRange, theParams))
    {
      isAdded = true;
    }

    if (isSplitIntervals && i < theIntervals.Upper())
    {
      const double aMidParam = (aStartParam + theIntervals.Value(i + 1)) / 2.;
      if (addParam(aMidParam, theRange, theParams))
      {
        isAdded = true;
      }
    }
  }

  return isAdded;
}

//! Checks whether intervals should be split.
//! Returns true in case if it is impossible to compute normal
//! directly on intervals, false is returned elsewhere.
bool toSplitIntervals(const occ::handle<Geom_Surface>& theSurf,
                      const NCollection_Array1<double> (&theIntervals)[2])
{
  int aIntervalU = theIntervals[0].Lower();
  for (; aIntervalU <= theIntervals[0].Upper(); ++aIntervalU)
  {
    const double aParamU = theIntervals[0].Value(aIntervalU);
    if (Precision::IsInfinite(aParamU))
      continue;

    int aIntervalV = theIntervals[1].Lower();
    for (; aIntervalV <= theIntervals[1].Upper(); ++aIntervalV)
    {
      gp_Dir       aNorm;
      const double aParamV = theIntervals[1].Value(aIntervalV);
      if (Precision::IsInfinite(aParamV))
        continue;

      if (GeomLib::NormEstim(theSurf, gp_Pnt2d(aParamU, aParamV), Precision::Confusion(), aNorm)
          != 0)
      {
        return true;
      }
      // TODO: do not split intervals if there is no normal in the middle of interval.
    }
  }

  return false;
}
} // namespace

//=================================================================================================

void BRepMesh_NURBSRangeSplitter::AdjustRange()
{
  BRepMesh_DefaultRangeSplitter::AdjustRange();
  mySurfaceType = GetSurface()->GetType();

  if (mySurfaceType == GeomAbs_BezierSurface)
  {
    const std::pair<double, double>& aRangeU = GetRangeU();
    const std::pair<double, double>& aRangeV = GetRangeV();

    myIsValid = !(aRangeU.first < -0.5 || aRangeU.second > 1.5 || aRangeV.first < -0.5
                  || aRangeV.second > 1.5);
  }
}

//=================================================================================================

Handle(IMeshData::ListOfPnt2d) BRepMesh_NURBSRangeSplitter::GenerateSurfaceNodes(
  const IMeshTools_Parameters& theParameters) const
{
  if (!initParameters())
  {
    return Handle(IMeshData::ListOfPnt2d)();
  }

  const std::pair<double, double>& aRangeU = GetRangeU();
  const std::pair<double, double>& aRangeV = GetRangeV();
  const std::pair<double, double>& aDelta  = GetDelta();

  const double                            aDefFace = GetDFace()->GetDeflection();
  const occ::handle<BRepAdaptor_Surface>& gFace    = GetSurface();
  occ::handle<Geom_Surface>               aSurface = gFace->Surface().Surface();

  const occ::handle<NCollection_IncAllocator> aTmpAlloc =
    new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE);

  const Handle(IMeshData::SequenceOfReal) aParams[2] = {
    computeGrainAndFilterParameters(GetParametersU(),
                                    gFace->UResolution(aDefFace),
                                    (aRangeU.second - aRangeU.first),
                                    aDelta.first,
                                    theParameters,
                                    aTmpAlloc),

    computeGrainAndFilterParameters(GetParametersV(),
                                    gFace->VResolution(aDefFace),
                                    (aRangeV.second - aRangeV.first),
                                    aDelta.second,
                                    theParameters,
                                    aTmpAlloc)};

  // check intermediate isolines
  Handle(IMeshData::MapOfReal) aFixedParams[2] = {new IMeshData::MapOfReal(1, aTmpAlloc),
                                                  new IMeshData::MapOfReal(1, aTmpAlloc)};

  const Handle(IMeshData::MapOfReal) aParamsToRemove[2] = {
    AnalyticalFilter(GetDFace(),
                     GeomAbs_IsoV,
                     aParams[1],
                     aParams[0],
                     aFixedParams[1],
                     aFixedParams[0])
      .GetControlParametersToRemove(theParameters),

    AnalyticalFilter(GetDFace(),
                     GeomAbs_IsoU,
                     aParams[0],
                     aParams[1],
                     aFixedParams[0],
                     aFixedParams[1])
      .GetControlParametersToRemove(theParameters),
  };

  NCollection_MapAlgo::Subtract(*aParamsToRemove[0], *aFixedParams[0]);
  NCollection_MapAlgo::Subtract(*aParamsToRemove[1], *aFixedParams[1]);

  // insert nodes of the regular grid
  Handle(IMeshData::ListOfPnt2d) aNodes =
    new IMeshData::ListOfPnt2d(new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE));

  // insert nodes of the regular grid
  for (int i = 1; i <= aParams[0]->Length(); ++i)
  {
    const double aParam1 = aParams[0]->Value(i);
    if (aParamsToRemove[0]->Contains(aParam1))
    {
      continue;
    }

    for (int j = 1; j <= aParams[1]->Length(); ++j)
    {
      const double aParam2 = aParams[1]->Value(j);
      if (aParamsToRemove[1]->Contains(aParam2))
      {
        continue;
      }

      aNodes->Append(gp_Pnt2d(aParam1, aParam2));
    }
  }

  return aNodes;
}

//=================================================================================================

int BRepMesh_NURBSRangeSplitter::getUndefinedIntervalNb(
  const occ::handle<Adaptor3d_Surface>& theSurface,
  const bool                            isU,
  const GeomAbs_Shape /*theContinuity*/) const
{
  return (isU ? theSurface->NbUPoles() : theSurface->NbVPoles()) - 1;
}

//=================================================================================================

void BRepMesh_NURBSRangeSplitter::getUndefinedInterval(
  const occ::handle<Adaptor3d_Surface>& theSurface,
  const bool                            isU,
  const GeomAbs_Shape                   theContinuity,
  const std::pair<double, double>&      theRange,
  NCollection_Array1<double>&           theIntervals) const
{
  int aIntervalsNb =
    isU ? theSurface->NbUIntervals(theContinuity) : theSurface->NbVIntervals(theContinuity);

  if (aIntervalsNb == 1)
  {
    aIntervalsNb = getUndefinedIntervalNb(theSurface, isU, theContinuity);
    if (aIntervalsNb > 1)
    {
      theIntervals       = NCollection_Array1<double>(1, aIntervalsNb - 1);
      const double aDiff = (theRange.second - theRange.first) / aIntervalsNb;
      for (int i = theIntervals.Lower(); i <= theIntervals.Upper(); ++i)
      {
        theIntervals.SetValue(i, theRange.first + i * aDiff);
      }
    }
  }

  if (theIntervals.IsEmpty())
  {
    theIntervals = NCollection_Array1<double>(1, aIntervalsNb + 1);
    if (isU)
    {
      theSurface->UIntervals(theIntervals, theContinuity);
    }
    else
    {
      theSurface->VIntervals(theIntervals, theContinuity);
    }
  }
}

//=================================================================================================

bool BRepMesh_NURBSRangeSplitter::initParameters() const
{
  const GeomAbs_Shape                     aContinuity = GeomAbs_CN;
  const occ::handle<BRepAdaptor_Surface>& aSurface    = GetSurface();

  NCollection_Array1<double> aIntervals[2];
  getUndefinedInterval(aSurface, true, aContinuity, GetRangeU(), aIntervals[0]);
  getUndefinedInterval(aSurface, false, aContinuity, GetRangeV(), aIntervals[1]);

  const bool isSplitIntervals = toSplitIntervals(aSurface->Surface().Surface(), aIntervals);

  if (!initParamsFromIntervals(aIntervals[0],
                               GetRangeU(),
                               isSplitIntervals,
                               const_cast<IMeshData::IMapOfReal&>(GetParametersU())))
  {
    // if (!grabParamsOfEdges (Edge_Frontier, Param_U))
    {
      return false;
    }
  }

  if (!initParamsFromIntervals(aIntervals[1],
                               GetRangeV(),
                               isSplitIntervals,
                               const_cast<IMeshData::IMapOfReal&>(GetParametersV())))
  {
    // if (!grabParamsOfEdges (Edge_Frontier, Param_V))
    {
      return false;
    }
  }

  return grabParamsOfEdges(Edge_Internal, Param_U | Param_V);
}

//=================================================================================================

bool BRepMesh_NURBSRangeSplitter::grabParamsOfEdges(const EdgeType theEdgeType,
                                                    const int      theParamDimensionFlag) const
{
  if ((theParamDimensionFlag & (Param_U | Param_V)) == 0)
  {
    return false;
  }

  const IMeshData::IFaceHandle& aDFace = GetDFace();
  for (int aWireIt = 0; aWireIt < aDFace->WiresNb(); ++aWireIt)
  {
    const IMeshData::IWireHandle& aDWire = aDFace->GetWire(aWireIt);
    for (int aEdgeIt = 0; aEdgeIt < aDWire->EdgesNb(); ++aEdgeIt)
    {
      const IMeshData::IEdgePtr& aDEdge = aDWire->GetEdge(aEdgeIt);
      for (int aPCurveIt = 0; aPCurveIt < aDEdge->PCurvesNb(); ++aPCurveIt)
      {
        const IMeshData::IPCurveHandle& aDPCurve = aDEdge->GetPCurve(aPCurveIt);
        if (aDPCurve->GetFace() == aDFace)
        {
          if (theEdgeType == Edge_Internal && !aDPCurve->IsInternal())
          {
            continue;
          }

          for (int aPointIt = 0; aPointIt < aDPCurve->ParametersNb(); ++aPointIt)
          {
            const gp_Pnt2d& aPnt2d = aDPCurve->GetPoint(aPointIt);
            if (theParamDimensionFlag & Param_U)
            {
              const_cast<IMeshData::IMapOfReal&>(GetParametersU()).Add(aPnt2d.X());
            }

            if (theParamDimensionFlag & Param_V)
            {
              const_cast<IMeshData::IMapOfReal&>(GetParametersV()).Add(aPnt2d.Y());
            }
          }
        }
      }
    }
  }

  return true;
}

//=================================================================================================

Handle(IMeshData::SequenceOfReal) BRepMesh_NURBSRangeSplitter::computeGrainAndFilterParameters(
  const IMeshData::IMapOfReal&                 theSourceParams,
  const double                                 theTol2d,
  const double                                 theRangeDiff,
  const double                                 theDelta,
  const IMeshTools_Parameters&                 theParameters,
  const occ::handle<NCollection_IncAllocator>& theAllocator) const
{
  // Sort and filter sequence of parameters
  double aMinDiff = Precision::PConfusion();
  if (theDelta < 1.)
  {
    aMinDiff /= theDelta;
  }

  const occ::handle<BRepAdaptor_Surface>& aSurface = GetSurface();
  const double aMinSize2d = std::max(aSurface->UResolution(theParameters.MinSize),
                                     aSurface->VResolution(theParameters.MinSize));

  aMinDiff = std::max(aMinSize2d, aMinDiff);

  const double aDiffMaxLim = 0.1 * theRangeDiff;
  const double aDiffMinLim = std::max(0.005 * theRangeDiff, 2. * theTol2d);
  const double aDiff       = std::max(aMinSize2d, std::min(aDiffMaxLim, aDiffMinLim));
  return filterParameters(theSourceParams, aMinDiff, aDiff, theAllocator);
}

//=================================================================================================

Handle(IMeshData::SequenceOfReal) BRepMesh_NURBSRangeSplitter::filterParameters(
  const IMeshData::IMapOfReal&                 theParams,
  const double                                 theMinDist,
  const double                                 theFilterDist,
  const occ::handle<NCollection_IncAllocator>& theAllocator) const
{
  Handle(IMeshData::SequenceOfReal) aResult = new IMeshData::SequenceOfReal(theAllocator);

  // Sort sequence of parameters
  const int anInitLen = theParams.Extent();

  if (anInitLen < 1)
  {
    return aResult;
  }

  NCollection_Array1<double> aParamArray(1, anInitLen);
  int                        j;
  for (j = 1; j <= anInitLen; j++)
    aParamArray(j) = theParams(j);

  std::sort(aParamArray.begin(), aParamArray.end());

  // mandatory pre-filtering using the first (minimal) filter value
  int aParamLength = 1;
  for (j = 2; j <= anInitLen; j++)
  {
    if ((aParamArray(j) - aParamArray(aParamLength)) > theMinDist)
    {
      if (++aParamLength < j)
        aParamArray(aParamLength) = aParamArray(j);
    }
  }

  // perform filtering on series
  double aLastAdded, aLastCandidate;
  bool   isCandidateDefined = false;
  aLastAdded                = aParamArray(1);
  aLastCandidate            = aLastAdded;
  aResult->Append(aLastAdded);

  for (j = 2; j < aParamLength; j++)
  {
    double aVal = aParamArray(j);
    if (aVal - aLastAdded > theFilterDist)
    {
      // adds the parameter
      if (isCandidateDefined)
      {
        aLastAdded         = aLastCandidate;
        isCandidateDefined = false;
        j--;
      }
      else
      {
        aLastAdded = aVal;
      }
      aResult->Append(aLastAdded);
      continue;
    }

    aLastCandidate     = aVal;
    isCandidateDefined = true;
  }
  aResult->Append(aParamArray(aParamLength));

  return aResult;
}
