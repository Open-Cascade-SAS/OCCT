// Created on: 1996-12-16
// Created by: Bruno DUMORTIER
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


#include <AppCont_Function.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <Approx_FitAndDivide.hxx>
#include <BiTgte_Blend.hxx>
#include <BiTgte_CurveOnEdge.hxx>
#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgo_AsDes.hxx>
#include <BRepAlgo_Loop.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepOffset_DataMapIteratorOfDataMapOfShapeOffset.hxx>
#include <BRepOffset_DataMapOfShapeOffset.hxx>
#include <BRepOffset_Inter2d.hxx>
#include <BRepOffset_Inter3d.hxx>
#include <BRepOffset_Interval.hxx>
#include <BRepOffset_ListOfInterval.hxx>
#include <BRepOffset_MakeLoops.hxx>
#include <BRepOffset_Offset.hxx>
#include <BRepOffset_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepTools_Quilt.hxx>
#include <BSplCLib.hxx>
#include <ChFi3d.hxx>
#include <Convert_CompBezierCurvesToBSplineCurve.hxx>
#include <ElSLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Sphere.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>

#ifdef OCCT_DEBUG
#include <OSD_Chronometer.hxx>
#endif

#include <stdio.h>
// include - all hxx,
//         - all small static functions.
//======================== START STATIC FUNCTIONS ============
// variables for performance 
Standard_Real t_mkcurve;
#ifdef OCCT_DEBUG
extern void ChFi3d_InitChron(OSD_Chronometer& ch);
extern void ChFi3d_ResultChron(OSD_Chronometer & ch, Standard_Real&    time);
#endif
#ifdef DRAW
static Standard_Boolean Affich = Standard_False;
static char name[100];
#include <DBRep.hxx>
#endif

//=======================================================================
//function : IsOnRestriction
//purpose  : 
//=======================================================================

