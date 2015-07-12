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
#include <BRepClass3d_SolidExplorer.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <gp_Pnt.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_Builder.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_EdgeBuilder.hxx>
#include <TopOpeBRepBuild_FaceBuilder.hxx>
#include <TopOpeBRepBuild_FuseFace.hxx>
#include <TopOpeBRepBuild_GTopo.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepBuild_PaveSet.hxx>
#include <TopOpeBRepBuild_ShapeSet.hxx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>
#include <TopOpeBRepBuild_SolidBuilder.hxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopOpeBRepDS_samdom.hxx>
#include <TopOpeBRepDS_SurfaceIterator.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_SC.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>

#ifdef OCCT_DEBUG
#define DEBSHASET(sarg,meth,shaset,str) TCollection_AsciiString sarg((meth));(sarg)=(sarg)+(shaset).DEBNumber()+(str);
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GetcontextNOFUFA();
Standard_EXPORT void debffsfs(const Standard_Integer i){cout<<"+++ debffsfs "<<i<<endl;}
Standard_EXPORT void debffflo(const Standard_Integer i){cout<<"+++ debffflo "<<i<<endl;}
#endif

static Standard_Boolean STATIC_motheropedef = Standard_False; static TopOpeBRepBuild_GTopo STATIC_Gmotherope;
Standard_EXPORT void FUN_setmotherope(const TopOpeBRepBuild_GTopo& G) {STATIC_Gmotherope = G;STATIC_motheropedef = Standard_True;}
Standard_EXPORT void FUN_unsetmotherope() { STATIC_motheropedef = Standard_False; }
Standard_EXPORT Standard_Boolean FUN_ismotheropedef() { return STATIC_motheropedef; }
Standard_EXPORT const TopOpeBRepBuild_GTopo& FUN_motherope() { return STATIC_Gmotherope; } 

//Standard_IMPORT extern Standard_Boolean GLOBAL_classifysplitedge;
Standard_EXPORTEXTERN Standard_Boolean GLOBAL_classifysplitedge;
//Standard_IMPORT extern Standard_Boolean GLOBAL_revownsplfacori;
Standard_EXPORTEXTERN Standard_Boolean GLOBAL_revownsplfacori;
Standard_EXPORT void FUNBUILD_ANCESTORRANKPREPARE
(TopOpeBRepBuild_Builder& B,const TopTools_ListOfShape& LF1,const TopTools_ListOfShape& LF2,const TopOpeBRepDS_Config c1,const TopOpeBRepDS_Config c2);
Standard_EXPORT void FUNBUILD_ANCESTORRANKGET(TopOpeBRepBuild_Builder& B,const TopoDS_Shape& f,Standard_Boolean& of1,Standard_Boolean& of2);

static Standard_Integer FUN_getAncestorFsp(TopOpeBRepBuild_Builder& B,TopOpeBRepTool_ShapeClassifier& SC,const TopTools_ListOfShape& LF,const TopoDS_Shape& fsp,Standard_Boolean& p3ddef,gp_Pnt& p3d);
static Standard_Integer FUN_getAncestorFsp(TopOpeBRepBuild_Builder& B,TopOpeBRepTool_ShapeClassifier& SC,const TopTools_ListOfShape& LF1,const TopTools_ListOfShape& LF2,const TopoDS_Shape& fsp);
#ifdef OCCT_DEBUG
//static void FUN_getAncestorFsp(const Handle(TopOpeBRepDS_HDataStructure)& HDS,TopOpeBRepTool_ShapeClassifier& SC,const TopTools_ListOfShape& LF1,const TopTools_ListOfShape& LF2,const TopTools_ListOfShape& spFOR,
//			       TopTools_DataMapOfShapeInteger* SplitAnc);
#endif

