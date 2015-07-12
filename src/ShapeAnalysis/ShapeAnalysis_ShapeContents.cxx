// Created on: 1999-02-25
// Created by: Pavel DURANDIN
// Copyright (c) 1999-1999 Matra Datavision
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

//szv#4 S4163

#include <BRep_Tool.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_ElementarySurface.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <ShapeAnalysis_ShapeContents.hxx>
#include <ShapeAnalysis_Wire.hxx>
#include <ShapeExtend_WireData.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

ShapeAnalysis_ShapeContents::ShapeAnalysis_ShapeContents()
{
  myBigSplineSec = new TopTools_HSequenceOfShape;
  myIndirectSec = new TopTools_HSequenceOfShape;
  myOffsetSurfaceSec = new TopTools_HSequenceOfShape;
  myTrimmed3dSec = new TopTools_HSequenceOfShape;
  myOffsetCurveSec = new TopTools_HSequenceOfShape;
  myTrimmed2dSec = new TopTools_HSequenceOfShape;
  ClearFlags();
}

void ShapeAnalysis_ShapeContents::Clear()
{
  myNbSolids = 0;
  myNbShells = 0;
  myNbFaces = 0;
  myNbWires = 0;
  myNbEdges = 0;
  myNbVertices = 0;
  myNbSolidsWithVoids = 0;
  myNbBigSplines = 0;
  myNbC0Surfaces = 0;
  myNbC0Curves = 0;
  myNbOffsetSurf = 0;
  myNbIndirectSurf = 0;
  myNbOffsetCurves = 0;
  myNbTrimmedCurve2d = 0;
  myNbTrimmedCurve3d  =0;
  myNbBSplibeSurf = 0;
  myNbBezierSurf = 0;
  myNbTrimSurf = 0;
  myNbWireWitnSeam = 0;
  myNbWireWithSevSeams = 0;
  myNbFaceWithSevWires = 0;
  myNbNoPCurve = 0;
  myNbFreeFaces = 0;
  myNbFreeWires = 0;
  myNbFreeEdges = 0;
  
  myNbSharedSolids = 0;
  myNbSharedShells = 0;
  myNbSharedFaces = 0;
  myNbSharedWires = 0;
  myNbSharedFreeWires = 0;
  myNbSharedFreeEdges = 0;
  myNbSharedEdges = 0;
  myNbSharedVertices = 0;
    
  myBigSplineSec->Clear();
  myIndirectSec->Clear();
  myOffsetSurfaceSec->Clear();
  myTrimmed3dSec->Clear();
  myOffsetCurveSec->Clear();
  myTrimmed2dSec->Clear();
}


void ShapeAnalysis_ShapeContents::ClearFlags()
{
  myBigSplineMode = Standard_False;
  myIndirectMode = Standard_False;
  myOffestSurfaceMode = Standard_False;
  myTrimmed3dMode = Standard_False;
  myOffsetCurveMode = Standard_False;
  myTrimmed2dMode = Standard_False;
}


