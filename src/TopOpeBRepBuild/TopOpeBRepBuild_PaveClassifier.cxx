// Created on: 1993-06-17
// Created by: Jean Yves LEBEY
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


#include <TopOpeBRepBuild_PaveClassifier.ixx>
#include <TopOpeBRepBuild_Pave.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRep_Tool.hxx>
#include <ElCLib.hxx>
#include <Geom_Curve.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>

#ifdef DEB
Standard_IMPORT Standard_Boolean TopOpeBRepTool_GettraceVC();
Standard_IMPORT Standard_Boolean TopOpeBRepTool_GettraceCLOV();
#endif

//=======================================================================
//function : TopOpeBRepBuild_PaveClassifier
//purpose  : 
//=======================================================================

TopOpeBRepBuild_PaveClassifier::TopOpeBRepBuild_PaveClassifier
  (const TopoDS_Shape& E) :
  myEdgePeriodic(Standard_False),
  mySameParameters(Standard_False),
  myClosedVertices(Standard_False)
{
  myEdge = TopoDS::Edge(E);

  if ( ! BRep_Tool::Degenerated(myEdge) ) {
    TopLoc_Location loc;
    Standard_Real f,l;
    Handle(Geom_Curve) C = BRep_Tool::Curve(myEdge,loc,f,l);
    if ( !C.IsNull() ) {
      if (C->IsPeriodic()) {
	TopoDS_Vertex v1,v2; 
	TopExp::Vertices(myEdge,v1,v2);  // v1 FORWARD, v2 REVERSED
	if ( !v1.IsNull() && !v2.IsNull() ) { 
	  // --- the edge has vertices
	  myFirst  = f;
	  Standard_Real fC = C->FirstParameter();
	  Standard_Real lC = C->LastParameter();
	  myPeriod = lC - fC;
	  myEdgePeriodic = mySameParameters = v1.IsSame(v2);
	  if ( mySameParameters ) {
	    myFirst = BRep_Tool::Parameter(v1,myEdge);
	  }
	}
	else { 
	  // --- the edge has no vertices
	  myFirst  = f;
	  myPeriod = l - f;
	  myEdgePeriodic = Standard_True;
	  mySameParameters = Standard_False;
	}
      }
    }
    
#ifdef DEB
    if (TopOpeBRepTool_GettraceVC()) {
      cout<<endl;
      if (myEdgePeriodic) {
	cout<<"VC : periodic edge : myFirst "<<myFirst<<" myPeriod "<<myPeriod<<endl;
	if (mySameParameters)cout<<"VC same parameters "<<endl;
	else                 cout<<"VC no same parameters"<<endl;
      }
      else {
	cout<<"VC : non periodic edge : f "<<f<<" l "<<l<<endl;
      }
    }
#endif

  } // ! degenerated

}

//=======================================================================
//function : CompareOnNonPeriodic
//purpose  : 
//=======================================================================

TopAbs_State  TopOpeBRepBuild_PaveClassifier::CompareOnNonPeriodic()
{

  TopAbs_State state = TopAbs_UNKNOWN;
  Standard_Boolean lower=Standard_False;
  switch (myO2) {
    case TopAbs_FORWARD  : lower = Standard_False; break;
    case TopAbs_REVERSED : lower = Standard_True;  break;
    case TopAbs_INTERNAL : state = TopAbs_IN; break;
    case TopAbs_EXTERNAL : state = TopAbs_OUT; break;
  }

  if (state == TopAbs_UNKNOWN) {
    if (myP1 == myP2) {
      if ( myO1 == myO2 ) state = TopAbs_IN;
      else                state = TopAbs_OUT;
    }
    else if (myP1 < myP2) {
      if (lower) state = TopAbs_IN;
      else       state = TopAbs_OUT;
    }
    else {
      if (lower) state = TopAbs_OUT;
      else       state = TopAbs_IN;
    }
  }

#ifdef DEB
  if (TopOpeBRepTool_GettraceVC()) {
    cout<<"VC_NP : ";
    if      (myP1 == myP2) cout<<" p1 = p2";
    else if (myP1  < myP2) cout<<" p1 < p2";
    else if (myP1  > myP2) cout<<" p1 > p2";
    cout<<" --> state "; TopAbs::Print(state,cout); cout<<endl;
  }
#endif

  return state;
}

//=======================================================================
//function : AdjustCase
//purpose  : 
//=======================================================================

Standard_Real TopOpeBRepBuild_PaveClassifier::AdjustCase(const Standard_Real p1,
							 const TopAbs_Orientation o,
							 const Standard_Real first,
							 const Standard_Real period,
							 const Standard_Real tol,
							 Standard_Integer& cas)
{
  Standard_Real p2;
  if ( Abs(p1-first) < tol ) { // p1 is first
    if (o == TopAbs_REVERSED) {
      p2 = p1 + period;
      cas = 1;
    }
    else {
      p2 = p1;
      cas = 2;
    }
  }
  else {  // p1 is not on first
    Standard_Real last = first+period;
    if ( Abs(p1-last) < tol ) { // p1 is on last
      p2 = p1;
      cas = 3;
    }
    else { // p1 is not on last
      p2 = ElCLib::InPeriod(p1,first,last);
      cas = 4;
    }
  }
  return p2;
}

//=======================================================================
//function : AdjustOnPeriodic
//purpose  : (private)
//=======================================================================

