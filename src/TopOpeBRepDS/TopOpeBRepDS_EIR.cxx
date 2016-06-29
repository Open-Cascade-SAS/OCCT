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


#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopOpeBRepDS_connex.hxx>
#include <TopOpeBRepDS_CurvePointInterference.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepDS_Dumper.hxx>
#include <TopOpeBRepDS_Edge3dInterferenceTool.hxx>
#include <TopOpeBRepDS_EdgeInterferenceTool.hxx>
#include <TopOpeBRepDS_EdgeVertexInterference.hxx>
#include <TopOpeBRepDS_EIR.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>
#include <TopOpeBRepDS_repvg.hxx>
#include <TopOpeBRepDS_TKI.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#define M_FORWARD(O)  (O == TopAbs_FORWARD)
#define M_REVERSED(O) (O == TopAbs_REVERSED)
#define M_INTERNAL(O) (O == TopAbs_INTERNAL)
#define M_EXTERNAL(O) (O == TopAbs_EXTERNAL)

// modified by NIZHNY-MKK  Mon Apr  2 15:34:28 2001.BEGIN
static Standard_Boolean CheckInterferenceIsValid(const Handle(TopOpeBRepDS_Interference)& I, 
						 const TopoDS_Edge&                       theEdge, 
						 const TopoDS_Edge&                       theSupportEdge, 
						 const TopoDS_Vertex&                     theVertex);
// modified by NIZHNY-MKK  Mon Apr  2 15:34:32 2001.END

//------------------------------------------------------
static void FDS_reduceONFACEinterferences(TopOpeBRepDS_ListOfInterference& LI,
                                          const TopOpeBRepDS_DataStructure& /*BDS*/,
                                          const Standard_Integer)
//------------------------------------------------------
{

  TopOpeBRepDS_ListIteratorOfListOfInterference it1;  // set hasONFACE = True if LI contains interfs with (ON,FACE) transition(s).
  Standard_Boolean hasONFACE = Standard_False;
  for (it1.Initialize(LI); it1.More(); it1.Next() ) {
    Handle(TopOpeBRepDS_Interference)& I1 = it1.Value();
    TopOpeBRepDS_Kind GT1,ST1; Standard_Integer G1,S1; FDS_data(I1,GT1,G1,ST1,S1);
    if ( GT1 == TopOpeBRepDS_POINT ) {
      hasONFACE = FUN_hasStateShape(I1->Transition(),TopAbs_ON,TopAbs_FACE);
      if ( hasONFACE ) break;
    }
  }

  if ( hasONFACE ) {
    // LI has (ON,FACE) : remove all other interf (POINT,(not(ON,FACE)))
    it1.Initialize(LI);
    while( it1.More() ) {
      Handle(TopOpeBRepDS_Interference)& I1 = it1.Value();
      TopOpeBRepDS_Kind GT1,ST1; Standard_Integer G1,S1; FDS_data(I1,GT1,G1,ST1,S1);
      if ( GT1 == TopOpeBRepDS_POINT ) {
	hasONFACE = FUN_hasStateShape(I1->Transition(),TopAbs_ON,TopAbs_FACE);
	if ( ! hasONFACE ) {
	  LI.Remove(it1);
	}
	else it1.Next();
      }
      else it1.Next();
    }
  }
}

//------------------------------------------------------
static void FUN_ReducerEdge3d(const Standard_Integer SIX, TopOpeBRepDS_DataStructure& BDS,
			      TopOpeBRepDS_ListOfInterference& LI,TopOpeBRepDS_ListOfInterference& reducedLI)	
