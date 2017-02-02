// Created on: 1994-03-29
// Created by: Isabelle GRIGNON
// Copyright (c) 1994-1999 Matra Datavision
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


#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_HCurveOnSurface.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_HCurve.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBlend_CSCircular.hxx>
#include <BRepBlend_Line.hxx>
#include <BRepLProp_CLProps.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <BSplCLib.hxx>
#include <ChFi3d.hxx>
#include <ChFi3d_Builder_0.hxx>
#include <ChFi3d_FilBuilder.hxx>
#include <ChFiDS_CommonPoint.hxx>
#include <ChFiDS_FaceInterference.hxx>
#include <ChFiDS_FilSpine.hxx>
#include <ChFiDS_HData.hxx>
#include <ChFiDS_HElSpine.hxx>
#include <ChFiDS_ListIteratorOfListOfStripe.hxx>
#include <ChFiDS_Regul.hxx>
#include <ChFiDS_SequenceOfSurfData.hxx>
#include <ChFiDS_Spine.hxx>
#include <ChFiDS_State.hxx>
#include <ChFiDS_Stripe.hxx>
#include <ChFiDS_SurfData.hxx>
#include <ChFiKPart_ComputeData.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomFill_ConstrainedFilling.hxx>
#include <GeomFill_SimpleBound.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <IntCurveSurface_HInter.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <IntSurf_LineOn2S.hxx>
#include <IntSurf_Transition.hxx>
#include <IntSurf_TypeTrans.hxx>
#include <Law_Function.hxx>
#include <Law_Linear.hxx>
#include <math_Vector.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_NotImplemented.hxx>
#include <StdFail_NotDone.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TopAbs.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_ListOfInterference.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#ifdef DRAW
#include <DrawTrSurf.hxx>
#endif
#ifdef OCCT_DEBUG
#include <Geom_TrimmedCurve.hxx>
extern Standard_Boolean ChFi3d_GettraceDRAWSPINE();
extern Standard_Boolean ChFi3d_GetcontextFORCEFILLING();
#include <OSD_Chronometer.hxx>

extern Standard_Real  t_t2cornerinit ,t_perf2cornerbyinter,t_chfikpartcompdata,
                     t_cheminement,t_remplissage,t_t2cornerDS;
extern void ChFi3d_InitChron(OSD_Chronometer& ch);
extern void ChFi3d_ResultChron(OSD_Chronometer & ch,Standard_Real& time);
#endif

//=======================================================================
//function : ToricRotule
//purpose  : Test if it is a particular case of torus routine.
//           Three planes with two constant incident fillets
//           of the same radius and the third face perpendicular to  
//           two others are required.
//=======================================================================

static Standard_Boolean ToricRotule(const BRepAdaptor_Surface& fac,
				    const BRepAdaptor_Surface& s1,
				    const BRepAdaptor_Surface& s2,
				    const Handle(ChFiDS_Stripe)& c1,
				    const Handle(ChFiDS_Stripe)& c2)

{
  Standard_Real tolesp = 1.e-7;

  Handle(ChFiDS_FilSpine) sp1=Handle(ChFiDS_FilSpine)::DownCast(c1->Spine()); 
  Handle(ChFiDS_FilSpine) sp2=Handle(ChFiDS_FilSpine)::DownCast(c2->Spine()); 
  if(sp1.IsNull() || sp2.IsNull()) return Standard_False;
  if (!sp1->IsConstant() || !sp2->IsConstant()) 
    return Standard_False;
  if ((fac.GetType() != GeomAbs_Plane) ||
      (s1.GetType() != GeomAbs_Plane) ||
      (s2.GetType() != GeomAbs_Plane)) return Standard_False;
  gp_Dir df = fac.Plane().Position().Direction();
  gp_Dir ds1 = s1.Plane().Position().Direction();
  gp_Dir ds2 = s2.Plane().Position().Direction();
  if ( Abs(df.Dot(ds1)) >= tolesp || Abs(df.Dot(ds2)) >= tolesp ) 
    return Standard_False;
  Standard_Real r1 = sp1->Radius();
  Standard_Real r2 = sp2->Radius();
  if(Abs(r1 - r2) >= tolesp) return Standard_False;
  return Standard_True;
}

static void RemoveSD(Handle(ChFiDS_Stripe)& Stripe,
		      const Standard_Integer num1,
                      const Standard_Integer num2 )
{
  ChFiDS_SequenceOfSurfData& Seq = 
    Stripe->ChangeSetOfSurfData()->ChangeSequence();
  if(Seq.IsEmpty()) return;
  if (num1==num2) 
    Seq.Remove(num1);
  else
    Seq.Remove(num1,num2);
}


//=======================================================================
//function : PerformTwoCorner
//purpose  : 
//=======================================================================

