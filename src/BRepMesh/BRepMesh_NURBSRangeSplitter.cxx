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
#include <GeomAdaptor_Curve.hxx>
#include <IMeshTools_Parameters.hxx>
#include <IMeshData_Wire.hxx>
#include <IMeshData_Edge.hxx>
#include <IMeshData_PCurve.hxx>
#include <GeomAbs_IsoType.hxx>
#include <BRepMesh_GeomTool.hxx>
#include <NCollection_Handle.hxx>
#include <algorithm>

namespace
{
  class AnalyticalFilter
  {
  public:
    //! Constructor.
    AnalyticalFilter(
      const IMeshData::IFaceHandle&             theDFace,
      const GeomAbs_IsoType                     theIsoType,
      const Handle(IMeshData::SequenceOfReal)&  theParams,
      const Handle(IMeshData::SequenceOfReal)&  theControlParams,
      const Handle(IMeshData::MapOfReal)&       theParamsForbiddenToRemove,
      const Handle(IMeshData::MapOfReal)&       theControlParamsForbiddenToRemove)
      : myDFace(theDFace),
        mySurface(myDFace->GetSurface()->ChangeSurface().Surface().Surface()),
        myIsoU(theIsoType == GeomAbs_IsoU),
        myParams(theParams),
        myControlParams(theControlParams),
        myParamsForbiddenToRemove(theParamsForbiddenToRemove),
        myControlParamsForbiddenToRemove(theControlParamsForbiddenToRemove),
        myAllocator(new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE)),
        myControlParamsToRemove(new IMeshData::MapOfReal(1, myAllocator))
    {
    }

    //! Returns map of parameters supposed to be removed.
    const Handle(IMeshData::MapOfReal)& GetControlParametersToRemove(
      const IMeshTools_Parameters& theParameters)
    {
      myParameters = theParameters;

      if (myParameters.MinSize <= Precision::Confusion())
      {
        myParameters.MinSize = 
          Max(IMeshTools_Parameters::RelMinSize() * myParameters.Deflection,
              Precision::Confusion());
      }

      Standard_Integer aStartIndex, aEndIndex;
      if (myIsoU)
      {
        aStartIndex = 1;
        aEndIndex = myParams->Length();
      }
      else
      {
        aStartIndex = 2;
        aEndIndex = myParams->Length() - 1;
      }

      for (Standard_Integer i = aStartIndex; i <= aEndIndex; ++i)
      {
        myCurrParam = myParams->Value(i);
        myIso = new GeomAdaptor_Curve(myIsoU ? mySurface->UIso(myCurrParam) : mySurface->VIso(myCurrParam));

        myPrevControlParam = myControlParams->Value(1);
        myIso->D1(myPrevControlParam, myPrevControlPnt, myPrevControlVec);
        for (Standard_Integer j = 2; j <= myControlParams->Length();)
        {
          j += checkControlPointAndMoveOn(j);
        }
      }

      return myControlParamsToRemove;
    }

  private:

