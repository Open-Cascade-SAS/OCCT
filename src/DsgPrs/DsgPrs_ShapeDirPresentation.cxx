// Created on: 1995-10-06
// Created by: Jing Cheng MEI
// Copyright (c) 1995-1999 Matra Datavision
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


#include <Bnd_Box.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepClass_Edge.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <DsgPrs_ShapeDirPresentation.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomLProp_CLProps.hxx>
#include <GeomLProp_SLProps.hxx>
#include <gp.hxx>
#include <gp_Dir.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ListOfShape.hxx>

//=======================================================================
//function : FindPointOnFace
//purpose  : internal use
//=======================================================================
static Standard_Boolean FindPointOnFace(const TopoDS_Face& face, gp_Pnt2d& pt2d)
{
  // discredisation of the external contour and computing the center of gravity

  TopExp_Explorer wireExp;
  wireExp.Init(face, TopAbs_WIRE);
  if (!wireExp.More()) {
    return Standard_False;
  }

  Standard_Integer npoints, nptt = 21;
  TColgp_Array1OfPnt2d points(1, nptt);
  Standard_Real area=0., xcent=0., ycent=0.;
  TopExp_Explorer edgeExp;

  for (edgeExp.Init(wireExp.Current(), TopAbs_EDGE); edgeExp.More(); edgeExp.Next()) {    
    // discretize the 2d curve
    Standard_Real first, last;
    Handle(Geom2d_Curve) c2d = BRep_Tool::CurveOnSurface(TopoDS::Edge(edgeExp.Current()), face, first, last);
    if (TopoDS::Edge(edgeExp.Current()).Orientation() == TopAbs_REVERSED) {
      Standard_Real change = first;
      first = last;
      last = change;
    }
    if (c2d->DynamicType() == STANDARD_TYPE(Geom2d_Line)) {
      npoints = 2;
      c2d->D0(first, points(1));
      c2d->D0(last, points(2));
    }
    else {
      Standard_Real deltaT, t;
      npoints = nptt;
      deltaT = (last - first) / (nptt-1);
      for (Standard_Integer i=1; i<=nptt; i++) {
	if (i == 1) {
	  t = first;
	}
	else if (i == nptt) {
	  t = last;
	}
	else {
	  t = first + (i-1) * deltaT;
	}
	c2d->D0(t, points(i));
      } 
    }
      
    // compute the contribution to the center of gravity

    Standard_Real h, c, d;
    for (Standard_Integer i=1; i<=npoints-1; i++) {

      h = 0.5*(points(i).Y() + points(i+1).Y());
      c = points(i+1).X() - points(i).X();
      d = points(i+1).X() + points(i).X();
      area += h*c;
      xcent += 0.5*h*c*d;
      ycent += 0.5*h*h*c;
    }
  }

  if (Abs(area) < gp::Resolution()) {
    pt2d.SetCoord(points(1).X(), points(1).Y());
    return Standard_False;
  }

  pt2d.SetCoord(xcent / area, ycent / area);

  // verify that (upar vpar) is a point on the face

  BRepClass_FaceClassifier fClass(face, pt2d, gp::Resolution());

  if ((fClass.State() == TopAbs_OUT) || (fClass.State() == TopAbs_UNKNOWN)) {
    // try to find a point on face
    pt2d=points(1);
  }
  return Standard_True;
}


//=======================================================================
//function : ComputeDir
//purpose  : internal use
//=======================================================================

