// Created on: 1996-03-21
// Created by: Modelistation
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

#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_EdgeInterferenceTool.hxx>
#include <TopOpeBRepDS_EdgeVertexInterference.hxx>
#include <TopOpeBRepDS_CurvePointInterference.hxx>
#include <TopOpeBRepDS_TKI.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopExp_Explorer.hxx>
#include <TopExp.hxx>
#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>
#include <BRep_Tool.hxx>
#include <Precision.hxx>
#include <Geom_Curve.hxx>
#include <BRep_Builder.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepDS_connex.hxx>
#include <TopOpeBRepTool_SC.hxx>

#define MDShcpi Handle(TopOpeBRepDS_CurvePointInterference)
#define MAKECPI(IJKLM) (Handle(TopOpeBRepDS_CurvePointInterference)::DownCast(IJKLM))

//------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_vertexofedge
//------------------------------------------------------
// FUN_vertexofedge :  True si le vertex V est un vertex de E
(const TopoDS_Shape& V, const TopoDS_Shape& E)
{
  Standard_Boolean isv = Standard_False;
  TopExp_Explorer ex;
  for (ex.Init(E,TopAbs_VERTEX); ex.More(); ex.Next())
//  for (TopExp_Explorer ex(E,TopAbs_VERTEX); ex.More(); ex.Next())
    if (ex.Current().IsSame(V) ) {
      isv = Standard_True;
      break;
    }
  return isv;
}

//------------------------------------------------------
static Standard_Boolean FUN_keepEinterference
//------------------------------------------------------
(const TopOpeBRepDS_DataStructure& DS,const Handle(TopOpeBRepDS_Interference)& I,const TopoDS_Shape& E)
{
  TopOpeBRepDS_Kind GT1,ST1; Standard_Integer G1,S1; FDS_data(I,GT1,G1,ST1,S1);
  
  Standard_Boolean res = Standard_True;
  if ( I->IsKind(STANDARD_TYPE(TopOpeBRepDS_EdgeVertexInterference)) ) {
    // EVI I rejetee si son arete-support est E accedant I
    Standard_Boolean k1 = ! ::FUN_interfhassupport(DS,I,E);
    res = k1;
    
    // EVI rejetee si transition ON EDGE before ou after
    const TopOpeBRepDS_Transition& T = I->Transition();
    TopAbs_ShapeEnum shab = T.ShapeBefore(),shaa = T.ShapeAfter();
    TopAbs_State stab = T.Before(),staa = T.After();
    Standard_Boolean k2 = ! (((shab == TopAbs_EDGE) && (stab == TopAbs_ON)) ||
		((shaa == TopAbs_EDGE) && (staa == TopAbs_ON))); 
    res = res && k2;
    
    const TopoDS_Shape& VG = DS.Shape(I->Geometry());

    /*   xpu : 20-01-98
    // EVI I  rejetee si son vertex-geometrie est un vertex de l'arete 
    // qui accede I.
    Standard_Boolean k3 = ! ::FUN_vertexofedge(VG,E);
    res = res && k3;
    */

    // EVI rejetee si OUT FACE before et after
    // et si le vertex-geometrie de l'interference collisionne avec
    // un des vertex de l'arete (E) accedant l'interference (I)
    {
      TopoDS_Vertex Vf,Vr; TopExp::Vertices(TopoDS::Edge(E),Vf,Vr);
      TopTools_ListIteratorOfListOfShape it(DS.ShapeSameDomain(VG));
      for (; it.More(); it.Next()) {
	const TopoDS_Shape& Vsd = it.Value();
	if      ( Vsd.IsSame(Vf) ) { break; }
	else if ( Vsd.IsSame(Vr) ) { break; }
      }
    }
    //    res = res && k4;
  }

  else if ( I->IsKind(STANDARD_TYPE(TopOpeBRepDS_CurvePointInterference)) ) {
    Handle(TopOpeBRepDS_CurvePointInterference) aCPI =
      Handle(TopOpeBRepDS_CurvePointInterference)::DownCast(I);

    // MSV Oct 4, 2001: reject interference having the parameter coinciding with
    //                  one of sides of the edge range

    Standard_Real eps = Precision::PConfusion();
    Standard_Real par = aCPI->Parameter();
    Standard_Real f,l;
    BRep_Tool::Range(TopoDS::Edge(E), f,l);
    if (Abs(par-f) < eps || Abs(par-l) < eps)
      res = Standard_False;
  }

  return res;
}

