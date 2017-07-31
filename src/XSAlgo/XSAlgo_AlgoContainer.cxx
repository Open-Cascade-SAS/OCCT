// Created on: 2000-01-19
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
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Interface_Static.hxx>
#include <Message_ListIteratorOfListOfMsg.hxx>
#include <Message_ListOfMsg.hxx>
#include <Message_Msg.hxx>
#include <Message_ProgressIndicator.hxx>
#include <Resource_Manager.hxx>
#include <ShapeAlgo.hxx>
#include <ShapeAlgo_AlgoContainer.hxx>
#include <ShapeAlgo_ToolContainer.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeCustom.hxx>
#include <ShapeExtend_DataMapOfShapeListOfMsg.hxx>
#include <ShapeExtend_MsgRegistrator.hxx>
#include <ShapeFix_Edge.hxx>
#include <ShapeFix_Shape.hxx>
#include <ShapeProcess.hxx>
#include <ShapeProcess_ShapeContext.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_TransientListBinder.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_ShapeBinder.hxx>
#include <TransferBRep_ShapeMapper.hxx>
#include <UnitsMethods.hxx>
#include <XSAlgo_AlgoContainer.hxx>
#include <XSAlgo_ToolContainer.hxx>
#include <TopExp_Explorer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XSAlgo_AlgoContainer,Standard_Transient)

//=======================================================================
//function : XSAlgo_AlgoContainer
//purpose  : 
//=======================================================================
XSAlgo_AlgoContainer::XSAlgo_AlgoContainer()
{
  myTC = new XSAlgo_ToolContainer;
}

//=======================================================================
//function : PrepareForTransfer
//purpose  : 
//=======================================================================

void XSAlgo_AlgoContainer::PrepareForTransfer() const
{
  UnitsMethods::SetCasCadeLengthUnit ( Interface_Static::IVal("xstep.cascade.unit") );
}

//=======================================================================
//function : ProcessShape
//purpose  : 
//=======================================================================

TopoDS_Shape XSAlgo_AlgoContainer::ProcessShape (const TopoDS_Shape& shape,
                                                 const Standard_Real Prec,
                                                 const Standard_Real maxTol,
                                                 const Standard_CString prscfile,
                                                 const Standard_CString pseq,
                                                 Handle(Standard_Transient)& info,
                                                 const Handle(Message_ProgressIndicator)& progress,
                                                 const Standard_Boolean NonManifold) const
{
  if ( shape.IsNull() ) return shape;
  
  Handle(ShapeProcess_ShapeContext) context = Handle(ShapeProcess_ShapeContext)::DownCast(info);
  if ( context.IsNull() )
  {
    Standard_CString rscfile = Interface_Static::CVal(prscfile);
    if (!rscfile)
      rscfile = prscfile;
    context = new ShapeProcess_ShapeContext(shape, rscfile);
    context->SetDetalisation(TopAbs_EDGE);
    if ( !progress.IsNull() )
      context->SetProgress(progress);
  }
  context->SetNonManifold(NonManifold);
  info = context;
  
  Standard_CString seq = Interface_Static::CVal ( pseq );
  if ( ! seq ) seq = pseq;
  
  // if resource file is not loaded or does not define <seq>.exec.op, 
  // do default fixes
  Handle(Resource_Manager) rsc = context->ResourceManager();
  TCollection_AsciiString str ( seq );
  str += ".exec.op";
  if ( ! rsc->Find ( str.ToCString() ) ) {
#ifdef OCCT_DEBUG
    {
      static Standard_Integer time = 0;
      if ( ! time )
	cout << "Warning: XSAlgo_AlgoContainer::ProcessShape(): Sequence " << str.ToCString() << 
	        " is not defined in " << prscfile << " resource; do default processing" << endl;
      time++;
    }
#endif
    // if reading, do default ShapeFix
    if ( ! strncmp ( pseq, "read.", 5 ) ) {
      try {
        OCC_CATCH_SIGNALS
	Handle(ShapeExtend_MsgRegistrator) msg = new ShapeExtend_MsgRegistrator;
	Handle(ShapeFix_Shape) sfs = ShapeAlgo::AlgoContainer()->ToolContainer()->FixShape();
	sfs->Init ( shape );
	sfs->SetMsgRegistrator ( msg );
	sfs->SetPrecision ( Prec );
	sfs->SetMaxTolerance ( maxTol );
	sfs->FixFaceTool()->FixWireTool()->FixSameParameterMode() = Standard_False;
	sfs->FixSolidTool()->CreateOpenSolidMode() = Standard_False;
	sfs->Perform(progress);

	TopoDS_Shape S = sfs->Shape();
	if ( ! S.IsNull() && S != shape ) {
	  context->RecordModification ( sfs->Context(), msg );
	  context->SetResult ( S );
	}
      }
      catch (Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
	cout << "Error: XSAlgo_AlgoContainer::ProcessShape(): Exception in ShapeFix::Shape" << endl;
        anException.Print(cout); cout << endl;
#endif
	(void)anException;
      }
      return context->Result();
    }
    // for writing, define default sequence of DirectFaces
    else if ( ! strncmp ( pseq, "write.", 6 ) ) {
      rsc->SetResource ( str.ToCString(), "DirectFaces" );
    }
  }
  
  // Define runtime tolerances and do Shape Processing 
  rsc->SetResource ( "Runtime.Tolerance", Prec );
  rsc->SetResource ( "Runtime.MaxTolerance", maxTol );

  if ( !ShapeProcess::Perform(context, seq) )
    return shape; // return original shape

  return context->Result();
}
						  
