// Created on: 1993-06-14
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


#include <Standard_NoSuchObject.hxx>
#include <Standard_ProgramError.hxx>
#include <Standard_Type.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_Builder.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_CurveExplorer.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_DataMapIteratorOfDataMapOfShapeListOfShapeOn1State.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_ListOfShapeOn1State.hxx>
#include <TopOpeBRepDS_ShapeShapeInterference.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TopOpeBRepBuild_HBuilder,Standard_Transient)

//=======================================================================
//function : TopOpeBRepBuild_HBuilder
//purpose  : 
//=======================================================================
TopOpeBRepBuild_HBuilder::TopOpeBRepBuild_HBuilder
(const TopOpeBRepDS_BuildTool& BT) 
: myBuilder(BT),
  myMakeEdgeAncestorIsDone(Standard_False),
  myMakeCurveAncestorIsDone(Standard_False),
  myMakePointAncestorIsDone(Standard_False)
{
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_HBuilder::Perform
(const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  myBuilder.Perform(HDS);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_HBuilder::Perform(const Handle(TopOpeBRepDS_HDataStructure)& HDS,const TopoDS_Shape& S1, const TopoDS_Shape& S2)
{
  myBuilder.Perform(HDS,S1,S2);
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_HBuilder::Clear()
{
  myBuilder.Clear();
  InitExtendedSectionDS();  
}

//=======================================================================
//function : DataStructure
//purpose  : 
//=======================================================================
Handle(TopOpeBRepDS_HDataStructure) TopOpeBRepBuild_HBuilder::DataStructure() const
{
  return myBuilder.DataStructure();
}

//=======================================================================
//function : ChangeBuildTool
//purpose  : 
//=======================================================================
TopOpeBRepDS_BuildTool& TopOpeBRepBuild_HBuilder::ChangeBuildTool() 
{
  TopOpeBRepDS_BuildTool& BT = myBuilder.ChangeBuildTool();
  return BT;
}

//=======================================================================
//function : BuildTool
//purpose  : 
//=======================================================================
const TopOpeBRepDS_BuildTool& TopOpeBRepBuild_HBuilder::BuildTool() const
{
  return myBuilder.BuildTool();
}

//=======================================================================
//function : MergeShapes
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_HBuilder::MergeShapes
(const TopoDS_Shape& S1, const TopAbs_State ToBuild1,const TopoDS_Shape& S2, const TopAbs_State ToBuild2)
{
  myBuilder.MergeShapes(S1,ToBuild1,S2,ToBuild2);
}


//=======================================================================
//function : MergeSolids
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_HBuilder::MergeSolids
(const TopoDS_Shape& S1, const TopAbs_State ToBuild1,const TopoDS_Shape& S2, const TopAbs_State ToBuild2)
{
  myBuilder.MergeSolids(S1,ToBuild1,S2,ToBuild2);
}


//=======================================================================
//function : MergeSolid
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_HBuilder::MergeSolid(const TopoDS_Shape& S, const TopAbs_State ToBuild)
{
  myBuilder.MergeSolid(S,ToBuild);
}


//=======================================================================
//function : IsSplit
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_HBuilder::IsSplit(const TopoDS_Shape& S, const TopAbs_State ToBuild) const
{
  Standard_Boolean res = myBuilder.IsSplit(S,ToBuild);
  return res;
}


//=======================================================================
//function : Splits
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& TopOpeBRepBuild_HBuilder::Splits(const TopoDS_Shape& S, const TopAbs_State ToBuild) const
{
  const TopTools_ListOfShape& L = myBuilder.Splits(S,ToBuild);
  return L;
}


//=======================================================================
//function : IsMerged
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_HBuilder::IsMerged(const TopoDS_Shape& S, const TopAbs_State ToBuild) const
{
  Standard_Boolean res = myBuilder.IsMerged(S,ToBuild);
  return res;
}


//=======================================================================
//function : Merged
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& TopOpeBRepBuild_HBuilder::Merged(const TopoDS_Shape& S, const TopAbs_State ToBuild) const
{
  const TopTools_ListOfShape& L = myBuilder.Merged(S,ToBuild);
  return L;
}


//=======================================================================
//function : NewVertex
//purpose  : 
//=======================================================================
const TopoDS_Shape& TopOpeBRepBuild_HBuilder::NewVertex(const Standard_Integer I) const
{
  const TopoDS_Shape& V = myBuilder.NewVertex(I);
  return V;
}


//=======================================================================
//function : NewEdges
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& TopOpeBRepBuild_HBuilder::NewEdges(const Standard_Integer I) const
{
  const TopTools_ListOfShape& L = myBuilder.NewEdges(I);
  return L;
}

//=======================================================================
//function : ChangeNewEdges
//purpose  : 
//=======================================================================
TopTools_ListOfShape& TopOpeBRepBuild_HBuilder::ChangeNewEdges(const Standard_Integer I)
{
  TopTools_ListOfShape& L = myBuilder.ChangeNewEdges(I);
  return L;
}


//=======================================================================
//function : NewFaces
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& TopOpeBRepBuild_HBuilder::NewFaces(const Standard_Integer I) const
{
  const TopTools_ListOfShape& L = myBuilder.NewFaces(I);
  return L;
}


//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& TopOpeBRepBuild_HBuilder::Section()
{
  const TopTools_ListOfShape& L = myBuilder.Section();
  return L;
}

static TopTools_ListOfShape* PLE = NULL;
static TopTools_ListIteratorOfListOfShape* PITLE = NULL;
//=======================================================================
//function : InitExtendedSectionDS
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_HBuilder::InitExtendedSectionDS(const Standard_Integer k)
{
  if      (k == 1) {
    myMakeCurveAncestorIsDone = Standard_False;
  }
  else if (k == 2) {
    myMakeEdgeAncestorIsDone = Standard_False;
  }
  else if (k == 3) {
    myMakeEdgeAncestorIsDone = Standard_False;
    myMakeCurveAncestorIsDone = Standard_False;
  }
  else return;
}

//=======================================================================
//function : InitSection
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_HBuilder::InitSection(const Standard_Integer k)
{
  if (PLE == NULL) PLE = new TopTools_ListOfShape();
  if (PITLE == NULL) PITLE = new TopTools_ListIteratorOfListOfShape();
  PLE->Clear(); PITLE->Initialize(*PLE);
  InitExtendedSectionDS(k);
  if      (k == 1) myBuilder.SectionCurves(*PLE);
  else if (k == 2) myBuilder.SectionEdges(*PLE);
  else if (k == 3) myBuilder.Section(*PLE);
  else return;
  PITLE->Initialize(*PLE);
}

//=======================================================================
//function : MoreSection
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_HBuilder::MoreSection() const
{
  if (PITLE == NULL) return Standard_False;
  Standard_Boolean b = PITLE->More();
  return b;
}

//=======================================================================
//function : NextSection
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_HBuilder::NextSection()
{
  if (PITLE == NULL) return;
  if (PITLE->More()) PITLE->Next();
}  

//=======================================================================
//function : CurrentSection
//purpose  : 
//=======================================================================

const TopoDS_Shape& TopOpeBRepBuild_HBuilder::CurrentSection() const
{
  if (PITLE == NULL) throw Standard_ProgramError("no more CurrentSection");
  if (!PITLE->More()) throw Standard_ProgramError("no more CurrentSection");
  return PITLE->Value();
}

//=======================================================================
//function : MakeEdgeAncestorMap
//purpose  : private
//=======================================================================

void TopOpeBRepBuild_HBuilder::MakeEdgeAncestorMap()
{
  if(myMakeEdgeAncestorIsDone)
    return;
  mySectEdgeDSEdges1.Clear();
  mySectEdgeDSEdges2.Clear();
  myDSEdgesDSFaces1.Clear();
  myDSEdgesDSFaces2.Clear();
  
  myMakeEdgeAncestorIsDone = Standard_True;
  TopTools_MapOfShape MF, ME;
  
  const TopOpeBRepDS_DataStructure& DS = DataStructure()->DS();  
//  Standard_Integer ns = DS.NbShapes(),is;
  Standard_Integer ns = DS.NbShapes();
//  Standard_Integer ei, fi, re, rf ,gi, ise;
  Standard_Integer ei, fi, re, rf ,gi;
  
  TopOpeBRepDS_DataMapIteratorOfDataMapOfShapeListOfShapeOn1State
    it(myBuilder.mySplitON);
  TopTools_ListIteratorOfListOfShape its;
  for(;it.More(); it.Next()) {
    const TopoDS_Shape& ShaSpl = it.Key();
    ei = DS.Shape(ShaSpl);
    re = DS.AncestorRank(ShaSpl);
    if(!re) continue;
    TopOpeBRepDS_ListOfShapeOn1State& losos1s = 
      (*(TopOpeBRepDS_ListOfShapeOn1State*)&it.Value());
    TopTools_ListOfShape& los = losos1s.ChangeListOnState();
    its.Initialize(los);
    if(re == 1)
      for(; its.More(); its.Next()) {
	TopoDS_Shape& SecEdg = its.Value();
	if(!mySectEdgeDSEdges1.IsBound(SecEdg))
	  mySectEdgeDSEdges1.Bind(SecEdg, ei);
      }
    else if(re == 2)
      for(; its.More(); its.Next()) {
	TopoDS_Shape& SecEdg = its.Value();
	if(!mySectEdgeDSEdges2.IsBound(SecEdg))
	  mySectEdgeDSEdges2.Bind(SecEdg,ei);
      }
  }
  
//  Standard_Boolean gb;
  TopOpeBRepDS_Kind gk;
  for (fi=1;fi<=ns;fi++) {
    const TopoDS_Shape& fds = DS.Shape(fi);
    if(fds.IsNull())
      continue;
    if (fds.ShapeType() != TopAbs_FACE) continue;
    TopOpeBRepDS_ListIteratorOfListOfInterference 
      it1(DS.ShapeInterferences(fds));
    for (;it1.More();it1.Next()) {
      Handle(TopOpeBRepDS_ShapeShapeInterference) SSI = 
	Handle(TopOpeBRepDS_ShapeShapeInterference)::DownCast(it1.Value());
      if (SSI.IsNull()) continue;
      gk = SSI->GeometryType();
      gi = SSI->Geometry();
      if (gk != TopOpeBRepDS_EDGE) continue;
      rf = DS.AncestorRank(fds);
      if(!rf) continue;
//      if (!MF.Contains(fds) ) {
//	MF.Add(fds);
      if(rf == 1) {
	if(!myDSEdgesDSFaces1.IsBound(gi)) {
          TColStd_ListOfInteger thelist;
	  myDSEdgesDSFaces1.Bind(gi,thelist );
        }
	myDSEdgesDSFaces1.ChangeFind(gi).Append(fi);
      }
      else if(rf == 2) {
	if(!myDSEdgesDSFaces2.IsBound(gi)){
          TColStd_ListOfInteger thelist1;
	  myDSEdgesDSFaces2.Bind(gi, thelist1);
        }
	myDSEdgesDSFaces2.ChangeFind(gi).Append(fi);
      }
      //      }
    }
  }
}

//=======================================================================
//function : GetDSEdgeFromSectEdge
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRepBuild_HBuilder::GetDSEdgeFromSectEdge(const TopoDS_Shape& E,const Standard_Integer rank)
{
  if(!myMakeEdgeAncestorIsDone)
    MakeEdgeAncestorMap();
  
  Standard_Integer i = 0;

  if(rank == 1) { 
    if(mySectEdgeDSEdges1.IsBound(E))
      i = mySectEdgeDSEdges1.Find(E);
  }

  if(rank == 2) { 
    if(mySectEdgeDSEdges2.IsBound(E))
      i = mySectEdgeDSEdges2.Find(E);
  }
  return i;
}

//=======================================================================
//function : GetDSFaceFromDSEdge
//purpose  : 
//=======================================================================
TColStd_ListOfInteger& TopOpeBRepBuild_HBuilder::GetDSFaceFromDSEdge
(const Standard_Integer indexEdg, const Standard_Integer rank)
{
  if(!myMakeEdgeAncestorIsDone)
    MakeEdgeAncestorMap();

  if(rank == 1) {
    if(myDSEdgesDSFaces1.IsBound(indexEdg)) {
      TColStd_ListOfInteger& loi = 
	myDSEdgesDSFaces1.ChangeFind(indexEdg);
      return loi;
    }
  }
  
  if(rank == 2) {
    if(myDSEdgesDSFaces2.IsBound(indexEdg)) {
      TColStd_ListOfInteger& loi = 
	myDSEdgesDSFaces2.ChangeFind(indexEdg);
      return loi;
    }
  }
  
  return myEmptyIntegerList;
}

//=======================================================================
//function : MakeCurveAncestorMap
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_HBuilder::MakeCurveAncestorMap()
{
  if(myMakeCurveAncestorIsDone)
    return;
  mySectEdgeDSCurve.Clear();
  myMakeCurveAncestorIsDone = Standard_True;
  const TopOpeBRepDS_DataStructure& DS = DataStructure()->DS();
  TopTools_ListIteratorOfListOfShape itloe;
  TopOpeBRepDS_CurveExplorer cex(DS,Standard_True);
//  Standard_Integer ic, icm;
  Standard_Integer ic;
  for (; cex.More(); cex.Next()) {
    ic = cex.Index();
    const TopTools_ListOfShape& LOS = myBuilder.myNewEdges.Find(ic);
    itloe.Initialize(LOS);
    for(;itloe.More();itloe.Next()) {
      TopoDS_Shape& E = *((TopoDS_Shape*)(&itloe.Value()));
      if(mySectEdgeDSCurve.IsBound(E)) {
#ifdef OCCT_DEBUG
	cout<<"BRepAlgo_DSAccess::MakeEdgeAncestorFromCurve : program error"<<endl;
#endif
      }
      mySectEdgeDSCurve.Bind(E, ic);
    }
  }
}

//=======================================================================
//function : GetDSCurveFromSectEdge
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepBuild_HBuilder::GetDSCurveFromSectEdge(const TopoDS_Shape& SectEdge)
{
  Standard_Integer i = 0;
  if(!myMakeCurveAncestorIsDone)
    MakeCurveAncestorMap();
  
  if(mySectEdgeDSCurve.IsBound(SectEdge)) {
    i = mySectEdgeDSCurve.Find(SectEdge);
  }
  return i;
}

//=======================================================================
//function : GetDSFaceFromDSCurve
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepBuild_HBuilder::GetDSFaceFromDSCurve(const Standard_Integer indexCur,const Standard_Integer rank)
{
  Standard_Integer i = 0;
  if(!myMakeCurveAncestorIsDone)
    MakeCurveAncestorMap();

  const TopOpeBRepDS_DataStructure& DS = DataStructure()->DS();
  if(rank == 1) {
    const TopOpeBRepDS_Curve& DSC = DS.Curve(indexCur);
    const TopoDS_Shape& F1 = DSC.Shape1();
    i = DS.Shape(F1);
  }
  
  if(rank == 2) {
    const TopOpeBRepDS_Curve& DSC = DS.Curve(indexCur);
    const TopoDS_Shape& F2 = DSC.Shape2();
    i = DS.Shape(F2);
  }

  return i;
}

//=======================================================================
//function : GetDSPointFromNewVertex
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepBuild_HBuilder::GetDSPointFromNewVertex(const TopoDS_Shape& NewVert)
{
  if(!myMakePointAncestorIsDone) {
    myMakePointAncestorIsDone = Standard_True;
    TopOpeBRepDS_DataStructure& DS = DataStructure()->ChangeDS();
    Standard_Integer i, NbPoint = DS.NbPoints();
    for(i = 1; i <= NbPoint; i++) {
      const TopoDS_Shape& Vertex = NewVertex(i);
      if(!Vertex.IsNull())
	myNewVertexDSPoint.Bind(Vertex, i);
    }
  }
  
  Standard_Integer iPnt = 0;
  if(myNewVertexDSPoint.IsBound(NewVert))
    iPnt = myNewVertexDSPoint.Find(NewVert);
  return iPnt;
}

//=======================================================================
//function : EdgeCurveAncestors
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_HBuilder::EdgeCurveAncestors(const TopoDS_Shape& E,TopoDS_Shape& F1,TopoDS_Shape& F2,Standard_Integer& IC)
{
  F1.Nullify(); F2.Nullify(); IC = 0;

  const Handle(TopOpeBRepDS_HDataStructure)& HDS = myBuilder.DataStructure();
  const TopOpeBRepDS_DataStructure& DS = HDS->DS();

//  TopTools_ListIteratorOfListOfShape itloe;
  IC = GetDSCurveFromSectEdge(E);
  if(!IC)
    return Standard_False;

  Standard_Integer iF1, iF2;
  iF1 = GetDSFaceFromDSCurve(IC, 1);
  iF2 = GetDSFaceFromDSCurve(IC, 2);
  
  F1 = DS.Shape(iF1);
  F2 = DS.Shape(iF2);
  return Standard_True;
}

//=======================================================================
//function : EdgeSectionAncestors
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepBuild_HBuilder::EdgeSectionAncestors
(const TopoDS_Shape& E, TopTools_ListOfShape& LF1, TopTools_ListOfShape& LF2, TopTools_ListOfShape& LE1, TopTools_ListOfShape& LE2)
{
  if (E.ShapeType() != TopAbs_EDGE) return Standard_False;

  LF1.Clear(); LF2.Clear(); LE1.Clear(); LE2.Clear();
  TColStd_ListOfInteger f1, f2;
  f1.Clear(); f2.Clear();
  Standard_Integer ie1, ie2, curr;
  
  ie1 = GetDSEdgeFromSectEdge(E,1);
  ie2 = GetDSEdgeFromSectEdge(E,2);
  TColStd_ListIteratorOfListOfInteger it;
  
  if(ie1 && ie2) {
    TColStd_MapOfInteger moi;
    
    f1 = GetDSFaceFromDSEdge(ie1,1);
    it.Initialize(f1);
    moi.Clear();
    for(; it.More(); it.Next()) {
      moi.Add(it.Value());
    }
    it.Initialize(GetDSFaceFromDSEdge(ie2,1));
    for(; it.More(); it.Next()) {
      curr = it.Value();
      if(!moi.Contains(curr)) {
	moi.Add(curr);
	f1.Append(curr);
      }
    }
    f2 = GetDSFaceFromDSEdge(ie1,2);
    it.Initialize(f2);
    moi.Clear();
    for(; it.More(); it.Next()) {
      moi.Add(it.Value());
    }
    it.Initialize(GetDSFaceFromDSEdge(ie2,2));
    for(; it.More(); it.Next()) {
      curr = it.Value();
      if(!moi.Contains(curr)) {
	moi.Add(curr);
	f2.Append(curr);
      }
    }
  }
  else {
    if(ie1) {
      f1 = GetDSFaceFromDSEdge(ie1,1);
      f2 = GetDSFaceFromDSEdge(ie1,2);
    }
    else if(ie2) { 
      f1 = GetDSFaceFromDSEdge(ie2,1);
      f2 = GetDSFaceFromDSEdge(ie2,2);
    }  
  }
  
  const TopOpeBRepDS_DataStructure& DS = myBuilder.DataStructure()->DS();  

  if(ie1)
    LE1.Append(DS.Shape(ie1));
  if(ie2)
    LE2.Append(DS.Shape(ie2));

  for(it.Initialize(f1); it.More(); it.Next()) {
    curr = it.Value();
    LF1.Append(DS.Shape(curr));
  }
  for(it.Initialize(f2); it.More(); it.Next()) {
    curr = it.Value();
    LF2.Append(DS.Shape(curr));
  }
  
  Standard_Boolean r = (!LF1.IsEmpty() && !LF2.IsEmpty());
  r = r && (!LE1.IsEmpty() || !LE2.IsEmpty());
  return r;
}

//=======================================================================
//function : IsKPart
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepBuild_HBuilder::IsKPart() 
{
  Standard_Integer kp = myBuilder.IsKPart();
  return kp;
}

//=======================================================================
//function : MergeKPart
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_HBuilder::MergeKPart(const TopAbs_State TB1,const TopAbs_State TB2)
{
  Standard_Integer kp = IsKPart();
  if ( kp ) myBuilder.MergeKPart(TB1,TB2);
}

//=======================================================================
//function : ChangeBuilder
//purpose  : 
//=======================================================================

TopOpeBRepBuild_Builder& TopOpeBRepBuild_HBuilder::ChangeBuilder()
{
  return myBuilder;
}
