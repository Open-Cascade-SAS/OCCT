// File:	IntTools_CurveRangeSample.cxx
// Created:	Wed Oct  5 17:04:22 2005
// Author:	Mikhail KLOKOV
//		<mkk@kurox>


#include <IntTools_CurveRangeSample.ixx>

IntTools_CurveRangeSample::IntTools_CurveRangeSample()
{
  myIndex = 0;
}

IntTools_CurveRangeSample::IntTools_CurveRangeSample(const Standard_Integer theIndex)
{
  myIndex = theIndex;
}

IntTools_Range IntTools_CurveRangeSample::GetRange(const Standard_Real    theFirst,
						   const Standard_Real    theLast,
						   const Standard_Integer theNbSample) const
{
  Standard_Real diffC = theLast - theFirst;
  IntTools_Range aResult;

  if(GetDepth() <= 0) {
    aResult.SetFirst(theFirst);
    aResult.SetLast(theLast);
  }
  else {
    Standard_Real tmp = pow(Standard_Real(theNbSample), Standard_Real(GetDepth()));
    Standard_Real localdiffC = diffC / Standard_Real(tmp);
    Standard_Real aFirstC = theFirst + Standard_Real(myIndex) * localdiffC;
    Standard_Real aLastC = aFirstC + localdiffC;
    aResult.SetFirst(aFirstC);
    aResult.SetLast(aLastC);
  }
  return aResult;
}
