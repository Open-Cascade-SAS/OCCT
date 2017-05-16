// Created on: 1994-03-18
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


#include <Standard_ProgramError.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_define.hxx>

//=======================================================================
//function : Print the name of a State
//=======================================================================
TCollection_AsciiString TopOpeBRepDS::SPrint(const TopAbs_State S)
{
  TCollection_AsciiString s;
  switch (S) {
  case TopAbs_IN      : s = s + "IN"; break;
  case TopAbs_OUT     : s = s + "OU"; break;
  case TopAbs_ON      : s = s + "ON"; break;
  case TopAbs_UNKNOWN : s = s + "UN"; break;
  }
  return s;
}

Standard_OStream& TopOpeBRepDS::Print(const TopAbs_State S,Standard_OStream& OS)
{ OS<<TopOpeBRepDS::SPrint(S); return OS; }

//=======================================================================
//purpose  : print the name of a Kind
//=======================================================================
TCollection_AsciiString TopOpeBRepDS::SPrint(const TopOpeBRepDS_Kind k)
{
  TCollection_AsciiString s;
  switch(k) {
  case TopOpeBRepDS_POINT   : s = s + "PO"; break;
  case TopOpeBRepDS_CURVE   : s = s + "CU"; break;
  case TopOpeBRepDS_SURFACE : s = s + "SU"; break;
  case TopOpeBRepDS_VERTEX  : s = s + "VE"; break;
  case TopOpeBRepDS_EDGE    : s = s + "ED"; break;
  case TopOpeBRepDS_WIRE    : s = s + "WI"; break;
  case TopOpeBRepDS_FACE    : s = s + "FA"; break;
  case TopOpeBRepDS_SHELL   : s = s + "SH"; break;
  case TopOpeBRepDS_SOLID   : s = s + "SO"; break;
  case TopOpeBRepDS_COMPSOLID : s = s + "CS"; break;
  case TopOpeBRepDS_COMPOUND : s = s + "CO"; break;
  default:
    break ;
  }
  return s;
}
TCollection_AsciiString TopOpeBRepDS::SPrint(const TopOpeBRepDS_Kind k,const Standard_Integer i,const TCollection_AsciiString& S1,const TCollection_AsciiString& S2)
{
  TCollection_AsciiString si = ""; if (i >= 0 && i <= 9) si = " ";
  TCollection_AsciiString s = S1 + "(" + TopOpeBRepDS::SPrint(k) + "," + si + TCollection_AsciiString(i) + ")" + S2;
  return s;
}

Standard_OStream& TopOpeBRepDS::Print(const TopOpeBRepDS_Kind k,Standard_OStream& OS)
{ OS<<TopOpeBRepDS::SPrint(k); return OS; }
Standard_OStream& TopOpeBRepDS::Print(const TopOpeBRepDS_Kind k,const Standard_Integer i,Standard_OStream& OS,const TCollection_AsciiString& S1,const TCollection_AsciiString& S2)
{ OS<<TopOpeBRepDS::SPrint(k,i,S1,S2); OS.flush();return OS; }

//=======================================================================
//purpose  : print the name of a ShapeEnum
//=======================================================================
TCollection_AsciiString TopOpeBRepDS::SPrint(const TopAbs_ShapeEnum t)
{ return TopOpeBRepDS::SPrint(TopOpeBRepDS::ShapeToKind(t)); }
TCollection_AsciiString TopOpeBRepDS::SPrint(const TopAbs_ShapeEnum t,const Standard_Integer i)
{ return TopOpeBRepDS::SPrint(TopOpeBRepDS::ShapeToKind(t),i); }

Standard_OStream& TopOpeBRepDS::Print(const TopAbs_ShapeEnum t,const Standard_Integer i,Standard_OStream& s)
{ s<<TopOpeBRepDS::SPrint(TopOpeBRepDS::ShapeToKind(t),i); return s; }

//=======================================================================
//purpose  : print the name of a Orientation
//=======================================================================
TCollection_AsciiString TopOpeBRepDS::SPrint(const TopAbs_Orientation o) 
{
  return TopAbs::ShapeOrientationToString (o);
}

