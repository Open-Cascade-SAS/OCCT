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



#include <BRepLib_MakePolygon.ixx>
#include <BRepLib.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS.hxx>
#include <Precision.hxx>

//=======================================================================
//function : BRepLib_MakePolygon
//purpose  : 
//=======================================================================

BRepLib_MakePolygon::BRepLib_MakePolygon() 
{
}


//=======================================================================
//function : BRepLib_MakePolygon
//purpose  : 
//=======================================================================

BRepLib_MakePolygon::BRepLib_MakePolygon(const gp_Pnt& P1, const gp_Pnt& P2) 
{
  Add(P1);
  Add(P2);
}


//=======================================================================
//function : BRepLib_MakePolygon
//purpose  : 
//=======================================================================

BRepLib_MakePolygon::BRepLib_MakePolygon(const gp_Pnt& P1,
					 const gp_Pnt& P2,
					 const gp_Pnt& P3, 
					 const Standard_Boolean Cl)
{
  Add(P1);
  Add(P2);
  Add(P3);
  if (Cl) Close();
}


//=======================================================================
//function : BRepLib_MakePolygon
//purpose  : 
//=======================================================================

BRepLib_MakePolygon::BRepLib_MakePolygon(const gp_Pnt& P1, 
					 const gp_Pnt& P2, 
					 const gp_Pnt& P3,
					 const gp_Pnt& P4,
					 const Standard_Boolean Cl)
{
  Add(P1);
  Add(P2);
  Add(P3);
  Add(P4);
  if (Cl) Close();
}


//=======================================================================
//function : BRepLib_MakePolygon
//purpose  : 
//=======================================================================

BRepLib_MakePolygon::BRepLib_MakePolygon(const TopoDS_Vertex& V1,
					 const TopoDS_Vertex& V2)
{
  Add(V1);
  Add(V2);
}


//=======================================================================
//function : BRepLib_MakePolygon
//purpose  : 
//=======================================================================

BRepLib_MakePolygon::BRepLib_MakePolygon(const TopoDS_Vertex& V1, 
					 const TopoDS_Vertex& V2, 
					 const TopoDS_Vertex& V3, 
					 const Standard_Boolean Cl)
{
  Add(V1);
  Add(V2);
  Add(V3);
  if (Cl) Close();
}


//=======================================================================
//function : BRepLib_MakePolygon
//purpose  : 
//=======================================================================

BRepLib_MakePolygon::BRepLib_MakePolygon(const TopoDS_Vertex& V1,
					 const TopoDS_Vertex& V2,
					 const TopoDS_Vertex& V3,
					 const TopoDS_Vertex& V4, 
					 const Standard_Boolean Cl)
{
  Add(V1);
  Add(V2);
  Add(V3);
  Add(V4);
  if (Cl) Close();
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  BRepLib_MakePolygon::Add(const gp_Pnt& P)
{
  BRep_Builder B;
  TopoDS_Vertex V;
  B.MakeVertex(V,P,Precision::Confusion());
  Add(V);
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  BRepLib_MakePolygon::Add(const TopoDS_Vertex& V)
{
  if (myFirstVertex.IsNull()) {
    myFirstVertex = V;
  }
  else {
    myEdge.Nullify();
    BRep_Builder B;
    TopoDS_Vertex last;

    Standard_Boolean second = myLastVertex.IsNull();
    if (second) {
      last = myFirstVertex;
      myLastVertex = V;
      B.MakeWire(TopoDS::Wire(myShape));
      myShape.Closed(Standard_False);
      myShape.Orientable(Standard_True);
    }
    else {
      last = myLastVertex;
      if (BRepTools::Compare(V,myFirstVertex)) {
	myLastVertex = myFirstVertex;
	myShape.Closed(Standard_True);
      }
      else 
	myLastVertex = V;
    }
    
    BRepLib_MakeEdge ME(last,myLastVertex);
    if (ME.IsDone()) {
      myEdge = ME;
      B.Add(myShape,myEdge);
      Done();
    }
    else {
      // restore the previous last vertex
      if (second)
	myLastVertex.Nullify();
      else
	myLastVertex = last;
    }
  }
} 
      
//=======================================================================
//function : Added
//purpose  : 
//=======================================================================

Standard_Boolean  BRepLib_MakePolygon::Added()const 
{
  return !myEdge.IsNull();
}


//=======================================================================
//function : Close
//purpose  : 
//=======================================================================

void  BRepLib_MakePolygon::Close()
{
  if (myFirstVertex.IsNull() || myLastVertex.IsNull())
    return;
  
  // check not already closed
  if (myShape.Closed()) 
    return;

  // build the last edge
  BRep_Builder B;
  myEdge.Nullify();
  BRepLib_MakeEdge ME(myLastVertex,myFirstVertex);
  if (ME.IsDone()) {
    myEdge = ME;
    B.Add(myShape,myEdge);
    myShape.Closed(Standard_True);
  }
}


//=======================================================================
//function : FirstVertex
//purpose  : 
//=======================================================================

const TopoDS_Vertex&  BRepLib_MakePolygon::FirstVertex()const 
{
  return myFirstVertex;
}


//=======================================================================
//function : LastVertex
//purpose  : 
//=======================================================================

const TopoDS_Vertex&  BRepLib_MakePolygon::LastVertex()const 
{
  return myLastVertex;
}

//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

const TopoDS_Edge&  BRepLib_MakePolygon::Edge()const 
{
  return myEdge;
}

//=======================================================================
//function : Wire
//purpose  : 
//=======================================================================

const TopoDS_Wire&  BRepLib_MakePolygon::Wire()const 
{
  return TopoDS::Wire(Shape());
}

//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepLib_MakePolygon::operator TopoDS_Edge() const
{
  return Edge();
}

//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepLib_MakePolygon::operator TopoDS_Wire() const
{
  return Wire();
}



