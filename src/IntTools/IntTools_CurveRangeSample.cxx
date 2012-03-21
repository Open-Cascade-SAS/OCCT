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
