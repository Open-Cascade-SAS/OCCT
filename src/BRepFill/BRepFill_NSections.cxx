// Created on: 1998-12-29
// Created by: Joelle CHAUVET
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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepFill.hxx>
#include <BRepFill_NSections.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepLProp.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BSplCLib.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Conic.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomFill_AppSurf.hxx>
#include <GeomFill_EvolvedSection.hxx>
#include <GeomFill_HArray1OfSectionLaw.hxx>
#include <GeomFill_Line.hxx>
#include <GeomFill_NSections.hxx>
#include <GeomFill_SectionGenerator.hxx>
#include <GeomFill_SectionLaw.hxx>
#include <GeomFill_UniformSection.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_Array1OfShape.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(BRepFill_NSections,BRepFill_SectionLaw)

#ifdef OCCT_DEBUG
static Standard_Boolean Affich = 0;
#endif

#ifdef DRAW
#include <DrawTrSurf.hxx>
#include <DBRep.hxx>
#endif


//=======================================================================
//function : EdgeToBSpline
//purpose  : auxiliary -- get curve from edge and convert it to bspline
//           parameterized from 0 to 1
//=======================================================================

// NOTE: this code duplicates the same function in BRepOffsetAPI_ThruSections.cxx
static Handle(Geom_BSplineCurve) EdgeToBSpline (const TopoDS_Edge& theEdge)
{
  Handle(Geom_BSplineCurve) aBSCurve;
  if (BRep_Tool::Degenerated(theEdge)) {
    // degenerated edge : construction of a point curve
    TColStd_Array1OfReal aKnots (1,2);
    aKnots(1) = 0.;
    aKnots(2) = 1.;

    TColStd_Array1OfInteger aMults (1,2);
    aMults(1) = 2;
    aMults(2) = 2;

    TColgp_Array1OfPnt aPoles(1,2);
    TopoDS_Vertex vf, vl;
    TopExp::Vertices(theEdge,vl,vf);
    aPoles(1) = BRep_Tool::Pnt(vf);
    aPoles(2) = BRep_Tool::Pnt(vl);

    aBSCurve = new Geom_BSplineCurve (aPoles, aKnots, aMults, 1);
  }
  else
  {
    // get the curve of the edge
    TopLoc_Location aLoc;
    Standard_Real aFirst, aLast;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve (theEdge, aLoc, aFirst, aLast);

    // convert its part used by edge to bspline; note that if edge curve is bspline,
    // conversion made via trimmed curve is still needed -- it will copy it, segment 
    // as appropriate, and remove periodicity if it is periodic (deadly for approximator)
    Handle(Geom_TrimmedCurve) aTrimCurve = new Geom_TrimmedCurve (aCurve, aFirst, aLast);

    // special treatment of conic curve
    if (aTrimCurve->BasisCurve()->IsKind(STANDARD_TYPE(Geom_Conic)))
    {
      const Handle(Geom_Curve)& aCurveTemp = aTrimCurve; // to avoid ambiguity
      GeomConvert_ApproxCurve anAppr (aCurveTemp, Precision::Confusion(), GeomAbs_C1, 16, 14);
      if (anAppr.HasResult())
        aBSCurve = anAppr.Curve();
    }

    // general case
    if (aBSCurve.IsNull())
      aBSCurve = GeomConvert::CurveToBSplineCurve (aTrimCurve);

    // apply transformation if needed
    if (! aLoc.IsIdentity())
      aBSCurve->Transform (aLoc.Transformation());

    // reparameterize to [0,1]
    TColStd_Array1OfReal aKnots (1, aBSCurve->NbKnots());
    aBSCurve->Knots (aKnots);
    BSplCLib::Reparametrize (0., 1., aKnots);
    aBSCurve->SetKnots (aKnots);
  }

  // reverse curve if edge is reversed
  if (theEdge.Orientation() == TopAbs_REVERSED)
    aBSCurve->Reverse();

  return aBSCurve;
}

//=======================================================================
//function : totalsurf
//purpose  : 
//=======================================================================

