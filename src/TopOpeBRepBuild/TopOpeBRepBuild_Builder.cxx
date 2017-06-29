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
#include <Standard_ProgramError.hxx>
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
Standard_EXPORT void debmarksplit(const Standard_Integer i) {cout<<"++ debmarksplit "<<i<<endl;}
Standard_EXPORT void debchangesplit(const Standard_Integer i) {cout<<"++ debchangesplit "<<i<<endl;}
Standard_EXPORT void debspf(const Standard_Integer i) {cout<<"++  debspf"<<i<<endl;}
#endif

static  Standard_Integer STATIC_SOLIDINDEX = 0;

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
  InitSection();
}

//modified by NIZHNY-MZV  Sat May  6 10:04:49 2000
//=======================================================================
//function : ~TopOpeBRepBuild_Builder
//purpose  : virtual destructor
//=======================================================================
TopOpeBRepBuild_Builder::~TopOpeBRepBuild_Builder()
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
    cout<<"tosplit "<<tosplit<<" : !issplit "<<(!issplit);
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
  return !Reverse
       ? Ori
       : TopAbs::Complement(Ori);
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

//=======================================================================
//function : SplitEdge
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Builder::SplitEdge(const TopoDS_Shape& E,
					const TopAbs_State ToBuild1,
					const TopAbs_State ToBuild2)
{
#ifdef OCCT_DEBUG
  if ( TopOpeBRepBuild_GetcontextSF2() ) {
    SplitEdge2(E,ToBuild1,ToBuild2);
    return;
  }
#endif
  SplitEdge1(E,ToBuild1,ToBuild2);
  return;
}

//=======================================================================
//function : SplitEdge1
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Builder::SplitEdge1(const TopoDS_Shape& Eoriented,
					 const TopAbs_State ToBuild1,
					 const TopAbs_State ToBuild2)
{
  // work on a FORWARD edge <Eforward>

  TopoDS_Shape Eforward = Eoriented; 
  Eforward.Orientation(TopAbs_FORWARD);

  Standard_Boolean tosplit = ToSplit(Eoriented,ToBuild1);

#ifdef OCCT_DEBUG
  Standard_Integer iEdge; Standard_Boolean tSPS = GtraceSPS(Eoriented,iEdge);
  if(tSPS){
    cout<<endl;
    GdumpSHASTA(Eoriented,ToBuild1,"--- SplitEdge ");
    cout<<endl;
  }
#endif
  
  if ( ! tosplit ) return;

  Reverse(ToBuild1,ToBuild2);
  Reverse(ToBuild2,ToBuild1);
  Standard_Boolean ConnectTo1 = Standard_True;
  Standard_Boolean ConnectTo2 = Standard_False;
  
  // build the list of edges to split : LE1, LE2
  TopTools_ListOfShape LE1,LE2;
  LE1.Append(Eforward);
  FindSameDomain(LE1,LE2);

#ifdef OCCT_DEBUG
  if(tSPS){GdumpSAMDOM(LE1, (char *) "1 : ");}
  if(tSPS){GdumpSAMDOM(LE2, (char *) "2 : ");}
  if(tSPS){cout<<endl;}
  if(tSPS){cout<<"V of edge ";TopAbs::Print(Eforward.Orientation(),cout);}
  if(tSPS){cout<<endl;}
  if(tSPS){GdumpEDG(Eforward);}
#endif
  
  // SplitEdge on a edge having other same domained edges on the
  // other shape : do not reverse orientation of edges in FillEdge
    
  // Make a PaveSet <PVS> on edge <Eforward>
  TopOpeBRepBuild_PaveSet PVS(Eforward);

  // Add the points/vertices found on edge <Eforward> in <PVS>
  TopOpeBRepDS_PointIterator EPIT(myDataStructure->EdgePoints(Eforward));
  FillVertexSet(EPIT,ToBuild1,PVS);
  
  TopOpeBRepBuild_PaveClassifier VCL(Eforward);
  Standard_Boolean equalpar = PVS.HasEqualParameters();
  if (equalpar) VCL.SetFirstParameter(PVS.EqualParameters());

  // ------------------------------------------
  // before return if PVS has no vertices, 
  // mark <Eforward> as split <ToBuild1>
  // ------------------------------------------
  MarkSplit(Eforward,ToBuild1);
  
  PVS.InitLoop();
  if ( !PVS.MoreLoop() ) {
#ifdef OCCT_DEBUG
    if(tSPS) {
      cout<<"NO VERTEX split "; TopAbs::Print(ToBuild1,cout);cout<<endl;
    }
#endif
    return;
  }
  
  // build the new edges
  TopOpeBRepBuild_EdgeBuilder EBU(PVS,VCL);
  
  // Build the new edges
  // -------------------
  TopTools_ListOfShape& EdgeList = ChangeMerged(Eforward,ToBuild1);
  MakeEdges(Eforward,EBU,EdgeList);

  TopTools_ListIteratorOfListOfShape itLE1,itLE2;

  // connect new edges as edges built <ToBuild1> on LE1 edge
  // --------------------------------------------------------
  for (itLE1.Initialize(LE1); itLE1.More(); itLE1.Next()) {
    TopoDS_Shape Ecur = itLE1.Value();
    MarkSplit(Ecur,ToBuild1);
    TopTools_ListOfShape& EL = ChangeSplit(Ecur,ToBuild1);
    if ( ConnectTo1 ) EL = EdgeList;
  }
  
  // connect new edges as edges built <ToBuild2> on LE2 edges
  // --------------------------------------------------------
  for (itLE2.Initialize(LE2); itLE2.More(); itLE2.Next()) {
    TopoDS_Shape Ecur = itLE2.Value();
    MarkSplit(Ecur,ToBuild2);
    TopTools_ListOfShape& EL = ChangeSplit(Ecur,ToBuild2);
    if ( ConnectTo2 ) EL = EdgeList;
  }

} // SplitEdge1