void TopOpeBRepBuild_PaveClassifier::AdjustOnPeriodic()
{
  if ( ! ToAdjustOnPeriodic() ) return;

#ifdef DEB
  Standard_Real p1 = myP1, p2 = myP2;
#endif

  Standard_Real tol = Precision::PConfusion();

  if (mySameParameters) {
    myP1 = AdjustCase(myP1,myO1,myFirst,myPeriod,tol,myCas1);
    myP2 = AdjustCase(myP2,myO2,myFirst,myPeriod,tol,myCas2);
  }
  else if (myO1 != myO2 ) {
    if (myO1 == TopAbs_FORWARD) myP2 = AdjustCase(myP2,myO2,myP1,myPeriod,tol,myCas2);
    if (myO2 == TopAbs_FORWARD) myP1 = AdjustCase(myP1,myO1,myP2,myPeriod,tol,myCas1);
  }

#ifdef DEB
  if (TopOpeBRepTool_GettraceVC()) {
    cout<<"p1 "<<p1<<" ";TopAbs::Print(myO1,cout);cout<<" --> "<<myP1<<endl;
    cout<<"p2 "<<p2<<" ";TopAbs::Print(myO2,cout);cout<<" --> "<<myP2<<endl;
  }
#endif

}

//=======================================================================
//function : ToAdjustOnPeriodic
//purpose  : (private)
//=======================================================================

Standard_Boolean TopOpeBRepBuild_PaveClassifier::ToAdjustOnPeriodic() const
{
  Standard_Boolean toadjust = ( (mySameParameters) || (myO1 != myO2 ) );
  return toadjust;
}

//=======================================================================
//function : CompareOnPeriodic
//purpose  : 
//=======================================================================

TopAbs_State  TopOpeBRepBuild_PaveClassifier::CompareOnPeriodic()
{
  TopAbs_State state;
  
  if ( ToAdjustOnPeriodic() ) {
    state = CompareOnNonPeriodic();
  }
  else if (myO1 == TopAbs_FORWARD) {
    state = TopAbs_OUT;
    myCas1 = myCas2 = 5;
  }
  else if (myO1 == TopAbs_REVERSED) {
    state = TopAbs_OUT;
    myCas1 = myCas2 = 6;
  }
  else {
    state = TopAbs_OUT;
    myCas1 = myCas2 = 7;
  }

#ifdef DEB
  if (TopOpeBRepTool_GettraceVC()) {
    cout<<"VC_P : cas "<<myCas1<<"__"<<myCas2;
    cout<<" --> state "; TopAbs::Print(state,cout); cout<<endl;
  }
#endif

  return state;
}


//=======================================================================
//function : Compare
//purpose  : 
//=======================================================================

TopAbs_State  TopOpeBRepBuild_PaveClassifier::Compare
  (const Handle(TopOpeBRepBuild_Loop)& L1,
   const Handle(TopOpeBRepBuild_Loop)& L2)
{
  const Handle(TopOpeBRepBuild_Pave)& PV1 = 
    *((Handle(TopOpeBRepBuild_Pave)*)&(L1));
  const Handle(TopOpeBRepBuild_Pave)& PV2 = 
    *((Handle(TopOpeBRepBuild_Pave)*)&(L2));

  myCas1 = myCas2 = 0;   // debug
  myO1 = PV1->Vertex().Orientation();
  myO2 = PV2->Vertex().Orientation();
  myP1 = PV1->Parameter();
  myP2 = PV2->Parameter();
  
#ifdef DEB
  if (TopOpeBRepTool_GettraceVC()) {  
    cout<<endl<<"VC : "<<myP1<<" "<<myP2<<" ";
    TopAbs::Print(myO1,cout); cout<<" "; TopAbs::Print(myO2,cout);
    cout<<" (p "<<myEdgePeriodic;
    cout<<" s "<<mySameParameters<<" c "<<myClosedVertices<<")"<<endl;
  }
#endif
  
  if ( myEdgePeriodic && ToAdjustOnPeriodic() ) {
    AdjustOnPeriodic();
  }
  
  TopAbs_State state;
  if (myEdgePeriodic)
    state = CompareOnPeriodic();
  else
    state = CompareOnNonPeriodic();
  
#ifdef DEB
  if (TopOpeBRepTool_GettraceVC()) { 
    cout<<"VC : --> final state "; TopAbs::Print(state,cout); cout<<endl;
  }
#endif
  
  return state;
}


//=======================================================================
//function : SetFirstParameter
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_PaveClassifier::SetFirstParameter
  (const Standard_Real P)
{
  myFirst = P;
  mySameParameters = Standard_True;

#ifdef DEB
  if (TopOpeBRepTool_GettraceVC())
    cout<<endl<<"VC : set first parameter "<<myFirst<<endl;
#endif
}


//=======================================================================
//function : ClosedVertices
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_PaveClassifier::ClosedVertices
  (const Standard_Boolean Closed)
{
  myClosedVertices = Closed;
#ifdef DEB
  if (TopOpeBRepTool_GettraceCLOV()) {
    myEdgePeriodic = Closed;
    cout<<"::::::::::::::::::::::::"<<endl;
    cout<<"VC : myClosedVertices"<<myClosedVertices<<endl;
    cout<<"VC : myEdgePeriodic  "<<myEdgePeriodic<<endl;
    cout<<"::::::::::::::::::::::::"<<endl;
  }
#endif
}