static Handle(Geom_BSplineSurface) totalsurf(const TopTools_Array2OfShape& shapes,
					     const Standard_Integer NbSects,
					     const Standard_Integer NbEdges,
					     const TColStd_SequenceOfReal& params,
					     const Standard_Boolean w1Point,
					     const Standard_Boolean w2Point,
					     const Standard_Boolean uClosed,
					     const Standard_Boolean vClosed,
					     const Standard_Real myPres3d)
{
  Standard_Integer i,j,jdeb=1,jfin=NbSects;
  TopoDS_Edge edge;
  TopoDS_Vertex vf,vl;

  GeomFill_SectionGenerator section;
  Handle(Geom_BSplineSurface) surface;
  Handle(Geom_BSplineCurve) BS, BS1;

  if (w1Point) {
    jdeb++;
    edge =  TopoDS::Edge(shapes.Value(1,1));
    TopExp::Vertices(edge,vl,vf);
    TColgp_Array1OfPnt Extremities(1,2);
    Extremities(1) = BRep_Tool::Pnt(vf);
    Extremities(2) = BRep_Tool::Pnt(vl);
    TColStd_Array1OfReal Bounds(1,2);
    Bounds(1) = 0.;
    Bounds(2) = 1.;
    TColStd_Array1OfInteger Mult(1,2);
    Mult(1) = 2;
    Mult(2) = 2;
    Handle(Geom_BSplineCurve) BSPoint
      = new Geom_BSplineCurve(Extremities,Bounds,Mult, 1);
    section.AddCurve(BSPoint);
  }

  if (w2Point) {
    jfin--;
  }

  for (j=jdeb; j<=jfin; j++) {

    // case of looping sections
    if (j==jfin && vClosed) {
      section.AddCurve(BS1);
    }

    else {
      // read the first edge to initialise CompBS;
      TopoDS_Edge aPrevEdge = TopoDS::Edge (shapes.Value(1,j));
      Handle(Geom_BSplineCurve) curvBS = EdgeToBSpline (aPrevEdge);
      
      // initialization
      GeomConvert_CompCurveToBSplineCurve CompBS(curvBS);

      for (i=2; i<=NbEdges; i++) {  
	// read the edge
        TopoDS_Edge aNextEdge = TopoDS::Edge (shapes.Value(i,j));
        curvBS = EdgeToBSpline (aNextEdge);

	// concatenation
	TopoDS_Vertex ComV;
	Standard_Real epsV;
	Standard_Boolean Bof = TopExp::CommonVertex(aPrevEdge, aNextEdge, ComV);
	if (Bof) epsV = BRep_Tool::Tolerance(ComV);
	else epsV = Precision::Confusion();
	Bof = CompBS.Add(curvBS, epsV, Standard_True, Standard_False, 1);
	if (!Bof) Bof = CompBS.Add(curvBS, 200*epsV,
				   Standard_True, Standard_False, 1);

        // remember previous edge
        aPrevEdge = aNextEdge;
      }

      // return the final section
      BS = CompBS.BSplineCurve();
      section.AddCurve(BS);

      // case of looping sections
      if (j==jdeb && vClosed) {
	BS1 = BS;
      }

    }
  }

  if (w2Point) {
    edge =  TopoDS::Edge(shapes.Value(NbEdges,NbSects));
    TopExp::Vertices(edge,vl,vf);
    TColgp_Array1OfPnt Extremities(1,2);
    Extremities(1) = BRep_Tool::Pnt(vf);
    Extremities(2) = BRep_Tool::Pnt(vl);
    TColStd_Array1OfReal Bounds(1,2);
    Bounds(1) = 0.;
    Bounds(2) = 1.;
    TColStd_Array1OfInteger Mult(1,2);
    Mult(1) = 2;
    Mult(2) = 2;
    Handle(Geom_BSplineCurve) BSPoint
      = new Geom_BSplineCurve(Extremities,Bounds,Mult,1);
    section.AddCurve(BSPoint);
  }

  Handle(TColStd_HArray1OfReal) HPar
    = new TColStd_HArray1OfReal(1,params.Length());
  for (i=1; i<=params.Length(); i++) {
    HPar->SetValue(i,params(i));
  }
  section.SetParam(HPar);
  section.Perform(Precision::PConfusion());
  Handle(GeomFill_Line) line = new GeomFill_Line(NbSects);
  Standard_Integer nbIt = 0, degmin = 2, degmax = 6;
  Standard_Boolean knownP = Standard_True;
  GeomFill_AppSurf anApprox(degmin, degmax, myPres3d, myPres3d, nbIt, knownP);
  Standard_Boolean SpApprox = Standard_True;
  anApprox.Perform(line, section, SpApprox);
  Standard_Boolean uperiodic = uClosed;
  Standard_Boolean vperiodic = vClosed;
  Standard_Integer nup = anApprox.SurfPoles().ColLength(),
                   nvp = anApprox.SurfPoles().RowLength();
  TColStd_Array1OfInteger Umults(1,anApprox.SurfUKnots().Length());
  Umults = anApprox.SurfUMults();
  TColStd_Array1OfInteger Vmults(1,anApprox.SurfVKnots().Length());
  Vmults = anApprox.SurfVMults();

  if (uperiodic) {
    Standard_Integer nbuk = anApprox.SurfUKnots().Length();
    Umults(1) --;
    Umults(nbuk) --;
    nup --;
  }

  if (vperiodic) {
    Standard_Integer nbvk = anApprox.SurfVKnots().Length();
    Vmults(1) --;
    Vmults(nbvk) --;
    nvp --;
  }

  TColgp_Array2OfPnt   poles  (1, nup, 1, nvp);
  TColStd_Array2OfReal weights(1, nup, 1, nvp);
    for (j = 1; j <= nvp; j++) {
      for (i = 1; i <= nup; i++) {
	poles(i, j) = anApprox.SurfPoles()(i,j);
	weights(i, j) = anApprox.SurfWeights()(i,j);
      }
    }

  // To create non-rational surface if possible
  Standard_Real TolEps = 1.e-13;
  Standard_Boolean Vrational = Standard_False, Urational = Standard_False;
  for (j = 1; j <= weights.UpperCol(); j++)
    if (!Vrational)
      for (i = 1; i <= weights.UpperRow()-1; i++)
	{
	  //Standard_Real signeddelta = weights(i,j) - weights(i+1,j);
	  Standard_Real delta = Abs( weights(i,j) - weights(i+1,j) );
//	  Standard_Real eps = Epsilon( Abs(weights(i,j)) );
	  if (delta > TolEps/* || delta > 3.*eps*/)
	    {
	      Vrational = Standard_True;
	      break;
	    }
	}
  for (i = 1; i <= weights.UpperRow(); i++)
    if (!Urational)
      for (j = 1; j <= weights.UpperCol()-1; j++)
	{
	  //Standard_Real signeddelta = weights(i,j) - weights(i,j+1);
	  Standard_Real delta = Abs( weights(i,j) - weights(i,j+1) );
//	  Standard_Real eps = Epsilon( Abs(weights(i,j)) );
	  if (delta > TolEps/* || delta > 3.*eps*/)
	    {
	      Urational = Standard_True;
	      break;
	    }
	}
  if (!Vrational && !Urational)
    {
      Standard_Real theWeight = weights(1,1);
      for (i = 1; i <= weights.UpperRow(); i++)
	for (j = 1; j <= weights.UpperCol(); j++)
	  weights(i,j) = theWeight;
    }
    
  surface = 
    new Geom_BSplineSurface(poles, weights,
			    anApprox.SurfUKnots(), anApprox.SurfVKnots(),
			    Umults, Vmults,
			    anApprox.UDegree(), anApprox.VDegree(),
			    uperiodic, vperiodic);
  return surface;
  
}


