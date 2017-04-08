// Created on: 1999-04-15
// Created by: Roman LYGIN
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

//    gka 23.06.99 S4208: using tool SU_TransferParameter 
//    pdn 13.07.99 synchronizing splitting values on 3d curve and pcurve
//    abv 14.07.99 dealing with edges without 3d curve
//    svv 10.01.00 porting on DEC

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepLib_MakeWire.hxx>
#include <Geom2d_BoundedCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom_BoundedCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_TransferParameters.hxx>
#include <ShapeAnalysis_TransferParametersProj.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeExtend.hxx>
#include <ShapeUpgrade.hxx>
#include <ShapeUpgrade_EdgeDivide.hxx>
#include <ShapeUpgrade_FixSmallCurves.hxx>
#include <ShapeUpgrade_SplitCurve2d.hxx>
#include <ShapeUpgrade_SplitCurve3d.hxx>
#include <ShapeUpgrade_WireDivide.hxx>
#include <Standard_Type.hxx>
#include <TColGeom2d_HArray1OfCurve.hxx>
#include <TColGeom_HArray1OfCurve.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TColStd_HSequenceOfReal.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_SequenceOfShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_WireDivide,ShapeUpgrade_Tool)

//=======================================================================
//function : ShapeUpgrade_WireDivide
//purpose  : 
//=======================================================================
ShapeUpgrade_WireDivide::ShapeUpgrade_WireDivide():
       ShapeUpgrade_Tool(), myStatus(0)
{
//  if (ShapeUpgrade::Debug()) cout <<"ShapeUpgrade_WireDivide"<<endl;
  mySplitCurve3dTool = new ShapeUpgrade_SplitCurve3d;
  mySplitCurve2dTool = new ShapeUpgrade_SplitCurve2d;
  myTransferParamTool = new ShapeAnalysis_TransferParametersProj;
  myEdgeMode = 2;
  myFixSmallCurveTool = new ShapeUpgrade_FixSmallCurves; 
  myEdgeDivide = new ShapeUpgrade_EdgeDivide;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void ShapeUpgrade_WireDivide::Init(const TopoDS_Wire& W,
				   const TopoDS_Face& F) 
{
//  if (ShapeUpgrade::Debug()) cout <<"ShapeUpgrade_WireDivide::Init with Wire, Face"<<endl;
  myWire = W;
  myFace = F;
  myStatus = ShapeExtend::EncodeStatus ( ShapeExtend_OK );
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void ShapeUpgrade_WireDivide::Init(const TopoDS_Wire& W,
				   const Handle(Geom_Surface)& S) 
{
//  if (ShapeUpgrade::Debug()) cout <<"ShapeUpgrade_WireDivide::Init with Wire, Surface "<<endl;
  myWire = W;
  BRepLib_MakeFace mkf(S, Precision::Confusion());
  myFace = mkf.Face();
  myStatus = ShapeExtend::EncodeStatus ( ShapeExtend_OK );
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void ShapeUpgrade_WireDivide::Load(const TopoDS_Wire& W)
{
  myWire = W;
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void ShapeUpgrade_WireDivide::Load(const TopoDS_Edge& E)
{
  BRepLib_MakeWire MakeWire (E);
  if (MakeWire.IsDone())
    Load (MakeWire.Wire());
}

//=======================================================================
//function : SetFace
//purpose  : 
//=======================================================================

void ShapeUpgrade_WireDivide::SetFace(const TopoDS_Face& F)
{
  myFace = F;
}

//=======================================================================
//function : SetSurface
//purpose  : 
//=======================================================================

void ShapeUpgrade_WireDivide::SetSurface(const Handle(Geom_Surface)& S)
{
  BRepLib_MakeFace mkf(S, Precision::Confusion());
  myFace = mkf.Face();
}

//=======================================================================
//function : SetSurface
//purpose  : 
//=======================================================================

void ShapeUpgrade_WireDivide::SetSurface(const Handle(Geom_Surface)& S,
					 const TopLoc_Location& L)
{
  BRep_Builder B;
  B.MakeFace(myFace,S,L,Precision::Confusion());
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

static void CorrectSplitValues(const Handle(TColStd_HSequenceOfReal) orig3d,
			       const Handle(TColStd_HSequenceOfReal) orig2d,
			       Handle(TColStd_HSequenceOfReal) new2d,
			       Handle(TColStd_HSequenceOfReal) new3d)
{
  Standard_Real preci = Precision::PConfusion();
  Standard_Integer len3d = orig3d->Length();
  Standard_Integer len2d = orig2d->Length();
  TColStd_Array1OfBoolean fixNew2d (1, len3d);
  fixNew2d.Init (Standard_False);
  TColStd_Array1OfBoolean fixNew3d (1, len2d);
  fixNew3d.Init (Standard_False);
  Standard_Real Last3d = orig3d->Value(len3d);
  Standard_Real Last2d = orig2d->Value(len2d);

  Standard_Integer i;// svv #1
  for( i = 1; i <= len3d ; i++) {
    Standard_Real par = new2d->Value(i);
    Standard_Integer index = 0;
    for(Standard_Integer j = 1; j <= len2d && !index; j++)
      if(Abs(par-orig2d->Value(j)) < preci)
	index = j;
    if(index&&!fixNew3d(index)) {
      Standard_Real newPar = orig2d->Value(index);
      new2d->SetValue(i,newPar);
      fixNew2d(i) = Standard_True;
      Standard_Real newPar3d = orig3d->Value(i);
      new3d->SetValue(index,newPar3d);
      fixNew3d(index) = Standard_True;
    }
  }
  
  for(i = 1; i <= len2d ; i++) {
    Standard_Real par = new3d->Value(i);
    Standard_Integer index = 0;
    for(Standard_Integer j = 1; j <= len3d && !index; j++)
      if(Abs(par-orig3d->Value(j)) < preci)
	index = j;
    if(index&&!fixNew2d(index)) {
      Standard_Real newPar = orig3d->Value(index);
      new3d->SetValue(i,newPar);
      fixNew3d(i) = Standard_True;
      Standard_Real newPar2d = orig2d->Value(i);
      new2d->SetValue(index,newPar2d);
      fixNew2d(index) = Standard_True;
    }
  }
  
  Standard_Real dpreci = 2* preci;
  for(i = 1; i < len3d; i++) {
    Standard_Real dist = new2d->Value(i+1) - new2d->Value(i);
    if(dist < preci) {
      if(fixNew2d(i+1)) {
	//changing
	Standard_Real tmp = new2d->Value(i+1);
	new2d->SetValue(i+1,new2d->Value(i)+dpreci);
	new2d->SetValue(i,tmp);
	fixNew2d(i) = Standard_True;
	fixNew2d(i+1) = Standard_False;
      } 
      else
	new2d->SetValue(i+1,new2d->Value(i)+dpreci);
    }
  }
  if(new2d->Value(len3d) > Last3d) {
    Standard_Integer ind; // svv #1
    for( ind = len3d; ind > 1 && !fixNew2d(ind); ind--);
    Standard_Real lastFix = new2d->Value(ind);
    for(i = len3d; i >= ind; i--) { 
      new2d->SetValue(i,lastFix);
      lastFix-=dpreci;
    }
  }
  
  for(i = 1; i < len2d; i++) {
    Standard_Real dist = new3d->Value(i+1) - new3d->Value(i);
    if(dist < preci) {
      if(fixNew3d(i+1)) {
	//changing
	Standard_Real tmp = new3d->Value(i+1);
	new3d->SetValue(i+1,new3d->Value(i)+dpreci);
	new3d->SetValue(i,tmp);
	fixNew3d(i) = Standard_True;
	fixNew3d(i+1) = Standard_False;
      }
      else 
	new3d->SetValue(i+1,new3d->Value(i)+dpreci);
    }
  }
  if(new3d->Value(len2d) > Last2d) {
    Standard_Integer ind; // svv #1
    for(ind = len2d; ind > 1 && !fixNew3d(ind); ind--);
    Standard_Real lastFix = new3d->Value(ind);
    for(i = len2d; i >= ind; i--) { 
      new3d->SetValue(i,lastFix);
      lastFix-=dpreci;
    }
  }
}      

void ShapeUpgrade_WireDivide::Perform ()
{
  
  myStatus = ShapeExtend::EncodeStatus ( ShapeExtend_OK );
  
//  if (ShapeUpgrade::Debug()) cout << "ShapeUpgrade_WireDivide::Perform" << endl;
  
  BRep_Builder B;
  ShapeAnalysis_Edge sae;
    
  TopoDS_Wire newWire;
  B.MakeWire (newWire);
  TopLoc_Location Loc;
  Handle(Geom_Surface) Surf;
  if(!myFace.IsNull())
    Surf = BRep_Tool::Surface(myFace, Loc);
  
  Standard_Boolean isSplit3d = Standard_True;
  switch(myEdgeMode) {
     case 0: if(!myFace.IsNull()) isSplit3d = Standard_False; break;
     case 1: if(myFace.IsNull()) isSplit3d = Standard_False; break;
     default : break;  
  }
  myEdgeDivide->SetFace(myFace);
  if(isSplit3d)
    myEdgeDivide->SetSplitCurve3dTool(GetSplitCurve3dTool());
  myEdgeDivide->SetSplitCurve2dTool(GetSplitCurve2dTool());
  for (TopoDS_Iterator ItW (myWire,Standard_False); ItW.More(); ItW.Next()) {
    // for each Edge:
    TopoDS_Shape sh = Context()->Apply(ItW.Value(),TopAbs_SHAPE);
    for(TopExp_Explorer exp(sh,TopAbs_EDGE); exp.More(); exp.Next()) {
      TopoDS_Edge E = TopoDS::Edge(exp.Current());
//      if (ShapeUpgrade::Debug()) cout << ".. Edge " << (void*) &(*E.TShape()) << endl;
      
      // skip degenerated edges (and also INTERNAL/EXTERNAL, to avoid failures)
      if ( E.Orientation() == TopAbs_INTERNAL || E.Orientation() == TopAbs_EXTERNAL ) {
	B.Add ( newWire, E );
	continue;
      }
      
      if(!myEdgeDivide->Compute(E)) {
	B.Add ( newWire, E );
	continue;
      }   
      // first iteration: getting split knots
      // on 3D curve: preliminary
      
      Handle(ShapeAnalysis_TransferParameters) theTransferParamTool =  GetTransferParamTool();
      theTransferParamTool->SetMaxTolerance(MaxTolerance());
      theTransferParamTool->Init(E,myFace);
      Standard_Boolean wasSR = theTransferParamTool->IsSameRange();
      
      // on pcurve(s): all knots
      // assume that if seam-edge, its pcurve1 and pcurve2 has the same split knots !!!
      Handle(TColStd_HSequenceOfReal) theKnots3d = myEdgeDivide->Knots3d();
      Handle(TColStd_HSequenceOfReal) theKnots2d = myEdgeDivide->Knots2d();
      
      // second iteration: transfer parameters and build segments
      Handle(TColStd_HSequenceOfReal) SplitValues2d;
      Handle(TColStd_HSequenceOfReal) SplitValues3d;
      if(myEdgeDivide->HasCurve2d() && myEdgeDivide->HasCurve3d() ) {
	SplitValues2d = theTransferParamTool->Perform(theKnots3d,Standard_True);
	SplitValues3d = theTransferParamTool->Perform(theKnots2d,Standard_False);
	CorrectSplitValues(theKnots3d,theKnots2d,SplitValues2d,SplitValues3d);
      }
      Handle(ShapeUpgrade_SplitCurve3d) theSplit3dTool = myEdgeDivide->GetSplitCurve3dTool();
      Handle(ShapeUpgrade_SplitCurve2d) theSplit2dTool = myEdgeDivide->GetSplitCurve2dTool();
      
      if ( myEdgeDivide->HasCurve2d() ) {
	if(! theKnots3d.IsNull() ) {
	  SplitValues2d->Remove(1);
	  SplitValues2d->Remove(SplitValues2d->Length());
	  theSplit2dTool->SetSplitValues (SplitValues2d);
	}
	theSplit2dTool->Build(Standard_True);
      }
      if ( myEdgeDivide->HasCurve3d() ) {
	if( ! theKnots2d.IsNull() ) {
	  SplitValues3d->Remove(1);
	  SplitValues3d->Remove(SplitValues3d->Length());
	  theSplit3dTool->SetSplitValues (SplitValues3d);
	}
	theSplit3dTool->Build (Standard_True);
      }
      // get 2d and 3d split values which should be the same
      if ( myEdgeDivide->HasCurve2d() ) theKnots2d = theSplit2dTool->SplitValues(); 
      if ( myEdgeDivide->HasCurve3d() ) theKnots3d = theSplit3dTool->SplitValues();
      
      Standard_Boolean isSeam = Standard_False;
      if (! myFace.IsNull() )
	isSeam = BRep_Tool::IsClosed ( E, myFace );
      Handle(TColGeom2d_HArray1OfCurve) theSegments2d;
      if(myEdgeDivide->HasCurve2d())
	theSegments2d = theSplit2dTool->GetCurves();
      Handle(TColGeom2d_HArray1OfCurve) theSegments2dR;
      if ( isSeam ) {
	Handle(Geom2d_Curve) c2;
	Standard_Real f2, l2;    
//smh#8
	TopoDS_Shape tmpE = E.Reversed();
	TopoDS_Edge erev = TopoDS::Edge (tmpE );
	if ( sae.PCurve ( erev, myFace, c2, f2, l2, Standard_False) ) {
	  theSplit2dTool->Init (c2, f2, l2);
	  if(!theKnots2d.IsNull())
	    theSplit2dTool->SetSplitValues (theKnots2d);
	  theSplit2dTool->Perform (Standard_True);
	  Handle(TColStd_HSequenceOfReal) revKnots2d = theSplit2dTool->SplitValues();
	  if(revKnots2d->Length()!=theKnots2d->Length()) {
	    isSeam = Standard_False;
#ifdef OCCT_DEBUG
	    cout << "Error: ShapeUpgrade_WireDivide: seam has different splitting values on pcurvesd" << endl;
#endif
	  }
	  else
	    theSegments2dR = theSplit2dTool->GetCurves();
	}
	else isSeam = Standard_False;
      }
      
      // Exploring theEdge
      TopoDS_Vertex V1o = TopExp::FirstVertex (E, Standard_False);
      TopoDS_Vertex V2o = TopExp::LastVertex  (E, Standard_False);
      Standard_Boolean isForward = ( E.Orientation() == TopAbs_FORWARD );
      Standard_Real TolEdge = BRep_Tool::Tolerance (E);
      Standard_Boolean isDeg = BRep_Tool::Degenerated ( E );

      // Copy vertices to protect original shape against SameParamseter
//smh#8
      TopoDS_Shape emptyCopiedV1 = V1o.EmptyCopied();
      TopoDS_Vertex V1 = TopoDS::Vertex ( emptyCopiedV1 );
      Context()->Replace ( V1o, V1 );
      TopoDS_Vertex V2;
      if ( V1o.IsSame ( V2o ) ) {
//smh#8
	TopoDS_Shape tmpV = V1.Oriented(V2o.Orientation() );
	V2 = TopoDS::Vertex ( tmpV ); 
      }
      else {
//smh#8
	TopoDS_Shape emptyCopied = V2o.EmptyCopied();
	V2 = TopoDS::Vertex ( emptyCopied );
	Context()->Replace ( V2o, V2 );
      }

      //collect NM vertices
     
      Standard_Real af = 0.,al = 0.;
      Handle(Geom_Curve) c3d;
      Adaptor3d_CurveOnSurface AdCS;
      if(myEdgeDivide->HasCurve3d()) 
	sae.Curve3d(E,c3d,af,al,Standard_False);
      else if(myEdgeDivide->HasCurve2d() && !Surf.IsNull()) {
	Handle(Geom2d_Curve) c2d;
	sae.PCurve ( E, myFace, c2d, af, al, Standard_False); 
	Handle(Adaptor3d_HSurface) AdS = new GeomAdaptor_HSurface(Surf);
	Handle(Adaptor2d_HCurve2d) AC2d  = new Geom2dAdaptor_HCurve(c2d,af,al);
	AdCS.Load(AC2d);
	AdCS.Load(AdS);
      }
      TopTools_SequenceOfShape aSeqNMVertices;
      TColStd_SequenceOfReal aSeqParNM;
      TopoDS_Iterator aItv(E,Standard_False);
      ShapeAnalysis_Curve sac;
      for ( ; aItv.More() ; aItv.Next()) {
	if(aItv.Value().Orientation() == TopAbs_INTERNAL ||
	   aItv.Value().Orientation() == TopAbs_EXTERNAL) {
	  TopoDS_Vertex aVold = TopoDS::Vertex(aItv.Value());
	  aSeqNMVertices.Append(aVold);
	  gp_Pnt aP = BRep_Tool::Pnt(TopoDS::Vertex(aVold));
	  Standard_Real ppar;
	  gp_Pnt pproj;
	  if(!c3d.IsNull())
	    sac.Project(c3d,aP,Precision(),pproj,ppar,af,al,Standard_False);
	  else  
	    sac.Project(AdCS,aP,Precision(),pproj,ppar);
	  aSeqParNM.Append(ppar);
	}
      }
      
      // creating new edge(s)
      Handle(TColGeom_HArray1OfCurve) theSegments3d;
      if(myEdgeDivide->HasCurve3d()) theSegments3d = theSplit3dTool->GetCurves();
      
      Standard_Integer nbc = 0;
      if (!theSegments3d.IsNull()) {
	nbc = theSegments3d->Length();
	if ( !theSegments2d.IsNull() ) {
	  Standard_Integer nbc2d = theSegments2d->Length();
	  if (nbc!=nbc2d) {
#ifdef OCCT_DEBUG
	    cout<<"Error: Number of intervals are not equal for 2d 3d. Ignored."<<endl;
#endif
	    nbc = Min( nbc,nbc2d);
	  }
	}
      }
      else
	if(!theSegments2d.IsNull())// if theSegments have different length ???
	  nbc = theSegments2d->Length(); 
      
      if ( nbc <= 1 && ! theSplit3dTool->Status ( ShapeExtend_DONE ) && 
	               ! theSplit2dTool->Status ( ShapeExtend_DONE ) ) {
	B.Add ( newWire, E );
	continue;
      }
      
     myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE1 );
      
      TopoDS_Wire resWire;
      B.MakeWire (resWire);
//      TopoDS_Vertex firstVertex, lastVertex;
      Standard_Integer numE =0;
      gp_Pnt pntV1 = BRep_Tool::Pnt(V1);
      //gp_Pnt pntV2 = BRep_Tool::Pnt(V2); // pntV2 not used - see below (skl)
      //Standard_Real V2Tol = LimitTolerance( BRep_Tool::Tolerance(V2) ); // V2Tol not used - see below (skl)
      
      Handle(ShapeUpgrade_FixSmallCurves) FixSmallCurveTool = GetFixSmallCurveTool(); //gka Precision
      FixSmallCurveTool->SetMinTolerance(MinTolerance());
      FixSmallCurveTool->Init(E, myFace);
      FixSmallCurveTool->SetSplitCurve3dTool(theSplit3dTool);
      FixSmallCurveTool->SetSplitCurve2dTool(theSplit2dTool);
      FixSmallCurveTool->SetPrecision(MinTolerance());
      Standard_Integer Savnum =0;
      Standard_Real SavParf;
      Standard_Integer Small = 0;
      for ( Standard_Integer icurv = 1; icurv <= nbc; icurv++ ) {
      
	Handle(Geom_Curve) theNewCurve3d;
	if(!theSegments3d.IsNull()) theNewCurve3d = theSegments3d->Value(icurv);
	
	Handle(Geom2d_Curve) theNewPCurve1;
	if(!theSegments2d.IsNull()) theNewPCurve1 = theSegments2d->Value(icurv);
	Handle(Geom2d_Curve) revPCurve;
	if(isSeam) 
	  revPCurve = theSegments2dR->Value(icurv);
	// construction of the intermediate Vertex
	TopoDS_Vertex V;
	if ( icurv <= nbc && nbc != 1 && ! isDeg ) {
	  Standard_Real par,parf /*,SavParl*/;
          //Standard_Real SaveParf; // SaveParf not used - see below (skl)
	  gp_Pnt P,P1,PM;
	  // if edge has 3d curve, take point from it
	  if ( ! theNewCurve3d.IsNull() ) { 
	    if(theNewCurve3d->IsKind(STANDARD_TYPE(Geom_BoundedCurve))) {
	      par = theNewCurve3d->LastParameter();
	      parf = theNewCurve3d->FirstParameter();
	    }
	    else {
	      par = theKnots3d->Value (icurv + 1);
	      parf = theKnots3d->Value (icurv);
	    }
	    P = theNewCurve3d->Value (par);
	    P1 = theNewCurve3d->Value (parf);
	    PM = theNewCurve3d->Value ((parf+par)/2);
	  }
	  // else use pcurve and surface (suppose that both exist)
	  else {
	    if ( Surf.IsNull() ) Surf = BRep_Tool::Surface ( myFace, Loc );
	    if ( theNewPCurve1->IsKind(STANDARD_TYPE(Geom2d_BoundedCurve)) ) {
	      par = theNewPCurve1->LastParameter();
	      parf = theNewPCurve1->FirstParameter();
	    }
	    else {
	      par = theKnots2d->Value (icurv + 1);
	      parf = theKnots2d->Value (icurv);
	    }
	    gp_Pnt2d p2d = theNewPCurve1->Value (par);
	    gp_Pnt2d p2df = theNewPCurve1->Value (parf);
	    gp_Pnt2d p2dM = theNewPCurve1->Value ((parf+par)/2);
	    P = Surf->Value ( p2d.X(), p2d.Y() );
	    P1 = Surf->Value ( p2df.X(), p2df.Y() );
	    PM = Surf->Value ( p2dM.X(), p2dM.Y() );
	    P.Transform ( Loc.Transformation() );
	    P1.Transform ( Loc.Transformation() );
	    PM.Transform ( Loc.Transformation() );
	  }
	  if(P.Distance(pntV1) < MinTolerance() && P.Distance(PM) < MinTolerance() && !myFace.IsNull()) {
	    if(!Small) {
	      SavParf = parf;
	      Savnum = icurv;
	    }
	    //SavParl = par;
	    Small++;
	    if(icurv == nbc) {
	      TopoDS_Vertex VVV = V1;
	      VVV.Orientation ( V2.Orientation() );
	      Context()->Replace(V2,VVV);
	    }
	    continue;
	  }
	  if(Small) {
	    if(P.Distance(P1) > MinTolerance() || P.Distance(PM) > MinTolerance()) {
	      //FixSmallCurveTool->Perform(prevEdge,theNewCurve3d,theNewPCurve1,revPCurve,SavParf,SavParl);
	      gp_Pnt pmid = 0.5 * ( pntV1.XYZ() + P1.XYZ() );
	      B.UpdateVertex(V1,pmid,0);
	    }
	    else {
	      Handle(Geom_Curve) atmpCurve;
	      Handle(Geom2d_Curve) atmpCurve2d1,atmprepcurve;
	      if(FixSmallCurveTool->Approx( atmpCurve,atmpCurve2d1,atmprepcurve,SavParf,par)) { //BRepTools
		theNewCurve3d = atmpCurve;
		theNewPCurve1 = atmpCurve2d1;
		revPCurve = atmprepcurve;
	      }
	      else {
		gp_Pnt pmid = 0.5 * ( pntV1.XYZ() + P1.XYZ() );
		B.UpdateVertex(V1,pmid,0);
	      }
	    }
	    Small =0;
	  }
	  //pdn
	 /* if(P.Distance (pntV1) < V1Tol) 
	    V = V1;
	  else if (P.Distance (pntV2) < V2Tol) {
	    V = V2;
	    V1Tol = V2Tol;
	    pntV1 = pntV2;
	  }
	  else {*/
	  if(icurv != nbc)  {
	    B.MakeVertex (V, P, TolEdge); //tolerance of the edge
	    pntV1 = P;
	  }
	  else V = V2;
	 // else  V2;
	 // }
//	  if (ShapeUpgrade::Debug()) cout <<"... New intermediate Vertex (" 
//	    <<P.X()<<","<<P.Y()<<","<<P.Z()<<") :"<<(void*) &(*V.TShape())
//	      <<" with Tolerance "<<TolEdge <<endl;
	}
	//else V = V2;
	
	TopoDS_Edge newEdge;
	ShapeBuild_Edge sbe;
	if ( isForward ) {
	  V1.Orientation ( TopAbs_FORWARD );
	  V.Orientation ( TopAbs_REVERSED );
	  newEdge = sbe.CopyReplaceVertices ( E, V1, V );
	}
	else {
	  V1.Orientation ( TopAbs_REVERSED );
	  V.Orientation ( TopAbs_FORWARD );
	  newEdge = sbe.CopyReplaceVertices ( E, V, V1 );
	}
	sbe.CopyPCurves ( newEdge, E );
	if(!theNewCurve3d.IsNull())
	  B.UpdateEdge ( newEdge, theNewCurve3d, 0. );
	else if ( isDeg )
	  B.Degenerated( newEdge, Standard_True);
	//if(isSeam) {
	 // Handle(Geom2d_Curve) revPCurve = theSegments2dR->Value(icurv);
	  //if(newEdge.Orientation()==TopAbs_FORWARD)
	    //B.UpdateEdge ( newEdge, theNewPCurve1, revPCurve, myFace, 0. );
	  //else
	    //B.UpdateEdge ( newEdge, revPCurve, theNewPCurve1, myFace, 0. );
	//}
	//else if ( ! myFace.IsNull() ) 
	  //B.UpdateEdge ( newEdge, theNewPCurve1, myFace, 0. );
	
        Standard_Real f3d = 0., l3d =0.;
        if(!Savnum) Savnum = icurv;
	Standard_Boolean srNew;
	if(!theNewCurve3d.IsNull()) {
	  if(theNewCurve3d->IsKind(STANDARD_TYPE(Geom_BoundedCurve))) {
	    f3d = theNewCurve3d->FirstParameter();
	    l3d = theNewCurve3d->LastParameter();
	    srNew = ((f3d == theKnots3d->Value (Savnum)) && (l3d == theKnots3d->Value (icurv + 1)));
	  }
	  else  {
	    f3d = theKnots3d->Value (Savnum);
	    l3d = theKnots3d->Value (icurv + 1);
	    srNew = Standard_True;
	  }
	}
	else
	  srNew = Standard_True;
      
	Standard_Real f2d=0, l2d=0;
	if(!theNewPCurve1.IsNull()){
	  if(theNewPCurve1->IsKind(STANDARD_TYPE(Geom2d_BoundedCurve))) {
	    f2d = theNewPCurve1->FirstParameter();
	    l2d = theNewPCurve1->LastParameter();
	    srNew &= ((f2d == theKnots2d->Value (Savnum)) && (l2d == theKnots2d->Value (icurv + 1)));
	  }
	  else {
	    f2d = theKnots2d->Value (Savnum);
	    l2d = theKnots2d->Value (icurv + 1);
	  }
	}
	//if(!Savnum) Savnum = icurv; 
	if(!theNewCurve3d.IsNull())
	  theTransferParamTool->TransferRange(newEdge,theKnots3d->Value (Savnum),theKnots3d->Value (icurv + 1),Standard_False);
	else
	  theTransferParamTool->TransferRange(newEdge,theKnots2d->Value (Savnum),theKnots2d->Value (icurv + 1),Standard_True);
	/*
	Standard_Real alpha = (theKnots3d->Value (icurv) - f)/(l - f);
	Standard_Real beta  = (theKnots3d->Value (icurv + 1) - f)/(l - f);
	sbe.CopyRanges(newEdge,E, alpha, beta);*/
	Savnum =0;
	Handle(Geom2d_Curve) c2dTmp;
	Standard_Real setF, setL;
	if( ! myFace.IsNull() && sae.PCurve (newEdge, myFace, c2dTmp, setF, setL, Standard_False))
	  srNew &= ( (setF==f2d) && (setL==l2d) );

	if(isSeam) {
	  // Handle(Geom2d_Curve  revPCurve = theSegments2dR->Value(icurv);
	  if(newEdge.Orientation()==TopAbs_FORWARD)
	    B.UpdateEdge ( newEdge, theNewPCurve1, revPCurve, myFace, 0. );
  	  else
	    B.UpdateEdge ( newEdge, revPCurve, theNewPCurve1, myFace, 0. );
	}
	else if ( ! myFace.IsNull() ) {
	  B.UpdateEdge ( newEdge, theNewPCurve1, myFace, 0. );
	}

        if(!theNewCurve3d.IsNull())
	  sbe.SetRange3d(newEdge,f3d,l3d);
	if(!theNewPCurve1.IsNull())
	{
	  B.Range ( newEdge, myFace, f2d, l2d);
	}
	if((!wasSR || !srNew)&&!BRep_Tool::Degenerated(newEdge) )
	{
	  B.SameRange(newEdge, Standard_False);
	}

        //addition NM vertices to new edges
        Standard_Real afpar = (myEdgeDivide->HasCurve3d () ? f3d : f2d);
        Standard_Real alpar = (myEdgeDivide->HasCurve3d () ? l3d : l2d);
        for (Standard_Integer n = 1; n <= aSeqParNM.Length (); ++n)
        {
          Standard_Real apar = aSeqParNM.Value (n);
          TopoDS_Vertex aVold = TopoDS::Vertex (aSeqNMVertices.Value (n));
          TopoDS_Vertex aNMVer = ShapeAnalysis_TransferParametersProj::CopyNMVertex (aVold, newEdge, E);
          Context ()->Replace (aVold, aNMVer);
          if (fabs (apar - afpar) <= Precision::PConfusion ())
          {
            Context ()->Replace (aNMVer, V1);
          }
          else if (fabs (apar - alpar) <= Precision::PConfusion ())
          {
            Context ()->Replace (aNMVer, V);
          }
          else if (apar > afpar && apar < alpar)
          {
            B.Add (newEdge, aNMVer);
          }
          else
          {
            continue;
          }

          aSeqNMVertices.Remove (n);
          aSeqParNM.Remove (n);
          n--;
        }

//	if (ShapeUpgrade::Debug()) cout <<"... New Edge "
//	  <<(void*) &(*newEdge.TShape())<<" on vertices "
//	    <<(void*) &(*V1.TShape())<<", " <<(void*) &(*V.TShape())
//	      <<" with Tolerance "<<TolEdge <<endl;
	B.Add ( resWire, newEdge );
	B.Add ( newWire, newEdge );
	numE++;
	V1 = V;
      }
      if(numE)
      {
        resWire.Closed (BRep_Tool::IsClosed (resWire));
	Context()->Replace(E,resWire);
      }
      else
	Context()->Remove(E);
    }
  }
  if ( Status ( ShapeExtend_DONE ) ) {
//smh#8
    newWire.Closed (BRep_Tool::IsClosed (newWire));
    TopoDS_Shape tmpW = Context()->Apply ( newWire ).Oriented(myWire.Orientation());
    myWire = TopoDS::Wire (tmpW );
  }
}

//=======================================================================
//function : Face
//purpose  : 
//=======================================================================

const TopoDS_Wire& ShapeUpgrade_WireDivide::Wire() const
{
  return myWire;
}

//=======================================================================
//function : Status
//purpose  : 
//=======================================================================

Standard_Boolean ShapeUpgrade_WireDivide::Status (const ShapeExtend_Status status) const
{
  return ShapeExtend::DecodeStatus ( myStatus, status );
}

//=======================================================================
//function : SetSplitCurve3dTool
//purpose  : 
//=======================================================================

void ShapeUpgrade_WireDivide::SetSplitCurve3dTool(const Handle(ShapeUpgrade_SplitCurve3d)& splitCurve3dTool)
{
  mySplitCurve3dTool = splitCurve3dTool;
}

//=======================================================================
//function : SetSplitCurve2dTool
//purpose  : 
//=======================================================================

void ShapeUpgrade_WireDivide::SetSplitCurve2dTool(const Handle(ShapeUpgrade_SplitCurve2d)& splitCurve2dTool)
{
  mySplitCurve2dTool = splitCurve2dTool;
}

//=======================================================================
//function : GetSplitCurve3dTool
//purpose  : 
//=======================================================================

Handle(ShapeUpgrade_SplitCurve3d) ShapeUpgrade_WireDivide::GetSplitCurve3dTool() const
{
  return mySplitCurve3dTool;
}

//=======================================================================
//function : GetSplitCurve2dTool
//purpose  : 
//=======================================================================

Handle(ShapeUpgrade_SplitCurve2d) ShapeUpgrade_WireDivide::GetSplitCurve2dTool() const
{
  return mySplitCurve2dTool;
}

//=======================================================================
//function : SetEdgeDivideTool
//purpose  : 
//=======================================================================

void ShapeUpgrade_WireDivide::SetEdgeDivideTool(const Handle (ShapeUpgrade_EdgeDivide)& edgeDivideTool)
{
  myEdgeDivide = edgeDivideTool;
}

//=======================================================================
//function : GetEdgeDivideTool
//purpose  : 
//=======================================================================

Handle (ShapeUpgrade_EdgeDivide) ShapeUpgrade_WireDivide::GetEdgeDivideTool() const
{
  return myEdgeDivide;
}

//=======================================================================
//function : SetTransferParamTool
//purpose  : 
//=======================================================================

void ShapeUpgrade_WireDivide::SetTransferParamTool(const Handle(ShapeAnalysis_TransferParameters)& TransferParam)
{
  myTransferParamTool = TransferParam;
}

//=======================================================================
//function : GetTransferParamTool
//purpose  : 
//=======================================================================

Handle(ShapeAnalysis_TransferParameters) ShapeUpgrade_WireDivide::GetTransferParamTool()
{
  return myTransferParamTool;
}

//=======================================================================
//function : SetEdgeMode
//purpose  : 
//=======================================================================

 void ShapeUpgrade_WireDivide::SetEdgeMode(const Standard_Integer EdgeMode) 
{
  myEdgeMode = EdgeMode;
}

//=======================================================================
//function : SetFixSmallCurveTool
//purpose  : 
//=======================================================================

 void ShapeUpgrade_WireDivide::SetFixSmallCurveTool(const Handle(ShapeUpgrade_FixSmallCurves)& FixSmallCurvesTool) 
{
  myFixSmallCurveTool = FixSmallCurvesTool; 
}

//=======================================================================
//function : GetFixSmallCurveTool
//purpose  : 
//=======================================================================

 Handle(ShapeUpgrade_FixSmallCurves) ShapeUpgrade_WireDivide::GetFixSmallCurveTool() const
{
  return  myFixSmallCurveTool;
}
