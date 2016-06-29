// Created on: 1996-03-07
// Created by: Jean Yves LEBEY
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


#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_Builder.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_EdgeBuilder.hxx>
#include <TopOpeBRepBuild_FaceBuilder.hxx>
#include <TopOpeBRepBuild_GTopo.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepBuild_PaveSet.hxx>
#include <TopOpeBRepBuild_ShapeSet.hxx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>
#include <TopOpeBRepBuild_SolidBuilder.hxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_InterferenceTool.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>
#include <TopOpeBRepDS_SurfaceIterator.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#ifdef DRAW
Standard_IMPORT void FUN_draw(const TopoDS_Shape& s);
Standard_IMPORT void FUN_draw2de (const TopoDS_Shape& ed,const TopoDS_Shape& fa);
#endif

#ifdef OCCT_DEBUG
extern void* GFABUMAKEFACEPWES_DEB;
#define DEBSHASET(sarg,meth,shaset,str) TCollection_AsciiString sarg((meth));(sarg)=(sarg)+(shaset).DEBNumber()+(str);
Standard_Integer GLOBAL_iexE = 0;
Standard_EXPORT void debfillw(const Standard_Integer /*i*/) {}
Standard_EXPORT void debfille(const Standard_Integer /*i*/) {}
Standard_EXPORT void debffwesON(const Standard_Integer i) {cout<<"++ debffwesON "<<i<<endl;}
Standard_EXPORT void debffwesmf(const Standard_Integer i) {cout<<"++ debffwesmf "<<i<<endl;}
Standard_EXPORT void debfillf(const Standard_Integer i) {cout<<"++ debfillf "<<i<<endl;}
Standard_EXPORT void debsplite(const Standard_Integer i) {cout<<"++ debsplite "<<i<<endl;}
Standard_EXPORT void debmergef(const Standard_Integer i) {cout<<"++ debmergef "<<i<<endl;}
Standard_IMPORT void debfctwesmess(const Standard_Integer i,
				   const TCollection_AsciiString& s = "");
extern void debaddpwes(const Standard_Integer iFOR, const TopAbs_State TB1, const Standard_Integer iEG,
                       const TopAbs_Orientation neworiE, const TopOpeBRepBuild_PBuilder& PB,
                       const TopOpeBRepBuild_PWireEdgeSet& PWES, const TCollection_AsciiString& str1, 
                       const TCollection_AsciiString& str2);
#endif

Standard_Boolean GLOBAL_faces2d = Standard_False;
Standard_EXPORT Standard_Boolean GLOBAL_classifysplitedge = Standard_False;  

#define M_IN(st )      (st == TopAbs_IN)
#define M_OUT(st)      (st == TopAbs_OUT)
#define M_FORWARD(st ) (st == TopAbs_FORWARD)
#define M_REVERSED(st) (st == TopAbs_REVERSED)
#define M_INTERNAL(st) (st == TopAbs_INTERNAL)
#define M_EXTERNAL(st) (st == TopAbs_EXTERNAL)

Standard_IMPORT Standard_Boolean FUN_HDS_FACESINTERFER(const TopoDS_Shape& F1, 
						       const TopoDS_Shape& F2,
						       const Handle(TopOpeBRepDS_HDataStructure)& HDS);

static 
  TopAbs_State ClassifyEdgeToSolidByOnePoint(const TopoDS_Edge& E,
					     const TopoDS_Shape& Ref);
static
  Standard_Boolean FUN_computeLIFfaces2d(const TopOpeBRepBuild_Builder& BU, 
					 const TopoDS_Face& F, 
					 const TopoDS_Edge& E, 
					 TopOpeBRepDS_PDataStructure& pDS2d);
static 
  Standard_Boolean FUN_computeLIFfaces2d(const TopOpeBRepBuild_Builder& BU, 
					 const TopoDS_Face& F, 
					 TopOpeBRepDS_PDataStructure& pDS2d);

//-------------------------------------------------------------
// Unused :
/*#ifdef OCCT_DEBUG
//=======================================================================
//function :FUN_BUI_FACESINTERFER
//purpose  : 
//=======================================================================
static Standard_Boolean FUN_BUI_FACESINTERFER(const TopoDS_Shape& F1,
					      const TopoDS_Shape& F2,
					      const TopOpeBRepBuild_Builder& B)
{
  Standard_Boolean yainterf = Standard_False;
  Handle(TopOpeBRepDS_HDataStructure) HDS = B.DataStructure();
  


  Standard_Boolean ya1 = FUN_HDS_FACESINTERFER(F1,F2,HDS);
  Standard_Boolean ya2 = FUN_HDS_FACESINTERFER(F2,F1,HDS);
  yainterf = (ya1 && ya2);
  return yainterf;
}
#endif*/

//=======================================================================
//function :TopOpeBRepBuild_FUN_aresamegeom
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_FUN_aresamegeom (const TopoDS_Shape& S1,
                                                  const TopoDS_Shape& S2)
{
  TopoDS_Shape SF1 = S1; 
  SF1.Orientation(TopAbs_FORWARD);
  TopoDS_Shape SF2 = S2; 
  SF2.Orientation(TopAbs_FORWARD);
  Standard_Boolean same = TopOpeBRepTool_ShapeTool::ShapesSameOriented(SF1,SF2);
  return same;
}

//=======================================================================
//function :FUN_computeLIFfaces2d
//purpose  : 
//=======================================================================
Standard_Boolean FUN_computeLIFfaces2d(const TopOpeBRepBuild_Builder& BU, 
				       const TopoDS_Face& F, 
				       const TopoDS_Edge& E, 
				       TopOpeBRepDS_PDataStructure& pDS2d)