//=======================================================================
//function : Create
//purpose  : WSeq
//=======================================================================

BRepFill_NSections::BRepFill_NSections(const TopTools_SequenceOfShape& S,
				       const Standard_Boolean Build) 

{
  myShapes = S;
  VFirst = 0.;
  VLast = 1.;
  TColStd_SequenceOfReal par;
  par.Clear();
  for (Standard_Integer i=1;i<=S.Length();i++) {
    par.Append(i-1);
  }
  myParams = par;
  Init(par,Build);
  myDone = Standard_True;
}

//=======================================================================
//function : Create
//purpose  : WSeq + Param
//=======================================================================

BRepFill_NSections::BRepFill_NSections(const TopTools_SequenceOfShape& S,
                                       const GeomFill_SequenceOfTrsf& Transformations,
				       const TColStd_SequenceOfReal & P,
				       const Standard_Real VF,
				       const Standard_Real VL,
				       const Standard_Boolean Build)

{
#ifdef OCCT_DEBUG
  if ( Affich) {
#ifdef DRAW
    Standard_Integer NBSECT = 0;
    for (Standard_Integer i=1;i<=S.Length();i++) {
      NBSECT++;
      char name[256];
      sprintf(name,"WIRE_%d",NBSECT);
      DBRep::Set(name,TopoDS::Wire(S.Value(i)));
    }
#endif
  }
#endif
  Standard_Boolean ok = Standard_True;
  for (Standard_Integer iseq=1;iseq<P.Length();iseq++) {
    ok = ok && (P.Value(iseq)<P.Value(iseq+1));
  }
  if (ok) {
    myParams = P;
    myShapes = S;
    myTrsfs = Transformations;
    VFirst = VF;
    VLast = VL;
    Init(P,Build);
    myDone = Standard_True;
  }
  else
    myDone = Standard_False;
}

