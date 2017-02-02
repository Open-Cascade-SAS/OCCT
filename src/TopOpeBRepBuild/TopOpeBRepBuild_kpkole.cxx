// Created on: 1994-08-30
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


#include <BRep_Builder.hxx>
#include <gp_Pnt.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_ProgramError.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopOpeBRepBuild_Builder.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_EdgeBuilder.hxx>
#include <TopOpeBRepBuild_FaceBuilder.hxx>
#include <TopOpeBRepBuild_GTopo.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepBuild_kpresu.hxx>
#include <TopOpeBRepBuild_PaveSet.hxx>
#include <TopOpeBRepBuild_ShapeSet.hxx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>
#include <TopOpeBRepBuild_SolidBuilder.hxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepBuild_WireToFace.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_connex.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopOpeBRepDS_ShapeShapeInterference.hxx>
#include <TopOpeBRepDS_SurfaceIterator.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepBuild_GettraceKPB();
void debiskole() {}
#endif

Standard_EXPORT Standard_Boolean FUNKP_KPiskolesh(const TopOpeBRepBuild_Builder& BU,const TopOpeBRepDS_DataStructure& BDS,const TopoDS_Shape& Sarg,TopTools_ListOfShape& lShsd,TopTools_ListOfShape& lfhsd);
Standard_EXPORT void FUNKP_KPmakefaces(const TopOpeBRepBuild_Builder& BU, const TopoDS_Shape& Fac1, const TopTools_ListOfShape& LF2,
				       const TopAbs_State Stfac1, const TopAbs_State Stfac2,
				       const Standard_Boolean R1, const Standard_Boolean R2,TopTools_ListOfShape& Lres);


