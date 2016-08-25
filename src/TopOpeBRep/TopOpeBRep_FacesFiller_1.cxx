// Created on: 1994-02-17
// Created by: Jean Yves LEBEY
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


#include <Bnd_Box.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_CString.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRep.hxx>
#include <TopOpeBRep_define.hxx>
#include <TopOpeBRep_FacesFiller.hxx>
#include <TopOpeBRep_FacesIntersector.hxx>
#include <TopOpeBRep_FFDumper.hxx>
#include <TopOpeBRep_FFTransitionTool.hxx>
#include <TopOpeBRep_GeomTool.hxx>
#include <TopOpeBRep_LineInter.hxx>
#include <TopOpeBRep_PointClassifier.hxx>
#include <TopOpeBRep_PointGeomTool.hxx>
#include <TopOpeBRep_VPointInter.hxx>
#include <TopOpeBRep_VPointInterClassifier.hxx>
#include <TopOpeBRep_VPointInterIterator.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepDS_Dumper.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_InterferenceTool.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>
#include <TopOpeBRepDS_Transition.hxx>
#include <TopOpeBRepTool_defineG.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_makeTransition.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#ifdef DRAW
#include <TopOpeBRep_DRAW.hxx>
#endif

#ifdef OCCT_DEBUG
Standard_EXPORT void debrest(const Standard_Integer i)   {cout<<"+ debrest "<<i<<endl;}
Standard_EXPORT void debrline()   {cout<<"+ debrline"<<endl;}

extern Standard_Boolean TopOpeBRep_GettraceNVP(Standard_Integer a,Standard_Integer b,Standard_Integer c,Standard_Integer d,Standard_Integer e);
extern Standard_Boolean GLOBAL_bvpr;
extern void debvprmess(Standard_Integer f1,Standard_Integer f2,Standard_Integer il,Standard_Integer vp,Standard_Integer si);
extern Standard_Boolean TopOpeBRep_GetcontextNOPUNK();

#ifdef DRAW
static void FUN_traceRLine(const TopOpeBRep_LineInter& L)
{

  TCollection_AsciiString ee("Edofline"); ee.Cat(L.Index()); char* eee = ee.ToCString();
  DBRep::Set(eee,L.Arc());

}
#else
static void FUN_traceRLine(const TopOpeBRep_LineInter&)
{
    //
}
#endif

#ifdef DRAW
static void FUN_traceGLine(const TopOpeBRep_LineInter& L)
#else
static void FUN_traceGLine(const TopOpeBRep_LineInter&)
#endif
{
#ifdef DRAW
  TCollection_AsciiString ll("Glineof"); ll.Cat(L.Index()); char* lll = ll.ToCString();
  Handle(Geom_Curve) c = L.Curve();
  Standard_Integer iINON1,iINONn,nINON; L.VPBounds(iINON1,iINONn,nINON);
  Standard_Real par1 = L.VPoint(iINON1).ParameterOnLine();
  Standard_Real parn = L.VPoint(iINONn).ParameterOnLine();
  Standard_Boolean isperiodic = L.IsPeriodic();
  Handle(Geom_TrimmedCurve) tc = new Geom_TrimmedCurve(c,par1,parn,Standard_True);  
  DrawTrSurf::Set(lll,tc);
#endif
}
#endif

#define M_FORWARD(o)  (o == TopAbs_FORWARD)
#define M_REVERSED(o) (o == TopAbs_REVERSED)

#define FORWARD  (1)
#define REVERSED (2)
#define INTERNAL (3)
#define EXTERNAL (4)
#define CLOSING  (5)

Standard_EXPORT void FUN_GetdgData(TopOpeBRepDS_PDataStructure& pDS,const TopOpeBRep_LineInter& L,
				   const TopoDS_Face& F1,const TopoDS_Face& F2, TopTools_DataMapOfShapeListOfShape& datamap);
Standard_EXPORT void FUN_FillVof12(const TopOpeBRep_LineInter& L,TopOpeBRepDS_PDataStructure pDS);

#define M_FINDVP  (0) // only look for new vp
#define M_MKNEWVP (1) // only make newvp
#define M_GETVP   (2) // steps (0) [+(1) if (O) fails]
Standard_EXPORT void FUN_VPIndex(TopOpeBRep_FacesFiller& FF,
				 const TopOpeBRep_LineInter& L,
				 const TopOpeBRep_VPointInter& VP,
				 const Standard_Integer ShapeIndex,
				 const Handle(TopOpeBRepDS_HDataStructure)& HDS,
				 const TopOpeBRepDS_ListOfInterference& DSCIL,
				 TopOpeBRepDS_Kind& PVKind, Standard_Integer& PVIndex, // out
				 Standard_Boolean& EPIfound, Handle(TopOpeBRepDS_Interference)& IEPI, // out 
				 Standard_Boolean& CPIfound, Handle(TopOpeBRepDS_Interference)& ICPI, // out
				 const Standard_Integer mkVP);
Standard_EXPORT Standard_Boolean FUN_newtransEdge(const Handle(TopOpeBRepDS_HDataStructure) HDS,
				     const TopOpeBRep_FacesFiller& FF,
				     const TopOpeBRep_LineInter& L,
				     const Standard_Boolean& Lonrest,
				     const TopOpeBRep_VPointInter& VP, 
				     const TopOpeBRepDS_Kind PVKind, const Standard_Integer PVIndex,
				     const Standard_Integer& OOShapeIndex,
				     const TopoDS_Edge& edge, const TopTools_ListOfShape& ERL, TopOpeBRepDS_Transition& T);
#define M_INTERNAL(st)  (st == TopAbs_INTERNAL) 


