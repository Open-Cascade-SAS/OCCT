// Created on: 1993-07-23
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


#include <BRepBuilderAPI_MakeWire.hxx>
#include <StdFail_NotDone.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

//=======================================================================
//function : BRepBuilderAPI_MakeWire
//purpose  : 
//=======================================================================
BRepBuilderAPI_MakeWire::BRepBuilderAPI_MakeWire()
{
}


//=======================================================================
//function : BRepBuilderAPI_MakeWire
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeWire::BRepBuilderAPI_MakeWire(const TopoDS_Edge& E)
: myMakeWire(E)
{
  if ( myMakeWire.IsDone()) {
    Done();
    myShape = myMakeWire.Wire();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeWire
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeWire::BRepBuilderAPI_MakeWire(const TopoDS_Edge& E1, 
				   const TopoDS_Edge& E2)
: myMakeWire(E1,E2)
{
  if ( myMakeWire.IsDone()) {
    Done();
    myShape = myMakeWire.Wire();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeWire
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeWire::BRepBuilderAPI_MakeWire(const TopoDS_Edge& E1,
				   const TopoDS_Edge& E2, 
				   const TopoDS_Edge& E3)
: myMakeWire(E1,E2,E3)
{
  if ( myMakeWire.IsDone()) {
    Done();
    myShape = myMakeWire.Wire();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeWire
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeWire::BRepBuilderAPI_MakeWire(const TopoDS_Edge& E1, 
				   const TopoDS_Edge& E2,
				   const TopoDS_Edge& E3, 
				   const TopoDS_Edge& E4)
: myMakeWire(E1,E2,E3,E4)
{
  if ( myMakeWire.IsDone()) {
    Done();
    myShape = myMakeWire.Wire();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeWire
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeWire::BRepBuilderAPI_MakeWire(const TopoDS_Wire& W)
: myMakeWire(W)
{
  if ( myMakeWire.IsDone()) {
    Done();
    myShape = myMakeWire.Wire();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeWire
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeWire::BRepBuilderAPI_MakeWire(const TopoDS_Wire& W, 
				   const TopoDS_Edge& E)
: myMakeWire(W,E)
{
  if ( myMakeWire.IsDone()) {
    Done();
    myShape = myMakeWire.Wire();
  }
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeWire::Add(const TopoDS_Wire& W)
{
  myMakeWire.Add(W);
  if ( myMakeWire.IsDone()) {
    Done();
    myShape = myMakeWire.Wire();
  }
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeWire::Add(const TopoDS_Edge& E)
{
  myMakeWire.Add(E);
  if ( myMakeWire.IsDone()) {
    Done();
    myShape = myMakeWire.Wire();
  }
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeWire::Add(const TopTools_ListOfShape& L)
{
  myMakeWire.Add(L);
  if ( myMakeWire.IsDone()) {
    Done();
    myShape = myMakeWire.Wire();
  }
}


//=======================================================================
//function : Wire
//purpose  : 
//=======================================================================

const TopoDS_Wire&  BRepBuilderAPI_MakeWire::Wire()
{
  return myMakeWire.Wire();
}


//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

const TopoDS_Edge&  BRepBuilderAPI_MakeWire::Edge()const 
{
  return myMakeWire.Edge();
}


//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================

const TopoDS_Vertex&  BRepBuilderAPI_MakeWire::Vertex()const 
{
  return myMakeWire.Vertex();
}


//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeWire::operator TopoDS_Wire()
{
  return Wire();
}


//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepBuilderAPI_MakeWire::IsDone() const
{
  return myMakeWire.IsDone();
}



//=======================================================================
//function : Error
//purpose  : 
//=======================================================================

BRepBuilderAPI_WireError BRepBuilderAPI_MakeWire::Error() const
{
  switch ( myMakeWire.Error()) {

  case BRepLib_WireDone: 
    return BRepBuilderAPI_WireDone;

  case BRepLib_EmptyWire:
    return BRepBuilderAPI_EmptyWire;

  case BRepLib_DisconnectedWire:
    return BRepBuilderAPI_DisconnectedWire;

  case BRepLib_NonManifoldWire:
    return BRepBuilderAPI_NonManifoldWire;
  }

  // portage WNT
  return BRepBuilderAPI_WireDone;
}
