//  -- File:	IntCurveSurface_IntersectionSegment.cdl
//  -- Created:	Wed Apr  7 16:47:10 1993
//  -- Author:	Laurent BUCHARD
//  --		<lbr@sdsun2>
//  ---Copyright:	 Matra Datavision 1993


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