//------------------------------------------------------
Standard_EXPORT Standard_Integer FUN_unkeepEinterferences
//------------------------------------------------------
(TopOpeBRepDS_ListOfInterference& LI,const TopOpeBRepDS_DataStructure& BDS,const Standard_Integer SIX)
{
 
  const TopoDS_Shape& E = BDS.Shape(SIX);
  Standard_Boolean isEd;
  isEd = BRep_Tool::Degenerated(TopoDS::Edge(E));
  
  if (isEd) {
    return LI.Extent();
  }
  
  TopOpeBRepDS_ListIteratorOfListOfInterference it1(LI);
  while (it1.More() ) {
    Handle(TopOpeBRepDS_Interference)& I1 = it1.Value();
    Standard_Boolean k1 = ::FUN_keepEinterference(BDS,I1,E);
    if ( !k1 ) {
      LI.Remove(it1);
      continue;
    }
    else {
      it1.Next();
    }
  }
  Standard_Integer n = LI.Extent();
  return n;
} // FUN_unkeepEinterferences

/*
//------------------------------------------------------
Standard_EXPORT void FUN_changeFOUT
//------------------------------------------------------
(TopOpeBRepDS_ListOfInterference& LF,const TopOpeBRepDS_ListOfInterference& LE,const TopOpeBRepDS_DataStructure& BDS,const Standard_Integer SIX)
{
  const TopoDS_Shape& E = BDS.Shape(SIX);
  
  // reduction du cas OUT(FACE),OUT(FACE) par un vertex si on 
  // trouve une transition EDGE par ce meme vertex.
  // pour toute interference Fi de LF = OUT(FACE),OUT(FACE) par vertex V
  // chercher une interference Ei de LE : 
  // 1/ Ei = IN(EDGE),OUT(EDGE) par vertex V <==> ON(FACE),OUT(EDGE) par vertex V
  // 2/ Ei = OUT(EDGE),IN(EDGE) par vertex V <==> OUT(EDGE),ON(EDGE) par vertex V
  // si trouve 1/ : Fi devient IN(FACE),OUT(FACE)
  // si trouve 2/ : Fe devient OUT(FACE),IN(FACE)
  
  TopOpeBRepDS_ListIteratorOfListOfInterference it1(LF);
  while (it1.More()) {
    Handle(TopOpeBRepDS_Interference)& I1 = it1.Value();
    TopOpeBRepDS_Kind GT1,ST1; Standard_Integer G1,S1; FDS_data(I1,GT1,G1,ST1,S1);
    
    if ( GT1 == TopOpeBRepDS_VERTEX ) {
      TopOpeBRepDS_Transition& T1 = I1->ChangeTransition();
      TopAbs_ShapeEnum shab1 = T1.ShapeBefore(), shaa1 = T1.ShapeAfter();
      TopAbs_State     stab1 = T1.Before(), staa1 = T1.After();
      const Standard_Boolean foub = (shab1 == TopAbs_FACE) && (stab1 == TopAbs_OUT);
      const Standard_Boolean foua = (shaa1 == TopAbs_FACE) && (staa1 == TopAbs_OUT);
      const Standard_Boolean foufou = foub && foua;
      if (!foufou) {
	it1.Next();
	continue;
      }
      
      TopOpeBRepDS_ListIteratorOfListOfInterference it2(LE);
      while (it2.More()) {
	const Handle(TopOpeBRepDS_Interference)& I2 = it2.Value();
	TopOpeBRepDS_Kind GT2,ST2; Standard_Integer G2,S2; FDS_data(I2,GT2,G2,ST2,S2);
	
	Standard_Boolean memver = ((GT2 == GT1) && (G2 == G1));
	if ( ! memver ) {
	  it2.Next();
	  continue;
	}
	
	const TopOpeBRepDS_Transition& T2 = I2->Transition();
	TopAbs_ShapeEnum shab2 = T2.ShapeBefore(), shaa2 = T2.ShapeAfter();
	TopAbs_State     stab2 = T2.Before(), staa2 = T2.After();
	const Standard_Boolean eoub = (shab2 == TopAbs_EDGE) && (stab2 == TopAbs_OUT);
	const Standard_Boolean einb = (shab2 == TopAbs_EDGE) && (stab2 == TopAbs_IN);
	const Standard_Boolean eoua = (shaa2 == TopAbs_EDGE) && (staa2 == TopAbs_OUT);
	const Standard_Boolean eina = (shaa2 == TopAbs_EDGE) && (staa2 == TopAbs_IN);
	const Standard_Boolean eouein = (eoub && eina);
	const Standard_Boolean eineou = (einb && eoua);
	Standard_Boolean changeFi = (eouein || eineou);
	if ( ! changeFi ) {
	  it2.Next();
	  continue;
	}
	
	// on modifie T1 de I1 et on arrete la pour I1
	if      (eouein) T1.After(TopAbs_IN,TopAbs_FACE);
	else if (eineou) T1.Before(TopAbs_IN,TopAbs_FACE);
	break;
	
	it2.Next();
      }
      
    }
    it1.Next();
  }
}*/

