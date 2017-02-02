// Created on: 1994-11-08
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
#include <BRepLProp_CLProps.hxx>
#include <Precision.hxx>
#include <Standard_ProgramError.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_CurvePointInterference.hxx>
#include <TopOpeBRepDS_EdgeInterferenceTool.hxx>
#include <TopOpeBRepDS_EdgeVertexInterference.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>

//=======================================================================
//function : TopOpeBRepDS_EdgeInterferenceTool
//purpose  : 
//=======================================================================

 TopOpeBRepDS_EdgeInterferenceTool::TopOpeBRepDS_EdgeInterferenceTool()
{
}

static Standard_Real Parameter(const Handle(TopOpeBRepDS_Interference)& I)
{ 
  Standard_Real p = 0; 
  if      ( I->IsKind(STANDARD_TYPE(TopOpeBRepDS_EdgeVertexInterference)) )
    p = Handle(TopOpeBRepDS_EdgeVertexInterference)::DownCast(I)->Parameter();
  else if ( I->IsKind(STANDARD_TYPE(TopOpeBRepDS_CurvePointInterference)) )
    p = Handle(TopOpeBRepDS_CurvePointInterference)::DownCast(I)->Parameter();
  else {
    throw Standard_ProgramError("TopOpeBRepDS_EdgeInterferenceTool1");
  }
  return p;
}
  

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TopOpeBRepDS_EdgeInterferenceTool::Init
(const TopoDS_Shape& E, const Handle(TopOpeBRepDS_Interference)& I)
{
  myEdgeOrientation = E.Orientation();
  myEdgeOriented    = I->Support();
  
  // NYI 971219 : on ne tient pas compte de l'orientation de E = arete-mere de l'interference I
  if (myEdgeOrientation == TopAbs_INTERNAL || 
      myEdgeOrientation == TopAbs_EXTERNAL) {
    return;
  }
  
  // la premiere arete orientee est gardee dans myEdgeOriented pour MAJ de
  // l'arete croisee dans l'interference resultat.

  // par = parametre sur l'arete croisee
  Standard_Real par = ::Parameter(I);
  gp_Dir T,N; Standard_Real C;
  TopOpeBRepTool_ShapeTool::EdgeData(E,par,T,N,C);
  myTool.Reset(T,N,C);
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void TopOpeBRepDS_EdgeInterferenceTool::Add
(const TopoDS_Shape& E,
 const TopoDS_Shape& V,
 const Handle(TopOpeBRepDS_Interference)& I)
{
  TopAbs_Orientation Eori = E.Orientation();
  if (Eori == TopAbs_INTERNAL || 
      Eori == TopAbs_EXTERNAL) {
    return;
  }

  // premiere interference sur arete orientee : Init
  if (myEdgeOrientation == TopAbs_INTERNAL || 
      myEdgeOrientation == TopAbs_EXTERNAL) {
    Init(E,I);
    return;
  }

  // V est un sommet de E ?
  Standard_Boolean VofE = Standard_False;
  TopoDS_Iterator it(E,Standard_False);
  for ( ; it.More(); it.Next() ) {
    const TopoDS_Shape& S = it.Value();
    if ( S.IsSame(V) ) {
      VofE = Standard_True; 
      break;
    }
  }
 
  if(!VofE)
  {
    return;
  }
  // V est un sommet de E
  const TopoDS_Vertex& VV = TopoDS::Vertex(V);
  const TopoDS_Edge& EE = TopoDS::Edge(E);
  Standard_Real par = BRep_Tool::Parameter(VV,EE);
  
  gp_Dir T,N; Standard_Real C;
  Standard_Real tol = TopOpeBRepTool_ShapeTool::EdgeData(E,par,T,N,C);
  TopAbs_Orientation oriloc = I->Transition().Orientation(TopAbs_IN);
  TopAbs_Orientation oritan = it.Value().Orientation();
  myTool.Compare(tol,T,N,C,oriloc,oritan);
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void TopOpeBRepDS_EdgeInterferenceTool::Add
(const TopoDS_Shape& E,
// const TopOpeBRepDS_Point& P,
 const TopOpeBRepDS_Point& ,
 const Handle(TopOpeBRepDS_Interference)& I)
{
  TopAbs_Orientation Eori = E.Orientation();
  if (Eori == TopAbs_INTERNAL || 
      Eori == TopAbs_EXTERNAL) {
    return;
  }

  // premiere interference sur arete orientee : Init
  if (myEdgeOrientation == TopAbs_INTERNAL || 
      myEdgeOrientation == TopAbs_EXTERNAL) {
    Init(E,I);
    return;
  }

  Standard_Real par = ::Parameter(I);
  
  gp_Dir T,N; Standard_Real C;
  Standard_Real tol = TopOpeBRepTool_ShapeTool::EdgeData(E,par,T,N,C);
  TopAbs_Orientation oriloc = I->Transition().Orientation(TopAbs_IN);
  TopAbs_Orientation oritan = TopAbs_INTERNAL;
  myTool.Compare(tol,T,N,C,oriloc,oritan);
}

//=======================================================================
//function : Transition
//purpose  : 
//=======================================================================

void TopOpeBRepDS_EdgeInterferenceTool::Transition
(const Handle(TopOpeBRepDS_Interference)& I) const 
{
  TopOpeBRepDS_Transition& T = I->ChangeTransition();

  if (myEdgeOrientation == TopAbs_INTERNAL) {
    T.Set(TopAbs_IN,TopAbs_IN);
  }
  else if (myEdgeOrientation == TopAbs_EXTERNAL) {
    T.Set(TopAbs_OUT,TopAbs_OUT);
  }
  else {
    I->Support(myEdgeOriented);
    T.Set(myTool.StateBefore(),myTool.StateAfter());
  }
}