void ChFi3d_FilBuilder::PerformTwoCorner(const Standard_Integer Index)
{
#ifdef OCCT_DEBUG
  OSD_Chronometer ch;
  ChFi3d_InitChron(ch); // init perf initialisation 
#endif 
  
  done = 0;
  const TopoDS_Vertex& Vtx = myVDataMap.FindKey(Index);
  TopOpeBRepDS_DataStructure& DStr = myDS->ChangeDS();
  ChFiDS_ListIteratorOfListOfStripe It;
  It.Initialize(myVDataMap(Index));
  Handle(ChFiDS_Stripe)  st1,st2;
  Standard_Integer Sens1,Sens2;
  Standard_Integer Isd1,Isd2,i1,i2;
  Handle(ChFiDS_SurfData) sd1,sd2;
  ChFiDS_SequenceOfSurfData SeqFil1,SeqFil2;
  Handle(Geom_Surface) surf1,surf2;
  Standard_Boolean  OkinterCC,Okvisavis,SameSide;
  Standard_Integer IFaCo1,IFaCo2;
  Standard_Real UIntPC1,UIntPC2;
  TopoDS_Face FaCo;
  TopoDS_Edge E1,E2,E;
  TopoDS_Vertex V1,V2;
//  gp_Pnt P1,P2;
  Standard_Integer nbsurf1,nbsurf2,deb1,fin1,deb2,fin2;
  Standard_Real parE1,parE2;
  //Necessary information on fillets is extracted 
  //------------------------------------------------------
  
  //the first
  //----------
 
  st1 = It.Value(); 
  Isd1 = ChFi3d_IndexOfSurfData(Vtx,st1,Sens1);
  
  
  //the second
  //----------
  It.Next();
  st2 = It.Value();
  if(st2 == st1) {
    Sens2 = -1;
    Isd2 = st2->SetOfSurfData()->Length();
  }
  else{ Isd2 = ChFi3d_IndexOfSurfData(Vtx,st2,Sens2); }
  
  // If two edges to rounded are tangent GeomPlate is called

  if (Sens1==1)  E1= st1->Spine()->Edges(1);
  else E1= st1->Spine()->Edges( st1->Spine()->NbEdges());
  
  if (Sens2==1)  E2= st2->Spine()->Edges(1);
  else E2= st2->Spine()->Edges( st2->Spine()->NbEdges());
  
  BRepAdaptor_Curve BCurv1(E1);
  BRepAdaptor_Curve BCurv2(E2);
  parE1=BRep_Tool::Parameter(Vtx,E1);
  parE2=BRep_Tool::Parameter(Vtx,E2);
  BRepLProp_CLProps CL1(BCurv1,parE1 , 1, 1.e-4);
  BRepLProp_CLProps CL2(BCurv2,parE2 , 1, 1.e-4);
  gp_Dir dir1,dir2 ;
  CL1.Tangent(dir1);
  CL2.Tangent(dir2);
  if (Sens1==-1)  dir1.Reverse();
  if (Sens2==-1)  dir2.Reverse();
  Standard_Real ang1;
  ang1=Abs(dir1.Angle(dir2));
  if (ang1<M_PI/180.) {
    PerformMoreThreeCorner(Index,2);
    done=1;
    return;
  }

  OkinterCC = ChFi3d_IsInFront(DStr,st1,st2,Isd1,Isd2,Sens1,Sens2,
			       UIntPC1,UIntPC2,FaCo,SameSide,
			       IFaCo1,IFaCo2,Okvisavis,Vtx,Standard_True);

  Standard_Boolean trouve=Standard_False;  
  if (!Okvisavis) {

  
// one is not limited to the first or the last surfdata 
// to find the opposing data
    nbsurf1=st1->SetOfSurfData()->Length();
    nbsurf2=st2->SetOfSurfData()->Length();
    deb1=1; 
    deb2=1;
    fin1=1;
    fin2=1;
    if (nbsurf1!=1) {
      if (Sens1==1) {
	deb1=1;
	fin1=2;
      }
      else  {
	deb1=nbsurf1-1;
	fin1=nbsurf1;
      }
    }
    if (nbsurf2!=1) {
      if (Sens2==1 ) {
	deb2=1;
	fin2=2;
      }
      else {
	deb2=nbsurf2-1;
	fin2=nbsurf2;
      }
    }
  
    for (i1=deb1;i1<=fin1 &&!trouve;i1++) {
      Isd1=i1;
      for (i2=deb2;i2<=fin2 &&!trouve;i2++) {
	Isd2=i2;
 
        OkinterCC = ChFi3d_IsInFront(DStr,st1,st2,Isd1,Isd2,Sens1,Sens2,
			       UIntPC1,UIntPC2,FaCo,SameSide,
			       IFaCo1,IFaCo2,Okvisavis,Vtx,Standard_True);
        trouve=Okvisavis;
      }
    } 
    if (!trouve){
      PerformMoreThreeCorner(Index,2);
      done=1;
      return;
    }
    else {
      if (Sens1==1 && Isd1!=1) RemoveSD(st1,1,1);
      if (Sens1!=1 && Isd1!=nbsurf1) RemoveSD(st1,fin1,fin1);
      if (Sens2==1 && Isd2!=1) RemoveSD(st2,1,1);
      if (Sens2!=1 && Isd2!=nbsurf2) RemoveSD(st2,fin2,fin2);

    }
    Isd1=ChFi3d_IndexOfSurfData(Vtx,st1,Sens1);
    Isd2=ChFi3d_IndexOfSurfData(Vtx,st2,Sens2);
  }
   // throw StdFail_NotDone("TwoCorner : no common face");
  Standard_Integer IFaArc1 = 3-IFaCo1, IFaArc2 = 3-IFaCo2;
  SeqFil1 = st1->ChangeSetOfSurfData()->ChangeSequence();
  SeqFil2 = st2->ChangeSetOfSurfData()->ChangeSequence();
  sd1 = SeqFil1.ChangeValue(Isd1);
  surf1 = DStr.Surface(sd1->Surf()).Surface();
  sd2 = SeqFil2.ChangeValue(Isd2);
  surf2 = DStr.Surface(sd2->Surf()).Surface();
  TopAbs_Orientation OFaCo = FaCo.Orientation(); 
  // The concavities are analyzed and the opposite face and the
  // eventual intersection of 2 pcurves on this face are found.
  
  ChFiDS_State Stat1,Stat2;
  Standard_Boolean isfirst1 = (Sens1 == 1);
  Standard_Boolean isfirst2 = (Sens2 == 1);
  Stat1 = st1->Spine()->Status(isfirst1);
  Stat2 = st2->Spine()->Status(isfirst2);
  Standard_Boolean c1biseau = (Stat1 == ChFiDS_AllSame); 
  Standard_Boolean c1rotule = (Stat1 == ChFiDS_OnSame && Stat2 == ChFiDS_OnSame);
  
  // It is checked if the fillets have a commonpoint on a common arc. 
  // This edge is the pivot of the bevel or the knee.
  
  ChFiDS_CommonPoint& CP1 = sd1->ChangeVertex(isfirst1,IFaArc1);
  ChFiDS_CommonPoint& CP2 = sd2->ChangeVertex(isfirst2,IFaArc2);
  
  Standard_Boolean resetcp1 = 0;
  Standard_Boolean resetcp2 = 0;
  
  TopoDS_Edge pivot;
  Standard_Boolean yapiv = Standard_False;
  if(CP1.IsOnArc()) pivot = CP1.Arc();
  else {
    PerformMoreThreeCorner(Index,2);
    done=1;
    return;
  }
  if(CP1.IsOnArc()&& CP2.IsOnArc()){
    yapiv = (pivot.IsSame(CP2.Arc()));
  }
  Handle(BRepAdaptor_HCurve) Hpivot;
  Standard_Boolean sameparam = Standard_False;
  Standard_Real parCP1 = 0., parCP2 = 0.;
  if(yapiv) {
    Hpivot = new BRepAdaptor_HCurve(pivot);
    parCP1 = CP1.ParameterOnArc();
    parCP2 = CP2.ParameterOnArc();
    gp_Pnt tst1 = Hpivot->Value(parCP1);
    gp_Pnt tst2 = Hpivot->Value(parCP2);
    sameparam = tst1.Distance(tst2) <= tolesp;
  }
  Handle(BRepAdaptor_HSurface) HFaCo = new BRepAdaptor_HSurface();
  Handle(BRepAdaptor_HSurface) HFaPiv;
  Handle(BRepAdaptor_HSurface) HBRS1 = new BRepAdaptor_HSurface();
  Handle(BRepAdaptor_HSurface) HBRS2 = new BRepAdaptor_HSurface();
  
  BRepAdaptor_Surface& BRS1 = HBRS1->ChangeSurface();
  BRepAdaptor_Surface& BRS2 = HBRS2->ChangeSurface();
  BRepAdaptor_Surface& BRFaCo = HFaCo->ChangeSurface();
  BRFaCo.Initialize(FaCo);
  
  TopoDS_Face FF1,FF2,F,FaPiv;
  TopAbs_Orientation pctrans = TopAbs_FORWARD ;
  Handle(Geom2d_BSplineCurve) PCurveOnPiv;
  FF1 = TopoDS::Face(DStr.Shape(sd1->Index(IFaArc1)));
  FF2 = TopoDS::Face(DStr.Shape(sd2->Index(IFaArc2)));
  if (FF1.IsNull()||FF2.IsNull()) 
  {PerformMoreThreeCorner(Index,2);
   done=1;
   return;
  }
  BRS1.Initialize(FF1);
  BRS2.Initialize(FF2);
  
  if(yapiv ) {
    TopTools_ListIteratorOfListOfShape Kt;
    Standard_Boolean ok1 = Standard_False, ok2 = Standard_False;
    for (Kt.Initialize(myEFMap(pivot)); Kt.More(); Kt.Next()){
      F = TopoDS::Face(Kt.Value());
      if(!ok1 && FF1.IsSame(F)){
	ok1 = Standard_True;
      }
      if(!ok2 && FF2.IsSame(F)){
	ok2 = Standard_True;
      }
    }
    if(!ok1 || !ok2){
      PerformMoreThreeCorner(Index,2);
      done=1;
      return;
    }
  }
  
#ifdef OCCT_DEBUG
  ChFi3d_ResultChron(ch ,t_t2cornerinit);//result perf initialisation 
#endif 
  
  //bevel
  //------
  ChFiDS_CommonPoint cp11,cp12,cp21,cp22;
  ChFiDS_FaceInterference intf11,intf12,intf21,intf22;

  if(c1biseau){
#ifdef OCCT_DEBUG
    ChFi3d_InitChron(ch); // init perf PerformTwoCornerbyInter
#endif 
    
    done = PerformTwoCornerbyInter(Index);
    
#ifdef OCCT_DEBUG
    ChFi3d_ResultChron(ch , t_perf2cornerbyinter); // result perf  PerformTwoCornerbyInter
#endif 
   
    if (!done){
      PerformMoreThreeCorner(Index,2);
      done=1;
      return;
    }
  } 
  else if(c1rotule){//save.
    cp11 = sd1->Vertex(isfirst1,1);
    cp12 = sd1->Vertex(isfirst1,2);
    cp21 = sd2->Vertex(isfirst2,1);
    cp22 = sd2->Vertex(isfirst2,2);
    intf11 = sd1->InterferenceOnS1();
    intf12 = sd1->InterferenceOnS2();
    intf21 = sd2->InterferenceOnS1();
    intf22 = sd2->InterferenceOnS2();
#ifdef OCCT_DEBUG
    ChFi3d_InitChron(ch); // init perf PerformTwoCornerbyInter
#endif 
      
    done = PerformTwoCornerbyInter(Index);
      
#ifdef OCCT_DEBUG
      ChFi3d_ResultChron(ch , t_perf2cornerbyinter); // result perf  PerformTwoCornerbyInter
#endif 
    if (!done) {
      // restore
      sd1->ChangeVertex(isfirst1,1) = cp11;
      sd1->ChangeVertex(isfirst1,2) = cp12;
      sd2->ChangeVertex(isfirst2,1) = cp21;
      sd2->ChangeVertex(isfirst2,2) = cp22;
      sd1->ChangeInterferenceOnS1() = intf11;
      sd1->ChangeInterferenceOnS2() = intf12;
      sd2->ChangeInterferenceOnS1() = intf21;
      sd2->ChangeInterferenceOnS2() = intf22;
      done = 0;
    }
  }
  
  if(!c1biseau && !done){
    //new cornerdata is created
    //-------------------------------
    Handle(ChFiDS_Stripe) corner = new ChFiDS_Stripe();
    Handle(ChFiDS_HData)& cornerset = corner->ChangeSetOfSurfData();
    cornerset = new ChFiDS_HData();
    Handle(ChFiDS_SurfData) coin = new ChFiDS_SurfData();
    cornerset->Append(coin);
    
    if (SameSide) {
      if(ToricRotule(BRFaCo,BRS1,BRS2,st1,st2)){
	// Direct construction.
	// ---------------------
	
	Standard_Integer bid;
	TopAbs_Orientation ori = OFaCo;
	TopAbs_Orientation oriS = st1->Orientation(IFaCo1);
	TopAbs_Orientation OFF1 = FF1.Orientation(); 
	TopAbs_Orientation oriSFF1 = st1->Orientation(IFaArc1);
	bid = 1;
	bid = ChFi3d::NextSide(ori,OFF1,oriS,oriSFF1,bid);
	TopAbs_Orientation op1 = TopAbs_FORWARD,op2 = TopAbs_FORWARD;
	if(yapiv) bid = ChFi3d::ConcaveSide(BRS1,BRS2,pivot,op1,op2);
	op1 = TopAbs::Reverse(op1);
	op2 = TopAbs::Reverse(op2);
#ifdef OCCT_DEBUG
	ChFi3d_InitChron(ch);// init perf ChFiKPart_ComputeData 
#endif 
	Standard_Real radius = 
	  Handle(ChFiDS_FilSpine)::DownCast(st1->Spine())->Radius();
	done = ChFiKPart_ComputeData::ComputeCorner(DStr,coin,HFaCo,HBRS1,HBRS2,
						    OFaCo,ori,op1,op2,radius);
#ifdef OCCT_DEBUG
	ChFi3d_ResultChron(ch , t_chfikpartcompdata);//result perf ChFiKPart_ComputeData 
#endif 
      }
      else {
	// Construction by filling remplissage
	// ----------------------------
	Standard_Real  uPCArc1,  uPCArc2;
	gp_Pnt2d p2da1,p2df1,p2da2,p2df2,p2dfac1,p2dfac2;
	gp_Vec2d v2dfac1,v2dfac2;
	Handle(GeomFill_Boundary) B1,B2,Bpiv,Bfac;
	uPCArc1 = sd1->Interference(IFaArc1).Parameter(isfirst1);
	p2da1 = sd1->Interference(IFaArc1).PCurveOnSurf()->Value(uPCArc1);
	p2df1 = sd1->Interference(IFaCo1).PCurveOnSurf()->Value(uPCArc1);
	sd1->Interference(IFaCo1).PCurveOnFace()->D1(uPCArc1,p2dfac1,v2dfac1);
	uPCArc2 = sd2->Interference(IFaArc2).Parameter(isfirst2);
	p2da2 = sd2->Interference(IFaArc2).PCurveOnSurf()->Value(uPCArc2);
	p2df2 = sd2->Interference(IFaCo2).PCurveOnSurf()->Value(uPCArc2);
	sd2->Interference(IFaCo2).PCurveOnFace()->D1(uPCArc2,p2dfac2,v2dfac2);
#ifdef OCCT_DEBUG
	ChFi3d_InitChron(ch ); // init perf filling
#endif 
	B1 = ChFi3d_mkbound(surf1,p2df1,p2da1,tolesp,2.e-4);
	B2 = ChFi3d_mkbound(surf2,p2df2,p2da2,tolesp,2.e-4);
	Handle(Geom2d_Curve) PCurveOnFace;
	Bfac = ChFi3d_mkbound(HFaCo,PCurveOnFace,Sens1,p2dfac1,v2dfac1,
			      Sens2,p2dfac2,v2dfac2,tolesp,2.e-4);
	GeomFill_ConstrainedFilling fil(8,20);
	if(sameparam) {
	  fil.Init(Bfac,B2,B1,1);
	}
	else {
	  Handle(Adaptor3d_HCurve) HPivTrim = Hpivot->ChangeCurve().
	    Trim(Min(parCP1,parCP2),Max(parCP1,parCP2),tolesp);
	  Bpiv = new GeomFill_SimpleBound(HPivTrim,tolesp,2.e-4);
	  fil.Init(Bfac,B2,Bpiv,B1,1);
	  BRepAdaptor_Curve2d pcpivot;
	  gp_Vec dArc,dcf;
	  gp_Pnt bidon;
	  Hpivot->D1(parCP1,bidon,dArc);
	  Standard_Real fb1,lb1;
	  B1->Bounds(fb1,lb1);
	  B1->D1(lb1,bidon,dcf);
	  Standard_Boolean pivotverslebas = dArc.Dot(dcf) <= 0.; 
	  Standard_Boolean pcfalenvers = (parCP1 > parCP2);
	  if((pivotverslebas && !pcfalenvers)||
	     (!pivotverslebas && pcfalenvers)) {
	    FaPiv = FF2;
	    HFaPiv = HBRS2;
	    resetcp2 = 1;
	  }
	  else {
	    FaPiv = FF1;
	    HFaPiv = HBRS1;
	    resetcp1 = 1;
	  }
	  FaPiv.Orientation(TopAbs_FORWARD);
	  pcpivot.Initialize(pivot,FaPiv);
	  TopExp_Explorer Expl;
	  for(Expl.Init(FaPiv,TopAbs_EDGE); Expl.More(); Expl.Next()){
	    if(Expl.Current().IsSame(pivot)) {
	      pctrans = Expl.Current().Orientation();
	      break;
	    }
	  }
	  if(pcpivot.GetType() != GeomAbs_BSplineCurve){
	    Handle(Geom2d_TrimmedCurve) 
	      trc = new Geom2d_TrimmedCurve(pcpivot.Curve(),
					    Min(parCP1,parCP2),
					    Max(parCP1,parCP2));
	    PCurveOnPiv = Geom2dConvert::CurveToBSplineCurve(trc);
	  }
	  else {
	    PCurveOnPiv = Geom2dConvert::SplitBSplineCurve
	      (Handle(Geom2d_BSplineCurve)::DownCast(pcpivot.Curve()),
	       Min(parCP1,parCP2),Max(parCP1,parCP2),tol2d);
	  }
	  TColStd_Array1OfReal kk(1,PCurveOnPiv->NbKnots());
	  PCurveOnPiv->Knots(kk);
	  BSplCLib::Reparametrize(0.,1.,kk);
	  PCurveOnPiv->SetKnots(kk);
	  if(pcfalenvers) {
	    PCurveOnPiv->Reverse();
	    pctrans = TopAbs::Reverse(pctrans);
	  }
	}
	Handle(Geom_Surface) Surfcoin = fil.Surface();
	done = CompleteData(coin,Surfcoin,
			    HFaCo,PCurveOnFace,
			    HFaPiv,PCurveOnPiv,OFaCo,1,
			    0,0,0,0);
#ifdef OCCT_DEBUG
	ChFi3d_ResultChron(ch , t_remplissage);// result perf filling 
#endif 
      }
#ifdef OCCT_DEBUG
      ChFi3d_InitChron(ch); // init perf update DS
#endif 
      if (done){
	// Update 3 CornerData and the DS
	// ----------------------------------------
	if(resetcp1){
	  gp_Pnt pjyl = CP1.Point();
	  Standard_Real tolsav = CP1.Tolerance();
	  CP1.Reset();
	  CP1.SetPoint(pjyl);
	  CP1.SetTolerance(tolsav);
	}
	else if(resetcp2){
	  gp_Pnt pjyl = CP2.Point();
	  Standard_Real tolsav = CP2.Tolerance();
	  CP2.Reset();
	  CP2.SetPoint(pjyl);
	  CP2.SetTolerance(tolsav);
	}
	Standard_Real P1deb,P2deb,P1fin,P2fin;
	Standard_Integer If1,If2,Il1,Il2,Icf,Icl;
	const ChFiDS_CommonPoint& Pf1 = coin->VertexFirstOnS1();
	ChFiDS_CommonPoint& Pf2 = coin->ChangeVertexFirstOnS2();
	const ChFiDS_CommonPoint& Pl1 = coin->VertexLastOnS1();
	ChFiDS_CommonPoint& Pl2 = coin->ChangeVertexLastOnS2();
	Pf2 = CP1;
	Pl2 = CP2;
	
	// the corner to start,
	// -----------------------
	ChFiDS_Regul regdeb, regfin;
	If1 = ChFi3d_IndexPointInDS(Pf1,DStr);
	If2 = ChFi3d_IndexPointInDS(Pf2,DStr);
	Il1 = ChFi3d_IndexPointInDS(Pl1,DStr);
	if(sameparam) Il2 = If2;
	else Il2 = ChFi3d_IndexPointInDS(Pl2,DStr);
	
	gp_Pnt2d pp1,pp2;
	pp1 = coin->InterferenceOnS1().PCurveOnSurf()->
	  Value(coin->InterferenceOnS1().FirstParameter());
	pp2 = coin->InterferenceOnS2().PCurveOnSurf()->
	  Value(coin->InterferenceOnS2().FirstParameter());
	Handle(Geom_Curve) C3d;
	Standard_Real tolreached;
	ChFi3d_ComputeArete(Pf1,pp1,Pf2,pp2,
			    DStr.Surface(coin->Surf()).Surface(),C3d,
			    corner->ChangeFirstPCurve(),P1deb,P2deb,
			    tolesp,tol2d,tolreached,0);
	Standard_Real par1 = sd1->Interference(IFaArc1).Parameter(isfirst1);
	pp1 = sd1->Interference(IFaCo1).PCurveOnSurf()->Value(par1);
	pp2 = sd1->Interference(IFaArc1).PCurveOnSurf()->Value(par1);
	Standard_Real tolr1;
	ChFi3d_ComputePCurv(C3d,pp1,pp2,st1->ChangePCurve(isfirst1),
			    DStr.Surface(sd1->Surf()).Surface(),
			    P1deb,P2deb,tolesp,tolr1);
	tolreached = Max(tolreached,tolr1);
	TopOpeBRepDS_Curve Tcurv1(C3d,tolreached);
	Icf = DStr.AddCurve(Tcurv1);
	regdeb.SetCurve(Icf);
	regdeb.SetS1(coin->Surf(),0);
	regdeb.SetS2(sd1->Surf(),0);
	myRegul.Append(regdeb);
	corner->ChangeFirstCurve(Icf);
	corner->ChangeFirstParameters(P1deb,P2deb);
	corner->ChangeIndexFirstPointOnS1(If1);
	corner->ChangeIndexFirstPointOnS2(If2);
      
	pp1 = coin->InterferenceOnS1().PCurveOnSurf()->
	  Value(coin->InterferenceOnS1().LastParameter());
	pp2 = coin->InterferenceOnS2().PCurveOnSurf()->
	  Value(coin->InterferenceOnS2().LastParameter());
	ChFi3d_ComputeArete(Pl1,pp1,Pl2,pp2,
			    DStr.Surface(coin->Surf()).Surface(),C3d,
			    corner->ChangeLastPCurve(),P1fin,P2fin,
			    tolesp,tol2d,tolreached,0);
	Standard_Real par2 = sd2->Interference(IFaArc2).Parameter(isfirst2);
	pp1 = sd2->Interference(IFaCo2).PCurveOnSurf()->Value(par2);
	pp2 = sd2->Interference(IFaArc2).PCurveOnSurf()->Value(par2);
	Standard_Real tolr2;
	ChFi3d_ComputePCurv(C3d,pp1,pp2,st2->ChangePCurve(isfirst2),
			    DStr.Surface(sd2->Surf()).Surface(),
			    P1deb,P2deb,tolesp,tolr2);
	tolreached = Max(tolreached,tolr2);
	TopOpeBRepDS_Curve Tcurv2(C3d,tolreached);
	Icl = DStr.AddCurve(Tcurv2);
	regfin.SetCurve(Icl);
	regfin.SetS1(coin->Surf(),0);
	regfin.SetS2(sd2->Surf(),0);
	myRegul.Append(regfin);
	corner->ChangeLastCurve(Icl);
	corner->ChangeLastParameters(P1fin,P2fin);
	corner->ChangeIndexLastPointOnS1(Il1);
	corner->ChangeIndexLastPointOnS2(Il2);
	
	coin->ChangeIndexOfS1(DStr.AddShape(FaCo));
	if(sameparam) coin->ChangeIndexOfS2(0);
	else {
	  coin->ChangeIndexOfS2(DStr.AddShape(FaPiv));
	  coin->ChangeInterferenceOnS2().SetTransition(pctrans);
	}
	corner->SetSolidIndex(st1->SolidIndex());
	
	// then the starting Stripe,
	// ------------------------
	st1->SetCurve(Icf,isfirst1);
	st1->SetIndexPoint(If1,isfirst1,IFaCo1);
	st1->SetIndexPoint(If2,isfirst1,IFaArc1);
	st1->SetParameters(isfirst1,P1deb,P2deb);
	sd1->ChangeVertex(isfirst1,IFaCo1) = Pf1;
	sd1->ChangeVertex(isfirst1,IFaArc1) = Pf2;
	sd1->ChangeInterference(IFaCo1).SetParameter(par1,isfirst1);
	if (IFaCo1 == 2) st1->SetOrientation(TopAbs_REVERSED,isfirst1);
	
	// then the end Stripe,
	// -------------------------
	st2->SetCurve(Icl,isfirst2);
	st2->SetIndexPoint(Il1,isfirst2,IFaCo2);
	st2->SetIndexPoint(Il2,isfirst2,IFaArc2);
	st2->SetParameters(isfirst2,P1fin,P2fin);
	sd2->ChangeVertex(isfirst2,IFaCo2) = Pl1;
	sd2->ChangeVertex(isfirst2,IFaArc2) = Pl2;
	sd2->ChangeInterference(IFaCo2).SetParameter(par2,isfirst2);
	if (IFaCo2 == 2) st2->SetOrientation(TopAbs_REVERSED,isfirst2);
      }
#ifdef OCCT_DEBUG
      ChFi3d_ResultChron(ch , t_t2cornerDS);// result perf update DS 
#endif 
    }
    else {
      //it is necessary to make difference with
      if(!OkinterCC) {
	throw Standard_Failure("TwoCorner : No intersection pc pc");
      }
      Handle(ChFiDS_Stripe) stsam, stdif;
      Handle(ChFiDS_SurfData) sdsam, sddif;
      Standard_Real uintpcsam = 0., uintpcdif = 0.;
      Standard_Integer ifacosam = 0, ifacodif = 0, ifaopsam = 0, ifaopdif = 0;
      Standard_Boolean isfirstsam = Standard_False, isfirstdif = Standard_False;
      if(Stat1 == ChFiDS_OnSame && Stat2 == ChFiDS_OnDiff){
	stsam = st1; sdsam = sd1; uintpcsam = UIntPC1; 
	ifacosam = IFaCo1; ifaopsam = IFaArc1; isfirstsam = isfirst1;
	stdif = st2; sddif = sd2; uintpcdif = UIntPC2; 
	ifacodif = IFaCo2; ifaopdif = IFaArc2; isfirstdif = isfirst2;
      }
      else if(Stat1 == ChFiDS_OnDiff && Stat2 == ChFiDS_OnSame){
	stsam = st2; sdsam = sd2; uintpcsam = UIntPC2; 
	ifacosam = IFaCo2; ifaopsam = IFaArc2; isfirstsam = isfirst2;
	stdif = st1; sddif = sd1; uintpcdif = UIntPC1; 
	ifacodif = IFaCo1; ifaopdif = IFaArc1; isfirstdif = isfirst1;
      }
      else {
	throw Standard_Failure("TwoCorner : Config unknown");
      }
      //It is checked if surface ondiff has a point on arc from the side opposed
      //to the common face and if this arc is connected to the base face  
      //opposed to common face of the surface onsame.
      ChFiDS_CommonPoint& cpopdif = sddif->ChangeVertex(isfirstdif,ifaopdif);
      if(!cpopdif.IsOnArc()) {
	throw Standard_Failure("TwoCorner : No point on restriction on surface OnDiff");
      }
      const TopoDS_Edge& Arcopdif = cpopdif.Arc();
      const TopoDS_Face& Fopsam = TopoDS::Face(DStr.Shape(sdsam->Index(ifaopsam)));
      TopExp_Explorer ex;
      for(ex.Init(Fopsam,TopAbs_EDGE); ex.More(); ex.Next()){
	if(ex.Current().IsSame(Arcopdif)) {
	  break;
	}
	else if(!ex.More()) {
	  throw Standard_Failure("TwoCorner : No common face to loop the contour");
	}
      }
#ifdef OCCT_DEBUG
      ChFi3d_InitChron(ch ); // init perf filling 
#endif 
      Handle(GeomFill_Boundary) Bsam,Bdif,Bfac;
      gp_Pnt2d ppopsam = 
	sdsam->Interference(ifaopsam).PCurveOnSurf()->Value(uintpcsam);
      gp_Pnt2d ppcosam = 
	sdsam->Interference(ifacosam).PCurveOnSurf()->Value(uintpcsam);
      Handle(Geom_Surface) surfsam = DStr.Surface(sdsam->Surf()).Surface();
      Handle(GeomAdaptor_HSurface) Hsurfsam = new GeomAdaptor_HSurface(surfsam);
      Handle(Geom2d_Curve) pcsurfsam;
      Bsam = ChFi3d_mkbound(Hsurfsam,pcsurfsam,ppopsam,ppcosam,tolesp,2.e-4);
      Standard_Real upcopdif = sddif->Interference(ifaopdif).Parameter(isfirstdif);
      gp_Pnt2d ppopdif = 
	sddif->Interference(ifaopdif).PCurveOnSurf()->Value(upcopdif);
      gp_Pnt2d ppcodif = 
	sddif->Interference(ifacodif).PCurveOnSurf()->Value(uintpcdif);
      Handle(Geom_Surface) surfdif = DStr.Surface(sddif->Surf()).Surface();
      Handle(GeomAdaptor_HSurface) Hsurfdif = new GeomAdaptor_HSurface(surfdif);
      Handle(Geom2d_Curve) pcsurfdif;
      Bdif = ChFi3d_mkbound(Hsurfdif,pcsurfdif,ppcodif,ppopdif,tolesp,2.e-4);
      gp_Pnt2d ppfacsam,ppfacdif;
      gp_Pnt PPfacsam,PPfacdif;
      gp_Vec VVfacsam,VVfacdif;
      sdsam->Interference(ifaopsam).PCurveOnFace()->D0(uintpcsam,ppfacsam);
      const Handle(Geom_Curve)& curvopsam = 
	DStr.Curve(sdsam->Interference(ifaopsam).LineIndex()).Curve();
      curvopsam->D1(uintpcsam,PPfacsam,VVfacsam);
      BRepAdaptor_Curve2d PCArcFac(Arcopdif,Fopsam);
      PCArcFac.D0(cpopdif.ParameterOnArc(),ppfacdif);
      //jgv for OCC26173
      BRepAdaptor_Surface SurFopsam(Fopsam);
      if (SurFopsam.IsUClosed())
      {
        Standard_Real Uperiod = SurFopsam.LastUParameter() - SurFopsam.FirstUParameter();
        if (Abs(ppfacsam.X() - ppfacdif.X()) > Uperiod/2)
        {
          if (ppfacdif.X() < ppfacsam.X())
            ppfacdif.SetX(ppfacdif.X() + Uperiod);
          else
            ppfacdif.SetX(ppfacdif.X() - Uperiod);
        }
      }
      //////////////////
      BRepAdaptor_Curve CArcFac(Arcopdif);
      CArcFac.D1(cpopdif.ParameterOnArc(),PPfacdif,VVfacdif);
      Handle(BRepAdaptor_HSurface) HBRFopsam = new BRepAdaptor_HSurface();
      BRepAdaptor_Surface& BRFopsam = HBRFopsam->ChangeSurface();
      BRFopsam.Initialize(Fopsam,Standard_False);
      Handle(Geom2d_Curve) pcFopsam = ChFi3d_BuildPCurve(HBRFopsam,
							 ppfacsam,VVfacsam,
							 ppfacdif,VVfacdif,1);
      Bfac = ChFi3d_mkbound(HBRFopsam,pcFopsam,tolesp,2.e-4);
      GeomFill_ConstrainedFilling fil(8,20);
      fil.Init(Bsam,Bdif,Bfac,1);
#if 0
      for(Standard_Integer ib = 0; ib < 4; ib++){
	if(ib == 2) continue;
	fil.CheckCoonsAlgPatch(ib);
	fil.CheckTgteField(ib);
	fil.CheckApprox(ib);
	fil.CheckResult(ib);
      }
#endif
      Handle(Geom_Surface) Surfcoin = fil.Surface();
      TopAbs_Orientation Osurfsam = sdsam->Orientation();
      Handle(Geom2d_Curve) pcnul;
      done = CompleteData(coin,Surfcoin,
			  Hsurfsam,pcsurfsam,
			  HBRFopsam,pcnul,Osurfsam,1,
			  0,0,0,0);
#ifdef OCCT_DEBUG
      ChFi3d_ResultChron(ch , t_remplissage);// result perf filling 
#endif 
      if(!done) throw Standard_Failure("concavites inverted : fail");
#ifdef OCCT_DEBUG
      ChFi3d_InitChron(ch); // init perf update DS
#endif 
      // Update 3 CornerData and the DS
      // ----------------------------------------
      // the corner to start,
      // -----------------------
      Standard_Real P1deb,P2deb,P1fin,P2fin;
      Standard_Integer If1,If2,Il1,Il2,Icf,Icl;
      const ChFiDS_CommonPoint& Pf1 = coin->VertexFirstOnS1();
      ChFiDS_CommonPoint& Pf2 = coin->ChangeVertexFirstOnS2();
      const ChFiDS_CommonPoint& Pl1 = coin->VertexLastOnS1();
      ChFiDS_CommonPoint& Pl2 = coin->ChangeVertexLastOnS2();
      Pf2 = Pl2 = cpopdif;
	
      ChFiDS_Regul regdeb, regfin;
      If1 = ChFi3d_IndexPointInDS(Pf1,DStr);
      If2 = ChFi3d_IndexPointInDS(Pf2,DStr);
      Il1 = ChFi3d_IndexPointInDS(Pl1,DStr);
      Il2 = If2;
	
      gp_Pnt2d pp1,pp2;
      pp1 = coin->InterferenceOnS1().PCurveOnSurf()->
	Value(coin->InterferenceOnS1().FirstParameter());
      pp2 = coin->InterferenceOnS2().PCurveOnSurf()->
	Value(coin->InterferenceOnS2().FirstParameter());
      Handle(Geom_Curve) C3d;
      Standard_Real tolreached;
      ChFi3d_ComputeArete(Pf1,pp1,Pf2,pp2,
			  DStr.Surface(coin->Surf()).Surface(),C3d,
			  corner->ChangeFirstPCurve(),P1deb,P2deb,
			  tolesp,tol2d,tolreached,0);
      Standard_Real tolr1;
      Handle(GeomAdaptor_HCurve) HC3d = new GeomAdaptor_HCurve(C3d);
      ChFi3d_SameParameter(HC3d,pcFopsam,HBRFopsam,tolesp,tolr1);
      tolreached = Max(tolreached,tolr1);
      TopOpeBRepDS_Curve Tcurv1(C3d,tolreached);
      Icf = DStr.AddCurve(Tcurv1);
      // place the pcurve on face in the DS
      TopAbs_Orientation OpcFopsam = sdsam->Interference(ifaopsam).Transition();
      Standard_Integer IFopsam = sdsam->Index(ifaopsam);
      if(isfirstsam) OpcFopsam = TopAbs::Reverse(OpcFopsam);
      Handle(TopOpeBRepDS_SurfaceCurveInterference) 
	interf = ChFi3d_FilCurveInDS(Icf,IFopsam,pcFopsam,OpcFopsam);
      DStr.ChangeShapeInterferences(IFopsam).Append(interf);

      regdeb.SetCurve(Icf);
      regdeb.SetS1(coin->Surf(),0);
      regdeb.SetS2(IFopsam,1);
      myRegul.Append(regdeb);
      corner->ChangeFirstCurve(Icf);
      corner->ChangeFirstParameters(P1deb,P2deb);
      corner->ChangeIndexFirstPointOnS1(If1);
      corner->ChangeIndexFirstPointOnS2(If2);
      
      pp1 = coin->InterferenceOnS1().PCurveOnSurf()->
	Value(coin->InterferenceOnS1().LastParameter());
      pp2 = coin->InterferenceOnS2().PCurveOnSurf()->
	Value(coin->InterferenceOnS2().LastParameter());
      ChFi3d_ComputeArete(Pl1,pp1,Pl2,pp2,
			  DStr.Surface(coin->Surf()).Surface(),C3d,
			  corner->ChangeLastPCurve(),P1fin,P2fin,
			  tolesp,tol2d,tolreached,0);
      Standard_Real tolr2;
      HC3d->ChangeCurve().Load(C3d);
      ChFi3d_SameParameter(HC3d,pcsurfdif,Hsurfdif,tolesp,tolr2);
      tolreached = Max(tolreached,tolr2);
      TopOpeBRepDS_Curve Tcurv2(C3d,tolreached);
      Icl = DStr.AddCurve(Tcurv2);
      regfin.SetCurve(Icl);
      regfin.SetS1(coin->Surf(),0);
      regfin.SetS2(sddif->Surf(),0);
      myRegul.Append(regfin);
      corner->ChangeLastCurve(Icl);
      corner->ChangeLastParameters(P1fin,P2fin);
      corner->ChangeIndexLastPointOnS1(Il1);
      corner->ChangeIndexLastPointOnS2(Il2);
	
      coin->ChangeIndexOfS1(-sdsam->Surf());
      coin->ChangeIndexOfS2(0);

      corner->SetSolidIndex(stsam->SolidIndex());
	
      // then Stripe OnSame
      // ---------------------
      const ChFiDS_FaceInterference& intcoin1 = coin->InterferenceOnS1();
      stsam->SetCurve(intcoin1.LineIndex(),isfirstsam);
      stsam->InDS(isfirstsam); // filDS already works from the corner.
      stsam->ChangePCurve(isfirstsam) = coin->InterferenceOnS1().PCurveOnFace();
      stsam->SetIndexPoint(If1,isfirstsam,ifaopsam);
      stsam->SetIndexPoint(Il1,isfirstsam,ifacosam);
      stsam->SetParameters(isfirstsam,
			   intcoin1.FirstParameter(),
			   intcoin1.LastParameter());
      sdsam->ChangeVertex(isfirstsam,ifaopsam) = Pf1;
      sdsam->ChangeVertex(isfirstsam,ifacosam) = Pl1;
      sdsam->ChangeInterferenceOnS1().SetParameter(uintpcsam,isfirstsam);
      sdsam->ChangeInterferenceOnS2().SetParameter(uintpcsam,isfirstsam);
      if (ifaopsam == 2) stsam->SetOrientation(TopAbs_REVERSED,isfirstsam);
	
      // then Stripe OnDiff
      // ---------------------
      stdif->SetCurve(Icl,isfirstdif);
      stdif->ChangePCurve(isfirstdif) = pcsurfdif;
      stdif->SetIndexPoint(Il2,isfirstdif,ifaopdif);
      stdif->SetIndexPoint(Il1,isfirstdif,ifacodif);
      stdif->SetParameters(isfirstdif,P1fin,P2fin);
      sddif->ChangeVertex(isfirstdif,ifaopdif) = Pl2;
      sddif->ChangeVertex(isfirstdif,ifacodif) = Pl1;
      sddif->ChangeInterference(ifacodif).SetParameter(uintpcdif,isfirstdif);
      if (ifaopdif == 1) stdif->SetOrientation(TopAbs_REVERSED,isfirstdif);
#ifdef OCCT_DEBUG
      ChFi3d_ResultChron(ch , t_t2cornerDS);// result perf update DS 
#endif 
    }
    if(!myEVIMap.IsBound(Vtx)){
      TColStd_ListOfInteger li;
      myEVIMap.Bind(Vtx,li);
    }
    myEVIMap.ChangeFind(Vtx).Append(coin->Surf());
    myListStripe.Append(corner);
  }
}  