//=======================================================================
//function : Init
//purpose  : Create a table of GeomFill_SectionLaw
//=======================================================================
void BRepFill_NSections::Init(const TColStd_SequenceOfReal & P,
			      const Standard_Boolean Build)
{
  BRepTools_WireExplorer wexp;
// Class BRep_Tool without fields and without Constructor :
//  BRep_Tool B;
  TopoDS_Edge E;
  Standard_Integer ii, NbEdge, jj, NbSects = P.Length();
  Standard_Integer ideb = 1, ifin = NbSects;
  Standard_Boolean wClosed, w1Point = Standard_True, 
                            w2Point = Standard_True;
  Standard_Real First, Last;
  TopoDS_Wire W;

  // Check if the start and end wires are punctual
  W = TopoDS::Wire(myShapes(1));
  for (wexp.Init(W); wexp.More(); wexp.Next()) 
//    w1Point = w1Point && B.Degenerated(wexp.Current());
    w1Point = w1Point && BRep_Tool::Degenerated(wexp.Current());
  if (w1Point) ideb++;
  W = TopoDS::Wire(myShapes(NbSects));
  for (wexp.Init(W); wexp.More(); wexp.Next()) 
//    w2Point = w2Point && B.Degenerated(wexp.Current());
    w2Point = w2Point && BRep_Tool::Degenerated(wexp.Current());
  if (w2Point) ifin--;

  // Check if the start and end wires are identical
  vclosed = myShapes(1).IsSame(myShapes(NbSects));

  // Count the number of non-degenerated edges
  W = TopoDS::Wire(myShapes(ideb));
  for (NbEdge=0, wexp.Init(W); wexp.More(); wexp.Next()) 
//    if (! B.Degenerated(wexp.Current())) NbEdge++;
    if (! BRep_Tool::Degenerated(wexp.Current())) NbEdge++;

  myEdges = new (TopTools_HArray2OfShape) (1, NbEdge, 1, NbSects);
  
  // Fill tables
  uclosed = Standard_True;
  for (jj = ideb; jj <= ifin; jj++){

    W = TopoDS::Wire(myShapes(jj));
    
    for (ii=1, wexp.Init(W); ii<=NbEdge ; wexp.Next(), ii++) {
      E =  wexp.Current();
      
//      if ( ! B.Degenerated(E)) {
      if ( ! BRep_Tool::Degenerated(E)) {
	myEdges->SetValue(ii, jj, E);
        if (E.Orientation() == TopAbs_FORWARD)
          myIndices.Bind(E, ii);
        else
          myIndices.Bind(E, -ii);
      }
    }

  // Is the law closed by U ?

    wClosed = W.Closed();
    if (!wClosed) {
      // if unsure about the flag, make check
      TopoDS_Edge Edge1, Edge2;
      TopoDS_Vertex V1,V2;
      Edge1 = TopoDS::Edge (myEdges->Value(NbEdge,jj));
      Edge2 = TopoDS::Edge (myEdges->Value(1,jj));
      
      if ( Edge1.Orientation() == TopAbs_REVERSED) {
	V1 = TopExp::FirstVertex(Edge1);
      }
      else {
	V1 = TopExp::LastVertex(Edge1);
      }
      if ( Edge2.Orientation() == TopAbs_REVERSED) {
	V2 = TopExp::LastVertex(Edge2);
      }
      else {
	V2 = TopExp::FirstVertex(Edge2);
      } 
      if (V1.IsSame(V2)) {
	wClosed = Standard_True;
      }
      else {
	BRepAdaptor_Curve Curve1(Edge1);
	BRepAdaptor_Curve Curve2(Edge2);
	Standard_Real U1 = BRep_Tool::Parameter(V1,Edge1);
	Standard_Real U2 = BRep_Tool::Parameter(V2,Edge2);
	Standard_Real Eps = BRep_Tool::Tolerance(V2) + 
	  BRep_Tool::Tolerance(V1);
	
	wClosed = Curve1.Value(U1).IsEqual(Curve2.Value(U2), Eps);
      }
    } 
    if (!wClosed) uclosed = Standard_False;
  }

  // point sections at end
  if (w1Point) {
    W = TopoDS::Wire(myShapes(1));
    wexp.Init(W);
    E =  wexp.Current();
    for (ii=1; ii<=NbEdge ; ii++) {
      myEdges->SetValue(ii, 1, E);
    }
  }
  
  if (w2Point) {
    W = TopoDS::Wire(myShapes(NbSects));
    wexp.Init(W);
    E =  wexp.Current();
    for (ii=1; ii<=NbEdge ; ii++) {
      myEdges->SetValue(ii, NbSects, E);
    }
  }
  
  
  myLaws = new (GeomFill_HArray1OfSectionLaw) (1, NbEdge);

  Standard_Real tol = Precision::Confusion();
  mySurface = totalsurf(myEdges->Array2(),myShapes.Length(),NbEdge,
			myParams,w1Point,w2Point,uclosed,vclosed,tol);
   
  // Increase the degree so that the position D2 
  // on GeomFill_NSections could be correct
  // see comments in GeomFill_NSections
  if (mySurface->VDegree()<2) {
    mySurface->IncreaseDegree(mySurface->UDegree(),2);
  }
#ifdef DRAW
  if ( Affich) {
    char*  name = new char[100];
    sprintf(name,"Ref_Surf");
    DrawTrSurf::Set(name,mySurface);
  }
#endif

  // Fill tables
  if (Build) {
    for (ii=1; ii<=NbEdge ; ii++) {
      TColGeom_SequenceOfCurve NC;
      NC.Clear();
      for (jj=1;jj<=NbSects;jj++) {
	E = TopoDS::Edge (myEdges->Value(ii,jj));
	Handle(Geom_Curve) C;
//	if (B.Degenerated(E)) {
	if (BRep_Tool::Degenerated(E)) {
	  TopoDS_Vertex vf,vl;
	  TopExp::Vertices(E,vl,vf);
	  TColgp_Array1OfPnt Extremities(1,2);
	  Extremities(1) = BRep_Tool::Pnt(vf);
	  Extremities(2) = BRep_Tool::Pnt(vl);
	  TColStd_Array1OfReal Bounds(1,2);
	  Bounds(1) = 0.;
	  Bounds(2) = 1.;
	  TColStd_Array1OfInteger Mult(1,2);
	  Mult(1) = 2;
	  Mult(2) = 2;
	  Handle(Geom_BSplineCurve) BSPoint
	    = new Geom_BSplineCurve(Extremities,Bounds,Mult,1);
	  C = BSPoint;
	}
	else {
	  C  = BRep_Tool::Curve(E,First,Last);
	
	  if (E.Orientation() == TopAbs_REVERSED) {
	    Standard_Real aux;
	    Handle(Geom_Curve) CBis;
	    CBis = C->Reversed(); // To avoid the spoiling of the topology
	    aux = C->ReversedParameter(First);
	    First = C->ReversedParameter(Last);
	    Last = aux;
	    C =  CBis;
	  } 
	  if ((ii>1) || (!BRep_Tool::IsClosed(E)) ) { // Cut C
	    Handle(Geom_TrimmedCurve) TC = 
	      new (Geom_TrimmedCurve) (C,First, Last);
	    C  = TC;
	  } 
	  //  otherwise preserve the integrity of the curve
	}
	NC.Append(C);
      }
 
      Standard_Real Ufirst = ii-1;
      Standard_Real Ulast = ii;
      myLaws->ChangeValue(ii) = new (GeomFill_NSections)(NC, myTrsfs, myParams,
							 Ufirst,Ulast,
							 VFirst,VLast,
							 mySurface);
    }
 
  } 
 
}