//=======================================================================
//function : SplitEdge2
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Builder::SplitEdge2(const TopoDS_Shape& Eoriented,
					 const TopAbs_State ToBuild1,
					 const TopAbs_State /*ToBuild2*/)
{
  Standard_Boolean tosplit = ToSplit(Eoriented,ToBuild1);
  if ( ! tosplit ) return;
  
  // work on a FORWARD edge <Eforward>
  TopoDS_Shape Eforward = Eoriented;
  myBuildTool.Orientation(Eforward,TopAbs_FORWARD);

#ifdef OCCT_DEBUG
  Standard_Integer iEdge; Standard_Boolean tSPS = GtraceSPS(Eoriented,iEdge);
  if(tSPS){cout<<endl;}
  if(tSPS){GdumpSHASTA(Eoriented,ToBuild1,"--- SplitEdge2 ");}
#endif
  
  // Make a PaveSet <PVS> on edge <Eforward>
  // Add the points/vertices found on edge <Eforward> in <PVS>
  TopOpeBRepBuild_PaveSet PVS(Eforward);

  TopOpeBRepDS_PointIterator EPIT(myDataStructure->EdgePoints(Eforward));
  FillVertexSet(EPIT,ToBuild1,PVS);
  
  TopOpeBRepBuild_PaveClassifier VCL(Eforward);
  Standard_Boolean equalpar = PVS.HasEqualParameters();
  if (equalpar) VCL.SetFirstParameter(PVS.EqualParameters());

  // ------------------------------------------
  // before return if PVS has no vertices, 
  // mark <Eforward> as split <ToBuild1>
  // ------------------------------------------
  MarkSplit(Eforward,ToBuild1);
  
  PVS.InitLoop();
  if ( !PVS.MoreLoop() ) {
#ifdef OCCT_DEBUG
    if(tSPS) {cout<<"NO VERTEX split ";TopAbs::Print(ToBuild1,cout);cout<<endl;}
#endif
    return;
  }
  
  // build the new edges
  TopOpeBRepBuild_EdgeBuilder EBU(PVS,VCL);
  
  // connect the new edges as split parts <ToBuild1> built on <Eforward>
  TopTools_ListOfShape& EL = ChangeSplit(Eforward,ToBuild1);
  MakeEdges(Eforward,EBU,EL);
  
} // SplitEdge2

//=======================================================================
//function : SplitFace
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Builder::SplitFace(const TopoDS_Shape& Foriented,
					const TopAbs_State ToBuild1,
					const TopAbs_State ToBuild2)
{
#ifdef OCCT_DEBUG
  if(TopOpeBRepBuild_GetcontextSF2()){
    SplitFace2(Foriented,ToBuild1,ToBuild2);
    return;
  }
#endif
  SplitFace1(Foriented,ToBuild1,ToBuild2);
  return;
}

//=======================================================================
//function : SplitFace1
//purpose  : tout dans le meme edge set
//=======================================================================

