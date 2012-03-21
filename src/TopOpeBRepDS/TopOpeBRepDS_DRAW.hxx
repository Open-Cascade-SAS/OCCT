// Created on: 1997-11-26
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
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