// purpose : compute new face/face interferences F FTRA,
//  {I = (T(F),ES,FTRA)} / Fsdm F and ES interfers with E which has splits ON
//  E is edge of F
{ 
  const TopOpeBRepDS_DataStructure& BDS = BU.DataStructure()->DS(); 
  const TopOpeBRepDS_ListOfInterference& LI = BDS.ShapeInterferences(E);
  Standard_Integer IE = BDS.Shape(E);
  Standard_Integer IF = BDS.Shape(F);
  Standard_Integer rkF = BDS.AncestorRank(F);
  Standard_Boolean hasspE = BU.IsSplit(E,TopAbs_ON);
  if (hasspE) hasspE = (BU.Splits(E,TopAbs_ON).Extent() > 0);
  TopTools_MapOfShape Ffound;
  
  TopTools_ListOfShape Fsdm; TopTools_ListIteratorOfListOfShape itf(BDS.ShapeSameDomain(F));
  for (; itf.More(); itf.Next()){
    const TopoDS_Shape& f = itf.Value();
    Standard_Integer rkf = BDS.AncestorRank(f);
    if (rkf == rkF) continue;
    Fsdm.Append(f);
  }

  for (TopOpeBRepDS_ListIteratorOfListOfInterference itI(LI); itI.More(); itI.Next()){
    const Handle(TopOpeBRepDS_Interference)& I = itI.Value();
//    const TopOpeBRepDS_Transition& T = I->Transition();
    TopAbs_ShapeEnum SB,SA;Standard_Integer IB,IA;TopOpeBRepDS_Kind GT,ST;Standard_Integer G,S;
    FDS_Idata(I,SB,IB,SA,IA,GT,G,ST,S);
    if (ST != TopOpeBRepDS_EDGE) return Standard_False;

    TopoDS_Face FTRA; Standard_Integer ITRA = IB;
    if      (SB == TopAbs_FACE) FTRA = TopoDS::Face(BDS.Shape(IB));
    else if (SB == TopAbs_EDGE) {
      Standard_Boolean ok = FUN_tool_findAncestor(Fsdm,TopoDS::Edge(BDS.Shape(S)),FTRA);
      ITRA = BDS.Shape(FTRA);
      if (!ok) return Standard_False;      
    }
    Standard_Boolean found = Ffound.Contains(FTRA);

    // prequesitory : F and FTRA are SDSO
    // -------------

    // attached to E : I = (T(FTRA),G,ES),
    // ES : support edge
    // GP : geometric point  
    // recall : rankE  = rankF
    //          rankTRA = rankS != rankE
    Standard_Real parE = FDS_Parameter(I);
    const TopoDS_Edge& ES = TopoDS::Edge(BDS.Shape(S));
    Standard_Boolean hasspES = BU.IsSplit(ES,TopAbs_ON);
    if (hasspES) hasspE = (BU.Splits(ES,TopAbs_ON).Extent() > 0); 

    Standard_Boolean sdm = FUN_ds_sdm(BDS,E,ES);
    Standard_Boolean mkTonEsdm = sdm && hasspE && !found;
    Standard_Boolean hasfeiF_E_FTRA = FUN_ds_hasFEI(pDS2d,F,IE,ITRA);  //xpu120698
    mkTonEsdm = mkTonEsdm && !hasfeiF_E_FTRA; //xpu120698
    if (mkTonEsdm) {
      Ffound.Add(FTRA);
      TopoDS_Edge dummy; TopOpeBRepDS_Transition newT;  
      Standard_Boolean ok = FUN_ds_mkTonFsdm(BU.DataStructure(),IF,ITRA,S,IE,parE,dummy,Standard_True,newT);

      if (ok) {
	newT.Index(ITRA); TopOpeBRepDS_Config C = TopOpeBRepDS_SAMEORIENTED;
	Handle(TopOpeBRepDS_Interference) newI = TopOpeBRepDS_InterferenceTool::MakeFaceEdgeInterference(newT,ITRA,IE,Standard_True,C);
	pDS2d->AddShapeInterference(F,newI);
      }
    }
    Standard_Boolean mkTonESsdm = sdm && hasspES;
    Standard_Boolean hasfeiFRA_E_F = FUN_ds_hasFEI(pDS2d,FTRA,IE,IF);  //xpu120698
    mkTonESsdm = mkTonESsdm && !hasfeiFRA_E_F;  //xpu120698
    if (mkTonESsdm) { // ff1, IE=3 has interferences, S=8 has none
      TopoDS_Edge dummy; TopOpeBRepDS_Transition newT; 

      Standard_Real parES; Standard_Boolean ok = FUN_tool_parE(E,parE,ES,parES);
      if (!ok) continue; 
      ok = FUN_ds_mkTonFsdm(BU.DataStructure(),ITRA,IF,IE,S,parES,dummy,Standard_True,newT);
      if (ok) {
	newT.Index(IF); TopOpeBRepDS_Config C = TopOpeBRepDS_SAMEORIENTED;
	Handle(TopOpeBRepDS_Interference) newI = TopOpeBRepDS_InterferenceTool::MakeFaceEdgeInterference(newT,IF,IE,Standard_False,C);
	pDS2d->AddShapeInterference(FTRA,newI);
      }

      ok = FUN_ds_mkTonFsdm(BU.DataStructure(),ITRA,IF,IE,IE,parE,dummy,Standard_True,newT);      
      if (ok) {
	newT.Index(IF); TopOpeBRepDS_Config C = TopOpeBRepDS_SAMEORIENTED;
	Handle(TopOpeBRepDS_Interference) newI = TopOpeBRepDS_InterferenceTool::MakeFaceEdgeInterference(newT,IF,S,Standard_True,C);
	pDS2d->AddShapeInterference(FTRA,newI);
      }
      
    }

    Standard_Boolean mkTonES = hasspES;
    Standard_Boolean hasfeiF_S_FTRA = FUN_ds_hasFEI(pDS2d,F,S,ITRA);  //xpu120698
    mkTonES = mkTonES && !hasfeiF_S_FTRA;
    if (mkTonES) {
      Standard_Real parES; Standard_Boolean ok = FUN_tool_parE(E,parE,ES,parES);
      if (!ok) continue;
      
      TopoDS_Edge dummy; TopOpeBRepDS_Transition newT; 
      ok = FUN_ds_mkTonFsdm(BU.DataStructure(),IF,ITRA,S,S,parES,dummy,Standard_True,newT);

      if (ok) {
	newT.Index(ITRA); TopOpeBRepDS_Config C = TopOpeBRepDS_SAMEORIENTED;
	Handle(TopOpeBRepDS_Interference) newI = TopOpeBRepDS_InterferenceTool::MakeFaceEdgeInterference(newT,ITRA,S,Standard_False,C);
	pDS2d->AddShapeInterference(F,newI);
      }
    }
  } // itI(LI)

  return Standard_True;
}
//=======================================================================
//function :FUN_computeLIFfaces2d
//purpose  : 
//=======================================================================
Standard_Boolean FUN_computeLIFfaces2d(const TopOpeBRepBuild_Builder& BU, 
				       const TopoDS_Face& F,
				       TopOpeBRepDS_PDataStructure& pDS2d)
{
  TopExp_Explorer ex(F, TopAbs_EDGE);   
  for (; ex.More(); ex.Next()){
    const TopoDS_Edge& E = TopoDS::Edge(ex.Current());
    Standard_Boolean ok = FUN_computeLIFfaces2d(BU,F,E,pDS2d);
    if (!ok) return Standard_False;
  }
  return Standard_True;
}
//=======================================================================
//variable : Standard_EXPORT TopOpeBRepDS_PDataStructure GLOBAL_DS2d
//purpose  : 
//=======================================================================
Standard_EXPORT TopOpeBRepDS_PDataStructure GLOBAL_DS2d = NULL;