//=======================================================================
//function : PerformFixShape
//purpose  : 
//=======================================================================

/*
TopoDS_Shape XSAlgo_AlgoContainer::PerformFixShape(const TopoDS_Shape& S,
						      const Handle(Transfer_TransientProcess)& TP,
						      const Standard_Real Prec,
						      const Standard_Real MaxTol) const
{
  if ( S.IsNull() ) return S;
  
  TopoDS_Shape shape = S;
  // fixing shape
  try {
    OCC_CATCH_SIGNALS
    Handle(ShapeFix_Shape) sfs = ShapeAlgo::AlgoContainer()->ToolContainer()->FixShape();
    sfs->Init ( S );
    sfs->SetMsgRegistrator ( new ShapeExtend_MsgRegistrator );
    sfs->SetPrecision ( Prec );
    sfs->SetMaxTolerance ( MaxTol );
    sfs->FixFaceTool()->FixWireTool()->FixSameParameterMode() = Standard_False;
    sfs->Perform();

    shape = sfs->Shape();
    
    // to be removed when messages come
    if ( shape == S || shape.IsNull() ) return S;
    
    // update map to reflect the substitutions
    Handle(ShapeBuild_ReShape) context = sfs->Context();
    const ShapeExtend_DataMapOfShapeListOfMsg& msgmap = 
      Handle(ShapeExtend_MsgRegistrator)::DownCast (sfs->MsgRegistrator())->MapShape();
    for ( Standard_Integer i=1; i <= TP->NbMapped(); i++ ) {
      Handle(Transfer_Binder) bnd = TP->MapItem ( i );
      Handle(TransferBRep_ShapeBinder) sb = Handle(TransferBRep_ShapeBinder)::DownCast ( bnd );
      if ( sb.IsNull() || sb->Result().IsNull() ) continue;
      
      TopoDS_Shape orig = sb->Result();
      
      // update messages (messages must be taken from each level in the substitution map)
      TopoDS_Shape cur, next = orig;
      do {
	cur = next;
	Message_ListOfMsg msglist;
	if (msgmap.IsBound (cur)) {
	  msglist = msgmap.Find (cur);
	  for (Message_ListIteratorOfListOfMsg iter (msglist); iter.More(); iter.Next()) {
	    const Message_Msg& msg = iter.Value();
	    sb->AddWarning (msg.Value(), msg.Original());
	  }
	}
	next = context->Value (cur);
      } while (cur != next);

      // update shapes
      TopoDS_Shape res;
      if ( ! context->Status ( orig, res, Standard_True ) ) continue;

      sb->SetResult ( res );
    }
  }
  catch (Standard_Failure) {
#ifdef OCCT_DEBUG
    cout << "Error: XSAlgo_AlgoContainer::PerformFixShape(): Exception in ShapeFix::Shape" << endl;
#endif
  }   
  return shape;
}
*/

// ============================================================================
// Method  : MakeEdgeOnCurve
// Purpose : for CheckPCurve
// ============================================================================

static TopoDS_Edge MakeEdgeOnCurve(const TopoDS_Edge edge)
{
  TopoDS_Edge result;
  //BRep_Builder B; // B not used - see below (skl)
  Handle(Geom_Curve) C3d;
  ShapeAnalysis_Edge sae;
  Standard_Real cf, cl;
  if (!sae.Curve3d (edge, C3d, cf, cl, Standard_False )) 
    return result;
  gp_Pnt PV1 = C3d->Value(cf);
  gp_Pnt PV2 = C3d->Value(cl);
  BRepBuilderAPI_MakeEdge mkEdge(C3d, PV1, PV2, cf, cl);
//:S4136  Standard_Real tol = BRep_Tool::Tolerance (edge);
  ShapeBuild_Edge SBE;          //skl 10.07.2001
  SBE.SetRange3d(mkEdge,cf,cl); //skl 10.07.2001
  result = mkEdge.Edge();
//:S4136  B.UpdateEdge(result,tol);
  return result;
}