void TopOpeBRepBuild_Builder::SplitFace1(const TopoDS_Shape& Foriented,
					 const TopAbs_State ToBuild1,
					 const TopAbs_State ToBuild2)
{
  //                              process  connect  connect
  // operation tobuild1 tobuild2  face F   to 1     to 2
  // --------- -------- --------  -------  -------  -------
  // common    IN       IN        yes      yes      yes
  // fuse      OUT      OUT       yes      yes      yes
  // cut 1-2   OUT      IN        yes      yes      no
  // cut 2-1   IN       OUT       yes      yes      no           
  //                                                       
  Standard_Boolean tosplit = ToSplit(Foriented,ToBuild1);
  if ( ! tosplit ) return;
  
  Standard_Boolean RevOri1 = Reverse(ToBuild1,ToBuild2);
  Standard_Boolean RevOri2 = Reverse(ToBuild2,ToBuild1);
  Standard_Boolean ConnectTo1 = Standard_True;
  Standard_Boolean ConnectTo2 = Standard_False;
  
  // work on a FORWARD face <Fforward>
  TopoDS_Shape Fforward = Foriented; 
  myBuildTool.Orientation(Fforward,TopAbs_FORWARD);
  
  // build the list of faces to split : LF1, LF2
  TopTools_ListOfShape LF1,LF2;
  LF1.Append(Fforward);
  FindSameDomain(LF1,LF2);
  Standard_Integer n1 = LF1.Extent();
  Standard_Integer n2 = LF2.Extent();
  
  // SplitFace on a face having other same domained faces on the
  // other shape : do not reverse orientation of faces in FillFace
  if (!n2) RevOri1 = Standard_False;
  if (!n1) RevOri2 = Standard_False;
  
  // Create an edge set <WES> connected by vertices
  // ----------------------------------------------
  TopOpeBRepBuild_WireEdgeSet WES(Fforward,this);
  
#ifdef OCCT_DEBUG
  Standard_Boolean tSPF=TopOpeBRepBuild_GettraceSPF();
  Standard_Integer iFace=myDataStructure->Shape(Foriented);
  if(tSPF){cout<<endl;GdumpSHASTA(Foriented,ToBuild1,"=== SplitFace ");}
  if(tSPF){GdumpSAMDOM(LF1, (char *) "1 : ");GdumpSAMDOM(LF2, (char *) "2 : ");}
  if(tSPF) debspf(iFace);
#endif
    
  TopTools_ListIteratorOfListOfShape itLF1,itLF2;

  for (itLF1.Initialize(LF1); itLF1.More(); itLF1.Next()) {
    const TopoDS_Shape& Fcur = itLF1.Value();
//                     myDataStructure->Shape(Fcur);//DEB
    FillFace(Fcur,ToBuild1,LF2,ToBuild2,WES,RevOri1);
  }
  
  for (itLF2.Initialize(LF2); itLF2.More(); itLF2.Next()) {
    const TopoDS_Shape& Fcur = itLF2.Value();
//                            myDataStructure->Shape(Fcur);//DEB
    FillFace(Fcur,ToBuild2,LF1,ToBuild1,WES,RevOri2);
  }
  
  // Add the intersection edges to edge set WES
  // -----------------------------------------
  AddIntersectionEdges(Fforward,ToBuild1,RevOri1,WES);
   
#ifdef OCCT_DEBUG
  Standard_Integer iF; Standard_Boolean tSPS = GtraceSPS(Fforward,iF);
  if(tSPS) WES.DumpSS();
#endif

  // Create a Face Builder FBU
  // ------------------------
  TopOpeBRepBuild_FaceBuilder FBU;
  FBU.InitFaceBuilder(WES,Fforward,Standard_False); //forceclass = False

  // Build the new faces
  // -------------------
  TopTools_ListOfShape& FaceList = ChangeMerged(Fforward,ToBuild1);
  MakeFaces(Fforward,FBU,FaceList);

  // connect new faces as faces built <ToBuild1> on LF1 faces
  // --------------------------------------------------------
  for (itLF1.Initialize(LF1); itLF1.More(); itLF1.Next()) {
    TopoDS_Shape Fcur = itLF1.Value();
    MarkSplit(Fcur,ToBuild1);
    TopTools_ListOfShape& FL = ChangeSplit(Fcur,ToBuild1);
    if ( ConnectTo1 ) FL = FaceList;
  }
  
  // connect new faces as faces built <ToBuild2> on LF2 faces
  // --------------------------------------------------------
  for (itLF2.Initialize(LF2); itLF2.More(); itLF2.Next()) {
    TopoDS_Shape Fcur = itLF2.Value();
    MarkSplit(Fcur,ToBuild2);
    TopTools_ListOfShape& FL = ChangeSplit(Fcur,ToBuild2);
    if ( ConnectTo2 ) FL = FaceList;
  }

} // SplitFace1

