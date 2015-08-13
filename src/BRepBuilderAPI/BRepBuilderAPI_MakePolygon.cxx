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


#include <BRepBuilderAPI_MakePolygon.hxx>
#include <gp_Pnt.hxx>
#include <StdFail_NotDone.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

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

const TopoDS_Wire&  BRepBuilderAPI_MakePolygon::Wire()
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

BRepBuilderAPI_MakePolygon::operator TopoDS_Wire()
{
  return Wire();
}