//=======================================================================
//function : GMergeFaces
//purpose  : 
//=======================================================================
  void TopOpeBRepBuild_Builder::GMergeFaces(const TopTools_ListOfShape& LF1,
					    const TopTools_ListOfShape& LF2,
					    const TopOpeBRepBuild_GTopo& G1)
{
  if ( LF1.IsEmpty() ) return;
  if (GLOBAL_DS2d == NULL) GLOBAL_DS2d = (TopOpeBRepDS_PDataStructure)new TopOpeBRepDS_DataStructure();
  GLOBAL_DS2d->Init();

  TopAbs_State TB1,TB2; G1.StatesON(TB1,TB2);

  const TopoDS_Shape& F1 = LF1.First();
#ifdef OCCT_DEBUG
  Standard_Integer iF; Standard_Boolean tSPS = GtraceSPS(F1,iF);
  if(tSPS){
    cout<<endl<<"--- GMergeFaces "<<endl;
    GdumpSAMDOM(LF1, (char *) "1 : ");
    GdumpSAMDOM(LF2, (char *) "2 : ");
    debmergef(iF);
  }
#endif

  // xpu070598 : filling up DS2
//  for (TopTools_ListIteratorOfListOfShape itF1(LF1); itF1.More(); itF1.Next()) GLOBAL_DS2d->AddShape(itF1.Value(),1); 
   TopTools_ListIteratorOfListOfShape itF1(LF1) ;
  for ( ; itF1.More(); itF1.Next()) GLOBAL_DS2d->AddShape(itF1.Value(),1);  
//  for (TopTools_ListIteratorOfListOfShape itF2(LF2); itF2.More(); itF2.Next()) GLOBAL_DS2d->AddShape(itF2.Value(),2);
  TopTools_ListIteratorOfListOfShape itF2(LF2) ;
  for ( ; itF2.More(); itF2.Next()) GLOBAL_DS2d->AddShape(itF2.Value(),2);

//  for (itF1.Initialize(LF1); itF1.More(); itF1.Next()){
  itF1.Initialize(LF1) ;
  for ( ; itF1.More(); itF1.Next()){
    const TopoDS_Face& FF1 = TopoDS::Face(itF1.Value());
    FUN_computeLIFfaces2d((*this),TopoDS::Face(FF1),GLOBAL_DS2d);
  }
//  for (itF2.Initialize(LF2); itF2.More(); itF2.Next()){
  itF2.Initialize(LF2) ;
  for ( ; itF2.More(); itF2.Next()){
    const TopoDS_Face& FF2 = TopoDS::Face(itF2.Value());
    FUN_computeLIFfaces2d((*this),TopoDS::Face(FF2),GLOBAL_DS2d);
  }
  // xpu070598

  {
    for (Standard_Integer ii=1; ii<=GLOBAL_DS2d->NbShapes(); ii++) {
      TopOpeBRepDS_ListOfInterference& LI = GLOBAL_DS2d->ChangeShapeInterferences(ii);
      FUN_reducedoublons(LI,(*GLOBAL_DS2d),ii);
    }
  }

  myFaceReference = TopoDS::Face(F1);
  TopOpeBRepBuild_WireEdgeSet WES(F1,this);

  GLOBAL_faces2d = Standard_True;
  Standard_Integer K1=1; GFillFacesWESK(LF1,LF2,G1,WES,K1);
  Standard_Integer K3=3; GFillFacesWESK(LF1,LF2,G1,WES,K3); // xpu060598
  GLOBAL_faces2d = Standard_False;

  // Create a face builder FABU
  TopoDS_Shape F1F = LF1.First(); F1F.Orientation(TopAbs_FORWARD);
  Standard_Boolean ForceClass = Standard_True;
  TopOpeBRepBuild_FaceBuilder FABU;
  FABU.InitFaceBuilder(WES,F1F,ForceClass);
  
  // Build new faces LFM
  TopTools_ListOfShape LFM;

#ifdef OCCT_DEBUG
  GFABUMAKEFACEPWES_DEB = (void*)&WES;
#endif

  TopTools_DataMapOfShapeInteger MWisOld;
  GFABUMakeFaces(F1F,FABU,LFM,MWisOld);
  
  // xpu281098 : regularisation after GFABUMakeFaces
  TopTools_ListOfShape newLFM; RegularizeFaces(F1F,LFM,newLFM);
  LFM.Clear(); LFM.Assign(newLFM);

  // connect new faces as faces built TB1 on LF1 faces
  TopTools_ListIteratorOfListOfShape it1;
  for (it1.Initialize(LF1); it1.More(); it1.Next()) {
    const TopoDS_Shape& F1x = it1.Value();
    Standard_Boolean tomerge = !IsMerged(F1x,TB1);
    if (tomerge) {
      ChangeMerged(F1x, TB1) = LFM;
    }
  }
  
  // connect new faces as faces built TB2 on LF2 faces
  TopTools_ListIteratorOfListOfShape it2;
  for (it2.Initialize(LF2); it2.More(); it2.Next()) {
    const TopoDS_Shape& F2 = it2.Value();
    Standard_Boolean tomerge = !IsMerged(F2,TB2);
    if (tomerge) ChangeMerged(F2,TB2) = LFM;
  }
  
} // GMergeFaces

//=======================================================================
//function : GFillFacesWES
//purpose  : 
//=======================================================================
  void TopOpeBRepBuild_Builder::GFillFacesWES(const TopTools_ListOfShape& ,
					      const TopTools_ListOfShape& ,
					      const TopOpeBRepBuild_GTopo& ,
					      TopOpeBRepBuild_WireEdgeSet& )
{
} // GFillFacesWES

static Standard_Boolean FUN_validF1edge(const TopoDS_Shape& F)
{
  Standard_Integer nE = 0; 
  TopTools_IndexedMapOfShape mEt;
  TopExp_Explorer exE(F, TopAbs_EDGE);
//  for ( exE ; exE.More(); exE.Next()) {
  for (  ; exE.More(); exE.Next()) {
    const TopoDS_Shape& e = exE.Current();
    if (mEt.Contains(e)) continue;
    mEt.Add(e);
    nE++; 
    if (nE > 2) break;
  }
  if (nE > 1) return Standard_True;
  if (nE == 1) {
    exE.Init(F, TopAbs_EDGE);
    const TopoDS_Edge& e = TopoDS::Edge(exE.Current());
    TopoDS_Vertex dummy; Standard_Boolean closed = TopOpeBRepTool_TOOL::ClosedE(e,dummy);
    return closed;
  }
  return Standard_False;
}

