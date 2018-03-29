// Created on: 1998-01-07
// Created by: Philippe MANGIN
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


#include <Adaptor3d_HCurveOnSurface.hxx>
#include <Approx_CurveOnSurface.hxx>
#include <Approx_SameParameter.hxx>
#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRepAdaptor_HCurve.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepCheck_Edge.hxx>
#include <BRepFill_CurveConstraint.hxx>
#include <BRepFill_LocationLaw.hxx>
#include <BRepFill_SectionLaw.hxx>
#include <BRepFill_Sweep.hxx>
#include <BRepFill_TrimShellCorner.hxx>
#include <BRepLib.hxx>
#include <BRepLib_FaceError.hxx>
#include <BRepLib_FindSurface.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepTools_Substitution.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomConvert_ApproxSurface.hxx>
#include <GeomFill_LocationLaw.hxx>
#include <GeomFill_SectionLaw.hxx>
#include <GeomFill_Sweep.hxx>
#include <GeomLib.hxx>
#include <GeomLib_IsPlanarSurface.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TColGeom_Array2OfSurface.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfBoolean.hxx>
#include <TColStd_Array2OfInteger.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_Array1OfShape.hxx>
#include <TopTools_Array2OfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopTools_HArray1OfShape.hxx>
#include <TopTools_HArray2OfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>

#include <stdio.h>
//#include <BRepFill_TrimCorner.hxx>
// modified by NIZHNY-MKK  Wed Oct 22 12:25:45 2003
//#include <GeomPlate_BuildPlateSurface.hxx>
//#include <GeomPlate_Surface.hxx>
//#include <GeomPlate_PointConstraint.hxx>
//OCC500(apo)
#ifdef DRAW
#include <Draw.hxx>
#include <DrawTrSurf.hxx>
#include <DBRep.hxx>
#include <Geom_BoundedSurface.hxx>
static Standard_Boolean Affich = 0;
#endif

//=======================================================================
//function : NumberOfPoles
//purpose  : 
//=======================================================================
static Standard_Integer NumberOfPoles(const TopoDS_Wire& W)
{
  Standard_Integer NbPoints = 0;

  TopoDS_Iterator iter(W);
  for (; iter.More(); iter.Next()) 
  {
    BRepAdaptor_Curve c(TopoDS::Edge(iter.Value()));

    Standard_Real dfUf = c.FirstParameter();
    Standard_Real dfUl = c.LastParameter();
    if (IsEqual(dfUf,dfUl))
      // Degenerate
      continue;

    switch (c.GetType()) 
    {
    case GeomAbs_BezierCurve:
      {
	// Put all poles for bezier
	Handle(Geom_BezierCurve) GC = c.Bezier();
	Standard_Integer iNbPol = GC->NbPoles();
	if ( iNbPol >= 2)
	  NbPoints += iNbPol;
	break;
      }
    case GeomAbs_BSplineCurve:
      {
	// Put all poles for bspline
	Handle(Geom_BSplineCurve) GC = c.BSpline();
	Standard_Integer iNbPol = GC->NbPoles();
	if ( iNbPol >= 2)
	  NbPoints += iNbPol;
	break;
      }
    case GeomAbs_Line:
      {
	NbPoints += 2;
	break;
      }
    case GeomAbs_Circle:
    case GeomAbs_Ellipse:
    case GeomAbs_Hyperbola:
    case GeomAbs_Parabola:
      {
	NbPoints += 4;
	break;
      }
    default:
      NbPoints += 15 + c.NbIntervals(GeomAbs_C3);
    } // switch (c.GetType()) ...
  } // for (; iter.More(); iter.Next())

  return NbPoints;
}

//=======================================================================
//function : HasPCurves
//purpose  : 
//=======================================================================
static Standard_Boolean HasPCurves(const TopoDS_Edge& E)
{
  Standard_Boolean haspcurves = Standard_False;

  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->Curves());
  for (; itcr.More(); itcr.Next())
    {
      const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
      if (cr->IsCurveOnSurface())
	{
	  haspcurves = Standard_True;
	  break;
	}
    }
  return haspcurves;
}

//=======================================================================
//function : Translate
//purpose  : Copy a column from one table to another. 
//=======================================================================
static void Translate(const Handle(TopTools_HArray2OfShape)& ArrayIn,
		      const Standard_Integer In,
		      Handle(TopTools_HArray2OfShape)& ArrayOut,
		      const Standard_Integer Out)
{
  Standard_Integer ii, Nb;
  Nb =  ArrayOut->ColLength();
  for (ii=1; ii<=Nb; ii++) {
     ArrayOut->SetValue(ii, Out,  ArrayIn->Value(ii, In));
  } 
}


//=======================================================================
//function : Box
//purpose  : Bounding box of a section.
//=======================================================================
static void Box(Handle(GeomFill_SectionLaw)& Sec,
		const Standard_Real U,
		Bnd_Box& Box)

{
  Standard_Integer NbPoles, bid;
  Box.SetVoid();
  Sec->SectionShape(NbPoles, bid, bid);
  TColgp_Array1OfPnt Poles(1, NbPoles);
  TColStd_Array1OfReal W(1, NbPoles);
  Sec->D0(U, Poles, W);
  for (Standard_Integer ii=1; ii<=NbPoles; ii++) {
      Box.Add(Poles(ii));
    }  
}

//=======================================================================
//function : Couture
//purpose  : Check if E is an edge of sewing on S
//           and make the representation HadHoc
//=======================================================================
static Handle(Geom2d_Curve) Couture(const TopoDS_Edge& E, 
				    const Handle(Geom_Surface)& S,
				    const TopLoc_Location& L)
{
  TopLoc_Location l = L.Predivided(E.Location());
  Standard_Boolean Eisreversed = (E.Orientation() == TopAbs_REVERSED);

  // find the representation
  BRep_ListIteratorOfListOfCurveRepresentation itcr
    ((*((Handle(BRep_TEdge)*)&E.TShape()))->ChangeCurves());

  while (itcr.More()) {
    Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if (cr->IsCurveOnSurface(S,l)) {
      Handle(BRep_GCurve) GC (Handle(BRep_GCurve)::DownCast (cr));
      if (GC->IsCurveOnClosedSurface() && Eisreversed) 
	return  GC->PCurve2();
      else
	return GC->PCurve();
    }
    itcr.Next();
  }
  Handle(Geom2d_Curve) pc;
  pc.Nullify();
  return pc;
}

//=======================================================================
//function : CheckSameParameter
//purpose  : Check a posteriori that sameparameter has worked correctly
//=======================================================================

static Standard_Boolean CheckSameParameter 
(const Handle(Adaptor3d_HCurve)&   C3d,
 const Handle(Geom2d_Curve)&           Pcurv,
 const Handle(Adaptor3d_HSurface)& S,
 const Standard_Real             tol3d,
 Standard_Real&                  tolreached)
{
  tolreached = 0.;
  Standard_Real f = C3d->FirstParameter();
  Standard_Real l = C3d->LastParameter();
  Standard_Integer nbp = 45;
  Standard_Real step = 1./(nbp -1);
  for(Standard_Integer i = 0; i < nbp; i++){
    Standard_Real t,u,v;
    t = step * i;
    t = (1-t) * f + t * l;
    Pcurv->Value(t).Coord(u,v);
    gp_Pnt pS = S->Value(u,v);
    gp_Pnt pC = C3d->Value(t);
    Standard_Real d2 = pS.SquareDistance(pC);
    tolreached = Max(tolreached,d2);
  }
  tolreached = sqrt(tolreached);
  if(tolreached > tol3d){
    tolreached *= 2.;
    return Standard_False;
  }
  tolreached *= 2.;
  tolreached = Max(tolreached,Precision::Confusion());
  return Standard_True;
}

//=======================================================================
//function : SameParameter
//purpose  : Encapsulation of Sameparameter
// Boolean informs if the pcurve was computed or not...
// The tolerance is always OK.
//=======================================================================

static Standard_Boolean SameParameter(TopoDS_Edge&    E,
				      Handle(Geom2d_Curve)&        Pcurv,
				      const Handle(Geom_Surface)&  Surf,
				      const Standard_Real          tol3d,
				      Standard_Real&               tolreached)
{
  //Handle(BRepAdaptor_HCurve) C3d = new (BRepAdaptor_HCurve)(E);
  Standard_Real f, l;
  Handle(Geom_Curve) C3d = BRep_Tool::Curve( E, f, l );
  GeomAdaptor_Curve GAC3d( C3d, f, l );
  Handle(GeomAdaptor_HCurve) HC3d = new GeomAdaptor_HCurve( GAC3d );

  Handle(GeomAdaptor_HSurface) S = new (GeomAdaptor_HSurface)(Surf);
  Standard_Real ResTol;

  if(CheckSameParameter( HC3d, Pcurv, S, tol3d, tolreached )) 
    return Standard_True;

  if (!HasPCurves(E))
    {
      Handle(Geom2dAdaptor_HCurve) HC2d = new Geom2dAdaptor_HCurve( Pcurv );
      Approx_CurveOnSurface AppCurve(HC2d, S, HC2d->FirstParameter(), HC2d->LastParameter(),
				     Precision::Confusion(), GeomAbs_C1, 10, 10, Standard_True);
      if (AppCurve.IsDone() && AppCurve.HasResult())
	{
	  C3d = AppCurve.Curve3d();
	  tolreached = AppCurve.MaxError3d();
	  BRep_Builder B;
	  B.UpdateEdge( E, C3d, tolreached );
	  return Standard_True;
	}
    }

  const Handle(Adaptor3d_HCurve)& aHCurve = HC3d; // to avoid ambiguity
  Approx_SameParameter sp (aHCurve, Pcurv, S, tol3d );
  if(sp.IsDone() && !sp.IsSameParameter()) Pcurv = sp.Curve2d();
  else if(!sp.IsDone() && !sp.IsSameParameter()){
#ifdef OCCT_DEBUG
    cout<<"echec SameParameter"<<endl;
#endif  
    return Standard_False;
  }

  ResTol = sp.TolReached();
  if(ResTol > tolreached ){
#ifdef OCCT_DEBUG
    cout<<"SameParameter : Tolerance not reached!"<<endl;
    cout<<"tol visee : "<<tol3d<<" tol obtained : "<<ResTol<<endl;
#endif  
    return Standard_False;
  }
  else {
    tolreached = 1.1*ResTol;
    if(sp.IsDone() && !sp.IsSameParameter()) Pcurv = sp.Curve2d();
  }
  return Standard_True;
}

static void CorrectSameParameter(TopoDS_Edge&       theEdge,
                                 const TopoDS_Face& theFace1,
                                 const TopoDS_Face& theFace2)
{
  if (BRep_Tool::Degenerated(theEdge))
    return;
  
  Standard_Real fpar, lpar;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve(theEdge, fpar, lpar);

  Standard_Boolean PCurveExists [2] = {Standard_False, Standard_False};
  BRepAdaptor_Curve BAcurve [2];
  
  if (!theFace1.IsNull())
  {
    PCurveExists[0] = Standard_True;
    BAcurve[0].Initialize(theEdge, theFace1);
  }
  if (!theFace1.IsNull() &&
      theFace1.IsSame(theFace2))
    theEdge.Reverse();
  if (!theFace2.IsNull())
  {
    PCurveExists[1] = Standard_True;
    BAcurve[1].Initialize(theEdge, theFace2);
  }

  Standard_Real MaxSqDist = 0.;
  const Standard_Integer NCONTROL = 23;
  Standard_Real delta = (lpar - fpar)/NCONTROL;

  for (Standard_Integer i = 0; i <= NCONTROL; i++)
  {
    Standard_Real aParam = fpar + i*delta;
    gp_Pnt aPnt = aCurve->Value(aParam);
    for (Standard_Integer j = 0; j < 2; j++)
      if (PCurveExists[j])
      {
        gp_Pnt aPntFromFace = BAcurve[j].Value(aParam);
        Standard_Real aSqDist = aPnt.SquareDistance(aPntFromFace);
        if (aSqDist > MaxSqDist)
          MaxSqDist = aSqDist;
      }
  }

  Standard_Real aTol = sqrt(MaxSqDist);
  BRep_Builder BB;
  BB.UpdateEdge(theEdge, aTol);
}