//------------------------------------------------------
Standard_EXPORT void FUN_unkeepEsymetrictransitions
//------------------------------------------------------
// unkeepEsymetric  : pour pallier au fonctionnement limite du TopTrans_CurveTransition
// qui ne peut pas gerer correctement deux fois la meme arete (FORWARD,REVERSED) 
// incidentes en un vertex. (cas d'un shell fait de deux faces partageant une arete.)
(TopOpeBRepDS_ListOfInterference& LI,const TopOpeBRepDS_DataStructure& BDS,const Standard_Integer SIX)		     
{
  const TopoDS_Shape& E = BDS.Shape(SIX);
  
  Standard_Boolean isEd;
  isEd = BRep_Tool::Degenerated(TopoDS::Edge(E));
  
  if (isEd) return;
  
  TopOpeBRepDS_ListIteratorOfListOfInterference it1;
  
  // process interferences of LI with VERTEX geometry
  
  it1.Initialize(LI);
  while (it1.More() ) {
    Standard_Boolean it1toremove = Standard_False;
    Handle(TopOpeBRepDS_Interference)& I1 = it1.Value();
    TopOpeBRepDS_Kind GT1,ST1; Standard_Integer G1,S1; FDS_data(I1,GT1,G1,ST1,S1);
    TopAbs_ShapeEnum tsb1,tsa1; Standard_Integer isb1,isa1; ::FDS_Tdata(I1,tsb1,isb1,tsa1,isa1);
    const TopOpeBRepDS_Transition T1 = I1->Transition();
    
    TopOpeBRepDS_ListIteratorOfListOfInterference it2(it1);
    it2.Next();
    while ( it2.More() ) {
      const Handle(TopOpeBRepDS_Interference)& I2 = it2.Value();
      TopOpeBRepDS_Kind GT2,ST2; Standard_Integer G2,S2; FDS_data(I2,GT2,G2,ST2,S2);
      TopAbs_ShapeEnum tsb2,tsa2; Standard_Integer isb2,isa2; ::FDS_Tdata(I2,tsb2,isb2,tsa2,isa2);
      const TopOpeBRepDS_Transition T2 = I2->Transition(); // + 03-03-97	
      Standard_Boolean idGS = (GT2 == GT1 && G2 == G1 && ST2 == ST1 && S2 == S1);
      Standard_Boolean idiba = (isb1 == isb2 && isa1 == isa2);
      Standard_Boolean cond = idGS;
      cond = cond && idiba; // ++971219

      if (cond) {
	Standard_Boolean idshape = ::FUN_transitionSHAPEEQUAL(T1,T2);
	Standard_Boolean idstate = ::FUN_transitionSTATEEQUAL(T1,T2);
	Standard_Boolean newV = Standard_True;
	Standard_Boolean oppostate = Standard_False;
	
	if (newV) {
	  // sym is not precise enough (croix3_3-> OUT/IN; ON/OUT)
#define M_OUTIN(st1,st2) ((st1 == TopAbs_OUT)&&(st2 == TopAbs_IN))
	  TopAbs_State t1b = T1.Before(), t2b = T2.Before();
	  TopAbs_State t1a = T1.After(), t2a = T2.After();
	  oppostate = M_OUTIN(t1b,t2b) || M_OUTIN(t2b,t1b);
	  oppostate = oppostate && (M_OUTIN(t1a,t2a) || M_OUTIN(t2a,t1a));
	}	  
	
	Standard_Boolean sym;
	if (newV) sym = idshape && oppostate;
	else sym = idshape && !idstate;
	
	if ( sym ) { // les 2 interferences ne different que leurs etats (symetriques)
	  LI.Remove(it2);
	  it1toremove = Standard_True;
	}
	else it2.Next();
      }
      else it2.Next();
    }
    if (it1toremove) {
      LI.Remove(it1);
    }
    else {
      it1.Next();
    }
  } // it1.More()
}