//=======================================================================
//function : GFillFacesWESMakeFaces
//purpose  : 
//=======================================================================
  void TopOpeBRepBuild_Builder::GFillFacesWESMakeFaces(const TopTools_ListOfShape& LLF1,
						       const TopTools_ListOfShape& LF2,
						       const TopTools_ListOfShape& ,//LSO,
						       const TopOpeBRepBuild_GTopo& GM)
{
  TopAbs_State TB1,TB2; GM.StatesON(TB1,TB2);
  if (LLF1.IsEmpty()) return;
  
  // xpu270898 : cto905E2 split(fref6,f33,f16) must be built on fref6
  TopTools_ListOfShape LF1;
  TopTools_ListIteratorOfListOfShape itf(LLF1);
  const TopOpeBRepDS_DataStructure& BDS = myDataStructure->DS();
  Standard_Integer iref = 0; 
  for (; itf.More(); itf.Next()){
    const TopoDS_Shape& fcur = itf.Value();
    Standard_Integer icur = BDS.Shape(fcur);
    iref = BDS.SameDomainRef(fcur);
    if (icur == iref) LF1.Prepend(fcur);
    else              LF1.Append(fcur);
  }
  // xpu270898 : cto905I1 split(f6,f30,fref14) must be built on fref6, f6 is in LFDO1
//  Standard_Boolean FFinDO1 = (iFF == iref);
//  const TopoDS_Shape& FF = BDS.Shape(iref);
  const TopoDS_Shape& FF = LF1.First().Oriented(TopAbs_FORWARD);
  Standard_Integer iFF = BDS.Shape(FF);

  TopOpeBRepBuild_WireEdgeSet WES(FF,this);

#ifdef OCCT_DEBUG
  Standard_Integer iF; Standard_Boolean tSPS = GtraceSPS(FF,iF);
  if(tSPS) GdumpSHASTA(iF,TB1,WES,"\n--- GFillFacesWESMakeFaces");
  if(tSPS) debfillf(iF);
  if(tSPS) debffwesmf(iF);
#endif

  Standard_Integer n1 = 0;
  GLOBAL_faces2d = Standard_True;
  Standard_Integer K1=1; GFillFacesWESK(LF1,LF2,GM,WES,K1);
  GLOBAL_faces2d = Standard_False;
  n1 = WES.StartElements().Extent();
  
  Standard_Integer K2=2; GFillFacesWESK(LF1,LF2,GM,WES,K2);
  n1 = WES.StartElements().Extent();
  
  Standard_Integer K3=3; GFillFacesWESK(LF1,LF2,GM,WES,K3);
  n1 = WES.StartElements().Extent();

  Standard_Integer n2 = WES.StartElements().Extent();
  myEdgeAvoid.Clear(); // Start edges dues a GFillCurveTopologyWES
  GCopyList(WES.StartElements(),(n1+1),n2,myEdgeAvoid);
  TopTools_ListOfShape LOF; // LOF : toutes les faces construites sur WES
  GWESMakeFaces(FF,WES,LOF);

  // xpu290498
  //cto 001 F2 : spIN(f18)
  TopTools_ListIteratorOfListOfShape itF(LOF);
  while (itF.More()){
    const TopoDS_Shape& F = itF.Value();
    Standard_Boolean valid = ::FUN_validF1edge(F);
    if (!valid) LOF.Remove(itF);
    else itF.Next();
  }
  // xpu290498

  TopTools_ListOfShape LOFS; // LOFS : LOF faces situees TB1/LSO2
  GKeepShapes(FF,myEmptyShapeList,TB1,LOF,LOFS);

  // les faces construites (LOFS) prennent l'orientation originale de FF  
  TopAbs_Orientation odsFF = myDataStructure->Shape(iFF).Orientation();
  for(TopTools_ListIteratorOfListOfShape itt(LOFS);itt.More();itt.Next()) itt.Value().Orientation(odsFF);

  TopTools_ListIteratorOfListOfShape it1;
  for (it1.Initialize(LF1); it1.More(); it1.Next()) {
    const TopoDS_Shape& S = it1.Value(); 
#ifdef OCCT_DEBUG
    Standard_Integer iS; GtraceSPS(S,iS);
#endif
    MarkSplit(S,TB1);
    TopTools_ListOfShape& LS1 = ChangeSplit(S,TB1);
    GCopyList(LOFS,LS1);
  }

  TopTools_ListIteratorOfListOfShape it2;
  for (it2.Initialize(LF2); it2.More(); it2.Next()) {
    const TopoDS_Shape& S = it2.Value(); 
#ifdef OCCT_DEBUG
    Standard_Integer iS; GtraceSPS(S,iS);
#endif
    MarkSplit(S,TB2);
    TopTools_ListOfShape& LS2 = ChangeSplit(S,TB2);
    GCopyList(LOFS,LS2);
  }
} // GFillFacesWESMakeFaces

//=======================================================================
//function : GFillFaceWES
//purpose  : 
//=======================================================================
  void TopOpeBRepBuild_Builder::GFillFaceWES(const TopoDS_Shape& FOR1,
					     const TopTools_ListOfShape& LFclass,
					     const TopOpeBRepBuild_GTopo& G1,
					     TopOpeBRepBuild_WireEdgeSet& WES)
{
  TopAbs_State TB1,TB2; G1.StatesON(TB1,TB2);
  Standard_Boolean RevOri1 = G1.IsToReverse1();
  
#ifdef OCCT_DEBUG
  Standard_Integer iF; Standard_Boolean tSPS = GtraceSPS(FOR1,iF);
  if(tSPS) GdumpSHASTA(iF,TB1,WES,"--- GFillFaceWES","START");
  if(tSPS) debfillf(iF);
#endif
  
  // xpu200598 bcl1;bcl2; tsp(f9)
  Standard_Boolean opeCut = Opec12() || Opec21(); //xpu200598
  Standard_Boolean ComOfCut = opeCut && (TB1 == TB2) && (TB1 == TopAbs_IN); //xpu200598
  Standard_Boolean hsdm = myDataStructure->HasSameDomain(FOR1);//xpu200598
  if (hsdm && ComOfCut) return; //xpu200598
  
  // work on a FORWARD face FF
  TopoDS_Shape FF = FOR1; FF.Orientation(TopAbs_FORWARD);
  myFaceToFill = TopoDS::Face(FF);
  
  TopOpeBRepTool_ShapeExplorer exWire(FF,TopAbs_WIRE);
  for (; exWire.More(); exWire.Next()) {
    TopoDS_Shape W = exWire.Current();
    Standard_Boolean hasshape = myDataStructure->HasShape(W);
    
    if ( ! hasshape ) {
      // wire W is not in DS : classify it with LFclass faces
      TopAbs_State pos;
      Standard_Boolean keep = GKeepShape1(W,LFclass,TB1,pos);
      if (keep) {
	TopAbs_Orientation oriW = W.Orientation();
	TopAbs_Orientation neworiW = Orient(oriW,RevOri1);
	W.Orientation(neworiW);
	WES.AddShape(W);
      }
      else if (myProcessON && pos == TopAbs_ON)
        myONElemMap.Add(W);
    }
    else { // wire W has edges(s) with geometry : split W edges
      GFillWireWES(W,LFclass,G1,WES);
    }
  }
  
#ifdef OCCT_DEBUG
  if(tSPS) GdumpSHASTA(iF,TB1,WES,"--- GFillFaceWES","END");
#endif

  return;
} // GFillFaceWES

