// Created on: 1998-11-24
// Created by: Prestataire Xuan PHAM PHU
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _TopOpeBRepTool_PURGE_HeaderFile
#define _TopOpeBRepTool_PURGE_HeaderFile
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
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>

#ifdef OCCT_DEBUG
extern void FUN_REINIT();
// extern int FUN_addepc(const TopoDS_Shape& ed,const TopoDS_Shape& f);
#endif

// ----------------------------------------------------------------------
// TopOpeBRepTool_faulty.cxx
// ----------------------------------------------------------------------
// Standard_IMPORT gp_Pnt2d FUN_GetVParonF(const TopoDS_Edge& E, const TopoDS_Face& F, const
// int Index); Standard_IMPORT double FUN_toluv(const GeomAdaptor_Surface& GAS,
// const double& tol3d); Standard_IMPORT void FUN_tool_Vertices(const TopoDS_Edge& E,
// NCollection_Array1<TopoDS_Shape>& vertices); Standard_IMPORT void FUN_mapVloe(const TopoDS_Shape&
// F, NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>,
// TopTools_ShapeMapHasher>& mapVloe);

// Standard_IMPORT bool FUN_DetectEdgeswithfaultyUV(const TopoDS_Face& Fin, const
// TopoDS_Shape& fF, const NCollection_List<TopoDS_Shape>& ISOEds, 						const
// bool has2fybounds,
// NCollection_List<TopoDS_Shape>& lfyE, int& Ivfaulty, 						const
// bool& stopatfirst=false); Standard_IMPORT bool
// FUN_DetectEdgewithfaultyUV(const
// TopoDS_Face& Fin, const TopoDS_Shape& fF, const NCollection_List<TopoDS_Shape>& ISOEds, const
// bool has2fybounds, TopoDS_Shape& fyE, int& Ivfaulty); Standard_IMPORT
// bool FUN_DetectFaultyClosingEdge(const TopoDS_Face& Fin,const NCollection_List<TopoDS_Shape>&
// Eds,const NCollection_List<TopoDS_Shape>& cEds,NCollection_List<TopoDS_Shape>& fyE);
// Standard_IMPORT bool FUN_isUVClosed(const TopoDS_Face& F, const TopoDS_Face& fF);

// ----------------------------------------------------------------------
// TopOpeBRepTool_PURGE.cxx
// ----------------------------------------------------------------------
// Standard_IMPORT void FUN_tool_ttranslate(const gp_Vec2d& tvector, const TopoDS_Face& fF,
// TopoDS_Edge& fyE);
#endif
