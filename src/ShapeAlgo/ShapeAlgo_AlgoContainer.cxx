// Created on: 2000-02-07
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepTools.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomInt_WLApprox.hxx>
#include <gp_Pnt.hxx>
#include <IntPatch_WLine.hxx>
#include <IntSurf_LineOn2S.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <Precision.hxx>
#include <ShapeAlgo_AlgoContainer.hxx>
#include <ShapeAlgo_ToolContainer.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_Wire.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeConstruct.hxx>
#include <ShapeCustom.hxx>
#include <ShapeCustom_Surface.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix.hxx>
#include <ShapeFix_Edge.hxx>
#include <ShapeFix_Wire.hxx>
#include <ShapeUpgrade.hxx>
#include <ShapeUpgrade_ShapeDivideContinuity.hxx>
#include <Standard_Type.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeAlgo_AlgoContainer,Standard_Transient)

//=======================================================================
//function : ShapeAlgo_AlgoContainer
//purpose  : 
//=======================================================================
ShapeAlgo_AlgoContainer::ShapeAlgo_AlgoContainer()
{
  myTC = new ShapeAlgo_ToolContainer;
}

//=======================================================================
//function : ConnectNextWire
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAlgo_AlgoContainer::ConnectNextWire (const Handle(ShapeAnalysis_Wire)& saw,
                                                           const Handle(ShapeExtend_WireData)& nextsewd,
                                                           const Standard_Real maxtol,
                                                           Standard_Real& distmin,
                                                           Standard_Boolean& revsewd,
                                                           Standard_Boolean& revnextsewd) const
{
  distmin = 0;
  revsewd = revnextsewd = Standard_False;
  if (nextsewd->NbEdges() == 0) return Standard_True;
  
  Handle(ShapeExtend_WireData) sewd = saw->WireData();
  //add edges into empty WireData
  if(sewd->NbEdges() == 0) {
    sewd->Add (nextsewd);
    return Standard_True;
  }
  
  Standard_Real tailhead, tailtail, headtail, headhead;
  saw->CheckShapeConnect (tailhead, tailtail, headtail, headhead, nextsewd->Wire(), maxtol);
  distmin = tailhead;
  Standard_Real precision = saw->Precision();
  
  if ( tailhead > precision && tailtail > precision &&
       ( saw->LastCheckStatus (ShapeExtend_DONE4) ||
	 saw->LastCheckStatus (ShapeExtend_DONE3) ) ) {
    sewd->Reverse();
    distmin = headhead;
    revsewd = Standard_True;
    if (saw->LastCheckStatus (ShapeExtend_DONE3)) {
      nextsewd->Reverse();
      revnextsewd = Standard_True;
      distmin = headtail;
    }
  }
  else if (!saw->LastCheckStatus (ShapeExtend_FAIL) && !saw->LastCheckStatus (ShapeExtend_DONE5)) {
    nextsewd->Reverse();
    revnextsewd = Standard_True;
    distmin     = tailtail;
  }
  Standard_Boolean OK = !saw->LastCheckStatus (ShapeExtend_FAIL);
  if (OK) sewd->Add (nextsewd);
  return OK;
}

//=======================================================================
//function : ApproxBSplineCurve
//purpose  : 
//=======================================================================