//=======================================================================
//function : GFillWireWES
//purpose  : 
//=======================================================================
  void TopOpeBRepBuild_Builder::GFillWireWES(const TopoDS_Shape& W,
					     const TopTools_ListOfShape& LSclass,
					     const TopOpeBRepBuild_GTopo& G1,
					     TopOpeBRepBuild_WireEdgeSet& WES)
{
  TopAbs_State TB1,TB2; G1.StatesON(TB1,TB2);

#ifdef OCCT_DEBUG
  Standard_Integer iW; Standard_Boolean tSPS = GtraceSPS(W,iW);
  if(tSPS){
    cout<<endl;DEBSHASET(s,"--- GFillWireWES ",WES," ");
    GdumpSHA(W,(Standard_Address)s.ToCString()); cout<<endl;
    Standard_Integer nbe = 0;
    TopOpeBRepTool_ShapeExplorer exE(W,TopAbs_EDGE);for (;exE.More(); exE.Next()) nbe++;
    cout<<"--- GFillWireWES on W "<<iW<<" with "<<nbe<<" edges "<<endl;
    debfillw(iW);
  }
  GLOBAL_iexE = 0;
#endif
  
  TopOpeBRepTool_ShapeExplorer exEdge(W,TopAbs_EDGE);
  for (; exEdge.More(); exEdge.Next()) {
    const TopoDS_Shape& EOR = exEdge.Current();
    
#ifdef OCCT_DEBUG
    GLOBAL_iexE++;
    if (tSPS) {
//      const TopoDS_Edge& ed = TopoDS::Edge(EOR);
//      Standard_Boolean isdegen = BRep_Tool::Degenerated(ed);
//      TopLoc_Location L;
//      Handle(Geom_Surface) S = BRep_Tool::Surface(myFaceToFill,L);
//      Standard_Boolean isclosed = BRep_Tool::IsClosed(ed,S,L);
//      TopAbs_Orientation oried = ed.Orientation();
//      Standard_Boolean trc = Standard_False;
#ifdef DRAW
//      if (trc) {FUN_draw(ed); FUN_draw2de(ed,myFaceReference);}
#endif
    }
#endif
    
    GFillEdgeWES(EOR,LSclass,G1,WES);
  }
} // GFillWireWES


//=======================================================================
//function : GFillEdgeWES
//purpose  : 
//=======================================================================
  void TopOpeBRepBuild_Builder::GFillEdgeWES(const TopoDS_Shape& EOR,
					     const TopTools_ListOfShape& LSclass,
					     const TopOpeBRepBuild_GTopo& G1,
					     TopOpeBRepBuild_WireEdgeSet& WES)
{
  TopAbs_State TB1,TB2; G1.StatesON(TB1,TB2);

#ifdef OCCT_DEBUG
  Standard_Integer iE; Standard_Boolean tSPS = GtraceSPS(EOR,iE);
  if(tSPS)cout<<endl;
#endif

#ifdef OCCT_DEBUG
  Standard_Boolean tosplit =
#endif
                GToSplit(EOR,TB1);
#ifdef OCCT_DEBUG
  Standard_Boolean tomerge =
#endif
                GToMerge(EOR);

#ifdef OCCT_DEBUG
  if(tSPS) GdumpSHASTA(iE,TB1,WES,"--- GFillEdgeWES ");
  if(tSPS) cout<<" tosplit "<<tosplit<<" tomerge "<<tomerge<<endl;
  if(tSPS) debfille(iE);
#endif
  
  TopOpeBRepBuild_GTopo GME = G1;
  GMergeEdgeWES(EOR,GME,WES);
  
  TopOpeBRepBuild_GTopo GSE = G1;
  GSE.ChangeConfig(TopOpeBRepDS_UNSHGEOMETRY,TopOpeBRepDS_UNSHGEOMETRY);
  GSplitEdgeWES(EOR,LSclass,GSE,WES);

} // GFillEdgeWES

static void FUN_samgeomori(const TopOpeBRepDS_DataStructure& BDS, const Standard_Integer iref, const Standard_Integer ifil,
			   Standard_Boolean& samgeomori)
{
  TopOpeBRepDS_Config cfill = BDS.SameDomainOri(ifil);
  TopAbs_Orientation oref=BDS.Shape(iref).Orientation(), ofil=BDS.Shape(ifil).Orientation();
  samgeomori = (cfill == TopOpeBRepDS_SAMEORIENTED);
  if (oref == TopAbs::Complement(ofil)) samgeomori = !samgeomori;
}

#define UNKNOWN   (0)
#define ONSAMESHA (1)
#define CLOSESAME (11)
#define ONOPPOSHA (2)
#define CLOSEOPPO (22)
#define FORREVOPPO (222)

