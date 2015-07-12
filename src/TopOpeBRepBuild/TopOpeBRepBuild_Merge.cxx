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
#include <BRep_Tool.hxx>
#include <BRepCheck.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>
#include <BRepCheck_Result.hxx>
#include <BRepCheck_Wire.hxx>
#include <gp_Pnt.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
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
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_ListOfShapeOn1State.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopOpeBRepDS_SurfaceIterator.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

//#include <DBRep.hxx>
#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepBuild_GettraceSPS();
#endif
//------------
//static Standard_Integer ifvNbFace = 0;
//static char *name = "                 ";
//-------------
static void CorrectEdgeOrientation(TopoDS_Shape& aWire)
{

  TopTools_ListOfShape anEdgeList, anAuxList, aTrueEdgeList;
  BRep_Builder BB;
  TopoDS_Vertex vf, vl, v1f, v1l;
  Standard_Boolean append = Standard_True;

  TopoDS_Iterator tdi(aWire, Standard_False, Standard_False);
  for(; tdi.More(); tdi.Next()) anEdgeList.Append(tdi.Value());

  Standard_Integer n = anEdgeList.Extent();
  if(n <= 1) return;

  TopTools_ListIteratorOfListOfShape anIt(anEdgeList);

  TopoDS_Shape anCurEdge = anIt.Value();
  TopExp::Vertices(TopoDS::Edge(anCurEdge), vf, vl, Standard_True);
  aTrueEdgeList.Append(anCurEdge);
  anIt.Next();

  while(n > 0 && append) {

    for(; anIt.More(); anIt.Next()) {
      append = Standard_False;
      anCurEdge =  anIt.Value();
      TopExp::Vertices(TopoDS::Edge(anCurEdge), v1f, v1l, Standard_True);
      if(v1f.IsSame(vl)) {
	aTrueEdgeList.Append(anCurEdge);
	vl = v1l;
	append = Standard_True;
	continue;
      }
      if(v1l.IsSame(vf)) {
	aTrueEdgeList.Append(anCurEdge);
	vf = v1f;
	append = Standard_True;
	continue;
      }

      if(v1l.IsSame(vl)) {
	TopoDS_Shape anRevEdge = anCurEdge.Reversed();
	aTrueEdgeList.Append(anRevEdge);
	vl = v1f;
	append = Standard_True;
	continue;
      }
      if(v1f.IsSame(vf)) {
	TopoDS_Shape anRevEdge = anCurEdge.Reversed();
	aTrueEdgeList.Append(anRevEdge);
	vf = v1l;
	append = Standard_True;
	continue;
      }
	
      anAuxList.Append(anCurEdge);
    }

    anEdgeList.Assign(anAuxList);
    anAuxList.Clear(); //something wrong in Assign when list contains 1 element.
    n = anEdgeList.Extent();
    anIt.Initialize(anEdgeList);
  }

  if(n > 0) aTrueEdgeList.Append(anEdgeList);

  aWire.Nullify();
  BB.MakeWire(TopoDS::Wire(aWire));
  anIt.Initialize(aTrueEdgeList);
  for(; anIt.More(); anIt.Next()) BB.Add(aWire, anIt.Value());

}



static void CorrectUnclosedWire(TopoDS_Shape& aWire)
{
//  cout << "-------CorrectUnclosedWire" << endl;
  BRep_Builder BB;
  TopoDS_Iterator tdi(aWire, Standard_False, Standard_False);
  Standard_Integer nbe = 0;
  for(; tdi.More(); tdi.Next()) {
    nbe++;
    const TopoDS_Shape& ed = tdi.Value();
    Standard_Integer nbv = 0;
    TopoDS_Iterator tdie(ed, Standard_False, Standard_False);
    for(; tdie.More(); tdie.Next()) {
      nbv++;
    }
//    cout << "Edge " << nbe << " : " << nbv << endl;
    if(nbv <= 1) {
//      cout << "Remove bad edge" << endl;
      BB.Remove(aWire, ed);
    }
  }
    
  TopTools_IndexedDataMapOfShapeListOfShape VElists;
  VElists.Clear();
  TopExp::MapShapesAndAncestors(aWire, TopAbs_VERTEX, TopAbs_EDGE, VElists);
  Standard_Integer nbVer = VElists.Extent(), i;

  for(i = 1; i <= nbVer; i++) {
    const TopTools_ListOfShape& Elist = VElists.FindFromIndex(i);
    if(Elist.Extent() == 1) {
      TopoDS_Shape anEdge = Elist.First();
//      cout << "Remove redundant edge" << endl;
      BB.Remove(aWire, anEdge);
    }
  }
}
      