//=======================================================================
//function : MergeKPartiskole
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Builder::MergeKPartiskole()
{
#ifdef OCCT_DEBUG
  Standard_Boolean TKPB = TopOpeBRepBuild_GettraceKPB();
  if (TKPB) KPreturn(myIsKPart);
  debiskole();
#endif
  
  Standard_Integer ibid;
  
  if ( myIsKPart != 1 ) return;
  
  GMapShapes(myShape1,myShape2);
  // NYI : on doit pouvoir faire l'economie du mapping GMapShapes(...)
  // NYI en allant chercher l'indice 1,2 retourne par GShapeRank(S)
  // NYI dans la DS. l'index est defini pour tous les shapes HasSameDomain
  
  TopTools_ListOfShape& lmergesha1 = ChangeMerged(myShape1,myState1);
  ChangeMerged(myShape2,myState2);
  
  TopTools_ListOfShape lShsd1,lShsd2; // liste de solides HasSameDomain
  TopTools_ListOfShape lfhsd1,lfhsd2; // liste de faces HasSameDomain
  KPiskolesh(myShape1,lShsd1,lfhsd1);
  KPiskolesh(myShape2,lShsd2,lfhsd2);
  // traitement de tous les solides NYI
  TopoDS_Shape sol1 = lShsd1.First();
  TopoDS_Shape sol2 = lShsd2.First();
  
  ChangeMerged(sol1,myState1); 
  ChangeMerged(sol2,myState2);
  
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itm1;
  itm1.Initialize(myKPMAPf1f2);
  if ( ! itm1.More() ) return;
  
#ifdef OCCT_DEBUG
  if (TKPB) {
    cout<<""<<endl;
    for (; itm1.More();itm1.Next()) {
      const TopoDS_Shape& f = itm1.Key();
      Standard_Integer fi = myDataStructure->Shape(f);
      cout<<"face "<<fi<<" : ";
      const TopTools_ListOfShape& l = itm1.Value();
      TopTools_ListIteratorOfListOfShape it(l);
      for(; it.More(); it.Next()) {
	const TopoDS_Shape& ff = it.Value();      
	Standard_Integer ffi = myDataStructure->Shape(ff);
	cout<<ffi<<" ";
      }
      cout<<endl;
    }
    itm1.Initialize(myKPMAPf1f2);
  }
#endif
  
  TopTools_ListOfShape  LFIN;
  TopTools_ListOfShape* plfIN = NULL;
  const TopoDS_Shape* pfOU = NULL;
  const TopoDS_Shape* pfIN = NULL;
  
  for (; itm1.More();itm1.Next()) {
//    const TopoDS_Shape& f = itm1.Key();
//   myDataStructure->Shape(f); //DEB
    const TopTools_ListOfShape& los = itm1.Value();
    Standard_Boolean emp = los.IsEmpty();
    if (!emp) {
      if (plfIN == NULL) plfIN = (TopTools_ListOfShape*)&itm1.Value();
      if (pfOU == NULL) pfOU = &itm1.Key();
      if (pfIN == NULL) pfIN = &plfIN->First();
      for (TopTools_ListIteratorOfListOfShape it(los);it.More();it.Next()) LFIN.Append(it.Value());
    }
  }
  
  if ( plfIN==NULL) return;
  if ( pfOU==NULL) return;
  if ( pfIN==NULL) return;
  
#ifdef OCCT_DEBUG
  Standard_Integer ifOU; Standard_Boolean tSPS = GtraceSPS(*pfOU,ifOU);
  if(tSPS || TKPB) { 
//    Standard_Integer iOU = myDataStructure->Shape(*pfOU);
//    Standard_Integer iIN = myDataStructure->Shape(*pfIN);
    GdumpSHA(*pfOU, (char *) "MergeKPartiskole pfOU ");cout<<endl;
    GdumpSAMDOM(LFIN, (char *) "LFIN : ");
    debiskole();
  }
#endif
  
  Standard_Integer rankpfOU = GShapeRank(*pfOU);
  Standard_Integer rankpfIN = GShapeRank(*pfIN);
  if ( rankpfOU != 1 && rankpfOU != 2 ) return;
  if ( rankpfIN != 1 && rankpfIN != 2 ) return;
  
  // solfOU = solide dont la face *pfOU est OUT / faces LFIN
  // solfIN = solide dont les faces *plfIN sont IN / face *pfOU
  TopoDS_Shape solfOU;
  if (rankpfOU == 1) solfOU = sol1;
  else               solfOU = sol2; 
  TopoDS_Shape solfIN;
  if (rankpfIN == 1) solfIN = sol1;
  else               solfIN = sol2; 
  TopAbs_State stsolfOU = KPclasSS(solfOU,*pfOU, solfIN);
  TopAbs_State stsolfIN = KPclasSS(solfIN,LFIN,solfOU);
  TopAbs_State stfOU = TopAbs_OUT;
  TopAbs_State stfIN = TopAbs_IN;
  
  TopAbs_State stsol1=TopAbs_UNKNOWN,stsol2=TopAbs_UNKNOWN;
  TopAbs_State stfac1=TopAbs_UNKNOWN,stfac2=TopAbs_UNKNOWN;
  TopoDS_Shape fac1,fac2;
  if      (rankpfOU == 1 ) {
    stsol1 = stsolfOU; stfac1 = stfOU; fac1 = *pfOU;
    stsol2 = stsolfIN; stfac2 = stfIN; fac2 = *pfIN;
  }
  else if (rankpfOU == 2 ) {
    stsol1 = stsolfIN; stfac1 = stfIN; fac1 = *pfIN;
    stsol2 = stsolfOU; stfac2 = stfOU; fac2 = *pfOU;
  }
  
  Standard_Integer ires,icla1,icla2;
  KPiskoleanalyse(stfac1,stfac2,stsol1,stsol2,ires,icla1,icla2);
  if (ires == RESUNDEF) return;
  if (icla1 == SHEUNDEF || icla2 == SHEUNDEF) return;
  
  TopoDS_Shape she1; // she1 = shell accedant fac1
  TopTools_IndexedDataMapOfShapeListOfShape Mfacshe1;
  TopExp::MapShapesAndAncestors(sol1,TopAbs_FACE,TopAbs_SHELL,Mfacshe1);
  const TopTools_ListOfShape& lshe1 = Mfacshe1.FindFromKey(fac1);
  TopTools_ListIteratorOfListOfShape itlshe1(lshe1);
  she1 = itlshe1.Value(); 
  
  TopoDS_Shape she2; // she2 = shell accedant fac2
  TopTools_IndexedDataMapOfShapeListOfShape Mfacshe2;
  TopExp::MapShapesAndAncestors(sol2,TopAbs_FACE,TopAbs_SHELL,Mfacshe2);
  const TopTools_ListOfShape& lshe2 = Mfacshe2.FindFromKey(fac2);
  TopTools_ListIteratorOfListOfShape itlshe2(lshe2);
  she2 = itlshe2.Value();
  
  ChangeMerged(she1,myState1);
  ChangeMerged(she2,myState2);
  
#ifdef OCCT_DEBUG
  if (TKPB) { cout<<"stsol1 ";TopAbs::Print(stsol1,cout); cout<<" "; }
  if (TKPB) { cout<<"stsol2 ";TopAbs::Print(stsol2,cout); cout<<endl; }
  debiskole();
#endif
  
  TopoDS_Shell newshe;
  
  if      ( ires == RESNULL ) {
    return;
  }
  
  else if (ires == RESSHAPE1) {
    myBuildTool.MakeShell(newshe);
    newshe = TopoDS::Shell(she1);
  }
  
  else if (ires == RESSHAPE2) {
    myBuildTool.MakeShell(newshe);
    newshe = TopoDS::Shell(she2);
  }
  
  else if ( ires == RESFACE1 ) {
    if      (rankpfOU == 1) {
      // resultat = face de rang 1 et face de rang 1 = face OUT
      lmergesha1.Append(*pfOU);
      ChangeMerged(fac2,myState2).Append(*pfOU);
    }
    else if (rankpfOU == 2) {
      // resultat = face de rang 1 et face de rang 1 = faces IN
      GCopyList(*plfIN,lmergesha1);
      GCopyList(*plfIN,ChangeMerged(fac2,myState2));
    }
    return;
  }
  
  else if ( ires == RESFACE2 ) {
    if      (rankpfOU == 2) {
      // resultat = face de rang 2 et face de rang 2 = face OUT
      lmergesha1.Append(*pfOU);
      ChangeMerged(fac1,myState1).Append(*pfOU);
    }
    else if (rankpfOU == 1) {
      // resultat = face de rang 2 et face de rang 2 = faces IN
      GCopyList(*plfIN,lmergesha1);
      GCopyList(*plfIN,ChangeMerged(fac1,myState1));
    }
    return;
  }
  
  else if (ires == RESNEWSHE) {
    
    itm1.Initialize(myKPMAPf1f2);
    if (! itm1.More() ) return;
    
    TopTools_DataMapOfShapeShape addedfaces;
    for (; itm1.More();itm1.Next()) {
      
      const TopoDS_Shape& f1 = itm1.Key();	
      const TopTools_ListOfShape& lf2 = itm1.Value();
      if (lf2.IsEmpty()) continue;
      
      TopTools_ListIteratorOfListOfShape it2;
      it2.Initialize(lf2);
      const TopoDS_Shape& f2 = it2.Value();
      
/*#ifdef OCCT_DEBUG
      Standard_Integer ii1 = myDataStructure->Shape(f1);
      Standard_Integer ii2 = myDataStructure->Shape(f2);
#endif*/
      Standard_Integer rankf1 = GShapeRank(f1);
      Standard_Integer rankf2 = GShapeRank(f2);
      if (rankf1 == 0) continue;
      if (rankf2 == 0) continue;
      
      TopAbs_State stf1,stf2; KPclassFF(f1,f2,stf1,stf2);
      if ( rankf1 == 1 ) KPiskoleanalyse(stf1,stf2,stsol1,stsol2,ires,ibid,ibid);
      if ( rankf1 == 2 ) KPiskoleanalyse(stf2,stf1,stsol2,stsol1,ires,ibid,ibid);
      if (ires == RESUNDEF) continue;
      
      Standard_Boolean r1 = (stsol1 == TopAbs_IN);
      Standard_Boolean r2 = (stsol2 == TopAbs_IN);
      TopoDS_Shape fac;
      if ( rankf1 == 1 ) fac = KPmakeface(f1,lf2,stf1,stf2,r1,r2);
      if ( rankf1 == 2 ) fac = KPmakeface(f1,lf2,stf1,stf2,r2,r1);
      if ( fac.IsNull() ) continue;
      if ( ! fac.IsNull() ) addedfaces.Bind(fac,fac);
      
      TopAbs_State statemergef1 = (rankf1 == 1) ? myState1 : myState2;
      TopAbs_State statemergef2 = (rankf2 == 2) ? myState2 : myState1;
      ChangeMerged(f1,statemergef1).Append(fac);
      it2.Initialize(lf2);
      for (;it2.More();it2.Next()) 
	ChangeMerged(it2.Value(),statemergef2).Append(fac);
      
      // les faces de she1 sauf les tangentes et celles deja ajoutees
      TopOpeBRepTool_ShapeExplorer fex1;
      for (fex1.Init(she1,TopAbs_FACE); fex1.More(); fex1.Next()) {
	const TopoDS_Shape& facur = fex1.Current();
	
	Standard_Boolean isfsd   = myKPMAPf1f2.IsBound(facur);
	Standard_Boolean isadded = addedfaces.IsBound(facur);
	Standard_Boolean toadd =  (!isfsd) && (!isadded) ;
	
	if ( toadd ) {
	  TopoDS_Shape fori = facur;
	  if (stsol1 == TopAbs_IN) fori.Complement();
	  addedfaces.Bind(fori,fori);
	}
      }
      
      // les faces de she2 sauf les tangentes et celles deja ajoutees
      TopOpeBRepTool_ShapeExplorer fex2;
      for (fex2.Init(she2,TopAbs_FACE); fex2.More(); fex2.Next()) {
	const TopoDS_Shape& facur = fex2.Current();
	
	Standard_Boolean isfsd   = myKPMAPf1f2.IsBound(facur);
	Standard_Boolean isadded = addedfaces.IsBound(facur);
	Standard_Boolean toadd =  (!isfsd) && (!isadded) ;
	
	if ( toadd ) {
	  TopoDS_Shape fori = facur;
	  if (stsol2 == TopAbs_IN) fori.Complement();
	  addedfaces.Bind(fori,fori);
	}
      }
    }  // === fin iteration fac1,fac2
    
    TopTools_DataMapIteratorOfDataMapOfShapeShape itadd(addedfaces);
    Standard_Boolean yauadd = itadd.More();
    if (yauadd) {
      myBuildTool.MakeShell(newshe);
      myBuildTool.Closed(newshe,Standard_True);  // NYI : check exact du caractere closed du shell
    }
    for (; itadd.More(); itadd.Next() ) {
      const TopoDS_Shape& ftoadd = itadd.Key();
      myBuildTool.AddShellFace(newshe,ftoadd);
    }
    
  } // === fin RESNEWSHE 
  
  else {
#ifdef OCCT_DEBUG
    cout<<"MergeKPartiskole : ires = "<<ires<<endl;
#endif
  }
  
  TopoDS_Solid newsol;
  if ( !newshe.IsNull() ) {
    myBuildTool.MakeSolid(newsol);
    myBuildTool.AddSolidShell(newsol,newshe);
  }
  
  if ( icla1 == SHECLASAUTR || icla1 == SHEGARDAUTR ) {
    // n.b. : ne pas prendre she1 accedant f1
    TopTools_ListOfShape loshe1;
    TopOpeBRepTool_ShapeExplorer ex1;
    for (ex1.Init(sol1,TopAbs_SHELL); ex1.More(); ex1.Next()) {
      const TopoDS_Shape& shecur = ex1.Current();
      if (she1.IsEqual(shecur)) continue;
      if (icla1 == SHECLASAUTR) {
	TopAbs_State state1 = KPclasSS(shecur,fac1,sol2);
	if (state1 == myState1) loshe1.Append(shecur);
      }
      else if (icla1 == SHEGARDAUTR) {
	loshe1.Append(shecur);
      }
    }
#ifdef OCCT_DEBUG
//    Standard_Integer nshe1 = loshe1.Extent();
#endif
    TopTools_ListIteratorOfListOfShape itloshe1;
    for( itloshe1.Initialize(loshe1); itloshe1.More(); itloshe1.Next() ) {
      const TopoDS_Shape& shecur = itloshe1.Value();
      myBuildTool.AddSolidShell(newsol,shecur);
    }
  }
  
  if ( icla2 == SHECLASAUTR || icla2 == SHEGARDAUTR ) {
    // n.b. : ne pas prendre she2 accedant f2
    TopTools_ListOfShape loshe2;
    TopOpeBRepTool_ShapeExplorer ex2;
    for (ex2.Init(sol2,TopAbs_SHELL); ex2.More(); ex2.Next()) {
      const TopoDS_Shape& shecur = ex2.Current();
      if (she2.IsEqual(shecur)) continue;
      if      (icla2 == SHECLASAUTR) {
	TopAbs_State state2 = KPclasSS(shecur,fac2,sol1);
	if (state2 == myState2) loshe2.Append(shecur);
      }
      else if (icla2 == SHEGARDAUTR) {
	loshe2.Append(shecur);
      }
    }
#ifdef OCCT_DEBUG
//    Standard_Integer nshe2 = loshe2.Extent();
#endif
    TopTools_ListIteratorOfListOfShape itloshe2;
    for( itloshe2.Initialize(loshe2); itloshe2.More(); itloshe2.Next() ) {
      const TopoDS_Shape& shecur = itloshe2.Value();
      myBuildTool.AddSolidShell(newsol,shecur);
    }
  }
  
  // le solide final
  if ( !newsol.IsNull() ) {
    lmergesha1.Append(newsol);
  }
  
} // MergeKPartiskole


