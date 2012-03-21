// Created on: 1997-11-28
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


#ifndef _TopOpeBRepTool_PROJECT_HeaderFile
#define _TopOpeBRepTool_PROJECT_HeaderFile

#include <TopOpeBRepTool_define.hxx>

#include <Geom_Surface.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <Extrema_ExtPC.hxx>
#include <Extrema_ExtPC2d.hxx>

// ----------------------------------------------------------------------
//  project point <P> on geometries (curve <C>,surface <S>)
// ----------------------------------------------------------------------
Standard_EXPORT void FUN_tool_bounds(const TopoDS_Edge& E,Standard_Real& f,Standard_Real& l);
Standard_EXPORT Standard_Integer FUN_tool_getindex(const Extrema_ExtPC& ponc);
Standard_EXPORT Standard_Integer FUN_tool_getindex(const Extrema_ExtPC2d& ponc);
Standard_EXPORT Standard_Boolean FUN_tool_projPonC(const gp_Pnt& P,const Standard_Real tole,const BRepAdaptor_Curve& BAC,const Standard_Real pmin,const Standard_Real pmax,Standard_Real& param,Standard_Real& dist);
Standard_EXPORT Standard_Boolean FUN_tool_projPonC(const gp_Pnt& P,const BRepAdaptor_Curve& BAC,const Standard_Real pmin,const Standard_Real pmax,Standard_Real& param,Standard_Real& dist);
Standard_EXPORT Standard_Boolean FUN_tool_projPonC(const gp_Pnt& P,const BRepAdaptor_Curve& BAC,Standard_Real& param,Standard_Real& dist);
Standard_EXPORT Standard_Boolean FUN_tool_projPonC2D(const gp_Pnt& P,const Standard_Real tole,const BRepAdaptor_Curve2d& BAC2D,const Standard_Real pmin,const Standard_Real pmax,Standard_Real& param,Standard_Real& dist);
Standard_EXPORT Standard_Boolean FUN_tool_projPonC2D(const gp_Pnt& P,const BRepAdaptor_Curve2d& BAC2D,const Standard_Real pmin,const Standard_Real pmax,Standard_Real& param,Standard_Real& dist);
Standard_EXPORT Standard_Boolean FUN_tool_projPonC2D(const gp_Pnt& P,const BRepAdaptor_Curve2d& BAC2D,Standard_Real& param,Standard_Real& dist);
Standard_EXPORT Standard_Boolean FUN_tool_projPonS(const gp_Pnt& P,const Handle(Geom_Surface)& S,gp_Pnt2d& UV,Standard_Real& dist);

// ----------------------------------------------------------------------
//  project point <P> on topologies (edge <E>,face <F>)
// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_projPonE(const gp_Pnt& P,const Standard_Real tole,const TopoDS_Edge& E,Standard_Real& param,Standard_Real& dist);
Standard_EXPORT Standard_Boolean FUN_tool_projPonE(const gp_Pnt& P,const TopoDS_Edge& E,Standard_Real& param,Standard_Real& dist);
Standard_EXPORT Standard_Boolean FUN_tool_projPonboundedF(const gp_Pnt& P,const TopoDS_Face& F,gp_Pnt2d& UV,Standard_Real& dist);
Standard_EXPORT Standard_Boolean FUN_tool_projPonF(const gp_Pnt& P,const TopoDS_Face& F,gp_Pnt2d& UV,Standard_Real& dist);

#endif