//------------------------------------------------------
// <LI> = { I3d = (TonFACE, G=POINT/VERTEX, S=EDGE) }
// {I3d} --reducing processing-> I3d' = (TonFACE, G=POINT/VERTEX, S=FACE) 
// <LI> -> <reducedLI> + <LI>
{
  reducedLI.Clear();

  Standard_Integer n3d = LI.Extent(); 
  if (n3d <= 1) return;
  const TopoDS_Edge& E = TopoDS::Edge(BDS.Shape(SIX));
  Standard_Integer rankE  = BDS.AncestorRank(E);
  TopoDS_Shape OOv; Standard_Integer Gsta = 0;
  
  TopOpeBRepDS_ListIteratorOfListOfInterference it1;
  it1.Initialize(LI);

  // <-- jyl 231198 : cts21797
  Standard_Integer nLI = LI.Extent();
  if (nLI >= 1) {
    while (it1.More()) {
      const Handle(TopOpeBRepDS_Interference)& I1 = it1.Value();
      TopOpeBRepDS_Kind GT1,ST1; Standard_Integer G1,S1; FDS_data(I1,GT1,G1,ST1,S1);
      TopAbs_ShapeEnum SB1,SA1; Standard_Integer IB1,IA1; FDS_Tdata(I1,SB1,IB1,SA1,IA1);
      Standard_Boolean torem = Standard_False;
      if (ST1 == TopOpeBRepDS_EDGE) {
	const TopoDS_Edge& EE = TopoDS::Edge(BDS.Shape(S1));
	const TopoDS_Face& FF = TopoDS::Face(BDS.Shape(IB1));
	TopAbs_Orientation o; Standard_Boolean ok = FUN_tool_orientEinFFORWARD(EE,FF,o);
	if (ok && (o == TopAbs_EXTERNAL)) torem = Standard_True;
      }
      if (torem) LI.Remove(it1);
      else it1.Next();
    }
    nLI = LI.Extent();
    if (nLI <= 1) return;
  }
  // --> jyl 231198 : cts21797

  it1.Initialize(LI); 
  while (it1.More()){

    Standard_Boolean isComplex = Standard_False;
    TopOpeBRepDS_Edge3dInterferenceTool EFITool;
    const Handle(TopOpeBRepDS_Interference)& I1 = it1.Value();
    TopOpeBRepDS_Kind GT1,ST1; Standard_Integer G1,S1; FDS_data(I1,GT1,G1,ST1,S1);
    TopAbs_ShapeEnum SB1,SA1; Standard_Integer IB1,IA1; FDS_Tdata(I1,SB1,IB1,SA1,IA1);

    // modified by NIZHNY-MKK  Mon Apr  2 15:35:58 2001.BEGIN
    TopoDS_Vertex aVertex;

    if((GT1 == TopOpeBRepDS_VERTEX) && G1!=0) {
      aVertex = TopoDS::Vertex(BDS.Shape(G1));
    }

    if(!CheckInterferenceIsValid(I1, E, TopoDS::Edge(BDS.Shape(S1)), aVertex)) {
      LI.Remove(it1);
      continue;
    }
    // modified by NIZHNY-MKK  Mon Apr  2 15:36:02 2001.END

    TopOpeBRepDS_ListIteratorOfListOfInterference it2(it1);
    if (it2.More()) it2.Next();
    else return;

    while ( it2.More()){
      const Handle(TopOpeBRepDS_Interference)& I2 = it2.Value();
      TopOpeBRepDS_Kind GT2,ST2; Standard_Integer G2,S2; FDS_data(I2,GT2,G2,ST2,S2);
      TopAbs_ShapeEnum SB2,SA2; Standard_Integer IB2,IA2; FDS_Tdata(I2,SB2,IB2,SA2,IA2);

      // modified by NIZHNY-MKK  Mon Apr  2 15:36:42 2001.BEGIN
      aVertex.Nullify();

      if((GT2 == TopOpeBRepDS_VERTEX) && G2!=0) {
	aVertex = TopoDS::Vertex(BDS.Shape(G2));
      }
      
      if(!CheckInterferenceIsValid(I2, E, TopoDS::Edge(BDS.Shape(S2)), aVertex)) {
	LI.Remove(it2);
	continue;
      }
      // modified by NIZHNY-MKK  Mon Apr  2 15:36:45 2001.END
	
      Standard_Boolean sameG = (GT2 == GT1) && (G2 == G1);
      if (!sameG) break;

      // <Gsta>, <OOv>
      if (GT1 == TopOpeBRepDS_VERTEX) {
	TopoDS_Vertex vG1 = TopoDS::Vertex(BDS.Shape(G1));
	Standard_Integer rankvG1 = BDS.AncestorRank(vG1);
	Standard_Integer sdG1; Standard_Boolean G1hsd = FUN_ds_getVsdm(BDS,G1,sdG1);	
	if (rankvG1 != rankE) { // vG1 not on E
	  OOv = vG1;
	  Gsta = G1hsd? 3: 2;
	}
	else { // vG on E
	  if (G1hsd) OOv = BDS.Shape(sdG1); 
	  Gsta = G1hsd? 3: 1;
	}
      }      

      const TopoDS_Face& F1 = TopoDS::Face(BDS.Shape(IB1));
      const TopoDS_Face& F2 = TopoDS::Face(BDS.Shape(IB2)); // F2 != F1

      Standard_Boolean sameS = (ST2 == ST1) && (S2 == S1);
      if (!sameS) {
	TopoDS_Shape Eshared; Standard_Boolean foundsh = FUN_tool_Eshared(OOv,F1,F2,Eshared);
	if (!foundsh) return;

	// modified by NIZHNY-MKK  Mon Apr  2 15:37:12 2001.BEGIN
	if(!BDS.HasShape(Eshared)) {
	  return;
	}
	// modified by NIZHNY-MKK  Mon Apr  2 15:37:15 2001.END

	S1 = S2 = BDS.Shape(Eshared);
      }

      const TopoDS_Edge& E1 = TopoDS::Edge(BDS.Shape(S1)); 
      const TopoDS_Edge& E2 = TopoDS::Edge(BDS.Shape(S2)); // E2 == E1

      Standard_Boolean sdm = FUN_ds_sdm(BDS,E,E1);
      if (sdm) {

	it2.Next(); continue;
      }

      Standard_Boolean init = !isComplex;
      Standard_Boolean isvertex = (GT1 == TopOpeBRepDS_VERTEX); 
      init = init || isvertex; // !!!KK a revoir!!!!

      if (init) {

	// xpu : 04-03-98 : !!!KK a revoir!!!!
	if (isComplex) {
	  Handle(TopOpeBRepDS_Interference) IBID = new TopOpeBRepDS_Interference();
	  EFITool.Transition(IBID);
	  I1->ChangeTransition().Set(IBID->Transition().Orientation(TopAbs_IN));
	}
	// !!!KK a revoir!!!!

	if (!isComplex) EFITool.InitPointVertex(Gsta,OOv);
	isComplex = Standard_True;
	EFITool.Init(E,E1,F1,I1); 
	EFITool.Add(E,E1,F1,I1);
      } // !isComplex
      
      EFITool.Add(E,E2,F2,I2);
      LI.Remove(it2);
      
    } // it2

    if (isComplex) {
      Handle(TopOpeBRepDS_Interference) newI;
      Handle(TopOpeBRepDS_Interference) IIBID = new TopOpeBRepDS_Interference();
      EFITool.Transition(IIBID); 
      TopOpeBRepDS_Transition T = IIBID->Transition(); T.Index(IB1);

      Standard_Boolean isevi = I1->IsKind(STANDARD_TYPE(TopOpeBRepDS_EdgeVertexInterference));
      Standard_Boolean iscpi = I1->IsKind(STANDARD_TYPE(TopOpeBRepDS_CurvePointInterference));
      if (isevi) {
	Handle(TopOpeBRepDS_EdgeVertexInterference) EVI (Handle(TopOpeBRepDS_EdgeVertexInterference)::DownCast(I1));
	newI = new TopOpeBRepDS_EdgeVertexInterference(T,TopOpeBRepDS_FACE,IB1,G1,EVI->GBound(),
			  TopOpeBRepDS_UNSHGEOMETRY,EVI->Parameter());	  
      }
      if (iscpi) {
	Handle(TopOpeBRepDS_CurvePointInterference) CPI (Handle(TopOpeBRepDS_CurvePointInterference)::DownCast(I1));
	newI = new TopOpeBRepDS_CurvePointInterference(T,TopOpeBRepDS_FACE,IB1,TopOpeBRepDS_POINT,G1,CPI->Parameter());		
      }

      if (!newI.IsNull()) {
	reducedLI.Append(newI);
	LI.Remove(it1);
      }
    }
    else 
      it1.Next();
  } // it1
}

