// Created on: 2005-10-14
// Created by: Mikhail KLOKOV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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


#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_ListIteratorOfListOfBox.hxx>
#include <IntTools_ListIteratorOfListOfSurfaceRangeSample.hxx>
#include <IntTools_MapIteratorOfMapOfSurfaceSample.hxx>
#include <IntTools_SurfaceRangeLocalizeData.hxx>
#include <IntTools_SurfaceRangeSample.hxx>
#include <Precision.hxx>

IntTools_SurfaceRangeLocalizeData::IntTools_SurfaceRangeLocalizeData()
{
  myNbSampleU = 1;
  myNbSampleV = 1;
  myMinRangeU = 0.;
  myMinRangeV = 0.;
//  Modified by skv - Thu Nov  3 11:58:24 2005 Optimization Begin
  myDeflection = 0.;
  myUIndMin    = 0;
  myUIndMax    = 0;
  myVIndMin    = 0;
  myVIndMax    = 0;
//  Modified by skv - Thu Nov  3 11:58:24 2005 Optimization End
}


IntTools_SurfaceRangeLocalizeData::IntTools_SurfaceRangeLocalizeData(const Standard_Integer theNbSampleU,
								     const Standard_Integer theNbSampleV,
								     const Standard_Real theMinRangeU,
								     const Standard_Real theMinRangeV)
{
  myNbSampleU = theNbSampleU;
  myNbSampleV = theNbSampleV;
  myMinRangeU = theMinRangeU;
  myMinRangeV = theMinRangeV;
//  Modified by skv - Thu Nov  3 11:58:24 2005 Optimization Begin
  myDeflection = 0.;
  myUIndMin    = 0;
  myUIndMax    = 0;
  myVIndMin    = 0;
  myVIndMax    = 0;
//  Modified by skv - Thu Nov  3 11:58:24 2005 Optimization End
}

IntTools_SurfaceRangeLocalizeData::IntTools_SurfaceRangeLocalizeData(const IntTools_SurfaceRangeLocalizeData& Other)
{
  Assign(Other);
}

IntTools_SurfaceRangeLocalizeData& IntTools_SurfaceRangeLocalizeData::Assign(const IntTools_SurfaceRangeLocalizeData& Other) 
{
  myNbSampleU = Other.myNbSampleU;
  myNbSampleV = Other.myNbSampleV;
  myMinRangeU = Other.myMinRangeU;
  myMinRangeV = Other.myMinRangeV;
  myMapRangeOut.Clear();
  myMapRangeOut = Other.myMapRangeOut;
  myMapBox.Clear();
  myMapBox   =  Other.myMapBox;
//  Modified by skv - Thu Nov  3 11:58:24 2005 Optimization Begin
  myDeflection = Other.myDeflection;
  myUIndMin    = Other.myUIndMin;
  myUIndMax    = Other.myUIndMax;
  myVIndMin    = Other.myVIndMin;
  myVIndMax    = Other.myVIndMax;

  if (!Other.myUParams.IsNull())
    myUParams = Other.myUParams;

  if (!Other.myVParams.IsNull())
    myVParams = Other.myVParams;

  if (!Other.myGridPoints.IsNull())
    myGridPoints = Other.myGridPoints;
//  Modified by skv - Thu Nov  3 11:58:24 2005 Optimization End
  return *(this);
}


void IntTools_SurfaceRangeLocalizeData::AddOutRange(const IntTools_SurfaceRangeSample& theRange) 
{
  myMapRangeOut.Add(theRange);
//   myMapBox.UnBind(theRange);
}

void IntTools_SurfaceRangeLocalizeData::AddBox(const IntTools_SurfaceRangeSample& theRange,
					       const Bnd_Box& theBox) 
{
  myMapBox.Bind(theRange, theBox);
}

Standard_Boolean IntTools_SurfaceRangeLocalizeData::FindBox(const IntTools_SurfaceRangeSample& theRange,
							    Bnd_Box& theBox) const
{
  if(myMapBox.IsBound(theRange)) {
    theBox = myMapBox(theRange);
    return Standard_True;
  }
  return Standard_False;
}

Standard_Boolean IntTools_SurfaceRangeLocalizeData::IsRangeOut(const IntTools_SurfaceRangeSample& theRange) const
{
  return myMapRangeOut.Contains(theRange);
}

void IntTools_SurfaceRangeLocalizeData::ListRangeOut(IntTools_ListOfSurfaceRangeSample& theList) const
{
  IntTools_MapIteratorOfMapOfSurfaceSample anIt( myMapRangeOut);

  for(; anIt.More(); anIt.Next())
    theList.Append(anIt.Key());
}