//=======================================================================
//function : GSplitEdgeWES
//purpose  : 
//=======================================================================
  void TopOpeBRepBuild_Builder::GSplitEdgeWES(const TopoDS_Shape& EOR,
					      const TopTools_ListOfShape& LSclass,
					      const TopOpeBRepBuild_GTopo& G1,
					      TopOpeBRepBuild_WireEdgeSet& WES)
{
  TopAbs_State TB1,TB2; G1.StatesON(TB1,TB2);
  Standard_Boolean RevOri1 = G1.IsToReverse1();
  TopAbs_Orientation oriE = EOR.Orientation();
  TopAbs_Orientation neworiE = Orient(oriE,RevOri1);
  const TopOpeBRepDS_DataStructure& BDS = myDataStructure->DS();
  
  TopAbs_Orientation oEinF; 
  Standard_Integer Oinref = 0;
  Standard_Boolean hsdm = myDataStructure->HasSameDomain(myFaceToFill);
  Standard_Boolean hsdmE = myDataStructure->HasSameDomain(EOR);
  Standard_Integer ifil = myDataStructure->Shape(myFaceToFill);
  Standard_Integer iref = myDataStructure->Shape(myFaceReference);
  if (hsdm) {
    Oinref = FUN_ds_oriEinF(BDS,TopoDS::Edge(EOR),myFaceReference,oEinF); //xpu060598

    // xpu150998 : cto900P6 : e35ou added to fref34,f53, oEinF=REVERSED, oEinfill=FORWARD
    TopAbs_Orientation oEinfill; 
    Standard_Integer Oinfill = FUN_ds_oriEinF(BDS,TopoDS::Edge(EOR),myFaceToFill,oEinfill);
    if (Oinref == Oinfill) {

      Standard_Boolean reverse = Standard_False; 
      if (iref != ifil) {
	// xpu230299 : FRA60275 (e6,fref4,ffill7) + PRO16297
	TopAbs_Orientation oref = myFaceReference.Orientation();
	Standard_Boolean samegeomori; FUN_samgeomori(BDS,iref,ifil,samegeomori);
	reverse = (!samegeomori);
	if (oref == TopAbs_REVERSED) reverse = !reverse;
      }
//      TopAbs_Orientation oref=myFaceReference.Orientation(), ofill=myFaceToFill.Orientation();
//      Standard_Boolean reverse = (oref != ofill);

      TopAbs_Orientation oEinfillTOref = reverse ? TopAbs::Complement(oEinfill) : oEinfill;
      Standard_Boolean same = (oEinF == oEinfillTOref);
      if (!same && (oEinF!=TopAbs_INTERNAL) && (oEinF!=TopAbs_EXTERNAL)) oEinF = oEinfillTOref;
    }
  }
  else      Oinref = FUN_ds_oriEinF(BDS,TopoDS::Edge(EOR),myFaceToFill,oEinF); //xpu060598
  Standard_Boolean newO = (Oinref == ONSAMESHA) || (Oinref == ONOPPOSHA); //xpu060598

  Standard_Boolean isfafa = (myIsKPart == 3);
  if (isfafa) newO = Standard_False;// xpu110598

  // if (fus) : faces are SDSO : we keep original edge's orientation
  // if (com) : faces are SDSO : we keep original edge's orientation
  // if (cut && TBToFill==OUT)  : myFaceToFill is the reference face,
  //                              we keep original edge's orientation

#ifdef OCCT_DEBUG
  Standard_Integer iEOR; Standard_Boolean tSPS = GtraceSPS(EOR,iEOR);
  Standard_Integer iWESF; /*Standard_Boolean tSPSW = */GtraceSPS(WES.Face(),iWESF);
  if(tSPS) GdumpSHASTA(iEOR,TB1,WES,"\n--- GSplitEdgeWES","START");
  if(tSPS) cout<<" RevOri1 : "<<RevOri1<<endl;
  if(tSPS) debsplite(iEOR);
#endif  
    
  Standard_Boolean tosplit = GToSplit(EOR,TB1);
  if (tosplit) {
    GSplitEdge(EOR,G1,LSclass);
    //modified by NIZNHY-PKV Mon Mar 19 16:53:40 2001 f
    if (myIsKPart==4) {
      // Only solids are available here
      TopAbs_State aState;
      Standard_Integer aRank1;
      TopTools_ListOfShape anAuxList;

      aRank1=ShapeRank(EOR);
      const TopoDS_Shape& aSolid=(aRank1==1) ? myShape2 : myShape1;
      
      TopTools_ListOfShape& aSplitList = ChangeSplit (EOR, TB1);
      TopTools_ListIteratorOfListOfShape anIt(aSplitList);
      for (; anIt.More(); anIt.Next()) {
	const TopoDS_Edge& aSplitEdge=TopoDS::Edge (anIt.Value());
	aState=ClassifyEdgeToSolidByOnePoint (aSplitEdge, aSolid);
	if (aState==TB1) {
	  anAuxList.Append (aSplitEdge);
	}
      }
      
      aSplitList.Clear();
      
      anIt.Initialize(anAuxList);
      for (; anIt.More(); anIt.Next()) {
	const TopoDS_Shape& aShape=anIt.Value();
	aSplitList.Append(aShape);
      }
    }
    //modified by NIZNHY-PKV Mon Mar 19 16:53:44 2001 t
  }

  //xpu200598 : never add spIN in fusion
  Standard_Boolean opeFus = Opefus(); //xpu200598
  if (opeFus) //xpu200598
    if (TB1 == TopAbs_IN) return;  //xpu200598
     
  Standard_Boolean issplit = IsSplit(EOR,TB1);
  if ( issplit ) {
    const TopTools_ListOfShape& LSE = Splits(EOR,TB1);

#ifdef OCCT_DEBUG
    if(tSPS) {
      GdumpSHASTA(iEOR,TB1,WES,"--- GSplitEdgeWES","WES+ Split");
      cout<<" ";TopAbs::Print(TB1,cout)<<" : "<<LSE.Extent()<<" edge(s) ";
      TopAbs::Print(neworiE,cout); cout<<endl;
    }
#endif

    for (TopTools_ListIteratorOfListOfShape it(LSE); 
	 it.More(); it.Next()) {
      TopoDS_Edge newE = TopoDS::Edge(it.Value()); 
      if (newO) {// xpu060598	
	// PRO13075 tspIN(f18), tspIN(e17)
	// we add sp(EOR) to myFaceToFill with its orientation
	newE.Orientation(oEinF);
	Standard_Boolean dgE = BRep_Tool::Degenerated(TopoDS::Edge(EOR)); 
	if (!dgE && hsdmE) {
          Standard_Real f,l; FUN_tool_bounds(newE,f,l); 
          Standard_Real x = 0.45678; Standard_Real par = (1-x)*f + x*l; 
          Standard_Boolean so = Standard_True;
	  Standard_Boolean ok = FUN_tool_curvesSO(newE,par,TopoDS::Edge(EOR),so);
	  if (!ok) {
#ifdef OCCT_DEBUG
            cout<<"GSplitEdgeWES: cannot orient SDM split of an edge"<<endl;
#endif
	    //return; // nyiFUNRAISE
	  }
	  if (!so) {
            newE.Reverse();
	  }
	} //!dgE && hsdmE
      }// xpu060598	
      else newE.Orientation(neworiE);
      
#ifdef OCCT_DEBUG
      if (tSPS) debaddpwes(iWESF,TB1,iEOR,neworiE,(TopOpeBRepBuild_Builder* const)this,&WES,"GSplitEdgeWES " ,"WES+ Eisspl ");
#endif
      
      WES.AddStartElement(newE);
    }
  } // issplit

  else {
    // EOR sans devenir de Split par TB1 : on la garde si elle est situee TB1 / LSclass
    Standard_Boolean se = BDS.IsSectionEdge(TopoDS::Edge(EOR));
    Standard_Boolean hs = myDataStructure->HasShape(EOR);
    Standard_Boolean hg = myDataStructure->HasGeometry(EOR);
    Standard_Boolean add = Standard_False;
    Standard_Boolean addON = Standard_False;

    Standard_Boolean isstart = Standard_False;
    isstart = hs;

    if (se) {
      Standard_Boolean ftg = !LSclass.IsEmpty();
      TopAbs_ShapeEnum tclass = LSclass.First().ShapeType();
      ftg = ftg && (tclass == TopAbs_FACE);
      if (!ftg) {
        TopAbs_State pos;
        Standard_Boolean keepse = GKeepShape1(EOR,LSclass,TB1,pos);
        if (keepse)
          add = Standard_True;
        else if (myProcessON && pos == TopAbs_ON)
          addON = Standard_True;
      }
      
#ifdef OCCT_DEBUG
      cout<<"o-o GridFF ffil F"<<ifil<<" se E"<<iEOR<<" / "<<iWESF<<" ";
      TopAbs::Print(TB1,cout);cout.flush();
      if (!ftg) {cout<<" : !ftg --> "; GKeepShape(EOR,LSclass,TB1);cout.flush();}
      else      {cout<<" : ftg --> non gardee"<<endl;cout.flush();}
#endif

    }
    else {
      add = Standard_True;
      Standard_Boolean testkeep = Standard_True;
      testkeep = hs && (!hg);
      if (testkeep) {
#ifdef OCCT_DEBUG
	if(tSPS){cout<<"--- GSplitEdgeWES ";}
#endif
        TopAbs_State pos;
	Standard_Boolean keep = GKeepShape1(EOR,LSclass,TB1,pos);
	if ( !keep ) {
	  Standard_Boolean testON = (!LSclass.IsEmpty());
	  if (testON) testON = (LSclass.First().ShapeType() == TopAbs_SOLID);
	  if (testON) keep = (pos == TopAbs_ON);
          addON = myProcessON && keep;
	}
	add = keep;
      }
    } // !se

    if (add) {
      TopoDS_Shape newE = EOR;
       
      if      (newO)                 newE.Orientation(oEinF);// xpu060598  
      else if (Oinref == FORREVOPPO) newE.Orientation(TopAbs_INTERNAL);// xpu120898 (PRO14785 : e36 shared by f34 & f39,
                                                                       // faces sdm with f16)
      else                           newE.Orientation(neworiE); 
#ifdef OCCT_DEBUG
      if(tSPS){
	DEBSHASET(ss,"--- GSplitEdgeWES ",WES," WES+ edge ");  
	GdumpSHA(newE,(Standard_Address)ss.ToCString());
	cout<<" ";TopAbs::Print(TB1,cout)<<" : 1 edge ";
	TopAbs::Print(neworiE,cout); cout<<endl;
      }
#endif

      if (isstart) {
#ifdef OCCT_DEBUG
	if (tSPS) debaddpwes(iWESF,TB1,iEOR,neworiE,(TopOpeBRepBuild_Builder* const)this,&WES,"GSplitEdgeWES " ,"WES+ Enospl ");
#endif
	WES.AddStartElement(newE);
      }
      else {
	WES.AddElement(newE);
      }
    } // add

    if (addON) {
      TopoDS_Shape newE = EOR;
      newE.Orientation(neworiE);
      myONElemMap.Add(newE);
    }
  } // !issplit

  if (myProcessON && IsSplit(EOR,TopAbs_ON)) {
    const TopTools_ListOfShape& LSE = Splits(EOR,TopAbs_ON);
    TopTools_ListIteratorOfListOfShape it(LSE);
    for (; it.More(); it.Next()) {
      TopoDS_Edge newE = TopoDS::Edge(it.Value());
      if (newO) {
	newE.Orientation(oEinF);
	Standard_Boolean dgE = BRep_Tool::Degenerated(TopoDS::Edge(EOR)); 
	if (!dgE && hsdmE) {
          Standard_Real f,l; FUN_tool_bounds(newE,f,l); 
          Standard_Real x = 0.45678; Standard_Real par = (1-x)*f + x*l; 
          Standard_Boolean so = Standard_True;
	  Standard_Boolean ok = FUN_tool_curvesSO(newE,par,TopoDS::Edge(EOR),so);
	  if (!ok) {
#ifdef OCCT_DEBUG
            cout<<"GSplitEdgeWES: cannot orient SDM split of an edge"<<endl;
#endif
          }
	  if (!so) newE.Reverse();
	}
      }
      else newE.Orientation(neworiE);
      myONElemMap.Add(newE);
    }
  }

#ifdef OCCT_DEBUG
  if(tSPS) GdumpSHASTA(iEOR,TB1,WES,"--- GSplitEdgeWES","END");
#endif  
  
  return;
} // GSplitEdgeWES

