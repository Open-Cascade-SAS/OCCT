// File:	TestTopOpe_HDSDisplayer.hxx
// Created:	Mon Sep 23 15:12:14 1996
// Author:	Jean Yves LEBEY
//		<jyl@bistrox.paris1.matra-dtv.fr>

#ifndef _TestTopOpe_HDSDisplayer_HeaderFile
#define _TestTopOpe_HDSDisplayer_HeaderFile

#include <TopOpeBRepDS_define.hxx>

class TestTopOpe_HDSDisplayer {

public:
  TestTopOpe_HDSDisplayer();
  TestTopOpe_HDSDisplayer(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
  void Init();
  void SetCurrentHDS(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
  const Handle(TopOpeBRepDS_HDataStructure)& CurrentHDS() const ;
  const TopOpeBRepDS_DataStructure& CurrentBDS() const ;
  void SetShape1(const TopoDS_Shape& S1);
  void SetShape2(const TopoDS_Shape& S2);  
  Standard_Boolean SubShape(const TopoDS_Shape& S,const Standard_Integer I);

  void ShapeName(const Standard_Integer,const TopoDS_Shape&,TCollection_AsciiString&) const ;
  void ShapeDisplayName(const Standard_Integer,const TopoDS_Shape&,TCollection_AsciiString&) ;
  TCollection_AsciiString AncestorMark(const TopoDS_Shape&,const TCollection_AsciiString&) ;
  void ShapeDisplayName(const TCollection_AsciiString&,const TopoDS_Shape&,TCollection_AsciiString&) ;

  void SectionEdgeName(const Standard_Integer,const TopoDS_Shape&,TCollection_AsciiString&) const ;
  void SectionEdgeDisplayName(const Standard_Integer,const TopoDS_Shape&,TCollection_AsciiString&) ;

  void GeometryDisplayName(const Standard_Integer,const TopOpeBRepDS_Kind,TCollection_AsciiString&) ;
  Standard_Boolean ShapeKind(const Standard_Integer IS,const TopAbs_ShapeEnum TS) const ;

private:
  Handle(TopOpeBRepDS_HDataStructure) myHDS;
  TopoDS_Shape myS1,myS2;
  TopTools_IndexedMapOfShape *myPMap1, *myPMap2;
};

#endif