void IntTools_SurfaceRangeLocalizeData::RemoveRangeOutAll() 
{
  myMapRangeOut.Clear();
}

//  Modified by skv - Thu Nov  3 11:58:24 2005 Optimization Begin
void IntTools_SurfaceRangeLocalizeData::SetRangeUGrid
                            (const Standard_Integer theNbUGrid)
{
  myUIndMin   = 0;
  myUIndMax   = 0;
  myVIndMin   = 0;
  myVIndMax   = 0;

  if (myUParams.IsNull() || theNbUGrid != myUParams->Length()) {
    myUParams = new TColStd_HArray1OfReal(1, theNbUGrid);

    if (!myVParams.IsNull()) {
      myGridPoints = new TColgp_HArray2OfPnt(1, theNbUGrid,
					     1, myVParams->Length());
    }
  }
}

void IntTools_SurfaceRangeLocalizeData::SetRangeVGrid
                            (const Standard_Integer theNbVGrid)
{
  myUIndMin   = 0;
  myUIndMax   = 0;
  myVIndMin   = 0;
  myVIndMax   = 0;

  if (myVParams.IsNull() || theNbVGrid != myVParams->Length()) {
    myVParams = new TColStd_HArray1OfReal(1, theNbVGrid);

    if (!myUParams.IsNull()) {
      myGridPoints = new TColgp_HArray2OfPnt(1, myUParams->Length(),
					    1, theNbVGrid);
    }
  }
}

void IntTools_SurfaceRangeLocalizeData::SetFrame(const Standard_Real theUMin,
						 const Standard_Real theUMax,
						 const Standard_Real theVMin,
						 const Standard_Real theVMax)
{
  myUIndMin   = 0;
  myUIndMax   = 0;
  myVIndMin   = 0;
  myVIndMax   = 0;

  if (myUParams.IsNull() || myVParams.IsNull()) {
    return;
  }

  Standard_Integer i;
  Standard_Integer aLmI;
  Standard_Integer aLen = myUParams->Length();

  // Compute frame along U.
  for (i = 1; i <= aLen; i++) {
    if (myUIndMin == 0 && theUMin < myUParams->Value(i))
      myUIndMin = i;

    aLmI = aLen - i + 1;

    if (myUIndMax == 0 && theUMax > myUParams->Value(aLmI))
      myUIndMax = aLmI;
  }

  if (myUIndMin == 0)
    myUIndMin = aLen + 1;

  // Compute frame along V.
  aLen = myVParams->Length();

  for (i = 1; i <= aLen; i++) {
    if (myVIndMin == 0 && theVMin < myVParams->Value(i))
      myVIndMin = i;

    aLmI = aLen - i + 1;

    if (myVIndMax == 0 && theVMax > myVParams->Value(aLmI))
      myVIndMax = aLmI;
  }

  if (myVIndMin == 0)
    myVIndMin = aLen + 1;

}

const gp_Pnt &IntTools_SurfaceRangeLocalizeData::GetPointInFrame
                       (const Standard_Integer theUIndex,
			const Standard_Integer theVIndex) const
{
  Standard_Integer aFrmUInd = theUIndex + myUIndMin - 1;
  Standard_Integer aFrmVInd = theVIndex + myVIndMin - 1;

  if (aFrmUInd > myUIndMax || aFrmVInd > myVIndMax)
    return gp::Origin();

  return myGridPoints->Value(aFrmUInd, aFrmVInd);
}

Standard_Real IntTools_SurfaceRangeLocalizeData::GetUParamInFrame
                       (const Standard_Integer theIndex) const
{
  Standard_Integer aFrmInd = theIndex + myUIndMin - 1;

  if (aFrmInd > myUIndMax)
    return Precision::Infinite();

  return myUParams->Value(aFrmInd);
}

Standard_Real IntTools_SurfaceRangeLocalizeData::GetVParamInFrame
                       (const Standard_Integer theIndex) const
{
  Standard_Integer aFrmInd = theIndex + myVIndMin - 1;

  if (aFrmInd > myVIndMax)
    return Precision::Infinite();

  return myVParams->Value(aFrmInd);
}

void IntTools_SurfaceRangeLocalizeData::ClearGrid()
{
  myDeflection = 0.;
  myUIndMin    = 0;
  myUIndMax    = 0;
  myVIndMin    = 0;
  myVIndMax    = 0;

  if (!myUParams.IsNull())
    myUParams.Nullify();

  if (!myVParams.IsNull())
    myVParams.Nullify();

  if (!myGridPoints.IsNull())
    myGridPoints.Nullify();
}
//  Modified by skv - Thu Nov  3 11:58:24 2005 Optimization End