//=======================================================================
//function : IsVertex
//purpose  :
//=======================================================================
 Standard_Boolean BRepFill_NSections::IsVertex() const
{
  return  Standard_False;
}

//=======================================================================
//function : IsConstant
//purpose  :
//=======================================================================
 Standard_Boolean BRepFill_NSections::IsConstant() const
{
  return Standard_False;
}

//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================
 TopoDS_Vertex 
 BRepFill_NSections::Vertex(const Standard_Integer Index, 
			    const Standard_Real Param) const
{
  BRep_Builder B;
  TopoDS_Vertex V;
  B.MakeVertex(V);
  gp_Pnt P;

  if (Index <= myEdges->ColLength()) {
    Handle(Geom_BSplineCurve) Curve 
      = Handle(Geom_BSplineCurve)::DownCast(myLaws->Value(Index)->
					    BSplineSurface()->VIso(Param));
    Standard_Real first = Curve ->FirstParameter();
    Curve->D0(first, P);
    B.UpdateVertex(V, P, Precision::Confusion());
  }
  else if (Index == myEdges->ColLength()+1) {
    Handle(Geom_BSplineCurve) Curve 
      = Handle(Geom_BSplineCurve)::DownCast(myLaws->Value(Index-1)->
					    BSplineSurface()->VIso(Param));
    Standard_Real last = Curve ->LastParameter();
    Curve->D0(last, P);
    B.UpdateVertex(V, P, Precision::Confusion());
  }

  return V;
}


