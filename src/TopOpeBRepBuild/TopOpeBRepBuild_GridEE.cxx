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


#include <gp_Pnt.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TCollection_AsciiString.hxx>
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
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopOpeBRepDS_SurfaceIterator.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>

#ifdef DRAW
#include <TopOpeBRepDS_DRAW.hxx>
#endif

#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_ProcessInterferencesTool.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_TOOL.hxx>
#include <TopOpeBRepDS_TKI.hxx>
#include <TopOpeBRepDS.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Precision.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopOpeBRepDS_Dumper.hxx>
#include <Standard_ProgramError.hxx>
#include <TopOpeBRepDS_EdgeVertexInterference.hxx>

#ifdef OCCT_DEBUG
Standard_EXPORT Standard_Boolean TopOpeBRepBuild_GetcontextNOSG();
#endif

#define M_FORWARD(st) (st == TopAbs_FORWARD)
#define M_REVERSED(st) (st == TopAbs_REVERSED)
#define M_INTERNAL(st) (st == TopAbs_INTERNAL)
#define M_EXTERNAL(st) (st == TopAbs_EXTERNAL)

Standard_Boolean TopOpeBRepBuild_FUN_aresamegeom(const TopoDS_Shape& S1,const TopoDS_Shape& S2);


//=======================================================================
//function : GMergeEdges
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::GMergeEdges(const TopTools_ListOfShape& LE1,const TopTools_ListOfShape& LE2,const TopOpeBRepBuild_GTopo& G1)
{
  if ( LE1.IsEmpty() ) return;
  TopAbs_State TB1,TB2; G1.StatesON(TB1,TB2);
  
  const TopoDS_Shape& E1 = LE1.First();
#ifdef OCCT_DEBUG
  Standard_Integer iE; Standard_Boolean tSPS1 = GtraceSPS(E1,iE);
  if(tSPS1){
    cout<<endl;cout<<"--- GMergeEdges "<<endl;
    GdumpEDG(E1);
    GdumpSAMDOM(LE1, (char *) "1 : ");
    GdumpSAMDOM(LE2, (char *) "2 : ");
  }
#endif
  
  myEdgeReference = TopoDS::Edge(E1);
  TopOpeBRepBuild_PaveSet PVS(E1);
  
  GFillEdgesPVS(LE1,LE2,G1,PVS);
  
  // Create a edge builder EBU
  TopoDS_Shape E1F = LE1.First(); E1F.Orientation(TopAbs_FORWARD);
  TopOpeBRepBuild_PaveClassifier VCL(E1F);
  Standard_Boolean equalpar = PVS.HasEqualParameters();
  if (equalpar) VCL.SetFirstParameter(PVS.EqualParameters());
  TopOpeBRepBuild_EdgeBuilder EDBU(PVS,VCL);
  
  // Build the new edges LEM
  TopTools_ListOfShape LEM;
  GEDBUMakeEdges(E1F,EDBU,LEM);
  
  // connect new edges as edges built TB1 on LE1 edges
  TopTools_ListIteratorOfListOfShape it1;
  for (it1.Initialize(LE1); it1.More(); it1.Next()) {
    const TopoDS_Shape& E11 = it1.Value();
    ChangeMerged(E11,TB1) = LEM;
  }
  
  // connect new edges as edges built TB2 on LE2 edges
  TopTools_ListIteratorOfListOfShape it2;
  for (it2.Initialize(LE2); it2.More(); it2.Next()) {
    const TopoDS_Shape& E2 = it2.Value();
    ChangeMerged(E2,TB2) = LEM;
  }
  
} // GMergeEdges

//=======================================================================
//function : GFillEdgesPVS
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::GFillEdgesPVS(const TopTools_ListOfShape& LE1,const TopTools_ListOfShape& LE2,const TopOpeBRepBuild_GTopo& G1,TopOpeBRepBuild_PaveSet& PVS)
{
  if ( LE1.IsEmpty() ) return;
  TopAbs_State TB1,TB2; G1.StatesON(TB1,TB2);
  
  const TopoDS_Shape& E1 = LE1.First();
  myEdgeReference = TopoDS::Edge(E1);
  
  TopTools_ListIteratorOfListOfShape it1;
  for (it1.Initialize(LE1); it1.More(); it1.Next()) {
    const TopoDS_Shape& E11 = it1.Value();
    Standard_Boolean ismerged = IsMerged(E11,TB1);
    
#ifdef OCCT_DEBUG
    Standard_Integer i1; Standard_Boolean tSPS1 = GtraceSPS(E11,i1);
    if(tSPS1){
      cout<<endl;cout<<"--- GFillEdgesPVS ";GdumpSHA(E11);
      cout<<" ismerged : "<<ismerged<<" ";TopAbs::Print(TB1,cout);cout<<endl;
    }
#endif
    
    if (!ismerged) GFillEdgePVS(E11,LE2,G1,PVS);
  }
  
  TopOpeBRepBuild_GTopo G2 = G1.CopyPermuted();
  
  TopTools_ListIteratorOfListOfShape it2;
  for (it2.Initialize(LE2); it2.More(); it2.Next() ) {
    const TopoDS_Shape& E2 = it2.Value();
    Standard_Boolean ismerged = IsMerged(E2,TB2);
    
#ifdef OCCT_DEBUG
    Standard_Integer i2; Standard_Boolean tSPS2 = GtraceSPS(E2,i2);
    if(tSPS2){
      cout<<endl;
      cout<<"--- GFillEdgesPVS ";GdumpSHA(E2);
      cout<<" ismerged : "<<ismerged<<" ";TopAbs::Print(TB2,cout);cout<<endl;
    }
#endif
    
    if (!ismerged) GFillEdgePVS(E2,LE1,G2,PVS);
  }
  
} // GFillEdgesPVS

