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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_ProgramError.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopOpeBRepBuild_Builder.hxx>
#include <TopOpeBRepBuild_EdgeBuilder.hxx>
#include <TopOpeBRepBuild_FaceBuilder.hxx>
#include <TopOpeBRepBuild_GTopo.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepBuild_PaveSet.hxx>
#include <TopOpeBRepBuild_ShapeSet.hxx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>
#include <TopOpeBRepBuild_SolidBuilder.hxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepBuild_WireToFace.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopOpeBRepDS_SurfaceIterator.hxx>
#include <TopOpeBRepTool.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepBuild_GetcontextNOREGUFA();
extern Standard_Boolean TopOpeBRepBuild_GetcontextREGUXPU();
extern Standard_Boolean TopOpeBRepBuild_GettraceSAVFREGU();
void debregufa(const Standard_Integer /*iF*/) {}
#endif

#ifdef DRAW
#include <DBRep.hxx>
#endif

#define M_FORWARD(O)  (O == TopAbs_FORWARD)
#define M_REVERSED(O) (O == TopAbs_REVERSED)

//=======================================================================
//function : RegularizeFaces
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::RegularizeFaces
(const TopoDS_Shape& FF,const TopTools_ListOfShape& lnewFace,TopTools_ListOfShape& LOF)
{
  LOF.Clear();
  myMemoSplit.Clear();

  TopTools_ListIteratorOfListOfShape itl(lnewFace);  
  for (;itl.More();itl.Next()) {
    const TopoDS_Shape& newFace = itl.Value();
    TopTools_ListOfShape newFaceLOF;
    RegularizeFace(FF,newFace,newFaceLOF);
#ifdef OCCT_DEBUG
//    Standard_Integer nnewFaceLOF = newFaceLOF.Extent(); // DEB
#endif
    LOF.Append(newFaceLOF);
  }
#ifdef OCCT_DEBUG
//  Standard_Integer nLOF = LOF.Extent(); // DEB
#endif

  Standard_Integer nr = myMemoSplit.Extent();
  if (nr == 0 ) return;

  // lfsdFF = faces SameDomain de FF
  TopTools_ListOfShape lfsdFF,lfsdFF1,lfsdFF2;
  GFindSamDom(FF,lfsdFF1,lfsdFF2);
  lfsdFF.Append(lfsdFF1);
  lfsdFF.Append(lfsdFF2);
  
  TopTools_ListIteratorOfListOfShape itlfsdFF(lfsdFF);
  for (; itlfsdFF.More(); itlfsdFF.Next()) {
    const TopoDS_Shape& fsdFF = itlfsdFF.Value();
    // au moins une arete de FF dont le Split() est lui meme Split()
    TopExp_Explorer x;
    for (x.Init(fsdFF,TopAbs_EDGE);x.More();x.Next()) {
//    for (TopExp_Explorer x(fsdFF,TopAbs_EDGE);x.More();x.Next()) {
      const TopoDS_Shape& e = x.Current();
#ifdef OCCT_DEBUG
//      Standard_Integer ie = myDataStructure->Shape(e); //DEB
//      Standard_Boolean issect = myDataStructure->DS().IsSectionEdge(TopoDS::Edge(e));
#endif

      Standard_Integer ranke = GShapeRank(e);
      TopAbs_State staeope = (ranke==1) ? myState1 : myState2;
      
      for (Standard_Integer iiista = 1; iiista <= 2; iiista++ ) {
	
	TopAbs_State stae = staeope;
	if (iiista == 2) stae = TopAbs_ON;
	
	Standard_Boolean issplite = IsSplit(e,stae);
	if (!issplite) continue;
	
	TopTools_ListOfShape& lspe = ChangeSplit(e,stae);
#ifdef OCCT_DEBUG
//	Standard_Integer nlspe = lspe.Extent(); // DEB
#endif
	TopTools_ListOfShape newlspe;
	for (TopTools_ListIteratorOfListOfShape itl1(lspe);itl1.More();itl1.Next()) {
	  const TopoDS_Shape& esp = itl1.Value();
	  Standard_Boolean espmemo = myMemoSplit.Contains(esp);
	  if (!espmemo) newlspe.Append(esp);
	  else {
	    const TopTools_ListOfShape& lspesp = Splits(esp,stae);
	    GCopyList(lspesp,newlspe);
	  }
	}	
	lspe.Clear();
	GCopyList(newlspe,lspe);

      } // iiista
    } // explorer (fsdFF,TopAbs_EDGE)
  } // itlfsdFF.More()
} // RegularizeFaces

