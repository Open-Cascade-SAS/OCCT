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
#include <Standard_ProgramError.hxx>
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
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopOpeBRepDS_SurfaceIterator.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopOpeBRepTool_defineG.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>

#ifdef OCCT_DEBUG
Standard_EXPORT void debfctwes(const Standard_Integer /*i*/) {}
Standard_EXPORT void debfctwesmess(const Standard_Integer i,const TCollection_AsciiString& s = "")
{cout<<"+ + + debfctwes "<<s<<"F"<<i<<endl;debfctwes(i);}
extern void debaddpwes(const Standard_Integer iFOR,const TopAbs_State TB1,const Standard_Integer iEG,const TopAbs_Orientation neworiE,
                       const TopOpeBRepBuild_PBuilder& PB,const TopOpeBRepBuild_PWireEdgeSet& PWES,const TCollection_AsciiString& str1,const TCollection_AsciiString& str2);
#endif

Standard_Boolean TopOpeBRepBuild_FUN_aresamegeom(const TopoDS_Shape& S1,const TopoDS_Shape& S2);

#define M_IN(st )      (st == TopAbs_IN)
#define M_OUT(st)      (st == TopAbs_OUT)
#define M_FORWARD(st ) (st == TopAbs_FORWARD)
#define M_REVERSED(st) (st == TopAbs_REVERSED)
#define M_INTERNAL(st) (st == TopAbs_INTERNAL)
#define M_EXTERNAL(st) (st == TopAbs_EXTERNAL)

//=======================================================================
//function : GFillCurveTopologyWES
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::GFillCurveTopologyWES(const TopoDS_Shape& F1,const TopOpeBRepBuild_GTopo& G1,TopOpeBRepBuild_WireEdgeSet& WES)
{
  TopAbs_State TB1,TB2; G1.StatesON(TB1,TB2);
  TopAbs_ShapeEnum t1,t2,ShapeInterf; G1.Type(t1,t2); ShapeInterf = t1;
  
#ifdef OCCT_DEBUG
  Standard_Integer iF; Standard_Boolean tSPS = GtraceSPS(F1,iF);
  if(tSPS) GdumpSHASTA(iF,TB1,WES,"--- GFillCurveTopologyWES");
  if(tSPS) {cout<<" ShapeInterf ";TopAbs::Print(ShapeInterf,cout);cout<<endl;}
  if(tSPS) {debfctwesmess(iF);}
#endif
  
  TopOpeBRepDS_CurveIterator FCit(myDataStructure->FaceCurves(F1));
  myFaceReference = TopoDS::Face(F1);
  myFaceToFill = TopoDS::Face(F1);
  //modified by NIZHNY-MZV  Thu Feb 24 09:15:33 2000
  //sometimes by the problem of tolerances we have intersection
  //lines between SameDomain faces, but Same domain faces can not
  //have intersection lines other than by its original edges
  //so we skip it if we find that two SameDomain faces have
  //new intersection edge
  Standard_Boolean hsd = myDataStructure->HasSameDomain(F1);
  TopTools_IndexedMapOfShape aSDMap;
  if(hsd) {
    TopTools_ListIteratorOfListOfShape it = myDataStructure -> SameDomain(F1);
    for(; it.More(); it.Next()) {
      const TopoDS_Shape& SDF = it.Value();
      aSDMap.Add(SDF);
    }
  }
  //End modified by NIZHNY-MZV  Thu Feb 24 09:21:08 2000

  for (; FCit.More(); FCit.Next()) {
    if ( ShapeInterf != TopAbs_SHAPE ) {
      const Handle(TopOpeBRepDS_Interference)& I = FCit.Value();
      const TopOpeBRepDS_Transition& T = I->Transition();
      TopAbs_ShapeEnum shab = T.ShapeBefore(),shaa = T.ShapeAfter();
      if ( (shaa != ShapeInterf) || (shab != ShapeInterf) ) continue;
      //modified by NIZHNY-MZV  Thu Feb 24 09:14:31 2000

      Standard_Integer si = I -> Support();
      TopoDS_Shape SS = myDataStructure -> Shape(si);
      //see comment above
      if(aSDMap.Contains(SS))
	continue;
      //End modified by NIZHNY-MZV  Thu Feb 24 09:21:34 2000
    }
    GFillCurveTopologyWES(FCit,G1,WES);
  }

  return;
} // GFillCurveTopologyWES

