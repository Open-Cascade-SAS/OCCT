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


#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <StdFail_NotDone.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================
BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge()
{}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const TopoDS_Vertex& V1, 
				   const TopoDS_Vertex& V2)
: myMakeEdge(V1,V2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const gp_Pnt& P1, 
				   const gp_Pnt& P2)
: myMakeEdge(P1,P2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const gp_Lin& L)
: myMakeEdge(L)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const gp_Lin& L, 
				   const Standard_Real p1, 
				   const Standard_Real p2)
: myMakeEdge(L,p1,p2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const gp_Lin& L, 
				   const gp_Pnt& P1, 
				   const gp_Pnt& P2)
: myMakeEdge(L,P1,P2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const gp_Lin& L, 
				   const TopoDS_Vertex& V1, 
				   const TopoDS_Vertex& V2)
: myMakeEdge(L,V1,V2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const gp_Circ& C) 
: myMakeEdge(C)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const gp_Circ& C,
				   const Standard_Real p1,
				   const Standard_Real p2)
: myMakeEdge(C,p1,p2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const gp_Circ& C ,
				   const gp_Pnt&  P1,
				   const gp_Pnt&  P2 )
: myMakeEdge(C,P1,P2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const gp_Circ& C,
				   const TopoDS_Vertex& V1,
				   const TopoDS_Vertex& V2)
: myMakeEdge(C,V1,V2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const gp_Elips& E) 
:myMakeEdge(E)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const gp_Elips& E,
				   const Standard_Real p1,
				   const Standard_Real p2)
: myMakeEdge(E,p1,p2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const gp_Elips& E,
				   const gp_Pnt& P1,
				   const gp_Pnt& P2)
: myMakeEdge(E,P1,P2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const gp_Elips& E,
				   const TopoDS_Vertex& V1,
				   const TopoDS_Vertex& V2)
: myMakeEdge(E,V1,V2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const gp_Hypr& H)
: myMakeEdge(H)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const gp_Hypr& H,
				   const Standard_Real p1,
				   const Standard_Real p2)
: myMakeEdge(H,p1,p2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const gp_Hypr& H,
				   const gp_Pnt& P1,
				   const gp_Pnt& P2)
: myMakeEdge(H,P1,P2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const gp_Hypr& H,
				   const TopoDS_Vertex& V1,
				   const TopoDS_Vertex& V2)
: myMakeEdge(H,V1,V2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const gp_Parab& P)
: myMakeEdge(P)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const gp_Parab& P,
				   const Standard_Real p1,
				   const Standard_Real p2)
: myMakeEdge(P,p1,p2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const gp_Parab& P,
				   const gp_Pnt& P1,
				   const gp_Pnt& P2)
: myMakeEdge(P,P1,P2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const gp_Parab& P,
				   const TopoDS_Vertex& V1,
				   const TopoDS_Vertex& V2)
: myMakeEdge(P,V1,V2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const Handle(Geom_Curve)& L)
: myMakeEdge(L)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const Handle(Geom_Curve)& L,
				   const Standard_Real p1,
				   const Standard_Real p2)
: myMakeEdge(L,p1,p2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const Handle(Geom_Curve)& L,
				   const gp_Pnt& P1,
				   const gp_Pnt& P2)
: myMakeEdge(L,P1,P2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const Handle(Geom_Curve)& L,
				   const TopoDS_Vertex& V1,
				   const TopoDS_Vertex& V2)
: myMakeEdge(L,V1,V2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const Handle(Geom_Curve)& L,
				   const gp_Pnt& P1,
				   const gp_Pnt& P2,
				   const Standard_Real p1,
				   const Standard_Real p2)
: myMakeEdge(L,P1,P2,p1,p2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const Handle(Geom_Curve)& L,
				   const TopoDS_Vertex& V1,
				   const TopoDS_Vertex& V2,
				   const Standard_Real p1,
				   const Standard_Real p2)
: myMakeEdge(L,V1,V2,p1,p2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const Handle(Geom2d_Curve)& L,
				   const Handle(Geom_Surface)& S)
: myMakeEdge(L,S)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const Handle(Geom2d_Curve)& L,
				   const Handle(Geom_Surface)& S,
				   const Standard_Real p1,
				   const Standard_Real p2)
: myMakeEdge(L,S,p1,p2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const Handle(Geom2d_Curve)& L,
				   const Handle(Geom_Surface)& S,
				   const gp_Pnt& P1,
				   const gp_Pnt& P2)
: myMakeEdge(L,S,P1,P2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const Handle(Geom2d_Curve)& L,
				   const Handle(Geom_Surface)& S,
				   const TopoDS_Vertex& V1,
				   const TopoDS_Vertex& V2)
: myMakeEdge(L,S,V1,V2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const Handle(Geom2d_Curve)& L,
				   const Handle(Geom_Surface)& S,
				   const gp_Pnt& P1,
				   const gp_Pnt& P2,
				   const Standard_Real p1,
				   const Standard_Real p2)
