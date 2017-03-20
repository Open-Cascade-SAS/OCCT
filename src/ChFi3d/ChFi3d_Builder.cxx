// Created on: 1993-11-18
// Created by: Isabelle GRIGNON
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <AppBlend_Approx.hxx>
#include <Blend_CurvPointFuncInv.hxx>
#include <Blend_FuncInv.hxx>
#include <Blend_Function.hxx>
#include <Blend_RstRstFunction.hxx>
#include <Blend_SurfCurvFuncInv.hxx>
#include <Blend_SurfPointFuncInv.hxx>
#include <Blend_SurfRstFunction.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepBlend_Line.hxx>
#include <BRepLib.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <ChFi3d.hxx>
#include <ChFi3d_Builder.hxx>
#include <ChFi3d_Builder_0.hxx>
#include <ChFiDS_CommonPoint.hxx>
#include <ChFiDS_HData.hxx>
#include <ChFiDS_HElSpine.hxx>
#include <ChFiDS_ListIteratorOfListOfStripe.hxx>
#include <ChFiDS_SequenceOfSurfData.hxx>
#include <ChFiDS_Spine.hxx>
#include <ChFiDS_Stripe.hxx>
#include <ChFiDS_SurfData.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <ShapeFix.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TopAbs.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_CurveExplorer.hxx>
#include <TopOpeBRepDS_CurvePointInterference.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_ListOfInterference.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

#ifdef OCCT_DEBUG
#include <OSD_Chronometer.hxx>



// variables for performances 


OSD_Chronometer cl_total,cl_extent,cl_perfsetofsurf,cl_perffilletonvertex,
cl_filds,cl_reconstruction,cl_setregul,cl_perform1corner,cl_perform2corner,
cl_performatend,cl_perform3corner,cl_performmore3corner;

Standard_EXPORT Standard_Real t_total, t_extent,t_perfsetofsurf,
t_perffilletonvertex, t_filds,t_reconstruction,t_setregul, t_perfsetofkgen,
t_perfsetofkpart,t_makextremities,t_performatend,t_startsol,t_performsurf,
t_perform1corner,t_perform2corner,t_perform3corner,t_performmore3corner,
t_batten,t_inter,t_sameinter,t_same,t_plate,t_approxplate,t_t2cornerinit,
t_perf2cornerbyinter,t_chfikpartcompdata,t_cheminement,t_remplissage,
t_t3cornerinit ,t_spherique,t_torique, t_notfilling,t_filling,t_sameparam,
t_computedata,t_completedata,t_t2cornerDS,t_t3cornerDS;
               
extern void ChFi3d_InitChron(OSD_Chronometer& ch);
extern void ChFi3d_ResultChron(OSD_Chronometer & ch, Standard_Real& time);
extern Standard_Boolean ChFi3d_GettraceCHRON();
#endif


//=======================================================================
//function : CompleteDS
//purpose  : 
//=======================================================================

static void CompleteDS(TopOpeBRepDS_DataStructure& DStr,
		       const TopoDS_Shape& S)
{
  ChFiDS_Map MapEW,MapFS;
  MapEW.Fill(S,TopAbs_EDGE,TopAbs_WIRE);
  MapFS.Fill(S,TopAbs_FACE,TopAbs_SHELL);
  
  TopExp_Explorer ExpE;
  for (ExpE.Init(S,TopAbs_EDGE); ExpE.More(); ExpE.Next()) {
    const TopoDS_Edge& E = TopoDS::Edge(ExpE.Current());
    Standard_Boolean hasgeom = DStr.HasGeometry(E);
    if (hasgeom) {
      const TopTools_ListOfShape& WireListAnc = MapEW(E);
      TopTools_ListIteratorOfListOfShape itaW(WireListAnc);
      while (itaW.More()) {
	const TopoDS_Shape& WireAnc = itaW.Value();
	DStr.AddShape(WireAnc);
	itaW.Next();
      }
    }
  }
  
  TopExp_Explorer ExpF;
  for (ExpF.Init(S,TopAbs_FACE); ExpF.More(); ExpF.Next()) {
    const TopoDS_Face& F = TopoDS::Face(ExpF.Current());
    Standard_Boolean hasgeom = DStr.HasGeometry(F);
    if (hasgeom) {
      const TopTools_ListOfShape& ShellListAnc = MapFS(F);
      TopTools_ListIteratorOfListOfShape itaS(ShellListAnc);
      while (itaS.More()) {
	const TopoDS_Shape& ShellAnc = itaS.Value();
	DStr.AddShape(ShellAnc);
	itaS.Next();
      }
    }
  }
  
  // set the range on the DS Curves
  for (Standard_Integer ic = 1; ic <= DStr.NbCurves(); ic++) {
    Standard_Real parmin = RealLast(), parmax = RealFirst();
    const TopOpeBRepDS_ListOfInterference& LI = DStr.CurveInterferences(ic);
    for (TopOpeBRepDS_PointIterator it(LI);
	 it.More();
	 it.Next() ) {
      Standard_Real par = it.Parameter();
      parmin = Min (parmin,par); parmax = Max (parmax,par);
    }
    DStr.ChangeCurve(ic).SetRange(parmin,parmax);
  }
}