//=======================================================================
//function : MergeShapes
//purpose  : 
//=======================================================================


void TopOpeBRepBuild_Builder::MergeShapes(const TopoDS_Shape& S1,const TopAbs_State ToBuild1,const TopoDS_Shape& S2,const TopAbs_State ToBuild2)
{
  Standard_Boolean lesmemes = S1.IsEqual(S2);
  if (lesmemes) {
#ifdef OCCT_DEBUG
    cout<<"TopOpeBRepBuild : S1 == S2"<<endl;
#endif
    return;
  }

#ifdef OCCT_DEBUG
  GdumpSHASETreset();
#endif

  myState1 = ToBuild1;
  myState2 = ToBuild2;
  myShape1 = S1;
  myShape2 = S2;
  Standard_Boolean S1null = S1.IsNull();
  Standard_Boolean S2null = S2.IsNull();
  
  MapShapes(S1,S2);
  SplitSectionEdges();
  
  //======================== debut KPart
  if (IsKPart()) {
    MergeKPart();
    ClearMaps();
    return;
  }
  //======================== fin KPart
  
  Standard_Boolean RevOri1 = Reverse(ToBuild1,ToBuild2);
  Standard_Boolean RevOri2 = Reverse(ToBuild2,ToBuild1);
  
  // Create a face set <SFS> connected by edges
  // -----------------------------------------
  TopOpeBRepBuild_ShellFaceSet SFS;
  
#ifdef OCCT_DEBUG
  SFS.DEBNumber(GdumpSHASETindex());
#endif
  
  // NYI : SplitCompound appele par SplitShape
  
  TopOpeBRepTool_ShapeExplorer ex1;

  TopAbs_ShapeEnum t1=TopAbs_COMPOUND,tex1=TopAbs_COMPOUND;

  if ( ! S1null ) {
    t1 = tex1 = TopType(S1);
    if ( t1 == TopAbs_COMPOUND ) {
      tex1 = TopAbs_SOLID;  ex1.Init(S1,tex1);
      if ( ! ex1.More() ) {
	tex1 = TopAbs_SHELL; ex1.Init(S1,tex1);
	if ( ! ex1.More() ) {
	  tex1 = TopAbs_FACE; ex1.Init(S1,tex1);
	  if( ! ex1.More() ) { 
	    tex1 = TopAbs_EDGE; ex1.Init(S1,tex1);
	  }
	}
      }
    }
    else if (t1 == TopAbs_WIRE) {
      tex1 = TopAbs_EDGE; ex1.Init(S1,tex1);
    }
    else ex1.Init(S1,tex1);
    SplitShapes(ex1,ToBuild1,ToBuild2,SFS,RevOri1);
  }
  
  TopOpeBRepTool_ShapeExplorer ex2;

  TopAbs_ShapeEnum t2=TopAbs_COMPOUND,tex2=TopAbs_COMPOUND;

  if ( ! S2null ) {
    t2 = tex2 = TopType(S2);
    if ( t2 == TopAbs_COMPOUND ) {
      tex2 = TopAbs_SOLID;  ex2.Init(S2,tex2);
      if ( ! ex2.More() ) {
	tex2 = TopAbs_SHELL; ex2.Init(S2,tex2);
	if ( ! ex2.More() ) {
	  tex2 = TopAbs_FACE; ex2.Init(S2,tex2);
	  if( ! ex2.More() ) { 
	    tex2 = TopAbs_EDGE; ex2.Init(S2,tex2);
	  }
	}
      }
    }
    else if (t2 == TopAbs_WIRE) {
      tex2 = TopAbs_EDGE; ex2.Init(S2,tex2);
    }
    else ex2.Init(S2,tex2);
    SplitShapes(ex2,ToBuild2,ToBuild1,SFS,RevOri2);
  }
  
  // S1 or S2 = COMPOUND : connect them the list of merged shapes build
  // on the first subshape.
  
  if ( ! S1null ) {
    if ( t1 == TopAbs_COMPOUND ) {
      TopTools_ListOfShape& L1 = ChangeMerged(S1,ToBuild1);
      ex1.Init(S1,tex1);
      if ( ex1.More() ) {
	const TopoDS_Shape& SS1 = ex1.Current();
	TopTools_ListOfShape& LSS1 = ChangeMerged(SS1,ToBuild1);
	L1 = LSS1;
      }
    }
  }
  
  if ( ! S2null ) {
    if ( t2 == TopAbs_COMPOUND ) {
      TopTools_ListOfShape& L2 = ChangeMerged(S2,ToBuild2);
      ex2.Init(S2,tex2);
      if ( ex2.More() ) {
	const TopoDS_Shape& SS2 = ex2.Current();
	TopTools_ListOfShape& LSS2 = ChangeMerged(SS2,ToBuild2);
	L2 = LSS2;
      }
    }
  }
  
  ClearMaps();
  
} // MergeShapes