//=======================================================================
//function : CheckPCurve
//purpose  : 
//=======================================================================

Standard_Boolean XSAlgo_AlgoContainer::CheckPCurve (const TopoDS_Edge& E,
						    const TopoDS_Face& face,
						    const Standard_Real preci,
						    const Standard_Boolean isSeam) const
{
  Standard_Real w1, w2;  
  Handle(Geom2d_Curve) thePC;
  ShapeAnalysis_Edge sae;
  if ( ! sae.PCurve (E, face, thePC, w1, w2, Standard_False ) ) {
    return Standard_False;
  }

  // Check for pcurve longer than surface
  Handle(Geom_Surface) surf = BRep_Tool::Surface(face);
  Standard_Real UF,UL,VF,VL;
  surf->Bounds (UF,UL,VF,VL);
  gp_Pnt2d PUV1, PUV2;
  PUV1 = thePC->Value(w1);
  PUV2 = thePC->Value(w2);
  //    Multi-periodique ? mieux vaut jeter (attention aux valeurs infinies)
  Standard_Real DU = Abs (PUV1.X() - PUV2.X());
  Standard_Real DV = Abs (PUV1.Y() - PUV2.Y());
  if ( DU/8. > (UL/6. - UF/6.) || DV/8. > (VL/6. - VF/6.) ) {
    ShapeBuild_Edge().RemovePCurve(E,face);
#ifdef OCCT_DEBUG
    cout<<"Removing pcuve periodic"<<endl;
#endif      
    return Standard_False;
  }

  // Second Check : 2D and 3D consistency (if the Pcurve has not been 
  //                dropped)
  //  On verifie aussi qu on ne s enroule pas trop ...
  //  ex. : UVV en DEGRES sur une surface en RADIANS, recalee = 57 tours !

  Handle(Geom_Curve) C3d;
  Standard_Real cf1, cl1;
  sae.Curve3d (E, C3d, cf1, cl1, Standard_False );

  gp_Pnt P1 = surf->Value(PUV1.X(), PUV1.Y());
  gp_Pnt P2 = surf->Value(PUV2.X(), PUV2.Y());
  TopoDS_Vertex V1 = TopExp::FirstVertex(E);
  TopoDS_Vertex V2 = TopExp::LastVertex(E);
  gp_Pnt PV1 = ( C3d.IsNull() ? BRep_Tool::Pnt(V1) : C3d->Value(cf1) );
  gp_Pnt PV2 = ( C3d.IsNull() ? BRep_Tool::Pnt(V2) : C3d->Value(cl1) );
  Standard_Real Dist11 = PV1.Distance(P1), Dist22 = PV2.Distance(P2);
    
  if (!((Dist11 <= preci) && (Dist22 <= preci))) {
    ShapeBuild_Edge().RemovePCurve(E,face);
#ifdef OCCT_DEBUG
    cout<<"Removing pcurve points"<<endl;
#endif      
    return Standard_False;
  }

  //
  // pdn checking deviation between pcurve and 3D curve
  //
  
  // Make temporary edge for analysis
  if ( C3d.IsNull() ) return Standard_False;
  TopoDS_Edge edge = MakeEdgeOnCurve(E);
  
  // fill it with pcurve(s)
  BRep_Builder B;
  Handle(Geom2d_Curve) seamPC;
  if ( isSeam ) {
    Standard_Real f, l;
    TopoDS_Shape REdge = E.Reversed() ;
    if ( ! sae.PCurve ( TopoDS::Edge ( REdge ), 
		        face, seamPC, f, l, Standard_False ) ||
	 seamPC == thePC ) 
      seamPC = Handle(Geom2d_Curve)::DownCast ( thePC->Copy() );
    B.UpdateEdge ( edge, thePC, seamPC, face, 0.);
  }
  else B.UpdateEdge ( edge, thePC, face, 0.);
  B.Range(edge,face,w1,w2);
  B.SameRange(edge, Standard_False );
  //:S4136
  Standard_Integer SPmode = Interface_Static::IVal("read.stdsameparameter.mode");
  if ( SPmode ) 
    B.SameParameter (edge, Standard_False );

  // call FixSP to see what it will do
  Handle(ShapeFix_Edge) sfe = new ShapeFix_Edge;
  sfe->FixSameParameter(edge);
  Standard_Real tol = BRep_Tool::Tolerance (edge);
  //    Standard_Real tolV1 = BRep_Tool::Tolerance(TopExp::FirstVertex(edge));
  //    Standard_Real tolV2 = BRep_Tool::Tolerance(TopExp::LastVertex(edge));			       
  Standard_Boolean sr = BRep_Tool::SameRange ( edge );
  Standard_Boolean sp = BRep_Tool::SameParameter ( edge );
  
  // if result is not nice, try to call projection and take the best
  if ( tol > Min ( 1., 2.*preci ) || ! sr ) {
    //pdn trying to recompute pcurve 
    TopoDS_Edge edgePr = MakeEdgeOnCurve(E);
    sfe->FixAddPCurve(edgePr, face, isSeam, preci);
    sfe->FixSameParameter(edgePr);
    Standard_Real tolPr = BRep_Tool::Tolerance (edgePr);
    //pdn choose the best pcurve
    if ( tolPr < tol || ! sr ) {
      //	tolV1 = BRep_Tool::Tolerance(TopExp::FirstVertex(edgePr));
      //	tolV2 = BRep_Tool::Tolerance(TopExp::LastVertex(edgePr));
      sr = BRep_Tool::SameRange ( edgePr );
      sp = BRep_Tool::SameParameter ( edgePr );
      tol = tolPr;
      edge = edgePr;
    }
  }
  
  // get corrected pcurve from the temporary edge, and put to original
  sae.PCurve ( edge, face, thePC, w1, w2, Standard_False );
  if ( isSeam ) {
    Standard_Real f, l;
    TopoDS_Shape REdge = edge.Reversed();
    sae.PCurve ( TopoDS::Edge ( REdge ), face, seamPC, f, l, Standard_False );
    if ( E.Orientation() == TopAbs_REVERSED ) //:abv 14.11.01: coneEl.sat loop
      B.UpdateEdge ( E, seamPC, thePC, face, tol );
    else 
      B.UpdateEdge ( E, thePC, seamPC, face, tol );
  }
  else B.UpdateEdge ( E, thePC, face, tol );

  B.UpdateVertex(V1,tol);
  B.UpdateVertex(V2,tol);
  B.Range(E,face, w1, w2);
  if(BRep_Tool::SameRange(E))
    B.SameRange( E, sr );
  if(BRep_Tool::SameParameter(E))
    B.SameParameter ( E, sp );
  
  return Standard_True;
}