//=======================================================================
//function : SplitFace2
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Builder::SplitFace2(const TopoDS_Shape& Foriented,
					 const TopAbs_State ToBuild1,
					 const TopAbs_State ToBuild2)
{
  //                              process  connect  connect
  // operation tobuild1 tobuild2  face F   to 1     to 2
  // --------- -------- --------  -------  -------  -------
  // common    IN       IN        yes      yes      yes
  // fuse      OUT      OUT       yes      yes      yes
  // cut 1-2   OUT      IN        yes      yes      no
  // cut 2-1   IN       OUT       yes      yes      no           
  //                                                       
  Standard_Boolean tosplit = ToSplit(Foriented,ToBuild1);
  if ( ! tosplit ) return;
  
  Standard_Boolean RevOri1 = Reverse(ToBuild1,ToBuild2);
  Standard_Boolean RevOri2 = Reverse(ToBuild2,ToBuild1);
  Standard_Boolean ConnectTo1 = Standard_True;
  Standard_Boolean ConnectTo2 = Standard_False;

  // work on a FORWARD face <Fforward>
  TopoDS_Shape Fforward = Foriented; 
  myBuildTool.Orientation(Fforward,TopAbs_FORWARD);

  TopTools_ListOfShape LF1 ; //liste des faces de 1   samedomain
  TopTools_ListOfShape LF2 ; //liste des faces de   2 samedomain
  LF1.Append(Fforward);
  FindSameDomain(LF1,LF2);
  Standard_Integer n1 = LF1.Extent();
  Standard_Integer n2 = LF2.Extent();
  
#ifdef OCCT_DEBUG
  Standard_Boolean tSPF = TopOpeBRepBuild_GettraceSPF();
//  Standard_Integer iFace = myDataStructure->Shape(Foriented);
  if (tSPF) {
    cout<<endl;
    GdumpSHASTA(Foriented,ToBuild1,"=== SplitFace ");
    GdumpSAMDOM(LF1, (char *) "samedomain 1 : ");
    GdumpSAMDOM(LF2, (char *) "samedomain 2 : ");
  }
#endif
  
  // SplitFace on a face having other same domained faces on the
  // other shape : do not reverse orientation of faces in FillFace
  if (!n2) RevOri1 = Standard_False;
  if (!n1) RevOri2 = Standard_False;

  TopTools_ListOfShape LFSO; //liste des faces de 1,2 samedomainsameorientation
  TopTools_ListOfShape LFOO; //liste des faces de 1,2 samedomainoppositeorient

  // LFSO : faces des shapes 1 ou 2, de meme orientation que Fforward.
  // LFOO : faces des shapes 1 ou 2, d'orientation contraire que Fforward.
  LFSO.Append(Fforward);
  FindSameDomainSameOrientation(LFSO,LFOO);

  TopTools_ListOfShape LFSO1,LFOO1; // same domain, same orientation, et du shape de F
  TopTools_ListOfShape LFSO2,LFOO2; // "" "",du shape autre que celui de F

  // on construit les parties ToBuild1 de F
  Standard_Integer rankF = ShapeRank(Foriented);
  Standard_Integer rankX = (rankF) ? ((rankF == 1) ? 2 : 1) : 0;

  FindSameRank(LFSO,rankF,LFSO1);
  FindSameRank(LFOO,rankF,LFOO1);
  FindSameRank(LFSO,rankX,LFSO2);
  FindSameRank(LFOO,rankX,LFOO2);

#ifdef OCCT_DEBUG
  if ( tSPF ) {
    GdumpSAMDOM(LFSO1, (char *) "LFSO1 : ");
    GdumpSAMDOM(LFOO1, (char *) "LFOO1 : ");
    GdumpSAMDOM(LFSO2, (char *) "LFSO2 : ");
    GdumpSAMDOM(LFOO2, (char *) "LFOO2 : ");
  }
#endif

  TopAbs_State tob1 = ToBuild1;
  TopAbs_State tob2 = ToBuild2;
  TopAbs_State tob1comp = (ToBuild1 == TopAbs_IN) ? TopAbs_OUT : TopAbs_IN;
  TopAbs_State tob2comp = (ToBuild2 == TopAbs_IN) ? TopAbs_OUT : TopAbs_IN;
  TopTools_ListIteratorOfListOfShape itLF ;
  
  // --------------------------------------------------------------------
  // traitement des faces de meme orientation que Fforward dans WireEdgeSet WES1
  // --------------------------------------------------------------------
  TopOpeBRepBuild_WireEdgeSet WES1(Fforward,this);

  // traitement des faces de 1 same domain, same orientation que F : LFSO1
  for (itLF.Initialize(LFSO1); itLF.More(); itLF.Next()) {
    const TopoDS_Shape& Fcur = itLF.Value();
//                            myDataStructure->Shape(Fcur);//DEB
    // les wires de Fcur sont a comparer avec les faces de 2
    FillFace(Fcur,tob1,LF2,tob2,WES1,RevOri1);
  }

  // traitement des faces de 2 same domain, same orientation que F : LFSO2
  for (itLF.Initialize(LFSO2); itLF.More(); itLF.Next()) {
    const TopoDS_Shape& Fcur = itLF.Value();
//                            myDataStructure->Shape(Fcur);//DEB
    // les wires de Fcur sont a comparer avec les faces de 1
    FillFace(Fcur,tob2,LF1,tob1,WES1,RevOri2);
  }

  // traitement des faces de 1 same domain, oppo orientation que F : LFOO1
  for (itLF.Initialize(LFOO1); itLF.More(); itLF.Next()) {
    const TopoDS_Shape& Fcur = itLF.Value();
//                            myDataStructure->Shape(Fcur);//DEB
    // les wires de Fcur sont a comparer avec les faces de 2
    FillFace(Fcur,tob1comp,LF2,ToBuild2,WES1,!RevOri1);
  }

  // traitement des faces de 2 same domain, oppo orientation que F : LFOO2
  for (itLF.Initialize(LFOO2); itLF.More(); itLF.Next()) {
    const TopoDS_Shape& Fcur = itLF.Value();
//                      myDataStructure->Shape(Fcur);//DEB
    // les wires de Fcur sont a comparer avec les faces de 1
    FillFace(Fcur,tob2comp,LF1,ToBuild1,WES1,!RevOri2);
  }
  
  // Add the intersection edges to edge set WES1
  // ------------------------------------------
  AddIntersectionEdges(Fforward,ToBuild1,RevOri1,WES1);

  // Create a Face Builder FBU1
  // ------------------------
  TopOpeBRepBuild_FaceBuilder FBU1(WES1,Fforward);

  // Build the new faces
  // -------------------
  TopTools_ListOfShape& FaceList1 = ChangeMerged(Fforward,ToBuild1);
  MakeFaces(Fforward,FBU1,FaceList1);

  // connect new faces as faces built <ToBuild1> on LF1 faces
  // --------------------------------------------------------
  for (itLF.Initialize(LF1); itLF.More(); itLF.Next()) {
    TopoDS_Shape Fcur = itLF.Value();
    MarkSplit(Fcur,ToBuild1);
    TopTools_ListOfShape& FL = ChangeSplit(Fcur,ToBuild1);
    if ( ConnectTo1 ) FL = FaceList1;
  }

  // --------------------------------------------------------------------
  // traitement des faces de meme orientation que Fforward dans WireEdgeSet WES2
  // --------------------------------------------------------------------
  TopOpeBRepBuild_WireEdgeSet WES2(Fforward,this);

  // traitement des faces de 1 same domain, same orientation que F : LFSO1
  for (itLF.Initialize(LFSO1); itLF.More(); itLF.Next()) {
    const TopoDS_Shape& Fcur = itLF.Value();
//                            myDataStructure->Shape(Fcur);//DEB
    // les wires de Fcur sont a comparer avec les faces de 2
    FillFace(Fcur,tob1comp,LF2,tob2,WES2,!RevOri1);
  }

  // traitement des faces de 2 same domain, same orientation que F : LFSO2
  for (itLF.Initialize(LFSO2); itLF.More(); itLF.Next()) {
    const TopoDS_Shape& Fcur = itLF.Value();
//                            myDataStructure->Shape(Fcur);//DEB
    // les wires de Fcur sont a comparer avec les faces de 1
    FillFace(Fcur,tob2comp,LF1,tob1,WES2,!RevOri2);
  }

  // traitement des faces de 1 same domain, oppo orientation que F : LFOO1
  for (itLF.Initialize(LFOO1); itLF.More(); itLF.Next()) {
    const TopoDS_Shape& Fcur = itLF.Value();
//                            myDataStructure->Shape(Fcur);//DEB
    // les wires de Fcur sont a comparer avec les faces de 2
    FillFace(Fcur,tob1,LF2,ToBuild2,WES2,RevOri1);
  }

  // traitement des faces de 2 same domain, oppo orientation que F : LFOO2
  for (itLF.Initialize(LFOO2); itLF.More(); itLF.Next()) {
    const TopoDS_Shape& Fcur = itLF.Value();
//                            myDataStructure->Shape(Fcur);//DEB
    // les wires de Fcur sont a comparer avec les faces de 1
    FillFace(Fcur,tob2,LF1,ToBuild1,WES2,RevOri2);
  }
  
  // Add the intersection edges to edge set WES2
  // ------------------------------------------
  AddIntersectionEdges(Fforward,ToBuild2,RevOri2,WES2);
   
  // Create a Face Builder FBU2
  // -------------------------
  TopOpeBRepBuild_FaceBuilder FBU2(WES2,Fforward);

  // Build the new faces
  // -------------------
  TopTools_ListOfShape& FaceList2 = ChangeMerged(Fforward,ToBuild2);
  MakeFaces(Fforward,FBU2,FaceList2);

  // connect new faces as faces built <ToBuild2> on LF2 faces
  // --------------------------------------------------------
  for (itLF.Initialize(LF2); itLF.More(); itLF.Next()) {
    TopoDS_Shape Fcur = itLF.Value();
    MarkSplit(Fcur,ToBuild2);
    TopTools_ListOfShape& FL = ChangeSplit(Fcur,ToBuild2);
    if ( ConnectTo2 ) FL = FaceList2;
  }

} // SplitFace2