//=======================================================================
//function : Classify
//purpose  : LocOpe use
//=======================================================================

Standard_Boolean TopOpeBRepBuild_Builder::Classify() const 
{
  return myClassifyVal;
}

//=======================================================================
//function : ChangeClassify
//purpose  : LocOpe use
//=======================================================================

void TopOpeBRepBuild_Builder::ChangeClassify(const Standard_Boolean classify)
{
  myClassifyDef = Standard_True;
  myClassifyVal = classify;
}

//=======================================================================
//function : MergeSolids
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Builder::MergeSolids(const TopoDS_Shape& S1,const TopAbs_State ToBuild1,const TopoDS_Shape& S2,const TopAbs_State ToBuild2)
{
  MergeShapes(S1,ToBuild1,S2,ToBuild2);
} // MergeSolids


//=======================================================================
//function : MergeSolid
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Builder::MergeSolid(const TopoDS_Shape& S,const TopAbs_State ToBuild)
{
  TopoDS_Shape Snull;
  MergeShapes(S,ToBuild,Snull,ToBuild);
} // MergeSolid


//=======================================================================
//function : MakeSolids
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Builder::MakeSolids(TopOpeBRepBuild_SolidBuilder& SOBU,TopTools_ListOfShape& L)
{
  TopoDS_Shape newSolid;
  TopoDS_Shape newShell;
  for (SOBU.InitSolid(); SOBU.MoreSolid(); SOBU.NextSolid()) {
    myBuildTool.MakeSolid(newSolid);
    for (SOBU.InitShell(); SOBU.MoreShell(); SOBU.NextShell()) {
      Standard_Boolean isold = SOBU.IsOldShell();
      if (isold) newShell = SOBU.OldShell();
      else {
	myBuildTool.MakeShell(newShell);
	for (SOBU.InitFace(); SOBU.MoreFace(); SOBU.NextFace()) {
	  TopoDS_Shape F = SOBU.Face();
	  myBuildTool.AddShellFace(newShell,F);
	}
      }
      myBuildTool.Closed(newShell,Standard_True); // NYI : check exact du caractere closed du shell
      myBuildTool.AddSolidShell(newSolid,newShell);
    }
    L.Append(newSolid);
  }
}
//=======================================================================
//function : MakeShells
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Builder::MakeShells(TopOpeBRepBuild_SolidBuilder& SOBU,TopTools_ListOfShape& L)
{
  TopoDS_Shape newShell;
  for (SOBU.InitShell(); SOBU.MoreShell(); SOBU.NextShell()) {
    Standard_Boolean isold = SOBU.IsOldShell();
    if (isold) newShell = SOBU.OldShell();
    else {
      myBuildTool.MakeShell(newShell);
      for (SOBU.InitFace(); SOBU.MoreFace(); SOBU.NextFace()) {
	TopoDS_Shape F = SOBU.Face();
	myBuildTool.AddShellFace(newShell,F);
      }
    }
    L.Append(newShell);
  }
}