//------------------------------------------------------
Standard_EXPORT void FUN_orderFFsamedomain
//------------------------------------------------------
// partition de LI en deux sous listes : 
// L1/ = interfs dont la transition est definie / face samedomain
// L2/ = les autres interfs
// L = L1 + L2;
  (TopOpeBRepDS_ListOfInterference& LI,
   const Handle(TopOpeBRepDS_HDataStructure)& HDS, const Standard_Integer)
{
  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  TopOpeBRepDS_ListOfInterference LIffsd,LIother;
//  BDS.Shape(SIX);
  TopOpeBRepDS_ListIteratorOfListOfInterference it1;
  
  it1.Initialize(LI);
  while (it1.More() ) {
    Handle(TopOpeBRepDS_Interference)& I1 = it1.Value();
    TopOpeBRepDS_Kind GT1,ST1; Standard_Integer G1,S1; FDS_data(I1,GT1,G1,ST1,S1);
    TopAbs_ShapeEnum tsb1,tsa1; Standard_Integer isb1,isa1; FDS_Tdata(I1,tsb1,isb1,tsa1,isa1);
//    I1->Transition();
    
    Standard_Boolean ffsd = Standard_False;
    if (tsb1 == TopAbs_FACE && tsa1 == TopAbs_FACE) {
      // T1 states are defined on FACEs
      const TopoDS_Shape& fb = BDS.Shape(isb1);
      const TopoDS_Shape& fa = BDS.Shape(isa1);
      Standard_Boolean ffsd1 = HDS->HasSameDomain(fb);
      Standard_Boolean ffsd2 = HDS->HasSameDomain(fa);
      ffsd = ffsd1 && ffsd2;
    }
    
    if (ffsd) LIffsd.Append(I1);
    else      LIother.Append(I1);
    
    LI.Remove(it1);
  } // it1.More()
  
  LI.Clear();
  LI.Append(LIffsd);
  LI.Append(LIother);
} // FUN_orderFFsamedomain

//------------------------------------------------------
Standard_EXPORT void FUN_orderSTATETRANSonG
//------------------------------------------------------
// LI : liste d'interf en une meme Geometrie (K,G) 
// partition de LI en deux sous listes : 
// L1/ = interfs dont la transition a des etats egaux
// L2/ = les autres interfs
// L = L1 + L2;
(TopOpeBRepDS_ListOfInterference& LI,const Handle(TopOpeBRepDS_HDataStructure)& /*HDS*/, const Standard_Integer)
{
//  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  TopOpeBRepDS_ListOfInterference L1,L2;
  TopOpeBRepDS_ListIteratorOfListOfInterference it1;
  
  it1.Initialize(LI);
  while (it1.More() ) {
    Handle(TopOpeBRepDS_Interference)& I1 = it1.Value();
    TopOpeBRepDS_Kind GT1,ST1; Standard_Integer G1,S1; TopAbs_ShapeEnum tsb1,tsa1; Standard_Integer isb1,isa1; 
    FDS_Idata(I1,tsb1,isb1,tsa1,isa1,GT1,G1,ST1,S1);
    const TopOpeBRepDS_Transition& T1 = I1->Transition();
    TopAbs_State stab1 = T1.Before();
    TopAbs_State staa1 = T1.After();

    Standard_Boolean steq = (tsb1 == tsa1 && isb1 == isa1 && stab1 == staa1);
    if (steq) L1.Append(I1);
    else      L2.Append(I1);

    it1.Next();
  } // it1.More()
  
  LI.Clear();
  LI.Append(L1);
  LI.Append(L2);
} // FUN_orderSTATETRANSonG