static Standard_Integer FUN_getAncestorFsp(TopOpeBRepBuild_Builder& B,TopOpeBRepTool_ShapeClassifier& SC,const TopTools_ListOfShape& LF,const TopoDS_Shape& fsp,Standard_Boolean& p3ddef,gp_Pnt& p3d)
{
  const TopOpeBRepDS_DataStructure& BDS = B.DataStructure()->DS(); // How to do static <--> const

  TopTools_ListIteratorOfListOfShape itf(LF);
  for (; itf.More(); itf.Next()){
    const TopoDS_Face& f = TopoDS::Face(itf.Value());
    TopAbs_State st = SC.StateShapeShape(fsp,f,1);
    if ((st == TopAbs_UNKNOWN) || (st == TopAbs_OUT)) continue;
    if (st == TopAbs_ON) {
      if (!p3ddef) {
	Standard_Boolean ok = BRepClass3d_SolidExplorer::FindAPointInTheFace(TopoDS::Face(fsp),p3d);
	if (!ok) return 0;
	p3ddef = Standard_True;
      }
      gp_Pnt2d p2d;Standard_Real dd = 0.; Standard_Boolean ok = FUN_tool_projPonF(p3d,f,p2d,dd);
      if (!ok) return 0;
      Standard_Real tolf = BRep_Tool::Tolerance(f)*1.e1;
      if (dd > tolf) return 0;
      Standard_Real  TolClass  = 1e-8; BRepTopAdaptor_FClass2d FClass2d(f,TolClass);
      st = FClass2d.Perform(p2d);
    }
    if (st == TopAbs_IN) {
      Standard_Integer ianc = BDS.Shape(f);
      return ianc;
    }
  } // itf(LF)
  return 0;
}

static Standard_Integer FUN_getAncestorFsp(TopOpeBRepBuild_Builder& B,TopOpeBRepTool_ShapeClassifier& SC,
			      const TopTools_ListOfShape& LF1,const TopTools_ListOfShape& LF2,const TopoDS_Shape& fsp)
{
  // IMPORTANT : fsp is split IN/OUT of F1,so it has only one ancestor face
  // LF1 faces sdm with LF2
  const TopOpeBRepDS_DataStructure& BDS = B.DataStructure()->DS();
  Standard_Boolean of1,of2; FUNBUILD_ANCESTORRANKGET(B,fsp,of1,of2);	
  Standard_Integer rkfsp = 0;
  if      (of1 && !of2) rkfsp = 1;
  else if (of2 && !of1) rkfsp = 2;
  Standard_Boolean unk = (rkfsp == 0);

  Standard_Integer rkf1 = BDS.AncestorRank(LF1.First()); 
  Standard_Integer rkf2 = BDS.AncestorRank(LF2.First());  
  Standard_Boolean p3ddef = Standard_False; gp_Pnt p3d;
  
  Standard_Boolean ison1 = (rkf1 == rkfsp); 
  Standard_Boolean ison2 = (rkf2 == rkfsp); 

  Standard_Integer ianc1=0,ianc2=0;
  if (ison1 || unk) 
    ianc1 = FUN_getAncestorFsp(B,SC,LF1,fsp,p3ddef,p3d);
  if (ison1) return ianc1;

  if (ison2 || unk) 
    ianc2 = FUN_getAncestorFsp(B,SC,LF2,fsp,p3ddef,p3d);
  if (ison2) return ianc2;

  if (ianc1 + ianc2 >0) {
    if      (ianc1 == 0) return ianc2; 
    else if (ianc2 == 0) return ianc1;
    else                 return 0; // fsp has 2 ancestor faces
  } 
  return 0;
}

Standard_EXPORT TopTools_DataMapOfShapeInteger* GLOBAL_SplitAnc = NULL; //xpu260598

static void FUN_getAncestorFsp(TopOpeBRepBuild_Builder& B,TopOpeBRepTool_ShapeClassifier& SC,
			       const TopTools_ListOfShape& LF1,const TopTools_ListOfShape& LF2,
			       const TopoDS_Shape& FOR,TopTools_DataMapOfShapeInteger* SplitAnc)
{	
  if (SplitAnc == NULL) return;

  Standard_Boolean issplitIN = B.IsSplit(FOR,TopAbs_IN); 
  Standard_Boolean issplitOU = B.IsSplit(FOR,TopAbs_OUT); 
  if (!issplitIN && !issplitOU) return;

  TopTools_ListOfShape spFOR; 
  if (issplitIN) FDS_copy(B.Splits(FOR,TopAbs_IN),spFOR);
  if (issplitOU) FDS_copy(B.Splits(FOR,TopAbs_OUT),spFOR);

  for (TopTools_ListIteratorOfListOfShape itsp(spFOR); itsp.More(); itsp.Next()){
    const TopoDS_Shape& fsp = itsp.Value();
    
    Standard_Boolean isbound = SplitAnc->IsBound(fsp);
    if (isbound) continue;
    
    Standard_Integer ianc = FUN_getAncestorFsp(B,SC,LF1,LF2,fsp);
    if (ianc != 0) {
      SplitAnc->Bind(fsp,ianc);
    }
  } // itsp
}