static Standard_Boolean IsOnRestriction(const TopoDS_Vertex& V,
					const TopoDS_Edge&   CurE,
					const TopoDS_Face&   F,
					      TopoDS_Edge&   E) 
{
  // find if Vertex V of CurE is on a restriction of F.
  // if yes, store this restriction in E.

  // dub - 03 01 97
  // Method somewhat brutal : possible to really optimize by a
  // direct call the SD of intersections -> See LBR

  Standard_Real f,l;
  Handle(Geom2d_Curve) CurC = BRep_Tool::CurveOnSurface(CurE,F,f,l);
  Standard_Real U = BRep_Tool::Parameter(V,CurE,F);
  gp_Pnt2d P = CurC->Value(U);

  Geom2dAPI_ProjectPointOnCurve Proj;
  
  // The tolerance is exaggerated : it is better to construct too many
  // tubes than to miss intersections.
  // Standard_Real Tol = 100 * BRep_Tool::Tolerance(V);
  Standard_Real Tol = BRep_Tool::Tolerance(V);
  TopExp_Explorer exp(F,TopAbs_EDGE);
  for ( ; exp.More(); exp.Next()) {
    E = TopoDS::Edge(exp.Current());
    Handle(Geom2d_Curve) PC = BRep_Tool::CurveOnSurface(E,F,f,l);
    Proj.Init(P,PC,f,l);
    if ( Proj.NbPoints() > 0) {
      if (Proj.LowerDistance() < Tol) {
	return Standard_True; 
      }
    }
  }
  return Standard_False;
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

static void Add(const TopoDS_Edge&         E,
		      TopTools_IndexedMapOfShape& Map,
		const TopoDS_Shape&        S,
		const BRepOffset_Offset&   OF,
		const BRepOffset_Analyse&  Analyse,
		const Standard_Boolean     WarningSurBordLibre)
// If WarningSurBordLibre = TRUE, no propagation if the edge is open.
{
  TopAbs_ShapeEnum Type = S.ShapeType();

  if ( Type == TopAbs_FACE) {
    TopExp_Explorer exp(S,TopAbs_EDGE);
    for ( ; exp.More(); exp.Next()) {
      const TopoDS_Edge& OriE = TopoDS::Edge(exp.Current());
      TopoDS_Shape aLocalShape = OF.Generated(OriE);
      const TopoDS_Edge& IE   = TopoDS::Edge(aLocalShape);
//      const TopoDS_Edge& IE   = TopoDS::Edge(OF.Generated(OriE));
      if ( E.IsEqual(IE)) {
	if (WarningSurBordLibre) {
	  // It is checked that the border is not free.
	  const TopTools_ListOfShape& L = Analyse.Ancestors(OriE);
	  if (L.Extent() == 1) break; // Nothing is done.
	}
	Map.Add(exp.Current());
	break;
      }
    }
  }
  else if ( Type == TopAbs_EDGE) {
    TopExp_Explorer exp(S,TopAbs_VERTEX);
    for ( ; exp.More(); exp.Next()) {
      TopoDS_Shape aLocalShape = OF.Generated(exp.Current());
      const TopoDS_Edge& IE = TopoDS::Edge(aLocalShape);
//      const TopoDS_Edge& IE = TopoDS::Edge(OF.Generated(exp.Current()));
      if ( E.IsEqual(IE)) {
	const TopTools_ListOfShape& L = Analyse.Ancestors(exp.Current());
	TopTools_ListIteratorOfListOfShape it(L);
	for ( ; it.More(); it.Next()) {
	  Map.Add(it.Value());
	}
	break;
      }
    }
  }
}


//=======================================================================
//function : IsInFace
//purpose  : 
//=======================================================================

static Standard_Boolean IsInFace(const TopoDS_Edge& E,
				 const TopoDS_Face& F)
{
  TopExp_Explorer exp(F,TopAbs_EDGE);
  for ( ;exp.More(); exp.Next()) 
    if ( E.IsSame(exp.Current())) return Standard_True;
  return Standard_False;
}


//=======================================================================
//function : KPartCurve3d
//purpose  : 
//=======================================================================

static void KPartCurve3d(TopoDS_Edge           Edge,
			 Handle(Geom2d_Curve)  Curve,
			 Handle(Geom_Surface)  Surf)
{
  // try to find the particular case
  // if not found call BRepLib::BuildCurve3d

  TopLoc_Location Loc;
  Standard_Real Tol = Precision::Confusion();

  // Seach only isos on analytical surfaces.
  Geom2dAdaptor_Curve C(Curve);
  GeomAdaptor_Surface S(Surf);
  GeomAbs_CurveType   CTy = C.GetType();
  GeomAbs_SurfaceType STy = S.GetType();
  BRep_Builder TheBuilder;

  if ( STy != GeomAbs_Plane) { // if plane buildcurve3d manage KPart
    if ( CTy == GeomAbs_Line) {
      gp_Dir2d D = C.Line().Direction();
      if ( D.IsParallel(gp::DX2d(),Precision::Angular())) { // Iso V.
	if ( STy == GeomAbs_Sphere) {
	  gp_Pnt2d  P    = C.Line().Location();
	  if ( Abs( Abs(P.Y()) -M_PI/2. ) < Precision::PConfusion()) {
	    TheBuilder.Degenerated(Edge, Standard_True);
	  }
	  else {
	    gp_Sphere Sph  = S.Sphere();
	    gp_Ax3    Axis = Sph.Position();
	    gp_Circ   Ci   = ElSLib::SphereVIso(Axis,
						Sph.Radius(),
						P.Y());
	    gp_Dir DRev = Axis.XDirection().Crossed(Axis.YDirection());
	    gp_Ax1 AxeRev(Axis.Location(), DRev);
	    Ci.Rotate(AxeRev, P.X());
	    Handle(Geom_Circle) Circle = new Geom_Circle(Ci);
	    if ( D.IsOpposite(gp::DX2d(),Precision::Angular())) 
	      Circle->Reverse();
	    TheBuilder.UpdateEdge(Edge, Circle, Loc, Tol);
	  }
	}
	else if ( STy == GeomAbs_Cylinder) {
	  gp_Cylinder Cyl  = S.Cylinder();
	  gp_Pnt2d    P    = C.Line().Location();
	  gp_Ax3      Axis = Cyl.Position();
	  gp_Circ     Ci   = ElSLib::CylinderVIso(Axis,
						  Cyl.Radius(),
						  P.Y());
	  gp_Dir DRev = Axis.XDirection().Crossed(Axis.YDirection());
	  gp_Ax1 AxeRev(Axis.Location(), DRev);
	  Ci.Rotate(AxeRev, P.X());
	  Handle(Geom_Circle) Circle = new Geom_Circle(Ci);
	  if ( D.IsOpposite(gp::DX2d(),Precision::Angular())) 
	    Circle->Reverse();
	  TheBuilder.UpdateEdge(Edge, Circle, Loc, Tol);
	}
	else if ( STy == GeomAbs_Cone) {
	  gp_Cone  Cone = S.Cone();
	  gp_Pnt2d P    = C.Line().Location();
	  gp_Ax3   Axis = Cone.Position();
	  gp_Circ  Ci   = ElSLib::ConeVIso(Axis,
					   Cone.RefRadius(),
					   Cone.SemiAngle(),
					   P.Y());
	  gp_Dir DRev = Axis.XDirection().Crossed(Axis.YDirection());
	  gp_Ax1 AxeRev(Axis.Location(), DRev);
	  Ci.Rotate(AxeRev, P.X());
	  Handle(Geom_Circle) Circle = new Geom_Circle(Ci);
	  if ( D.IsOpposite(gp::DX2d(),Precision::Angular())) 
	    Circle->Reverse();
	  TheBuilder.UpdateEdge(Edge, Circle, Loc, Tol);
	}
	else if ( STy == GeomAbs_Torus) {
	  gp_Torus Tore = S.Torus();
	  gp_Pnt2d P    = C.Line().Location();
	  gp_Ax3   Axis = Tore.Position();
	  gp_Circ  Ci   = ElSLib::TorusVIso(Axis,
					    Tore.MajorRadius(),
					    Tore.MinorRadius(),
					    P.Y());
	  gp_Dir DRev = Axis.XDirection().Crossed(Axis.YDirection());
	  gp_Ax1 AxeRev(Axis.Location(), DRev);
	  Ci.Rotate(AxeRev, P.X());
	  Handle(Geom_Circle) Circle = new Geom_Circle(Ci);
	  if ( D.IsOpposite(gp::DX2d(),Precision::Angular())) 
	    Circle->Reverse();
	  TheBuilder.UpdateEdge(Edge, Circle, Loc, Tol);
	}
      }
      else if ( D.IsParallel(gp::DY2d(),Precision::Angular())) { // Iso U.
	if ( STy == GeomAbs_Sphere) {
	  gp_Sphere Sph  = S.Sphere();
	  gp_Pnt2d  P    = C.Line().Location();
	  gp_Ax3    Axis = Sph.Position();
	  // calculate iso 0.
	  gp_Circ   Ci   = ElSLib::SphereUIso(Axis, Sph.Radius(),0.);

	  // set to sameparameter (rotation of the circle - offset from Y)
	  gp_Dir DRev = Axis.XDirection().Crossed(Axis. Direction());
	  gp_Ax1 AxeRev(Axis.Location(),DRev);
	  Ci.Rotate(AxeRev, P.Y());

	  // transformation by iso U ( = P.X())
	  DRev = Axis.XDirection().Crossed(Axis.YDirection());
	  AxeRev = gp_Ax1(Axis.Location(), DRev);
	  Ci.Rotate(AxeRev, P.X());
	  Handle(Geom_Circle) Circle = new Geom_Circle(Ci);
	  
	  if ( D.IsOpposite(gp::DY2d(),Precision::Angular())) 
	    Circle->Reverse();
	  TheBuilder.UpdateEdge(Edge, Circle, Loc, Tol);
	}
	else if ( STy == GeomAbs_Cylinder) {
	  gp_Cylinder Cyl = S.Cylinder();
	  gp_Pnt2d    P   = C.Line().Location();
	  gp_Lin      L   = ElSLib::CylinderUIso(Cyl.Position(),
						 Cyl.Radius(),
						 P.X());
	  gp_Vec Tr(L.Direction());
	  Tr.Multiply(P.Y());
	  L.Translate(Tr);
	  Handle(Geom_Line) Line = new Geom_Line(L);
	  if ( D.IsOpposite(gp::DY2d(),Precision::Angular()))
	    Line->Reverse();
	  TheBuilder.UpdateEdge(Edge, Line, Loc, Tol);
	}
	else if ( STy == GeomAbs_Cone) {
	  gp_Cone  Cone = S.Cone();
	  gp_Pnt2d P    = C.Line().Location();
	  gp_Lin   L    = ElSLib::ConeUIso(Cone.Position(),
					   Cone.RefRadius(),
					   Cone.SemiAngle(),
					   P.X());
	  gp_Vec Tr(L.Direction());
	  Tr.Multiply(P.Y());
	  L.Translate(Tr);	  Handle(Geom_Line) Line = new Geom_Line(L);
	  if ( D.IsOpposite(gp::DY2d(),Precision::Angular()))
	    Line->Reverse();
	  TheBuilder.UpdateEdge(Edge, Line, Loc, Tol);
	}
	else if ( STy == GeomAbs_Torus) {
	}
      }
    }
  }
  else { // Case Plane
    Handle(Geom_Curve) C3d = GeomAPI::To3d(Curve,S.Plane());
    TheBuilder.UpdateEdge(Edge, C3d, Loc, Tol);
  }
}


//=======================================================================
//function : MakeCurve
//purpose  : 
//=======================================================================

class MakeCurve_Function : public AppCont_Function
{
  BiTgte_CurveOnEdge myCurve;

  public :

  MakeCurve_Function(const BiTgte_CurveOnEdge& C)
  : myCurve(C)
  {
    myNbPnt = 1;
    myNbPnt2d = 0;
  }

  Standard_Real FirstParameter() const
  {
    return myCurve.FirstParameter();
  }

  Standard_Real LastParameter() const
  {
    return myCurve.LastParameter();
  }

  Standard_Boolean Value(const Standard_Real   theT,
                         NCollection_Array1<gp_Pnt2d>& /*thePnt2d*/,
                         NCollection_Array1<gp_Pnt>&   thePnt) const
  {
      thePnt(1) = myCurve.Value(theT);
      return Standard_True;
  }
  
  Standard_Boolean D1(const Standard_Real   /*theT*/,
                      NCollection_Array1<gp_Vec2d>& /*theVec2d*/,
                      NCollection_Array1<gp_Vec>&   /*theVec*/) const
  {
      return Standard_False;
  }

};

Handle(Geom_Curve) MakeCurve (const BiTgte_CurveOnEdge& HC)
{
  Handle(Geom_Curve) C;

#ifdef OCCT_DEBUG
   OSD_Chronometer ch;
   ChFi3d_InitChron(ch);
#endif

  if ( HC.GetType() == GeomAbs_Circle) {
    C = new Geom_Circle(HC.Circle());
    C = new Geom_TrimmedCurve(C,HC.FirstParameter(),HC.LastParameter());
  }
  else { // the approximation is done
    MakeCurve_Function F(HC);
    Standard_Integer Deg1, Deg2;
    Deg1 = Deg2 = 8;
    Standard_Real Tol = Precision::Approximation();
    Approx_FitAndDivide Fit(F,Deg1,Deg2,Tol,Tol,Standard_True);
    Standard_Integer i;
    Standard_Integer NbCurves = Fit.NbMultiCurves();
    // it is attempted to make the curve at least C1
    Convert_CompBezierCurvesToBSplineCurve Conv;
    
    for (i = 1; i <= NbCurves; i++) {
      AppParCurves_MultiCurve MC = Fit.Value( i);    //Load the Ith Curve
      TColgp_Array1OfPnt Poles( 1, MC.Degree() + 1); //Return poles
      MC.Curve(1, Poles);
      
      Conv.AddCurve(Poles);
    }
    
    Conv.Perform();
    
    Standard_Integer NbPoles    = Conv.NbPoles();
    Standard_Integer NbKnots    = Conv.NbKnots();
    TColgp_Array1OfPnt      NewPoles(1,NbPoles);
    TColStd_Array1OfReal    NewKnots(1,NbKnots);
    TColStd_Array1OfInteger NewMults(1,NbKnots);
    
    Conv.KnotsAndMults(NewKnots,NewMults);
    Conv.Poles(NewPoles);
    
    BSplCLib::Reparametrize(HC.FirstParameter(),
			    HC.LastParameter(),
			    NewKnots);
    
    C = new Geom_BSplineCurve (NewPoles,
			       NewKnots,
			       NewMults,
			       Conv.Degree());
  }

#ifdef OCCT_DEBUG
  ChFi3d_ResultChron(ch, t_mkcurve);
#endif

  return C;
}


//=======================================================================
//function : Touched
//purpose  : Only the faces connected with caps are given
//=======================================================================

static void Touched(const BRepOffset_Analyse&, 
                    const TopTools_MapOfShape&, 
                    const TopoDS_Shape&,
                    TopTools_MapOfShape&)
{
  // currently nothing is done !!
  /*if ( Standard_True) {
    return;
  }
  else {
    TopExp_Explorer exp(Shape, TopAbs_EDGE);
    for ( ; exp.More(); exp.Next()) {
      const TopTools_ListOfShape& L = Analyse.Ancestors(exp.Current());
      if (StopFaces.Contains(L.First())) 
        TouchedByCork.Add(L.Last());
      else if (StopFaces.Contains(L.Last())) 
        TouchedByCork.Add(L.First());
    }
  }*/
  return;
}

//=======================================================================
//function : FindVertex
//purpose  : 
//=======================================================================

static TopoDS_Vertex FindVertex(const gp_Pnt&              P,
				const TopTools_MapOfShape& Map,
				const Standard_Real        Tol)
{
  BRep_Builder B;
  // Find in <Map> a vertex which represent the point <P>.
  Standard_Real Tol2,Dist;
  TopoDS_Vertex V,VV[2];
  Standard_Real TolCarre = Tol*Tol;
  TopTools_MapIteratorOfMapOfShape it(Map);
  for ( ; it.More(); it.Next()) {
    const TopoDS_Edge& E = TopoDS::Edge(it.Key());
    if ( !E.IsNull()) {
      TopExp::Vertices(E,VV[0],VV[1]);

      for (Standard_Integer i = 0; i < 2 ; i++) {
	// if OK la Tolerance du Vertex
	Tol2 = BRep_Tool::Tolerance(VV[i]);
	Tol2 *= Tol2;
	gp_Pnt P1 = BRep_Tool::Pnt(VV[i]);
	Dist = P.SquareDistance(P1);
	if ( Dist <= Tol2) return VV[i];
	// otherwise with the required tolerance.
	if (TolCarre > Tol2) {
	  if ( Dist <= TolCarre) {
	    // so it is necessary to update the tolerance of Vertex.
	    B.UpdateVertex(VV[i],Tol);
	    return VV[i];
	  }
	}
      }
    }
  }

  return V;
}


//=======================================================================
//function : MakeDegeneratedEdge
//purpose  : 
//=======================================================================

static TopoDS_Edge MakeDegeneratedEdge(const Handle(Geom_Curve)& CC,
				       const TopoDS_Vertex&    VfOnE)
{
  BRep_Builder B;
  Standard_Real Tol = Precision::Confusion();
  // kill trimmed curves
  Handle(Geom_Curve) C = CC;
  Handle(Geom_TrimmedCurve) CT = Handle(Geom_TrimmedCurve)::DownCast(C);
  while (!CT.IsNull()) {
    C = CT->BasisCurve();
    CT = Handle(Geom_TrimmedCurve)::DownCast(C);
  }

  TopoDS_Vertex V1,V2;
  if ( VfOnE.IsNull()) {
    gp_Pnt P = C->Value(C->FirstParameter());
    B.MakeVertex(V1,P,Tol);
    V2 = V1;
  }
  else {
    V1 = V2 = VfOnE;
  }
  V1.Orientation(TopAbs_FORWARD);
  V2.Orientation(TopAbs_REVERSED);

  TopoDS_Edge E;
  B.MakeEdge(E,C,Tol);
  B.Add(E,V1); B.Add(E,V2);
//  B.UpdateVertex(V1,C->FirstParameter(),E,Tol);
//  B.UpdateVertex(V2,C->LastParameter(),E,Tol);
  B.Range(E,CC->FirstParameter(),CC->LastParameter());
  return E;
}


//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

static TopAbs_Orientation Orientation(const TopoDS_Edge& E,
				      const TopoDS_Face& F,
				      const TopTools_ListOfShape& L)
{
  TopAbs_Orientation Orien = TopAbs_FORWARD;
  TopTools_ListIteratorOfListOfShape itld;
  for ( itld.Initialize(L); itld.More(); itld.Next()) {
    if ( itld.Value().IsSame(E)) {
      Orien = itld.Value().Orientation();
      break;
    }
  }
  if ( F.Orientation() == TopAbs_REVERSED) 
    Orien = TopAbs::Reverse(Orien);
  
  return Orien;
}

//=======================================================================
//function : FindCreatedEdge
//purpose  : 
//=======================================================================

static TopoDS_Edge FindCreatedEdge
(const TopoDS_Vertex&                   V1,
 const TopoDS_Edge&                     E,
 const BRepOffset_DataMapOfShapeOffset& MapSF,
       TopTools_MapOfShape&             MapOnV,
 const BRepOffset_Analyse&              CenterAnalyse,
       Standard_Real                    Radius,
       Standard_Real                    Tol)
{
  TopoDS_Edge E1;
  if (!CenterAnalyse.HasAncestor(V1)) return E1; // return a Null Shape.

  TopTools_ListOfShape TangE;
  CenterAnalyse.TangentEdges(E,V1,TangE);

  TopTools_ListIteratorOfListOfShape itl(TangE);
  Standard_Boolean Find = Standard_False;
  for ( ; itl.More() && !Find; itl.Next()) {
    const TopoDS_Edge& ET = TopoDS::Edge(itl.Value());
    if ( MapSF.IsBound(ET)) {
      TopoDS_Shape aLocalShape = MapSF(ET).Generated(V1);
      E1 = TopoDS::Edge(aLocalShape);
//      E1 = TopoDS::Edge(MapSF(ET).Generated(V1));
      MapOnV.Add(E1);
      Find = Standard_True;
    }
    else {
      // Find the sharing of vertices in case of tangent consecutive 3 edges 
      // the second of which is the edge that degenerates the tube.
      TopLoc_Location CLoc;
      Standard_Real ff,ll;
      Handle(Geom_Curve) CET = 
	BRep_Tool::Curve(ET,CLoc,ff,ll);
      if ( CET->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve)) {
	CET = Handle(Geom_TrimmedCurve)::DownCast(CET)->BasisCurve();
      }
      Handle(Geom_Circle) Circ = Handle(Geom_Circle)::DownCast(CET);
      if ( Circ.IsNull()) continue;
      if ( Abs(Circ->Radius() - Abs(Radius)) > Tol) continue;
      
      TopoDS_Vertex U1,U2;
      TopExp::Vertices(ET,U1,U2);
      if ( U1.IsSame(V1)) U1 = U2;
      TopTools_ListOfShape Tang2;
      CenterAnalyse.TangentEdges(ET,U1,Tang2);
      TopTools_ListIteratorOfListOfShape it2(Tang2);
      for ( ; it2.More() ; it2.Next()) {
	const TopoDS_Edge& ET2 = TopoDS::Edge(it2.Value());
	if ( MapSF.IsBound(ET2)) {
	  TopoDS_Shape aLocalShape = MapSF(ET2).Generated(U1);
	  MapOnV.Add(TopoDS::Edge(aLocalShape));
//	  MapOnV.Add(TopoDS::Edge(MapSF(ET2).Generated(U1)));
	}
      }
    }
  }
  if (!Find) {
    TangE.Clear();
    //	CenterAnalyse.Edges(V1f, OT, TangE);
    if (CenterAnalyse.HasAncestor(V1)) {
      TangE = CenterAnalyse.Ancestors(V1);
      itl.Initialize(TangE);
      for ( ; itl.More() && !Find; itl.Next()) {
	if ( MapSF.IsBound(itl.Value())) {
	  MapOnV.Add(MapSF(itl.Value()).Generated(V1));
	}
      }
    }
  }
  
  return E1;
}

//=======================================================================
//function : Bubble
//purpose  : Sets in increasing order the sequence of vertices. 
//=======================================================================