static Standard_Boolean FUN_IwithsuppiS(const TopOpeBRepDS_ListOfInterference& loI, const Standard_Integer iS, TopOpeBRepDS_ListOfInterference& loIfound)
{
  TopOpeBRepDS_ListIteratorOfListOfInterference it(loI);
  for (; it.More(); it.Next()) {const Handle(TopOpeBRepDS_Interference)& I = it.Value(); 
				if (I->Support() == iS) loIfound.Append(I);}
  Standard_Boolean ok = (loIfound.Extent() > 0);
  return ok;
}
static Standard_Boolean FUN_IwithsuppkS(const TopOpeBRepDS_ListOfInterference& loI, const TopOpeBRepDS_Kind& kS, TopOpeBRepDS_ListOfInterference& loIfound)
{
  TopOpeBRepDS_ListIteratorOfListOfInterference it(loI);
  for (; it.More(); it.Next()) {const Handle(TopOpeBRepDS_Interference)& I = it.Value(); 
				if (I->SupportType() == kS) loIfound.Append(I);}
  Standard_Boolean ok = (loIfound.Extent() > 0);
  return ok;
}
static Standard_Boolean FUN_IwithToniS(const TopOpeBRepDS_ListOfInterference& loI, const Standard_Integer iS, TopOpeBRepDS_ListOfInterference& loIfound)
{
  TopOpeBRepDS_ListIteratorOfListOfInterference it(loI);
  for (; it.More(); it.Next()) {const Handle(TopOpeBRepDS_Interference)& I = it.Value(); 
				if (I->Transition().Index() == iS) loIfound.Append(I);}
  Standard_Boolean ok = (loIfound.Extent() > 0);
  return ok;
}
static Standard_Boolean FUN_supponF(const TopOpeBRepDS_PDataStructure pDS, 
		       const TopOpeBRepDS_ListOfInterference& loI, const Standard_Integer iF,
		       TopOpeBRepDS_ListOfInterference& lIsupponF, TColStd_ListOfInteger& losupp)
{
  //<losupp> = list of support S / I in <loI> : I = (T,G,S = Edge on <F>);
  Standard_Boolean ok = (0 < iF) && (iF <= pDS->NbShapes());
  if (!ok) return Standard_False; 

  TopTools_IndexedMapOfShape MOOE; TopExp::MapShapes(pDS->Shape(iF),TopAbs_EDGE,MOOE);
  TopOpeBRepDS_ListIteratorOfListOfInterference it(loI);
  for (; it.More(); it.Next()){ 
    const Handle(TopOpeBRepDS_Interference)& I = it.Value(); Standard_Integer iS = I->Support();
    TopOpeBRepDS_Kind skind = I->SupportType(); 
    Standard_Boolean add = Standard_False;
    if (skind == TopOpeBRepDS_EDGE) add = MOOE.Contains(pDS->Shape(iS));
    if (add) {losupp.Append(iS); lIsupponF.Append(I);}
  }
  if (losupp.Extent() < 1) return Standard_False;
  return Standard_True;
}
static Standard_Boolean FUN_IoflSsuppS(const TopOpeBRepDS_PDataStructure pDS, 
			  const Standard_Integer iS,const TColStd_ListOfInteger& lShape, 
			  TopOpeBRepDS_ListOfInterference& IsuppiS)
{
  Standard_Boolean ok = Standard_False;
  // E in <losupp> /
  // I on E : I = (T, G, S=iS)

  // looking for interferences attached to shapes of <lShape> with support <iS>
  TColStd_ListIteratorOfListOfInteger iti(lShape);
  for (; iti.More(); iti.Next()){
    const TopOpeBRepDS_ListOfInterference& lI = pDS->ShapeInterferences(iti.Value());
    ok = FUN_IwithsuppiS(lI,iS,IsuppiS);
  }
  return ok;
}

//=======================================================================
// 3D
// purpose : The compute of a transition edge/face given interferences
//           attached to the edge (stored in the DS).
//=======================================================================
static Standard_Boolean FUN_findTF(const TopOpeBRepDS_PDataStructure pDS,
		      const Standard_Integer iE, const Standard_Integer, const Standard_Integer iOOF,
		      TopOpeBRepDS_Transition& TF)
{  
  Standard_Real factor = 0.5;
  // ----------------------------------------------------------------------
  // <Ifound> on <E> : Ifound = (T, S=OOF, G=POINT/VERTEX on <E>)
  //                            (T, S=edge of OOF, G=POINT/VERTEX on <E>)
  // ----------------------------------------------------------------------

  // <lITonOOF>
  TopOpeBRepDS_ListOfInterference lITonOOF;
  Standard_Boolean ok = FUN_IwithToniS(pDS->ShapeInterferences(iE),iOOF,lITonOOF);
  if (!ok) return Standard_False;  
  TopOpeBRepDS_ListOfInterference lITOOFskFACE;
  Standard_Boolean found = FUN_IwithsuppkS(lITonOOF,TopOpeBRepDS_FACE,lITOOFskFACE);
  if (found) {
    // NYI : a deeper analysis is needed, for the moment, we make the following
    // prequesitory : transition on E of F on point of ES is valid for
    //                all the ES (here restriction) ie :
    //                TF : transition face(F) / face(OOF) on G = ES =
    //                TE : transition edge(E) / face(OOF) at G = POINT/VERTEX 
    //Ifound on <E> : Ifound = (T(on face OOF), S=FACE, G=POINT/VERTEX on <E>)
    const Handle(TopOpeBRepDS_Interference)& Ifound = lITOOFskFACE.First();
    TF = Ifound->Transition(); 
  }

  Standard_Boolean done = Standard_False;
  TopOpeBRepDS_ListOfInterference lITOOFskEDGE;
  if (!found) done = FUN_IwithsuppkS(lITonOOF,TopOpeBRepDS_EDGE,lITOOFskEDGE);
  if (done) {
    // Ifound on <E> : Ifound = (T(on face OOF), S=FACE, G=POINT/VERTEX on <E>) 
    // if <Ifound> found : compute TE at G / <OOF>.
    // TE ->TF. 
    const Handle(TopOpeBRepDS_Interference)& Ifound = lITOOFskEDGE.First();
    const TopoDS_Edge& OOE = TopoDS::Edge(pDS->Shape(Ifound->Support()));	      
    Standard_Real paronE; Standard_Boolean OOdone = FDS_Parameter(Ifound,paronE);
    if (!OOdone) return Standard_False;

    const TopoDS_Edge& E   = TopoDS::Edge(pDS->Shape(iE));
    const TopoDS_Face& OOF = TopoDS::Face(pDS->Shape(iOOF));

    Standard_Real f,l; FUN_tool_bounds(E,f,l);
    TopOpeBRepTool_makeTransition MKT; 

    Standard_Boolean OOEboundOOF = FUN_tool_EboundF(OOE,OOF);    
    Standard_Boolean iscl = TopOpeBRepTool_TOOL::IsClosingE(OOE,OOF);
    if (OOEboundOOF && (!iscl)) {
      Standard_Real oopar; Standard_Boolean ok1 = FUN_tool_parE(E,paronE,OOE,oopar);
      if (!ok1) return Standard_False;
      gp_Pnt2d uv;   ok1 = FUN_tool_paronEF(OOE,oopar,OOF,uv);
      if (!ok1) return Standard_False;

      ok = MKT.Initialize(E,f,l,paronE, OOF,uv, factor); 
      if (ok) ok = MKT.SetRest(OOE,oopar);
    }
    else {
      gp_Pnt2d uv; Standard_Boolean ok1 = FUN_tool_parF(E,paronE,OOF,uv);
      if (!ok1) return Standard_False;

      ok = MKT.Initialize(E,f,l,paronE, OOF,uv, factor);       
    }
    TopAbs_State stb,sta; ok = MKT.MkTonE(stb,sta);
    if (!ok) return Standard_False;
    TF.Before(stb); TF.After(sta);
    return Standard_True;
    
  }
  ok = found || done;
  return ok;
}
static Standard_Boolean FUN_findTOOF(const TopOpeBRepDS_PDataStructure pDS,
			const Standard_Integer iE, const Standard_Integer iF, const Standard_Integer iOOF,
			TopOpeBRepDS_Transition& TOOF)
{
  Standard_Real factor = 0.5;

  // ----------------------------------------------------------------------
  // <E> bound of <F>, 
  // <OOE> on <OOF> / 
  // <OOIfound> on <OOE>  : OOIfound = (T, S=iF, G=POINT/VERTEX on <E>)
  // ----------------------------------------------------------------------
 
  // <lIsuppOOE> = list of interferences attached to <E> of support S = edge of <OOF>
  // <liOOE> = list of supports of <lIsuppOOE>.
  const TopOpeBRepDS_ListOfInterference& loIE = pDS->ShapeInterferences(iE);
  TopOpeBRepDS_ListOfInterference lITonOOF; Standard_Boolean ok = FUN_IwithToniS(loIE,iOOF,lITonOOF);
  TopOpeBRepDS_ListOfInterference lIsuppOOE; 
  TColStd_ListOfInteger liOOEGonE;   
  if (ok) {
    ok = FUN_IwithsuppkS(lITonOOF,TopOpeBRepDS_EDGE,lIsuppOOE);
    if (ok) {TopOpeBRepDS_ListIteratorOfListOfInterference it(lIsuppOOE);
	     for (; it.More(); it.Next()) liOOEGonE.Append(it.Value()->Support());}
  }
  else ok = FUN_supponF(pDS,loIE,iOOF,lIsuppOOE,liOOEGonE);
  if (!ok) return Standard_False;

//  TopAbs_Orientation oritransOOE;

  // <lOOIfound> = list of I attached to shapes of <liOOE> /
  //               I = (T, S=F, G=POINT/VERTEX on <E>)
  TopOpeBRepDS_ListOfInterference lIOOEsuppFGonE;
  Standard_Boolean OOfound = FUN_IoflSsuppS(pDS,iF,liOOEGonE,lIOOEsuppFGonE);
  if (OOfound) {	
    // NYI : a deeper analysis is needed, for the moment, we make the following
    // prequesitory : transition on OOE of OOF on point of ES is valid for
    //                all the ES (here restriction) ie :
    //                TOOF : transition face(OOF) / face(F) on (G == ES)
    //            <=> TOOE : transition edge(OOE) / face(F) at G = POINT/VERTEX 
    const Handle(TopOpeBRepDS_Interference)& OOIfound = lIOOEsuppFGonE.First(); 
    TOOF = OOIfound->Transition();    
  }

  Standard_Boolean OOdone = Standard_False;
  if (!OOfound) {
    // Ifound on <E> : Ifound = (T, S=EDGE on <OOF>, G=POINT/VERTEX on <E>)
    // if <Ifound> found : compute TOOE at G / <F>
    // TOOE ->TOOF.
    const Handle(TopOpeBRepDS_Interference)& Ifound = lIsuppOOE.First();
    const TopoDS_Edge& OOE = TopoDS::Edge(pDS->Shape(Ifound->Support()));	      
    Standard_Real paronE; OOdone = FDS_Parameter(Ifound,paronE);
    if (!OOdone) return Standard_False;

    const TopoDS_Edge& E = TopoDS::Edge(pDS->Shape(iE));
    const TopoDS_Face& F = TopoDS::Face(pDS->Shape(iF));
    
    Standard_Real oopar; Standard_Boolean ok1 = FUN_tool_parE(E,paronE,OOE,oopar);
    if (!ok1) return Standard_False;
    gp_Pnt2d uv;   ok1 = FUN_tool_paronEF(E,paronE,F,uv);
    if (!ok1) return Standard_False;
    Standard_Real f,l; FUN_tool_bounds(OOE,f,l);
    
    TopAbs_State stb = TopAbs_UNKNOWN,sta = TopAbs_UNKNOWN;
    TopOpeBRepTool_makeTransition MKT; 
    OOdone = MKT.Initialize(OOE,f,l,oopar,F,uv,factor);
    if (OOdone) OOdone = MKT.SetRest(E,paronE);
    if (OOdone) OOdone = MKT.MkTonE(stb,sta);
    if (OOdone) {TOOF.Before(stb); TOOF.After(sta);}
  }
  ok = OOfound || OOdone;
  return ok;
} 