//=======================================================================
//function : KPiskole
//purpose  : detection faces collees
//=======================================================================

Standard_Integer TopOpeBRepBuild_Builder::KPiskole()
{  
  
  TopTools_ListOfShape lShsd1,lShsd2; // liste de solides HasSameDomain
  TopTools_ListOfShape lfhsd1,lfhsd2; // liste de faces HasSameDomain
  
  Standard_Boolean iskp1 = KPiskolesh(myShape1,lShsd1,lfhsd1);
  if ( !iskp1 ) return 0;
  Standard_Integer nfhsd1 = lfhsd1.Extent();
  if ( nfhsd1 == 0 ) return 0;
  
  Standard_Boolean iskp2 = KPiskolesh(myShape2,lShsd2,lfhsd2);
  if ( !iskp2 ) return 0;
  Standard_Integer nfhsd2 = lfhsd2.Extent();
  if ( nfhsd2 == 0 ) return 0;
  
  // Si l'un des objets est constitue de plusieur solides on passe
  // dans le cas general , sinon on obtient
  //** Exception ** Standard_OutOfRange: TCollection_IndexedDataMap::FindFromKey at
  // TopOpeBRepBuild_Builder::MergeKPartiskole(this = 0xf7988), 
  // line 397 in "/adv_21/MDL/k1deb/ref/prod/TopOpeBRepBuild/src/TopOpeBRepBuild_KPart.cxx"
  // DPF le 10/07/1997
  Standard_Integer nshsd1 = lShsd1.Extent();
  Standard_Integer nshsd2 = lShsd2.Extent();
  if (nshsd1>1 || nshsd2>1) return 0;
  
  TopTools_ListOfShape lf1,lf2;
  TopTools_ListOfShape les; //section
  
  for (TopTools_ListIteratorOfListOfShape itlf1(lfhsd1);
       itlf1.More();itlf1.Next()) {
    
    const TopoDS_Shape& f1 = itlf1.Value();
#ifdef OCCT_DEBUG
//    Standard_Boolean isb1 = myKPMAPf1f2.IsBound(f1); // DEB
#endif
    lf1.Clear(); lf1.Append(f1);
    lf2.Clear(); KPSameDomain(lf1,lf2);
#ifdef OCCT_DEBUG
//    Standard_Integer n1 = lf1.Extent();
//    Standard_Integer n2 = lf2.Extent();
#endif
    
#ifdef OCCT_DEBUG
    Standard_Integer iF1; Standard_Boolean tSPS1 = GtraceSPS(f1,iF1);
    if(tSPS1) { 
      GdumpSHA(f1, (char *) "KPiskole ");cout<<endl;
      GdumpSAMDOM(lf2, (char *) "lf2 : ");
    }
#endif
    
    for (TopTools_ListIteratorOfListOfShape itlf2(lf2); 
	 itlf2.More(); itlf2.Next() ) {
      
      const TopoDS_Shape& f2 = itlf2.Value();
#ifdef OCCT_DEBUG
//      Standard_Boolean isb2 = myKPMAPf1f2.IsBound(f2); // DEB
#endif
      TopAbs_State state1,state2;
      Standard_Boolean classok = KPiskoleFF(f1,f2,state1,state2);
      if ( ! classok ) return 0;
      
      // on va reconstuire la face OUT
      if ( state1 == TopAbs_OUT && state2 == TopAbs_IN) {
	Standard_Boolean isb1 = myKPMAPf1f2.IsBound(f1);
	if ( ! isb1 ) { TopTools_ListOfShape los; myKPMAPf1f2.Bind(f1,los); }
	TopTools_ListOfShape& los = myKPMAPf1f2.ChangeFind(f1);
	los.Append(f2);
	
	Standard_Boolean isb2 = myKPMAPf1f2.IsBound(f2);
	if ( ! isb2 ) { TopTools_ListOfShape los1; myKPMAPf1f2.Bind(f2,los1); }
      }
      else if ( state2 == TopAbs_OUT && state1 == TopAbs_IN) {
	Standard_Boolean isb2 = myKPMAPf1f2.IsBound(f2);
	if ( ! isb2 ) { TopTools_ListOfShape los; myKPMAPf1f2.Bind(f2,los); }
	TopTools_ListOfShape& los = myKPMAPf1f2.ChangeFind(f2);
	los.Append(f1);
	
	Standard_Boolean isb1 = myKPMAPf1f2.IsBound(f1);
	if ( ! isb1 ) { TopTools_ListOfShape los1; myKPMAPf1f2.Bind(f1,los1); }
      }
      
      // les aretes de la face IN sont des aretes de section
      TopoDS_Shape fw;
      if      (state1 == TopAbs_IN) fw = f1;
      else if (state2 == TopAbs_IN) fw = f2;
      if (fw.IsNull()) continue;
      
      TopOpeBRepTool_ShapeExplorer ex(fw,TopAbs_EDGE);
      for (;ex.More();ex.Next()) les.Append(ex.Current());
    } 
  }
  
  // aretes de section iskole
  TopOpeBRepDS_DataStructure& DS = myDataStructure->ChangeDS();
  DS.InitSectionEdges(); TopTools_ListIteratorOfListOfShape it(les);
  for (;it.More();it.Next()) DS.AddSectionEdge(TopoDS::Edge(it.Value()));
  
  return 1;
} // TopOpeBRepBuild_Builder::KPiskole