//=======================================================================
//Objet : Orientate an edge of natural restriction 
//      : General
//=======================================================================
static void Oriente(const Handle(Geom_Surface)& S,
		    TopoDS_Edge& E)
{
  gp_Pnt2d P;
  gp_Vec2d D, URef(1, 0), VRef(0, 1);
  Standard_Boolean isuiso, isfirst, isopposite;
  Standard_Real UFirst, ULast, VFirst, VLast, f, l;
  S->Bounds(UFirst, ULast, VFirst, VLast);
  Handle(Geom2d_Curve) C;
  TopLoc_Location bid;

  C = BRep_Tool::CurveOnSurface(E, S, bid, f, l);
  C->D1((f+l)/2, P, D);

  isuiso = D.IsParallel(VRef, 0.1);
  
  if ( isuiso ) {
    isfirst = (Abs (P.X()-UFirst) < Precision::Confusion());
    isopposite = D.IsOpposite(VRef, 0.1);
    E.Orientation(TopAbs_REVERSED);
  }
  else {
    isfirst = (Abs (P.Y()-VFirst) < Precision::Confusion());
    isopposite = D.IsOpposite(URef, 0.1);
    E.Orientation(TopAbs_FORWARD);
  }

  if (!isfirst)   E.Reverse();
  if (isopposite) E.Reverse();
}
//OCC500(apo)->
static void UpdateEdgeOnPlane(const TopoDS_Face& F, const TopoDS_Edge& E,
			      const BRep_Builder& BB)
{
  Standard_Real f, l;
  Handle(Geom2d_Curve) C2d = BRep_Tool::CurveOnSurface(E,F,f,l);
  Handle(Geom_Surface) S = BRep_Tool::Surface(F);
  TopLoc_Location Loc;
  Standard_Real Tol = BRep_Tool::Tolerance(E);
  BB.UpdateEdge(E, C2d, S, Loc, Tol);
  BRepCheck_Edge Check(E);
  Tol = Max(Tol,Check.Tolerance());
  BB.UpdateEdge(E, Tol);
  TopoDS_Vertex V; 
  Tol *= 1.01;
  V = TopExp::FirstVertex(E);
  if(BRep_Tool::Tolerance(V) < Tol) BB.UpdateVertex(V, Tol);
  V = TopExp::LastVertex(E);
  if(BRep_Tool::Tolerance(V) < Tol) BB.UpdateVertex(V, Tol);
}
//<-OCC500(apo)
//=======================================================================
//Function : BuildFace
//Objet : Construct a Face via a surface and 4 Edges (natural borders)
//      : Only one Hypothesis : isos u and v are switched :
//        Edge1/3 are iso u (recp v)
//        Edge2/4 are iso v (recp u)
//=======================================================================
static void BuildFace(const Handle(Geom_Surface)& S,
		      const TopoDS_Edge& E1,
		      const TopoDS_Edge& E2,
		      const TopoDS_Edge& E3,
		      const TopoDS_Edge& E4,
		      TopTools_DataMapOfShapeShape& EEmap,
		      const Standard_Boolean ExchUV,
		      const Standard_Boolean UReverse,
		      TopoDS_Face& F)
{
  Standard_Real Tol;
// Class BRep_Tool without fields and without Constructor :
//  BRep_Tool BT;
  TopoDS_Edge e1, e2, E;
  TopoDS_Wire WW;
  BRep_Builder BB;
  BRepBuilderAPI_MakeWire B;
  TopoDS_Iterator Iter;
  //gp_Pnt2d P;

  //Is the surface planar ?
  Standard_Real Tol1, Tol2, Tol3, Tol4;
  Tol1 = BRep_Tool::Tolerance( E1 );
  Tol2 = BRep_Tool::Tolerance( E2 );
  Tol3 = BRep_Tool::Tolerance( E3 );
  Tol4 = BRep_Tool::Tolerance( E4 );
//  Tol = Min( BT.Tolerance(E1), BT.Tolerance(E2));
  Tol = Min( Tol1, Tol2 );
//  Tol = Min(Tol, Min(BT.Tolerance(E3),BT.Tolerance(E4)));
  Tol = Min( Tol, Min( Tol3, Tol4 ) );
  Standard_Boolean IsPlan = Standard_False;
  Handle(Geom_Plane) thePlane;

  if (!E1.IsSame(E3) && !E2.IsSame(E4)) //exclude cases with seam edges: they are not planar
    {
      GeomLib_IsPlanarSurface IsP(S, Tol);
      if (IsP.IsPlanar())
	{
	  IsPlan = Standard_True;
	  thePlane = new Geom_Plane( IsP.Plan() );
	}
      else
	{
	  Handle(BRep_TEdge)& TE1 = *((Handle(BRep_TEdge)*)&E1.TShape());
	  Handle(BRep_TEdge)& TE2 = *((Handle(BRep_TEdge)*)&E2.TShape());
	  Handle(BRep_TEdge)& TE3 = *((Handle(BRep_TEdge)*)&E3.TShape());
	  Handle(BRep_TEdge)& TE4 = *((Handle(BRep_TEdge)*)&E4.TShape());
	  TE1->Tolerance( Precision::Confusion() );
	  TE2->Tolerance( Precision::Confusion() );
	  TE3->Tolerance( Precision::Confusion() );
	  TE4->Tolerance( Precision::Confusion() );
	  
	  TopoDS_Wire theWire = BRepLib_MakeWire( E1, E2, E3, E4 );
	  Standard_Integer NbPoints = NumberOfPoles( theWire );
	  if (NbPoints <= 100) //limitation for CPU
	    {
	      BRepLib_FindSurface FS( theWire, -1, Standard_True );
	      if (FS.Found())
		{
		  IsPlan = Standard_True;
		  thePlane = Handle(Geom_Plane)::DownCast(FS.Surface());
		}
	    }
	  BB.UpdateEdge( E1, Tol1 );
	  BB.UpdateEdge( E2, Tol2 );
	  BB.UpdateEdge( E3, Tol3 );
	  BB.UpdateEdge( E4, Tol4 );
	}
    }

  // Construction of the wire
//  B.MakeWire(WW);
  e1 = E1;
  Oriente(S, e1);
//  if (!IsPlan || !BT.Degenerated(e1)) 
  if (!IsPlan || !BRep_Tool::Degenerated(e1)) 
    B.Add(e1);

  e2 = E2;
  Oriente(S, e2);  
//  if (!IsPlan || !BT.Degenerated(e2)) 
  if (!IsPlan || !BRep_Tool::Degenerated(e2))
    {
      B.Add(e2);
      if (!BRep_Tool::Degenerated(e2))
	{
	  WW = B.Wire();
	  TopoDS_Shape NewEdge;
	  //take the last edge added to WW
	  for (Iter.Initialize( WW ); Iter.More(); Iter.Next())
	    NewEdge = Iter.Value();
	  if (! e2.IsSame(NewEdge))
	    EEmap.Bind( e2, NewEdge );
	}
    }

  if (E3.IsSame(E1)) {
    E = e1;
    E.Reverse();
  }
  else {
    E = E3;
    Oriente(S, E);
  }
//  if (!IsPlan || !BT.Degenerated(E))   
  if (!IsPlan || !BRep_Tool::Degenerated(E))
    {
      B.Add(E);
      if (!BRep_Tool::Degenerated(E))
	{
	  WW = B.Wire();
	  TopoDS_Shape NewEdge;
	  //take the last edge added to WW
	  for (Iter.Initialize( WW ); Iter.More(); Iter.Next())
	    NewEdge = Iter.Value();
	  if (! E.IsSame(NewEdge))
	    EEmap.Bind( E, NewEdge );
	}
    }

  if (E4.IsSame(E2)) {
    E = e2;
    E.Reverse();
  }
  else {
    E = E4;
    Oriente(S, E);
  }
//  if (!IsPlan || !BT.Degenerated(E))  
  if (!IsPlan || !BRep_Tool::Degenerated(E)) 
    {
      B.Add(E);
      if (!BRep_Tool::Degenerated(E))
	{
	  WW = B.Wire();
	  TopoDS_Shape NewEdge;
	  //take the last edge added to WW
	  for (Iter.Initialize( WW ); Iter.More(); Iter.Next())
	    NewEdge = Iter.Value();
	  if (! E.IsSame(NewEdge))
	    EEmap.Bind( E, NewEdge );
	}
    }

  WW = B.Wire();
#if DRAW
  if (Affich)
    DBRep::Set("wire-on-face", WW);
#endif
  
// Construction of the face.
  if (IsPlan) { // Suspend representation 2d 
    // and construct face Plane

    //BRepLib_MakeFace MkF(IsP.Plan(), WW);
    gp_Pnt aPnt;
    gp_Vec DU, DV, NS, NP;
    Standard_Real Ufirst, Ulast, Vfirst, Vlast;
    S->Bounds( Ufirst, Ulast, Vfirst, Vlast );
    S->D1( (Ufirst+Ulast)/2., (Vfirst+Vlast)/2., aPnt, DU, DV );
    NS = DU ^ DV;
    NP = thePlane->Pln().Axis().Direction();
    if (NS.Dot(NP) < 0.)
      thePlane->UReverse();
    BRepLib_MakeFace MkF( thePlane, WW );
    if (MkF.Error() != BRepLib_FaceDone) {
#ifdef OCCT_DEBUG
      BRepLib_FaceError Err = MkF.Error();
      cout << "Planar Face Error :" <<   Err << endl;
#endif
    }
    else {
      Handle(Geom2d_Curve) NullC2d;
      TopLoc_Location Loc;
      BB.UpdateEdge( E1, NullC2d, S, Loc, Tol1 );
      BB.UpdateEdge( E2, NullC2d, S, Loc, Tol2 );
      BB.UpdateEdge( E3, NullC2d, S, Loc, Tol3 );
      BB.UpdateEdge( E4, NullC2d, S, Loc, Tol4 );

      F =  MkF.Face();
      UpdateEdgeOnPlane(F,E1,BB);
      UpdateEdgeOnPlane(F,E2,BB);
      UpdateEdgeOnPlane(F,E3,BB);
      UpdateEdgeOnPlane(F,E4,BB);
/*OCC500(apo)->
      TopLoc_Location Loc;
      Handle(Geom2d_Curve) NC;
      NC.Nullify();
//      B.UpdateEdge(E1, NC, S, Loc, BT.Tolerance(E1));
      BB.UpdateEdge(E1, NC, S, Loc, BRep_Tool::Tolerance(E1));
//      B.UpdateEdge(E2, NC, S, Loc, BT.Tolerance(E2));
      BB.UpdateEdge(E2, NC, S, Loc, BRep_Tool::Tolerance(E2));
//      B.UpdateEdge(E3, NC, S, Loc, BT.Tolerance(E3));
      BB.UpdateEdge(E3, NC, S, Loc, BRep_Tool::Tolerance(E3));
//      B.UpdateEdge(E4, NC, S, Loc, BT.Tolerance(E4));
      BB.UpdateEdge(E4, NC, S, Loc, BRep_Tool::Tolerance(E4));
<-OCC500(apo)*/
    }  
  }

  if (!IsPlan) {// Cas Standard : Ajout
    BB.MakeFace(F, S, Precision::Confusion());
    BB.Add(F, WW);
  }

  // Reorientation
  if (ExchUV) F.Reverse();
  if (UReverse) F.Reverse();
}


//=======================================================================
//Fonction : BuildEdge
//Objet : Construct non-closed Edge 
//=======================================================================
static TopoDS_Edge BuildEdge(Handle(Geom_Curve)& C3d,
			     Handle(Geom2d_Curve)& C2d,
			     Handle(Geom_Surface)& S,
			     const TopoDS_Vertex& VF,
			     const TopoDS_Vertex& VL,
			     const Standard_Real f, 
			     const Standard_Real l,
			     const Standard_Real Tol3d)
{
  gp_Pnt P1, P2, P;
  Standard_Real Tol1, Tol2, Tol, d;
// Class BRep_Tool without fields and without Constructor :
//  BRep_Tool BT;
  BRep_Builder B;
  TopoDS_Edge E;

//  P1  = BT.Pnt(VF);
  P1  = BRep_Tool::Pnt(VF);
//  Tol1 = BT.Tolerance(VF);
  Tol1 = BRep_Tool::Tolerance(VF);
//  P2  = BT.Pnt(VL);
  P2  = BRep_Tool::Pnt(VL);
//  Tol2 = BT.Tolerance(VF);
  Tol2 = BRep_Tool::Tolerance(VL);
  Tol = Max(Tol1, Tol2);

  if (VF.IsSame(VL) || 
      (P1.Distance(P2) < Tol ) ) { 
    // Degenerated case
    gp_Pnt2d P2d;
    C2d->D0(f, P2d);
    S->D0(P2d.X(), P2d.Y(), P);
    d = P1.Distance(P);
    if (d > Tol) Tol = d;
    C2d->D0(l, P2d);
    S->D0(P2d.X(), P2d.Y(), P);
    d = P2.Distance(P);
    if (d > Tol) Tol = d;

    B.UpdateVertex(VF, Tol);
    B.UpdateVertex(VL, Tol);

    B.MakeEdge(E);
    B.UpdateEdge(E,C2d,S,TopLoc_Location(), Tol);
    B.Add(E,VF);
    B.Add(E,VL);
    B.Range(E,f,l);
    B.Degenerated(E, Standard_True);

    return E;
  }

  C3d->D0(f, P);
  d = P1.Distance(P);
  if (d > Tol1)
      B.UpdateVertex(VF, d);

//  P1 = BT.Pnt(VL);
  P1 = BRep_Tool::Pnt(VL);
  C3d->D0(l, P);
  d = P2.Distance(P);
  if (d > Tol2)
      B.UpdateVertex(VL, d); 

  BRepLib_MakeEdge MkE (C3d, VF, VL, f, l);
  if (!MkE.IsDone()) { // Error of construction !!     
#ifdef DRAW    
    char name[100];
    sprintf(name,"firstvertex_error");
    DBRep::Set(name, VF);
    sprintf(name,"lastvertex_error");
    DBRep::Set(name, VL);
    sprintf(name,"curve3d_error");
    char* Temp = name ;
    DrawTrSurf::Set(Temp, C3d);
//    DrawTrSurf::Set(name, C3d);
    throw Standard_ConstructionError("BRepFill_Sweep::BuildEdge");
#endif
  }

  E = MkE.Edge();
  TopLoc_Location Loc;
  B.UpdateEdge(E, C2d, S, Loc, Tol3d);

  return E;
}

	
//=======================================================================
//Fonction : Filling
//Objet : Construct the faces of filling
//=======================================================================
static Standard_Boolean Filling(const TopoDS_Shape& EF,
				const TopoDS_Shape& F1,
				const TopoDS_Shape& EL,
				const TopoDS_Shape& F2,
				TopTools_DataMapOfShapeShape& EEmap,
				const Standard_Real Tol,
				const  gp_Ax2& Axe,
				const  gp_Vec& TangentOnPart1,
				TopoDS_Edge& Aux1,
				TopoDS_Edge& Aux2,
				TopoDS_Face& Result)
{
  BRep_Builder B;
// Class BRep_Tool without fields and without Constructor :
//  BRep_Tool BT;
//  Standard_Integer NbInt =0;
//  Standard_Real Tol3d = Tol;
  Standard_Boolean WithE3, WithE4;

// Return constraints
  TopoDS_Vertex V1, V2, Vf, Vl;
  TopoDS_Edge E1, E2, E3, E4;
  E1 = TopoDS::Edge(EF);
  E2 = TopoDS::Edge(EL);
  
  TopExp::Vertices(E1, Vf, Vl);
  Vf.Orientation(TopAbs_FORWARD);
  Vl.Orientation(TopAbs_FORWARD);

  TopExp::Vertices(E2, V1, V2);
  V1.Orientation(TopAbs_REVERSED);
  V2.Orientation(TopAbs_REVERSED);  
  
  B.MakeEdge(E3);
  B.MakeEdge(E4);

  WithE3 = WithE4 = Standard_False;

  if ((!Aux1.IsNull()) && (!Vf.IsSame(V1))) {
    E3 = Aux1;
//    E3 = TopoDS::Edge(Aux1);
    WithE3 = Standard_True;
  }

  if (Vf.IsSame(Vl)) {
    E4 = E3;
    E4.Reverse();
    WithE4 = WithE3;
  }
  else if (!Aux2.IsNull() && (!Vl.IsSame(V2))) {
    E4 = Aux2;
//    E4 = TopoDS::Edge(Aux2);
    WithE4 = Standard_True;
  }

#if DRAW
  if (Affich) {
    DBRep::Set("Fill_Edge1", E1);
    DBRep::Set("Fill_Edge2", E2);
    if (!E3.IsNull())
      DBRep::Set("Fill_Edge3", E3);
    if (!E4.IsNull())
      DBRep::Set("Fill_Edge4", E4);
  }
#endif

// Construction of a surface of revolution
  Handle(Geom_Curve) Prof1, Prof2;
  //Standard_Integer ii, jj;//, Nb;
  Standard_Real f1, f2, l1, l2,/*d1, d2,*/ Angle;//, Eps = 1.e-9;
//  Prof1 = BT.Curve(E1, f1, l1);
  Prof1 = BRep_Tool::Curve(E1, f1, l1);
//  Prof2 = BT.Curve(E2, f2, l2);
  Prof2 = BRep_Tool::Curve(E2, f2, l2);

  // Indeed, both Prof1 and Prof2 are the same curves but in different positions

  gp_Pnt P1, P2, P;

  // Choose the angle of opening
  gp_Trsf aTf;
  aTf.SetTransformation(Axe);

  // Choose the furthest point from the "center of revolution"
  // to provide correct angle measurement.
  const Standard_Real aPrm[] = {f1, 0.5*(f1 + l1), l1};
  const gp_Pnt aP1[] = {Prof1->Value(aPrm[0]).Transformed(aTf),
                        Prof1->Value(aPrm[1]).Transformed(aTf),
                        Prof1->Value(aPrm[2]).Transformed(aTf)};

  Standard_Integer aMaxIdx = -1;
  Standard_Real aMaxDist = RealFirst();
  for (Standard_Integer i = 0; i < 3; i++)
  {
    const Standard_Real aDist = aP1[i].X()*aP1[i].X() + aP1[i].Z()*aP1[i].Z();
    if (aDist > aMaxDist)
    {
      aMaxDist = aDist;
      aMaxIdx = i;
    }
  }

  const gp_Pnt aP2 = Prof2->Value(aPrm[aMaxIdx]).Transformed(aTf);
  const gp_Vec2d aV1(aP1[aMaxIdx].Z(), aP1[aMaxIdx].X());
  const gp_Vec2d aV2(aP2.Z(), aP2.X());
  if (aV1.SquareMagnitude() <= gp::Resolution() ||
      aV2.SquareMagnitude() <= gp::Resolution())
  {
    return Standard_False;
  }

  Angle = aV1.Angle(aV2);

  gp_Ax1 axe(Axe.Location(), Axe.YDirection());

  if (Angle < 0) {
    Angle = -Angle;
    axe.Reverse();
  }

  Handle(Geom_SurfaceOfRevolution) Rev = 
    new (Geom_SurfaceOfRevolution) (Prof1, axe);

  Handle(Geom_Surface) Surf = 
    new (Geom_RectangularTrimmedSurface) (Rev, 0, Angle, f1, l1);

  // Control the direction of the rotation
  Standard_Boolean ToReverseResult = Standard_False;
  gp_Vec d1u;
  d1u = Surf->DN(0, (f1+l1)/2, 1, 0);
  if (d1u.Angle(TangentOnPart1) > M_PI/2) { //Invert everything
    ToReverseResult = Standard_True;
    /*
    axe.Reverse();
    Angle = 2*M_PI - Angle;
    Rev = new (Geom_SurfaceOfRevolution) (Prof1, axe);
    Surf = new (Geom_RectangularTrimmedSurface) 
           (Rev, 0, Angle, f1, l1);
    */
  }

#if DRAW
  if (Affich) { 
      char* Temp = "Surf_Init" ;
      DrawTrSurf::Set(Temp, Surf);
    }
#endif

  Handle(Geom2d_Curve) C1, C2, C3, C4;
/*
// Deform the surface of revolution.
  GeomPlate_BuildPlateSurface BPS;

  Handle(BRepAdaptor_HSurface) AS;
  Handle(BRepAdaptor_HCurve2d) AC2d;
  Handle(Adaptor3d_HCurveOnSurface) HConS;
*/
  Handle(Geom2d_Line) L;
  gp_Pnt2d P2d(0.,0.);

  L = new (Geom2d_Line) (P2d, gp::DY2d());
  C1 = new (Geom2d_TrimmedCurve) (L, f1, l1);

  P2d.SetCoord(Angle,0.);
  L = new (Geom2d_Line) (P2d, gp::DY2d());
  C2 = new (Geom2d_TrimmedCurve) (L, f1, l1);
 
  // It is required to control the direction and the range.
  C2->D0(f1, P2d);
  Surf->D0(P2d.X(), P2d.Y(), P1);
  C2->D0(l1, P2d);
  Surf->D0(P2d.X(), P2d.Y(), P2);
//  P = BT.Pnt(V1);
  P = BRep_Tool::Pnt(V1);
  if (P.Distance(P2)+Tol < P.Distance(P1)) {
    // E2 is parsed in the direction opposite to E1
    C2->Reverse();
    TopoDS_Vertex aux;
    aux = V2;
    V2 = V1;
    V1 = aux;
  }
  GeomLib::SameRange(Precision::PConfusion(), C2, 
		     C2->FirstParameter(),
		     C2->LastParameter(),
		     f2, l2, C3);
  C2 = C3;

//  P1 = BT.Pnt(Vf);
  P1 = BRep_Tool::Pnt(Vf);
//  P2 = BT.Pnt(V1);
  P2 = BRep_Tool::Pnt(V1);
//  pointu_f = Vf.IsSame(V1) || (P1.Distance(P2) < BT.Tolerance(Vf));
//  P1 = BT.Pnt(Vl);
  P1 = BRep_Tool::Pnt(Vl);
//  P2 = BT.Pnt(V2);
  P2 = BRep_Tool::Pnt(V2);
//  pointu_l = Vl.IsSame(V2) || (P1.Distance(P2) < BT.Tolerance(Vl));

  P2d.SetCoord(0.,f1);
  L = new (Geom2d_Line) (P2d, gp::DX2d());
  C3 = new (Geom2d_TrimmedCurve) (L, 0, Angle);


  P2d.SetCoord(0.,l1);
  L = new (Geom2d_Line) (P2d, gp::DX2d());   
  C4 = new (Geom2d_TrimmedCurve) (L, 0, Angle);
/*
  // Determine the constraints and  
  // their parametric localisation.
  if (!E1.IsNull()) {
     AS = new BRepAdaptor_HSurface(TopoDS::Face(F1));
     AC2d = new BRepAdaptor_HCurve2d();
     AC2d->ChangeCurve2d().Initialize(E1,TopoDS::Face(F1));
     HConS = new (Adaptor3d_HCurveOnSurface)();
     HConS->ChangeCurve().Load(AC2d);
     HConS->ChangeCurve().Load(AS);

     Handle(BRepFill_CurveConstraint) Cont
      = new BRepFill_CurveConstraint(HConS, 1, 15);
     Cont->SetCurve2dOnSurf(C1);

     BPS.Add(Cont);
     NbInt = HConS->NbIntervals(GeomAbs_CN);
   }

  if (!E2.IsNull()) {
    AS = new BRepAdaptor_HSurface(TopoDS::Face(F2));
    AC2d = new BRepAdaptor_HCurve2d();
    AC2d->ChangeCurve2d().Initialize(E2,TopoDS::Face(F2));
    HConS = new (Adaptor3d_HCurveOnSurface);

    HConS->ChangeCurve().Load(AC2d);
    HConS->ChangeCurve().Load(AS);

    Handle(BRepFill_CurveConstraint) Cont
      = new BRepFill_CurveConstraint(HConS, 1, 15);
    Cont->SetCurve2dOnSurf(C2);
    BPS.Add(Cont);
  }

  if (WithE3) {
    Handle(BRepAdaptor_HCurve) AC = new (BRepAdaptor_HCurve) (E3);
    Handle(BRepFill_CurveConstraint) Cont
      = new BRepFill_CurveConstraint(AC, 0);
    Cont->SetCurve2dOnSurf(C3);
    BPS.Add(Cont);
  } 
  else if (pointu_f) {
    Standard_Real delta = Angle / 11;
//    P = BT.Pnt(Vf);
    P = BRep_Tool::Pnt(Vf);
    Handle(GeomPlate_PointConstraint) PC;
    for (ii=1; ii<=10; ii++) {
      C3->D0(ii*delta, P2d);
      PC = new (GeomPlate_PointConstraint) (P, 0); 
      PC->SetPnt2dOnSurf(P2d);
      BPS.Add(PC);
    }
  }


  if (WithE4) {
    Handle(BRepAdaptor_HCurve) AC = new (BRepAdaptor_HCurve) (E4);
    Handle(BRepFill_CurveConstraint) Cont
      = new BRepFill_CurveConstraint(AC, 0);
    Cont->SetCurve2dOnSurf(C4);
    BPS.Add(Cont);
  }
  else if (pointu_l) {
    Standard_Real delta = Angle / 11;
//    P = BT.Pnt(Vl);
    P = BRep_Tool::Pnt(Vl);
    Handle(GeomPlate_PointConstraint) PC;
    for (ii=1; ii<=10; ii++) {
      C4->D0(ii*delta, P2d);
      PC = new (GeomPlate_PointConstraint) (P, 0); 
      PC->SetPnt2dOnSurf(P2d);
      BPS.Add(PC);
    }
  }

  BPS.LoadInitSurface(Surf);
  BPS.Perform();

  // Controle s'il y a une deformation effective
  Handle(GeomPlate_Surface) plate;
  plate = BPS.Surface();
  plate->SetBounds(0, Angle, f1, l1);
  Standard_Boolean Ok=Standard_True;
  Standard_Real u, v;
  d1 = (l1-f1)/5;
  d2 = Angle/5;
  for (ii=0; ii<=5 && Ok; ii++) {
    u = f1 + ii*d1;
    for (jj=0; jj<=5 && Ok; jj++) {
      v = jj*d2;
      plate->D0(u, v, P1);
      Surf->D0(u, v, P2);
      Ok = (P2.IsEqual(P1, Tol)); 
    }
  }


  if (!Ok) {
    // Approx de la plate surface
    // Bords naturelles => pas besoin de criteres.
    GeomConvert_ApproxSurface App(BPS.Surface(), 
				  Tol3d,
				  GeomAbs_C1, GeomAbs_C1,
				  8, 8, 2*NbInt, 0);
    if (!App.HasResult()) {
#ifdef OCCT_DEBUG
      cout << "Filling_Approx : Pas de resultat" << endl;
#endif      
      return Standard_False;
    }
#ifdef OCCT_DEBUG
    cout <<  "Filling_Approx Error 3d = " << 
      App.MaxError() << endl;
#endif
    Surf = App.Surface();
    Tol3d = App.MaxError();  
  }
*/

// Update des Edges
  TopLoc_Location Loc;
  Handle(Geom_Curve) C3d;
  B.UpdateEdge(E1, C1, Surf, Loc, /*Tol3d*/Precision::Confusion());
  B.UpdateEdge(E2, C2, Surf, Loc, /*Tol3d*/Precision::Confusion());
 
  if (E3.IsSame(E4)) {
    if (!WithE3)
      {
	C3d = Surf->VIso(f1);
	E3 = BuildEdge(C3d, C3, Surf, Vf, V1, 0, Angle, /*Tol3d*/Precision::Confusion());
      }
    else
      {
	BRepAdaptor_Curve aCurve(E3);
	Standard_Real AngleOld = aCurve.LastParameter();
	if (Angle > AngleOld)
	  {
	    B.Range( E3, 0, Angle );
	    TopoDS_Vertex V (TopExp::LastVertex(E3));
	    Handle(BRep_TVertex)& TVlast = *((Handle(BRep_TVertex)*) &V.TShape());
	    TVlast->Tolerance( Precision::Confusion() );
	  }
      }

    B.UpdateEdge(E3, C3, C4, Surf, Loc, /*Tol3d*/Precision::Confusion());
    E4 = E3;
    E4.Reverse();
  }
  else {
    if (!WithE3)
      {
	C3d = Surf->VIso(f1);
	E3 = BuildEdge(C3d, C3, Surf, Vf, V1, 0, Angle, /*Tol3d*/Precision::Confusion());
      }
    else
      {
	BRepAdaptor_Curve aCurve(E3);
	Standard_Real AngleOld = aCurve.LastParameter();
	if (Angle > AngleOld)
	  {
	    B.Range( E3, 0, Angle );
	    TopoDS_Vertex V(TopExp::LastVertex(E3));
	    Handle(BRep_TVertex)& TVlast = *((Handle(BRep_TVertex)*) &V.TShape());
	    TVlast->Tolerance( Precision::Confusion() );
	  }

	B.UpdateEdge(E3, C3, Surf, Loc, /*Tol3d*/Precision::Confusion());
      }
    
    if (!WithE4)
      {
	C3d = Surf->VIso(l1);
	E4 = BuildEdge(C3d, C4, Surf, Vl, V2, 0, Angle, /*Tol3d*/Precision::Confusion());
      }
    else
      {
	BRepAdaptor_Curve aCurve(E4);
	Standard_Real AngleOld = aCurve.LastParameter();
	if (Angle > AngleOld)
	  {
	    B.Range( E4, 0, Angle );
	    TopoDS_Vertex V (TopExp::LastVertex(E4));
	    Handle(BRep_TVertex)& TVlast = *((Handle(BRep_TVertex)*)&V.TShape());
	    TVlast->Tolerance( Precision::Confusion() );
	  }

	B.UpdateEdge(E4, C4, Surf, Loc, /*Tol3d*/Precision::Confusion());
      }
  }

//Construct face
  BuildFace(Surf,E1, E3, E2, E4, EEmap,
	    Standard_False, Standard_False, 
	    Result);

// Set the continuities.
  B.Continuity(E1, TopoDS::Face(F1), Result, GeomAbs_G1);
  B.Continuity(E2, TopoDS::Face(F2), Result, GeomAbs_G1);

// Render the calculated borders.
//  if (!BT.Degenerated(E3))
  if (!BRep_Tool::Degenerated(E3))
    Aux1 = E3;
  else
    B.MakeEdge(Aux1); //Nullify

//  if (!BT.Degenerated(E4))
  if (!BRep_Tool::Degenerated(E4))
    Aux2 = E4;
  else
    B.MakeEdge(Aux2);

  // Set the orientation

  // Provide correct normals computation
  // (the normal will be computed not in 
  // singularity point definitely).
  Angle = RealFirst();
  for (Standard_Integer i = 0; i < 3; i++)
  {
    gp_Vec D1U, D1V, N1, N2;
    C1->D0(aPrm[i], P2d);
    Surf->D1(P2d.X(), P2d.Y(), P, D1U, D1V);
    N1 = D1U^D1V;

    if (N1.SquareMagnitude() < Precision::SquareConfusion())
      continue;

    //  C1 = BT.CurveOnSurface(E1, TopoDS::Face(F1), f2, l2);
    C1 = BRep_Tool::CurveOnSurface(E1, TopoDS::Face(F1), f2, l2);
    C1->D0(aPrm[i], P2d);
    Handle(BRepAdaptor_HSurface) AS = new BRepAdaptor_HSurface(TopoDS::Face(F1));
    AS->D1(P2d.X(), P2d.Y(), P, D1U, D1V);
    N2 = D1U^D1V;

    if (N2.SquareMagnitude() < Precision::SquareConfusion())
      continue;

    Angle = N1.Angle(N2);

    break;
  }
  
  if (Angle == RealFirst())
    return Standard_False;

  if ( (F1.Orientation() == TopAbs_REVERSED) ^ (Angle>M_PI/2))
    Result.Orientation(TopAbs_REVERSED);
  else  Result.Orientation(TopAbs_FORWARD);

  if (ToReverseResult)
    Result.Reverse();

#if DRAW
  if (Affich) DBRep::Set("BoucheTrou", Result);
#endif

  return Standard_True;
}