//=======================================================================
//function : ~ChFi3d_Builder
//purpose  : 
//=======================================================================

ChFi3d_Builder::~ChFi3d_Builder()
{}

//=======================================================================
//function : ExtentAnalyse
//purpose  : 
//=======================================================================

void ChFi3d_Builder::ExtentAnalyse ()
{
  Standard_Integer nbedges, nbs;
  for (Standard_Integer iv = 1; iv <= myVDataMap.Extent(); iv++) {
    nbs = myVDataMap(iv).Extent();
    const TopoDS_Vertex& Vtx = myVDataMap.FindKey(iv);
    //nbedges = ChFi3d_NumberOfEdges(Vtx, myVEMap); 
    nbedges = ChFi3d_NumberOfSharpEdges(Vtx, myVEMap, myEFMap); 
    switch (nbs) {
    case 1 :
      ExtentOneCorner(Vtx, myVDataMap.FindFromIndex(iv).First());
      break;
    case 2 :
      if (nbedges <= 3)
	ExtentTwoCorner(Vtx, myVDataMap.FindFromIndex(iv));
      break;
    case 3 :
      if (nbedges <= 3)
	ExtentThreeCorner(Vtx, myVDataMap.FindFromIndex(iv));
      break;
    default : 
      break;
    }
  }
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void  ChFi3d_Builder::Compute()
{
  
#ifdef OCCT_DEBUG   //perf 
  t_total=0;t_extent=0; t_perfsetofsurf=0;t_perffilletonvertex=0;
  t_filds=0;t_reconstruction=0;t_setregul=0;
  t_perfsetofkpart=0; t_perfsetofkgen=0;t_makextremities=0;
  t_performsurf=0;t_startsol=0; t_perform1corner=0;t_perform2corner=0;
  t_perform3corner=0;t_performmore3corner=0;t_inter=0;t_same=0;t_sameinter=0;
  t_plate=0;t_approxplate=0; t_batten=0;t_remplissage=0;t_t3cornerinit=0;
  t_spherique=0;t_torique=0;t_notfilling=0;t_filling=0;t_performatend=0;
  t_t2cornerinit=0; t_perf2cornerbyinter=0;t_chfikpartcompdata=0;
  t_cheminement=0; t_sameparam=0; t_computedata=0;t_completedata=0;
  t_t2cornerDS=0;t_t3cornerDS=0;
  ChFi3d_InitChron(cl_total);
  ChFi3d_InitChron(cl_extent);
#endif 
  
  if (myListStripe.IsEmpty())
    throw Standard_Failure("There are no suitable edges for chamfer or fillet");
  
  Reset();
  myDS = new TopOpeBRepDS_HDataStructure();
  TopOpeBRepDS_DataStructure& DStr = myDS->ChangeDS();
  done = Standard_True;
  hasresult=Standard_False;
  
  // filling of myVDatatMap
  ChFiDS_ListIteratorOfListOfStripe itel;
  
  for (itel.Initialize(myListStripe);itel.More(); itel.Next()) {
    if ((itel.Value()->Spine()->FirstStatus() <= ChFiDS_BreakPoint))
      myVDataMap.Add(itel.Value()->Spine()->FirstVertex(),itel.Value());
    else if (itel.Value()->Spine()->FirstStatus() == ChFiDS_FreeBoundary)
      ExtentOneCorner(itel.Value()->Spine()->FirstVertex(),itel.Value());
    if ((itel.Value()->Spine()->LastStatus() <= ChFiDS_BreakPoint))
      myVDataMap.Add(itel.Value()->Spine()->LastVertex() ,itel.Value());
    else if (itel.Value()->Spine()->LastStatus() == ChFiDS_FreeBoundary)
      ExtentOneCorner(itel.Value()->Spine()->LastVertex(),itel.Value());
  }
  // preanalysis to evaluate the extensions.
  ExtentAnalyse();
  
  
#ifdef OCCT_DEBUG //perf 
  ChFi3d_ResultChron(cl_extent,t_extent);
  ChFi3d_InitChron(cl_perfsetofsurf);
#endif
  
  // Construction of the stripe of fillet on each stripe.
  for (itel.Initialize(myListStripe);itel.More(); itel.Next()) {
    itel.Value()->Spine()->SetErrorStatus(ChFiDS_Ok);
    try {
      OCC_CATCH_SIGNALS
      PerformSetOfSurf(itel.Value());
    }
    catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
      cout <<"EXCEPTION Stripe compute " << anException << endl;
#endif
      (void)anException;
      badstripes.Append(itel.Value());
      done = Standard_True;
      if (itel.Value()->Spine()->ErrorStatus()==ChFiDS_Ok) 
      itel.Value()->Spine()->SetErrorStatus(ChFiDS_Error);
    }
    if (!done) badstripes.Append(itel.Value());
    done = Standard_True;
  }
  done = (badstripes.IsEmpty());
  
#ifdef OCCT_DEBUG //perf 
  ChFi3d_ResultChron(cl_perfsetofsurf,t_perfsetofsurf);
  ChFi3d_InitChron(cl_perffilletonvertex);
#endif 
  
  //construct fillets on each vertex + feed the Ds
  if (done) {
    Standard_Integer j;
    for (j=1;j<=myVDataMap.Extent();j++)
    {
      try
      {
        OCC_CATCH_SIGNALS
        PerformFilletOnVertex(j);
      }
      catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
        cout <<"EXCEPTION Corner compute " << anException << endl;
#endif
        (void)anException;
        badvertices.Append(myVDataMap.FindKey(j));
        hasresult=Standard_False;
        done = Standard_True;
      }
      if (!done) badvertices.Append(myVDataMap.FindKey(j));
      done = Standard_True;
    }
    if (!hasresult) done = badvertices.IsEmpty();
  }
  