static void Bubble(const TopoDS_Edge&        E,
		   TopTools_SequenceOfShape& Seq) 
{
  Standard_Boolean Invert   = Standard_True;
  Standard_Integer NbPoints = Seq.Length();
  Standard_Real    U1,U2;
  TopoDS_Vertex    V1,V2;

  while (Invert) {
    Invert = Standard_False;
    for ( Standard_Integer i = 1; i < NbPoints; i++) {
      TopoDS_Shape aLocalShape = Seq.Value(i)  .Oriented(TopAbs_INTERNAL);
      V1 = TopoDS::Vertex(aLocalShape);
      aLocalShape = Seq.Value(i+1).Oriented(TopAbs_INTERNAL);
      V2 = TopoDS::Vertex(aLocalShape);
//      V1 = TopoDS::Vertex(Seq.Value(i)  .Oriented(TopAbs_INTERNAL));
//      V2 = TopoDS::Vertex(Seq.Value(i+1).Oriented(TopAbs_INTERNAL));

      U1 = BRep_Tool::Parameter(V1,E);
      U2 = BRep_Tool::Parameter(V2,E);
      if (U2 < U1) {
	Seq.Exchange(i,i+1);
	Invert = Standard_True;
      }
    }
  }
}

//=======================================================================
//function : CutEdge
//purpose  : 
//=======================================================================

static void CutEdge (const TopoDS_Edge&          E,
		     const TopTools_ListOfShape& VOnE,
		           TopTools_ListOfShape& NE   )
{
  TopoDS_Shape aLocalShapeOrientedE = E.Oriented(TopAbs_FORWARD);
  TopoDS_Edge WE = TopoDS::Edge(aLocalShapeOrientedE);
//  TopoDS_Edge WE = TopoDS::Edge(E.Oriented(TopAbs_FORWARD));

  Standard_Real                      U1,U2;
  TopoDS_Vertex                      V1,V2;
  TopTools_SequenceOfShape           SV;
  TopTools_ListIteratorOfListOfShape it(VOnE);
  BRep_Builder                       B;

  for ( ; it.More(); it.Next()) {
    SV.Append(it.Value());
  }
  //--------------------------------
  // Parse vertices on the edge.
  //--------------------------------
  Bubble (WE,SV);

  Standard_Integer NbVer = SV.Length();
  //----------------------------------------------------------------
  // Construction of new edges.
  // The vertices at the extremities of edges are not 
  // necessarily in the list of vertices
  //----------------------------------------------------------------
  if (SV.IsEmpty()) {
    NE.Append(E);
    return;
  }
  TopoDS_Vertex    VF,VL;
  Standard_Real    f,l;
  BRep_Tool::Range(WE,f,l);
  TopExp::Vertices(WE,VF,VL);

  if (NbVer == 2) {
    if (SV(1).IsEqual(VF) && SV(2).IsEqual(VL)) {
      NE.Append(E);
      return;
    }
  }
  //----------------------------------------------------
  // Processing of closed edges 
  // If a vertex of intersection is on the common vertex,
  // it should appear at the beginning and the end of SV.
  //----------------------------------------------------
  TopoDS_Vertex VCEI;

  if (!VF.IsNull() && !VF.IsSame(SV.First())) SV.Prepend(VF);
  if (!VL.IsNull() && !VL.IsSame(SV.Last ())) SV.Append (VL);

  V1  = TopoDS::Vertex(SV.First());
  SV.Remove(1);

  while (!SV.IsEmpty()) {
    
    V2  = TopoDS::Vertex(SV.First());
    SV.Remove(1);
    
    if ( V1.IsSame(V2)) {
      cout << "Vertex Confondus dans CutEdges" << endl;
      continue;
    }
    //-------------------------------------------
    // Copy the edge and restriction by V1 V2.
    //-------------------------------------------
    TopoDS_Shape aLocalShape =WE.EmptyCopied();
    TopoDS_Edge NewEdge = TopoDS::Edge(aLocalShape);
//    TopoDS_Edge NewEdge = TopoDS::Edge(WE.EmptyCopied());
    B.Add  (NewEdge,V1.Oriented(TopAbs_FORWARD));
    B.Add  (NewEdge,V2.Oriented(TopAbs_REVERSED));
    if (V1.IsSame(VF)) 
      U1 = f;
    else {
      aLocalShape = V1.Oriented(TopAbs_INTERNAL);
      U1 = BRep_Tool::Parameter(TopoDS::Vertex(aLocalShape),WE);
//      U1 = BRep_Tool::Parameter
//	(TopoDS::Vertex(V1.Oriented(TopAbs_INTERNAL)),WE);
    }
    if (V2.IsSame(VL))
      U2 = l;
    else {
      aLocalShape = V2.Oriented(TopAbs_INTERNAL);
      U2 = BRep_Tool::Parameter(TopoDS::Vertex(aLocalShape),WE);
//      U2 = BRep_Tool::Parameter
//	(TopoDS::Vertex(V2.Oriented(TopAbs_INTERNAL)),WE);
    }
    B.Range (NewEdge,U1,U2);
    NE.Append(NewEdge.Oriented(E.Orientation()));

    V1 = V2;
  }
}
//======================== END OF STATIC FUNCTIONS ============




//=======================================================================
//function : BiTgte_Blend
//purpose  : 
//=======================================================================

BiTgte_Blend::BiTgte_Blend()
{
  myAsDes = new BRepAlgo_AsDes();
  myNbBranches = -1;
}


//=======================================================================
//function : BiTgte_Blend
//purpose  : 
//=======================================================================

BiTgte_Blend::BiTgte_Blend(const TopoDS_Shape&    S,
			   const Standard_Real    Radius,
			   const Standard_Real    Tol,
			   const Standard_Boolean NUBS)
{
  myAsDes = new BRepAlgo_AsDes();
  Init(S,Radius,Tol,NUBS);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BiTgte_Blend::Init(const TopoDS_Shape&    S,
			const Standard_Real    Radius,
			const Standard_Real    Tol,
			const Standard_Boolean NUBS) 
{
  Clear();
  myShape      = S;
  myTol        = Tol;
  myNubs       = NUBS;
  myRadius     = Radius;
  myNbBranches = -1;
//  TopExp::MapShapesAndAncestors(S,TopAbs_EDGE,TopAbs_FACE,myAncestors);
}


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void BiTgte_Blend::Clear() 
{
  myInitOffsetFace.Clear();
  myImage         .Clear();
  myImageOffset   .Clear();
  myStopFaces     .Clear();  
  myAnalyse       .Clear();
  myAsDes        ->Clear();
  myNbBranches   = -1;
  myDone         = Standard_False;
}


//=======================================================================
//function : SetStoppingFace
//purpose  : 
//=======================================================================

void BiTgte_Blend::SetStoppingFace(const TopoDS_Face& Face) 
{
  myStopFaces.Add(Face);    
  //-------------
  // MAJ SD. -> To end loop, set faces of edges
  //-------------
//  myInitOffsetFace.SetRoot(Face);    
//  myInitOffsetFace.Bind   (Face,Face);
//  myImageOffset.SetRoot   (Face);  
}


//=======================================================================
//function : SetFaces
//purpose  : 
//=======================================================================

void BiTgte_Blend::SetFaces(const TopoDS_Face& F1,const TopoDS_Face& F2) 
{
  myFaces.Add(F1);
  myFaces.Add(F2);
}


//=======================================================================
//function : SetEdge
//purpose  : 
//=======================================================================

void BiTgte_Blend::SetEdge(const TopoDS_Edge& Edge) 
{
  myEdges.Add(Edge);
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BiTgte_Blend::Perform(const Standard_Boolean BuildShape) 
{
  myBuildShape = BuildShape;

  // Try cutting to avoid tubes on free borders
  // that are not actually free.
  Handle(BRepBuilderAPI_Sewing) Sew = new BRepBuilderAPI_Sewing(myTol);
  BRepLib::BuildCurves3d(myShape);
  TopExp_Explorer expf(myShape,TopAbs_FACE);
  for ( ;expf.More(); expf.Next()) Sew->Add(expf.Current());
  Sew->Perform();
  TopoDS_Shape SewedShape = Sew->SewedShape();
  if ( SewedShape.IsNull()) throw Standard_Failure("Sewing aux fraises");

  // Check if the sewing modified the orientation.
  expf.Init(myShape,TopAbs_FACE);
  TopoDS_Face FaceRef = TopoDS::Face(expf.Current());
  TopAbs_Orientation OriRef  = FaceRef.Orientation();
  if (Sew->IsModified(FaceRef)) FaceRef = TopoDS::Face(Sew->Modified(FaceRef));
  expf.Init(SewedShape, TopAbs_FACE);
  for (; expf.More(); expf.Next()) {
    const TopoDS_Face& FF = TopoDS::Face(expf.Current());
    if (FaceRef.IsSame(FF) && (FF.Orientation() != OriRef)) {
      SewedShape.Reverse();
      break;
    }
  } 
  
  // Make SameParameter if Sew does not do it (Detect that edges
  // are not sameparameter but it does nothing.)
  expf.Init(SewedShape, TopAbs_EDGE);
  for (; expf.More(); expf.Next()) {
    const TopoDS_Edge& sec = TopoDS::Edge(expf.Current());
    BRepLib::SameParameter(sec, BRep_Tool::Tolerance(sec));
  }
  
  TopExp::MapShapesAndAncestors
    (SewedShape,TopAbs_EDGE,TopAbs_FACE,myAncestors);
  
  // Extend myFaces with the faces of the sewed shape.
  expf.Init(myShape,TopAbs_FACE);
  for ( ; expf.More(); expf.Next()) {
    const TopoDS_Shape& F = expf.Current();
    if ( myFaces.Contains(F) && Sew->IsModified(F)) {
      myFaces.RemoveKey(F);
      myFaces.Add(Sew->Modified(F));
    }
  }
  
  myShape = SewedShape;
// end Sewing for false free borders.

#ifdef OCCT_DEBUG
  OSD_Chronometer cl_total, ch;
  Standard_Real   t_total, t_center, t_surface, t_shape;

  t_total=0; t_center=0; t_surface=0; t_mkcurve=0; t_shape=0;
  ChFi3d_InitChron(cl_total); 
#endif  

  // ----------------------------------------------------------------
  // place faces with the proper orientation in the initial shape
  // ----------------------------------------------------------------
  TopExp_Explorer exp(myShape,TopAbs_FACE);
  for ( ; exp.More(); exp.Next()) {
    const TopoDS_Shape& F = exp.Current();
    if ( myFaces.Contains(F)) {
      myFaces.RemoveKey(F);
      myFaces.Add(F);
    }
    else if ( myStopFaces.Contains(F)) {
      myStopFaces.Remove(F);
      myStopFaces.Add(F);
    }
  }

  // ----------------------------------------------
  // Calculate lines of centers and of surfaces
  // ----------------------------------------------
#ifdef OCCT_DEBUG
  ChFi3d_InitChron(ch);
#endif  

  ComputeCenters();
  
#ifdef OCCT_DEBUG
  ChFi3d_ResultChron(ch, t_center);
#endif 

  // -----------------------------
  // Calculate connection Surfaces
  // -----------------------------
#ifdef OCCT_DEBUG
  ChFi3d_InitChron(ch);
#endif

  ComputeSurfaces();

#ifdef OCCT_DEBUG
  ChFi3d_ResultChron(ch, t_surface);
#endif 

  // ----------------------------------
  // Calculate the generated shape if required
  // ----------------------------------
#ifdef OCCT_DEBUG
  ChFi3d_InitChron(ch);
#endif  

  if ( myBuildShape) ComputeShape();

#ifdef OCCT_DEBUG
  ChFi3d_ResultChron(ch, t_shape);
#endif 

  // Finally construct curves 3d from edges to be transfered
  // since the partition is provided ( A Priori);
  BRepLib::BuildCurves3d(myResult, Precision::Confusion());

#ifdef OCCT_DEBUG
  ChFi3d_ResultChron(cl_total, t_total);
  cout<<endl; 
  cout<<"Blend_PERFORM: temps total "<<t_total<<" s  dont :"<<endl;
  cout<<"- ComputeCenters  "<<t_center<<" s"<<endl;  
  cout<<"- ComputeSurfaces "<<t_surface<<" s"<<endl;
  cout<<"----> MakeCurve   "<<t_mkcurve<<" s"<<endl;
  if ( myBuildShape)  cout<<"- ComputeShape "<<t_shape<<" s"<<endl;
#endif

  myDone = Standard_True;
}


//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BiTgte_Blend::IsDone() const
{
  return myDone;
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopoDS_Shape& BiTgte_Blend::Shape() const
{
  return myResult;
}


//=======================================================================
//function : NbSurfaces
//purpose  : 
//=======================================================================

Standard_Integer BiTgte_Blend::NbSurfaces() const
{
  return myCenters.Extent();
}


//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

Handle(Geom_Surface) BiTgte_Blend::Surface(const Standard_Integer Index) const
{
  return Surface(myCenters(Index));
}

//=======================================================================
//function : TopoDS_Face&
//purpose  : 
//=======================================================================

const TopoDS_Face& BiTgte_Blend::Face(const Standard_Integer Index) const
{
  return Face(myCenters(Index));
}



//=======================================================================
//function : CenterLines
//purpose  : 
//=======================================================================

void BiTgte_Blend::CenterLines(TopTools_ListOfShape& LC) const
{
  LC.Clear();
  Standard_Integer Nb = NbSurfaces();
  for ( Standard_Integer i = 1; i <= Nb; i++) 
    LC.Append(myCenters(i));
}


//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

Handle(Geom_Surface) BiTgte_Blend::Surface(const TopoDS_Shape& CenterLine) 
const
{ 
  const TopoDS_Face& F = myMapSF(CenterLine).Face();
  return BRep_Tool::Surface(F);
}

//=======================================================================
//function : TopoDS_Face&
//purpose  : 
//=======================================================================

const TopoDS_Face& BiTgte_Blend::Face(const TopoDS_Shape& CenterLine) const
{
  if ( !myMapSF.IsBound(CenterLine)) {
    throw Standard_DomainError("BiTgte_Blend::Face");
  }

  return myMapSF(CenterLine).Face();
}

//=======================================================================
//function : ContactType
//purpose  : 
//=======================================================================

BiTgte_ContactType BiTgte_Blend::ContactType(const Standard_Integer Index)
     const
{
  const TopoDS_Shape& S1 = SupportShape1(Index);
  const TopoDS_Shape& S2 = SupportShape2(Index);

  TopAbs_ShapeEnum Type1 = S1.ShapeType();
  TopAbs_ShapeEnum Type2 = S2.ShapeType();
  
  if (Type2 < Type1) {
    TopAbs_ShapeEnum Dummy = Type1;
    Type1 = Type2;
    Type2 = Dummy;
  }
  BiTgte_ContactType Type = BiTgte_VertexVertex;

  switch (Type1) {
    
  case TopAbs_VERTEX:
    switch (Type2) {
    case TopAbs_VERTEX: Type = BiTgte_VertexVertex; break;
    case TopAbs_EDGE:   Type = BiTgte_EdgeVertex;   break;
    case TopAbs_FACE:   Type = BiTgte_FaceVertex;   break;
    default:
      break;
    }
    
  case TopAbs_EDGE:
    switch (Type2) {
    case TopAbs_EDGE:   Type = BiTgte_EdgeEdge; break;
    case TopAbs_FACE:   Type = BiTgte_FaceEdge; break;
    default:
      break;
   }
    
  case TopAbs_FACE:
    switch (Type2) {
    case TopAbs_FACE:   Type = BiTgte_FaceEdge; break;
    default:
      break;
    }
  default:
    break;
  }
  
  return Type;
}



//=======================================================================
//function : SupportShape1
//purpose  : 
//=======================================================================

const TopoDS_Shape& BiTgte_Blend::SupportShape1(const Standard_Integer Index)
     const
{
  const TopoDS_Edge& CurE = TopoDS::Edge(myCenters(Index));
  
  const TopTools_ListOfShape& L = myAsDes->Ascendant(CurE);
    
  // --------------------------------------------------------------
  // F1 and F2 = 2 parallel faces intersecting at CurE.
  // --------------------------------------------------------------
  const TopoDS_Face& F1 = TopoDS::Face(L.First()); 
  const TopoDS_Shape& Or1 = myInitOffsetFace.ImageFrom(F1);
  return Or1;
}


//=======================================================================
//function : SupportShape2
//purpose  : 
//=======================================================================

const TopoDS_Shape& BiTgte_Blend::SupportShape2(const Standard_Integer Index)
     const
{
  const TopoDS_Edge& CurE = TopoDS::Edge(myCenters(Index));
  
  const TopTools_ListOfShape& L = myAsDes->Ascendant(CurE);
    
  // --------------------------------------------------------------
  // F1 and F2 = 2 parallel faces intersecting at CurE.
  // --------------------------------------------------------------
  const TopoDS_Face& F2 = TopoDS::Face(L.Last()); 
  const TopoDS_Shape& Or2 = myInitOffsetFace.ImageFrom(F2);
  return Or2;
}


//=======================================================================
//function : CurveOnShape1
//purpose  : 
//=======================================================================

Handle(Geom_Curve) BiTgte_Blend::CurveOnShape1
(const Standard_Integer Index) const
{
  const TopoDS_Edge& CurE = TopoDS::Edge(myCenters(Index));
  const TopoDS_Shape& F = myMapSF(CurE).Face();

  // somewhat brutal method based ONLY on the construction of the fillet:
  // the first edge of the tube is exactly the edge on Shape1.

  TopExp_Explorer exp(F,TopAbs_EDGE);
  const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
  Handle(Geom_Curve) C;
  if ( !BRep_Tool::Degenerated(E)) { 
    Standard_Real f,l;
    C = BRep_Tool::Curve(E,f,l);
    C = new Geom_TrimmedCurve(C,f,l);
  }
  return C;
}


//=======================================================================
//function : CurveOnShape2
//purpose  : 
//=======================================================================

Handle(Geom_Curve) BiTgte_Blend::CurveOnShape2
(const Standard_Integer Index) const
{
  const TopoDS_Edge& CurE = TopoDS::Edge(myCenters(Index));
  const TopoDS_Shape& F = myMapSF(CurE).Face();

  // somewhat brutal method based ONLY on the construction of the fillet:
  // the first edge of the tube is exactly the edge on Shape2.

  TopExp_Explorer exp(F,TopAbs_EDGE);
  exp.Next();
  const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
  Handle(Geom_Curve) C;
  if ( !BRep_Tool::Degenerated(E)) { 
    Standard_Real f,l;
    C = BRep_Tool::Curve(E,f,l);
    C = new Geom_TrimmedCurve(C,f,l);
  }
  return C;
}


//=======================================================================
//function : PCurveOnFace1
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve) BiTgte_Blend::PCurveOnFace1
(const Standard_Integer /*Index*/) const
{
  Handle(Geom2d_Curve) C;
  return C;
}


//=======================================================================
//function : PCurve1OnFillet
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve) BiTgte_Blend::PCurve1OnFillet
(const Standard_Integer /*Index*/) const
{
  Handle(Geom2d_Curve) C;
  return C;
}


//=======================================================================
//function : PCurveOnFace2
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve) BiTgte_Blend::PCurveOnFace2
(const Standard_Integer /*Index*/) const
{
  Handle(Geom2d_Curve) C;
  return C;
}