//------------------------------------------------------
static void FUN_ReducerEdge(const Standard_Integer SIX,const TopOpeBRepDS_DataStructure& BDS,
			    TopOpeBRepDS_ListOfInterference& LI,TopOpeBRepDS_ListOfInterference& reducedLI)
//------------------------------------------------------
{
  FDS_repvg(BDS,SIX,TopOpeBRepDS_VERTEX,LI,reducedLI);
  FDS_reduceONFACEinterferences(LI,BDS,SIX);
  FDS_repvg(BDS,SIX,TopOpeBRepDS_POINT,LI,reducedLI);
}

//------------------------------------------------------
static void FUN_ReducerSDEdge(const Standard_Integer SIX,const TopOpeBRepDS_DataStructure& BDS,
			      TopOpeBRepDS_ListOfInterference& LI,TopOpeBRepDS_ListOfInterference& reducedLI)
//------------------------------------------------------
{
  reducedLI.Clear();
  Standard_Integer nI = LI.Extent();

  if (nI <= 1) return;

  TopOpeBRepDS_ListOfInterference newLI;
  const TopoDS_Shape& EIX = BDS.Shape(SIX);
  TopOpeBRepDS_TKI tki; tki.FillOnGeometry(LI);
  for (tki.Init(); tki.More(); tki.Next()) {
    TopOpeBRepDS_Kind K; Standard_Integer G; tki.Value(K,G);
    TopOpeBRepDS_ListOfInterference& loi = tki.ChangeValue(K,G); TopOpeBRepDS_ListOfInterference Rloi;
    
    TopOpeBRepDS_ListIteratorOfListOfInterference it1(loi);
    while (it1.More()) {
      const Handle(TopOpeBRepDS_Interference)& I1 = it1.Value(); const TopOpeBRepDS_Transition& T1 = I1->Transition();
      TopAbs_Orientation O1 = T1.Orientation(TopAbs_IN); Standard_Integer IB1 = T1.Index();
      Standard_Boolean cond1 = FUN_ds_sdm(BDS,EIX,BDS.Shape(IB1));  
      if (!cond1) {newLI.Append(I1); it1.Next(); continue;}  
 
      Standard_Boolean complex1d = Standard_False;
      TopOpeBRepDS_ListIteratorOfListOfInterference it2(it1);
      if (it2.More()) it2.Next();
      else break;

      TopOpeBRepDS_Transition T(TopAbs_IN,TopAbs_IN,TopAbs_EDGE,TopAbs_EDGE);
      while (it2.More()){
	const Handle(TopOpeBRepDS_Interference)& I2 = it2.Value(); const TopOpeBRepDS_Transition& T2 = I2->Transition();
	TopAbs_Orientation O2 = T2.Orientation(TopAbs_IN); Standard_Integer IB2 = T2.Index();
	Standard_Boolean cond2 = FUN_ds_sdm(BDS,EIX,BDS.Shape(IB2));  
	if (!cond2) {newLI.Append(I2); it2.Next(); continue;} 

	complex1d =              (M_FORWARD(O1) && M_REVERSED(O2));
	complex1d = complex1d || (M_FORWARD(O2) && M_REVERSED(O1)); 
	if (!complex1d) {newLI.Append(I2); it2.Next(); continue;}

	if (complex1d) {
	  Standard_Integer IB = (M_REVERSED(O1)) ? IB1 : IB2; T.IndexBefore(IB); 
	  Standard_Integer IA = (M_REVERSED(O1)) ? IB2 : IB1; T.IndexAfter(IA);	
	}
	loi.Remove(it2);
	break; // no more than 2 interferences on sdmTRASHA at same G
      } // it2

      if (complex1d) {
	Handle(TopOpeBRepDS_EdgeVertexInterference) EVI (Handle(TopOpeBRepDS_EdgeVertexInterference)::DownCast(I1));
	TopOpeBRepDS_Config cEIX=BDS.SameDomainOri(SIX), c1=BDS.SameDomainOri(IB1); 
	TopOpeBRepDS_Config Conf = (cEIX == c1) ? TopOpeBRepDS_SAMEORIENTED : TopOpeBRepDS_DIFFORIENTED;
	Handle(TopOpeBRepDS_Interference) newI = new TopOpeBRepDS_EdgeVertexInterference(T,TopOpeBRepDS_EDGE,IB1,G,EVI->GBound(),Conf,EVI->Parameter());	
	reducedLI.Append(newI);
	it1.Next();
      }
      else {
	newLI.Append(I1);
	loi.Remove(it1);
      }
    } // it1    
  } // tki
  
  LI.Clear(); LI.Append(newLI);
}

