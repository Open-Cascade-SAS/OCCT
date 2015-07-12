// Created on: 1996-04-02
// Created by: Jacques GOUSSARD
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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pnt2d.hxx>
#include <LocOpe.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_MapOfShape.hxx>

#define NECHANT 10


//=======================================================================
//function : Closed
//purpose  : 
//=======================================================================

Standard_Boolean LocOpe::Closed(const TopoDS_Wire& W,
				const TopoDS_Face& F)
{
  TopoDS_Vertex Vf,Vl;
  TopExp::Vertices(W,Vf,Vl);
  if (!Vf.IsSame(Vl)) {
    return Standard_False;
  }

  // On recherche l`edge contenant Vf FORWARD

  TopExp_Explorer exp,exp2;
  for (exp.Init(W.Oriented(TopAbs_FORWARD),TopAbs_EDGE);
       exp.More();exp.Next()) {
    for (exp2.Init(exp.Current(),TopAbs_VERTEX);exp2.More();exp2.Next()) {
      if (exp2.Current().IsSame(Vf) && 
	  exp2.Current().Orientation() == TopAbs_FORWARD) {
	break;
      }
    }
    if (exp2.More()) {
      break;
    }
  }
  TopoDS_Edge Ef = TopoDS::Edge(exp.Current());
  
  // On recherche l`edge contenant Vl REVERSED

  for (exp.Init(W.Oriented(TopAbs_FORWARD),TopAbs_EDGE);
       exp.More();exp.Next()) {
    for (exp2.Init(exp.Current(),TopAbs_VERTEX);exp2.More();exp2.Next()) {
      if (exp2.Current().IsSame(Vl) && 
	  exp2.Current().Orientation() == TopAbs_REVERSED) {
	break;
      }
    }
    if (exp2.More()) {
      break;
    }
  }
  TopoDS_Edge El = TopoDS::Edge(exp.Current());
  
  Standard_Real f,l;
  gp_Pnt2d pf,pl;
  Handle(Geom2d_Curve) C2d = BRep_Tool::CurveOnSurface(Ef,F,f,l);
  if (Ef.Orientation() == TopAbs_FORWARD) {
    pf = C2d->Value(f);
  }
  else {
    pf = C2d->Value(l);
  }
  C2d = BRep_Tool::CurveOnSurface(El,F,f,l);
  if (El.Orientation() == TopAbs_FORWARD) {
    pl = C2d->Value(l);
  }
  else {
    pl = C2d->Value(f);
  }

  if (pf.Distance(pl) <= Precision::PConfusion(Precision::Confusion())) {
    return Standard_True;
  }
  return Standard_False;
}


//=======================================================================
//function : Closed
//purpose  : 
//=======================================================================

Standard_Boolean LocOpe::Closed(const TopoDS_Edge& E,
				const TopoDS_Face& F)
{
  BRep_Builder B;
  TopoDS_Wire W;
  B.MakeWire(W);
  B.Add(W,E.Oriented(TopAbs_FORWARD));
  return LocOpe::Closed(W,F);
}



//=======================================================================
//function : Closed
//purpose  : 
//=======================================================================