//=======================================================================
//function : Substitute
//purpose  :
//=======================================================================
static void Substitute(BRepTools_Substitution& aSubstitute,
		       const TopoDS_Edge& Old,
		       const TopoDS_Edge& New) 
{
  TopTools_ListOfShape listShape;

  TopoDS_Vertex OldV1, OldV2, NewV1, NewV2;
  TopExp::Vertices( Old, OldV1, OldV2 );
  TopExp::Vertices( New, NewV1, NewV2 );

  if (!aSubstitute.IsCopied( OldV1 ))
    {
      listShape.Append( NewV1.Oriented(TopAbs_FORWARD) );
      aSubstitute.Substitute( OldV1, listShape );
      listShape.Clear();
    }
  if (!aSubstitute.IsCopied( OldV2 ))
    {
      listShape.Append( NewV2.Oriented(TopAbs_FORWARD) );
      aSubstitute.Substitute( OldV2, listShape );
      listShape.Clear();
    }
  if (!aSubstitute.IsCopied( Old ))
    {
      listShape.Append( New.Oriented(TopAbs_FORWARD) );
      aSubstitute.Substitute( Old, listShape );
    }
}

//=======================================================================
//Function : SetCommonEdgeInFace
//Purpose  : Replace an edge of the face by the corresponding edge from
//           myUEdges
//=======================================================================
/*
static void SetCommonEdgeInFace(BRepTools_Substitution& aSubstitute,
				const TopoDS_Shape& Face,
				const TopoDS_Shape& Edge)
{
  if (Edge.IsNull())
    return;

  Standard_Boolean done = Standard_False;
// Class BRep_Tool without fields and without Constructor :
//  BRep_Tool BT;
  Standard_Real f, l;
  TopExp_Explorer Exp(Face, TopAbs_EDGE);
  Handle(Geom_Curve) Cref, C;
  TopLoc_Location Lref, L;
//  Cref = BT.Curve(TopoDS::Edge(Edge), Lref, f, l); 
  const TopoDS_Edge& NewEdge = TopoDS::Edge(Edge);
  Cref = BRep_Tool::Curve( NewEdge, Lref, f, l ); 

  for ( ; Exp.More() && !done; Exp.Next()) {
//    C = BT.Curve(TopoDS::Edge(Exp.Current()), L, f, l);
    const TopoDS_Edge& OldEdge = TopoDS::Edge(Exp.Current());
    C = BRep_Tool::Curve(OldEdge, L, f, l);
    if ((Cref==C) && (Lref == L)) {
      done = Standard_True;
      Substitute( aSubstitute, OldEdge, NewEdge );
    }
  }
#ifdef OCCT_DEBUG
  if (!done) cout << "Substitution of Edge failed" << endl;
#endif  
}
*/

//=======================================================================
//Fonction : KeepEdge
//Objet : Find edges of the face supported by the same Curve.
//=======================================================================
static void KeepEdge(const TopoDS_Shape& Face,
		     const TopoDS_Shape& Edge,
		     TopTools_ListOfShape& List)
{
  List.Clear();
// Class BRep_Tool without fields and without Constructor :
//  BRep_Tool BT;
  Standard_Real f, l;
  TopExp_Explorer Exp(Face, TopAbs_EDGE);
  Handle(Geom_Curve) Cref, C;
  TopLoc_Location Lref, L;
//  Cref = BT.Curve(TopoDS::Edge(Edge), Lref, f, l); 
  Cref = BRep_Tool::Curve(TopoDS::Edge(Edge), Lref, f, l); 

  for ( ; Exp.More(); Exp.Next()) {
//    C = BT.Curve(TopoDS::Edge(Exp.Current()), L, f, l);
    C = BRep_Tool::Curve(TopoDS::Edge(Exp.Current()), L, f, l);
    if ((Cref==C) && (Lref == L)) { 
      List.Append(Exp.Current());
    }
  }
}

//=======================================================================
//Function : 
//Objet : Construct a vertex via an iso
//=======================================================================
static void BuildVertex(const Handle(Geom_Curve)& Iso,
			const Standard_Boolean isfirst,
			const Standard_Real First,
			const Standard_Real Last,
			TopoDS_Shape& Vertex)
{
  BRep_Builder B;
  Standard_Real val;

  if (isfirst) val = First;
  else val = Last;
  B.MakeVertex(TopoDS::Vertex(Vertex),
	       Iso->Value(val),
	       Precision::Confusion());
}

//=======================================================================
//Function : 
//Objet : Construct an empty edge
//=======================================================================
static TopoDS_Edge NullEdge(TopoDS_Shape& Vertex)
{
  TopoDS_Edge E;
  BRep_Builder B;
  B.MakeEdge(E);
  Vertex.Orientation(TopAbs_FORWARD);
  B.Add(E, Vertex);
  B.Add(E, Vertex.Reversed());
  B.Degenerated(E, Standard_True);
  return E;

}
	