//=======================================================================
//function : Curve2OnFillet
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve) BiTgte_Blend::PCurve2OnFillet
(const Standard_Integer /*Index*/) const
{
  Handle(Geom2d_Curve) C;
  return C;
}



//=======================================================================
//function : NbBranches
//purpose  : 
//=======================================================================

Standard_Integer BiTgte_Blend::NbBranches() 
{
  if (myNbBranches != -1) return myNbBranches;

  // else, compute the Branches.
  BRepTools_Quilt Glue;
  
  Standard_Integer NbFaces = myCenters.Extent();


  if (NbFaces == 0) return 0;


  Standard_Integer i;
  for ( i = 1; i <= NbFaces; i++) {
    const TopoDS_Shape& CenterLine = myCenters(i);
    Glue.Add(myMapSF(CenterLine).Face()); 
  }

  const TopoDS_Shape Shells = Glue.Shells();


  // Reorder Map myCenters.
  // The method is brutal and unpolished, 
  // it is possible to refine it.
  myNbBranches = 0;
  TopTools_IndexedMapOfShape tmpMap;

  TopExp_Explorer exp(Shells,TopAbs_SHELL);
  for (; exp.More(); exp.Next()) {
    myNbBranches++;
  }

  myIndices = new TColStd_HArray1OfInteger(1,myNbBranches+1);

  myIndices->SetValue(1,0);
  Standard_Integer Count = 0;
  Standard_Integer Index = 2;
  

  exp.Init(Shells,TopAbs_SHELL);
  for (; exp.More(); exp.Next()) {
    // CurS = the current Shell.
    const TopoDS_Shape CurS = exp.Current();

    TopExp_Explorer exp2(CurS, TopAbs_FACE);
    for (; exp2.More(); exp2.Next()) {
      // CurF = the current face of the current Shell.
      const TopoDS_Shape CurF = exp2.Current();

      for ( i = 1; i <= NbFaces; i++) {
	const TopoDS_Shape& Center = myCenters(i);
	const TopoDS_Shape& Rakk   = myMapSF(Center).Face();
	// Rakk = the ith generated connection face
	if (CurF.IsEqual(Rakk)) {
	  tmpMap.Add(Center);
	  Count++;
	  break;
	}
      }
    }
    myIndices->SetValue(Index, Count);
    Index++;
  }
  
  myCenters = tmpMap;
  return myNbBranches;
}


//=======================================================================
//function : IndicesOfBranche
//purpose  : 
//=======================================================================

void BiTgte_Blend::IndicesOfBranche
(const Standard_Integer  Index,
       Standard_Integer& From,
       Standard_Integer& To   ) const 
{
  // Attention to the ranking in myIndices:
  // If the branches are  1-4 5-9 10-12, it is ranked in myIndices:
  //                      0 4   9    12
  From = myIndices->Value(Index) + 1;
  To   = myIndices->Value(Index + 1);
}


//=======================================================================
//function : ComputeCenters
//purpose  : 
//=======================================================================

