// Created on: 1995-10-23
// Created by: Yves FRICAUD
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


#include <Bnd_Box2d.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPDS_DS.hxx>
#include <BOPTools_AlgoTools2D.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_HCurve.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgo_AsDes.hxx>
#include <BRepAlgo_Image.hxx>
#include <BRepAlgo_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepLib_MakePolygon.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepOffset_Analyse.hxx>
#include <BRepOffset_Interval.hxx>
#include <BRepOffset_ListOfInterval.hxx>
#include <BRepOffset_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepTools_Modifier.hxx>
#include <BRepTools_TrsfModification.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Extrema_ExtPC.hxx>
#include <Extrema_ExtPC2d.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dConvert_ApproxCurve.hxx>
#include <Geom2dConvert_CompCurveToBSplineCurve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Conic.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomInt_IntSS.hxx>
#include <GeomLib.hxx>
#include <GeomProjLib.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <IntTools_FaceFace.hxx>
#include <Precision.hxx>
#include <ProjLib_HProjectedCurve.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <ShapeCustom_Curve2d.hxx>
#include <Standard_ConstructionError.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>

#include <stdio.h>

// The constant defines the maximal value to enlarge surfaces.
// It is limited to 1.e+7. This limitation is justified by the
// floating point format. As we can have only 15
// valuable decimal numbers, then during intersection of surfaces with
// bounds of 1.e+8 the possible inaccuracy might appear already in seventh
// decimal place which will be more than Precision::Confusion value -
// 1.e-7, default tolerance value for the section curves.
// By decreasing the max enlarge value to 1.e+7 the inaccuracy will be
// shifted to eighth decimal place, i.e. the inaccuracy will be
// decreased to values less than 1.e-7.
const Standard_Real TheInfini = 1.e+7;

//tma: for new boolean operation
#ifdef DRAW
#include <DBRep.hxx>
#include <Geom2d_Conic.hxx>
#include <Geom_ElementarySurface.hxx>
#include <Geom_BoundedCurve.hxx>
Standard_Boolean AffichInter  = Standard_False;
static Standard_Integer NbNewEdges  = 1;
static Standard_Integer NbFaces     = 1;
static Standard_Integer NbFOB       = 1;
static Standard_Integer NbFTE       = 1;
static Standard_Integer NbExtE      = 1;
#endif

#ifdef OCCT_DEBUG
static Standard_Boolean AffichExtent = Standard_False;
#endif

static
  void PerformPlanes(const TopoDS_Face& theFace1,
                     const TopoDS_Face& theFace2,
                     const TopAbs_State theState,
                     TopTools_ListOfShape& theL1,
                     TopTools_ListOfShape& theL2);

inline
  Standard_Boolean IsInf(const Standard_Real theVal);

//=======================================================================
//function : EdgeVertices
//purpose  : 
//=======================================================================

void BRepOffset_Tool::EdgeVertices (const TopoDS_Edge&   E,
				    TopoDS_Vertex& V1, 
				    TopoDS_Vertex& V2)
{
  if (E.Orientation() == TopAbs_REVERSED) {
    TopExp::Vertices(E,V2,V1);
  }
  else {
    TopExp::Vertices(E,V1,V2);
  }
}

//=======================================================================
//function : OriEdgeInFace
//purpose  : 
//=======================================================================

TopAbs_Orientation BRepOffset_Tool::OriEdgeInFace (const TopoDS_Edge& E,
						   const TopoDS_Face& F )

{
  TopExp_Explorer Exp;
  Exp.Init(F.Oriented(TopAbs_FORWARD),TopAbs_EDGE);

  for (; Exp.More() ;Exp.Next()) {
    if (Exp.Current().IsSame(E)) {
      return Exp.Current().Orientation();
    }
  }
  throw Standard_ConstructionError("BRepOffset_Tool::OriEdgeInFace");
}


//=======================================================================
//function : FindPeriod
//purpose  : 
//=======================================================================

static void FindPeriod (const TopoDS_Face& F,
			Standard_Real&     umin,
			Standard_Real&     umax,
			Standard_Real&     vmin,
			Standard_Real&     vmax)
{

  Bnd_Box2d B;
  TopExp_Explorer exp;
  for (exp.Init(F,TopAbs_EDGE); exp.More(); exp.Next()) {
    const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
    
    Standard_Real pf,pl;
    const Handle(Geom2d_Curve) C = BRep_Tool::CurveOnSurface(E,F,pf,pl);
    if (C.IsNull()) return;
    Geom2dAdaptor_Curve PC(C,pf,pl);
    Standard_Real i, nbp = 20;
    if (PC.GetType() == GeomAbs_Line) nbp = 2;
    Standard_Real step = (pl - pf) / nbp;
    gp_Pnt2d P;
    PC.D0(pf,P);
    B.Add(P);
    for (i = 2; i < nbp; i++) {
      pf += step;
      PC.D0(pf,P);
      B.Add(P);
    }
    PC.D0(pl,P);
    B.Add(P);
    B.Get(umin,vmin,umax,vmax);
  }
}

//=======================================================================
//function : PutInBounds
//purpose  : Recadre la courbe 2d dans les bounds de la face
//=======================================================================

static void PutInBounds (const TopoDS_Face&          F,
			 const TopoDS_Edge&          E,
			 Handle(Geom2d_Curve)&       C2d)
{
  Standard_Real   umin,umax,vmin,vmax;
  Standard_Real   f,l;
  BRep_Tool::Range(E,f,l);  

  TopLoc_Location L; // Recup S avec la location pour eviter la copie.
  Handle (Geom_Surface) S   = BRep_Tool::Surface(F,L);

  if (S->IsInstance(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    S = Handle(Geom_RectangularTrimmedSurface)::DownCast (S)->BasisSurface();
  }
  //---------------
  // Recadre en U.
  //---------------
  if (!S->IsUPeriodic() && !S->IsVPeriodic()) return;

  FindPeriod (F,umin,umax,vmin,vmax);

  if (S->IsUPeriodic()) {
    Standard_Real period  = S->UPeriod();
    Standard_Real eps     = period*1.e-6;
    gp_Pnt2d      Pf      = C2d->Value(f);
    gp_Pnt2d      Pl      = C2d->Value(l);
    gp_Pnt2d      Pm      = C2d->Value(0.34*f + 0.66*l);
    Standard_Real minC    = Min(Pf.X(),Pl.X()); minC = Min(minC,Pm.X());
    Standard_Real maxC    = Max(Pf.X(),Pl.X()); maxC = Max(maxC,Pm.X());
    Standard_Real du = 0.;
    if (minC< umin - eps) {
      du = (int((umin - minC)/period) + 1)*period;
    }
    if (minC > umax + eps) {
      du = -(int((minC - umax)/period) + 1)*period;
    }
    if (du != 0) {
      gp_Vec2d T1(du,0.);
      C2d->Translate(T1);
      minC += du; maxC += du;
    }
    // Ajuste au mieux la courbe dans le domaine.
    if (maxC > umax +100*eps) {
      Standard_Real d1 = maxC - umax;
      Standard_Real d2 = umin - minC + period;
      if (d2 < d1) du =-period;
      if ( du != 0.) {
	gp_Vec2d T2(du,0.);
	C2d->Translate(T2);
      }
    }
  }
  //------------------
  // Recadre en V.
  //------------------
  if (S->IsVPeriodic()) {
    Standard_Real period  = S->VPeriod();
    Standard_Real eps     = period*1.e-6;
    gp_Pnt2d      Pf      = C2d->Value(f);
    gp_Pnt2d      Pl      = C2d->Value(l);
    gp_Pnt2d      Pm      = C2d->Value(0.34*f + 0.66*l);
    Standard_Real minC    = Min(Pf.Y(),Pl.Y()); minC = Min(minC,Pm.Y());
    Standard_Real maxC    = Max(Pf.Y(),Pl.Y()); maxC = Max(maxC,Pm.Y());
    Standard_Real dv = 0.;
    if (minC< vmin - eps) {
      dv = (int((vmin - minC)/period) + 1)*period;
    }
    if (minC > vmax + eps) {
      dv = -(int((minC - vmax)/period) + 1)*period;
    }
    if (dv != 0) {
      gp_Vec2d T1(0.,dv);
      C2d->Translate(T1);
      minC += dv; maxC += dv;
    }
    // Ajuste au mieux la courbe dans le domaine.
    if (maxC > vmax +100*eps) {
      Standard_Real d1 = maxC - vmax;
      Standard_Real d2 = vmin - minC + period;
      if (d2 < d1) dv =-period;
      if ( dv != 0.) {
	gp_Vec2d T2(0.,dv);
	C2d->Translate(T2);
      }
    }
  }
}

//=======================================================================
//function : Gabarit
//purpose  : 
//=======================================================================

Standard_Real BRepOffset_Tool::Gabarit(const Handle(Geom_Curve)& aCurve)
{
  GeomAdaptor_Curve GC( aCurve );
  Bnd_Box aBox;
  BndLib_Add3dCurve::Add( GC, Precision::Confusion(), aBox );
  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax, dist;
  aBox.Get( aXmin, aYmin, aZmin, aXmax, aYmax, aZmax );
  dist = Max( (aXmax-aXmin), (aYmax-aYmin) );
  dist = Max( dist, (aZmax-aZmin) );
  return dist;
}

//=======================================================================
//function : BuildPCurves
//purpose  : 
//=======================================================================

static void BuildPCurves (const TopoDS_Edge&  E,
			  const TopoDS_Face&  F)
{ 
  Standard_Real   ff,ll;
  Handle (Geom2d_Curve) C2d = BRep_Tool::CurveOnSurface (E,F,ff,ll);
  if (!C2d.IsNull()) return;

  //Standard_Real Tolerance = Max(Precision::Confusion(),BRep_Tool::Tolerance(E));
  Standard_Real Tolerance = Precision::Confusion();

  BRepAdaptor_Surface AS(F,0);
  BRepAdaptor_Curve   AC(E);

  //Try to find pcurve on a bound of BSpline or Bezier surface
  Handle( Geom_Surface ) theSurf = BRep_Tool::Surface( F );
  Handle( Standard_Type ) typS = theSurf->DynamicType();
  if (typS == STANDARD_TYPE(Geom_OffsetSurface))
    typS = Handle(Geom_OffsetSurface)::DownCast (theSurf)->BasisSurface()->DynamicType();
  if (typS == STANDARD_TYPE(Geom_BezierSurface) || typS == STANDARD_TYPE(Geom_BSplineSurface))
    {
      gp_Pnt fpoint = AC.Value( AC.FirstParameter() );
      gp_Pnt lpoint = AC.Value( AC.LastParameter() );
      TopoDS_Face theFace = BRepLib_MakeFace( theSurf, Precision::Confusion() );
      Standard_Real U1 = 0., U2 = 0., TolProj = 1.e-4; //1.e-5;
      TopoDS_Edge theEdge;
      TopExp_Explorer Explo;
      Explo.Init( theFace, TopAbs_EDGE );
      for (; Explo.More(); Explo.Next())
	{
	  TopoDS_Edge anEdge = TopoDS::Edge( Explo.Current() );
	  BRepAdaptor_Curve aCurve( anEdge );
	  Extrema_ExtPC fextr( fpoint, aCurve );
	  if (!fextr.IsDone() || fextr.NbExt() < 1)
	    continue;
	  Standard_Real dist2, dist2min = RealLast();
          Standard_Integer i;
	  for (i = 1; i <= fextr.NbExt(); i++)
	    {
	      dist2 = fextr.SquareDistance(i);
	      if (dist2 < dist2min)
		{
		  dist2min = dist2;
		  U1 = fextr.Point(i).Parameter();
		}
	    }
	  if (dist2min > TolProj * TolProj)
	    continue;
	  Extrema_ExtPC lextr( lpoint, aCurve );
	  if (!lextr.IsDone() || lextr.NbExt() <1)
	    continue;
	  dist2min = RealLast();
	  for (i = 1; i <= lextr.NbExt(); i++)
	    {
	      dist2 = lextr.SquareDistance(i);
	      if (dist2 < dist2min)
		{
		  dist2min = dist2;
		  U2 = lextr.Point(i).Parameter();
		}
	    }
	  if (dist2min <= TolProj * TolProj)
	    {
	      theEdge = anEdge;
	      break;
	    }
	} // for (; Explo.More(); Explo.Current())
      
      if (! theEdge.IsNull())
	{
	  //Construction of pcurve
	  if (U2 < U1)
	    {
	      Standard_Real temp = U1;
	      U1 = U2;
	      U2 = temp;
	    }
	  Standard_Real f, l;
	  C2d = BRep_Tool::CurveOnSurface( theEdge, theFace, f, l );
	  C2d = new Geom2d_TrimmedCurve( C2d, U1, U2 );

	  if (theSurf->IsUPeriodic() || theSurf->IsVPeriodic())
	    PutInBounds( F, E, C2d );

	  BRep_Builder B;
	  B.UpdateEdge( E, C2d, F, BRep_Tool::Tolerance(E) );
	  BRepLib::SameRange( E );

	  return;
	}
    } // if (typS == ...

  Handle(BRepAdaptor_HSurface) HS = new BRepAdaptor_HSurface(AS);
  Handle(BRepAdaptor_HCurve)   HC = new BRepAdaptor_HCurve(AC);

  ProjLib_ProjectedCurve Proj(HS,HC,Tolerance);
  
  switch ( Proj.GetType()) {

  case GeomAbs_Line:
    C2d = new Geom2d_Line(Proj.Line());
    break;

  case GeomAbs_Circle:
    C2d = new Geom2d_Circle(Proj.Circle());
    break;

  case GeomAbs_Ellipse:
    C2d = new Geom2d_Ellipse(Proj.Ellipse());
    break;

  case GeomAbs_Parabola:
    C2d = new Geom2d_Parabola(Proj.Parabola());
    break;

  case GeomAbs_Hyperbola:
    C2d = new Geom2d_Hyperbola(Proj.Hyperbola());
    break;

  case GeomAbs_BezierCurve:
    C2d = Proj.Bezier();
    break;

  case GeomAbs_BSplineCurve:
    C2d = Proj.BSpline();
    break;
  default:
    break;
  }

  if (AS.IsUPeriodic() || AS.IsVPeriodic()) {
    PutInBounds(F,E,C2d);
  }
  if (!C2d.IsNull()) {
    BRep_Builder    B;
    B.UpdateEdge (E,C2d,F,BRep_Tool::Tolerance(E));
  }
  else {
    throw Standard_ConstructionError("BRepOffset_Tool::BuildPCurves");
  }
}

//=======================================================================
//function : OriSect
//purpose  : 
//=======================================================================

void BRepOffset_Tool::OrientSection (const TopoDS_Edge&  E,
				     const TopoDS_Face&  F1,
				     const TopoDS_Face&  F2,
				     TopAbs_Orientation& O1,
				     TopAbs_Orientation& O2) 
{
  TopLoc_Location L;
  Standard_Real   f,l;
  
  
  Handle (Geom_Surface) S1 = BRep_Tool::Surface(F1);
  Handle (Geom_Surface) S2 = BRep_Tool::Surface(F2);    
  Handle (Geom2d_Curve) C1 = BRep_Tool::CurveOnSurface(E,F1,f,l);
  Handle (Geom2d_Curve) C2 = BRep_Tool::CurveOnSurface(E,F2,f,l); 
  Handle (Geom_Curve)   C  = BRep_Tool::Curve(E,L,f,l);

  BRepAdaptor_Curve BAcurve( E );
  
  GCPnts_AbscissaPoint AP(BAcurve,GCPnts_AbscissaPoint::Length(BAcurve)/2.0,f);
  Standard_Real ParOnC;

  if(AP.IsDone())
    ParOnC = AP.Parameter();
  else
    ParOnC = BOPTools_AlgoTools2D::IntermediatePoint(f, l);

  gp_Vec T1 = C->DN(ParOnC,1).Transformed(L.Transformation());
  if (T1.SquareMagnitude() > gp::Resolution()) {
    T1.Normalize();
  }

  gp_Pnt2d P  = C1->Value(ParOnC);
  gp_Pnt   P3;
  gp_Vec   D1U,D1V;
  
  S1->D1(P.X(),P.Y(),P3,D1U,D1V);
  gp_Vec DN1(D1U^D1V);
  if (F1.Orientation() == TopAbs_REVERSED) DN1.Reverse();
  
  P = C2->Value(ParOnC);
  S2->D1(P.X(),P.Y(),P3,D1U,D1V);
  gp_Vec DN2(D1U^D1V);
  if (F2.Orientation() == TopAbs_REVERSED) DN2.Reverse();
  
  gp_Vec        ProVec = DN2^T1;
  Standard_Real Prod  =  DN1.Dot(ProVec);
  if (Prod < 0.0) {
    O1 = TopAbs_FORWARD;
  }
  else {
    O1 = TopAbs_REVERSED;
  }
  ProVec = DN1^T1;
  Prod  =  DN2.Dot(ProVec);
  if (Prod < 0.0) {
    O2 = TopAbs_FORWARD;
  }
  else {
    O2 = TopAbs_REVERSED;
  }
  if (F1.Orientation() == TopAbs_REVERSED) O1 = TopAbs::Reverse(O1);
  if (F2.Orientation() == TopAbs_REVERSED) O2 = TopAbs::Reverse(O2);
}

//=======================================================================
//function : FindCommonShapes
//purpose  : 
//=======================================================================
Standard_Boolean BRepOffset_Tool::FindCommonShapes(const TopoDS_Face& theF1,
                                                   const TopoDS_Face& theF2,
                                                   TopTools_ListOfShape& theLE,
                                                   TopTools_ListOfShape& theLV)
{
  Standard_Boolean bFoundEdges =
    FindCommonShapes(theF1, theF2, TopAbs_EDGE,   theLE);
  Standard_Boolean bFoundVerts =
    FindCommonShapes(theF1, theF2, TopAbs_VERTEX, theLV);
  return bFoundEdges || bFoundVerts;
}

//=======================================================================
//function : FindCommonShapes
//purpose  : 
//=======================================================================
Standard_Boolean BRepOffset_Tool::FindCommonShapes(const TopoDS_Shape& theS1,
                                                   const TopoDS_Shape& theS2,
                                                   const TopAbs_ShapeEnum theType,
                                                   TopTools_ListOfShape& theLSC)
{
  theLSC.Clear();
  //
  TopTools_MapOfShape aMS;
  TopExp_Explorer aExp(theS1, theType);
  for (; aExp.More(); aExp.Next()) {
    aMS.Add(aExp.Current());
  }
  //
  if (aMS.IsEmpty()) {
    return Standard_False;
  }
  //
  TopTools_MapOfShape aMFence;
  aExp.Init(theS2, theType);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Shape& aS2 = aExp.Current();
    if (aMS.Contains(aS2)) {
      if (aMFence.Add(aS2)) {
        theLSC.Append(aS2);
      }
    }
  }
  //
  return !theLSC.IsEmpty();
}

//=======================================================================
//function : ToSmall
//purpose  : 
//=======================================================================

static Standard_Boolean ToSmall (const Handle(Geom_Curve)& C)
{
  Standard_Real Tol = 10*Precision::Confusion();
  Standard_Real m   = (C->FirstParameter()*0.668 + C->LastParameter()*0.332); 
  gp_Pnt P1 = C->Value(C->FirstParameter());
  gp_Pnt P2 = C->Value(C->LastParameter());
  gp_Pnt P3 = C->Value(m);
  if (P1.Distance(P2) > Tol) return Standard_False;
  if (P2.Distance(P3) > Tol) return Standard_False;
  return Standard_True;
}


//=======================================================================
//function : IsOnSurface
//purpose  : 
//=======================================================================