static Standard_Boolean ComputeDir(const TopoDS_Shape& shape, gp_Pnt& pt, gp_Dir& dir, const Standard_Integer mode)
{
  TopLoc_Location loc;
  if (shape.ShapeType() == TopAbs_EDGE) {
    Standard_Real first, last;
    Handle(Geom_Curve) curv0 = BRep_Tool::Curve(TopoDS::Edge(shape), loc, first, last);
    Handle(Geom_Curve) curve = Handle(Geom_Curve)::DownCast(curv0->Copy());
    curve->Transform(loc.Transformation()); 
    GeomLProp_CLProps lProps(curve, 1, gp::Resolution());
    lProps.SetParameter((mode == 0)? last : first);
    if (!lProps.IsTangentDefined())
      return Standard_False;
    pt = lProps.Value();
    lProps.Tangent(dir);
  }
  else if (shape.ShapeType() == TopAbs_FACE) {
    gp_Pnt2d pt2d;
    Handle(Geom_Surface) surface = BRep_Tool::Surface(TopoDS::Face(shape));    
    if (BRep_Tool::NaturalRestriction(TopoDS::Face(shape))) {
      Standard_Real u1, u2, v1, v2;
      surface->Bounds(u1, u2, v1, v2);
      pt2d.SetCoord((u1+u2)*0.5, (v1+v2)*0.5);
    }
    else {
      if (!FindPointOnFace(TopoDS::Face(shape), pt2d))
        return Standard_False;
    }
    
    GeomLProp_SLProps lProps(surface, pt2d.X(), pt2d.Y(), 1, gp::Resolution());
    if (!lProps.IsNormalDefined())
      return Standard_False;

    pt = lProps.Value();
    dir = lProps.Normal();
  }
  if (((shape.Orientation() == TopAbs_FORWARD) && (mode == 1)) ||
      ((shape.Orientation() == TopAbs_REVERSED) && (mode == 0))) {
    dir.Reverse();
  }
  return Standard_True;
}  


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void DsgPrs_ShapeDirPresentation::Add(const Handle(Prs3d_Presentation)& prs,
				      const Handle(Prs3d_Drawer)& drawer,
				      const TopoDS_Shape& shape,
				      const Standard_Integer mode)
     
{
  if ((mode != 0) && (mode != 1))
    return;
  
  gp_Dir dir;
  gp_Pnt pt;
  Bnd_Box box;

  if (shape.ShapeType() == TopAbs_EDGE) {
    ComputeDir(shape, pt, dir, mode);
    BRepBndLib::Add(shape, box);
  }
  else if (shape.ShapeType() == TopAbs_FACE) {
    ComputeDir(shape, pt, dir, mode);
    BRepBndLib::Add(shape, box);
  }    
  else if (shape.ShapeType() == TopAbs_WIRE) {
    TopTools_ListOfShape aList;
    Standard_Integer nb = 0;
    BRepTools_WireExplorer anExp;
    for (anExp.Init(TopoDS::Wire(shape)); anExp.More(); anExp.Next()) {
      const TopoDS_Edge& edge = anExp.Current();
      nb++;
      if (nb <=3)
        BRepBndLib::Add(edge, box);
      aList.Append(edge);
    }

    if (mode == 0) {
      const TopoDS_Edge& edge = TopoDS::Edge(aList.Last());
      ComputeDir(edge, pt, dir, mode);
    }
    else {
      const TopoDS_Edge& edge = TopoDS::Edge(aList.First());
      ComputeDir(edge, pt, dir, mode);
    }
  }
  else {
    TopExp_Explorer faceExp;
    
    TopTools_ListOfShape aList;
    Standard_Integer nb = 0;
    for (faceExp.Init(shape, TopAbs_FACE); faceExp.More(); faceExp.Next()) {
      nb++;
      const TopoDS_Face& face = TopoDS::Face(faceExp.Current());
      aList.Append(face);
      BRepBndLib::Add(face, box);
      if (nb > 3) break;
    }
    const TopoDS_Face& face = TopoDS::Face(aList.Last());
    ComputeDir(face, pt, dir, mode);
  }  

  Standard_Real c[6];
  box.Get(c[0],c[1],c[2],c[3],c[4],c[5]);
  
  gp_Pnt ptmin(c[0], c[1], c[2]), ptmax(c[3], c[4], c[5]);
  Standard_Real leng = ptmin.Distance(ptmax)/3.;
  // mei 19/09/96 extrusion infinie -> taille fixe
  if (leng >= 20000.) leng = 50;

  gp_Pnt pt2(pt.XYZ()+leng*dir.XYZ());

  Prs3d_Root::CurrentGroup(prs)->SetPrimitivesAspect(drawer->LineAspect()->Aspect());

  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(pt);
  aPrims->AddVertex(pt2);
  Prs3d_Root::CurrentGroup(prs)->AddPrimitiveArray(aPrims);

  Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (prs), pt2, dir, M_PI/180.*10., leng*0.3);
}