//=======================================================================
//function : KPiskoleanalyse
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Builder::KPiskoleanalyse(const TopAbs_State Stfac1, const TopAbs_State Stfac2,
			   const TopAbs_State Stsol1, const TopAbs_State Stsol2,
			   Standard_Integer& ires,Standard_Integer& icla1,Standard_Integer& icla2) const 
{
  ires = RESUNDEF; icla1 = icla2 = SHEUNDEF;
  
  if      (Opefus())  {
    if      (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_OUT) {
      ires = RESNEWSHE; icla1 = SHEGARDAUTR; icla2 = SHEGARDAUTR;
    }
    else if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_IN ) {
      ires = RESSHAPE1; icla1 = SHECLASAUTR; icla2 = SHEAUCU;
    }
    else if (Stsol1 == TopAbs_IN  && Stsol2 == TopAbs_OUT) {
      ires = RESSHAPE2; icla1 = SHEAUCU; icla2 = SHECLASAUTR;
    }
  }
  else if (Opec12()) {
    if      (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_OUT) {
      ires = RESSHAPE1; icla1 = SHEGARDAUTR; icla2 = SHEAUCU;
    }
    else if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_IN ) {
      ires = RESNEWSHE; icla1 = SHECLASAUTR; icla2 = SHEAUCU;
    }
    else if (Stsol1 == TopAbs_IN  && Stsol2 == TopAbs_OUT) {
      ires = RESNULL; icla1 = icla2 = SHEAUCU;
    }
  }
  else if (Opec21()) {
    if      (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_OUT) {
      ires = RESSHAPE2; icla1 = SHEAUCU; icla2 = SHEGARDAUTR;
    }
    else if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_IN ) {
      ires = RESNULL; icla1 = icla2 = SHEAUCU;
    }
    else if (Stsol1 == TopAbs_IN  && Stsol2 == TopAbs_OUT) {
      ires = RESNEWSHE; icla1 = SHEAUCU; icla2 = SHECLASAUTR;
    }
  }
  else if (Opecom()) {
    if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_OUT) { 
      if (Stfac1 == TopAbs_IN) {
	ires = RESFACE1; icla1 = icla2 = SHEAUCU;
      }
      if (Stfac2 == TopAbs_IN) {
	ires = RESFACE2; icla1 = icla2 = SHEAUCU;
      }
    }
    else if (Stsol1 == TopAbs_OUT && Stsol2 == TopAbs_IN ) {
      ires = RESSHAPE2; icla1 = SHECLASAUTR; icla2 = SHEGARDAUTR;
    }
    else if (Stsol1 == TopAbs_IN  && Stsol2 == TopAbs_OUT) {
      ires = RESSHAPE1; icla1 = SHEGARDAUTR; icla2 = SHECLASAUTR;
    }
  }
  
