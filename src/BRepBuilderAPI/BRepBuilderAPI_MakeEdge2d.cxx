// Created on: 1993-07-23
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


#include <BRepBuilderAPI_MakeEdge2d.ixx>


//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const TopoDS_Vertex& V1, 
				       const TopoDS_Vertex& V2)
: myMakeEdge2d(V1,V2)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const gp_Pnt2d& P1, 
				       const gp_Pnt2d& P2)
: myMakeEdge2d(P1,P2)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const gp_Lin2d& L)
: myMakeEdge2d(L)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const gp_Lin2d& L, 
				       const Standard_Real p1, 
				       const Standard_Real p2)
: myMakeEdge2d(L,p1,p2)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const gp_Lin2d& L, 
				       const gp_Pnt2d& P1, 
				       const gp_Pnt2d& P2)
: myMakeEdge2d(L,P1,P2)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const gp_Lin2d& L, 
				       const TopoDS_Vertex& V1, 
				       const TopoDS_Vertex& V2)
: myMakeEdge2d(L,V1,V2)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const gp_Circ2d& C) 
: myMakeEdge2d(C)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const gp_Circ2d& C,
				       const Standard_Real p1,
				       const Standard_Real p2)
:myMakeEdge2d(C,p1,p2)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const gp_Circ2d& C,
				       const gp_Pnt2d& P1,
				       const gp_Pnt2d& P2)
: myMakeEdge2d(C,P1,P2)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const gp_Circ2d& C,
				       const TopoDS_Vertex& V1,
				       const TopoDS_Vertex& V2)
: myMakeEdge2d(C,V1,V2)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const gp_Elips2d& E) 
: myMakeEdge2d(E)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const gp_Elips2d& E,
				       const Standard_Real p1,
				       const Standard_Real p2)
: myMakeEdge2d(E,p1,p2)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const gp_Elips2d& E,
				       const gp_Pnt2d& P1,
				       const gp_Pnt2d& P2)
: myMakeEdge2d(E,P1,P2)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const gp_Elips2d& E,
				       const TopoDS_Vertex& V1,
				       const TopoDS_Vertex& V2)
: myMakeEdge2d(E,V1,V2)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const gp_Hypr2d& H)
: myMakeEdge2d(H)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const gp_Hypr2d& H,
				       const Standard_Real p1,
				       const Standard_Real p2)
: myMakeEdge2d(H,p1,p2)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const gp_Hypr2d& H,
				       const gp_Pnt2d& P1,
				       const gp_Pnt2d& P2)
: myMakeEdge2d(H,P1,P2)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const gp_Hypr2d& H,
				       const TopoDS_Vertex& V1,
				       const TopoDS_Vertex& V2)
: myMakeEdge2d(H,V1,V2)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const gp_Parab2d& P)
: myMakeEdge2d(P)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const gp_Parab2d& P,
				       const Standard_Real p1,
				       const Standard_Real p2)
: myMakeEdge2d(P,p1,p2)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const gp_Parab2d& P,
				       const gp_Pnt2d& P1,
				       const gp_Pnt2d& P2)
: myMakeEdge2d(P,P1,P2)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const gp_Parab2d& P,
				       const TopoDS_Vertex& V1,
				       const TopoDS_Vertex& V2)
: myMakeEdge2d(P,V1,V2)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const Handle(Geom2d_Curve)& L)
: myMakeEdge2d(L)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const Handle(Geom2d_Curve)& L,
				       const Standard_Real p1,
				       const Standard_Real p2)
: myMakeEdge2d(L,p1,p2)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const Handle(Geom2d_Curve)& L,
				       const gp_Pnt2d& P1,
				       const gp_Pnt2d& P2)
: myMakeEdge2d(L,P1,P2)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const Handle(Geom2d_Curve)& L,
				       const TopoDS_Vertex& V1,
				       const TopoDS_Vertex& V2)
: myMakeEdge2d(L,V1,V2)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const Handle(Geom2d_Curve)& L,
				       const gp_Pnt2d& P1,
				       const gp_Pnt2d& P2,
				       const Standard_Real p1,
				       const Standard_Real p2)
: myMakeEdge2d(L,P1,P2,p1,p2)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : BRepBuilderAPI_MakeEdge2d
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::BRepBuilderAPI_MakeEdge2d(const Handle(Geom2d_Curve)& L,
				       const TopoDS_Vertex& V1,
				       const TopoDS_Vertex& V2,
				       const Standard_Real p1,
				       const Standard_Real p2)
: myMakeEdge2d(L,V1,V2,p1,p2)
{
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeEdge2d::Init(const Handle(Geom2d_Curve)& C)
{
  myMakeEdge2d.Init(C);
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeEdge2d::Init(const Handle(Geom2d_Curve)& C,
			       const Standard_Real p1,
			       const Standard_Real p2)
{
  myMakeEdge2d.Init(C,p1,p2);
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeEdge2d::Init(const Handle(Geom2d_Curve)& C,
			       const gp_Pnt2d& P1,
			       const gp_Pnt2d& P2)
{
  myMakeEdge2d.Init(C,P1,P2);
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeEdge2d::Init(const Handle(Geom2d_Curve)& C,
			       const TopoDS_Vertex& V1,
			       const TopoDS_Vertex& V2)
{
  myMakeEdge2d.Init(C,V1,V2);
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeEdge2d::Init(const Handle(Geom2d_Curve)& C,
			       const gp_Pnt2d& P1,
			       const gp_Pnt2d& P2,
			       const Standard_Real p1,
			       const Standard_Real p2)
{
  myMakeEdge2d.Init(C,P1,P2,p1,p2);
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeEdge2d::Init(const Handle(Geom2d_Curve)& CC,
			       const TopoDS_Vertex& VV1,
			       const TopoDS_Vertex& VV2,
			       const Standard_Real pp1,
			       const Standard_Real pp2)
{
  myMakeEdge2d.Init(CC,VV1,VV2,pp1,pp2);
  if ( myMakeEdge2d.IsDone()) {
    Done();
    myShape = myMakeEdge2d.Shape();
  }
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepBuilderAPI_MakeEdge2d::IsDone() const
{
  return myMakeEdge2d.IsDone();
}


//=======================================================================
//function : Error
//purpose  : 
//=======================================================================

BRepBuilderAPI_EdgeError BRepBuilderAPI_MakeEdge2d::Error() const
{
  switch ( myMakeEdge2d.Error()) {

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

const TopoDS_Edge&  BRepBuilderAPI_MakeEdge2d::Edge()const 
{
  return myMakeEdge2d.Edge();
}


//=======================================================================
//function : Vertex1
//purpose  : 
//=======================================================================

const TopoDS_Vertex&  BRepBuilderAPI_MakeEdge2d::Vertex1()const 
{
  return myMakeEdge2d.Vertex1();
}


//=======================================================================
//function : Vertex2
//purpose  : 
//=======================================================================

const TopoDS_Vertex&  BRepBuilderAPI_MakeEdge2d::Vertex2()const 
{
  return myMakeEdge2d.Vertex2();
}



//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeEdge2d::operator TopoDS_Edge() const
{
  return Edge();
}
