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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopOpeBRepBuild_Builder.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_EdgeBuilder.hxx>
#include <TopOpeBRepBuild_FaceBuilder.hxx>
#include <TopOpeBRepBuild_GTool.hxx>
#include <TopOpeBRepBuild_GTopo.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepBuild_PaveSet.hxx>
#include <TopOpeBRepBuild_ShapeSet.hxx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>
#include <TopOpeBRepBuild_SolidBuilder.hxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_Config.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_CurveExplorer.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_DataMapIteratorOfDataMapOfShapeListOfShapeOn1State.hxx>
#include <TopOpeBRepDS_Filter.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_ListOfShapeOn1State.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopOpeBRepDS_Reducer.hxx>
#include <TopOpeBRepDS_Surface.hxx>
#include <TopOpeBRepDS_SurfaceIterator.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepBuild_GettraceCU();
extern Standard_Boolean TopOpeBRepBuild_GettraceCUV();
extern Standard_Boolean TopOpeBRepBuild_GettraceSPF();
extern Standard_Boolean TopOpeBRepBuild_GettraceSPS();
extern Standard_Boolean TopOpeBRepBuild_GetcontextSF2();
extern Standard_Boolean TopOpeBRepBuild_GettraceSHEX();
Standard_EXPORT void debmarksplit(const Standard_Integer i) {cout<<"++ debmarksplit "<<i<<endl;}
Standard_EXPORT void debchangesplit(const Standard_Integer i) {cout<<"++ debchangesplit "<<i<<endl;}
#endif

static  Standard_Integer STATIC_SOLIDINDEX = 0;
#include <TopOpeBRepBuild_SplitEdge.hxx>
#include <TopOpeBRepBuild_SplitFace.hxx>
#include <TopOpeBRepBuild_SplitSolid.hxx>
#include <TopOpeBRepBuild_SplitShapes.hxx>
#include <TopOpeBRepBuild_Fill.hxx>

Standard_EXPORT TopOpeBRepBuild_Builder* GLOBAL_PBUILDER;

//=======================================================================
//function : TopOpeBRepBuild_Builder
//purpose  : 
//=======================================================================
TopOpeBRepBuild_Builder::TopOpeBRepBuild_Builder(const TopOpeBRepDS_BuildTool& BT) 
: myBuildTool(BT),
  mySectionDone(Standard_False),
  myIsKPart(0),
  myClassifyDef(Standard_False),
  myClassifyVal(Standard_True),
  myProcessON(Standard_False)
{
  GLOBAL_PBUILDER = this;
  InitSection();
}

//modified by NIZHNY-MZV  Sat May  6 10:04:49 2000
//=======================================================================
//function : Destroy
//purpose  : virtual destructor
//=======================================================================
void TopOpeBRepBuild_Builder::Destroy()
{
} 

//=======================================================================
//function : ChangeBuildTool
//purpose  : 
//=======================================================================
TopOpeBRepDS_BuildTool& TopOpeBRepBuild_Builder::ChangeBuildTool() 
{
  return myBuildTool;
}

//=======================================================================
//function : BuildTool
//purpose  : 
//=======================================================================
const TopOpeBRepDS_BuildTool& TopOpeBRepBuild_Builder::BuildTool() const
{
  return myBuildTool;
}

