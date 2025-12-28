// Created on: 1994-10-24
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

#ifndef _TopOpeBRepTool_DRAW_HeaderFile
#define _TopOpeBRepTool_DRAW_HeaderFile
#ifdef DRAW

  #include <gp_Pnt.hxx>
  #include <gp_Pnt2d.hxx>
  #include <gp_Lin.hxx>
  #include <Draw_ColorKind.hxx>
  #include <Draw_MarkerShape.hxx>
  #include <TopAbs_ShapeEnum.hxx>
  #include <TopAbs_Orientation.hxx>
  #include <TopAbs_State.hxx>
  #include <TopoDS_Shape.hxx>
  #include <TopTools_ShapeMapHasher.hxx>
  #include <NCollection_Map.hxx>
  #include <NCollection_List.hxx>
  #include <NCollection_IndexedMap.hxx>
  #include <NCollection_DataMap.hxx>
  #include <Standard_Integer.hxx>
  #include <NCollection_IndexedDataMap.hxx>
  #include <TopoDS_Face.hxx>
  #include <TopoDS_Edge.hxx>
  #include <TopoDS_Vertex.hxx>
  #include <TCollection_AsciiString.hxx>
  #include <Geom_Curve.hxx>
  #include <Geom2d_Curve.hxx>

Standard_EXPORT void           TopOpeBRepTool_DrawPoint(const gp_Pnt&          P,
                                                        const Draw_MarkerShape T,
                                                        const Draw_ColorKind   C);
Standard_EXPORT void           TopOpeBRepTool_DrawSegment(const gp_Pnt&        P1,
                                                          const gp_Pnt&        P2,
                                                          const Draw_ColorKind C);
Standard_EXPORT void           TopOpeBRepTool_DrawSegment(const gp_Pnt& P1, const gp_Pnt& P2);
Standard_EXPORT void           TopOpeBRepTool_DrawSegment(const gp_Pnt& P,
                                                          const gp_Lin&,
                                                          const double  Par,
                                                          const Draw_ColorKind C);
Standard_EXPORT Draw_ColorKind TopOpeBRepTool_ColorOnState(const TopAbs_State S);
Standard_EXPORT void           TopOpeBRepTool_DrawSegment(const gp_Pnt&       P,
                                                          const gp_Lin&       L,
                                                          const double Par,
                                                          const TopAbs_State  S);
Standard_EXPORT void           FDRAW_DINS(const TCollection_AsciiString pref,
                                          const TopoDS_Shape&           SS,
                                          const TCollection_AsciiString Snam,
                                          const TCollection_AsciiString suff = "");
Standard_EXPORT void           FDRAW_DINE(const TCollection_AsciiString pref,
                                          const TopoDS_Edge&            EE,
                                          const TCollection_AsciiString Enam,
                                          const TCollection_AsciiString suff = "");
Standard_EXPORT void           FDRAW_DINLOE(const TCollection_AsciiString pref,
                                            const NCollection_List<TopoDS_Shape>&   LOE,
                                            const TCollection_AsciiString str1,
                                            const TCollection_AsciiString str2);
Standard_EXPORT void           FUN_tool_draw(const TCollection_AsciiString& aa,
                                             const gp_Pnt&                  p,
                                             const gp_Dir&                  d);
Standard_EXPORT void           FUN_tool_draw(const TCollection_AsciiString& aa,
                                             const gp_Pnt2d&                p,
                                             const gp_Dir2d&                d,
                                             const int&        i);
Standard_EXPORT void           FUN_tool_draw(const TCollection_AsciiString aa, const gp_Pnt2d& p2d);
Standard_EXPORT void           FUN_tool_draw(const TCollection_AsciiString aa, const gp_Pnt& p);
Standard_EXPORT void           FUN_tool_draw(const TCollection_AsciiString aa,
                                             const occ::handle<Geom2d_Curve>    c2d);
Standard_EXPORT void           FUN_tool_draw(const TCollection_AsciiString aa,
                                             const occ::handle<Geom2d_Curve>    c2d,
                                             const double           f,
                                             const double           l);
Standard_EXPORT void FUN_tool_draw(const TCollection_AsciiString& aa, const occ::handle<Geom_Curve>& C);
Standard_EXPORT void FUN_tool_draw(const TCollection_AsciiString aa,
                                   const occ::handle<Geom_Curve>      c,
                                   const double           f,
                                   const double           l);
Standard_EXPORT void FUN_tool_draw(const TCollection_AsciiString aa, const TopoDS_Shape& s);
Standard_EXPORT void FUN_tool_draw(const TCollection_AsciiString aa,
                                   const TopoDS_Shape&           S,
                                   const int        is);
Standard_EXPORT void FUN_tool_draw(TCollection_AsciiString aa,
                                   const TopoDS_Edge&      E,
                                   const TopoDS_Face&      F,
                                   const int  ie);
Standard_EXPORT void FUN_tool_draw(TCollection_AsciiString aa,
                                   const TopoDS_Edge&      E,
                                   const TopoDS_Face&      F,
                                   const int  ie);
Standard_EXPORT const TCollection_AsciiString& FUN_tool_PRODINS();
Standard_EXPORT const TCollection_AsciiString& FUN_tool_PRODINP();
#endif
#endif