//------------------------------------------------------
static void FUN_reclSE2(const Standard_Integer SIX,const TopOpeBRepDS_DataStructure& BDS,
			TopOpeBRepDS_ListOfInterference& LI,TopOpeBRepDS_ListOfInterference& reducedLI)
//------------------------------------------------------
// reducing LI = {I = (T(edge),G0,edge)}
//  edge <SIX> is same domain with edge <SE>
//  {I1 = (OU/IN(SE),VG,SE) 
//  I2 = (IN/OU(SE),VG,SE))} -> Ir = (IN/IN(SE),VG,SE)
{
  reducedLI.Clear();

  const TopoDS_Edge& E = TopoDS::Edge(BDS.Shape(SIX));

    TopOpeBRepDS_ListIteratorOfListOfInterference it1(LI);
    while (it1.More()) {
      const Handle(TopOpeBRepDS_Interference)& I1 = it1.Value();
      TopOpeBRepDS_Kind GT1,ST1; Standard_Integer G1,S1; FDS_data(I1,GT1,G1,ST1,S1);

      const TopOpeBRepDS_Transition& T1 = I1->Transition(); TopAbs_Orientation O1 = T1.Orientation(TopAbs_IN);
      if (M_INTERNAL(O1) || M_EXTERNAL(O1)) {it1.Next(); continue;}
      TopAbs_Orientation cO1 = TopAbs::Complement(O1);

      const TopoDS_Vertex& v1 = TopoDS::Vertex(BDS.Shape(G1));      
      const TopoDS_Edge& E1 = TopoDS::Edge(BDS.Shape(S1));
      TopoDS_Vertex vclo1; Standard_Boolean iscE1 = TopOpeBRepTool_TOOL::ClosedE(E1,vclo1);
      if (!iscE1)            {it1.Next(); continue;}
      if (!vclo1.IsSame(v1)) {it1.Next(); continue;}

      Standard_Boolean sdm = FUN_ds_sdm(BDS,E,E1);
      if (!sdm) {it1.Next(); continue;}

      Standard_Boolean hascO = Standard_False;
      TopOpeBRepDS_ListIteratorOfListOfInterference it2(it1); 
      if (it2.More()) it2.Next(); 
      else {it1.Next(); continue;}
      while ( it2.More()){ 
	const Handle(TopOpeBRepDS_Interference)& I2 = it2.Value();
	const TopOpeBRepDS_Transition& T2 = I2->Transition(); TopAbs_Orientation O2 = T2.Orientation(TopAbs_IN);
	TopOpeBRepDS_Kind GT2,ST2; Standard_Integer G2,S2; FDS_data(I2,GT2,G2,ST2,S2);
	
	if (S1 != S2)  {it2.Next(); continue;}
	if (O2 != cO1) {it2.Next(); continue;}
	
	LI.Remove(it2);
	hascO = Standard_True; break;
      } //it2
      
      if (hascO) {
	I1->ChangeTransition().Set(TopAbs_INTERNAL);
	reducedLI.Append(I1); LI.Remove(it1);
      }
      else it1.Next();
    } //it1
} // FUN_reclSE2

//------------------------------------------------------
Standard_EXPORT void FUN_reclSE(const Standard_Integer EIX,const TopOpeBRepDS_DataStructure& BDS,
				TopOpeBRepDS_ListOfInterference& LOI,TopOpeBRepDS_ListOfInterference& RLOI)
//------------------------------------------------------
{
  TopOpeBRepDS_TKI tki; tki.FillOnGeometry(LOI);

  LOI.Clear();
  for (tki.Init(); tki.More(); tki.Next()) {
    TopOpeBRepDS_Kind K; Standard_Integer G; tki.Value(K,G);
    if (K != TopOpeBRepDS_VERTEX) continue; 

    TopOpeBRepDS_ListOfInterference& loi = tki.ChangeValue(K,G); TopOpeBRepDS_ListOfInterference Rloi;
    Standard_Integer nloi = loi.Extent();
    if      (nloi == 0) continue;
    else if (nloi == 1) LOI.Append(loi);
    else {
      FUN_reclSE2(EIX,BDS,loi,Rloi);  
      LOI.Append(loi); RLOI.Append(Rloi);
    }
  }
} // FUN_reclSE

//------------------------------------------------------
static void FUN_unkeepEVIonGb1(const TopOpeBRepDS_DataStructure& BDS,const Standard_Integer EIX,TopOpeBRepDS_ListOfInterference& LI)
//------------------------------------------------------
// LI = {I attached to <E> = (T,G,S)}, unkeep I = EVI with G = vertex of <E>
{

  const TopoDS_Edge& E = TopoDS::Edge(BDS.Shape(EIX));
  
  TopOpeBRepDS_ListIteratorOfListOfInterference it(LI);
  while (it.More()) {
    const Handle(TopOpeBRepDS_Interference)& I = it.Value();
    Standard_Boolean evi = I->IsKind(STANDARD_TYPE(TopOpeBRepDS_EdgeVertexInterference));
    if (!evi) {it.Next(); continue;}
    TopOpeBRepDS_Kind GT,ST; Standard_Integer G,S; FDS_data(I,GT,G,ST,S);
    if (GT != TopOpeBRepDS_VERTEX){it.Next(); continue;}

    const TopoDS_Vertex& V = TopoDS::Vertex(BDS.Shape(G));
    Standard_Integer o = FUN_tool_orientVinE(V,E);
    if (o == 0){it.Next(); continue;}

    LI.Remove(it);
  }
}
static void FUN_keepl3dF(const Standard_Integer, const Handle(TopOpeBRepDS_HDataStructure)&
			 ,const TopOpeBRepDS_ListOfInterference& l3dF
			 ,const TopOpeBRepDS_ListOfInterference& LR3dFE,
			 TopOpeBRepDS_ListOfInterference& l3dFkeep)