void ShapeAlgo_AlgoContainer::ApproxBSplineCurve (const Handle(Geom_BSplineCurve)& bspline,
                                                  TColGeom_SequenceOfCurve& seq) const
{
  seq.Clear();
  Handle(Geom_BSplineCurve)  res, modifCurve;
  TColGeom_SequenceOfCurve SCurve;

  // si la BSpline est de degre 1 , on approxime .
  // on passe par le programme des intersections ou tout le travail
  // est deja fait !!! ( il faut faire des paquets de 30 points
  // maximum , travailler dans un espace 0,1 pour tenir la precision)
  
  if (bspline->Degree() != 1) {
    seq.Append(bspline);
    return;
  }

  // on detecte d`eventuelles cassures par la multiplicite des poles.
  // Puis on approxime chaque "partie" de BSpline 

  Standard_Integer NbKnots = bspline->NbKnots();
  Standard_Integer NbPoles = bspline->NbPoles();
  TColgp_Array1OfPnt      Poles(1,NbPoles);
  TColStd_Array1OfReal    Weigs(1,NbPoles); Weigs.Init(1.);
  TColStd_Array1OfReal    Knots(1,NbKnots);
  TColStd_Array1OfInteger Mults(1,NbKnots); 

  bspline->Poles(Poles);
  if ( bspline->IsRational()) bspline->Weights(Weigs);
  bspline->Knots(Knots);
  bspline->Multiplicities(Mults);
  Standard_Integer deg = bspline->Degree();
  
  Standard_Integer jpole = 1;
  Standard_Integer j, PoleIndex, I1;
  PoleIndex = 1;
  I1 = 1;
  for ( Standard_Integer ipole = 1; ipole < NbPoles; ipole++) {
    if (Poles(ipole).IsEqual(Poles(ipole+1),Precision::Confusion())) {
      if (jpole == 1) {
	PoleIndex++;
      }
      else {
	TColgp_Array1OfPnt      newPoles(1,jpole);
	TColStd_Array1OfReal    newWeigs(1,jpole); Weigs.Init(1.);
	Standard_Integer NbNew = jpole - deg + 1;
	TColStd_Array1OfReal    newKnots(1,NbNew);
	TColStd_Array1OfInteger newMults(1,NbNew);
	for ( j = 1; j <= NbNew; j++) {
	  newKnots(j) = Knots(I1+j-1);
	  newMults(j) = Mults(I1+j-1);
	}
	newMults(1) = newMults(NbNew) = deg+1;
	for ( j = 1; j <= jpole; j++) {
	  newWeigs(j) = Weigs(PoleIndex  );
	  newPoles(j) = Poles(PoleIndex++);
	}
	
	Handle(Geom_BSplineCurve) newC = new Geom_BSplineCurve
	  (newPoles, newWeigs, newKnots, newMults, deg);
	SCurve.Append(newC);
	I1 = ipole+1;
	jpole = 1;
      }
    }
    else {
      jpole++;
    }
  }  
    

  Handle(Geom_BSplineCurve) mycurve;
  Standard_Integer nbcurves = SCurve.Length();
  if (nbcurves == 0) {
    nbcurves = 1;
    SCurve.Append(bspline);
  }
			
  for (Standard_Integer itab = 1; itab <= nbcurves; itab++) { 
    mycurve = Handle(Geom_BSplineCurve)::DownCast(SCurve.Value(itab));
    jpole = mycurve->NbPoles();
    if ( jpole > 2) {
      TColgp_Array1OfPnt newP(1,jpole);      
      mycurve->Poles(newP);
      Handle(IntSurf_LineOn2S) R = new IntSurf_LineOn2S();
      Standard_Real u1,v1,u2,v2;
      u1 = v1 = 0.;
      u2 = v2 = 1.;
      for( j=1; j<=jpole; j++) {
	IntSurf_PntOn2S POn2S;
	POn2S.SetValue(newP(j),u1,v1,u2,v2);
	R->Add(POn2S);
      }
      GeomInt_WLApprox theapp3d;
      Standard_Real Tol = Precision::Approximation();
      theapp3d.SetParameters(Tol, Tol, 4, 8, 0, 30, Standard_True);
      Handle(IntPatch_WLine) WL = new IntPatch_WLine(R, Standard_False);
      Standard_Integer indicemin = 1;
      Standard_Integer indicemax = jpole;
      theapp3d.Perform(WL, Standard_True, Standard_False, 
		       Standard_False, indicemin, indicemax);
      if (!theapp3d.IsDone()) {
	modifCurve = mycurve; 
      }
      else if (theapp3d.NbMultiCurves() != 1) {
	modifCurve = mycurve;
      }
      else {
	const AppParCurves_MultiBSpCurve& mbspc = theapp3d.Value(1);
	Standard_Integer nbpoles = mbspc.NbPoles();
	TColgp_Array1OfPnt tpoles(1,nbpoles);
	mbspc.Curve(1,tpoles);
	modifCurve = new Geom_BSplineCurve(tpoles,
					   mbspc.Knots(),
					   mbspc.Multiplicities(),
					   mbspc.Degree());
      }
    }
    else {
      modifCurve = mycurve;
    }
    seq.Append(modifCurve);
  }
}

//=======================================================================
//function : ApproxBSplineCurve
//purpose  : 
//=======================================================================