void BiTgte_Blend::ComputeCenters() 
{
  // ------------
  // Preanalyze.
  // ------------
  Standard_Real TolAngle = 2*ASin(myTol/Abs(myRadius*0.5));
  myAnalyse.Perform(myShape,TolAngle);

  // ------------------------------------------
  // calculate faces touched by caps
  // ------------------------------------------
  TopTools_MapOfShape TouchedByCork;
  Touched(myAnalyse, myStopFaces, myShape, TouchedByCork);

  // -----------------------
  // init of the intersector
  // -----------------------
  TopAbs_State       Side = TopAbs_IN;
  if (myRadius < 0.) Side = TopAbs_OUT;
  BRepOffset_Inter3d Inter(myAsDes,Side,myTol);

  TopTools_DataMapOfShapeBox         MapSBox;
  TopTools_MapOfShape              Done;
  //TopTools_MapIteratorOfMapOfShape it;

  BRep_Builder B;
  TopoDS_Compound Co; // to only know on which edges the tubes are made
  B.MakeCompound(Co);

  // ----------------------------------------
  // Calculate Sections Face/Face + Propagation
  // ----------------------------------------
  Standard_Boolean JenRajoute = Standard_True;
  Standard_Integer i;
  
  while ( JenRajoute) {
    JenRajoute = Standard_False;

    Standard_Boolean Fini = Standard_False;

    TopTools_DataMapOfShapeShape EdgeTgt;
    
    while ( !Fini) {

      // -------------------------------------------------
      // locate in myFaces the Faces connected to myEdges.
      // -------------------------------------------------
      Fini = Standard_True;
      //for (it.Initialize(myEdges); it.More(); it.Next()) {
      for (i = 1; i <= myEdges.Extent(); i++) {
	const TopoDS_Edge& E = TopoDS::Edge(myEdges(i));
	if (BRep_Tool::Degenerated(E)) continue;

	const TopTools_ListOfShape& L = myAncestors.FindFromKey(E);
	if ( L.Extent() == 1) {
	  // So this is a free border onwhich the ball should roll.
	  myFaces.Add(E);
	  
	  // set in myStopFaces to not propagate the tube on free border.
	  myStopFaces.Add(E);
	}
	else {
	  TopTools_ListIteratorOfListOfShape itl;
	  for (itl.Initialize(L); itl.More(); itl.Next()) {
	    const TopoDS_Shape& Sh = itl.Value();
	    if ( !myStopFaces.Contains(Sh)) myFaces.Add(itl.Value());
	  }
	}
      }
      myEdges.Clear();
      
      // --------------------------------------------
      // Construction of Offsets of all faces.
      // --------------------------------------------
      //for (it.Initialize(myFaces); it.More(); it.Next()) {
      for (i = 1; i <= myFaces.Extent(); i++) {
	const TopoDS_Shape& AS = myFaces(i);
	if ( myMapSF.IsBound(AS)) continue;

	BRepOffset_Offset OF1;
	TopoDS_Face BigF;

	if (AS.ShapeType() == TopAbs_FACE) {
	  const TopoDS_Face& F = TopoDS::Face(myFaces(i));
	  if ( TouchedByCork.Contains(F)) {
	    BRepOffset_Tool::EnLargeFace(F,BigF,Standard_True);
	    OF1.Init(BigF,myRadius,EdgeTgt);
	  }
	  else {
	    OF1.Init(F,myRadius,EdgeTgt);
	  }
	}
	else { // So this is a Free Border edge on which the ball rolls.
	  OF1.Init(TopoDS::Edge(AS),myRadius);
	}

	// ------------------------------------
	// Increment the map of created tangents
	// ------------------------------------
	TopTools_ListOfShape Let;
	if ( AS.ShapeType() == TopAbs_FACE) { 
	  myAnalyse.Edges(TopoDS::Face(AS),BRepOffset_Tangent,Let);
	}
	TopTools_ListIteratorOfListOfShape itlet(Let);
    
	for ( ; itlet.More(); itlet.Next()) {
	  const TopoDS_Edge& Cur = TopoDS::Edge(itlet.Value());
	  if ( !EdgeTgt.IsBound(Cur)) {
	    TopoDS_Shape aLocalShape = OF1.Generated(Cur);
	    const TopoDS_Edge& OTE = TopoDS::Edge(aLocalShape);
//	    const TopoDS_Edge& OTE = TopoDS::Edge(OF1.Generated(Cur));
	    EdgeTgt.Bind(Cur,OF1.Generated(Cur));
	    TopoDS_Vertex V1,V2,OV1,OV2;
	    TopExp::Vertices (Cur,V1,V2);
	    TopExp::Vertices (OTE,OV1,OV2);      
	    TopTools_ListOfShape LE;
	    if (!EdgeTgt.IsBound(V1)) {
	      myAnalyse.Edges(V1,BRepOffset_Tangent,LE);
	      const TopTools_ListOfShape& LA = myAnalyse.Ancestors(V1);
	      if (LE.Extent() == LA.Extent())
		EdgeTgt.Bind(V1,OV1);
	    }
	    if (!EdgeTgt.IsBound(V2)) {
	      LE.Clear();
	      myAnalyse.Edges(V2,BRepOffset_Tangent,LE);
	      const TopTools_ListOfShape& LA = myAnalyse.Ancestors(V2);
	      if (LE.Extent() == LA.Extent())
		EdgeTgt.Bind(V2,OV2);
	    }
	  }
	}
	// end of map created tangent

	if (OF1.Status() == BRepOffset_Reversed ||
	    OF1.Status() == BRepOffset_Degenerated ) continue;

	const TopoDS_Face& F1 = OF1.Face();

	// increment S D
	myInitOffsetFace.SetRoot(AS);
	myInitOffsetFace.Bind(AS,F1);

	Bnd_Box Box1;
	BRepBndLib::Add(F1,Box1);
	MapSBox.Bind(F1,Box1);

	// ---------------------------------------------
	// intersection with all already created faces.
	// ---------------------------------------------
	Fini = !Intersect(AS,F1,MapSBox,OF1,Inter);

	if (AS.ShapeType() == TopAbs_FACE) B.Add(Co,AS);

	myMapSF.Bind(AS, OF1);

      }
    } // end of : while ( !Fini)


    //--------------------------------------------------------
    // so the offsets were created and intersected.
    // now the tubes are constructed.
    //--------------------------------------------------------
    // Construction of tubes on edge.
    //--------------------------------------------------------
    BRepOffset_Type    OT = BRepOffset_Convex;
    if (myRadius < 0.) OT = BRepOffset_Concave; 
    
    TopTools_IndexedDataMapOfShapeListOfShape Map;
    TopExp::MapShapesAndAncestors(Co,TopAbs_EDGE,TopAbs_FACE,Map);
    TopExp::MapShapesAndAncestors(Co,TopAbs_VERTEX,TopAbs_EDGE,Map);
    
    TopExp_Explorer exp(Co,TopAbs_EDGE);
    for ( ; exp.More(); exp.Next()) {
      const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
      if ( myMapSF.IsBound(E)) continue;
      
      const TopTools_ListOfShape& Anc = Map.FindFromKey(E);
      if (Anc.Extent() == 2) {
	const BRepOffset_ListOfInterval& L = myAnalyse.Type(E);
	if (!L.IsEmpty() && L.First().Type() == OT) {
	  TopoDS_Shape aLocalShapeGen = myMapSF(Anc.First()).Generated(E);
	  TopoDS_Edge EOn1 = TopoDS::Edge(aLocalShapeGen);
    aLocalShapeGen = myMapSF(Anc.Last()) .Generated(E);
	  TopoDS_Edge EOn2 = TopoDS::Edge(aLocalShapeGen);
//	  TopoDS_Edge EOn1 = TopoDS::Edge(myMapSF(Anc.First()).Generated(E));
//	  TopoDS_Edge EOn2 = TopoDS::Edge(myMapSF(Anc.Last()) .Generated(E));
	  // find if exits tangent edges in the original shape
	  TopoDS_Edge E1f, E1l;
	  TopoDS_Vertex V1f, V1l;
	  TopExp::Vertices(E,V1f,V1l);
	  TopTools_ListOfShape TangE;
	  myAnalyse.TangentEdges(E,V1f,TangE);
	  // find if the pipe on the tangent edges are soon created.
	  TopTools_ListIteratorOfListOfShape itl(TangE);
	  Standard_Boolean Find = Standard_False;
	  for ( ; itl.More() && !Find; itl.Next()) {
	    if ( myMapSF.IsBound(itl.Value())) {
	      TopoDS_Shape aLocalShape = myMapSF(itl.Value()).Generated(V1f);
	      E1f  = TopoDS::Edge(aLocalShape);
//	      E1f  = TopoDS::Edge(myMapSF(itl.Value()).Generated(V1f));
	      Find = Standard_True;
	    }
	  }
	  TangE.Clear();
	  myAnalyse.TangentEdges(E,V1l,TangE);
	  // find if the pipe on the tangent edges are soon created.
	  itl.Initialize(TangE);
	  Find = Standard_False;
	  for ( ; itl.More() && !Find; itl.Next()) {
	    if ( myMapSF.IsBound(itl.Value())) {
	      TopoDS_Shape aLocalShape = myMapSF(itl.Value()).Generated(V1l);
	      E1l  = TopoDS::Edge(aLocalShape);
//	      E1l  = TopoDS::Edge(myMapSF(itl.Value()).Generated(V1l));
	      Find = Standard_True;
	    }
	  }
	  BRepOffset_Offset OF1 (E,EOn1,EOn2,myRadius,E1f, E1l);
	  const TopoDS_Face& F1 = OF1.Face();

	  // maj S D
	  myInitOffsetFace.SetRoot(E);
	  myInitOffsetFace.Bind(E,F1);

	  Bnd_Box Box1;
	  BRepBndLib::Add(F1,Box1);
	  MapSBox.Bind(F1,Box1);

	  // ---------------------------------------------
	  // intersection with all already created faces.
	  // ---------------------------------------------
	  Standard_Boolean IsOnRest = Intersect(E,F1,MapSBox,OF1,Inter);
	  JenRajoute = JenRajoute || IsOnRest;

	  myMapSF.Bind(E,OF1);
	}
      }
    }
    
  } // end while JenRajoute
  

  myEdges.Clear();
  myEdges = Inter.NewEdges();

  // -------------------------------------------------------------------
  // now it is necessary to limit edges on the neighbors (otherwise one 
  // will go too far and will not be able to construct faces).
  // -------------------------------------------------------------------

  // Proceed with MakeLoops 
  TopTools_IndexedDataMapOfShapeListOfShape aDMVV;
  BRepOffset_Type    OT = BRepOffset_Concave;
  if (myRadius < 0.) OT = BRepOffset_Convex; 
   
  TopTools_ListOfShape LOF;
  //it.Initialize(myFaces);
  for (i = 1; i <= myFaces.Extent(); i++) {
    const TopoDS_Shape& CurS  = myFaces(i);

    // tube on free border, it is undesirable.
    if ( myStopFaces.Contains(CurS)) continue;

    if ( !myMapSF.IsBound(CurS)) continue; // inverted or degenerated

    const TopoDS_Face& CurOF = myMapSF(CurS).Face();
    LOF.Append(CurOF);

    if (CurS.ShapeType() == TopAbs_FACE) {
      const TopoDS_Face& CurF = TopoDS::Face(CurS);
      TopExp_Explorer expe(CurF.Oriented(TopAbs_FORWARD),TopAbs_EDGE);
      for (; expe.More(); expe.Next()) {
	// --------------------------------------------------------------
	// set in myAsDes the edges generated by limitations of the 
	// initial square if the type is correct (The edges that will 
	// disappear are not set)
	// --------------------------------------------------------------
	const TopoDS_Edge& CurE = TopoDS::Edge(expe.Current());
	const BRepOffset_ListOfInterval& L = myAnalyse.Type(CurE);
	if (!L.IsEmpty() && L.First().Type() != OT) {
	  // a priori doe s not disappear, so it is set
	  TopoDS_Shape aLocalShape = myMapSF(CurF).Generated(CurE);
	  const TopoDS_Edge& CurOE = TopoDS::Edge(aLocalShape);
//	  const TopoDS_Edge& CurOE = 
//	    TopoDS::Edge(myMapSF(CurF).Generated(CurE));
	  myAsDes->Add(CurOF,CurOE.Oriented(CurE.Orientation()));
	}
	else {
	  const TopTools_ListOfShape& Lanc = myAnalyse.Ancestors(CurE);
	  if (   !myFaces    .Contains(Lanc.First()) 
	      || !myFaces    .Contains(Lanc.Last ())
	      ||  myStopFaces.Contains(Lanc.First()) 
	      ||  myStopFaces.Contains(Lanc.Last ())) {
	    TopoDS_Shape aLocalShape = myMapSF(CurF).Generated(CurE);
	    const TopoDS_Edge& CurOE = TopoDS::Edge(aLocalShape);
//	    const TopoDS_Edge& CurOE = 
//	      TopoDS::Edge(myMapSF(CurF).Generated(CurE));
	    myAsDes->Add(CurOF,CurOE.Oriented(CurE.Orientation()));
	  }
	}
      }
      BRepOffset_Inter2d::Compute(myAsDes,
				  CurOF,
				  myEdges,
				  myTol,
				  aDMVV);
    }
  }

  // ----------------------------------------------------------------
  // It is also required to make 2D intersections with generated tubes
  // (Useful for unwinding)
  // ----------------------------------------------------------------
  BRepOffset_DataMapIteratorOfDataMapOfShapeOffset It(myMapSF);
  for ( ; It.More(); It.Next()) {
    const TopoDS_Shape& CurS = It.Key();
    if ( CurS.ShapeType() == TopAbs_FACE) continue;

    const TopoDS_Face& CurOF = It.Value().Face();

    // no unwinding by tubes on free border.
    if ( myStopFaces.Contains(CurS)) continue;

    LOF.Append(CurOF);

    // --------------------------------------------------------------
    // set in myAsDes the edge restrictions of the square
    // --------------------------------------------------------------
    TopExp_Explorer expe(CurOF.Oriented(TopAbs_FORWARD),TopAbs_EDGE);
    for (; expe.More(); expe.Next()) {
      const TopoDS_Edge& CurOE = TopoDS::Edge(expe.Current());
      myAsDes->Add(CurOF,CurOE);
    }

    BRepOffset_Inter2d::Compute(myAsDes,
				CurOF,
				myEdges,
				myTol,
				aDMVV);
  }
  //
  // fuse vertices on edges stored in AsDes
  BRepOffset_Inter2d::FuseVertices(aDMVV, myAsDes);
  // ------------
  // unwinding 
  // ------------
  BRepOffset_MakeLoops MakeLoops;
  MakeLoops.Build( LOF, myAsDes, myImageOffset );

  // ------------------------------------------------------------
  // It is possible to unwind edges at least one ancestor which of 
  // is a face of the initial shape, so:
  // the edges generated by intersection tube-tube are missing
  // ------------------------------------------------------------

  // --------------------------------------------------------------
  // Currently set the unwinded surfaces in <myResult>
  // --------------------------------------------------------------
  B.MakeCompound(TopoDS::Compound(myResult));
  TopTools_ListIteratorOfListOfShape itLOF(LOF);
  for ( ; itLOF.More(); itLOF.Next()) {
    const TopoDS_Shape& CurLOF = itLOF.Value();

    if ( !myImageOffset.HasImage(CurLOF)) 
      continue;
    
    TopTools_ListOfShape Lim;
    myImageOffset.LastImage(CurLOF,Lim);
    TopTools_ListIteratorOfListOfShape itLim(Lim);
    for ( ;itLim.More(); itLim.Next()) {
      // If a face is its own image, it is not set
      const TopoDS_Shape& CurLIM = itLim.Value();
      if (CurLIM.IsSame(CurLOF)) break;

      B.Add(myResult,CurLIM);
    }
  }

#ifdef OCCT_DEBUG
  if ( myResult.IsNull()) {
    cout << " No Lines of Generated Centers" << endl;
  }
#ifdef DRAW
  else {
    if (Affich) DBRep::Set("Unwind",myResult);
  }
#endif
#endif
}