//=======================================================================
//function : MakeFaces
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Builder::MakeFaces(const TopoDS_Shape& aFace,TopOpeBRepBuild_FaceBuilder& FABU,TopTools_ListOfShape& L)
{
#ifdef OCCT_DEBUG
  Standard_Integer iF = 0; Standard_Boolean tSPS = GtraceSPS(aFace,iF);
  if(tSPS){GdumpFABU(FABU);}
#endif
  Standard_Boolean hashds = (!myDataStructure.IsNull());
  TopoDS_Shape newFace;
  TopoDS_Shape newWire;

  for (FABU.InitFace(); FABU.MoreFace(); FABU.NextFace()) {
    myBuildTool.CopyFace(aFace,newFace);
    Standard_Boolean hns = Standard_False;
    if (hashds) {
      const TopOpeBRepDS_DataStructure& BDS = myDataStructure->DS();
      hns = BDS.HasNewSurface(aFace);
      if (hns) {
	const Handle(Geom_Surface)& SU = BDS.NewSurface(aFace);
	myBuildTool.UpdateSurface(newFace,SU);
      }
    }


    for (FABU.InitWire(); FABU.MoreWire(); FABU.NextWire()) {
      Standard_Boolean isold = FABU.IsOldWire();
      if (isold) newWire = FABU.OldWire();
      else {
	myBuildTool.MakeWire(newWire);
	for(FABU.InitEdge(); FABU.MoreEdge(); FABU.NextEdge()) {
	  TopoDS_Shape E = FABU.Edge();
	  if (hns) myBuildTool.UpdateSurface(E,aFace,newFace);
	  myBuildTool.AddWireEdge(newWire,E);
	}
      }
      //----------- IFV
      if(!isold) {
	BRepCheck_Analyzer bca(newWire, Standard_False);
	if (!bca.IsValid()) {
	  newWire.Free(Standard_True);
	  CorrectUnclosedWire(newWire);
	  const Handle(BRepCheck_Result)& bcr = bca.Result(newWire);
	  BRepCheck_ListIteratorOfListOfStatus itl(bcr->Status());
	  for(; itl.More(); itl.Next() ) {
	    if(itl.Value() == BRepCheck_BadOrientationOfSubshape) {
	      CorrectEdgeOrientation(newWire);
	      break;
	    }
	  }
	}
      }
      myBuildTool.Closed(newWire,Standard_True); // NYI : check exact du caractere closed du wire
      myBuildTool.AddFaceWire(newFace,newWire);
    }


    L.Append(newFace);
  }
}


//=======================================================================
//function : MakeEdges
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Builder::MakeEdges(const TopoDS_Shape& anEdge,TopOpeBRepBuild_EdgeBuilder& EDBU,TopTools_ListOfShape& L)
{
#ifdef OCCT_DEBUG
  Standard_Integer iE; Standard_Boolean tSPS = GtraceSPS(anEdge,iE);
  Standard_Integer ne = 0;
#endif
  
  Standard_Integer nvertex = 0;
  for (TopOpeBRepTool_ShapeExplorer ex(anEdge,TopAbs_VERTEX); ex.More(); ex.Next()) nvertex++;
  
  TopoDS_Shape newEdge;
  for (EDBU.InitEdge(); EDBU.MoreEdge(); EDBU.NextEdge()) {
    
    // 1 vertex sur edge courante => suppression edge
    Standard_Integer nloop = 0;
    for (EDBU.InitVertex(); EDBU.MoreVertex(); EDBU.NextVertex()) nloop++; 
    if ( nloop <= 1 ) continue;
    
    myBuildTool.CopyEdge(anEdge,newEdge);
    
    Standard_Boolean hasvertex = Standard_False;
    for (EDBU.InitVertex(); EDBU.MoreVertex(); EDBU.NextVertex()) {
      TopoDS_Shape V = EDBU.Vertex();
      TopAbs_Orientation Vori = V.Orientation();
      
      Standard_Boolean hassd = myDataStructure->HasSameDomain(V);
      if (hassd) {
	// on prend le vertex reference de V
	Standard_Integer iref = myDataStructure->SameDomainReference(V);
	V = myDataStructure->Shape(iref);
	V.Orientation(Vori);
      }
      
      TopAbs_Orientation oriV = V.Orientation();
      if ( oriV != TopAbs_EXTERNAL ) {
	// betonnage
	Standard_Boolean equafound = Standard_False;
	TopExp_Explorer exE(newEdge,TopAbs_VERTEX);
	for (; exE.More(); exE.Next() ) {
	  const TopoDS_Shape& VE = exE.Current();
	  TopAbs_Orientation oriVE = VE.Orientation();
	  if ( V.IsEqual(VE) ) {
	    equafound = Standard_True;
	    break;
	  }
	  else if (oriVE == TopAbs_FORWARD || oriVE == TopAbs_REVERSED) {
	    if (oriV == oriVE) {
	      equafound = Standard_True;
	      break;
	    }
	  }
	  else if (oriVE == TopAbs_INTERNAL || oriVE == TopAbs_EXTERNAL) {
	    Standard_Real parV = EDBU.Parameter();
	    Standard_Real parVE = BRep_Tool::Parameter(TopoDS::Vertex(VE),
					     TopoDS::Edge(newEdge));
	    if ( parV == parVE ) {
	      equafound = Standard_True;
	      break;
	    }
	  }
	}
	if ( !equafound ) {
	  hasvertex = Standard_True;
	  Standard_Real parV = EDBU.Parameter();
	  myBuildTool.AddEdgeVertex(newEdge,V);
	  myBuildTool.Parameter(newEdge,V,parV);
	}
	
      }
    } // loop on vertices of new edge newEdge
    
#ifdef OCCT_DEBUG
    if(tSPS){cout<<endl;}
    if(tSPS){cout<<"V of new edge "<<++ne<<endl;}
    if(tSPS){GdumpEDG(newEdge);} 
#endif
    
    if (hasvertex) L.Append(newEdge);
  } // loop on EDBU edges
} // MakeEdges