void ShapeAlgo_AlgoContainer::ApproxBSplineCurve (const Handle(Geom2d_BSplineCurve)& bspline,
                                                  TColGeom2d_SequenceOfCurve& seq) const
{
  seq.Clear();
  Handle(Geom2d_BSplineCurve)  res, modifCurve;
  TColGeom2d_SequenceOfCurve SCurve;

  // si la BSpline est de degre 1 , on approxime .
  // on passe par le programme des intersections ou tout le travail
  // est deja fait !!! ( il faut faire des paquets de 30 points
  // maximum , travailler dans un espace 0,1 pour tenir la precision
  // puis reconstruire une BSpline somme des toutes les Bspline).

  if (bspline->Degree() != 1) {
    seq.Append(bspline);
    return;
  }

  // on detecte d`eventuelles cassures par la multiplicite des poles.
  // Puis on approxime chaque "partie" de BSpline et on reconstruit 
  // une BSpline = somme des BSplines traitees 

  Standard_Integer NbKnots = bspline->NbKnots();
  Standard_Integer NbPoles = bspline->NbPoles();
  TColgp_Array1OfPnt2d    Poles(1,NbPoles);
  TColStd_Array1OfReal    Weigs(1,NbPoles); Weigs.Init(1.);
  TColStd_Array1OfReal    Knots(1,NbKnots);
  TColStd_Array1OfInteger Mults(1,NbKnots); 

  bspline->Poles(Poles);
  if ( bspline->IsRational()) bspline->Weights(Weigs);
  bspline->Knots(Knots);
  bspline->Multiplicities(Mults);
  Standard_Integer deg = bspline->Degree();
  
  Standard_Integer jpole = 1;
  Standard_Integer j, PoleIndex, I1;
  PoleIndex = 1;
  I1 = 1;
  for ( Standard_Integer ipole = 1; ipole < NbPoles; ipole++) {
    if (Poles(ipole).IsEqual(Poles(ipole+1),Precision::PConfusion())) {
      if (jpole == 1) {
	PoleIndex++;
      }
      else {
	TColgp_Array1OfPnt2d    newPoles(1,jpole);
	TColStd_Array1OfReal    newWeigs(1,jpole); Weigs.Init(1.);
	Standard_Integer NbNew = jpole - deg + 1;
	TColStd_Array1OfReal    newKnots(1,NbNew);
	TColStd_Array1OfInteger newMults(1,NbNew);
	for ( j = 1; j <= NbNew; j++) {
	  newKnots(j) = Knots(I1+j-1);
	  newMults(j) = Mults(I1+j-1);
	}
	newMults(1) = newMults(NbNew) = deg+1;
	for ( j = 1; j <= jpole; j++) {
	  newWeigs(j) = Weigs(PoleIndex  );
	  newPoles(j) = Poles(PoleIndex++);
	}
	
	Handle(Geom2d_BSplineCurve) newC = new Geom2d_BSplineCurve
	  (newPoles, newWeigs, newKnots, newMults, deg);
	SCurve.Append(newC);
	I1 = ipole+1;
	jpole = 1;
      }
    }
    else {
      jpole++;
    }
  }  
    

  Handle(Geom2d_BSplineCurve) mycurve;
  Standard_Integer nbcurves = SCurve.Length();
  if (nbcurves == 0) {
    nbcurves = 1;
    SCurve.Append(bspline);
  }
  
  for (Standard_Integer itab = 1; itab <= nbcurves; itab++) { 
    mycurve = Handle(Geom2d_BSplineCurve)::DownCast(SCurve.Value(itab));
    jpole = mycurve->NbPoles();
    if ( jpole > 10) {
      TColgp_Array1OfPnt P(1,jpole);      
      TColgp_Array1OfPnt2d newP(1,jpole);      
      mycurve->Poles(newP);
      Handle(IntSurf_LineOn2S) R = new IntSurf_LineOn2S();
      Standard_Real u2,v2;
      u2 = v2 = 1.;
      for( j=1; j<=jpole; j++) {
	IntSurf_PntOn2S POn2S;
	POn2S.SetValue(P(j),newP(j).X(),newP(j).Y(),u2,v2);
	R->Add(POn2S);
      }
      GeomInt_WLApprox theapp3d;
      Standard_Real Tol = Precision::PApproximation();
      theapp3d.SetParameters(Tol, Tol, 4, 8, 0, 30, Standard_True);
      Handle(IntPatch_WLine) WL = new IntPatch_WLine(R, Standard_False);
      Standard_Integer indicemin = 1;
      Standard_Integer indicemax = jpole;
      theapp3d.Perform(WL, Standard_False, Standard_True, 
		       Standard_False, indicemin, indicemax);
      if (!theapp3d.IsDone()) {
	modifCurve = mycurve; 
      }
      else if (theapp3d.NbMultiCurves() != 1) {
	modifCurve = mycurve;
      }
      else {
	const AppParCurves_MultiBSpCurve& mbspc = theapp3d.Value(1);
	Standard_Integer nbpoles = mbspc.NbPoles();
	TColgp_Array1OfPnt2d tpoles(1,nbpoles);
	mbspc.Curve(1,tpoles);
	for( j=1; j<=jpole; j++) {	
	}
	modifCurve = new Geom2d_BSplineCurve(tpoles,
					     mbspc.Knots(),
					     mbspc.Multiplicities(),
					     mbspc.Degree());
      }
    }
    else {
      modifCurve = mycurve;
    }
    seq.Append(modifCurve);
  }
}