//------------------------------------------------------
Standard_EXPORT void FUN_orderSTATETRANS
//------------------------------------------------------
// partition de LI en deux sous listes : 
// L1/ = interfs dont la transition a des etats egaux
// L2/ = les autres interfs
// L = L1 + L2;
(TopOpeBRepDS_ListOfInterference& LI,const Handle(TopOpeBRepDS_HDataStructure)& HDS,const Standard_Integer SIX)    
{
  TopOpeBRepDS_TKI tki;
  tki.FillOnGeometry(LI);
  tki.Init();
  for (; tki.More(); tki.Next()) {
    TopOpeBRepDS_Kind K; Standard_Integer G; TopOpeBRepDS_ListOfInterference& loi = tki.ChangeValue(K,G);
    ::FUN_orderSTATETRANSonG(loi,HDS,SIX);
  }

  LI.Clear();
  tki.Init();
  for (; tki.More(); tki.Next()) {
    TopOpeBRepDS_Kind K; Standard_Integer G; TopOpeBRepDS_ListOfInterference& loi = tki.ChangeValue(K,G);
    LI.Append(loi);
  }
} // FUN_orderSTATETRANS

//------------------------------------------------------
Standard_EXPORT void FUN_resolveEUNKNOWN
//------------------------------------------------------
(TopOpeBRepDS_ListOfInterference& LI,TopOpeBRepDS_DataStructure& BDS,const Standard_Integer SIX)
{
  const TopoDS_Shape& E = BDS.Shape(SIX);
  TopOpeBRepDS_ListIteratorOfListOfInterference it1;
  
  const TopoDS_Edge& EE = TopoDS::Edge(E);
  Standard_Real fE,lE; BRep_Tool::Range(EE,fE,lE);
  
  // process interferences of LI with UNKNOWN transition
  
  for (it1.Initialize(LI); it1.More(); it1.Next() ) {
    Handle(TopOpeBRepDS_Interference)& I1 = it1.Value();
    const TopOpeBRepDS_Transition& T1 = I1->Transition();
    Standard_Boolean isunk = T1.IsUnknown();
    if (!isunk) continue;
    
    TopOpeBRepDS_Kind GT1,ST1; Standard_Integer G1,S1; TopAbs_ShapeEnum tsb1,tsa1; Standard_Integer isb1,isa1; 
    FDS_Idata(I1,tsb1,isb1,tsa1,isa1,GT1,G1,ST1,S1);
    Standard_Boolean idt = (tsb1==TopAbs_FACE && tsa1==TopAbs_FACE 
	       && GT1==TopOpeBRepDS_POINT && ST1==TopOpeBRepDS_FACE);
    Standard_Boolean idi = (isb1==S1 && isa1==S1);
    Standard_Boolean etgf = idt && idi; // edge tangent a une face en 1 point
    if (!etgf) continue;

    Handle(TopOpeBRepDS_CurvePointInterference) cpi = MAKECPI(I1);
    if (cpi.IsNull()) continue;

    Standard_Real bid; Handle(Geom_Curve) CE = BRep_Tool::Curve(EE,bid,bid);
    if (CE.IsNull()) continue; // NYI : get points from 2d curve

    Standard_Real parcpi = cpi->Parameter();
    Standard_Real ttb = 0.8; Standard_Real parb = (1-ttb)*fE + ttb*parcpi;
    Standard_Real tta = 0.2; Standard_Real para = (1-tta)*parcpi + tta*lE;
    gp_Pnt Pb;CE->D0(parb,Pb);
    gp_Pnt Pa;CE->D0(para,Pa);

    BRep_Builder BB;
    const TopoDS_Face& FS = TopoDS::Face(BDS.Shape(S1));
    TopoDS_Shell sh; BB.MakeShell(sh);
    TopoDS_Solid so; BB.MakeSolid(so);
    BB.Add(sh,FS);BB.Add(so,sh);
    TopOpeBRepTool_ShapeClassifier& PSC = FSC_GetPSC(so);
    PSC.StateP3DReference(Pb);
    TopAbs_State stateb = PSC.State();
    PSC.StateP3DReference(Pa);
    TopAbs_State statea = PSC.State();
    if (stateb==TopAbs_UNKNOWN || statea==TopAbs_UNKNOWN) continue;

    TopOpeBRepDS_Transition& newT1 = I1->ChangeTransition();
    newT1.Set(stateb,statea,tsb1,tsa1);
  }
  FUN_unkeepUNKNOWN(LI,BDS,SIX);
}

