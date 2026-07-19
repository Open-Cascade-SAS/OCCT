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

#include <Geom_Surface.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <Extrema_ExtPC.hxx>
#include <Extrema_ExtPC2d.hxx>
#include <Extrema_ExtFlag.hxx>
#include <Extrema_ExtAlgo.hxx>

// ----------------------------------------------------------------------
//  project point <P> on geometries (curve <C>,surface <S>)
// ----------------------------------------------------------------------
Standard_EXPORT void FUN_tool_bounds(const TopoDS_Edge& E, double& f, double& l);
Standard_EXPORT int  FUN_tool_getindex(const Extrema_ExtPC& ponc);
Standard_EXPORT int  FUN_tool_getindex(const Extrema_ExtPC2d& ponc);
Standard_EXPORT bool FUN_tool_projPonC(const gp_Pnt&            P,
                                       const double             tole,
                                       const BRepAdaptor_Curve& BAC,
                                       const double             pmin,
                                       const double             pmax,
                                       double&                  param,
                                       double&                  dist);
Standard_EXPORT bool FUN_tool_projPonC(const gp_Pnt&            P,
                                       const BRepAdaptor_Curve& BAC,
                                       const double             pmin,
                                       const double             pmax,
                                       double&                  param,
                                       double&                  dist);
Standard_EXPORT bool FUN_tool_projPonC(const gp_Pnt&            P,
                                       const BRepAdaptor_Curve& BAC,
                                       double&                  param,
                                       double&                  dist);
Standard_EXPORT bool FUN_tool_projPonC2D(const gp_Pnt&              P,
                                         const double               tole,
                                         const BRepAdaptor_Curve2d& BAC2D,
                                         const double               pmin,
                                         const double               pmax,
                                         double&                    param,
                                         double&                    dist);
Standard_EXPORT bool FUN_tool_projPonC2D(const gp_Pnt&              P,
                                         const BRepAdaptor_Curve2d& BAC2D,
                                         const double               pmin,
                                         const double               pmax,
                                         double&                    param,
                                         double&                    dist);
Standard_EXPORT bool FUN_tool_projPonC2D(const gp_Pnt&              P,
                                         const BRepAdaptor_Curve2d& BAC2D,
                                         double&                    param,
                                         double&                    dist);
Standard_EXPORT bool FUN_tool_projPonS(const gp_Pnt&                    P,
                                       const occ::handle<Geom_Surface>& S,
                                       gp_Pnt2d&                        UV,
                                       double&                          dist,
                                       const Extrema_ExtFlag anExtFlag = Extrema_ExtFlag_MINMAX,
                                       const Extrema_ExtAlgo anExtAlgo = Extrema_ExtAlgo_Grad);

// ----------------------------------------------------------------------
//  project point <P> on topologies (edge <E>,face <F>)
// ----------------------------------------------------------------------
Standard_EXPORT bool FUN_tool_projPonE(const gp_Pnt&      P,
                                       const double       tole,
                                       const TopoDS_Edge& E,
                                       double&            param,
                                       double&            dist);
Standard_EXPORT bool FUN_tool_projPonE(const gp_Pnt&      P,
                                       const TopoDS_Edge& E,
                                       double&            param,
                                       double&            dist);
Standard_EXPORT bool FUN_tool_projPonboundedF(const gp_Pnt&      P,
                                              const TopoDS_Face& F,
                                              gp_Pnt2d&          UV,
                                              double&            dist);
Standard_EXPORT bool FUN_tool_projPonF(const gp_Pnt&         P,
                                       const TopoDS_Face&    F,
                                       gp_Pnt2d&             UV,
                                       double&               dist,
                                       const Extrema_ExtFlag anExtFlag = Extrema_ExtFlag_MINMAX,
                                       const Extrema_ExtAlgo anExtAlgo = Extrema_ExtAlgo_Grad);

#endif