void ShapeAnalysis_ShapeContents::Perform(const TopoDS_Shape& Shape)
{
  Clear();
  //  On y va
  TopExp_Explorer exp;
  TopTools_MapOfShape mapsh;
//  On note pour les SOLIDES : ceux qui ont des trous (plus d un SHELL)
  
  for (exp.Init (Shape,TopAbs_SOLID); exp.More(); exp.Next()) {
    TopoDS_Solid sol = TopoDS::Solid (exp.Current());
    sol.Location(TopLoc_Location());
    mapsh.Add(sol);
    Standard_Integer nbs = 0;
    for (TopExp_Explorer shel (sol,TopAbs_SHELL); shel.More(); shel.Next())
      nbs ++;
    if (nbs > 1) myNbSolidsWithVoids++;
    myNbSolids++;  
  }
  myNbSharedSolids = mapsh.Extent();

//  Pour les SHELLS, on compte les faces dans les SHELLS
//  Ensuite une soustraction, et on a les faces libres
  mapsh.Clear();
  Standard_Integer nbfaceshell = 0;
  for (exp.Init (Shape,TopAbs_SHELL); exp.More(); exp.Next()) {
    myNbShells++;
    TopoDS_Shell she = TopoDS::Shell(exp.Current());
    she.Location(TopLoc_Location());
    mapsh.Add(she);
    for (TopExp_Explorer shel (she,TopAbs_FACE); shel.More(); shel.Next())
      nbfaceshell ++;
  }
  myNbSharedShells = mapsh.Extent();
//  On note pour les FACES pas mal de choses (surface, topologie)
//  * Surface BSpline > 8192 poles
//  * Surface BSpline "OnlyC0" (not yet impl)
//  * Surface Offset
//  * Surface Elementaire INDIRECTE
//  * Presence de COUTURES; en particulier WIRE A PLUS D UNE COUTURE
//  * Edge : OffsetCurve

  mapsh.Clear();
  for (exp.Init (Shape,TopAbs_FACE); exp.More(); exp.Next()) {
    TopoDS_Face face = TopoDS::Face(exp.Current());
    myNbFaces++;
    TopLoc_Location loc;
    Handle(Geom_Surface) surf = BRep_Tool::Surface (face,loc);
    face.Location(TopLoc_Location());
    mapsh.Add(face);
    Handle(Geom_RectangularTrimmedSurface) trsu =
      Handle(Geom_RectangularTrimmedSurface)::DownCast (surf);
    if (!trsu.IsNull()) {
      myNbTrimSurf++;
      surf = trsu->BasisSurface();
    }
    //#10 rln 27/02/98 BUC50003 entity 56
    //C0 if at least in one direction (U or V)
    if (!surf.IsNull() && !(surf->IsCNu(1) && surf->IsCNv(1))) {
      myNbC0Surfaces++;
    }

    Handle(Geom_BSplineSurface) bsps = Handle(Geom_BSplineSurface)::DownCast(surf);
    if (!bsps.IsNull()) {
      myNbBSplibeSurf++;
      if (bsps->NbUPoles() * bsps->NbVPoles() > 8192) {
	myNbBigSplines++;
	if (myBigSplineMode) myBigSplineSec->Append(face);
      }
    }
    Handle(Geom_ElementarySurface) els = Handle(Geom_ElementarySurface)::DownCast(surf);
    if (!els.IsNull()) {
      if (!els->Position().Direct()) {
	myNbIndirectSurf++;
	if (myIndirectMode) myIndirectSec->Append(face);
      }
    }
    if (surf->IsKind(STANDARD_TYPE(Geom_OffsetSurface))) {
      myNbOffsetSurf++;
      if (myOffestSurfaceMode) myOffsetSurfaceSec->Append(face);
    }
    else if (surf->IsKind(STANDARD_TYPE(Geom_BezierSurface))) {
      myNbBezierSurf++;
    }

    Standard_Integer maxseam = 0, nbwires = 0;
    for (TopExp_Explorer wires(face,TopAbs_WIRE); wires.More(); wires.Next()) {
      TopoDS_Wire wire = TopoDS::Wire (wires.Current());
      Standard_Integer nbseam = 0;
      nbwires ++;
      for (TopExp_Explorer edg(wire,TopAbs_EDGE); edg.More(); edg.Next()) {
	TopoDS_Edge edge = TopoDS::Edge (edg.Current());
	Standard_Real first,last;	
	if (BRep_Tool::IsClosed (edge,face)) nbseam ++;
	Handle(Geom_Curve) c3d = BRep_Tool::Curve (edge,first,last);
	if (!c3d.IsNull()) {
	  if (c3d->IsKind (STANDARD_TYPE(Geom_TrimmedCurve))) {
	    myNbTrimmedCurve3d++;
	    if (myTrimmed3dMode) myTrimmed3dSec->Append(face);
	  }
	}
	Handle(Geom2d_Curve) c2d = BRep_Tool::CurveOnSurface (edge,face,first,last);
	if (c2d.IsNull()) myNbNoPCurve++;
	else if (c2d->IsKind (STANDARD_TYPE(Geom2d_OffsetCurve))) {
	  myNbOffsetCurves++;
	  if (myOffsetCurveMode) myOffsetCurveSec->Append(face);
	}
	else if (c2d->IsKind (STANDARD_TYPE(Geom2d_TrimmedCurve))) {
	  myNbTrimmedCurve2d++;
 	  if (myTrimmed2dMode) myTrimmed2dSec->Append(face);
	}
      }
      if (nbseam > maxseam) maxseam = nbseam;
    }
    if (maxseam == 1) myNbWireWitnSeam++;
    else if (maxseam > 1)
      myNbWireWithSevSeams++;
    if (nbwires > 1) myNbFaceWithSevWires++;
  }
  myNbSharedFaces = mapsh.Extent();
  
  mapsh.Clear();
  for (exp.Init (Shape,TopAbs_WIRE); exp.More(); exp.Next()) {
    TopoDS_Wire wire = TopoDS::Wire(exp.Current());
    wire.Location(TopLoc_Location());
    mapsh.Add(wire);
    myNbWires++;
  }
  myNbSharedWires = mapsh.Extent();
    
//  Ne pas oublier les FACES :
  myNbFreeFaces = myNbFaces - nbfaceshell;

  mapsh.Clear();
  for (exp.Init (Shape,TopAbs_EDGE); exp.More(); exp.Next()) {
    TopoDS_Edge edge = TopoDS::Edge (exp.Current());
    edge.Location(TopLoc_Location());
    mapsh.Add (edge);
    TopLoc_Location loc;
    Standard_Real first,last;
    myNbEdges++;
    Handle(Geom_Curve) c3d = BRep_Tool::Curve (edge,loc,first,last);
    if (!c3d.IsNull() && c3d->IsKind(STANDARD_TYPE(Geom_OffsetCurve))) {
      myNbOffsetCurves++;
      if (myOffsetCurveMode) myOffsetCurveSec->Append(edge);
    }
    if (!c3d.IsNull() && !c3d->IsCN(1)) myNbC0Curves++;
  }
  myNbSharedEdges=mapsh.Extent();

  mapsh.Clear();
  for (exp.Init (Shape,TopAbs_VERTEX); exp.More(); exp.Next()) {
    TopoDS_Vertex vert = TopoDS::Vertex(exp.Current());
    vert.Location(TopLoc_Location());
    myNbVertices++;
    mapsh.Add (vert);
  }
  myNbSharedVertices=mapsh.Extent();
  
  mapsh.Clear();
  for (exp.Init(Shape, TopAbs_EDGE, TopAbs_FACE); exp.More(); exp.Next()) {
    TopoDS_Edge edge = TopoDS::Edge (exp.Current());
    edge.Location(TopLoc_Location());
    myNbFreeEdges++;
    mapsh.Add (edge);
  }
  myNbSharedFreeEdges=mapsh.Extent();
  
  mapsh.Clear();
  for (exp.Init(Shape, TopAbs_WIRE, TopAbs_FACE); exp.More(); exp.Next()) {
    TopoDS_Wire wire = TopoDS::Wire(exp.Current());
    wire.Location(TopLoc_Location());
    myNbFreeWires++;
    mapsh.Add (wire);
  }
  myNbSharedFreeWires=mapsh.Extent();
}