//=======================================================================
//function : ComputeSurfaces
//purpose  : 
//=======================================================================

void BiTgte_Blend::ComputeSurfaces()
{
  // set in myFaces, the faces actually implied in the connection
  myFaces.Clear();

  // construct
  // 1 - Tubes (True Fillets)
  // 2 - Spheres.

#ifdef DRAW
  Standard_Integer nbc = 1;
#endif

  TopTools_ListOfShape Empty;
  TopTools_DataMapOfShapeListOfShape EmptyMap;

  Handle(Geom_Surface) GS1, GS2;
  Handle(Geom_Curve)   GC1, GC2;

  Standard_Real TolAngle = 2*ASin(myTol/Abs(myRadius*0.5));
  BRepOffset_Analyse CenterAnalyse(myResult,TolAngle);

  // -----------------------------------------------------
  // Construction of tubes in myResult
  // -----------------------------------------------------
  BRep_Builder B;
  B.MakeCompound(TopoDS::Compound(myResult));

  // --------------------------------------------------------------------
  // Dummy: for construction of spheres:
  // Set in Co the center line, then it there are at least 3
  // center lines sharing the same vertex, Sphere on this vertex.
  // --------------------------------------------------------------------
  TopoDS_Compound Co; 
  B.MakeCompound(Co);

  // --------------------------------------------------------------------
  // Iteration on the edges lines of center
  // and their valid valid part is taken after cut and tube construction.
  // --------------------------------------------------------------------

  //TopTools_MapIteratorOfMapOfShape ic(myEdges);
  Standard_Integer i;
  for (i = 1; i <= myEdges.Extent(); i++) {
    const TopoDS_Edge& CurE = TopoDS::Edge(myEdges(i));

    const TopTools_ListOfShape& L = myAsDes->Ascendant(CurE);
    if ( L.Extent() != 2) continue;
    
    // --------------------------------------------------------------
    // F1 and F2 = 2 parallel faces intersecting in CurE.
    // --------------------------------------------------------------
    const TopoDS_Face& F1 = TopoDS::Face(L.First()); 
    const TopoDS_Face& F2 = TopoDS::Face(L.Last());

    // -----------------------------------------------------
    // find the orientation of edges of intersection
    // in the initial faces.
    // -----------------------------------------------------
    const TopTools_ListOfShape& LD1 = myAsDes->Descendant(F1);
    const TopTools_ListOfShape& LD2 = myAsDes->Descendant(F2);

    TopAbs_Orientation Orien1 = Orientation(CurE, F1, LD1);
    TopAbs_Orientation Orien2 = Orientation(CurE, F2, LD2);
    
    // ---------------------------------------------------------
    // Or1 and Or2 : the shapes generators of parallel faces 
    // ---------------------------------------------------------
    const TopoDS_Shape& Or1 = myInitOffsetFace.ImageFrom(F1);
    const TopoDS_Shape& Or2 = myInitOffsetFace.ImageFrom(F2);

    myFaces.Add(Or1);
    myFaces.Add(Or2);

    TopoDS_Edge     OE1, OE2;
    TopoDS_Face     OF1, OF2;
    TopLoc_Location Loc;
    Standard_Real   f1,l1,f2,l2;
    
    Standard_Boolean OF1isEdge = Standard_False;
    
    if ( Or1.ShapeType() == TopAbs_EDGE) {
      OF1isEdge = Standard_True;
      OE1 = TopoDS::Edge(Or1);
      GC1 = BRep_Tool::Curve(OE1,Loc,f1,l1);
      GC1 = 
	Handle(Geom_Curve)::DownCast(GC1->Transformed(Loc.Transformation()));
    }
    else if ( Or1.ShapeType() == TopAbs_FACE) {
      OF1 = TopoDS::Face(Or1);
      GS1 = BRep_Tool::Surface(OF1);
    }

    // ----------------------------------------------------------------
    // If a vertex is used in contact, currently nothing is done  
    // and the vertexes are not managed (Intersections with sphere);
    // ----------------------------------------------------------------
    if ( OF1.IsNull() && OE1.IsNull()) continue;
    
    Standard_Boolean OF2isEdge = Standard_False;
    
    if ( Or2.ShapeType() == TopAbs_EDGE) {
      OF2isEdge = Standard_True;
      OE2 = TopoDS::Edge(Or2);
      GC2 = BRep_Tool::Curve(OE2,Loc,f2,l2);
      GC2 = 
	Handle(Geom_Curve)::
	  DownCast(GC2->Transformed(Loc.Transformation()));
    }
    else if ( Or2.ShapeType() == TopAbs_FACE) {
      OF2 = TopoDS::Face(Or2);
      GS2 = BRep_Tool::Surface(OF2);
    }
    // ----------------------------------------------------------------
    // If a vertex is used in contact, currently nothing is done  
    // and the vertexes are not managed (Intersections with sphere);
    // ----------------------------------------------------------------
    if ( OF2.IsNull() && OE2.IsNull()) continue;


    TopTools_ListOfShape CurL;

    if ( !myImageOffset.HasImage(CurE)) {// the tubes are not unwinded
      if ( OF1isEdge && OF2isEdge) {     // if I don't have the image, possibly
	CurL.Append(CurE);               // I'm on intersection tube-tube
      }                                  // See comment on the call to 
      else                               // MakeLoops
	continue;
    }
    else {
      myImageOffset.LastImage(CurE,CurL);
    }

    // ---------------------------------------------------------------
    // CurL = List of edges descending from CurE ( = Cuts of CurE)
    // ---------------------------------------------------------------
    TopTools_ListIteratorOfListOfShape itl(CurL);
    for ( ; itl.More(); itl.Next()) {
      const TopoDS_Edge& CurCutE = TopoDS::Edge(itl.Value());
      
      Handle(Geom2d_Curve) PC1 = 
	BRep_Tool::CurveOnSurface(CurCutE,F1,f1,l1);
      Handle(Geom2d_Curve) PC2 = 
	BRep_Tool::CurveOnSurface(CurCutE,F2,f2,l2);
      if ( PC1.IsNull() || PC2.IsNull()) {
#ifdef OCCT_DEBUG
	cout << "No PCurves on Intersections : No tubes constructed";
	cout << endl;
#endif	
	continue;
      }

      TopoDS_Edge   E1f, E1l;
      TopoDS_Vertex V1f, V1l;
      TopoDS_Vertex VfOnE1,VlOnE1,VfOnE2,VlOnE2;
      TopTools_ListOfShape TangE;
      TopTools_MapOfShape MapOnV1f, MapOnV1l;

      TopExp::Vertices(CurCutE,V1f,V1l);

      // find if the pipe on the tangent edges are soon created.
      // edges generated by V1f and V1l + Maj MapOnV1f/l
      E1f = FindCreatedEdge(V1f,CurCutE,myMapSF,MapOnV1f,
			    CenterAnalyse,myRadius,myTol);

      E1l = FindCreatedEdge(V1l,CurCutE,myMapSF,MapOnV1l,
			    CenterAnalyse,myRadius,myTol);
      
      TopoDS_Edge E1, E2;
      if ( OF1isEdge) {
	BiTgte_CurveOnEdge ConE(CurCutE, OE1);
	Handle(Geom_Curve) C = MakeCurve(ConE);
	gp_Pnt P1 = C->Value(C->FirstParameter());
	gp_Pnt P2 = C->Value(C->LastParameter());
	VfOnE1 = FindVertex(P1,MapOnV1f,myTol);
	if ( VfOnE1.IsNull())
	  VfOnE1 = FindVertex(P1,MapOnV1l,myTol);
	VlOnE1 = FindVertex(P2,MapOnV1l,myTol);
	if ( VlOnE1.IsNull()) 
	  VlOnE1 = FindVertex(P2,MapOnV1f,myTol);
	if ( P1.SquareDistance(P2) < myTol*myTol) {
	  //BRepOffset_Offset manages degenerated KPart 
	  //It is REQUIRED that C should be a circle with ZERO radius
	  E1 = MakeDegeneratedEdge(C,VfOnE1);
	}
	else {
	  E1 = BRepLib_MakeEdge(C,VfOnE1,VlOnE1);
	}
      }
      else {
	gp_Pnt2d P2d;
	P2d = PC1->Value(f1);
	gp_Pnt P1 = GS1->Value(P2d.X(),P2d.Y());
	P2d = PC1->Value(l1);
	gp_Pnt P2 = GS1->Value(P2d.X(),P2d.Y());
	VfOnE1 = FindVertex(P1,MapOnV1f,myTol);
	VlOnE1 = FindVertex(P2,MapOnV1l,myTol);
	BRepLib_MakeEdge MKE(PC1,GS1,VfOnE1,VlOnE1,f1,l1);
	if (MKE.IsDone()) 
	  E1 = MKE.Edge();
	else  {
	  cout << "Edge Not Done" << endl;
	  E1 = MKE.Edge();
	}
	  
	KPartCurve3d(E1,PC1,GS1);
      }

      if ( OF2isEdge) {
	BiTgte_CurveOnEdge ConE(CurCutE, OE2);
	Handle(Geom_Curve) C = MakeCurve(ConE);
	gp_Pnt P1 = C->Value(C->FirstParameter());
	gp_Pnt P2 = C->Value(C->LastParameter());
	VfOnE2 = FindVertex(P1,MapOnV1f,myTol);
	if ( VfOnE2.IsNull()) 
	  VfOnE2 = FindVertex(P1,MapOnV1l,myTol);
	VlOnE2 = FindVertex(P2,MapOnV1l,myTol);
	if ( VlOnE2.IsNull())
	  VlOnE2 = FindVertex(P2,MapOnV1f,myTol);
	if ( P1.SquareDistance(P2) < myTol*myTol) {
	  //BRepOffset_Offset manages degenerated KParts
	  //It is REQUIRED that C should be a circle with ZERO radius
	  E2 = MakeDegeneratedEdge(C,VfOnE2);
	}
	else {
	  E2 = BRepLib_MakeEdge(C,VfOnE2,VlOnE2);
	}
      }
      else {
	gp_Pnt2d P2d;
	P2d = PC2->Value(f2);
	gp_Pnt P1 = GS2->Value(P2d.X(),P2d.Y());
	P2d = PC2->Value(l2);
	gp_Pnt P2 = GS2->Value(P2d.X(),P2d.Y());
	VfOnE2 = FindVertex(P1,MapOnV1f,myTol);
	VlOnE2 = FindVertex(P2,MapOnV1l,myTol);
	BRepLib_MakeEdge MKE(PC2,GS2,VfOnE2,VlOnE2,f2,l2);
	if (MKE.IsDone()) 
	  E2 = MKE.Edge();
	else {
	  cout << "edge not Done" << endl;
	  E2 = MKE.Edge();
	}
	KPartCurve3d(E2,PC2,GS2);
      }
      // Increment of the Map of Created if reconstruction of the Shape is required
      if ( myBuildShape) {
	myCreated.Bind(CurCutE,EmptyMap);

	myCreated(CurCutE).Bind(Or1,Empty);
	myCreated(CurCutE)(Or1).Append(E1);

	myCreated(CurCutE).Bind(Or2,Empty);
	myCreated(CurCutE)(Or2).Append(E2);
      }

      // ----------------------------------------------------------
      // try to init E1f, E1l, if not found with Analysis.
      // Should happen only if the THEORETICALLY tangent edges 
      // are not actually tangent ( Cf: Approximation of lines 
      // of intersection that add noise.)
      // ----------------------------------------------------------
      TopoDS_Vertex aVertex1, aVertex2;
      if ( E1f.IsNull() && !VfOnE1.IsNull() && !VfOnE2.IsNull()) {
	TopTools_MapIteratorOfMapOfShape it(MapOnV1f);
	for ( ; it.More(); it.Next()) {
	  const TopoDS_Edge& E = TopoDS::Edge(it.Key());
	  if ( !E.IsNull()) {
	    TopExp::Vertices(E, aVertex1, aVertex2);
	    if ((aVertex1.IsSame(VfOnE1) && aVertex2.IsSame(VfOnE2)) ||
		(aVertex2.IsSame(VfOnE1) && aVertex1.IsSame(VfOnE2))   ) {
	      E1f = E;
	      break;
	    }
	  }
	}
      }
      if ( E1l.IsNull() && !VlOnE1.IsNull() && !VlOnE2.IsNull()) {
	TopTools_MapIteratorOfMapOfShape it(MapOnV1l);
	for ( ; it.More(); it.Next()) {
	  const TopoDS_Edge& E = TopoDS::Edge(it.Key());
	  if ( !E.IsNull()) {
	    TopExp::Vertices(E, aVertex1, aVertex2);
	    if ((aVertex1.IsSame(VlOnE1) && aVertex2.IsSame(VlOnE2)) ||
		(aVertex2.IsSame(VlOnE1) && aVertex1.IsSame(VlOnE2))   ) {
	      E1l = E;
	      break;
	    }
	  }
	}
      }
      
      E1.Orientation(Orien1);
      E2.Orientation(Orien2);

      BRepOffset_Offset AnOffset(CurCutE,E1,E2,-myRadius,E1f,E1l,
				 myNubs, myTol, GeomAbs_C2);
      myMapSF.Bind(CurCutE,AnOffset);
      myCenters.Add(CurCutE);
      B.Add(Co, CurCutE);

      const TopoDS_Face& Tuyo = AnOffset.Face();
      B.Add(myResult,Tuyo);

      if ( myBuildShape) {
	// method based ONLY on the construction of fillet:
	// the first edge of the tube is exactly on Shape1.
	GeomAPI_ProjectPointOnCurve Projector;
	TopExp_Explorer exp(Tuyo,TopAbs_EDGE);
	TopoDS_Vertex V1,V2;
	if (OF1isEdge) { // Update CutEdges.
	  const TopoDS_Edge& EOnF1 = TopoDS::Edge(exp.Current());
	  TopExp::Vertices(EOnF1,V1,V2);

	  gp_Pnt P1 = BRep_Tool::Pnt(V1);
	  Projector.Init(P1,GC1);
	  Standard_Real U1 = Projector.LowerDistanceParameter();

	  gp_Pnt P2 = BRep_Tool::Pnt(V2);
	  Projector.Init(P2,GC1);
	  Standard_Real U2 = Projector.LowerDistanceParameter();

	  TopoDS_Shape aLocalShape = V1.Oriented(TopAbs_INTERNAL);
	  B.UpdateVertex(TopoDS::Vertex(aLocalShape),U1,TopoDS::Edge(Or1),myTol);
	  aLocalShape = V2.Oriented(TopAbs_INTERNAL);
	  B.UpdateVertex(TopoDS::Vertex(aLocalShape),U2,TopoDS::Edge(Or1),myTol);
//	  B.UpdateVertex(TopoDS::Vertex(V1.Oriented(TopAbs_INTERNAL)),U1,
//			 TopoDS::Edge(Or1),myTol);
//	  B.UpdateVertex(TopoDS::Vertex(V2.Oriented(TopAbs_INTERNAL)),U2,
//			 TopoDS::Edge(Or1),myTol);

	  if (!myCutEdges.IsBound(Or1)) {
	    TopTools_ListOfShape Dummy;
	    myCutEdges.Bind(Or1,Dummy);
	  }
	  TopTools_ListOfShape& L1 = myCutEdges(Or1);
	  L1.Append(V1); L1.Append(V2);
	}
	if (OF2isEdge) { // Update CutEdges.
	  exp.Next();
	  const TopoDS_Edge& EOnF2 = TopoDS::Edge(exp.Current());
	  TopExp::Vertices(EOnF2,V1,V2);;

	  gp_Pnt P1 = BRep_Tool::Pnt(V1);
	  Projector.Init(P1,GC2);
	  Standard_Real U1 = Projector.LowerDistanceParameter();

	  gp_Pnt P2 = BRep_Tool::Pnt(V2);
	  Projector.Init(P2,GC2);
	  Standard_Real U2 = Projector.LowerDistanceParameter();

	  TopoDS_Shape aLocalShape = V1.Oriented(TopAbs_INTERNAL);
	  B.UpdateVertex(TopoDS::Vertex(aLocalShape),U1,TopoDS::Edge(Or2),myTol);
	  aLocalShape = V2.Oriented(TopAbs_INTERNAL);
	  B.UpdateVertex(TopoDS::Vertex(aLocalShape),U2,TopoDS::Edge(Or2),myTol);
//	  B.UpdateVertex(TopoDS::Vertex(V1.Oriented(TopAbs_INTERNAL)),U1,
//			 TopoDS::Edge(Or2),myTol);
//	  B.UpdateVertex(TopoDS::Vertex(V2.Oriented(TopAbs_INTERNAL)),U2,
//			 TopoDS::Edge(Or2),myTol);

	  if (!myCutEdges.IsBound(Or2)) {
	    TopTools_ListOfShape Dummy;
	    myCutEdges.Bind(Or2,Dummy);
	  }
	  TopTools_ListOfShape& L2 = myCutEdges(Or2);
	  L2.Append(V1); L2.Append(V2);
	}
      }

#ifdef DRAW
      if ( Affich) {
	sprintf(name,"%s_%d","SURF",nbc);
	DBRep::Set(name,AnOffset.Face());
	nbc++;
      }
#endif
    }
  }

  // ---------------------------------------------------
  // Construction of spheres, 
  // if enough tubes arrive at the vertex
  // ---------------------------------------------------
  TopTools_IndexedDataMapOfShapeListOfShape Map;
  TopExp::MapShapesAndAncestors(Co,TopAbs_VERTEX,TopAbs_EDGE,Map);
  
  for ( Standard_Integer j = 1; j <= Map.Extent(); j++) {
    const TopoDS_Vertex& V = TopoDS::Vertex(Map.FindKey(j));
    if ( Map(j).Extent() != 3) continue;

    TopTools_ListOfShape LOE;
    TopTools_ListIteratorOfListOfShape it;
    
    for (it.Initialize(Map(j)) ; it.More(); it.Next()) {
      Standard_Boolean Reverse = Standard_True;
      if ( Reverse) 
	LOE.Append(myMapSF(it.Value()).Generated(V).Reversed());
      else 
	LOE.Append(myMapSF(it.Value()).Generated(V));
    }
    
    BRepOffset_Offset OFT(V,LOE,-myRadius,myNubs, myTol, GeomAbs_C2);
    myMapSF.Bind(V,OFT);
    myCenters.Add(V);

    B.Add(myResult,OFT.Face());

#ifdef DRAW
    if (Affich) {
      sprintf(name,"%s_%d","SURF",nbc);
      DBRep::Set(name,OFT.Face());
      nbc++;
    }
#endif
  }
}


