// Created on: 1993-06-23
// Created by: Remi LEQUETTE
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



#include <BRepPrim_FaceBuilder.ixx>
#include <Precision.hxx>
#include <gp_Pnt.hxx>
#include <Geom2d_Line.hxx>
#include <TopoDS_Wire.hxx>

#include <Standard_OutOfRange.hxx>

//=======================================================================
//function : BRepPrim_FaceBuilder
//purpose  : 
//=======================================================================

BRepPrim_FaceBuilder::BRepPrim_FaceBuilder()
{
}

//=======================================================================
//function : BRepPrim_FaceBuilder
//purpose  : 
//=======================================================================

BRepPrim_FaceBuilder::BRepPrim_FaceBuilder(const BRep_Builder& B, 
					   const Handle(Geom_Surface)& S) 
{
  Init(B,S);
}


//=======================================================================
//function : BRepPrim_FaceBuilder
//purpose  : 
//=======================================================================

BRepPrim_FaceBuilder::BRepPrim_FaceBuilder(const BRep_Builder& B,
					   const Handle(Geom_Surface)& S, 
					   const Standard_Real UMin, 
					   const Standard_Real UMax, 
					   const Standard_Real VMin,
					   const Standard_Real VMax) 
{
  Init(B,S,UMin,UMax,VMin,VMax);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepPrim_FaceBuilder::Init(const BRep_Builder& B,
				const Handle(Geom_Surface)& S)
{ 
  Standard_Real UMin,UMax,VMin,VMax;
  S->Bounds(UMin,UMax,VMin,VMax);
  Init(B,S,UMin,UMax,VMin,VMax);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepPrim_FaceBuilder::Init(const BRep_Builder& B,
				const Handle(Geom_Surface)& S, 
				const Standard_Real UMin, 
				const Standard_Real UMax, 
				const Standard_Real VMin,
				const Standard_Real VMax) 
{
  // Check the values
  Standard_Real USMin,USMax,VSMin,VSMax;
  S->Bounds(USMin,USMax,VSMin,VSMax);
  
  if (UMin >= UMax) Standard_ConstructionError::Raise("BRepPrim_FaceBuilder");
  if (VMin >= VMax) Standard_ConstructionError::Raise("BRepPrim_FaceBuilder");
  if (UMin < USMin) Standard_ConstructionError::Raise("BRepPrim_FaceBuilder");
  if (UMax > USMax) Standard_ConstructionError::Raise("BRepPrim_FaceBuilder");
  if (VMin < VSMin) Standard_ConstructionError::Raise("BRepPrim_FaceBuilder");
  if (VMax > VSMax) Standard_ConstructionError::Raise("BRepPrim_FaceBuilder");

  // Make the vertices
  B.MakeVertex(myVertex[0],S->Value(UMin,VMin),Precision::Confusion());
  B.MakeVertex(myVertex[1],S->Value(UMax,VMin),Precision::Confusion());
  B.MakeVertex(myVertex[2],S->Value(UMax,VMax),Precision::Confusion());
  B.MakeVertex(myVertex[3],S->Value(UMin,VMax),Precision::Confusion());
  
  // Make the edges
  B.MakeEdge(myEdges[0]);
  B.MakeEdge(myEdges[1]);
  B.MakeEdge(myEdges[2]);
  B.MakeEdge(myEdges[3]);

  // Make the face
  B.MakeFace(myFace,S,Precision::Confusion());

  // set the pcurves
  Handle(Geom2d_Line) L;
  L = new Geom2d_Line(gp_Pnt2d(UMin,VMin),gp_Dir2d(1,0));
  B.UpdateEdge(myEdges[0],L,myFace,Precision::Confusion());
  L = new Geom2d_Line(gp_Pnt2d(UMax,VMin),gp_Dir2d(0,1));
  B.UpdateEdge(myEdges[1],L,myFace,Precision::Confusion());
  L = new Geom2d_Line(gp_Pnt2d(UMax,VMax),gp_Dir2d(-1,0));
  B.UpdateEdge(myEdges[2],L,myFace,Precision::Confusion());
  L = new Geom2d_Line(gp_Pnt2d(UMin,VMax),gp_Dir2d(0,-1));
  B.UpdateEdge(myEdges[3],L,myFace,Precision::Confusion());

  // set the parameters
  B.UpdateVertex(myVertex[0],0,myEdges[0],0);
  B.UpdateVertex(myVertex[1],UMax-UMin,myEdges[0],0);
  B.UpdateVertex(myVertex[1],0,myEdges[1],0);
  B.UpdateVertex(myVertex[2],VMax-VMin,myEdges[1],0);
  B.UpdateVertex(myVertex[2],0,myEdges[2],0);
  B.UpdateVertex(myVertex[3],UMax-UMin,myEdges[2],0);
  B.UpdateVertex(myVertex[3],0,myEdges[3],0);
  B.UpdateVertex(myVertex[0],VMax-VMin,myEdges[3],0);

  // insert vertices in edges
  myVertex[0].Orientation(TopAbs_REVERSED);
  B.Add(myEdges[3],myVertex[0]);
  myVertex[0].Orientation(TopAbs_FORWARD);
  B.Add(myEdges[0],myVertex[0]);
  myVertex[1].Orientation(TopAbs_REVERSED);
  B.Add(myEdges[0],myVertex[1]);
  myVertex[1].Orientation(TopAbs_FORWARD);
  B.Add(myEdges[1],myVertex[1]);
  myVertex[2].Orientation(TopAbs_REVERSED);
  B.Add(myEdges[1],myVertex[2]);
  myVertex[2].Orientation(TopAbs_FORWARD);
  B.Add(myEdges[2],myVertex[2]);
  myVertex[3].Orientation(TopAbs_REVERSED);
  B.Add(myEdges[2],myVertex[3]);
  myVertex[3].Orientation(TopAbs_FORWARD);
  B.Add(myEdges[3],myVertex[3]);

  // insert edges in a wire and in the face
  TopoDS_Wire W;
  B.MakeWire(W);
  B.Add(W,myEdges[0]);
  B.Add(W,myEdges[1]);
  B.Add(W,myEdges[2]);
  B.Add(W,myEdges[3]);

  B.Add(myFace,W);

  // set the natural restriction flag
  if ( UMin == USMin && UMax == USMax && VMin == VSMin && VMax == VSMax)
    B.NaturalRestriction(myFace,Standard_True);
}


//=======================================================================
//function : Face
//purpose  : 
//=======================================================================

const TopoDS_Face&  BRepPrim_FaceBuilder::Face()const 
{
  return myFace;
}


//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

const TopoDS_Edge&  BRepPrim_FaceBuilder::Edge(const Standard_Integer I)const 
{
  Standard_OutOfRange_Raise_if(I<1 || I >4,"BRepPrim_FaceBuilder::Edge");
  return myEdges[I-1];
}


//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================

const TopoDS_Vertex&  BRepPrim_FaceBuilder::Vertex(const Standard_Integer I)const 
{
  Standard_OutOfRange_Raise_if(I<1 || I >4,"BRepPrim_FaceBuilder::Vertex");
  return myVertex[I-1];
}



//=======================================================================
//function : operator TopoDS_Face
//purpose  : 
//=======================================================================

BRepPrim_FaceBuilder::operator TopoDS_Face()
{
  return Face();
}
