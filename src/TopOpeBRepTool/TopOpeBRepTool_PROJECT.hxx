// Created on: 1997-11-28
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
#include <Extrema_ExtFlag.hxx>
#include <Extrema_ExtAlgo.hxx>

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
Standard_EXPORT Standard_Boolean FUN_tool_projPonS(const gp_Pnt& P,const Handle(Geom_Surface)& S,gp_Pnt2d& UV,Standard_Real& dist,
                                                   const Extrema_ExtFlag anExtFlag=Extrema_ExtFlag_MINMAX,
                                                   const Extrema_ExtAlgo anExtAlgo=Extrema_ExtAlgo_Grad);

// ----------------------------------------------------------------------
//  project point <P> on topologies (edge <E>,face <F>)
// ----------------------------------------------------------------------
Standard_EXPORT Standard_Boolean FUN_tool_projPonE(const gp_Pnt& P,const Standard_Real tole,const TopoDS_Edge& E,Standard_Real& param,Standard_Real& dist);
Standard_EXPORT Standard_Boolean FUN_tool_projPonE(const gp_Pnt& P,const TopoDS_Edge& E,Standard_Real& param,Standard_Real& dist);
Standard_EXPORT Standard_Boolean FUN_tool_projPonboundedF(const gp_Pnt& P,const TopoDS_Face& F,gp_Pnt2d& UV,Standard_Real& dist);
Standard_EXPORT Standard_Boolean FUN_tool_projPonF(const gp_Pnt& P,const TopoDS_Face& F,gp_Pnt2d& UV,Standard_Real& dist,
                                                   const Extrema_ExtFlag anExtFlag=Extrema_ExtFlag_MINMAX,
                                                   const Extrema_ExtAlgo anExtAlgo=Extrema_ExtAlgo_Grad);

#endif