    //! Checks the given control point for deviation.
    //! Returns number of steps to be used to move point iterator.
    Standard_Integer checkControlPointAndMoveOn(const Standard_Integer theIndex)
    {
      Standard_Integer aMoveSteps = 0;
      myCurrControlParam = myControlParams->Value(theIndex);
      myIso->D1(myCurrControlParam, myCurrControlPnt, myCurrControlVec);

      const Standard_Real aMidParam = 0.5 * (myPrevControlParam + myCurrControlParam);
      const gp_Pnt aMidPnt = myIso->Value(aMidParam);

      const Standard_Real aSqDist = BRepMesh_GeomTool::SquareDeflectionOfSegment(
        myPrevControlPnt, myCurrControlPnt, aMidPnt);

      const Standard_Real aSqMaxDeflection = myDFace->GetDeflection() * myDFace->GetDeflection();
      if ((aSqDist > aSqMaxDeflection) &&
          aSqDist > myParameters.MinSize * myParameters.MinSize)
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
        if (aSqDist < aSqMaxDeflection &&
            myControlParams->Length() > 3 &&
            theIndex < myControlParams->Length())
        {
          // Remove too dense points
          const Standard_Real aTmpParam = myControlParams->Value(theIndex + 1);
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
    Standard_Boolean checkParameterForDeflectionAndUpdateCache(const Standard_Real theParam)
    {
      gp_Pnt aTmpPnt;
      gp_Vec aTmpVec;
      myIso->D1(theParam, aTmpPnt, aTmpVec);

      const Standard_Real aTmpMidParam = 0.5 * (myPrevControlParam + theParam);
      const gp_Pnt        aTmpMidPnt = myIso->Value(aTmpMidParam);

      // Lets check next parameter.
      // If it also fits deflection, we can remove previous parameter.
      const Standard_Real aSqDist = BRepMesh_GeomTool::SquareDeflectionOfSegment(
        myPrevControlPnt, aTmpPnt, aTmpMidPnt);

      if (aSqDist < myDFace->GetDeflection() * myDFace->GetDeflection())
      {
        // Lets check parameters for angular deflection.
        if (myPrevControlVec.SquareMagnitude() < gp::Resolution() ||
            aTmpVec.SquareMagnitude()          < gp::Resolution() ||
            myPrevControlVec.Angle(aTmpVec)    < myParameters.Angle)
        {
          // For current Iso line we can remove this parameter.
          myControlParamsToRemove->Add(myCurrControlParam);
          myCurrControlParam = theParam;
          myCurrControlPnt   = aTmpPnt;
          myCurrControlVec   = aTmpVec;
          return Standard_True;
        }
        else
        {
          // We have found a place on the surface refusing 
          // removement of this parameter.
          myParamsForbiddenToRemove       ->Add(myCurrParam);
          myControlParamsForbiddenToRemove->Add(myCurrControlParam);
        }
      }

      return Standard_False;
    }

  private:

    IMeshData::IFaceHandle                myDFace;
    Handle(Geom_Surface)                  mySurface;
    Standard_Boolean                      myIsoU;
    Handle(IMeshData::SequenceOfReal)     myParams;
    Handle(IMeshData::SequenceOfReal)     myControlParams;

    Handle(IMeshData::MapOfReal)          myParamsForbiddenToRemove;
    Handle(IMeshData::MapOfReal)          myControlParamsForbiddenToRemove;

    Handle(NCollection_IncAllocator)      myAllocator;
    Handle(IMeshData::MapOfReal)          myControlParamsToRemove;


    IMeshTools_Parameters                 myParameters;
    NCollection_Handle<GeomAdaptor_Curve> myIso;

    Standard_Real                         myCurrParam;

    Standard_Real                         myCurrControlParam;
    gp_Pnt                                myCurrControlPnt;
    gp_Vec                                myCurrControlVec;

    Standard_Real                         myPrevControlParam;
    gp_Pnt                                myPrevControlPnt;
    gp_Vec                                myPrevControlVec;
  };

  //! Adds param to map if it fits specified range.
  inline Standard_Boolean addParam(
    const Standard_Real&                           theParam,
    const std::pair<Standard_Real, Standard_Real>& theRange,
    IMeshData::IMapOfReal&                         theParams)
  {
    if (theParam < theRange.first ||
        theParam > theRange.second)
    {
      return Standard_False;
    }

    theParams.Add(theParam);
    return Standard_True;
  }

  //! Initializes parameters map using CN intervals.
  inline Standard_Boolean initParamsFromIntervals(
    const TColStd_Array1OfReal&                    theIntervals,
    const std::pair<Standard_Real, Standard_Real>& theRange,
    const Standard_Boolean                         isSplitIntervals,
    IMeshData::IMapOfReal&                         theParams)
  {
    Standard_Boolean isAdded = Standard_False;

    for (Standard_Integer i = theIntervals.Lower(); i <= theIntervals.Upper(); ++i)
    {
      const Standard_Real aStartParam = theIntervals.Value(i);
      if (addParam(aStartParam, theRange, theParams))
      {
        isAdded = Standard_True;
      }

      if (isSplitIntervals && i < theIntervals.Upper())
      {
        const Standard_Real aMidParam = (aStartParam + theIntervals.Value(i + 1)) / 2.;
        if (addParam(aMidParam, theRange, theParams))
        {
          isAdded = Standard_True;
        }
      }
    }

    return isAdded;
  }
}

//=======================================================================
// Function: AdjustRange
// Purpose : 
//=======================================================================
void BRepMesh_NURBSRangeSplitter::AdjustRange()
{
  BRepMesh_DefaultRangeSplitter::AdjustRange();
  mySurfaceType = GetSurface()->GetType();

  if (mySurfaceType == GeomAbs_BezierSurface)
  {
    const std::pair<Standard_Real, Standard_Real>& aRangeU = GetRangeU();
    const std::pair<Standard_Real, Standard_Real>& aRangeV = GetRangeV();

    myIsValid = !(aRangeU.first  < -0.5 ||
                  aRangeU.second >  1.5 ||
                  aRangeV.first  < -0.5 ||
                  aRangeV.second >  1.5);
  }
}

//=======================================================================
// Function: GenerateSurfaceNodes
// Purpose : 
//=======================================================================
Handle(IMeshData::ListOfPnt2d) BRepMesh_NURBSRangeSplitter::GenerateSurfaceNodes(
  const IMeshTools_Parameters& theParameters) const
{
  initParameters();

  const std::pair<Standard_Real, Standard_Real>& aRangeU = GetRangeU();
  const std::pair<Standard_Real, Standard_Real>& aRangeV = GetRangeV();
  const std::pair<Standard_Real, Standard_Real>& aDelta  = GetDelta ();

  const Standard_Real                 aDefFace = GetDFace()->GetDeflection();
  const Handle(BRepAdaptor_HSurface)& gFace    = GetSurface();
  Handle(Geom_Surface)                aSurface = gFace->ChangeSurface().Surface().Surface();

  const Handle(NCollection_IncAllocator) aTmpAlloc =
    new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE);

