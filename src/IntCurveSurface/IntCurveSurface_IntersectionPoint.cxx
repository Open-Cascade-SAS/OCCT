// Created on: 1993-04-07
// Created by: Laurent BUCHARD
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <gp_Pnt.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>

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



