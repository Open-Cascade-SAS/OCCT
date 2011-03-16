// File:	IntTools_SurfaceRangeSample.cxx
// Created:	Wed Oct  5 16:54:37 2005
// Author:	Mikhail KLOKOV
//		<mkk@kurox>


#include <IntTools_SurfaceRangeSample.ixx>

IntTools_SurfaceRangeSample::IntTools_SurfaceRangeSample()
{
}

IntTools_SurfaceRangeSample::IntTools_SurfaceRangeSample(const Standard_Integer theIndexU,const Standard_Integer theDepthU,
							 const Standard_Integer theIndexV,const Standard_Integer theDepthV)
{
  myRangeU.SetRangeIndex(theIndexU);
  myRangeU.SetDepth(theDepthU);
  myRangeV.SetRangeIndex(theIndexV);
  myRangeV.SetDepth(theDepthV);
}

IntTools_SurfaceRangeSample::IntTools_SurfaceRangeSample(const IntTools_CurveRangeSample& theRangeU,
							 const IntTools_CurveRangeSample& theRangeV)
{
  myRangeU = theRangeU;
  myRangeV = theRangeV;
}

IntTools_SurfaceRangeSample::IntTools_SurfaceRangeSample(const IntTools_SurfaceRangeSample& Other)
{
  Assign(Other);
}

IntTools_SurfaceRangeSample& IntTools_SurfaceRangeSample::Assign(const IntTools_SurfaceRangeSample& Other) 
{
  myRangeU = Other.myRangeU;
  myRangeV = Other.myRangeV;
  return (*this);
}


IntTools_Range IntTools_SurfaceRangeSample::GetRangeU(const Standard_Real theFirstU,const Standard_Real theLastU,
						      const Standard_Integer theNbSampleU) const
{
  return myRangeU.GetRange(theFirstU, theLastU, theNbSampleU);
}

IntTools_Range IntTools_SurfaceRangeSample::GetRangeV(const Standard_Real theFirstV,const Standard_Real theLastV,
						      const Standard_Integer theNbSampleV) const
{
  return myRangeV.GetRange(theFirstV, theLastV, theNbSampleV);
}