Standard_EXPORT Standard_Boolean GLOBAL_btcx = Standard_False;
Standard_EXPORT void debtcxmess(Standard_Integer f1,Standard_Integer f2,Standard_Integer il)
{cout<<"f1,f2,il : "<<f1<<","<<f2<<","<<il<<endl;cout.flush();}

//=======================================================================
//function : ProcessLine
//purpose  : 
//=======================================================================
void TopOpeBRep_FacesFiller::ProcessLine()
{
  Standard_Boolean reject = ( !myLineOK || myLine==NULL );
  if (reject) return;
  ResetDSC();

  Standard_Boolean HasVPonR = myLine->HasVPonR();
  if (HasVPonR) FillLineVPonR();
  else          FillLine();
  
  Standard_Boolean inl = myLine->INL();
  if (inl) return;
  
  myHDS->SortOnParameter(myDSCIL);

  AddShapesLine();
}

//=======================================================================
//function : ResetDSC
//purpose  : 
//=======================================================================
void TopOpeBRep_FacesFiller::ResetDSC()
{
  myDSCIndex = 0;
  myDSCIL.Clear();
}

//=======================================================================
//function : ProcessVPInotonR
//purpose  : Same as ProcessVPnotonR.
//=======================================================================
void TopOpeBRep_FacesFiller::ProcessVPInotonR(TopOpeBRep_VPointInterIterator& VPI)
{
  const TopOpeBRep_VPointInter& VP = VPI.CurrentVP();
  ProcessVPnotonR(VP); 
} 

//=======================================================================
//function : ProcessVPnotonR
//purpose  : 
//=======================================================================
void TopOpeBRep_FacesFiller::ProcessVPnotonR(const TopOpeBRep_VPointInter& VP)
{
  Standard_Integer ShapeIndex = 0;
  Standard_Integer iVP = VP.Index();

#ifdef OCCT_DEBUG
  Standard_Integer ili=myLine->Index(),ivp=iVP,isi=ShapeIndex;
  GLOBAL_bvpr = TopOpeBRep_GettraceNVP(myexF1,myexF2,ili,ivp,isi);
  if (GLOBAL_bvpr) debvprmess(myexF1,myexF2,ili,ivp,isi);
#endif

  Standard_Integer iINON1,iINONn,nINON;
  myLine->VPBounds(iINON1,iINONn,nINON);  
  TopOpeBRepDS_ListIteratorOfListOfInterference itCPIL(myDSCIL);

  TopOpeBRepDS_Kind PVKind; Standard_Integer PVIndex;
  Standard_Boolean CPIfound = GetGeometry(itCPIL,VP,PVIndex,PVKind);
  if ( !CPIfound ) {
    if (iVP != iINON1 && iVP != iINONn) {
#ifdef OCCT_DEBUG
      cout<<"VP "<<iVP<<" on "<<0<<" : point d'intersection anormal : rejet"<<endl;
#endif
      return;
    }
  }

  if ( ! CPIfound ) {
    Standard_Boolean found = GetFFGeometry(VP,PVKind,PVIndex);
    if ( ! found ) PVIndex = MakeGeometry(VP,ShapeIndex,PVKind);
  }
  
  TopOpeBRepDS_Transition transLine;
  if ( CPIfound ) {
    const Handle(TopOpeBRepDS_Interference)& I = itCPIL.Value();
    const TopOpeBRepDS_Transition& TI = I->Transition();
    transLine = TI.Complement();
  }
  else {
    if      (iVP == iINON1) transLine.Set(TopAbs_FORWARD);
    else if (iVP == iINONn) transLine.Set(TopAbs_REVERSED);
  }
  
  Standard_Real parline = VP.ParameterOnLine();
  Handle(TopOpeBRepDS_Interference) CPI = TopOpeBRepDS_InterferenceTool::MakeCurveInterference
    (transLine,TopOpeBRepDS_CURVE,0,PVKind,PVIndex,parline);
  StoreCurveInterference(CPI);
  
} // ProcessVPnotonR

