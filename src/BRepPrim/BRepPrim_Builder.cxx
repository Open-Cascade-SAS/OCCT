// Created on: 1991-07-25
// Created by: Christophe MARION
// Copyright (c) 1991-1999 Matra Datavision
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




#include <BRepPrim_Builder.ixx>

#include <Precision.hxx>
#include <TopoDS.hxx>

#include <Geom_Plane.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>

#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>

#include <BRepTools.hxx>

//=======================================================================
//function : BRepPrim_Builder
//purpose  : constructor
//=======================================================================

BRepPrim_Builder::BRepPrim_Builder () 
{
}

//=======================================================================
//function : BRepPrim_Builder
//purpose  : constructor
//=======================================================================

BRepPrim_Builder::BRepPrim_Builder (const BRep_Builder& B) :
       myBuilder(B)
{
}

//=======================================================================
//function : MakeShell
//purpose  : Make a Shell
//=======================================================================

void BRepPrim_Builder::MakeShell (TopoDS_Shell& S) const
{
  myBuilder.MakeShell(S);
  S.Closed(Standard_True);
}

//=======================================================================
//function : MakeFace
//purpose  : Make a Planar Face
//=======================================================================

void BRepPrim_Builder::MakeFace (TopoDS_Face& F, const gp_Pln& P) const
{
  myBuilder.MakeFace(F,new Geom_Plane(P),Precision::Confusion());
}

//=======================================================================
//function : MakeWire
//purpose  : Make an empty Wire
//=======================================================================

void BRepPrim_Builder::MakeWire (TopoDS_Wire& W) const
{
  myBuilder.MakeWire(W);
}

//=======================================================================
//function : MakeDegeneratedEdge
//purpose  : 
//=======================================================================

void BRepPrim_Builder::MakeDegeneratedEdge (TopoDS_Edge& E) const
{
  myBuilder.MakeEdge(E);
  myBuilder.Degenerated(E,Standard_True);
}

//=======================================================================
//function : MakeEdge
//purpose  : Make a linear Edge
//=======================================================================

void BRepPrim_Builder::MakeEdge (TopoDS_Edge& E, const gp_Lin& L) const
{
  myBuilder.MakeEdge(E,new Geom_Line(L),Precision::Confusion());
}

//=======================================================================
//function : MakeEdge
//purpose  : Make a Circular Edge
//=======================================================================

void BRepPrim_Builder::MakeEdge (TopoDS_Edge& E, const gp_Circ& C) const
{
  myBuilder.MakeEdge(E,new Geom_Circle(C),Precision::Confusion());
}

//=======================================================================
//function : SetPCurve
//purpose  : 
//=======================================================================

void BRepPrim_Builder::SetPCurve(TopoDS_Edge& E,
				 const TopoDS_Face& F,
				 const gp_Lin2d& L) const
{
  myBuilder.UpdateEdge(E,new Geom2d_Line(L),F,Precision::Confusion());
}

//=======================================================================
//function : SetPCurve
//purpose  : 
//=======================================================================

void BRepPrim_Builder::SetPCurve(TopoDS_Edge& E,
				 const TopoDS_Face& F,
				 const gp_Lin2d& L1,
				 const gp_Lin2d& L2) const
{
  TopoDS_Shape aLocalShape = E.Oriented(TopAbs_FORWARD);
  myBuilder.UpdateEdge(TopoDS::Edge(aLocalShape),
		       new Geom2d_Line(L1),
		       new Geom2d_Line(L2),
		       F,Precision::Confusion());
//  myBuilder.UpdateEdge(TopoDS::Edge(E.Oriented(TopAbs_FORWARD)),
//		       new Geom2d_Line(L1),
//		       new Geom2d_Line(L2),
//		       F,Precision::Confusion());
  myBuilder.Continuity(E,F,F,GeomAbs_CN);
}

//=======================================================================
//function : SetPCurve
//purpose  : 
//=======================================================================