//=======================================================================
//purpose  : print the name of a Config
//=======================================================================
TCollection_AsciiString TopOpeBRepDS::SPrint(const TopOpeBRepDS_Config C)
{
  TCollection_AsciiString SS;
  switch (C) {
  case TopOpeBRepDS_UNSHGEOMETRY : SS = "UNSH"; break;
  case TopOpeBRepDS_SAMEORIENTED : SS = "SAME"; break;
  case TopOpeBRepDS_DIFFORIENTED : SS = "DIFF"; break;
  }
  return SS;
}

Standard_OStream& TopOpeBRepDS::Print(const TopOpeBRepDS_Config C,Standard_OStream& OS)
{ OS<<TopOpeBRepDS::SPrint(C); return OS; }

//=======================================================================
//function : IsTopology
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS::IsTopology(const TopOpeBRepDS_Kind k)
{
  switch (k) {
  case TopOpeBRepDS_COMPOUND : return Standard_True; break;
  case TopOpeBRepDS_COMPSOLID : return Standard_True; break;
  case TopOpeBRepDS_SOLID   : return Standard_True; break;
  case TopOpeBRepDS_SHELL   : return Standard_True; break;
  case TopOpeBRepDS_FACE    : return Standard_True; break;
  case TopOpeBRepDS_WIRE    : return Standard_True; break;
  case TopOpeBRepDS_EDGE    : return Standard_True; break;
  case TopOpeBRepDS_VERTEX  : return Standard_True; break;
  default : return Standard_False;
  }
}

//=======================================================================
//function : IsGeometry
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS::IsGeometry(const TopOpeBRepDS_Kind k)
{
  switch (k) {
  case TopOpeBRepDS_SURFACE : return Standard_True; break;
  case TopOpeBRepDS_CURVE   : return Standard_True; break;
  case TopOpeBRepDS_POINT   : return Standard_True; break;
  default:
    break ;
  }
  return Standard_False;
}

//=======================================================================
//function : ShapeToKind
//purpose  : 
//=======================================================================

TopOpeBRepDS_Kind TopOpeBRepDS::ShapeToKind(const TopAbs_ShapeEnum S) 
{
  TopOpeBRepDS_Kind res = TopOpeBRepDS_SOLID; // bidon
  switch (S) {
  case TopAbs_VERTEX : res = TopOpeBRepDS_VERTEX; break;
  case TopAbs_EDGE   : res = TopOpeBRepDS_EDGE; break;
  case TopAbs_WIRE   : res = TopOpeBRepDS_WIRE; break;
  case TopAbs_FACE   : res = TopOpeBRepDS_FACE; break;
  case TopAbs_SHELL  : res = TopOpeBRepDS_SHELL; break;
  case TopAbs_SOLID  : res = TopOpeBRepDS_SOLID; break;
  case TopAbs_COMPSOLID : res = TopOpeBRepDS_COMPSOLID; break;
  case TopAbs_COMPOUND : res = TopOpeBRepDS_COMPOUND; break;
  default : throw Standard_ProgramError("TopOpeBRepDS::ShapeToKind");
  }
  return res;
}

//=======================================================================
//function : KindToShape
//purpose  : 
//=======================================================================

TopAbs_ShapeEnum TopOpeBRepDS::KindToShape(const TopOpeBRepDS_Kind K)
{
  TopAbs_ShapeEnum res = TopAbs_SHAPE; // bidon
  if ( ! TopOpeBRepDS::IsTopology(K) ) return res;

  switch (K) {
  case TopOpeBRepDS_VERTEX : res = TopAbs_VERTEX; break;
  case TopOpeBRepDS_EDGE   : res = TopAbs_EDGE; break;
  case TopOpeBRepDS_WIRE   : res = TopAbs_WIRE; break;
  case TopOpeBRepDS_FACE   : res = TopAbs_FACE; break;
  case TopOpeBRepDS_SHELL  : res = TopAbs_SHELL; break;
  case TopOpeBRepDS_SOLID  : res = TopAbs_SOLID; break;
  case TopOpeBRepDS_COMPSOLID  : res = TopAbs_COMPSOLID; break;
  case TopOpeBRepDS_COMPOUND : res = TopAbs_COMPOUND; break;
  default : throw Standard_ProgramError("TopOpeBRepDS::KindToShape");
  }
  return res;
}