//=======================================================================
//function : MergeTransferInfo
//purpose  : 
//=======================================================================

void XSAlgo_AlgoContainer::MergeTransferInfo(const Handle(Transfer_TransientProcess)& TP,
						const Handle(Standard_Transient) &info,
						const Standard_Integer startTPitem) const
{
  Handle(ShapeProcess_ShapeContext) context = Handle(ShapeProcess_ShapeContext)::DownCast ( info );
  if ( context.IsNull() ) return;

  const TopTools_DataMapOfShapeShape &map = context->Map();
  Handle(ShapeExtend_MsgRegistrator) msg = context->Messages();
  if ( map.Extent() <=0 && ( msg.IsNull() || msg->MapShape().Extent() <=0 ) )
    return;

  Standard_Integer i = ( startTPitem >0 ? startTPitem : 1 );
  for ( ; i <= TP->NbMapped(); i++ )
  {
    Handle(Transfer_Binder) bnd = TP->MapItem ( i );
    Handle(TransferBRep_ShapeBinder) sb = Handle(TransferBRep_ShapeBinder)::DownCast ( bnd );
    if ( sb.IsNull() || sb->Result().IsNull() ) continue;

    TopoDS_Shape orig = sb->Result();

    if ( map.IsBound ( orig ) )
    {
      sb->SetResult ( map.Find ( orig ) );
    }
    else if ( !orig.Location().IsIdentity() )
    {
      TopLoc_Location aNullLoc;
      TopoDS_Shape atmpSh = orig.Located(aNullLoc);
      if ( map.IsBound ( atmpSh ) ) sb->SetResult ( map.Find ( atmpSh ) );
    }
    else
    {
      // Some of edges may be modified.
      BRepTools_ReShape aReShape;
      Standard_Boolean hasModifiedEdges = Standard_False;
      TopExp_Explorer anExpSE(orig, TopAbs_EDGE);

      // Remember modifications.
      for( ; anExpSE.More() ; anExpSE.Next() )
      {
        if (  map.IsBound ( anExpSE.Current() ) )
        {
          hasModifiedEdges = Standard_True;
          TopoDS_Shape aModifiedShape = map.Find( anExpSE.Current() );
          aReShape.Replace(anExpSE.Current(), aModifiedShape);
        }
      }

      // Apply modifications and store result in binder.
      if (hasModifiedEdges)
      {
        TopoDS_Shape aRes = aReShape.Apply(orig);
        sb->SetResult ( aRes );
      }
    }

      
    // update messages
    if ( ! msg.IsNull() ) {
      const ShapeExtend_DataMapOfShapeListOfMsg& msgmap = msg->MapShape();
      if ( msgmap.IsBound (orig) ) {
	const Message_ListOfMsg &msglist = msgmap.Find (orig);
	for (Message_ListIteratorOfListOfMsg iter (msglist); iter.More(); iter.Next()) {
	  const Message_Msg& mess = iter.Value();
	  sb->AddWarning (TCollection_AsciiString(mess.Value()).ToCString(),
                          TCollection_AsciiString(mess.Original()).ToCString());
	}
      }
    }
  }
}