//=======================================================================
//function : ComputeShape
//purpose  : 
//=======================================================================
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <Geom_Curve.hxx>

void BiTgte_Blend::ComputeShape() 
{
  // Find in the initial Shapel:
  //  - untouched Faces
  //  - generated tubes
  //  - the faces neighbors of tubes that sould be reconstucted preserving sharing.

  // For Debug : Visualize edges of the initial shape that should be reconstructed.
#ifdef DRAW
  if (Affich) {
    TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itm(myCutEdges);
    Standard_Integer NbEdges = 0;
    for ( ; itm.More(); itm.Next()) {
      const TopoDS_Edge&          E    = TopoDS::Edge(itm.Key());
      const TopTools_ListOfShape& VonE = itm.Value();
      TopTools_ListOfShape NewE;
      
      CutEdge(E,VonE,NewE);
      for (TopTools_ListIteratorOfListOfShape it(NewE); it.More(); it.Next()) {
	sprintf(name,"%s_%d","CUTE",++NbEdges);
	DBRep::Set(name,it.Value());
      }
    }
  }
#endif
  // end debug

  //
  // modify the tubes on edge for partition of edges.
  //
  Standard_Integer NbS = NbSurfaces();
  for (Standard_Integer i = 1; i <= NbS; i++) {
    const TopoDS_Shape& S1 = SupportShape1(i);

    if ( S1.ShapeType() == TopAbs_EDGE) {
      const TopoDS_Edge& E1   = TopoDS::Edge(S1);
      // it is required to replace in F the cut edges of E1, that
      // represent CutE
      const TopTools_ListOfShape& VonE = myCutEdges(E1);
      TopTools_ListOfShape NewE;
      CutEdge(E1,VonE,NewE);
      
    }
  }


  TopTools_DataMapOfShapeShape Created;

  TopTools_ListOfShape Empty;
  TopTools_DataMapOfShapeListOfShape EmptyMap;
  
  BRep_Builder B;


  // Maj of the Map of created.
  // Update edges that do not change in the resulting shape 
  // i.e. invariant edges in the unwinding.
  TopExp_Explorer exp(myShape,TopAbs_FACE);
  // Standard_Integer nbe = 1;
  for ( ;exp.More(); exp.Next()) {

    const TopoDS_Face& CurF = TopoDS::Face(exp.Current());

    if ( !myFaces.Contains(CurF)) continue; // so the face is not touched

    // so the faces are unwinded
    if ( !myMapSF.IsBound(CurF)) continue; // inverted or degenerated
    
    const BRepOffset_Offset& Offset = myMapSF(CurF);
    const TopoDS_Face& CurOF = myMapSF(CurF).Face();
    
    if ( !myImageOffset.HasImage(CurOF)) // face disappears in unwinding
      continue;
    
    TopExp_Explorer exp2(CurF,TopAbs_EDGE);
    for ( ;exp2.More(); exp2.Next()) {
      const TopoDS_Edge& CurE  = TopoDS::Edge(exp2.Current());
      TopoDS_Shape aLocalShape = Offset.Generated(CurE);
      const TopoDS_Edge& CurOE = TopoDS::Edge(aLocalShape);
//      const TopoDS_Edge& CurOE = TopoDS::Edge(Offset.Generated(CurE));

      if (!myImageOffset.HasImage(CurOE)) continue; 
      // CurOE disappears

      const TopoDS_Edge& ImE = 
	TopoDS::Edge(myImageOffset.Image(CurOE).First());
      if (ImE.IsSame(CurOE)) {
	myCreated.Bind(CurOE,EmptyMap);      
	myCreated(CurOE).Bind(CurF,Empty);
	myCreated(CurOE)(CurF).Append(CurE);
      }
    }
  }

  // The connected faces are already in myResult.
  // So it is necessary to add faces:
  //    - non-touched (so not in myFaces)
  //    - issuing from the unwinding (non degenerated, non inverted, non disappeared)
  exp.Init(myShape,TopAbs_FACE);
  for ( ;exp.More(); exp.Next()) {

    const TopoDS_Face& CurF = TopoDS::Face(exp.Current());

    if ( !myFaces.Contains(CurF)) { 
      // so the face is not touched
      B.Add(myResult,CurF);
    }
    else { // so the faces are unwindeds

      if ( !myMapSF.IsBound(CurF)) continue; // inverted or degenerated

      const TopoDS_Face& CurOF = myMapSF(CurF).Face();
      
      if ( !myImageOffset.HasImage(CurOF)) // face disappears in unwinding
	continue;
      
      // List of faces generated by a face in the unwinding
      TopTools_ListOfShape Lim;
      myImageOffset.LastImage(CurOF,Lim);
      TopTools_ListIteratorOfListOfShape itLim(Lim);
      for ( ;itLim.More(); itLim.Next()) {
	// DeboucFace = offset Face unwinded in "Debouc".
	const TopoDS_Face& DeboucFace = TopoDS::Face(itLim.Value());

	TopLoc_Location L;
	Handle(Geom_Surface) S = BRep_Tool::Surface(CurF,L);
	
	TopoDS_Face NewF; B.MakeFace(NewF);
	B.UpdateFace(NewF,S,L,BRep_Tool::Tolerance(CurF));
	
	TopTools_DataMapOfShapeShape MapSS;
		 
	TopoDS_Shape aLocalShape = DeboucFace.Oriented(TopAbs_FORWARD);
	const TopoDS_Face& Face = TopoDS::Face(aLocalShape);
//	const TopoDS_Face& Face = 
//	  TopoDS::Face(DeboucFace.Oriented(TopAbs_FORWARD));
	TopExp_Explorer exp2(Face, TopAbs_EDGE);
	for ( ; exp2.More(); exp2.Next()) {
	  const TopoDS_Edge& E = TopoDS::Edge(exp2.Current());
	  TopoDS_Vertex V1,V2,OV1,OV2;
	  TopExp::Vertices(E ,V1 ,V2 );
	  if (myCreated.IsBound(E)) {
	    if (myCreated(E).IsBound(CurF)) {
	      const TopoDS_Edge& OE = TopoDS::Edge(myCreated(E)(CurF).First());
	      TopExp::Vertices(OE,OV1,OV2);
	      if ( !myCreated.IsBound(V1)) myCreated.Bind(V1,EmptyMap);
	      if ( !myCreated.IsBound(V2)) myCreated.Bind(V2,EmptyMap);
	      if ( !myCreated(V1).IsBound(CurF))  {
		myCreated(V1).Bind(CurF,Empty);
		myCreated(V1)(CurF).Append(OV1);
	      }
	      if ( !myCreated(V2).IsBound(CurF))  {
		myCreated(V2).Bind(CurF,Empty);
		myCreated(V2)(CurF).Append(OV2);
	      }
	    }
	  }
	}

	TopExp_Explorer expw(Face, TopAbs_WIRE);
	for ( ; expw.More(); expw.Next()) {
	  const TopoDS_Wire& W = TopoDS::Wire(expw.Current());
	  TopExp_Explorer expe(W.Oriented(TopAbs_FORWARD),
			       TopAbs_EDGE);
	  TopoDS_Wire OW;
	  B.MakeWire(OW);
	  
	  for ( ; expe.More(); expe.Next()) {
	    const TopoDS_Edge& E = TopoDS::Edge(expe.Current());
	    Standard_Real f,l;
	    Handle(Geom2d_Curve) C2d = 
	      BRep_Tool::CurveOnSurface(E,Face,f,l);
	    TopoDS_Edge OE;
	    if ( MapSS.IsBound(E)) { // this is an edge of cutting 
	      OE = TopoDS::Edge(MapSS(E));
	      TopoDS_Shape aLocalShapeReversedE = E.Reversed();
	      Handle(Geom2d_Curve) C2d_1 = 
		BRep_Tool::CurveOnSurface(TopoDS::Edge(aLocalShapeReversedE),Face,f,l);
//	      Handle(Geom2d_Curve) C2d_1 = 
//		BRep_Tool::CurveOnSurface(TopoDS::Edge(E.Reversed()),
//					  Face,f,l);
	      if ( E.Orientation() == TopAbs_FORWARD)
		B.UpdateEdge(OE,C2d,C2d_1,NewF,BRep_Tool::Tolerance(E));
	      else
		B.UpdateEdge(OE,C2d_1,C2d,NewF,BRep_Tool::Tolerance(E));
	      B.Range(OE,f,l);
	    }
	    else {
	      // Is there an image in the Map of Created ?
	      if ( myCreated.IsBound(E)) {
		if ( myCreated(E).IsBound(CurF)) {
		  OE = TopoDS::Edge(myCreated(E)(CurF).First());
		}
	      }
	      else {
		B.MakeEdge(OE);
		TopoDS_Vertex V1,V2,OV1,OV2;
		TopExp::Vertices(E,V1,V2);
		if ( myCreated.IsBound(V1) && myCreated(V1).IsBound(CurF)) {
		  OV1 = TopoDS::Vertex(myCreated(V1)(CurF).First());
		}
		else {
		  B.MakeVertex(OV1);
		  gp_Pnt2d P2d = 
		    C2d->Value(BRep_Tool::Parameter(V1,E,Face));
		  gp_Pnt P;
		  S->D0(P2d.X(),P2d.Y(),P);
		  P.Transform(L.Transformation());
		  B.UpdateVertex(OV1,P,BRep_Tool::Tolerance(V1));
		  myCreated.Bind(V1,EmptyMap);
		  myCreated(V1).Bind(CurF,Empty);
		  myCreated(V1)(CurF).Append(OV1);
		}
		if ( myCreated.IsBound(V2) && myCreated(V2).IsBound(CurF)) {
		  OV2 = TopoDS::Vertex(myCreated(V2)(CurF).First());
		}
		else {
		  B.MakeVertex(OV2);
		  gp_Pnt2d P2d = 
		    C2d->Value(BRep_Tool::Parameter(V2,E,Face));
		  gp_Pnt P;
		  S->D0(P2d.X(),P2d.Y(),P);
		  P.Transform(L.Transformation());
		  B.UpdateVertex(OV2,P,BRep_Tool::Tolerance(V2));
		  myCreated.Bind(V2,EmptyMap);
		  myCreated(V2).Bind(CurF,Empty);
		  myCreated(V2)(CurF).Append(OV2);
		}
		B.Add(OE,OV1.Oriented(V1.Orientation()));
		B.Add(OE,OV2.Oriented(V2.Orientation()));
	      }
	      B.UpdateEdge(OE,C2d,NewF,BRep_Tool::Tolerance(E));
	      B.Range(OE,f,l);
//	      ComputeCurve3d(OE,C2d,TheSurf,L,BRep_Tool::Tolerance(E));
	      MapSS.Bind(E,OE);
	    }
	    B.Add(OW, OE.Oriented(E.Orientation()));
	  }
	  B.Add(NewF, OW.Oriented(W.Orientation()));
	}

	NewF.Orientation(DeboucFace.Orientation());
  
	BRepTools::Update(NewF);
	B.Add(myResult,NewF);
      }
    }
  }

  // non-regarding the cause, there always remain greeb borders on this Shape, so it is sewn.
  Handle(BRepBuilderAPI_Sewing) Sew = new BRepBuilderAPI_Sewing(myTol);
  
  BRepLib::BuildCurves3d(myResult);
  
  exp.Init(myResult,TopAbs_FACE);
  for ( ;exp.More(); exp.Next())
    Sew->Add(exp.Current());
  
  Sew->Perform();

  // SameParameter is done in case Sew does not do it (Detect that the edges
  // are not sameparameter but does nothing.)
  
  const TopoDS_Shape& SewedShape = Sew->SewedShape();
  if ( !SewedShape.IsNull()) {
    exp.Init(Sew->SewedShape(), TopAbs_EDGE);
    for (; exp.More(); exp.Next()) {
      const TopoDS_Edge& sec = TopoDS::Edge(exp.Current());
      BRepLib::SameParameter(sec, BRep_Tool::Tolerance(sec));
    }
    myResult = SewedShape;
  }
}