#ifdef OCCT_DEBUG //perf 
  ChFi3d_ResultChron(cl_perffilletonvertex,t_perffilletonvertex);
  ChFi3d_InitChron(cl_filds);
#endif
  
  TColStd_MapOfInteger MapIndSo;
  TopExp_Explorer expso(myShape,TopAbs_SOLID);
  for(; expso.More(); expso.Next()){
    const TopoDS_Shape& cursol = expso.Current();
    Standard_Integer indcursol = DStr.AddShape(cursol);
    MapIndSo.Add(indcursol);
  }
  TopExp_Explorer expsh(myShape,TopAbs_SHELL,TopAbs_SOLID);
  for(; expsh.More(); expsh.Next()){
    const TopoDS_Shape& cursh = expsh.Current();
    Standard_Integer indcursh = DStr.AddShape(cursh);
    MapIndSo.Add(indcursh);
  }
  if (done) {
    Standard_Integer i1;
    for (itel.Initialize(myListStripe), i1=0;
	 itel.More(); 
	 itel.Next(), i1++) {
      const Handle(ChFiDS_Stripe)& st = itel.Value();
      // 05/02/02 akm vvv : (OCC119) First we'll check ain't there 
      //                    intersections between fillets
      ChFiDS_ListIteratorOfListOfStripe itel1;
      Standard_Integer i2;
      for (itel1.Initialize(myListStripe), i2=0;
	   itel1.More(); 
	   itel1.Next(), i2++) {
	if (i2 <= i1)
	  // Do not twice intersect the stripes
	  continue;
	Handle(ChFiDS_Stripe) aCheckStripe = itel1.Value();
	try {
	  OCC_CATCH_SIGNALS
	  ChFi3d_StripeEdgeInter (st, aCheckStripe, DStr, tol2d);
	}
	catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
	  cout <<"EXCEPTION Fillets compute " << anException << endl;
#endif
	  (void)anException;
	  badstripes.Append(itel.Value());
	  hasresult=Standard_False;
	  done = Standard_False;
	  break;
	}
      }
      // 05/02/02 akm ^^^
      Standard_Integer solidindex = st->SolidIndex();
      ChFi3d_FilDS(solidindex,st,DStr,myRegul,tolesp,tol2d);
      if (!done) break;
    }
    
