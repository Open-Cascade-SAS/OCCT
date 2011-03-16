// File:	IntTools_BaseRangeSample.cxx
// Created:	Wed Oct  5 16:52:02 2005
// Author:	Mikhail KLOKOV
//		<mkk@kurox>


#include <IntTools_BaseRangeSample.ixx>

IntTools_BaseRangeSample::IntTools_BaseRangeSample()
{
  myDepth = 0;
}

IntTools_BaseRangeSample::IntTools_BaseRangeSample(const Standard_Integer theDepth)
{
  myDepth = theDepth;
}