  const Handle(IMeshData::SequenceOfReal) aParams[2] = {
    computeGrainAndFilterParameters(GetParametersU(), gFace->UResolution(aDefFace),
      (aRangeU.second - aRangeU.first), aDelta.first,  theParameters, aTmpAlloc),

    computeGrainAndFilterParameters(GetParametersV(), gFace->VResolution(aDefFace),
      (aRangeV.second - aRangeV.first), aDelta.second, theParameters, aTmpAlloc)
  };

  // check intermediate isolines
  Handle(IMeshData::MapOfReal) aFixedParams[2] = {
    new IMeshData::MapOfReal(1, aTmpAlloc),
    new IMeshData::MapOfReal(1, aTmpAlloc)
  };

  const Handle(IMeshData::MapOfReal) aParamsToRemove[2] = {
    AnalyticalFilter(GetDFace(), GeomAbs_IsoV, aParams[1], aParams[0],
      aFixedParams[1], aFixedParams[0]).GetControlParametersToRemove(theParameters),

    AnalyticalFilter(GetDFace(), GeomAbs_IsoU, aParams[0], aParams[1],
      aFixedParams[0], aFixedParams[1]).GetControlParametersToRemove(theParameters),
  };

  aParamsToRemove[0]->Subtract(*aFixedParams[0]);
  aParamsToRemove[1]->Subtract(*aFixedParams[1]);

  // insert nodes of the regular grid
  Handle(IMeshData::ListOfPnt2d) aNodes = new IMeshData::ListOfPnt2d(
    new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE));

  // insert nodes of the regular grid
  for (Standard_Integer i = 1; i <= aParams[0]->Length(); ++i)
  {
    const Standard_Real aParam1 = aParams[0]->Value(i);
    if (aParamsToRemove[0]->Contains(aParam1))
    {
      continue;
    }

    for (Standard_Integer j = 1; j <= aParams[1]->Length(); ++j)
    {
      const Standard_Real aParam2 = aParams[1]->Value(j);
      if (aParamsToRemove[1]->Contains(aParam2))
      {
        continue;
      }

      aNodes->Append(gp_Pnt2d(aParam1, aParam2));
    }
  }

  return aNodes;
}

//=======================================================================
// Function: initParameters
// Purpose : 
//=======================================================================
void BRepMesh_NURBSRangeSplitter::initParameters() const
{
  const Handle(BRepAdaptor_HSurface)& aSurface = GetSurface();

  const GeomAbs_Shape aContinuity = GeomAbs_CN;
  const std::pair<Standard_Integer, Standard_Integer> aIntervalsNb(
    aSurface->NbUIntervals(aContinuity),
    aSurface->NbVIntervals(aContinuity)
  );

  TColStd_Array1OfReal aIntervals[2] = {
    TColStd_Array1OfReal(1, aIntervalsNb.first  + 1),
    TColStd_Array1OfReal(1, aIntervalsNb.second + 1)
  };

  aSurface->UIntervals(aIntervals[0], aContinuity);
  aSurface->VIntervals(aIntervals[1], aContinuity);

  Standard_Boolean isSplitIntervals =
    (aIntervalsNb.first > 1 || aIntervalsNb.second > 1);

  if (!isSplitIntervals &&
      (aSurface->GetType() == GeomAbs_BezierSurface ||
       aSurface->GetType() == GeomAbs_BSplineSurface))
  {
    isSplitIntervals = (aSurface->NbUPoles() > 2 && aSurface->NbVPoles() > 2);
  }

  initParamsFromIntervals(aIntervals[0], GetRangeU(), isSplitIntervals,
                          const_cast<IMeshData::IMapOfReal&>(GetParametersU()));

  initParamsFromIntervals(aIntervals[1], GetRangeV(), isSplitIntervals,
                          const_cast<IMeshData::IMapOfReal&>(GetParametersV()));
}