static Standard_Boolean IsOnSurface(const Handle(Geom_Curve)&   C,
				    const Handle(Geom_Surface)& S,
				    Standard_Real               TolConf,
				    Standard_Real&              TolReached) 
{
  Standard_Real    f = C->FirstParameter();
  Standard_Real    l = C->LastParameter();
  Standard_Integer n = 5;
  Standard_Real du = (f-l)/(n-1);
  TolReached = 0.;

  gp_Pnt P;
  Standard_Real U,V;

  GeomAdaptor_Surface AS(S);

  switch ( AS.GetType()) {
  case GeomAbs_Plane:
    {
      gp_Ax3 Ax = AS.Plane().Position();
      for ( Standard_Integer i = 0; i < n; i++) {
	P = C->Value(f+i*du);
	ElSLib::PlaneParameters(Ax,P,U,V); 
	TolReached = P.Distance(ElSLib::PlaneValue(U,V,Ax));
	if ( TolReached  > TolConf)
	  return Standard_False;
      }
      break;
    }
  case GeomAbs_Cylinder:
    {
      gp_Ax3        Ax  = AS.Cylinder().Position();
      Standard_Real Rad = AS.Cylinder().Radius();
      for ( Standard_Integer i = 0; i < n; i++) {
	P = C->Value(f+i*du);
	ElSLib::CylinderParameters(Ax,Rad,P,U,V); 
	TolReached = P.Distance(ElSLib::CylinderValue(U,V,Ax,Rad));
	if ( TolReached > TolConf)
	  return Standard_False;
      }
      break;
    }
  case GeomAbs_Cone:
    {
      gp_Ax3        Ax  = AS.Cone().Position();
      Standard_Real Rad = AS.Cone().RefRadius();
      Standard_Real Alp = AS.Cone().SemiAngle();
      for ( Standard_Integer i = 0; i < n; i++) {
	P = C->Value(f+i*du);
	ElSLib::ConeParameters(Ax,Rad,Alp,P,U,V); 
	TolReached = P.Distance(ElSLib::ConeValue(U,V,Ax,Rad,Alp));
	if ( TolReached > TolConf)
	  return Standard_False;
      }
      break;
    }
  case GeomAbs_Sphere:
    {
      gp_Ax3        Ax  = AS.Sphere().Position();
      Standard_Real Rad = AS.Sphere().Radius();
      for ( Standard_Integer i = 0; i < n; i++) {
	P = C->Value(f+i*du);
	ElSLib::SphereParameters(Ax,Rad,P,U,V); 
	TolReached = P.Distance(ElSLib::SphereValue(U,V,Ax,Rad));
	if ( TolReached > TolConf)
	  return Standard_False;
      }
      break;
    }
  case GeomAbs_Torus:
    {
      gp_Ax3        Ax  = AS.Torus().Position();
      Standard_Real R1  = AS.Torus().MajorRadius();
      Standard_Real R2  = AS.Torus().MinorRadius();
      for ( Standard_Integer i = 0; i < n; i++) {
	P = C->Value(f+i*du);
	ElSLib::TorusParameters(Ax,R1,R2,P,U,V); 
	TolReached =  P.Distance(ElSLib::TorusValue(U,V,Ax,R1,R2));
	if ( TolReached > TolConf)
	  return Standard_False;
      }
      break;
    }
    
  default:
    {
      return Standard_False;
    }
  }
  
  return Standard_True;
} 


//=======================================================================
//function : PipeInter
//purpose  : 
//=======================================================================

void BRepOffset_Tool::PipeInter(const TopoDS_Face& F1,
				const TopoDS_Face& F2,
				TopTools_ListOfShape& L1,
				TopTools_ListOfShape& L2,
				const TopAbs_State    Side)
{ 
#ifdef DRAW
  if (AffichInter) {
    char name[256];
    sprintf(name,"FF_%d",NbFaces++);
    DBRep::Set(name,F1);
    sprintf(name,"FF_%d",NbFaces++);
    DBRep::Set(name,F2);
  }
#endif

  Handle (Geom_Curve) CI;
  TopAbs_Orientation O1,O2;
  L1.Clear(); L2.Clear();
  BRep_Builder B;
  Handle (Geom_Surface) S1 = BRep_Tool::Surface(F1);
  Handle (Geom_Surface) S2 = BRep_Tool::Surface(F2);  

  GeomInt_IntSS Inter (S1,S2, Precision::Confusion(),1,1,1);
  
  if (Inter.IsDone()) {
    for (Standard_Integer i = 1; i <= Inter.NbLines(); i++) {
      CI = Inter.Line(i);
      if (ToSmall(CI)) continue;
      TopoDS_Edge E = BRepLib_MakeEdge(CI);
      if (Inter.HasLineOnS1(i)) {
	Handle(Geom2d_Curve) C2 = Inter.LineOnS1(i);
	PutInBounds  (F1,E,C2);
	B.UpdateEdge (E,C2,F1,BRep_Tool::Tolerance(E));
      }
      else {
	BuildPCurves (E,F1);
      }
      if (Inter.HasLineOnS2(i)) {
	Handle(Geom2d_Curve) C2 = Inter.LineOnS2(i);
	PutInBounds  (F2,E,C2);
	B.UpdateEdge (E,C2,F2,BRep_Tool::Tolerance(E));
      }
      else {
	BuildPCurves (E,F2);
      }
      OrientSection (E,F1,F2,O1,O2);
      if (Side == TopAbs_OUT) {
	O1 = TopAbs::Reverse(O1);
	O2 = TopAbs::Reverse(O2);
      }
      L1.Append (E.Oriented(O1));
      L2.Append (E.Oriented(O2));
#ifdef DRAW
      if (AffichInter) {
        char name[256];
	sprintf(name,"EI_%d",NbNewEdges++);	
	DBRep::Set(name,E.Oriented(O1));
      }
#endif      
    }
  }
}


//=======================================================================
//function : IsAutonomVertex
//purpose  : Checks wether a vertex is "autonom" or not
//=======================================================================

static Standard_Boolean IsAutonomVertex(const TopoDS_Shape& aVertex,
					const BOPDS_PDS& pDS)
{
  Standard_Integer index;
  Standard_Integer aNbVVs, aNbEEs, aNbEFs, aInt;
  //
  index = pDS->Index(aVertex);
  if (index == -1) {
    Standard_Integer i, i1, i2;
    i1=pDS->NbSourceShapes();
    i2=pDS->NbShapes();
    for (i=i1; i<i2; ++i) {
      const TopoDS_Shape& aSx=pDS->Shape(i);
      if(aSx.IsSame(aVertex)) {
		  index = i;
		  break;
		}
	    }
	}
  //
  if (!pDS->IsNewShape(index)) {
    return Standard_False;
    }
  //check if vertex with index "index" is not created in VV or EE or EF interference
  //VV
  BOPDS_VectorOfInterfVV& aVVs=pDS->InterfVV();
  aNbVVs = aVVs.Extent();
  for(aInt = 0; aInt < aNbVVs; aInt++) {
    const BOPDS_InterfVV& aVV = aVVs(aInt);
    if (aVV.HasIndexNew()) {
      if (aVV.IndexNew() == index) {
		return Standard_False;
	    }
	}
    }
  //EE
  BOPDS_VectorOfInterfEE& aEEs=pDS->InterfEE();
  aNbEEs = aEEs.Extent();
  for(aInt = 0; aInt < aNbEEs; aInt++) {
    const BOPDS_InterfEE& aEE = aEEs(aInt);
    IntTools_CommonPrt aCP = aEE.CommonPart();
    if(aCP.Type() == TopAbs_VERTEX) {
      if (aEE.IndexNew() == index) {
		    return Standard_False;
		}
	    }
	}
  //EF
  BOPDS_VectorOfInterfEF& aEFs=pDS->InterfEF();
  aNbEFs = aEFs.Extent();
  for(aInt = 0; aInt < aNbEFs; aInt++) {
    const BOPDS_InterfEF& aEF = aEFs(aInt);
    IntTools_CommonPrt aCP = aEF.CommonPart();
    if(aCP.Type() == TopAbs_VERTEX) {
      if (aEF.IndexNew() == index) {
        return Standard_False;
    }
    }
  }
  return Standard_True;
}


//=======================================================================
//function : AreConnex
//purpose  : define if two shapes are connex by a vertex (vertices)
//=======================================================================

