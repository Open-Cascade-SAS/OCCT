// Created on: 1993-04-07
// Created by: Laurent BUCHARD
// Copyright (c) 1993-1999 Matra Datavision
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

#include <IntCurveSurface_IntersectionSegment.ixx>


IntCurveSurface_IntersectionSegment::IntCurveSurface_IntersectionSegment() 
{ }
//================================================================================
IntCurveSurface_IntersectionSegment::IntCurveSurface_IntersectionSegment(const IntCurveSurface_IntersectionPoint& P1,
									 const IntCurveSurface_IntersectionPoint& P2):
       myP1(P1),myP2(P2)
{ 
} 
//================================================================================
void IntCurveSurface_IntersectionSegment::SetValues(const IntCurveSurface_IntersectionPoint& P1,
						    const IntCurveSurface_IntersectionPoint& P2) { 
  myP1 = P1; 
  myP2 = P2;
} 
//================================================================================
void IntCurveSurface_IntersectionSegment::Values(IntCurveSurface_IntersectionPoint& P1,
						 IntCurveSurface_IntersectionPoint& P2) const
{ 
  P1 = myP1; 
  P2 = myP2;
} 
//================================================================================
void IntCurveSurface_IntersectionSegment::FirstPoint(IntCurveSurface_IntersectionPoint& P1) const { 
  P1 = myP1;
}
//================================================================================
void IntCurveSurface_IntersectionSegment::SecondPoint(IntCurveSurface_IntersectionPoint& P2) const { 
  P2 = myP2;
}
//================================================================================
const IntCurveSurface_IntersectionPoint &
  IntCurveSurface_IntersectionSegment::FirstPoint() const { 
  return(myP1);
}
//================================================================================
const IntCurveSurface_IntersectionPoint &
  IntCurveSurface_IntersectionSegment::SecondPoint() const { 
  return(myP2);
}
//================================================================================
void IntCurveSurface_IntersectionSegment::Dump() const { 
  cout<<"\nIntersectionSegment : "<<endl;
  myP1.Dump();
  myP2.Dump();
  cout<<endl;
}