///=======================================================================
//function : VertexTol
//purpose  : Evaluate the hole between 2 edges of the section
//=======================================================================
 Standard_Real BRepFill_NSections::VertexTol(const Standard_Integer Index,
					      const Standard_Real Param) const
{
  Standard_Real Tol = Precision::Confusion();
  Standard_Integer I1, I2;
  if ( (Index==0) || (Index==myEdges->ColLength()) ) {
    if (!uclosed) return Tol; //The least possible error
    I1 = myEdges->ColLength();
    I2 = 1;
  }
  else {
    I1 = Index;
    I2 = I1 +1;
  } 

  Handle(GeomFill_SectionLaw) Loi;
  Standard_Integer NbPoles,  NbKnots, Degree;
  Handle(TColgp_HArray1OfPnt) Poles;
  Handle(TColStd_HArray1OfReal) Knots, Weigth;
  Handle(TColStd_HArray1OfInteger) Mults;
  Handle(Geom_BSplineCurve) BS;
  gp_Pnt PFirst;

  Loi = myLaws->Value(I1);
  Loi->SectionShape( NbPoles,  NbKnots, Degree);
  Poles = new (TColgp_HArray1OfPnt) (1, NbPoles);
  Weigth = new  (TColStd_HArray1OfReal) (1, NbPoles);
  Loi->D0(Param, Poles->ChangeArray1(), Weigth->ChangeArray1());
  Knots = new  (TColStd_HArray1OfReal) (1, NbKnots);
  Loi->Knots(Knots->ChangeArray1());
  Mults = new (TColStd_HArray1OfInteger) (1, NbKnots);
  Loi->Mults(Mults->ChangeArray1());
  BS = new (Geom_BSplineCurve) (Poles->Array1(), 
				Weigth->Array1(), 
				Knots->Array1(), 
				Mults->Array1(), 
				Degree,
				Loi->IsUPeriodic());
  PFirst = BS->Value( Knots->Value(Knots->Length()) );

  Loi = myLaws->Value(I2);
  Loi->SectionShape( NbPoles,  NbKnots, Degree);
  Poles = new (TColgp_HArray1OfPnt) (1, NbPoles);
  Weigth = new  (TColStd_HArray1OfReal) (1, NbPoles);
  Loi->D0(Param, Poles->ChangeArray1(), Weigth->ChangeArray1());
  Knots = new  (TColStd_HArray1OfReal) (1, NbKnots);
  Loi->Knots(Knots->ChangeArray1());
  Mults = new (TColStd_HArray1OfInteger) (1, NbKnots);
  Loi->Mults(Mults->ChangeArray1());
  BS = new (Geom_BSplineCurve) (Poles->Array1(), 
				Weigth->Array1(), 
				Knots->Array1(), 
				Mults->Array1(), 
				Degree,
				Loi->IsUPeriodic());
  Tol += PFirst.Distance(BS->Value( Knots->Value(1)));
  return Tol;
}

