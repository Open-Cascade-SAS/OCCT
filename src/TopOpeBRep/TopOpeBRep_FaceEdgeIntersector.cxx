// Created on: 1994-10-07
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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


#include <BRep_Tool.hxx>
#include <BRepIntCurveSurface_Inter.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <IntCurveSurface_TransitionOnCurve.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRep_FaceEdgeIntersector.hxx>
#include <TopOpeBRepDS_Transition.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>

#ifdef OCCT_DEBUG
#include <TopAbs.hxx>
extern Standard_Boolean TopOpeBRep_GettraceFITOL();
extern Standard_Boolean TopOpeBRep_GettraceSAVFF();
#include <TCollection_AsciiString.hxx>
#include <Standard_CString.hxx>
#include <BRepTools.hxx>
static void SAVFE(const TopoDS_Face& F1,const TopoDS_Edge& E)
{
  TCollection_AsciiString aname_1("FE_face"), aname_2("FE_edge");
  Standard_CString name_1 = aname_1.ToCString(), name_2 = aname_2.ToCString();
  cout<<"FaceEdgeIntersector : "<<name_1<<","<<name_2<<endl;
  BRepTools::Write(F1,name_1); BRepTools::Write(E,name_2); 
}
extern Standard_Boolean TopOpeBRepTool_GettraceKRO();
#include <TopOpeBRepTool_KRO.hxx>
Standard_EXPORT TOPKRO KRO_DSFILLER_INTFE("intersection face/edge");
#endif


//=======================================================================
//function : TopOpeBRep_FaceEdgeIntersector
//purpose  : 
//=======================================================================

 TopOpeBRep_FaceEdgeIntersector::TopOpeBRep_FaceEdgeIntersector()
{
  ResetIntersection();
}

//=======================================================================
//function : ResetIntersection
//purpose  : 
//=======================================================================