//=======================================================================
//function : ProcessVPR
//purpose  : 
//=======================================================================
void TopOpeBRep_FacesFiller::ProcessVPR(TopOpeBRep_FacesFiller& FF,const TopOpeBRep_VPointInter& VP)
{
  TopOpeBRepDS_Transition LineTonF1 = FaceFaceTransition(1);
  TopOpeBRepDS_Transition LineTonF2 = FaceFaceTransition(2);
  TopoDS_Face F1 = myF1; 
  TopoDS_Face F2 = myF2;
  // --- check interiority of VPoint to the restrictions    
  Standard_Boolean tokeep = VP.Keep();
  if ( !tokeep ) return;
  
  Standard_Integer ShapeIndex = VP.ShapeIndex();
  
  if (ShapeIndex == 0) {
    FF.ProcessVPnotonR(VP);
  }
  else if (ShapeIndex == 1) {
    FF.ProcessVPonR(VP,LineTonF1,F1,1);
  }
  else if (ShapeIndex == 2) {
    FF.ProcessVPonR(VP,LineTonF2,F2,2);
  }
  else if (ShapeIndex == 3) {
    
    Standard_Boolean isV1 = VP.IsVertexOnS1();
    Standard_Boolean isV2 = VP.IsVertexOnS2();
    
    Standard_Integer   shin1 = 1;
    if (isV2 && !isV1) shin1 = 2;
    
    if      (shin1 == 1) {
      FF.ProcessVPonR(VP,LineTonF1,F1,1);
      FF.ProcessVPonR(VP,LineTonF2,F2,2);
    }
    else if (shin1 == 2) {
      FF.ProcessVPonR(VP,LineTonF2,F2,2);
      FF.ProcessVPonR(VP,LineTonF1,F1,1);
    }
  }
} // FUNvponr

static Standard_Boolean FUN_brep_ONfirstP(const TopOpeBRep_VPointInter& vpf, const TopOpeBRep_VPointInter& VP)
// prequesitory : gline is on edge
{  
  Standard_Real parfirst = vpf.ParameterOnLine();
  Standard_Real parcur = VP.ParameterOnLine();
  Standard_Real d = parcur - parfirst;
  Standard_Real tol = Precision::Confusion(); //nyixpu051098 : see lbr...
  Standard_Boolean ONfirstP = (Abs(d) < tol);
  return ONfirstP;
}

