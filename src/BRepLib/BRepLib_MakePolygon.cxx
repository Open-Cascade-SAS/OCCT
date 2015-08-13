// Created on: 1993-07-29
// Created by: Remi LEQUETTE
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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakePolygon.hxx>
#include <BRepTools.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <StdFail_NotDone.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

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

const TopoDS_Wire&  BRepLib_MakePolygon::Wire() 
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

BRepLib_MakePolygon::operator TopoDS_Wire()
{
  return Wire();
}