//=======================================================================
//function : GFillEdgePVS
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::GFillEdgePVS(const TopoDS_Shape& E,
                                           const TopTools_ListOfShape& /*LE2*/,
                                           const TopOpeBRepBuild_GTopo& G,
                                           TopOpeBRepBuild_PaveSet& PVS)
{
  TopAbs_ShapeEnum t1,t2;
  G.Type(t1,t2);
  TopAbs_State TB1,TB2;
  G.StatesON(TB1,TB2);
  
  // work on a FORWARD edge EF
  TopoDS_Shape EF = E;
  EF.Orientation(TopAbs_FORWARD);
  // Add the point/vertex topology build/found on edge EF in PVS
  GFillPointTopologyPVS(EF,G,PVS);
  
} // GFillEdgePVS

// --- iterer sur EPit jusqu'a la prochaine interference dont la
// --- transition est definie sur un shape de type SHA Before et After
static Standard_Boolean FUN_MoreSHAINT(TopOpeBRepDS_PointIterator& EPit,
                                       const TopAbs_ShapeEnum SHA)
{
  Standard_Boolean more = Standard_False;
  while (EPit.More()) {
    const Handle(TopOpeBRepDS_Interference)& I = EPit.Value();
    const TopOpeBRepDS_Transition& T = I->Transition();
    TopOpeBRepDS_Kind GT,ST; Standard_Integer G,S; FDS_data(I,GT,G,ST,S);
    TopAbs_ShapeEnum SB,SA; Standard_Integer IB,IA; FDS_Tdata(I,SB,IB,SA,IA);
    
    TopAbs_ShapeEnum b = T.ShapeBefore(), a = T.ShapeAfter();
    Standard_Boolean rejet = ( (b != SHA) || (a != SHA) );
    if ( rejet ) EPit.Next();
    else {
      more = Standard_True;
      break;
    }
  }
  return more;
}

// Unused :
/*#ifdef OCCT_DEBUG
static Standard_Integer FUN_getTRASHA(const Standard_Integer geti,
			  const TopOpeBRepDS_ListOfInterference& lFOR, const Standard_Integer FOR,
			  const TopOpeBRepDS_ListOfInterference& lREV, const Standard_Integer REV,
			  const TopOpeBRepDS_ListOfInterference& lINT, const Standard_Integer INT)
{
  Standard_Integer trasha = 0;
  if (geti == 1) { // get i before
    if (REV) trasha = lREV.First()->Transition().Before();
    if (INT) trasha = lINT.First()->Transition().Before();    
  }
  if (geti == 2) { // get i after
    if (FOR) trasha = lFOR.Last()->Transition().After();
    if (INT) trasha = lINT.Last()->Transition().After();
  }
  return trasha;
}
#endif*/

#ifdef OCCT_DEBUG
void debfillp(const Standard_Integer i) {cout <<"+ + debfillp "<<i<<endl;}
void debfillpon(const Standard_Integer i) {cout <<"+ + debfillpon "<<i<<endl;}
void debfillpin(const Standard_Integer i) {cout <<"+ + debfillpin "<<i<<endl;}
void debfillpou(const Standard_Integer i) {cout <<"+ + debfillpou "<<i<<endl;}
void debfillp2(const Standard_Integer i) {cout <<"+ + debfillp2 "<<i<<endl;}
#endif

//Standard_IMPORT extern Standard_Boolean GLOBAL_faces2d;
extern Standard_Boolean GLOBAL_faces2d;

Standard_EXPORT Standard_Boolean FDS_SIisGIofIofSBAofTofI(const TopOpeBRepDS_DataStructure& BDS,const Standard_Integer SI,const Handle(TopOpeBRepDS_Interference)& I);
//Standard_IMPORT extern Standard_Boolean GLOBAL_IEtoMERGE; // xpu240498
Standard_IMPORT Standard_Boolean GLOBAL_IEtoMERGE; // xpu240498
//Standard_IMPORT extern Standard_Integer GLOBAL_issp;
extern Standard_Integer GLOBAL_issp;
//Standard_IMPORT extern Standard_Integer GLOBAL_hassd;
Standard_IMPORT Standard_Integer GLOBAL_hassd;