Standard_IMPORT Standard_Boolean FUN_ismotheropedef();
Standard_IMPORT const TopOpeBRepBuild_GTopo& FUN_motherope();
Standard_EXPORT Standard_Boolean GLOBAL_IEtoMERGE = 0; // xpu240498

#ifdef OCCT_DEBUG
void debmergee(const Standard_Integer /*i*/) {}
#endif

//=======================================================================
//function : GMergeEdgeWES
//purpose  : 
//=======================================================================
  void TopOpeBRepBuild_Builder::GMergeEdgeWES(const TopoDS_Shape& EOR,
					      const TopOpeBRepBuild_GTopo& G1,
					      TopOpeBRepBuild_WireEdgeSet& WES)
{
#ifdef OCCT_DEBUG
  Standard_Integer iWESF; /*Standard_Boolean tSPSW = */GtraceSPS(WES.Face(),iWESF);
  Standard_Integer iEOR; Standard_Boolean tSPS = GtraceSPS(EOR,iEOR);
  if(tSPS){ debmergee(iEOR);
    DEBSHASET(s,"\n--- GMergeEdgeWES ",WES," START ");  
    GdumpSHAORIGEO(EOR,(Standard_Address)s.ToCString()); cout<<endl;
  }
#endif

  Standard_Boolean closing = BRep_Tool::IsClosed(TopoDS::Edge(EOR),myFaceToFill); // xpu050598
  if (closing) return; // xpu050598

  if (Opefus()) return;
  
//  const TopOpeBRepDS_DataStructure& BDS = myDataStructure->DS();
  TopAbs_State TB1,TB2; G1.StatesON(TB1,TB2);
  Standard_Boolean RevOri1 = G1.IsToReverse1();
  TopAbs_Orientation oriE = EOR.Orientation();
  TopAbs_Orientation neworiE = Orient(oriE,RevOri1);
  
  Standard_Boolean hassame = myDataStructure->HasSameDomain(EOR);
  if (!hassame) return;

  Standard_Boolean Eisref = Standard_False;
  if (hassame) {
    Standard_Integer iEref = myDataStructure->SameDomainReference(EOR);
    const TopoDS_Shape& Eref = myDataStructure->Shape(iEref);
    Eisref = EOR.IsSame(Eref);
  }

  TopAbs_State TBEOR = (Eisref) ? TB1 : TB2; 
  if (TBEOR == TopAbs_OUT) return; //xpu040598

  Standard_Boolean ismerged = IsMerged(EOR,TBEOR);
  if (ismerged) {
    if (!Eisref) return;

    const TopTools_ListOfShape& ME = Merged(EOR,TBEOR);
    TopTools_ListIteratorOfListOfShape it(ME);
    for(; it.More(); it.Next()) {
      TopoDS_Shape newE = it.Value();
      newE.Orientation(neworiE);

#ifdef OCCT_DEBUG
      if (tSPS) debaddpwes(iWESF,TB1,iEOR,neworiE,(TopOpeBRepBuild_Builder* const)this,&WES,"GMergeEdgeWES " ,"WES+ Emerge ");
#endif

      WES.AddStartElement(newE);
    }
    return;
  } 

  ChangeMerged(EOR,TBEOR) = myEmptyShapeList; 
  TopAbs_State stspEOR;
//  if (isfafa) stspEOR = TBEOR; // xpu110598
//  else stspEOR = (TBEOR == TopAbs_IN) ? TopAbs_ON : TopAbs_OUT;
  stspEOR = TBEOR; // xpu120598

  Standard_Boolean issplit = IsSplit(EOR,stspEOR);
  if (!issplit) return;

  ChangeMerged(EOR,TBEOR) = Splits(EOR,stspEOR);

  const TopTools_ListOfShape& ME = Merged(EOR,TBEOR);
#ifdef OCCT_DEBUG
  if(tSPS){
    DEBSHASET(s,"GMergeEdgeWES(1) ",WES," WES+ Merged ");  
    GdumpSHA(EOR,(Standard_Address)s.ToCString());
    cout<<" ";TopAbs::Print(TBEOR,cout);
    cout<<" : "<<ME.Extent()<<" edge"<<endl;
  }
#endif
  for(TopTools_ListIteratorOfListOfShape it(ME);it.More();it.Next()) {
    TopoDS_Shape newE = it.Value();
    newE.Orientation(neworiE);
    WES.AddStartElement(newE);
  }

#ifdef OCCT_DEBUG
  if(tSPS){
    DEBSHASET(sss,"GMergeEdgeWES ",WES," END ");
    GdumpSHA(EOR,(Standard_Address)sss.ToCString());cout<<endl;
  }
#endif
  
} // GMergeEdgeWES

