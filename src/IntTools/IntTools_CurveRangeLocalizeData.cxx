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
#include <IntTools_CurveRangeLocalizeData.hxx>
#include <IntTools_CurveRangeSample.hxx>
#include <IntTools_ListIteratorOfListOfBox.hxx>
#include <IntTools_ListIteratorOfListOfCurveRangeSample.hxx>
#include <IntTools_MapIteratorOfMapOfCurveSample.hxx>

IntTools_CurveRangeLocalizeData::IntTools_CurveRangeLocalizeData(const Standard_Integer theNbSample,
								 const Standard_Real theMinRange)
{
  myNbSampleC = theNbSample;
  myMinRangeC = theMinRange;
}

void IntTools_CurveRangeLocalizeData::AddOutRange(const IntTools_CurveRangeSample& theRange) 
{
  myMapRangeOut.Add(theRange);
  myMapBox.UnBind(theRange);
}

void IntTools_CurveRangeLocalizeData::AddBox(const IntTools_CurveRangeSample& theRange,
					     const Bnd_Box& theBox) 
{
  myMapBox.Bind(theRange, theBox);
}

Standard_Boolean IntTools_CurveRangeLocalizeData::FindBox(const IntTools_CurveRangeSample& theRange,Bnd_Box& theBox) const
{
  if(myMapBox.IsBound(theRange)) {
    theBox = myMapBox(theRange);
    return Standard_True;
  }
  return Standard_False;
}

Standard_Boolean IntTools_CurveRangeLocalizeData::IsRangeOut(const IntTools_CurveRangeSample& theRange) const
{
  return myMapRangeOut.Contains(theRange);
}

void IntTools_CurveRangeLocalizeData::ListRangeOut(IntTools_ListOfCurveRangeSample& theList) const
{
  IntTools_MapIteratorOfMapOfCurveSample anIt(myMapRangeOut);

  for(; anIt.More(); anIt.Next())
    theList.Append(anIt.Key());
}

