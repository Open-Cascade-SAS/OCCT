// Created on: 1994-10-24
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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


#ifndef _TopOpeBRepTool_DRAW_HeaderFile
#define _TopOpeBRepTool_DRAW_HeaderFile
#ifdef DRAW

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Lin.hxx>
#include <Draw_ColorKind.hxx>
#include <Draw_MarkerShape.hxx>
#include <TopOpeBRepTool_define.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <TopOpeBRepTool_define.hxx>

Standard_EXPORT void TopOpeBRepTool_DrawPoint(const gp_Pnt& P, const Draw_MarkerShape T, const Draw_ColorKind C) ;
Standard_EXPORT void TopOpeBRepTool_DrawSegment(const gp_Pnt& P1,const gp_Pnt& P2, const Draw_ColorKind C); 
Standard_EXPORT void TopOpeBRepTool_DrawSegment(const gp_Pnt& P1, const gp_Pnt& P2);
Standard_EXPORT void TopOpeBRepTool_DrawSegment(const gp_Pnt& P,const gp_Lin& ,const Standard_Real Par,const Draw_ColorKind C);
Standard_EXPORT Draw_ColorKind TopOpeBRepTool_ColorOnState(const TopAbs_State S);
Standard_EXPORT void TopOpeBRepTool_DrawSegment(const gp_Pnt& P,const gp_Lin& L,const Standard_Real Par,const TopAbs_State S);
Standard_EXPORT void FDRAW_DINS(const TCollection_AsciiString pref,const TopoDS_Shape& SS,const TCollection_AsciiString Snam,const TCollection_AsciiString suff = "");
Standard_EXPORT void FDRAW_DINE(const TCollection_AsciiString pref,const TopoDS_Edge& EE,const TCollection_AsciiString Enam,const TCollection_AsciiString suff = "");
Standard_EXPORT void FDRAW_DINLOE(const TCollection_AsciiString pref,const TopTools_ListOfShape& LOE,const TCollection_AsciiString str1,const TCollection_AsciiString str2);
Standard_EXPORT void FUN_tool_draw (const TCollection_AsciiString& aa, const gp_Pnt& p, const gp_Dir& d);
Standard_EXPORT void FUN_tool_draw(const TCollection_AsciiString& aa,const gp_Pnt2d& p, const gp_Dir2d& d,const Standard_Integer& i);
Standard_EXPORT void FUN_tool_draw(const TCollection_AsciiString aa,const gp_Pnt2d& p2d);
Standard_EXPORT void FUN_tool_draw (const TCollection_AsciiString aa,const gp_Pnt& p);
Standard_EXPORT void FUN_tool_draw(const TCollection_AsciiString aa,const Handle(Geom2d_Curve) c2d);
Standard_EXPORT void FUN_tool_draw(const TCollection_AsciiString aa,const Handle(Geom2d_Curve) c2d,const Standard_Real f, const Standard_Real l);
Standard_EXPORT void FUN_tool_draw(const TCollection_AsciiString& aa, const Handle(Geom_Curve)& C);
Standard_EXPORT void FUN_tool_draw(const TCollection_AsciiString aa,const Handle(Geom_Curve) c,const Standard_Real f, const Standard_Real l);
Standard_EXPORT void FUN_tool_draw(const TCollection_AsciiString aa, const TopoDS_Shape& s);
Standard_EXPORT void FUN_tool_draw(const TCollection_AsciiString aa,const TopoDS_Shape& S,const Standard_Integer is);
Standard_EXPORT void FUN_tool_draw(TCollection_AsciiString aa,const TopoDS_Edge& E, const TopoDS_Face& F,const Standard_Integer ie);
Standard_EXPORT void FUN_tool_draw(TCollection_AsciiString aa,const TopoDS_Edge& E, const TopoDS_Face& F,const Standard_Integer ie);
Standard_EXPORT const TCollection_AsciiString& FUN_tool_PRODINS();
Standard_EXPORT const TCollection_AsciiString& FUN_tool_PRODINP();
#endif
#endif
