// Created on: 1998-05-18
// Created by: Andre LIEUTIER
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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



#include <Plate_SampledCurveConstraint.ixx>
#include <Plate_PinpointConstraint.hxx>
#include <Plate_SequenceOfPinpointConstraint.hxx>

static inline Standard_Real B0( Standard_Real t)
{
  Standard_Real s = t;
  if(s<0.) s = -s;
  s = 1. - s;
  if(s<0.) s = 0;
  return s;
}

Plate_SampledCurveConstraint::Plate_SampledCurveConstraint(const Plate_SequenceOfPinpointConstraint &SOPPC,
							   const Standard_Integer n)
:myLXYZC(n,SOPPC.Length())
{
  Standard_Integer m = SOPPC.Length();

  if (n > m)  Standard_DimensionMismatch::Raise();
  for(Standard_Integer index =1; index <= m;index++)
    myLXYZC.SetPPC(index,SOPPC(index));

  Standard_Real ratio = Standard_Real(n+1) /Standard_Real(m+1); 
  for (Standard_Integer i=1;i<=n;i++)
    for (Standard_Integer j=1;j<=m;j++)
      {
	myLXYZC.SetCoeff(i,j,B0(ratio*j - i));
      }
}