//=======================================================================
//function : ConcatenedLaw
//purpose  : 
//=======================================================================

 Handle(GeomFill_SectionLaw) BRepFill_NSections::ConcatenedLaw() const
{
  Handle(GeomFill_SectionLaw) Law;
  if (myLaws->Length() == 1) 
    return myLaws->Value(1);
  else {
    Standard_Real Ufirst, Ulast, Vfirst, Vlast;
    mySurface->Bounds(Ufirst, Ulast, Vfirst, Vlast);
    TColGeom_SequenceOfCurve NCompo;
    NCompo.Clear();
    for (Standard_Integer jj=1; jj<=myShapes.Length(); jj++) {
      NCompo.Append(mySurface->VIso(myParams(jj)));
    }
    Law = new (GeomFill_NSections)(NCompo, myTrsfs, myParams, 
				   Ufirst, Ulast, 
				   Vfirst, Vlast,
				   mySurface);
  }
  return Law;
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================
 GeomAbs_Shape BRepFill_NSections::Continuity(const Standard_Integer Index,
					      const Standard_Real TolAngular) const
{

 Standard_Integer jj;
 GeomAbs_Shape cont_jj;
 GeomAbs_Shape cont = GeomAbs_C0; 

 for (jj=1; jj<=myShapes.Length(); jj++) {

   TopoDS_Edge Edge1, Edge2;
   if ( (Index==0) || (Index==myEdges->ColLength()) ) {
     if (!uclosed) return GeomAbs_C0; //The least possible error
     
     Edge1 = TopoDS::Edge (myEdges->Value(myEdges->ColLength(),jj));
     Edge2 = TopoDS::Edge (myEdges->Value(1,jj));
   }
   else {
     Edge1 = TopoDS::Edge (myEdges->Value(Index,jj));
     Edge2 = TopoDS::Edge (myEdges->Value(Index+1,jj));
   } 
   
   TopoDS_Vertex V1,V2;
   if ( Edge1.Orientation() == TopAbs_REVERSED) {
     V1 = TopExp::FirstVertex(Edge1);
   }
   else {
     V1 = TopExp::LastVertex(Edge1);
   }
   if ( Edge2.Orientation() == TopAbs_REVERSED) {
     V2 = TopExp::LastVertex(Edge2);
   }
   else {
     V2 = TopExp::FirstVertex(Edge2);
   }
   
   if (BRep_Tool::Degenerated(Edge1) || BRep_Tool::Degenerated(Edge2))
     cont_jj = GeomAbs_CN;
   else
     {
       Standard_Real U1 = BRep_Tool::Parameter(V1,Edge1);
       Standard_Real U2 = BRep_Tool::Parameter(V2,Edge2);
       BRepAdaptor_Curve Curve1(Edge1);
       BRepAdaptor_Curve Curve2(Edge2);
       Standard_Real Eps = BRep_Tool::Tolerance(V2) +
	 BRep_Tool::Tolerance(V1);
       cont_jj = BRepLProp::Continuity(Curve1,Curve2,U1,U2, Eps, TolAngular);
     }

   if (jj==1) cont = cont_jj;
   if (cont>cont_jj) cont = cont_jj;

 }
   
 return cont;
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================
 void BRepFill_NSections::D0(const Standard_Real V, TopoDS_Shape& S)
{
  TopoDS_Wire W;
  BRepLib_MakeWire MW;
  Standard_Integer ii, NbEdge = myLaws->Length();
  for (ii=1; ii<=NbEdge ; ii++) {
    Handle(Geom_BSplineCurve) Curve 
      = Handle(Geom_BSplineCurve)::DownCast(myLaws->Value(ii)->BSplineSurface()->VIso(V));
    Standard_Real first = Curve ->FirstParameter(),
    last = Curve ->LastParameter();
    TopoDS_Edge E = BRepLib_MakeEdge(Curve,first,last);
    MW.Add(E);
  }
  TopAbs_Orientation Orien = TopAbs_FORWARD;
  TopoDS_Shape aLocalShape = MW.Wire().Oriented(Orien);
  S = TopoDS::Wire(aLocalShape);
//  S = TopoDS::Wire(MW.Wire().Oriented(Orien));
  
} 
  