//=======================================================================
//function : GFillCurveTopologyWES
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::GFillCurveTopologyWES(const TopOpeBRepDS_CurveIterator& FCit,const TopOpeBRepBuild_GTopo& G1,TopOpeBRepBuild_WireEdgeSet& WES) const 
{
  Standard_Boolean more = FCit.More();
  if (!more) return;

  TopAbs_State TB1,TB2; G1.StatesON(TB1,TB2);
  TopOpeBRepDS_Config Conf = G1.Config1();
  TopAbs_State TB = TB1;
  if ( Conf == TopOpeBRepDS_DIFFORIENTED ) { // -jyl980525
//    if      (TB1 == TopAbs_OUT) TB = TopAbs_IN;
//    else if (TB1 == TopAbs_IN ) TB = TopAbs_OUT;
  }

  TopoDS_Face& WESF = *((TopoDS_Face*)((void*)&WES.Face()));
  TopoDS_Face& FTF = *((TopoDS_Face*)((void*)&myFaceToFill));
#ifdef OCCT_DEBUG
//  Standard_Boolean FTFeqWESF = myFaceReference.IsEqual(WESF);
#endif

#ifdef OCCT_DEBUG
  Standard_Integer iWESF = myDataStructure->Shape(WESF);
  Standard_Integer iref = myDataStructure->Shape(myFaceReference);
  Standard_Integer ifil = myDataStructure->Shape(myFaceToFill);
#endif

  Standard_Boolean opeCut = Opec12() || Opec21();
  Standard_Boolean ComOfCut = opeCut && (TB1 == TB2)&& (TB1 == TopAbs_IN);

  const TopOpeBRepDS_Transition& T = FCit.Value()->Transition();
  TopAbs_Orientation neworiE = T.Orientation(TB);
  Standard_Boolean samegeom = TopOpeBRepBuild_FUN_aresamegeom(FTF,WESF);
  if (!samegeom) {
    neworiE = TopAbs::Complement(neworiE);
  }
  
#ifdef OCCT_DEBUG
  Standard_Boolean tSPS = GtraceSPS(iWESF);
  if(tSPS){
    cout<<"ifil : "<<ifil<<" iref : "<<iref<<" iwes : "<<iWESF<<endl;
    cout<<"face "<<ifil<<" is ";TopOpeBRepDS::Print(Conf,cout);cout<<endl;
    cout<<"ComOfCut "<<ComOfCut<<endl;
    debfctwesmess(iWESF);
  }
#endif
  
  if (ComOfCut) return;

  Standard_Integer iG = FCit.Current();
  const TopTools_ListOfShape& LnewE = NewEdges(iG);
  TopTools_ListIteratorOfListOfShape Iti(LnewE);
  for (; Iti.More(); Iti.Next()) {
    TopoDS_Shape EE = Iti.Value();
    TopoDS_Edge& E = TopoDS::Edge(EE);

    //modified by NIZHNY-MZV  Fri Mar 17 12:51:03 2000
    if(BRep_Tool::Degenerated(E))
      continue;

//    TopAbs_Orientation neworiE = FCit.Orientation(TB);
    E.Orientation(neworiE);

    const Handle(Geom2d_Curve)& PC = FCit.PCurve();

    Standard_Boolean EhasPConFTF =
     
                      FC2D_HasCurveOnSurface(E,FTF);
    //modified by NIZHNY-MZV  Mon Mar 27 15:24:39 2000
    if(!EhasPConFTF)
      myBuildTool.PCurve(FTF,E,PC);
    
    Standard_Boolean EhasPConWESF = FC2D_HasCurveOnSurface(E,WESF);
    
    if ( !EhasPConWESF) {
//      Standard_Real tolE = BRep_Tool::Tolerance(E);
      Standard_Real f2,l2,tolpc; Handle(Geom2d_Curve) C2D;
      C2D = FC2D_CurveOnSurface(E,WESF,f2,l2,tolpc);
      if (C2D.IsNull()) throw Standard_ProgramError("GFillCurveTopologyWES");
#ifdef OCCT_DEBUG
//      Standard_Real tol = Max(tolE,tolpc);
#endif
      
      myBuildTool.PCurve(WESF,E,C2D);

#ifdef OCCT_DEBUG
      EhasPConWESF = FC2D_HasCurveOnSurface(E,WESF);
      if (!EhasPConWESF) cout<<"TopOpeBRepBuild_Builder::GFillCurveTopologyWES : Null PCurve on F"<<iWESF<<endl;
#endif
    }

#ifdef OCCT_DEBUG
    if (tSPS) debaddpwes(iWESF,TB,iG,neworiE,(TopOpeBRepBuild_Builder* const)this,&WES,"GFillCurveTopology " ,"WES+ EofC ");
#endif

    WES.AddStartElement(E);
  }
  
} // GFillCurveTopologyWES