/*static void FUN_setInternal(TopoDS_Face& F)
{
  TopExp_Explorer ex(F,TopAbs_EDGE);
  TopTools_MapOfShape meF,meR,meI;
  for (; ex.More(); ex.Next()){
    const TopoDS_Edge& E = TopoDS::Edge(ex.Current());
    TopAbs_Orientation oE = E.Orientation();
    Standard_Boolean isclo = BRep_Tool::IsClosed(E,F); // E has 2d rep on F
    if (isclo) continue; 
    Standard_Boolean isb = Standard_False; // the edge is FOR + REV in F
    if      (M_FORWARD(oE))  {meF.Add(E); isb = meR.Contains(E);}
    else if (M_REVERSED(oE)) {meR.Add(E); isb = meF.Contains(E);}
    if (isb) meI.Add(E.Oriented(TopAbs_INTERNAL));
  }  

  BRep_Builder BB;
  TopTools_MapIteratorOfMapOfShape it(meI);
  for (; it.More(); it.Next()){
    const TopoDS_Edge& E = TopoDS::Edge(it.Key());
    BB.Remove(F,E.Oriented(TopAbs_FORWARD));
    BB.Remove(F,E.Oriented(TopAbs_REVERSED));
    BB.Add(F,E);
  }
}*/

//=======================================================================
//function : RegularizeFace
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::RegularizeFace
(const TopoDS_Shape& FF,const TopoDS_Shape& anewFace,TopTools_ListOfShape& LOF)
{
  LOF.Clear();
  const TopoDS_Face& newFace = TopoDS::Face(anewFace);
  Standard_Boolean toregu = Standard_True;
  Standard_Boolean usewtof = Standard_True;
  
#ifdef OCCT_DEBUG
  Standard_Integer iF;Standard_Boolean tSPSFF=GtraceSPS(FF,iF);
//  Standard_Boolean savfregu = TopOpeBRepBuild_GettraceSAVFREGU();
  if (TopOpeBRepBuild_GetcontextNOREGUFA()) toregu = Standard_False;
  if (TopOpeBRepBuild_GetcontextREGUXPU()) usewtof = Standard_False;
  if (tSPSFF) debregufa(iF);
#endif

  // If the same edge appears FOR+REV in the resulting face and
  // whereas it's not a closing edge, set it as INTERNAL instead.
  // FRA60275(iF=4) + PRO16297 
//  FUN_setInternal(newFace);

  if (!toregu) {
    LOF.Append(newFace);
    return;
  }
  
  TopTools_DataMapOfShapeListOfShape ownw; // OldWires --> NewWires;
  Standard_Boolean rw = Standard_False;
  Standard_Boolean rf = Standard_False;
  myESplits.Clear();
  
  rw = TopOpeBRepTool::RegularizeWires(newFace,ownw,myESplits);
  
  if ( !rw ) {
    LOF.Append(newFace);
    return;
  }      
  
  TopTools_ListOfShape newfaces;
  if (usewtof) {
    TopOpeBRepBuild_WireToFace wtof;
    TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itownw(ownw);
    for (; itownw.More(); itownw.Next()) {
      const TopTools_ListOfShape& lw = itownw.Value();

      // xpu200798 : cto902D4 f14ou
      // recall ownw = {(ow = old wire, lnw = {list of new wires descendant of old wire}}
      // if lnw is empty, then ow is kept unchanged.
      Standard_Boolean kept = lw.IsEmpty();
      if (kept) {
	const TopoDS_Wire& ow = TopoDS::Wire(itownw.Key());
	wtof.AddWire(ow);
      }
      for(TopTools_ListIteratorOfListOfShape iw(lw);iw.More();iw.Next()) {
	const TopoDS_Wire& w = TopoDS::Wire(iw.Value());
	wtof.AddWire(w);
      }
    }
    wtof.MakeFaces(newFace,newfaces);
#ifdef OCCT_DEBUG
//    Standard_Integer nnewfaces = newfaces.Extent(); // DEB
#endif
    rf = (newfaces.Extent() != 0);
  }
  else {
    rf = TopOpeBRepTool::RegularizeFace(newFace,ownw,newfaces);
  }
  
  if (!rf) {
    LOF.Append(newFace);
    return;
  }
  
#ifdef OCCT_DEBUG
  if (tSPSFF) { cout<<"RegularizeFace "<<iF<<endl; debregufa(iF); }
#endif
  
  // LOF = nouvelles faces regularisees de newFace
  TopTools_ListIteratorOfListOfShape itlnf(newfaces);
  for (; itlnf.More(); itlnf.Next()) 
    LOF.Append(TopoDS::Face(itlnf.Value()));
  
  // mise a jour des aretes decoupees
  // Edge(FF) = {E}, E-->Split(E) = {E'}, E'-->myESplits(E') = {E''}

  TopTools_MapOfShape menf; // menf = aretes de newFace
  TopExp_Explorer x;
  for (x.Init(newFace,TopAbs_EDGE);x.More();x.Next()) {
    const TopoDS_Shape& E = x.Current();
    menf.Add(E);
  }
  
  // lfsdFF = faces SameDomain de FF
  TopTools_ListOfShape lfsdFF,lfsdFF1,lfsdFF2;
  GFindSamDom(FF,lfsdFF1,lfsdFF2);
  lfsdFF.Append(lfsdFF1);
  lfsdFF.Append(lfsdFF2);
  
  TopTools_ListIteratorOfListOfShape itlfsdFF(lfsdFF);
  for (; itlfsdFF.More(); itlfsdFF.Next()) {
    const TopoDS_Shape& fsdFF = itlfsdFF.Value();

#ifdef OCCT_DEBUG
    Standard_Integer ifsdFF=0;Standard_Boolean tSPSfsdFF=GtraceSPS(fsdFF,ifsdFF);
    if (tSPSfsdFF) debregufa(ifsdFF);
#endif    

    Standard_Integer rankfsdFF = GShapeRank(fsdFF);
    TopAbs_State stafsdFF = (rankfsdFF == 1) ? myState1 : myState2;
#ifdef OCCT_DEBUG
//    Standard_Boolean issplitfsdFF = IsSplit(fsdFF,stafsdFF);
#endif

/*#ifdef OCCT_DEBUG
    const TopTools_ListOfShape& lspfsdFF = Splits(fsdFF,stafsdFF);
    Standard_Integer nlspfsdFF = lspfsdFF.Extent();
#endif*/
    
    // iteration sur les aretes de fsdFF
    for (x.Init(fsdFF,TopAbs_EDGE);x.More();x.Next()) {

      //fsdFFe : 1 edge de fsdFF = 1 face SameDomain de FF
      const TopoDS_Shape& fsdFFe = x.Current(); 

#ifdef OCCT_DEBUG
      Standard_Integer ifsdFFe = 0;Standard_Boolean tSPSfsdFFe=GtraceSPS(fsdFFe,ifsdFFe);
      if (tSPSfsdFFe) debregufa(ifsdFFe);
#endif    
      
      // a priori, on ne peut avoir plus de deux etats splites
      // sur l'arete , soit (IN + ON) , soit (OUT + ON) 
      for (Standard_Integer iiista = 1; iiista <= 2; iiista++ ) {
	TopAbs_State stafsdFFe = stafsdFF;
	if (iiista == 2) stafsdFFe = TopAbs_ON;
	
	TopTools_ListOfShape& lspfsdFFe = ChangeSplit(fsdFFe,stafsdFFe);
#ifdef OCCT_DEBUG
//	Standard_Boolean issplitfsdFFe = IsSplit(fsdFFe,stafsdFFe);
//	Standard_Integer nlspfsdFFe = lspfsdFFe.Extent();
#endif    
	  
	for (TopTools_ListIteratorOfListOfShape it(lspfsdFFe);it.More();it.Next()) {
	  
	  // fsdFFe (Cf supra E) a ete splittee, espfdsFFe = arete splittee de fsdFFe
	  
	  const TopoDS_Shape& espfsdFFe = it.Value();
	  Standard_Boolean inmenf = menf.Contains(espfsdFFe);
	  if (!inmenf) continue;
	  
	  // fsdFFe (Cf supra E) a ete splittee, espfdsFFe = arete splittee de fsdFFe
	  // espfsdFFe est une arete de Split(fsdFFe) ET figure dans newFace (Cf supra E')
	  
	  Standard_Boolean resplitloc = myESplits.IsBound(espfsdFFe);
	  if (resplitloc) {
	    
	    // fsdFFe (Cf supra E) a ete splittee, espfdsFFe = arete splittee de fsdFFe
	    // espfsdFFe est une arete de Split(fsdFFe) ET figure dans newFace (Cf supra E')
	    // espfsdFFe de newFace a ete redecoupee par RegularizeWires
	    
	    // son decoupage lresplit est stocke dans la DS du Builder
	    const TopTools_ListOfShape& lresplit = myESplits.Find(espfsdFFe); //Cf supra E''
	    
	    // on memorise que espfsdFFe est redecoupee ...
	    myMemoSplit.Add(espfsdFFe);
	    
	    // on stocke le nouveau decoupage de espfsdFFe dans la DS du builder ...
	    TopTools_ListOfShape& lsp = ChangeSplit(espfsdFFe,stafsdFFe);  
	    GCopyList(lresplit,lsp);
	  }
	} // it.More
      } // iiista
    } // explore(fsdFF,TopAbs_EDGE)
  } // itlfsdFF.More()

#ifdef DRAW
  if (tSPSFF) debregufa(iF);
  if (tSPSFF && savfregu) {
    TCollection_AsciiString str("fregu"); str = str + iF;
    DBRep::Set(str.ToCString(),newFace);
    cout<<"newFace "<<str<<" built on face "<<iF<<" saved"<<endl;
  }
#endif
  
} // RegularizeFace
