// Created on: 1996-09-23
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