Standard_EXPORT TopTools_ListOfShape* GLOBAL_lfr1 = NULL;
Standard_EXPORT Standard_Boolean GLOBAL_lfrtoprocess = Standard_False;
//Standard_IMPORT extern TopTools_ListOfShape* GLOBAL_lfr1;
//Standard_IMPORT TopTools_ListOfShape* GLOBAL_lfr1;
//Standard_IMPORT extern Standard_Boolean GLOBAL_lfrtoprocess;
//Standard_IMPORT Standard_Boolean GLOBAL_lfrtoprocess;

//=======================================================================
//function : GFillFaceSFS
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::GFillFaceSFS(const TopoDS_Shape& FOR,const TopTools_ListOfShape& LSO2,const TopOpeBRepBuild_GTopo& Gin,TopOpeBRepBuild_ShellFaceSet& SFS)
{
  TopAbs_State TB1,TB2; Gin.StatesON(TB1,TB2);
  
#ifdef OCCT_DEBUG
  Standard_Integer iF; Standard_Boolean tSPS = GtraceSPS(FOR,iF);
  if(tSPS)cout<<endl;
#endif
  const TopOpeBRepDS_DataStructure& BDS = myDataStructure->DS();
  Standard_Boolean tosplit = GToSplit(FOR,TB1);
  Standard_Boolean tomerge = GToMerge(FOR);
#ifdef OCCT_DEBUG
//  Standard_Integer iFOR = BDS.Shape(FOR);
#endif
  Standard_Integer rkFOR = BDS.AncestorRank(FOR);
  
#ifdef OCCT_DEBUG
  if(tSPS){
    GdumpSHASTA(FOR,TB1,"--- GFillFaceSFS START ");
    cout<<" tosplit "<<tosplit<<" tomerge "<<tomerge<<endl;
    debffsfs(iF);
  }
#endif
  
  TopoDS_Shape FF = FOR; FF.Orientation(TopAbs_FORWARD);  
  Standard_Boolean hsd = myDataStructure->HasSameDomain(FOR);	//xpu280598
  GLOBAL_lfrtoprocess = Standard_False;

  if ( tosplit && tomerge ) {
    
    // merge des faces SameDomain
    
    // on effectue le merge ssi FOR est la reference de ses faces SameDomain
    Standard_Integer iref = myDataStructure->SameDomainReference(FOR);
    const TopoDS_Shape& fref = myDataStructure->Shape(iref);
    Standard_Boolean isref = FOR.IsSame(fref);

    Standard_Boolean makemerge = isref;
    // xpu280199 PRO16958 : f10=ref(f6), GToMerge(f10)=false 
    Standard_Boolean makemergeref = GToMerge(fref);
    makemerge = makemerge || (!makemergeref && (rkFOR == 1));
    
    if (makemerge) {
      
#ifdef OCCT_DEBUG
      if(tSPS) {
	GdumpSHASTA(FOR,TB1,"[[[[[[[[[[[[[[[[[[[[[[[[[[ GFillFaceSFS makemerge START ");
	cout<<endl;
      }
#endif

      Standard_Boolean performfufa = Standard_True;
#ifdef OCCT_DEBUG
      performfufa = !TopOpeBRepBuild_GetcontextNOFUFA();
#endif	  
      if (performfufa) {	
	GLOBAL_lfrtoprocess = Standard_True;
	if (GLOBAL_lfrtoprocess) {
	  if (GLOBAL_lfr1==NULL) GLOBAL_lfr1=(TopTools_ListOfShape*)new TopTools_ListOfShape();
	  GLOBAL_lfr1->Clear();
	}
      }

      // xpu280598 : Filling up GLOBAL_SplitAnc = {(fsp,ifanc)}
      //              . fsp = spIN/OU(fanc),
      //              . fanc hsdm is the unique ancestor face     
      if ( GLOBAL_SplitAnc == NULL ) GLOBAL_SplitAnc = (TopTools_DataMapOfShapeInteger*) new TopTools_DataMapOfShapeInteger(); 
      GLOBAL_SplitAnc->Clear();

      TopTools_ListOfShape LFSO,LFDO,LFSO1,LFDO1,LFSO2,LFDO2;
      GFindSamDomSODO(FF,LFSO,LFDO); // -980617
//      FDSSDM_sordor(FF,LFSO,LFDO);
      Standard_Integer rankF=GShapeRank(FF),rankX=(rankF)?((rankF==1)?2:1):0;
      GFindSameRank(LFSO,rankF,LFSO1);
      GFindSameRank(LFDO,rankF,LFDO1);
      GFindSameRank(LFSO,rankX,LFSO2);
      GFindSameRank(LFDO,rankX,LFDO2);
      TopTools_ListOfShape LF1,LF2;
      
      TopOpeBRepBuild_GTopo GM;
      TopAbs_State TB,NTB;
      Standard_Boolean dodo;
      Standard_Integer l1,l2;
      
      // WES : toutes les faces de meme orientation topologique
      LF1 = LFSO1; // NYI pointeurs
      LF2 = LFSO2; // NYI pointeurs
      l1 = LF1.Extent(); l2 = LF2.Extent();
      dodo = (l1!=0) && (l2!=0);      
      
      FUN_unsetmotherope(); // +12/07
      
      GM = Gin;
      GM.ChangeConfig(TopOpeBRepDS_SAMEORIENTED,TopOpeBRepDS_SAMEORIENTED);
      if (dodo) {
#ifdef OCCT_DEBUG
	if(tSPS){
	  GdumpSAMDOM(LF1, (char *) "LF1 (LFSO1) : ");
	  GdumpSAMDOM(LF2, (char *) "LF2 (LFSO2) : ");
	}
#endif
	GLOBAL_classifysplitedge = Standard_True;
	GFillFacesWESMakeFaces(LF1,LF2,LSO2,GM);
	GLOBAL_classifysplitedge = Standard_False;
	
	GLOBAL_revownsplfacori = Standard_True;
	FUNBUILD_ANCESTORRANKPREPARE
	  (*this,LF1,LF2,TopOpeBRepDS_SAMEORIENTED,TopOpeBRepDS_SAMEORIENTED);
	if (hsd) FUN_getAncestorFsp((*this),myShapeClassifier,
				    LF1,LF2,FOR,GLOBAL_SplitAnc); //xpu280598

	// GLOBAL_lfrtoprocess = t 
	// ==> on ne stocke PAS les faces 'startelement' dans le SFS
	//     mais dans GLOBAL_lfr1,
	// GLOBAL_lfrtoprocess = f
	// ==> on stocke normalement dans le SFS et pas dans GLOBAL_lfr1
	// NYI : + argument a la methode GSplitFaceSFS ?? a voir

	// ici : GLOBAL_lfrtoprocess = t
	if (GLOBAL_lfr1==NULL) GLOBAL_lfr1=(TopTools_ListOfShape*)new TopTools_ListOfShape(); //flo150998
	GLOBAL_lfr1->Clear();
	GSplitFaceSFS(FOR,LSO2,GM,SFS);
	GLOBAL_lfrtoprocess = Standard_False;
	GLOBAL_revownsplfacori = Standard_False;
      }
      
      // WES : FOR + faces d'orientation topologique opposee
      LF1 = LFSO1; // NYI pointeurs
      LF2 = LFDO2; // NYI pointeurs
      l1 = LF1.Extent(); l2 = LF2.Extent();
      dodo = (l1!=0) && (l2!=0);
      
      
      GM = Gin; // OUT,OUT-> OUT,IN + IN OUT
      TB = TB2; //           ------
      NTB = TopAbs_IN; // NTB = (TB == TopAbs_OUT) ? TopAbs_IN : TopAbs_OUT;
      GM.ChangeValue(TB,TopAbs_ON,Standard_False);
      GM.ChangeValue(NTB,TopAbs_ON,Standard_True);
      GM.ChangeConfig(TopOpeBRepDS_SAMEORIENTED,TopOpeBRepDS_DIFFORIENTED);
      FUN_setmotherope(GM); // +12/07
      if (dodo) {
#ifdef OCCT_DEBUG
	if(tSPS){
	  TopAbs_State TB11,TB21; GM.StatesON(TB11,TB21);
	  cout<<endl;
	  cout <<"@@@@"<<endl;
	  cout <<"@@@@@@@@ partie 1 : ";TopAbs::Print(TB11,cout);cout<<" ";TopAbs::Print(TB21,cout);cout<<endl;
	  cout <<"@@@@"<<endl;
	  GdumpSAMDOM(LF1, (char *) "LF1 (LFSO1) : ");
	  GdumpSAMDOM(LF2, (char *) "LF2 (LFDO2) : ");
	  cout<<endl;
	}
#endif
	GLOBAL_classifysplitedge = Standard_True;
	GFillFacesWESMakeFaces(LF1,LF2,LSO2,GM);
	GLOBAL_classifysplitedge = Standard_False;
	
	GLOBAL_revownsplfacori = Standard_True;
	FUNBUILD_ANCESTORRANKPREPARE
	  (*this,LF1,LF2,TopOpeBRepDS_SAMEORIENTED,TopOpeBRepDS_DIFFORIENTED);
	if (hsd) FUN_getAncestorFsp((*this),myShapeClassifier,
				    LF1,LF2,FOR,GLOBAL_SplitAnc); //xpu280598
	
	if ( Opecom() ) {
	  Standard_Boolean issplitIN = IsSplit(FOR,TopAbs_IN); 
	  if (issplitIN) {
	    const TopTools_ListOfShape& spFOR = Splits(FOR,TopAbs_IN);
	    TopTools_ListOfShape spFORcopy; FDS_copy(spFOR,spFORcopy);

	    TopTools_ListIteratorOfListOfShape it(LF1);
	    for (; it.More(); it.Next()) {
	      const TopoDS_Shape& f = it.Value();
	      Standard_Boolean issplit = IsSplit(f,TopAbs_IN);
	      if ( issplit ) ChangeSplit(f,TopAbs_IN).Clear();
	    }
	    it.Initialize(LF2);
	    for (; it.More(); it.Next()) {
	      const TopoDS_Shape& f = it.Value();
	      Standard_Boolean issplit = IsSplit(f,TopAbs_IN);
	      if ( issplit ) ChangeSplit(f,TopAbs_IN).Clear();
	    }
	    ChangeSplit(FOR,TopAbs_IN).Append(spFORcopy); // keep split for reference
	  } // issplitIN
	} // OpeCom 

	GSplitFaceSFS(FOR,LSO2,GM,SFS);
	GLOBAL_revownsplfacori = Standard_False;

      } // dodo
      
      if (!Opecom()) {
	GM = Gin; // OUT,OUT-> OUT,IN + IN OUT
	TB = TB1; //                    ------ 
	//      NTB = (TB == TopAbs_OUT) ? TopAbs_IN : TopAbs_OUT;
	NTB = TopAbs_IN;
	GM.ChangeValue(TopAbs_ON,TB,Standard_False);
	GM.ChangeValue(TopAbs_ON,NTB,Standard_True);
	GM.ChangeConfig(TopOpeBRepDS_SAMEORIENTED,TopOpeBRepDS_DIFFORIENTED);
	FUN_setmotherope(GM); // +12/07
	if (dodo) {
#ifdef OCCT_DEBUG
	  if(tSPS){
	    TopAbs_State TB12,TB22; GM.StatesON(TB12,TB22);
	    cout<<endl;
	    cout <<"@@@@"<<endl;
	    cout <<"@@@@@@@@ partie 2 : ";TopAbs::Print(TB12,cout);cout<<" ";TopAbs::Print(TB22,cout);cout<<endl;
	    cout <<"@@@@"<<endl;
	    GdumpSAMDOM(LF1, (char *) "LF1 (LFSO1) : ");
	    GdumpSAMDOM(LF2, (char *) "LF2 (LFDO2) : ");
	    cout<<endl;
	  }
#endif
	  GLOBAL_classifysplitedge = Standard_True;
	  GFillFacesWESMakeFaces(LF1,LF2,LSO2,GM);
	  GLOBAL_classifysplitedge = Standard_False;
	  
	  GLOBAL_revownsplfacori = Standard_True;
	  FUNBUILD_ANCESTORRANKPREPARE
	    (*this,LF1,LF2,TopOpeBRepDS_SAMEORIENTED,TopOpeBRepDS_DIFFORIENTED);
	  if (hsd) FUN_getAncestorFsp((*this),myShapeClassifier,
				      LF1,LF2,FOR,GLOBAL_SplitAnc); //xpu280598
	  GSplitFaceSFS(FOR,LSO2,GM,SFS);
	  GLOBAL_revownsplfacori = Standard_False;
	}
      } // !Opecom
      
      FUN_unsetmotherope(); // +12/07
      
#ifdef OCCT_DEBUG
      if(tSPS) { 
	GdumpSHASTA(FOR,TB1,"]]]]]]]]]]]]]]]]]]]]]]]]]] GFillFaceSFS makemerge END ");
	cout<<endl;
      }
#endif

      GLOBAL_SplitAnc->Clear(); // xpu280598

      // FuseFace
      SFS.ChangeStartShapes().Extent();
      if (performfufa) {	
#ifdef OCCT_DEBUG
	if(tSPS) debffflo(iF);
#endif
//	const TopTools_ListOfShape& lou = Splits(FF,TopAbs_OUT); Standard_Integer nou = lou.Extent();
//	const TopTools_ListOfShape& lin = Splits(FF,TopAbs_IN);  Standard_Integer nin = lin.Extent();
//	GCopyList(lou,*GLOBAL_lfr1);
//	GCopyList(lin,*GLOBAL_lfr1);	
#ifdef OCCT_DEBUG
//	Standard_Integer nlfr1 = GLOBAL_lfr1->Extent();
#endif

	// NYI : Builder += methode pour le process fufa
	TopOpeBRepBuild_FuseFace fufa; TopTools_ListOfShape ldum; Standard_Integer addinternal = 1; // disparition
	fufa.Init(ldum,*GLOBAL_lfr1,addinternal);
	fufa.PerformFace();
	Standard_Boolean isdone = fufa.IsDone();
	if (!isdone) return;
#ifdef OCCT_DEBUG
//	Standard_Boolean ismodified = fufa.IsModified();
#endif
	const TopTools_ListOfShape& lfr2 = fufa.LFuseFace();
//
//	const TopTools_ListOfShape& lfr2 = *GLOBAL_lfr1
	// les faces remplacantes
	for (TopTools_ListIteratorOfListOfShape itlfr2(lfr2);itlfr2.More();itlfr2.Next()) {
	  const TopoDS_Shape& flfr2 = itlfr2.Value();
	  
#ifdef OCCT_DEBUG
	  if(tSPS){
	    DEBSHASET(ss,"--- FillFaceSFS apres fufa",SFS," AddStartElement SFS+ face ");  
	    GdumpSHA(flfr2,(Standard_Address)ss.ToCString());
	    cout<<" ";TopAbs::Print(TB1,cout)<<" : 1 face ";
	    TopAbs::Print(flfr2.Orientation(),cout); cout<<endl;
	  }
#endif
	  SFS.AddStartElement(flfr2);
	}
      } // performfufa (context)
      
    } // makemerge
  } // tosplit && tomerge
  
  else if ( tosplit && !tomerge ) {    
    GSplitFace(FOR,Gin,LSO2);
    GSplitFaceSFS(FOR,LSO2,Gin,SFS);
  }
  else if ( !tosplit && !tomerge ) {
    GSplitFaceSFS(FOR,LSO2,Gin,SFS);
  }
  
  myEdgeAvoid.Clear();
  
#ifdef OCCT_DEBUG
  if(tSPS){
    GdumpSHASTA(FOR,TB1,"--- GFillFaceSFS END ");cout<<endl;
  }
#endif
 
} // GFillFaceSFS