//=======================================================================
//function : C0ShapeToC1Shape
//purpose  : 
//=======================================================================

 TopoDS_Shape ShapeAlgo_AlgoContainer::C0ShapeToC1Shape (const TopoDS_Shape& shape,
							    const Standard_Real tol) const
{
  ShapeUpgrade_ShapeDivideContinuity sdc(shape);
  sdc.SetTolerance(tol);
  sdc.SetBoundaryCriterion(GeomAbs_C1);
  sdc.SetSurfaceCriterion(GeomAbs_C1);
  sdc.Perform();
  return sdc.Result();
}

//=======================================================================
//function : ConvertSurfaceToBSpline
//purpose  : 
//=======================================================================

 Handle(Geom_BSplineSurface) ShapeAlgo_AlgoContainer::ConvertSurfaceToBSpline(const Handle(Geom_Surface)& surf,
										 const Standard_Real UF,
										 const Standard_Real UL,
										 const Standard_Real VF,
										 const Standard_Real VL) const
{
  return ShapeConstruct::ConvertSurfaceToBSpline(surf, UF, UL, VF, VL,
						    Precision::Confusion(), GeomAbs_C1, 100,
						    Geom_BSplineSurface::MaxDegree());
}

//=======================================================================
//function : HomoWires
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeAlgo_AlgoContainer::HomoWires(const TopoDS_Wire& wireIn1,
							const TopoDS_Wire& wireIn2,
							TopoDS_Wire& wireOut1,
							TopoDS_Wire& wireOut2,
							const Standard_Boolean) const
{
  //Standard_Boolean res = Standard_False; //szv#4:S4163:12Mar99 not needed
  TopoDS_Iterator  Cook,      Perry;
  TopoDS_Edge      edge1,     edge2;
  TopLoc_Location  loc1,      loc2;
//  BRepBuilderAPI_MakeWire makeWire1, makeWire2;
  ShapeExtend_WireData makeWire1, makeWire2;
  Standard_Boolean iterCook,  iterPerry;
  Standard_Integer nEdges1,   nEdges2;
  Standard_Real    length1,   length2;
  Standard_Real    first1,    first2;
  Standard_Real    last1,     last2;
  Standard_Real    delta1,    delta2;

  Handle (Geom_Curve) crv1;
  Handle (Geom_Curve) crv2;

  //Standard_Boolean notEnd  = Standard_True; //szv#4:S4163:12Mar99 unused
  Standard_Integer nbCreatedEdges = 0;
  // gka
  //TopoDS_Vertex v11,v12,v21,v22
  Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire();
  sfw->Load(wireIn1);
  sfw->FixReorder();
  TopoDS_Wire wireIn11 = sfw->Wire() ;
  sfw->Load(wireIn2);
  sfw->FixReorder();
  TopoDS_Wire wireIn22 = sfw->Wire() ;
  
  iterCook = iterPerry = Standard_True;
  length1  = length2   = 0.;
  nEdges1  = nEdges2   = 0;
  for (Cook.Initialize(wireIn11); Cook.More(); Cook.Next()) {
    nEdges1++;
    edge1    = TopoDS::Edge(Cook.Value());
    crv1     = BRep_Tool::Curve(edge1, loc1, first1, last1);
    length1 += last1 - first1;
  }
  for (Perry.Initialize(wireIn22); Perry.More(); Perry.Next()) {
    nEdges2++;
    edge2    = TopoDS::Edge(Perry.Value());
    crv2     = BRep_Tool::Curve(edge2, loc2, first2, last2);
    length2 += last2 - first2;
  }
  Standard_Real epsilon = Precision::PConfusion() * (length1 + length2);
  if (nEdges1 == 1 && nEdges2 == 1){
    wireOut1 = wireIn11;
    wireOut2 = wireIn22;
    return Standard_True; //szv#4:S4163:12Mar99 `res=` not needed
  }

  if (length1 < epsilon) { 
    Cook.Initialize(wireIn11);
    for (Perry.Initialize(wireIn22); Perry.More(); Perry.Next()) {
      edge1  = TopoDS::Edge(Cook.Value());
      makeWire1.Add(edge1);
    }
    wireOut1 = makeWire1.Wire();
    wireOut2 = wireIn22;
    return Standard_True; //szv#4:S4163:12Mar99 `res=` not needed
  }
  if (length2 < epsilon) {
    Perry.Initialize(wireIn22);
    for (Cook.Initialize(wireIn11); Cook.More(); Cook.Next()) {
      edge2 = TopoDS::Edge(Perry.Value());
      makeWire2.Add(edge2);
    }
    wireOut1 = wireIn11;
    wireOut2 = makeWire2.Wire();
    return Standard_True; //szv#4:S4163:12Mar99 `res=` not needed
  }


  Standard_Real ratio = length2 / length1;

  Cook.Initialize(wireIn11);
  Perry.Initialize(wireIn22);
  edge1  = TopoDS::Edge(Cook.Value());
  edge2  = TopoDS::Edge(Perry.Value());
  // essai mjm du 22/05/97 
  Standard_Boolean IsToReverse1 = Standard_False;
  Standard_Boolean IsToReverse2 = Standard_False;
  if (edge1.Orientation() == TopAbs_REVERSED)
    IsToReverse1 = Standard_True;
  if (edge2.Orientation() == TopAbs_REVERSED)
    IsToReverse2 = Standard_True;
  crv1   = BRep_Tool::Curve(edge1, loc1, first1, last1);
  crv2   = BRep_Tool::Curve(edge2, loc2, first2, last2);
  delta1 = last1 - first1;
  delta2 = last2 - first2;
  while (nbCreatedEdges < (nEdges1 + nEdges2-1)) {  /*just a security. */

    if ((delta1*ratio - delta2) > epsilon) {
      BRepBuilderAPI_MakeEdge makeEdge1;
      if(!IsToReverse1) {
	makeEdge1.Init(crv1, first1, first1 + delta2/ratio);
	first1+= delta2/ratio;
      }
      else {                                               // gka BUC60685
	makeEdge1.Init(crv1, last1 - delta2/ratio , last1);
	last1 -= delta2/ratio;
      }
      BRepBuilderAPI_MakeEdge makeEdge2(crv2, first2, last2);
      edge1 = makeEdge1.Edge();
      edge2 = makeEdge2.Edge();
// essai mjm du 22/05/97
      iterCook  = Standard_False;
      //first1   += delta2/ratio;
      delta1    = last1 - first1;
      iterPerry = Standard_True;
      nbCreatedEdges++;
    }
    else if (Abs(delta1*ratio - delta2) <= epsilon) {
      BRepBuilderAPI_MakeEdge makeEdge1(crv1, first1, last1);
      BRepBuilderAPI_MakeEdge makeEdge2(crv2, first2, last2);
      edge1 = makeEdge1.Edge();
      edge2 = makeEdge2.Edge();
      iterCook  = Standard_True;
      iterPerry = Standard_True;
      nbCreatedEdges += 2;
    }
    else /*((delta1*ratio - delta2) < -epsilon)*/ {
      BRepBuilderAPI_MakeEdge makeEdge1(crv1, first1, last1);
      edge1 = makeEdge1.Edge();
      BRepBuilderAPI_MakeEdge makeEdge2;
      if(!IsToReverse2) {
	makeEdge2.Init(crv2, first2, first2 + delta1*ratio);
	first2   += delta1*ratio;
      }
      else {                                              // gka BUC60685
	makeEdge2.Init(crv2, last2 - delta1*ratio, last2);
	last2 -= delta1*ratio;
      }
      edge1 = makeEdge1.Edge();
      edge2 = makeEdge2.Edge();
      iterCook  = Standard_True;
      iterPerry = Standard_False;
      //first2   += delta1*ratio;
      delta2    = last2 - first2;
      nbCreatedEdges++;
    }
    edge1.Move(loc1);
    edge2.Move(loc2);
    if ( IsToReverse1) edge1.Reverse();
    if ( IsToReverse2) edge2.Reverse();
    makeWire1.Add(edge1);
    makeWire2.Add(edge2);

    if (iterCook && iterPerry) {
      TopoDS_Iterator Copernic = Cook;
      if (Copernic.More())
	  Copernic.Next();
      if (!Copernic.More()) {
	  wireOut1 = makeWire1.Wire();
	  wireOut2 = makeWire2.Wire();
	return Standard_True; //szv#4:S4163:12Mar99 `res=` not needed
      }
    }
    if (iterCook) {
      Cook.Next();
      edge1  = TopoDS::Edge(Cook.Value());
      if (edge1.Orientation() == TopAbs_REVERSED)
	IsToReverse1 = Standard_True;
      else IsToReverse1 = Standard_False;
      crv1   = BRep_Tool::Curve(edge1, loc1, first1, last1);
      delta1 = last1 - first1;
    }
    if (iterPerry) {
      Perry.Next();
      edge2  = TopoDS::Edge(Perry.Value());
      if (edge2.Orientation() == TopAbs_REVERSED)
	IsToReverse2 = Standard_True;
      else IsToReverse2 = Standard_False;
      crv2   = BRep_Tool::Curve(edge2, loc2, first2, last2);
      delta2 = last2 - first2;
    }

  }
  return Standard_False; //szv#4:S4163:12Mar99 `res=` not needed
}