void BRepPrim_Builder::SetPCurve(TopoDS_Edge& E,
				 const TopoDS_Face& F,
				 const gp_Circ2d& C) const
{
  myBuilder.UpdateEdge(E,new Geom2d_Circle(C),F,Precision::Confusion());
}


//=======================================================================
//function : MakeVertex
//purpose  : Make a Vertex
//=======================================================================

void BRepPrim_Builder::MakeVertex (TopoDS_Vertex& V, const gp_Pnt& P) const
{
  myBuilder.MakeVertex(V,P,Precision::Confusion());
}

//=======================================================================
//function : ReverseFace
//purpose  : Reverse a Face
//=======================================================================

void BRepPrim_Builder::ReverseFace (TopoDS_Face& F) const
{
  F.Reverse();
}

//=======================================================================
//function : AddEdgeVertex
//purpose  : Add a Vertex to an Edge
//=======================================================================

void BRepPrim_Builder::AddEdgeVertex (TopoDS_Edge& E, 
				      const TopoDS_Vertex& V,
				      const Standard_Real P,
				      const Standard_Boolean direct) const
{
  TopoDS_Vertex VV = V;
  if (!direct)
    VV.Reverse();
  myBuilder.Add(E,VV);
  myBuilder.UpdateVertex(VV,P,E,Precision::Confusion());
}


//=======================================================================
//function : AddEdgeVertex
//purpose  : Add a Vertex to an Edge
//=======================================================================

void BRepPrim_Builder::AddEdgeVertex (TopoDS_Edge& E, 
				      const TopoDS_Vertex& V,
				      const Standard_Real P1,
				      const Standard_Real P2) const
{
  TopoDS_Vertex VV = V;
  VV.Orientation(TopAbs_FORWARD);
  myBuilder.Add(E,VV);
  VV.Orientation(TopAbs_REVERSED);
  myBuilder.Add(E,VV);
  myBuilder.Range(E,P1,P2);
  E.Closed(Standard_True);
}

//=======================================================================
//function : SetParameters
//purpose  : 
//=======================================================================

void BRepPrim_Builder::SetParameters (TopoDS_Edge& E, 
				      const TopoDS_Vertex& ,
				      const Standard_Real P1,
				      const Standard_Real P2) const
{
  myBuilder.Range(E,P1,P2);
}

//=======================================================================
//function : AddWireEdge
//purpose  : Add an Edge to a Wire
//=======================================================================

void BRepPrim_Builder::AddWireEdge (TopoDS_Wire& W, 
				    const TopoDS_Edge& E,
				    const Standard_Boolean direct) const
{
  TopoDS_Edge EE = E;
  if (!direct)
    EE.Reverse();
  myBuilder.Add(W,EE);
}

//=======================================================================
//function : AddFaceWire
//purpose  : Add a Wire to a Face
//=======================================================================

void BRepPrim_Builder::AddFaceWire (TopoDS_Face& F, 
				    const TopoDS_Wire& W) const
{
  myBuilder.Add(F,W);
}

//=======================================================================
//function : AddShellFace
//purpose  : Add a Face to a Shell
//=======================================================================

void BRepPrim_Builder::AddShellFace(TopoDS_Shell& S,
				    const TopoDS_Face& F) const
{
  myBuilder.Add(S,F);
}

//=======================================================================
//function : Complete
//purpose  : 
//=======================================================================

void  BRepPrim_Builder::CompleteEdge(TopoDS_Edge& E)const 
{
  BRepTools::Update(E);
}

//=======================================================================
//function : Complete
//purpose  : 
//=======================================================================

void  BRepPrim_Builder::CompleteWire(TopoDS_Wire& W)const 
{
  BRepTools::Update(W);
}


//=======================================================================
//function : Complete
//purpose  : 
//=======================================================================

void  BRepPrim_Builder::CompleteFace(TopoDS_Face& F)const 
{
  BRepTools::Update(F);
}


//=======================================================================
//function : Complete
//purpose  : 
//=======================================================================

void  BRepPrim_Builder::CompleteShell(TopoDS_Shell& S)const 
{
  BRepTools::Update(S);
}