//=======================================================================
//Function : 
//Objet : Construct an edge via an iso
//=======================================================================
static TopoDS_Edge BuildEdge(const Handle(Geom_Surface)& S,
			     const Standard_Boolean isUiso,
			     const Standard_Real ValIso,
			     const TopoDS_Shape&  VFirst,
			     const TopoDS_Shape&  VLast,
			     const Standard_Real  Tol)
{
  TopoDS_Edge E;
  BRep_Builder B;
  Handle(Geom_Curve) Iso;
  Standard_Boolean sing = Standard_False;
  if (isUiso) {
    Iso = S->UIso(ValIso);
  }
  else {      
    Iso = S->VIso(ValIso);
  }

  if (VFirst.IsSame(VLast)) { // Singular case ?
    gp_Pnt P; 
// Class BRep_Tool without fields and without Constructor :
//    BRep_Tool BT;
    const TopoDS_Vertex& V = TopoDS::Vertex(VFirst);
//    Standard_Real tol = BT.Tolerance(V);
    Standard_Real tol = BRep_Tool::Tolerance(V);
    if (Tol > tol) tol = Tol; 
    Iso->D0((Iso->FirstParameter()+Iso->LastParameter())/2, P);
//    if (P.Distance(BT.Pnt(V)) < tol) {
    if (P.Distance(BRep_Tool::Pnt(V)) < tol) {
      GeomAdaptor_Curve AC(Iso);
      sing = GCPnts_AbscissaPoint::Length(AC, tol/4) < tol;
    }
  }


  if (sing) { // Singular case
    TopoDS_Shape V;
    V = VFirst;
    E = NullEdge(V);
//    Iso.Nullify();
//    B.UpdateEdge(E, Iso, Precision::Confusion());
    B.Degenerated(E, Standard_True);
  }
  
  else {
    // Construction Via 3d
//    if (isUiso) {
//      Iso = S->UIso(ValIso);
    gp_Pnt P1,P2;
    Standard_Real p1, p2, p11, p12, p21, p22;
    Standard_Boolean fwd = Standard_False;
    p1 = Iso->FirstParameter();
    p2 = Iso->LastParameter();
    P1 = Iso->Value(p1);
    P2 = Iso->Value(p2);

    Standard_Real t1 = BRep_Tool::Tolerance(TopoDS::Vertex(VFirst));
    Standard_Real t2 = BRep_Tool::Tolerance(TopoDS::Vertex(VLast));

    BRep_Builder BB;

    p11 = P1.Distance(BRep_Tool::Pnt(TopoDS::Vertex(VFirst)));
    p22 = P2.Distance(BRep_Tool::Pnt(TopoDS::Vertex(VLast)));
    p12 = P1.Distance(BRep_Tool::Pnt(TopoDS::Vertex(VLast)));
    p21 = P2.Distance(BRep_Tool::Pnt(TopoDS::Vertex(VFirst)));
    
    if(p11 < p12 && p22 < p21) fwd = Standard_True;

    if(fwd) { //OCC500(apo)
      if (p11 >= t1) BB.UpdateVertex(TopoDS::Vertex(VFirst), 1.01*p11);
      if (p22 >= t2) BB.UpdateVertex(TopoDS::Vertex(VLast), 1.01*p22);
    }
    else {      
//      Iso = S->VIso(ValIso);
      if (p12 >= t2) BB.UpdateVertex(TopoDS::Vertex(VLast), 1.01*p12);
      if (p21 >= t1) BB.UpdateVertex(TopoDS::Vertex(VFirst), 1.01*p21);
    }

    BRepLib_MakeEdge MkE;

//    MkE.Init(Iso, 
//	     TopoDS::Vertex(VFirst), 
//	     TopoDS::Vertex(VLast), 
//	     Iso->FirstParameter(),
//	     Iso->LastParameter());
    if(fwd)
      MkE.Init(Iso, 
	       TopoDS::Vertex(VFirst), 
	       TopoDS::Vertex(VLast), 
	       Iso->FirstParameter(),
	       Iso->LastParameter());
    else
      MkE.Init(Iso,
	       TopoDS::Vertex(VLast),
	       TopoDS::Vertex(VFirst), 
	       Iso->FirstParameter(),
	       Iso->LastParameter());

//    if (!MkE.IsDone()) { // Il faut peut etre permuter les Vertex
//      MkE.Init(Iso,
//	       TopoDS::Vertex(VLast),
//	       TopoDS::Vertex(VFirst), 
//	       Iso->FirstParameter(),
//	       Iso->LastParameter());
//    }

    if (!MkE.IsDone()) { // Erreur de construction !!     
#ifdef DRAW
      char name[100];
      sprintf(name,"firstvertex_error");
      DBRep::Set(name, VFirst);
      sprintf(name,"lastvertex_error");
      DBRep::Set(name, VLast);
      sprintf(name,"curve3d_error");
      char* Temp = name ;
      DrawTrSurf::Set(Temp,Iso);
//      DrawTrSurf::Set(name,Iso);
#endif
      throw Standard_ConstructionError("BRepFill_Sweep::BuildEdge");
    }

    E = MkE.Edge();
  }

  // Associate 2d
  Handle(Geom2d_Line) L;
  TopLoc_Location Loc;
  Standard_Real Umin, Umax, Vmin, Vmax;
  S->Bounds(Umin, Umax, Vmin, Vmax);
  if (isUiso) {
    //gp_Pnt2d P(ValIso, 0);
    gp_Pnt2d P( ValIso, Vmin - Iso->FirstParameter() );
    gp_Vec2d V(0., 1.);
    L = new (Geom2d_Line) (P, V);
  }
  else {
    //gp_Pnt2d P(0., ValIso);
    gp_Pnt2d P( Umin -Iso->FirstParameter() , ValIso );
    gp_Vec2d V(1., 0.);
    L = new (Geom2d_Line) (P, V);
  }

  B.UpdateEdge(E, L, S, Loc, Precision::Confusion());
  if (sing) B.Range(E, S, Loc, 
		    Iso->FirstParameter(), 
		    Iso->LastParameter());

  Standard_Real MaxTol = 1.e-4;
  Standard_Real theTol;
  GeomAdaptor_Curve GAiso(Iso);
  Handle(GeomAdaptor_HCurve) GAHiso = new GeomAdaptor_HCurve(GAiso);
  GeomAdaptor_Surface GAsurf(S);
  Handle(GeomAdaptor_HSurface) GAHsurf = new GeomAdaptor_HSurface(GAsurf);
  CheckSameParameter( GAHiso, L, GAHsurf, MaxTol, theTol);
  B.UpdateEdge(E, theTol);

  return E;
}

//=======================================================================
//Function : 
//Objet : Complete an edge via an iso
//=======================================================================
static void UpdateEdge(TopoDS_Edge& E,
		       const Handle(Geom_Surface)& S,
		       const Standard_Boolean isUiso,
		       const Standard_Real ValIso)
{
  BRep_Builder B;
  Handle(Geom2d_Line) L;
  Handle(Geom2d_Curve) PCurve, CL;
  TopLoc_Location Loc;
  Standard_Real UFirst, ULast, VFirst, VLast, F2d, L2d;
  S->Bounds( UFirst, ULast, VFirst, VLast);

  Standard_Boolean sing = Standard_False;
  Handle(Geom_Curve) Iso;
  if (isUiso) {
    Iso = S->UIso(ValIso);
  }
  else {      
    Iso = S->VIso(ValIso);
  }

  TopoDS_Vertex Vf, Vl;
  TopExp::Vertices(E, Vf, Vl);
  if (Vf.IsSame(Vl)) { // Singular case ?
    gp_Pnt Pmid; 
    Standard_Real tol = BRep_Tool::Tolerance(Vf);
    Iso->D0((Iso->FirstParameter()+Iso->LastParameter())/2, Pmid);
    if (Pmid.Distance(BRep_Tool::Pnt(Vf)) < tol) {
      GeomAdaptor_Curve AC(Iso);
      sing = GCPnts_AbscissaPoint::Length(AC, tol/4) < tol;
    }
  }

  if (isUiso) {
    gp_Pnt2d P(ValIso, 0);
    gp_Vec2d V(0., 1.);
    L = new (Geom2d_Line) (P, V);
    F2d = VFirst;
    L2d = VLast;
  }
  else {
    gp_Pnt2d P(0., ValIso);
    gp_Vec2d V(1., 0.);
    L = new (Geom2d_Line) (P, V);
    F2d = UFirst;
    L2d = ULast;
  }
  CL = new (Geom2d_TrimmedCurve) (L, F2d, L2d);

  // Control direction & Range
  Standard_Real R, First, Last, Tol=1.e-4;
  Standard_Boolean reverse = Standard_False;;
  

// Class BRep_Tool without fields and without Constructor :
//  BRep_Tool BT;
  gp_Pnt POnS;
  gp_Pnt2d P2d;
//  BT.Range(E, First, Last);
  BRep_Tool::Range(E, First, Last);

  if (!Vf.IsSame(Vl)) {
    // Test distances between "FirstPoint" and "Vertex"
    P2d = CL->Value(F2d);
    POnS = S->Value(P2d.X(), P2d.Y());
//    reverse = POnS.Distance(BT.Pnt(Vl)) < POnS.Distance(BT.Pnt(Vf));
    reverse = POnS.Distance(BRep_Tool::Pnt(Vl)) < POnS.Distance(BRep_Tool::Pnt(Vf));
  }
  else if (!sing) {
    // Test angle between "First Tangente"
    gp_Vec2d V2d;
    gp_Vec V3d, du, dv, dC3d;
    BRepAdaptor_Curve C3d(E);

    C3d.D1(First, POnS,  dC3d);
    CL->D1(F2d, P2d, V2d);
    S->D1(P2d.X(), P2d.Y(), POnS, du, dv);
    V3d.SetLinearForm(V2d.X(), du, V2d.Y(), dv);
    reverse = ( dC3d.Angle(V3d) > Tol);
  }
  if (reverse ) { // Return curve 2d
    CL = new (Geom2d_TrimmedCurve)(L, F2d, L2d);
    CL->Reverse();  
    F2d = CL->FirstParameter();
    L2d = CL->LastParameter();
  }

  if (sing)
    {
      Handle(Geom_Curve) NullCurve;
      B.UpdateEdge(E, NullCurve, 0.);
      B.Degenerated(E, Standard_True);
      B.Range(E, F2d, L2d);
      First = F2d;
      Last  = L2d;
    }

  if (First != F2d || Last != L2d) {
    Handle(Geom2d_Curve) C2d;
    GeomLib::SameRange(Precision::PConfusion(), CL, 
		       F2d, L2d, First, Last,
		       C2d);
    CL = new (Geom2d_TrimmedCurve)(C2d, First, Last);
  }

  // Update des Vertex

  TopoDS_Vertex V;

  P2d = CL->Value(First);
  POnS = S->Value(P2d.X(), P2d.Y());
  V = TopExp::FirstVertex(E); 
//  R = POnS.Distance(BT.Pnt(V));
  R = POnS.Distance(BRep_Tool::Pnt(V));
  B.UpdateVertex(V, R);

  P2d = CL->Value(Last);
  POnS = S->Value(P2d.X(), P2d.Y());
  V = TopExp::LastVertex(E);
//  R = POnS.Distance(BT.Pnt(V));
  R = POnS.Distance(BRep_Tool::Pnt(V));
  B.UpdateVertex(V, R);

  // Update Edge
  if (!sing && SameParameter(E, CL, S, Tol, R)) {
    B.UpdateEdge(E, R);
  }

  PCurve = Couture(E, S, Loc);
  if (PCurve.IsNull())
    B.UpdateEdge(E, CL, S, Loc, Precision::Confusion());
  else { // Sewing edge
    TopoDS_Edge e = E;
    Oriente(S, e);
    if (e.Orientation() == TopAbs_REVERSED)
      B.UpdateEdge(E, CL, PCurve, S, Loc, Precision::Confusion());
    else         
      B.UpdateEdge(E, PCurve, CL, S, Loc, Precision::Confusion());
  }

  // Attention to case not SameRange on its shapes (PRO13551)
//  if (!BT.SameRange(E)) B.Range(E, S, Loc, First, Last);
  if (!BRep_Tool::SameRange(E)) B.Range(E, S, Loc, First, Last);
}

//=======================================================================
// Object : Check if a surface is degenerated
//=======================================================================
static Standard_Boolean IsDegen(const Handle(Geom_Surface)& S,
				const Standard_Real Tol)
{
  Standard_Integer Nb = 5;
  Standard_Boolean B = Standard_True;
  Standard_Real Umax, Umin, Vmax, Vmin, t, dt, l;
  Standard_Integer ii;
  Handle(Geom_Curve) Iso;
  gp_Pnt P1,P2,P3;
  GCPnts_AbscissaPoint GC;

  S->Bounds(Umin, Umax, Vmin, Vmax);

  // Check the length of Iso-U
  t = (Umin + Umax)/2;
  S->D0(t, Vmin, P1);
  S->D0(t, (Vmin+Vmax)/2, P2);
  S->D0(t, Vmax, P3);
  B = ((P1.Distance(P2) + P2.Distance(P3)) < Tol);
  
  for (ii=1, dt = (Umax-Umin)/(Nb+1); B && (ii<=Nb); ii++) {
    t =  Umin + ii*dt;
    Iso = S->UIso(t);
    GeomAdaptor_Curve AC(Iso);
    l = GC.Length(AC, Tol/4);
    B = (l <= Tol);
  }
 
  if (B) return Standard_True;

  // Check the length of Iso-V
  t = (Vmin + Vmax)/2;
  S->D0(Umin, t, P1);
  S->D0((Umin+Umax)/2, t, P2);
  S->D0(Umax, t, P3);
  B = ((P1.Distance(P2) + P2.Distance(P3)) < Tol); 
 
 
  for (ii=1, dt = (Vmax-Vmin)/(Nb+1); B && (ii<=Nb); ii++) {
    t =  Vmin + ii*dt;
    Iso = S->VIso(t);
    GeomAdaptor_Curve AC(Iso);
    l = GC.Length(AC, Tol/4);
    B = (l <= Tol);
  }  
    
  return B;
}

static void ReverseEdgeInFirstOrLastWire(TopoDS_Shape&       theWire,
                                         const TopoDS_Shape& theEdge)
{
  TopoDS_Shape EdgeToReverse;
  TopoDS_Iterator itw(theWire);

  for (; itw.More(); itw.Next())
  {
    const TopoDS_Shape& anEdge = itw.Value();
    if (anEdge.IsSame(theEdge))
    {
      EdgeToReverse = anEdge;
      break;
    }
  }

  if (!EdgeToReverse.IsNull())
  {
    BRep_Builder BB;
    theWire.Free(Standard_True);
    BB.Remove(theWire, EdgeToReverse);
    EdgeToReverse.Reverse();
    BB.Add(theWire, EdgeToReverse);
  }
}

static void ReverseModifiedEdges(TopoDS_Wire&               theWire,
                                 const TopTools_MapOfShape& theEmap)
{
  if (theEmap.IsEmpty())
    return;
  
  TopoDS_Iterator itw(theWire);
  BRep_Builder BB;
  
  TopTools_ListOfShape Ledges;
  for (; itw.More(); itw.Next())
    Ledges.Append(itw.Value());
  
  theWire.Free(Standard_True);
  TopTools_ListIteratorOfListOfShape itl(Ledges);
  for (; itl.More(); itl.Next())
    BB.Remove(theWire, itl.Value());
        
  for (itl.Initialize(Ledges); itl.More(); itl.Next())
  {
    TopoDS_Shape anEdge = itl.Value();
    if (theEmap.Contains(anEdge))
      anEdge.Reverse();
    BB.Add(theWire, anEdge);
  }
}


//=======================================================================
//function : Constructeur
//purpose  : 
//======================================================================
BRepFill_Sweep::BRepFill_Sweep(const Handle(BRepFill_SectionLaw)& Section,
			       const Handle(BRepFill_LocationLaw)& Location,
			       const Standard_Boolean WithKPart) : 
			       isDone(Standard_False),
			       KPart(WithKPart)


{
 mySec = Section;
 myLoc = Location;
 
 SetTolerance(1.e-4);
 SetAngularControl();
 myAuxShape.Clear();

 myApproxStyle = GeomFill_Location;
 myContinuity  = GeomAbs_C2;
 myDegmax      = 11;
 mySegmax      = 30;
 myForceApproxC1 = Standard_False;
}

//=======================================================================
//function : SetBounds
//purpose  : Define start and end shapes
//======================================================================
 void BRepFill_Sweep::SetBounds(const TopoDS_Wire& First,
				const TopoDS_Wire& Last)
{ 
  FirstShape = First;
  LastShape  = Last; 

  // It is necessary to check the SameRange on its (PRO13551)
#ifdef OCCT_DEBUG
  Standard_Boolean issame = Standard_True;
#endif
  BRep_Builder B;
  BRepTools_WireExplorer wexp;
  if (!FirstShape.IsNull()) {
    for (wexp.Init(FirstShape); wexp.More(); wexp.Next()) {
      if (!BRepLib::CheckSameRange(wexp.Current())) {
	B.SameRange(wexp.Current(), Standard_False);
	B.SameParameter(wexp.Current(), Standard_False);
#ifdef OCCT_DEBUG
	issame = Standard_False;
#endif
      }
    }
  }
  
  if (!LastShape.IsNull()) {
    for (wexp.Init(LastShape); wexp.More(); wexp.Next()) {
      if (!BRepLib::CheckSameRange(wexp.Current())) {
	B.SameRange(wexp.Current(), Standard_False); 
	B.SameParameter(wexp.Current(), Standard_False);
#ifdef OCCT_DEBUG
	issame = Standard_False;
#endif
      }
    }
  }

#ifdef OCCT_DEBUG
  if (!issame) 
    cout<<"Sweep Warning : Edge not SameRange in the limits"<<endl;
#endif
}

//=======================================================================
//function : SetTolerance
//purpose  :
//======================================================================
 void BRepFill_Sweep::SetTolerance(const Standard_Real Tol3d,
				   const Standard_Real BoundTol,
				   const Standard_Real Tol2d, 
				   const Standard_Real TolAngular) 
{
  myTol3d = Tol3d;
  myBoundTol = BoundTol;
  myTol2d =Tol2d;
  myTolAngular = TolAngular;
}
//=======================================================================
//function : SetAngularControl
//purpose  :
//======================================================================
 void BRepFill_Sweep::SetAngularControl(const Standard_Real MinAngle,
				   const Standard_Real MaxAngle)
{
  myAngMin = Max (MinAngle, Precision::Angular());
  myAngMax = Min (MaxAngle, 6.28);
}

//=======================================================================
//function : SetForceApproxC1
//purpose  : Set the flag that indicates attempt to approximate
//           a C1-continuous surface if a swept surface proved
//           to be C0.
//=======================================================================
 void BRepFill_Sweep::SetForceApproxC1(const Standard_Boolean ForceApproxC1)
{
  myForceApproxC1 = ForceApproxC1;
}