static Standard_Boolean FUN_isonbound(const Handle(TopOpeBRepDS_HDataStructure)& HDS,
                                      const Handle(TopOpeBRepDS_Interference)& I)
{
  Standard_Integer G = I->Geometry();
  TopOpeBRepDS_Kind KG =  I->GeometryType();
  Standard_Boolean Gb1 = (KG == TopOpeBRepDS_VERTEX); 
  if (Gb1) {
    Handle(TopOpeBRepDS_EdgeVertexInterference) EVI= Handle(TopOpeBRepDS_EdgeVertexInterference)::DownCast(I);
    Standard_Boolean vhassd = HDS->HasSameDomain(HDS->DS().Shape(G));
    Gb1 = (EVI.IsNull()) ? Standard_False : EVI->GBound();
    Gb1 = Gb1 && !vhassd;    
  }
  return Gb1;
}

#define TheIN (1)
#define TheON (2)
#define TheOUT (3)
#define HASSD2d (2)
#define HASSD3d (3)
#define FIRST (1)
#define LAST  (2)

//=======================================================================
//function : GFillPointTopologyPVS
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::GFillPointTopologyPVS(const TopoDS_Shape& E,
                                                    const TopOpeBRepBuild_GTopo& G,
                                                    TopOpeBRepBuild_PaveSet& PVS)
{
#ifdef OCCT_DEBUG
//  TopAbs_State TB1,TB2; 
//  G.StatesON(TB1,TB2);
//  TopOpeBRepDS_Config GConf1 = G.Config1();
//  TopOpeBRepDS_Config GConf2 = G.Config2();
#endif
  TopAbs_ShapeEnum t1,t2,ShapeInterf; 
  G.Type(t1,t2); 
  ShapeInterf = t1;
  const TopOpeBRepDS_DataStructure& BDS = myDataStructure->DS();
  const Standard_Integer iEDS = BDS.Shape(E);
#ifdef OCCT_DEBUG
//  Standard_Integer rkE = BDS.AncestorRank(E); 
#endif
  Standard_Boolean isSE = BDS.IsSectionEdge(TopoDS::Edge(E));
  Standard_Boolean dgE = BRep_Tool::Degenerated(TopoDS::Edge(E));
  Standard_Boolean isEd;

  isEd = BRep_Tool::Degenerated(TopoDS::Edge(E));
#ifdef OCCT_DEBUG
//  Standard_Boolean hsd = myDataStructure->HasSameDomain(E); //xpu170498
#endif
  Standard_Boolean isfafa = BDS.Isfafa(); //xpu120598

#ifdef OCCT_DEBUG
  Standard_Boolean tSPSE=GtraceSPS(iEDS);
  TCollection_AsciiString striE=TopOpeBRepDS::SPrint(TopAbs_EDGE,iEDS);
  const TopoDS_Shape& EPVS=PVS.Edge();Standard_Integer iEPVS;Standard_Boolean tSPSEPVS=GtraceSPS(EPVS,iEPVS);
  Standard_Boolean tSPS = tSPSE || tSPSEPVS;
  if(tSPS){
    cout<<endl;
    cout<<"--- GFillPointTopologyPVS : ShapeInterf ";TopAbs::Print(ShapeInterf,cout);
    cout<<",efil ";GdumpSHA(E);cout<<",eref ";GdumpSHA(myEdgeReference);
    cout<<",ffil ";GdumpSHA(myFaceToFill);cout<<",fref ";GdumpSHA(myFaceReference);
    cout<<endl;
    debfillp(iEDS);
  }
#endif

  Standard_Boolean isspin=(GLOBAL_issp==TheIN), isspou=(GLOBAL_issp==TheOUT), isspon=(GLOBAL_issp==TheON);
  if (isSE && (GLOBAL_issp == 0)) return; // splits done in process ProcessSectionEdges

#ifdef OCCT_DEBUG
//  Standard_Integer iefil = BDS.Shape(E);
//  Standard_Integer iffil = BDS.Shape(myFaceToFill);
//  Standard_Integer ieref = BDS.Shape(myEdgeReference);
//  Standard_Integer ifref = BDS.Shape(myFaceReference);
  if(tSPS) {
    if (isspon) debfillpon(iEDS);
    if (isspin) debfillpin(iEDS);
    if (isspou) debfillpou(iEDS);
  }
#endif

  // 0.
  //---
  const TopOpeBRepDS_ListOfInterference& lIE = BDS.ShapeInterferences(E);
  Standard_Boolean scanall = (isspin || isspou || isspon); // xpu161198: BUC60382 //xpu011098: CTS21180(e8on); cto900I7(e12on)

  // loiSHAINT = interferences avec les 2 proprietes 
  // - fournies par un PointIterator
  // - dont la transition est definie / shape = ShapeInterf
  TopOpeBRepDS_ListOfInterference loiSHAINT;
  if (scanall) FDS_assign(lIE,loiSHAINT);
  else { 
    TopOpeBRepDS_PointIterator EPit(lIE);
    EPit.Init(BDS.ShapeInterferences(E));
    Standard_Boolean addi = FUN_MoreSHAINT(EPit,ShapeInterf);
    while (addi) {
      const Handle(TopOpeBRepDS_Interference)& II = EPit.Value();
      loiSHAINT.Append(II);
      EPit.Next();
      addi = FUN_MoreSHAINT(EPit,ShapeInterf);
    }
  }

  TopOpeBRepDS_TKI tki;
  tki.FillOnGeometry(loiSHAINT);  // groupage par geometrie d'interference
  
  // - kp1 -
  // BUC60093 :  only 2 G : 1 point && 1 vertex  
  // deleting interfs on G = vertex sdm && closingE  
  TopoDS_Vertex vclo; Standard_Boolean closedE = TopOpeBRepTool_TOOL::ClosedE(TopoDS::Edge(E),vclo);
  Standard_Integer kp1 = 0;
  if (closedE) {
    tki.Init();
    Standard_Integer nG = 0;
    while (tki.More()) {
      nG++;
      TopOpeBRepDS_Kind Kcur;Standard_Integer Gcur;
      tki.Value(Kcur,Gcur);
      if (Kcur == TopOpeBRepDS_POINT) {tki.Next();continue;}
      const TopoDS_Shape& v = BDS.Shape(Gcur);
      TopoDS_Shape oov;
      FUN_ds_getoov(v,myDataStructure,oov);
      Standard_Boolean samev = v.IsSame(vclo), sameoov = oov.IsSame(vclo);
      if (samev || sameoov) {kp1 = Gcur;}
      tki.Next();
    }
    if (nG == 1) kp1 = 0; // we have only one interf on vGclo -> keep the interf
  } //kp1

  // - kp6 - nyiReducing
  // xpu250998    : cto900M5 (e5,p1) 
  // prequesitory : if we have I3d on Gb0 FORWARD or REVERSED, we do NOT need
  //                interference on Gb1 to determinate split IN/OUT of edge.
  Standard_Boolean kp6 = (!isSE); 
  if (kp6) {
    kp6 = Standard_False;
    TopOpeBRepDS_ListIteratorOfListOfInterference it(loiSHAINT);
    for (; it.More(); it.Next()){
      const Handle(TopOpeBRepDS_Interference)& I = it.Value();
      TopOpeBRepDS_Kind ST = I->SupportType();
      if (ST != TopOpeBRepDS_FACE) continue;
      TopAbs_Orientation O = I->Transition().Orientation(TopAbs_IN);
      Standard_Boolean FORREV = (O == TopAbs_FORWARD) || (O == TopAbs_REVERSED);
      if (!FORREV) continue;
      Standard_Boolean Gb1 = ::FUN_isonbound(myDataStructure,I);
      if (!Gb1) {kp6 = Standard_True; break;}
    } // it(l3dFOR+l3dREV)	
  }
    
  // 1.
  //---
  tki.Init();
  while (tki.More()) {
    
    // lieu courant : Kcur,Gcur; Interferences : LICur
    TopOpeBRepDS_Kind Kcur;
    Standard_Integer Gcur;
    const TopOpeBRepDS_ListOfInterference& LICur = tki.Value(Kcur,Gcur);
    Standard_Boolean point  = (Kcur == TopOpeBRepDS_POINT); //xpu170498
    Standard_Boolean vertex = (Kcur == TopOpeBRepDS_VERTEX);//xpu170498
    TopoDS_Shape vGsd; 
    if (vertex) FUN_ds_getoov(BDS.Shape(Gcur), myDataStructure, vGsd); //xpu221098

    // recall : I3d=(I3dF,I3dFE) : I3dF=(T(F),G,F), I3dFE=(T(F),G,E)
    //          I2d=I2dFE
    //          I1d=(T(E),V,E)
    
    if ((Kcur == TopOpeBRepDS_VERTEX) && (kp1 == Gcur)) {tki.Next();continue;}
    const Handle(TopOpeBRepDS_Interference)& I = LICur.First();
    Standard_Real parSE = FDS_Parameter(I);
    TopOpeBRepDS_ListOfInterference LICurcopy; 
    TopOpeBRepDS_ListOfInterference l3dFcur; FDS_assign(LICur,LICurcopy); Standard_Integer n3d=FUN_selectSKinterference(LICurcopy,TopOpeBRepDS_FACE,l3dFcur);
    TopOpeBRepDS_ListOfInterference l2dFEcur; FDS_assign(LICur,LICurcopy); Standard_Integer n2d=FUN_ds_hasI2d(iEDS,LICurcopy,l2dFEcur);
    TopOpeBRepDS_ListOfInterference l1dEcur; FDS_assign(LICur,LICurcopy);
    FUN_selectTRASHAinterference(LICurcopy,TopAbs_EDGE,l1dEcur);

    TopAbs_State stb; Standard_Integer isb; Standard_Integer bdim;
    TopAbs_State sta; Standard_Integer isa; Standard_Integer adim;
    FUN_ds_GetTr(BDS,iEDS,Gcur,LICur,
                 stb,isb,bdim,
                 sta,isa,adim);    
    if (isSE) {
      // before
      Standard_Boolean bIN1d = (stb==TopAbs_IN)&&(bdim==1);
      Standard_Boolean bIN2d = (stb==TopAbs_IN)&&(bdim==2);
      Standard_Boolean bIN3d = (stb==TopAbs_IN)&&(bdim==3);

      Standard_Boolean bOUT2d = (stb==TopAbs_OUT)&&(bdim==2);
      Standard_Boolean bOUT3d = (stb==TopAbs_OUT)&&(bdim==3);
      // after
      Standard_Boolean aIN1d = (sta==TopAbs_IN)&&(adim==1);
      Standard_Boolean aIN2d = (sta==TopAbs_IN)&&(adim==2);
      Standard_Boolean aIN3d = (sta==TopAbs_IN)&&(adim==3);

      Standard_Boolean aOUT2d = (sta==TopAbs_OUT)&&(adim==2);
      Standard_Boolean aOUT3d = (sta==TopAbs_OUT)&&(adim==3);
      
      TopOpeBRepDS_Transition newT; Standard_Boolean INb=Standard_False,INa=Standard_False;
      if (isfafa) {
	if       (isspon) {
	  if ((stb == TopAbs_OUT)&&(sta == TopAbs_OUT)) {tki.Next(); continue;}
	  INb = bIN1d;
	  INa = aIN1d;
	  newT.Index(isb); newT.ShapeBefore(TopAbs_EDGE); newT.ShapeAfter(TopAbs_EDGE);
	} else if (isspin) {
	  INb = bIN2d;
	  INa = aIN2d;
	  newT.ShapeBefore(TopAbs_FACE); newT.ShapeAfter(TopAbs_FACE);
	} else if (isspou) {
	  INb = !bOUT2d;
	  INa = !aOUT2d;
	  newT.ShapeBefore(TopAbs_FACE); newT.ShapeAfter(TopAbs_FACE);
	}
      }
      else {
	if       (isspon) {
	  if ((stb == TopAbs_OUT)&&(sta == TopAbs_OUT)) {tki.Next(); continue;}
	  INb = bIN1d || bIN2d;
	  INa = aIN1d || aIN2d;
	  newT.Index(isb); newT.ShapeBefore(TopAbs_EDGE); newT.ShapeAfter(TopAbs_EDGE);
	} else if (isspin) {
	  if ((stb == TopAbs_OUT)&&(sta == TopAbs_OUT)) {tki.Next(); continue;}
	  INb = bIN3d;
	  INa = aIN3d;
	  if (INb) newT.Index(isb);
	  else     newT.Index(isa);
	  newT.ShapeBefore(TopAbs_FACE); newT.ShapeAfter(TopAbs_FACE);
	} else if (isspou) {
	  if ((stb == TopAbs_IN)&&(sta == TopAbs_IN)) {tki.Next(); continue;}
	  INb = !bOUT3d;
	  INa = !aOUT3d;
	  if (bOUT3d) newT.Index(isb);
	  else        newT.Index(isa);
	  newT.ShapeBefore(TopAbs_FACE); newT.ShapeAfter(TopAbs_FACE);
	}
      }
      TopAbs_State sb = INb ? TopAbs_IN : TopAbs_OUT;
      TopAbs_State sa = INa ? TopAbs_IN : TopAbs_OUT;
      newT.StateBefore(sb);newT.StateAfter(sa);
      Standard_Integer S=0; // dummy
      Standard_Boolean B = (Kcur == TopOpeBRepDS_POINT) ? Standard_False : (Handle(TopOpeBRepDS_EdgeVertexInterference)::DownCast(I)->GBound()); 
      Handle(TopOpeBRepDS_Interference) newI = MakeEPVInterference(newT,S,Gcur,parSE,Kcur,B);
      
      TopOpeBRepDS_ListOfInterference li; li.Append(newI); TopOpeBRepDS_PointIterator itCur(li);
      GFillPointTopologyPVS(E,itCur,G,PVS);
      {tki.Next(); continue;}
    } // isSE    

    // - kp3 -
    // xpu200598 interference 2d at GPOINT
    Standard_Boolean kp3 = (n2d > 0) && point;
    if (kp3) l2dFEcur.First()->Transition().Orientation(TopAbs_IN);


    TopOpeBRepDS_PointIterator itCur(LICur); Standard_Integer iICur=0;
    while ( itCur.More() ) {
      iICur++;
      const Handle(TopOpeBRepDS_Interference)& I1=itCur.Value();
      const TopOpeBRepDS_Transition& T1=I1->Transition();
      T1.Orientation(TopAbs_IN);
      TopAbs_ShapeEnum SB1,SA1;Standard_Integer IB1,IA1;TopOpeBRepDS_Kind GT1,ST1;Standard_Integer G1,S1;
      FDS_Idata(I1,SB1,IB1,SA1,IA1,GT1,G1,ST1,S1);

      Standard_Boolean keepinterf1 = Standard_False;
      if      (isEd) {
	keepinterf1 = Standard_True;
      }
      else {
	if (GLOBAL_faces2d) {  // split 2d
	  Standard_Boolean has2d3d = (n2d >0 && n3d >0); // JYL300998
	  // JYL300998 : traitement correct de cto 100 K1 e27 (chanceux auparavant, schema d'I faux)
	  // e27 n'est PAS arete de section mais doit etre traitee comme telle.
	  // e27 possede des I de nature 2d et 3d en V8
	  // on privilegie l'info 3d 
	  if (has2d3d && !isSE) {
#ifdef OCCT_DEBUG
	    const Handle(TopOpeBRepDS_Interference)& i2d =
#endif
                               l2dFEcur.First();
	    const Handle(TopOpeBRepDS_Interference)& i3d = l3dFcur.First();
	    Standard_Boolean id3d = (I1 == i3d);
	    keepinterf1 = id3d;
#ifdef OCCT_DEBUG
	    Standard_Boolean id2d = (I1 == i2d);
	    if (tSPS) {
	      cout<<"DEB : GFillPointTopologyPVS E"<<iEDS<<" has2d3d"<<endl;
	      if (id3d) cout<<"--> Interference 3d ";
	      if (id2d) cout<<"--> Interference 2d ";
	      if (keepinterf1) cout<<" traitee"<<endl;
	      else             cout<<" non traitee"<<endl;
	      cout<<endl;
	    }
#endif
	  }
	  else {
	    keepinterf1 = Standard_True; // PRO13075 tspIN(e17)
	  }
	} // split 2d
	else { // split 3d
	  keepinterf1 = (ST1 == TopOpeBRepDS_FACE); // (iICur == 1);
	}
      }
      if ( keepinterf1 ) {
	if (kp6) {
	  Standard_Boolean Gb1 = ::FUN_isonbound(myDataStructure,I1);
	  if (!Gb1) GFillPointTopologyPVS(E,itCur,G,PVS);
	}
	else       {
	  GFillPointTopologyPVS(E,itCur,G,PVS);
	}
	if (!dgE) break; // xpu140498	
      } // keepinterf1
      itCur.Next();
    } // itCur.More

    tki.Next();
  } // tki.More()
}