#ifdef OCCT_DEBUG //perf 
    ChFi3d_ResultChron(cl_filds,t_filds);
    ChFi3d_InitChron(cl_reconstruction);
#endif
    
    
    if (done) {
      BRep_Builder B1;
      CompleteDS(DStr,myShape);
      //Update tolerances on vertex to max adjacent edges or
      //Update tolerances on degenerated edge to max of adjacent vertexes.
      TopOpeBRepDS_CurveExplorer cex(DStr);
      for(;cex.More();cex.Next()){
	TopOpeBRepDS_Curve& c = *((TopOpeBRepDS_Curve*)(void*)&(cex.Curve()));
	Standard_Real tolc = 0.;
	Standard_Boolean degen = c.Curve().IsNull();
	if(!degen) tolc = c.Tolerance();
	Standard_Integer ic = cex.Index();
	TopOpeBRepDS_PointIterator It(myDS->CurvePoints(ic));
	for(;It.More();It.Next()){
	  Handle(TopOpeBRepDS_CurvePointInterference) II;
	  II = Handle(TopOpeBRepDS_CurvePointInterference)::DownCast(It.Value());
	  if (II.IsNull()) continue;
	  TopOpeBRepDS_Kind gk = II->GeometryType();
	  Standard_Integer gi = II->Geometry();
	  if(gk == TopOpeBRepDS_VERTEX){
	    const TopoDS_Vertex& v = TopoDS::Vertex(myDS->Shape(gi));
	    Standard_Real tolv = BRep_Tool::Tolerance(v);
	    if( tolv > 0.0001 ) {
	      tolv += 0.0003;
	      if( tolc < tolv ) tolc = tolv + 0.00001;
	    }
	    if(degen && tolc < tolv) tolc = tolv;
	    else if(tolc>tolv) B1.UpdateVertex(v,tolc);
	  }
	  else if(gk == TopOpeBRepDS_POINT){
	    TopOpeBRepDS_Point& p = DStr.ChangePoint(gi);
	    Standard_Real tolp = p.Tolerance();
	    if(degen && tolc < tolp) tolc = tolp;
	    else if(tolc>tolp) p.Tolerance(tolc);
	  }
	}
	if(degen) c.Tolerance(tolc);
      }
      myCoup->Perform(myDS);
      TColStd_MapIteratorOfMapOfInteger It(MapIndSo);
      for(; It.More(); It.Next()){
	Standard_Integer indsol = It.Key();
	const TopoDS_Shape& curshape = DStr.Shape(indsol);
	myCoup->MergeSolid(curshape,TopAbs_IN);
      }
      
      Standard_Integer i=1,n=DStr.NbShapes();
      for (;i<=n;i++) {
	const TopoDS_Shape S = DStr.Shape(i);
	if (S.ShapeType() != TopAbs_EDGE) continue;
	Standard_Boolean issplitIN = myCoup->IsSplit(S,TopAbs_IN);
	if ( !issplitIN ) continue;
	TopTools_ListIteratorOfListOfShape it(myCoup->Splits(S,TopAbs_IN));
	for (; it.More(); it.Next() ) {
	  const TopoDS_Edge& newE = TopoDS::Edge(it.Value());
	  Standard_Real tole = BRep_Tool::Tolerance(newE);
	  TopExp_Explorer exv(newE,TopAbs_VERTEX);
	  for (; exv.More(); exv.Next() ) {
	    const TopoDS_Vertex& v = TopoDS::Vertex(exv.Current());
	    Standard_Real tolv = BRep_Tool::Tolerance(v);
	    if (tole>tolv) B1.UpdateVertex(v,tole);
	  }
	}
      }
      if (!hasresult) {
      B1.MakeCompound(TopoDS::Compound(myShapeResult));
      for(It.Reset(); It.More(); It.Next()){
	Standard_Integer indsol = It.Key();
	const TopoDS_Shape& curshape = DStr.Shape(indsol);
	TopTools_ListIteratorOfListOfShape 
	  its = myCoup->Merged(curshape,TopAbs_IN);
	if(!its.More()) B1.Add(myShapeResult,curshape);
	else {
	  //If the old type of Shape is Shell, Shell is placed instead of Solid, 
          //However there is a problem for compound of open Shell.
	  while (its.More()) {
	    const TopAbs_ShapeEnum letype = curshape.ShapeType();
	    if (letype == TopAbs_SHELL){
	      TopExp_Explorer expsh2(its.Value(),TopAbs_SHELL);
	      const TopoDS_Shape& cursh = expsh2.Current();
	      TopoDS_Shape tt = cursh;
	      B1.Add(myShapeResult,cursh);
	      its.Next();
	    }
	    else {
	      B1.Add(myShapeResult,its.Value());
	      its.Next();
	    }
	  }
	}
      }
      }
      else {
       done=Standard_False;
       B1.MakeCompound(TopoDS::Compound(badShape));
      for(It.Reset(); It.More(); It.Next()){
	Standard_Integer indsol = It.Key();
	const TopoDS_Shape& curshape = DStr.Shape(indsol);
	TopTools_ListIteratorOfListOfShape 
	  its = myCoup->Merged(curshape,TopAbs_IN);
	if(!its.More()) B1.Add(badShape,curshape);
	else {
	  while (its.More()) { 
	    B1.Add(badShape,its.Value());
	    its.Next();
	  }
	}
      }
      }
#ifdef OCCT_DEBUG //perf 
      ChFi3d_ResultChron(cl_reconstruction ,t_reconstruction);
      ChFi3d_InitChron(cl_setregul);
#endif
      
      // Regularities are coded after cutting.
      SetRegul();
      
      
#ifdef OCCT_DEBUG //perf 
 ChFi3d_ResultChron(cl_setregul ,t_setregul);
#endif
    }
  }