//=======================================================================
//function : SplitSolid
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Builder::SplitSolid(const TopoDS_Shape& S1oriented,
					 const TopAbs_State ToBuild1,
					 const TopAbs_State ToBuild2)
{
  //modified by IFV for treating shell
  Standard_Boolean tosplit = Standard_False;
  Standard_Boolean IsShell = (S1oriented.ShapeType() == TopAbs_SHELL);
  if(IsShell) {
    TopExp_Explorer ex;
    ex.Init(S1oriented, TopAbs_FACE);
    for (; ex.More(); ex.Next()) {
      const TopoDS_Shape& sh = ex.Current();
      tosplit = ToSplit(sh,ToBuild1);
      if(tosplit) break;
    }
  }
  else tosplit = ToSplit(S1oriented,ToBuild1);

  if ( ! tosplit ) return;
  // end IFV

  Standard_Boolean RevOri1 = Reverse(ToBuild1,ToBuild2);
  Standard_Boolean RevOri2 = Reverse(ToBuild2,ToBuild1);
  Standard_Boolean ConnectTo1 = Standard_True;
  Standard_Boolean ConnectTo2 = Standard_False;

  // work on a FORWARD solid <S1forward>
  TopoDS_Shape S1forward = S1oriented; 
  myBuildTool.Orientation(S1forward,TopAbs_FORWARD);
  
  // build the list of solids to split : LS1, LS2
  TopTools_ListOfShape LS1,LS2;
  LS1.Append(S1forward);
  FindSameDomain(LS1,LS2);
  Standard_Integer n1 = LS1.Extent();
  Standard_Integer n2 = LS2.Extent();
  
  if (!n2) RevOri1 = Standard_False;
  if (!n1) RevOri2 = Standard_False;
  
  // Create a face set <FS> connected by edges
  // -----------------------------------------
  TopOpeBRepBuild_ShellFaceSet SFS;
  
#ifdef OCCT_DEBUG
  Standard_Boolean tSPS = TopOpeBRepBuild_GettraceSPS();
//  Standard_Integer iSolid = myDataStructure->Shape(S1oriented);
  if (tSPS) {
    cout<<endl;
    GdumpSHASTA(S1oriented,ToBuild1,"___ SplitSolid ");
    GdumpSAMDOM(LS1, (char *) "1 : "); 
    GdumpSAMDOM(LS2, (char *) "2 : ");
  }
  SFS.DEBNumber(GdumpSHASETindex());
#endif

  STATIC_SOLIDINDEX = 1;
  TopTools_ListIteratorOfListOfShape itLS1;
  for (itLS1.Initialize(LS1); itLS1.More(); itLS1.Next()) {
    TopoDS_Shape Scur = itLS1.Value();
    FillSolid(Scur,ToBuild1,LS2,ToBuild2,SFS,RevOri1);
  }
  
  STATIC_SOLIDINDEX = 2;
  TopTools_ListIteratorOfListOfShape itLS2;
  for (itLS2.Initialize(LS2); itLS2.More(); itLS2.Next()) {
    TopoDS_Shape Scur = itLS2.Value();
    FillSolid(Scur,ToBuild2,LS1,ToBuild1,SFS,RevOri2);
  }
  
  // Add the intersection surfaces
  // -----------------------------
  if (myDataStructure->NbSurfaces() > 0) {
    TopOpeBRepDS_SurfaceIterator SSurfaces = myDataStructure->SolidSurfaces(S1forward);
    for (; SSurfaces.More(); SSurfaces.Next()) {
      Standard_Integer iS = SSurfaces.Current();
      const TopTools_ListOfShape& LnewF = NewFaces(iS);
      for (TopTools_ListIteratorOfListOfShape Iti(LnewF); Iti.More(); Iti.Next()) {
	TopoDS_Shape aFace = Iti.Value();
	TopAbs_Orientation ori = SSurfaces.Orientation(ToBuild1);
	myBuildTool.Orientation(aFace,ori);
	
#ifdef OCCT_DEBUG
	if (tSPS){
          TCollection_AsciiString ss("--- SplitSolid ");
          ss = ss + SFS.DEBNumber() + " AddElement SFS+ face ";
	  GdumpSHA(aFace,(Standard_Address)ss.ToCString());
	  cout<<" ";TopAbs::Print(ToBuild1,cout)<<" : 1 face ";
	  TopAbs::Print(ori,cout); cout<<endl;
	}
#endif
	SFS.AddElement(aFace);
      }
    }
  }

  // Create a Solid Builder SOBU
  // -------------------------
  TopOpeBRepBuild_SolidBuilder SOBU(SFS);

  // Build the new solids on S1
  // --------------------------
  TopTools_ListOfShape& SolidList = ChangeMerged(S1oriented,ToBuild1);
  if(IsShell)
    MakeShells(SOBU,SolidList);
  else
    MakeSolids(SOBU,SolidList);

  // connect list of new solids <SolidList> as solids built on LS1 solids
  // --------------------------------------------------------------------

  for (itLS1.Initialize(LS1); itLS1.More(); itLS1.Next()) {
    TopoDS_Shape Scur = itLS1.Value();
    MarkSplit(Scur,ToBuild1);
    TopTools_ListOfShape& SL = ChangeSplit(Scur,ToBuild1);
    if ( ConnectTo1 ) SL = SolidList;

  }
  
  // connect list of new solids <SolidList> as solids built on LS2 solids
  // --------------------------------------------------------------------
  for (itLS2.Initialize(LS2); itLS2.More(); itLS2.Next()) {
    TopoDS_Shape Scur = itLS2.Value();
    MarkSplit(Scur,ToBuild2);
    TopTools_ListOfShape& SL = ChangeSplit(Scur,ToBuild2);
    if ( ConnectTo2 ) SL = SolidList;
  }

} // SplitSolid