: myMakeEdge(L,S,P1,P2,p1,p2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::BRepBuilderAPI_MakeEdge(const Handle(Geom2d_Curve)& L,
				   const Handle(Geom_Surface)& S,
				   const TopoDS_Vertex& V1,
				   const TopoDS_Vertex& V2,
				   const Standard_Real p1,
				   const Standard_Real p2)
: myMakeEdge(L,S,V1,V2,p1,p2)
{
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeEdge::Init(const Handle(Geom_Curve)& C)
{
  myMakeEdge.Init(C);
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeEdge::Init(const Handle(Geom_Curve)& C,
			     const Standard_Real p1,
			     const Standard_Real p2)
{
  myMakeEdge.Init(C,p1,p2);
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeEdge::Init(const Handle(Geom_Curve)& C,
			     const gp_Pnt& P1,
			     const gp_Pnt& P2)
{
  myMakeEdge.Init(C,P1,P2);
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeEdge::Init(const Handle(Geom_Curve)& C,
			     const TopoDS_Vertex& V1,
			     const TopoDS_Vertex& V2)
{
  myMakeEdge.Init(C,V1,V2);
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeEdge::Init(const Handle(Geom_Curve)& C,
			     const gp_Pnt& P1,
			     const gp_Pnt& P2,
			     const Standard_Real p1,
			     const Standard_Real p2)
{
  myMakeEdge.Init(C,P1,P2,p1,p2);
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeEdge::Init(const Handle(Geom_Curve)& CC,
			     const TopoDS_Vertex& VV1,
			     const TopoDS_Vertex& VV2,
			     const Standard_Real pp1,
			     const Standard_Real pp2)
{
  myMakeEdge.Init(CC,VV1,VV2,pp1,pp2);
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeEdge::Init(const Handle(Geom2d_Curve)& C,
			     const Handle(Geom_Surface)& S)
{
  myMakeEdge.Init(C,S);
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeEdge::Init(const Handle(Geom2d_Curve)& C,
			     const Handle(Geom_Surface)& S,
			     const Standard_Real p1,
			     const Standard_Real p2)
{
  myMakeEdge.Init(C,S,p1,p2);
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeEdge::Init(const Handle(Geom2d_Curve)& C,
			     const Handle(Geom_Surface)& S,
			     const gp_Pnt& P1,
			     const gp_Pnt& P2)
{
  myMakeEdge.Init(C,S,P1,P2);
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeEdge::Init(const Handle(Geom2d_Curve)& C,
			     const Handle(Geom_Surface)& S,
			     const TopoDS_Vertex& V1,
			     const TopoDS_Vertex& V2)
{
  myMakeEdge.Init(C,S,V1,V2);
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeEdge::Init(const Handle(Geom2d_Curve)& C,
			     const Handle(Geom_Surface)& S,
			     const gp_Pnt& P1,
			     const gp_Pnt& P2,
			     const Standard_Real p1,
			     const Standard_Real p2)
{
  myMakeEdge.Init(C,S,P1,P2,p1,p2);
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeEdge::Init(const Handle(Geom2d_Curve)& CC,
			     const Handle(Geom_Surface)& S,
			     const TopoDS_Vertex& VV1,
			     const TopoDS_Vertex& VV2,
			     const Standard_Real pp1,
			     const Standard_Real pp2)
{
  myMakeEdge.Init(CC,S,VV1,VV2,pp1,pp2);
  if ( myMakeEdge.IsDone()) {
    Done();
    myShape = myMakeEdge.Shape();
  }
}


//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepBuilderAPI_MakeEdge::IsDone() const
{
  return myMakeEdge.IsDone();
}


//=======================================================================
//function : Error
//purpose  : 
//=======================================================================

BRepBuilderAPI_EdgeError BRepBuilderAPI_MakeEdge::Error() const
{
  switch ( myMakeEdge.Error()) {

  case BRepLib_EdgeDone:
    return BRepBuilderAPI_EdgeDone;

  case BRepLib_PointProjectionFailed:
    return BRepBuilderAPI_PointProjectionFailed;

  case BRepLib_ParameterOutOfRange:
    return BRepBuilderAPI_ParameterOutOfRange;

  case BRepLib_DifferentPointsOnClosedCurve:
    return BRepBuilderAPI_DifferentPointsOnClosedCurve;

  case BRepLib_PointWithInfiniteParameter:
    return BRepBuilderAPI_PointWithInfiniteParameter;

  case BRepLib_DifferentsPointAndParameter:
    return BRepBuilderAPI_DifferentsPointAndParameter;

  case BRepLib_LineThroughIdenticPoints:
    return BRepBuilderAPI_LineThroughIdenticPoints;

  }

  // portage WNT
  return BRepBuilderAPI_EdgeDone;
}

//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

const TopoDS_Edge&  BRepBuilderAPI_MakeEdge::Edge() 
{
  return myMakeEdge.Edge();
}


//=======================================================================
//function : Vertex1
//purpose  : 
//=======================================================================

const TopoDS_Vertex&  BRepBuilderAPI_MakeEdge::Vertex1()const 
{
  return myMakeEdge.Vertex1();
}


//=======================================================================
//function : Vertex2
//purpose  : 
//=======================================================================

const TopoDS_Vertex&  BRepBuilderAPI_MakeEdge::Vertex2()const 
{
  return myMakeEdge.Vertex2();
}



//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge::operator TopoDS_Edge()
{
  return Edge();
}
