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