//purpose : soit I de l3dF, on cherche IR interf dans LR3dFE de meme geometrie
//          si on n'en trouve pas, l3dFkeep += I
{
  TopOpeBRepDS_TKI tki;
  tki.FillOnGeometry(l3dF);

  TopOpeBRepDS_TKI tkiR;
  tkiR.FillOnGeometry(LR3dFE); 
  
  for (tki.Init(); tki.More(); tki.Next()) {
    TopOpeBRepDS_Kind K; Standard_Integer G; tki.Value(K,G);
    TopOpeBRepDS_ListOfInterference& loi = tki.ChangeValue(K,G);
    tkiR.Init();
    Standard_Boolean isbound = tkiR.IsBound(K,G);
    if (!isbound) {l3dFkeep.Append(loi);}

  }  
} // FUN_keepl3dF

static void FUN_reducepure2dI0(TopOpeBRepDS_ListOfInterference& LI, TopOpeBRepDS_ListOfInterference& RLI)
// xpu210798 : CTS21216 (e7,G=P3,S=E9,FTRASHA=f8,f10)
//  LI attached to section edge
//  2dI1=(REVERSED(ftrasha1),G,ES), 2dI2=(FORWARD(ftrasha2),G,ES)
//  for the compute of splitON(EIX), reduce 2dI1+2dI2 -> 2dR=(IN(ftrasha1,ftrasha2)
// LI-> RLI + LI
{
  const Handle(TopOpeBRepDS_Interference)& I1 = LI.First();
  TopAbs_Orientation O1 = I1->Transition().Orientation(TopAbs_IN);
  TopOpeBRepDS_Kind GT1,ST1; Standard_Integer G1,S1; TopAbs_ShapeEnum tsb1,tsa1; Standard_Integer isb1,isa1; 
  FDS_Idata(I1,tsb1,isb1,tsa1,isa1,GT1,G1,ST1,S1);
  
  const Handle(TopOpeBRepDS_Interference)& I2 = LI.Last();
  TopAbs_Orientation O2 = I2->Transition().Orientation(TopAbs_IN);
  TopOpeBRepDS_Kind GT2,ST2; Standard_Integer G2,S2; TopAbs_ShapeEnum tsb2,tsa2; Standard_Integer isb2,isa2; 
  FDS_Idata(I2,tsb2,isb2,tsa2,isa2,GT2,G2,ST2,S2);
  
  if (isb1 == isb2) return; //xpu170898 FOR+REV ->INT/EXT (cto009B1(es6,p1,f9))

  // recall : G1==G2 && ST1==ST2==EDGE
  Standard_Boolean ok = (G1==G2);
  ok = ok && (tsb1 == TopAbs_FACE) && (tsb1==tsb2) && (isb1==isa1) && (isb2==isa2);
  if (!ok) return;

  Standard_Boolean int12 = M_REVERSED(O1) && M_FORWARD(O2);
  Standard_Boolean int21 = M_REVERSED(O2) && M_FORWARD(O1);
  ok = int12 || int21;
  if (!ok) return;

  TopOpeBRepDS_Transition newT(TopAbs_INTERNAL);
  Standard_Integer bef = int12? isb1 : isb2;
  Standard_Integer aft = int21? isb1 : isb2; 
  newT.IndexBefore(bef); newT.IndexAfter(aft);
  I1->ChangeTransition() = newT;
  RLI.Append(I1);
  LI.Clear();
} // FUN_reducepure2dI0

static void FUN_reducepure2dI(TopOpeBRepDS_ListOfInterference& LI, TopOpeBRepDS_ListOfInterference& RLI)
{  
  TopOpeBRepDS_TKI tki; tki.FillOnGeometry(LI);
  TopOpeBRepDS_ListOfInterference newLI;
  for (tki.Init(); tki.More(); tki.Next()) {
    TopOpeBRepDS_Kind K; Standard_Integer G; tki.Value(K,G);
    TopOpeBRepDS_ListOfInterference& loi = tki.ChangeValue(K,G); TopOpeBRepDS_ListOfInterference Rloi;
    Standard_Integer nloi = loi.Extent();
    Standard_Boolean ok = (nloi == 2) && (K == TopOpeBRepDS_POINT);
    if (ok) ::FUN_reducepure2dI0(loi,Rloi);
    RLI.Append(Rloi);
    newLI.Append(loi);
  }
  LI.Clear(); LI.Append(newLI);
} // FUN_reducepure2dI

//=======================================================================
//function : TopOpeBRepDS_EIR
//purpose  : 
//=======================================================================
TopOpeBRepDS_EIR::TopOpeBRepDS_EIR
(const Handle(TopOpeBRepDS_HDataStructure)& HDS) : myHDS(HDS)
{}

