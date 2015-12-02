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
#include <TopExp.hxx>
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
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopOpeBRepDS_SurfaceIterator.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepBuild_GetcontextSPEON();
#endif

//=======================================================================
//function : GToSplit
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_Builder::GToSplit(const TopoDS_Shape& S,const TopAbs_State TB) const 
{
  Standard_Boolean issplit = IsSplit(S,TB);
  Standard_Boolean facshap = myDataStructure->HasShape(S) && (S.ShapeType() == TopAbs_FACE);
  Standard_Boolean hasgeom = myDataStructure->HasGeometry(S);
  Standard_Boolean hassame = myDataStructure->HasSameDomain(S);
  Standard_Boolean tosplit = (!issplit) && (facshap || hasgeom || hassame);

#ifdef OCCT_DEBUG
  Standard_Integer iS; Standard_Boolean tSPS = GtraceSPS(S,iS);
  if (tSPS) { 
    cout<<"GToSplit ";GdumpSHA(S);cout<<" ";TopAbs::Print(TB,cout);
    cout<<" "<<tosplit<<" : !issplit "<<(!issplit);
    cout<<" && (facshap || hasgeom || hassame) ";
    cout<<"("<<facshap<<" || "<<hasgeom<<" || "<<hassame<<")"<<endl;
    cout.flush();
  }
#endif
  
#ifdef OCCT_DEBUG
  if (TopOpeBRepBuild_GetcontextSPEON()) { //CONTEXT
    tSPS = Standard_True; //CONTEXT
    Standard_Boolean hasON = Standard_False; //CONTEXT
    Standard_Boolean isE = (S.ShapeType() == TopAbs_EDGE); //CONTEXT
    if (isE) { //CONTEXT
      const TopoDS_Edge& E = TopoDS::Edge(S); //CONTEXT
      Standard_Boolean issE = myDataStructure->DS().IsSectionEdge(E); //CONTEXT
      if (issE) { //CONTEXT
	Standard_Boolean issplitON = IsSplit(E,TopAbs_ON); //CONTEXT
	if (issplitON) { //CONTEXT
	  Standard_Integer n = Splits(E,TopAbs_ON).Extent(); //CONTEXT
	  hasON = (n>0); //CONTEXT
	} //CONTEXT
      } //CONTEXT
    } //CONTEXT
    Standard_Boolean tosplitH = tosplit || hasON; //CONTEXT
    if(tSPS){cout<<"GToSplit context SPEON";} //CONTEXT
    if(tSPS){cout<<" "<<tosplitH<<" : tosplit "<<tosplit;} //CONTEXT
    if(tSPS){cout<<" || hasON "<<hasON<<endl;} //CONTEXT
  } //CONTEXT
#endif
  
  return tosplit;
} // GToSplit


//=======================================================================
//function : GToMerge
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_Builder::GToMerge(const TopoDS_Shape& S) const 
{
  TopAbs_State TB = TopAbs_IN;
  
  Standard_Boolean ismerged = IsMerged(S,TB);
  Standard_Boolean hassame = myDataStructure->HasSameDomain(S);
  Standard_Boolean tomerge = (!ismerged && hassame);
  
#ifdef OCCT_DEBUG
  Standard_Integer iS; Standard_Boolean tSPS = GtraceSPS(S,iS);
  if(tSPS){cout<<"GToMerge ";GdumpSHA(S);cout<<" ";TopAbs::Print(TB,cout);}
  if(tSPS){cout<<" "<<tomerge<<" : !ismerged "<<(!ismerged)<<" && hassame "<<hassame<<endl;}
#endif
  
  return tomerge;
} // GToMerge

//=======================================================================
//function : GTakeCommonOfSame
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_Builder::GTakeCommonOfSame(const TopOpeBRepBuild_GTopo& G)
{
  TopAbs_State t1,t2; G.StatesON(t1,t2);
  Standard_Boolean sam = Standard_False;
  if      (t1 == TopAbs_OUT && t2 == TopAbs_OUT) sam = Standard_True;
  else if (t1 == TopAbs_OUT && t2 == TopAbs_IN ) sam = Standard_False;
  else if (t1 == TopAbs_IN  && t2 == TopAbs_OUT) sam = Standard_False;
  else if (t1 == TopAbs_IN  && t2 == TopAbs_IN ) sam = Standard_True;

  return sam;
}