static Standard_Boolean FUN_touched(const TopOpeBRepDS_DataStructure& BDS,const TopoDS_Edge& EOR)
{
  TopoDS_Vertex vf,vl; TopExp::Vertices(EOR,vf,vl);
  Standard_Boolean hvf = BDS.HasShape(vf);
  Standard_Boolean hvl = BDS.HasShape(vl);
  return (hvf || hvl);
}

//=======================================================================
//function : SplitShapes
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::SplitShapes(TopOpeBRepTool_ShapeExplorer& Ex,
					  const TopAbs_State ToBuild1, 
					  const TopAbs_State ToBuild2,
					  TopOpeBRepBuild_ShapeSet& aSet,
					  const Standard_Boolean RevOri)
{
  TopoDS_Shape aShape;
  TopAbs_Orientation newori;

  for (; Ex.More(); Ex.Next()) {
    aShape = Ex.Current();

    // compute new orientation <newori> to give to the new shapes
    newori = Orient(myBuildTool.Orientation(aShape),RevOri);

    TopAbs_ShapeEnum t = aShape.ShapeType();

    if      ( t == TopAbs_SOLID || t == TopAbs_SHELL )
      SplitSolid(aShape,ToBuild1,ToBuild2);
    else if ( t == TopAbs_FACE  ) SplitFace(aShape,ToBuild1,ToBuild2);
    else if ( t == TopAbs_EDGE  ) SplitEdge(aShape,ToBuild1,ToBuild2);
    else continue;

    if ( IsSplit(aShape,ToBuild1) ) {
      TopoDS_Shape newShape;
      TopTools_ListIteratorOfListOfShape It;
      //----------------------- IFV
      Standard_Boolean IsLSon = Standard_False;
      //----------------------- IFV
      const TopTools_ListOfShape& LS = Splits(aShape,ToBuild1);
      //----------------------- IFV
      if(t == TopAbs_EDGE && ToBuild1 == TopAbs_IN && LS.Extent() == 0) {
	const TopTools_ListOfShape& LSon = Splits(aShape,TopAbs_ON);
	It.Initialize(LSon);
	IsLSon = Standard_True;
      }
      else {
	It.Initialize(LS);
      }
      //----------------------- IFV
      for (; It.More(); It.Next()) {
	newShape = It.Value();
	myBuildTool.Orientation(newShape,newori);
#ifdef OCCT_DEBUG
//	TopAbs_ShapeEnum tns = TopType(newShape);
#endif
	//----------------------- IFV
	if(IsLSon) {
	  Standard_Boolean add = Standard_True;
	  if ( !myListOfFace.IsEmpty()) { // 2d pur
	    add = KeepShape(newShape,myListOfFace,ToBuild1);
	  }
	  if(add) aSet.AddStartElement(newShape);

	}
	else {
	//----------------------- IFV
	  aSet.AddStartElement(newShape);
	}
      }
    }
    else {
      // aShape n'a pas de devenir de split par ToBuild1
      // on construit les parties ToBuild1 de aShape (de S1)
      Standard_Boolean add = Standard_True;
      Standard_Boolean testkeep = Standard_False;
      Standard_Boolean isedge = (t == TopAbs_EDGE);
      Standard_Boolean hs = (myDataStructure->HasShape(aShape));
      Standard_Boolean hg = (myDataStructure->HasGeometry(aShape));
      
      testkeep = isedge && hs && (!hg);
      
      // xpu010399 : USA60299 (!hs)&&(!hg), but vertex on bound is touched (v7)
      //             -> testkeep
      Standard_Boolean istouched = isedge && (!hs) && (!hg);
      if (istouched) istouched = FUN_touched(myDataStructure->DS(),TopoDS::Edge(aShape));
      testkeep = testkeep || istouched;

      if (testkeep) { 
	if ( !myListOfFace.IsEmpty()) { // 2d pur
	  Standard_Boolean keep = KeepShape(aShape,myListOfFace,ToBuild1);
	  add = keep;
	}
	else { // 3d
	  // on classifie en solide uniqt si 
	  // E dans la DS et E a ete purgee de ses interfs car en bout
	  TopoDS_Shape sol;
	  if (STATIC_SOLIDINDEX == 1) sol = myShape2;
	  else                        sol = myShape1;
	  if ( !sol.IsNull() ) {	    
	    Standard_Real first,last;
	    Handle(Geom_Curve) C3D;
	    C3D = BRep_Tool::Curve(TopoDS::Edge(aShape),first,last);
	    if ( !C3D.IsNull() ) {
	      Standard_Real tt = 0.127956477;
	      Standard_Real par = (1-tt)*first + tt*last;
	      gp_Pnt P3D = C3D->Value(par);
	      Standard_Real tol3d = Precision::Confusion();
	      BRepClass3d_SolidClassifier SCL(sol,P3D,tol3d); 
	      TopAbs_State state = SCL.State();
	      add = (state == ToBuild1);
	    }
	    else {
	      throw Standard_ProgramError("SplitShapes no 3D curve on edge");
	      // NYI pas de courbe 3d : prendre un point sur (courbe 2d,face)
	    }
	  }
	  else { //  sol.IsNull
	    add = Standard_True;
	  }
	}
      }
      if ( add ) {
	myBuildTool.Orientation(aShape,newori);
	aSet.AddElement(aShape);
      }
    }

  } // Ex.More

} // SplitShapes