//=======================================================================
//function : Intersect
//purpose  : 
//=======================================================================

Standard_Boolean BiTgte_Blend::Intersect
(const TopoDS_Shape&             Init,
 const TopoDS_Face&              Face,
 const TopTools_DataMapOfShapeBox& MapSBox,
 const BRepOffset_Offset&        OF1,
       BRepOffset_Inter3d&       Inter) 
{
  Standard_Boolean JenRajoute = Standard_False;

  const Bnd_Box& Box1 = MapSBox(Face);

  // -----------------------------------------------
  // intersection with all already created faces.
  // -----------------------------------------------
  const TopoDS_Shape& InitShape1 = OF1.InitialShape();
  Standard_Boolean F1surBordLibre = 
    InitShape1.ShapeType() == TopAbs_EDGE &&
      myStopFaces.Contains(InitShape1);

  TopTools_MapOfShape Done;
  BRepOffset_DataMapIteratorOfDataMapOfShapeOffset It(myMapSF);
  for ( ; It.More(); It.Next()) {
    const BRepOffset_Offset& OF2 = It.Value();
    const TopoDS_Face&       F2  = OF2.Face();
    
    if (Box1.IsOut(MapSBox(F2))) continue;
    
    if ( Inter.IsDone(Face,F2)) continue;
    
    // 2 tubes created on free border are not intersected.
    const TopoDS_Shape& InitShape2 = OF2.InitialShape();
    Standard_Boolean F2surBordLibre = 
      InitShape2.ShapeType() == TopAbs_EDGE &&
	myStopFaces.Contains(InitShape2);

#ifdef OCCT_DEBUG
    if ( F1surBordLibre && F2surBordLibre) {
      cout << "Rejection : 2 tubes on free border are not intersected";
      cout << endl;
    }
#endif

    if ( F1surBordLibre && F2surBordLibre) continue;

    // -------------------------------------------------------
    // Tubes are not intersected with neighbor faces.
    // -------------------------------------------------------
    const TopoDS_Shape& ItKey = It.Key();

    if ( Init.ShapeType() == TopAbs_EDGE) {
      if (ItKey.ShapeType() == TopAbs_FACE &&
	  IsInFace(TopoDS::Edge(Init), TopoDS::Face(ItKey)))  continue;
    }
    
    Inter.FaceInter(Face,F2,myInitOffsetFace);
    
    // ------------------------------------------
    // an edge of F1 or F2 has been touched ?
    // if yes, add faces in myFaces
    //   ==> JenRajoute = True
    // ------------------------------------------
    TopTools_ListOfShape LInt;
    Done.Clear();
    if (myAsDes->HasCommonDescendant(Face,F2,LInt)) {
      TopTools_ListIteratorOfListOfShape  itl2;
      for (itl2.Initialize(LInt); itl2.More(); itl2.Next()) {
	const TopoDS_Edge& CurE = TopoDS::Edge(itl2.Value());
	TopoDS_Vertex V1,V2;
	TopoDS_Edge   E1,E2;
	TopExp::Vertices(CurE,V1,V2);
	
	if ( Done.Add(V1)) {
	  Standard_Boolean IsOnR1 = IsOnRestriction(V1,CurE,Face,E1);
	  Standard_Boolean IsOnR2 = IsOnRestriction(V1,CurE,F2,E2);
#ifdef OCCT_DEBUG
	  if (IsOnR1 && IsOnR2) {
	    cout << "Leave in the same tps on 2 faces, ";
	    cout << "propagation only on free border";
	    cout << endl;
	  }
#endif
	  if ( IsOnR1 ) {
	    if ( !myStopFaces.Contains(Init)) {
	      Add(E1,myEdges,Init,OF1,myAnalyse,IsOnR1 && IsOnR2);
	      JenRajoute = Standard_True;
	    }
	  }
	  if ( IsOnR2) {
	    if ( !myStopFaces.Contains(ItKey)) {
	      Add(E2,myEdges, ItKey,OF2,myAnalyse,IsOnR1 && IsOnR2);
	      JenRajoute = Standard_True;
	    }
	  }
	}

	if ( Done.Add(V2)) {
	  Standard_Boolean IsOnR1 = IsOnRestriction(V2,CurE,Face,E1);
	  Standard_Boolean IsOnR2 = IsOnRestriction(V2,CurE,F2,E2);

	  // If IsOnR1 && IsOnR2,
	  // Leave in the same tps on 2 faces, propagate only on 
	  // free borders.
	  // A priori, only facet is closed.
#ifdef OCCT_DEBUG
	  if (IsOnR1 && IsOnR2) {
	    cout << "Leave with the same tps on 2 faces, ";
	    cout << "propagate only if the border is free";
	    cout << endl;
	  }
#endif
	  if ( IsOnR1) {
	    if ( !myStopFaces.Contains(Init)) {
	      Add(E1,myEdges,Init,OF1,myAnalyse,IsOnR1 && IsOnR2);
	      JenRajoute = Standard_True;
	    }
	  }
	  if ( IsOnR2) {
	    if ( !myStopFaces.Contains(ItKey)) {
	      Add(E2,myEdges,ItKey,OF2,myAnalyse,IsOnR1 && IsOnR2);
	      JenRajoute = Standard_True;
	    }
	  }
	}
      }
    }
  }

  return JenRajoute;
}