//=======================================================================
//function : GTakeCommonOfDiff
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_Builder::GTakeCommonOfDiff(const TopOpeBRepBuild_GTopo& G)
{
  TopAbs_State t1,t2; G.StatesON(t1,t2);
  Standard_Boolean dif = Standard_False;
  if      (t1 == TopAbs_OUT && t2 == TopAbs_OUT) dif = Standard_False;
  else if (t1 == TopAbs_OUT && t2 == TopAbs_IN ) dif = Standard_True;
  else if (t1 == TopAbs_IN  && t2 == TopAbs_OUT) dif = Standard_True;
  else if (t1 == TopAbs_IN  && t2 == TopAbs_IN ) dif = Standard_False;

  return dif;
}

//=======================================================================
//function : GFindSamDom
//purpose  : complete the lists L1,L2 with the shapes of the DS
//           having same domain
//=======================================================================
void TopOpeBRepBuild_Builder::GFindSamDom(const TopoDS_Shape& S,TopTools_ListOfShape& L1,TopTools_ListOfShape& L2) const 
{
  L1.Clear(); L2.Clear();
  L1.Append(S);
  GFindSamDom(L1,L2);
}

//=======================================================================
//function : GFindSamDom
//purpose  : complete the lists L1,L2 with the shapes of the DS
//           having same domain
//=======================================================================
void TopOpeBRepBuild_Builder::GFindSamDom(TopTools_ListOfShape& L1,TopTools_ListOfShape& L2) const 
{
  Standard_Integer i;
  Standard_Integer nl1 = L1.Extent(), nl2 = L2.Extent();
  
  while ( nl1 > 0 || nl2 > 0 )  {
    
    TopTools_ListIteratorOfListOfShape it1(L1);
    for (i=1 ; i<=nl1; i++) {
      const TopoDS_Shape& S1 = it1.Value();
#ifdef OCCT_DEBUG
//      Standard_Integer iS1 = myDataStructure->Shape(S1);
#endif
      TopTools_ListIteratorOfListOfShape itsd(myDataStructure->SameDomain(S1));
      for (; itsd.More(); itsd.Next() ) {
	const TopoDS_Shape& S2 = itsd.Value();
#ifdef OCCT_DEBUG
//	Standard_Integer iS2 = myDataStructure->Shape(S2);
#endif
	Standard_Boolean found = GContains(S2,L2);
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
//      Standard_Integer iS2 = myDataStructure->Shape(S2);
#endif
      TopTools_ListIteratorOfListOfShape itsd(myDataStructure->SameDomain(S2));
      for (; itsd.More(); itsd.Next() ) {
	const TopoDS_Shape& S1 = itsd.Value();
#ifdef OCCT_DEBUG
//	Standard_Integer iS1 = myDataStructure->Shape(S1);
#endif
	Standard_Boolean found = GContains(S1,L1);
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
//function : GFindSamDomSODO
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::GFindSamDomSODO(const TopoDS_Shape& S,TopTools_ListOfShape& LSO,TopTools_ListOfShape& LDO) const 
{
  LSO.Clear();
  LDO.Clear();
  LSO.Append(S);
  GFindSamDomSODO(LSO,LDO);
}

//=======================================================================
//function : GFindSamDomSODO
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::GFindSamDomSODO(TopTools_ListOfShape& LSO,TopTools_ListOfShape& LDO) const 
{
  TopTools_ListIteratorOfListOfShape it;
  it.Initialize(LSO);
  if ( ! it.More() ) return;
  const TopoDS_Shape& sref = it.Value();
#ifdef OCCT_DEBUG
//  Standard_Integer  iref = myDataStructure->SameDomainReference(sref);
#endif
  TopOpeBRepDS_Config oref = myDataStructure->SameDomainOrientation(sref);
  
  GFindSamDom(LSO,LDO);
  
#ifdef OCCT_DEBUG
  Standard_Integer iS; Standard_Boolean tSPS = GtraceSPS(sref,iS);
  if(tSPS) {
    TCollection_AsciiString ss("GFindSamDom result on ");  
    GdumpSHA(sref,(Standard_Address)ss.ToCString());cout<<endl;
    GdumpSAMDOM(LSO, (char *) "L1 : ");
    GdumpSAMDOM(LDO, (char *) "L2 : ");
  }
#endif
  
  TopTools_ListOfShape LLSO,LLDO;
  
  for (it.Initialize(LSO); it.More(); it.Next() ) {
    const TopoDS_Shape& s = it.Value();
    TopOpeBRepDS_Config o = myDataStructure->SameDomainOrientation(s);
#ifdef OCCT_DEBUG
//    Standard_Integer iS = myDataStructure->Shape(s);
#endif
    if      ( o == oref && !GContains(s,LLSO) ) LLSO.Append(s);
    else if ( o != oref && !GContains(s,LLDO) ) LLDO.Append(s);
  }
  
  for (it.Initialize(LDO); it.More(); it.Next() ) {
    const TopoDS_Shape& s = it.Value();
    TopOpeBRepDS_Config o = myDataStructure->SameDomainOrientation(s);
#ifdef OCCT_DEBUG
//    Standard_Integer iS = myDataStructure->Shape(s);
#endif
    if      ( o == oref && !GContains(s,LLSO) ) LLSO.Append(s);
    else if ( o != oref && !GContains(s,LLDO) ) LLDO.Append(s);
  }
  
  LSO = LLSO;
  LDO = LLDO;
}

//=======================================================================
//function : GMapShapes
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::GMapShapes(const TopoDS_Shape& S1,const TopoDS_Shape& S2)
{
  Standard_Boolean S1null = S1.IsNull();
  Standard_Boolean S2null = S2.IsNull();
  GClearMaps();
  if ( ! S1null ) TopExp::MapShapes(S1,myMAP1);
  if ( ! S2null ) TopExp::MapShapes(S2,myMAP2);
}

//=======================================================================
//function : GClearMaps
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::GClearMaps()
{
  myMAP1.Clear();
  myMAP2.Clear(); 
}

//=======================================================================
//function : GFindSameRank
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::GFindSameRank
(const TopTools_ListOfShape& L1,const Standard_Integer Rank,TopTools_ListOfShape& L2) const 
{
  for (  TopTools_ListIteratorOfListOfShape it1(L1); it1.More(); it1.Next() ) {
    const TopoDS_Shape& s = it1.Value();
#ifdef OCCT_DEBUG
//    Standard_Integer iS = myDataStructure->Shape(s);
#endif
    Standard_Integer r = GShapeRank(s);
    if ( r == Rank && !GContains(s,L2) ) {
      L2.Append(s);
    }
  }
}

//=======================================================================
//function : GShapeRank
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRepBuild_Builder::GShapeRank(const TopoDS_Shape& S) const 
{
  Standard_Boolean isof1 = GIsShapeOf(S,1);
  Standard_Boolean isof2 = GIsShapeOf(S,2);
  Standard_Integer ancetre = (isof1 || isof2) ? ((isof1) ? 1 : 2) : 0;
  return ancetre;
}

//=======================================================================
//function : GIsShapeOf
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_Builder::GIsShapeOf(const TopoDS_Shape& S,const Standard_Integer I) const 
{
  if (S.IsNull()) return Standard_False;
  Standard_Boolean b = Standard_False;
  if      (I == 1) b = myMAP1.Contains(S);
  else if (I == 2) b = myMAP2.Contains(S);
  return b;
}

//=======================================================================
//function : GContains
//purpose  : returns True if S is in the list L.
//=======================================================================
Standard_Boolean TopOpeBRepBuild_Builder::GContains(const TopoDS_Shape& S,const TopTools_ListOfShape& L)
{
  for (TopTools_ListIteratorOfListOfShape it(L); it.More(); it.Next() ) {
    const TopoDS_Shape& SL = it.Value();
    Standard_Boolean issame = SL.IsSame(S);
    if ( issame ) return Standard_True;
  }
  return Standard_False;
}


//=======================================================================
//function : GCopyList
//purpose  : 
// copy des elements [i1..i2] de Lin dans Lou. 1er element de Lin = index 1 
//=======================================================================
void TopOpeBRepBuild_Builder::GCopyList(const TopTools_ListOfShape& Lin,const Standard_Integer I1,const Standard_Integer I2,TopTools_ListOfShape& Lou)
{
  TopTools_ListIteratorOfListOfShape it(Lin);
  Standard_Integer nadd = 0;
  for ( Standard_Integer i = 1; it.More(); it.Next(),i++ ) {
    const TopoDS_Shape& EL = it.Value();
    if ( i >= I1 && i <= I2 ) {
      Lou.Append(EL);
      nadd++;
    }
  }
}


//=======================================================================
//function : GCopyList
//purpose  : 
// copy de Lin dans Lou
//=======================================================================
void TopOpeBRepBuild_Builder::GCopyList(const TopTools_ListOfShape& Lin,TopTools_ListOfShape& Lou)
{
  const Standard_Integer I1 = 1;
  const Standard_Integer I2 = Lin.Extent();
  GCopyList(Lin,I1,I2,Lou);
}
