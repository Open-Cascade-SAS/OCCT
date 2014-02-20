// Created on: 1997-11-26
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _TopOpeBRepDS_DRAW_HeaderFile
#define _TopOpeBRepDS_DRAW_HeaderFile

#ifdef DRAW
#include <TopOpeBRepDS_define.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <gp_Pnt2d.hxx>
#include <Geom2d_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Geom_Curve.hxx>
#include <gp_Dir.hxx>

Standard_EXPORT void FUN_draw (const TopoDS_Shape& s);
Standard_EXPORT void FUN_draw (const gp_Pnt& p);
Standard_EXPORT void FUN_draw (const gp_Pnt2d& p);
Standard_EXPORT void FUN_draw (const Handle(Geom2d_Curve) c, const Standard_Real dpar);
Standard_EXPORT void FUN_draw (const gp_Pnt& p, const gp_Dir& v);
Standard_EXPORT void FUN_draw (const gp_Pnt& p, const gp_Vec& v);
Standard_EXPORT void FUN_brep_draw (const TCollection_AsciiString& aa, const gp_Pnt& p);
Standard_EXPORT void FUN_brep_draw (const TCollection_AsciiString& aa, const gp_Pnt& p, const gp_Dir& d);
Standard_EXPORT void FUN_brep_draw (const TCollection_AsciiString& aa, const TopoDS_Shape& s);
Standard_EXPORT void FUN_brep_draw (const TCollection_AsciiString& aa, const Handle(Geom_Curve)& C, const Standard_Real& f, const Standard_Real& l);
Standard_EXPORT void FUN_brep_draw (const TCollection_AsciiString& aa, const Handle(Geom_Curve)& C);
Standard_EXPORT void FUN_DrawMap(const TopTools_DataMapOfShapeListOfShape& DataforDegenEd);
Standard_EXPORT void FUN_draw2de (const TopoDS_Shape& ed,const TopoDS_Shape& fa);
Standard_EXPORT void FUN_draw2d(const Standard_Real& par,const TopoDS_Edge& E,const TopoDS_Edge& Eref,const TopoDS_Face& Fref);

#endif
// DRAW

#endif