#ifdef OCCT_DEBUG
  Standard_Boolean TKPB = TopOpeBRepBuild_GettraceKPB();
  if (TKPB) cout<<"ires = "<<ires<<" icla1 "<<icla1<<" icla2 "<<icla2<<endl;
#endif
} // TopOpeBRepBuild_Builder::KPiskoleanalyse

Standard_EXPORT void FUNKP_KPmakefaces(const TopOpeBRepBuild_Builder& BU,
                                       const TopoDS_Shape& Fac1,
                                       const TopTools_ListOfShape& LF2,
				       const TopAbs_State Stfac1,
                                       const TopAbs_State /*Stfac2*/,
				       const Standard_Boolean R1,
                                       const Standard_Boolean R2,
                                       TopTools_ListOfShape& Lres)
{
  // reconstruisons la face qui contient les autres
  BRep_Builder BB;
  TopoDS_Face fac; BB.MakeFace(fac);
  
  Standard_Integer rankIN = 0;
  TopTools_ListOfShape LFSO,LFDO;
  
#ifdef OCCT_DEBUG
  Standard_Integer iF1; Standard_Boolean tSPS = BU.GtraceSPS(Fac1,iF1);
  if(tSPS) { BU.GdumpSHA(Fac1, (char *) "KPmakeFace ");cout<<endl; }
#endif
  
  if (Stfac1 == TopAbs_OUT) {
    TopoDS_Shape aLocalShape = Fac1.EmptyCopied();
    fac = TopoDS::Face(aLocalShape);
//    fac = TopoDS::Face(Fac1.EmptyCopied());
    Standard_Integer rankF = BU.GShapeRank(Fac1);
    rankIN = (rankF) ? ( (rankF==1) ? 2 : 1) : 0;
    BU.GFindSamDomSODO(Fac1,LFSO,LFDO);
  }
  else {
    throw Standard_ProgramError("KPmakeface Stfac1 != OUT");
  }
  
  if (rankIN == 0) {
    throw Standard_ProgramError("KPmakeface rankIN = 0");
  }
  
  TopTools_ListOfShape LFIN;
  BU.GFindSameRank(LFSO,rankIN,LFIN);
  BU.GFindSameRank(LFDO,rankIN,LFIN);
  
#ifdef OCCT_DEBUG
  if(tSPS) { 
    BU.GdumpSAMDOM(LFSO, (char *) "LESO : ");
    BU.GdumpSAMDOM(LFDO, (char *) "LEDO : ");
    BU.GdumpSAMDOM(LFIN, (char *) "LFIN : ");
  }
#endif
  
  TopOpeBRepBuild_WireToFace wtof;
  
  TopOpeBRepTool_ShapeExplorer wex1;
  for (wex1.Init(Fac1,TopAbs_WIRE); wex1.More(); wex1.Next()) {
    const TopoDS_Shape& wicur = wex1.Current();
    TopoDS_Wire wori = TopoDS::Wire(wicur);
    if (R1) wori.Complement();
    wtof.AddWire(wori);
    //myBuildTool.AddFaceWire(fac,wori);
  }
  
  TopOpeBRepTool_ShapeExplorer wex2;
  for (TopTools_ListIteratorOfListOfShape it2(LF2);it2.More();it2.Next()) {
    const TopoDS_Shape& Fac2 = it2.Value();
    for (wex2.Init(Fac2,TopAbs_WIRE); wex2.More(); wex2.Next()) {
      const TopoDS_Shape& wicur = wex2.Current();
      TopoDS_Wire wori = TopoDS::Wire(wicur);
      if (R2) wori.Complement();
      wtof.AddWire(wori);
      //myBuildTool.AddFaceWire(fac,wori);
    }
  }
  
  const TopoDS_Face& F1 = TopoDS::Face(Fac1);
  wtof.MakeFaces(F1,Lres);
#ifdef OCCT_DEBUG
//  Standard_Integer nlres = Lres.Extent(); // DEB
#endif
  
  return;
} // FUNKP_KPmakefaces