///=======================================================================
//function : CorrectApproxParameters
//purpose  : 
//=======================================================================
 Standard_Boolean BRepFill_Sweep::CorrectApproxParameters()
{
  TopoDS_Wire thePath = myLoc->Wire();
  GeomAbs_Shape    NewCont   = myContinuity;
  Standard_Integer NewSegmax = mySegmax;

  TopoDS_Iterator iter(thePath);
  for (; iter.More(); iter.Next())
    {
      TopoDS_Edge anEdge = TopoDS::Edge(iter.Value());
      BRepAdaptor_Curve aBAcurve(anEdge);
      GeomAbs_Shape aContinuity = aBAcurve.Continuity();
      Standard_Integer aNbInterv = aBAcurve.NbIntervals(GeomAbs_CN);
      if (aContinuity < NewCont)
	NewCont = aContinuity;
      if (aNbInterv > NewSegmax)
	NewSegmax = aNbInterv;
    }

  Standard_Boolean Corrected = Standard_False;
  if (NewCont != myContinuity || NewSegmax != mySegmax)
    Corrected = Standard_True;
  myContinuity = NewCont;
  mySegmax     = NewSegmax;
  return Corrected;
}

//=======================================================================
//function : BuildWire
//purpose  : Construit a wire by sweeping
//======================================================================
 Standard_Boolean BRepFill_Sweep::
 BuildWire(const BRepFill_TransitionStyle /*Transition*/)
{
  Standard_Integer ipath, isec = 1;
  gp_Pnt P1;//, P2;

  BRep_Builder B;
// Class BRep_Tool without fields and without Constructor :
//  BRep_Tool BT;
  Standard_Integer NbPath = myLoc->NbLaw();
  Standard_Boolean vclose;
  vclose = (myLoc->IsClosed() && (myLoc->IsG1(0, myTol3d)>= 0));
  Error = 0.;
  Handle(Geom_Surface) S;
  Handle(Geom_Curve) Iso;
  Standard_Real val, bid, First, Last, Tol;

 TopoDS_Wire wire;
 TopoDS_Edge E;
 B.MakeWire(wire);

 // (1) Construction of all curves

 // (1.1) Construction of Tables
 myFaces = new (TopTools_HArray2OfShape) (1, 1, 1, NbPath);
 myUEdges = new (TopTools_HArray2OfShape) (1, 2, 1, NbPath);
 myVEdges = new (TopTools_HArray2OfShape) (1, 1, 1, NbPath+1);

 // (1.2) Calculate curves / vertex / edge
  for (ipath=1; ipath <=NbPath; ipath++) { 
    // Curve by iso value
    GeomFill_Sweep Sweep(myLoc->Law(ipath), KPart);
    Sweep.SetTolerance(myTol3d, myBoundTol, myTol2d, myTolAngular);
    Sweep.SetForceApproxC1(myForceApproxC1);
    Sweep.Build(mySec->Law(isec), myApproxStyle, myContinuity, myDegmax, mySegmax);
    if (!Sweep.IsDone())  
      return Standard_False;
    S = Sweep.Surface();
    if (Sweep.ExchangeUV()) {
      if  (Sweep.UReversed()) S->Bounds(First, Last, bid, val);
      else S->Bounds(First, Last, val, bid);
      Iso = S->VIso(val); 
    }
    else {
      if (Sweep.UReversed()) S->Bounds(bid,  val, First, Last);
      else  S->Bounds(val, bid, First, Last);
      Iso = S->UIso(val); 
    }
    // Vertex by position
    if (ipath < NbPath) 
      BuildVertex(Iso, Standard_False, First, Last, 
		  myVEdges->ChangeValue(1, ipath+1));
    else {
      if (vclose) { 
	TopoDS_Vertex& V = TopoDS::Vertex(myVEdges->ChangeValue(1, 1));
	myVEdges->SetValue(1, ipath+1, V);
	Iso->D0(Last, P1);
//	Tol = P1.Distance(BT.Pnt(V));
	Tol = P1.Distance(BRep_Tool::Pnt(V));
	B.UpdateVertex(V, Tol);
      }
      else {
	if (!LastShape.IsNull()) myVEdges->SetValue(1, NbPath, FirstShape);
	else BuildVertex(Iso, Standard_False, First, Last, 
			 myVEdges->ChangeValue(1, NbPath+1));
      }
    }
  
    if (ipath > 1) {
      Iso->D0(First, P1);
      TopoDS_Vertex& V = TopoDS::Vertex(myVEdges->ChangeValue(1, ipath));
//      Tol = P1.Distance(BT.Pnt(V));
      Tol = P1.Distance(BRep_Tool::Pnt(V));
      B.UpdateVertex(V, Tol);
    }
    if (ipath == 1) {
      if (!FirstShape.IsNull()) myVEdges->SetValue(1,1, FirstShape);
      else BuildVertex(Iso, Standard_True, First, Last, 
		       myVEdges->ChangeValue(1, 1));
    }

    // Construction of the edge
    BRepLib_MakeEdge MkE;
    MkE.Init(Iso, 
	     TopoDS::Vertex(myVEdges->Value(1, ipath)), 
	     TopoDS::Vertex(myVEdges->Value(1, ipath+1)), 
	     Iso->FirstParameter(),
	     Iso->LastParameter());
    if (!MkE.IsDone()) { // Error of construction !!     
#ifdef DRAW
      char name[100];
      sprintf(name,"firstvertex_error");
      DBRep::Set(name, myVEdges->Value(1, ipath));
      sprintf(name,"lastvertex_error");
      DBRep::Set(name, myVEdges->Value(1, ipath+1));
      sprintf(name,"curve3d_error");
      char* Temp = name ;
      DrawTrSurf::Set(Temp,Iso);
//       DrawTrSurf::Set(name,Iso);
      throw Standard_ConstructionError("BRepFill_Sweep::BuildEdge");
#endif
       return Standard_False;  
     }
    E = MkE.Edge();
#if DRAW
    if (Affich) {
      sprintf(name,"Surf_%d", ipath);
      char* Temp = name;
      DrawTrSurf::Set(Temp, S);
//      DrawTrSurf::Set(name, S);
      sprintf(name,"Edge_%d", ipath);
      DBRep::Set(name, E);
    }
#endif
    B.UpdateEdge(E, Sweep.ErrorOnSurface());
    B.Add(wire, E);
    myFaces->SetValue(1, ipath, E);
  }  
  myShape = wire;
  return Standard_True;
}