#ifdef OCCT_DEBUG //perf 
  ChFi3d_ResultChron(cl_total,t_total);
#endif
      
  
  // display of time for perfs 
  
#ifdef OCCT_DEBUG
  if(ChFi3d_GettraceCHRON()){
    cout<<endl; 
    cout<<"COMPUTE: temps total "<<t_total<<"s  dont :"<<endl;
    cout<<"- Init + ExtentAnalyse "<<t_extent<<"s"<<endl;  
    cout<<"- PerformSetOfSurf "<<t_perfsetofsurf<<"s"<<endl;
    cout<<"- PerformFilletOnVertex "<<t_perffilletonvertex<<"s"<<endl; 
    cout<<"- FilDS "<<t_filds<<"s"<<endl; 
    cout<<"- Reconstruction "<<t_reconstruction<<"s"<<endl;
    cout<<"- SetRegul "<<t_setregul<<"s"<<endl<<endl;

    cout<<endl;
    cout <<"temps PERFORMSETOFSURF "<<t_perfsetofsurf <<"s  dont : "<<endl;
    cout <<"- SetofKPart "<<t_perfsetofkpart<<"s"<<endl;
    cout <<"- SetofKGen "<< t_perfsetofkgen <<"s"<<endl;
    cout <<"- MakeExtremities "<<t_makextremities<<"s"<<endl<<endl;
    
    
    cout <<"temps SETOFKGEN "<< t_perfsetofkgen<<"s dont : "<<endl;
    cout<<"- PerformSurf "<<t_performsurf<<"s"<<endl;
    cout<<"- starsol "<< t_startsol <<"s"<<endl<<endl;
    
    cout<<"temps PERFORMSURF "<<t_performsurf<<"s  dont : "  << endl;
    cout<<"- computedata "<<t_computedata<<"s"<<endl;
    cout<<"- completedata "<<t_completedata<<"s"<<endl<<endl;
    
    
    cout<<"temps PERFORMFILLETVERTEX "<<t_perffilletonvertex <<"s dont : "  << endl;
    cout<<"- PerformOneCorner "<<t_perform1corner<<"s"<<endl;
    cout<<"- PerformIntersectionAtEnd "<<t_performatend<<"s"<<endl;
    cout<<"- PerformTwoCorner "<<t_perform2corner<<"s"<<endl;
    cout<<"- PerformThreeCorner "<<t_perform3corner<<"s"<<endl;
    cout<<"- PerformMoreThreeCorner "<<t_performmore3corner<<"s"<<endl<<endl;
    
    
    cout<<"temps PerformOneCorner "<<t_perform1corner<<"s dont:"<<endl;
    cout<<"- temps condition if (same) "<<t_same << "s "<<endl; 
    cout<<"- temps condition if (inter) "<<t_inter<<"s " <<endl;
    cout<<"- temps condition if (same inter) "<<t_sameinter<<"s " <<endl<<endl;
    
    cout<<"temps PerformTwocorner "<<t_perform2corner<<"s  dont:"<<endl;
    cout<<"- temps initialisation "<< t_t2cornerinit<<"s"<<endl;
    cout<<"- temps PerformTwoCornerbyInter "<<t_perf2cornerbyinter<<"s"<<endl;
    cout<<"- temps ChFiKPart_ComputeData "<<t_chfikpartcompdata <<"s"<<endl;
    cout<<"- temps cheminement "<<t_cheminement<<"s"<<endl;
    cout<<"- temps remplissage "<<t_remplissage<<"s"<<endl;
    cout<<"- temps mise a jour stripes  "<<t_t2cornerDS<<"s"<<endl<<endl;
    
    cout<<" temps PerformThreecorner "<<t_perform3corner<<"s  dont:"<<endl;
    cout<<"- temps initialisation "<< t_t3cornerinit<<"s"<<endl;
    cout<<"- temps cas spherique  "<<t_spherique<<"s"<<endl;
    cout<<"- temps cas torique  "<<t_torique<<"s"<<endl;
    cout<<"- temps notfilling "<<t_notfilling<<"s"<<endl;
    cout<<"- temps filling "<<t_filling<<"s"<<endl;
    cout<<"- temps mise a jour stripes  "<<t_t3cornerDS<<"s"<<endl<<endl;
    
    cout<<"temps PerformMore3Corner "<<t_performmore3corner<<"s dont:"<<endl;
    cout<<"-temps plate "<<t_plate << "s "<<endl; 
    cout<<"-temps approxplate "<<t_approxplate<<"s " <<endl;
    cout<<"-temps batten "<< t_batten<<"s " <<endl<<endl;
    
    cout <<"TEMPS DIVERS "<<endl;
    cout<<"-temps ChFi3d_sameparameter "<<t_sameparam<<"s"<<endl<<endl;
  }