//=======================================================================
//function : MergeTransferInfo
//purpose  : 
//=======================================================================

void XSAlgo_AlgoContainer::MergeTransferInfo(const Handle(Transfer_FinderProcess)& FP,
						const Handle(Standard_Transient) &info) const
{
  Handle(ShapeProcess_ShapeContext) context = Handle(ShapeProcess_ShapeContext)::DownCast ( info );
  if ( context.IsNull() ) return;

  const TopTools_DataMapOfShapeShape &map = context->Map();
  TopTools_DataMapIteratorOfDataMapOfShapeShape ShapeShapeIterator(map);
  Handle(ShapeExtend_MsgRegistrator) msg = context->Messages();

  for ( ; ShapeShapeIterator.More(); ShapeShapeIterator.Next() ) {

    TopoDS_Shape orig = ShapeShapeIterator.Key(), res = ShapeShapeIterator.Value();
    Handle(TransferBRep_ShapeMapper) resMapper = TransferBRep::ShapeMapper ( FP, res );
    Handle(Transfer_Binder) resBinder = FP->Find ( resMapper );
    
    if (resBinder.IsNull()) {
      resBinder = new TransferBRep_ShapeBinder(res);
      //if <orig> shape was splitted, put entities corresponding to new shapes
      // into Transfer_TransientListBinder.
      if ( orig.ShapeType() > res.ShapeType() ) {
	TopoDS_Shape sub;
	Handle(Transfer_TransientListBinder) TransientListBinder = new Transfer_TransientListBinder;
	for (TopoDS_Iterator it(res); it.More(); it.Next()) {
	  Handle(Transfer_Finder) subMapper = TransferBRep::ShapeMapper ( FP, it.Value());
	  if (subMapper.IsNull()) continue;

	  Handle(Standard_Transient) tr = FP->FindTransient ( subMapper );
	  if (tr.IsNull()) continue;
	  TransientListBinder->AddResult(tr);
	  sub = it.Value();
	}
	if ( TransientListBinder->NbTransients() == 1 ) resBinder = new TransferBRep_ShapeBinder(sub);
	else if ( TransientListBinder->NbTransients() > 1 ) {
          resBinder->AddResult(TransientListBinder);
//	  resBinder->SetNext(TransientListBinder, Standard_True);
#ifdef OCCT_DEBUG
	  cout<<"Info: TransientListBinder created for splitted shape"<<endl;
	} 
	else {
	  cout<<"Warning: XSAlgo_AlgoContainer::MergeTransferInfo() "
	    <<"No results were found for splitted shape. "<<endl;
	  //<<"Transfer_FinderProcess->NbMapped() = "<<FP->NbMapped()<<endl;
#endif	  
	}
      }
    }
    
    Handle(TransferBRep_ShapeMapper) origMapper= TransferBRep::ShapeMapper ( FP, orig);
    Handle(Transfer_Binder) origBinder = FP->Find ( origMapper );
    if ( origBinder.IsNull() ) {
      FP->Bind(origMapper, resBinder);
    }
    else {
      origBinder->AddResult ( resBinder );
    }
    
    // update messages
    if ( ! msg.IsNull() ) {
      const ShapeExtend_DataMapOfShapeListOfMsg& msgmap = msg->MapShape();
      if ( msgmap.IsBound (orig) ) {
	const Message_ListOfMsg &msglist = msgmap.Find (orig);
	for (Message_ListIteratorOfListOfMsg iter (msglist); iter.More(); iter.Next()) {
	  const Message_Msg& mess = iter.Value();
	  resBinder->AddWarning (TCollection_AsciiString(mess.Value()).ToCString(),
                                 TCollection_AsciiString(mess.Original()).ToCString());
	}
      }
    }
  }
}