//=======================================================================
//function : BuildShell
//purpose  : Construct a Shell by sweeping
//======================================================================
 Standard_Boolean BRepFill_Sweep::
 BuildShell(const BRepFill_TransitionStyle /*Transition*/,
	    const Standard_Integer IFirst,
	    const Standard_Integer ILast,
            TopTools_MapOfShape& ReversedEdges,
            BRepFill_DataMapOfShapeHArray2OfShape& Tapes,
            BRepFill_DataMapOfShapeHArray2OfShape& Rails,
	    const Standard_Real ExtendFirst,
	    const Standard_Real ExtendLast) 
{
  Standard_Integer ipath, isec, IPath;
#ifdef DRAW
  char name[100];
#endif
  BRep_Builder B;
  Standard_Integer NbPath = ILast - IFirst;
  Standard_Integer NbLaw =  mySec->NbLaw();
  Standard_Boolean uclose, vclose,  constSection, hasdegen = Standard_False;
  constSection = mySec->IsConstant();
  uclose = mySec->IsUClosed();
  vclose = (mySec->IsVClosed() && myLoc->IsClosed()) && 
           (NbPath == myLoc->NbLaw()) && (myLoc->IsG1(0, myTol3d)>= 0);
  Error = 0.;

  // (1) Construction of all surfaces

  // (1.1) Construction of Tables

  TColStd_Array2OfBoolean ExchUV     (1, NbLaw, 1, NbPath);
  TColStd_Array2OfBoolean UReverse   (1, NbLaw, 1, NbPath);
  TColStd_Array2OfBoolean Degenerated(1, NbLaw, 1, NbPath);
  Degenerated.Init (false);
  // No VReverse for the moment...
  TColStd_Array2OfReal TabErr(1, NbLaw   , 1, NbPath);
  TColGeom_Array2OfSurface TabS(1, NbLaw , 1, NbPath);
  
  TopTools_Array2OfShape UEdge(1, NbLaw+1, 1, NbPath);
  TopTools_Array2OfShape VEdge(1, NbLaw  , 1, NbPath+1);
  TopTools_Array2OfShape Vertex(1,NbLaw+1, 1, NbPath+1);

  TopoDS_Vertex VNULL;
  VNULL.Nullify();
  Vertex.Init(VNULL);

  TopTools_Array1OfShape SecVertex(1, NbLaw+1);
  TColStd_Array1OfReal VError(1, NbLaw+1);
  TColStd_Array1OfReal Vi(1, NbPath+1);

//Initialization of management of parametric intervals 
//(Case of evolutionary sections)
  Standard_Real Length, SecDom, SecDeb;
  myLoc->CurvilinearBounds(myLoc->NbLaw(), SecDom, Length);
  mySec->Law(1)->GetDomain(SecDeb, SecDom);
  SecDom -= SecDeb;
  if (IFirst > 1) {
    Standard_Real Lf, Ll;
    myLoc->CurvilinearBounds(IFirst-1, Lf, Ll);
    Vi(1) = SecDeb + (Ll/Length)*SecDom;
  }
  else 
    Vi(1) = SecDeb;

  // Error a priori on vertices
  if (constSection) {
    for (isec=1; isec<=NbLaw+1; isec++) {
      VError(isec) = mySec->VertexTol(isec-1, 0.);
      SecVertex(isec) = mySec->Vertex(isec, 0.);
    }
  }
 

  // (1.2) Calculate surfaces
  for (ipath=1, IPath=IFirst; ipath <=NbPath; ipath++, IPath++) {

    GeomFill_Sweep Sweep(myLoc->Law(IPath), KPart);
    Sweep.SetTolerance(myTol3d, myBoundTol, myTol2d, myTolAngular);
    Sweep.SetForceApproxC1(myForceApproxC1);

    // Case of evolutionary section, definition of parametric correspondence
    if (!constSection) {
      Standard_Real lf, ll, Lf, Ll;
      myLoc->Law(IPath)->GetDomain(lf, ll);
      myLoc->CurvilinearBounds(IPath, Lf, Ll);
      Vi(ipath+1) = SecDeb + (Ll/Length)*SecDom;
      Sweep.SetDomain(lf, ll, Vi(ipath), Vi(ipath+1));
    }
    else //section is constant
      {
	Standard_Real lf, ll, Lf, Ll;
	myLoc->Law(IPath)->GetDomain(lf, ll);
	myLoc->CurvilinearBounds(IPath, Lf, Ll);
	Vi(ipath+1) = SecDeb + (Ll/Length)*SecDom;
      }
    
    for(isec=1; isec<=NbLaw; isec++) {
      Sweep.Build(mySec->Law(isec), myApproxStyle, myContinuity, myDegmax, mySegmax);
      if (!Sweep.IsDone()) 
	return Standard_False;
      TabS(isec,ipath) = Sweep.Surface();
      TabErr(isec,ipath) = Sweep.ErrorOnSurface();
      ExchUV(isec, ipath) =  Sweep.ExchangeUV();
      UReverse(isec, ipath) =  Sweep.UReversed();
      if (Sweep.ErrorOnSurface()>Error) Error = Sweep.ErrorOnSurface();

      if ((ipath==1)&&(ExtendFirst>0)) {
	Handle(Geom_BoundedSurface) BndS;
	BndS = Handle(Geom_BoundedSurface)::DownCast(TabS(isec,ipath));
	GeomLib::ExtendSurfByLength(BndS, ExtendFirst, 1, 
				    Sweep.ExchangeUV(), Standard_False);
	TabS(isec,ipath) = BndS;
      }
      if ((ipath==NbPath)&&(ExtendLast>0)){
	Handle(Geom_BoundedSurface) BndS;
	BndS = Handle(Geom_BoundedSurface)::DownCast(TabS(isec,ipath));
	GeomLib::ExtendSurfByLength(BndS, ExtendLast, 1, 
				    Sweep.ExchangeUV(), Standard_True);
	TabS(isec,ipath) = BndS;
      }

#ifdef DRAW
      if (Affich) {
	sprintf(name,"Surf_%d_%d", isec, IPath);
	char* Temp = name ;
	DrawTrSurf::Set(Temp, TabS(isec,ipath));
      }
#endif
    }
  }

  // (2) Construction of Edges
  Standard_Real UFirst, ULast, VFirst, VLast;
  Standard_Boolean exuv, singu, singv;
  Handle(Geom_Surface) S;

  //Correct <FirstShape> and <LastShape>: reverse modified edges
  ReverseModifiedEdges(FirstShape, ReversedEdges);
  ReverseModifiedEdges(LastShape,  ReversedEdges);

  // (2.0) return preexisting Edges and vertices
  TopoDS_Edge E;
  TColStd_Array1OfBoolean IsBuilt(1, NbLaw);
  IsBuilt.Init(Standard_False);
  TopTools_Array1OfShape StartEdges(1, NbLaw);
  if (! FirstShape.IsNull() && (IFirst==1)) {
    mySec->Init(FirstShape);
    for (isec=1; isec<=NbLaw; isec++) {
      E = mySec->CurrentEdge();
      TopoDS_Vertex Vfirst, Vlast;
      TopExp::Vertices(E, Vfirst, Vlast);
      VEdge(isec, 1) = E;
      if (E.Orientation() == TopAbs_REVERSED)
        Vertex(isec+1, 1) = Vfirst; //TopExp::FirstVertex(E);
      else 
        Vertex(isec+1, 1) = Vlast; //TopExp::LastVertex(E);
      UpdateVertex(IFirst-1, isec+1, 
                   TabErr(isec, 1), Vi(1),  Vertex(isec+1, 1));

      StartEdges(isec) = E;
      if (Tapes.IsBound(E))
      {
        IsBuilt(isec) = Standard_True;
        
        //Initialize VEdge, UEdge, Vertex and myFaces
        Standard_Integer j;
        for (j = 1; j <= NbPath+1; j++)
        {
          VEdge(isec, j) = Tapes(E)->Value(1, j);
          VEdge(isec, j).Reverse(); //direction of round is reversed
        }
        Standard_Integer ifirst = isec+1, ilast = isec; //direction of round is reversed
        for (j = 1; j <= NbPath; j++)
          UEdge(ifirst, j) = Tapes(E)->Value(2, j);
        for (j = 1; j <= NbPath; j++)
          UEdge(ilast, j) = Tapes(E)->Value(3, j);
        for (j = 1; j <= NbPath+1; j++)
          Vertex(ifirst, j) = Tapes(E)->Value(4, j);
        for (j = 1; j <= NbPath+1; j++)
          Vertex(ilast, j) = Tapes(E)->Value(5, j);
        for (j = 1; j <= NbPath; j++)
          myFaces->SetValue(isec, j, Tapes(E)->Value(6, j));

        if (uclose && isec == 1)
        {
          for (j = 1; j <= NbPath; j++)
            UEdge(NbLaw+1, j) = UEdge(1, j);
          for (j = 1; j <= NbPath+1; j++)
            Vertex(NbLaw+1, j) = Vertex(1, j);
        }
        if (uclose && isec == NbLaw)
        {
          for (j = 1; j <= NbPath; j++)
            UEdge(1, j) = UEdge(NbLaw+1, j);
          for (j = 1; j <= NbPath+1; j++)
            Vertex(1, j) = Vertex(NbLaw+1, j);
        }
      }
      else
      {
        Handle(TopTools_HArray2OfShape) EmptyArray = new TopTools_HArray2OfShape(1, 6, 1, NbPath+1);
        Tapes.Bind(E, EmptyArray);
        Standard_Integer j;
        if (Rails.IsBound(Vfirst))
        {
          Standard_Integer ind = (E.Orientation() == TopAbs_REVERSED)? isec+1 : isec;
          for (j = 1; j <= NbPath; j++)
            UEdge(ind, j) = Rails(Vfirst)->Value(1, j);
          for (j = 1; j <= NbPath+1; j++)
            Vertex(ind, j) = Rails(Vfirst)->Value(2, j);
        }
        if (Rails.IsBound(Vlast))
        {
          Standard_Integer ind = (E.Orientation() == TopAbs_FORWARD)? isec+1 : isec;
          for (j = 1; j <= NbPath; j++)
            UEdge(ind, j) = Rails(Vlast)->Value(1, j);
          for (j = 1; j <= NbPath+1; j++)
            Vertex(ind, j) = Rails(Vlast)->Value(2, j);
        }
      }
    }
    
    if (VEdge(1, 1).Orientation() == TopAbs_REVERSED)
      Vertex(1, 1) =  TopExp::LastVertex(TopoDS::Edge(VEdge(1, 1)));
    else
      Vertex(1, 1) = TopExp::FirstVertex(TopoDS::Edge(VEdge(1, 1)));
    UpdateVertex(IFirst-1, 1, 
                 TabErr(1, 1), Vi(1),  Vertex(1, 1));
  }
  
  Standard_Real u, v, aux;
  Standard_Boolean ureverse;
  for (isec=1; isec<=NbLaw+1; isec++) {
    // Return data
    if (isec >NbLaw) {
      S = TabS(NbLaw, 1);
      ureverse = UReverse(NbLaw, 1);
      exuv = ExchUV(NbLaw, 1);
    }
    else {
      S = TabS(isec, 1);
      ureverse = UReverse(isec, 1);
      exuv = ExchUV(isec, 1);
    }
    S->Bounds(UFirst, ULast, VFirst, VLast);
    
    // Choice of parameters
    if (ureverse) {
      if (exuv) {
        aux = VFirst; VFirst = VLast; VLast = aux;	  
      }
      else {
        aux = UFirst; UFirst = ULast; ULast = aux;
      }
    }
    if (isec!= NbLaw+1) {
      u = UFirst;
      v = VFirst;
    }
    else {
      if (exuv) {
        u = UFirst;
        v = VLast;
      }
      else {
        u = ULast;
        v = VFirst;
      }
    }
    
    // construction of vertices
    if (Vertex(isec, 1).IsNull())
      B.MakeVertex(TopoDS::Vertex(Vertex(isec, 1)), 
                   S->Value(u,v), 
                   mySec->VertexTol(isec-1,Vi(1)));
    else
    {
      TopLoc_Location Identity;
      Vertex(isec, 1).Location(Identity);
      B.UpdateVertex(TopoDS::Vertex(Vertex(isec, 1)),
                     S->Value(u,v), 
                       mySec->VertexTol(isec-1,Vi(1)));
    }
  } //end of for (isec=1; isec<=NbLaw+1; isec++)
  
  if (! LastShape.IsNull() && (ILast==myLoc->NbLaw()+1) ) {
    mySec->Init(LastShape);
    for (isec=1; isec<=NbLaw; isec++) {
      E = mySec->CurrentEdge();
      if (VEdge(isec, NbPath+1).IsNull())
        VEdge(isec, NbPath+1) = E;

      if (Vertex(isec+1, NbPath+1).IsNull())
      {
        if (VEdge(isec, NbPath+1).Orientation() == TopAbs_REVERSED)
          Vertex(isec+1, NbPath+1) = TopExp::FirstVertex(TopoDS::Edge(VEdge(isec, NbPath+1)));
        else 
          Vertex(isec+1, NbPath+1) = TopExp::LastVertex(TopoDS::Edge(VEdge(isec, NbPath+1)));
      }
      UpdateVertex(ILast-1, isec+1, TabErr(isec, NbPath), 
                   Vi(NbPath+1),  Vertex(isec+1, NbPath+1));
    }

    if (Vertex(1,  NbPath+1).IsNull())
    {
      if (VEdge(1,  NbPath+1).Orientation() == TopAbs_REVERSED)
        Vertex(1,  NbPath+1) = TopExp::LastVertex(TopoDS::Edge(VEdge(1,  NbPath+1)));
      else
        Vertex(1,  NbPath+1) = TopExp::FirstVertex(TopoDS::Edge(VEdge(1, NbPath+1)));
    }
    UpdateVertex(ILast-1, 1, 
                 TabErr(1, NbPath), Vi(NbPath+1),  Vertex(1, NbPath+1 ));
  }
  
  for (isec=1; isec<=NbLaw+1; isec++) {
    // Return data
    if (isec >NbLaw) {
        S = TabS(NbLaw, NbPath);
        ureverse = UReverse(NbLaw, NbPath);
        exuv = ExchUV(NbLaw, NbPath);
    }
    else {
      S = TabS(isec, NbPath);
      ureverse = UReverse(isec, NbPath);
      exuv = ExchUV(isec, NbPath);
    }
    S->Bounds(UFirst, ULast, VFirst, VLast);
    
    // Choice of parametres
    if (ureverse) {
      if (exuv) {
        aux = VFirst; VFirst = VLast; VLast = aux;	  
      }
      else {
        aux = UFirst; UFirst = ULast; ULast = aux;
      }
    }
    if (isec == NbLaw+1) {
      u = ULast;
      v = VLast;
    }
    else {
      if (exuv) {
        u = ULast;
        v = VFirst;
      }
      else {
        u = UFirst;
        v = VLast;
      }
    }
    
    // construction of vertex
    if (Vertex(isec, NbPath+1).IsNull())
      B.MakeVertex(TopoDS::Vertex(Vertex(isec, NbPath+1)), 
                   S->Value(u,v), 
                   mySec->VertexTol(isec-1, Vi(NbPath+1)));
      else
      {
        TopLoc_Location Identity;
        Vertex(isec, NbPath+1).Location(Identity);
        B.UpdateVertex(TopoDS::Vertex(Vertex(isec, NbPath+1)), 
                       S->Value(u,v), 
                       mySec->VertexTol(isec-1, Vi(NbPath+1)));
      }
  } //end of for (isec=1; isec<=NbLaw+1; isec++)

 
  // ---------- Creation of Vertex and edge ------------
  for (ipath=1, IPath=IFirst; ipath<=NbPath; 
       ipath++, IPath++) {
    for (isec=1; isec <=NbLaw; isec++) {
      if (IsBuilt(isec))
        continue;
      
      S = TabS(isec, ipath);
      exuv = ExchUV(isec, ipath);
      S->Bounds(UFirst, ULast, VFirst, VLast);
      if (UReverse(isec, ipath)) {
	Standard_Real aux2;
	if (exuv) {
	  aux2 = VFirst; VFirst = VLast; VLast = aux2;	  
	}
	else {
	  aux2 = UFirst; UFirst = ULast; ULast = aux2;
	}
      }

      // (2.1) Construction of new vertices
      if (isec == 1) {
	if (ipath == 1 && Vertex(1, 1).IsNull()) {
	  // All first
	  if (constSection)
	    myLoc->PerformVertex(IPath-1, 
				 TopoDS::Vertex(SecVertex(1)), 
				 VError(1),
				 TopoDS::Vertex(Vertex(1, 1)));
	  else
	    myLoc->PerformVertex(IPath-1, 
				 mySec->Vertex(1,Vi(1)), 
				 mySec->VertexTol(0,Vi(1)),
				 TopoDS::Vertex(Vertex(1, 1)));
	}
	// the first and the next column
	if (vclose &&(ipath == NbPath) ) {
	  Vertex(1, ipath+1) =  Vertex(1, 1);
	}
	else if (Vertex(1, ipath+1).IsNull()) {
	  if (constSection)
	    myLoc->PerformVertex(IPath, 
				 TopoDS::Vertex(SecVertex(1)),
				 TabErr(1,ipath)+VError(1),
				 TopoDS::Vertex(Vertex(1, ipath+1)) );
	  else
	    myLoc->PerformVertex(IPath, 
				 mySec->Vertex(1,Vi(ipath+1)), 
				 TabErr(1,ipath) +
				 mySec->VertexTol(0,Vi(ipath+1)),
				 TopoDS::Vertex(Vertex(1, ipath+1)));

	  if (MergeVertex(Vertex(1,ipath), Vertex(1,ipath+1))) {
	    UEdge(1, ipath) = NullEdge(Vertex(1,ipath));
	  }
	 }
       }

      if (ipath == 1) {
        if (uclose && (isec == NbLaw)) {
          Vertex(isec+1, 1) =  Vertex(1, 1);
        }  
        else if (Vertex(isec+1, 1).IsNull()) {
          if (constSection)
            myLoc->PerformVertex(IPath-1, 
               TopoDS::Vertex(SecVertex(isec+1)),
               TabErr(isec,1)+VError(isec+1),
               TopoDS::Vertex(Vertex(isec+1, 1)) );
          else
            myLoc->PerformVertex(IPath-1, 
               mySec->Vertex(isec+1,Vi(1)), 
               TabErr(isec,1) +
               mySec->VertexTol(isec,Vi(1)),
               TopoDS::Vertex(Vertex(isec+1, 1)) );
                                            
          if (MergeVertex(Vertex(isec,1), Vertex(isec+1,1))) {
            VEdge(isec, 1) = NullEdge(Vertex(isec, 1)); 
          }
        }
      }

      if (uclose && (isec == NbLaw)) {
	Vertex(isec+1, ipath+1) = Vertex(1, ipath+1);
      }
      else if (vclose && (ipath == NbPath)) {
	Vertex(isec+1, ipath+1) =  Vertex(isec+1, 1);
      }
      else if (Vertex(isec+1, ipath+1).IsNull()) {
	if (constSection)
	  myLoc->PerformVertex(IPath, 
			       TopoDS::Vertex(SecVertex(isec+1)),
			       TabErr(isec, ipath)+ VError(isec+1),
			       TopoDS::Vertex(Vertex(isec+1, ipath+1)) );
	else
	 myLoc->PerformVertex(IPath, 
			      mySec->Vertex(isec+1,Vi(ipath+1)),
			      TabErr(isec, ipath) + 
			      mySec->VertexTol(isec, Vi(ipath+1)),
			      TopoDS::Vertex(Vertex(isec+1, ipath+1)) ); 
      }

      // Singular cases
      singv = MergeVertex(Vertex(isec,ipath+1), Vertex(isec+1,ipath+1));
      singu = MergeVertex(Vertex(isec+1,ipath), Vertex(isec+1,ipath+1));

      

      if (singu || singv) {
	Degenerated(isec, ipath) = IsDegen(TabS(isec,ipath), 
					   Max(myTol3d, TabErr(isec,ipath)));
      }
      if (Degenerated(isec, ipath)) { 
#ifdef OCCT_DEBUG
	cout << "Sweep : Degenerated case" << endl;
#endif
	hasdegen = Standard_True;
        // Particular construction of edges
	if (UEdge(isec+1, ipath).IsNull()) {
	  if (singu) {
	    // Degenerated edge
	    UEdge(isec+1, ipath) = NullEdge(Vertex(isec+1,ipath));
	  }
	  else { // Copy the previous edge
	    UEdge(isec+1, ipath) = UEdge(isec, ipath);
	  }
	}
	if (VEdge(isec, ipath+1).IsNull()) {
	  if (singv) {
	    // Degenerated Edge
	    VEdge(isec, ipath+1) = NullEdge(Vertex(isec,ipath+1));
	  }
	  else { // Copy the previous edge
	    VEdge(isec, ipath+1) = VEdge(isec, ipath);
	  }
	}
      }
      else { // Construction of edges by isos
	if (exuv) {
	  Standard_Real UV;
	  UV = UFirst; UFirst = VFirst; VFirst = UV;
	  UV = ULast ; ULast = VLast  ; VLast = UV;
	}
  
	// (2.2) Iso-u
	if (isec == 1 && UEdge(1, ipath).IsNull()) {
	  if (!Vertex(1,ipath).IsSame(Vertex(1,ipath+1))) {
	    gp_Pnt P1 = BRep_Tool::Pnt(TopoDS::Vertex(Vertex(1,ipath)));
	    gp_Pnt P2 = BRep_Tool::Pnt(TopoDS::Vertex(Vertex(1,ipath+1)));
	    if (P1.Distance(P2) <= myTol3d)
	      Vertex(1,ipath+1) = Vertex(1,ipath);
	  }
	  UEdge(1, ipath) = BuildEdge(S, !exuv, UFirst, 
				      Vertex(1,ipath), 
				      Vertex(1,ipath+1),
				      myTol3d);
	}
 	else
        {
          if (UEdge(isec, ipath).IsNull()) //sweep failed
            return Standard_False;
          UpdateEdge(TopoDS::Edge(UEdge(isec, ipath)), 
                     S, !exuv, UFirst);
        }
     
	if (uclose && (isec==NbLaw)) {
          if (UEdge(1, ipath).IsNull()) //degenerated case
          {
            UEdge(isec+1, ipath) = BuildEdge(S, !exuv, ULast, 
                                             Vertex(isec+1, ipath), 
                                             Vertex(isec+1, ipath+1),
                                             myTol3d);
          }
          else {
            UpdateEdge(TopoDS::Edge(UEdge(1, ipath)), 
		       S, !exuv, ULast);
            UEdge(isec+1, ipath) = UEdge(1, ipath);
          }
	}
	else {
          if (UEdge(isec+1, ipath).IsNull())
            UEdge(isec+1, ipath) = BuildEdge(S, !exuv, ULast, 
                                             Vertex(isec+1, ipath), 
                                             Vertex(isec+1, ipath+1),
                                             myTol3d);
          else
            UpdateEdge(TopoDS::Edge(UEdge(isec+1, ipath)), S, !exuv, ULast);
	}

	// (2.3) Iso-v 
	if (ipath == 1)
        {
          TopoDS_Edge aNewFirstEdge = BuildEdge(S, exuv, VFirst, 
                                                Vertex(isec  , 1), 
                                                Vertex(isec+1, 1),
                                                myTol3d);
          if (VEdge(isec, ipath).IsNull())
            VEdge(isec, ipath) = aNewFirstEdge;
          else //rebuild first edge
          {
            RebuildTopOrBottomEdge(aNewFirstEdge,
                                   TopoDS::Edge(VEdge(isec, ipath)),
                                   ReversedEdges);
            if (ReversedEdges.Contains(VEdge(isec, ipath)))
            {
              ReverseEdgeInFirstOrLastWire(FirstShape, VEdge(isec, ipath));
              StartEdges(isec).Reverse();
            }
          }
        }
	
	else UpdateEdge(TopoDS::Edge(VEdge(isec, ipath)), 
			S, exuv, VFirst);
	
	if (vclose && (ipath == NbPath)) {
          if (VEdge(isec, 1).IsNull()) //degenerated case
          {
            VEdge(isec, ipath+1) = BuildEdge(S, exuv, VLast, 
					     Vertex(isec  , ipath+1), 
                                             Vertex(isec+1, ipath+1),
                                             myTol3d);
          }
          else {
            UpdateEdge(TopoDS::Edge(VEdge(isec, 1)), 
		       S, exuv, VLast);
            VEdge(isec, ipath+1) = VEdge(isec, 1);
          }
	}
	else if (VEdge(isec, ipath+1).IsNull())
	  VEdge(isec, ipath+1) = BuildEdge(S, exuv, VLast, 
					   Vertex(isec  , ipath+1), 
					   Vertex(isec+1, ipath+1),
					   myTol3d);
	else
        {
          if (ipath != NbPath || vclose)
            UpdateEdge(TopoDS::Edge(VEdge(isec, ipath+1)), 
                       S, exuv, VLast);
          else //ipath == NbPath && !vclose => rebuild last edge
          {
            TopoDS_Edge aNewLastEdge = BuildEdge(S, exuv, VLast, 
                                                 Vertex(isec  , ipath+1), 
                                                 Vertex(isec+1, ipath+1),
                                                 myTol3d);
            RebuildTopOrBottomEdge(aNewLastEdge,
                                   TopoDS::Edge(VEdge(isec, ipath+1)),
                                   ReversedEdges);
            if (ReversedEdges.Contains(VEdge(isec, ipath+1)))
              ReverseEdgeInFirstOrLastWire(LastShape, VEdge(isec, ipath+1));
          }
        }
      }
    }// End of construction of edges
  }

  // (3) Construction of Faces
  TopoDS_Face face;

#ifdef DRAW
  if (Affich) {
    for (ipath=1, IPath=IFirst; ipath<=NbPath; ipath++, IPath++) {
      for (isec=1; isec <=NbLaw+1; isec++){
	sprintf(name,"uedge_%d_%d", isec, IPath);
	DBRep::Set(name,UEdge(isec, ipath));
      }
    }

    for (ipath=1, IPath=IFirst; ipath<=NbPath+1; ipath++, IPath++) {
      for (isec=1; isec <=NbLaw; isec++){
	sprintf(name,"vedge_%d_%d", isec, IPath);
	DBRep::Set(name,VEdge(isec, ipath));
      }

      for (isec=1; isec <=NbLaw+1; isec++){
	sprintf(name,"vertex_%d_%d", isec, IPath);
	DBRep::Set(name,Vertex(isec, ipath));
      }
    }
  }
#endif 

  for (ipath=1, IPath=IFirst; ipath<=NbPath; ipath++, IPath++) {
    for (isec=1; isec <=NbLaw; isec++) {
      if (Degenerated(isec, ipath)) {
	if (UEdge(isec, ipath).IsSame(UEdge(isec+1, ipath))) 
	  myFaces->SetValue(isec, IPath, UEdge(isec, ipath));
	else  
	  myFaces->SetValue(isec, IPath, VEdge(isec, ipath));
      }
      else if (myFaces->Value(isec, IPath).IsNull()) {
	BuildFace(TabS(isec,ipath), 
		  TopoDS::Edge(UEdge(isec, ipath)),
		  TopoDS::Edge(VEdge(isec, ipath)),
		  TopoDS::Edge(UEdge(isec+1, ipath)),
		  TopoDS::Edge(VEdge(isec, ipath+1)),
		  myVEdgesModified,
		  ExchUV(isec, ipath),
		  UReverse(isec, ipath),
		  face);
	myFaces->SetValue(isec, IPath, face);
      }
    }
  }

  // (3.1) Reverse the faces that have been built ealier
  for (ipath = 1; ipath <= NbPath; ipath++)
    for (isec = 1; isec <= NbLaw; isec++)
      if (IsBuilt(isec))
        myFaces->ChangeValue(isec, ipath).Reverse();
  

  // (4) History and Continuity 

  if (hasdegen) {
  //(4.1) // Degenerated case => Sledgehammer
    TopoDS_Compound Comp;
    B.MakeCompound(Comp);
    for (isec=1; isec <=  NbLaw+1; isec++) 
      for (ipath=1, IPath=IFirst; ipath<=  NbPath+1; ipath++, IPath++) {
        if (ipath <= NbPath) myUEdges->SetValue(isec, IPath, UEdge(isec, ipath));
        if (isec <= NbLaw) myVEdges->SetValue(isec, IPath, VEdge(isec, ipath)); 
        if ((ipath <= NbPath) && (isec <= NbLaw) && 
            !myFaces->Value(isec, IPath).IsNull() &&
            myFaces->Value(isec, IPath).ShapeType() == TopAbs_FACE)
          B.Add(Comp, myFaces->Value(isec, IPath));
    }
    BRepLib::EncodeRegularity(Comp, myTolAngular);
  }
  else {
    //(4.2) // General case => Tweezers 
    Standard_Boolean isG1;
    TopoDS_Face FF;
    TopoDS_Edge anEdge;
 
    for (isec=1; isec <=  NbLaw+1; isec++) {
      if (isec>1) isG1 = 
	(mySec->Continuity(isec-1, myTolAngular) >= GeomAbs_G1);
      else isG1 = Standard_False;
      for (ipath=1, IPath=IFirst; ipath<=  NbPath; ipath++, IPath++) {
	myUEdges->SetValue(isec, IPath, UEdge(isec, ipath));
	if (isG1) {
	  if (isec == NbLaw+1) FF = TopoDS::Face(myFaces->Value(1, IPath));
	  else  FF = TopoDS::Face(myFaces->Value(isec, IPath));
	  B.Continuity(TopoDS::Edge(myUEdges->Value(isec, IPath)),
		       TopoDS::Face(myFaces->Value(isec-1, IPath)), 
		       FF, GeomAbs_G1);
	}
      }
    }

    Standard_Integer nbpath = NbPath;
    if (vclose) nbpath++; //Another test G1 
    for (ipath=1, IPath=IFirst; ipath<=  NbPath+1; ipath++, IPath++) {
      if ((ipath > 1) && (ipath <=nbpath)) 
	isG1 = (myLoc->IsG1(IPath-1, myTol3d, myTolAngular) >= 0);
      else isG1 = Standard_False;
      for (isec=1; isec <=  NbLaw; isec++) {
	myVEdges->SetValue(isec, IPath, VEdge(isec, ipath));
	if (isG1) { 
	  if (ipath==NbPath+1) FF = TopoDS::Face(myFaces->Value(isec, 1));
	  else  FF = TopoDS::Face(myFaces->Value(isec, IPath));
          anEdge = TopoDS::Edge(myVEdges->Value(isec, IPath));
	  BRepLib::EncodeRegularity(anEdge, FF,
				    TopoDS::Face(myFaces->Value(isec, IPath-1)),
				    myTolAngular);
	}
      } 
    }
  }

  // (5) Update Tapes and Rails
  Standard_Integer j;
  if (IFirst == 1 && !Tapes.IsEmpty()) //works only in case of single shell
  {
    for (isec = 1; isec <= NbLaw; isec++)
    {
      for (j = 1; j <= NbPath+1; j++)
        Tapes(StartEdges(isec))->SetValue(1, j, myVEdges->Value(isec, j));
      for (j = 1; j <= NbPath; j++)
        Tapes(StartEdges(isec))->SetValue(2, j, myUEdges->Value(isec, j));
      for (j = 1; j <= NbPath; j++)
        Tapes(StartEdges(isec))->SetValue(3, j, myUEdges->Value(isec+1, j));
      for (j = 1; j <= NbPath+1; j++)
        Tapes(StartEdges(isec))->SetValue(4, j, Vertex(isec, j));
      for (j = 1; j <= NbPath+1; j++)
        Tapes(StartEdges(isec))->SetValue(5, j, Vertex(isec+1, j));
      for (j = 1; j <= NbPath; j++)
        Tapes(StartEdges(isec))->SetValue(6, j, myFaces->Value(isec, j));
      TopoDS_Vertex Vfirst, Vlast;
      TopExp::Vertices(TopoDS::Edge(StartEdges(isec)), Vfirst, Vlast, Standard_True); //with orientation
      if (!Rails.IsBound(Vfirst))
      {
        Handle(TopTools_HArray2OfShape) anArray = new TopTools_HArray2OfShape(1, 2, 1, NbPath+1);
        for (j = 1; j <= NbPath; j++)
          anArray->SetValue(1, j, myUEdges->Value(isec, j));
        for (j = 1; j <= NbPath+1; j++)
          anArray->SetValue(2, j, Vertex(isec, j));
        Rails.Bind(Vfirst, anArray);
      }
      if (!Rails.IsBound(Vlast))
      {
        Handle(TopTools_HArray2OfShape) anArray = new TopTools_HArray2OfShape(1, 2, 1, NbPath+1);
        for (j = 1; j <= NbPath; j++)
          anArray->SetValue(1, j, myUEdges->Value(isec+1, j));
        for (j = 1; j <= NbPath+1; j++)
          anArray->SetValue(2, j, Vertex(isec+1, j));
        Rails.Bind(Vlast, anArray);
      }
    }
  }
  
  return Standard_True;
}