//=======================================================================
//function : DataStructure
//purpose  : 
//=======================================================================
Handle(TopOpeBRepDS_HDataStructure) TopOpeBRepBuild_Builder::DataStructure() const
{
  return myDataStructure;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::Perform(const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
#ifdef OCCT_DEBUG
  GdumpSHASETreset();
#endif
  Clear();
  myDataStructure = HDS;
  BuildVertices(HDS); 
  SplitEvisoONperiodicF();
  BuildEdges(HDS); 
  BuildFaces(HDS);
  myIsKPart = 0;
  InitSection();
  SplitSectionEdges();
  TopOpeBRepDS_Filter F(HDS, &myShapeClassifier);
  F.ProcessFaceInterferences(mySplitON);
  TopOpeBRepDS_Reducer R(HDS);
  R.ProcessFaceInterferences(mySplitON);
} // Perform

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::Perform(const Handle(TopOpeBRepDS_HDataStructure)& HDS,const TopoDS_Shape& S1, const TopoDS_Shape& S2)
{
  Perform(HDS);
  myShape1 = S1; myShape2 = S2;
  myIsKPart = FindIsKPart();
} // Perform

//=======================================================================
//function : AddIntersectionEdges
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::AddIntersectionEdges
(TopoDS_Shape& aFace,const TopAbs_State ToBuild1,const Standard_Boolean RevOri1,TopOpeBRepBuild_ShapeSet& WES) const
{
  TopoDS_Shape anEdge;
  TopOpeBRepDS_CurveIterator FCurves = myDataStructure->FaceCurves(aFace);
  for (; FCurves.More(); FCurves.Next()) {
    Standard_Integer iC = FCurves.Current();
#ifdef OCCT_DEBUG
    Standard_Boolean tCU = TopOpeBRepBuild_GettraceCU();
    Standard_Boolean NtCUV = !TopOpeBRepBuild_GettraceCUV();
    if(tCU) {cout<<endl;myDataStructure->Curve(iC).Dump(cout,iC,NtCUV);}
#endif
    const TopTools_ListOfShape& LnewE = NewEdges(iC);
    for (TopTools_ListIteratorOfListOfShape Iti(LnewE); Iti.More(); Iti.Next()) {
      anEdge = Iti.Value();
      TopAbs_Orientation ori = FCurves.Orientation(ToBuild1);
      TopAbs_Orientation newori = Orient(ori,RevOri1);

      if(newori == TopAbs_EXTERNAL) continue;

      myBuildTool.Orientation(anEdge,newori);
      const Handle(Geom2d_Curve)& PC = FCurves.PCurve();
      myBuildTool.PCurve(aFace,anEdge,PC);
      WES.AddStartElement(anEdge);
    }
  }
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::Clear()
{
  const TopOpeBRepDS_DataStructure& BDS = myDataStructure->DS();
  TopOpeBRepDS_DataMapIteratorOfDataMapOfShapeListOfShapeOn1State it;
  for (it.Initialize(mySplitOUT); it.More(); it.Next()) {
    const TopoDS_Shape& e = it.Key();
    if ( e.ShapeType() == TopAbs_EDGE ) {
      Standard_Boolean isse =  BDS.IsSectionEdge(TopoDS::Edge(e));
      if (!isse) mySplitOUT.ChangeFind(e).Clear();
    }
  }
  for (it.Initialize(mySplitIN); it.More(); it.Next()) {
    const TopoDS_Shape& e = it.Key();
    if ( e.ShapeType() == TopAbs_EDGE ) {
      Standard_Boolean isse =  BDS.IsSectionEdge(TopoDS::Edge(e));
      if (!isse) mySplitIN.ChangeFind(e).Clear();
    }
  }
  for (it.Initialize(mySplitON); it.More(); it.Next()) {
    const TopoDS_Shape& e = it.Key();
    if ( e.ShapeType() == TopAbs_EDGE ) {
      Standard_Boolean isse =  BDS.IsSectionEdge(TopoDS::Edge(e));
      if (!isse) mySplitON.ChangeFind(e).Clear();
    }
  }
  myMergedOUT.Clear();
  myMergedIN.Clear();
  myMergedON.Clear();
} // Clear

//=======================================================================
//function : NewFaces
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& TopOpeBRepBuild_Builder::NewFaces(const Standard_Integer I) const
{
  const TopTools_ListOfShape& L = myNewFaces->Array1().Value(I);
  return L;
} // NewFaces

//=======================================================================
//function : ChangeNewFaces
//purpose  : private
//=======================================================================
TopTools_ListOfShape& TopOpeBRepBuild_Builder::ChangeNewFaces(const Standard_Integer I)
{
  TopTools_ListOfShape& L = myNewFaces->ChangeArray1().ChangeValue(I);
  return L;
} // ChangeNewFaces

//=======================================================================
//function : NewEdges
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& TopOpeBRepBuild_Builder::NewEdges(const Standard_Integer I) const
{
  if ( myNewEdges.IsBound(I) ) {
    return myNewEdges.Find(I);
  }
  else {
    return myEmptyShapeList;
  }
} // NewEdges

//=======================================================================
//function : ChangeNewEdges
//purpose  : private
//=======================================================================
TopTools_ListOfShape& TopOpeBRepBuild_Builder::ChangeNewEdges(const Standard_Integer I)
{
  if ( ! myNewEdges.IsBound(I) ) {
    TopTools_ListOfShape thelist;
    myNewEdges.Bind(I, thelist);
  }
  TopTools_ListOfShape& L = myNewEdges.ChangeFind(I);
  return L;
} // ChangeNewEdges

//=======================================================================
//function : NewVertex
//purpose  : 
//=======================================================================
const TopoDS_Shape& TopOpeBRepBuild_Builder::NewVertex(const Standard_Integer I) const
{
  const TopoDS_Shape& V = myNewVertices->Array1().Value(I);
  return V;
} // NewVertex

//=======================================================================
//function : ChangeNewVertex
//purpose  : private
//=======================================================================
TopoDS_Shape& TopOpeBRepBuild_Builder::ChangeNewVertex(const Standard_Integer I)
{
  TopoDS_Shape& V = myNewVertices->ChangeArray1().ChangeValue(I);
  return V;
} // ChangeNewVertex

//=======================================================================
//function : ToSplit
//purpose  : private
//=======================================================================
Standard_Boolean TopOpeBRepBuild_Builder::ToSplit(const TopoDS_Shape& S,const TopAbs_State ToBuild) const
{
  Standard_Boolean issplit = IsSplit(S,ToBuild);
  Standard_Boolean hasgeom = myDataStructure->HasGeometry(S);
  Standard_Boolean hassame = myDataStructure->HasSameDomain(S);
  Standard_Boolean tosplit = (!issplit) && (hasgeom || hassame);

#ifdef OCCT_DEBUG
  Standard_Integer iS; Standard_Boolean tSPS = GtraceSPS(S,iS);
  if (tSPS) { 
    cout<<"tosplit "<<tosplit<<" : "<<"!issplit "<<(!issplit);
    cout<<" && (hasgeom || hassame) ("<<hasgeom<<" || "<<hassame<<")"<<endl;
  }
#endif

  return tosplit;
} // ToSplit

//=======================================================================
//function : MarkSplit
//purpose  : private
//=======================================================================
void TopOpeBRepBuild_Builder::MarkSplit(const TopoDS_Shape& S,const TopAbs_State ToBuild,const Standard_Boolean Bval) 
{
  TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State* p = NULL;
  if      ( ToBuild == TopAbs_OUT ) p = &mySplitOUT;
  else if ( ToBuild == TopAbs_IN  ) p = &mySplitIN;
  else if ( ToBuild == TopAbs_ON  ) p = &mySplitON;
  if ( p == NULL ) return;

  TopOpeBRepDS_ListOfShapeOn1State thelist;
  if (!(*p).IsBound(S)) (*p).Bind(S, thelist);
  TopOpeBRepDS_ListOfShapeOn1State& losos = (*p).ChangeFind(S);
  losos.Split(Bval);
  
#ifdef OCCT_DEBUG
  Standard_Integer iS; Standard_Boolean tSPS = GtraceSPS(S,iS);
  if(tSPS){
    GdumpSHA(S, (char *) "MarkSplit ");
    cout<<" ";TopAbs::Print(ToBuild,cout);cout<<" "<<Bval<<endl;
    debmarksplit(iS);
  }
#endif
  
} // MarkSplit

//=======================================================================
//function : IsSplit
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_Builder::IsSplit(const TopoDS_Shape& S,const TopAbs_State ToBuild) const
{
  Standard_Boolean res = Standard_False;  
  const TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State* p = NULL;
  if      ( ToBuild == TopAbs_OUT ) p = &mySplitOUT;
  else if ( ToBuild == TopAbs_IN  ) p = &mySplitIN;
  else if ( ToBuild == TopAbs_ON  ) p = &mySplitON;
  if ( p == NULL ) return res;

  if ((*p).IsBound(S)) {
    const TopOpeBRepDS_ListOfShapeOn1State& losos = (*p).Find(S);
    res = losos.IsSplit();
#ifdef OCCT_DEBUG
//    Standard_Integer n = losos.ListOnState().Extent();
#endif
  }
  return res;
} // IsSplit

//=======================================================================
//function : Splits
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& TopOpeBRepBuild_Builder::Splits(const TopoDS_Shape& S, const TopAbs_State ToBuild) const
{
  const TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State* p = NULL;
  if      ( ToBuild == TopAbs_OUT ) p = &mySplitOUT;
  else if ( ToBuild == TopAbs_IN  ) p = &mySplitIN;
  else if ( ToBuild == TopAbs_ON  ) p = &mySplitON;
  if ( p == NULL ) return myEmptyShapeList;

  if ((*p).IsBound(S)) {
    const TopOpeBRepDS_ListOfShapeOn1State& losos = (*p).Find(S);
    const TopTools_ListOfShape& L = losos.ListOnState();
    return L;
  }
  return myEmptyShapeList;
} // Splits

//=======================================================================
//function : ChangeSplit
//purpose  : private
//=======================================================================
TopTools_ListOfShape& TopOpeBRepBuild_Builder::ChangeSplit(const TopoDS_Shape& S,const TopAbs_State ToBuild)
{
#ifdef OCCT_DEBUG
  Standard_Integer iS; Standard_Boolean tSPS = GtraceSPS(S,iS);
  if(tSPS){
    GdumpSHA(S, (char *) "ChangeSplit ");
    cout<<" ";TopAbs::Print(ToBuild,cout);cout<<endl;
    debchangesplit(iS);
  }
#endif

  TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State* p = NULL;
  if      ( ToBuild == TopAbs_OUT ) p = &mySplitOUT;
  else if ( ToBuild == TopAbs_IN  ) p = &mySplitIN;
  else if ( ToBuild == TopAbs_ON  ) p = &mySplitON;
  if ( p == NULL ) return myEmptyShapeList;
  TopOpeBRepDS_ListOfShapeOn1State thelist1;
  if (!(*p).IsBound(S)) (*p).Bind(S, thelist1);
  TopOpeBRepDS_ListOfShapeOn1State& losos = (*p).ChangeFind(S);
  TopTools_ListOfShape& L = losos.ChangeListOnState();
  return L;
} // ChangeSplit

//=======================================================================
//function : ShapePosition
//purpose  : compute position of shape S compared with the shapes of list LS
//           if S is found IN any shape of LS, return IN
//           else return OUT
//=======================================================================
TopAbs_State TopOpeBRepBuild_Builder::ShapePosition(const TopoDS_Shape& S, const TopTools_ListOfShape& LS)
{ 
  TopAbs_State state = TopAbs_UNKNOWN;

  // take the edges of myEdgeAvoid as shape to avoid
  // during face classification
  const TopTools_ListOfShape* PLOS = &myEmptyShapeList;
  TopAbs_ShapeEnum tS = S.ShapeType();
  if (tS == TopAbs_FACE) PLOS = &myEdgeAvoid;
  // NYI : idem with myFaceAvoid if (tS == TopAbs_SOLID)

  for (TopTools_ListIteratorOfListOfShape Iti(LS); Iti.More(); Iti.Next()) {
    const TopoDS_Shape& SLS = Iti.Value();
#ifdef OCCT_DEBUG
//    TopAbs_ShapeEnum tSLS = SLS.ShapeType();
#endif
    state = myShapeClassifier.StateShapeShape(S,(*PLOS),SLS);
    if (state != TopAbs_OUT && state != TopAbs_UNKNOWN) return state;
  }
  if (state == TopAbs_UNKNOWN) return state;
  return TopAbs_OUT;
}

//=======================================================================
//function : KeepShape
//purpose  : compute <pos2> = position of shape <S1> / shapes of list <LS2>
//           shape S1 is kept
//           - if LS2 is empty
//           - if (pos2 == ToBuild1)
//=======================================================================
Standard_Boolean TopOpeBRepBuild_Builder::KeepShape(const TopoDS_Shape& S1,const TopTools_ListOfShape& LS2,const TopAbs_State ToBuild1)
{ 
  Standard_Boolean keep = Standard_True;
  if ( ! LS2.IsEmpty() ) {
    TopAbs_State pos2 = ShapePosition(S1,LS2);
    if ( pos2 != ToBuild1 ) keep = Standard_False;
  }
  return keep;
}

//=======================================================================
//function : TopType
//purpose  : return the type of upper subshape found in <S>
//=======================================================================
TopAbs_ShapeEnum TopOpeBRepBuild_Builder::TopType(const TopoDS_Shape& S)
{
  TopAbs_ShapeEnum t;
  TopOpeBRepTool_ShapeExplorer e;

  t = TopAbs_COMPOUND;   e.Init(S,t); if (e.More()) return t;
  t = TopAbs_COMPSOLID;  e.Init(S,t); if (e.More()) return t;
  t = TopAbs_SOLID;      e.Init(S,t); if (e.More()) return t;
  t = TopAbs_SHELL;      e.Init(S,t); if (e.More()) return t;
  t = TopAbs_FACE;       e.Init(S,t); if (e.More()) return t;
  t = TopAbs_WIRE;       e.Init(S,t); if (e.More()) return t;
  t = TopAbs_EDGE;       e.Init(S,t); if (e.More()) return t;
  t = TopAbs_VERTEX;     e.Init(S,t); if (e.More()) return t;

  return TopAbs_SHAPE;
}

//=======================================================================
//function : Reverse
//purpose  : compute orientation reversibility according to build states
//=======================================================================
Standard_Boolean TopOpeBRepBuild_Builder::Reverse(const TopAbs_State ToBuild1,const TopAbs_State ToBuild2)
{
  Standard_Boolean rev;
  if (ToBuild1 == TopAbs_IN && ToBuild2 == TopAbs_IN) rev = Standard_False;
  else rev = (ToBuild1 == TopAbs_IN);
  return rev;
}

//=======================================================================
//function : Orient
//purpose  : reverse the orientation
//=======================================================================
TopAbs_Orientation TopOpeBRepBuild_Builder::Orient(const TopAbs_Orientation Ori,const Standard_Boolean Reverse)
{
  TopAbs_Orientation result=TopAbs_FORWARD;
  switch (Reverse) {
    case Standard_True  : result = TopAbs::Complement(Ori); break;
    case Standard_False : result = Ori; break;
  }
  return result;
}

//=======================================================================
//function : FindSameDomain
//purpose  : complete the lists L1,L2 with the shapes of the DS
//           having same domain :
//           L1 = shapes sharing the same domain of L2 shapes
//           L2 = shapes sharing the same domain of L1 shapes
// (L1 contains a face)
//=======================================================================
void TopOpeBRepBuild_Builder::FindSameDomain(TopTools_ListOfShape& L1,TopTools_ListOfShape& L2) const 
{
  Standard_Integer i;
  Standard_Integer nl1 = L1.Extent(), nl2 = L2.Extent();

  while ( nl1 > 0 || nl2 > 0 )  {

    TopTools_ListIteratorOfListOfShape it1(L1);
    for (i=1 ; i<=nl1; i++) {
      const TopoDS_Shape& S1 = it1.Value();
#ifdef OCCT_DEBUG
//      Standard_Integer iS1 = myDataStructure->Shape(S1);  // DEB
#endif
      TopTools_ListIteratorOfListOfShape itsd(myDataStructure->SameDomain(S1));
      for (; itsd.More(); itsd.Next() ) {
	const TopoDS_Shape& S2 = itsd.Value();
#ifdef OCCT_DEBUG
//	Standard_Integer iS2 = myDataStructure->Shape(S2);// DEB
#endif
	Standard_Boolean found = Contains(S2,L2);
	if ( ! found ) {
	  L2.Prepend(S2);
	  nl2++;
	}
      }
      it1.Next();
    }
    nl1 = 0;

    TopTools_ListIteratorOfListOfShape it2(L2);
    for (i=1 ; i<=nl2; i++) {
      const TopoDS_Shape& S2 = it2.Value();
#ifdef OCCT_DEBUG
//      Standard_Integer iS2 = myDataStructure->Shape(S2);// DEB
#endif
      TopTools_ListIteratorOfListOfShape itsd(myDataStructure->SameDomain(S2));
      for (; itsd.More(); itsd.Next() ) {
	const TopoDS_Shape& S1 = itsd.Value();
#ifdef OCCT_DEBUG
//	Standard_Integer iS1 = myDataStructure->Shape(S1);// DEB
#endif
	Standard_Boolean found = Contains(S1,L1);
	if ( ! found ) {
	  L1.Prepend(S1);
	  nl1++;
	}
      }
      it2.Next();
    }
    nl2 = 0;

  }

}


//=======================================================================
//function : FindSameDomainSameOrientation
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::FindSameDomainSameOrientation(TopTools_ListOfShape& L1, TopTools_ListOfShape& L2) const 
{
  FindSameDomain(L1,L2);
  TopTools_ListIteratorOfListOfShape it(L1);
  if ( !it.More() ) return;

  const TopoDS_Shape& sref = it.Value();
#ifdef OCCT_DEBUG
//  Standard_Integer  iref = myDataStructure->SameDomainReference(sref);
#endif
  TopOpeBRepDS_Config oref = myDataStructure->SameDomainOrientation(sref);

  TopTools_ListOfShape LL1,LL2;

  for (it.Initialize(L1); it.More(); it.Next() ) {
    const TopoDS_Shape& s = it.Value();
    TopOpeBRepDS_Config o = myDataStructure->SameDomainOrientation(s);
    if      ( o == oref && !Contains(s,LL1) ) LL1.Append(s);
    else if ( o != oref && !Contains(s,LL2) ) LL2.Append(s);
  }

  for (it.Initialize(L2); it.More(); it.Next() ) {
    const TopoDS_Shape& s = it.Value();
    TopOpeBRepDS_Config o = myDataStructure->SameDomainOrientation(s);
    if      ( o == oref && !Contains(s,LL1) ) LL1.Append(s);
    else if ( o != oref && !Contains(s,LL2) ) LL2.Append(s);
  }

  L1 = LL1;
  L2 = LL2;

}

//=======================================================================
//function : MapShapes
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::MapShapes(const TopoDS_Shape& S1,const TopoDS_Shape& S2)
{
  Standard_Boolean S1null = S1.IsNull();
  Standard_Boolean S2null = S2.IsNull();
  ClearMaps();
  if ( ! S1null ) TopExp::MapShapes(S1,myMAP1);
  if ( ! S2null ) TopExp::MapShapes(S2,myMAP2);
}

//=======================================================================
//function : ClearMaps
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::ClearMaps()
{
  myMAP1.Clear(); 
  myMAP2.Clear(); 
}

//=======================================================================
//function : FindSameRank
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::FindSameRank(const TopTools_ListOfShape& L1,const Standard_Integer rank,TopTools_ListOfShape& L2) const
{
  for (  TopTools_ListIteratorOfListOfShape it1(L1); it1.More(); it1.Next() ) {
    const TopoDS_Shape& s = it1.Value();
    Standard_Integer r = ShapeRank(s);
    if ( r == rank && !Contains(s,L2) ) {
      L2.Append(s);
    }
  }
}

//=======================================================================
//function : ShapeRank
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRepBuild_Builder::ShapeRank(const TopoDS_Shape& s) const 
{
  Standard_Boolean isof1 = IsShapeOf(s,1);
  Standard_Boolean isof2 = IsShapeOf(s,2);
  Standard_Integer ancetre = (isof1 || isof2) ? ((isof1) ? 1 : 2) : 0;
  return ancetre;
}

//=======================================================================
//function : IsShapeOf
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_Builder::IsShapeOf(const TopoDS_Shape& s,const Standard_Integer i) const 
{
  Standard_Boolean b = Standard_False;
  if      (i == 1) b = myMAP1.Contains(s);
  else if (i == 2) b = myMAP2.Contains(s);
  return b;
}

//=======================================================================
//function : Contains
//purpose  : returns True if S is in the list L.
//=======================================================================
Standard_Boolean TopOpeBRepBuild_Builder::Contains(const TopoDS_Shape& S,const TopTools_ListOfShape& L) 
{
  for (TopTools_ListIteratorOfListOfShape it(L); it.More(); it.Next() ) {
    TopoDS_Shape& SL = it.Value();
    Standard_Boolean issame = SL.IsSame(S);
    if ( issame ) return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : Opec12
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_Builder::Opec12() const 
{
  Standard_Boolean b = (myState1 == TopAbs_OUT) && (myState2 == TopAbs_IN );
  return b;
}

//=======================================================================
//function : Opec21
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_Builder::Opec21() const 
{
  Standard_Boolean b = (myState1 == TopAbs_IN ) && (myState2 == TopAbs_OUT);
  return b;
}

//=======================================================================
//function : Opecom
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_Builder::Opecom() const 
{
  Standard_Boolean b = (myState1 == TopAbs_IN ) && (myState2 == TopAbs_IN );
  return b;
}

//=======================================================================
//function : Opefus
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_Builder::Opefus() const 
{
  Standard_Boolean b = (myState1 == TopAbs_OUT) && (myState2 == TopAbs_OUT);
  return b;
}

//=======================================================================
//function : MSplit
//purpose  : 
//=======================================================================
const TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State& TopOpeBRepBuild_Builder::MSplit(const TopAbs_State s) const
{
  if      (s == TopAbs_IN)  return mySplitIN;
  else if (s == TopAbs_OUT) return mySplitOUT;
  else if (s == TopAbs_ON)  return mySplitON;
  return mySplitIN;
}

//=======================================================================
//function : ChangeMSplit
//purpose  : 
//=======================================================================
TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State& TopOpeBRepBuild_Builder::ChangeMSplit(const TopAbs_State s)
{
  if      (s == TopAbs_IN)  return mySplitIN;
  else if (s == TopAbs_OUT) return mySplitOUT;
  else if (s == TopAbs_ON)  return mySplitON;
  return mySplitIN;
}