#endif
  //
  // Inspect the new faces to provide sameparameter 
  // if it is necessary
  if (IsDone())
  {
    Standard_Real SameParTol = Precision::Confusion();
    Standard_Integer aNbSurfaces, iF;
    TopTools_ListIteratorOfListOfShape aIt;
    //
    aNbSurfaces=myDS->NbSurfaces();
    
    for (iF=1; iF<=aNbSurfaces; ++iF) {
      const TopTools_ListOfShape& aLF=myCoup->NewFaces(iF);
      aIt.Initialize(aLF);
      for (; aIt.More(); aIt.Next()) {
	const TopoDS_Shape& aF=aIt.Value();
	BRepLib::SameParameter(aF, SameParTol, Standard_True);
	ShapeFix::SameParameter(aF, Standard_False, SameParTol);
      }
    }
  }
}

//=======================================================================
//function : PerformSingularCorner
//purpose  : Load vertex and degenerated edges.
//=======================================================================

void ChFi3d_Builder::PerformSingularCorner
(const Standard_Integer Index){
  ChFiDS_ListIteratorOfListOfStripe It;
  Handle(ChFiDS_Stripe) stripe;
  TopOpeBRepDS_DataStructure&  DStr = myDS->ChangeDS();
  const TopoDS_Vertex& Vtx = myVDataMap.FindKey(Index);
  
  Handle(ChFiDS_SurfData) Fd;
  Standard_Integer i, Icurv;
  Standard_Integer Ivtx = 0;
  for (It.Initialize(myVDataMap(Index)), i=0; It.More(); It.Next(),i++){
    stripe = It.Value(); 
    // SurfData concerned and its CommonPoints,
    Standard_Integer sens = 0;
    Standard_Integer num = ChFi3d_IndexOfSurfData(Vtx,stripe,sens);
    Standard_Boolean isfirst = (sens == 1);
    Fd =  stripe->SetOfSurfData()->Sequence().Value(num);
    const ChFiDS_CommonPoint& CV1 = Fd->Vertex(isfirst,1);
    const ChFiDS_CommonPoint& CV2 = Fd->Vertex(isfirst,2);
    // Is it always degenerated ?
    if ( CV1.Point().IsEqual( CV2.Point(), 0) ) { 
      // if yes the vertex is stored in the stripe
      // and the edge at end is created
      if (i==0) Ivtx = ChFi3d_IndexPointInDS(CV1, DStr);
      Standard_Real tolreached;
      Standard_Real Pardeb, Parfin; 
      gp_Pnt2d VOnS1, VOnS2;
      Handle(Geom_Curve) C3d;
      Handle(Geom2d_Curve) PCurv;
      TopOpeBRepDS_Curve Crv;
      if (isfirst) {
	VOnS1 = Fd->InterferenceOnS1().PCurveOnSurf()->
		    Value(Fd->InterferenceOnS1().FirstParameter());
	VOnS2 = Fd->InterferenceOnS2().PCurveOnSurf()->
		     Value(Fd->InterferenceOnS2().FirstParameter());
      }
      else {
	VOnS1 = Fd->InterferenceOnS1().PCurveOnSurf()->
		    Value(Fd->InterferenceOnS1().LastParameter());
	VOnS2 = Fd->InterferenceOnS2().PCurveOnSurf()->
		     Value(Fd->InterferenceOnS2().LastParameter());
      }
      
      ChFi3d_ComputeArete(CV1, VOnS1,
			  CV2, VOnS2,
			  DStr.Surface(Fd->Surf()).Surface(),
			  C3d, PCurv,
			  Pardeb,Parfin,tolapp3d,tolapp2d,tolreached,0);
      Crv = TopOpeBRepDS_Curve(C3d,tolreached);
      Icurv = DStr.AddCurve(Crv);

      stripe->SetCurve(Icurv, isfirst);
      stripe->SetParameters(isfirst, Pardeb,Parfin);
      stripe->ChangePCurve(isfirst) = PCurv;
      stripe->SetIndexPoint(Ivtx, isfirst, 1);
      stripe->SetIndexPoint(Ivtx, isfirst, 2);	  
    }
  }  
}