//=======================================================================
//function : Build
//purpose  : Construt the result of sweeping
//======================================================================
void BRepFill_Sweep::Build(TopTools_MapOfShape& ReversedEdges,
                           BRepFill_DataMapOfShapeHArray2OfShape& Tapes,
                           BRepFill_DataMapOfShapeHArray2OfShape& Rails,
                           const BRepFill_TransitionStyle Transition,
                           const GeomAbs_Shape Continuity,
                           const GeomFill_ApproxStyle Approx,
                           const Standard_Integer Degmax,
                           const Standard_Integer Segmax) 
{
  myContinuity  = Continuity;
  myApproxStyle = Approx;
  myDegmax = Degmax;
  mySegmax = Segmax;

  CorrectApproxParameters();

  // Wire
  if (mySec->IsVertex()) isDone = BuildWire(Transition);

  else { // Shell   
    Standard_Integer NbTrous = myLoc->NbHoles(myTol3d),
                     NbPath   = myLoc->NbLaw(),
                     NbLaw    = mySec->NbLaw(), ii, jj, NbPart=1;
    Standard_Integer ipath, isec;
    BRep_Builder B;
    myUEdges = new (TopTools_HArray2OfShape) (1, NbLaw+1, 1, NbPath);
    myVEdges = new (TopTools_HArray2OfShape) (1, NbLaw, 1, NbPath+1); 
    myFaces = new (TopTools_HArray2OfShape) (1, NbLaw, 1, NbPath);
    myTapes = new (TopTools_HArray1OfShape) (1, NbLaw);
    BRep_Builder BB;
    for (Standard_Integer i = 1; i <= NbLaw; i++)
    {
      TopoDS_Shell aShell;
      BB.MakeShell(aShell);
      myTapes->ChangeValue(i) = aShell;
    }
    Handle (TopTools_HArray2OfShape) Bounds =  
      new (TopTools_HArray2OfShape) (1, NbLaw, 1, 2);
 
    Handle(TColStd_HArray1OfInteger) Trous;
 
    if (NbTrous>0) { // How many sub-parts ?
      Trous = new (TColStd_HArray1OfInteger) (1, NbTrous);
      myLoc->Holes(Trous->ChangeArray1());
      NbPart += NbTrous;
      if (Trous->Value(NbTrous) == NbPath+1) NbPart--;  
    }
    if (NbPart == 1)  { // This is done at once
      Standard_Real Extend = 0.0;
      if (NbTrous==1)  Extend = EvalExtrapol(1, Transition);
      isDone = BuildShell(Transition, 
			  1, NbPath+1,
                          ReversedEdges,
                          Tapes, Rails,
			  Extend, Extend);
    }
    else { //  This is done piece by piece
      Standard_Integer IFirst = 1, ILast;
      for (ii=1, isDone=Standard_True; 
	   ii<=NbPart && isDone; ii++) {
	if (ii > NbTrous) ILast =  NbPath+1;
	else ILast = Trous->Value(ii);
	isDone = BuildShell(Transition, 
			    IFirst, ILast,
                            ReversedEdges,
                            Tapes, Rails,
			    EvalExtrapol(IFirst, Transition),
			    EvalExtrapol(ILast,  Transition));
	if (IFirst>1) {
	  Translate(myVEdges, IFirst, Bounds, 2);
	  PerformCorner(IFirst, 
			Transition, Bounds);
	}
	IFirst = ILast;
	Translate(myVEdges, IFirst, Bounds, 1);
      }
    }
    // Management of looping ends
    if ( (NbTrous>0) && (myLoc->IsClosed()) &&
	 (Trous->Value(NbTrous) == NbPath+1) ) {
      Translate(myVEdges,  NbPath+1, Bounds, 1);
      Translate(myVEdges,  1, Bounds, 2);
      PerformCorner(1, Transition, Bounds); 
    }

    // Construction of the shell
    TopoDS_Shell shell;
    B.MakeShell(shell);
    for (ipath=1; ipath<=NbPath; ipath++) 
      for (isec=1; isec <=NbLaw; isec++) {
      const TopoDS_Shape& face = myFaces->Value(isec, ipath);
	if (!face.IsNull() && 
	    (face.ShapeType() == TopAbs_FACE) ) B.Add(shell, face);
      }

    TopTools_ListIteratorOfListOfShape It(myAuxShape);
    for (; It.More(); It.Next()) {
       const TopoDS_Shape& face = It.Value();
       if (!face.IsNull() && 
	    (face.ShapeType() == TopAbs_FACE) ) B.Add(shell, face);
    }
    //Set common Uedges to faces
    BRepTools_Substitution aSubstitute;
    /*
    for (ii = 1; ii <= NbLaw; ii++)
      for (jj = 1; jj <= NbPath; jj++)
	{
	  SetCommonEdgeInFace(aSubstitute,
			      myFaces->Value(ii, jj),
			      myUEdges->Value(ii, jj));
	  SetCommonEdgeInFace(aSubstitute,
			      myFaces->Value(ii, jj),
			      myUEdges->Value(ii+1, jj));
	}
    if (mySec->IsUClosed())
      for (jj = 1; jj <= NbPath; jj++)
	SetCommonEdgeInFace(aSubstitute,
			    myFaces->Value( 1, jj ),
			    myUEdges->Value( NbLaw+1, jj));
    */
    TopTools_DataMapIteratorOfDataMapOfShapeShape mapit( myVEdgesModified );
    for (; mapit.More(); mapit.Next())
      {
	const TopoDS_Edge& OldEdge = TopoDS::Edge(mapit.Key());
	const TopoDS_Edge& NewEdge = TopoDS::Edge(mapit.Value());
	Substitute( aSubstitute, OldEdge, NewEdge );
      }
    aSubstitute.Build( shell );
    if (aSubstitute.IsCopied( shell )) {
      const TopTools_ListOfShape& listSh = aSubstitute.Copy( shell );
      shell = TopoDS::Shell( listSh.First() );
    }

    for (ii = myFaces->LowerRow(); ii <= myFaces->UpperRow(); ii++) {
      for (jj = myFaces->LowerCol(); jj <= myFaces->UpperCol(); jj++) {
	const TopoDS_Shape& aLocalShape = myFaces->Value(ii, jj);

	if(!aLocalShape.IsNull() && aSubstitute.IsCopied(aLocalShape)) {
	  const TopTools_ListOfShape& aList = aSubstitute.Copy(aLocalShape);

	  if(!aList.IsEmpty())
	    myFaces->ChangeValue(ii, jj) = aList.First();
	}
      }
    }

    for (ii = myVEdges->LowerRow(); ii <= myVEdges->UpperRow(); ii++) {
      for (jj = myVEdges->LowerCol(); jj <= myVEdges->UpperCol(); jj++) {
	const TopoDS_Shape& aLocalShape = myVEdges->Value(ii, jj);

	if(!aLocalShape.IsNull() && aSubstitute.IsCopied(aLocalShape)) {
	  const TopTools_ListOfShape& aList = aSubstitute.Copy(aLocalShape);

	  if(!aList.IsEmpty())
	    myVEdges->ChangeValue(ii, jj) = aList.First();
	}
      }
    }

    for (ii = myUEdges->LowerRow(); ii <= myUEdges->UpperRow(); ii++) {
      for (jj = myUEdges->LowerCol(); jj <= myUEdges->UpperCol(); jj++) {
	const TopoDS_Shape& aLocalShape = myUEdges->Value(ii, jj);

	if(!aLocalShape.IsNull() && aSubstitute.IsCopied(aLocalShape)) {
	  const TopTools_ListOfShape& aList = aSubstitute.Copy(aLocalShape);

	  if(!aList.IsEmpty())
	    myUEdges->ChangeValue(ii, jj) = aList.First();
	}
      }
    }

    //Ensure Same Parameter on U-edges
    for (ii = myUEdges->LowerRow(); ii <= myUEdges->UpperRow(); ii++)
    {
      if (mySec->IsUClosed() && ii == myUEdges->UpperRow())
        continue;
      for (jj = myUEdges->LowerCol(); jj <= myUEdges->UpperCol(); jj++)
      {
	TopoDS_Edge anEdge = TopoDS::Edge(myUEdges->Value(ii, jj));
        if (anEdge.IsNull())
          continue;
        TopoDS_Face Face1, Face2;
        Standard_Integer i1 = ii-1, i2 = ii;
        if (i1 == 0 && mySec->IsUClosed())
          i1 = myFaces->UpperRow();
        if (i2 > myFaces->UpperRow())
          i2 = 0;
        if (i1 != 0)
          Face1 = TopoDS::Face(myFaces->Value(i1, jj));
        if (i2 != 0)
          Face2 = TopoDS::Face(myFaces->Value(i2, jj));
        CorrectSameParameter(anEdge, Face1, Face2);
      }
    }

    for (ii = 1; ii <= NbLaw; ii++)
      for (jj = 1; jj <= NbPath; jj++)
      {
        const TopoDS_Shape& aFace = myFaces->Value(ii,jj);
        if (!aFace.IsNull() && aFace.ShapeType() == TopAbs_FACE)
          BB.Add(myTapes->ChangeValue(ii), aFace);
      }

    // Is it Closed ?
    if (myLoc->IsClosed() && mySec->IsUClosed()) {
      //Check
      Standard_Boolean closed = Standard_True;
      Standard_Integer iedge;
      TopTools_IndexedDataMapOfShapeListOfShape EFmap;
      TopExp::MapShapesAndAncestors(shell, TopAbs_EDGE, 
				    TopAbs_FACE, EFmap);
      
      for (iedge = 1; iedge <=EFmap.Extent() && closed; iedge++) {
        const TopoDS_Edge& theEdge = TopoDS::Edge(EFmap.FindKey(iedge));
	if (BRep_Tool::Degenerated(theEdge)) continue;
	closed = (  EFmap(iedge).Extent() > 1);
      }
      shell.Closed(closed);
    }
    myShape = shell;
  }
}