//=======================================================================
//function : ProcessRLine
//purpose  : 
//=======================================================================
void TopOpeBRep_FacesFiller::ProcessRLine()
{
  if (myLine->TypeLineCurve() != TopOpeBRep_RESTRICTION) {return;}
  
  Standard_Boolean addIFE = Standard_True;
  if (!addIFE) return;

  const TopoDS_Edge& Erest = TopoDS::Edge(myLine->Arc());
  Standard_Boolean FIisrest = myFacesIntersector->IsRestriction(Erest);
  if (!FIisrest) return; 

  Standard_Boolean isedge1 = myLine->ArcIsEdge(1);
  Standard_Boolean isedge2 = myLine->ArcIsEdge(2);
  Standard_Integer EShapeIndex = (isedge1) ? 1 : (isedge2) ? 2 : 0;
  
  Standard_Integer iErest = myDS->AddShape(Erest,EShapeIndex);
  Standard_Integer rank = myDS->AncestorRank(iErest);
  Standard_Integer OOrank = (rank == 1)? 2: 1;
  
  Standard_Integer iF1 = myDS->AddShape(myF1,1);
  Standard_Integer iF2 = myDS->AddShape(myF2,2);
  Handle(TopOpeBRepDS_Interference) IFE;
  
  TopOpeBRepDS_Transition T1 = FaceFaceTransition(1); T1.Index(iF2);
  TopOpeBRepDS_Transition T2 = FaceFaceTransition(2); T2.Index(iF1);
  
  Standard_Boolean T1unk = T1.IsUnknown();
  Standard_Boolean T2unk = T2.IsUnknown();
  Standard_Boolean processUNK = Standard_False;
#ifdef OCCT_DEBUG
  Standard_Boolean nopunk = TopOpeBRep_GetcontextNOPUNK();
  if (nopunk) processUNK = Standard_False;
#endif
  if (processUNK && (T1unk || T2unk)) {
    TopoDS_Shape F =   (*this).Face(rank);   Standard_Integer iF = myDS->Shape(F);
    TopoDS_Shape OOF = (*this).Face(OOrank); Standard_Integer iOOF = myDS->Shape(OOF);
    Standard_Boolean findTOOF = (T1unk && (OOrank == 1)) || (T2unk && (OOrank == 2));
    Standard_Boolean findTF = (T1unk && (rank == 1)) || (T2unk && (rank == 2));
    
    if (findTOOF) {
      // <Erest> on <F>, 
      // ?<OOE> on <OOF> / 
      // ?<OOIfound> on <OOE>  : OOIfound = (T, S=iF, G=POINT/VERTEX on <Erest>)
      TopOpeBRepDS_Transition T; Standard_Boolean OOTok = FUN_findTOOF(myDS,iErest,iF,iOOF,T);
      if (OOTok) {
	if (OOrank == 1) FDS_SetT(T1,T);
	else             FDS_SetT(T2,T);
      }
    } // !findTOOF
    if (findTF) {
      // ?Ifound on <Erest> : Ifound = (T on FACE=iOOF, S, G=POINT/VERTEX on <Erest>)
      // if <Ifound> found : compute TErest at G / <OOF>
      TopOpeBRepDS_Transition T; Standard_Boolean Tok = FUN_findTF(myDS,iErest,iF,iOOF,T);
      if (Tok) {
	if (rank == 1)  FDS_SetT(T1,T);
	else            FDS_SetT(T2,T);
      }
    }
    T1unk = T1.IsUnknown();
    T2unk = T2.IsUnknown();
  } // processUNK && (T1unk || T2unk)
  
  IFE = TopOpeBRepDS_InterferenceTool::MakeFaceEdgeInterference
    (T1,iF2,iErest,isedge1,TopOpeBRepDS_UNSHGEOMETRY);
  myHDS->StoreInterference(IFE,iF1);

  IFE = TopOpeBRepDS_InterferenceTool::MakeFaceEdgeInterference
    (T2,iF1,iErest,isedge2,TopOpeBRepDS_UNSHGEOMETRY);
  myHDS->StoreInterference(IFE,iF2);

  //#################### Rline Processing ####################
  // xpu061098 
  TopOpeBRep_VPointInterIterator VPI;
  VPI.Init((*myLine));  
  Standard_Real tola = Precision::Angular()*1.e5;//NYIXPUTOL
  const TopOpeBRep_VPointInter& vpf = VPI.CurrentVP();
  for (; VPI.More(); VPI.Next()) {
    const TopOpeBRep_VPointInter& VP = VPI.CurrentVP();
    Standard_Integer absindex = VP.ShapeIndex(); // 0,1,2,3
    Standard_Real parRest;
    Standard_Boolean okR  = VP.ParonE(Erest,parRest);
    if (!okR) parRest = VP.ParameterOnLine();
    Standard_Boolean on2edges = (absindex == 3) || (absindex == OOrank);

    if (!on2edges) {
      // MSV: treat the case when an edge is touched by interior of a face

// MSV: the commented code below leads to exception on 1cto 025 H3
//      (object and tool have same subshapes), so to enable it
//      the debug is needed

//        Standard_Boolean SIisvertex = VP.IsVertex(EShapeIndex);
//        if (SIisvertex) continue;
//        Standard_Integer ShapeIndex = EShapeIndex;
//        Standard_Integer OOShapeIndex = (ShapeIndex == 1) ? 2 : 1;
//        TopoDS_Face OOFace = (*this).Face(OOShapeIndex);
//        Standard_Integer iOOFace = myDS->Shape(OOFace);

//        // make PVIndex
//        TopOpeBRepDS_Kind PVKind = TopOpeBRepDS_POINT;
//        Standard_Integer PVIndex = 0;
//        Standard_Boolean EPIfound=Standard_False,CPIfound=Standard_False;
//        Handle(TopOpeBRepDS_Interference) IEPI,ICPI;
//        FUN_VPIndex((*this),(*myLine),VP,ShapeIndex,myHDS,myDSCIL,
//                    PVKind,PVIndex,EPIfound,IEPI,CPIfound,ICPI,
//                    M_GETVP);
//        Standard_Boolean Efound = (EPIfound || CPIfound);
//        Standard_Boolean Ifound = (PVIndex != 0);
//        Standard_Boolean condmake = (!Efound && !Ifound);
//        if (condmake)
//          PVIndex = MakeGeometry(VP,ShapeIndex,PVKind);

//        // make transition on edge
//        TopOpeBRepDS_Transition transEdge;
//        TopOpeBRepDS_Transition Trans = FaceFaceTransition(ShapeIndex);
//        Standard_Boolean TransUNK = Trans.IsUnknown();
//        if (!TransUNK) {
//          TopAbs_Orientation oriErest = Erest.Orientation();
//          transEdge = TopOpeBRep_FFTransitionTool::ProcessLineTransition(VP,ShapeIndex,oriErest);
//  	TransUNK = FDS_hasUNK(transEdge);
//        }
//        if (TransUNK) {
//          Standard_Boolean ONfirstP = ::FUN_brep_ONfirstP(vpf,VP);
//          TopAbs_Orientation OVP = ONfirstP ? TopAbs_FORWARD : TopAbs_REVERSED;
//          transEdge.Set(OVP);
//          if (ONfirstP) transEdge.StateAfter(TopAbs_ON);
//          else          transEdge.StateBefore(TopAbs_ON);
//          TransUNK = FDS_hasUNK(transEdge);
//        }
//        if (TransUNK) continue;

//        // see if there is already such interference in DS
//        TopAbs_Orientation otransEdge = transEdge.Orientation(TopAbs_IN);
//        const TopOpeBRepDS_ListOfInterference& lIedge = myHDS->DS().ShapeInterferences(Erest);
//        TopOpeBRepDS_ListOfInterference copy; FDS_copy(lIedge,copy);
//        TopOpeBRepDS_ListOfInterference l1,l2;
//        Standard_Integer nfound = FUN_selectGIinterference(copy,PVIndex,l1);
//        if (nfound) {
//  	if (iOOFace != 0) {
//  	  TopOpeBRepDS_ListOfInterference l3,l4;
//            nfound = FUN_selectITRASHAinterference(l2,iOOFace,l3);
//  	  if (nfound != 0) nfound = FUN_selectTRAORIinterference(l3,otransEdge,l4);
//  	  if (nfound) continue;
//  	}
//        }

//        // make and store interference
//        Handle(TopOpeBRepDS_Interference) EPIf;
//        if (iOOFace == 0) iOOFace = myDS->AddShape(OOFace,OOShapeIndex);
//        TopOpeBRepDS_Transition T = transEdge; T.Index(iOOFace);
//        EPIf = MakeEPVInterference(T,iOOFace,PVIndex,parRest,PVKind,
//                                   TopOpeBRepDS_FACE,SIisvertex);
//        myHDS->StoreInterference(EPIf,Erest);

      continue;
    }

    TopoDS_Edge OOE = TopoDS::Edge(VP.Edge(OOrank)); 
    Standard_Integer iOO = myDS->AddShape(OOE,OOrank);

    Standard_Real OOpar; 
    VP.ParonE(OOE,OOpar);
 
    // xpu091198 : 1d interf done in EdgesFiller processing (cto cylcong *)     
    Standard_Boolean sdmeds = FUN_ds_sdm((*myDS),Erest,OOE);
    if (sdmeds) continue;
    
    Standard_Integer obRest = TopOpeBRepTool_TOOL::OnBoundary(parRest,Erest); //vertex can be missed
    Standard_Integer obOO   = TopOpeBRepTool_TOOL::OnBoundary(OOpar,OOE);     //vertex can be missed

    if ((obRest == EXTERNAL)||(obOO == EXTERNAL)){
#ifdef OCCT_DEBUG
      if(obRest==EXTERNAL) cout<<"***********ProcessRLine : faulty parameter on Erest"<<endl;
      if(obOO==EXTERNAL)   cout<<"***********ProcessRLine : faulty parameter on OOE"<<endl;
#endif
    }

    Standard_Boolean tgeds = FUN_tool_EtgOOE(parRest,Erest, OOpar,OOE, tola);

    TopOpeBRepDS_Kind PVKind = TopOpeBRepDS_POINT; Standard_Integer PVIndex = 0;  // POINT or VERTEX index

    for (Standard_Integer ShapeIndex = 1; ShapeIndex<=2; ShapeIndex++) {    
      Standard_Integer OOShapeIndex = (ShapeIndex == 1) ? 2 : 1;
      Standard_Boolean SIErest = (ShapeIndex == rank);
    
      Standard_Boolean SIisvertex = VP.IsVertex(ShapeIndex);
      Standard_Boolean OOisvertex = VP.IsVertex(OOShapeIndex);
      TopoDS_Face OOFace = (*this).Face(OOShapeIndex);
      Standard_Integer iOOFace = myDS->Shape(OOFace);

      TopoDS_Edge edge,OOedge; Standard_Integer OOedgeIndex;
      Standard_Real paredge;
      Standard_Integer onbound;
      if (SIErest) {
        edge = Erest;
        paredge = parRest;
        onbound = obRest;
        OOedge = OOE;
        OOedgeIndex = iOO;
      } else {
        OOedge = Erest;
        OOedgeIndex = iErest;
        onbound = obOO;
        edge = OOE;
        paredge = OOpar;
      }
      // PVIndex :
      // --------
      // xpu150399 : BUC60382
      Standard_Boolean EPIfound=Standard_False,CPIfound=Standard_False; Handle(TopOpeBRepDS_Interference) IEPI,ICPI;
      ProcessVPondgE(VP, ShapeIndex,
		     PVKind,PVIndex, // out
		     EPIfound,IEPI,  // out
		     CPIfound,ICPI); // out 
		     
      if (PVIndex == 0) {
//	Standard_Boolean EPIfound=Standard_False,CPIfound=Standard_False; Handle(TopOpeBRepDS_Interference) IEPI,ICPI;
	FUN_VPIndex((*this),(*myLine),VP,ShapeIndex,myHDS,myDSCIL,
		    PVKind,PVIndex,EPIfound,IEPI,CPIfound,ICPI,
		    M_GETVP);
	Standard_Boolean Efound = (EPIfound || CPIfound);
	Standard_Boolean Ifound = (PVIndex != 0);
	Standard_Boolean condmake = (!Efound && !Ifound);
	if (condmake) {
	  if      ( SIisvertex ) PVIndex = MakeGeometry(VP,ShapeIndex,PVKind);
	  else if ( OOisvertex ) PVIndex = MakeGeometry(VP,OOShapeIndex,PVKind);
	  else                   PVIndex = MakeGeometry(VP,ShapeIndex,PVKind);	
	}
      }      
  
      // transEdge : 
      // ----------
      if (OOedgeIndex == 0) OOedgeIndex = myDS->AddShape(OOedge,OOShapeIndex);
      const TopOpeBRepDS_Transition& llt1 = FaceFaceTransition(1);
      const TopOpeBRepDS_Transition& llt2 = FaceFaceTransition(2);
      TopOpeBRepDS_Transition Trans = (ShapeIndex == 1)? llt1 : llt2;
      Standard_Boolean TransUNK = Trans.IsUnknown();

      TopOpeBRepDS_Transition transEdge; Standard_Boolean Tunk = Standard_True;
      if (!TransUNK) { //xpu281098 PRO12875(edge9,OOface11)
	if ((absindex==ShapeIndex)||(absindex==3)) { 
	  if (SIErest) {
	    // transition on Erest at VP / OOface = transition at VP on Line restriction
	    TopAbs_Orientation oriErest = Erest.Orientation();
	    transEdge = TopOpeBRep_FFTransitionTool::ProcessLineTransition(VP,ShapeIndex,oriErest);
	    
	    if (((onbound == 1)||(onbound == 2))&&tgeds) // xpu290399 : edge is restriction, 
	      // edge15,OOedge14,OOface13		
	      {transEdge.Before(TopAbs_UNKNOWN); transEdge.After(TopAbs_UNKNOWN);}
	  }
	  else {
	    // transition on edge at VP / OOface ?= 
	    //  TopOpeBRep_FFTransitionTool::ProcessEdgeTransition(VP,ShapeIndex,Transori);  
	    // nyi
	  }
	}
	Tunk = FDS_hasUNK(transEdge);
      }
      if (Tunk) {
	if (SIErest) {
	  // As edge=Erest is on OOFace, we only compute 2d interferences
	  Standard_Boolean ONfirstP = ::FUN_brep_ONfirstP(vpf,VP);
	  TopAbs_Orientation OVP = ONfirstP ? TopAbs_FORWARD : TopAbs_REVERSED;
	  TopAbs_Orientation oOO; Standard_Boolean ok = FUN_tool_orientEinFFORWARD(OOedge,OOFace,oOO);
	  if (!ok) continue;
	  if (M_INTERNAL(oOO)) OVP = TopAbs_INTERNAL;

	  // xpu240399 : cto015I2 (e15,v16)
	  //             edge and OOedge are tangent, we do not keep the orientation
	  if (!tgeds) transEdge.Set(OVP);
	}
	else { 	
	  TopOpeBRepDS_Transition Tr;
	  Standard_Boolean ok = FUN_newtransEdge(myHDS,(*this),(*myLine),myLineIsonEdge,VP,PVKind,PVIndex,OOShapeIndex,
				  edge,myERL,Tr);
	  if (!ok) continue;
	  transEdge.Before(Tr.Before()); transEdge.After(Tr.After());  
	}
      }//Tunk
      Tunk = FDS_hasUNK(transEdge);
      if (Tunk) continue;

      TopAbs_Orientation otransEdge = transEdge.Orientation(TopAbs_IN);
      const TopOpeBRepDS_ListOfInterference& lIedge = myHDS->DS().ShapeInterferences(edge);
      TopOpeBRepDS_ListOfInterference copy; FDS_copy(lIedge,copy);
      TopOpeBRepDS_ListOfInterference l1,l2; Standard_Integer nfound = FUN_selectGIinterference(copy,PVIndex,l1);
      if (OOedgeIndex != 0) nfound = FUN_selectSIinterference(l1,OOedgeIndex,l2);
      if (nfound) {
	if (sdmeds) {
	  TopOpeBRepDS_ListOfInterference l3,l4; nfound = FUN_selectITRASHAinterference(l2,OOedgeIndex,l3);
	  if (nfound != 0) nfound = FUN_selectTRAORIinterference(l3,otransEdge,l4);
	  if (nfound) continue; // has I1d=(transEdge(OOedgeIndex),PVIndex,OOedgeIndex);
	}
	else if (iOOFace != 0) {
	  TopOpeBRepDS_ListOfInterference l3,l4; nfound = FUN_selectITRASHAinterference(l2,iOOFace,l3);
	  if (nfound != 0) nfound = FUN_selectTRAORIinterference(l3,otransEdge,l4);
	  if (nfound) continue; // has I2d=(transEdge(iOOFace),PVIndex,OOedgeIndex)
	}  
      }// nfound
      
      // EPI : 
      // ----
      Handle(TopOpeBRepDS_Interference) EPI;
      {	
	if (iOOFace == 0) iOOFace = myDS->AddShape(OOFace,OOShapeIndex);
	TopOpeBRepDS_Transition T = transEdge; T.Index(iOOFace);
	EPI = MakeEPVInterference(T,OOedgeIndex,PVIndex,paredge,PVKind,SIisvertex);
      }
      myHDS->StoreInterference(EPI,edge);     

      // EPIf : 
      // -----
      if (!SIErest) {
	Handle(TopOpeBRepDS_Interference) EPIf;
	TopOpeBRepDS_Transition T = transEdge; T.Index(iOOFace);
	EPIf = MakeEPVInterference(T,iOOFace,PVIndex,paredge,PVKind,
				   TopOpeBRepDS_FACE,SIisvertex);
	myHDS->StoreInterference(EPIf,edge); 	
      }

    } // ShapeIndex=1..2
  } // VPI
  //####################
}