Standard_Boolean LocOpe::TgtFaces(const TopoDS_Edge& E,
				  const TopoDS_Face& F1,
				  const TopoDS_Face& F2)
{
  BRepAdaptor_Surface bs(F1, Standard_False);
  Standard_Real u;
  Standard_Real ta = 0.0001;

  TopoDS_Edge e = E;

  Handle(BRepAdaptor_HSurface) HS1 = new BRepAdaptor_HSurface(F1);
  Handle(BRepAdaptor_HSurface) HS2 = new BRepAdaptor_HSurface(F2);
  e.Orientation(TopAbs_FORWARD);
  Handle(BRepAdaptor_HCurve2d) HC2d = new BRepAdaptor_HCurve2d();
  Handle(BRepAdaptor_HCurve2d) HC2d2 = new BRepAdaptor_HCurve2d();
  HC2d->ChangeCurve2d().Initialize(e,F1);
  HC2d2->ChangeCurve2d().Initialize(e,F2);
  

//  Adaptor3d_CurveOnSurface C1(HC2d,HS1);

  Standard_Boolean rev1 = (F1.Orientation() == TopAbs_REVERSED);
  Standard_Boolean rev2 = (F2.Orientation() == TopAbs_REVERSED);
  Standard_Real f,l,eps,angmin = M_PI,angmax = -M_PI,ang;
  BRep_Tool::Range(e,f,l);

  eps = (l - f)/100.;
  f += eps; // pour eviter de faire des calculs sur les 
  l -= eps; // pointes des carreaux pointus.
  gp_Pnt2d p;
  gp_Pnt pp1;
  gp_Vec du,dv;
  gp_Vec d1,d2;

  Standard_Real uu, vv;

  Standard_Integer i;
  for(i = 0; i <= 20; i++){
    u = f + (l-f)*i/20;
    HC2d->D0(u,p);
    HS1->D1(p.X(),p.Y(),pp1,du,dv);
    d1 = (du.Crossed(dv)).Normalized();
    if(rev1) d1.Reverse();
    HC2d2->D0(u,p);
    p.Coord(uu,vv);
    HS2->D1(uu,vv,pp1,du,dv);
    d2 = (du.Crossed(dv)).Normalized();
    if(rev2) d2.Reverse();
    ang = d1.Angle(d2);
    if(ang <= angmin) angmin = ang;
    if(ang >= angmax) angmax = ang;
  }
  return (angmax<=ta);
}


//=======================================================================
//function : SampleEdges
//purpose  : 
//=======================================================================

void LocOpe::SampleEdges(const TopoDS_Shape& theShape,
			 TColgp_SequenceOfPnt& theSeq)
{
  theSeq.Clear();
  TopTools_MapOfShape theMap;
  
  TopExp_Explorer exp(theShape,TopAbs_EDGE);
  TopLoc_Location Loc;
  Handle(Geom_Curve) C;
  Standard_Real f,l,prm;
  Standard_Integer i;

  // Computes points on edge, but does not take the extremities into account
  for (; exp.More(); exp.Next()) {
    const TopoDS_Edge& edg = TopoDS::Edge(exp.Current());
    if (!theMap.Add(edg)) {
      continue;
    }
    if (!BRep_Tool::Degenerated(edg)) {
      C = BRep_Tool::Curve(edg,Loc,f,l);
      C = Handle(Geom_Curve)::
	DownCast(C->Transformed(Loc.Transformation()));
      Standard_Real delta = (l - f)/NECHANT*0.123456;
      for (i=1; i<NECHANT; i++) {
	prm = delta + ((NECHANT-i)*f+i*l)/NECHANT;
	theSeq.Append(C->Value(prm));
      }
    }
  }

  // Adds every vertex
  for (exp.Init(theShape,TopAbs_VERTEX); exp.More(); exp.Next()) {
    if (theMap.Add(exp.Current())) {
      theSeq.Append(BRep_Tool::Pnt(TopoDS::Vertex(exp.Current())));
    }
  }
}





/*
Standard_Boolean LocOpe::IsInside(const TopoDS_Face& F1,
				  const TopoDS_Face& F2)
{
  Standard_Boolean Result = Standard_True;

  TopExp_Explorer exp1, exp2;

  for(exp1.Init(F1, TopAbs_EDGE); exp1.More(); exp1.Next())  {
    TopoDS_Edge e1 = TopoDS::Edge(exp1.Current());
    BRepAdaptor_Curve2d C1(e1, F1); 
    for(exp2.Init(F2, TopAbs_EDGE); exp2.More(); exp2.Next())  {
      TopoDS_Edge e2 = TopoDS::Edge(exp2.Current());
      BRepAdaptor_Curve2d C2(e2, F2); 
      Geom2dInt_GInter C;
      C.Perform(C1, C2, Precision::Confusion(), Precision::Confusion());
      if(!C.IsEmpty()) Result = Standard_False;
      if(Result == Standard_False) {
	for(exp3.Init(e2, TopAbs_VERTEX); exp3.More(); exp3.Next())  {
	  
        }
      }
    }
    if(Result == Standard_False) break;
  }
  return Result;
}

*/



