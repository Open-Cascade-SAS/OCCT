// Created on: 2005-10-05
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


#include <IntTools_CurveRangeSample.hxx>
#include <IntTools_Range.hxx>
#include <IntTools_SurfaceRangeSample.hxx>

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
