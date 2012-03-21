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

#include <IntCurveSurface_IntersectionPoint.ixx>


//================================================================================
IntCurveSurface_IntersectionPoint::IntCurveSurface_IntersectionPoint() { } 
//================================================================================
IntCurveSurface_IntersectionPoint::IntCurveSurface_IntersectionPoint(const gp_Pnt& P,
								     const Standard_Real USurf,
								     const Standard_Real VSurf,
								     const Standard_Real UCurv,
								     const IntCurveSurface_TransitionOnCurve TrOnCurv):
       myP(P),myUSurf(USurf),myVSurf(VSurf),myUCurv(UCurv),myTrOnCurv(TrOnCurv) 
{ 
}
//================================================================================
void IntCurveSurface_IntersectionPoint::SetValues(const gp_Pnt& P,
						  const Standard_Real USurf,
						  const Standard_Real VSurf,
						  const Standard_Real UCurv,
						  const IntCurveSurface_TransitionOnCurve TrOnCurv) { 
  myP=P;
  myUSurf=USurf;
  myVSurf=VSurf;
  myUCurv=UCurv;
  myTrOnCurv=TrOnCurv;
}
//================================================================================
void IntCurveSurface_IntersectionPoint::Values(gp_Pnt& P,
					       Standard_Real& USurf,
					       Standard_Real& VSurf,
					       Standard_Real& UCurv,
					       IntCurveSurface_TransitionOnCurve& TrOnCurv) const 
{ 
  P=myP;
  USurf=myUSurf;
  VSurf=myVSurf;
  UCurv=myUCurv;
  TrOnCurv=myTrOnCurv;
}
//================================================================================
void IntCurveSurface_IntersectionPoint::Dump() const { 
  cout<<"IntersectionPoint: P("<<myP.X()<<","<<myP.Y()<<","<<myP.Z()<<")"<<endl;
  cout<<"                 : USurf("<<myUSurf<<")  VSurf("<<myVSurf<<")   UCurve("<<myUCurv<<")"<<endl;
  cout<<"                 : TransitionOnSurf ";

  switch(myTrOnCurv) { 
  case IntCurveSurface_Tangent: cout<<" Tangent "; break;
  case IntCurveSurface_In: cout<<" In "; break;
  case IntCurveSurface_Out: cout<<" Out "; break;

  default: cout<< " XXXXX ";
  }  
  cout<<endl;
}