//=======================================================================
//function : KPmakeface
//purpose  : 
//=======================================================================

TopoDS_Shape TopOpeBRepBuild_Builder::KPmakeface(const TopoDS_Shape& Fac1,
                                                 const TopTools_ListOfShape& LF2,
                                                 const TopAbs_State Stfac1,
                                                 const TopAbs_State /*Stfac2*/,
                                                 const Standard_Boolean R1,
                                                 const Standard_Boolean R2)
{
  // reconstruisons la face qui contient l'autre
  BRep_Builder BB;
  TopoDS_Face fac; BB.MakeFace(fac);
  
  Standard_Integer rankIN = 0;
  TopTools_ListOfShape LFSO,LFDO;
  
#ifdef OCCT_DEBUG
  Standard_Integer iF1;
  Standard_Boolean tSPS = GtraceSPS(Fac1,iF1);
  if(tSPS) {
    GdumpSHA(Fac1, (char *) "KPmakeFace ");
    cout<<endl;
  }
#endif
  
  if (Stfac1 == TopAbs_OUT) {
    TopoDS_Shape aLocalShape = Fac1.EmptyCopied();
    fac = TopoDS::Face(aLocalShape);
//    fac = TopoDS::Face(Fac1.EmptyCopied());
    Standard_Integer rankF = GShapeRank(Fac1);
    rankIN = (rankF) ? ( (rankF==1) ? 2 : 1) : 0;
    GFindSamDomSODO(Fac1,LFSO,LFDO);
  }
  else {
    throw Standard_ProgramError("KPmakeface Stfac1 != OUT");
  }
  
  if (rankIN == 0) {
    throw Standard_ProgramError("KPmakeface rankIN = 0");
  }
  
  TopTools_ListOfShape LFIN;
  GFindSameRank(LFSO,rankIN,LFIN);
  GFindSameRank(LFDO,rankIN,LFIN);
  
#ifdef OCCT_DEBUG
  if(tSPS) { 
    GdumpSAMDOM(LFSO, (char *) "LESO : ");
    GdumpSAMDOM(LFDO, (char *) "LEDO : ");
    GdumpSAMDOM(LFIN, (char *) "LFIN : ");
  }
#endif
  
  
  TopOpeBRepTool_ShapeExplorer wex1;
  for (wex1.Init(Fac1,TopAbs_WIRE); wex1.More(); wex1.Next()) {
    const TopoDS_Shape& wicur = wex1.Current();
    TopoDS_Shape wori = wicur;
    if (R1) wori.Complement();
    myBuildTool.AddFaceWire(fac,wori);
  }
  
  TopOpeBRepTool_ShapeExplorer wex2;
  for (TopTools_ListIteratorOfListOfShape it2(LF2);it2.More();it2.Next()) {
    const TopoDS_Shape& Fac2 = it2.Value();
    for (wex2.Init(Fac2,TopAbs_WIRE); wex2.More(); wex2.Next()) {
      const TopoDS_Shape& wicur = wex2.Current();
      TopoDS_Shape wori = wicur;
      if (R2) wori.Complement();
      myBuildTool.AddFaceWire(fac,wori);
    }
  }
  
  return fac;
} // TopOpeBRepBuild_Builder::KPmakeface