//=======================================================================
//function : ProcessEdgeInterferences
//purpose  : 
//=======================================================================
void TopOpeBRepDS_EIR::ProcessEdgeInterferences()
{
  TopOpeBRepDS_DataStructure& BDS = myHDS->ChangeDS();
  Standard_Integer i,nshape = BDS.NbShapes();
  for (i = 1; i <= nshape; i++) {
    const TopoDS_Shape& S = BDS.Shape(i);
    if(S.IsNull()) continue;
    if ( S.ShapeType() == TopAbs_EDGE ) {
      ProcessEdgeInterferences(i);
    }
  }
}
static void FUN_ProcessEdgeInterferences(const Standard_Integer EIX
                                         , const TopOpeBRepDS_Kind
                                         ,const Standard_Integer
                                         , const Handle(TopOpeBRepDS_HDataStructure)& HDS, 
					 TopOpeBRepDS_ListOfInterference& LI)
{
  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  const TopoDS_Shape& E = BDS.Shape(EIX);

  // LI -> (lF + lFE) + lE + [LI]
  // lF  = {interference on edge <EIX> :  (T(face),G=POINT/VERTEX,S)
  // lFE = {interference on edge <EIX> :  (T(face),G=POINT/VERTEX,S=EDGE)
  // lE  = {interference on edge <EIX> :  (T(edge),G=POINT/VERTEX,S)
  TopOpeBRepDS_ListOfInterference lF; FUN_selectTRASHAinterference(LI,TopAbs_FACE,lF);
  TopOpeBRepDS_ListOfInterference lFE; FUN_selectSKinterference(lF,TopOpeBRepDS_EDGE,lFE);
  TopOpeBRepDS_ListOfInterference lE; FUN_selectTRASHAinterference(LI,TopAbs_EDGE,lE);

  // xpu210798 : CTS21216 (e7,G=P3,S=E9,FTRASHA=f8,f10)
  //   EIX is section edge
  //   2dI1=(REVERSED(ftrasha1),G,ES), 2dI2=(FORWARD(ftrasha2),G,ES)
  //   for the compute of splitON(EIX), reduce 2dI1+2dI2 -> 2dR=(IN(ftrasha1,ftrasha2)
  Standard_Boolean isse = BDS.IsSectionEdge(TopoDS::Edge(E));
  if (isse) {
    TopOpeBRepDS_ListOfInterference lI2dFE,lRI2dFE;
    // lF  -> lF
    // lFE -> lI2dFE + [lFE] / lI2dFE={FEI=(T(FTRASHA),G,SE) : no FFI=(T(FTRASHA),G,FTRASHA)}
    FUN_selectpure2dI(lF,lFE,lI2dFE);
    ::FUN_reducepure2dI(lI2dFE,lRI2dFE); // lI2dFE -> lRI2dFE + lI2dFE
    lFE.Append(lI2dFE); lFE.Append(lRI2dFE);
  }

  // I -> 3dI +2dI + 1dI:
  // -------------------
  // lFE -> l3dFE [+l3dFEresi] +l2dFE [+lFE (+lFEresi)]
  //    : l3dFE={I3d } to reduce; for I3dFE=(T(F),G,SE) there is I3dF=(T(F),G,F)
  //      l2dFE={I2d} to reduce
  //      lFEresi to remove 
  // lF  -> l3dF [+lF] : l3dF to remove

  // lE  -> l1dE +l1dEsd [+lE}
  //    : l1dE={I1d=(T(ETRA),G,ETRA)/same ETRA} to reduce
  //      l2dEsd={I1dsd=(T(ETRA),G,ETRA)/ E sdm ETRA}

  TopOpeBRepDS_ListOfInterference lFEresi,l3dFE,l3dF,l3dFEresi,l2dFE;
  FUN_select3dinterference(EIX,BDS,lF,l3dF,
			   lFE,lFEresi,l3dFE,l3dFEresi,
			   l2dFE);
  TopOpeBRepDS_ListOfInterference l1dE; FUN_select2dI(EIX,BDS,TopAbs_EDGE,lE,l1dE);
  TopOpeBRepDS_ListOfInterference l1dEsd; FUN_select1dI(EIX,BDS,lE,l1dEsd);

  // reducer3d :
  // ----------
  // l3dFE -> lR3dFE [+l3dFE (non reduced 3dI)]
  TopOpeBRepDS_ListOfInterference lR3dFE; FUN_ReducerEdge3d(EIX,BDS,l3dFE,lR3dFE);

//  FUN_unkeepEVIonGb1(BDS,EIX,l1dE);  // filter : 
//  FUN_unkeepEVIonGb1(BDS,EIX,l2dFE); // filter : 

  // no reduction on G : we keep l3dF(G)
  TopOpeBRepDS_ListOfInterference l3dFkeep; FUN_keepl3dF(EIX,HDS,l3dF,lR3dFE,l3dFkeep);
  lF.Append(l3dFkeep);

  // reducer2d :
  // ----------
  // l2dFE -> LR2dFE + l2dFE (non reduced 2dI)
  TopOpeBRepDS_ListOfInterference LR2dFE; FUN_ReducerEdge(EIX,BDS,l2dFE,LR2dFE);

  // reducer1d :
  // ----------
  // xpu210498 : reduce interferences I1(T1(esd1),VG,esd1), I2(T2(esd2),VG,esd2)
  // if EIX sdm {esd1,esd2}
  // - t2_2 e32 has I1(in/ou(e32),v30,e20) && I2(ou/in(e20),v30,e20) -
  TopOpeBRepDS_ListOfInterference lR1dEsd; FUN_ReducerSDEdge(EIX,BDS,l1dEsd,lR1dEsd);

  // xpu190298 : edge <EIX> same domain with closed support edge <SE>
  // at same G = vertex <VG>  = closing vertex of <SE>
  //  (I1 = (OU/IN(SE),VG,SE) && I2 = (IN/OU(SE),VG,SE)) -> Ir = (IN/IN(SE),VG,SE)
  TopOpeBRepDS_ListOfInterference lR1dclosedSE; FUN_reclSE(EIX,BDS,l1dE,lR1dclosedSE);  

  // l1dE  -> LR1dE + l1dE (non reduced 2dI)
  TopOpeBRepDS_ListOfInterference LR1dE;  FUN_ReducerEdge(EIX,BDS,l1dE,LR1dE);

  //  attached to edge <EIX>,
  //  at same G : I  =(T,  G,S)   gives less information than
  //              Ir =(Tr,G,Sr) -reduced interference- with valid Tr 
  //  -> unkeep I.
  // using reduced I : lR3dFE, LR2dFE, LR1dE
  TopOpeBRepDS_ListOfInterference LRI; 
  LRI.Append(lR1dEsd); LRI.Append(LR1dE); LRI.Append(lR1dclosedSE);
  LRI.Append(LR2dFE);
  LRI.Append(lR3dFE);

  lF.Append(lFE); lF.Append(l3dFE); lF.Append(l2dFE);// lF += LFE + l3dFE + l2dFE
  lE.Append(l1dE);                                   // lE += l1dE
  lE.Append(l1dEsd);                                 // lE += l1dEsd xpu210498
//  TopOpeBRepDS_ListOfInterference LItmp; LItmp.Append(lF); LItmp.Append(lE);

  // xpu : 23-01-98 : attached to edge <EIX>,
  //  at same G : I  =(T, G, S) gives less information than
  //              Ir =(Tr,G,Sr) -reduced interference- 
  //  -> I is added after Ir to help the edge builder.
  LI.Clear(); 
  LI.Append(LRI);LI.Append(lE);LI.Append(lF);
//  FUN_reorder(EIX,HDS,LRI, LItmp, LI); 
  
  // xpu260698 : cto902A5, spOU(e6)
  //             cto801G1, spON(se26) 
  if (isse) {
    FUN_unkeepEVIonGb1(BDS,EIX,LI);  // filter : 
  }
//FUN_unkeepEVIonGb1(BDS,EIX,LI);  // filter : 
} // ProcessEdgeInterferences