//=======================================================================
//function : GFillPointTopologyPVS
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::GFillPointTopologyPVS(const TopoDS_Shape& E,
                                                    const TopOpeBRepDS_PointIterator& EPit,
                                                    const TopOpeBRepBuild_GTopo& G1,
                                                    TopOpeBRepBuild_PaveSet& PVS) const
{
  const TopoDS_Shape& EPVS = PVS.Edge();
  
  //modified by NIZHNY-MZV  Mon Feb 21 14:47:34 2000
  const Handle(TopOpeBRepDS_Interference)& I1=EPit.Value();
  TopOpeBRepDS_Kind ST1 = I1->SupportType();
    
#ifdef OCCT_DEBUG
  Standard_Integer iE; Standard_Boolean tSPSE = GtraceSPS(E,iE);
  Standard_Integer iEPVS; Standard_Boolean tSPSEPVS = GtraceSPS(EPVS,iEPVS);
  Standard_Boolean tSPS = tSPSE || tSPSEPVS;
  if ( tSPS ) debfillp(iE);
#endif
  
  TopAbs_State TB1,TB2; 
  G1.StatesON(TB1,TB2);
  TopOpeBRepDS_Config Conf = G1.Config1();
  TopAbs_State TB = TB1;
  
  // iG = index of new point or existing vertex
  Standard_Integer iG = EPit.Current();
  Standard_Boolean ispoint = EPit.IsPoint();
  TopoDS_Vertex VIG; // NYI pointer
  if (ispoint) VIG = TopoDS::Vertex(NewVertex(iG));
  else VIG = TopoDS::Vertex(myDataStructure->Shape(iG));

  if (VIG.IsNull()) return; //PMN 17/02/99 Nothing to add.
  
  Standard_Boolean hasVSD = Standard_False;
  Standard_Integer iVRE = 0; TopoDS_Shape VRE; // NYI pointer
  if (!ispoint) {
    hasVSD = myDataStructure->HasSameDomain(VIG);
    if (hasVSD) { // on prend VRE = vertex reference de VIG
      iVRE = myDataStructure->SameDomainReference(VIG);
      VRE = TopoDS::Vertex(myDataStructure->Shape(iVRE));
    }
  }
  
  TopoDS_Vertex VPV; // NYI pointer on VRE or VIG
  if (hasVSD) VPV = TopoDS::Vertex(VRE);
  else        VPV = VIG;
//  else        VPV = TopoDS::Vertex(VIG);
  
  Standard_Real      par = EPit.Parameter();
  TopAbs_Orientation ori = EPit.Orientation(TB);

#ifdef OCCT_DEBUG
  if ( tSPS ) debfillp(iE);
#endif

  Standard_Boolean samegeom = ::TopOpeBRepBuild_FUN_aresamegeom(E,EPVS);
  if (Conf == TopOpeBRepDS_DIFFORIENTED) ori = TopAbs::Complement(ori);
#ifdef OCCT_DEBUG
  if (!TopOpeBRepBuild_GetcontextNOSG()) {
#endif
    if (!samegeom) ori = TopAbs::Complement(ori);
#ifdef OCCT_DEBUG
  }
#endif

  
  Standard_Boolean lesmemes = E.IsEqual(myEdgeReference);
  if ( !lesmemes ) {
    Standard_Real parref = par;
    const TopoDS_Edge& EE = TopoDS::Edge(E);
    GParamOnReference(VPV,EE,parref);
#ifdef OCCT_DEBUG
    if(tSPS){
      cout<<"par "<<par<<" / ";GdumpSHA(E);cout<<" --> parref "<<parref<<" / ";GdumpSHA(EPVS);
      cout<<endl;
    }
#endif
    par = parref;
  }

  Standard_Boolean kpbound = Standard_False;
  {
    TopoDS_Vertex vclo; Standard_Boolean Eclosed = TopOpeBRepTool_TOOL::ClosedE(myEdgeReference,vclo);
#ifdef OCCT_DEBUG
//    Standard_Integer ivclo = myDataStructure->Shape(vclo);
#endif
    TopAbs_Orientation oriI = EPit.Orientation(TopAbs_IN);
//    kpbound = lesmemes && Eclosed && hasVSD && (ori == TopAbs_INTERNAL) && (TB == TopAbs_OUT); -xpu140898
    // xpu110398 cto 009 L2 : e6ou en v11
    // xpu140898 USA60111 : e9ou (!=0) + e7ou(=0)
    Standard_Boolean INTEXT =       (oriI == TopAbs_INTERNAL) && (TB == TopAbs_IN);
    INTEXT = INTEXT || ((oriI == TopAbs_EXTERNAL) && (TB == TopAbs_OUT));
    kpbound = lesmemes && Eclosed && INTEXT; 
    if ( kpbound ) {
      kpbound = vclo.IsSame(VIG);
      if (!kpbound) {
	TopoDS_Shape VSD; Standard_Boolean ok = FUN_ds_getoov(VIG,myDataStructure->DS(),VSD);
	if (ok) kpbound = vclo.IsSame(VSD);
      }
    }
  }

  if (!kpbound) {
    VPV.Orientation(ori);
    Standard_Boolean vofe = Standard_False;
    Handle(TopOpeBRepBuild_Pave) PV = new TopOpeBRepBuild_Pave(VPV,par,vofe);
    if (hasVSD) {
      PV->HasSameDomain(Standard_True);
      const TopoDS_Shape& VSD = myDataStructure->SameDomain(VPV).Value();
      Standard_Integer iVSD = myDataStructure->Shape(VSD);
      if (iVSD == iVRE) PV->SameDomain(VIG);
      else              PV->SameDomain(VSD);
    }
    //modified by NIZHNY-MZV  Mon Feb 21 14:48:37 2000
    PV -> InterferenceType() = ST1;
    PVS.Append(PV);
    
    
#ifdef OCCT_DEBUG
    gp_Pnt P = BRep_Tool::Pnt(VPV);
    if(tSPS){cout<<"+";if(ispoint)cout<<" PDS ";else cout<<" VDS ";}
    if(tSPS){cout<<iG<<" : ";GdumpORIPARPNT(ori,par,P);cout<<endl;}
    if(tSPS) {
//      Standard_Boolean trc = Standard_False;
#ifdef DRAW
      if (trc) {
	FUN_draw2d(par,TopoDS::Edge(E),myEdgeReference,myFaceReference);
	FUN_draw(VPV); FUN_draw(E); FUN_draw(myFaceReference);
      }
#endif
    }
#endif
  }
  else {
    Standard_Real parf,parl; FUN_tool_bounds(myEdgeReference,parf,parl);
    TopAbs_Orientation ovpv;
    ovpv = TopAbs_FORWARD;
    VPV.Orientation(ovpv);
    Standard_Boolean vfofe = Standard_False;
    Handle(TopOpeBRepBuild_Pave) PVF = new TopOpeBRepBuild_Pave(VPV,parf,vfofe);
    if (hasVSD) {
      PVF->HasSameDomain(Standard_True);
      const TopoDS_Shape& VSD = myDataStructure->SameDomain(VPV).Value();
      Standard_Integer iVSD = myDataStructure->Shape(VSD);
      if (iVSD == iVRE) PVF->SameDomain(VIG);
      else              PVF->SameDomain(VSD);
    }
    //modified by NIZHNY-MZV  Mon Feb 21 14:48:37 2000
    PVF -> InterferenceType() = ST1;
    PVS.Append(PVF);
    
#ifdef OCCT_DEBUG
    gp_Pnt PF = BRep_Tool::Pnt(VPV);
    if(tSPS){cout<<"+";if(ispoint)cout<<" PDS ";else cout<<" VDS ";}
    if(tSPS){cout<<iG<<" : ";GdumpORIPARPNT(ovpv,parf,PF);cout<<endl;}
#endif

    ovpv = TopAbs_REVERSED;
    VPV.Orientation(ovpv);
    Standard_Boolean vrofe = Standard_False;
    Handle(TopOpeBRepBuild_Pave) PVR = new TopOpeBRepBuild_Pave(VPV,parl,vrofe);
    if (hasVSD) {
      PVR->HasSameDomain(Standard_True);
      const TopoDS_Shape& VSD = myDataStructure->SameDomain(VPV).Value();
      Standard_Integer iVSD = myDataStructure->Shape(VSD);
      if (iVSD == iVRE) PVR->SameDomain(VIG);
      else              PVR->SameDomain(VSD);
    }
    //modified by NIZHNY-MZV  Mon Feb 21 14:48:37 2000
    PVR -> InterferenceType() = ST1;
    PVS.Append(PVR);
#ifdef OCCT_DEBUG
    gp_Pnt PR = BRep_Tool::Pnt(VPV);
    if(tSPS){cout<<"+";if(ispoint)cout<<" PDS ";else cout<<" VDS ";}
    if(tSPS){cout<<iG<<" : ";GdumpORIPARPNT(ovpv,parl,PR);cout<<endl;}
#endif

    PVS.RemovePV(Standard_False); // jyl + 980217
  }

}