//=======================================================================
//function : PerformFilletOnVertex
//purpose  : 
//=======================================================================

void ChFi3d_Builder::PerformFilletOnVertex
(const Standard_Integer Index){
  
  ChFiDS_ListIteratorOfListOfStripe It;
  Handle(ChFiDS_Stripe) stripe;
  Handle(ChFiDS_Spine) sp;
  const TopoDS_Vertex& Vtx = myVDataMap.FindKey(Index);
  
  Handle(ChFiDS_SurfData) Fd;
  Standard_Integer i;
  Standard_Boolean nondegenere = Standard_True;
  Standard_Boolean toujoursdegenere = Standard_True; 
  Standard_Boolean isfirst = Standard_False;
  for (It.Initialize(myVDataMap(Index)), i=0; It.More(); It.Next(),i++){
    stripe = It.Value(); 
    sp = stripe->Spine();
    // SurfData and its CommonPoints,
    Standard_Integer sens = 0;
    Standard_Integer num = ChFi3d_IndexOfSurfData(Vtx,stripe,sens);
    isfirst = (sens == 1);
    Fd =  stripe->SetOfSurfData()->Sequence().Value(num);
    const ChFiDS_CommonPoint& CV1 = Fd->Vertex(isfirst,1);
    const ChFiDS_CommonPoint& CV2 = Fd->Vertex(isfirst,2);
    // Is it always degenerated ?
    if ( CV1.Point().IsEqual( CV2.Point(), 0) )  
      nondegenere = Standard_False;
    else  toujoursdegenere = Standard_False;
  }
  
  // calcul du nombre de faces = nombre d'aretes
/*  TopTools_ListIteratorOfListOfShape ItF,JtF,ItE;
  Standard_Integer nbf = 0, jf = 0;
  for (ItF.Initialize(myVFMap(Vtx)); ItF.More(); ItF.Next()){
    jf++;
    Standard_Integer kf = 1;
    const TopoDS_Shape& cur = ItF.Value();
    for (JtF.Initialize(myVFMap(Vtx)); JtF.More() && (kf < jf); JtF.Next(), kf++){
      if(cur.IsSame(JtF.Value())) break;
    }
    if(kf == jf) nbf++;
  }
  Standard_Integer nba=myVEMap(Vtx).Extent();
  for (ItE.Initialize(myVEMap(Vtx)); ItE.More(); ItE.Next()){
    const TopoDS_Edge& cur = TopoDS::Edge(ItE.Value());
    if (BRep_Tool::Degenerated(cur)) nba--;
  }
  nba=nba/2;*/
  Standard_Integer nba = ChFi3d_NumberOfSharpEdges(Vtx, myVEMap, myEFMap);

  if (nondegenere) { // Normal processing
    switch (i) {
    case 1 : 
      {
	if(sp->Status(isfirst) == ChFiDS_FreeBoundary) return; 
	if(nba>3) {
#ifdef OCCT_DEBUG //perf    
	  ChFi3d_InitChron(cl_performatend);
#endif
	  PerformIntersectionAtEnd(Index);
#ifdef OCCT_DEBUG
	  ChFi3d_ResultChron(cl_performatend,t_performatend);
#endif 
	}
	else { 
#ifdef OCCT_DEBUG //perf    
	  ChFi3d_InitChron(cl_perform1corner);
#endif
          if (MoreSurfdata(Index))
             PerformMoreSurfdata(Index);
	  else PerformOneCorner(Index);
#ifdef OCCT_DEBUG //perf 
	  ChFi3d_ResultChron(cl_perform1corner,t_perform1corner);
#endif  
	}
      }
      break;
    case 2 : 
      {
	if(nba>3){
#ifdef OCCT_DEBUG //perf 
	  ChFi3d_InitChron(cl_performmore3corner);
#endif
	  PerformMoreThreeCorner(Index, i);
#ifdef OCCT_DEBUG //perf 
	  ChFi3d_ResultChron(cl_performmore3corner,t_performmore3corner);
#endif
	}
	else { 
#ifdef OCCT_DEBUG //perf 
	  ChFi3d_InitChron(cl_perform2corner);
#endif
	  PerformTwoCorner(Index);
#ifdef OCCT_DEBUG //perf 
	  ChFi3d_ResultChron(cl_perform2corner,t_perform2corner);
#endif
	}
      }
      break;
    case 3 : 
      {
	if(nba>3){
#ifdef OCCT_DEBUG //perf 
	  ChFi3d_InitChron(cl_performmore3corner);
#endif
	  PerformMoreThreeCorner(Index, i);
#ifdef OCCT_DEBUG //perf 
	  ChFi3d_ResultChron(cl_performmore3corner,t_performmore3corner);
#endif
	}
	else {
#ifdef OCCT_DEBUG //perf 
	  ChFi3d_InitChron(cl_perform3corner);
#endif
	  PerformThreeCorner(Index);
#ifdef OCCT_DEBUG //perf 
	  ChFi3d_ResultChron(cl_perform3corner,t_perform3corner);
#endif
	}
      }
      break;
      default : {
#ifdef OCCT_DEBUG //perf 
	ChFi3d_InitChron(cl_performmore3corner);
#endif
	PerformMoreThreeCorner(Index, i);
#ifdef OCCT_DEBUG //perf 
	ChFi3d_ResultChron(cl_performmore3corner,t_performmore3corner);
#endif
      }
    }
  }
  else { // Single case processing
    if (toujoursdegenere) PerformSingularCorner(Index);
    else                  PerformMoreThreeCorner(Index, i);//Last chance...
  }              
}