void TopOpeBRep_FaceEdgeIntersector::ResetIntersection() 
{
  mySequenceOfPnt.Clear();
  mySequenceOfState.Clear();
  myNbPoints = 0;
  myIntersectionDone = Standard_False;
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void TopOpeBRep_FaceEdgeIntersector::Perform(const TopoDS_Shape& SF,
					     const TopoDS_Shape& SE)
{
  ResetIntersection();
  if (!myForceTolerance) ShapeTolerances(SF,SE);
  myTol = BRep_Tool::Tolerance(TopoDS::Edge(SE));
#ifdef OCCT_DEBUG
  if (TopOpeBRep_GettraceFITOL()) cout<<"Perform : myTol = "<<myTol<<endl;
#endif
  
  myFace = TopoDS::Face(SF); myFace.Orientation(TopAbs_FORWARD);
  myEdge = TopoDS::Edge(SE); myEdge.Orientation(TopAbs_FORWARD);

#ifdef OCCT_DEBUG
  if (TopOpeBRep_GettraceSAVFF()) SAVFE(myFace,myEdge);
#endif
  
  Standard_Real f,l;
  TopLoc_Location loc;
  const Handle(Geom_Curve) C = BRep_Tool::Curve(myEdge,loc,f,l);
  
  Handle(Geom_Geometry) GGao1 = C->Transformed(loc.Transformation());
  Handle(Geom_Curve)* PGCao1 = (Handle(Geom_Curve)*)&GGao1;
  myCurve.Load(*PGCao1,f,l);


#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceKRO()) KRO_DSFILLER_INTFE.Start();
#endif

  BRepIntCurveSurface_Inter FEINT;
  FEINT.Init(myFace,myCurve,myTol);

#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceKRO()) KRO_DSFILLER_INTFE.Stop();
#endif

  for (FEINT.Init(myFace,myCurve,myTol); FEINT.More(); FEINT.Next()) {
    mySequenceOfPnt.Append(FEINT.Point());
    Standard_Integer i = (FEINT.State() == TopAbs_IN) ? 0 : 1;
    mySequenceOfState.Append(i);
  }

  myNbPoints = mySequenceOfPnt.Length();
  myIntersectionDone = Standard_True;

}


//=======================================================================
//function : IsEmpty
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRep_FaceEdgeIntersector::IsEmpty () 
{
  Standard_Boolean b = myNbPoints == 0;
  return b;
}


//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopoDS_Shape& TopOpeBRep_FaceEdgeIntersector::Shape
(const Standard_Integer Index) const 
{
  if      ( Index == 1 ) return myFace;
  else if ( Index == 2 ) return myEdge;
  else throw Standard_ProgramError("TopOpeBRep_FaceEdgeIntersector::Shape");
}

//=======================================================================
//function : ForceTolerance
//purpose  : 
//=======================================================================

void TopOpeBRep_FaceEdgeIntersector::ForceTolerance(const Standard_Real Tol)
{
  myTol = Tol;
  myForceTolerance = Standard_True;
  
#ifdef OCCT_DEBUG
  if (TopOpeBRep_GettraceFITOL())
    cout<<"ForceTolerance : myTol = "<<myTol<<endl;
#endif
}

//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

Standard_Real  TopOpeBRep_FaceEdgeIntersector::Tolerance() const
{
  return myTol;
}

//=======================================================================
//function : NbPoints
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRep_FaceEdgeIntersector::NbPoints() const 
{
  Standard_Integer n = myNbPoints;
  return n;
}


//=======================================================================
//function : InitPoint
//purpose  : 
//=======================================================================

void TopOpeBRep_FaceEdgeIntersector::InitPoint()
{
  myPointIndex = 1;
}

//=======================================================================
//function : MorePoint
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRep_FaceEdgeIntersector::MorePoint() const 
{
  Standard_Boolean b = myPointIndex <= myNbPoints;
  return b;
}

//=======================================================================
//function : NextPoint
//purpose  : 
//=======================================================================

void TopOpeBRep_FaceEdgeIntersector::NextPoint()
{
  myPointIndex++;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt TopOpeBRep_FaceEdgeIntersector::Value() const 
{
  const IntCurveSurface_IntersectionPoint& IP = mySequenceOfPnt(myPointIndex);
  const gp_Pnt& P = IP.Pnt();
  return P;
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Standard_Real TopOpeBRep_FaceEdgeIntersector::Parameter() const
{
  const IntCurveSurface_IntersectionPoint& IP = mySequenceOfPnt(myPointIndex);
  Standard_Real p = IP.W();
  return p;
}

//=======================================================================
//function : UVPoint
//purpose  : 
//=======================================================================

void TopOpeBRep_FaceEdgeIntersector::UVPoint(gp_Pnt2d& P2d) const
{
  const IntCurveSurface_IntersectionPoint& IP = mySequenceOfPnt(myPointIndex);
  Standard_Real u = IP.U();
  Standard_Real v = IP.V();
  P2d.SetCoord(u,v);
}

//=======================================================================
//function : State
//purpose  : 
//=======================================================================

TopAbs_State TopOpeBRep_FaceEdgeIntersector::State() const
{
  Standard_Integer i = mySequenceOfState(myPointIndex);
  TopAbs_State s = (i == 0 ) ? TopAbs_IN : TopAbs_ON;
  return s;
}

//=======================================================================
//function : Transition
//purpose  : 
//=======================================================================

TopOpeBRepDS_Transition TopOpeBRep_FaceEdgeIntersector::Transition
(const Standard_Integer Index,
 const TopAbs_Orientation FaceOrientation) const 
{
//  TopAbs_ShapeEnum onB = TopAbs_FACE, onA = TopAbs_FACE; // bidon
//  if ((FaceOrientation == TopAbs_INTERNAL) || 
//      (FaceOrientation == TopAbs_EXTERNAL)) {
//    TopOpeBRepDS_Transition TR(TopAbs_IN,TopAbs_IN,onB,onA); // IN bidon
//    TR.Set(FaceOrientation);
//    return TR;
//  }

  TopAbs_State stB, stA;

  const IntCurveSurface_IntersectionPoint& IP = mySequenceOfPnt(myPointIndex);

  if ( Index == 2 ) {  //--   Edge In <=>   Rentre ds la matiere face
    switch (IP.Transition()) { 
      case IntCurveSurface_In  : stB = TopAbs_OUT; stA = TopAbs_IN; break;
      case IntCurveSurface_Out : stB = TopAbs_IN; stA = TopAbs_OUT; break;
      default :                  stB = TopAbs_IN; stA = TopAbs_IN; break;
    }

    TopOpeBRepDS_Transition TR;
    TopAbs_ShapeEnum onB = TopAbs_FACE, onA = TopAbs_FACE; 
    if      (FaceOrientation == TopAbs_FORWARD) 
      TR.Set(stB,stA,onB,onA);
    else if (FaceOrientation == TopAbs_REVERSED) 
      TR.Set(stA,stB,onA,onB);
    else if (FaceOrientation == TopAbs_EXTERNAL) 
      TR.Set(TopAbs_OUT,TopAbs_OUT,onA,onB);
    else if (FaceOrientation == TopAbs_INTERNAL) 
      TR.Set(TopAbs_IN,TopAbs_IN,onA,onB);
    return TR;
  }

  else if ( Index == 1 ) { //-- Face On est toujours ds la face . 
    switch (IP.Transition()) { 
      case IntCurveSurface_In  : stB = stA = TopAbs_IN; break;
      case IntCurveSurface_Out : stB = stA = TopAbs_IN; break;
      default :                  stB = stA = TopAbs_IN; break;
    }
    TopAbs_ShapeEnum onB = TopAbs_FACE, onA = TopAbs_FACE; 
    TopOpeBRepDS_Transition TR;
    TR.Set(stB,stA,onB,onA);
    return TR;
  }

  else throw Standard_ProgramError("FEINT Transition Index");
}

//=======================================================================
//function : IsVertex
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRep_FaceEdgeIntersector::IsVertex
(const TopoDS_Shape& S, const gp_Pnt& P,
 const Standard_Real Tol, TopoDS_Vertex& VR)
{
  Standard_Boolean isv = Standard_False;
  VR = myNullVertex;

  Standard_Real Tol2=Tol*Tol;
  for (myVertexExplorer.Init(S,TopAbs_VERTEX); 
       myVertexExplorer.More(); 
       myVertexExplorer.Next()) {
    const TopoDS_Shape& SS = myVertexExplorer.Current();
    const TopoDS_Vertex& VV = TopoDS::Vertex(SS);
    gp_Pnt PV = BRep_Tool::Pnt(VV);
    isv = P.SquareDistance(PV) < Tol2;
    if (isv) {
      VR = VV;
    }
  }

  return isv;
}

//=======================================================================
//function : IsVertex
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRep_FaceEdgeIntersector::IsVertex
(const Standard_Integer I, TopoDS_Vertex& VR)
{
  Standard_Boolean isv = Standard_False;
  gp_Pnt P = Value();
  if      (I == 1) isv = IsVertex(myFace,P,myTol,VR);
  else if (I == 2) isv = IsVertex(myEdge,P,myTol,VR);
  return isv;
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRep_FaceEdgeIntersector::Index() const 
{
#ifdef OCCT_DEBUG
  return myPointIndex;
#else
  return 0;
#endif
}


//=======================================================================
//function : ShapeTolerances
//purpose  : (private)
//=======================================================================

void TopOpeBRep_FaceEdgeIntersector::ShapeTolerances(const TopoDS_Shape& S1,
						     const TopoDS_Shape& S2)
{
  myTol = Max(ToleranceMax(S1,TopAbs_EDGE),ToleranceMax(S2,TopAbs_EDGE));
  myForceTolerance = Standard_False;
  
#ifdef OCCT_DEBUG
  if (TopOpeBRep_GettraceFITOL()) {
    cout<<"ShapeTolerances on S1 = ";TopAbs::Print(S1.ShapeType(),cout);
    cout<<" S2 = ";TopAbs::Print(S2.ShapeType(),cout);
    cout<<" : myTol = "<<myTol<<endl;
  }
#endif
}

//=======================================================================
//function : ToleranceMax
//purpose  : (private)
//=======================================================================

Standard_Real TopOpeBRep_FaceEdgeIntersector::ToleranceMax
(const TopoDS_Shape& S,
 const TopAbs_ShapeEnum T)const
{
  TopExp_Explorer e(S,T);
  if ( ! e.More() ) return Precision::Intersection();
  else {
    Standard_Real tol = RealFirst();
    for (; e.More(); e.Next())
      tol = Max(tol,TopOpeBRepTool_ShapeTool::Tolerance(e.Current()));
    return tol;
  }
}