static Standard_Boolean FUN_haslastvpon0(const TopOpeBRep_LineInter& L)
{
  const Standard_Boolean wline = (L.TypeLineCurve() == TopOpeBRep_WALKING);
  if (!wline) return Standard_False;
  
  Standard_Integer iINON1,iINONn,nINON; L.VPBounds(iINON1,iINONn,nINON);
  
  TopOpeBRep_VPointInterIterator VPI;
  VPI.Init(L);  
  for (; VPI.More(); VPI.Next()) {
    const TopOpeBRep_VPointInter& VP = VPI.CurrentVP();
    const Standard_Integer absindex = VP.ShapeIndex();
    const Standard_Integer iVP = VP.Index();
    if (iVP == iINONn && absindex == 0) return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : FillLineVPonR
//purpose  : 
//=======================================================================
void TopOpeBRep_FacesFiller::FillLineVPonR()
{
#ifdef OCCT_DEBUG
  Standard_Boolean trline = Standard_False;
#endif
  // if a VP is on degenerated edge, adds the triplet
  // (vertex, closing edge, degenerated edge) to the
  // map as vertex for key.
//  myDataforDegenEd.Clear();
  FUN_GetdgData(myDS,(*myLine),myF1,myF2,myDataforDegenEd);
  FUN_FillVof12((*myLine),myDS) ;
  
  mykeptVPnbr = 0; 
  
  if (myLine->TypeLineCurve() == TopOpeBRep_RESTRICTION) { 
#ifdef OCCT_DEBUG
    if (trline) FUN_traceRLine(*myLine);
#endif
    ProcessRLine();
    return;
  }
  
  Standard_Integer iINON1,iINONn,nINON;
  myLine->VPBounds(iINON1,iINONn,nINON);
  if ( nINON == 0 ) {
    return; 
  }
   
#ifdef OCCT_DEBUG
  if (trline) FUN_traceGLine(*myLine);
#endif
  myLineIsonEdge = LSameDomainERL(*myLine, myERL);
  
  // walking (case mouch1a 1 1) : line (vpfirst on 3,vplast on 0,nvpkept = 2) => kept
  myLastVPison0 = ::FUN_haslastvpon0(*myLine);

  //----------------------------------------------------------------------  // IMPORTANT : 
  // Some of Curve/Point transitions for vpoints keep on RESTRICTION lines
  // sharing same domain with the current geometric line are computed here
  //----------------------------------------------------------------------
  
#ifdef OCCT_DEBUG
#ifdef DRAW
  Standard_Boolean trcd = Standard_False;
  if (trcd) FUN_DrawMap(myDataforDegenEd);
#endif
#endif
  
  TopOpeBRep_VPointInterIterator VPI;
  VPI.Init((*myLine));  
  for (; VPI.More(); VPI.Next()) {
    const TopOpeBRep_VPointInter& VP = VPI.CurrentVP();
    ProcessVPR((*this),VP);
  }
  
  if ( myLineIsonEdge && (!myDSCIL.IsEmpty()) ) {
    myDSCIL.Clear();
  }
}

//=======================================================================
//function : FillLine
//purpose  : 
//=======================================================================
void TopOpeBRep_FacesFiller::FillLine()
{
  Standard_Integer iINON1,iINONn,nINON;
  myLine->VPBounds(iINON1,iINONn,nINON);
  if ( nINON == 0 ) return; 
  
  Standard_Integer ShapeIndex = 0;
  Handle(TopOpeBRepDS_Interference) CPI;
  
  TopOpeBRep_VPointInterIterator VPI;
  for (VPI.Init((*myLine)); VPI.More(); VPI.Next()) {
    
    const TopOpeBRep_VPointInter& VP = VPI.CurrentVP();
    if ( ! VP.Keep() ) continue;

    Standard_Integer PVIndex;
    TopOpeBRepDS_Kind    PVKind;
    TopOpeBRepDS_ListIteratorOfListOfInterference itCPIL(myDSCIL);
    Standard_Boolean CPIfound;
    CPIfound = GetGeometry(itCPIL,VP,PVIndex,PVKind);
    if ( ! CPIfound ) {
      Standard_Boolean found = GetFFGeometry(VP,PVKind,PVIndex);
      if ( !found ) PVIndex = MakeGeometry(VP,ShapeIndex,PVKind);
    }
    
    TopOpeBRepDS_Transition transLine;
    if (! CPIfound) {
      Standard_Integer iVP = VPI.CurrentVPIndex();
      if      (iVP == iINON1) transLine.Set(TopAbs_FORWARD);
      else if (iVP == iINONn) transLine.Set(TopAbs_REVERSED);
    }
    else transLine = itCPIL.Value()->Transition().Complement();
    
    Standard_Real parline = VPI.CurrentVP().ParameterOnLine();
    CPI = TopOpeBRepDS_InterferenceTool::MakeCurveInterference
      (transLine,TopOpeBRepDS_CURVE,0,PVKind,PVIndex,parline);
    StoreCurveInterference(CPI);
    
  } //   loop on VPoints
  
} // FillLine

//=======================================================================
//function : AddShapesLine
//purpose  : 
//=======================================================================
void TopOpeBRep_FacesFiller::AddShapesLine()
{
  Standard_Boolean dscilemp = myDSCIL.IsEmpty();
  if (dscilemp) return;
  
  Standard_Boolean inl = myLine->INL();
  if (inl) return;
  
  TopOpeBRepDS_Curve& DSC = myDS->ChangeCurve(myDSCIndex);
  
  Handle(Geom_Curve) C3D;
  Handle(Geom2d_Curve) PC1,PC2;
  Handle(TopOpeBRepDS_Interference) FCI1, FCI2;
  
  Standard_Integer iF1 = myDS->AddShape(myF1,1);
  Standard_Integer iF2 = myDS->AddShape(myF2,2);
  
  Standard_Real pmin,pmax;
  myHDS->MinMaxOnParameter(myDSCIL,pmin,pmax);
  
  Standard_Real d = Abs(pmin-pmax);
  Standard_Boolean id = (d <= Precision::PConfusion());
  Standard_Boolean isper = myLine->IsPeriodic(); 
  id = (id && !isper);
 
  Standard_Boolean wline = (myLine->TypeLineCurve() == TopOpeBRep_WALKING);
  Standard_Boolean vclosed = myLine->IsVClosed();
  if (wline && !isper && vclosed) {
    //xpu240399 : USA60298 : avoid creating curve
    // MSV: take into account that geometry can be of type VERTEX
    Standard_Integer ipf = myDSCIL.First()->Geometry();
    TopOpeBRepDS_Kind kpf = myDSCIL.First()->GeometryType();
    gp_Pnt ptf;
    Standard_Real tol,tolf, toll;
    if (kpf == TopOpeBRepDS_POINT) {
      TopOpeBRepDS_Point pf = myDS->Point(ipf);
      ptf = pf.Point();
      tolf = pf.Tolerance();
    }
    else { // VERTEX
      TopoDS_Vertex vf = TopoDS::Vertex(myDS->Shape(ipf));
      ptf = BRep_Tool::Pnt(vf);
      tolf = BRep_Tool::Tolerance(vf);
    }

    Standard_Integer ipl = myDSCIL.Last()->Geometry();
    TopOpeBRepDS_Kind kpl = myDSCIL.Last()->GeometryType();
    if (kpl == TopOpeBRepDS_POINT) {
      TopOpeBRepDS_Point pl = myDS->Point(ipl); 
      toll = pl.Tolerance();
    }
    else { // VERTEX
      TopoDS_Vertex vl = TopoDS::Vertex(myDS->Shape(ipl));
      toll = BRep_Tool::Tolerance(vl);
    }

    tol = Max(tolf, toll);
    Standard_Boolean onsampt = Standard_True;
    for (Standard_Integer ii = 1; ii <= myLine->NbWPoint(); ii++) {
      TopOpeBRep_WPointInter wp = myLine->WPoint(ii);
      gp_Pnt pp = wp.Value();
      if (!pp.IsEqual(ptf,tol)) {onsampt = Standard_False;break;}
    }
    if (onsampt) id = Standard_True;
  }

  if (id) {
    DSC.ChangeKeep(Standard_False);
    return;
  }
  
  TopOpeBRep_GeomTool::MakeCurves(pmin,pmax,(*myLine),myF1,myF2,DSC,PC1,PC2);

  //Patch: avoid making too small edges. Made for the bug buc60926 by jgv, 14.06.01.
  Standard_Real fpar, lpar;
  DSC.Range(fpar, lpar);
  GeomAdaptor_Curve theCurve( DSC.Curve(), fpar, lpar );
  Bnd_Box theBox;
  BndLib_Add3dCurve::Add( theCurve, 0., theBox );
  Standard_Real Xmin, Ymin, Zmin, Xmax, Ymax, Zmax, MaxSide;
  theBox.Get( Xmin, Ymin, Zmin, Xmax, Ymax, Zmax );
  MaxSide = Max( Max(Xmax-Xmin, Ymax-Ymin), Zmax-Zmin );
  Standard_Real MinTol = Min( BRep_Tool::Tolerance(myF1), BRep_Tool::Tolerance(myF2) );
  if (MaxSide < MinTol)
    {
      DSC.ChangeKeep(Standard_False);
      return;
    }

  Standard_Real tolDSC = 1.e-8;
  DSC.Tolerance(tolDSC);
  const TopOpeBRepDS_Transition& lllt1 = FaceFaceTransition(1);
  const TopOpeBRepDS_Transition& lllt2 = FaceFaceTransition(2);
  
  myDS->ChangeCurveInterferences(myDSCIndex).Append(myDSCIL);
  {
    TopOpeBRepDS_Transition T1 = lllt1; T1.Index(iF2);
    FCI1 = TopOpeBRepDS_InterferenceTool::MakeFaceCurveInterference
      (T1,iF2,myDSCIndex,PC1);
    myHDS->StoreInterference(FCI1,myF1);
  }
  
  {
    TopOpeBRepDS_Transition T2 = lllt2; T2.Index(iF1);
    FCI2 = TopOpeBRepDS_InterferenceTool::MakeFaceCurveInterference
      (T2,iF1,myDSCIndex,PC2);
    myHDS->StoreInterference(FCI2,myF2);
  }
  
  DSC.SetShapes(myF1,myF2);
  DSC.SetSCI(FCI1,FCI2);
  
}

//=======================================================================
//function : StoreCurveInterference
//purpose  : private
//=======================================================================
void TopOpeBRep_FacesFiller::StoreCurveInterference(const Handle(TopOpeBRepDS_Interference)& I)
{
  if ( myDSCIndex == 0 ) {
    TopOpeBRepDS_Curve DSC;
    myDSCIndex = myDS->AddCurve(DSC);
  }
  
  I->Support(myDSCIndex);
  myHDS->StoreInterference(I,myDSCIL);
}

//=======================================================================
//function : GetGeometry
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRep_FacesFiller::GetGeometry(TopOpeBRepDS_ListIteratorOfListOfInterference& IT,const TopOpeBRep_VPointInter& VP,Standard_Integer& G,TopOpeBRepDS_Kind& K)
{
  TopOpeBRepDS_Point DSP = TopOpeBRep_PointGeomTool::MakePoint(VP);
  Standard_Boolean b = myHDS->GetGeometry(IT,DSP,G,K);
  return b;
}

//=======================================================================
//function : MakeGeometry
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRep_FacesFiller::MakeGeometry(const TopOpeBRep_VPointInter& VP,const Standard_Integer ShapeIndex,TopOpeBRepDS_Kind& K)
{
  Standard_Integer G;
  Standard_Boolean isvertex = VP.IsVertex(ShapeIndex);
  if ( isvertex ) {
    const TopoDS_Shape& S = VP.Vertex(ShapeIndex);
    G = myDS->AddShape(S,ShapeIndex);
    K = TopOpeBRepDS_VERTEX;
  }
  else {
    TopOpeBRepDS_Point P = TopOpeBRep_PointGeomTool::MakePoint(VP);
    G = myDS->AddPoint(P);
    K = TopOpeBRepDS_POINT;
  }
  
  return G;
}

//=======================================================================
//function : GetFFGeometry
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRep_FacesFiller::GetFFGeometry(const TopOpeBRepDS_Point& DSP,TopOpeBRepDS_Kind& K,Standard_Integer& G) const 
{
  Standard_Boolean found = Standard_False;
  Standard_Integer i = myFFfirstDSP, n = myDS->NbPoints();
  for (; i <= n; i++) {
    const TopOpeBRepDS_Point& OODSP = myDS->Point(i);
    found = TopOpeBRep_PointGeomTool::IsEqual(DSP,OODSP);
    if (found) break; 
  }
  if ( found ) {
    K = TopOpeBRepDS_POINT;
    G = i;
  }
  return found;
}

//=======================================================================
//function : GetFFGeometry
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRep_FacesFiller::GetFFGeometry(const TopOpeBRep_VPointInter& VP,TopOpeBRepDS_Kind& K,Standard_Integer& G) const 
{
  TopOpeBRepDS_Point DSP = TopOpeBRep_PointGeomTool::MakePoint(VP);
  Standard_Boolean found = GetFFGeometry(DSP,K,G);
  return found;
}