//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================
 Standard_Boolean BRepFill_Sweep::IsDone() const
{
  return isDone;
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================
 TopoDS_Shape BRepFill_Sweep::Shape() const
{
  return myShape;
}

//=======================================================================
//function : ErrorOnSurface
//purpose  : 
//=======================================================================
 Standard_Real BRepFill_Sweep::ErrorOnSurface() const
{
  return Error;
}

//=======================================================================
//function : SubShape
//purpose  : Faces obtained by sweeping
//=======================================================================
 Handle(TopTools_HArray2OfShape) BRepFill_Sweep::SubShape() const
{
  return myFaces;
}

//=======================================================================
//function : InterFaces
//purpose  : Edges obtained by sweeping
//=======================================================================
 Handle(TopTools_HArray2OfShape) BRepFill_Sweep::InterFaces() const
{
  return myUEdges;
}

//=======================================================================
//function : Sections
//purpose  : Edges or Face (or compound of 2) Transition between 2 sweepings
//=======================================================================
 Handle(TopTools_HArray2OfShape) BRepFill_Sweep::Sections() const
{
  return myVEdges;
}

//=======================================================================
//function : Tape
//purpose  : returns the Tape corresponding to Index-th edge of section
//=======================================================================
TopoDS_Shape BRepFill_Sweep::Tape(const Standard_Integer Index) const
{
  return myTapes->Value(Index);
}

//=======================================================================
//function : PerformCorner
//purpose  : Trim and/or loop a corner
//======================================================================
 void  BRepFill_Sweep::PerformCorner(const Standard_Integer Index,
				     const BRepFill_TransitionStyle Transition,
				     const Handle(TopTools_HArray2OfShape)& Bounds)
{

  if (Transition == BRepFill_Modified) return; // Do nothing.

  BRepFill_TransitionStyle TheTransition = Transition;
  Standard_Boolean isTangent=Standard_False;
  Standard_Real F, L;
  Standard_Integer I1, I2, ii; //, jj;
  gp_Pnt P1,P2;
  gp_Vec T1, T2, Tang, Sortant;
//  gp_Mat M;
  //Handle(TopTools_HArray1OfShape) TheShape = 
    //new TopTools_HArray1OfShape( 1, mySec->NbLaw() );
//  TopTools_ListIteratorOfListOfShape Iterator;

  if (Index > 1) { 
    I1 = Index-1;
    I2 = Index;
  }
  else {
    I1 = myLoc->NbLaw();
    I2 = 1;
  }

  // Construct an axis supported by the bissectrice
  myLoc->Law(I1)->GetDomain(F, L);
  myLoc->Law(I1)->GetCurve()->D1(L, P1, T1);
  T1.Normalize();

  myLoc->Law(I2)->GetDomain(F, L);  
  myLoc->Law(I2)->GetCurve()->D1(F, P2, T2);
  T2.Normalize();

  if (T1.Angle(T2) <  myAngMin) {
    isTangent = Standard_True;
    gp_Vec t1, t2, V;
    gp_Mat M;
    myLoc->Law(I1)->GetDomain(F, L);
    myLoc->Law(I1)->D0(L, M, V);
    t1 = M.Column(3);
    myLoc->Law(I2)->GetDomain(F, L);
    myLoc->Law(I2)->D0(L, M, V);
    t2 = M.Column(3);

    if (t1.Angle(t2) < myAngMin) {
#ifdef OCCT_DEBUG
      cout << "BRepFill_Sweep::PerformCorner : This is not a corner !" << endl;
#endif
      return;
    }
    Sortant = t2 - t1;
  }

  if ((TheTransition == BRepFill_Right) 
      && (T1.Angle(T2) >  myAngMax) ) {
    TheTransition =  BRepFill_Round;
  }

  Tang = T1 + T2; //Average direction
  gp_Dir NormalOfBisPlane = Tang;

  if (isTangent) {
    Sortant -= Tang.Dot(Tang)*Tang;
  }
  else {
    Sortant = T2-T1; //Direction input 
    Sortant *= -1; //   "   "   output
    Tang -= (Tang.Dot(T2))*T2;
  }

  P1.BaryCenter(0.5, P2, 0.5);
  gp_Dir N(Sortant);
  gp_Dir Dx(Tang);
    
  gp_Ax2 Axe (P1, N, Dx);
  gp_Ax2 AxeOfBisPlane( P1, NormalOfBisPlane );

  // Construct 2 intersecting Shells
  Handle (TopTools_HArray2OfShape) UEdges =
    new TopTools_HArray2OfShape( 1, mySec->NbLaw()+1, 1, myLoc->NbLaw() );
  UEdges->ChangeArray2() = myUEdges->Array2();

// modified by NIZHNY-MKK  Wed Oct 29 18:31:47 2003.BEGIN
  Handle (TopTools_HArray2OfShape) aFaces =
    new TopTools_HArray2OfShape(myFaces->LowerRow(), myFaces->UpperRow(), 1, 2);
  Translate(myFaces, I1, aFaces, 1);
  Translate(myFaces, I2, aFaces, 2);

  Handle (TopTools_HArray2OfShape) aUEdges =
    new TopTools_HArray2OfShape(myUEdges->LowerRow(), myUEdges->UpperRow(), 1, 2);
  Translate(myUEdges, I1, aUEdges, 1);
  Translate(myUEdges, I2, aUEdges, 2);

  gp_Vec aNormal = T2 + T1;
  TopoDS_Face aPlaneF;

  if(aNormal.Magnitude() > gp::Resolution()) {
    gp_Pln pl(P1, gp_Dir(aNormal));
    BRepLib_MakeFace aFMaker(pl);

    if(aFMaker.Error() == BRepLib_FaceDone) {
      aPlaneF = aFMaker.Face();
      BRep_Builder aBB;
      aBB.UpdateFace(aPlaneF, Precision::Confusion() * 10.);
    }
  }

  BRepFill_TrimShellCorner aTrim(aFaces, Transition, AxeOfBisPlane);
  aTrim.AddBounds(Bounds);
  aTrim.AddUEdges(aUEdges);
  aTrim.Perform();

  if (aTrim.IsDone()) {
    TopTools_ListOfShape listmodif;
    Standard_Integer iit = 0;

    for(iit = 0; iit < 2; iit++) {
      Standard_Integer II = (iit == 0) ? I1 : I2;

      for (ii = 1; ii <= mySec->NbLaw(); ii++) {
	aTrim.Modified(myFaces->Value(ii, II), listmodif);

	if(!listmodif.IsEmpty()) {
	  myFaces->SetValue(ii, II, listmodif.First());
	}
      }

      for (ii = myUEdges->LowerRow(); ii <= myUEdges->UpperRow(); ii++) {
	aTrim.Modified(myUEdges->Value(ii, II), listmodif);

	if(!listmodif.IsEmpty()) {
	  myUEdges->SetValue(ii, II, listmodif.First());
	}
      }
    }
  }
  else if ((TheTransition == BRepFill_Right) ||
	   aTrim.HasSection() ) { 
#ifdef OCCT_DEBUG
    cout << "Fail of TrimCorner" << endl;
#endif
    return; // Nothing is touched
  }

  if (mySec->IsUClosed())
    {
      myUEdges->SetValue( 1, I1, myUEdges->Value(mySec->NbLaw()+1, I1) );
      myUEdges->SetValue( 1, I2, myUEdges->Value(mySec->NbLaw()+1, I2) );
    }

  if (TheTransition == BRepFill_Round) {
  // Filling
    TopTools_ListOfShape list1, list2;
    TopoDS_Edge Bord1, Bord2, BordFirst;
    BordFirst.Nullify();
    Bord1.Nullify();
    Bord2.Nullify();
    Standard_Boolean HasFilling = Standard_False;
    TopoDS_Face FF;
    for (ii=1; ii<=mySec->NbLaw(); ii++) {
      KeepEdge(myFaces->Value(ii, I1), Bounds->Value(ii, 1), list1);
      KeepEdge(myFaces->Value(ii, I2), Bounds->Value(ii, 2), list2);
      if (list1.Extent() == list2.Extent()) {
	TopTools_ListIteratorOfListOfShape It1(list1);
	TopTools_ListIteratorOfListOfShape It2(list2);
	Standard_Boolean B;
	for (; It1.More(); It1.Next(), It2.Next()) {
	  if (HasFilling) { // Transversal choice of constraints
	    TopoDS_Vertex VF, VL, VC;
	    TopoDS_Edge E = TopoDS::Edge(It1.Value());
	    TopoDS_Edge E1, E2;
	    E1.Nullify();
	    E2.Nullify();
	    TopExp::Vertices(E, VF, VL);
	    if (!Bord1.IsNull() && 
		TopExp::CommonVertex(E, Bord1, VC)) {
	      if (VC.IsSame(VF)) E1 = Bord1;
	      else               E2 = Bord1;
	    }
	    if (!Bord2.IsNull() && 
		TopExp::CommonVertex(E, Bord2, VC)) {
	      if (VC.IsSame(VF)) E1 = Bord2;
	      else               E2 = Bord2;
	    }
	    if (!BordFirst.IsNull() && 
		TopExp::CommonVertex(E, BordFirst, VC)) {
	      if (VC.IsSame(VF)) E1 = BordFirst;
	      else               E2 = BordFirst;
	    }
	    Bord1 = E1;
	    Bord2 = E2;
	  }
	  
	  // Filling
	  B = Filling(It1.Value(), myFaces->Value(ii, I1),
		      It2.Value(), myFaces->Value(ii, I2),
		      myVEdgesModified, myTol3d, Axe, T1, Bord1, Bord2, FF);
	  
	  if (B) {
	    myAuxShape.Append(FF);
	    myVEdges->ChangeValue(ii, I2) = FF;
            BRep_Builder BB;
            BB.Add(myTapes->ChangeValue(ii), FF);
	    HasFilling = Standard_True;
	  }
	  if (ii==1) BordFirst = Bord1;
	}
      }
#ifdef OCCT_DEBUG
      else cout << "PerformCorner : Unsymmetry of free border" << endl;
#endif
    }
  }

/*  
#if DRAW
  if (Affich) {
    Standard_Integer jj;
    char name[100];
    DBRep::Set("TrimmedShell", TheShape);
    for (jj=1; jj <=myFaces->ColLength(); jj++){
      sprintf(name,"Tfaces_%d_%d", jj, I1);
      DBRep::Set(name, myFaces->Value(jj, I1));
      sprintf(name,"Tfaces_%d_%d", jj, I2);
      DBRep::Set(name, myFaces->Value(jj, I2));
    }
  }
#endif
*/
}

//=======================================================================
//function : EvalExtrapol
//purpose  : 
//======================================================================
Standard_Real BRepFill_Sweep::
  EvalExtrapol(const Standard_Integer Index,
	       const BRepFill_TransitionStyle Transition) const
{
  Standard_Real Extrap = 0.0;
  if (Transition == BRepFill_Right) {
    Standard_Integer I1, I2;
    if ((Index == 1) || (Index ==myLoc->NbLaw()+1) ) {
      if (!myLoc->IsClosed() || !mySec->IsVClosed()) return Extrap;
      I1 = myLoc->NbLaw();
      I2 = 1;
    }
    else {
      I1 = Index-1;
      I2 = Index;
    }

    gp_Vec V1, V2, T1, T2;
    gp_Mat M1, M2;
    Standard_Real Xmin, Ymin, Zmin, Xmax, Ymax, Zmax, R, f, l;

    myLoc->Law(I1)->GetDomain(f, l);
    myLoc->Law(I1)->D0(l, M1, V1);
    T1.SetXYZ(M1.Column(3));
    myLoc->Law(I2)->GetDomain(f, l);
    myLoc->Law(I2)->D0(f, M2, V2);
    T2.SetXYZ(M2.Column(3));
     
    Standard_Real alpha = T1.Angle(T2);
    if ((alpha >  myAngMax) || (alpha <  myAngMin)) {
      //Angle too great => No "straight" connection
      //Angle too small => No connection
      return Extrap; // = 0.0
    }   

    Handle(GeomFill_SectionLaw) Sec;
    Sec = mySec->ConcatenedLaw();

    //Calculating parameter U
    Standard_Real U, Length, SecFirst, SecLen, Lf, Ll;
    myLoc->CurvilinearBounds( myLoc->NbLaw(), Lf, Length );
    mySec->Law(1)->GetDomain( SecFirst, SecLen );
    SecLen -= SecFirst;
    myLoc->CurvilinearBounds( I1, Lf, Ll );
    U = SecFirst + (Ll/Length)*SecLen;

    Bnd_Box box;
    //Box(Sec, 0., box);
    Box(Sec, U, box);
    box.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
    
    R =  Max(Max(Abs(Xmin), Abs(Xmax)),Max(Abs(Ymin), Abs(Ymax)));
    //R *= 1.1;
    // modified by NIZHNY-MKK  Fri Oct 31 18:57:51 2003
    //     Standard_Real coef = 1.2;
    Standard_Real coef = 2.;
    R *= coef;
    Extrap = Max(Abs(Zmin), Abs(Zmax)) + 100*myTol3d; 
    Extrap += R*Tan(alpha/2);
  }
  return Extrap;
}

//=======================================================================
//function : MergeVertex
//purpose  : Make V2 = V1 if V2 is too close to V1
//======================================================================
Standard_Boolean BRepFill_Sweep::MergeVertex(const TopoDS_Shape& V1,
					           TopoDS_Shape& V2) const
{
// Class BRep_Tool without fields and without Constructor :
//  BRep_Tool BT;
  const TopoDS_Vertex& v1 = TopoDS::Vertex(V1);
  const TopoDS_Vertex& v2 = TopoDS::Vertex(V2);  
  Standard_Real tol;
//  tol = Max(BT.Tolerance(v1), BT.Tolerance(v2));
  tol = Max(BRep_Tool::Tolerance(v1), BRep_Tool::Tolerance(v2));
  if (tol < myTol3d) tol = myTol3d;
//  if (BT.Pnt(v1).Distance(BT.Pnt(v2)) <= tol ){
  if (BRep_Tool::Pnt(v1).Distance(BRep_Tool::Pnt(v2)) <= tol ){
    V2 = V1;
    return Standard_True;
  }				 
  return Standard_False;
}

	
//=======================================================================
//function : UpdateVertex
//purpose  : Update the Tolerance of Vertices depending on Laws.
//======================================================================
void BRepFill_Sweep::UpdateVertex(const Standard_Integer ipath,
				  const Standard_Integer isec,
				  const Standard_Real ErrApp,
				  const Standard_Real Param,
				  TopoDS_Shape& V) const
{
  TopoDS_Vertex vv, TheV;
  TheV = TopoDS::Vertex(V);
  myLoc->PerformVertex(ipath, 
		       mySec->Vertex(isec, Param), 
		       ErrApp+mySec->VertexTol(isec-1, Param),
		       vv);
// Class BRep_Tool without fields and without Constructor :
//  BRep_Tool BT;
  gp_Pnt P1, P2;
//  P1 = BT.Pnt(vv);
  P1 = BRep_Tool::Pnt(vv);
//  P2 = BT.Pnt(TheV);
  P2 = BRep_Tool::Pnt(TheV);

//  Standard_Real Tol = BT.Tolerance(vv);
  Standard_Real Tol = BRep_Tool::Tolerance(vv);
  Tol += P1.Distance(P2);
  
//  if (Tol >  BT.Tolerance(TheV)) {
  if (Tol >  BRep_Tool::Tolerance(TheV)) {
    BRep_Builder B;
    B.UpdateVertex(TheV, Tol);
  }
}

//=======================================================================
//function : RebuildTopOrBottomEdge
//purpose  : Rebuild v-iso edge of top or bottom section
//           inserting new 3d and 2d curves taken from swept surfaces
//======================================================================
void BRepFill_Sweep::RebuildTopOrBottomEdge(const TopoDS_Edge& aNewEdge,
                                            TopoDS_Edge& anEdge,
                                            TopTools_MapOfShape& ReversedEdges) const
{
  Standard_Real fpar, lpar;
  Handle(Geom_Curve) aNewCurve = BRep_Tool::Curve(aNewEdge, fpar, lpar);
  TopLoc_Location Identity;
  
  Standard_Boolean ToReverse = Standard_False;
  Standard_Boolean IsDegen = BRep_Tool::Degenerated(aNewEdge);
  if (IsDegen)
    BRep_Tool::Range(aNewEdge, fpar, lpar);
  else
  {
    TopoDS_Vertex V1, V2, NewV1, NewV2;
    TopExp::Vertices(anEdge, V1, V2);
    if (!V1.IsSame(V2))
    {
      TopExp::Vertices(aNewEdge, NewV1, NewV2);
      V1.Location(Identity);
      if (!V1.IsSame(NewV1))
      {
        if (V1.IsSame(NewV2))
          ToReverse = Standard_True;
        else
        {
          gp_Pnt Pnt1 = BRep_Tool::Pnt(V1);
          gp_Pnt NewPnt1 = BRep_Tool::Pnt(NewV1);
          Standard_Real TolSum = BRep_Tool::Tolerance(V1) + BRep_Tool::Tolerance(NewV1);
          if (!Pnt1.IsEqual(NewPnt1, TolSum))
            ToReverse = Standard_True;
        }
      }
    }
    else
    {
      Standard_Real OldFirst, OldLast;
      Handle(Geom_Curve) OldCurve = BRep_Tool::Curve(anEdge, OldFirst, OldLast);
      gp_Vec OldD1, NewD1;
      gp_Pnt MidPnt;
      OldCurve->D1(0.5*(OldFirst + OldLast), MidPnt, OldD1);
      aNewCurve->D1(0.5*(fpar + lpar), MidPnt, NewD1);
      if (OldD1 * NewD1 < 0.)
        ToReverse = Standard_True;
    }
  }
  
  anEdge.Location(Identity);
  const Handle(BRep_TEdge)& TEdge = *((Handle(BRep_TEdge)*) &anEdge.TShape());
  TEdge->Tolerance(BRep_Tool::Tolerance(aNewEdge));
  BRep_Builder BB;
  BB.Range(anEdge, fpar, lpar);
  BB.UpdateEdge(anEdge, aNewCurve, Precision::Confusion());
  const Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*) &aNewEdge.TShape());
  const BRep_ListOfCurveRepresentation& lcr = TE->Curves();
  BRep_ListIteratorOfListOfCurveRepresentation itrep(lcr);
  for (; itrep.More(); itrep.Next())
  {
    const Handle(BRep_CurveRepresentation)& CurveRep = itrep.Value();
    if (CurveRep->IsCurveOnSurface())
    {
      Handle(BRep_GCurve) GC (Handle(BRep_GCurve)::DownCast (CurveRep));
      Handle(Geom2d_Curve) aPCurve = GC->PCurve();
      Handle(Geom_Surface) aSurf = GC->Surface();
      TopLoc_Location aLoc = aNewEdge.Location() * GC->Location();
      BB.UpdateEdge(anEdge, aPCurve, aSurf, aLoc, Precision::Confusion());
    }
  }
  
  anEdge.Free(Standard_True);
  TopoDS_Vertex V1, V2;
  TopExp::Vertices(anEdge, V1, V2);

  TopoDS_Shape anEdgeFORWARD = anEdge.Oriented(TopAbs_FORWARD);
  
  BB.Remove(anEdgeFORWARD, V1);
  BB.Remove(anEdgeFORWARD, V2);
 
  V1.Location(Identity);
  V2.Location(Identity);
  if (ToReverse)
  {
    V2.Orientation(TopAbs_FORWARD);
    V1.Orientation(TopAbs_REVERSED);
  }
  BB.Add(anEdgeFORWARD, V1);
  BB.Add(anEdgeFORWARD, V2);
  
  if (ToReverse)
  {
    anEdge.Reverse();
    ReversedEdges.Add(anEdge);
  }

  BB.Degenerated(anEdge, IsDegen);
}