//=======================================================================
//function : FillShape
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::FillShape(const TopoDS_Shape& S1,
					const TopAbs_State ToBuild1,
					const TopTools_ListOfShape& LS2,
					const TopAbs_State ToBuild2,
					TopOpeBRepBuild_ShapeSet& aSet,
					const Standard_Boolean In_RevOri)
{
  Standard_Boolean RevOri = In_RevOri;
  TopAbs_ShapeEnum t = S1.ShapeType();
  TopAbs_ShapeEnum t1=TopAbs_COMPOUND,t11=TopAbs_COMPOUND;

  if      (t == TopAbs_FACE )  { 
    t1 = TopAbs_WIRE;
    t11 = TopAbs_EDGE;
  }
  else if (t == TopAbs_SOLID || t == TopAbs_SHELL) {
    t1 = TopAbs_SHELL; 
    t11 = TopAbs_FACE; 
  }

  // if the shape S1 is a SameDomain one, get its orientation compared
  // with the shape taken as reference for all of the SameDomain shape of S1.
  Standard_Boolean hsd = myDataStructure->HasSameDomain(S1);
  if (hsd) {
    TopOpeBRepDS_Config ssc = myDataStructure->SameDomainOrientation(S1);
    if ( ssc == TopOpeBRepDS_DIFFORIENTED ) {
      RevOri = ! RevOri;
#ifdef OCCT_DEBUG
//      Standard_Integer iFace = myDataStructure->Shape(S1);
//      cout<<endl<<"********** ";
//      cout<<"retournement d'orientation de ";TopAbs::Print(t,cout);
//      cout<<" "<<iFace<<endl;
#endif
    }
  }

  // work on a FORWARD shape <aShape>
  TopoDS_Shape aShape = S1; 
  myBuildTool.Orientation(aShape,TopAbs_FORWARD);

  TopoDS_Shape aSubShape;
  TopAbs_Orientation newori;

  // Explore the SubShapes of type <t1>
  for (TopOpeBRepTool_ShapeExplorer ex1(aShape,t1); ex1.More(); ex1.Next()) {
    aSubShape = ex1.Current();

    if ( ! myDataStructure->HasShape(aSubShape) ) {
      // SubShape is not in DS : classify it with shapes of LS2
      Standard_Boolean keep = KeepShape(aSubShape,LS2,ToBuild1);
      if (keep) {
	newori = Orient(myBuildTool.Orientation(aSubShape),RevOri);
	myBuildTool.Orientation(aSubShape,newori);
	aSet.AddShape(aSubShape);
      }
    }
    else {
      // SubShape has geometry : split the <t11> SubShapes of the SubShape
      TopOpeBRepTool_ShapeExplorer ex11(aSubShape,t11);
      SplitShapes(ex11,ToBuild1,ToBuild2,aSet,RevOri);
    } 
  } // exploration ot SubShapes of type <t1> of shape <S1>

} // FillShape


