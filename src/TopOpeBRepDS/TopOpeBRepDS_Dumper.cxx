// Created on: 1994-08-04
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


#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomTools_Curve2dSet.hxx>
#include <GeomTools_CurveSet.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Stream.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopAbs.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_CurveExplorer.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepDS_Dumper.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS_PointExplorer.hxx>
#include <TopOpeBRepDS_ShapeShapeInterference.hxx>
#include <TopOpeBRepDS_Surface.hxx>
#include <TopOpeBRepDS_SurfaceCurveInterference.hxx>
#include <TopOpeBRepDS_SurfaceExplorer.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>

#include <string.h>
//=======================================================================
//function : TopOpeBRepDS_Dumper::TopOpeBRepDS_Dumper
//purpose  : 
//=======================================================================
TopOpeBRepDS_Dumper::TopOpeBRepDS_Dumper(const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  myHDS = HDS;
}

//=======================================================================
//function : SDumpRefOri
//purpose  : 
//=======================================================================

TCollection_AsciiString TopOpeBRepDS_Dumper::SDumpRefOri(const TopOpeBRepDS_Kind K,const Standard_Integer I) const
{
  TCollection_AsciiString SS;
  Standard_Boolean fk = Standard_False;
  const TopOpeBRepDS_DataStructure& DS = myHDS->DS();
  if ( ! TopOpeBRepDS::IsTopology(K) ) return SS;
  TopAbs_ShapeEnum t = TopOpeBRepDS::KindToShape(K);
  if ( DS.Shape(I,fk).ShapeType() != t ) return SS;
  const TopoDS_Shape& S = myHDS->Shape(I,fk);
  Standard_Integer  r = myHDS->SameDomainReference(S);
  TopOpeBRepDS_Config o = myHDS->SameDomainOrientation(S);
  SS=SS+"("+SPrintShape(r)+","+TopOpeBRepDS::SPrint(o)+")";
  return SS;
}

//=======================================================================
//function : SDumpRefOri
//purpose  : 
//=======================================================================

TCollection_AsciiString TopOpeBRepDS_Dumper::SDumpRefOri(const TopoDS_Shape& S) const
{
  TCollection_AsciiString SS;
  TopOpeBRepDS_Kind k = TopOpeBRepDS::ShapeToKind(S.ShapeType());
  Standard_Boolean fk = Standard_False;
  Standard_Integer i = myHDS->Shape(S,fk);
  SS = SDumpRefOri(k,i);
  return SS;
}

//=======================================================================
//function : SPrintShape
//purpose  : 
//=======================================================================

TCollection_AsciiString TopOpeBRepDS_Dumper::SPrintShape(const Standard_Integer IS) const
{
  TCollection_AsciiString SS;
  const TopOpeBRepDS_DataStructure& BDS = myHDS->DS(); if ( IS<1 || IS>BDS.NbShapes()) return SS;
  SS = SPrintShape(BDS.Shape(IS));
  return SS;
}

//=======================================================================
//function : SPrintShape
//purpose  : 
//=======================================================================

TCollection_AsciiString TopOpeBRepDS_Dumper::SPrintShape(const TopoDS_Shape& S) const
{
  const TopOpeBRepDS_DataStructure& BDS = myHDS->DS();
  const Standard_Integer IS = myHDS->DS().Shape(S);
  Standard_Integer rankIS = BDS.AncestorRank(IS);
//JR/Hp  TCollection_AsciiString s1,s2;
  Standard_CString s1,s2;
  if(BDS.KeepShape(IS)) {
    s1 = (Standard_CString ) ((rankIS == 1) ? "*" : "");
    s2 = (Standard_CString ) ((rankIS == 2) ? "*" : "");
  }
  else {
    s1 = (Standard_CString ) ((rankIS == 1) ? "~" : "");
    s2 = (Standard_CString ) ((rankIS == 2) ? "~" : "");
  }
  TCollection_AsciiString sse = TopOpeBRepDS::SPrint(TopOpeBRepDS::ShapeToKind(S.ShapeType()),IS,s1,s2);
  return sse;
}

//=======================================================================
//function : SPrintShapeRefOri
//purpose  : 
//=======================================================================

TCollection_AsciiString TopOpeBRepDS_Dumper::SPrintShapeRefOri(const TopoDS_Shape& S,const TCollection_AsciiString& astr) const
{ TCollection_AsciiString SS=astr+SPrintShape(S)+" "+SDumpRefOri(S); return SS; }

//=======================================================================
//function : SPrintShapeRefOri
//purpose  : 
//=======================================================================

TCollection_AsciiString TopOpeBRepDS_Dumper::SPrintShapeRefOri(const TopTools_ListOfShape& L,const TCollection_AsciiString& astr) const
{
  TCollection_AsciiString SS;TopTools_ListIteratorOfListOfShape it(L); if (!it.More()) return SS;
  SS=SS+astr; TCollection_AsciiString bst(astr.Length(),' ');
  for(Standard_Integer il = 0; it.More(); it.Next(),il++) {
    TCollection_AsciiString ss=SPrintShapeRefOri(it.Value());if (il) ss=bst+ss; SS=SS+ss+"\n";
  }
  return SS;
}
