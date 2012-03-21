// Created on: 1993-04-16
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



#include <IntCurveSurface_Intersection.ixx>

#include <IntCurveSurface_IntersectionPoint.hxx>
#include <IntCurveSurface_IntersectionSegment.hxx>
#include <IntCurveSurface_TransitionOnCurve.hxx>

#define PARAMEQUAL(a,b) (Abs((a)-(b))< (1e-8))

//================================================================================
IntCurveSurface_Intersection::IntCurveSurface_Intersection(): done(Standard_False) { 
}
//================================================================================
Standard_Boolean IntCurveSurface_Intersection::IsDone() const { return(done); } 
//================================================================================
Standard_Integer IntCurveSurface_Intersection::NbPoints() const { 
  if (!done) {StdFail_NotDone::Raise();}
  return lpnt.Length();
}
//================================================================================
Standard_Integer IntCurveSurface_Intersection::NbSegments() const { 
  if (!done) {StdFail_NotDone::Raise();}
  return lseg.Length();
}
//================================================================================
const IntCurveSurface_IntersectionPoint& IntCurveSurface_Intersection::Point( const Standard_Integer N) const {
  if (!done) {StdFail_NotDone::Raise();}
  return lpnt.Value(N);
}
//================================================================================
const IntCurveSurface_IntersectionSegment& IntCurveSurface_Intersection::Segment( const Standard_Integer N) const {
  if (!done) {StdFail_NotDone::Raise();}
  return lseg.Value(N);
}
//================================================================================
void IntCurveSurface_Intersection::SetValues(const IntCurveSurface_Intersection& Other) {
  if(Other.done) {
    lseg.Clear();
    lpnt.Clear();
    Standard_Integer N = Other.lpnt.Length();
    Standard_Integer i ;
    for( i=1; i<= N ; i++) { 
      lpnt.Append(Other.lpnt.Value(i));
    }
    N = Other.lseg.Length();
    for(i=1; i<= N ; i++) { 
      lseg.Append(Other.lseg.Value(i));
    } 
    done=Standard_True;
  }
  else {
    done=Standard_False;
  }
}
//================================================================================
void IntCurveSurface_Intersection::Append(const IntCurveSurface_Intersection& Other,
//					  const Standard_Real a,
					  const Standard_Real ,
//					  const Standard_Real b) 
					  const Standard_Real ) 
{ 
  Standard_Integer i,ni;
  if(Other.done) { 
    ni = Other.lpnt.Length();
    for(i=1;i<=ni;i++) {   Append(Other.Point(i)); }
    ni = Other.lseg.Length();
    for(i=1;i<=ni;i++) {   Append(Other.Segment(i)); }
  }
}
//================================================================================
void IntCurveSurface_Intersection::Append(const IntCurveSurface_IntersectionPoint& OtherPoint) { 
  Standard_Integer i,ni;
  Standard_Real anu,anv,anw,u,v,w;
  IntCurveSurface_TransitionOnCurve   TrOnCurve,anTrOnCurve;
  gp_Pnt P,anP;
  ni = lpnt.Length();
  for(i=1;i<=ni;i++) {
    OtherPoint.Values(P,u,v,w,TrOnCurve);
    lpnt(i).Values(anP,anu,anv,anw,anTrOnCurve);
    if(PARAMEQUAL(u,anu)) { 
      if(PARAMEQUAL(v,anv)) { 
	if(PARAMEQUAL(w,anw)) { 
	  if(anTrOnCurve==TrOnCurve) { 
	    return;
	  }
	}
      }
    }
  }
  lpnt.Append(OtherPoint);
}
//================================================================================
void IntCurveSurface_Intersection::Append(const IntCurveSurface_IntersectionSegment& OtherSegment) { 
  lseg.Append(OtherSegment);
}
//================================================================================
void IntCurveSurface_Intersection::ResetFields() {
  if(done) {
    lseg.Clear();
    lpnt.Clear();
    done=Standard_False;
  }
}
//================================================================================
void IntCurveSurface_Intersection::Dump() const { 
  if(done) { 
    Standard_Integer i,ni;
    ni = lpnt.Length();
    for(i=1;i<=ni;i++) {   Point(i).Dump(); }
    ni = lseg.Length();
    for(i=1;i<=ni;i++) {   Segment(i).Dump(); }
  }
  else { 
    cout<<" Intersection NotDone"<<endl;
  }
}
