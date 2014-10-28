// Created on: 1996-09-19
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

#include <TestTopOpe_HDSDisplayer.hxx>

#include <TopOpeBRepDS_Curve.hxx>
#include <Draw_Appli.hxx>
#include <TopExp.hxx>
#include <TestTopOpeDraw_Displayer.hxx>
#include <TestTopOpeDraw_TTOT.hxx>

//=======================================================================
//function : TestTopOpe_HDSDisplayer
//purpose  : 
//=======================================================================

TestTopOpe_HDSDisplayer::TestTopOpe_HDSDisplayer() : 
myPMap1(NULL),myPMap2(NULL)
{}

//=======================================================================
//function : TestTopOpe_HDSDisplayer
//purpose  : 
//=======================================================================

TestTopOpe_HDSDisplayer::TestTopOpe_HDSDisplayer(const Handle(TopOpeBRepDS_HDataStructure)& HDS) :
myPMap1(NULL),myPMap2(NULL)
{
  SetCurrentHDS(HDS);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TestTopOpe_HDSDisplayer::Init()
{
  if ( myHDS.IsNull() ) return; 
  myHDS->ChangeDS().Init();
}

//=======================================================================
//function : SetCurrentHDS
//purpose  : 
//=======================================================================

void TestTopOpe_HDSDisplayer::SetCurrentHDS(const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  if ( !myHDS.IsNull() ) Init();
  myHDS = HDS;
}

//=======================================================================
//function : CurrentHDS
//purpose  : 
//=======================================================================

const Handle(TopOpeBRepDS_HDataStructure)& TestTopOpe_HDSDisplayer::CurrentHDS() const 
{
  return myHDS;
}

//=======================================================================
//function : DS
//purpose  : 
//=======================================================================

const TopOpeBRepDS_DataStructure& TestTopOpe_HDSDisplayer::CurrentBDS() const 
{
  if ( !myHDS.IsNull() ) return myHDS->DS();
  else {
    static TopOpeBRepDS_DataStructure* dummypbid = NULL;
    if (!dummypbid) dummypbid = 
      (TopOpeBRepDS_DataStructure*) new (TopOpeBRepDS_DataStructure);
    return (*dummypbid);
  }
}

//=======================================================================
//function : SetShape1
//purpose  : 
//=======================================================================

void TestTopOpe_HDSDisplayer::SetShape1(const TopoDS_Shape& S1)
{
  myS1 = S1;
  if (myPMap1) myPMap1->Clear();
}

//=======================================================================
//function : Shape2
//purpose  : 
//=======================================================================

void TestTopOpe_HDSDisplayer::SetShape2(const TopoDS_Shape& S2)
{
  myS2 = S2;
  if (myPMap2) myPMap2->Clear();
}

#define FindKeep Standard_False

//=======================================================================
//function : SubShape
//purpose  : 
//=======================================================================

Standard_Boolean TestTopOpe_HDSDisplayer::SubShape(const TopoDS_Shape& S,const Standard_Integer I12)
{
  Standard_Integer h = myHDS->DS().HasShape(S,FindKeep);
  if ( !h ) return Standard_False;
  
  // returns True if <S> is a subshape of loaded shape <I12> = 1,2
  Standard_Integer rankS = myHDS->DS().AncestorRank(S);  
  if ( rankS == 1 || rankS == 2 ) {
    Standard_Boolean r = rankS == I12;
    return r;
  }

#ifdef OCCT_DEBUG
  Standard_Integer iS = myHDS->Shape(S,FindKeep);
  cout<<endl<<"***** shape "<<iS<<" has no ancestor index : use shapemaps"<<endl;
#endif
  
  if (myPMap1 == NULL) myPMap1 = new TopTools_IndexedMapOfShape();
  if ( !myS1.IsNull() ) TopExp::MapShapes(myS1,*myPMap1);
  if (myPMap2 == NULL) myPMap2 = new TopTools_IndexedMapOfShape();
  if ( !myS2.IsNull() ) TopExp::MapShapes(myS2,*myPMap2);
  Standard_Boolean r = Standard_False;
  switch (I12) {
  case 1 : r = myPMap1->Contains(S); break;
  case 2 : r = myPMap2->Contains(S); break;
  }
  return r;
}

//=======================================================================
//function : ShapeName
//purpose  : 
//=======================================================================

void TestTopOpe_HDSDisplayer::ShapeName(const Standard_Integer I,const TopoDS_Shape& S,TCollection_AsciiString& N) const 
{
  TestTopOpeDraw_TTOT::ShapeEnumToString(S.ShapeType(),N);
  N = N + "_" + TCollection_AsciiString(I);
}

//  if (S.ShapeType()==TopAbs_VERTEX) N = N + "  ";
//  switch (S.Orientation()) {
//  case TopAbs_FORWARD  : N = N + "F"; break;
//  case TopAbs_REVERSED : N = N + "R"; break;
//  case TopAbs_INTERNAL : N = N + "I"; break;
//  case TopAbs_EXTERNAL : N = N + "E"; break;
//  }

//=======================================================================
//function : AncestorMark
//purpose  : 
//=======================================================================

TCollection_AsciiString TestTopOpe_HDSDisplayer::AncestorMark(const TopoDS_Shape& S,const TCollection_AsciiString& Nin)
{
  Standard_Boolean of1 = SubShape(S,1),of2 = SubShape(S,2);
  // N = "*Nin" / "Nin*" if S is subshape of first / second shape
  TCollection_AsciiString N = ""; if (of1) N="*"; N=N+Nin; if (of2) N=N+"*";
  return N;
}

//=======================================================================
//function : ShapeDisplayName
//purpose  : 
//=======================================================================

void TestTopOpe_HDSDisplayer::ShapeDisplayName(const Standard_Integer I,const TopoDS_Shape& S,TCollection_AsciiString& N)
{
  TCollection_AsciiString name; 
  ShapeName(I,S,name);
  N = AncestorMark(S,name);
}

//=======================================================================
//function : ShapeDisplayName
//purpose  : 
//=======================================================================

void TestTopOpe_HDSDisplayer::ShapeDisplayName(const TCollection_AsciiString& NameNBRep,const TopoDS_Shape& S,TCollection_AsciiString& N)
{
  // get subshape information on S
  Standard_Boolean of1 = SubShape(S,1);
  Standard_Boolean of2 = SubShape(S,2);

  // namedisp = "*name" if shape S is a subshape of first shape
  // namedisp = "name*" if shape S is a subshape of second shape
  N = ""; 
  if (S.ShapeType()==TopAbs_VERTEX) N = N + "  ";
  if (of1) N = N + "*";
  N = N + NameNBRep;
  if (of2) N = N + "*";
}

//=======================================================================
//function : SectionEdgeName
//purpose  : 
//=======================================================================

void TestTopOpe_HDSDisplayer::SectionEdgeName(const Standard_Integer ISE,const TopoDS_Shape& S,TCollection_AsciiString& N) const 
{
  TestTopOpeDraw_TTOT::ShapeEnumToString(S.ShapeType(),N);
  N = N + "s" + "_" + TCollection_AsciiString(ISE);
}

//=======================================================================
//function : SectionEdgeDisplayName
//purpose  : 
//=======================================================================

void TestTopOpe_HDSDisplayer::SectionEdgeDisplayName(const Standard_Integer ISE,const TopoDS_Shape& S,TCollection_AsciiString& N)  
{
  TCollection_AsciiString name;
  SectionEdgeName(ISE,S,name);

  Standard_Integer ids = CurrentBDS().Shape(S,FindKeep);
  name = name + "<" + TCollection_AsciiString(ids) + ">";

  N = AncestorMark(S,name); 
}

//=======================================================================
//function : GeometryDisplayName
//purpose  : 
//=======================================================================

void TestTopOpe_HDSDisplayer::GeometryDisplayName(const Standard_Integer  I,const TopOpeBRepDS_Kind K,TCollection_AsciiString& N) 
{
  if (K == TopOpeBRepDS_CURVE) {
    const Handle(Geom_Curve) GC = CurrentBDS().Curve(I).Curve();
    TestTopOpeDraw_TTOT::CurveDisplayName(I,GC,N);
  }
  else {
    TCollection_AsciiString n;
    TestTopOpeDraw_TTOT::GeometryName(I,K,n);
    N = " "; N = N + n;
  }
}

//=======================================================================
//function : ShapeKind
//purpose  : 
//=======================================================================

Standard_Boolean TestTopOpe_HDSDisplayer::ShapeKind(const Standard_Integer IS,const TopAbs_ShapeEnum TS) const 
{ 
  const TopoDS_Shape& S = CurrentBDS().Shape(IS,FindKeep);
  Standard_Boolean b = TestTopOpeDraw_TTOT::ShapeKind(S,TS);
  return b;
}