Standard_EXPORT Standard_Boolean FUNKP_KPiskolesh(const TopOpeBRepBuild_Builder& BU,
                                                  const TopOpeBRepDS_DataStructure& BDS,
                                                  const TopoDS_Shape& Sarg,
                                                  TopTools_ListOfShape& lShsd,
                                                  TopTools_ListOfShape& /*lfhsd*/)
     // <lShsd> : the list of solids same domain with <Sarg>
     // sol is  <lShsd>'s first solid      
     // <lfhsd> : the list of <sol>'s same domain faces, none of the list carries geometric interf
{
  if ( Sarg.IsNull() ) return Standard_False;
  
  Standard_Integer nsol = BU.KPlhsd(Sarg,TopAbs_SOLID,lShsd);
  if ( nsol == 0 ) return Standard_False;
  const TopoDS_Shape& sol = lShsd.First();
  
  TopTools_ListOfShape lfhg; 
  Standard_Integer nfhg = BU.KPlhg(sol,TopAbs_FACE,lfhg);
  if ( nfhg != 0 ) {
    TopTools_ListIteratorOfListOfShape its(lfhg);
    for(; its.More(); its.Next()) {
      TopOpeBRepDS_ListIteratorOfListOfInterference iti(BDS.ShapeInterferences(its.Value()));
      for (;iti.More();iti.Next()) {
	Handle(TopOpeBRepDS_ShapeShapeInterference) ssi;
	ssi = Handle(TopOpeBRepDS_ShapeShapeInterference)::DownCast(iti.Value());
	if (ssi.IsNull()) {
	  return Standard_False;
	}
      }
    }  
  }
  return Standard_True;
} // FUNKP_KPiskolesh

