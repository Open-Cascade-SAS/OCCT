// Created on: 1993-06-24
// Created by: Jean Yves LEBEY
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


#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRep_define.hxx>
#include <TopOpeBRep_DSFiller.hxx>
#include <TopOpeBRep_EdgesFiller.hxx>
#include <TopOpeBRep_FaceEdgeFiller.hxx>
#include <TopOpeBRep_FacesFiller.hxx>
#include <TopOpeBRep_ShapeIntersector.hxx>
#include <TopOpeBRep_ShapeIntersector2d.hxx>
#include <TopOpeBRepDS_connex.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_Filter.hxx>
#include <TopOpeBRepDS_GapFiller.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_IndexedDataMapOfShapeWithState.hxx>
#include <TopOpeBRepDS_Reducer.hxx>
#include <TopOpeBRepDS_samdom.hxx>
#include <TopOpeBRepDS_ShapeWithState.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopOpeBRepTool_box.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_SC.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

extern void BREP_sortonparameter(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
extern void BREP_mergePDS(const Handle(TopOpeBRepDS_HDataStructure)& HDS);


#ifdef OCCT_DEBUG
Standard_EXPORT void debcomplete(){};
Standard_EXPORT void debcompleteds(){};
static int Vdebfillerds=0;void debfillerds(void){Vdebfillerds++;}
static int Vdebfillerff=0;void debfillerff(void){Vdebfillerff++;}
static int Vdebfillerffsd=0;void debfillerffsd(void){Vdebfillerffsd++;}
static int Vdebfilleree=0;void debfilleree(void){Vdebfilleree++;}
static int Vdebfillereesd=0;void debfillereesd(void){Vdebfillereesd++;}
static int Vdebfilleref=0;void debfilleref(void){Vdebfilleref++;}
static int Vdebfillerfe=0;void debfillerfe(void){Vdebfillerfe++;}
void debfillerreset(void){Vdebfillerds=Vdebfillerff=Vdebfilleree=Vdebfilleref=Vdebfillerfe=0;}
void debfillerss(const Standard_Integer i1,const Standard_Integer i2){cout<<"+ + + + debfillerss "<<i1<<" "<<i2<<endl;}

#include <TopOpeBRepTool_KRO.hxx>
Standard_EXPORT TOPKRO KRO_DSFILLER_TOTAL("dsfiller");
extern TOPKRO KRO_DSFILLER_INTFF;
extern TOPKRO KRO_DSFILLER_INTEE;
extern TOPKRO KRO_DSFILLER_INTFE;
static void FUN_RESET_KRO_DSFILLER()
{
  KRO_DSFILLER_TOTAL.Reset();
  KRO_DSFILLER_INTFF.Reset();
  KRO_DSFILLER_INTEE.Reset();
  KRO_DSFILLER_INTFE.Reset();
}

#include <TopOpeBRep_traceSIFF.hxx>
#include <BRepTools.hxx>
#include <TopOpeBRepDS_ShapeShapeInterference.hxx>
#include <TopOpeBRepDS_CurvePointInterference.hxx>

extern TopOpeBRep_traceSIFF SIFF;

extern Standard_Boolean TopOpeBRepTool_GettraceKRO();
extern Standard_Boolean TopOpeBRep_GettraceSIFF();
extern Standard_Boolean TopOpeBRepTool_GettraceEND();
extern Standard_Boolean TopOpeBRep_GettraceSHA(const Standard_Integer);
extern Standard_Boolean TopOpeBRep_GetcontextNONOG();
#endif

//=======================================================================
//function : TopOpeBRep_DSFiller
//purpose  : 
//=======================================================================
TopOpeBRep_DSFiller::TopOpeBRep_DSFiller() 
:
  myPShapeClassifier(NULL)
{
}

//modified by NIZNHY-PKV Mon Dec 16 11:12:38 2002 f
//=======================================================================
//function : ~TopOpeBRep_DSFiller
//purpose  : 
//=======================================================================
TopOpeBRep_DSFiller::~TopOpeBRep_DSFiller() 
{
  if (myPShapeClassifier) {
    delete myPShapeClassifier;
  }
}
//modified by NIZNHY-PKV Mon Dec 16 11:12:41 2002 t

//=======================================================================
//function : PShapeClassifier
//purpose  : 
//=======================================================================
TopOpeBRepTool_PShapeClassifier TopOpeBRep_DSFiller::PShapeClassifier() const
{
  return myPShapeClassifier;
}

//modified by NIZNHY-PKV Mon Dec 16 11:30:43 2002 f
/*
//=======================================================================
//function : SetPShapeClassifier
//purpose  : 
//=======================================================================
void TopOpeBRep_DSFiller::SetPShapeClassifier(const TopOpeBRepTool_PShapeClassifier& PSC) 
{
  myPShapeClassifier = PSC;
}
*/
//modified by NIZNHY-PKV Mon Dec 16 11:30:52 2002 t

//=======================================================================
//function : BREP_correctgbound
//purpose  : 
//=======================================================================
void BREP_correctgbound(const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  Standard_Integer i=1,n=BDS.NbShapes();
  for (;i<=n;i++) {
    Standard_Boolean ehassiv = Standard_False;
    const TopoDS_Shape& s = BDS.Shape(i);
    TopAbs_ShapeEnum t = s.ShapeType();
    if ( t != TopAbs_EDGE ) continue;
    TopOpeBRepDS_ListIteratorOfListOfInterference it;
    it.Initialize(BDS.ChangeShapeInterferences(s));
    if ( !it.More() ) continue;    
    for (; it.More(); it.Next()) {
      const Handle(TopOpeBRepDS_Interference)& I = it.Value();
      Handle(TopOpeBRepDS_ShapeShapeInterference) SSI = 
	Handle(TopOpeBRepDS_ShapeShapeInterference)::DownCast(I);
      if ( SSI.IsNull() ) continue;
      TopOpeBRepDS_Kind GK = SSI->GeometryType();
      if ( GK != TopOpeBRepDS_VERTEX ) continue;
      
      ehassiv = Standard_True;
      break;
    } // it.More
    
    if (! ehassiv ) continue;
    
    // l'arete s a au moins une shapeshapeinterference de geometrie VERTEX.
    // on verifie l'information GBound de toutes ses shapeshapeinterferences.    
    TopTools_IndexedMapOfShape imev;
    TopExp::MapShapes(s,TopAbs_VERTEX,imev);
    
    it.Initialize(BDS.ChangeShapeInterferences(s));
    for (; it.More(); it.Next()) {
      const Handle(TopOpeBRepDS_Interference)& I = it.Value();
      Handle(TopOpeBRepDS_ShapeShapeInterference) SSI = Handle(TopOpeBRepDS_ShapeShapeInterference)::DownCast(I);
      if ( SSI.IsNull() ) continue;
      Standard_Integer GI = SSI->Geometry();
      TopOpeBRepDS_Kind GK = SSI->GeometryType();
      if ( GK != TopOpeBRepDS_VERTEX ) continue;
      
      const TopoDS_Shape& v = BDS.Shape(GI);
      Standard_Boolean vofe = imev.Contains(v);
      SSI->SetGBound(vofe);
    } // it.More()   
    
    // l'arete s a au moins une shapeshapeinterference de geometrie VERTEX.
    it.Initialize(BDS.ChangeShapeInterferences(s));
    for (; it.More(); it.Next()) {
      const Handle(TopOpeBRepDS_Interference)& I = it.Value();
      Handle(TopOpeBRepDS_ShapeShapeInterference) SSI = Handle(TopOpeBRepDS_ShapeShapeInterference)::DownCast(I);
      if ( SSI.IsNull() ) continue;
      Standard_Integer GI = SSI->Geometry();
      TopOpeBRepDS_Kind GK = SSI->GeometryType();
      if ( GK != TopOpeBRepDS_VERTEX ) continue;
      const TopoDS_Shape& v = BDS.Shape(GI);      
      Standard_Boolean vhassd = HDS->HasSameDomain(v);
      if (! vhassd ) continue;
      Standard_Integer ivref = BDS.SameDomainRef(v);
      if ( ivref == GI ) continue;
      
      const TopoDS_Shape& vref = BDS.Shape(ivref);
      Standard_Boolean vrefofe = imev.Contains(vref);
      I->SetGeometry(ivref);
      SSI->SetGBound(vrefofe);    
    } // it.More
  } // i
  return;
} // correctGBound
//=======================================================================
//function : BREP_UnfillSameDomain
//purpose  : 
//=======================================================================
Standard_Boolean BREP_UnfillSameDomain(const TopoDS_Shape& F1,
				       const TopoDS_Shape& F2,
				       const Handle(TopOpeBRepDS_HDataStructure)& HDS,
				       TopOpeBRepTool_ShapeClassifier& SC)
{
  Standard_Boolean unfill = Standard_True;
  TopAbs_State st1=TopAbs_UNKNOWN,st2=TopAbs_UNKNOWN;
  Standard_Integer samdom = 1;
  st1 = SC.StateShapeShape(F1,F2,samdom);
  st2 = SC.StateShapeShape(F2,F1,samdom);
  if (((st1==TopAbs_OUT) && (st2==TopAbs_OUT)) || 
      ((st1==TopAbs_UNKNOWN) && (st2==TopAbs_UNKNOWN)) ) {
    unfill = Standard_True; // NYI IN IN aussi
  }
  else {
    unfill = Standard_False;
  }
  if (unfill) {
    TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
    BDS.UnfillShapesSameDomain(F1,F2);
  }
  return unfill;
}
//=======================================================================
//function : FUN_shareNOG
//purpose  : 
//=======================================================================
static 
  Standard_Boolean FUN_shareNOG(const Handle(TopOpeBRepDS_HDataStructure)& HDS, 
				const TopoDS_Shape& lFF1, 
				const TopoDS_Shape& lFF2)
{
  const TopOpeBRepDS_DataStructure& BDS = HDS->DS();
  const TopoDS_Face& F1 = TopoDS::Face(lFF1);
  const TopoDS_Face& F2 = TopoDS::Face(lFF2);  
  TopTools_IndexedMapOfShape map1; TopExp::MapShapes(F1,TopAbs_EDGE,map1);
  TopTools_IndexedMapOfShape map2; TopExp::MapShapes(F2,TopAbs_EDGE,map2);
  Standard_Real tola = Precision::Angular();

  const TopOpeBRepDS_ListOfInterference& lIF1 = BDS.ShapeInterferences(F1);
  TopOpeBRepDS_ListIteratorOfListOfInterference it1(lIF1);
  for (; it1.More(); it1.Next()){
    const Handle(TopOpeBRepDS_Interference)& I1 = it1.Value();
    Standard_Integer G = I1->Geometry(); TopOpeBRepDS_Kind GT = I1->GeometryType(); 
    if (GT != TopOpeBRepDS_EDGE) continue;
    const TopoDS_Shape& EG = BDS.Shape(G);
    if (map2.Contains(EG)) return Standard_False;
  }
  const TopOpeBRepDS_ListOfInterference& lIF2 = BDS.ShapeInterferences(F2);
  TopOpeBRepDS_ListIteratorOfListOfInterference it2(lIF2);
  for (; it2.More(); it2.Next()){
    const Handle(TopOpeBRepDS_Interference)& I2 = it2.Value();
    Standard_Integer G = I2->Geometry(); TopOpeBRepDS_Kind GT = I2->GeometryType(); 
    if (GT != TopOpeBRepDS_EDGE) continue;
    const TopoDS_Shape& EG = BDS.Shape(G);
    if (map1.Contains(EG)) return Standard_False;
  }


  TopExp_Explorer ex1(F1, TopAbs_EDGE);
  for (; ex1.More(); ex1.Next()){
    const TopoDS_Edge& e1 = TopoDS::Edge(ex1.Current());
    Standard_Boolean hsd1 = HDS->HasSameDomain(e1);
    if (hsd1) {
      TopTools_ListIteratorOfListOfShape itsd1(HDS->SameDomain(e1));
      for (; itsd1.More(); itsd1.Next()){
	const TopoDS_Shape& e2 = itsd1.Value();
	if (map2.Contains(e2)) return Standard_False;
      }
    }

    const TopOpeBRepDS_ListOfInterference& lie1 = BDS.ShapeInterferences(e1);
    TopOpeBRepDS_ListIteratorOfListOfInterference iter1(lie1);
    for (; iter1.More(); iter1.Next()){
      const Handle(TopOpeBRepDS_Interference)& I1 = iter1.Value();
      Standard_Integer S1 = I1->Support(); TopOpeBRepDS_Kind ST1 = I1->SupportType();
      if (ST1 != TopOpeBRepDS_EDGE) continue;
      const TopoDS_Edge& e2 = TopoDS::Edge(BDS.Shape(S1));
      if (!map2.Contains(e2)) continue;

  TopOpeBRepDS_Kind GT1 = I1->GeometryType();
      if (GT1 == TopOpeBRepDS_POINT) {
	Handle(TopOpeBRepDS_CurvePointInterference) CPI1 = Handle(TopOpeBRepDS_CurvePointInterference)::DownCast(I1); 
	Standard_Real par1 = CPI1->Parameter();
	Standard_Real par2; Standard_Boolean ok = FUN_tool_parE(e1,par1,e2,par2);
	if (!ok) {
#ifdef OCCT_DEBUG
	  cout<<"**********TopOpeBRep FUN_shareNOG**********"<<endl;
#endif
	  return Standard_False;
	}
	gp_Vec tge1 = FUN_tool_tggeomE(par1,e1);
	gp_Vec tge2 = FUN_tool_tggeomE(par2,e2);
	Standard_Real dot = gp_Dir(tge1).Dot(gp_Dir(tge2));
	Standard_Real x = Abs(1-Abs(dot));
	if (x > tola) return Standard_False; // e1,e2 not tangent

	gp_Vec xxF1 = FUN_tool_getgeomxx(F1,e1,par1);
	gp_Vec xxF2 = FUN_tool_getgeomxx(F2,e2,par2);
	dot = gp_Dir(xxF1).Dot(gp_Dir(xxF2));
	if (dot > 0) return Standard_False; // F1,F2 share geometric domain near G1
      }
    }// it1.More()
  }// ex1(F1,EDGE)    
  return Standard_True;

} // FUN_shareNOG

//=======================================================================
//function : Insert
//purpose  : 
//=======================================================================
void TopOpeBRep_DSFiller::Insert(const TopoDS_Shape& aS1,
				 const TopoDS_Shape& aS2,
				 const Handle(TopOpeBRepDS_HDataStructure)& HDS,
				 const Standard_Boolean orientFORWARD)
{
  InsertIntersection(aS1,aS2,HDS,orientFORWARD);
  Complete(HDS);
#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceEND()) cout<<"DSFiller::Insert"<<endl;
#endif
} // Insert

//=======================================================================
//function : InsertIntersection
//purpose  : 
//=======================================================================
void TopOpeBRep_DSFiller::InsertIntersection(const TopoDS_Shape& aS1,
					     const TopoDS_Shape& aS2,
					     const Handle(TopOpeBRepDS_HDataStructure)& HDS,
					     const Standard_Boolean orientFORWARD)
{
  FBOX_Prepare();
  FC2D_Prepare(aS1,aS2);

  if (myPShapeClassifier == NULL) myPShapeClassifier = new TopOpeBRepTool_ShapeClassifier();
  myFacesFiller.SetPShapeClassifier(myPShapeClassifier);
  
#ifdef OCCT_DEBUG
  Standard_Boolean tsiff = TopOpeBRep_GettraceSIFF();
  Standard_Boolean tkro = TopOpeBRepTool_GettraceKRO();
  if (tsiff) SIFF.Start("DSFiller debut tsif ",cout);
  if (tkro) { FUN_RESET_KRO_DSFILLER(); KRO_DSFILLER_TOTAL.Start(); }
#endif
  
  if ( ! CheckInsert(aS1,aS2) ) {
    HDS->AddAncestors(aS1); // xpu100798 cto902B4 (f24 as face ancestor of e19)
    HDS->AddAncestors(aS2); // xpu100798 
    FDSCNX_Prepare(aS1,aS2,HDS);
    return;
  }
  
  TopoDS_Shape S1 = aS1; 
  TopoDS_Shape S2 = aS2; 
  if ( orientFORWARD ) {
    if ( S1.Orientation() == TopAbs_REVERSED ) {
      S1.Orientation(TopAbs_FORWARD);
    }
    if ( S2.Orientation() == TopAbs_REVERSED ) {
      S2.Orientation(TopAbs_FORWARD);
    }
  }

  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  BDS.AddShape(S1,1);
  BDS.AddShape(S2,2);
  TopExp_Explorer ex1,ex2;
  for (ex1.Init(S1,TopAbs_SOLID); ex1.More(); ex1.Next()) {
    const TopoDS_Shape& so1 = ex1.Current();
    for (ex2.Init(S2,TopAbs_SOLID); ex2.More(); ex2.Next()) {
      const TopoDS_Shape& so2 = ex2.Current();
      BDS.FillShapesSameDomain(so1,so2);
    }
  }
  
#ifdef OCCT_DEBUG
  debfillerreset(); // debug
#endif

  Standard_Boolean FFsamdom = Standard_True; // xpu060598
  Standard_Boolean islFFsamdom = Standard_False;
  Standard_Boolean isFF = Standard_False;
  Standard_Boolean isFFsamdom = Standard_False;
  Standard_Boolean isEE = Standard_False;
  Standard_Boolean unfill = Standard_False;
  TopoDS_Shape lFF1, lFF2;
  //
  // Find all Rejected Faces on the Object and on the Tool
  /////////////// Rejected Faces' Block
  TopOpeBRepDS_DataStructure& aDataStructure=HDS->ChangeDS();
  
  TopTools_IndexedMapOfShape& aMapOfRejectedShapesObj=
    aDataStructure.ChangeMapOfRejectedShapesObj();
  TopTools_IndexedMapOfShape& aMapOfRejectedShapesTool=
    aDataStructure.ChangeMapOfRejectedShapesTool();
  aMapOfRejectedShapesObj.Clear();
  aMapOfRejectedShapesTool.Clear();

  // 1.Find all Rejected Faces on the Object and on the Tool
  TopTools_ListOfShape aListObj, aListTool;

  //modified by NIZHNY-MKK  Fri Apr 14 09:35:26 2000.BEGIN
  TopTools_IndexedMapOfShape aMapOfSolids;

  TopExp::MapShapes(S2, TopAbs_SOLID, aMapOfSolids);
  if(!aMapOfSolids.IsEmpty())
    myShapeIntersector.RejectedFaces (S1, S2, aListObj);

  aMapOfSolids.Clear();
  TopExp::MapShapes(S1, TopAbs_SOLID, aMapOfSolids);
  if(!aMapOfSolids.IsEmpty())
    myShapeIntersector.RejectedFaces (S2, S1, aListTool);
  //modified by NIZHNY-MKK  Fri Apr 14 09:37:32 2000.END
 
  // 2.Insert all rejected faces, wires, edges of Object in DS:
  TopTools_ListIteratorOfListOfShape anIt(aListObj);
  
  for (; anIt.More(); anIt.Next()) {
    const TopoDS_Shape& aS=anIt.Value();
    aMapOfRejectedShapesObj.Add(aS);
  }

  TopTools_ListIteratorOfListOfShape anIt1(aListTool);
  for (; anIt1.More(); anIt1.Next()) {
    const TopoDS_Shape& aS=anIt1.Value();
    aMapOfRejectedShapesTool.Add(aS);
  }
 
  //
  myShapeIntersector.InitIntersection(S1,S2);
  while (myShapeIntersector.MoreIntersection()) {
    
#ifdef OCCT_DEBUG
    Standard_Integer i1deb = myShapeIntersector.Index(1);
    Standard_Integer i2deb = myShapeIntersector.Index(2);
    Standard_Boolean b1deb = TopOpeBRep_GettraceSHA(i1deb);
    Standard_Boolean b2deb = TopOpeBRep_GettraceSHA(i2deb);
    if (b1deb && b2deb) debfillerss(i1deb,i2deb);
    debfillerds(); // debug
#endif
    
    // The two intersecting GeomShapes gs1,gs2 and their types t1,t2
    const TopoDS_Shape& gs1 = myShapeIntersector.CurrentGeomShape(1);
    const TopoDS_Shape& gs2 = myShapeIntersector.CurrentGeomShape(2);
    TopAbs_ShapeEnum t1 = gs1.ShapeType();
    TopAbs_ShapeEnum t2 = gs2.ShapeType();
    
    // si le couple courant apres un couple facefacesamedomain n'est
    // pas un couple edgeedge ==> facefacesamedomain est deconnecte 
    isFF = ((t1==TopAbs_FACE) && (t2==TopAbs_FACE));
    isFFsamdom = Standard_False;
    isEE = ((t1==TopAbs_EDGE) && (t2==TopAbs_EDGE));
    unfill = (!isEE && islFFsamdom);
    
    if (unfill) {
      if (myPShapeClassifier==NULL) myPShapeClassifier = new TopOpeBRepTool_ShapeClassifier();
      unfill = BREP_UnfillSameDomain(lFF1,lFF2,HDS,*myPShapeClassifier);
    }
    
    if      (isFF) {
#ifdef OCCT_DEBUG
      if (tsiff) SIFF.Add(i1deb,i2deb); debfillerff();
#endif
      TopOpeBRep_FacesIntersector& FF=myShapeIntersector.ChangeFacesIntersector();
      isFFsamdom = FF.SameDomain();
      FFsamdom = FFsamdom && isFFsamdom; //xpu060598
      if (isFFsamdom) {
	lFF1 = FF.Face(1);
	lFF2 = FF.Face(2);
      }
      
#ifdef OCCT_DEBUG
      if (isFFsamdom) debfillerffsd(); // debug
      Standard_Integer exf1 = myShapeIntersector.Index(1);
      Standard_Integer exf2 = myShapeIntersector.Index(2);
      myFacesFiller.SetTraceIndex(exf1,exf2);
#endif
      myFacesFiller.Insert(gs1,gs2,FF,HDS);
    }
    
    else if ((t1 == TopAbs_EDGE) && (t2 == TopAbs_EDGE)) {
#ifdef OCCT_DEBUG
      debfilleree(); // debug
#endif
      TopOpeBRep_EdgesIntersector& EE = 
	myShapeIntersector.ChangeEdgesIntersector();
      EE.Dimension(2);
#ifdef OCCT_DEBUG
      Standard_Boolean EEsamedomain =
#endif
                         EE.SameDomain();
#ifdef OCCT_DEBUG
      if (EEsamedomain) debfillereesd(); // debug
#endif
      if ( islFFsamdom ) {
	myEdgesFiller.Face(1,lFF1);
	myEdgesFiller.Face(2,lFF2);
      }
      myEdgesFiller.Insert(gs1,gs2,EE,HDS);
    }
    
    else if ((t1 == TopAbs_FACE) && (t2 == TopAbs_EDGE)) {
#ifdef OCCT_DEBUG
      debfillerfe(); // debug
#endif
      TopOpeBRep_FaceEdgeIntersector& FE = 
	myShapeIntersector.ChangeFaceEdgeIntersector();
      myFaceEdgeFiller.Insert(gs1,gs2,FE,HDS);
    }
    
    else if ((t1 == TopAbs_EDGE) && (t2 == TopAbs_FACE)) {
#ifdef OCCT_DEBUG
      debfilleref(); // debug
#endif
      TopOpeBRep_FaceEdgeIntersector& FE = 
	myShapeIntersector.ChangeFaceEdgeIntersector();
      myFaceEdgeFiller.Insert(gs2,gs1,FE,HDS);
    }
    
    islFFsamdom = isFFsamdom;
    myShapeIntersector.NextIntersection();
    
  } // while (MoreIntersection())

  // FFsamdom = true : rien que des faces tangentes
  // codage des aretes de section des aretes samedomain
  if (FFsamdom) FUN_ds_FillSDMFaces(HDS);// xpu060598
  // xpu280199 : On stocke les edges sdm comme edges de section aussi ! - CTS21801 -
  else          FUN_ds_addSEsdm1d(HDS);

  if (FFsamdom) HDS->ChangeDS().Isfafa(Standard_True);// xpu120598
  
  if (islFFsamdom && !isEE) {
    if (myPShapeClassifier==NULL) myPShapeClassifier = new TopOpeBRepTool_ShapeClassifier();
    unfill = BREP_UnfillSameDomain(lFF1,lFF2,HDS,*myPShapeClassifier);
  }
  
  BREP_sortonparameter(HDS);
  BREP_correctgbound(HDS);
  BREP_mergePDS(HDS);

  HDS->AddAncestors(S1); // xpu100798 cto902B4 (f24 as face ancestor of e19)
  HDS->AddAncestors(S2); // xpu100798 
  FDSCNX_Prepare(aS1,aS2,HDS);
  FDSSDM_prepare(HDS);
  
#ifdef OCCT_DEBUG
  if (tkro) KRO_DSFILLER_TOTAL.Stop();if (tsiff) SIFF.End("DSFiller fin tsif ",cout);
#endif
#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceEND()) cout<<"DSFiller::InsertIntersection"<<endl;
#endif
  
} // InsertIntersection

//=======================================================================
//function : Complete
//purpose  : 
//=======================================================================
void TopOpeBRep_DSFiller::Complete(const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
#ifdef OCCT_DEBUG
  debcomplete();
#endif
  GapFiller(HDS);
  CompleteDS(HDS);
  Filter(HDS);
  Reducer(HDS);
  RemoveUnsharedGeometry(HDS);
  Checker(HDS);
#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceEND()) cout<<"DSFiller::Complete"<<endl;
#endif
} // Complete

//=======================================================================
//function : GapFiller
//purpose  : 
//=======================================================================
void TopOpeBRep_DSFiller::GapFiller(const Handle(TopOpeBRepDS_HDataStructure)& HDS) const
{
  TopOpeBRepDS_GapFiller GF(HDS); GF.Perform();
#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceEND()) cout<<"DSFiller::GapFiller"<<endl;
#endif
}

//=======================================================================
//function : CompleteDS
//purpose  : 
//=======================================================================
void TopOpeBRep_DSFiller::CompleteDS(const Handle(TopOpeBRepDS_HDataStructure)& HDS) const
{
#ifdef OCCT_DEBUG
  debcompleteds();
#endif
  const TopoDS_Shape& S1 = myShapeIntersector.Shape(1);
  const TopoDS_Shape& S2 = myShapeIntersector.Shape(2);
  if(S1.IsNull() || S2.IsNull()) return;
//  HDS->AddAncestors(S1); -xpu100798 (is done before FDSCNX_Prepare)
//  HDS->AddAncestors(S2); -xpu100798

//  FUN_ds_unkeepEVIonGb1(HDS,TopAbs_IN); //xpu290698
  
  FUN_ds_PointToVertex(HDS);  // xpu090698, NYI TopOpeBRepDS_Filler
  FUN_ds_redusamsha(HDS);     // xpu201098
  FUN_ds_PURGEforE9(HDS);     // xpu040998,
  FUN_ds_completeforSE8(HDS); // xpu020998,
  FUN_ds_completeforSE1(HDS); // xpu160398,
  FUN_ds_completeforSE2(HDS); // xpu250398,
  FUN_ds_completeforSE3(HDS); // xpu170498,
  FUN_ds_completeforSE4(HDS); // xpu160698,
  FUN_ds_completeforSE5(HDS); // xpu190698,
  FUN_ds_completeforSE6(HDS); // xpu280798,
//  FUN_ds_completeforSE7(HDS); // xpu100898,
  FUN_ds_completeforE7(HDS); // xpu130898,
  FUN_ds_completeforSE9(HDS); // xpu011098
  FUN_ds_complete1dForSESDM(HDS); // MSV 25.03.2002 : OCC251
#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceEND()) cout<<"DSFiller::CompleteDS"<<endl;
#endif
} // CompleteDS

//=======================================================================
//function : Filter
//purpose  : 
//=======================================================================
void TopOpeBRep_DSFiller::Filter(const Handle(TopOpeBRepDS_HDataStructure)& HDS) const
{
  TopOpeBRepDS_Filter F(HDS);
  F.ProcessEdgeInterferences();
  F.ProcessCurveInterferences();
#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceEND()) cout<<"DSFiller::Filter"<<endl;
#endif
} // Filter

//=======================================================================
//function : Reducer
//purpose  : 
//=======================================================================
void TopOpeBRep_DSFiller::Reducer(const Handle(TopOpeBRepDS_HDataStructure)& HDS) const
{
  TopOpeBRepDS_Reducer R(HDS);
  R.ProcessEdgeInterferences();
#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceEND()) cout<<"DSFiller::Reducer"<<endl;
#endif
} // Reducer

//=======================================================================
//function : RemoveUnsharedGeometry
//purpose  : 
//=======================================================================
void TopOpeBRep_DSFiller::RemoveUnsharedGeometry(const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  Standard_Boolean processNOG = Standard_True;
#ifdef OCCT_DEBUG
  if (TopOpeBRep_GetcontextNONOG()) processNOG = Standard_False;
#endif
  if (!processNOG) return;
  
  // xpu290998 : PRO15369 (f5,f20 only share geometric point (vertex))
  // end processing for all information on shapes is given.
  const TopOpeBRepDS_DataStructure& BDS = HDS->DS();
  Standard_Integer nbs = BDS.NbShapes();
  for (Standard_Integer i = 1; i <= nbs; i++) {
    TopoDS_Shape S = BDS.Shape(i);
    if (S.ShapeType() != TopAbs_FACE) continue;
    Standard_Boolean hsd = HDS->HasSameDomain(S);
    if (!hsd) continue;
    Standard_Integer rkS = BDS.AncestorRank(S);
    if (rkS != 1) continue;
    const TopTools_ListOfShape& lSsd = BDS.ShapeSameDomain(S);
    TopTools_ListIteratorOfListOfShape itsd(lSsd);
    for (; itsd.More(); itsd.Next()){
      TopoDS_Shape Ssd = itsd.Value(); //xpuxpu
      Standard_Integer rkSsd = BDS.AncestorRank(Ssd);
      if (rkSsd == 1) continue;
      
      Standard_Boolean unfill = ::FUN_shareNOG(HDS,S,Ssd);
      unfill = unfill && FUN_ds_sdm(BDS,S,Ssd) && FUN_ds_sdm(BDS,Ssd,S);
      if (unfill) {
	if (myPShapeClassifier==NULL) myPShapeClassifier = new TopOpeBRepTool_ShapeClassifier();
	unfill = BREP_UnfillSameDomain(S,Ssd,HDS,*myPShapeClassifier);
      }
    }
  }
#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceEND()) cout<<"DSFiller::RemoveUnsharedGeometry"<<endl;
#endif
} // RemoveUnsharedGeometry

//=======================================================================
//function : Checker
//purpose  : 
//=======================================================================
void TopOpeBRep_DSFiller::Checker(const Handle(TopOpeBRepDS_HDataStructure)& /*HDS*/) const
{
  // TopOpeBRepDS_Checker C(HDS); // NYI
  //#ifdef OCCT_DEBUG
  //  if (TopOpeBRepTool_GettraceEND()) cout<<"DSFiller::Checker"<<endl;
  //#endif
}

//=======================================================================
//function : Insert2d
//purpose  : 
//=======================================================================

void TopOpeBRep_DSFiller::Insert2d
(const TopoDS_Shape& aS1,const TopoDS_Shape& aS2,const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  InsertIntersection2d(aS1,aS2,HDS);
  CompleteDS2d(HDS);
#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceEND()) cout<<"DSFiller::Insert2d"<<endl;
#endif
} // Insert

//=======================================================================
//function : InsertIntersection2d
//purpose  : 
//=======================================================================

void TopOpeBRep_DSFiller::InsertIntersection2d
(const TopoDS_Shape& aS1,const TopoDS_Shape& aS2,const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  if (myPShapeClassifier == NULL) {
    myPShapeClassifier = new TopOpeBRepTool_ShapeClassifier();
  }
  myFacesFiller.SetPShapeClassifier(myPShapeClassifier);
  
  if ( ! ClearShapeSameDomain(aS1, aS2, HDS) ) return;
  
  TopoDS_Shape S1 = aS1; 
  TopoDS_Shape S2 = aS2; 
  TopOpeBRepDS_DataStructure& BDS1 = HDS->ChangeDS();
  BDS1.AddShape(S1,1);
  BDS1.AddShape(S2,2);
  
  TopoDS_Shape lFF1, lFF2;
  Standard_Boolean isFFsamdom = Standard_False;
  
  myShapeIntersector2d.InitIntersection(S1,S2);
  while (myShapeIntersector2d.MoreIntersection()) {
    
    // The two intersecting GeomShapes gs1,gs2 and their types t1,t2
    const TopoDS_Shape& gs1 = myShapeIntersector2d.CurrentGeomShape(1);
    const TopoDS_Shape& gs2 = myShapeIntersector2d.CurrentGeomShape(2);
    TopAbs_ShapeEnum t1 = gs1.ShapeType();
    TopAbs_ShapeEnum t2 = gs2.ShapeType();
    
    // si le couple courant apres un couple facefacesamedomain n'est
    // pas un couple edgeedge ==> facefacesamedomain est deconnecte 
    Standard_Boolean isFF = ((t1==TopAbs_FACE) && (t2==TopAbs_FACE));
    Standard_Boolean isEE = ((t1==TopAbs_EDGE) && (t2==TopAbs_EDGE));
    Standard_Boolean unfill = (!isEE && isFFsamdom);
    if (unfill) {
      if (myPShapeClassifier==NULL) 
	myPShapeClassifier = new TopOpeBRepTool_ShapeClassifier();
      // NYI : mettre en champs un ShapeClassifier commun a tous 
      // NYI : les fillers
      TopAbs_State st1=TopAbs_UNKNOWN,st2=TopAbs_UNKNOWN;
      Standard_Integer samdom = 1;
      st1 = myPShapeClassifier->StateShapeShape(lFF1,lFF2,samdom);
      st2 = myPShapeClassifier->StateShapeShape(lFF2,lFF1,samdom);
      if ( ((st1 == TopAbs_OUT) && (st2 == TopAbs_OUT)) ||
	  ((st1 == TopAbs_UNKNOWN) && (st2 == TopAbs_UNKNOWN)) ) {
	unfill = Standard_True; // NYI IN IN aussi
      }
      else {
	unfill = Standard_False;
      }
      if (unfill) {
	TopOpeBRepDS_DataStructure& BDS2 = HDS->ChangeDS();
	if(!lFF1.IsNull() && !lFF2.IsNull())
	  BDS2.UnfillShapesSameDomain(lFF1,lFF2);
      }
    }
    
    if      (isFF) {
      isFFsamdom = Standard_True;
      myShapeIntersector2d.ChangeEdgesIntersector().SetFaces(gs1,gs2);
      lFF1 = gs1; lFF2 = gs2;
      BDS1.FillShapesSameDomain(gs1,gs2);
    }
    else if ((t1 == TopAbs_EDGE) && (t2 == TopAbs_EDGE)) {
      TopOpeBRep_EdgesIntersector& EE = myShapeIntersector2d.ChangeEdgesIntersector();
      EE.Dimension(2);
      myEdgesFiller.Face(1,lFF1);
      myEdgesFiller.Face(2,lFF2);
      myEdgesFiller.Insert(gs1,gs2,EE,HDS);
    }
    myShapeIntersector2d.NextIntersection();
  } // while (MoreIntersection())
  
  BREP_sortonparameter(HDS);
  BREP_correctgbound(HDS);
  BREP_mergePDS(HDS);

#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceEND()) cout<<"DSFiller::InsertIntersection2d"<<endl;
#endif

} // InsertIntersection2d


//=======================================================================
//function : CompleteDS2d
//purpose  : 
//=======================================================================

void TopOpeBRep_DSFiller::CompleteDS2d(const Handle(TopOpeBRepDS_HDataStructure)& HDS) const
{
  const TopoDS_Shape& S1 = myShapeIntersector2d.Shape(1);
  const TopoDS_Shape& S2 = myShapeIntersector2d.Shape(2);
  if(S1.IsNull() || S2.IsNull()) return;
  HDS->AddAncestors(S1);
  HDS->AddAncestors(S2);
  
  TopOpeBRepDS_Filter F(HDS);
  F.ProcessEdgeInterferences();
  F.ProcessCurveInterferences();
  TopOpeBRepDS_Reducer R(HDS);
  R.ProcessEdgeInterferences();
  // TopOpeBRepDS_Checker C(HDS); // NYI
#ifdef OCCT_DEBUG
  if (TopOpeBRepTool_GettraceEND()) cout<<"DSFiller::CompleteDS2d"<<endl;
#endif
} // CompleteDS2d

//=======================================================================
//function : IsMadeOf1d
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRep_DSFiller::IsMadeOf1d(const TopoDS_Shape& aS) const 
{
  // vrai si il existe > 1 WIRE et/ou > 1 EDGE sans ascendance de face
  Standard_Boolean res = Standard_False;
  TopAbs_ShapeEnum t = aS.ShapeType();
  if      ( t == TopAbs_WIRE ) {
    res = Standard_True;
  }
  else if ( t == TopAbs_EDGE ) {
    res = Standard_True;
  }
  else if ( t == TopAbs_COMPOUND ) {
    TopoDS_Iterator it(aS);
    if ( ! it.More() ) {
      res = Standard_False;
    }
    else {
      res = Standard_True;
      for (; it.More(); it.Next()) {
	const TopoDS_Shape& S = it.Value();
	Standard_Boolean is1d = IsMadeOf1d(S);
	if ( !is1d ) {
	  res = Standard_False;
	  break;
	}
      }
    }
  }
  else { // != COMPOUND,WIRE,EDGE 
    res = Standard_False;
  }
  
  return res;
}

//=======================================================================
//function : IsContext1d
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRep_DSFiller::IsContext1d(const TopoDS_Shape& aS) const 
{
  Standard_Boolean is1d = IsMadeOf1d(aS);
#ifdef OCCT_DEBUG
  if ( is1d ) cout<<"TopOpeBRep_DSFiller : 1d"<<endl;
#endif
  if ( !is1d ) return Standard_False;
  return Standard_True;
}

//=======================================================================
//function : Insert1d
//purpose  : 
//=======================================================================

void TopOpeBRep_DSFiller::Insert1d
(const TopoDS_Shape& aS1,const TopoDS_Shape& aS2,const TopoDS_Face& aF1,const TopoDS_Face& aF2,
 const Handle(TopOpeBRepDS_HDataStructure)& HDS,const Standard_Boolean orientFORWARD)
{
  if ( ! CheckInsert(aS1,aS2) ) return;
  
  TopoDS_Shape S1 = aS1; 
  TopoDS_Shape S2 = aS2; 
  if ( orientFORWARD ) {
    if ( S1.Orientation() == TopAbs_REVERSED ) S1.Orientation(TopAbs_FORWARD);
    if ( S2.Orientation() == TopAbs_REVERSED ) S2.Orientation(TopAbs_FORWARD);
  }
  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  
  BDS.AddShape(S1,1);
  BDS.AddShape(S2,2);
  
  myShapeIntersector.InitIntersection(S1,S2,aF1,aF2);
  for (;myShapeIntersector.MoreIntersection(); myShapeIntersector.NextIntersection()) {
    
    // The two intersecting GeomShapes gs1,gs2 and their types t1,t2
    const TopoDS_Shape& gs1 = myShapeIntersector.CurrentGeomShape(1);
    const TopoDS_Shape& gs2 = myShapeIntersector.CurrentGeomShape(2);
    TopAbs_ShapeEnum t1 = gs1.ShapeType(), t2 = gs2.ShapeType();
    
    if ((t1 == TopAbs_EDGE) && (t2 == TopAbs_EDGE)) {
      TopOpeBRep_EdgesIntersector& EE = 
	myShapeIntersector.ChangeEdgesIntersector();
      EE.Dimension(1);
      myEdgesFiller.Insert(gs1,gs2,EE,HDS);
    }
  }
  // update wires
  CompleteDS(HDS);
  
} // Insert


//=======================================================================
//function : CheckInsert
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRep_DSFiller::CheckInsert(const TopoDS_Shape& aS1,const TopoDS_Shape& aS2) const 
{
  if (aS1.IsEqual(aS2)) {
#ifdef OCCT_DEBUG
    cout<<"TopOpeBRep_DSFiller : CheckInsert : S1 == S2"<<endl;
#endif
    return Standard_False;
  }
  return Standard_True;
}

//=======================================================================
//function : ClearShapeSameDomain
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRep_DSFiller::ClearShapeSameDomain
(const TopoDS_Shape& aS1,const TopoDS_Shape& aS2,const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  TopOpeBRepDS_DataStructure& DS = HDS->ChangeDS();
  const Standard_Boolean b = Standard_False;
  if(!CheckInsert(aS1,aS2))
    return b;
  TopExp_Explorer exp1(aS1, TopAbs_FACE), exp2(aS2, TopAbs_FACE);
  for(; exp1.More(); exp1.Next()) {
    const TopoDS_Shape& Shape1 = exp1.Current();
    if(!HDS->HasShape(Shape1))
      if(HDS->HasSameDomain(Shape1)) {
	TopTools_ListOfShape& los =
	  DS.ChangeShapeSameDomain(Shape1); 
	los.Clear();
      }
  }
  for(; exp2.More(); exp2.Next()) {
    const TopoDS_Shape& Shape2 = exp2.Current();
    if(HDS->HasShape(Shape2))
      if(HDS->HasSameDomain(Shape2)) {
	TopTools_ListOfShape& los =
	  DS.ChangeShapeSameDomain(Shape2); 
	los.Clear();
      }
  }
  return Standard_True;
}


//=======================================================================
//function : ChangeShapeIntersector
//purpose  : 
//=======================================================================

TopOpeBRep_ShapeIntersector& TopOpeBRep_DSFiller::ChangeShapeIntersector()
{  return myShapeIntersector; }

//=======================================================================
//function : ChangeShapeIntersector2d
//purpose  : 
//=======================================================================

TopOpeBRep_ShapeIntersector2d& TopOpeBRep_DSFiller::ChangeShapeIntersector2d()
{  return myShapeIntersector2d; }


//=======================================================================
//function : ChangeFacesFiller
//purpose  : 
//=======================================================================

TopOpeBRep_FacesFiller& TopOpeBRep_DSFiller::ChangeFacesFiller()
{ return myFacesFiller; }


//=======================================================================
//function : ChangeEdgesFiller
//purpose  : 
//=======================================================================

TopOpeBRep_EdgesFiller& TopOpeBRep_DSFiller::ChangeEdgesFiller()
{ return myEdgesFiller; }


//=======================================================================
//function : ChangeFaceEdgeFiller
//purpose  : 
//=======================================================================

TopOpeBRep_FaceEdgeFiller& TopOpeBRep_DSFiller::ChangeFaceEdgeFiller()
{ return myFaceEdgeFiller; }