//=======================================================================
//function : computeGrainAndFilterParameters
//purpose  : 
//=======================================================================
Handle(IMeshData::SequenceOfReal) BRepMesh_NURBSRangeSplitter::computeGrainAndFilterParameters(
  const IMeshData::IMapOfReal&            theSourceParams,
  const Standard_Real                     theTol2d,
  const Standard_Real                     theRangeDiff,
  const Standard_Real                     theDelta,
  const IMeshTools_Parameters&            theParameters,
  const Handle(NCollection_IncAllocator)& theAllocator) const
{
  // Sort and filter sequence of parameters
  Standard_Real aMinDiff = Precision::PConfusion();
  if (theDelta < 1.)
  {
    aMinDiff /= theDelta;
  }

  const Standard_Real aMinSize =
    theParameters.MinSize > Precision::Confusion() ? theParameters.MinSize :
    Max(IMeshTools_Parameters::RelMinSize() * theParameters.Deflection,
        Precision::Confusion());

  aMinDiff = Max(aMinSize, aMinDiff);

  const Standard_Real aDiffMaxLim = 0.1 * theRangeDiff;
  const Standard_Real aDiffMinLim = Max(0.005 * theRangeDiff, 2. * theTol2d);
  const Standard_Real aDiff = Max(aMinSize, Min(aDiffMaxLim, aDiffMinLim));
  return filterParameters(theSourceParams, aMinDiff, aDiff, theAllocator);
}

//=======================================================================
//function : filterParameters
//purpose  : 
//=======================================================================
Handle(IMeshData::SequenceOfReal) BRepMesh_NURBSRangeSplitter::filterParameters(
  const IMeshData::IMapOfReal&            theParams,
  const Standard_Real                     theMinDist,
  const Standard_Real                     theFilterDist,
  const Handle(NCollection_IncAllocator)& theAllocator) const
{
  Handle(IMeshData::SequenceOfReal) aResult = new IMeshData::SequenceOfReal(theAllocator);

  // Sort sequence of parameters
  const Standard_Integer anInitLen = theParams.Extent();

  if (anInitLen < 1)
  {
    return aResult;
  }

  TColStd_Array1OfReal aParamArray(1, anInitLen);
  Standard_Integer j;
  for (j = 1; j <= anInitLen; j++)
    aParamArray(j) = theParams(j);

  std::sort(aParamArray.begin(), aParamArray.end());

  // mandatory pre-filtering using the first (minimal) filter value
  Standard_Integer aParamLength = 1;
  for (j = 2; j <= anInitLen; j++)
  {
    if ((aParamArray(j) - aParamArray(aParamLength)) > theMinDist)
    {
      if (++aParamLength < j)
        aParamArray(aParamLength) = aParamArray(j);
    }
  }

  //perform filtering on series
  Standard_Real aLastAdded, aLastCandidate;
  Standard_Boolean isCandidateDefined = Standard_False;
  aLastAdded = aParamArray(1);
  aLastCandidate = aLastAdded;
  aResult->Append(aLastAdded);

  for (j = 2; j < aParamLength; j++)
  {
    Standard_Real aVal = aParamArray(j);
    if (aVal - aLastAdded > theFilterDist)
    {
      //adds the parameter
      if (isCandidateDefined)
      {
        aLastAdded = aLastCandidate;
        isCandidateDefined = Standard_False;
        j--;
      }
      else
      {
        aLastAdded = aVal;
      }
      aResult->Append(aLastAdded);
      continue;
    }

    aLastCandidate = aVal;
    isCandidateDefined = Standard_True;
  }
  aResult->Append(aParamArray(aParamLength));

  return aResult;
}