//=======================================================================
//function : FillFace
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::FillFace(const TopoDS_Shape& F1, 
				       const TopAbs_State ToBuild1,
				       const TopTools_ListOfShape& LF2,
				       const TopAbs_State ToBuild2,
				       TopOpeBRepBuild_WireEdgeSet& WES,
				       const Standard_Boolean RevOri)
{
#ifdef OCCT_DEBUG
  Standard_Boolean tSPF = TopOpeBRepBuild_GettraceSPF();
//  Standard_Integer iFace = myDataStructure->Shape(F1);
  if(tSPF){cout<<endl;}
  if(tSPF){GdumpSHASTA(F1,ToBuild1,"=-= FillFace ");}
#endif
  myListOfFace = LF2;
  FillShape(F1,ToBuild1,LF2,ToBuild2,WES,RevOri);
  myListOfFace.Clear();
} // FillFace


//=======================================================================
//function : FillSolid
//purpose  : load shells and faces from the solid in the ShellFaceSet <aSet>
//=======================================================================
void TopOpeBRepBuild_Builder::FillSolid(const TopoDS_Shape& S1, 
					const TopAbs_State ToBuild1, 
					const TopTools_ListOfShape& LS2,
					const TopAbs_State ToBuild2, 
					TopOpeBRepBuild_ShapeSet& aSet,
					const Standard_Boolean RevOri)
{
  FillShape(S1,ToBuild1,LS2,ToBuild2,aSet,RevOri);
} // FillSolid


//=======================================================================
//function : FillVertexSet
//purpose  : private
//=======================================================================
void TopOpeBRepBuild_Builder::FillVertexSet(TopOpeBRepDS_PointIterator& IT,
					    const TopAbs_State ToBuild,
					    TopOpeBRepBuild_PaveSet& PVS) const
{
  for (; IT.More(); IT.Next()) {
    FillVertexSetOnValue(IT,ToBuild,PVS);
  }
}


//=======================================================================
//function : FillVertexSetOnValue
//purpose  : private
//=======================================================================
void TopOpeBRepBuild_Builder::FillVertexSetOnValue
(const TopOpeBRepDS_PointIterator& IT,
 const TopAbs_State ToBuild,
 TopOpeBRepBuild_PaveSet& PVS) const
{
  TopoDS_Shape V;
  
  // ind = index of new point or existing vertex
  Standard_Integer ind = IT.Current();
  Standard_Boolean ispoint  = IT.IsPoint();
  //**!
  //if (ispoint) V = NewVertex(ind);
  if (ispoint && ind <= myDataStructure->NbPoints()) V = NewVertex(ind);
  //**!
  else         V = myDataStructure->Shape(ind);
  Standard_Real      par = IT.Parameter();
  TopAbs_Orientation ori = IT.Orientation(ToBuild);
  
  Standard_Boolean keep = Standard_True;
  //    if (ori==TopAbs_EXTERNAL || ori==TopAbs_INTERNAL) keep = Standard_False;
  
  if ( keep ) {
    myBuildTool.Orientation(V,ori);
    Handle(TopOpeBRepBuild_Pave) PV = new TopOpeBRepBuild_Pave(V,par,Standard_False);
    PVS.Append(PV);
  }
  
#ifdef OCCT_DEBUG
  const TopoDS_Edge& EDEB = PVS.Edge();
  Standard_Integer iE; Standard_Boolean tSPS = GtraceSPS(EDEB,iE);
  if (tSPS) {
    if (keep) cout<<"+"; else cout<<"-";
    if (ispoint) cout<<" PDS "; else cout<<" VDS ";
    cout<<ind<<" : "; GdumpORIPARPNT(ori,par,BRep_Tool::Pnt(TopoDS::Vertex(V)));
    cout<<endl;
  }
#endif
}