//=======================================================================
//function : IsMerged
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_Builder::IsMerged(const TopoDS_Shape& S,const TopAbs_State ToBuild) const
{
  const TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State* p = NULL;
  if      ( ToBuild == TopAbs_OUT ) p = &myMergedOUT;
  else if ( ToBuild == TopAbs_IN  ) p = &myMergedIN;
  else if ( ToBuild == TopAbs_ON  ) p = &myMergedON;
  if ( p == NULL ) return Standard_False;

  Standard_Boolean notbound = ! (*p).IsBound(S); 
  if ( notbound ) {
    return Standard_False;
  }
  else {
    const TopTools_ListOfShape& L = Merged(S,ToBuild);
    Standard_Boolean isempty = L.IsEmpty();
    return (!isempty);
  }
} // IsMerged


//=======================================================================
//function : Merged
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& TopOpeBRepBuild_Builder::Merged(const TopoDS_Shape& S,const TopAbs_State ToBuild) const
{
  const TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State* p = NULL;
  if      ( ToBuild == TopAbs_OUT ) p = &myMergedOUT;
  else if ( ToBuild == TopAbs_IN  ) p = &myMergedIN;
  else if ( ToBuild == TopAbs_ON  ) p = &myMergedON;
  if ( p == NULL ) return myEmptyShapeList;

  if ( ! (*p).IsBound(S) ) {
    return myEmptyShapeList;
  }
  else {
    const TopTools_ListOfShape& L = (*p)(S).ListOnState();
    return L;
  }
} // Merged


//=======================================================================
//function : ChangeMerged
//purpose  : 
//=======================================================================

TopTools_ListOfShape& TopOpeBRepBuild_Builder::ChangeMerged(const TopoDS_Shape& S, const TopAbs_State ToBuild)
{
  TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State* p = NULL;
  if      ( ToBuild == TopAbs_OUT ) p = &myMergedOUT;
  else if ( ToBuild == TopAbs_IN  ) p = &myMergedIN;
  else if ( ToBuild == TopAbs_ON  ) p = &myMergedON;
  if ( p == NULL ) return myEmptyShapeList;

  if ( ! (*p).IsBound(S) ) {
    TopOpeBRepDS_ListOfShapeOn1State thelist;
    (*p).Bind(S, thelist);
  }
  TopTools_ListOfShape& L = (*p)(S).ChangeListOnState();
  return L;
} // ChangeMerged

//=======================================================================
//function : MergeEdges
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Builder::MergeEdges(const TopTools_ListOfShape& ,//L1,
		      const TopAbs_State ,//ToBuild1,
		      const TopTools_ListOfShape& ,//L2,
		      const TopAbs_State ,//ToBuild2,
		      const Standard_Boolean ,//Keepon1,
		      const Standard_Boolean ,//Keepon2,
		      const Standard_Boolean )//Keepon12)
{
} // MergeEdges

//=======================================================================
//function : MergeFaces
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Builder::MergeFaces(const TopTools_ListOfShape& , //S1,
		      const TopAbs_State , //ToBuild1,
		      const TopTools_ListOfShape& , //S2,
		      const TopAbs_State , //ToBuild2
		      const Standard_Boolean ,//onA,
		      const Standard_Boolean ,//onB,
		      const Standard_Boolean )//onAB)
{
}
