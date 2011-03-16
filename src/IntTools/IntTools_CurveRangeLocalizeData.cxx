// File:	IntTools_CurveRangeLocalizeData.cxx
// Created:	Fri Oct 14 19:59:41 2005
// Author:	Mikhail KLOKOV
//		<mkk@kurox>


#include <IntTools_CurveRangeLocalizeData.ixx>
#include <IntTools_ListIteratorOfListOfCurveRangeSample.hxx>
#include <IntTools_ListIteratorOfListOfBox.hxx>
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