static Standard_Boolean AreConnex(const TopoDS_Wire& W1,
				  const TopoDS_Wire& W2,
				  const BOPDS_PDS& pDS)
{
  TopoDS_Vertex V11, V12, V21, V22;
  TopExp::Vertices( W1, V11, V12 );
  TopExp::Vertices( W2, V21, V22 );

  if (V11.IsSame(V21) || V11.IsSame(V22) ||
      V12.IsSame(V21) || V12.IsSame(V22))
    {
      Standard_Boolean isCV1 = V11.IsSame(V21) || V11.IsSame(V22);
      Standard_Boolean isCV2 = V12.IsSame(V21) || V12.IsSame(V22);
      if (isCV1 && !IsAutonomVertex(V11, pDS))
	{
	  if (!isCV2)
	    return Standard_False;
          if (!IsAutonomVertex(V12, pDS))
	    return Standard_False;
	}
      if (!isCV1 && !IsAutonomVertex(V12, pDS))
	return Standard_False;

      TopoDS_Vertex CV = (V11.IsSame(V21) || V11.IsSame(V22))? V11 : V12;
      TopoDS_Edge E1, E2;
      TopoDS_Iterator itw( W1 );
      for (; itw.More(); itw.Next())
	{
	  E1 = TopoDS::Edge(itw.Value());
	  TopoDS_Vertex V1, V2;
	  TopExp::Vertices( E1, V1, V2 );
	  if (V1.IsSame(CV) || V2.IsSame(CV))
	    break;
	}
      itw.Initialize( W2 );
      E2 = TopoDS::Edge(itw.Value());

      Standard_Real f, l;
      Handle(Geom_Curve) C1 = BRep_Tool::Curve( E1, f, l ); 
      if (C1->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
	C1 = Handle(Geom_TrimmedCurve)::DownCast (C1)->BasisCurve();
      
      Handle(Geom_Curve) C2 = BRep_Tool::Curve( E2, f, l );
      if (C2->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
	C2 = Handle(Geom_TrimmedCurve)::DownCast (C2)->BasisCurve();

      if (C1->IsInstance(STANDARD_TYPE(Geom_Line)) &&
	  C2->IsInstance(STANDARD_TYPE(Geom_Line)))
	{
	  Handle(Geom_Line) L1 = Handle(Geom_Line)::DownCast (C1);
	  gp_Ax1 Axis1 = L1->Position();
	  Handle(Geom_Line) L2 = Handle(Geom_Line)::DownCast (C2);
	  gp_Ax1 Axis2 = L2->Position();
	  if (! Axis1.IsParallel( Axis2, Precision::Angular() ))
	    return Standard_False;
	}

      return Standard_True;
    }

  return Standard_False;
}

//=======================================================================
//function : AreClosed
//purpose  : define if two edges are connex by two vertices
//=======================================================================

static Standard_Boolean AreClosed(const TopoDS_Edge& E1,
				  const TopoDS_Edge& E2)
{
  TopoDS_Vertex V11, V12, V21, V22;
  TopExp::Vertices( E1, V11, V12 );
  TopExp::Vertices( E2, V21, V22 );

  if ((V11.IsSame(V21) && V12.IsSame(V22)) ||
      (V11.IsSame(V22) && V12.IsSame(V21)))
    return Standard_True;

  return Standard_False;
}

//=======================================================================
//function : BSplineEdges
//purpose  : 
//=======================================================================

static Standard_Boolean BSplineEdges(const TopoDS_Edge& E1,
				     const TopoDS_Edge& E2,
				     const Standard_Integer par1,
				     const Standard_Integer par2,
				     Standard_Real& angle)
{
  Standard_Real first1, last1, first2, last2, Param1, Param2;

  Handle(Geom_Curve) C1 = BRep_Tool::Curve( E1, first1, last1 ); 
  if (C1->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
    C1 = Handle(Geom_TrimmedCurve)::DownCast (C1)->BasisCurve();

  Handle(Geom_Curve) C2 = BRep_Tool::Curve( E2, first2, last2 );
  if (C2->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
    C2 = Handle(Geom_TrimmedCurve)::DownCast (C2)->BasisCurve();

  if (!C1->IsInstance(STANDARD_TYPE(Geom_BSplineCurve)) ||
      !C2->IsInstance(STANDARD_TYPE(Geom_BSplineCurve)))
    return Standard_False;

  Param1 = (par1 == 0)? first1 : last1;
  Param2 = (par2 == 0)? first2 : last2;

  gp_Pnt Pnt1, Pnt2;
  gp_Vec Der1, Der2;
  C1->D1( Param1, Pnt1, Der1 );
  C2->D1( Param2, Pnt2, Der2 );

  if (Der1.Magnitude() <= gp::Resolution() ||
      Der2.Magnitude() <= gp::Resolution())
    angle = M_PI/2.;
  else
    angle = Der1.Angle(Der2);

  return Standard_True;
}

//=======================================================================
//function : AngleWireEdge
//purpose  : 
//=======================================================================

static Standard_Real AngleWireEdge(const TopoDS_Wire& aWire,
				   const TopoDS_Edge& anEdge)
{
  TopoDS_Vertex V11, V12, V21, V22, CV;
  TopExp::Vertices( aWire,  V11, V12 );
  TopExp::Vertices( anEdge, V21, V22 );
  CV = (V11.IsSame(V21) || V11.IsSame(V22))? V11 : V12;
  TopoDS_Edge FirstEdge;
  TopoDS_Iterator itw(aWire);
  for (; itw.More(); itw.Next())
    {
      FirstEdge = TopoDS::Edge(itw.Value());
      TopoDS_Vertex v1, v2;
      TopExp::Vertices( FirstEdge, v1, v2 );
      if (v1.IsSame(CV) || v2.IsSame(CV))
	{
	  V11 = v1;
	  V12 = v2;
	  break;
	}
    }
  Standard_Real Angle;
  if (V11.IsSame(CV) && V21.IsSame(CV))
    {
      BSplineEdges( FirstEdge, anEdge, 0, 0, Angle );
      Angle = M_PI - Angle;
    }
  else if (V11.IsSame(CV) && V22.IsSame(CV))
    BSplineEdges( FirstEdge, anEdge, 0, 1, Angle );
  else if (V12.IsSame(CV) && V21.IsSame(CV))
    BSplineEdges( FirstEdge, anEdge, 1, 0, Angle );
  else
    {
      BSplineEdges( FirstEdge, anEdge, 1, 1, Angle );
      Angle = M_PI - Angle;
    }
  return Angle;
}


//=======================================================================
//function : ReconstructPCurves
//purpose  : 
//=======================================================================

static void ReconstructPCurves(const TopoDS_Edge& anEdge)
{
  Standard_Real f, l;
  Handle(Geom_Curve) C3d = BRep_Tool::Curve(anEdge, f, l);

  BRep_ListIteratorOfListOfCurveRepresentation
    itcr( (Handle(BRep_TEdge)::DownCast(anEdge.TShape()))->ChangeCurves() );
  for (; itcr.More(); itcr.Next())
    {
      Handle( BRep_CurveRepresentation ) CurveRep = itcr.Value();
      if (CurveRep->IsCurveOnSurface())
	{
	  Handle(Geom_Surface) theSurf  = CurveRep->Surface();
	  TopLoc_Location      theLoc   = CurveRep->Location();
	  theLoc = anEdge.Location() * theLoc;
	  theSurf = Handle(Geom_Surface)::DownCast
	    (theSurf->Transformed(theLoc.Transformation()));
	  Handle(Geom2d_Curve) ProjPCurve =
	    GeomProjLib::Curve2d( C3d, f, l, theSurf );
          if(!ProjPCurve.IsNull())
          {
	    CurveRep->PCurve( ProjPCurve );
          }
	}
    }
}

//=======================================================================
//function : ConcatPCurves
//purpose  : 
//=======================================================================

static Handle(Geom2d_Curve) ConcatPCurves(const TopoDS_Edge& E1,
					  const TopoDS_Edge& E2,
					  const TopoDS_Face& F,
					  const Standard_Boolean After,
					  Standard_Real& newFirst,
					  Standard_Real& newLast)
{
  Standard_Real Tol = 1.e-7;
  GeomAbs_Shape Continuity = GeomAbs_C1;
  Standard_Integer MaxDeg = 14;
  Standard_Integer MaxSeg = 16;

  Standard_Real first1, last1, first2, last2;
  Handle(Geom2d_Curve) PCurve1, PCurve2, newPCurve;

  PCurve1 = BRep_Tool::CurveOnSurface( E1, F, first1, last1 );
  if (PCurve1->IsInstance(STANDARD_TYPE(Geom2d_TrimmedCurve)))
    PCurve1 = Handle(Geom2d_TrimmedCurve)::DownCast (PCurve1)->BasisCurve();

  PCurve2 = BRep_Tool::CurveOnSurface( E2, F, first2, last2 );
  if (PCurve2->IsInstance(STANDARD_TYPE(Geom2d_TrimmedCurve)))
    PCurve2 = Handle(Geom2d_TrimmedCurve)::DownCast (PCurve2)->BasisCurve();
      
  if (PCurve1 == PCurve2)
    {
      newPCurve = PCurve1;
      newFirst  = Min( first1, first2 );
      newLast   = Max( last1, last2 );
    }
  else if (PCurve1->DynamicType() == PCurve2->DynamicType() &&
	   (PCurve1->IsInstance(STANDARD_TYPE(Geom2d_Line)) ||
	    PCurve1->IsKind(STANDARD_TYPE(Geom2d_Conic))))
    {
      newPCurve = PCurve1;
      gp_Pnt2d P1, P2;
      P1 = PCurve2->Value( first2 );
      P2 = PCurve2->Value( last2 );
      if (PCurve1->IsInstance(STANDARD_TYPE(Geom2d_Line)))
	{
	  Handle(Geom2d_Line) Lin1 = Handle(Geom2d_Line)::DownCast (PCurve1);
	  gp_Lin2d theLin = Lin1->Lin2d();
	  first2 = ElCLib::Parameter( theLin, P1 );
	  last2  = ElCLib::Parameter( theLin, P2 );
	}
      else if (PCurve1->IsInstance(STANDARD_TYPE(Geom2d_Circle)))
	{
	  Handle(Geom2d_Circle) Circ1 = Handle(Geom2d_Circle)::DownCast (PCurve1);
	  gp_Circ2d theCirc = Circ1->Circ2d();
	  first2 = ElCLib::Parameter( theCirc, P1 );
	  last2  = ElCLib::Parameter( theCirc, P2 );
	}
      else if (PCurve1->IsInstance(STANDARD_TYPE(Geom2d_Ellipse)))
	{
	  Handle(Geom2d_Ellipse) Ell1 = Handle(Geom2d_Ellipse)::DownCast (PCurve1);
	  gp_Elips2d theElips = Ell1->Elips2d();
	  first2 = ElCLib::Parameter( theElips, P1 );
	  last2  = ElCLib::Parameter( theElips, P2 );
	}
      else if (PCurve1->IsInstance(STANDARD_TYPE(Geom2d_Parabola)))
	{
	  Handle(Geom2d_Parabola) Parab1 = Handle(Geom2d_Parabola)::DownCast (PCurve1);
	  gp_Parab2d theParab = Parab1->Parab2d();
	  first2 = ElCLib::Parameter( theParab, P1 );
	  last2  = ElCLib::Parameter( theParab, P2 );
	}
      else if (PCurve1->IsInstance(STANDARD_TYPE(Geom2d_Hyperbola)))
	{
	  Handle(Geom2d_Hyperbola) Hypr1 = Handle(Geom2d_Hyperbola)::DownCast (PCurve1);
	  gp_Hypr2d theHypr = Hypr1->Hypr2d();
	  first2 = ElCLib::Parameter( theHypr, P1 );
	  last2  = ElCLib::Parameter( theHypr, P2 );
	}
      newFirst  = Min( first1, first2 );
      newLast   = Max( last1, last2 );
    }
  else
    {
      Handle(Geom2d_TrimmedCurve) TC1 = new Geom2d_TrimmedCurve( PCurve1, first1, last1 );
      Handle(Geom2d_TrimmedCurve) TC2 = new Geom2d_TrimmedCurve( PCurve2, first2, last2 );
      Geom2dConvert_CompCurveToBSplineCurve Concat2d( TC1 );
      Concat2d.Add( TC2, Precision::Confusion(), After );
      newPCurve = Concat2d.BSplineCurve();
      if (newPCurve->Continuity() < GeomAbs_C1)
	{
	  Geom2dConvert_ApproxCurve Approx2d( newPCurve, Tol, Continuity, MaxSeg, MaxDeg );
	  if (Approx2d.HasResult())
	    newPCurve = Approx2d.Curve();
	}
      newFirst = newPCurve->FirstParameter();
      newLast  = newPCurve->LastParameter();
    }

  return newPCurve;
}

//=======================================================================
//function : Glue
//purpose  : glue two edges.
//=======================================================================

static TopoDS_Edge Glue(const TopoDS_Edge& E1,
                        const TopoDS_Edge& E2,
                        const TopoDS_Vertex& Vfirst,
                        const TopoDS_Vertex& Vlast,
                        const Standard_Boolean After,
                        const TopoDS_Face& F1,
                        const Standard_Boolean addPCurve1,
                        const TopoDS_Face& F2,
                        const Standard_Boolean addPCurve2,
                        const Standard_Real theGlueTol)
{
  Standard_Real Tol = 1.e-7;
  GeomAbs_Shape Continuity = GeomAbs_C1;
  Standard_Integer MaxDeg = 14;
  Standard_Integer MaxSeg = 16;

  Handle(Geom_Curve) C1, C2, newCurve;
  Handle(Geom2d_Curve) PCurve1, PCurve2, newPCurve;
  Standard_Real first1, last1, first2, last2, fparam=0., lparam=0.;
  Standard_Boolean IsCanonic = Standard_False;

  C1 = BRep_Tool::Curve( E1, first1, last1 ); 
  if (C1->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
    C1 = Handle(Geom_TrimmedCurve)::DownCast (C1)->BasisCurve();

  C2 = BRep_Tool::Curve( E2, first2, last2 );
  if (C2->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
    C2 = Handle(Geom_TrimmedCurve)::DownCast (C2)->BasisCurve();

  if (C1 == C2)
    {
      newCurve = C1;
      fparam = Min( first1, first2 );
      lparam = Max( last1, last2 );
    }
  else if (C1->DynamicType() == C2->DynamicType() &&
	   (C1->IsInstance(STANDARD_TYPE(Geom_Line)) ||
	    C1->IsKind(STANDARD_TYPE(Geom_Conic))))
    {
      IsCanonic = Standard_True;
      newCurve  = C1;
    }
  else
    {
      Handle(Geom_TrimmedCurve) TC1 = new Geom_TrimmedCurve( C1, first1, last1 );
      Handle(Geom_TrimmedCurve) TC2 = new Geom_TrimmedCurve( C2, first2, last2 );
      GeomConvert_CompCurveToBSplineCurve Concat( TC1 );
      Concat.Add( TC2, theGlueTol, After );
      newCurve = Concat.BSplineCurve();
      if (newCurve->Continuity() < GeomAbs_C1)
	{
	  GeomConvert_ApproxCurve Approx3d( newCurve, Tol, Continuity, MaxSeg, MaxDeg );
	  if (Approx3d.HasResult())
	    newCurve = Approx3d.Curve();
	}
      fparam = newCurve->FirstParameter();
      lparam = newCurve->LastParameter();
    }

  TopoDS_Edge newEdge;
  BRep_Builder BB;

  if (IsCanonic)
    newEdge = BRepLib_MakeEdge( newCurve, Vfirst, Vlast );
  else
    newEdge = BRepLib_MakeEdge( newCurve, Vfirst, Vlast, fparam, lparam );

  Standard_Real newFirst, newLast;
  if (addPCurve1)
    {
      newPCurve = ConcatPCurves( E1, E2, F1, After, newFirst, newLast );
      BB.UpdateEdge( newEdge, newPCurve, F1, 0. );
      BB.Range( newEdge, F1, newFirst, newLast );
    }
  if (addPCurve2)
    {
      newPCurve = ConcatPCurves( E1, E2, F2, After, newFirst, newLast );
      BB.UpdateEdge( newEdge, newPCurve, F2, 0. );
      BB.Range( newEdge, F2, newFirst, newLast );
    }

  return newEdge;
}


//=======================================================================
//function : FindNewVerticesOnBoundsOfFace
//purpose  : 
//=======================================================================

static void FindNewVerticesOnBoundsOfFace(const BOPDS_PDS& pDS,
					  const TopoDS_Face& aFace,
					  TopTools_DataMapOfShapeShape& VEmap)
{
  TopTools_IndexedMapOfShape OldVertices;
  TopExp::MapShapes( aFace, TopAbs_VERTEX, OldVertices );
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  TopoDS_Vertex V1, V2;

  TopExp_Explorer Explo( aFace, TopAbs_EDGE );
  for (; Explo.More(); Explo.Next()) {
    const TopoDS_Shape& aE = Explo.Current();
    Standard_Integer nE = pDS->Index(aE);
    //
    const BOPDS_ListOfPaveBlock& aLPB = pDS->PaveBlocks(nE);
    aItLPB.Initialize(aLPB);
    for (; aItLPB.More(); aItLPB.Next()) {
      const Handle(BOPDS_PaveBlock)& aPB = aItLPB.Value();
      const TopoDS_Edge& aESp = *(TopoDS_Edge*)&pDS->Shape(aPB->Edge());
      //
      TopExp::Vertices( aESp, V1, V2 );
      if (!OldVertices.Contains( V1 )) {
        VEmap.Bind( V1, aE );
	}
      //
      if (!OldVertices.Contains( V2 )) {
        VEmap.Bind( V2, aE );
    }
}
  }
}
				   
//=======================================================================
//function : CheckIntersFF
//purpose  : 
//=======================================================================

static Standard_Boolean CheckIntersFF(const BOPDS_PDS& pDS,
				      const TopoDS_Edge& RefEdge,
				      const TopoDS_Face& F1,
				      const TopoDS_Face& F2,
				      TopTools_IndexedMapOfShape& TrueEdges)
{
  Standard_Boolean isEl1 = Standard_False, isEl2 = Standard_False;
  Standard_Boolean isPlane1 = Standard_False, isPlane2 = Standard_False;

  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(F1);
  if (aSurf->IsInstance(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    aSurf = Handle(Geom_RectangularTrimmedSurface)::DownCast (aSurf)->BasisSurface();
  if (aSurf->IsInstance(STANDARD_TYPE(Geom_Plane)))
    isPlane1 = Standard_True;
  else if (aSurf->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
    isEl1 = Standard_True;

  aSurf = BRep_Tool::Surface(F2);
  if (aSurf->IsInstance(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    aSurf = Handle(Geom_RectangularTrimmedSurface)::DownCast (aSurf)->BasisSurface();
  if (aSurf->IsInstance(STANDARD_TYPE(Geom_Plane)))
    isPlane2 = Standard_True;
  else if (aSurf->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
    isEl2 = Standard_True;

  if (isPlane1 || isPlane2)
    return Standard_True;

  if (isEl1 && isEl2)
    return Standard_True;

  BOPDS_VectorOfInterfFF& aFFs = pDS->InterfFF();
  Standard_Integer aNb = aFFs.Extent();
  Standard_Integer i, j, nbe = 0;

  TopTools_SequenceOfShape Edges;
  
  for (i = 0; i < aNb; ++i) 
    {
      BOPDS_InterfFF& aFFi=aFFs(i);
      const BOPDS_VectorOfCurve& aBCurves=aFFi.Curves();
      Standard_Integer aNbCurves = aBCurves.Extent();
      
      for (j = 0; j < aNbCurves; ++j) 
	{
          const BOPDS_Curve& aBC=aBCurves(j);
          const BOPDS_ListOfPaveBlock& aSectEdges = aBC.PaveBlocks();
	  
          BOPDS_ListIteratorOfListOfPaveBlock aPBIt;
          aPBIt.Initialize(aSectEdges);
	  
	  for (; aPBIt.More(); aPBIt.Next())
	    {
              const Handle(BOPDS_PaveBlock)& aPB = aPBIt.Value();
              Standard_Integer nSect = aPB->Edge();
              const TopoDS_Edge& anEdge = *(TopoDS_Edge*)&pDS->Shape(nSect);
	      Edges.Append( anEdge );
	      nbe++;
	    }
	}
    }

  if (nbe <= 1)
    return Standard_True;

  //define tangents of RefEdge on start and on end
  BRepAdaptor_Curve cref(RefEdge);
  gp_Vec RefTangFirst = cref.DN(cref.FirstParameter(), 1);
  gp_Vec RefTangLast  = cref.DN(cref.LastParameter(), 1);

  //find the start edge and take it from Edges
  TopoDS_Edge StartEdge; //, StartEonF1, StartEonF2, EndEonF1, EndEonF2;

  TopTools_DataMapOfShapeShape VEmapF1, VEmapF2;
  FindNewVerticesOnBoundsOfFace( pDS, F1, VEmapF1 );
  FindNewVerticesOnBoundsOfFace( pDS, F2, VEmapF2 );

  Standard_Real AngTol = 0.1;
  Standard_Boolean V1onBound = Standard_False;
  Standard_Boolean V2onBound = Standard_False;
  TopoDS_Vertex V1, V2, Vcur;
  gp_Vec TangFirst, TangLast, TangCur;
  for (i = 1; i <= Edges.Length(); i++)
    {
      StartEdge = TopoDS::Edge(Edges(i));
      TopExp::Vertices( StartEdge, V1, V2 );
      V1onBound = VEmapF1.IsBound(V1) || VEmapF2.IsBound(V1); // && ?
      V2onBound = VEmapF1.IsBound(V2) || VEmapF2.IsBound(V2); // && ?
      if (V1onBound || V2onBound)
	{
	  BRepAdaptor_Curve CE(StartEdge);
	  TangFirst = CE.DN( CE.FirstParameter(), 1 );
	  TangLast = CE.DN( CE.LastParameter(), 1 );
	  if (V1onBound)
	    {
	      if (TangFirst.IsParallel( RefTangFirst, AngTol ) ||
		  TangFirst.IsParallel( RefTangLast,  AngTol ))
		break; //start edged found
	    }
	  else if (V2onBound)
	    {
	      if (TangLast.IsParallel( RefTangLast,  AngTol ) ||
		  TangLast.IsParallel( RefTangFirst, AngTol ))
		break; //start edged found
	    }
	}
    }

  if (i > Edges.Length()) //start edged not found
    return Standard_False;

  if (V1onBound && V2onBound)
    {
      if ((TangFirst.IsParallel(RefTangFirst,AngTol) && TangLast.IsParallel(RefTangLast,AngTol)) ||
          (TangFirst.IsParallel(RefTangLast,AngTol)  && TangLast.IsParallel(RefTangFirst,AngTol)))
      {
        TrueEdges.Add( Edges(i) );
        return Standard_True;
      }
      else
        return Standard_False;
    }

  //StartEonF1 = (V1onBound)? VEmapF1( V1 ) : VEmapF1( V2 );
  //StartEonF2 = (V1onBound)? VEmapF2( V1 ) : VEmapF2( V2 );

  TrueEdges.Add( Edges(i) );
  Edges.Remove(i);
  Vcur    = (V1onBound)? V2 : V1;
  TangCur = (V1onBound)? TangLast : TangFirst;
  if (V2onBound)
    TangCur.Reverse();
  
  //build the chain from StartEdge till the opposite bound of face
  for (;;)
    {
      TColStd_SequenceOfInteger Candidates;
      for (i = 1; i <= Edges.Length(); i++)
	{
	  TopoDS_Edge anEdge = TopoDS::Edge(Edges(i));
	  TopExp::Vertices( anEdge, V1, V2 );
	  if (V1.IsSame(Vcur) || V2.IsSame(Vcur))
	    Candidates.Append(i);
	}
      if (Candidates.IsEmpty())
	{
	  TrueEdges.Clear();
	  return Standard_False;
	}

      Standard_Integer minind = 1;
      if (Candidates.Length() > 1)
	{
	  Standard_Real MinAngle = RealLast();
	  for (i = 1; i <= Candidates.Length(); i++)
	    {
	      TopoDS_Edge anEdge = TopoDS::Edge(Edges(Candidates(i)));
	      TopExp::Vertices( anEdge, V1, V2 );
	      Standard_Boolean ConnectByFirst = (Vcur.IsSame(V1))? Standard_True : Standard_False;
	      BRepAdaptor_Curve CE(anEdge);
	      gp_Vec aTang = (ConnectByFirst)?
		CE.DN( CE.FirstParameter(), 1 ) : CE.DN( CE.LastParameter(), 1 );
	      if (!ConnectByFirst)
		aTang.Reverse();
	      Standard_Real anAngle = TangCur.Angle(aTang);
	      if (anAngle < MinAngle)
		{
		  MinAngle = anAngle;
		  minind = i;
		}
	    }
	}
      TopoDS_Edge CurEdge = TopoDS::Edge(Edges(Candidates(minind)));
      TrueEdges.Add( CurEdge );
      Edges.Remove(Candidates(minind));
      TopExp::Vertices( CurEdge, V1, V2 );
      Standard_Boolean ConnectByFirst = (Vcur.IsSame(V1))? Standard_True : Standard_False;
      Vcur = (ConnectByFirst)? V2 : V1;
      BRepAdaptor_Curve CE(CurEdge);
      TangCur = (ConnectByFirst)? CE.DN( CE.LastParameter(), 1 ) : CE.DN( CE.FirstParameter(), 1 );
      if (!ConnectByFirst)
	TangCur.Reverse();
      //check if Vcur is on bounds of faces
      if (VEmapF1.IsBound(Vcur) || VEmapF2.IsBound(Vcur))
	break;
    } //end of for (;;)
  
  if (TangCur.IsParallel( RefTangFirst, AngTol ) ||
      TangCur.IsParallel( RefTangLast,  AngTol ))
    return Standard_True;

  TrueEdges.Clear();
  return Standard_False;
}

//=======================================================================
//function : AssembleEdge
//purpose  : 
//=======================================================================

static TopoDS_Edge AssembleEdge(const BOPDS_PDS& pDS,
				const TopoDS_Face& F1,
				const TopoDS_Face& F2,
				const Standard_Boolean addPCurve1,
				const Standard_Boolean addPCurve2,
				const TopTools_SequenceOfShape& EdgesForConcat)
{
  TopoDS_Edge CurEdge = TopoDS::Edge( EdgesForConcat(1) );
  Standard_Real aGlueTol = Precision::Confusion();
  for (Standard_Integer j = 2; j <= EdgesForConcat.Length(); j++)
    {
      TopoDS_Edge anEdge = TopoDS::Edge( EdgesForConcat(j) );
      Standard_Boolean After = Standard_False;
      TopoDS_Vertex Vfirst, Vlast;
      if (AreClosed( CurEdge, anEdge ))
	{
	  TopoDS_Vertex V1, V2;
	  TopExp::Vertices( CurEdge, V1, V2 );
          if (IsAutonomVertex( V1, pDS ))
	    {
	      After = Standard_False;
	      Vfirst = Vlast = V2;
	    }
	  else
	    {
	      After = Standard_True;
	      Vfirst = Vlast = V1;
	    }
	}
      else
	{
	  TopoDS_Vertex CV, V11, V12, V21, V22;
	  TopExp::CommonVertex( CurEdge, anEdge, CV );
          aGlueTol = BRep_Tool::Tolerance(CV);
	  TopExp::Vertices( CurEdge, V11, V12 );
	  TopExp::Vertices( anEdge,  V21, V22 );
	  if (V11.IsSame(CV) && V21.IsSame(CV))
	    {
	      Vfirst = V22;
	      Vlast  = V12;
	    }
	  else if (V11.IsSame(CV) && V22.IsSame(CV))
	    {
	      Vfirst = V21;
	      Vlast  = V12;
	    }
	  else if (V12.IsSame(CV) && V21.IsSame(CV))
	    {
	      Vfirst = V11;
	      Vlast  = V22;
	    }
	  else
	    {
	      Vfirst = V11;
	      Vlast  = V21;
	    }
	} //end of else (open wire)
      
      TopoDS_Edge NewEdge = Glue(CurEdge, anEdge, Vfirst, Vlast, After,
                                 F1, addPCurve1, F2, addPCurve2, aGlueTol);
      CurEdge = NewEdge;
    } //end of for (Standard_Integer j = 2; j <= EdgesForConcat.Length(); j++)
  
  return CurEdge;
}

//=======================================================================
//function : Inter3D
//purpose  : 
//=======================================================================

void BRepOffset_Tool::Inter3D(const TopoDS_Face& F1,
			      const TopoDS_Face& F2,
			      TopTools_ListOfShape& L1,
			      TopTools_ListOfShape& L2,
			      const TopAbs_State    Side,
			      const TopoDS_Edge&     RefEdge,
			      const Standard_Boolean IsRefEdgeDefined)
{
#ifdef DRAW
  if (AffichInter) {
    char name[256];
    sprintf(name,"FF_%d",NbFaces++);
    DBRep::Set(name,F1);
    sprintf(name,"FF_%d",NbFaces++);
    DBRep::Set(name,F2);
  }
#endif

  // Check if the faces are planar and not trimmed - in this case
  // the IntTools_FaceFace intersection algorithm will be used directly.
  BRepAdaptor_Surface aBAS1(F1, Standard_False), aBAS2(F2, Standard_False);
  if (aBAS1.GetType() == GeomAbs_Plane &&
      aBAS2.GetType() == GeomAbs_Plane) {
    aBAS1.Initialize(F1, Standard_True);
    if (IsInf(aBAS1.LastUParameter()) && IsInf(aBAS1.LastVParameter())) {
      aBAS2.Initialize(F2, Standard_True);
      if (IsInf(aBAS2.LastUParameter()) && IsInf(aBAS2.LastVParameter())) {
        // Intersect the planes without pave filler
        PerformPlanes(F1, F2, Side, L1, L2);
        return;
      }
    }
  }
  //
  TopoDS_Face cpF1=F1; 
  TopoDS_Face cpF2=F2;
  // create 3D curves on faces
  BRepLib::BuildCurves3d(cpF1);
  BRepLib::BuildCurves3d(cpF2);
 
  BOPAlgo_PaveFiller aPF1, aPF2;
  BOPCol_ListOfShape aLS;
  aLS.Append(cpF1);
  aLS.Append(cpF2);
  aPF1.SetArguments(aLS);
  //
  aPF1.Perform();
  
  BOPAlgo_PaveFiller *pPF = &aPF1;

  aLS.Clear();
  TopTools_IndexedMapOfShape TrueEdges;
  if (IsRefEdgeDefined && !CheckIntersFF( pPF->PDS(), RefEdge, cpF1, cpF2, TrueEdges ))
    {
      cpF1 = F2;
      cpF2 = F1;
      aLS.Append(cpF1);
      aLS.Append(cpF2);
      aPF2.SetArguments(aLS);
      aPF2.Perform();
      pPF = &aPF2;
      CheckIntersFF( pPF->PDS(), RefEdge, cpF1, cpF2, TrueEdges );
   }

  Standard_Boolean addPCurve1 = 1;
  Standard_Boolean addPCurve2 = 1;
  
  const BOPDS_PDS& pDS = pPF->PDS();
  BOPDS_VectorOfInterfFF& aFFs=pDS->InterfFF();
  Standard_Integer aNb = aFFs.Extent();
  Standard_Integer i = 0, j = 0, k;
  // Store Result
  L1.Clear(); L2.Clear();
  TopAbs_Orientation O1,O2;
  //
  const Handle(IntTools_Context)& aContext = pPF->Context();
  //
  for (i = 0; i < aNb; i++) {
    BOPDS_InterfFF& aFFi=aFFs(i);
    const BOPDS_VectorOfCurve& aBCurves=aFFi.Curves();
        
    Standard_Integer aNbCurves = aBCurves.Extent();
      
    for (j = 0; j < aNbCurves; j++) {
      const BOPDS_Curve& aBC=aBCurves(j);
      const BOPDS_ListOfPaveBlock& aSectEdges = aBC.PaveBlocks();
      
      BOPDS_ListIteratorOfListOfPaveBlock aPBIt;
      aPBIt.Initialize(aSectEdges);
      
      for (; aPBIt.More(); aPBIt.Next()) {
        const Handle(BOPDS_PaveBlock)& aPB = aPBIt.Value();
        Standard_Integer nSect = aPB->Edge();
        const TopoDS_Edge& anEdge = *(TopoDS_Edge*)&pDS->Shape(nSect);
	if (!TrueEdges.IsEmpty() && !TrueEdges.Contains(anEdge))
	  continue;
        
        Standard_Real f, l;
	const Handle(Geom_Curve)& aC3DE = BRep_Tool::Curve(anEdge, f, l);
	Handle(Geom_TrimmedCurve) aC3DETrim;
	    
	if(!aC3DE.IsNull()) 
            aC3DETrim = new Geom_TrimmedCurve(aC3DE, f, l);
        
        BRep_Builder aBB;
	Standard_Real aTolEdge = BRep_Tool::Tolerance(anEdge);
	        
        if (!BOPTools_AlgoTools2D::HasCurveOnSurface(anEdge, cpF1)) {
          Handle(Geom2d_Curve) aC2d = aBC.Curve().FirstCurve2d();
          if(!aC3DETrim.IsNull()) {
		Handle(Geom2d_Curve) aC2dNew;
		
		if(aC3DE->IsPeriodic()) {
                  BOPTools_AlgoTools2D::AdjustPCurveOnFace(cpF1, f, l,  aC2d, aC2dNew, aContext);
		  }
		else {
                  BOPTools_AlgoTools2D::AdjustPCurveOnFace(cpF1, aC3DETrim, aC2d, aC2dNew, aContext); 
		  }
		aC2d = aC2dNew;
	      }
	      aBB.UpdateEdge(anEdge, aC2d, cpF1, aTolEdge);
        }
        
        if (!BOPTools_AlgoTools2D::HasCurveOnSurface(anEdge, cpF2)) {
          Handle(Geom2d_Curve) aC2d = aBC.Curve().SecondCurve2d();
          if(!aC3DETrim.IsNull()) {
		Handle(Geom2d_Curve) aC2dNew;
		
		if(aC3DE->IsPeriodic()) {
                  BOPTools_AlgoTools2D::AdjustPCurveOnFace(cpF2, f, l,  aC2d, aC2dNew, aContext);
		  }
		else {
                  BOPTools_AlgoTools2D::AdjustPCurveOnFace(cpF2, aC3DETrim, aC2d, aC2dNew, aContext); 
		  }
		aC2d = aC2dNew;
	      }
	      aBB.UpdateEdge(anEdge, aC2d, cpF2, aTolEdge);
        }
         
        OrientSection (anEdge, F1, F2, O1, O2);
        if (Side == TopAbs_OUT) {
          O1 = TopAbs::Reverse(O1);
          O2 = TopAbs::Reverse(O2);
        }
        
        L1.Append (anEdge.Oriented(O1));
        L2.Append (anEdge.Oriented(O2));
        
#ifdef DRAW
        if (AffichInter) {
	  char name[256];
          sprintf(name,"EI_%d",NbNewEdges++);	
          DBRep::Set(name,anEdge.Oriented(O1));
         
        }
#endif       
      }
    }
  }

  Standard_Real aSameParTol = Precision::Confusion();
  Standard_Boolean isEl1 = Standard_False, isEl2 = Standard_False;

  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(cpF1);
  if (aSurf->IsInstance(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    aSurf = Handle(Geom_RectangularTrimmedSurface)::DownCast (aSurf)->BasisSurface();
  if (aSurf->IsInstance(STANDARD_TYPE(Geom_Plane)))
    addPCurve1 = Standard_False;
  else if (aSurf->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
    isEl1 = Standard_True;

  aSurf = BRep_Tool::Surface(cpF2);
  if (aSurf->IsInstance(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    aSurf = Handle(Geom_RectangularTrimmedSurface)::DownCast (aSurf)->BasisSurface();
  if (aSurf->IsInstance(STANDARD_TYPE(Geom_Plane)))
    addPCurve2 = Standard_False;
  else if (aSurf->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
    isEl2 = Standard_True;

  if (L1.Extent() > 1 && (!isEl1 || !isEl2)) {
    TopTools_SequenceOfShape eseq;
    TopTools_SequenceOfShape EdgesForConcat;

    if (!TrueEdges.IsEmpty())
      {
	for (i = TrueEdges.Extent(); i >= 1; i--)
	  EdgesForConcat.Append( TrueEdges(i) );
	TopoDS_Edge theEdge =
          AssembleEdge( pDS, cpF1, cpF2, addPCurve1, addPCurve2, EdgesForConcat );
	eseq.Append(theEdge);
      }
    else
      {
	
	TopTools_SequenceOfShape wseq;
	TopTools_SequenceOfShape edges;
	TopTools_ListIteratorOfListOfShape itl(L1);
	for (; itl.More(); itl.Next())
	  edges.Append( itl.Value() );
	while (!edges.IsEmpty())
	  {
	    TopoDS_Edge anEdge = TopoDS::Edge( edges.First() );
	    TopoDS_Wire aWire = BRepLib_MakeWire( anEdge ), resWire;
	    TColStd_SequenceOfInteger Candidates;
	    for (k = 1; k <= wseq.Length(); k++)
	      {
		resWire = TopoDS::Wire(wseq(k));
                if (AreConnex( resWire, aWire, pDS ))
		  {
		    Candidates.Append( 1 );
		    break;
		  }
	      }
	    if (Candidates.IsEmpty())
	      {
		wseq.Append( aWire );
		edges.Remove(1);
	      }
	    else
	      {
		for (j = 2; j <= edges.Length(); j++)
		  {
		    anEdge = TopoDS::Edge( edges(j) );
		    //if (anEdge.IsSame(edges(Candidates.First())))
		    //continue;
		    aWire = BRepLib_MakeWire( anEdge );
                    if (AreConnex( resWire, aWire, pDS ))
		      Candidates.Append( j );
		  }
		Standard_Integer minind = 1;
		if (Candidates.Length() > 1)
		  {
		    Standard_Real MinAngle = RealLast();
		    for (j = 1; j <= Candidates.Length(); j++)
		      {
			anEdge = TopoDS::Edge( edges(Candidates(j)) );
			Standard_Real anAngle = AngleWireEdge( resWire, anEdge );
			if (anAngle < MinAngle)
			  {
			    MinAngle = anAngle;
			    minind = j;
			  }
		      }
		  }
		TopoDS_Wire NewWire = BRepLib_MakeWire( resWire, TopoDS::Edge(edges(Candidates(minind))) );
		wseq(k) = NewWire;
		edges.Remove(Candidates(minind));
	      }
	  } //end of while (!edges.IsEmpty())
	
	for (i = 1; i <= wseq.Length(); i++)
	  {
	    TopoDS_Wire aWire = TopoDS::Wire(wseq(i));
	    TopTools_SequenceOfShape aLocalEdgesForConcat;
	    if (aWire.Closed())
	      {
		TopoDS_Vertex StartVertex;
		TopoDS_Edge StartEdge;
		Standard_Boolean StartFound = Standard_False;
		TopTools_ListOfShape Elist;
		
		TopoDS_Iterator itw(aWire);
		for (; itw.More(); itw.Next())
		  {
		    TopoDS_Edge anEdge = TopoDS::Edge(itw.Value());
		    if (StartFound)
		      Elist.Append(anEdge);
		    else
		      {
			TopoDS_Vertex V1, V2;
			TopExp::Vertices( anEdge, V1, V2 );
                        if (!IsAutonomVertex( V1, pDS ))
			  {
			    StartVertex = V2;
			    StartEdge = anEdge;
			    StartFound = Standard_True;
			  }
                        else if (!IsAutonomVertex( V2, pDS ))
			  {
			    StartVertex = V1;
			    StartEdge = anEdge;
			    StartFound = Standard_True;
			  }
			else
			  Elist.Append(anEdge);
		      }
		  } //end of for (; itw.More(); itw.Next())
		if (!StartFound)
		  {
		    itl.Initialize(Elist);
		    StartEdge = TopoDS::Edge(itl.Value());
		    Elist.Remove(itl);
		    TopoDS_Vertex V1, V2;
		    TopExp::Vertices( StartEdge, V1, V2 );
		    StartVertex = V1;
		  }
                aLocalEdgesForConcat.Append( StartEdge );
		while (!Elist.IsEmpty())
		  {
		    for (itl.Initialize(Elist); itl.More(); itl.Next())
		      {
			TopoDS_Edge anEdge = TopoDS::Edge(itl.Value());
			TopoDS_Vertex V1, V2;
			TopExp::Vertices( anEdge, V1, V2 );
			if (V1.IsSame(StartVertex))
			  {
			    StartVertex = V2;
                            aLocalEdgesForConcat.Append( anEdge );
			    Elist.Remove(itl);
			    break;
			  }
			else if (V2.IsSame(StartVertex))
			  {
			    StartVertex = V1;
                            aLocalEdgesForConcat.Append( anEdge );
			    Elist.Remove(itl);
			    break;
			  }
		      }
		  } //end of while (!Elist.IsEmpty())
	      } //end of if (aWire.Closed())
	      else
	      {
		BRepTools_WireExplorer Wexp( aWire );
		for (; Wexp.More(); Wexp.Next())
                  aLocalEdgesForConcat.Append( Wexp.Current() );
	      }
	    
	    TopoDS_Edge theEdge =
              AssembleEdge( pDS, cpF1, cpF2, addPCurve1, addPCurve2, aLocalEdgesForConcat );
	    eseq.Append( theEdge );
	  }
      } //end of else (when TrueEdges is empty)
    
    if (eseq.Length() < L1.Extent())
      {
	L1.Clear();
	L2.Clear();
	for (i = 1; i <= eseq.Length(); i++)
	  {
	    TopoDS_Edge anEdge = TopoDS::Edge(eseq(i));
	    BRepLib::SameParameter(anEdge, aSameParTol, Standard_True);
	    Standard_Real EdgeTol = BRep_Tool::Tolerance(anEdge);
#ifdef OCCT_DEBUG
	    cout<<"Tolerance of glued E =      "<<EdgeTol<<endl;
#endif
	    if (EdgeTol > 1.e-2)
	      continue;

	    if (EdgeTol >= 1.e-4)
	      {
		ReconstructPCurves(anEdge);
		BRepLib::SameParameter(anEdge, aSameParTol, Standard_True);
#ifdef OCCT_DEBUG
	        cout<<"After projection tol of E = "<<BRep_Tool::Tolerance(anEdge)<<endl;
#endif
	      }

	    OrientSection( anEdge, F1, F2, O1, O2 );
	    if (Side == TopAbs_OUT)
	      {
		O1 = TopAbs::Reverse(O1);
		O2 = TopAbs::Reverse(O2);
	      }
	    
	    L1.Append( anEdge.Oriented(O1) );
	    L2.Append( anEdge.Oriented(O2) );
	  }
      }
  } //end of if (L1.Extent() > 1)

  else
    {
      TopTools_ListIteratorOfListOfShape itl(L1);
      for (; itl.More(); itl.Next())
	{
	  const TopoDS_Edge& anEdge = TopoDS::Edge( itl.Value() );
	  BRepLib::SameParameter(anEdge, aSameParTol, Standard_True);
	}
    }
}

//=======================================================================
//function : TryProject
//purpose  : 
//=======================================================================

Standard_Boolean BRepOffset_Tool::TryProject
(const TopoDS_Face&          F1,
 const TopoDS_Face&          F2,
 const TopTools_ListOfShape& Edges,
       TopTools_ListOfShape& LInt1,
       TopTools_ListOfShape& LInt2,
 const TopAbs_State          Side,
 const Standard_Real         TolConf)
{
#ifdef DRAW
  if (AffichInter) {
    char name[256];
    sprintf(name,"FF_%d",NbFaces++);
    DBRep::Set(name,F1);
    sprintf(name,"FF_%d",NbFaces++);
    DBRep::Set(name,F2);
  }
#endif

  // try to find if the edges <Edges> are laying on the face F1.
  LInt1.Clear(); LInt2.Clear();
  TopTools_ListIteratorOfListOfShape it(Edges);
  Standard_Boolean     isOk = Standard_True;
  Standard_Boolean     Ok   = Standard_True;
  TopAbs_Orientation   O1,O2;
  Handle(Geom_Surface) Bouchon = BRep_Tool::Surface(F1);
  BRep_Builder B;

  for ( ; it.More(); it.Next()) {
    TopLoc_Location L;
    Standard_Real f,l;
    TopoDS_Edge CurE     = TopoDS::Edge(it.Value());
    Handle(Geom_Curve) C = BRep_Tool::Curve(CurE,L,f,l);
    if (C.IsNull()) {
      BRepLib::BuildCurve3d(CurE,BRep_Tool::Tolerance(CurE));
      C  = BRep_Tool::Curve(CurE,L,f,l);
    }
    C = new Geom_TrimmedCurve(C,f,l);
    if ( !L.IsIdentity()) C->Transform(L);
    Standard_Real TolReached;
    isOk = IsOnSurface(C,Bouchon,TolConf,TolReached);
    
    if ( isOk) {
      B.UpdateEdge(CurE,TolReached);
      BuildPCurves(CurE,F1);
      OrientSection (CurE,F1,F2,O1,O2);
      if (Side == TopAbs_OUT) {
	O1 = TopAbs::Reverse(O1);
	O2 = TopAbs::Reverse(O2);
      }
      LInt1.Append (CurE.Oriented(O1));
      LInt2.Append (CurE.Oriented(O2));
#ifdef DRAW
      if (AffichInter) {
        char name[256];
        sprintf(name,"EI_%d",NbNewEdges++);	
	DBRep::Set(name,CurE.Oriented(O1));
      }
#endif      
    }
    else 
      Ok = Standard_False;
  }
  return Ok;
}


//=======================================================================
//function : InterOrExtent
//purpose  : 
//=======================================================================

void BRepOffset_Tool::InterOrExtent(const TopoDS_Face& F1,
				    const TopoDS_Face& F2,
				    TopTools_ListOfShape& L1,
				    TopTools_ListOfShape& L2,
				    const TopAbs_State    Side)
{
#ifdef DRAW
  if (AffichInter) {
    char name[256];
    sprintf(name,"FF_%d",NbFaces++);
    DBRep::Set(name,F1);
    sprintf(name,"FF_%d",NbFaces++);
    DBRep::Set(name,F2);
  }
#endif

  Handle (Geom_Curve) CI;
  TopAbs_Orientation O1,O2;
  L1.Clear(); L2.Clear();
  Handle (Geom_Surface) S1 = BRep_Tool::Surface(F1);
  Handle (Geom_Surface) S2 = BRep_Tool::Surface(F2);  

  if (S1->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    Handle(Geom_RectangularTrimmedSurface) RTS ;
    RTS = Handle(Geom_RectangularTrimmedSurface)::DownCast (S1);
    if (RTS->BasisSurface()->DynamicType() == STANDARD_TYPE(Geom_Plane)) {
      S1 = RTS->BasisSurface();
    }
  }
  if (S2->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    Handle(Geom_RectangularTrimmedSurface) RTS ;
    RTS = Handle(Geom_RectangularTrimmedSurface)::DownCast (S2);
    if (RTS->BasisSurface()->DynamicType() == STANDARD_TYPE(Geom_Plane)) {
      S2 = RTS->BasisSurface();
    }
  }

  GeomInt_IntSS Inter  (S1,S2, Precision::Confusion());

  if (Inter.IsDone()) {
    for (Standard_Integer i = 1; i <= Inter.NbLines(); i++) {
      CI = Inter.Line(i);
      
      if (ToSmall(CI)) continue;
      TopoDS_Edge E = BRepLib_MakeEdge(CI);
      BuildPCurves (E,F1);
      BuildPCurves (E,F2);
      OrientSection (E,F1,F2,O1,O2);
      if (Side == TopAbs_OUT) {
	O1 = TopAbs::Reverse(O1);
	O2 = TopAbs::Reverse(O2);
      }
      L1.Append (E.Oriented(O1));
      L2.Append (E.Oriented(O2));
#ifdef DRAW
      if (AffichInter) {
        char name[256];
	sprintf(name,"EI_%d",NbNewEdges++);	
	DBRep::Set(name,E.Oriented(O1));
      }
#endif      
    }
  }
}

//=======================================================================
//function : ExtentEdge
//purpose  : 
//=======================================================================

static void ExtentEdge(const TopoDS_Face& F,
		       const TopoDS_Face& EF,
		       const TopoDS_Edge& E,
		       TopoDS_Edge&       NE)
{
  BRepAdaptor_Curve CE(E);
  GeomAbs_CurveType Type = CE.GetType();
  TopoDS_Shape aLocalEdge = E.EmptyCopied();
  NE = TopoDS::Edge(aLocalEdge); 
//  NE = TopoDS::Edge(E.EmptyCopied()); 

  if (Type == GeomAbs_Line || Type == GeomAbs_Circle || Type == GeomAbs_Ellipse || 
      Type == GeomAbs_Hyperbola || Type == GeomAbs_Parabola) {
    return;
  }
  // Extension en tangence jusqu'au bord de la surface.
  Standard_Real   PMax = 1.e2;
  TopLoc_Location L;
  Handle(Geom_Surface) S = BRep_Tool::Surface(F,L);
  Standard_Real umin,umax,vmin,vmax;

  S->Bounds(umin,umax,vmin,vmax);
  umin = Max(umin,-PMax);vmin = Max(vmin,-PMax);
  umax = Min(umax, PMax);vmax = Min(vmax, PMax);

 
  Standard_Real f,l;
  Handle(Geom2d_Curve) C2d = BRep_Tool::CurveOnSurface(E,F,f,l);

  //calcul point cible . ie point d'intersection du prolongement tangent et des bords.
  gp_Pnt2d P;
  gp_Vec2d Tang;
  C2d->D1(CE.FirstParameter(),P,Tang);
  Standard_Real tx,ty,tmin;
  tx = ty = Precision::Infinite();
  if (Abs(Tang.X()) > Precision::Confusion())
    tx = Min (Abs((umax - P.X())/Tang.X()), Abs((umin - P.X())/Tang.X()));
  if (Abs(Tang.Y()) > Precision::Confusion())
    ty = Min (Abs((vmax - P.Y())/Tang.Y()), Abs((vmin - P.Y())/Tang.Y()));
  tmin = Min (tx,ty);
  Tang = tmin*Tang;
  gp_Pnt2d  PF2d (P.X() - Tang.X(),P.Y() - Tang.Y());

  C2d->D1(CE.LastParameter(),P,Tang);
  tx = ty = Precision::Infinite();
  if (Abs(Tang.X()) > Precision::Confusion())
    tx = Min (Abs((umax - P.X())/Tang.X()), Abs((umin - P.X())/Tang.X()));
  if (Abs(Tang.Y()) > Precision::Confusion())
    ty = Min (Abs((vmax - P.Y())/Tang.Y()), Abs((vmin - P.Y())/Tang.Y()));
  tmin = Min (tx,ty);
  Tang = tmin*Tang;
  gp_Pnt2d  PL2d (P.X() + Tang.X(),P.Y() + Tang.Y());

  Handle(Geom_Curve) CC = GeomAPI::To3d(C2d,gp_Pln(gp::XOY()));
  gp_Pnt PF(PF2d.X(),PF2d.Y(),0.);
  gp_Pnt PL(PL2d.X(),PL2d.Y(),0.);

  Handle(Geom_BoundedCurve) ExtC = Handle(Geom_BoundedCurve)::DownCast(CC);
  if (ExtC.IsNull()) return;

  GeomLib::ExtendCurveToPoint(ExtC,PF,1,0);
  GeomLib::ExtendCurveToPoint(ExtC,PL,1,1); 

  Handle(Geom2d_Curve) CNE2d = GeomAPI::To2d(ExtC,gp_Pln(gp::XOY()));

  //Construction de la nouvelle arrete;
  BRep_Builder B;
  B.MakeEdge(NE);
//  B.UpdateEdge (NE,CNE2d,F,BRep_Tool::Tolerance(E));
  B.UpdateEdge (NE,CNE2d,EF,BRep_Tool::Tolerance(E));
  B.Range (NE,CNE2d->FirstParameter(), CNE2d->LastParameter());
  NE.Orientation(E.Orientation());
#ifdef DRAW
  if (AffichExtent) {
    char name[256];
    sprintf (name,"F_%d",NbExtE);
    DBRep::Set(name,EF);
    sprintf (name,"OE_%d",NbExtE);
    DBRep::Set (name,E);
    sprintf (name,"ExtE_%d",NbExtE++);
    DBRep::Set(name,NE);
  }
#endif
}

//=======================================================================
//function : ProjectVertexOnEdge
//purpose  : 
//=======================================================================

static Standard_Boolean  ProjectVertexOnEdge(TopoDS_Vertex&     V,
					     const TopoDS_Edge& E,
					     Standard_Real      TolConf)
{ 
#ifdef DRAW
  if (AffichExtent) {
    DBRep::Set("V",V); 
    DBRep::Set("E",E);
  }
#endif
  BRep_Builder    B;
  Standard_Real   f,l;
  Standard_Real U = 0.;
  TopLoc_Location L;
  Standard_Boolean found = Standard_False;

  gp_Pnt            P = BRep_Tool::Pnt  (V);  
  BRepAdaptor_Curve C = BRepAdaptor_Curve(E);
  f = C.FirstParameter(); l = C.LastParameter();

  if (V.Orientation() == TopAbs_FORWARD) {
    if (Abs(f) < Precision::Infinite()) { 
      gp_Pnt PF = C.Value (f);
      if (PF.IsEqual(P,TolConf)) {
	U = f;
	found = Standard_True;
      }
    }
  }
  if (V.Orientation() == TopAbs_REVERSED) {
    if (!found && Abs(l) < Precision::Infinite()) {
      gp_Pnt PL = C.Value (l);
      if (PL.IsEqual(P,TolConf)) {
	U = l;
	found = Standard_True;
      }
    }
  }
  if (!found) {
    Extrema_ExtPC Proj(P,C);
    if (Proj.IsDone() && Proj.NbExt() > 0) {
      Standard_Real Dist2,Dist2Min = Proj.SquareDistance(1);
      U = Proj.Point(1).Parameter();
      for (Standard_Integer i = 2; i <= Proj.NbExt(); i++) {
	Dist2 = Proj.SquareDistance(i);
	if (Dist2 < Dist2Min) {
	  Dist2Min = Dist2;
	  U = Proj.Point(i).Parameter();
	}
      }
      found = Standard_True;
    }
  }

#ifdef OCCT_DEBUG
  if (AffichExtent) {
    Standard_Real Dist = P.Distance(C.Value(U));
    if (Dist > TolConf) {
      cout << " ProjectVertexOnEdge :distance vertex edge :"<<Dist<<endl;
    }
    if (U < f - Precision::Confusion() || 
	U > l + Precision::Confusion()) {
      cout << " ProjectVertexOnEdge : hors borne :"<<endl;
      cout << " f = "<<f<<" l ="<<l<< " U ="<<U<<endl;
    }
  }
  if (!found) {
    cout <<"BRepOffset_Tool::ProjectVertexOnEdge Parameter no found"<<endl;
    if (Abs(f) < Precision::Infinite() && 
	Abs(l) < Precision::Infinite()) {
#ifdef DRAW
      DBRep::Set("E",E);
#endif
    }
  }    
#endif 
  if (found) {
    TopoDS_Shape aLocalShape = E.Oriented(TopAbs_FORWARD);
    TopoDS_Edge EE = TopoDS::Edge(aLocalShape);
    aLocalShape = V.Oriented(TopAbs_INTERNAL);
//    TopoDS_Edge EE = TopoDS::Edge(E.Oriented(TopAbs_FORWARD));
    B.UpdateVertex(TopoDS::Vertex(aLocalShape),
		   U,EE,BRep_Tool::Tolerance(E));
    
  }
  return found;
}

//=======================================================================
//function : Inter2d					
//purpose  : 
//=======================================================================

void BRepOffset_Tool::Inter2d (const TopoDS_Face&    F,
			       const TopoDS_Edge&    E1,
			       const TopoDS_Edge&    E2,
			       TopTools_ListOfShape& LV,
			       const Standard_Real   TolConf) 
{
#ifdef DRAW
  if (AffichExtent) {
    DBRep::Set("E1",E1); 
    DBRep::Set("E2",E2);
    DBRep::Set("F",F);
  }
#endif
  BRep_Builder  B;
  Standard_Real fl1[2],fl2[2];
  LV.Clear();
  
  // Si l edge a ete etendu les pcurves ne sont pas forcement 
  // a jour.
  BuildPCurves(E1,F);
  BuildPCurves(E2,F);
  

  // Construction des curves 3d si elles n existent pas 
  // utile pour coder correctement les parametres des vertex 
  // d intersection sur les edges.
  //TopLoc_Location L;
  //Standard_Real   f,l;
  //Handle(Geom_Curve) C3d1 = BRep_Tool::Curve(E1,L,f,l);
  //if (C3d1.IsNull()) {
  //  BRepLib::BuildCurve3d(E1,BRep_Tool::Tolerance(E1));
  //}
  //Handle(Geom_Curve) C3d2 = BRep_Tool::Curve(E2,L,f,l);
  //if (C3d2.IsNull()) {
  //  BRepLib::BuildCurve3d(E2,BRep_Tool::Tolerance(E2));
  //}
  
  Standard_Integer NbPC1 = 1, NbPC2 = 1;
  if (BRep_Tool::IsClosed(E1,F)) NbPC1++;
  if (BRep_Tool::IsClosed(E2,F)) NbPC2++;
  
  Handle(Geom_Surface) S  = BRep_Tool::Surface(F);
  Handle(Geom2d_Curve) C1, C2;
  Standard_Boolean YaSol = Standard_False;
  Standard_Integer itry  = 0;

  while (!YaSol && itry < 2) {
    for ( Standard_Integer i = 1; i <= NbPC1 ; i++) {
	TopoDS_Shape aLocalEdgeReversedE1 = E1.Reversed();
      if (i == 1)  C1 = BRep_Tool::CurveOnSurface(E1,F,fl1[0],fl1[1]);
      else         C1 = BRep_Tool::CurveOnSurface(TopoDS::Edge(aLocalEdgeReversedE1),
						  F,fl1[0],fl1[1]);
//      if (i == 1)  C1 = BRep_Tool::CurveOnSurface(E1,F,fl1[0],fl1[1]);
//     else         C1 = BRep_Tool::CurveOnSurface(TopoDS::Edge(E1.Reversed()),
//						  F,fl1[0],fl1[1]);
      for ( Standard_Integer j = 1; j <= NbPC2; j++ ) {
	TopoDS_Shape aLocalEdge = E2.Reversed();
	if (j == 1)  C2 = BRep_Tool::CurveOnSurface(E2,F,fl2[0],fl2[1]);
	else         C2 = BRep_Tool::CurveOnSurface(TopoDS::Edge(aLocalEdge),
						    F,fl2[0],fl2[1]);
//	if (j == 1)  C2 = BRep_Tool::CurveOnSurface(E2,F,fl2[0],fl2[1]);
//	else         C2 = BRep_Tool::CurveOnSurface(TopoDS::Edge(E2.Reversed()),
//						    F,fl2[0],fl2[1]);
#ifdef OCCT_DEBUG
	if (C1.IsNull() || C2.IsNull()) {
	  cout <<"Inter2d : Pas de pcurve"<<endl;
#ifdef DRAW
	  DBRep::Set("E1",E1); 
	  DBRep::Set("E2",E2);
	  DBRep::Set("F",F);
#endif
	  return;
	}
#endif
	Standard_Real    U1 = 0.,U2 = 0.;
	gp_Pnt2d         P2d;	
	if (itry == 1) {
	  fl1[0] = C1->FirstParameter(); fl1[1] = C1->LastParameter();
	  fl2[0] = C2->FirstParameter(); fl2[1] = C2->LastParameter();
	}
	Geom2dAdaptor_Curve   AC1(C1,fl1[0],fl1[1]);
	Geom2dAdaptor_Curve   AC2(C2,fl2[0],fl2[1]);

	if (itry == 0) {
	  gp_Pnt2d P1[2],P2[2];
	  P1[0] = C1->Value(fl1[0]); P1[1] = C1->Value(fl1[1]);
	  P2[0] = C2->Value(fl2[0]); P2[1] = C2->Value(fl2[1]);

	  Standard_Integer i1 ;
	  for ( i1 = 0; i1 < 2; i1++) {
	    for (Standard_Integer i2 = 0; i2 < 2; i2++) {
	      if (Abs(fl1[i1]) < Precision::Infinite() &&
		  Abs(fl2[i2]) < Precision::Infinite()   ) {
		if (P1[i1].IsEqual(P2[i2],TolConf)) {
		  YaSol = Standard_True;
		  U1  = fl1[i1]; U2 = fl2[i2];
		  P2d = C1->Value(U1);
		}
	      }
	    }
	  }
	  if (!YaSol)
	    for (i1 = 0; i1 < 2; i1++)
	      {
		Extrema_ExtPC2d extr( P1[i1], AC2 );
		if (extr.IsDone() && extr.NbExt() > 0)
		  {
		    Standard_Real Dist2, Dist2Min = extr.SquareDistance(1);
		    Standard_Integer IndexMin = 1;
		    for (Standard_Integer ind = 2; ind <= extr.NbExt(); ind++)
		      {
			Dist2 = extr.SquareDistance(ind);
			if (Dist2 < Dist2Min)
			  {
			    Dist2Min = Dist2;
			    IndexMin = ind;
			  }
		      }
            if (Dist2Min <= Precision::SquareConfusion())
		      {
			YaSol = Standard_True;
			P2d = P1[i1];
			U1 = fl1[i1];
			U2 = (extr.Point(IndexMin)).Parameter();
			break;
		      }
		  }
	      }
	  if (!YaSol)
	    for (Standard_Integer i2 = 0; i2 < 2; i2++)
	      {
		Extrema_ExtPC2d extr( P2[i2], AC1 );
		if (extr.IsDone() && extr.NbExt() > 0)
		  {
		    Standard_Real Dist2, Dist2Min = extr.SquareDistance(1);
		    Standard_Integer IndexMin = 1;
		    for (Standard_Integer ind = 2; ind <= extr.NbExt(); ind++)
		      {
			Dist2 = extr.SquareDistance(ind);
			if (Dist2 < Dist2Min)
			  {
			    Dist2Min = Dist2;
			    IndexMin = ind;
			  }
		      }
            if (Dist2Min <= Precision::SquareConfusion())
		      {
			YaSol = Standard_True;
			P2d = P2[i2];
			U2 = fl2[i2];
			U1 = (extr.Point(IndexMin)).Parameter();
			break;
		      }
		  }
	      }
	}
	
	if (!YaSol) {
	  Geom2dInt_GInter Inter (AC1,AC2,TolConf,TolConf);
	  
	  if (!Inter.IsEmpty() && Inter.NbPoints() > 0) {
	    YaSol = Standard_True;
	    U1  = Inter.Point(1).ParamOnFirst();
	    U2  = Inter.Point(1).ParamOnSecond();
	    P2d = Inter.Point(1).Value();
	  }
	  else if (!Inter.IsEmpty() && Inter.NbSegments() > 0) {
	    YaSol = Standard_True;
	    IntRes2d_IntersectionSegment Seg = Inter.Segment(1);
	    IntRes2d_IntersectionPoint IntP1 = Seg.FirstPoint();
	    IntRes2d_IntersectionPoint IntP2 = Seg.LastPoint();
	    Standard_Real U1on1 = IntP1.ParamOnFirst();
	    Standard_Real U1on2 = IntP2.ParamOnFirst();
	    Standard_Real U2on1 = IntP1.ParamOnSecond();
	    Standard_Real U2on2 = IntP2.ParamOnSecond();
#ifdef OCCT_DEBUG
	    cout << " BRepOffset_Tool::Inter2d SEGMENT d intersection" << endl;
	    cout << "     ===> Parametres sur Curve1 : ";
	    cout << U1on1 << " " << U1on2 << endl;
	    cout << "     ===> Parametres sur Curve2 : ";
	    cout << U2on1 << " " << U2on2 << endl;
#endif
	    U1 = (U1on1 + U1on2)/2.;
	    U2 = (U2on1 + U2on2)/2.;
	    gp_Pnt2d P2d1 = C1->Value(U1);
	    gp_Pnt2d P2d2 = C2->Value(U2);
	    P2d.SetX( (P2d1.X() + P2d2.X()) / 2.);
	    P2d.SetY( (P2d1.Y() + P2d2.Y()) / 2.);
	  }
	}
	if (YaSol) {
	  gp_Pnt        P   = S->Value(P2d.X(),P2d.Y());
	  TopoDS_Vertex V = BRepLib_MakeVertex(P);
	  V.Orientation(TopAbs_INTERNAL);
	  TopoDS_Shape aLocalEdgeOrientedE1 = E1.Oriented(TopAbs_FORWARD);
	  B.UpdateVertex(V,U1,TopoDS::Edge(aLocalEdgeOrientedE1),TolConf);
          aLocalEdgeOrientedE1 = E2.Oriented(TopAbs_FORWARD);
	  B.UpdateVertex(V,U2,TopoDS::Edge(aLocalEdgeOrientedE1),TolConf);
//	  B.UpdateVertex(V,U1,TopoDS::Edge(E1.Oriented(TopAbs_FORWARD)),TolConf);
//	  B.UpdateVertex(V,U2,TopoDS::Edge(E2.Oriented(TopAbs_FORWARD)),TolConf);
	  LV.Append(V);
	}
      }
    }
    itry++;
  }

  if (LV.Extent() > 1) {
    //------------------------------------------------
    // garde seulement les vertex les plus proches du 
    //debut et de la fin. 
    //------------------------------------------------
    TopTools_ListIteratorOfListOfShape it(LV);
    TopoDS_Vertex         VF,VL;
    Standard_Real         UMin =  Precision::Infinite();
    Standard_Real         UMax = -Precision::Infinite();
    Standard_Real         U;

    for ( ; it.More(); it.Next()) {
      TopoDS_Vertex CV = TopoDS::Vertex(it.Value());
      TopoDS_Shape aLocalEdge = E1.Oriented(TopAbs_FORWARD);
      U = BRep_Tool::Parameter(CV,TopoDS::Edge(aLocalEdge));
//      U = BRep_Tool::Parameter(CV,TopoDS::Edge(E1.Oriented(TopAbs_FORWARD)));
      if ( U < UMin) {
	VF = CV; UMin = U;
      }
      if ( U > UMax) {
	VL = CV; UMax = U;
      }
    }
    LV.Clear();LV.Append(VF); LV.Append(VL);
  }

#ifdef OCCT_DEBUG
  if (!YaSol) {
    cout <<"Inter2d : Pas de solution"<<endl;
#ifdef DRAW
    DBRep::Set("E1",E1); 
    DBRep::Set("E2",E2);
    DBRep::Set("F",F);
#endif
  }
#endif    
}

//=======================================================================
//function : SelectEdge
//purpose  : 
//=======================================================================

static void SelectEdge (const TopoDS_Face& /*F*/,
			const TopoDS_Face& /*EF*/,
			const TopoDS_Edge& E,
			TopTools_ListOfShape& LInt)
{
 //------------------------------------------------------------
  // detrompeur sur les intersections sur les faces periodiques
  //------------------------------------------------------------
   TopTools_ListIteratorOfListOfShape it(LInt);
  Standard_Real dU = 1.0e100;
  TopoDS_Edge   GE;

  Standard_Real Fst, Lst, tmp;
  BRep_Tool::Range(E, Fst, Lst);
  BRepAdaptor_Curve  Ad1(E);
 
  gp_Pnt PFirst = Ad1.Value( Fst );  
  gp_Pnt PLast  = Ad1.Value( Lst );  

  //----------------------------------------------------------------------
  // Selection de l edge qui couvre le plus le domaine de l edge initiale.
  //---------------------------------------------------------------------- 
  for (; it.More(); it.Next()) {
    const TopoDS_Edge& EI = TopoDS::Edge(it.Value());

    BRep_Tool::Range(EI, Fst, Lst);
    BRepAdaptor_Curve  Ad2(EI);
    gp_Pnt P1 = Ad2.Value(Fst);
    gp_Pnt P2 = Ad2.Value(Lst);
       
    tmp = P1.Distance(PFirst) + P2.Distance(PLast);
    if( tmp <= dU ) {
      dU = tmp;
      GE = EI;
    } 

  }
  LInt.Clear(); 
  LInt.Append(GE);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

static void MakeFace(const Handle(Geom_Surface)& S,
		     const Standard_Real Um,
		     const Standard_Real UM,
		     const Standard_Real Vm,
		     const Standard_Real VM,
		     const Standard_Boolean isVminDegen,
		     const Standard_Boolean isVmaxDegen,
		     TopoDS_Face&        F)
{
  Standard_Real UMin = Um;
  Standard_Real UMax = UM;
  Standard_Real VMin = Vm;
  Standard_Real VMax = VM;
  Standard_Real epsilon = Precision::Confusion();

  Standard_Real umin,umax,vmin,vmax;
  S->Bounds(umin,umax,vmin,vmax);


  // compute infinite flags
  Standard_Boolean umininf = Precision::IsNegativeInfinite(UMin);
  Standard_Boolean umaxinf = Precision::IsPositiveInfinite(UMax);
  Standard_Boolean vmininf = Precision::IsNegativeInfinite(VMin);
  Standard_Boolean vmaxinf = Precision::IsPositiveInfinite(VMax);
  
  // closed flag
  Standard_Boolean IsSuclosed = S->IsUClosed(), IsSvclosed = S->IsVClosed();
  if (S->DynamicType() == STANDARD_TYPE(Geom_OffsetSurface))
    {
      Handle(Geom_Surface) BasisSurf = Handle(Geom_OffsetSurface)::DownCast (S)->BasisSurface();
      IsSuclosed = BasisSurf->IsUClosed();
      IsSvclosed = BasisSurf->IsVClosed();
    }

  Standard_Boolean uclosed = 
    IsSuclosed && 
      Abs(UMin - umin) < epsilon && 
	Abs(UMax - umax) < epsilon;
  
  Standard_Boolean vclosed = 
    IsSvclosed && 
      Abs(VMin - vmin) < epsilon && 
	Abs(VMax - vmax) < epsilon;
  
  // degenerated flags (for cones)
  Standard_Boolean vmindegen = isVminDegen, vmaxdegen = isVmaxDegen;
  Handle(Geom_Surface) theSurf = S;
  if (S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
    theSurf = Handle(Geom_RectangularTrimmedSurface)::DownCast (S)->BasisSurface();
  if (theSurf->DynamicType() == STANDARD_TYPE(Geom_ConicalSurface))
    {
      Handle(Geom_ConicalSurface) ConicalS = Handle(Geom_ConicalSurface)::DownCast (theSurf);
      gp_Cone theCone = ConicalS->Cone();
      gp_Pnt theApex = theCone.Apex();
      Standard_Real Uapex, Vapex;
      ElSLib::Parameters( theCone, theApex, Uapex, Vapex );
      if (Abs(VMin - Vapex) <= Precision::Confusion())
	vmindegen = Standard_True;
      if (Abs(VMax - Vapex) <= Precision::Confusion())
	vmaxdegen = Standard_True;
    }
  
  // compute vertices
  BRep_Builder B;
  Standard_Real tol = Precision::Confusion();
  
  TopoDS_Vertex V00,V10,V11,V01;
  
  if (!umininf) {
    if (!vmininf) B.MakeVertex(V00,S->Value(UMin,VMin),tol);
    if (!vmaxinf) B.MakeVertex(V01,S->Value(UMin,VMax),tol);
  }
  if (!umaxinf) {
    if (!vmininf) B.MakeVertex(V10,S->Value(UMax,VMin),tol);
    if (!vmaxinf) B.MakeVertex(V11,S->Value(UMax,VMax),tol);
  }

  if (uclosed) {
    V10 = V00;
    V11 = V01;
  }

  if (vclosed) {
    V01 = V00;
    V11 = V10;
  }

  if (vmindegen)
    V10 = V00;
  if (vmaxdegen)
    V11 = V01;

  // make the lines
  Handle(Geom2d_Line) Lumin,Lumax,Lvmin,Lvmax;
  if (!umininf)
    Lumin = new Geom2d_Line(gp_Pnt2d(UMin,0),gp_Dir2d(0,1));
  if (!umaxinf)
    Lumax = new Geom2d_Line(gp_Pnt2d(UMax,0),gp_Dir2d(0,1));
  if (!vmininf)
    Lvmin = new Geom2d_Line(gp_Pnt2d(0,VMin),gp_Dir2d(1,0));
  if (!vmaxinf)
    Lvmax = new Geom2d_Line(gp_Pnt2d(0,VMax),gp_Dir2d(1,0));
  
  Handle(Geom_Curve) Cumin,Cumax,Cvmin,Cvmax;
  Standard_Real TolApex = 1.e-5;
  //Standard_Boolean hasiso = ! S->IsKind(STANDARD_TYPE(Geom_OffsetSurface));
  Standard_Boolean hasiso = S->IsKind(STANDARD_TYPE(Geom_ElementarySurface));
  if (hasiso) {
    if (!umininf)
      Cumin = S->UIso(UMin);
    if (!umaxinf)
      Cumax = S->UIso(UMax);
    if (!vmininf)
      {
	Cvmin = S->VIso(VMin);
	if (BRepOffset_Tool::Gabarit( Cvmin ) <= TolApex)
	  vmindegen = Standard_True;
      }
    if (!vmaxinf)
      {
	Cvmax = S->VIso(VMax);
	if (BRepOffset_Tool::Gabarit( Cvmax ) <= TolApex)
	  vmaxdegen = Standard_True;
      }
  }

  // make the face
  B.MakeFace(F,S,tol);

  // make the edges
  TopoDS_Edge eumin,eumax,evmin,evmax;

  if (!umininf) {
    if (hasiso)
      B.MakeEdge(eumin,Cumin,tol);
    else
      B.MakeEdge(eumin);
    if (uclosed) 
      B.UpdateEdge(eumin,Lumax,Lumin,F,tol);
    else
      B.UpdateEdge(eumin,Lumin,F,tol);
    if (!vmininf) {
      V00.Orientation(TopAbs_FORWARD);
      B.Add(eumin,V00);
    }
    if (!vmaxinf) {
      V01.Orientation(TopAbs_REVERSED);
      B.Add(eumin,V01);
    }
    B.Range(eumin,VMin,VMax);
  }

  if (!umaxinf) {
    if (uclosed)
      eumax = eumin;
    else {
      if (hasiso)
	B.MakeEdge(eumax,Cumax,tol);
      else
	B.MakeEdge(eumax);
      B.UpdateEdge(eumax,Lumax,F,tol);
      if (!vmininf) {
	V10.Orientation(TopAbs_FORWARD);
	B.Add(eumax,V10);
      }
      if (!vmaxinf) {
	V11.Orientation(TopAbs_REVERSED);
	B.Add(eumax,V11);
      }
      B.Range(eumax,VMin,VMax);
    }
  }

  if (!vmininf) {
    if (hasiso && !vmindegen)
      B.MakeEdge(evmin,Cvmin,tol);
    else
      B.MakeEdge(evmin);
    if (vclosed)
      B.UpdateEdge(evmin,Lvmin,Lvmax,F,tol);
    else
      B.UpdateEdge(evmin,Lvmin,F,tol);
    if (!umininf) {
      V00.Orientation(TopAbs_FORWARD);
      B.Add(evmin,V00);
    }
    if (!umaxinf) {
      V10.Orientation(TopAbs_REVERSED);
      B.Add(evmin,V10);
    }
    B.Range(evmin,UMin,UMax);
    if (vmindegen)
      B.Degenerated(evmin, Standard_True);
  }

  if (!vmaxinf) {
    if (vclosed)
      evmax = evmin;
    else {
      if (hasiso && !vmaxdegen)
	B.MakeEdge(evmax,Cvmax,tol);
      else
	B.MakeEdge(evmax);
      B.UpdateEdge(evmax,Lvmax,F,tol);
      if (!umininf) {
	V01.Orientation(TopAbs_FORWARD);
	B.Add(evmax,V01);
      }
      if (!umaxinf) {
	V11.Orientation(TopAbs_REVERSED);
	B.Add(evmax,V11);
      }
      B.Range(evmax,UMin,UMax);
      if (vmaxdegen)
	B.Degenerated(evmax, Standard_True);
    }
  }

  // make the wires and add them to the face
  eumin.Orientation(TopAbs_REVERSED);
  evmax.Orientation(TopAbs_REVERSED);
  
  TopoDS_Wire W;

  if (!umininf && !umaxinf && vmininf && vmaxinf) {
    // two wires in u
    B.MakeWire(W);
    B.Add(W,eumin);
    B.Add(F,W);
    B.MakeWire(W);
    B.Add(W,eumax);
    B.Add(F,W);
    F.Closed(uclosed);
  }
    
  else if (umininf && umaxinf && !vmininf && !vmaxinf) {
    // two wires in v
    B.MakeWire(W);
    B.Add(W,evmin);
    B.Add(F,W);
    B.MakeWire(W);
    B.Add(W,evmax);
    B.Add(F,W);
    F.Closed(vclosed);
  }
    
  else if (!umininf || !umaxinf || !vmininf || !vmaxinf) {
    // one wire
    B.MakeWire(W);
    if (!umininf) B.Add(W,eumin);
    if (!vmininf) B.Add(W,evmin);
    if (!umaxinf) B.Add(W,eumax);
    if (!vmaxinf) B.Add(W,evmax);
    B.Add(F,W);
    W.Closed(!umininf && !umaxinf && !vmininf && !vmaxinf);
    F.Closed(uclosed && vclosed);
  }
}

//=======================================================================
//function : EnLargeGeometry
//purpose  : 
//=======================================================================

static Standard_Boolean EnlargeGeometry(Handle(Geom_Surface)& S,
					Standard_Real&        U1,
					Standard_Real&        U2,
					Standard_Real&        V1,
					Standard_Real&        V2,
					Standard_Boolean&     IsV1degen,
					Standard_Boolean&     IsV2degen,
					const Standard_Real   uf1,
					const Standard_Real   uf2,
					const Standard_Real   vf1,
					const Standard_Real   vf2,
					const Standard_Boolean GlobalEnlargeU,
					const Standard_Boolean GlobalEnlargeVfirst,
					const Standard_Boolean GlobalEnlargeVlast)
{
  const Standard_Real coeff = 4.;
  const Standard_Real TolApex = 1.e-5;

  Standard_Boolean SurfaceChange = Standard_False;
  if ( S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    Handle(Geom_Surface) BS = Handle(Geom_RectangularTrimmedSurface)::DownCast (S)->BasisSurface();
    EnlargeGeometry(BS,U1,U2,V1,V2,IsV1degen,IsV2degen,
		    uf1,uf2,vf1,vf2,GlobalEnlargeU,GlobalEnlargeVfirst,GlobalEnlargeVlast);
    if (!GlobalEnlargeVfirst)
      V1 = vf1;
    if (!GlobalEnlargeVlast)
      V2 = vf2;
    if (!GlobalEnlargeVfirst || !GlobalEnlargeVlast)
      //Handle(Geom_RectangularTrimmedSurface)::DownCast (S)->SetTrim( U1, U2, V1, V2 );
      S = new Geom_RectangularTrimmedSurface( BS, U1, U2, V1, V2 );
    else
      S = BS;
    SurfaceChange = Standard_True;
  }
  else if (S->DynamicType() == STANDARD_TYPE(Geom_OffsetSurface)) {
    Handle(Geom_Surface) Surf = Handle(Geom_OffsetSurface)::DownCast (S)->BasisSurface();
    SurfaceChange = EnlargeGeometry(Surf,U1,U2,V1,V2,IsV1degen,IsV2degen,
				    uf1,uf2,vf1,vf2,GlobalEnlargeU,GlobalEnlargeVfirst,GlobalEnlargeVlast);
    Handle(Geom_OffsetSurface)::DownCast(S)->SetBasisSurface(Surf);
  }
  else if (S->DynamicType() == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion) ||
	   S->DynamicType() == STANDARD_TYPE(Geom_SurfaceOfRevolution))
    {
      Standard_Real du=0., dv=0.;
      Handle( Geom_Curve ) uiso, viso, uiso1, uiso2, viso1, viso2;
      Standard_Real u1, u2, v1, v2;
      Standard_Boolean enlargeU = GlobalEnlargeU, enlargeV = Standard_True;
      Standard_Boolean enlargeUfirst = enlargeU, enlargeUlast = enlargeU;
      Standard_Boolean enlargeVfirst = GlobalEnlargeVfirst, enlargeVlast = GlobalEnlargeVlast;
      S->Bounds( u1, u2, v1, v2 );
      if (Precision::IsInfinite(u1) || Precision::IsInfinite(u2))
	{
	  du = uf2-uf1;
	  u1 = uf1-du;
	  u2 = uf2+du;
	  enlargeU = Standard_False;
	}
      else if (S->IsUClosed())
	enlargeU = Standard_False;
      else
	{
	  viso = S->VIso( vf1 );
	  GeomAdaptor_Curve gac( viso );
	  du = GCPnts_AbscissaPoint::Length( gac ) / coeff;
	  uiso1 = S->UIso( uf1 );
	  uiso2 = S->UIso( uf2 );
	  if (BRepOffset_Tool::Gabarit( uiso1 ) <= TolApex)
	    enlargeUfirst = Standard_False;
	  if (BRepOffset_Tool::Gabarit( uiso2 ) <= TolApex)
	    enlargeUlast = Standard_False;
	}
      if (Precision::IsInfinite(v1) || Precision::IsInfinite(v2))
	{
	  dv = vf2-vf1;
	  v1 = vf1-dv;
	  v2 = vf2+dv;
	  enlargeV = Standard_False;
	}
      else if (S->IsVClosed())
	enlargeV = Standard_False;
      else
	{
	  uiso = S->UIso( uf1 );
	  GeomAdaptor_Curve gac( uiso );
	  dv = GCPnts_AbscissaPoint::Length( gac ) / coeff;
	  viso1 = S->VIso( vf1 );
	  viso2 = S->VIso( vf2 );
	  if (BRepOffset_Tool::Gabarit( viso1 ) <= TolApex)
	    {
	      enlargeVfirst = Standard_False;
	      IsV1degen = Standard_True;
	    }
	  if (BRepOffset_Tool::Gabarit( viso2 ) <= TolApex)
	    {
	      enlargeVlast = Standard_False;
	      IsV2degen = Standard_True;
	    }
	}
      Handle(Geom_BoundedSurface) aSurf = new Geom_RectangularTrimmedSurface( S, u1, u2, v1, v2 );
      if (enlargeU)
	{
	  if (enlargeUfirst)
	    GeomLib::ExtendSurfByLength (aSurf, du, 1, Standard_True, Standard_False);
	  if (enlargeUlast)
	    GeomLib::ExtendSurfByLength (aSurf, du, 1, Standard_True, Standard_True);
	}
      if (enlargeV)
	{
	  if (enlargeVfirst)
	    GeomLib::ExtendSurfByLength (aSurf, dv, 1, Standard_False, Standard_False);
	  if (enlargeVlast)
	    GeomLib::ExtendSurfByLength (aSurf, dv, 1, Standard_False, Standard_True);
	}
      S = aSurf;
      S->Bounds( U1, U2, V1, V2 );
      SurfaceChange = Standard_True;
    }
  else if (S->DynamicType() == STANDARD_TYPE(Geom_BezierSurface) ||
	   S->DynamicType() == STANDARD_TYPE(Geom_BSplineSurface))
    {
      Standard_Boolean enlargeU = GlobalEnlargeU, enlargeV = Standard_True;
      Standard_Boolean enlargeUfirst = enlargeU, enlargeUlast = enlargeU;
      Standard_Boolean enlargeVfirst = GlobalEnlargeVfirst, enlargeVlast = GlobalEnlargeVlast;
      if (S->IsUClosed())
	enlargeU = Standard_False;
      if (S->IsVClosed())
	enlargeV = Standard_False;

      Standard_Real duf = uf2-uf1, dvf = vf2-vf1;
      Standard_Real u1, u2, v1, v2;
      S->Bounds( u1, u2, v1, v2 );

      Standard_Real du=0., dv=0.;
      Handle( Geom_Curve ) uiso, viso, uiso1, uiso2, viso1, viso2;
      GeomAdaptor_Curve gac;
      if (enlargeU)
	{
	  viso = S->VIso( v1 );
	  gac.Load( viso );
	  du = GCPnts_AbscissaPoint::Length( gac ) / coeff;
	  uiso1 = S->UIso( u1 );
	  uiso2 = S->UIso( u2 );
	  if (BRepOffset_Tool::Gabarit( uiso1 ) <= TolApex)
	    enlargeUfirst = Standard_False;
	  if (BRepOffset_Tool::Gabarit( uiso2 ) <= TolApex)
	    enlargeUlast = Standard_False;
	}
      if (enlargeV)
	{
	  uiso = S->UIso( u1 );
	  gac.Load( uiso );
	  dv = GCPnts_AbscissaPoint::Length( gac ) / coeff;
	  viso1 = S->VIso( v1 );
	  viso2 = S->VIso( v2 );
	  if (BRepOffset_Tool::Gabarit( viso1 ) <= TolApex)
	    {
	      enlargeVfirst = Standard_False;
	      IsV1degen = Standard_True;
	    }
	  if (BRepOffset_Tool::Gabarit( viso2 ) <= TolApex)
	    {
	      enlargeVlast = Standard_False;
	      IsV2degen = Standard_True;
	    }
	}

      Handle(Geom_BoundedSurface) aSurf = Handle(Geom_BoundedSurface)::DownCast (S);
      if (enlargeU)
	{
	  if (enlargeUfirst && uf1-u1 < duf)
	    GeomLib::ExtendSurfByLength (aSurf, du, 1, Standard_True, Standard_False);
	  if (enlargeUlast && u2-uf2 < duf)
	    GeomLib::ExtendSurfByLength (aSurf, du, 1, Standard_True, Standard_True);
	}
      if (enlargeV)
	{
	  if (enlargeVfirst && vf1-v1 < dvf)
	    GeomLib::ExtendSurfByLength (aSurf, dv, 1, Standard_False, Standard_False);
	  if (enlargeVlast && v2-vf2 < dvf)
	    GeomLib::ExtendSurfByLength (aSurf, dv, 1, Standard_False, Standard_True);
	}
      S = aSurf;

      S->Bounds( U1, U2, V1, V2 );
      SurfaceChange = Standard_True;
    }
//  else if (S->DynamicType() == STANDARD_TYPE(Geom_BezierSurface) ||
//	   S->DynamicType() == STANDARD_TYPE(Geom_BSplineSurface)) {
//    S->Bounds(U1,U2,V1,V2);
//  }
  else { 
    Standard_Real UU1,UU2,VV1,VV2;
    S->Bounds(UU1,UU2,VV1,VV2);
    // Pas d extension au dela des bornes de la surface.
    U1 = Max(UU1,U1);    
    V1 = Max(VV1,V1);
    U2 = Min(UU2,U2);    
    V2 = Min(VV2,V2);
  }
  return SurfaceChange;
}

//=======================================================================
//function : UpDatePCurve
//purpose  :  Mise a jour des pcurves de F sur la surface de de BF.
//            F and BF has to be FORWARD,
//=======================================================================

static void UpdatePCurves (const TopoDS_Face& F,
			         TopoDS_Face& BF)
{
  Standard_Real   f,l;
  Standard_Integer i;
  BRep_Builder    B;
  TopTools_IndexedMapOfShape Emap;
  Handle(Geom2d_Curve) NullPCurve;

  TopExp::MapShapes( F, TopAbs_EDGE, Emap );
  
  for (i = 1; i <= Emap.Extent(); i++)
    {
      TopoDS_Edge CE = TopoDS::Edge( Emap(i) );
      CE.Orientation( TopAbs_FORWARD );
      Handle(Geom2d_Curve) C2 = BRep_Tool::CurveOnSurface( CE, F, f, l );
      if (!C2.IsNull())
	{
	  if (BRep_Tool::IsClosed( CE, F ))
	    {
	      CE.Reverse();
	      Handle(Geom2d_Curve) C2R = BRep_Tool::CurveOnSurface( CE, F, f, l );
	      B.UpdateEdge( CE, NullPCurve, NullPCurve, F, BRep_Tool::Tolerance(CE) );
	      B.UpdateEdge( CE, C2, C2R, BF, BRep_Tool::Tolerance(CE) );
	    }
	  else
	    {
	      B.UpdateEdge( CE, NullPCurve, F, BRep_Tool::Tolerance(CE) );
	      B.UpdateEdge( CE, C2, BF, BRep_Tool::Tolerance(CE) );
	    }

	  B.Range(CE,f,l);
	}
    }
}

//=======================================================================
//function :CompactUVBounds
//purpose  : 
//=======================================================================

static void CompactUVBounds (const TopoDS_Face& F,
				 Standard_Real& UMin,
				 Standard_Real& UMax,
				 Standard_Real& VMin,
				 Standard_Real& VMax)
{
  // Calcul serre pour que les bornes ne couvrent pas plus d une periode
  Standard_Real U1,U2;
  Standard_Real N = 33;
  Bnd_Box2d B;
  
  TopExp_Explorer exp;	
  for (exp.Init(F, TopAbs_EDGE); exp.More(); exp.Next()) {
    const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
    BRepAdaptor_Curve2d C(E,F);
    BRep_Tool::Range(E,U1,U2);
    gp_Pnt2d P;
    Standard_Real U  = U1;
    Standard_Real DU = (U2-U1)/(N-1);
    for (Standard_Integer j=1;j<N;j++) {
      C.D0(U,P);
      U+=DU;
      B.Add(P);
    }
    C.D0(U2,P);
    B.Add(P);
  }
  B.Get(UMin,VMin,UMax,VMax);
}

//=======================================================================
//function : CheckBounds
//purpose  : 
//=======================================================================

void BRepOffset_Tool::CheckBounds(const TopoDS_Face& F,
				  const BRepOffset_Analyse& Analyse,
				  Standard_Boolean& enlargeU,
				  Standard_Boolean& enlargeVfirst,
				  Standard_Boolean& enlargeVlast)
{
  enlargeU = Standard_True;
  enlargeVfirst = Standard_True; enlargeVlast = Standard_True;

  Standard_Integer Ubound = 0, Vbound = 0;
  Standard_Real Ufirst = RealLast(), Ulast = RealFirst();
  Standard_Real Vfirst = RealLast(), Vlast = RealFirst();

  Standard_Real UF1,UF2,VF1,VF2;
  CompactUVBounds(F,UF1,UF2,VF1,VF2);

  Handle(Geom_Surface) theSurf = BRep_Tool::Surface(F);
  if (theSurf->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
    theSurf = Handle(Geom_RectangularTrimmedSurface)::DownCast (theSurf)->BasisSurface();

  if (theSurf->DynamicType() == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion) ||
      theSurf->DynamicType() == STANDARD_TYPE(Geom_SurfaceOfRevolution) ||
      theSurf->DynamicType() == STANDARD_TYPE(Geom_BezierSurface) ||
      theSurf->DynamicType() == STANDARD_TYPE(Geom_BSplineSurface))
    {
      TopExp_Explorer Explo(F, TopAbs_EDGE);
      for (; Explo.More(); Explo.Next())
	{
	  const TopoDS_Edge& anEdge = TopoDS::Edge(Explo.Current());
	  const BRepOffset_ListOfInterval& L = Analyse.Type(anEdge);
	  if (!L.IsEmpty() || BRep_Tool::Degenerated(anEdge))
	    {
	      BRepOffset_Type OT = L.First().Type();
	      if (OT == BRepOffset_Tangent || BRep_Tool::Degenerated(anEdge))
		{
		  Standard_Real fpar, lpar;
		  Handle(Geom2d_Curve) aCurve = BRep_Tool::CurveOnSurface(anEdge, F, fpar, lpar);
		  if (aCurve->DynamicType() == STANDARD_TYPE(Geom2d_TrimmedCurve))
		    aCurve = Handle(Geom2d_TrimmedCurve)::DownCast (aCurve)->BasisCurve();
		  
		  Handle(Geom2d_Line) theLine;
		  if (aCurve->DynamicType() == STANDARD_TYPE(Geom2d_Line))
		    theLine = Handle(Geom2d_Line)::DownCast (aCurve);
		  else if (aCurve->DynamicType() == STANDARD_TYPE(Geom2d_BezierCurve) ||
			   aCurve->DynamicType() == STANDARD_TYPE(Geom2d_BSplineCurve))
		    {
		      Standard_Real newFpar, newLpar, deviation;
		      theLine = ShapeCustom_Curve2d::ConvertToLine2d(aCurve, fpar, lpar, Precision::Confusion(),
								     newFpar, newLpar, deviation);
		    }

		  if (!theLine.IsNull())
		    {
		      gp_Dir2d theDir = theLine->Direction();
		      if (theDir.IsParallel( gp::DX2d(), Precision::Angular() ))
			{
			  Vbound++;
			  if (BRep_Tool::Degenerated(anEdge))
			    {
			      if (Abs(theLine->Location().Y() - VF1) <= Precision::Confusion())
				enlargeVfirst = Standard_False;
			      else //theLine->Location().Y() is near VF2
				enlargeVlast  = Standard_False;
			    }
			  else
			    {
			      if (theLine->Location().Y() < Vfirst)
				Vfirst = theLine->Location().Y();
			      if (theLine->Location().Y() > Vlast)
				Vlast  = theLine->Location().Y();
			    }
			}
		      else if (theDir.IsParallel( gp::DY2d(), Precision::Angular() ))
			{
			  Ubound++;
			  if (theLine->Location().X() < Ufirst)
			    Ufirst = theLine->Location().X();
			  if (theLine->Location().X() > Ulast)
			    Ulast  = theLine->Location().X();
			}
		    }
		}
	    }
	}
    }

  if (Ubound >= 2 || Vbound >= 2)
    {
      if (Ubound >= 2 &&
	  Abs(UF1-Ufirst) <= Precision::Confusion() &&
	  Abs(UF2-Ulast)  <= Precision::Confusion())
	enlargeU = Standard_False;
      if (Vbound >= 2 &&
	  Abs(VF1-Vfirst) <= Precision::Confusion() &&
	  Abs(VF2-Vlast)  <= Precision::Confusion())
	{
	  enlargeVfirst = Standard_False;
	  enlargeVlast  = Standard_False;
	}
    }
}

//=======================================================================
//function : EnLargeFace
//purpose  : 
//=======================================================================

Standard_Boolean BRepOffset_Tool::EnLargeFace 
(const TopoDS_Face&       F,
 TopoDS_Face&             BF,
 const Standard_Boolean   CanExtentSurface,
 const Standard_Boolean   UpdatePCurve,
 const Standard_Boolean   enlargeU,
 const Standard_Boolean   enlargeVfirst,
 const Standard_Boolean   enlargeVlast)
{
  //---------------------------
  // extension de la geometrie.
  //---------------------------
  TopLoc_Location       L;
  Handle (Geom_Surface) S = BRep_Tool::Surface(F,L);
  Standard_Real         UU1,VV1,UU2,VV2;
  Standard_Boolean      isVV1degen = Standard_False, isVV2degen = Standard_False;
  Standard_Real         US1,VS1,US2,VS2;
  Standard_Real         UF1,VF1,UF2,VF2;
  Standard_Boolean      SurfaceChange = Standard_False;

  if (S->IsUPeriodic() || S->IsVPeriodic()) {
    // Calcul serre pour que les bornes ne couvre pas plus d une periode
    CompactUVBounds(F,UF1,UF2,VF1,VF2);					       
  }
  else {
    BRepTools::UVBounds(F,UF1,UF2,VF1,VF2);
  }

  S->Bounds            (US1,US2,VS1,VS2);
  UU1 = VV1 = - TheInfini;
  UU2 = VV2 =   TheInfini;
  
  if (CanExtentSurface) {
    SurfaceChange = EnlargeGeometry( S, UU1, UU2, VV1, VV2, isVV1degen, isVV2degen, UF1, UF2, VF1, VF2,
				     enlargeU, enlargeVfirst, enlargeVlast );
  }
  else {
    UU1 = Max(US1,UU1); UU2 = Min(UU2,US2); 
    VV1 = Max(VS1,VV1); VV2 = Min(VS2,VV2);
  }

  if (S->IsUPeriodic()) {
    Standard_Real    Period = S->UPeriod(); 
    Standard_Real    Delta  = Period - (UF2 - UF1);
    Standard_Real    alpha  = 0.1;
    UU1 = UF1 - alpha*Delta; UU2 = UF2 + alpha*Delta;
    if ((UU2 - UU1) > Period) {
      UU2 = UU1 + Period;
    }
  }
  if (S->IsVPeriodic()) {
    Standard_Real    Period = S->VPeriod(); 
    Standard_Real    Delta  = Period - (VF2 - VF1);
    Standard_Real    alpha  = 0.1;
    VV1 = VF1 - alpha*Delta; VV2 = VF2 + alpha*Delta;
    if ((VV2 - VV1) > Period) {
      VV2 = VV1 + Period;
    }
  }

  //Special treatment for conical surfaces
  Handle(Geom_Surface) theSurf = S;
  if (S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
    theSurf = Handle(Geom_RectangularTrimmedSurface)::DownCast (S)->BasisSurface();
  if (theSurf->DynamicType() == STANDARD_TYPE(Geom_ConicalSurface))
    {
      Handle(Geom_ConicalSurface) ConicalS = Handle(Geom_ConicalSurface)::DownCast (theSurf);
      gp_Cone theCone = ConicalS->Cone();
      gp_Pnt theApex = theCone.Apex();
      Standard_Real Uapex, Vapex;
      ElSLib::Parameters( theCone, theApex, Uapex, Vapex );
      if (VV1 < Vapex && Vapex < VV2)
	{
	  //consider that VF1 and VF2 are on the same side from apex
	  Standard_Real TolApex = 1.e-5;
	  if (Vapex - VF1 >= TolApex ||
	      Vapex - VF2 >= TolApex)   //if (VF1 < Vapex || VF2 < Vapex)
	    VV2 = Vapex;
	  else
	    VV1 = Vapex;
	}
    }

  if (!enlargeU)
    {
      UU1 = UF1; UU2 = UF2;
    }
  if (!enlargeVfirst)
    VV1 = VF1;
  if (!enlargeVlast)
    VV2 = VF2;

  MakeFace(S,UU1,UU2,VV1,VV2,isVV1degen,isVV2degen,BF);
  BF.Location(L);
/*
  if (S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    BRep_Builder B;
    //----------------------------------------------------------------
    // utile pour les bouchons on ne doit pas changer leur geometrie.
    // (Ce que fait BRepLib_MakeFace si S est restreinte).
    // On remet S et on update les pcurves.
    //----------------------------------------------------------------
    TopExp_Explorer exp;
    exp.Init(BF,TopAbs_EDGE);
    Standard_Real f=0.,l=0.;
    for (; exp.More(); exp.Next()) {
      TopoDS_Edge   CE  = TopoDS::Edge(exp.Current());
      Handle(Geom2d_Curve) C2 = BRep_Tool::CurveOnSurface(CE,BF,f,l); 
      B.UpdateEdge (CE,C2,S,L,BRep_Tool::Tolerance(CE));
    }    
    B.UpdateFace(BF,S,L,BRep_Tool::Tolerance(F)); 
  }
*/  
  if (SurfaceChange && UpdatePCurve) {
    TopoDS_Shape aLocalFace = F.Oriented(TopAbs_FORWARD);
    UpdatePCurves(TopoDS::Face(aLocalFace),BF);
    //UpdatePCurves(TopoDS::Face(F.Oriented(TopAbs_FORWARD)),BF);
    BRep_Builder BB;
    BB.UpdateFace( F, S, L, BRep_Tool::Tolerance(F) );
  }

  BF.Orientation(F.Orientation());
  return SurfaceChange;
}

//=======================================================================
//function : TryParameter
//purpose  : 
//=======================================================================

static Standard_Boolean TryParameter (const TopoDS_Edge& OE,
				      TopoDS_Vertex&     V,
				      const TopoDS_Edge& NE,
				      Standard_Real      TolConf)
{
  BRepAdaptor_Curve OC(OE);
  BRepAdaptor_Curve NC(NE);
  Standard_Real Of = OC.FirstParameter(); Standard_Real Ol = OC.LastParameter();
  Standard_Real Nf = NC.FirstParameter(); Standard_Real Nl = NC.LastParameter();
  Standard_Real U = 0.;
  gp_Pnt           P  = BRep_Tool::Pnt(V);
  Standard_Boolean OK = Standard_False;

  if (P.Distance(OC.Value(Of)) < TolConf) {
    if (Of > Nf && Of < Nl  && P.Distance(NC.Value(Of)) < TolConf) {
      OK = Standard_True;
      U    = Of;
    }
  }
  if (P.Distance(OC.Value(Ol)) < TolConf) {
    if (Ol > Nf && Ol < Nl  && P.Distance(NC.Value(Ol)) < TolConf) {
      OK = Standard_True;
      U    = Ol;
    }
  }
  if (OK) {
    BRep_Builder B;
    TopoDS_Shape aLocalShape = NE.Oriented(TopAbs_FORWARD);
    TopoDS_Edge EE = TopoDS::Edge(aLocalShape);
//    TopoDS_Edge EE = TopoDS::Edge(NE.Oriented(TopAbs_FORWARD));
    aLocalShape = V.Oriented(TopAbs_INTERNAL);
    B.UpdateVertex(TopoDS::Vertex(aLocalShape),
		   U,NE,BRep_Tool::Tolerance(NE));
//    B.UpdateVertex(TopoDS::Vertex(V.Oriented(TopAbs_INTERNAL)),
//		   U,NE,BRep_Tool::Tolerance(NE));
  }
  return OK;  
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void BRepOffset_Tool::MapVertexEdges (const TopoDS_Shape& S,
				      TopTools_DataMapOfShapeListOfShape& MEV)
{
  TopExp_Explorer      exp;
  exp.Init(S.Oriented(TopAbs_FORWARD),TopAbs_EDGE);
  TopTools_MapOfShape  DejaVu;
  for ( ; exp.More(); exp.Next()) {
    const TopoDS_Edge& E = TopoDS::Edge(exp.Current());
    if (DejaVu.Add(E)) {
      TopoDS_Vertex     V1,V2;
      TopExp::Vertices (E,V1,V2);
      if (!MEV.IsBound(V1)) {
	TopTools_ListOfShape empty;
	MEV.Bind(V1,empty);
      }
      MEV(V1).Append(E);
      if (!V1.IsSame(V2)) {
	if (!MEV.IsBound(V2)) {
	  TopTools_ListOfShape empty;
	  MEV.Bind(V2,empty);
	}
	MEV(V2).Append(E);
      }
    }
  }
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void BRepOffset_Tool::BuildNeighbour (const TopoDS_Wire& W,
				      const TopoDS_Face& F,
				      TopTools_DataMapOfShapeShape& NOnV1,
				      TopTools_DataMapOfShapeShape& NOnV2)
{
  TopoDS_Vertex V1,V2,VP1,VP2,FV1,FV2;
  TopoDS_Edge   CurE,FirstE,PrecE;
  BRepTools_WireExplorer         wexp;
  
  TopoDS_Shape aLocalFace = F.Oriented(TopAbs_FORWARD);
  TopoDS_Shape aLocalWire = W.Oriented(TopAbs_FORWARD);
  wexp.Init(TopoDS::Wire(aLocalWire),TopoDS::Face(aLocalFace));
//  wexp.Init(TopoDS::Wire(W.Oriented(TopAbs_FORWARD)),
//	    TopoDS::Face(F.Oriented(TopAbs_FORWARD)));
  CurE = FirstE = PrecE = wexp.Current();
  TopExp::Vertices(CurE,V1,V2);
  FV1 = VP1 = V1; FV2 = VP2 = V2;
  wexp.Next();
  while (wexp.More()) {
    CurE = wexp.Current();
    TopExp::Vertices(CurE,V1,V2);
    if (V1.IsSame(VP1)) { NOnV1.Bind(PrecE,CurE); NOnV1.Bind(CurE,PrecE);}
    if (V1.IsSame(VP2)) { NOnV2.Bind(PrecE,CurE); NOnV1.Bind(CurE,PrecE);}
    if (V2.IsSame(VP1)) { NOnV1.Bind(PrecE,CurE); NOnV2.Bind(CurE,PrecE);}
    if (V2.IsSame(VP2)) { NOnV2.Bind(PrecE,CurE); NOnV2.Bind(CurE,PrecE);}
    PrecE = CurE;
    VP1 = V1; VP2 = V2;
    wexp.Next();
  }
  if (V1.IsSame(FV1)) { NOnV1.Bind(FirstE,CurE); NOnV1.Bind(CurE,FirstE);}
  if (V1.IsSame(FV2)) { NOnV2.Bind(FirstE,CurE); NOnV1.Bind(CurE,FirstE);}
  if (V2.IsSame(FV1)) { NOnV1.Bind(FirstE,CurE); NOnV2.Bind(CurE,FirstE);}
  if (V2.IsSame(FV2)) { NOnV2.Bind(FirstE,CurE); NOnV2.Bind(CurE,FirstE);}
}

//=======================================================================
//function : ExtentFace
//purpose  : 
//=======================================================================

void BRepOffset_Tool::ExtentFace (const TopoDS_Face&            F,
				  TopTools_DataMapOfShapeShape& ConstShapes,
				  TopTools_DataMapOfShapeShape& ToBuild,
				  const TopAbs_State            Side,
				  const Standard_Real           TolConf,
				  TopoDS_Face&                  NF)
{
#ifdef DRAW
  if (AffichInter) {
    char name[256];
    sprintf(name,"FTE_%d",NbFTE++);
    DBRep::Set(name,F);
  }
#endif
  
  TopExp_Explorer                    exp,exp2;
  TopTools_DataMapOfShapeShape       Build;
  TopTools_DataMapOfShapeShape       Extent;
  TopoDS_Edge                        FirstE,PrecE,CurE,NE;
  BRep_Builder B;
  TopoDS_Face  EF;

  // Construction de la boite englobante de la face a etendre et des bouchons pour
  // limiter les extensions.
  //Bnd_Box ContextBox;
  //BRepBndLib::Add(F,B);
  //TopTools_DataMapIteratorOfDataMapOfShape itTB(ToBuild);
  //for (; itTB.More(); itTB.Next()) {
  //BRepBndLib::Add(TopBuild.Value(), ContextBox);
  //}

   
  Standard_Boolean SurfaceChange;
  SurfaceChange = EnLargeFace (F,EF,Standard_True);

  TopoDS_Shape aLocalShape = EF.EmptyCopied();
  NF = TopoDS::Face(aLocalShape);
//  NF = TopoDS::Face(EF.EmptyCopied());
  NF.Orientation(TopAbs_FORWARD);

  if (SurfaceChange) {
    //------------------------------------------------
    // Mise a jour des pcurves sur la surface de base.
    //------------------------------------------------
    TopoDS_Face Fforward = F;
    Fforward.Orientation(TopAbs_FORWARD);
    TopTools_IndexedMapOfShape Emap;
    TopExp::MapShapes( Fforward, TopAbs_EDGE, Emap );
    Standard_Real f,l;
    for (Standard_Integer i = 1; i <= Emap.Extent(); i++) {
      TopoDS_Edge   CE  = TopoDS::Edge( Emap(i) );
      CE.Orientation(TopAbs_FORWARD);
      TopoDS_Edge   Ecs; //patch
      Handle(Geom2d_Curve) C2 = BRep_Tool::CurveOnSurface(CE,Fforward,f,l);
      if (!C2.IsNull()) {
	if (ConstShapes.IsBound(CE)) {
	  Ecs = TopoDS::Edge(ConstShapes(CE));
	  BRep_Tool::Range(Ecs,f,l);
	}
	if (BRep_Tool::IsClosed(CE,Fforward))  {
	  TopoDS_Shape aLocalShapeReversedCE = CE.Reversed();
	  Handle(Geom2d_Curve) C2R = 
	    BRep_Tool::CurveOnSurface(TopoDS::Edge(aLocalShapeReversedCE),Fforward,f,l);
//	  Handle(Geom2d_Curve) C2R = 
//	    BRep_Tool::CurveOnSurface(TopoDS::Edge(CE.Reversed()),F,f,l);
	  B.UpdateEdge (CE,C2,C2R,EF,BRep_Tool::Tolerance(CE));
	  if (! Ecs.IsNull())
	    B.UpdateEdge (Ecs,C2,C2R,EF,BRep_Tool::Tolerance(CE));
	}
	else {
	  B.UpdateEdge (CE,C2,EF,BRep_Tool::Tolerance(CE));
	  if (! Ecs.IsNull())
	    B.UpdateEdge (Ecs,C2,EF,BRep_Tool::Tolerance(CE));
	}
	B.Range(CE,f,l);
	if (! Ecs.IsNull())
	  B.Range(Ecs,f,l);
      }
    }
  }
  
  for (exp.Init(F.Oriented(TopAbs_FORWARD),TopAbs_WIRE); 
       exp.More();
       exp.Next()) {
    const TopoDS_Wire& W = TopoDS::Wire(exp.Current());
    TopTools_DataMapOfShapeListOfShape MVE; // Vertex -> Edges incidentes.
    TopTools_DataMapOfShapeShape       NOnV1;
    TopTools_DataMapOfShapeShape       NOnV2;

    MapVertexEdges (W,MVE);
    BuildNeighbour (W,F,NOnV1,NOnV2);
    
    TopTools_ListOfShape LInt1,LInt2;
    TopoDS_Face          StopFace;
    //------------------------------------------------
    // Construction edges
    //------------------------------------------------
    for (exp2.Init(W.Oriented(TopAbs_FORWARD),TopAbs_EDGE); 
	 exp2.More(); exp2.Next()) {
      const TopoDS_Edge& E = TopoDS::Edge(exp2.Current());
      if (ConstShapes.IsBound(E)) ToBuild.UnBind(E);
      if (ToBuild.IsBound(E)) {
	TopTools_ListOfShape LOE;
	LOE.Append(E);
	BRepOffset_Tool::TryProject (TopoDS::Face(ToBuild(E)),
				     EF,LOE,LInt2,LInt1,Side,TolConf);
	if (!LInt1.IsEmpty()) 
	  ToBuild.UnBind(E);
      }
    }

    for (exp2.Init(W.Oriented(TopAbs_FORWARD),TopAbs_EDGE); 
	 exp2.More(); exp2.Next()) {
      const TopoDS_Edge& E = TopoDS::Edge(exp2.Current());
      if (ConstShapes.IsBound(E)) ToBuild.UnBind(E);
      if (ToBuild.IsBound(E)) {
        EnLargeFace(TopoDS::Face(ToBuild(E)),StopFace,Standard_False);
        BRepOffset_Tool::Inter3D (EF,StopFace,LInt1,LInt2,Side,E,Standard_True);
        // No intersection, it may happen for example for a chosen (non-offseted) planar face and 
        // its neighbour offseted cylindrical face, if the offset is directed so that 
        // the radius of the cylinder becomes smaller.
        if (LInt1.IsEmpty())
          continue;  
	if (LInt1.Extent() > 1) { 
	  // l intersection est en plusieurs edges (franchissement de couture)
	  SelectEdge (F,EF,E,LInt1);
	}
	NE = TopoDS::Edge(LInt1.First());
	Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*) &NE.TShape());
	TE->Tolerance( TE->Tolerance()*10. ); //????
	if (NE.Orientation() == E.Orientation()) {
	  Build.Bind(E,NE.Oriented(TopAbs_FORWARD));
	}
	else {
	  Build.Bind(E,NE.Oriented(TopAbs_REVERSED));
	}
	const TopoDS_Edge& EOnV1 = TopoDS::Edge(NOnV1(E));
	if (!ToBuild    .IsBound(EOnV1) && 
	    !ConstShapes.IsBound(EOnV1) && 
	    !Build      .IsBound(EOnV1)) {
	  ExtentEdge (F,EF,EOnV1,NE);
	  Build.Bind (EOnV1,NE.Oriented(TopAbs_FORWARD));
	}
	const TopoDS_Edge& EOnV2 = TopoDS::Edge(NOnV2(E));
	if (!ToBuild    .IsBound(EOnV2) && 
	    !ConstShapes.IsBound(EOnV2) && 
	    !Build      .IsBound(EOnV2)) {
	  ExtentEdge (F,EF,EOnV2,NE);
	  Build.Bind (EOnV2,NE.Oriented (TopAbs_FORWARD));
	}
      }
    }

    //------------------------------------------------
    // Construction Vertex.
    //------------------------------------------------
    TopTools_ListOfShape LV;
    Standard_Real        f,l;
    TopoDS_Edge          ERef;
    TopoDS_Vertex        V1,V2;

    for (exp2.Init(W.Oriented(TopAbs_FORWARD),TopAbs_EDGE); exp2.More(); exp2.Next())
    {
      const TopoDS_Edge& E = TopoDS::Edge(exp2.Current());
      TopExp::Vertices (E,V1,V2);
      BRep_Tool::Range (E,f,l);
      TopoDS_Vertex V;
      if (Build.IsBound(E))
      {
        const TopoDS_Edge& NEOnV1 = TopoDS::Edge(NOnV1(E));
        if (Build.IsBound(NEOnV1) && (ToBuild.IsBound(E) || ToBuild.IsBound(NEOnV1)))
        {
          if (E.IsSame(NEOnV1))
            V = TopExp::FirstVertex(TopoDS::Edge(Build(E)));
          else
          {
            //---------------
            // intersection.
            //---------------
            if (!Build.IsBound(V1))
            {
              Inter2d (EF,TopoDS::Edge(Build(E)), TopoDS::Edge(Build(NEOnV1)),LV,/*TolConf*/Precision::Confusion());
              
              if(!LV.IsEmpty())
              {
                if (Build(E).Orientation() == TopAbs_FORWARD)
                {
                  V = TopoDS::Vertex(LV.First());
                }
                else
                {
                  V = TopoDS::Vertex(LV.Last());
                }
              }
              else
              {
                return;
              }
            }
            else
            {
              V = TopoDS::Vertex(Build(V1));
              if (MVE (V1).Extent() > 2)
              {
                V.Orientation(TopAbs_FORWARD);
                if (Build(E).Orientation() == TopAbs_REVERSED)
                  V.Orientation(TopAbs_REVERSED);

                ProjectVertexOnEdge(V,TopoDS::Edge(Build(E)),TolConf);
              }
            }
          }
        }
        else
        {
          //------------
          //projection
          //------------
          V = V1;
          if (ConstShapes.IsBound(V1)) V = TopoDS::Vertex(ConstShapes(V1));
          V.Orientation(TopAbs_FORWARD);
          if (Build(E).Orientation() == TopAbs_REVERSED)
            V.Orientation(TopAbs_REVERSED);
          if (!TryParameter    (E,V,TopoDS::Edge(Build(E)),TolConf))
            ProjectVertexOnEdge(V,TopoDS::Edge(Build(E)),TolConf);
        }

        ConstShapes.Bind(V1,V);
        Build.Bind      (V1,V);
        const TopoDS_Edge& NEOnV2 = TopoDS::Edge(NOnV2(E));
        if (Build.IsBound(NEOnV2) && (ToBuild.IsBound(E) || ToBuild.IsBound(NEOnV2)))
        {
          if (E.IsSame(NEOnV2))
            V = TopExp::LastVertex(TopoDS::Edge(Build(E)));
          else
          {
            //--------------
            // intersection.
            //---------------

            if (!Build.IsBound(V2))
            {
              Inter2d (EF,TopoDS::Edge(Build(E)), TopoDS::Edge(Build(NEOnV2)),LV,/*TolConf*/Precision::Confusion());

              if(!LV.IsEmpty())
              {
                if (Build(E).Orientation() == TopAbs_FORWARD)
                {
                  V = TopoDS::Vertex(LV.Last());
                }
                else
                {
                  V = TopoDS::Vertex(LV.First());
                }
              }
              else
              {
                return;
              }
            }
            else
            {
              V = TopoDS::Vertex(Build(V2));
              if (MVE (V2).Extent() > 2)
              {
                V.Orientation(TopAbs_REVERSED);
                if (Build(E).Orientation() == TopAbs_REVERSED)
                  V.Orientation(TopAbs_FORWARD);

                ProjectVertexOnEdge(V,TopoDS::Edge(Build(E)),TolConf);
              }
            }
          }
        }
        else
        {
          //------------
          //projection
          //------------
          V = V2;
          if (ConstShapes.IsBound(V2))
            V = TopoDS::Vertex(ConstShapes(V2));
          V.Orientation(TopAbs_REVERSED);	
          if (Build(E).Orientation() == TopAbs_REVERSED)
            V.Orientation(TopAbs_FORWARD);
          if (!TryParameter (E,V,TopoDS::Edge(Build(E)),TolConf))
            ProjectVertexOnEdge(V,TopoDS::Edge(Build(E)),TolConf);
        }
        ConstShapes.Bind(V2,V);
        Build.Bind(V2,V);
      }
    }
    
    TopoDS_Wire        NW;
    TopoDS_Vertex      NV1,NV2;
    TopAbs_Orientation Or;
    Standard_Real      U1,U2;
    Standard_Real      eps = Precision::Confusion();

#ifdef OCCT_DEBUG
    TopLoc_Location    L;
#endif
    B.MakeWire(NW);

    //-----------------
    // Reconstruction.
    //-----------------
    for (exp2.Init(W.Oriented(TopAbs_FORWARD),TopAbs_EDGE); 
	 exp2.More(); exp2.Next()) {
      const TopoDS_Edge& E = TopoDS::Edge(exp2.Current());
      TopExp::Vertices (E,V1,V2);
      if (Build.IsBound(E)) {
	NE = TopoDS::Edge(Build(E));
	BRep_Tool::Range(NE,f,l);
	Or = NE.Orientation();
	//-----------------------------------------------------
	// Copy pour virer les vertex deja sur la nouvelle edge.
	//-----------------------------------------------------
	NV1 = TopoDS::Vertex(ConstShapes(V1));
	NV2 = TopoDS::Vertex(ConstShapes(V2));

	TopoDS_Shape aLocalVertexOrientedNV1 = NV1.Oriented(TopAbs_INTERNAL);
	TopoDS_Shape aLocalEdge   = NE.Oriented(TopAbs_INTERNAL);
	
	U1 = BRep_Tool::Parameter(TopoDS::Vertex(aLocalVertexOrientedNV1),
				  TopoDS::Edge  (aLocalEdge));
        aLocalVertexOrientedNV1 = NV2.Oriented(TopAbs_INTERNAL);
	aLocalEdge   = NE.Oriented(TopAbs_FORWARD);
	U2 = BRep_Tool::Parameter (TopoDS::Vertex(aLocalVertexOrientedNV1),TopoDS::Edge  (aLocalEdge));
//	U1 = BRep_Tool::Parameter
//	  (TopoDS::Vertex(NV1.Oriented(TopAbs_INTERNAL)),
//	   TopoDS::Edge  (NE .Oriented(TopAbs_FORWARD)));
//	U2 = BRep_Tool::Parameter
//	  (TopoDS::Vertex(NV2.Oriented(TopAbs_INTERNAL)),
//	   TopoDS::Edge  (NE.Oriented(TopAbs_FORWARD)));
	aLocalEdge = NE.EmptyCopied();
	NE = TopoDS::Edge(aLocalEdge);
	NE.Orientation(TopAbs_FORWARD);
	if (NV1.IsSame(NV2))
	  {
	    //--------------
	    // edge ferme.
	    //--------------
	    if (Or == TopAbs_FORWARD) {U1 = f; U2 = l;}
	    else                      {U1 = l; U2 = f;}
	    if (Or == TopAbs_FORWARD)
	      {
		if (U1 > U2)
		  {
		    if (Abs(U1-l) < eps) U1 = f;
		    if (Abs(U2-f) < eps) U2 = l;	    
		  }
		TopoDS_Shape aLocalVertex = NV1.Oriented(TopAbs_FORWARD );
		B.Add (NE,TopoDS::Vertex(aLocalVertex));
		aLocalVertex = NV2.Oriented(TopAbs_REVERSED);
		B.Add (NE,TopoDS::Vertex(aLocalVertex));
//		B.Add (NE,TopoDS::Vertex(NV1.Oriented(TopAbs_FORWARD )));
//		B.Add (NE,TopoDS::Vertex(NV2.Oriented(TopAbs_REVERSED)));
		B.Range(NE,U1,U2);
		ConstShapes.Bind(E,NE);
		NE.Orientation(E.Orientation());
	      }
	    else
	      {
		if (U2 > U1)
		  {
		    if (Abs(U2-l) < eps) U2 = f;
		    if (Abs(U1-f) < eps) U1 = l;	    
		  }
		TopoDS_Shape aLocalVertex = NV2.Oriented(TopAbs_FORWARD );
		B.Add (NE,TopoDS::Vertex(aLocalVertex));
		aLocalVertex = NV1.Oriented(TopAbs_REVERSED);
		B.Add (NE,TopoDS::Vertex(aLocalVertex));
//		B.Add (NE,TopoDS::Vertex(NV2.Oriented(TopAbs_FORWARD )));
//		B.Add (NE,TopoDS::Vertex(NV1.Oriented(TopAbs_REVERSED)));
		B.Range(NE,U2,U1);
		ConstShapes.Bind(E,NE.Oriented(TopAbs_REVERSED));
		NE.Orientation(TopAbs::Reverse(E.Orientation()));
	      }
	  }
	else
	  {
	    //-------------------
	    // edge is not ferme.
	    //-------------------
	    if (Or == TopAbs_FORWARD) {
	      if (U1 > U2) {
		TopoDS_Shape aLocalVertex = NV2.Oriented(TopAbs_FORWARD );
		B.Add (NE,TopoDS::Vertex(aLocalVertex));
		aLocalVertex = NV1.Oriented(TopAbs_REVERSED);
		B.Add (NE,TopoDS::Vertex(aLocalVertex));
//		B.Add (NE,TopoDS::Vertex(NV2.Oriented(TopAbs_FORWARD )));
//		B.Add (NE,TopoDS::Vertex(NV1.Oriented(TopAbs_REVERSED)));
		B.Range(NE,U2,U1);
	      }
	      else
		{
		  TopoDS_Shape aLocalVertex = NV1.Oriented(TopAbs_FORWARD );
		  B.Add (NE,TopoDS::Vertex(aLocalVertex));
		  aLocalVertex = NV2.Oriented(TopAbs_REVERSED);
		  B.Add (NE,TopoDS::Vertex(aLocalVertex));
//		  B.Add (NE,TopoDS::Vertex(NV1.Oriented(TopAbs_FORWARD )));
//		  B.Add (NE,TopoDS::Vertex(NV2.Oriented(TopAbs_REVERSED)));
		  B.Range(NE,U1,U2);
		}
	      ConstShapes.Bind(E,NE);
	      NE.Orientation(E.Orientation());
	    }
	    else {
	      if (U2 > U1) {
		TopoDS_Shape aLocalVertex = NV1.Oriented(TopAbs_FORWARD );
		B.Add (NE,TopoDS::Vertex(aLocalVertex));
		aLocalVertex = NV2.Oriented(TopAbs_REVERSED);
		B.Add (NE,TopoDS::Vertex(aLocalVertex));
//		B.Add (NE,TopoDS::Vertex(NV1.Oriented(TopAbs_FORWARD )));
//		B.Add (NE,TopoDS::Vertex(NV2.Oriented(TopAbs_REVERSED)));
		B.Range(NE,U1,U2);
		ConstShapes.Bind(E,NE);
		NE.Orientation(E.Orientation());
	      }
	      else
		{
		  TopoDS_Shape aLocalVertex = NV2.Oriented(TopAbs_FORWARD );
		  B.Add (NE,TopoDS::Vertex(aLocalVertex));
		  aLocalVertex = NV1.Oriented(TopAbs_REVERSED);
		  B.Add (NE,TopoDS::Vertex(aLocalVertex));
//		  B.Add (NE,TopoDS::Vertex(NV2.Oriented(TopAbs_FORWARD )));
//		  B.Add (NE,TopoDS::Vertex(NV1.Oriented(TopAbs_REVERSED)));
		  B.Range(NE,U2,U1);
		  ConstShapes.Bind(E,NE.Oriented(TopAbs_REVERSED));
		  NE.Orientation(TopAbs::Reverse(E.Orientation()));
		}
	    }
	  }
	Build.UnBind(E);
      } // Build.IsBound(E)
      else if (ConstShapes.IsBound(E)) { // !Build.IsBound(E)
	NE = TopoDS::Edge(ConstShapes(E));	
	BuildPCurves(NE,NF);
	Or = NE.Orientation();
	if (Or == TopAbs_REVERSED) {
	  NE.Orientation(TopAbs::Reverse(E.Orientation()));
	}
	else {
	  NE.Orientation(E.Orientation());
	}
      }
      else {
	NE = E;
	ConstShapes.Bind(E,NE.Oriented(TopAbs_FORWARD));
      }
      B.Add(NW,NE);
    }
    B.Add(NF,NW.Oriented(W.Orientation()));
  }
  NF.Orientation(F.Orientation());
  BRepTools::Update(NF); // Maj des UVPoints
  
#ifdef DRAW
  if (AffichInter) {
    char name[256];
    sprintf(name,"FOB_%d",NbFOB++);
    DBRep::Set(name,NF);
  }
#endif
}
  

//=======================================================================
//function : Deboucle3D
//purpose  : 
//=======================================================================

TopoDS_Shape BRepOffset_Tool::Deboucle3D(const TopoDS_Shape& S,
					 const TopTools_MapOfShape& Boundary)
{
  return BRepAlgo_Tool::Deboucle3D(S,Boundary);
}

//=======================================================================
//function : IsInOut
//purpose  : 
//=======================================================================

static Standard_Boolean IsInOut (BRepTopAdaptor_FClass2d& FC,
				 Geom2dAdaptor_Curve      AC,
				 const TopAbs_State&      S )
{
 Standard_Real Def = 100*Precision::Confusion();
 GCPnts_QuasiUniformDeflection QU(AC,Def);
 
 for (Standard_Integer i = 1; i <= QU.NbPoints(); i++) {
   gp_Pnt2d P = AC.Value(QU.Parameter(i));
   if (FC.Perform(P) != S) {
     return Standard_False;
   } 
 }
 return Standard_True;
}
				  
//=======================================================================
//function : CorrectOrientation
//purpose  : 
//=======================================================================

void BRepOffset_Tool::CorrectOrientation(const TopoDS_Shape&        SI,
					 const TopTools_IndexedMapOfShape& NewEdges,
					 Handle(BRepAlgo_AsDes)&    AsDes,
					 BRepAlgo_Image&            InitOffset,
					 const Standard_Real        Offset)
{

  TopExp_Explorer exp;
  exp.Init(SI,TopAbs_FACE);
  Standard_Real   f=0.,l=0.;

  for (; exp.More(); exp.Next()) {

    const TopoDS_Face&          FI  = TopoDS::Face(exp.Current());
    const TopTools_ListOfShape& LOF = InitOffset.Image(FI);
    TopTools_ListIteratorOfListOfShape it(LOF);
    for (; it.More(); it.Next()) {
      const TopoDS_Face&    OF   = TopoDS::Face(it.Value());
      TopTools_ListOfShape& LOE = AsDes->ChangeDescendant(OF);
      TopTools_ListIteratorOfListOfShape itE(LOE);

      Standard_Boolean YaInt = Standard_False;
      for (; itE.More(); itE.Next()) {
	const TopoDS_Edge& OE = TopoDS::Edge(itE.Value());
	if (NewEdges.Contains(OE)) {YaInt = Standard_True; break;}
      }
      if (YaInt) {
	TopoDS_Shape aLocalFace = FI.Oriented(TopAbs_FORWARD);
	BRepTopAdaptor_FClass2d FC (TopoDS::Face(aLocalFace),
				    Precision::Confusion());
//	BRepTopAdaptor_FClass2d FC (TopoDS::Face(FI.Oriented(TopAbs_FORWARD)),
//				    Precision::Confusion());
	for (itE.Initialize(LOE); itE.More(); itE.Next()) {
	  TopoDS_Shape&   OE   = itE.Value();
	  if (NewEdges.Contains(OE)) {
	    Handle(Geom2d_Curve) CO2d = 
	      BRep_Tool::CurveOnSurface(TopoDS::Edge(OE),OF,f,l);
	    Geom2dAdaptor_Curve  AC(CO2d,f,l);
	    
	    if (Offset > 0) {
	      if (IsInOut(FC,AC,TopAbs_OUT)) OE.Reverse();
	    }
//	    else {
//	      if (IsInOut(FC,AC,TopAbs_IN)) OE.Reverse();	    
//	    }
	  }
	}
      }
    }
  }

}

//=======================================================================
//function : CheckNormals
//purpose  : 
//=======================================================================
Standard_Boolean BRepOffset_Tool::CheckPlanesNormals(const TopoDS_Face& theFace1,
                                                     const TopoDS_Face& theFace2,
                                                     const Standard_Real theTolAng)
{
  BRepAdaptor_Surface aBAS1(theFace1, Standard_False), aBAS2(theFace2, Standard_False);
  if (aBAS1.GetType() != GeomAbs_Plane ||
      aBAS2.GetType() != GeomAbs_Plane) {
    return Standard_False;
  }
  //
  gp_Dir aDN1 = aBAS1.Plane().Position().Direction();
  if (theFace1.Orientation() == TopAbs_REVERSED) {
    aDN1.Reverse();
  }
  //
  gp_Dir aDN2 = aBAS2.Plane().Position().Direction();
  if (theFace2.Orientation() == TopAbs_REVERSED) {
    aDN2.Reverse();
  }
  //
  Standard_Real anAngle = aDN1.Angle(aDN2);
  return (anAngle < theTolAng);
}

//=======================================================================
//function : PerformPlanes
//purpose  : 
//=======================================================================
void PerformPlanes(const TopoDS_Face& theFace1,
                   const TopoDS_Face& theFace2,
                   const TopAbs_State theSide,
                   TopTools_ListOfShape& theL1,
                   TopTools_ListOfShape& theL2)
{
  theL1.Clear();
  theL2.Clear();
  // Intersect the planes using IntTools_FaceFace directly
  IntTools_FaceFace aFF;
  aFF.SetParameters(Standard_True, Standard_True, Standard_True, Precision::Confusion());
  aFF.Perform(theFace1, theFace2);
  //
  if (!aFF.IsDone()) {
    return;
  }
  //
  const IntTools_SequenceOfCurves& aSC = aFF.Lines();
  if (aSC.IsEmpty()) {
    return;
  }
  //
  // In Plane/Plane intersection only one curve is always produced.
  // Make the edge from this section curve.
  TopoDS_Edge aE;
  {
    BRep_Builder aBB;
    const IntTools_Curve& aIC = aSC(1);
    const Handle(Geom_Curve)& aC3D = aIC.Curve();
    aBB.MakeEdge(aE, aC3D, aIC.Tolerance());
    // Get bounds of the curve
    Standard_Real aTF, aTL;
    gp_Pnt aPF, aPL;
    aIC.Bounds(aTF, aTL, aPF, aPL);
    // Make the bounding vertices
    TopoDS_Vertex aVF, aVL;
    aBB.MakeVertex(aVF, aPF, aIC.Tolerance());
    aBB.MakeVertex(aVL, aPL, aIC.Tolerance());
    aVL.Orientation(TopAbs_REVERSED);
    // Add vertices to the edge
    aBB.Add(aE, aVF);
    aBB.Add(aE, aVL);
    // Add 2D curves to the edge
    aBB.UpdateEdge(aE, aIC.FirstCurve2d(), theFace1, aIC.Tolerance());
    aBB.UpdateEdge(aE, aIC.SecondCurve2d(), theFace2, aIC.Tolerance());
    // Update range of the new edge
    aBB.Range(aE, aTF, aTL);
  }
  //
  // Orient section
  TopAbs_Orientation O1, O2;
  BRepOffset_Tool::OrientSection(aE, theFace1, theFace2, O1, O2);
  if (theSide == TopAbs_OUT) {
    O1 = TopAbs::Reverse(O1);
    O2 = TopAbs::Reverse(O2);
  }
  //
  BRepLib::SameParameter(aE, Precision::Confusion(), Standard_True);
  //
  // Add edge to result
  theL1.Append(aE.Oriented(O1));
  theL2.Append(aE.Oriented(O2));
}

//=======================================================================
//function : IsInf
//purpose  : Checks if the given value is close to infinite (TheInfini)
//=======================================================================
Standard_Boolean IsInf(const Standard_Real theVal)
{
  return (theVal > TheInfini*0.9);
}