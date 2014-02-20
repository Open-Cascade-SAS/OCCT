// Created on: 1996-02-05
// Created by: Jea Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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

#include <TestTopOpe_BOOP.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_Surface.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS.hxx>
#include <TopAbs.hxx>
#include <gp_Pnt.hxx>

void TestTopOpe_BOOP::SetTol(const TopOpeBRepDS_Kind K,const Standard_Integer index,const Standard_Real tol)
{
  TopOpeBRepDS_DataStructure& BDS = myHDS->ChangeDS();
  switch(K) {
  case TopOpeBRepDS_SURFACE: {
    TopOpeBRepDS_Surface& Surf = BDS.ChangeSurface(index);
    Surf.Tolerance(tol);
    break;
  }
  case TopOpeBRepDS_CURVE: {
    TopOpeBRepDS_Curve& Curv = BDS.ChangeCurve(index);
    Curv.Tolerance(tol);
    break;
  }
  case TopOpeBRepDS_POINT: {
    TopOpeBRepDS_Point& Poin = BDS.ChangePoint(index);
    Poin.Tolerance(tol);
    break;
  }
  default:
    break;
  }
  TopOpeBRepDS::Print(K, index, cout);
  cout<<endl;
}

void TestTopOpe_BOOP::SetTol(const Standard_Integer index,const Standard_Real tol)
{
  TopoDS_Shape& S = *((TopoDS_Shape*)(&myHDS->Shape(index)));
  TopAbs_ShapeEnum se = S.ShapeType();
  BRep_Builder BRB;
  if(se == TopAbs_FACE) {
    TopoDS_Face& F = TopoDS::Face(S);
    BRB.UpdateFace(F, tol);
  }
  else if(se == TopAbs_EDGE) {
    TopoDS_Edge& E = TopoDS::Edge(S);
    BRB.UpdateEdge(E, tol);
  }
  else if(se == TopAbs_VERTEX) {
    TopoDS_Vertex& V = TopoDS::Vertex(S);
    BRB.UpdateVertex(V, tol);
  }
  else {
    cout<<"Impossible to change the tolerance of ";
    TopOpeBRepDS::Print(TopOpeBRepDS::ShapeToKind(se),cout);
    cout<<endl;
    return;
  }
  TopOpeBRepDS::Print(se, index, cout);
  cout<<endl;
}

void TestTopOpe_BOOP::SetPnt(const Standard_Integer i,const Standard_Real x,const Standard_Real y,const Standard_Real z)
{
  TopOpeBRepDS_Point& Poin = myHDS->ChangeDS().ChangePoint(i);
  gp_Pnt& Pnt = Poin.ChangePoint();
  Pnt.SetCoord(x,y,z);
}
