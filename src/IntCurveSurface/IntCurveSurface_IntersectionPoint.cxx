//  ---- File:	IntCurveSurface_IntersectionPoint.cxx
//  -- Created:	Wed Apr  7 16:35:02 1993
//  -- Author:	Laurent BUCHARD
//  --		<lbr@sdsun2>
//  ---Copyright:	 Matra Datavision 1993



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



