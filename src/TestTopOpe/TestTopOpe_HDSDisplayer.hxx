// Created on: 1996-09-23
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
