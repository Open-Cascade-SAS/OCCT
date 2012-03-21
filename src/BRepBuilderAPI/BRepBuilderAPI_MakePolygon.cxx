// Created on: 1993-07-29
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



#include <BRepBuilderAPI_MakePolygon.ixx>

//=======================================================================
//function : BRepBuilderAPI_MakePolygon
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakePolygon::BRepBuilderAPI_MakePolygon()
{
}


//=======================================================================
//function : BRepBuilderAPI_MakePolygon
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakePolygon::BRepBuilderAPI_MakePolygon(const gp_Pnt& P1, const gp_Pnt& P2) 
: myMakePolygon(P1,P2)
{
  if (myMakePolygon.IsDone()) {
    Done();
    myShape = myMakePolygon.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakePolygon
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakePolygon::BRepBuilderAPI_MakePolygon(const gp_Pnt& P1,
					 const gp_Pnt& P2,
					 const gp_Pnt& P3, 
					 const Standard_Boolean Cl)
: myMakePolygon(P1,P2,P3,Cl)
{
  if (myMakePolygon.IsDone()) {
    Done();
    myShape = myMakePolygon.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakePolygon
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakePolygon::BRepBuilderAPI_MakePolygon(const gp_Pnt& P1, 
					 const gp_Pnt& P2, 
					 const gp_Pnt& P3,
					 const gp_Pnt& P4,
					 const Standard_Boolean Cl)
: myMakePolygon(P1,P2,P3,P4,Cl)
{
  if (myMakePolygon.IsDone()) {
    Done();
    myShape = myMakePolygon.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakePolygon
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakePolygon::BRepBuilderAPI_MakePolygon(const TopoDS_Vertex& V1,
					 const TopoDS_Vertex& V2)
: myMakePolygon(V1,V2)
{
  if (myMakePolygon.IsDone()) {
    Done();
    myShape = myMakePolygon.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakePolygon
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakePolygon::BRepBuilderAPI_MakePolygon(const TopoDS_Vertex& V1, 
					 const TopoDS_Vertex& V2, 
					 const TopoDS_Vertex& V3, 
					 const Standard_Boolean Cl)
: myMakePolygon(V1,V2,V3,Cl)
{
  if (myMakePolygon.IsDone()) {
    Done();
    myShape = myMakePolygon.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakePolygon
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakePolygon::BRepBuilderAPI_MakePolygon(const TopoDS_Vertex& V1,
					 const TopoDS_Vertex& V2,
					 const TopoDS_Vertex& V3,
					 const TopoDS_Vertex& V4, 
					 const Standard_Boolean Cl)
: myMakePolygon(V1,V2,V3,V4,Cl)
{
  if (myMakePolygon.IsDone()) {
    Done();
    myShape = myMakePolygon.Shape();
  }
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakePolygon::Add(const gp_Pnt& P)
{
  myMakePolygon.Add(P);
  if (myMakePolygon.IsDone())  {
    Done();
    if ( !LastVertex().IsNull())
      myShape = myMakePolygon.Shape();
  }
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakePolygon::Add(const TopoDS_Vertex& V)
{
  myMakePolygon.Add(V);
  if (myMakePolygon.IsDone()) {
    Done();
    myShape = myMakePolygon.Shape();
  }
} 
      
//=======================================================================
//function : Added
//purpose  : 
//=======================================================================

Standard_Boolean  BRepBuilderAPI_MakePolygon::Added()const 
{
  return myMakePolygon.Added();
}


//=======================================================================
//function : Close
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakePolygon::Close()
{
  myMakePolygon.Close();
  myShape = myMakePolygon.Shape();
}


//=======================================================================
//function : FirstVertex
//purpose  : 
//=======================================================================

const TopoDS_Vertex&  BRepBuilderAPI_MakePolygon::FirstVertex()const 
{
  return myMakePolygon.FirstVertex();
}


//=======================================================================
//function : LastVertex
//purpose  : 
//=======================================================================

const TopoDS_Vertex&  BRepBuilderAPI_MakePolygon::LastVertex()const 
{
  return myMakePolygon.LastVertex();
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepBuilderAPI_MakePolygon::IsDone() const
{
  return myMakePolygon.IsDone();
}


//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

const TopoDS_Edge&  BRepBuilderAPI_MakePolygon::Edge()const 
{
  return myMakePolygon.Edge();
}

//=======================================================================
//function : Wire
//purpose  : 
//=======================================================================

const TopoDS_Wire&  BRepBuilderAPI_MakePolygon::Wire()const 
{
  return myMakePolygon.Wire();
}

//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakePolygon::operator TopoDS_Edge() const
{
  return Edge();
}

//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakePolygon::operator TopoDS_Wire() const
{
  return Wire();
}



