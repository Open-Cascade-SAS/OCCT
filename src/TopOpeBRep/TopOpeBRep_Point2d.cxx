// Created on: 1998-10-29
// Created by: Jean Yves LEBEY
// Copyright (c) 1998-1999 Matra Datavision
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

#ifdef DRAW
#include <TopOpeBRepTool_DRAW.hxx>
#endif


#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRep_define.hxx>
#include <TopOpeBRep_EdgesIntersector.hxx>
#include <TopOpeBRep_Hctxee2d.hxx>
#include <TopOpeBRep_Hctxff2d.hxx>
#include <TopOpeBRep_Point2d.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_Transition.hxx>

//=======================================================================
//function : TopOpeBRep_Point2d
//purpose  : 
//=======================================================================
TopOpeBRep_Point2d::TopOpeBRep_Point2d() :
myhaspint(Standard_False),
myisvertex1(Standard_False),
myparameter1(0.),
myisvertex2(Standard_False),
myparameter2(0.),
myispointofsegment(Standard_False),
myips1(0),myips2(0),myhasancestors(Standard_False),
mystatus(TopOpeBRep_P2DUNK),
myindex(0),
mykeep(Standard_True),
myedgesconfig(TopOpeBRepDS_UNSHGEOMETRY),
mytolerance(0.)
{
}

//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================
const TopoDS_Vertex& TopOpeBRep_Point2d::Vertex(const Standard_Integer Index) const
{
  if (!IsVertex(Index)) throw Standard_Failure("TopOpeBRep_Point2d::Vertex");
  if      (Index == 1) return myvertex1;
  else if (Index == 2) return myvertex2;
  else throw Standard_Failure("TopOpeBRep_Point2d::Vertex");
}

//=======================================================================
//function : Transition
//purpose  : 
//=======================================================================
const TopOpeBRepDS_Transition& TopOpeBRep_Point2d::Transition(const Standard_Integer Index) const
{
  if      (Index == 1) return mytransition1;
  else if (Index == 2) return mytransition2;
  else throw Standard_Failure("TopOpeBRep_Point2d::Transition");
}

//=======================================================================
//function : ChangeTransition
//purpose  : 
//=======================================================================
TopOpeBRepDS_Transition& TopOpeBRep_Point2d::ChangeTransition(const Standard_Integer Index)
{
  if      (Index == 1) return mytransition1;
  else if (Index == 2) return mytransition2;
  else throw Standard_Failure("TopOpeBRep_Point2d::ChangeTransition");
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
#ifdef OCCT_DEBUG
void TopOpeBRep_Point2d::Dump(const Standard_Integer E1index,const Standard_Integer E2index) const
{
  Standard_Real par1 = Parameter(1);
  Standard_Real par2 = Parameter(2);
  
  Standard_Integer index = Index();
  Standard_Boolean keep = Keep();
  Standard_Integer sts = Status();
  Standard_Boolean pos = IsPointOfSegment();

  Standard_Boolean isvertex1 = IsVertex(1); TopoDS_Vertex V1; if (isvertex1) V1 = Vertex(1);
  Standard_Boolean isvertex2 = IsVertex(2); TopoDS_Vertex V2; if (isvertex2) V2 = Vertex(2);

  Standard_Integer ia1,ia2; SegmentAncestors(ia1,ia2);
  cout<<endl<<"p2d "<<index<<"  k="<<keep<<" pos="<<pos;
  switch (sts) {
  case TopOpeBRep_P2DUNK : cout<<" sts=u";break;
  case TopOpeBRep_P2DSGF : cout<<" sts=f";break;
  case TopOpeBRep_P2DSGL : cout<<" sts=l";break;
  case TopOpeBRep_P2DNEW :
    cout<<" sts=n";
    cout<<" anc="<<ia1<<","<<ia2;
    break;
  case TopOpeBRep_P2DINT : cout<<" sts=i";break;
  } // switch
  cout<<" cfg=";TopOpeBRepDS::Print(myedgesconfig,cout);
  cout<<endl;
  
  gp_Pnt P3D = Value();
#ifdef DRAW
  cout<<FUN_tool_PRODINP()<<"P"<<Index()<<" "<<P3D.X()<<" "<<P3D.Y()<<" "<<P3D.Z()<<"; # tol = "<<tol<<endl;
#endif
  cout<<"     on (1) :";
  cout<<" vertex(1) : ";
  cout<<(isvertex1?1:0);
  cout<<"  T "<<E1index<<"(1)";
  cout<<" par(1) = "<<par1;
  if (isvertex1) {
    P3D = BRep_Tool::Pnt(V1);
    cout<<" PV(1) : "<<P3D.X()<<" "<<P3D.Y()<<" "<<P3D.Z();
  }
  cout<<endl;
  
  cout<<"     on (2) :";
  cout<<" vertex(2) : ";
  cout<<(isvertex2?1:0);
  cout<<"  T "<<E2index<<"(2)";
  cout<<" par(2) = "<<par2;
  if (isvertex2) {
    P3D = BRep_Tool::Pnt(V2);
    cout<<" PV(2) : "<<P3D.X()<<" "<<P3D.Y()<<" "<<P3D.Z();
  }
  cout<<endl;
}
#else
void TopOpeBRep_Point2d::Dump(const Standard_Integer,const Standard_Integer) const {}
#endif