//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void  ChFi3d_Builder::Reset()
{
  done = Standard_False;
  myVDataMap.Clear();
  myRegul.Clear();
  myEVIMap.Clear();
  badstripes.Clear();
  badvertices.Clear();

  ChFiDS_ListIteratorOfListOfStripe itel;
  for (itel.Initialize(myListStripe); itel.More(); ){
    if(!itel.Value()->Spine().IsNull()){
      itel.Value()->Reset();
      itel.Next();
    }
    else myListStripe.Remove(itel);
  }
}

//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& ChFi3d_Builder::Generated(const TopoDS_Shape& EouV)
{
  myGenerated.Clear();
  if(EouV.IsNull()) return myGenerated;
  if(EouV.ShapeType() != TopAbs_EDGE &&
     EouV.ShapeType() != TopAbs_VERTEX) return myGenerated;
  if(myEVIMap.IsBound(EouV)) {
    const TColStd_ListOfInteger& L = myEVIMap.Find(EouV);
    TColStd_ListIteratorOfListOfInteger IL;
    for(IL.Initialize(L); IL.More(); IL.Next()){
      Standard_Integer I = IL.Value();
      const TopTools_ListOfShape& LS =  myCoup->NewFaces(I);
      TopTools_ListIteratorOfListOfShape ILS;
      for(ILS.Initialize(LS); ILS.More(); ILS.Next()){
	myGenerated.Append(ILS.Value());
      }
    }
  }
  return myGenerated;
}