//=======================================================================
//function : GParamOnReference
//purpose  : calcul du parametre de V sur myEdgeReference de myFaceReference
//           V est sur la surface de myFaceReference, 
//           V est un vertex de E
//           E est une arete samedomain de myEdgeReference
// retourne true si ok
//=======================================================================
Standard_Boolean TopOpeBRepBuild_Builder::GParamOnReference(const TopoDS_Vertex& V,
                                                            const TopoDS_Edge& /*E*/,
                                                            Standard_Real& P) const 
{
  Handle(Geom_Surface) su = BRep_Tool::Surface(myFaceReference);
  Handle(Geom_Plane) suplan = Handle(Geom_Plane)::DownCast(su);
  if ( suplan.IsNull() ) { 
#ifdef OCCT_DEBUG
    cout<<"NYI : GParamOnReference : not planar"<<endl;
#endif
    return Standard_False;
  }

  gp_Pln pln = suplan->Pln(); gp_Pnt p3 = BRep_Tool::Pnt(V);
  Standard_Real u,v; ElSLib::Parameters(pln,p3,u,v); gp_Pnt2d p2(u,v);
  Standard_Real f,l,tolpc; Handle(Geom2d_Curve) C2D;
  C2D = FC2D_CurveOnSurface(myEdgeReference,myFaceReference,f,l,tolpc);
  if (C2D.IsNull()) throw Standard_ProgramError("TopOpeBRepBuild_Builder::GParamOnReference");
  
//  Standard_Real U;
  Geom2dAdaptor_Curve AC(C2D);
  switch ( AC.GetType() ) {
  case GeomAbs_Line: 
    P = ElCLib::Parameter(AC.Line(),p2); break;
  case GeomAbs_Circle: 
    P = ElCLib::Parameter(AC.Circle(),p2); break;
  case GeomAbs_Ellipse: 
    P = ElCLib::Parameter(AC.Ellipse(),p2); break;
  case GeomAbs_Hyperbola: 
    P = ElCLib::Parameter(AC.Hyperbola(),p2); break;
  case GeomAbs_Parabola:
    P = ElCLib::Parameter(AC.Parabola(),p2); break;
    default : 
#ifdef OCCT_DEBUG
      cout<<"NYI : GParamOnReference : OtherCurve on planar surface"<<endl;
#endif
    return Standard_False;
  }
  
  return Standard_True;
}