//=======================================================================
//function : GSplitEdge
//purpose  : 
//=======================================================================
  void TopOpeBRepBuild_Builder::GSplitEdge(const TopoDS_Shape& EOR,
					   const TopOpeBRepBuild_GTopo& G1,
					   const TopTools_ListOfShape& LSclass)
{
  TopAbs_ShapeEnum t1,t2;
  G1.Type(t1,t2);
  TopAbs_State TB1,TB2; G1.StatesON(TB1,TB2);
  // work on a FORWARD edge <EF>
  TopoDS_Shape EF = EOR; EF.Orientation(TopAbs_FORWARD);
  
#ifdef OCCT_DEBUG
  Standard_Integer iE; Standard_Boolean tSPS = GtraceSPS(EOR,iE);
  if (tSPS) GdumpSHASTA(EOR,TB1,"--- GSplitEdge ","\n");
  if (tSPS) GdumpEDG(EF);
  if (tSPS) debsplite(iE);
#endif

  const TopoDS_Edge& EEF = TopoDS::Edge(EF);
  Standard_Boolean isse = myDataStructure->DS().IsSectionEdge(EEF);
  Standard_Boolean issplitON = IsSplit(EEF,TopAbs_ON);
  Standard_Boolean takeON = (TB1 == TopAbs_IN) && (isse) && (issplitON);
  takeON = Standard_False;
#ifdef OCCT_DEBUG
  if (tSPS) cout<<"---- takeON mis a 0"<<endl;
#endif

  if ( takeON ) {

#ifdef OCCT_DEBUG
    if (tSPS) GdumpSHASTA(EOR,TB1,"--- GSplitEdge takeON ","\n");
#endif

    MarkSplit(EF,TB1);
    TopTools_ListOfShape& SSEL = ChangeSplit(EF,TB1);
    SSEL.Clear();
    SSEL = Splits(EEF,TopAbs_ON);
    return;
  }

  TopTools_ListOfShape LOE;
  
  // Make a PaveSet PVS on edge EF
  TopOpeBRepBuild_PaveSet PVS(EF);
  
  // Add the point topology found on edge EF in PVS
  myEdgeReference = TopoDS::Edge(EF);
  GFillPointTopologyPVS(EF,G1,PVS);
  
  // mark EF as split TB1
  MarkSplit(EF,TB1);
  
  // build the new edges LOE on EF from the Parametrized Vertex set PVS
  GPVSMakeEdges(EF,PVS,LOE);
  
  Standard_Boolean novertex = LOE.IsEmpty();
  if (novertex) return;

  TopTools_ListOfShape& SEL = ChangeSplit(EF,TB1);
  SEL.Clear();
  // NYI ne pas faire de classification des aretes reconstruites / liste de solides
  // NYI dans le cas ou l'appel a SplitEdge est utilise pour construire les parties
  // NYI (TopAbs_ON,SOLID) (i.e par la construction des parties (TopAbs_IN,FACE)).
  TopOpeBRepDS_Config c1 = G1.Config1(),c2 = G1.Config2();
  Standard_Boolean UUFACE = (c1==TopOpeBRepDS_UNSHGEOMETRY && c2==TopOpeBRepDS_UNSHGEOMETRY);

  Standard_Boolean ONSOLID = Standard_False;
  if ( ! LSclass.IsEmpty() ) {
    TopAbs_ShapeEnum t = LSclass.First().ShapeType();
    ONSOLID = (t == TopAbs_SOLID);
  }

  Standard_Boolean toclass = UUFACE;
  toclass = ! ONSOLID; 

  TopTools_ListOfShape loos;
  const TopTools_ListOfShape* pls;
  if (GLOBAL_classifysplitedge) {
    Standard_Integer r=GShapeRank(EOR);
    TopoDS_Shape oos=myShape1;
    if (r==1) oos = myShape2;
    if (!oos.IsNull()) loos.Append(oos); // PMN 5/03/99 Nothing to append
    pls = &loos;
  }
  else if (toclass) {
    pls = &LSclass;
  }
  else {
    pls = &myEmptyShapeList;
  }

  TopTools_ListOfShape aLON;
  TopTools_ListIteratorOfListOfShape it(LOE);
  for(;it.More();it.Next()) {
    const TopoDS_Shape& aE = it.Value();
    TopAbs_State pos;
    if (GKeepShape1(aE,*pls,TB1,pos))
      SEL.Append(aE);
    else if (myProcessON && pos == TopAbs_ON)
      aLON.Append(aE);
  }

  if (!aLON.IsEmpty()) {
    MarkSplit(EF,TopAbs_ON);
    TopTools_ListOfShape& aSLON = ChangeSplit(EF,TopAbs_ON);
    aSLON.Clear();
    aSLON.Append(aLON);
  }

} // GSplitEdge

//modified by NIZNHY-PKV Mon Mar 19 16:50:33 2001 f
#include <BRepClass3d_SolidClassifier.hxx>
//=======================================================================
//function : ClassifyEdgeToSolidByOnePoint
//purpose  : 
//=======================================================================
TopAbs_State ClassifyEdgeToSolidByOnePoint(const TopoDS_Edge& E,
						  const TopoDS_Shape& Ref)
{
  const Standard_Real PAR_T = 0.43213918;//10.*e^-PI
  Standard_Real f2 = 0., l2 = 0., par = 0.;

  Handle(Geom_Curve) C3D = BRep_Tool::Curve(E, f2, l2);
  gp_Pnt aP3d;

  if(C3D.IsNull()) {
    //it means that we are in degenerated edge
    const TopoDS_Vertex& fv = TopExp::FirstVertex(E);
    if(fv.IsNull())
      return TopAbs_UNKNOWN;
    aP3d = BRep_Tool::Pnt(fv);
  }
  else {//usual case
    par = f2*PAR_T + (1 - PAR_T)*l2;
    C3D -> D0(par, aP3d);
  }
    
  BRepClass3d_SolidClassifier SC(Ref);
  SC.Perform(aP3d, 1e-7);

  return SC.State();
}
//modified by NIZNHY-PKV Mon Mar 19 16:50:36 2001 t
