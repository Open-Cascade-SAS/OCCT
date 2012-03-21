// Created on: 1996-02-05
// Created by: Jea Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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
#ifndef DEB
  default:
    break;
#endif
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