//=======================================================================
//function : ProcessEdgeInterferences
//purpose  : 
//=======================================================================
void TopOpeBRepDS_EIR::ProcessEdgeInterferences(const Standard_Integer EIX)
{
  TopOpeBRepDS_DataStructure& BDS = myHDS->ChangeDS();

  // E is the edge, LI is list of interferences to compact
  const TopoDS_Edge& E = TopoDS::Edge(BDS.Shape(EIX));
  Standard_Boolean isdg = BRep_Tool::Degenerated(E);
  if (isdg) return;

  TopOpeBRepDS_ListOfInterference& LI = BDS.ChangeShapeInterferences(EIX);
  TopOpeBRepDS_TKI newtki; newtki.FillOnGeometry(LI);
  TopOpeBRepDS_TKI tki; tki.FillOnGeometry(LI);
  for (tki.Init(); tki.More(); tki.Next()) {
    TopOpeBRepDS_Kind K; Standard_Integer G; tki.Value(K,G);
    const TopOpeBRepDS_ListOfInterference& loi = tki.Value(K,G);
    if (K==TopOpeBRepDS_POINT) continue;
    const TopoDS_Shape& vG = BDS.Shape(G);
    TopoDS_Shape oovG; Standard_Boolean sdm = FUN_ds_getoov(vG,BDS,oovG);
    if (!sdm) continue;
    Standard_Integer OOG=BDS.Shape(oovG);
    if (OOG == 0) continue;//NYIRaise

    Standard_Boolean isb = newtki.IsBound(K,OOG);

    // xpu201098 : cto904F6, e10,v6
    Standard_Boolean isbound = Handle(TopOpeBRepDS_EdgeVertexInterference)::DownCast(loi.First())->GBound();
    if (isbound) { // replacing vG with oovG
      TopOpeBRepDS_ListIteratorOfListOfInterference it(loi); TopOpeBRepDS_ListOfInterference newloi;
      for (; it.More(); it.Next()){
	const Handle(TopOpeBRepDS_Interference)& I = it.Value();
	TopOpeBRepDS_Kind GT,ST;
        Standard_Integer G1,S;
        FDS_data(I,GT,G1,ST,S);
	Standard_Real par = FDS_Parameter(I);
	Handle(TopOpeBRepDS_Interference) newI = MakeEPVInterference(I->Transition(),S,OOG,par,K,ST,Standard_False);
	newloi.Append(newI);
      }
      newtki.ChangeInterferences(K,G).Clear();
      if (!isb) newtki.Add(K,OOG);
      newtki.ChangeInterferences(K,OOG).Append(newloi);
      continue;
    }

    if (!isb) continue;
    TopOpeBRepDS_ListOfInterference& li = newtki.ChangeInterferences(K,OOG);
    newtki.ChangeInterferences(K,G).Append(li);
  } // tki

  TopOpeBRepDS_ListOfInterference LInew;
  for (newtki.Init(); newtki.More(); newtki.Next()) {
    TopOpeBRepDS_Kind K; Standard_Integer G; newtki.Value(K,G);
    TopOpeBRepDS_ListOfInterference& loi = newtki.ChangeValue(K,G);
    ::FUN_ProcessEdgeInterferences(EIX,K,G,myHDS,loi);
    LInew.Append(loi);
  }
  LI.Clear();
  LI.Append(LInew);

  Standard_Boolean performPNC = Standard_False; // JYL 28/09/98 : temporaire

  if (!performPNC) return;

  // suppression des I/G(I) n'est accede par aucune courbe
  // portee par une des faces cnx a EIX.
  Standard_Boolean isfafa = BDS.Isfafa();
  if (!isfafa) {

    tki.Clear();
    tki.FillOnGeometry(LI);
    LI.Clear();

    for (tki.Init(); tki.More(); tki.Next()) {
      TopOpeBRepDS_Kind K; Standard_Integer G; tki.Value(K,G);
      TopOpeBRepDS_ListOfInterference& loi = tki.ChangeValue(K,G);
      if (K != TopOpeBRepDS_POINT) {
	LI.Append(loi);
	continue;
      }

      const TopTools_ListOfShape& lfx = FDSCNX_EdgeConnexitySameShape(E,myHDS);

      // nlfx < 2 => 0 ou 1 face accede E => pas d'autre fcx pouvant generer une courbe 3d
      TopTools_ListIteratorOfListOfShape itlfx(lfx);
      Standard_Boolean curvefound = Standard_False;
      for (; itlfx.More();itlfx.Next()) {
	const TopoDS_Face& fx = TopoDS::Face(itlfx.Value());
//                  BDS.Shape(fx);
	const TopOpeBRepDS_ListOfInterference& lifx = BDS.ShapeInterferences(fx); TopOpeBRepDS_ListIteratorOfListOfInterference itlifx(lifx);
	if (!itlifx.More()) continue;
	
	Handle(TopOpeBRepDS_Interference) I1;TopOpeBRepDS_Kind GT1;Standard_Integer G1;TopOpeBRepDS_Kind ST1;Standard_Integer S1;
	for (; itlifx.More(); itlifx.Next()) {
	  FDS_data(itlifx,I1,GT1,G1,ST1,S1);
	  Standard_Boolean isfci = (GT1 == TopOpeBRepDS_CURVE);
	  if (!isfci) continue;
	  
	  TopOpeBRepDS_ListOfInterference& lic = BDS.ChangeCurveInterferences(G1);
	  TopOpeBRepDS_ListIteratorOfListOfInterference itlic(lic);
	  if (!itlic.More()) continue;
	  
	  Handle(TopOpeBRepDS_Interference) I2;TopOpeBRepDS_Kind GT2;Standard_Integer G2;TopOpeBRepDS_Kind ST2;Standard_Integer S2;
	  for (; itlic.More(); itlic.Next()) {
	    FDS_data(itlic,I2,GT2,G2,ST2,S2);
	    Standard_Boolean isp = (GT2 == TopOpeBRepDS_POINT);
	    if (!isp) continue;
	    if ( G2 != G ) continue;
	    curvefound = Standard_True;
	    break;
	  } // itlic.More()

	  if (curvefound) break;
	} // itlifx.More()

	if (curvefound) break;
      } // itlfx.More()
      
      if (curvefound) {
	LI.Append(loi);
      } 
    } // tki.More()
  } // (!isfafa)
  
} // ProcessEdgeInterferences

