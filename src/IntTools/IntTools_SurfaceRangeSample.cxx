// Created on: 2005-10-05
// Created by: Mikhail KLOKOV
// Copyright (c) 2005-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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