//=======================================================================
//function : C0BSplineToSequenceOfC1BSplineCurve
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeAlgo_AlgoContainer::C0BSplineToSequenceOfC1BSplineCurve(const Handle(Geom_BSplineCurve)& BS,
									       Handle(TColGeom_HSequenceOfBoundedCurve)& seqBS) const 
{
  return ShapeUpgrade::C0BSplineToSequenceOfC1BSplineCurve (BS, seqBS);
}

//=======================================================================
//function : C0BSplineToSequenceOfC1BSplineCurve
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeAlgo_AlgoContainer::C0BSplineToSequenceOfC1BSplineCurve(const Handle(Geom2d_BSplineCurve)& BS,
									       Handle(TColGeom2d_HSequenceOfBoundedCurve)& seqBS) const
{
  return ShapeUpgrade::C0BSplineToSequenceOfC1BSplineCurve (BS, seqBS);
}

//=======================================================================
//function : OuterWire
//purpose  : 
//=======================================================================

TopoDS_Wire ShapeAlgo_AlgoContainer::OuterWire(const TopoDS_Face& face) const
{
  return ShapeAnalysis::OuterWire(face);
}

//=======================================================================
//function : ConvertToPeriodic
//purpose  : 
//=======================================================================

 Handle(Geom_Surface) ShapeAlgo_AlgoContainer::ConvertToPeriodic (const Handle(Geom_Surface)& surf) const
{
  ShapeCustom_Surface scs (surf);
  return scs.ConvertToPeriodic (Standard_False);
}

//=======================================================================
//function : GetFaceUVBounds
//purpose  : 
//=======================================================================

 void ShapeAlgo_AlgoContainer::GetFaceUVBounds (const TopoDS_Face& F,
						Standard_Real& Umin,
						Standard_Real& Umax,
						Standard_Real& Vmin,
						Standard_Real& Vmax) const
{
  ShapeAnalysis::GetFaceUVBounds (F, Umin, Umax, Vmin, Vmax);
}

//=======================================================================
//function : ConvertCurveToBSpline
//purpose  : 
//=======================================================================

Handle(Geom_BSplineCurve) ShapeAlgo_AlgoContainer::ConvertCurveToBSpline(const Handle(Geom_Curve)& C3D,
									 const Standard_Real First,
									 const Standard_Real Last,
									 const Standard_Real Tol3d,
									 const GeomAbs_Shape Continuity, 
									 const Standard_Integer MaxSegments,
									 const Standard_Integer MaxDegree) const
{
  return  ShapeConstruct::ConvertCurveToBSpline(C3D, First, Last, Tol3d, Continuity,  MaxSegments, MaxDegree);
}