// modified by NIZHNY-MKK  Mon Apr  2 15:34:56 2001.BEGIN
static Standard_Boolean CheckInterferenceIsValid(const Handle(TopOpeBRepDS_Interference)& I, 
						 const TopoDS_Edge&                       theEdge, 
						 const TopoDS_Edge&                       theSupportEdge, 
						 const TopoDS_Vertex&                     theVertex) {
  Standard_Real pref = 0. ;
  Standard_Boolean ok = Standard_False;
  BRepAdaptor_Curve BC(theEdge);

  Handle(TopOpeBRepDS_CurvePointInterference) CPI;
  CPI = Handle(TopOpeBRepDS_CurvePointInterference)::DownCast(I);

  if(!CPI.IsNull()) {
    pref = CPI->Parameter();
    ok = Standard_True;
  }

  if(!ok) {
    Handle(TopOpeBRepDS_EdgeVertexInterference) EVI = Handle(TopOpeBRepDS_EdgeVertexInterference)::DownCast(I);
    if(!EVI.IsNull()) {
      pref = EVI->Parameter();
      ok = Standard_True;
    }
  }

  if(!ok)
    return ok;

  gp_Pnt P3d1 = BC.Value(pref);
  Standard_Real dist, paronSupportE;      
  ok = FUN_tool_projPonE(P3d1,theSupportEdge,paronSupportE,dist);

  if(!ok)
    return ok;
  BRepAdaptor_Curve BCtmp(theSupportEdge);
  gp_Pnt P3d2 = BCtmp.Value(paronSupportE);
  Standard_Real Tolerance = (BRep_Tool::Tolerance(theEdge) > BRep_Tool::Tolerance(theSupportEdge)) ? BRep_Tool::Tolerance(theEdge) : BRep_Tool::Tolerance(theSupportEdge);
  if(!theVertex.IsNull()) {
    Tolerance = (BRep_Tool::Tolerance(theVertex) > Tolerance) ? BRep_Tool::Tolerance(theVertex) : Tolerance;
  }
  if(P3d1.Distance(P3d2) > Tolerance) {
    ok = Standard_False;
  }
  
  return ok;
}