//=======================================================================
//function : KPiskolesh
//purpose  : 
// KPiskolesh : 
// S est il un shape traite par le cas particulier du collage ?
// si oui : retourne un solide et une liste de faces de collage
//=======================================================================

Standard_Boolean TopOpeBRepBuild_Builder::KPiskolesh(const TopoDS_Shape& Sarg,
                                                     TopTools_ListOfShape& lShsd,
                                                     TopTools_ListOfShape& lfhsd) const 
{
#ifdef OCCT_DEBUG
  Standard_Boolean TKPB = TopOpeBRepBuild_GettraceKPB();
#endif
  const TopOpeBRepDS_DataStructure& BDS = myDataStructure->DS();
  Standard_Boolean iskolesh = FUNKP_KPiskolesh(*this,BDS,Sarg,lShsd,lfhsd);
  if (!iskolesh) return Standard_False;
  
#ifdef OCCT_DEBUG
  Standard_Integer nfhsd =
#endif
              KPlhsd(Sarg,TopAbs_FACE,lfhsd);
  TopTools_ListIteratorOfListOfShape it(lfhsd);
  for (; it.More(); it.Next() ) {
    const TopoDS_Shape& fac = it.Value();    
    Standard_Boolean isplan = FUN_tool_plane(fac); //pro7993 BUG
    if ( !isplan ) return Standard_False;

    Standard_Integer nw = KPls(fac,TopAbs_WIRE);
    if (nw > 1) return Standard_False; 

    TopTools_ListOfShape lehg;
    Standard_Integer nehg = KPlhg(fac,TopAbs_EDGE,lehg);
    if ( nehg != 0 ) return Standard_False;
    
#ifdef OCCT_DEBUG
    Standard_Integer isol = myDataStructure->Shape(Sarg);
    Standard_Integer ifac = myDataStructure->Shape(fac); 
    if(TKPB){cout<<"isol "<<isol<<endl;}
    if(TKPB){cout<<"nfhsd  "<<nfhsd<<endl;}
    if(TKPB){cout<<"ifac "<<ifac<<endl;}
    if(TKPB){cout<<"isplan "<<isplan<<endl;}
    if(TKPB){cout<<"nehg "<<nehg<<endl;}
    if(TKPB){cout<<endl;}
#endif
  }
  
  return Standard_True;
} // TopOpeBRepBuild_Builder::KPiskolesh