// ----------------------------------------------------------------------
Standard_EXPORT void FUN_purgeDSonSE(const Handle(TopOpeBRepDS_HDataStructure)& HDS,const Standard_Integer EIX,TopOpeBRepDS_ListOfInterference& LI)
// ----------------------------------------------------------------------
{
  // recall  : (I1,I2) / I1=(T(F),G,S=edge), I2=(T(F),G,S=F) describes a 3d interference  
  //   
  // purpose : attached to EIX (section egde SE), I=(T(Fsdm),G,S) /
  //           Fsdm shares same domain with Fanc ancestor face of SE
  //            => SE has split ON near G =>
  //           I'=(T(Fsdm),G,S=Fsdm) gives bad information (3d information whereas
  //           we should only have 2d information)
  // - PRO12660 for spON(e48) - 
  
  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  const TopoDS_Edge& SE = TopoDS::Edge(BDS.Shape(EIX));
  Standard_Integer rkSE = BDS.AncestorRank(SE);
  Standard_Boolean isse = BDS.IsSectionEdge(SE);
  if (!isse) return;

  TopTools_MapOfShape fsdmFancSE;
  // ---------------
  const TopTools_ListOfShape& lFancSE = FDSCNX_EdgeConnexitySameShape(SE,HDS);
  for (TopTools_ListIteratorOfListOfShape itf(lFancSE); itf.More(); itf.Next()){
    const TopTools_ListOfShape& fsdm = BDS.ShapeSameDomain(itf.Value());
    for (TopTools_ListIteratorOfListOfShape itsdm(fsdm); itsdm.More(); itsdm.Next()){
      const TopoDS_Shape& sdmf = itsdm.Value();
      Standard_Integer sdmrk = BDS.AncestorRank(sdmf);
      if (sdmrk == rkSE) continue;
      fsdmFancSE.Add(sdmf);
    }
  }  
  if (fsdmFancSE.IsEmpty()) return;

  TopOpeBRepDS_ListOfInterference newLI;
  // ---------
  TopOpeBRepDS_TKI tki;
  tki.FillOnGeometry(LI);       
  for (tki.Init(); tki.More(); tki.Next()) {  
  
    TopOpeBRepDS_Kind Kcur; Standard_Integer Gcur; TopOpeBRepDS_ListOfInterference& loi = tki.ChangeValue(Kcur,Gcur);  
    TopOpeBRepDS_ListIteratorOfListOfInterference it(loi);
//    for (; it.More(); it.Next()){
//      const Handle(TopOpeBRepDS_Interference)& I = it.Value();
//      TopAbs_ShapeEnum tsb,tsa; Standard_Integer isb,isa; FDS_Tdata(I,tsb,isb,tsa,isa);
//      if (tsb != TopAbs_FACE) continue;
//      const TopoDS_Shape& f = BDS.Shape(isb); Standard_Integer ifa = BDS.Shape(f); // DEB
//      Standard_Boolean isbound = fsdmFancSE.Contains(f);
//      if (isbound) {hasfsdmFanc = Standard_True; break;}
//    }
//    if (!hasfsdmFanc) 
//      {newLI.Append(loi); continue;}    
//    TopOpeBRepDS_ListOfInterference LIface; Standard_Integer nfound = FUN_selectSIinterference(loi,TopOpeBRepDS_FACE,LIface);

    // - cto 900 D1 - : we need interference I''=(T(face),G,face), face !sdmFancSE
    //                  to compute spOUT(e9)

    TopOpeBRepDS_ListOfInterference LIface; 
    for (; it.More(); it.Next()){
      const Handle(TopOpeBRepDS_Interference)& I = it.Value();
      TopOpeBRepDS_Kind GT,ST; Standard_Integer G,S; FDS_data(I,GT,G,ST,S);
      TopAbs_ShapeEnum tsb,tsa; Standard_Integer isb,isa; FDS_Tdata(I,tsb,isb,tsa,isa);

      if (tsb != TopAbs_FACE)       {newLI.Append(I); continue;}
      if (ST  != TopOpeBRepDS_FACE) {newLI.Append(I); continue;}

      const TopoDS_Shape& f = BDS.Shape(isb);
      Standard_Boolean isbound = fsdmFancSE.Contains(f);
      if (isbound) LIface.Append(I);
      else         newLI.Append(I);
    }
//    newLI.Append(loi);
  } // tki

  LI.Clear();
  LI.Append(newLI);
}
